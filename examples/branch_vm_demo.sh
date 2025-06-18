#!/usr/bin/env bash
set -euo pipefail
set -euo pipefail

usage() {
  echo "Usage: $0 [--help] [branch]" >&2
  echo "Run branch_vm_demo with optional branch name." >&2
}

branch="demo"
if [ "$#" -gt 0 ]; then
  case "$1" in
    -h|--help)
      usage
      exit 0
      ;;
    *)
      branch="$1"
      ;;
  esac
fi

if ! command -v qemu-system-x86_64 >/dev/null 2>&1; then
  echo "qemu-system-x86_64 not found. Install QEMU to run this demo." >&2
  exit 1
fi

./build/branch_vm_demo "$branch"
