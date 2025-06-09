#!/bin/sh
set -euo pipefail
# usage: ./aos_updater.sh
# Simple self-updater fetching latest main branch

git pull --rebase && ./scripts/generate_changelog.sh
