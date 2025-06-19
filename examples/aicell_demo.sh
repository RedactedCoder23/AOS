#!/usr/bin/env bash
set -euo pipefail
command -v qemu-system-x86_64 || { echo; exit 1; }
./build/aicell_daemon &
DAEMON=$!
sleep 1
./build/aicell_client
kill $DAEMON
