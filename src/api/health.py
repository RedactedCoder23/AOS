from fastapi import FastAPI, HTTPException
from src.service.security import apply_security_headers
import redis

app = FastAPI()
apply_security_headers(app)


@app.get("/health/live")
def live() -> dict:
    """Liveness probe."""
    return {"status": "ok"}


@app.get("/health/ready")
def ready() -> dict:
    """Readiness probe; checks Redis connectivity."""
    try:
        redis.Redis().ping()
    except Exception as exc:
        raise HTTPException(status_code=503, detail="redis unreachable") from exc
    return {"status": "ok"}
