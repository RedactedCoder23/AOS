/* AOS — src/api/memory.py — Purpose: Python module */
# AOS — ./src/api/memory.py
# (c) 2025 RedactedCoder23
# Brief: Source file.
from fastapi import FastAPI, Request
from src.service.security import apply_security_headers
from src.memory.monitor import fragmentation_ratio
from src.security.permissions import requires_permission
from src.api.errors import install as install_errors
from src.api.profiler import install as install_profiler

app = FastAPI()
apply_security_headers(app)
install_errors(app)
install_profiler(app)


@app.get("/memory/fragmentation")
@requires_permission("view")
def fragmentation(request: Request):
    return {"ratio": fragmentation_ratio()}
