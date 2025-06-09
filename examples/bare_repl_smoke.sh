#!/bin/sh
# Run smoke tests for the bare-metal REPL. Errors are tolerated so we can
# report failures instead of exiting early.
set +e

log="AOS-CHECKLIST.log"
pass=1

# run_qemu <commands>
# Feeds the command list to QEMU and returns its output. QEMU may not run in
# this environment, so capture whatever error is produced.
run_qemu() {
  printf "%s" "$1" | timeout 10 qemu-system-x86_64 -nographic -drive format=raw,file=aos.bin 2>&1
  return $?
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

pos_out=$(run_qemu "$pos_cmds"); pos_rc=$?
neg_out=$(run_qemu "$neg_cmds"); neg_rc=$?

check_pass() { echo "PASS: $1"; }
check_fail() { echo "FAIL: $1" | tee -a "$log"; pass=0; }

echo "$pos_out" | grep -q "Allocated" && check_pass MEM_ALLOC || check_fail MEM_ALLOC
echo "$pos_out" | grep -q "fd=" && check_pass FS_OPEN || check_fail FS_OPEN
echo "$pos_out" | grep -q "Created" && check_pass BR_CREATE || check_fail BR_CREATE
echo "$pos_out" | grep -q "MEM_ALLOC" && check_pass help || check_fail help
[ $pos_rc -eq 0 ] && check_pass exit || check_fail exit

err_count=$(echo "$neg_out" | grep -c "Error")
[ "$err_count" -ge 1 ] && check_pass "MEM_ALLOC huge" || check_fail "MEM_ALLOC huge"
[ "$err_count" -ge 2 ] && check_pass "FS_OPEN bad" || check_fail "FS_OPEN bad"
echo "$neg_out" | grep -q "Unknown command" && check_pass "unknown command" || check_fail "unknown command"
[ $neg_rc -eq 0 ] && check_pass exit || check_fail exit

if [ $pass -eq 1 ]; then
  echo "bare-smoke: ALL TESTS PASS" | tee -a "$log"
else
  echo "bare-smoke: FAILS" | tee -a "$log"
  exit 1
fi
