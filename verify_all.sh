#!/usr/bin/env bash
set -e

# Run unit and integration tests
make test-unit
make test-integration
