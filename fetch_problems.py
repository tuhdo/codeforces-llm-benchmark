"""Fetch September 2026 contests from contest.list and select all E/F problems.

Output: data/sept2026_ef.json
"""
import datetime
import json
import os

os.chdir(os.path.dirname(os.path.abspath(__file__)))
from cfapi import call  # noqa: E402

WINDOW_LO = int(datetime.datetime(2026, 9, 1, tzinfo=datetime.timezone.utc).timestamp())
WINDOW_HI = int(datetime.datetime(2026, 10, 1, tzinfo=datetime.timezone.utc).timestamp())

contests = call("contest.list", signed=False)
sep = sorted(
    (c for c in contests if WINDOW_LO <= c.get("startTimeSeconds", 0) < WINDOW_HI),
    key=lambda c: c["startTimeSeconds"],
)

selected = []
seen_names = set()
for c in sep:
    # Anonymous call with contestId only - API rejects extra params for non-admins.
    standings = call("contest.standings", signed=False, contestId=c["id"])
    for p in standings["problems"]:
        if p["index"] in ("E", "F") and p["name"] not in seen_names:
            seen_names.add(p["name"])
            selected.append({
                "contestId": c["id"],
                "contestName": c["name"],
                "index": p["index"],
                "name": p["name"],
                "rating": p.get("rating"),
                "tags": p.get("tags", []),
            })

with open("data/sept2026_ef.json", "w", encoding="utf-8") as f:
    json.dump(selected, f, indent=2)

print(f"{len(sep)} contests, {len(selected)} E/F problems")
for p in selected:
    print(f"{p['contestId']}{p['index']:<2} rating={p['rating']}  {p['name']}")
