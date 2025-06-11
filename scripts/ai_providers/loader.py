import importlib
import json
import os
from typing import Dict

from .base import AIProvider

_ROOT = os.path.dirname(os.path.dirname(os.path.dirname(__file__)))
_CFG = os.path.join(_ROOT, "providers.json")

PROVIDERS: Dict[str, AIProvider] = {}
_mtime: float | None = None


def _load_providers() -> None:
    """Load provider plugins from :data:`providers.json`."""
    global _mtime
    try:
        mtime = os.path.getmtime(_CFG)
    except OSError:
        mtime = None
    if PROVIDERS and mtime == _mtime:
        return

    data = {}
    if mtime is not None:
        try:
            with open(_CFG, "r", encoding="utf-8") as fh:
                data = json.load(fh)
        except Exception:
            data = {}

    PROVIDERS.clear()
    for name, info in data.items():
        try:
            mod = importlib.import_module(f"scripts.ai_providers.{info['module']}")
            cls = getattr(mod, info["class"])
            PROVIDERS[name] = cls(name)
        except Exception:
            continue
    _mtime = mtime


def load_providers() -> None:
    """Public wrapper to (re)load provider map if needed."""
    _load_providers()


def get_provider(name: str) -> AIProvider | None:
    """Return provider instance by ``name`` reloading config if changed."""
    _load_providers()
    return PROVIDERS.get(name)
