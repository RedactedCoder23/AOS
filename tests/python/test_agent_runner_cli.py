/* AOS — tests/python/test_agent_runner_cli.py — Purpose: Python module */
# AOS — ./tests/python/test_agent_runner_cli.py
# (c) 2025 RedactedCoder23
# Brief: Source file.
import os
import sys
import json
import tempfile
import unittest
from unittest import mock

sys.path.insert(0, os.path.abspath(os.path.join(os.path.dirname(__file__), "../..")))

from scripts import agent_runner  # noqa: E402


class AgentRunnerCliTest(unittest.TestCase):
    def _run(self, args):
        with tempfile.TemporaryDirectory() as tmp:
            hb = os.path.join(tmp, "hb")
            res = os.path.join(tmp, "res.json")
            cmd = (
                f"{sys.executable} -c "
                "\"import os, json; meta=os.getenv('AOS_TASK_META'); "
                "print(json.loads(meta)['provider'] if meta else "
                "os.getenv('AOS_AI_PROVIDER','openai'))\""
            )
            argv = ["agent_runner.py", cmd, hb, res] + args
            with mock.patch.object(sys, "argv", argv):
                agent_runner.main()
            with open(res, "r") as fh:
                data = json.load(fh)
            return data["stdout"].strip()

    def test_provider_flag(self):
        out = self._run(["--provider", "echo"])
        self.assertEqual(out, "echo")

    def test_default_provider(self):
        if "AOS_AI_PROVIDER" in os.environ:
            del os.environ["AOS_AI_PROVIDER"]
        out = self._run([])
        self.assertEqual(out, "openai")


if __name__ == "__main__":
    unittest.main()
