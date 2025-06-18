# AOS — ./src/api/errors.py
# (c) 2025 RedactedCoder23
# Brief: Source file.
from fastapi import FastAPI, Request
from fastapi.responses import JSONResponse
from src.fs import FSException


def install(app: FastAPI) -> None:
    @app.exception_handler(FSException)
    async def fs_handler(request: Request, exc: FSException):
        return JSONResponse(status_code=500, content={"detail": str(exc)})

    @app.exception_handler(Exception)
    async def general_handler(request: Request, exc: Exception):
        return JSONResponse(status_code=500, content={"detail": str(exc)})
