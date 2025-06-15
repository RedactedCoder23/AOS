from functools import wraps
from typing import Any, Callable, Awaitable
from fastapi import HTTPException, Request

PERMISSIONS = {
    "view": {"user", "admin"},
    "write": {"admin"},
}


def _get_role(request: Request) -> str:
    if hasattr(request.state, "role"):
        return request.state.role
    return request.headers.get("Role", "user")


def requires_permission(
    name: str,
) -> Callable[[Callable[..., Awaitable[Any]]], Callable[..., Awaitable[Any]]]:
    def decorator(func: Callable[..., Awaitable[Any]]):
        @wraps(func)
        async def wrapper(*args, **kwargs):
            request = kwargs.get("request")
            if request is None:
                for arg in args:
                    if isinstance(arg, Request):
                        request = arg
                        break
            if request is None:
                raise HTTPException(status_code=400, detail="request missing")
            role = _get_role(request)
            if role not in PERMISSIONS.get(name, set()):
                raise HTTPException(status_code=403, detail="forbidden")
            if (
                not hasattr(func, "__call__")
                or not hasattr(func, "__code__")
                or not func.__code__.co_flags & 0x80
            ):
                return func(*args, **kwargs)
            return await func(*args, **kwargs)

        return wrapper

    return decorator
