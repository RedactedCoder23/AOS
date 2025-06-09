#!/bin/sh
# Demo for plugin loader with error check
set -e
out=$(./build/plugin_demo 2>&1)
echo "$out"
echo "$out" | grep -q "missing plugin handled"
