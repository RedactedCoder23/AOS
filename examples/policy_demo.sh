#!/bin/sh
# Demo for policy engine with deny rule
set -e
> AOS-CHECKLIST.log
out=$(./build/policy_demo)
echo "$out"
echo "$out" | grep -q "allowed"
echo "$out" | grep -q "denied"
grep -q "policy deny" AOS-CHECKLIST.log
