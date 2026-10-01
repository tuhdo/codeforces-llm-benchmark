"""Filter data/problems_full.json to August 2026 contests -> data/problems_aug.json."""
import datetime
import json
import os

os.chdir(os.path.dirname(os.path.abspath(__file__)))
from cfapi import call  # noqa: E402

LO = int(datetime.datetime(2026, 8, 1, tzinfo=datetime.timezone.utc).timestamp())
HI = int(datetime.datetime(2026, 9, 1, tzinfo=datetime.timezone.utc).timestamp())

contests = call("contest.list", signed=False)
aug_ids = {c["id"] for c in contests if LO <= c.get("startTimeSeconds", 0) < HI}

problems = json.load(open("data/problems_full.json", encoding="utf-8"))
aug = [p for p in problems if p["contestId"] in aug_ids]
json.dump(aug, open("data/problems_aug.json", "w", encoding="utf-8"), indent=2)

print(f"{len(aug)} August 2026 E/F problems (contests {sorted(aug_ids)}):")
for p in aug:
    print(f"  {p['contestId']}{p['index']} {p['contestName']} - {p['name']}")
