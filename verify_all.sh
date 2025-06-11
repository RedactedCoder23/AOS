#!/usr/bin/env bash
set -e
<<<<<< codex/add-verify-all-target-to-makefile

# Run unit and integration tests
make test-unit
make test-integration
=======
make build
make test
docker-compose -f demo/docker-compose.yml up --build --abort-on-container-exit
>>>>>> main
