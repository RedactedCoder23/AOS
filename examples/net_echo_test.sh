#!/usr/bin/env bash
set -euo pipefail
command -v qemu-system-x86_64 || { echo; exit 1; }
set -e
./build/net_echo --server --port 23456 &
server=$!
sleep 1
out=$(./build/net_echo --host 127.0.0.1 --port 23456)
kill $server 2>/dev/null || true
[ "$out" = "ping" ]
echo "$out"
