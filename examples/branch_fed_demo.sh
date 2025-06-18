#!/usr/bin/env bash
set -euo pipefail
PORT1=10000
PORT2=10001

./build/branch_fed_demo --port $PORT1 &
S1=$!
sleep 1
./build/branch_fed_demo --port $PORT2 --peer 127.0.0.1:$PORT1 --sync
wait $S1
