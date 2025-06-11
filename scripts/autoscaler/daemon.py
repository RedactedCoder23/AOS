from __future__ import annotations

import os
import time

import docker
import redis
from rq import Queue

AOS_RQ_MIN = int(os.environ.get("AOS_RQ_MIN", "1"))
AOS_RQ_MAX = int(os.environ.get("AOS_RQ_MAX", "10"))
CHECK_INTERVAL = int(os.environ.get("CHECK_INTERVAL", "5"))
WORKER_IMAGE = os.environ.get("AOS_WORKER_IMAGE", "aos:latest")
WORKER_CMD = os.environ.get("AOS_WORKER_CMD", "python scripts/jobqueue/worker.py")
LABEL = "AOS_WORKER"


def _running_workers(client: docker.DockerClient):
    return client.containers.list(filters={"label": LABEL})


def _start_worker(client: docker.DockerClient) -> None:
    client.containers.run(
        WORKER_IMAGE,
        WORKER_CMD,
        detach=True,
        labels={LABEL: "1"},
        restart_policy={"Name": "unless-stopped"},
    )


def _stop_worker(container: docker.models.containers.Container) -> None:
    try:
        container.stop()
        container.remove()
    except Exception:
        pass


# If Redis is unreachable skip scaling decisions to avoid killing all workers
# unintentionally.
def check_and_scale(client: docker.DockerClient, queue: Queue) -> None:
    try:
        qlen = queue.count
    except Exception:
        return

    workers = _running_workers(client)
    desired = max(AOS_RQ_MIN, min(AOS_RQ_MAX, qlen))
    running = len(workers)

    if running < desired:
        for _ in range(desired - running):
            _start_worker(client)
    elif running > desired:
        # Stop at most one worker per cycle to reduce rapid flaps when the
        # queue drains momentarily.
        for cont in workers[desired : desired + 1]:
            _stop_worker(cont)


def main() -> None:
    client = docker.from_env()
    redis_client = redis.Redis()
    queue = Queue(connection=redis_client)
    while True:
        check_and_scale(client, queue)
        time.sleep(CHECK_INTERVAL)


if __name__ == "__main__":
    main()
