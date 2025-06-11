#!/usr/bin/env bash
set -e

pre-commit run --files $(git ls-files '*.py' '*.c' '*.h' '*.yaml')
pytest -m "not slow" -q tests/python --ignore tests/python/test_ui_playwright.py
python -m compileall docs >/dev/null
