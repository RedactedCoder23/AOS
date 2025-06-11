#!/usr/bin/env python3
"""Audit log utilities."""
import argparse
import json
import os
import time
from typing import Iterable, Dict

LOG_PATH = os.environ.get("AOS_AUDIT_LOG", "/var/log/aos-audit.log")


def log_entry(user: str, action: str, resource: str, result: str) -> None:
    """Append a single audit record."""
    path = os.environ.get("AOS_AUDIT_LOG", LOG_PATH)
    entry = {
        "timestamp": time.strftime("%Y-%m-%dT%H:%M:%SZ", time.gmtime()),
        "user": user,
        "action": action,
        "resource": resource,
        "result": result,
    }
    os.makedirs(os.path.dirname(path), exist_ok=True)
    with open(path, "a") as fh:
        json.dump(entry, fh)
        fh.write("\n")


def iter_entries(path: str) -> Iterable[Dict]:
    if not os.path.exists(path):
        return []
    with open(path, "r") as fh:
        for line in fh:
            try:
                yield json.loads(line)
            except json.JSONDecodeError:
                continue


def show(args):
    filters = {}
    if args.user:
        filters["user"] = args.user
    if args.action:
        filters["action"] = args.action
    if args.resource:
        filters["resource"] = args.resource
    for flt in args.filter:
        if ":" in flt:
            k, v = flt.split(":", 1)
            filters[k] = v

    for entry in iter_entries(args.file):
        matched = True
        for k, v in filters.items():
            if str(entry.get(k)) != v:
                matched = False
                break
        if matched:
            print(json.dumps(entry, indent=2))


def main():
    p = argparse.ArgumentParser()
    sub = p.add_subparsers(dest="cmd", required=True)

    show_p = sub.add_parser("show")
    show_p.add_argument("--file", default=LOG_PATH)
    show_p.add_argument("--user")
    show_p.add_argument("--action")
    show_p.add_argument("--resource")
    show_p.add_argument(
        "--filter",
        action="append",
        default=[],
        help="filter as key:value, e.g. action:branch_create",
    )
    show_p.set_defaults(func=show)

    args = p.parse_args()
    args.func(args)


if __name__ == "__main__":
    main()
