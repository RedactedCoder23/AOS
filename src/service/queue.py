# AOS — ./src/service/queue.py
# (c) 2025 RedactedCoder23
# Brief: Source file.
import json
import os
import sys

if "queue" in sys.modules and getattr(sys.modules["queue"], "__file__", "").startswith(
    os.path.join(os.path.dirname(__file__), "..", "scripts", "queue")
):
    del sys.modules["queue"]

import redis
from rq import Queue
from scripts.jobqueue import tasks

_redis = redis.Redis()
_queue = Queue(connection=_redis)


def enqueue_provider_job(branch_id: str, provider: str, prompt: str) -> str:
    """Enqueue provider job and persist parameters."""
    job = _queue.enqueue(tasks.provider_job, provider, prompt, branch_id)
    tasks.update_status(branch_id, "QUEUED", job.id)
    path = os.path.expanduser(
        os.path.join("~/.aos/branches", str(branch_id), "status.json")
    )
    data: dict[str, str] = {}
    if os.path.exists(path):
        try:
            with open(path, "r", encoding="utf-8") as fh:
                data = json.load(fh)
        except Exception:
            data = {}
    data["provider"] = provider
    data["prompt"] = prompt
    with open(path, "w", encoding="utf-8") as fh:
        json.dump(data, fh)
    return job.id


def job_params(branch_id: str) -> tuple[str, str] | None:
    """Return stored provider and prompt for *branch_id* if present."""
    info = load_status(branch_id)
    provider = info.get("provider")
    prompt = info.get("prompt")
    if provider and prompt:
        return str(provider), str(prompt)
    return None


def load_status(branch_id: str) -> dict:
    path = os.path.expanduser(
        os.path.join("~/.aos/branches", str(branch_id), "status.json")
    )
    if not os.path.exists(path):
        return {}
    try:
        with open(path, "r", encoding="utf-8") as fh:
            return json.load(fh)
    except Exception:
        return {}
