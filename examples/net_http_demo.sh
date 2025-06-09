#!/bin/sh
# Demo for basic HTTP server
PORT=9001
./build/http_server --port $PORT &
PID=$!
sleep 1
curl -s http://127.0.0.1:$PORT/
kill $PID 2>/dev/null
