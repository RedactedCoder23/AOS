# AOS — scripts/jobqueue/worker.py — Purpose: Python module 
# AOS — ./scripts/jobqueue/worker.py
# (c) 2025 RedactedCoder23
# Brief: Source file.
import threading

import redis
from rq import Worker, Queue, Connection

import uvicorn

from src.api.health import app as health_app


def _run_health() -> None:
    """Start health probe server in a background thread."""
    uvicorn.run(health_app, host="0.0.0.0", port=8000, log_level="info")


def main():
    t = threading.Thread(target=_run_health, daemon=True)
    t.start()

    redis_conn = redis.Redis()
    with Connection(redis_conn):
        worker = Worker([Queue()])
        worker.work()


if __name__ == "__main__":
    main()
