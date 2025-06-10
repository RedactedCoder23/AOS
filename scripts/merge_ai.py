#!/usr/bin/env python3
"""Mock merge assistant returning a patch summary."""
import json
import sys


def main():
    bid = sys.argv[1] if len(sys.argv) > 1 else "0"
    patch = f"apply patchset for branch {bid}"
    print(json.dumps({"patch": patch, "status": "ok"}))


if __name__ == "__main__":
    main()
