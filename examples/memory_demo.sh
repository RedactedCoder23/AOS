#!/bin/sh
# Demo for memory subsystem with error cases
set -e
> AOS-CHECKLIST.log
out=$(./build/memory_demo)
echo "$out"
echo "--- log ---"
cat AOS-CHECKLIST.log
echo "$out" | grep -q "expected NULL"
grep -q "Out of memory" AOS-CHECKLIST.log
grep -q "invalid pointer" AOS-CHECKLIST.log
