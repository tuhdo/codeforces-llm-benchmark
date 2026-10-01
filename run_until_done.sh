#!/bin/bash
# Auto-resume loop: rerun until every (problem, model) pair reaches a terminal state.
# Codex tiers: medium/high/xhigh/max. Claude tiers: medium/high/max.
cd /e/workspace/ai/eval_codeforces
for i in $(seq 1 3000); do
  echo "=== runner pass $i $(date +%H:%M:%S) ===" >> logs/full_run.log
  python runner.py >> logs/full_run.log 2>&1
  rc=$?
  eval "$(python - <<'PY'
import json
from collections import defaultdict
rows = [json.loads(l) for l in open('results.jsonl', encoding='utf-8') if l.strip()]
problems = json.load(open('data/problems_full.json', encoding='utf-8'))
CLAUDE = 'claude-sonnet-5-5'
MODELS = ['gpt-6.1-sol', 'gpt-6-luna', 'gpt-5.6-terra', 'gpt-5.6-luna', CLAUDE]
pairs = defaultdict(set)
solved = set()
for r in rows:
    if r.get('effort') == 'LANE_ERROR':
        continue
    pairs[(r['problem'], r['model'])].add(r['effort'])
    if r['solved']:
        solved.add((r['problem'], r['model']))
def need(m):
    return {'medium','high','xhigh','max'} if m != CLAUDE else {'medium','high','max'}
done = sum(1 for p in problems for m in MODELS
           if (f"{p['contestId']}{p['index']}", m) in solved
           or not (need(m) - pairs.get((f"{p['contestId']}{p['index']}", m), set())))
total = len(problems) * len(MODELS)
print(f"TERMINAL={done} TOTAL={total}")
PY
)"
  echo "=== pass $i rc=$rc terminal_pairs=$TERMINAL/$TOTAL ===" >> logs/full_run.log
  if [ "$TERMINAL" -ge "$TOTAL" ]; then echo "ALL DONE" >> logs/full_run.log; break; fi
  sleep 60
done
