#!/usr/bin/env python3
# AOS — ./scripts/branch_cli.py
# (c) 2025 RedactedCoder23
# Brief: Source file.
"""Simple command-line wrapper for branch operations."""
import os
import sys
import argparse


def main() -> int:
    parser = argparse.ArgumentParser(description="AOS branch CLI")
    parser.add_argument("action", nargs="?", help="operation to perform")
    parser.add_argument("--id", dest="branch_id", default=os.environ.get("BRANCH"))
    args, extra = parser.parse_known_args()

    print(f"branch_cli action={args.action} branch_id={args.branch_id} extra={extra}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
