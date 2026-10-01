#!/bin/bash
# August 2026 Div2 E/F: effort-major LOW-effort pass, local verdicts (no judge).
# For effort=low: for each problem: run all 5 models, one attempt each.
cd /e/workspace/ai/eval_codeforces
for i in $(seq 1 3000); do
  echo "=== august-low pass $i $(date +%H:%M:%S) ===" >> logs/full_run.log
  python runner.py --problems-file data/problems_aug.json --full-ladder --effort low >> logs/full_run.log 2>&1
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
    if r.get('effort') == 'LANE_ERROR':
        continue
    pairs[(r['problem'], r['model'])].add(r['effort'])
done = total = 0
for p in problems:
    key = f"{p['contestId']}{p['index']}"
    for m in MODELS:
        total += 1
        if 'low' in pairs.get((key, m), set()):
            done += 1
print(f"TERMINAL={done} TOTAL={total}")
PY
)"
  echo "=== august-low pass $i rc=$rc low_done=$TERMINAL/$TOTAL ===" >> logs/full_run.log
  if [ "$TERMINAL" -ge "$TOTAL" ]; then echo "AUGUST-LOW ALL DONE" >> logs/full_run.log; break; fi
  sleep 60
done
