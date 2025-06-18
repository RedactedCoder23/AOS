# AOS — ./src/api/profiler.py
# (c) 2025 RedactedCoder23
# Brief: Source file.
import time
from fastapi import FastAPI, Request
from src.logging.logger import get_logger, log

logger = get_logger(__name__)


def install(app: FastAPI) -> None:
    @app.middleware("http")
    async def _profiler(request: Request, call_next):
        start = time.perf_counter()
        response = await call_next(request)
        elapsed = (time.perf_counter() - start) * 1000
        if elapsed > 200:
            log(logger, "warning", path=request.url.path, ms=elapsed)
        return response
