#!/bin/bash
# August 2026 Div2 E/F benchmark: full ladder per pair (no stop-on-AC),
# per-effort timing in results.jsonl, minimum viable effort -> runs/<problem>/mve.json
cd /e/workspace/ai/eval_codeforces
for i in $(seq 1 3000); do
  echo "=== august pass $i $(date +%H:%M:%S) ===" >> logs/full_run.log
  python runner.py --problems-file data/problems_aug.json --full-ladder >> logs/full_run.log 2>&1
  rc=$?
  eval "$(python - <<'PY'
import json
from collections import defaultdict
rows = [json.loads(l) for l in open('results.jsonl', encoding='utf-8') if l.strip()]
problems = json.load(open('data/problems_aug.json', encoding='utf-8'))
CLAUDE = 'claude-sonnet-5-5'
MODELS = ['gpt-6.1-sol', 'gpt-6-luna', 'gpt-5.6-terra', 'gpt-5.6-luna', CLAUDE]
pairs = defaultdict(set)
for r in rows:
    if r.get('effort') == 'LANE_ERROR' or (r.get('verdict') or '').startswith('SUBMIT_ERROR'):
        continue
    pairs[(r['problem'], r['model'])].add(r['effort'])
def need(m):
    return {'medium','high','xhigh','max'} if m != CLAUDE else {'medium','high','max'}
done = total = 0
for p in problems:
    key = f"{p['contestId']}{p['index']}"
    for m in MODELS:
        total += 1
        if need(m) <= pairs.get((key, m), set()):
            done += 1
print(f"TERMINAL={done} TOTAL={total}")
PY
)"
  echo "=== august pass $i rc=$rc terminal=$TERMINAL/$TOTAL ===" >> logs/full_run.log
  if [ "$TERMINAL" -ge "$TOTAL" ]; then echo "AUGUST ALL DONE" >> logs/full_run.log; break; fi
  sleep 60
done
