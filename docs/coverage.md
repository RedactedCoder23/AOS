# Coverage Recorder

After running unit tests the CI workflow records coverage with:

```
coverage run -m pytest
coverage json -o coverage.json
```

`scripts/coverage_recorder.py` moves this file into
`~/.aos/branches/<id>/coverage-YYYYMMDD.json`. The API aggregates all files and
returns:

```
{
  "coverage": [{"date":"20250612","lines":83.4,"branches":79.1}],
  "threshold": 75.0
}
```

### Gatekeeper

Merges are blocked if the newest entry falls below `coverage_threshold` in
`config.yml`. The merge endpoint returns HTTP 412 and audit action
`merge_blocked` when this happens.
