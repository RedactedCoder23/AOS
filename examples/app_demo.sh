#!/bin/sh
# Demo for app runtime and installer
set -e
../aos install build/apps/fileman
../aos install build/apps/textedit
printf "app list\nexit\n" | ../build/host_test
