from fastapi import FastAPI, HTTPException
from src.service.security import apply_security_headers
import redis
import psutil
import time
from src.security.permissions import requires_permission
from src.api.errors import install as install_errors
from src.api.profiler import install as install_profiler

app = FastAPI()
apply_security_headers(app)
install_errors(app)
install_profiler(app)


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


@app.get("/health")
@requires_permission("view")
def health() -> dict:
    proc = psutil.Process()
    uptime = time.time() - proc.create_time()
    mem = proc.memory_info().rss
    return {"status": "ok", "memory": mem, "uptime": uptime}
