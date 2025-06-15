import threading
import time
import psutil

from src.logging.logger import get_logger, log
from .allocator import allocator

logger = get_logger(__name__)
_samples: list[int] = []
_lock = threading.Lock()


def _watch() -> None:
    proc = psutil.Process()
    while True:
        rss = proc.memory_info().rss
        with _lock:
            _samples.append(rss)
            if len(_samples) > 5:
                _samples.pop(0)
            if len(_samples) == 5 and rss > _samples[0] * 1.10:
                log(logger, "warning", function="_watch", rss=rss, base=_samples[0])
        time.sleep(10)


def fragmentation_ratio() -> float:
    total = allocator.total_allocated
    in_use = allocator.in_use
    if total == 0:
        return 0.0
    return (total - in_use) / total


threading.Thread(target=_watch, daemon=True).start()
