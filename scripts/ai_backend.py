#!/usr/bin/env python3
"""Backend helper to query OpenAI API."""
import os
import sys
from ai_cred_client import get_api_key

try:
    import openai
except Exception:  # pragma: no cover - optional dependency
    openai = None

PROMPT_ERR = "usage: ai_backend.py <prompt>"


def main():
    if len(sys.argv) < 2:
        print(PROMPT_ERR, file=sys.stderr)
        return 1
    if os.environ.get("AOS_AI_OFFLINE"):
        prompt = sys.argv[1]
        print(f"[mock-ai] response to: {prompt}")
        return 0

    key = get_api_key("openai")
    if not key:
        print("ERROR: no AI credential for 'openai' \u2014 run `ai-cred set --service=openai <key>`", file=sys.stderr)
        return 2
    if openai is None:
        print("openai package missing", file=sys.stderr)
        return 2
    client = openai.OpenAI(api_key=key)
    prompt = sys.argv[1]
    try:
        resp = client.chat.completions.create(
            model="gpt-3.5-turbo",
            messages=[{"role": "user", "content": prompt}],
        )
        print(resp.choices[0].message.content.strip())
    except Exception as e:
        print(f"error: {e}", file=sys.stderr)
        return 3
    return 0


if __name__ == "__main__":
    sys.exit(main())
