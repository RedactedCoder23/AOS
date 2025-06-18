#!/usr/bin/env bash
set -euo pipefail
set -e
out=$(./build/fs_demo)
echo "$out"
echo "$out" | grep -q "read: hello"
