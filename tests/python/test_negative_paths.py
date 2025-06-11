# TODO: This IPC negative-path test requires kernel-ipc host tool. Skipped until available.
import json
import os
import shutil
import subprocess
import sys

repo_root = os.path.abspath(os.path.join(os.path.dirname(__file__), "../.."))
sys.path.insert(0, repo_root)
from scripts import branch_ui

import pytest

from scripts import branch_ui

IPC_TOOL = shutil.which("kernel-ipc") or os.path.join("build", "host_test")


def _run_tool(cmd, bid):
    if not os.path.exists(IPC_TOOL):
        pytest.skip("ipc tool missing")
    out = subprocess.check_output([IPC_TOOL, "--ipc", cmd, str(bid)], text=True)
    out = out.strip()
    try:
        return json.loads(out)
    except json.JSONDecodeError:
        return {"retval": int(out)}


def test_merge_invalid_syscall():
    data = _run_tool("merge", 9999)
    assert "error" in data


def test_snapshot_invalid_syscall():
    data = _run_tool("snapshot", 9999)
    assert "error" in data


def test_delete_invalid_syscall():
    data = _run_tool("delete", 9999)
    assert "error" in data


def test_http_merge_invalid_id(monkeypatch):
    branch_ui.app.config["TESTING"] = True
    branch_ui.service.branches.clear()
    monkeypatch.setattr(branch_ui, "kernel_ipc", lambda *a, **k: -22)
    client = branch_ui.app.test_client()
    res = client.post("/branches/9999/merge")
    data = json.loads(res.data)
    assert "error" in data
