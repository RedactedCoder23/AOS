"""Simple agent orchestrator with timeout and SSE status events."""

import subprocess
import queue
import json
from typing import Dict
from flask import Response


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


class AgentOrchestrator:
    def __init__(self, logger):
        self.logger = logger
        self._sses: Dict[int, _SSE] = {}

    def _get_sse(self, branch_id: int) -> _SSE:
        if branch_id not in self._sses:
            self._sses[branch_id] = _SSE()
        return self._sses[branch_id]

    def stream(self, branch_id: int):
        return self._get_sse(branch_id).stream()

    def launch(self, branch_id: int, agent_id: int, cmd, timeout: int = 60):
        proc = subprocess.Popen(
            cmd,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            text=True,
        )
        try:
            proc.wait(timeout=timeout)
            exit_code = proc.returncode
        except subprocess.TimeoutExpired:
            proc.kill()
            exit_code = -1
        out = proc.stdout.read() if proc.stdout else ""
        err = proc.stderr.read() if proc.stderr else ""
        result = out or err
        status = "success" if exit_code == 0 else "failed"
        event = {"agent_id": agent_id, "status": status}
        if exit_code == -1:
            event["error"] = "Timeout"
        elif status == "failed":
            event["error"] = f"Exit {exit_code}"
        if result:
            event["output"] = result.strip()
        if status != "success":
            self.logger.error("Agent %d failed: %s", agent_id, err.strip())
        sse = self._get_sse(branch_id)
        sse.publish(event, event="agent-status")
        return event
