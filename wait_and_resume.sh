#!/bin/bash
# Wait for the orphaned runner pass to finish (no codex.exe, no runner.py python),
# then relaunch the auto-resume loop.
cd /e/workspace/ai/eval_codeforces
while true; do
  codex_n=$(tasklist //FI "IMAGENAME eq codex.exe" 2>/dev/null | grep -c codex.exe)
  runner_n=$(powershell -NoProfile -Command "(Get-CimInstance Win32_Process -Filter \"Name='python.exe'\" | Where-Object { \$_.CommandLine -match 'runner.py' } | Measure-Object).Count" 2>/dev/null)
  echo "[wait] codex=$codex_n runner=$runner_n $(date +%H:%M:%S)" >> logs/full_run.log
  if [ "$codex_n" -eq 0 ] && [ "$runner_n" -eq 0 ]; then break; fi
  sleep 90
done
sleep 30
echo "[wait] orphan gone, relaunching loop $(date +%H:%M:%S)" >> logs/full_run.log
exec bash run_until_done.sh
