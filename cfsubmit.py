"""Submit a solution to Codeforces and poll the verdict.

Uses the logged-in session from session.txt and per-page csrf/ftaa/bfaa.
Public entrypoints:
  submit(problem, source_path, program_type_id=89) -> bool submitted
  poll_verdict(handle, since_ts, timeout_s=420) -> verdict string or "TIMEOUT"
"""
import json
import re
import time
import urllib.request
import uuid

import cfapi

UA = "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/154.0.0.0 Safari/537.36"


UA_CHROME = "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/154.0.0.0 Safari/537.36"
UA_FIREFOX = "Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:157.0) Gecko/20100101 Firefox/157.0"


def _headers(extra=None):
    return channel_headers("session.txt", UA_CHROME, extra)


def channel_headers(cookie_file: str, ua: str, extra=None):
    h = {
        "User-Agent": ua,
        "Cookie": open(cookie_file).read().strip(),
        "Origin": "https://codeforces.com",
    }
    if extra:
        h.update(extra)
    return h


def _multipart(fields: dict) -> tuple[bytes, str]:
    boundary = "----cfsub" + uuid.uuid4().hex
    body = b""
    for name, value in fields.items():
        body += f"--{boundary}\r\n".encode()
        body += f'Content-Disposition: form-data; name="{name}"\r\n\r\n'.encode()
        body += str(value).encode("utf-8") + b"\r\n"
    body += f"--{boundary}--\r\n".encode()
    return body, f"multipart/form-data; boundary={boundary}"


def _fresh_tokens(cid: int, idx: str, cookie_file: str = "session.txt",
                  ua: str = UA_CHROME) -> dict:
    """Scrape current csrf/ftaa/bfaa from the problem page (they rotate)."""
    url = f"https://codeforces.com/contest/{cid}/problem/{idx}"
    req = urllib.request.Request(url, headers=channel_headers(cookie_file, ua, {"Referer": "https://codeforces.com/"}))
    with urllib.request.urlopen(req, timeout=60) as r:
        page = r.read().decode("utf-8", errors="replace")
    m = (re.search(r"name='csrf_token'\s+value='([^']+)'", page)
         or re.search(r"data-csrf=['\"]([0-9a-f]+)['\"]", page))
    csrf = m.group(1) if m else ""
    ftaa = (re.search(r'window\._ftaa\s*=\s*"([^"]*)"', page) or [None, ""])[1]
    bfaa = (re.search(r'window\._bfaa\s*=\s*"([^"]*)"', page) or [None, ""])[1]
    if not csrf:
        raise RuntimeError("could not scrape fresh csrf token")
    return {"csrf": csrf, "ftaa": ftaa, "bfaa": bfaa}


def _fields(problem: dict, source: str, program_type_id: int) -> dict:
    return {
        "csrf_token": problem["csrf"],
        "ftaa": problem.get("ftaa", ""),
        "bfaa": problem.get("bfaa", ""),
        "action": "submitSolutionFormSubmitted",
        "submittedProblemIndex": problem["index"],
        "source": source,
        "sourceFile": "",
        "programTypeId": program_type_id,
        "tabSize": "4",
        "indent": "4",
        "mobile": "false",
    }


def submit_once(problem: dict, source_path: str, program_type_id: int = 89,
                cookie_file: str = "session.txt", ua: str = UA_CHROME,
                handle: str = "") -> None:
    """Single POST + account confirmation. Raises on failure; no retries, no sleeps
    beyond the confirmation poll. Caller owns serialization and cooldowns."""
    cid, idx = problem["contestId"], problem["index"]
    url = f"https://codeforces.com/contest/{cid}/problem/{idx}"
    problem = {**problem, **_fresh_tokens(cid, idx, cookie_file, ua)}
    source = open(source_path, encoding="utf-8", errors="replace").read()
    body, ctype = _multipart(_fields(problem, source, program_type_id))
    req = urllib.request.Request(url + f"?csrf_token={problem['csrf']}", data=body, method="POST")
    req.add_header("Content-Type", ctype)
    for k, v in channel_headers(cookie_file, ua, {"Referer": url, "X-Requested-With": "XMLHttpRequest"}).items():
        req.add_header(k, v)
    try:
        with urllib.request.urlopen(req, timeout=90) as r:
            r.read()
    except urllib.error.HTTPError as e:
        raise RuntimeError(f"submit POST failed: HTTP {e.code}")
    since = int(time.time())
    deadline = time.time() + 60
    while time.time() < deadline:
        time.sleep(5)
        try:
            subs = cfapi.call("user.status", handle=handle, from_=1, count=1)
        except Exception:
            continue
        if subs:
            s = subs[0]
            if (s.get("contestId") == cid and s.get("problem", {}).get("index") == idx
                    and s.get("creationTimeSeconds", 0) >= since - 10):
                return
    raise RuntimeError("submit not confirmed on account (likely rate limit)")


def submit(problem: dict, source_path: str, program_type_id: int = 89) -> bool:
    """Retry wrapper: up to 3 POSTs, 300 s cooldown between (cooldown happens
    OUTSIDE any caller lock only if the caller releases it - prefer submit_once
    under a short-lived lock)."""
    for attempt_no in range(3):
        try:
            submit_once(problem, source_path, program_type_id)
            return True
        except Exception:
            if attempt_no == 2:
                raise
            print("  submit failed, cooling down 300 s", flush=True)
            time.sleep(300)
    return False


def poll_verdict(handle: str, since_ts: int, problem_cid: int, problem_idx: str,
                 timeout_s: int = 420, interval_s: float = 6.0):
    """Return (submission_id, verdict). verdict None while still judging."""
    deadline = time.time() + timeout_s
    while time.time() < deadline:
        time.sleep(interval_s)
        try:
            subs = cfapi.call("user.status", handle=handle, from_=1, count=1)
        except Exception as e:
            print(f"  poll error: {e}", flush=True)
            time.sleep(5)
            continue
        if not subs:
            continue
        s = subs[0]
        same = (s.get("contestId") == problem_cid
                and s.get("problem", {}).get("index") == problem_idx
                and s.get("creationTimeSeconds", 0) >= since_ts - 10)
        if not same:
            continue
        verdict = s.get("verdict")
        if verdict not in (None, "TESTING"):
            return s["id"], verdict, s.get("passedTestCount"), s.get("timeConsumedMillis")
    return None, "POLL_TIMEOUT", None, None


if __name__ == "__main__":
    import sys
    problems = {f"{p['contestId']}{p['index']}": p
                for p in json.load(open("data/problems_full.json", encoding="utf-8"))}
    key = sys.argv[1]
    src = sys.argv[2]
    print(json.dumps(submit(problems[key], src), default=str) if False else "submit returned")
