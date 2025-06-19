#!/usr/bin/env bash
set -euo pipefail
command -v qemu-system-x86_64 || { echo; exit 1; }
set -e
out=$(./build/plugin_demo 2>&1)
echo "$out"
echo "$out" | grep -q "missing plugin handled"
echo "$out" | grep -q "Unloaded sample"
