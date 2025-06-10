#!/bin/sh
# Demo for branch dashboard and ipc host
set -e
./build/ipc_host &
HOST_PID=$!
sleep 1
printf 'c\nnew\nq\n' | ./build/branch_dashboard
kill $HOST_PID
