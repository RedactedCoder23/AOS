# Coverage Recorder

After running tests the orchestrator writes `coverage.json` containing line
coverage percentages. History is kept in `history.json` per branch and can be
queried via `/branches/<id>/coverage-history`. Branch merges are blocked if the
latest coverage is below `coverage_threshold` configured in `rules.yaml`.
