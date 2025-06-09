#!/bin/sh
# Demo for TCP echo
./build/net_echo --server --port 12345 &
PID=$!
sleep 1
./build/net_echo --host 127.0.0.1 --port 12345
kill $PID
