# AOS — ./scripts/ai_providers/plugin_runner.py
# (c) 2025 RedactedCoder23
# Brief: Source file.
from __future__ import annotations

import docker
from pathlib import Path

from .base import AIProvider


class PluginRunner(AIProvider):
    """Execute provider inside a sandbox container."""

    def __init__(self, name: str, image: str, entry: str):
        super().__init__(name)
        self.image = image
        self.entry = entry
        self.client = docker.from_env()
        self.seccomp = str(
            Path(__file__).resolve().parents[2] / "profiles" / "aos-seccomp.json"
        )

    def generate(self, prompt: str) -> str:  # pragma: no cover - docker mocked
        env = {"ENTRY_CLASS": self.entry, "PROMPT": prompt, "PROV_NAME": self.name}
        cont = self.client.containers.run(
            self.image,
            environment=env,
            volumes={"/contracts": {"bind": "/contracts", "mode": "ro"}},
            mem_limit=64 * 1024 * 1024,
            cap_drop=["ALL"],
            pids_limit=256,
            security_opt=[f"seccomp={self.seccomp}"],
            detach=True,
        )
        try:
            cont.wait(timeout=5)
            output = cont.logs().decode().strip()
        finally:
            cont.remove(force=True)
        return output
