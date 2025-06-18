# AOS — scripts/tests/test_agent_orchestrator.py — Purpose: Python module 
# AOS — ./scripts/tests/test_agent_orchestrator.py
# (c) 2025 RedactedCoder23
# Brief: Source file.
import os
import json
import tempfile
import unittest
import sys

sys.path.insert(0, os.path.abspath(os.path.join(os.path.dirname(__file__), "../..")))

from scripts import agent_orchestrator  # noqa: E402


class AgentOrchestratorTest(unittest.TestCase):
    def setUp(self):
        self.tmp = tempfile.TemporaryDirectory()
        self.orig = os.getcwd()
        os.chdir(self.tmp.name)
        os.makedirs("branches/1", exist_ok=True)
        with open("branches/1/tasks.yaml", "w") as fh:
            fh.write(
                "- agent_id: ok\n  command: 'echo hi'\n"
                "- agent_id: fail\n  command: 'python -c \"import sys; sys.exit(1)\"'\n"
            )

    def tearDown(self):
        os.chdir(self.orig)
        self.tmp.cleanup()

    def test_main_runs_tasks(self):
        agent_orchestrator.main(1)
        out_dir = os.path.join("branches", "1", "agents")
        files = sorted(os.listdir(out_dir))
        self.assertEqual(len(files), 2)
        statuses = {json.load(open(os.path.join(out_dir, f)))["status"] for f in files}
        self.assertIn("success", statuses)
        self.assertIn("failed", statuses)


if __name__ == "__main__":
    unittest.main()
