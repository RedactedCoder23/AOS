/* AOS — src/logging/audit.py — Purpose: Python module */
# AOS — ./src/logging/audit.py
# (c) 2025 RedactedCoder23
# Brief: Source file.
import hashlib
import json
import os
import time
from typing import Any

_LOG_DIR = os.path.expanduser("~/.aos")


def _log_path() -> str:
    day = time.strftime("%Y%m%d")
    os.makedirs(_LOG_DIR, exist_ok=True)
    return os.path.join(_LOG_DIR, f"audit.{day}.log")


def log_call(user: str, route: str, payload: Any) -> None:
    entry = {
        "timestamp": time.strftime("%Y-%m-%dT%H:%M:%SZ", time.gmtime()),
        "user": user,
        "route": route,
        "hash": hashlib.sha256(
            json.dumps(payload, sort_keys=True).encode()
        ).hexdigest(),
    }
    with open(_log_path(), "a", encoding="utf-8") as fh:
        fh.write(json.dumps(entry) + "\n")
