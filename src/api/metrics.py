from fastapi import FastAPI

app = FastAPI()


@app.get("/branches/{id}/metrics")
def get_metrics(id: str):
    """Return stub metrics for branch ``id``."""
    return {"pending_tasks": 0, "cpu_pct": 0.0, "mem_pct": 0.0, "history": []}
