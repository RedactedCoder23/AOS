import json
import os
from typing import List, Optional

from src.models.branch import Branch


_PATH = os.path.expanduser("~/.aos/branches.json")


def _load() -> List[dict]:
    if not os.path.exists(_PATH):
        return []
    with open(_PATH, "r", encoding="utf-8") as fh:
        return json.load(fh).get("branches", [])


def _save(data: List[dict]) -> None:
    os.makedirs(os.path.dirname(_PATH), exist_ok=True)
    with open(_PATH, "w", encoding="utf-8") as fh:
        json.dump({"branches": data}, fh)


def list_branches() -> List[Branch]:
    return [Branch(**b) for b in _load()]


def get_branch(bid: int) -> Optional[Branch]:
    for b in _load():
        if b.get("id") == bid:
            return Branch(**b)
    return None


def save_branch(branch: Branch) -> None:
    data = _load()
    found = False
    for idx, b in enumerate(data):
        if b.get("id") == branch.id:
            data[idx] = branch.__dict__
            found = True
            break
    if not found:
        data.append(branch.__dict__)
    _save(data)
