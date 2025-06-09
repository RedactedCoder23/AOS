#!/bin/sh
set -e
cmds="MEM_ALLOC 32\nFS_OPEN foo w\nFS_WRITE 0 hi\nFS_READ 0 2\nFS_LS\nBR_CREATE test\nUNKNOWN\nhelp\nexit\n"
out=$(printf "$cmds" | timeout 5 qemu-system-x86_64 -nographic -drive format=raw,file=aos.bin 2>/dev/null)
echo "$out"
# verify outputs
printf "%s" "$out" | grep -q "Allocated"
printf "%s" "$out" | grep -q "fd="
printf "%s" "$out" | grep -q "Created"
printf "%s" "$out" | grep -q "Unknown command"
printf "%s" "$out" | grep -q "MEM_ALLOC"
