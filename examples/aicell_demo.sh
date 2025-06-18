#!/usr/bin/env bash
set -euo pipefail
./build/aicell_daemon &
DAEMON=$!
sleep 1
./build/aicell_client
kill $DAEMON
