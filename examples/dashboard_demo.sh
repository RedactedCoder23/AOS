#!/usr/bin/env bash
set -euo pipefail
command -v qemu-system-x86_64 || { echo; exit 1; }
set -e
./build/ipc_host &
HOST_PID=$!
sleep 1
printf 'c\nnew\nq\n' | ./build/branch_dashboard
kill $HOST_PID
