/* AOS — tests/python/test_task_dependencies.py — Purpose: Python module */
# AOS — ./tests/python/test_task_dependencies.py
# (c) 2025 RedactedCoder23
# Brief: Source file.
import os
import sys
import tempfile
import unittest
import json

sys.path.insert(0, os.path.abspath(os.path.join(os.path.dirname(__file__), "../..")))

from scripts import agent_orchestrator  # noqa: E402


class DependencyTest(unittest.TestCase):
    def setUp(self):
        self.tmp = tempfile.TemporaryDirectory()
        self.orig = os.getcwd()
        os.chdir(self.tmp.name)
        os.makedirs("branches/1", exist_ok=True)

    def tearDown(self):
        os.chdir(self.orig)
        self.tmp.cleanup()

    def test_priority_and_dependencies(self):
        data = [
            {"agent_id": "lint", "command": "echo lint", "priority": 0},
            {"agent_id": "build", "command": "echo build", "priority": 1},
            {
                "agent_id": "test",
                "command": "echo test",
                "depends_on": ["lint", "build"],
                "priority": 5,
            },
        ]
        with open("branches/1/tasks.json", "w") as fh:
            json.dump(data, fh)
        events = list(agent_orchestrator.run_tasks(1))
        order = [e.get("stdout") for e in events if e.get("status")]
        self.assertEqual(order[0].strip(), "lint")
        self.assertEqual(order[1].strip(), "build")
        res = json.load(open("branches/1/agents/agent-test.json"))
        self.assertEqual(res["status"], "success")


if __name__ == "__main__":
    unittest.main()
