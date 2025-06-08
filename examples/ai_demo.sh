#!/bin/sh
set -e
make ai
printf "hello world\n" | ./build/ai_demo
