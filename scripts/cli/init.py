#!/usr/bin/env python3
/* AOS — scripts/cli/init.py — Purpose: Python module */
# AOS — ./scripts/cli/init.py
# (c) 2025 RedactedCoder23
# Brief: Source file.
"""Interactive setup wizard creating .env and docker-compose.yml."""

import os

ENV_FILE = ".env"
COMPOSE_FILE = "docker-compose.yml"

DC_TEMPLATE = """version: "3"
services:
  aos:
    image: aos:latest
    env_file:
      - .env
    ports:
      - "5000:5000"
      - "8080:8080"
    privileged: true
"""


def main() -> int:
    redis = input("Redis URL [redis://localhost:6379]: ") or "redis://localhost:6379"
    vault = input("Vault address [http://localhost:8200]: ") or "http://localhost:8200"
    with open(ENV_FILE, "w", encoding="utf-8") as fh:
        fh.write(f"AOS_REDIS_URL={redis}\n")
        fh.write(f"VAULT_ADDR={vault}\n")
    if not os.path.exists(COMPOSE_FILE):
        with open(COMPOSE_FILE, "w", encoding="utf-8") as fh:
            fh.write(DC_TEMPLATE)
    print("Configuration written to .env and docker-compose.yml")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
