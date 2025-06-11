#!/usr/bin/env bash
set -e
make build
make test
docker-compose -f demo/docker-compose.yml up --build --abort-on-container-exit
