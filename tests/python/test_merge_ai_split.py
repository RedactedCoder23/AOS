# AOS — ./tests/python/test_merge_ai_split.py
# (c) 2025 RedactedCoder23
# Brief: Source file.
import io
import json
import os
import sys

sys.path.insert(0, os.path.abspath(os.path.join(os.path.dirname(__file__), "../..")))

from scripts import merge_ai  # noqa: E402


def test_hunk_splitting():
    small = (
        "diff --git a/a.txt b/a.txt\n"
        "--- a/a.txt\n"
        "+++ b/a.txt\n"
        "@@\n"
        "-old\n"
        "+new\n"
    )
    diff_text = small * 2000  # >10KB
    hunks = merge_ai.split_hunks(diff_text)
    assert len(hunks) >= 3


def test_conflict_fallback(monkeypatch):
    diff = (
        "diff --git a/a.txt b/a.txt\n"
        "--- a/a.txt\n"
        "+++ b/a.txt\n"
        "@@\n"
        "-a\n"
        "+b\n"
        "@@\n"
        "-c\n"
        "+d\n"
    )

    monkeypatch.setattr(merge_ai, "run_diff", lambda b, m, br: diff)
    responses = ["-a\n+b\n", ""]

    def fake_llm(_prompt):
        return responses.pop(0)

    def fake_valid(patch):
        return bool(patch)

    monkeypatch.setattr(merge_ai, "call_llm", fake_llm)
    monkeypatch.setattr(merge_ai, "valid_patch", fake_valid)

    argv = ["merge_ai.py", "--main", "main", "--branch", "br", "--base", "base"]
    monkeypatch.setattr(sys, "argv", argv)
    buf = io.StringIO()
    monkeypatch.setattr(sys, "stdout", buf)
    merge_ai.main()
    data = json.loads(buf.getvalue())
    assert "<<<<< CONFLICT" in data["patch"] and ">>>>>" in data["patch"]
