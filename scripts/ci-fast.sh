#!/usr/bin/env bash
set -e

make fast-test
python -m compileall docs >/dev/null
