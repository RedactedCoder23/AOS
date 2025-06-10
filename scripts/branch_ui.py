#!/usr/bin/env python3
"""Branch lifecycle HTTP service with micro-VM support."""
import json
import os
import queue
import subprocess
from flask import Flask, Response, jsonify, request, send_from_directory
from .agent_orchestrator import AgentOrchestrator
from .aos_audit import log_entry

PORT = 8000
BASE = os.path.dirname(os.path.abspath(__file__))
WEB_DIR = os.path.join(BASE, "..", "ui")
GRAPH_FILE = os.path.join(BASE, "..", "examples", "graph_sample.json")
METRICS_FILE = os.path.join(BASE, "..", "examples", "metrics_sample.json")
FC_BIN = os.environ.get("FIRECRACKER", "firecracker")


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
    if os.path.exists(host):
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
    if cmd == "snapshot":
        return len(service.branches) + 10
    if cmd == "delete":
        return 0
    return 0


class BranchService:
    def __init__(self, logger):
        self.branches = {}
        self.logger = logger

    def create(self):
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
            ]
        )
        self.logger.info("Launched branch %d, PID %d", branch_id, proc.pid)
        self.branches[branch_id] = {
            "pid": proc.pid,
            "sock": f"/tmp/fc-{branch_id}.sock",
            "status": "RUNNING",
            "last_snapshot_id": 0,
            "owner_uid": os.getuid(),
        }
        log_entry(str(os.getuid()), "branch_create", f"branch:{branch_id}", "success")
        flask_sse.publish({"branch_id": branch_id}, event="branch-created")
        return branch_id

    def list(self):
        return [
            {
                "branch_id": bid,
                "status": info["status"],
                "last_snapshot_id": info.get("last_snapshot_id", 0),
            }
            for bid, info in self.branches.items()
        ]

    def merge(self, bid):
        info = self.branches.get(bid)
        if info and info.get("owner_uid") != os.getuid():
            log_entry(str(os.getuid()), "branch_merge", f"branch:{bid}", "denied")
            return {"error": "permission denied"}
        rc = kernel_ipc("merge", {"branch_id": bid})
        try:
            out = subprocess.check_output(
                [
                    "python3",
                    os.path.join(BASE, "merge_ai.py"),
                    str(bid),
                ],
                text=True,
            ).strip()
            if isinstance(rc, int) and rc < 0:
                log_entry(str(os.getuid()), "branch_merge", f"branch:{bid}", "denied")
                return {"error": "permission denied"}
            flask_sse.publish({"branch_id": bid}, event="branch-merged")
            log_entry(str(os.getuid()), "branch_merge", f"branch:{bid}", "success")
            return {"merged": rc == 0, "detail": out}
        except subprocess.CalledProcessError as e:
            return {"error": e.output.strip()}

    def snapshot(self, bid):
        info = self.branches.get(bid)
        if info and info.get("owner_uid") != os.getuid():
            log_entry(str(os.getuid()), "branch_snapshot", f"branch:{bid}", "denied")
            return {"error": "permission denied"}
        sid = kernel_ipc("snapshot", {"branch_id": bid})
        if isinstance(sid, int) and sid < 0:
            log_entry(str(os.getuid()), "branch_snapshot", f"branch:{bid}", "denied")
            return {"error": "invalid branch"}
        if bid in self.branches:
            self.branches[bid]["last_snapshot_id"] = sid
        flask_sse.publish(
            {"branch_id": bid, "snapshot_id": sid}, event="branch-snapshot"
        )
        log_entry(str(os.getuid()), "branch_snapshot", f"branch:{bid}", "success")
        return {"snapshot_id": sid}

    def delete(self, bid):
        info = self.branches.get(bid)
        if info and info.get("owner_uid") != os.getuid():
            log_entry(str(os.getuid()), "branch_delete", f"branch:{bid}", "denied")
            return {"error": "permission denied"}
        rc = kernel_ipc("delete", {"branch_id": bid})
        if isinstance(rc, int) and rc < 0:
            log_entry(str(os.getuid()), "branch_delete", f"branch:{bid}", "denied")
            return {"error": "invalid branch"}
        self.branches.pop(bid, None)
        flask_sse.publish({"branch_id": bid}, event="branch-deleted")
        log_entry(str(os.getuid()), "branch_delete", f"branch:{bid}", "success")
        return {"success": True}


flask_sse = _SSE()
app = Flask(__name__, static_folder=WEB_DIR, static_url_path="")
service = BranchService(app.logger)
agent_orch = AgentOrchestrator(app.logger)


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
    bid = service.create()
    return jsonify({"branch_id": bid})


@app.route("/branches", methods=["GET"])
def list_branches():
    return jsonify(service.list())


@app.route("/branches/<int:bid>/merge", methods=["POST"])
def merge_branch(bid):
    return jsonify(service.merge(bid))


@app.route("/branches/<int:bid>/snapshot", methods=["POST"])
def snapshot_branch(bid):
    res = service.snapshot(bid)
    if "error" in res:
        return jsonify(res), 400
    return jsonify(res)


@app.route("/branches/<int:bid>", methods=["DELETE"])
def delete_branch(bid):
    res = service.delete(bid)
    if "error" in res:
        return jsonify(res), 400
    return jsonify(res)


@app.route("/events")
def events():
    return flask_sse.stream()


@app.route("/branches/<int:bid>/agents")
def branch_agents(bid):
    return agent_orch.stream(bid)


@app.route("/<path:path>")
def static_files(path):
    return send_from_directory(WEB_DIR, path)


@app.route("/")
def index():
    return send_from_directory(WEB_DIR, "index.html")


if __name__ == "__main__":
    app.run(port=PORT)
