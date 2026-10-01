# codeforces-llm-benchmark

Benchmark harness that runs coding-CLI agents (OpenAI codex models and
Claude) on Codeforces problems and scores them with real judge verdicts.

## How it works

```text
fetchers/          problem discovery + statements and sample tests (official API + pages)
runner.py          per (problem, model, effort): isolated agent run -> local gate -> judge submission
cfsubmit.py        web-form submission with per-attempt tokens; verdict polling via the official API
report.py          per-model / per-problem aggregation; per-problem minimum-viable-effort files
```

- Each attempt runs in an isolated directory containing only the statement
  and samples; agents may compile and test locally, and must write a single
  C++20 file.
- Every submission is verified through the official Codeforces API
  (`user.status`), never trusted from HTML responses.
- Sessions are supplied by the operator (never committed); all credentials
  and run artifacts are git-ignored.

## Scope

- September 2026 rounds (Div 1/2/3/4), all E/F problems - complete for the
  GPT models; Claude ladders partially gated by provider usage limits.
- August 2026 Division 2 rounds, all E/F problems - low-effort pass
  (one attempt per model), higher efforts pending.

## Results snapshot

See `REPORT.md` and `results.jsonl` (one row per attempt: verdict, wall
time, submission id). Per-problem minimum-viable-effort files live under
`runs/<problem>/mve.json`; solutions as `runs/<problem>/sol_<model>_<effort>.cpp`.

Headline (judge-scored, per model): gpt-6.1-sol solves every attempted
problem at medium effort; gpt-5.6-terra is the fastest per solve;
gpt-6-luna and gpt-5.6-luna trade tiers for speed; claude-sonnet-5-5
holds medium on nearly everything it ran.

## Operator notes

- `cfapi.py <method> k=v ...` for quick API queries (public endpoints).
- Sessions are read from `session.txt` / `session2.txt` (cookie lines);
  keep them private and refresh them when Codeforces rotates sessions.
- The judge's submission budget rewards pacing: attempts are serialized
  and failures are replayed on later passes.
