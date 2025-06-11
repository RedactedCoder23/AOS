#!/usr/bin/env bash
set -e

# Build all components and run tests
make build
make test
coverage run -m pytest
coverage json -o coverage.json

docker-compose -f demo/docker-compose.yml up --build --abort-on-container-exit

