#!/usr/bin/env python3
"""Simple HTTP server for the branch graph UI."""
import http.server
import socketserver
import os
import subprocess

from scripts.ai_cred_client import get_api_key  # noqa: F401  # placeholder

PORT = 8000
BASE = os.path.dirname(os.path.abspath(__file__))
WEB_DIR = os.path.join(BASE, "..", "ui")
GRAPH_FILE = os.path.join(BASE, "..", "examples", "graph_sample.json")
METRICS_FILE = os.path.join(BASE, "..", "examples", "metrics_sample.json")
BRANCHES_FILE = os.path.expanduser("~/.aos/branches.json")
FC_BIN = os.environ.get("FIRECRACKER", "firecracker")


class BranchUIHandler(http.server.SimpleHTTPRequestHandler):
    def __init__(self, *args, **kwargs):
        super().__init__(*args, directory=WEB_DIR, **kwargs)

    def do_GET(self):
        if self.path == "/graph":
            self.send_response(200)
            self.send_header("Content-Type", "application/json")
            self.end_headers()
            with open(GRAPH_FILE, "rb") as f:
                self.wfile.write(f.read())
        elif self.path == "/metrics":
            self.send_response(200)
            self.send_header("Content-Type", "application/json")
            self.end_headers()
            with open(METRICS_FILE, "rb") as f:
                self.wfile.write(f.read())
        elif self.path == "/export":
            if os.path.exists(BRANCHES_FILE):
                path = BRANCHES_FILE
            else:
                path = GRAPH_FILE
            self.send_response(200)
            self.send_header("Content-Type", "application/json")
            self.end_headers()
            with open(path, "rb") as f:
                self.wfile.write(f.read())
        elif self.path == "/branches":
            self.send_response(200)
            self.send_header("Content-Type", "application/json")
            self.end_headers()
            data = (
                subprocess.check_output(
                    ["./build/host_test", "--ipc", "list"], text=True
                )
                if os.path.exists("./build/host_test")
                else "[]"
            )
            self.wfile.write(data.encode())
        else:
            if self.path == "/":
                self.path = "/index.html"
            super().do_GET()

    def do_POST(self):
        if self.path == "/import":
            length = int(self.headers.get("Content-Length", 0))
            data = self.rfile.read(length)
            os.makedirs(os.path.dirname(BRANCHES_FILE), exist_ok=True)
            with open(BRANCHES_FILE, "wb") as f:
                f.write(data)
            self.send_response(200)
            self.end_headers()
            self.wfile.write(b"OK")
        elif self.path == "/branches":
            self.send_response(200)
            self.send_header("Content-Type", "application/json")
            self.end_headers()
            # Placeholder create branch via kernel ipc
            self.wfile.write(b'{"branch_id":0}')
        elif self.path.startswith("/branches/") and self.path.endswith("/merge"):
            self.send_response(200)
            self.send_header("Content-Type", "application/json")
            self.end_headers()
            self.wfile.write(b'{"status":"ok"}')
        else:
            self.send_error(404)


if __name__ == "__main__":
    with socketserver.TCPServer(("", PORT), BranchUIHandler) as httpd:
        print(f"Branch UI available at http://localhost:{PORT}")
        try:
            httpd.serve_forever()
        except KeyboardInterrupt:
            pass
