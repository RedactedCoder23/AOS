#!/usr/bin/env bash
set -e

# Build all components and run tests
make build
make test
coverage run -m pytest
coverage json -o coverage.json


