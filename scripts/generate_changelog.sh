#!/bin/sh
set -euo pipefail
# usage: ./generate_changelog.sh
# Auto generate changelog from git history

git log --pretty=format:'- %s' > CHANGELOG.md
