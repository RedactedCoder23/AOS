#!/usr/bin/env python3
import os
import sys
import openai

PROMPT_ERR = "usage: ai_backend.py <prompt>"


def main():
    if len(sys.argv) < 2:
        print(PROMPT_ERR, file=sys.stderr)
        return 1
    key = os.environ.get("OPENAI_API_KEY") or os.environ.get("AOS_OPENAI_API_KEY")
    if not key:
        print("missing OPENAI_API_KEY", file=sys.stderr)
        return 2
    client = openai.OpenAI(api_key=key)
    prompt = sys.argv[1]
    try:
        resp = client.chat.completions.create(
            model="gpt-3.5-turbo", messages=[{"role": "user", "content": prompt}]
        )
        print(resp.choices[0].message.content.strip())
    except Exception as e:
        print(f"error: {e}", file=sys.stderr)
        return 3
    return 0


if __name__ == "__main__":
    sys.exit(main())
