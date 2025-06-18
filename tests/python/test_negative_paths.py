# AOS — ./tests/python/test_negative_paths.py
# (c) 2025 RedactedCoder23
# Brief: Source file.
import json
import os
import shutil
import subprocess
import sys

repo_root = os.path.abspath(os.path.join(os.path.dirname(__file__), "../.."))
sys.path.insert(0, repo_root)
from scripts import branch_ui  # noqa: E402

import pytest  # noqa: E402

IPC_TOOL = shutil.which("kernel-ipc") or os.path.join(
    repo_root, "scripts", "kernel_ipc.py"
)


def _run_tool(args):
    if not os.path.exists(IPC_TOOL):
        pytest.skip("ipc tool missing")
    cmd = [IPC_TOOL] + args
    if IPC_TOOL.endswith(".py"):
        cmd.insert(0, sys.executable)
    out = subprocess.check_output(cmd, text=True).strip()
    return json.loads(out)


def test_merge_invalid_syscall():
    data = _run_tool(["merge-branch", "9999"])
    assert data.get("code") == 404


def test_snapshot_invalid_syscall():
    data = _run_tool(["snapshot-branch", "9999"])
    assert data.get("code") == 404


def test_delete_invalid_syscall():
    data = _run_tool(["delete-branch", "9999"])
    assert data.get("code") == 404


def test_http_merge_invalid_id(monkeypatch):
    branch_ui.app.config["TESTING"] = True
    branch_ui.service.branches.clear()
    monkeypatch.setattr(branch_ui, "kernel_ipc", lambda *a, **k: -22)
    client = branch_ui.app.test_client()
    res = client.post("/branches/9999/merge")
    assert res.status_code == 404
    data = json.loads(res.data)
    assert data.get("error")

    res = client.delete("/branches/9999")
    assert res.status_code == 404
    data = json.loads(res.data)
    assert data.get("error")
