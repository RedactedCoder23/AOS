from fastapi import FastAPI, HTTPException
import redis

app = FastAPI()


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
