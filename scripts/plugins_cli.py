#!/usr/bin/env python3
# AOS — scripts/plugins_cli.py — Purpose: Python module 
# AOS — ./scripts/plugins_cli.py
# (c) 2025 RedactedCoder23
# Brief: Source file.
"""Plugin registry management CLI."""

from __future__ import annotations

import argparse
import json
import os
import shutil
import subprocess
import tempfile
from pathlib import Path

import yaml

ROOT = Path(__file__).resolve().parent.parent
DEFAULT_STORE = ROOT / "plugins"
DEFAULT_REGISTRY = ROOT / "providers.json"


def _load_manifest(path: Path) -> dict:
    with open(path, "r", encoding="utf-8") as fh:
        data = yaml.safe_load(fh)
    for key in ("name", "version", "entry_class", "permissions"):
        if key not in data:
            raise ValueError(f"missing {key} in provider.yaml")
    return data


def _clone(src: str, dest: Path) -> None:
    if os.path.isdir(src):
        shutil.copytree(src, dest)
    else:
        subprocess.check_call(["git", "clone", src, str(dest)])


def _build_image(path: Path, tag: str) -> None:
    subprocess.check_call(
        ["docker", "build", "-t", tag, "-f", "Dockerfile.sandbox", str(path)]
    )


def _register(manifest: dict, registry: Path, image: str) -> None:
    if registry.exists():
        with open(registry, "r", encoding="utf-8") as fh:
            data = json.load(fh)
    else:
        data = {}
    data[manifest["name"]] = {
        "module": "scripts.ai_providers.plugin_runner",
        "class": "PluginRunner",
        "image": image,
        "entry": manifest["entry_class"],
    }
    with open(registry, "w", encoding="utf-8") as fh:
        json.dump(data, fh, indent=2)


def cmd_add(args: argparse.Namespace) -> int:
    tmpdir = Path(tempfile.mkdtemp())
    shutil.rmtree(tmpdir)
    _clone(args.repo, tmpdir)
    manifest = _load_manifest(tmpdir / "provider.yaml")
    dest = Path(args.store) / manifest["name"]
    if dest.exists():
        shutil.rmtree(dest)
    shutil.copytree(tmpdir, dest)
    image = f"{manifest['name']}:{manifest['version']}"
    _build_image(dest, image)
    _register(manifest, Path(args.registry), image)
    print(f"added {manifest['name']}")
    return 0


def cmd_list(args: argparse.Namespace) -> int:
    reg = Path(args.registry)
    if not reg.exists():
        return 0
    with open(reg, "r", encoding="utf-8") as fh:
        data = json.load(fh)
    for name in data:
        print(name)
    return 0


def main(argv: list[str] | None = None) -> int:
    p = argparse.ArgumentParser(prog="aos plugins")
    sub = p.add_subparsers(dest="cmd")

    add = sub.add_parser("add", help="add plugin from git")
    add.add_argument("repo")
    add.add_argument("--registry", default=str(DEFAULT_REGISTRY))
    add.add_argument("--store", default=str(DEFAULT_STORE))
    add.set_defaults(func=cmd_add)

    lsp = sub.add_parser("list", help="list registered plugins")
    lsp.add_argument("--registry", default=str(DEFAULT_REGISTRY))
    lsp.set_defaults(func=cmd_list)

    args = p.parse_args(argv)
    if not hasattr(args, "func"):
        p.print_help()
        return 1
    return args.func(args)


if __name__ == "__main__":  # pragma: no cover - manual invocation
    import sys

    sys.exit(main())
