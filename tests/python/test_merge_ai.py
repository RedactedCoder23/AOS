import io
import json
import os
import subprocess
import sys
import tempfile
import unittest
import importlib
from unittest import mock

sys.path.insert(
    0, os.path.abspath(os.path.join(os.path.dirname(__file__), "../.."))
)  # noqa: E402

from scripts import merge_ai  # noqa: E402

SCRIPT = os.path.join("scripts", "merge_ai.py")


def init_repo():
    tmp = tempfile.TemporaryDirectory()
    repo = tmp.name
    subprocess.run(["git", "init"], cwd=repo, check=True, stdout=subprocess.PIPE)
    subprocess.run(["git", "config", "user.email", "a@b"], cwd=repo, check=True)
    subprocess.run(["git", "config", "user.name", "tester"], cwd=repo, check=True)
    return tmp, repo


def write(repo, path, text):
    with open(os.path.join(repo, path), "w") as f:
        f.write(text)


def commit(repo, msg):
    subprocess.run(["git", "add", "-A"], cwd=repo, check=True)
    subprocess.run(["git", "commit", "-m", msg], cwd=repo, check=True)


def git_diff(repo, base, main, branch):
    return subprocess.check_output(
        ["git", "diff", "--no-prefix", "--unified=0", base, main, branch],
        cwd=repo,
        text=True,
    )


class MergeAiTest(unittest.TestCase):
    def test_one_hunk_auto(self):
        tmp, repo = init_repo()
        write(repo, "f.txt", "a\nbase\n")
        commit(repo, "base")
        base = (
            subprocess.check_output(["git", "rev-parse", "HEAD"], cwd=repo)
            .decode()
            .strip()
        )
        subprocess.run(["git", "checkout", "-b", "main"], cwd=repo, check=True)
        write(repo, "f.txt", "a\nmain\n")
        commit(repo, "main")
        subprocess.run(["git", "checkout", "-b", "branch", base], cwd=repo, check=True)
        write(repo, "f.txt", "a\nbranch\n")
        commit(repo, "branch")

        mod = importlib.reload(merge_ai)
        with mock.patch.object(
            mod, "run_diff", side_effect=lambda b, m, br: git_diff(repo, b, m, br)
        ), mock.patch.object(
            mod,
            "call_llm",
            return_value="merged",
        ), mock.patch.object(
            mod,
            "valid_patch",
            return_value=True,
        ), mock.patch(
            "sys.argv",
            [
                "merge_ai.py",
                "--main",
                "main",
                "--branch",
                "branch",
                "--base",
                base,
            ],
        ):
            buf = io.StringIO()
            with mock.patch("sys.stdout", buf):
                mod.main()
            data = json.loads(buf.getvalue())
        self.assertEqual(data["patch"], "merged\n")
        self.assertEqual(data["conflicts"], [])
        tmp.cleanup()

    def test_large_segmentation(self):
        tmp, repo = init_repo()
        lines = "".join(f"line-{i:04}.........................\n" for i in range(400))
        write(repo, "f.txt", lines)
        commit(repo, "base")
        base = (
            subprocess.check_output(["git", "rev-parse", "HEAD"], cwd=repo)
            .decode()
            .strip()
        )
        subprocess.run(["git", "checkout", "-b", "main"], cwd=repo, check=True)
        write(repo, "f.txt", lines.replace("line-", "main-"))
        commit(repo, "main")
        subprocess.run(
            [
                "git",
                "checkout",
                "-b",
                "branch",
                base,
            ],
            cwd=repo,
            check=True,
        )
        write(repo, "f.txt", lines.replace("line-", "branch-"))
        commit(repo, "branch")

        calls = []

        def fake(prompt):
            calls.append(prompt)
            return "merge"

        mod = importlib.reload(merge_ai)
        with mock.patch.object(
            mod, "run_diff", side_effect=lambda b, m, br: git_diff(repo, b, m, br)
        ), mock.patch.object(
            mod,
            "call_llm",
            side_effect=fake,
        ), mock.patch.object(
            mod,
            "valid_patch",
            return_value=True,
        ), mock.patch(
            "sys.argv",
            [
                "merge_ai.py",
                "--main",
                "main",
                "--branch",
                "branch",
                "--base",
                base,
            ],
        ):
            buf = io.StringIO()
            with mock.patch("sys.stdout", buf):
                mod.main()
            data = json.loads(buf.getvalue())
        patch = git_diff(repo, base, "main", "branch")
        hunks = mod.split_hunks(patch)
        self.assertGreaterEqual(len(hunks), 3)
        self.assertGreater(len(calls), 1)
        self.assertEqual(len(data["conflicts"]), 0)
        tmp.cleanup()

    def test_llm_failure(self):
        tmp, repo = init_repo()
        lines = "".join(f"line-{i:04}.........................\n" for i in range(400))
        write(repo, "f.txt", lines)
        commit(repo, "base")
        base = (
            subprocess.check_output(["git", "rev-parse", "HEAD"], cwd=repo)
            .decode()
            .strip()
        )
        subprocess.run(["git", "checkout", "-b", "main"], cwd=repo, check=True)
        write(repo, "f.txt", lines.replace("line-", "main-"))
        commit(repo, "main")
        subprocess.run(["git", "checkout", "-b", "branch", base], cwd=repo, check=True)
        write(repo, "f.txt", lines.replace("line-", "branch-"))
        commit(repo, "branch")

        mod = importlib.reload(merge_ai)
        calls = []

        def fake_llm(_):
            idx = len(calls)
            calls.append(idx)
            return "" if idx == 1 else "merged"

        with mock.patch.object(
            mod, "run_diff", side_effect=lambda b, m, br: git_diff(repo, b, m, br)
        ), mock.patch.object(
            mod,
            "call_llm",
            side_effect=fake_llm,
        ), mock.patch.object(
            mod,
            "valid_patch",
            return_value=True,
        ), mock.patch(
            "sys.argv",
            [
                "merge_ai.py",
                "--main",
                "main",
                "--branch",
                "branch",
                "--base",
                base,
            ],
        ):
            buf = io.StringIO()
            with mock.patch("sys.stdout", buf):
                mod.main()
            data = json.loads(buf.getvalue())
        self.assertIn("<<<<< CONFLICT", data["patch"])
        self.assertEqual(data["conflicts"], [1])
        tmp.cleanup()


if __name__ == "__main__":
    unittest.main()

def test_hunk_splitting():
    small = (
        "diff --git a/a.txt b/a.txt\n"
        "--- a/a.txt\n"
        "+++ b/a.txt\n"
        "@@\n"
        "-old\n"
        "+new\n"
    )
    diff_text = small * 2000
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
    monkeypatch.setattr(merge_ai, "call_llm", lambda *_: "")
    monkeypatch.setattr(merge_ai, "valid_patch", lambda *_: False)

    argv = ["merge_ai.py", "--main", "main", "--branch", "br", "--base", "base"]
    monkeypatch.setattr(sys, "argv", argv)
    buf = io.StringIO()
    monkeypatch.setattr(sys, "stdout", buf)
    merge_ai.main()
    data = json.loads(buf.getvalue())
    assert "<<<<< CONFLICT" in data["patch"]
