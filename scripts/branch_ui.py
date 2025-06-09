#!/usr/bin/env python3
import http.server
import socketserver
import os

PORT = 8000
BASE = os.path.dirname(os.path.abspath(__file__))
WEB_DIR = os.path.join(BASE, '..', 'webui')
GRAPH_FILE = os.path.join(BASE, '..', 'examples', 'graph_sample.json')

class BranchUIHandler(http.server.SimpleHTTPRequestHandler):
    def __init__(self, *args, **kwargs):
        super().__init__(*args, directory=WEB_DIR, **kwargs)

    def do_GET(self):
        if self.path == '/graph':
            self.send_response(200)
            self.send_header('Content-Type', 'application/json')
            self.end_headers()
            with open(GRAPH_FILE, 'rb') as f:
                self.wfile.write(f.read())
        else:
            if self.path == '/':
                self.path = '/index.html'
            super().do_GET()

if __name__ == '__main__':
    with socketserver.TCPServer(('', PORT), BranchUIHandler) as httpd:
        print(f'Branch UI available at http://localhost:{PORT}')
        try:
            httpd.serve_forever()
        except KeyboardInterrupt:
            pass
