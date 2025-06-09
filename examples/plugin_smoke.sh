#!/bin/sh
# Smoke test for plugin subsystem
set -e
out=$(./build/plugin_demo 2>&1)
echo "$out"
echo "$out" | grep -q "missing plugin handled"
