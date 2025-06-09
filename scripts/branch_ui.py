#!/usr/bin/env python3
import http.server
import socketserver
import os

PORT = 8000
BASE = os.path.dirname(os.path.abspath(__file__))
WEB_DIR = os.path.join(BASE, "..", "ui")
GRAPH_FILE = os.path.join(BASE, "..", "examples", "graph_sample.json")
METRICS_FILE = os.path.join(BASE, "..", "examples", "metrics_sample.json")
BRANCHES_FILE = os.path.expanduser("~/.aos/branches.json")


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
            path = BRANCHES_FILE if os.path.exists(BRANCHES_FILE) else GRAPH_FILE
            self.send_response(200)
            self.send_header("Content-Type", "application/json")
            self.end_headers()
            with open(path, "rb") as f:
                self.wfile.write(f.read())
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
        else:
            self.send_error(404)


if __name__ == "__main__":
    with socketserver.TCPServer(("", PORT), BranchUIHandler) as httpd:
        print(f"Branch UI available at http://localhost:{PORT}")
        try:
            httpd.serve_forever()
        except KeyboardInterrupt:
            pass
