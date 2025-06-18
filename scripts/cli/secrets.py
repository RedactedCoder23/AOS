#!/usr/bin/env python3
# AOS — scripts/cli/secrets.py — Purpose: Python module 
# AOS — ./scripts/cli/secrets.py
# (c) 2025 RedactedCoder23
# Brief: Source file.
"""Minimal secrets helper using ai_cred_manager."""

import argparse
from scripts import ai_cred_manager as cm


def main() -> int:
    ap = argparse.ArgumentParser(description="Manage secrets")
    sub = ap.add_subparsers(dest="cmd", required=True)

    s = sub.add_parser("set")
    s.add_argument("key")
    s.add_argument("value")

    g = sub.add_parser("get")
    g.add_argument("key")

    args = ap.parse_args()

    if args.cmd == "set":
        cm.vault_set(args.key, args.value)
    elif args.cmd == "get":
        val = cm.vault_get(args.key)
        print(val)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
