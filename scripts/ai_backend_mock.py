#!/usr/bin/env python3
"""Offline mock for ai_backend."""
import sys


def main():
    prompt = sys.argv[1] if len(sys.argv) > 1 else ""
    print(f"[mock-ai] response to: {prompt}")


if __name__ == "__main__":
    main()
