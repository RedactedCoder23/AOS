#!/usr/bin/env python3
"""Backend helper to query AI provider plugins."""
import os
import sys
from scripts.ai_providers.loader import get_provider


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
