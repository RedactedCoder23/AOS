#!/usr/bin/env bash
set -euo pipefail
set -e
../scripts/aos_install.sh install build/apps/fileman
../scripts/aos_install.sh install build/apps/textedit
printf "app list\nexit\n" | ../build/host_test
