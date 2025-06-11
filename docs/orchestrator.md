# Orchestrator

The agent orchestrator spawns lightweight workers to execute branch tasks. Task
definitions are loaded into instances of the `Task` dataclass; dictionary
formats are converted on load and not used elsewhere.
Agent count is adjusted automatically based on pending task count and host
load. The scaling algorithm computes a desired count using:

```
desired = ceil(pending_tasks / TASKS_PER_AGENT)
```

The value is clamped to `MAX_AGENTS` and at most one agent is spawned or
stopped per cycle. A `scaling` server-sent event is emitted whenever the
active agent count changes.
