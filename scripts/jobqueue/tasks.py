/* AOS — scripts/jobqueue/tasks.py — Purpose: Python module */
# AOS — ./scripts/jobqueue/tasks.py
# (c) 2025 RedactedCoder23
# Brief: Source file.
import json
import os
import time
import redis
from scripts.ai_providers import loader

_redis = redis.Redis()


def _status_path(branch_id: str) -> str:
    base = os.path.expanduser(os.path.join("~/.aos/branches", str(branch_id)))
    os.makedirs(base, exist_ok=True)
    return os.path.join(base, "status.json")


def update_status(branch_id: str, status: str, job_id: str | None = None) -> None:
    path = _status_path(branch_id)
    data = {}
    if os.path.exists(path):
        try:
            with open(path, "r", encoding="utf-8") as fh:
                data = json.load(fh)
        except Exception:
            data = {}
    ts_key = f"{status.lower()}_at"
    data["status"] = status
    data[ts_key] = time.strftime("%Y-%m-%dT%H:%M:%SZ", time.gmtime())
    if job_id is not None:
        data["job_id"] = job_id
    with open(path, "w", encoding="utf-8") as fh:
        json.dump(data, fh)
    try:
        _redis.publish(f"branch:{branch_id}", json.dumps({"status": status}))
    except Exception:
        pass


def provider_job(provider: str, prompt: str, branch_id: str) -> str:
    """Run provider generation task."""
    update_status(branch_id, "RUNNING")
    try:
        prov = loader.get_provider(provider)
        result = prov.generate(prompt)
        update_status(branch_id, "COMPLETE")
        _redis.publish(
            f"branch:{branch_id}", json.dumps({"status": "COMPLETE", "result": result})
        )
        return result
    except Exception as exc:  # pragma: no cover - provider failure
        update_status(branch_id, "FAILED")
        _redis.publish(
            f"branch:{branch_id}", json.dumps({"status": "FAILED", "error": str(exc)})
        )
        raise
