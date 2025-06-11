import json
import os
import importlib
from .base import AIProvider


def get_provider(alias: str) -> AIProvider:
    """Return provider instance for *alias* using providers.json mapping."""
    cfg = os.path.join(
        os.path.dirname(os.path.dirname(os.path.dirname(__file__))), "providers.json"
    )
    with open(cfg, "r", encoding="utf-8") as fh:
        data = json.load(fh)
    path = data.get(alias)
    if not path:
        raise ValueError(f"Unknown provider: {alias}")
    module_path, class_name = path.rsplit(".", 1)
    module = importlib.import_module(module_path)
    cls = getattr(module, class_name)
    return cls(alias)
