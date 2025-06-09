#!/bin/sh
# Demo for app runtime and installer
set -e
../scripts/aos_install.sh install build/apps/fileman
../scripts/aos_install.sh install build/apps/textedit
printf "app list\nexit\n" | ../build/host_test
