#!/bin/sh
set -e
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
PID=$!
sleep 3
kill $PID || true
