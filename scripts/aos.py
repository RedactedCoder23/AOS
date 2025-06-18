#!/usr/bin/env python3
# AOS — scripts/aos.py — Purpose: Python module 
"""
AOS — scripts/aos.py
(c) 2025 RedactedCoder23
Brief: Unified AOS command line interface.
"""

from __future__ import annotations

import argparse


def main(argv: list[str] | None = None) -> int:
    parser = argparse.ArgumentParser(prog="aos")
    sub = parser.add_subparsers(dest="cmd", required=True)

    sub.add_parser("branch", help="branch operations")
    sub.add_parser("plugins", help="plugin management")
    sub.add_parser("secrets", help="manage secrets")
    sub.add_parser("audit", help="audit log management")

    try:
        import argcomplete  # type: ignore

        argcomplete.autocomplete(parser)
    except Exception:  # pragma: no cover - optional
        pass

    args, extra = parser.parse_known_args(argv)

    if args.cmd == "branch":
        from scripts import branch_cli

        return branch_cli.main(extra)
    if args.cmd == "plugins":
        from scripts import plugins_cli

        return plugins_cli.main(extra)
    if args.cmd == "secrets":
        from scripts.cli import secrets

        return secrets.main(extra)
    if args.cmd == "audit":
        from scripts import aos_audit

        return aos_audit.main()
    return 0


if __name__ == "__main__":  # pragma: no cover - CLI
    raise SystemExit(main())
