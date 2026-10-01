"""Codeforces benchmark runner.

For each problem x model, run one attempt per effort rung (medium -> high -> max),
stop at the first judge-accepted (OK) verdict. Each attempt runs the CLI agent in
its own empty directory containing only statement.md and the sample tests.

Records every attempt to results.jsonl with wall-clock time.

Usage:
  python runner.py                     # full benchmark
  python runner.py --only-problem 2259E --only-model gpt-6.1-sol
"""
import argparse
import glob
import json
import os
import shutil
import subprocess
import tempfile
import threading
import time
from collections import defaultdict
from concurrent.futures import ThreadPoolExecutor

os.chdir(os.path.dirname(os.path.abspath(__file__)))
import cfsubmit  # noqa: E402

CODEX_MODELS = ["gpt-6.1-sol", "gpt-6-luna", "gpt-5.6-terra", "gpt-5.6-luna"]
CLAUDE_MODEL = "claude-sonnet-5-5"
EFFORTS_CODEX = ["medium", "high", "xhigh", "max"]
EFFORTS_CLAUDE = ["medium", "high", "max"]
PROGRAM_TYPE_ID = 89  # GNU G++20 13.2 (64 bit, winlibs)
AGENT_TIMEOUT_S = 2400
POLL_TIMEOUT_S = 420
SAMPLE_TOLERANCE = None  # exact match after whitespace normalization

PROMPT = """Solve the competitive programming problem in statement.md.

Deliverable: a single C++20 source file saved as sol.cpp in the CURRENT directory.
It must read from stdin and write to stdout.

Resources: statement.md and the sample_N.in / sample_N.expected files in the
current directory. You may compile with g++ -std=c++20 -O2 and run your solution
against these samples to check yourself.

Hard constraints:
- No network or internet access of any kind (no curl, wget, pip, git fetch, downloads).
- Do not read, write, or search any location outside the current directory.
  No looking for editorials, extra tests, or other solutions on this computer.
- Solve purely from statement.md.

When finished, sol.cpp must exist in the current directory and compile with:
g++ -std=c++20 -O2 -o sol sol.cpp
Do not ask questions; work autonomously until sol.cpp is saved.
"""

submit_lock = threading.Lock()
results_lock = threading.Lock()
LIMIT_PAUSED = set()  # models whose CLI reported an account/usage limit

# Submission channels: each has its own judge budget and its own lock.
# Identity lives in channels.json (gitignored); see channels.json.example.
UAS = {"chrome": cfsubmit.UA_CHROME, "firefox": cfsubmit.UA_FIREFOX}
CHANNELS = []
if os.path.exists("channels.json"):
    cfg = json.load(open("channels.json", encoding="utf-8"))
    for ch in cfg["channels"]:
        CHANNELS.append({"name": ch.get("name", cfg["handle"]),
                         "cookie_file": ch.get("cookie_file", "session.txt"),
                         "ua": UAS.get(ch.get("ua", "chrome"), cfsubmit.UA_CHROME),
                         "handle": ch.get("handle", cfg["handle"]),
                         "lock": threading.Lock()})
_channel_lock = threading.Lock()


def acquire_channel(required_flag: str | None = None):
    """Pick any free channel; if none, block on the first. None if the only
    free ones need a ready-flag that is absent."""
    while True:
        with _channel_lock:
            for ch in CHANNELS:
                if required_flag and ch["name"] != CHANNELS[0]["name"] and not os.path.exists("channel2.ready"):
                    continue
                if ch["lock"].acquire(blocking=False):
                    return ch
        CHANNELS[0]["lock"].acquire()
        return CHANNELS[0]


def _limit_message(run_dir: str) -> str | None:
    log = os.path.join(run_dir, "attempt.log")
    if not os.path.exists(log):
        return None
    content = open(log, encoding="utf-8", errors="replace").read()
    low = content.lower()
    for needle in ("weekly limit", "usage limit", "rate limit reached", "credit balance"):
        if needle in low:
            return needle
    return None


def log_result(row: dict):
    with results_lock:
        with open("results.jsonl", "a", encoding="utf-8") as f:
            f.write(json.dumps(row) + "\n")
    print(f"[{row['problem']}][{row['model']}/{row['effort']}] "
          f"local={row['local']} verdict={row['verdict']} wall={row['wall_s']:.0f}s", flush=True)


def load_done() -> tuple[set, set, set]:
    solved, attempted, submit_errored = set(), set(), set()
    if os.path.exists("results.jsonl"):
        for line in open("results.jsonl", encoding="utf-8"):
            try:
                r = json.loads(line)
            except json.JSONDecodeError:
                continue
            k = (r["problem"], r["model"], r["effort"])
            verdict = r.get("verdict") or ""
            if verdict.startswith("SUBMIT_ERROR"):
                # Infra failure: the agent ran and a solution exists, but the
                # judge never scored it. Rung stays open; solution is replayed.
                submit_errored.add(k)
                continue
            if r.get("local_proxy"):
                # Local-only verdict (judge session was dead): replay the banked
                # solution so the pair gets a real judge verdict.
                submit_errored.add(k)
                continue
            attempted.add(k)
            if r.get("solved"):
                solved.add((r["problem"], r["model"]))
    return solved, attempted, submit_errored


def prepare_run_dir(path: str, problem: dict):
    os.makedirs(path, exist_ok=True)
    with open(os.path.join(path, "statement.md"), "w", encoding="utf-8") as f:
        f.write(f"# {problem['contestName']} - {problem['index']}. {problem['name']}\n"
                f"{problem['url']}\n\n{problem['statement']}\n")
    with open(os.path.join(path, "prompt.txt"), "w", encoding="utf-8") as f:
        f.write(PROMPT)
    for i, s in enumerate(problem["samples"], 1):
        with open(os.path.join(path, f"sample_{i}.in"), "w", encoding="utf-8", newline="\n") as f:
            f.write(s["input"])
        with open(os.path.join(path, f"sample_{i}.expected"), "w", encoding="utf-8", newline="\n") as f:
            f.write(s["output"])


def norm(text: str) -> str:
    return "\n".join(line.rstrip() for line in text.strip().splitlines())


def find_solution(run_dir: str) -> str | None:
    """sol.cpp if present, else the newest .cpp the agent left behind."""
    direct = os.path.join(run_dir, "sol.cpp")
    if os.path.exists(direct):
        return direct
    cpps = glob.glob(os.path.join(run_dir, "*.cpp"))
    if not cpps:
        return None
    return max(cpps, key=os.path.getmtime)


def local_check(run_dir: str) -> str:
    """Compile the agent's solution and run samples. CE_LOCAL | LOCAL_FAIL | LOCAL_OK."""
    src = find_solution(run_dir)
    exe = os.path.join(run_dir, "sol.exe")
    if not src:
        return "CE_LOCAL"
    comp = subprocess.run(["g++", "-std=c++20", "-O2", "-o", exe, src],
                          capture_output=True, timeout=120)
    if comp.returncode != 0:
        with open(os.path.join(run_dir, "compile.log"), "wb") as f:
            f.write(comp.stderr)
        return "CE_LOCAL"
    i = 1
    while os.path.exists(os.path.join(run_dir, f"sample_{i}.in")):
        inp = open(os.path.join(run_dir, f"sample_{i}.in"), encoding="utf-8").read()
        exp = open(os.path.join(run_dir, f"sample_{i}.expected"), encoding="utf-8").read()
        try:
            run = subprocess.run([exe], input=inp.encode(), capture_output=True, timeout=30)
        except subprocess.TimeoutExpired:
            return "LOCAL_FAIL"
        if norm(run.stdout.decode(errors="replace")) != norm(exp):
            with open(os.path.join(run_dir, f"sample_{i}.got"), "w", encoding="utf-8") as f:
                f.write(run.stdout.decode(errors="replace"))
            return "LOCAL_FAIL"
        i += 1
    return "LOCAL_OK"


def run_codex(run_dir: str, model: str, effort: str) -> tuple[int, float, str]:
    cmd = [shutil.which("codex"), "exec", "--skip-git-repo-check", "--sandbox", "danger-full-access",
           "-m", model, "-c", f'model_reasoning_effort="{effort}"', PROMPT]
    return _run_cli(cmd, run_dir)


def run_claude(run_dir: str, model: str, effort: str) -> tuple[int, float, str]:
    cmd = [shutil.which("claude"), "-p", PROMPT, "--model", model, "--effort", effort,
           "--permission-mode", "acceptEdits",
           "--allowedTools",
           "Bash(g++:*)", "Bash(./sol*)", "Bash(sol.exe*)", "Bash(diff:*)", "Bash(ls:*)",
           "Bash(cat:*)", "Bash(echo:*)", "Bash(printf:*)", "Bash(head:*)", "Bash(tail:*)",
           "Bash(wc:*)", "Bash(rm:*)", "Bash(mv:*)", "Bash(mkdir:*)",
           "--disallowedTools", "WebFetch", "WebSearch", "Task", "Agent", "NotebookEdit"]
    return _run_cli(cmd, run_dir)


def _run_cli(cmd: list, run_dir: str) -> tuple[int, float, str]:
    t0 = time.monotonic()
    try:
        proc = subprocess.run(cmd, cwd=run_dir, capture_output=True, timeout=AGENT_TIMEOUT_S,
                              shell=False)
        out = (proc.stdout + b"\n" + proc.stderr).decode(errors="replace")
        code = proc.returncode
    except subprocess.TimeoutExpired as e:
        out = f"TIMEOUT after {AGENT_TIMEOUT_S}s\n" + (e.stdout or b"").decode(errors="replace")
        code = -9
    wall = time.monotonic() - t0
    with open(os.path.join(run_dir, "attempt.log"), "w", encoding="utf-8") as f:
        f.write(out)
    return code, wall, out[-2000:]


def archive(tmp_dir: str, archive_dir: str):
    os.makedirs(archive_dir, exist_ok=True)
    keep = ["*.cpp", "*.log", "*.got", "*.txt", "*.md", "*.in", "*.expected"]
    patterns = [p for k in keep for p in glob.glob(os.path.join(tmp_dir, k))]
    for f in patterns:
        if os.path.isfile(f):
            shutil.copy2(f, os.path.join(archive_dir, os.path.basename(f)))
    sol = find_solution(tmp_dir)
    if sol and os.path.basename(sol) != "sol.cpp":
        shutil.copy2(sol, os.path.join(archive_dir, "sol.cpp"))


def do_submission(problem: dict, sol_path: str) -> dict:
    """One POST on any free channel (short lock hold), then verdict polling
    with that channel's handle. No cooldown sleeps: on throttle the rung
    records SUBMIT_ERROR and the next runner pass retries it.
    Returns {'submission_id', 'verdict', 'passed', 'judge_time_ms', 'solved', 'handle'}.
    """
    ch = acquire_channel()
    try:
        err = None
        try:
            cfsubmit.submit_once(problem, sol_path, PROGRAM_TYPE_ID,
                                 ch["cookie_file"], ch["ua"], ch["handle"])
        except Exception as e:
            err = e
        if err is None:
            since = int(time.time())
            sid, verdict, passed, jms = cfsubmit.poll_verdict(
                ch["handle"], since, problem["contestId"], problem["index"], POLL_TIMEOUT_S)
            return {"submission_id": sid, "verdict": verdict, "passed": passed,
                    "judge_time_ms": jms, "solved": verdict == "OK", "handle": ch["handle"]}
        return {"submission_id": None, "verdict": f"SUBMIT_ERROR: {err}",
                "passed": None, "judge_time_ms": None, "solved": False,
                "handle": ch["handle"]}
    finally:
        ch["lock"].release()


def replay_submission(problem: dict, model: str, effort: str, archive_dir: str,
                      submit_errored: set):
    """Re-submit a saved solution for a rung whose first POST was never scored.

    Returns the result row, or None when the rung has no replayable state.
    """
    key = f"{problem['contestId']}{problem['index']}"
    if (key, model, effort) not in submit_errored:
        return None
    saved = os.path.join(archive_dir, "sol.cpp")
    if not os.path.exists(saved):
        return None
    local = local_check(archive_dir)
    row = {"problem": key, "contestId": problem["contestId"], "index": problem["index"],
           "model": model, "effort": effort, "attempt": 2, "wall_s": 0, "replay": True,
           "cli_exit": None, "local": local, "submission_id": None, "verdict": None,
           "passed": None, "judge_time_ms": None, "solved": False,
           "ts": time.strftime("%Y-%m-%dT%H:%M:%S")}
    if local in ("LOCAL_OK", "LOCAL_FAIL"):
        res = do_submission(problem, saved)
        row.update(res)
    else:
        row["verdict"] = f"REPLAY_{local}"
    log_result(row)
    return row


def write_mve(key: str):
    """Write runs/<key>/mve.json: per-model minimum viable effort (cheapest
    effort with judge OK), plus verdicts and wall time per effort."""
    rows = []
    for line in open("results.jsonl", encoding="utf-8"):
        try:
            r = json.loads(line)
        except json.JSONDecodeError:
            continue
        if r.get("problem") == key and r.get("effort") != "LANE_ERROR" \
                and not (r.get("verdict") or "").startswith("SUBMIT_ERROR") \
                and r.get("effort") in ("low", "medium", "high", "xhigh", "max"):
            rows.append(r)
    rank = {e: i for i, e in enumerate(["low", "medium", "high", "xhigh", "max"])}
    models = {}
    for r in sorted(rows, key=lambda r: (r["model"], rank.get(r["effort"], 99))):
        m = models.setdefault(r["model"], {"mve_effort": None, "verdicts": {}, "wall_s": {}})
        if r["effort"] not in m["verdicts"]:
            m["verdicts"][r["effort"]] = r.get("verdict") or r.get("local")
            m["wall_s"][r["effort"]] = r.get("wall_s")
        solved_row = r.get("solved") or (r.get("local_proxy") and r.get("verdict") == "LOCAL_OK")
        if solved_row and (m["mve_effort"] is None
                           or rank[r["effort"]] < rank[m["mve_effort"]]):
            m["mve_effort"] = r["effort"]
    out = {"problem": key, "updated": time.strftime("%Y-%m-%dT%H:%M:%S"), "models": models}
    path = os.path.join("runs", key, "mve.json")
    os.makedirs(os.path.dirname(path), exist_ok=True)
    with open(path, "w", encoding="utf-8") as f:
        json.dump(out, f, indent=2)


def solve_problem_model(problem: dict, model: str, runner_fn, done: set, attempted: set,
                        submit_errored: set, efforts: list | None = None,
                        full_ladder: bool = False, no_submit: bool = False):
    key = f"{problem['contestId']}{problem['index']}"
    if model in LIMIT_PAUSED:
        return
    if efforts is None:
        efforts = EFFORTS_CLAUDE if runner_fn is run_claude else EFFORTS_CODEX
    for effort in efforts:
        if (key, model, effort) in attempted or (key, model) in done:
            continue
        archive_dir = os.path.join("runs", key, model, effort)
        replayed = replay_submission(problem, model, effort, archive_dir, submit_errored)
        if replayed is not None:
            if replayed["solved"]:
                break
            continue
        # Attempts run in an isolated temp dir: no benchmark state within reach,
        # so agents cannot read other attempts' solutions or prior results.
        tmp_dir = tempfile.mkdtemp(prefix=f"cfbench_{key}_{model.replace('-', '_')}_{effort}_")
        archive_dir = os.path.join("runs", key, model, effort)
        prepare_run_dir(tmp_dir, problem)
        print(f">>> {key} {model} effort={effort} starting", flush=True)
        code, wall, tail = runner_fn(tmp_dir, model, effort)
        limit = _limit_message(tmp_dir)
        if limit:
            # CLI account limit: not a model result. Leave the rung open for a
            # later pass (after reset) and pause this model for this process.
            print(f"!!! {model} account limit detected ({limit}); pausing model", flush=True)
            LIMIT_PAUSED.add(model)
            archive(tmp_dir, archive_dir)
            shutil.rmtree(tmp_dir, ignore_errors=True)
            return
        local = local_check(tmp_dir)
        row = {"problem": key, "contestId": problem["contestId"], "index": problem["index"],
               "model": model, "effort": effort, "attempt": 1, "wall_s": round(wall, 1),
               "cli_exit": code, "local": local, "submission_id": None, "verdict": None,
               "passed": None, "judge_time_ms": None, "solved": False,
               "ts": time.strftime("%Y-%m-%dT%H:%M:%S")}
        sol = find_solution(tmp_dir)
        if no_submit:
            row["verdict"] = local  # local compile+sample verdict is the record
            row["local_proxy"] = True
            row["solved"] = local == "LOCAL_OK"
        elif local in ("LOCAL_OK", "LOCAL_FAIL"):
            # Judge is ground truth: exact-match sample comparison cannot decide
            # "output any valid answer" problems, so LOCAL_FAIL still submits.
            row.update(do_submission(problem, sol))
        elif local == "CE_LOCAL":
            row["verdict"] = "CE_LOCAL" if os.path.exists(os.path.join(tmp_dir, "compile.log")) else "NO_SOLUTION"
        archive(tmp_dir, archive_dir)
        shutil.rmtree(tmp_dir, ignore_errors=True)
        log_result(row)
        if full_ladder:
            write_mve(key)
        if row["solved"] and not full_ladder:
            break
        time.sleep(150)  # pace judge submissions; the CDN throttles POST bursts


def safe_solve(problem: dict, model: str, runner_fn, done: set, attempted: set,
               submit_errored: set, efforts: list | None = None,
               full_ladder: bool = False, no_submit: bool = False):
    try:
        if model in LIMIT_PAUSED:
            return
        solve_problem_model(problem, model, runner_fn, done, attempted, submit_errored,
                            efforts, full_ladder, no_submit)
    except Exception as e:
        key = f"{problem['contestId']}{problem['index']}"
        print(f"!!! lane error {key} {model}: {type(e).__name__}: {e}", flush=True)
        log_result({"problem": key, "contestId": problem["contestId"], "index": problem["index"],
                    "model": model, "effort": "LANE_ERROR", "attempt": 0, "wall_s": 0,
                    "cli_exit": None, "local": None, "submission_id": None,
                    "verdict": f"{type(e).__name__}: {e}", "passed": None,
                    "judge_time_ms": None, "solved": False,
                    "ts": time.strftime("%Y-%m-%dT%H:%M:%S")})


def xhigh_backfill():
    """Re-run the xhigh rung for pairs whose cheapest Accept is at max.

    The xhigh tier was added after those ladders finished, so they never got
    an xhigh attempt. Codex models only (claude has no xhigh tier).
    """
    problems = json.load(open("data/problems_full.json", encoding="utf-8"))
    by_key = {f"{p['contestId']}{p['index']}": p for p in problems}
    rows_by_pair = defaultdict(list)
    solved_pairs, attempted, submit_errored = set(), set(), set()
    for line in open("results.jsonl", encoding="utf-8"):
        try:
            r = json.loads(line)
        except json.JSONDecodeError:
            continue
        if r.get("effort") == "LANE_ERROR":
            continue
        k = (r["problem"], r["model"])
        rows_by_pair[k].append(r)
        if (r.get("verdict") or "").startswith("SUBMIT_ERROR"):
            submit_errored.add((r["problem"], r["model"], r["effort"]))
            continue
        attempted.add((r["problem"], r["model"], r["effort"]))
        if r.get("solved"):
            solved_pairs.add(k)

    rank = {e: i for i, e in enumerate(EFFORTS_CODEX)}
    targets = []
    for key, model in sorted(solved_pairs):
        if model not in CODEX_MODELS:
            continue
        srows = [r for r in rows_by_pair[(key, model)] if r.get("solved")]
        cheapest = min(srows, key=lambda r: rank.get(r["effort"], 99))["effort"]
        if cheapest == "max" and (key, model, "xhigh") not in attempted:
            targets.append((key, model))
    print(f"xhigh backfill targets ({len(targets)}): {targets}", flush=True)
    if not targets:
        return
    jobs = [(by_key[key], model) for key, model in targets if key in by_key]
    with ThreadPoolExecutor(max_workers=4) as ex:
        futs = [ex.submit(safe_solve, p, m, run_codex, set(), attempted,
                          submit_errored, ["xhigh"]) for p, m in jobs]
        for f in futs:
            f.result()


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--only-problem")
    ap.add_argument("--only-model")
    ap.add_argument("--problems-file", default="data/problems_full.json")
    ap.add_argument("--full-ladder", action="store_true",
                    help="run every effort rung regardless of verdicts (no stop-on-AC)")
    ap.add_argument("--xhigh-backfill", action="store_true",
                    help="run only the xhigh wave for pairs whose cheapest Accept is max")
    ap.add_argument("--effort", choices=["low", "medium", "high", "xhigh", "max"],
                    help="single-effort mode: run exactly this rung for every pair")
    ap.add_argument("--no-submit", action="store_true",
                    help="skip judge submission; record local compile+sample verdict only")
    args = ap.parse_args()
    if args.xhigh_backfill:
        xhigh_backfill()
        return

    problems = json.load(open(args.problems_file, encoding="utf-8"))
    if args.only_problem:
        problems = [p for p in problems if f"{p['contestId']}{p['index']}" == args.only_problem]
    solved, attempted, submit_errored = load_done()

    single_effort = [args.effort] if args.effort else None
    lanes = [(m, run_codex) for m in CODEX_MODELS] + [(CLAUDE_MODEL, run_claude)]
    if args.only_model:
        lanes = [(m, fn) for m, fn in lanes if m == args.only_model]
    pause_file = "claude.paused"
    if os.path.exists(pause_file):
        try:
            until = float(open(pause_file).read().strip() or 0)
        except ValueError:
            until = float("inf")
        if time.time() < until:
            print(f">>> claude lane paused until epoch {until} (claude.paused)", flush=True)
            lanes = [(m, fn) for m, fn in lanes if m != CLAUDE_MODEL]

    jobs = []
    for p in problems:
        for model, fn in lanes:
            jobs.append((p, model, fn))
    # Agents run in parallel; submissions serialize on channel locks (judge POST
    # budget is the scarce resource, agent runs are not).
    max_workers = int(os.environ.get("CF_MAX_WORKERS", str(len(lanes))))
    with ThreadPoolExecutor(max_workers=max_workers) as ex:
        futs = [ex.submit(safe_solve, p, m, fn, solved, attempted, submit_errored,
                          single_effort, args.full_ladder, args.no_submit)
                for p, m, fn in jobs]
        for f in futs:
            f.result()


if __name__ == "__main__":
    main()
