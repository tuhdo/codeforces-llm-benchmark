"""Fetch problem pages for data/sept2026_ef.json using the logged-in session.

Extracts per problem: statement text, sample tests, csrf token, ftaa/bfaa.
Output: data/pages/<cid><idx>.html, data/problems_full.json
"""
import html as html_mod
import json
import os
import re
import time
import urllib.request

os.chdir(os.path.dirname(os.path.abspath(__file__)))
UA = "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/154.0.0.0 Safari/537.36"
COOKIE = open("session.txt").read().strip()

problems = json.load(open("data/sept2026_ef.json", encoding="utf-8"))


def get(url: str) -> str:
    req = urllib.request.Request(url, headers={"User-Agent": UA, "Cookie": COOKIE})
    with urllib.request.urlopen(req, timeout=60) as r:
        return r.read().decode("utf-8", errors="replace")


def strip_tags(fragment: str) -> str:
    fragment = re.sub(r"(?i)<br\s*/?>", "\n", fragment)
    fragment = re.sub(r"(?i)</p>|</div>|</li>", "\n", fragment)
    fragment = re.sub(r"<[^>]+>", "", fragment)
    return html_mod.unescape(fragment)


def extract_samples(page: str) -> list:
    samples = []
    m = re.search(r'(?s)<div class="sample-test">(.*?)<div class="section"?>', page)
    body = m.group(1) if m else page
    blocks = re.findall(r'(?s)<div class="(input|output)">(.*?)(?=<div class="(?:input|output)">|</div>\s*</div>\s*$)', body)
    if not blocks:
        blocks = re.findall(r'(?s)<div class="(input|output)">\s*<div class="title">[^<]*</div>\s*<pre>(.*?)</pre>', body)
    for kind, frag in blocks:
        frag = re.sub(r"(?i)<br\s*/?>", "\n", frag)
        m2 = re.search(r"(?s)<pre>(.*?)</pre>", frag)
        content = m2.group(1) if m2 else frag
        content = strip_tags(content).strip("\n")
        samples.append({"kind": kind, "content": content})
    # Pair inputs/outputs in order
    inputs = [s["content"] for s in samples if s["kind"] == "input"]
    outputs = [s["content"] for s in samples if s["kind"] == "output"]
    return [{"input": i, "output": o} for i, o in zip(inputs, outputs)]


full = []
for p in problems:
    cid, idx = p["contestId"], p["index"]
    url = f"https://codeforces.com/contest/{cid}/problem/{idx}"
    page = get(url)
    with open(f"data/pages/{cid}{idx}.html", "w", encoding="utf-8") as f:
        f.write(page)

    m = re.search(r'(?s)<div class="problem-statement"[^>]*>(.*?)<div class="roundbox', page)
    stmt_html = m.group(1) if m else ""
    statement = strip_tags(stmt_html).strip()

    csrf = ""
    m = re.search(r'name="csrf_token"\s+value="([^"]+)"', page)
    if m:
        csrf = m.group(1)
    else:
        m = re.search(r"data-csrf='([^']+)'", page) or re.search(r'data-csrf="([^"]+)"', page)
        if m:
            csrf = m.group(1)

    ftaa = (re.search(r'window\._ftaa\s*=\s*"([^"]*)"', page) or [None, ""])[1]
    bfaa = (re.search(r'window\._bfaa\s*=\s*"([^"]*)"', page) or [None, ""])[1]

    # Language options from the inline submit form (for this account)
    langs = dict(re.findall(r'<option value="(\d+)"[^>]*>([^<]+)</option>', page))

    p.update({"statement": statement, "samples": extract_samples(page),
              "csrf": csrf, "ftaa": ftaa, "bfaa": bfaa, "languages": langs,
              "url": url})
    full.append(p)
    print(f"{cid}{idx}: stmt={len(statement)}ch samples={len(p['samples'])} csrf={'yes' if csrf else 'NO'} ftaa={ftaa[:6]} bfaa={bfaa[:8]}... langs={len(langs)}")
    time.sleep(4)

json.dump(full, open("data/problems_full.json", "w", encoding="utf-8"), indent=2)
print("saved data/problems_full.json")
