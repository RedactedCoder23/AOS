import json
import os
import socket

SOCK_PATH = os.environ.get("AICRED_SOCK", "/run/ai-cred.sock")


def get_api_key(service):
    try:
        with socket.socket(socket.AF_UNIX, socket.SOCK_STREAM) as s:
            s.connect(SOCK_PATH)
            s.sendall(
                json.dumps({"method": "get", "params": {"service": service}}).encode()
            )
            data = s.recv(1024)
        resp = json.loads(data.decode())
        return resp.get("result")
    except Exception:
        return None
