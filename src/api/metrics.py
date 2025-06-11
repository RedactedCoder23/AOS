from fastapi import FastAPI, HTTPException
from scripts.agent_orchestrator import get_stats
from scripts import aos_audit as audit

app = FastAPI()


@app.get("/branches/{id}/metrics")
def get_metrics(id: str):
    """Return live metrics for branch ``id``."""
    data = get_stats(id)
    if data is None:
        raise HTTPException(status_code=404, detail="branch not found")
    audit.log("get_metrics", branch=id, user="api")
    return data
