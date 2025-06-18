/* AOS — src/api/ws.py — Purpose: Python module */
# AOS — ./src/api/ws.py
# (c) 2025 RedactedCoder23
# Brief: Source file.
import asyncio
import json
from fastapi import FastAPI, WebSocket
from src.service.security import apply_security_headers
from src.service.queue import load_status
from src.api.errors import install as install_errors
from src.api.profiler import install as install_profiler
from src.security.permissions import requires_permission

app = FastAPI()
apply_security_headers(app)
install_errors(app)
install_profiler(app)


@app.websocket("/ws/status/{id}")
@requires_permission("view")
async def status_ws(websocket: WebSocket, id: str):
    await websocket.accept()
    while True:
        data = load_status(id)
        await websocket.send_text(json.dumps(data))
        await asyncio.sleep(1)
