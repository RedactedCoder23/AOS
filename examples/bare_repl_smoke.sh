#!/bin/sh
set -e
log="AOS-CHECKLIST.log"
pass=1
run_qemu() {
  printf "%s" "$1" | timeout 10 qemu-system-x86_64 -nographic -drive format=raw,file=aos.bin 2>&1
}

pos_cmds="$(cat <<'CMDS'
MEM_ALLOC 32
FS_OPEN foo w
BR_CREATE test
help
exit
CMDS
)"

neg_cmds="$(cat <<'CMDS'
MEM_ALLOC 999999999
FS_OPEN bad z
UNKNOWN_CMD
exit
CMDS
)"

pos_out=$(run_qemu "$pos_cmds")
neg_out=$(run_qemu "$neg_cmds")

check_pass() { echo "PASS: $1"; }
check_fail() { echo "FAIL: $1" | tee -a "$log"; pass=0; }

echo "$pos_out" | grep -q "Allocated" && check_pass MEM_ALLOC || check_fail MEM_ALLOC
echo "$pos_out" | grep -q "fd=" && check_pass FS_OPEN || check_fail FS_OPEN
echo "$pos_out" | grep -q "Created" && check_pass BR_CREATE || check_fail BR_CREATE
echo "$pos_out" | grep -q "MEM_ALLOC" && check_pass help || check_fail help

err_count=$(echo "$neg_out" | grep -c "Error")
[ "$err_count" -ge 2 ] && check_pass errors || check_fail errors
echo "$neg_out" | grep -q "Unknown command" && check_pass unknown || check_fail unknown

if [ $pass -eq 1 ]; then
  echo "bare-smoke PASS"
else
  echo "bare-smoke FAIL" | tee -a "$log"
  exit 1
fi
