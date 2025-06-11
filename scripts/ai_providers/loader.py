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
    """Load provider plugins from ``providers.json`` if changed."""
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
            if isinstance(info, str):
                module_path, cls_name = info.rsplit(".", 1)
            else:
                module_path = f"scripts.ai_providers.{info['module']}"
                cls_name = info["class"]
            mod = importlib.import_module(module_path)
            cls = getattr(mod, cls_name)
            PROVIDERS[name] = cls(name)
        except Exception:
            continue
    _mtime = mtime


def load_providers() -> None:
    """Public wrapper to (re)load provider map if needed."""
    _load_providers()


def get_provider(alias: str) -> AIProvider:
    """Return provider instance for ``alias`` reloading config if needed."""
    _load_providers()
    provider = PROVIDERS.get(alias)
    if provider is None:
        raise ValueError(f"Unknown provider: {alias}")
    return provider
