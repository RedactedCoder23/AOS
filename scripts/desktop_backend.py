#!/usr/bin/env python3
# AOS — ./scripts/desktop_backend.py
# (c) 2025 RedactedCoder23
# Brief: Source file.
"""Backend server powering the desktop web UI."""
import http.server
import socketserver
import json
import os
import subprocess
import urllib.parse

PORT = 8000
BASE = os.path.dirname(os.path.abspath(__file__))
WEB_DIR = os.path.join(BASE, "..", "ui")
GRAPH_FILE = os.path.expanduser("~/.aos/branches.json")
FALLBACK_GRAPH = os.path.join(BASE, "..", "examples", "graph_sample.json")
METRICS_FILE = os.path.join(BASE, "..", "examples", "metrics_sample.json")


class DesktopHandler(http.server.SimpleHTTPRequestHandler):
    def __init__(self, *args, **kwargs):
        super().__init__(*args, directory=WEB_DIR, **kwargs)

    def _send_json(self, data):
        self.send_response(200)
        self.send_header("Content-Type", "application/json")
        self.end_headers()
        self.wfile.write(data.encode())

    def do_GET(self):
        if self.path == "/graph":
            path = GRAPH_FILE if os.path.exists(GRAPH_FILE) else FALLBACK_GRAPH
            with open(path, "r") as f:
                self._send_json(f.read())
        elif self.path == "/metrics":
            with open(METRICS_FILE, "r") as f:
                self._send_json(f.read())
        elif self.path == "/export":
            path = GRAPH_FILE if os.path.exists(GRAPH_FILE) else FALLBACK_GRAPH
            with open(path, "r") as f:
                self._send_json(f.read())
        elif self.path.startswith("/ai"):
            q = urllib.parse.urlparse(self.path).query
            prompt = urllib.parse.parse_qs(q).get("q", [""])[0]
            if not prompt:
                self._send_json('{"error":"missing prompt"}')
                return
            try:
                out = subprocess.check_output(
                    ["python3", os.path.join(BASE, "ai_backend.py"), prompt],
                    stderr=subprocess.STDOUT,
                    text=True,
                )
                self._send_json('{"reply": %s}' % json.dumps(out.strip()))
            except subprocess.CalledProcessError as e:
                self._send_json('{"error":%s}' % json.dumps(e.output.strip()))
        else:
            if self.path == "/":
                self.path = "/desktop.html"
            super().do_GET()

    def do_POST(self):
        if self.path == "/import":
            length = int(self.headers.get("Content-Length", 0))
            data = self.rfile.read(length)
            os.makedirs(os.path.dirname(GRAPH_FILE), exist_ok=True)
            with open(GRAPH_FILE, "wb") as f:
                f.write(data)
            self.send_response(200)
            self.end_headers()
            self.wfile.write(b"OK")
        else:
            self.send_error(404)


if __name__ == "__main__":
    with socketserver.TCPServer(("", PORT), DesktopHandler) as httpd:
        print(f"Desktop available at http://localhost:{PORT}")
        try:
            httpd.serve_forever()
        except KeyboardInterrupt:
            pass
