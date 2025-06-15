import os
from datetime import datetime, timedelta
import jwt

_SECRET = os.environ.get("JWT_SECRET", "dev-secret")


def create_access_token(user: str, role: str) -> str:
    payload = {
        "user": user,
        "role": role,
        "exp": datetime.utcnow() + timedelta(minutes=15),
    }
    return jwt.encode(payload, _SECRET, algorithm="HS256")


def verify(token: str) -> dict:
    return jwt.decode(token, _SECRET, algorithms=["HS256"])
