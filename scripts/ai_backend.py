#!/usr/bin/env python3
"""Backend helper to query AI provider plugins."""
import importlib
import json
import os
import sys
from scripts.ai_providers.base import AIProvider

PROVIDERS: dict[str, AIProvider] = {}


def _load_providers() -> None:
    """Load provider plugins from ``providers.json``."""
    global PROVIDERS
    if PROVIDERS:
        return
    cfg_path = os.path.join(
        os.path.dirname(os.path.dirname(__file__)), "providers.json"
    )
    try:
        with open(cfg_path, "r", encoding="utf-8") as fh:
            data = json.load(fh)
    except Exception:  # pragma: no cover - config missing
        data = {}
    for name, info in data.items():
        try:
            mod = importlib.import_module(f"scripts.ai_providers.{info['module']}")
            cls = getattr(mod, info["class"])
            PROVIDERS[name] = cls(name)
        except Exception:  # pragma: no cover - plugin errors
            continue


def _get_provider(name: str):
    _load_providers()
    return PROVIDERS.get(name)


PROMPT_ERR = "usage: ai_backend.py <prompt>"


def main():
    if len(sys.argv) < 2:
        print(PROMPT_ERR, file=sys.stderr)
        return 1

    provider_name = os.environ.get("AOS_AI_PROVIDER", "openai")
    provider = _get_provider(provider_name)
    if provider is None:
        print(f"ERROR: provider '{provider_name}' not available", file=sys.stderr)
        return 2
    prompt = sys.argv[1]

    if os.environ.get("AOS_AI_OFFLINE"):
        print(f"[mock-ai] response to: {prompt}")
        return 0

    try:
        out = provider.generate(prompt)
    except Exception as e:  # pragma: no cover - provider error
        print(f"ERROR: {e}", file=sys.stderr)
        return 3
    print(out)
    return 0


if __name__ == "__main__":
    sys.exit(main())
