# Async Job Queue

The job queue uses Redis and RQ to execute provider jobs in the background.
Tasks update a status file under `~/.aos/branches/<id>/status.json` so the web
APIs can report progress. A lightweight worker consumes tasks by running
`scripts/jobqueue/worker.py`.

## Workflow
1. `enqueue_provider_job()` stores a job and publishes `QUEUED` status.
2. `worker.py` processes tasks and updates status to `RUNNING` and `COMPLETE`.
3. WebSocket and metrics endpoints stream status updates to the dashboard.
