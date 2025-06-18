# AOS — ./scripts/ai_cred_client.py
# (c) 2025 RedactedCoder23
# Brief: Source file.
"""Client helper to fetch AI credentials from the vault daemon."""

from __future__ import annotations

import json
import os
import socket


SOCK_PATH = os.environ.get("AICRED_SOCK", "/run/ai-cred.sock")


def get_api_key(service: str) -> str:
    """Return API key for *service* via ``ai_cred_manager``.

    Raises ``KeyError`` if no credential is stored.
    """

    req = json.dumps({"method": "get", "params": {"service": service}}).encode()
    with socket.socket(socket.AF_UNIX, socket.SOCK_STREAM) as s:
        s.connect(SOCK_PATH)
        s.sendall(req)
        data = s.recv(1024)

    resp = json.loads(data.decode())
    if not resp.get("result"):
        raise KeyError(service)
    return resp["result"]
