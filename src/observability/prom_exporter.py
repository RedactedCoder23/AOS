# AOS — ./src/observability/prom_exporter.py
# (c) 2025 RedactedCoder23
# Brief: Source file.
from fastapi import FastAPI, Response
from src.service.security import apply_security_headers
import time
import redis
from rq import Queue, Worker

__all__ = ["expose_metrics"]

app = FastAPI()
apply_security_headers(app)
_r = redis.Redis()
_q = Queue(connection=_r)


def _queue_len() -> int:
    try:
        return len(_q)
    except Exception:
        return 0


def _worker_count() -> int:
    try:
        return len(Worker.all(connection=_r))
    except Exception:
        return 0


def _job_latency() -> float:
    try:
        jobs = _q.jobs
        if not jobs:
            return 0.0
        total = sum(time.time() - j.enqueued_at.timestamp() for j in jobs)
        return total / len(jobs)
    except Exception:
        return 0.0


@app.get("/metrics")
def metrics() -> Response:
    q_len = _queue_len()
    workers = _worker_count()
    latency = _job_latency()
    body = (
        "# TYPE queue_len gauge\n"
        f"queue_len {q_len}\n"
        "# TYPE job_latency gauge\n"
        f"job_latency {latency}\n"
        "# TYPE worker_count gauge\n"
        f"worker_count {workers}\n"
    )
    return Response(content=body, media_type="text/plain; version=0.0.4")


def expose_metrics() -> FastAPI:
    """Return the FastAPI app exposing Prometheus metrics."""
    return app


if __name__ == "__main__":
    import uvicorn

    uvicorn.run(app, port=8000)
