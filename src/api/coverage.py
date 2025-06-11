from fastapi import FastAPI

app = FastAPI()


@app.get("/branches/{id}/coverage-history")
def get_coverage(id: str):
    """Return stub coverage history for branch ``id``."""
    return {"coverage": [], "threshold": 0.0}
