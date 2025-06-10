#!/usr/bin/env bash
set -e

# Basic dev packages
sudo apt-get update
sudo apt-get install -y build-essential clang clang-tidy clang-format qemu-system-x86 pkg-config libcurl4-openssl-dev libncurses-dev python3-pip

# Python dependencies
python3 -m pip install --upgrade pip
python3 -m pip install -r requirements.txt -r requirements-dev.txt compiledb

echo "Bootstrap complete. Run 'make compdb' to generate compile_commands.json."
