/* AOS — src/api/metrics.py — Purpose: Python module */
# AOS — ./src/api/metrics.py
# (c) 2025 RedactedCoder23
# Brief: Source file.
from fastapi import FastAPI, HTTPException
from src.service.security import apply_security_headers
from scripts.agent_orchestrator import get_stats, get_cached_stats
from scripts import aos_audit as audit
from fastapi.responses import JSONResponse
from src.service.queue import load_status
from src.security.permissions import requires_permission
from src.api.errors import install as install_errors
from src.api.profiler import install as install_profiler

app = FastAPI()
apply_security_headers(app)
install_errors(app)
install_profiler(app)


@app.get("/branches/{id}/metrics")
@requires_permission("view")
def get_metrics(id: str):
    """Return live metrics for branch ``id``.

    Exceptions raised by the orchestrator are mapped to ``502`` responses.
    """
    try:
        data = get_stats(id)
    except RuntimeError as exc:
        cached = get_cached_stats(id)
        return JSONResponse(
            status_code=502, content={"error": str(exc), "data": cached}
        )
    if data is None:
        raise HTTPException(status_code=404, detail="branch not found")
    status = load_status(id).get("status")
    if status is not None:
        data["status"] = status
    audit.log("get_metrics", branch=id, user="api")
    return data
