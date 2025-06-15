from fastapi import FastAPI
from src.service.security import apply_security_headers
from src.security.permissions import requires_permission
from src.api.errors import install as install_errors
from src.api.profiler import install as install_profiler

app = FastAPI()
apply_security_headers(app)
install_errors(app)
install_profiler(app)


@app.get("/branches/{id}/coverage-history")
@requires_permission("view")
def get_coverage(id: str):
    return {"coverage": [], "timestamps": []}
