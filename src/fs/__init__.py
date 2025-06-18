# AOS — ./src/fs/__init__.py
# (c) 2025 RedactedCoder23
# Brief: Source file.
import json
import os
import time
import hashlib
from dataclasses import dataclass
from functools import lru_cache

from src.logging.logger import get_logger

logger = get_logger(__name__)


class FSException(Exception):
    def __init__(self, path: str, errno: int | None):
        super().__init__(f"{path}: errno {errno}")
        self.path = path
        self.errno = errno


def _journal_path() -> str:
    path = os.path.join(".journal", "log")
    os.makedirs(os.path.dirname(path), exist_ok=True)
    return path


def _write_journal(entry: dict) -> None:
    with open(_journal_path(), "a", encoding="utf-8") as fh:
        fh.write(json.dumps(entry) + "\n")


@dataclass
class JournalEntry:
    timestamp: float
    path: str
    op: str
    checksum: str
    committed: bool = False


@lru_cache(maxsize=128)
def _cached_read(path: str) -> bytes:
    with open(path, "rb") as fh:
        return fh.read()


@lru_cache(maxsize=128)
def _cached_stat(path: str) -> os.stat_result:
    return os.stat(path)


def invalidate_cache() -> None:
    _cached_read.cache_clear()
    _cached_stat.cache_clear()


def read_file(path: str) -> bytes:
    try:
        if os.path.islink(path):
            path = os.readlink(path)
        return _cached_read(path)
    except IOError as exc:
        raise FSException(path, exc.errno) from exc


def write_file(path: str, data: bytes) -> None:
    entry = JournalEntry(time.time(), path, "write", hashlib.sha256(data).hexdigest())
    _write_journal(entry.__dict__)
    try:
        if os.path.islink(path):
            path = os.readlink(path)
        with open(path, "wb") as fh:
            fh.write(data)
        entry.committed = True
        _write_journal(entry.__dict__)
        invalidate_cache()
    except IOError as exc:
        raise FSException(path, exc.errno) from exc


def stat(path: str) -> os.stat_result:
    try:
        if os.path.islink(path):
            path = os.readlink(path)
        return _cached_stat(path)
    except IOError as exc:
        raise FSException(path, exc.errno) from exc
