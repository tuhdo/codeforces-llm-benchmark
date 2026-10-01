"""Aggregate results.jsonl into REPORT.md."""
import json
import os
from collections import Counter, defaultdict

os.chdir(os.path.dirname(os.path.abspath(__file__)))

MODELS = ["gpt-6.1-sol", "gpt-6-luna", "gpt-5.6-terra", "gpt-5.6-luna", "claude-sonnet-5-5"]
EFFORTS = ["low", "medium", "high", "xhigh", "max"]
problems = {f"{p['contestId']}{p['index']}": p
            for p in json.load(open("data/problems_full.json", encoding="utf-8"))}
rows = [json.loads(l) for l in open("results.jsonl", encoding="utf-8") if l.strip()]

best = {}          # (problem, model) -> cheapest-effort Accepted row
attempts_by = defaultdict(list)
for r in rows:
    if r.get("effort") == "LANE_ERROR":
        continue
    attempts_by[(r["problem"], r["model"])].append(r)
    if r["solved"]:
        k = (r["problem"], r["model"])
        cur = best.get(k)
        rank = {e: i for i, e in enumerate(EFFORTS)}
        if cur is None or (rank.get(r["effort"], 99), r["ts"]) < (rank.get(cur["effort"], 99), cur["ts"]):
            best[k] = r

effort_rank = {e: i for i, e in enumerate(EFFORTS)}
lines = []
lines.append("# Codeforces September 2026 E/F benchmark — results\n")
lines.append(f"Problems: {len(problems)} E/F tasks from 9 rounds (2259–2269). "
             f"Ladder: one attempt per effort — codex models medium → high → xhigh → max, "
             f"claude medium → high → max; stop at first judge OK. "
             f"Submission language: C++20 (GCC 13-64), judged on full tests. "
             f"Claude lane paused on weekly quota (resets Oct 2, 9pm Asia/Bangkok); "
             f"its 2268F/2269F ladders are pending.\n")

lines.append("## Per-model summary\n")
nprob = len(problems)
lines.append(f"| model | solved /{nprob} | AC@medium | AC@high | AC@xhigh | AC@max | not solved | attempts | total agent wall (min) |")
lines.append("|---|---|---|---|---|---|---|---|---|")
for m in MODELS:
    solved = [best[k] for k in best if k[1] == m]
    dist = Counter(r["effort"] for r in solved)
    pairs = [k for k in attempts_by if k[1] == m]
    nsolved = len(solved)
    natt = sum(len(attempts_by[k]) for k in pairs)
    wall = sum(r["wall_s"] for k in pairs for r in attempts_by[k]) / 60
    lines.append(f"| {m} | {nsolved} | {dist.get('medium', 0)} | {dist.get('high', 0)} | "
                 f"{dist.get('xhigh', 0)} | {dist.get('max', 0)} | {nprob - nsolved} | {natt} | {wall:.0f} |")
lines.append("")

lines.append("## Problem × model: effort of first Accepted\n")
hdr = "| problem | rating | " + " | ".join(MODELS) + " |"
lines.append(hdr)
lines.append("|---|---|" + "---|" * len(MODELS))
for key, p in problems.items():
    cells = []
    for m in MODELS:
        r = best.get((key, m))
        cells.append(r["effort"] if r else "–")
    rating = p.get("rating") or "unrated"
    lines.append(f"| {key} {p['name'][:38]} | {rating} | " + " | ".join(cells) + " |")
lines.append("")

lines.append("## All attempts\n")
lines.append("| problem | model | effort | local | judge verdict | tests | wall s |")
lines.append("|---|---|---|---|---|---|---|")
order = {"low": 0, "medium": 1, "high": 2, "xhigh": 3, "max": 4}
for k in sorted(attempts_by, key=lambda k: (k[0], MODELS.index(k[1]))):
    for r in sorted(attempts_by[k], key=lambda r: order.get(r["effort"], 9)):
        lines.append(f"| {r['problem']} | {r['model']} | {r['effort']} | {r['local']} | "
                     f"{r['verdict']} | {r['passed'] if r['passed'] is not None else ''} | "
                     f"{r['wall_s']} |")
lines.append("")

solved_by = defaultdict(list)
for k in best:
    solved_by[k[0]].append(k[1])
lines.append("## Notes\n")
hard = [k for k in problems if len(solved_by.get(k, [])) == 0]
lines.append(f"- Unsolved by every model: {', '.join(hard) or 'none'}")
no_sol = [r for r in rows if r.get("verdict") == "NO_SOLUTION"]
lines.append(f"- NO_SOLUTION attempts (agent ended without writing a file): {len(no_sol)} "
             + (", ".join(f"{r['problem']}/{r['model']}@{r['effort']}" for r in no_sol[:8]) or ""))
lines.append("- Agent constraints: no network, no reads outside the attempt dir (isolated temp cwd); "
             "statements had hidden anti-AI honeypot spans stripped (found in 2259E, 2259F).")
open("REPORT.md", "w", encoding="utf-8").write("\n".join(lines))
print(f"REPORT.md written: {len(rows)} attempts, {len(best)}/{len(problems) * len(MODELS)} solved")
