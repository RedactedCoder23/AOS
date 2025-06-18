#!/usr/bin/env python3
# AOS — ./scripts/audit_cli.py
# (c) 2025 RedactedCoder23
# Brief: Source file.
"""Simple audit and rollback CLI"""
import argparse
import subprocess
import os
import sys


def show_log():
    if os.path.exists("AOS-CHECKLIST.log"):
        with open("AOS-CHECKLIST.log") as f:
            print(f.read())
    else:
        print("log empty")


def clear_log():
    open("AOS-CHECKLIST.log", "w").close()
    print("log cleared")


def show_commits():
    subprocess.run(["git", "log", "--oneline", "-n", "5"])


def rollback(commit):
    try:
        subprocess.check_call(["git", "reset", "--hard", commit])
        print("rollback complete")
    except subprocess.CalledProcessError as e:
        print(f"rollback failed: {e}", file=sys.stderr)
        return 1
    return 0


def main():
    ap = argparse.ArgumentParser()
    sub = ap.add_subparsers(dest="cmd")
    sub.required = True
    sub.add_parser("log")
    sub.add_parser("clear")
    sub.add_parser("commits")
    rb = sub.add_parser("rollback")
    rb.add_argument("commit")
    args = ap.parse_args()

    if args.cmd == "log":
        show_log()
    elif args.cmd == "clear":
        clear_log()
    elif args.cmd == "commits":
        show_commits()
    elif args.cmd == "rollback":
        return rollback(args.commit)
    return 0


if __name__ == "__main__":
    sys.exit(main())
