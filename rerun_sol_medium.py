"""Rerun gpt-6.1-sol at medium for 2234E and 2234F: fresh agent runs, judge submissions."""
import json, os, shutil, subprocess, tempfile, threading, time
os.chdir(os.path.dirname(os.path.abspath(__file__)))
import cfsubmit
from runner import (prepare_run_dir, local_check, find_solution, do_submission,
                    archive, write_mve, run_codex, AGENT_TIMEOUT_S)

problems = {f"{p['contestId']}{p['index']}": p
            for p in json.load(open("data/problems_full.json", encoding="utf-8"))}
jobs = [("2234E", "medium"), ("2234F", "medium")]
results = []
threads = []
locks = {"2234E": threading.Lock(), "2234F": threading.Lock()}

def work(key, effort):
    problem = problems[key]
    model = "gpt-6.1-sol"
    tmp_dir = tempfile.mkdtemp(prefix=f"rerun_{key}_sol_medium_")
    archive_dir = os.path.join("runs", key, model, effort)
    prepare_run_dir(tmp_dir, problem)
    print(f">>> rerun {key} gpt-6.1-sol {effort} starting", flush=True)
    code, wall, _ = run_codex(tmp_dir, model, effort)
    if _limit_message := None:
        pass
    local = local_check(tmp_dir)
    row = {"problem": key, "contestId": problem["contestId"], "index": problem["index"],
           "model": model, "effort": effort, "attempt": 2, "wall_s": round(wall, 1),
           "rerun": True, "cli_exit": code, "local": local, "submission_id": None,
           "verdict": None, "passed": None, "judge_time_ms": None, "solved": False,
           "ts": time.strftime("%Y-%m-%dT%H:%M:%S")}
    sol = find_solution(tmp_dir)
    if sol and local in ("LOCAL_OK", "LOCAL_FAIL"):
        with locks[key]:
            res = do_submission(problem, sol)
        row.update(res)
    else:
        row["verdict"] = local
    archive(tmp_dir, archive_dir)
    shutil.rmtree(tmp_dir, ignore_errors=True)
    with results_lock_print():
        print(f"[{key}][gpt-6.1-sol/{effort}] local={row['local']} verdict={row['verdict']} wall={row['wall_s']}s", flush=True)
    with open("results.jsonl", "a", encoding="utf-8") as f:
        f.write(json.dumps(row) + "\n")
    results.append(row)
    write_mve(key)

def results_lock_print():
    class _C:
        def __enter__(self): return self
        def __exit__(self, *a): return False
    return _C()

for key, effort in jobs:
    t = threading.Thread(target=work, args=(key, effort))
    t.start()
    threads.append(t)
for t in threads:
    t.join()
print("RERUN COMPLETE")
