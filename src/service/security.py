from fastapi import FastAPI, Request, Response


def apply_security_headers(app: FastAPI) -> None:
    """Add basic security headers to ``app``."""

    @app.middleware("http")
    async def _security_headers(request: Request, call_next):
        response: Response = await call_next(request)
        response.headers["Content-Security-Policy"] = "default-src 'self'"
        response.headers["Strict-Transport-Security"] = (
            "max-age=63072000; includeSubDomains"
        )
        response.headers["X-Frame-Options"] = "DENY"
        return response
