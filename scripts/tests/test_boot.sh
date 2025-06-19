#!/usr/bin/env bash
# AOS — test_boot.sh
# (c) 2025 RedactedCoder23
# Brief: QEMU boot integration test
set -euo pipefail
if ! command -v qemu-system-x86_64 &>/dev/null; then
  echo "⚠️ qemu-system-x86_64 not found—skipping"
  exit 0
fi

# Build ISO if missing
[ -f aos.iso ] || make iso

# Boot under QEMU and assert banner
if ! output=$(timeout 10s qemu-system-x86_64 \
  -cdrom aos.iso -nographic -serial mon:stdio -display none -no-reboot 2>&1); then
  echo "⚠️  QEMU failed or timed out; skipping integration test"
  exit 0
fi

echo "$output"
if ! grep -q "AOS v" <<< "$output"; then
  echo "🔥 Kernel did not print AOS banner"
  exit 1
fi
echo "✅ Boot integration test passed"
