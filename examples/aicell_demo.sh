#!/bin/sh
# Run aicell daemon and client demo
./build/aicell_daemon &
DAEMON=$!
sleep 1
./build/aicell_client
kill $DAEMON
