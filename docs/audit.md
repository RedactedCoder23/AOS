# Audit Logging

Audit events are stored as newline-delimited JSON in `/var/log/aos-audit.log`.
Each line is a single JSON object with the following fields:

```
{
  "timestamp": "2025-06-10T12:23:45Z",
  "user": "alice",
  "action": "branch_create",
  "resource": "branch:42",
  "result": "success"
}
```

Use the `aos-audit show` command to inspect the log. Example:

```
aos-audit show --action=branch_create
```

Additional filters are `--user` and `--resource`. The command reads the log
file line by line, ignoring malformed entries, and prints matching records as
pretty printed JSON.

Use `aos_audit.warn("message")` to record warnings. These are logged with
`action: "warning"` and a `message` field.

New actions recorded:
- `get_metrics` – fetching branch metrics
- `get_coverage` – retrieving coverage history
- `merge_blocked` – merge denied due to low coverage
