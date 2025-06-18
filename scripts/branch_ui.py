#!/usr/bin/env python3
/* AOS — scripts/branch_ui.py — Purpose: Python module */
# AOS — ./scripts/branch_ui.py
# (c) 2025 RedactedCoder23
# Brief: Source file.
"""Branch lifecycle HTTP service with micro-VM support."""
import json
import os
import queue
import subprocess
import sys
from datetime import datetime, timedelta
import uuid
from flask import Flask, Response, jsonify, request, send_from_directory
import jwt
from . import agent_orchestrator
from .aos_audit import log_entry

try:
    import yaml  # type: ignore
except Exception:  # pragma: no cover - optional
    yaml = None

PORT = 8000
BASE = os.path.dirname(os.path.abspath(__file__))
WEB_DIR = os.path.join(BASE, "..", "ui")
GRAPH_FILE = os.path.join(BASE, "..", "examples", "graph_sample.json")
METRICS_FILE = os.path.join(BASE, "..", "examples", "metrics_sample.json")
FC_BIN = os.environ.get("FIRECRACKER", "firecracker")
JWT_SECRET = os.environ.get("AOS_JWT_SECRET", "dev-secret")


def issue_tokens(user: str, role: str) -> dict:
    """Generate access and refresh JWTs."""
    now = datetime.utcnow()
    access = jwt.encode(
        {
            "user": user,
            "role": role,
            "exp": now + timedelta(minutes=15),
            "jti": str(uuid.uuid4()),
        },
        JWT_SECRET,
        algorithm="HS256",
    )
    refresh = jwt.encode(
        {
            "user": user,
            "role": role,
            "exp": now + timedelta(days=7),
            "type": "refresh",
            "jti": str(uuid.uuid4()),
        },
        JWT_SECRET,
        algorithm="HS256",
    )
    return {"access": access, "refresh": refresh, "role": role}


class _SSE:
    """Minimal in-memory server-sent events helper."""

    def __init__(self):
        self._clients = []

    def publish(self, data, event=None):
        for q in list(self._clients):
            q.put({"event": event, "data": data})

    def stream(self):
        def gen():
            q = queue.Queue()
            self._clients.append(q)
            try:
                while True:
                    msg = q.get()
                    if msg["event"]:
                        yield f"event: {msg['event']}\n"
                    yield f"data: {json.dumps(msg['data'])}\n\n"
            finally:
                self._clients.remove(q)

        return Response(gen(), mimetype="text/event-stream")


def kernel_ipc(cmd, payload=None):
    """Call host binary if available, otherwise return stub values."""
    host = os.path.join("./build", "host_test")
    if os.path.exists(host) and not os.environ.get("AOS_IPC_LOCAL"):
        args = [host, "--ipc", cmd]
        if payload and "branch_id" in payload:
            args.append(str(payload["branch_id"]))
        out = subprocess.check_output(args, text=True).strip()
        try:
            return int(out)
        except ValueError:
            return out
    if cmd == "create":
        return 1 + len(service.branches)
    if cmd == "list":
        return service.list()
    if cmd == "merge":
        bid = payload.get("branch_id", 0) if payload else 0
        return 0 if bid in service.branches else -22
    if cmd == "snapshot":
        bid = payload.get("branch_id", 0) if payload else 0
        return len(service.branches) + 10 if bid in service.branches else -22
    if cmd == "delete":
        bid = payload.get("branch_id", 0) if payload else 0
        return 0 if bid in service.branches else -22
    return 0


class BranchService:
    def __init__(self, logger):
        self.branches = {}
        self.logger = logger

    def create(self, uid: int | None = None):
        uid = uid if uid is not None else os.getuid()
        branch_id = kernel_ipc("create")
        vm_path = f"/var/lib/branches/branch-{branch_id}.img"
        proc = subprocess.Popen(
            [
                FC_BIN,
                "--api-sock",
                f"/tmp/fc-{branch_id}.sock",
                "--config-file",
                "/etc/aos/firecracker.json",
                "--root-drive",
                vm_path,
                "--cow",
            ]
        )
        self.logger.info("Launched branch %d, PID %d", branch_id, proc.pid)
        self.branches[branch_id] = {
            "pid": proc.pid,
            "sock": f"/tmp/fc-{branch_id}.sock",
            "status": "RUNNING",
            "last_snapshot_id": 0,
            "owner_uid": uid,
        }
        log_entry(str(uid), "branch_create", f"branch:{branch_id}", "success")
        flask_sse.publish({"branch_id": branch_id}, event="branch-created")
        self._capture_thumbnail(branch_id)
        return branch_id

    def _capture_thumbnail(self, branch_id: int) -> None:
        path = os.path.join("branches", str(branch_id))
        os.makedirs(path, exist_ok=True)
        out = os.path.join(path, "thumbnail.png")
        try:
            subprocess.run(
                [
                    "virsh",
                    "screenshot",
                    f"branch-{branch_id}",
                    out,
                ],
                check=True,
            )
        except Exception:
            try:
                banner = subprocess.check_output(
                    ["bash", "-c", "echo AOS branch"], text=True
                )
                with open(out, "wb") as fh:
                    fh.write(banner.encode())
            except Exception:
                with open(out, "wb") as fh:
                    fh.write(b"")

    def list(self):
        return [
            {
                "branch_id": bid,
                "status": info["status"],
                "last_snapshot_id": info.get("last_snapshot_id", 0),
            }
            for bid, info in self.branches.items()
        ]

    def merge(self, bid, uid: int | None = None):
        uid = uid if uid is not None else os.getuid()
        info = self.branches.get(bid)
        if info and info.get("owner_uid") != uid:
            log_entry(str(uid), "branch_merge", f"branch:{bid}", "denied")
            return {"error": "permission denied"}
        rc = kernel_ipc("merge", {"branch_id": bid})
        if isinstance(rc, int) and rc < 0:
            log_entry(str(uid), "branch_merge", f"branch:{bid}", "denied")
            return {"error": "invalid branch"}
        try:
            out = subprocess.check_output(
                [
                    sys.executable,
                    os.path.join(BASE, "merge_ai.py"),
                    str(bid),
                ],
                text=True,
            ).strip()
            flask_sse.publish({"branch_id": bid}, event="branch-merged")
            log_entry(str(uid), "branch_merge", f"branch:{bid}", "success")
            return {"merged": rc == 0, "detail": out}
        except subprocess.CalledProcessError as e:
            return {"error": e.output.strip()}

    def snapshot(self, bid, uid: int | None = None):
        uid = uid if uid is not None else os.getuid()
        info = self.branches.get(bid)
        if info and info.get("owner_uid") != uid:
            log_entry(str(uid), "branch_snapshot", f"branch:{bid}", "denied")
            return {"error": "permission denied"}
        sid = kernel_ipc("snapshot", {"branch_id": bid})
        if isinstance(sid, int) and sid < 0:
            log_entry(str(uid), "branch_snapshot", f"branch:{bid}", "denied")
            return {"error": "invalid branch"}
        if bid in self.branches:
            self.branches[bid]["last_snapshot_id"] = sid
        flask_sse.publish(
            {"branch_id": bid, "snapshot_id": sid}, event="branch-snapshot"
        )
        log_entry(str(uid), "branch_snapshot", f"branch:{bid}", "success")
        return {"snapshot_id": sid}

    def delete(self, bid, uid: int | None = None):
        uid = uid if uid is not None else os.getuid()
        info = self.branches.get(bid)
        if info and info.get("owner_uid") != uid:
            log_entry(str(uid), "branch_delete", f"branch:{bid}", "denied")
            return {"error": "permission denied"}
        rc = kernel_ipc("delete", {"branch_id": bid})
        if isinstance(rc, int) and rc < 0:
            log_entry(str(uid), "branch_delete", f"branch:{bid}", "denied")
            return {"error": "invalid branch"}
        self.branches.pop(bid, None)
        flask_sse.publish({"branch_id": bid}, event="branch-deleted")
        log_entry(str(uid), "branch_delete", f"branch:{bid}", "success")
        return {"success": True}


flask_sse = _SSE()
app = Flask(__name__, static_folder=WEB_DIR, static_url_path="")
service = BranchService(app.logger)


def _relay(event: dict) -> None:
    """Forward orchestrator events to clients via SSE."""
    flask_sse.publish(event, event=event.get("type"))


agent_orchestrator.events.register(_relay)


@app.route("/auth/login", methods=["POST"])
def login():
    data = request.get_json(force=True) or {}
    user = data.get("username")
    pwd = data.get("password")
    if (user, pwd) in (("test", "test"), ("viewer", "viewer")):
        role = "admin" if user == "test" else "viewer"
        return jsonify(issue_tokens(user, role))
    return jsonify({"error": "invalid"}), 401


@app.route("/auth/refresh", methods=["POST"])
def refresh():
    data = request.get_json(force=True) or {}
    token = data.get("token")
    if not token:
        return jsonify({"error": "invalid"}), 401
    try:
        payload = jwt.decode(token, JWT_SECRET, algorithms=["HS256"])
        if payload.get("type") != "refresh":
            raise jwt.InvalidTokenError
    except Exception:
        return jsonify({"error": "invalid"}), 401
    return jsonify(issue_tokens(payload["user"], payload.get("role", "viewer")))


@app.route("/plugins/install", methods=["POST"])
def plugins_install():
    auth = request.headers.get("Authorization", "")
    token = auth.replace("Bearer ", "")
    try:
        payload = jwt.decode(token, JWT_SECRET, algorithms=["HS256"])
    except Exception:
        return jsonify({"error": "unauthorized"}), 401
    if payload.get("role") != "admin":
        return jsonify({"error": "forbidden"}), 403
    repo = (request.get_json(force=True) or {}).get("repo")
    if not repo:
        return jsonify({"error": "missing repo"}), 400
    return jsonify({"installed": repo})


@app.route("/graph")
def graph():
    with open(GRAPH_FILE, "r") as f:
        return Response(f.read(), mimetype="application/json")


@app.route("/metrics")
def metrics():
    with open(METRICS_FILE, "r") as f:
        return Response(f.read(), mimetype="application/json")


@app.route("/export")
def export_graph():
    path = GRAPH_FILE
    if os.path.exists(GRAPH_FILE):
        path = GRAPH_FILE
    with open(path, "r") as f:
        return Response(f.read(), mimetype="application/json")


@app.route("/import", methods=["POST"])
def import_graph():
    data = request.get_data()
    with open(GRAPH_FILE, "wb") as f:
        f.write(data)
    return "OK"


@app.route("/branches", methods=["POST"])
def create_branch():
    uid = int(request.remote_user or os.getuid())
    bid = service.create(uid)
    return jsonify({"branch_id": bid})


@app.route("/branches", methods=["GET"])
def list_branches():
    return jsonify(service.list())


@app.route("/api/branches")
def api_branches():
    limit = int(request.args.get("limit", "10"))
    cursor = request.args.get("cursor")
    items = service.list()
    start = 0
    if cursor is not None:
        try:
            ids = [b["branch_id"] for b in items]
            start = ids.index(int(cursor)) + 1
        except ValueError:
            start = 0
    slice_ = items[start : start + limit]
    next_cursor = slice_[-1]["branch_id"] if len(slice_) == limit else None
    return jsonify({"items": slice_, "next": next_cursor})


@app.route("/branches/<int:branch_id>/coverage-history")
def coverage_history(branch_id):
    path = os.path.join("branches", str(branch_id), "history.json")
    if not os.path.exists(path):
        return jsonify([])
    with open(path, "r", encoding="utf-8") as fh:
        try:
            data = json.load(fh)
        except Exception:
            data = []
    return jsonify(data)


@app.route("/branches/<int:bid>/merge", methods=["POST"])
def merge_branch(bid):
    info = service.branches.get(bid)
    if info and str(request.remote_user) != str(info.get("owner_uid")):
        return jsonify({"error": "permission denied"}), 403
    cov_file = os.path.join("branches", str(bid), "coverage.json")
    cov = 100.0
    if os.path.exists(cov_file):
        try:
            with open(cov_file, "r", encoding="utf-8") as fh:
                cov = float(json.load(fh).get("totals", {}).get("percent_covered", 0))
        except Exception:
            cov = 0.0
    threshold = 0
    if os.path.exists("rules.yaml") and yaml is not None:
        with open("rules.yaml", "r", encoding="utf-8") as fh:
            try:
                rules = yaml.safe_load(fh)
                threshold = int(rules.get("coverage_threshold", 0))
            except Exception:
                threshold = 0
    if threshold and cov < threshold:
        return jsonify({"error": f"coverage below {threshold}%", "code": 422}), 422
    uid = int(request.remote_user or os.getuid())
    res = service.merge(bid, uid)
    if "error" in res:
        return jsonify(res), 404
    return jsonify(res)


@app.route("/branches/<int:bid>/snapshot", methods=["POST"])
def snapshot_branch(bid):
    info = service.branches.get(bid)
    if info and str(request.remote_user) != str(info.get("owner_uid")):
        return jsonify({"error": "permission denied"}), 403
    uid = int(request.remote_user or os.getuid())
    res = service.snapshot(bid, uid)
    if "error" in res:
        return jsonify(res), 404
    return jsonify(res)


@app.route("/branches/<int:bid>/thumbnail.png")
def branch_thumbnail(bid):
    path = os.path.join("branches", str(bid), "thumbnail.png")
    if not os.path.exists(path):
        return jsonify({"error": "not found"}), 404
    return send_from_directory(os.path.dirname(path), "thumbnail.png")


@app.route("/branches/<int:bid>", methods=["DELETE"])
def delete_branch(bid):
    info = service.branches.get(bid)
    if info and str(request.remote_user) != str(info.get("owner_uid")):
        return jsonify({"error": "permission denied"}), 403
    uid = int(request.remote_user or os.getuid())
    res = service.delete(bid, uid)
    if "error" in res:
        return jsonify(res), 404
    return jsonify(res)


@app.route("/events")
def events():
    return flask_sse.stream()


@app.route("/branches/<int:branch_id>/metrics")
def branch_metrics(branch_id):
    data = agent_orchestrator.METRICS.get(
        branch_id,
        {
            "agents_spawned": 0,
            "success_rate": 0.0,
            "avg_runtime": 0.0,
        },
    )
    return jsonify(data)


@app.route("/branches/<int:branch_id>/agents/stream")
def stream_agents(branch_id: int):
    def gen():
        for res in agent_orchestrator.run_tasks(branch_id):
            yield f"event: agent-status\ndata: {json.dumps(res)}\n\n"

    return Response(gen(), mimetype="text/event-stream")


@app.route("/<path:path>")
def static_files(path):
    return send_from_directory(WEB_DIR, path)


@app.route("/")
def index():
    return send_from_directory(WEB_DIR, "index.html")


if __name__ == "__main__":
    app.run(port=PORT)
