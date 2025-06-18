#!/usr/bin/env bash
# AOS — test_boot.sh
# (c) 2025 RedactedCoder23
# Brief: QEMU boot integration test
set -e

# Build ISO if missing
[ -f aos.iso ] || make iso

# Boot under QEMU and assert banner
output=$(timeout 10s qemu-system-x86_64 -cdrom aos.iso -nographic -serial mon:stdio -display none -no-reboot)
echo "$output"
if ! grep -q "AOS" <<< "$output"; then
  echo "🔥 Kernel did not print AOS banner"
  exit 1
fi
echo "✅ Boot integration test passed"
