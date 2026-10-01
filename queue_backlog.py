"""Queue all unlanded August SUBMIT_ERROR rungs as browser jobs (dedup by rung)."""
import json, os, uuid
os.chdir(os.path.dirname(os.path.abspath(__file__)))
HANDLE = json.load(open("channels.json", encoding="utf-8"))["handle"]
problems = {f"{p['contestId']}{p['index']}": p
            for p in json.load(open("data/problems_aug.json", encoding="utf-8"))}
rows = [json.loads(l) for l in open("results.jsonl", encoding="utf-8") if l.strip()]
done_rungs = set()
for f in os.listdir("browser_queue/done"):
    j = json.load(open(f"browser_queue/done/{f}", encoding="utf-8"))
    if j.get("reconciled") and j.get("submission_id"):
        done_rungs.add((j["problem_key"], j["model"], j["effort"]))
pending_rungs = set()
for f in os.listdir("browser_queue/pending"):
    j = json.load(open(f"browser_queue/pending/{f}", encoding="utf-8"))
    pending_rungs.add((j["problem_key"], j["model"], j["effort"]))
seen, judged = set(), set()
for r in rows:
    v = r.get("verdict") or ""
    if v and not v.startswith("SUBMIT_ERROR") and v not in ("NO_SOLUTION",) and r.get("effort") in ("medium", "high", "xhigh", "max"):
        judged.add((r["problem"], r["model"], r["effort"]))
for r in rows:
    queued = 0
    seen = set()
for r in rows:
    if r["problem"] not in problems:
        continue
    if not (r.get("verdict") or "").startswith("SUBMIT_ERROR"):
        continue
    rung = (r["problem"], r["model"], r["effort"])
    if rung in judged or rung in done_rungs or rung in pending_rungs or rung in seen:
        continue
    seen.add(rung)
    sol = f"runs/{rung[0]}/{rung[1]}/{rung[2]}/sol.cpp"
    if not os.path.exists(sol):
        continue
    p = problems[rung[0]]
    job = {"id": uuid.uuid4().hex[:10], "problem_key": rung[0], "contest_id": p["contestId"],
           "index": p["index"], "name": p["name"], "label": f"{p['index']} - {p['name']}",
           "sol_path": os.path.abspath(sol), "model": rung[1], "effort": rung[2],
           "handle": HANDLE}
    with open(f"browser_queue/pending/{job['id']}.json", "w", encoding="utf-8") as f:
        json.dump(job, f, indent=2)
    queued += 1
print(f"queued {queued} new jobs; pending now {len(os.listdir('browser_queue/pending'))}")
