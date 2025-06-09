#!/bin/sh
# Smoke test for fs subsystem
set -e
out=$(./build/fs_demo)
echo "$out"
echo "$out" | grep -q "read: hello"
