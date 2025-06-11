#!/usr/bin/env python3
"""Backend helper to query AI provider plugins."""
import os
import sys
from scripts.ai_providers import loader
from scripts.ai_providers.base import AIProvider

PROVIDERS: dict[str, AIProvider] = loader.PROVIDERS


def _load_providers() -> None:
    """Compatibility wrapper around :func:`loader.load_providers`."""
    loader.load_providers()


def _get_provider(name: str):
    return loader.get_provider(name)


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
