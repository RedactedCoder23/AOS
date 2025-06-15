import json
import logging
import time
from typing import Any, Dict


def get_logger(name: str = "aos") -> logging.Logger:
    """Return a logger emitting JSON structured messages."""
    logger = logging.getLogger(name)
    if not logger.handlers:
        handler = logging.StreamHandler()
        handler.setFormatter(logging.Formatter("%(message)s"))
        logger.addHandler(handler)
        logger.setLevel(logging.INFO)
    return logger


def log(logger: logging.Logger, level: str, **fields: Any) -> None:
    """Emit a structured log using *logger* with *level*."""
    record: Dict[str, Any] = {
        "timestamp": time.strftime("%Y-%m-%dT%H:%M:%SZ", time.gmtime()),
        **fields,
    }
    msg = json.dumps(record)
    getattr(logger, level.lower(), logger.info)(msg)
