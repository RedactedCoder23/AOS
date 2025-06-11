import json
import os
import sys
import importlib

if 'queue' in sys.modules and getattr(sys.modules['queue'], '__file__', '').startswith(os.path.join(os.path.dirname(__file__), '..', 'scripts', 'queue')):
    del sys.modules['queue']

import redis
from rq import Queue
from scripts.jobqueue import tasks

_redis = redis.Redis()
_queue = Queue(connection=_redis)


def enqueue_provider_job(branch_id: str, provider: str, prompt: str) -> str:
    job = _queue.enqueue(tasks.provider_job, provider, prompt, branch_id)
    tasks.update_status(branch_id, "QUEUED", job.id)
    return job.id


def load_status(branch_id: str) -> dict:
    path = os.path.expanduser(os.path.join("~/.aos/branches", str(branch_id), "status.json"))
    if not os.path.exists(path):
        return {}
    try:
        with open(path, "r", encoding="utf-8") as fh:
            return json.load(fh)
    except Exception:
        return {}
