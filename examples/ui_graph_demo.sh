#!/bin/sh
set -e
make ui
mkdir -p build
gcc -Iinclude examples/setup_branches.c src/branch_manager.c -o build/setup_branches
./build/setup_branches
./build/ui_graph
