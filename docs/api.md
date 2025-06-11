# API Endpoints

## GET /branches/{id}/metrics
Returns live orchestrator statistics.

```
{
  "pending_tasks": 3,
  "cpu_pct": 12.5,
  "mem_pct": 42.1,
  "history": [
    {"timestamp": "2025-06-12T10:00:00Z", "pending_tasks": 3, "cpu_pct": 12.5, "mem_pct": 42.1}
  ]
}
```

## GET /branches/{id}/coverage-history
Returns coverage history and current threshold.

```
{
  "coverage": [
    {"date": "20250612", "lines": 83.4, "branches": 79.1}
  ],
  "threshold": 75.0
}
```
