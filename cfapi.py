"""Minimal signed Codeforces REST API client (read-only scope).

Usage: python cfapi.py <method> [k=v ...]
Reads apiKey from agent.key and apiSecret from agent.secret.
Enforces the 1 request / 2 seconds rate limit.
"""
import hashlib
import json
import random
import string
import sys
import time
import urllib.parse
import urllib.request

import os as _os

def _load_creds():
    """API credentials are optional: only signed calls need them, and every
    benchmark endpoint is public. Read from env or key files if present."""
    for d in ("", "keys"):
        k, s = _os.path.join(d, "agent.key"), _os.path.join(d, "agent.secret")
        if _os.path.exists(k) and _os.path.exists(s):
            return open(k).read().strip(), open(s).read().strip()
    return None, None

API_KEY, API_SECRET = _load_creds()
BASE = "https://codeforces.com/api/"
_last_call = [0.0]


def _api_sig(method: str, params: dict) -> str:
    if not (API_KEY and API_SECRET):
        raise RuntimeError("signed API call requested but no API credentials present")
    rand = "".join(random.choice(string.ascii_lowercase + string.digits) for _ in range(6))
    items = sorted(params.items()) + [("apiKey", API_KEY)]
    plain = method + "/" + "/".join(f"{k}:{v}" for k, v in items) + "#" + API_SECRET
    return rand + hashlib.sha512(plain.encode()).hexdigest()


def call(method: str, signed: bool = True, **params) -> dict:
    wait = 2.0 - (time.monotonic() - _last_call[0])
    if wait > 0:
        time.sleep(wait)
    _last_call[0] = time.monotonic()

    params = {k.rstrip("_"): str(v) for k, v in params.items() if v is not None}
    if signed:
        params["time"] = str(int(time.time()))
        params["apiSig"] = _api_sig(method, params)
    url = BASE + method + "?" + urllib.parse.urlencode(params)
    with urllib.request.urlopen(url, timeout=60) as r:
        data = json.load(r)
    if data.get("status") != "OK":
        raise RuntimeError(f"API error: {data}")
    return data["result"]


if __name__ == "__main__":
    method = sys.argv[1]
    kwargs = {}
    for arg in sys.argv[2:]:
        k, v = arg.split("=", 1)
        kwargs[k] = v
    print(json.dumps(call(method, **kwargs), indent=2)[:4000])
