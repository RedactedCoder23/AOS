#!/bin/sh
# Auto generate changelog from git history

git log --pretty=format:'- %s' > CHANGELOG.md
