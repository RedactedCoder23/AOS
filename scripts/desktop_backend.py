#!/usr/bin/env python3
import http.server
import socketserver
import json
import os
import subprocess
import urllib.parse

PORT = 8000
BASE = os.path.dirname(os.path.abspath(__file__))
WEB_DIR = os.path.join(BASE, '..', 'webui')
GRAPH_FILE = os.path.expanduser('~/.aos/branches.json')
FALLBACK_GRAPH = os.path.join(BASE, '..', 'examples', 'graph_sample.json')

class DesktopHandler(http.server.SimpleHTTPRequestHandler):
    def __init__(self, *args, **kwargs):
        super().__init__(*args, directory=WEB_DIR, **kwargs)

    def _send_json(self, data):
        self.send_response(200)
        self.send_header('Content-Type', 'application/json')
        self.end_headers()
        self.wfile.write(data.encode())

    def do_GET(self):
        if self.path == '/graph':
            path = GRAPH_FILE if os.path.exists(GRAPH_FILE) else FALLBACK_GRAPH
            with open(path, 'r') as f:
                self._send_json(f.read())
        elif self.path.startswith('/ai'):
            q = urllib.parse.urlparse(self.path).query
            prompt = urllib.parse.parse_qs(q).get('q', [''])[0]
            if not prompt:
                self._send_json('{"error":"missing prompt"}')
                return
            try:
                out = subprocess.check_output([
                    'python3', os.path.join(BASE, 'ai_backend.py'), prompt
                ], stderr=subprocess.STDOUT, text=True)
                self._send_json('{"reply": %s}' % json.dumps(out.strip()))
            except subprocess.CalledProcessError as e:
                self._send_json('{"error":%s}' % json.dumps(e.output.strip()))
        else:
            if self.path == '/':
                self.path = '/desktop.html'
            super().do_GET()

if __name__ == '__main__':
    with socketserver.TCPServer(('', PORT), DesktopHandler) as httpd:
        print(f'Desktop available at http://localhost:{PORT}')
        try:
            httpd.serve_forever()
        except KeyboardInterrupt:
            pass
