#!/bin/sh
set -euo pipefail
# usage: ./aos_install.sh install <files...>
cmd=$1
shift
case "$cmd" in
  install)
    [ $# -ge 1 ] || { echo "usage: aos install <file>"; exit 1; }
    mkdir -p packages
    for f in "$@"; do
        name=$(basename "$f")
        cp "$f" packages/ || exit 1
        chmod +x packages/"$name"
        echo "Installed $name"
    done
    ;;
  *)
    echo "usage: aos install <file>"
    exit 1
    ;;
esac
