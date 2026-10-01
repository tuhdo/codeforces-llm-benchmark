"""Build the June-Aug 2026 E/F problem set with per-round standings (mirror-safe),
fetch every problem page, and merge with the September set into data/problems_all.json.
"""
import datetime
import html as html_mod
import json
import os
import re
import time
import urllib.request

os.chdir(os.path.dirname(os.path.abspath(__file__)))
from cfapi import call  # noqa: E402

UA = "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/154.0.0.0 Safari/537.36"
COOKIE = open("session.txt").read().strip()
JUNE1 = 1780272000
SEPT1 = 1788220800


def strip_tags(fragment: str) -> str:
    fragment = re.sub(r"(?i)<br\s*/?>", "\n", fragment)
    fragment = re.sub(r"(?i)</p>|</div>|</li>|</tr>|</h[1-6]>", "\n", fragment)
    fragment = re.sub(r"<[^>]+>", "", fragment)
    return html_mod.unescape(fragment)


def clean_statement(page: str) -> str:
    i = page.find('<div class="problem-statement">')
    j = page.find('<div class="sample-tests">', i)
    frag = page[i:j] if i >= 0 and j > i else ""
    frag = re.sub(r"(?s)<span class=\"text-hidden\">.*?</span>", "", frag)
    frag = strip_tags(frag)
    frag = re.sub(r"[ \t]+\n", "\n", frag)
    return re.sub(r"\n{3,}", "\n\n", frag).strip()


def extract_samples(page: str) -> list:
    inputs = re.findall(r'(?s)<div class="input">\s*<div class="title">Input</div>\s*<pre>(.*?)</pre>', page)
    outputs = re.findall(r'(?s)<div class="output">\s*<div class="title">Output</div>\s*<pre>(.*?)</pre>', page)
    return [{"input": re.sub(r"\n{2,}", "\n", strip_tags(i)).strip("\n"),
             "output": re.sub(r"\n{2,}", "\n", strip_tags(o)).strip("\n")}
            for i, o in zip(inputs, outputs)]


# 1. Per-round E/F selection, name-deduped across summer AND September
contests = call("contest.list", signed=False)
summer = sorted((c for c in contests if JUNE1 <= c.get("startTimeSeconds", 0) < SEPT1),
                key=lambda c: c["startTimeSeconds"])
sept_names = {p["name"] for p in json.load(open("data/problems_full.json", encoding="utf-8"))}

selected, seen = [], set(sept_names)
for c in summer:
    standings = call("contest.standings", signed=False, contestId=c["id"])
    for p in standings["problems"]:
        if p["index"] in ("E", "F") and p["name"] not in seen:
            seen.add(p["name"])
            selected.append({"contestId": c["id"], "contestName": c["name"],
                             "index": p["index"], "name": p["name"],
                             "rating": p.get("rating"), "tags": p.get("tags", [])})
    time.sleep(2)
print(f"summer E/F after dedup vs September: {len(selected)}")

# 2. Fetch pages + extract
full = []
for n, p in enumerate(selected, 1):
    cid, idx = p["contestId"], p["index"]
    url = f"https://codeforces.com/contest/{cid}/problem/{idx}"
    req = urllib.request.Request(url, headers={"User-Agent": UA, "Cookie": COOKIE})
    with urllib.request.urlopen(req, timeout=60) as r:
        page = r.read().decode("utf-8", errors="replace")
    with open(f"data/pages/{cid}{idx}.html", "w", encoding="utf-8") as f:
        f.write(page)
    m = (re.search(r"name='csrf_token'\s+value='([^']+)'", page)
         or re.search(r"data-csrf=['\"]([0-9a-f]+)['\"]", page))
    langs = {k: v for k, v in re.findall(r'<option value="(\d+)"[^>]*>([^<]+)</option>', page)
             if "C++" in v or "Python" in v or "Java" in v}
    p.update({"statement": clean_statement(page), "samples": extract_samples(page),
              "csrf": m.group(1) if m else "",
              "ftaa": (re.search(r'window\._ftaa\s*=\s*"([^"]*)"', page) or [None, ""])[1],
              "bfaa": (re.search(r'window\._bfaa\s*=\s*"([^"]*)"', page) or [None, ""])[1],
              "languages": langs, "url": url})
    full.append(p)
    hidden = "HONEYPOT" if "text-hidden" in page else ""
    print(f"[{n}/{len(selected)}] {cid}{idx} stmt={len(p['statement'])} samples={len(p['samples'])} "
          f"csrf={'y' if p['csrf'] else 'NO'} {hidden}", flush=True)
    time.sleep(4)

# 3. Merge with September into the full queue
sept = json.load(open("data/problems_full.json", encoding="utf-8"))
json.dump(sept + full, open("data/problems_all.json", "w", encoding="utf-8"), indent=2)
print(f"data/problems_all.json: {len(sept)} sept + {len(full)} summer = {len(sept) + len(full)} problems")
