# API Endpoints

## GET /branches/{id}/metrics
Returns JSON with pending_tasks, cpu_pct, mem_pct and history.

```
{
  "pending_tasks": 0,
  "cpu_pct": 0.0,
  "mem_pct": 0.0,
  "history": []
}
```

## GET /branches/{id}/coverage-history
Returns coverage history and current threshold.

```
{
  "coverage": [],
  "threshold": 0.0
}
```
