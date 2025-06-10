#!/usr/bin/env python3
"""Audit log utilities."""
import argparse
import json
import os
from typing import Iterable

LOG_PATH = "/var/log/aos-audit.log"


def iter_entries(path: str) -> Iterable[dict]:
    if not os.path.exists(path):
        return []
    with open(path, "r") as fh:
        for line in fh:
            try:
                yield json.loads(line)
            except json.JSONDecodeError:
                continue


def show(args):
    for entry in iter_entries(args.file):
        if args.action and entry.get("action") != args.action:
            continue
        if args.resource and entry.get("resource") != args.resource:
            continue
        if args.user and entry.get("user") != args.user:
            continue
        print(json.dumps(entry, indent=2))


def main():
    p = argparse.ArgumentParser()
    sub = p.add_subparsers(dest="cmd", required=True)

    show_p = sub.add_parser("show")
    show_p.add_argument("--file", default=LOG_PATH)
    show_p.add_argument("--action")
    show_p.add_argument("--resource")
    show_p.add_argument("--user")
    show_p.set_defaults(func=show)

    args = p.parse_args()
    args.func(args)


if __name__ == "__main__":
    main()
