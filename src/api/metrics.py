from fastapi import FastAPI, HTTPException
from scripts.agent_orchestrator import get_stats, get_cached_stats
from scripts import aos_audit as audit
from fastapi.responses import JSONResponse

app = FastAPI()


@app.get("/branches/{id}/metrics")
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
    audit.log("get_metrics", branch=id, user="api")
    return data
