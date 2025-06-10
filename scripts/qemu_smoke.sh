#!/bin/sh
set -euo pipefail
# usage: ./qemu_smoke.sh <arch>
ARCH=$1
case "$ARCH" in
  x86_64) EMU=qemu-system-x86_64; OPT="-drive format=raw,file=aos.bin" ;;
  riscv64) EMU=qemu-system-riscv64; OPT="-machine virt -nographic -kernel aos.bin" ;;
  arm) EMU=qemu-system-arm; OPT="-machine virt -nographic -kernel aos.bin" ;;
  *) echo "unknown arch"; exit 1;;
esac
if ! command -v "$EMU" >/dev/null; then
  echo "$EMU not installed"; exit 1
fi
$EMU -serial stdio $OPT -no-reboot -d guest_errors -snapshot &
QPID=$!
./build/ipc_host &
HPID=$!
sleep 3
kill $QPID || true
kill $HPID || true
