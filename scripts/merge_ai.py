#!/usr/bin/env python3
"""LLM-assisted three-way merge helper."""

from __future__ import annotations

import argparse
import json
import os
import subprocess
import sys
from typing import List

from ai_cred_client import get_api_key

try:
    import openai
except Exception:  # pragma: no cover - optional dependency
    openai = None

SIZE_LIMIT = 4096


def run_diff(base: str, ref: str) -> str:
    """Return unified diff text for ``base``..``ref`` without prefixes."""
    return subprocess.check_output(
        ["git", "diff", "--no-prefix", "--unified=0", f"{base}..{ref}"],
        text=True,
    )


def split_hunks(patch: str) -> List[str]:
    """Split *patch* into hunks no larger than :data:`SIZE_LIMIT`."""
    hunks: List[str] = []
    current = ""
    for line in patch.splitlines(keepends=True):
        if line.startswith("@@"):
            if "@@" in current:
                hunks.append(current)
                current = ""
            current += line
            continue
        if len(current) + len(line) > SIZE_LIMIT and current:
            hunks.append(current)
            current = line
            continue
        current += line
    if current:
        hunks.append(current)
    return hunks


def call_llm(prompt: str) -> str:
    """Send *prompt* to OpenAI and return the response."""
    if os.environ.get("AOS_AI_OFFLINE"):
        return ""
    try:
        key = get_api_key("openai")
    except KeyError as exc:  # pragma: no cover - credentials missing
        raise RuntimeError("missing API credential") from exc
    if openai is None:  # pragma: no cover - optional dependency
        raise RuntimeError("openai package missing")
    client = openai.OpenAI(api_key=key)
    resp = client.chat.completions.create(
        model="gpt-3.5-turbo",
        messages=[{"role": "user", "content": prompt}],
    )
    return resp.choices[0].message.content.strip()


def main(argv: List[str] | None = None) -> int:
    parser = argparse.ArgumentParser(description="merge using an LLM")
    parser.add_argument("--main", required=True)
    parser.add_argument("--branch", required=True)
    parser.add_argument("--base", required=True)
    args = parser.parse_args(argv)

    main_patch = run_diff(args.base, args.main)
    branch_patch = run_diff(args.base, args.branch)

    main_hunks = split_hunks(main_patch)
    branch_hunks = split_hunks(branch_patch)

    merged: List[str] = []
    conflicts: List[int] = []
    for idx in range(max(len(main_hunks), len(branch_hunks))):
        mh = main_hunks[idx] if idx < len(main_hunks) else ""
        bh = branch_hunks[idx] if idx < len(branch_hunks) else ""
        prompt = (
            "Resolve this conflict hunk between MAIN, BRANCH, and BASE:\n"
            "<<<<<<< MAIN\n"
            f"{mh}"
            "=======\n"
            f"{bh}"
            ">>>>>>> BRANCH\n"
            "Return only the merged patch fragment without commentary."
        )
        try:
            frag = call_llm(prompt)
        except Exception:  # pragma: no cover - API failures
            frag = ""
        if frag:
            merged.append(frag.rstrip("\n") + "\n")
        else:
            merged.append(
                "<<<<<<< MAIN\n" + mh + "=======\n" + bh + ">>>>>>> BRANCH\n"
            )
            conflicts.append(idx)

    patch = "".join(merged)
    print(json.dumps({"success": True, "patch": patch, "conflicts": conflicts}))
    return 0


if __name__ == "__main__":
    sys.exit(main())
