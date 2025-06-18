/* AOS — src/branch/policy.py — Purpose: Python module */
# AOS — ./src/branch/policy.py
# (c) 2025 RedactedCoder23
# Brief: Source file.
import json
import os
from src.models.branch import Branch


_POLICY_PATH = os.path.expanduser("~/.aos/policy.json")


def _load() -> dict:
    if not os.path.exists(_POLICY_PATH):
        return {}
    with open(_POLICY_PATH, "r", encoding="utf-8") as fh:
        return json.load(fh)


def allowed_merge(branch: Branch, target: str) -> bool:
    cfg = _load()
    if cfg.get("no_direct_main") and target == "main":
        return False
    if cfg.get("require_review_tag") and "review" not in branch.tags:
        return False
    return True
