#!/usr/bin/env bash
set -euo pipefail
command -v qemu-system-x86_64 || { echo; exit 1; }
set -e
out=$(./build/fs_demo)
echo "$out"
echo "$out" | grep -q "read: hello"
