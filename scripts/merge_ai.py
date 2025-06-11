#!/usr/bin/env python3
"""LLM-assisted three-way merge helper."""

from __future__ import annotations

import argparse
import json
import os
import subprocess
import sys
import time
import logging
from typing import List
import importlib

from scripts.ai_providers.base import AIProvider

PROVIDERS: dict[str, AIProvider] = {}


def _load_providers() -> None:
    global PROVIDERS
    if PROVIDERS:
        return
    cfg = os.path.join(os.path.dirname(os.path.dirname(__file__)), "providers.json")
    try:
        with open(cfg, "r", encoding="utf-8") as fh:
            data = json.load(fh)
    except Exception:
        data = {}
    for name, info in data.items():
        try:
            mod = importlib.import_module(f"scripts.ai_providers.{info['module']}")
            cls = getattr(mod, info['class'])
            PROVIDERS[name] = cls(name)
        except Exception:
            continue

MAX_HUNK_SIZE = 4096


def run_diff(base: str, main: str, branch: str) -> str:
    """Return combined diff text for ``MAIN`` and ``BRANCH`` relative to ``BASE``."""
    return subprocess.check_output(
        ["git", "diff", "--no-prefix", "--unified=0", base, main, branch],
        text=True,
    )


def split_hunks(patch: str) -> List[str]:
    """Split *patch* into hunks respecting :data:`MAX_HUNK_SIZE`."""
    hunks: List[str] = []
    current = ""
    for line in patch.splitlines(keepends=True):
        if line.startswith("diff --git"):
            if current:
                hunks.append(current)
                current = ""
            current += line
            continue
        if line.startswith("@@") and current and "@@" in current:
            hunks.append(current)
            current = line
            continue
        if len(current) + len(line) > MAX_HUNK_SIZE and current:
            hunks.append(current)
            current = line
            continue
        current += line
    if current:
        hunks.append(current)
    return hunks


def call_llm(prompt: str, provider_name: str | None = None) -> str:
    """Send *prompt* to an AI provider and return the response."""
    if os.environ.get("AOS_AI_OFFLINE"):
        return ""
    _load_providers()
    provider_name = provider_name or os.environ.get("AOS_AI_PROVIDER", "openai")
    prov = PROVIDERS.get(provider_name)
    if prov is None:
        raise RuntimeError(f"provider {provider_name} not available")
    return prov.generate(prompt)


def valid_patch(patch: str) -> bool:
    """Return ``True`` if *patch* applies cleanly."""
    proc = subprocess.run(
        ["git", "apply", "--check", "-"], input=patch, text=True, capture_output=True
    )
    return proc.returncode == 0


def main(argv: List[str] | None = None) -> int:
    parser = argparse.ArgumentParser(description="merge using an LLM")
    parser.add_argument("--main", required=True)
    parser.add_argument("--branch", required=True)
    parser.add_argument("--base", required=True)
    args = parser.parse_args(argv)

    logging.basicConfig(level=logging.INFO)

    patch_text = run_diff(args.base, args.main, args.branch)
    hunks = split_hunks(patch_text)

    merged: List[str] = []
    conflicts: List[int] = []
    for idx, hunk in enumerate(hunks):
        logging.info("hunk %d size %d", idx, len(hunk))
        prompt = (
            "Resolve this diff hunk between MAIN, BASE, and BRANCH. Return a valid patch snippet.\n"
            f"{hunk}"
        )
        start = time.perf_counter()
        try:
            frag = call_llm(prompt)
        except Exception:  # pragma: no cover - API failures
            frag = ""
        elapsed = time.perf_counter() - start
        logging.info("hunk %d took %.2fs", idx, elapsed)
        if frag and valid_patch(frag):
            merged.append(frag.rstrip("\n") + "\n")
        else:
            merged.append("<<<<< CONFLICT\n" + hunk + ">>>>>\n")
            conflicts.append(idx)

    patch = "".join(merged)
    print(json.dumps({"success": True, "patch": patch, "conflicts": conflicts}))
    return 0


if __name__ == "__main__":
    sys.exit(main())
