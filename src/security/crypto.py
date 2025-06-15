import os
from cryptography.hazmat.primitives.ciphers.aead import AESGCM

_KEY = os.environ.get("ENCRYPTION_KEY", "0" * 32).encode()


def encrypt(data: bytes) -> bytes:
    aes = AESGCM(_KEY)
    nonce = os.urandom(12)
    return nonce + aes.encrypt(nonce, data, None)


def decrypt(token: bytes) -> bytes:
    aes = AESGCM(_KEY)
    nonce, ct = token[:12], token[12:]
    return aes.decrypt(nonce, ct, None)
