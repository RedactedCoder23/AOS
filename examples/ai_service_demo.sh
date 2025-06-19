#!/usr/bin/env bash
set -euo pipefail
command -v qemu-system-x86_64 || { echo; exit 1; }
./build/ai_service_demo
