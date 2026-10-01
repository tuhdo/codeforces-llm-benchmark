"""Fetch June/July/August 2026 Division 2 rounds, select E/F problems,
fetch their pages, and merge into data/problems_full.json (Sept entries kept).

Div. 2 = contest name contains "Div. 2" or "Rated for Div. 2".
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

LO = int(datetime.datetime(2026, 6, 1, tzinfo=datetime.timezone.utc).timestamp())
HI = int(datetime.datetime(2026, 9, 1, tzinfo=datetime.timezone.utc).timestamp())


def is_div2(name: str) -> bool:
    return "Div. 2" in name or "Rated for Div. 2" in name


def get(url: str) -> str:
    req = urllib.request.Request(url, headers={"User-Agent": UA, "Cookie": COOKIE})
    with urllib.request.urlopen(req, timeout=60) as r:
        return r.read().decode("utf-8", errors="replace")


def strip_tags(fragment: str) -> str:
    fragment = re.sub(r"(?i)<br\s*/?>", "\n", fragment)
    fragment = re.sub(r"(?i)</p>|</div>|</li>|</tr>|</h[1-6]>", "\n", fragment)
    fragment = re.sub(r"<[^>]+>", "", fragment)
    return html_mod.unescape(fragment)


def clean_statement(page: str) -> str:
    i = page.find('<div class="problem-statement">')
    j = page.find('<div class="sample-tests">', i)
    frag = page[i:j] if i >= 0 and j > i else ""
    frag = re.sub(r'(?s)<span class="text-hidden">.*?</span>', "", frag)
    frag = strip_tags(frag)
    frag = re.sub(r"[ \t]+\n", "\n", frag)
    return re.sub(r"\n{3,}", "\n\n", frag).strip()


def extract_samples(page: str) -> list:
    inputs = re.findall(r'(?s)<div class="input">\s*<div class="title">Input</div>\s*<pre>(.*?)</pre>', page)
    outputs = re.findall(r'(?s)<div class="output">\s*<div class="title">Output</div>\s*<pre>(.*?)</pre>', page)
    to_lines = lambda s: re.sub(r"\n{2,}", "\n", strip_tags(s)).strip("\n")
    return [{"input": to_lines(i), "output": to_lines(o)} for i, o in zip(inputs, outputs)]


contests = call("contest.list", signed=False)
window = sorted((c for c in contests if LO <= c.get("startTimeSeconds", 0) < HI),
                key=lambda c: c["startTimeSeconds"])
div2 = [c for c in window if is_div2(c["name"])]
print(f"{len(window)} contests in Jun-Aug window, {len(div2)} Div. 2 rounds:")
for c in div2:
    print(" ", c["id"], c["name"])

selected, seen = [], set()
for c in div2:
    standings = call("contest.standings", signed=False, contestId=c["id"])
    for p in standings["problems"]:
        if p["index"] in ("E", "F") and p["name"] not in seen:
            seen.add(p["name"])
            selected.append({"contestId": c["id"], "contestName": c["name"],
                             "index": p["index"], "name": p["name"],
                             "rating": p.get("rating"), "tags": p.get("tags", [])})
    time.sleep(2.2)

print(f"{len(selected)} unique E/F problems")

full = []
os.makedirs("data/pages", exist_ok=True)
for p in selected:
    cid, idx = p["contestId"], p["index"]
    key = f"{cid}{idx}"
    page_path = f"data/pages/{key}.html"
    if os.path.exists(page_path):
        page = open(page_path, encoding="utf-8").read()
    else:
        try:
            page = get(f"https://codeforces.com/contest/{cid}/problem/{idx}")
            with open(page_path, "w", encoding="utf-8") as f:
                f.write(page)
        except Exception as e:
            print(f"{key}: FETCH FAILED {e}")
            continue
        time.sleep(4)
    m = (re.search(r"name='csrf_token'\s+value='([^']+)'", page)
         or re.search(r"data-csrf=['\"]([0-9a-f]+)['\"]", page))
    ftaa = (re.search(r'window\._ftaa\s*=\s*"([^"]*)"', page) or [None, ""])[1]
    bfaa = (re.search(r'window\._bfaa\s*=\s*"([^"]*)"', page) or [None, ""])[1]
    langs = {k: v for k, v in re.findall(r'<option value="(\d+)"[^>]*>([^<]+)</option>', page)
             if "C++" in v or "Python" in v or "Java" in v}
    p.update({"statement": clean_statement(page), "samples": extract_samples(page),
              "csrf": m.group(1) if m else "", "ftaa": ftaa, "bfaa": bfaa,
              "languages": langs,
              "url": f"https://codeforces.com/contest/{cid}/problem/{idx}"})
    full.append(p)
    print(f"{key}: stmt={len(p['statement'])}ch samples={len(p['samples'])} "
          f"csrf={'yes' if p['csrf'] else 'NO'}")

existing = json.load(open("data/problems_full.json", encoding="utf-8"))
have = {f"{p['contestId']}{p['index']}" for p in existing}
added = [p for p in full if f"{p['contestId']}{p['index']}" not in have]
json.dump(existing + added, open("data/problems_full.json", "w", encoding="utf-8"), indent=2)
print(f"merged: +{len(added)} problems -> data/problems_full.json now {len(existing) + len(added)}")
