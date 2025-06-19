#!/usr/bin/env bash
set -euo pipefail
command -v qemu-system-x86_64 || { echo; exit 1; }
./build/net_echo --server --port 12345 &
PID=$!
sleep 1
./build/net_echo --host 127.0.0.1 --port 12345
kill $PID
