#!/usr/bin/env bash
set -e

# Basic dev packages
sudo apt-get update || echo "⚠️ offline"
sudo apt-get install -y \
  build-essential clang tidy nasm gcc gcc-multilib libc6-dev-i386 \
  binutils grub2-common grub-pc-bin xorriso qemu-system-x86_64 \
  python3-pip nodejs npm \
  || echo "⚠️ some packages failed—continuing"

# Python dependencies
python3 -m pip install --upgrade pip
python3 -m pip install -r requirements.txt -r requirements-dev.txt compiledb

# Install UI dependencies
if [ -f ui/package-lock.json ]; then
  (cd ui && npm ci)
fi

echo "Bootstrap complete. Run 'make compdb' to generate compile_commands.json."
