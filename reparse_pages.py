"""Re-parse downloaded problem pages (data/pages/*.html) into problems_full.json.

No network. Extracts statement text (with hidden anti-AI spans removed),
sample tests, csrf, ftaa/bfaa, language options.
"""
import html as html_mod
import json
import os
import re

os.chdir(os.path.dirname(os.path.abspath(__file__)))

problems = json.load(open("data/sept2026_ef.json", encoding="utf-8"))

INJECT_HITS = []


def strip_tags(fragment: str) -> str:
    fragment = re.sub(r"(?i)<br\s*/?>", "\n", fragment)
    fragment = re.sub(r"(?i)</p>|</div>|</li>|</tr>|</h[1-6]>", "\n", fragment)
    fragment = re.sub(r"<[^>]+>", "", fragment)
    return html_mod.unescape(fragment)


def clean_statement(page: str) -> str:
    i = page.find('<div class="problem-statement">')
    j = page.find('<div class="sample-tests">', i)
    frag = page[i:j] if i >= 0 and j > i else ""
    hidden = re.findall(r'(?s)<span class="text-hidden">(.*?)</span>', frag)
    if hidden:
        INJECT_HITS.append((hidden,))
        frag = re.sub(r'(?s)<span class="text-hidden">.*?</span>', "", frag)
    # math: replace TeX-ish spans markers, keep text
    frag = strip_tags(frag)
    frag = re.sub(r"[ \t]+\n", "\n", frag)
    frag = re.sub(r"\n{3,}", "\n\n", frag)
    return frag.strip()


def extract_samples(page: str) -> list:
    inputs = re.findall(r'(?s)<div class="input">\s*<div class="title">Input</div>\s*<pre>(.*?)</pre>', page)
    outputs = re.findall(r'(?s)<div class="output">\s*<div class="title">Output</div>\s*<pre>(.*?)</pre>', page)
    samples = []
    for i, o in zip(inputs, outputs):
        to_lines = lambda s: re.sub(r"\n{2,}", "\n", strip_tags(s)).strip("\n")
        samples.append({"input": to_lines(i), "output": to_lines(o)})
    return samples


full = []
for p in problems:
    cid, idx = p["contestId"], p["index"]
    page = open(f"data/pages/{cid}{idx}.html", encoding="utf-8").read()

    m = re.search(r"name='csrf_token'\s+value='([^']+)'", page) or re.search(r'data-csrf=[\'"]([0-9a-f]+)[\'"]', page)
    csrf = m.group(1) if m else ""
    ftaa = (re.search(r'window\._ftaa\s*=\s*"([^"]*)"', page) or [None, ""])[1]
    bfaa = (re.search(r'window\._bfaa\s*=\s*"([^"]*)"', page) or [None, ""])[1]
    langs = dict(re.findall(r'<option value="(\d+)"[^>]*>([^<]+)</option>', page))
    # keep only plausible compilers
    langs = {k: v for k, v in langs.items() if "C++" in v or "Python" in v or "Java" in v}

    p.update({"statement": clean_statement(page), "samples": extract_samples(page),
              "csrf": csrf, "ftaa": ftaa, "bfaa": bfaa, "languages": langs,
              "url": f"https://codeforces.com/contest/{cid}/problem/{idx}"})
    full.append(p)
    print(f"{cid}{idx}: stmt={len(p['statement'])}ch samples={len(p['samples'])} "
          f"csrf={'yes' if csrf else 'NO'} langs={len(langs)}")

json.dump(full, open("data/problems_full.json", "w", encoding="utf-8"), indent=2)
print("saved data/problems_full.json")
print("text-hidden injection spans found in:", INJECT_HITS or "none")
