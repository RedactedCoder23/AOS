#!/usr/bin/env python3
# AOS — scripts/ai_backend.py — Purpose: Python module 
"""
AOS — scripts/ai_backend.py
(c) 2025 RedactedCoder23
Brief: FastAPI backend for the AI interpreter.
"""
import os
import sys

from scripts.ai_providers import loader
from scripts.ai_providers.base import AIProvider

PROVIDERS: dict[str, AIProvider] = loader.PROVIDERS


def _load_providers() -> None:
    """Compatibility wrapper around :func:`loader.load_providers`."""
    loader.load_providers()


def _get_provider(name: str) -> AIProvider:
    return loader.get_provider(name)


def get_provider(name: str) -> AIProvider:
    """Public helper returning provider instance by *name*."""
    return _get_provider(name)


PROMPT_ERR = "usage: ai_backend.py <prompt>"


def main() -> int:
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
