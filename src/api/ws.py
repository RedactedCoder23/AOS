import asyncio
import json
import redis
from fastapi import FastAPI, WebSocket
from src.service.queue import load_status

app = FastAPI()


@app.websocket("/ws/branches/{id}")
async def branch_ws(ws: WebSocket, id: str):
    await ws.accept()
    r = redis.Redis()
    pubsub = r.pubsub()
    channel = f"branch:{id}"
    pubsub.subscribe(channel)
    status = load_status(id).get("status")
    if status is not None:
        await ws.send_text(json.dumps({"status": status}))
    try:
        while True:
            message = pubsub.get_message(timeout=1)
            if message and message["type"] == "message":
                await ws.send_text(message["data"].decode())
            await asyncio.sleep(0.1)
    except Exception:
        pass
    finally:
        pubsub.close()
        await ws.close()
