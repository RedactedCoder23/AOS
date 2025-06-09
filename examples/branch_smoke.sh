#!/bin/sh
# Smoke test for branch subsystem
set -e
out=$(./build/branch_demo)
echo "$out"
echo "$out" | grep -q "running"
