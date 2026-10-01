"""Pair browser-queue done jobs with new account submissions (by seq order),
append result rows, requeue jobs whose submission never appeared."""
import json, os, sys, time
os.chdir(os.path.dirname(os.path.abspath(__file__)))
HANDLE = json.load(open("channels.json", encoding="utf-8"))["handle"]
from cfapi import call
from runner import write_mve

STATE = "browser_queue/last_sub_id.txt"
done_dir = "browser_queue/done"
jobs = []
for f in sorted(os.listdir(done_dir)):
    j = json.load(open(f"{done_dir}/{f}", encoding="utf-8"))
    if j.get("reconciled"):
        continue
    jobs.append((f, j))
if not jobs:
    print("nothing to reconcile")
    sys.exit(0)
jobs.sort(key=lambda t: (t[1].get("seq") or 0))

last_id = int(open(STATE).read()) if os.path.exists(STATE) else 0
subs = [s for s in sorted(call("user.status", handle=HANDLE, from_=1, count=40),
                          key=lambda s: s["creationTimeSeconds"]) if s["id"] > last_id]

paired, unlanded = [], []
si = 0
# jobs with a known still-testing submission: check that id directly
still = []
for f, job in jobs:
    tid = job.get("testing_sub_id")
    if tid:
        match = [s for s in subs if s["id"] == tid]
        if match and match[0].get("verdict") not in (None, "TESTING"):
            paired.append((f, job, match[0]))
        else:
            still.append((f, job))
    elif tid is not None:
        still.append((f, job))
pending_jobs = [(f, job) for f, job in jobs if not job.get("testing_sub_id")]

si = 0
for f, job in pending_jobs:
    paired_flag = False
    while si < len(subs):
        s = subs[si]
        key = f"{s.get('problem',{}).get('contestId')}{s.get('problem',{}).get('index')}"
        if key == job["problem_key"]:
            if s.get("verdict") in (None, "TESTING"):
                job["testing_sub_id"] = s["id"]
                still.append((f, job))
            else:
                paired.append((f, job, s))
            si += 1
            paired_flag = True
            break
        si += 1
    if not paired_flag:
        unlanded.append((f, job))

if paired:
    open(STATE, "w").write(str(max(s["id"] for _, _, s in paired)))

for f, job, s in paired:
    verdict = s.get("verdict")
    row = {"problem": job["problem_key"], "contestId": job["contest_id"], "index": job["index"],
           "model": job["model"], "effort": job["effort"], "attempt": 2, "wall_s": 0,
           "replay": "browser", "handle": HANDLE, "cli_exit": None, "local": None,
           "submission_id": s["id"], "verdict": verdict, "passed": s.get("passedTestCount"),
           "judge_time_ms": s.get("timeConsumedMillis"), "solved": verdict == "OK",
           "ts": time.strftime("%Y-%m-%dT%H:%M:%S")}
    with open("results.jsonl", "a", encoding="utf-8") as out:
        out.write(json.dumps(row) + "\n")
    write_mve(job["problem_key"])
    job["reconciled"] = True
    json.dump(job, open(f"{done_dir}/{f}", "w", encoding="utf-8"), indent=2)
    print("RECONCILED:", job["problem_key"], job["model"], job["effort"], "->", s["id"], verdict)

if paired:
    last_paired = max(s["id"] for _, _, s in paired)
    open(STATE, "w").write(str(max(last_id, last_paired)))

for f, job in unlanded:
    job.pop("seq", None); job.pop("clicked", None); job.pop("testing_sub_id", None)
    print("REQUEUED (no submission found):", job["problem_key"], job["model"], job["effort"])
for f, job in still:
    json.dump(job, open(f"{done_dir}/{f}", "w", encoding="utf-8"), indent=2)
    print("still testing:", job["problem_key"], job["model"], job["effort"], "sub", job.get("testing_sub_id"))
for f, job in unlanded:
    pass
    json.dump(job, open(f"browser_queue/pending/{f}", "w", encoding="utf-8"), indent=2)
    os.remove(f"{done_dir}/{f}")
    print("REQUEUED (no submission found):", job["problem_key"], job["model"], job["effort"])
