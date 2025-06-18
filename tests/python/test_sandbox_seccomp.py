/* AOS — tests/python/test_sandbox_seccomp.py — Purpose: Python module */
# AOS — ./tests/python/test_sandbox_seccomp.py
# (c) 2025 RedactedCoder23
# Brief: Source file.
import unittest
from pathlib import Path

import docker

ROOT = Path(__file__).resolve().parents[2]
FIXTURE = ROOT / "tests" / "fixtures" / "echo_plugin"
PROFILE = ROOT / "profiles" / "aos-seccomp.json"
IMAGE = "echo_plugin:test"


class SandboxSeccompTest(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.client = docker.from_env()
        cls.client.images.build(
            path=str(FIXTURE), dockerfile="Dockerfile.sandbox", tag=IMAGE
        )

    def test_syscall_blocked(self):
        cmd = ["python", "-c", "import os, signal; os.kill(os.getpid(), 0)"]
        cont = self.client.containers.run(
            IMAGE,
            cmd,
            cap_drop=["ALL"],
            pids_limit=256,
            security_opt=[f"seccomp={PROFILE}"],
            detach=True,
        )
        try:
            res = cont.wait(timeout=5)
            logs = cont.logs().decode()
        finally:
            cont.remove(force=True)
        self.assertNotEqual(res["StatusCode"], 0)
        self.assertIn("Operation not permitted", logs)


if __name__ == "__main__":  # pragma: no cover - manual run
    unittest.main()
