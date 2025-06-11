#!/usr/bin/env python3
"""Backend helper to query AI provider plugins."""
import os
import sys
<<<<<< codex/implement-plugin-loader-hot-reload
from scripts.ai_providers import loader
from scripts.ai_providers.base import AIProvider

PROVIDERS: dict[str, AIProvider] = loader.PROVIDERS


def _load_providers() -> None:
<<<<<< codex/add-echo-and-openai-provider-plugins
    """Load provider plugins from ``providers.json``."""
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
            if isinstance(info, str):
                module_path, cls_name = info.rsplit(".", 1)
            else:  # backward compat
                module_path = f"scripts.ai_providers.{info['module']}"
                cls_name = info["class"]
            mod = importlib.import_module(module_path)
            cls = getattr(mod, cls_name)
            PROVIDERS[name] = cls(name)
        except Exception:  # pragma: no cover - plugin errors
            continue
=======
    """Compatibility wrapper around :func:`loader.load_providers`."""
    loader.load_providers()
>>>>>> main


def _get_provider(name: str):
    return loader.get_provider(name)
=======
from scripts.ai_providers.loader import get_provider
>>>>> main


def get_provider(name: str):
    """Public helper returning provider instance by *name*."""
    return _get_provider(name)


PROMPT_ERR = "usage: ai_backend.py <prompt>"


def main():
    if len(sys.argv) < 2:
        print(PROMPT_ERR, file=sys.stderr)
        return 1

    provider_name = os.environ.get("AOS_AI_PROVIDER", "openai")
    try:
        provider = get_provider(provider_name)
    except ValueError:
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
