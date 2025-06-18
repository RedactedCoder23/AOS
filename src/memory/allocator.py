/* AOS — src/memory/allocator.py — Purpose: Python module */
# AOS — ./src/memory/allocator.py
# (c) 2025 RedactedCoder23
# Brief: Source file.
from __future__ import annotations
import threading
from typing import Dict

from src.logging.logger import get_logger, log

logger = get_logger(__name__)

_GUARD = b"GUARD"


class Allocation:
    def __init__(self, size: int):
        self.buffer = bytearray(_GUARD + b"\x00" * size + _GUARD)
        self.size = size

    def view(self) -> memoryview:
        return memoryview(self.buffer)[len(_GUARD) : -len(_GUARD)]

    def check(self) -> None:
        if not (self.buffer.startswith(_GUARD) and self.buffer.endswith(_GUARD)):
            log(
                logger,
                "error",
                function="Allocation.check",
                detail="guard page corrupted",
            )
            raise MemoryError("guard page violated")


class Allocator:
    def __init__(self) -> None:
        self._lock = threading.Lock()
        self.blocks: Dict[int, Allocation] = {}
        self.total_allocated = 0
        self.in_use = 0

    def alloc(self, size: int) -> memoryview:
        with self._lock:
            a = Allocation(size)
            self.blocks[id(a)] = a
            self.total_allocated += size
            self.in_use += size
            log(logger, "info", function="Allocator.alloc", size=size)
            return a.view()

    def free(self, ptr: memoryview) -> None:
        with self._lock:
            for key, alloc in list(self.blocks.items()):
                if alloc.view() is ptr:
                    alloc.check()
                    self.in_use -= alloc.size
                    del self.blocks[key]
                    log(logger, "info", function="Allocator.free", size=alloc.size)
                    return
            log(logger, "error", function="Allocator.free", detail="invalid pointer")
            raise MemoryError("invalid pointer")


allocator = Allocator()
