import json
import os
import sys

sys.path.insert(0, os.path.abspath(os.path.join(os.path.dirname(__file__), "../..")))

from scripts import branch_ui


def _client():
    branch_ui.app.config["TESTING"] = True
    branch_ui.service.branches.clear()
    return branch_ui.app.test_client()


def test_snapshot_endpoint(monkeypatch):
    client = _client()

    def fake_ipc(cmd, payload=None):
        if cmd == "create":
            return 1
        if cmd == "snapshot":
            bid = payload.get("branch_id", 0)
            return 42 if bid == 1 else -22
        return 0

    monkeypatch.setattr(branch_ui, "kernel_ipc", fake_ipc)
    monkeypatch.setattr(branch_ui.subprocess, "Popen", lambda *a, **k: type("P", (), {"pid": 1})())

    res = client.post("/branches")
    bid = json.loads(res.data)["branch_id"]
    res = client.post(f"/branches/{bid}/snapshot")
    assert res.status_code == 200
    data = json.loads(res.data)
    assert data["snapshot_id"] > 0
    listing = json.loads(client.get("/branches").data)
    assert any(e["last_snapshot_id"] > 0 for e in listing)

    res = client.post("/branches/9999/snapshot")
    assert res.status_code >= 400


def test_delete_endpoint(monkeypatch):
    client = _client()

    def fake_ipc(cmd, payload=None):
        if cmd == "create":
            return 1
        if cmd == "delete":
            bid = payload.get("branch_id", 0)
            return 0 if bid == 1 else -22
        return 0

    monkeypatch.setattr(branch_ui, "kernel_ipc", fake_ipc)
    monkeypatch.setattr(branch_ui.subprocess, "Popen", lambda *a, **k: type("P", (), {"pid": 2})())

    res = client.post("/branches")
    bid = json.loads(res.data)["branch_id"]
    res = client.delete(f"/branches/{bid}")
    assert res.status_code == 200
    data = json.loads(res.data)
    assert data.get("result") == "deleted" or data.get("success") is True
    listing = json.loads(client.get("/branches").data)
    ids = [e["branch_id"] for e in listing]
    assert bid not in ids

    res = client.delete("/branches/9999")
    assert res.status_code >= 400
