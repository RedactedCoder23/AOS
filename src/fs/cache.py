# AOS — ./src/fs/cache.py
# (c) 2025 RedactedCoder23
# Brief: Source file.
from functools import lru_cache
import os


@lru_cache(maxsize=128)
def read_file(path: str) -> bytes:
    with open(path, "rb") as fh:
        return fh.read()


@lru_cache(maxsize=128)
def stat(path: str) -> os.stat_result:
    return os.stat(path)


def invalidate() -> None:
    read_file.cache_clear()
    stat.cache_clear()
