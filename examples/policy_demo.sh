#!/usr/bin/env bash
set -euo pipefail
command -v qemu-system-x86_64 || { echo; exit 1; }
set -e
> AOS-CHECKLIST.log
> AOS-AUDIT.log
out=$(./build/policy_demo)
echo "$out"
echo "$out" | grep -q "allowed"
echo "$out" | grep -q "denied"
grep -q "policy deny" AOS-CHECKLIST.log
grep -q "FS_WRITE DENY" AOS-AUDIT.log

