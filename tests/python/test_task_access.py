/* AOS — tests/python/test_task_access.py — Purpose: Python module */
# AOS — ./tests/python/test_task_access.py
# (c) 2025 RedactedCoder23
# Brief: Source file.
import os
import json
import tempfile
import unittest

from scripts import agent_orchestrator


class TaskAccessTest(unittest.TestCase):
    def setUp(self):
        self.tmp = tempfile.TemporaryDirectory()
        self.orig = os.getcwd()
        os.chdir(self.tmp.name)
        os.makedirs("branches/1", exist_ok=True)

    def tearDown(self):
        os.chdir(self.orig)
        self.tmp.cleanup()

    def test_results_dict_key_type(self):
        data = [
            {"agent_id": "1", "command": "echo one"},
            {"agent_id": "2", "command": "echo two"},
        ]
        with open("branches/1/tasks.json", "w") as fh:
            json.dump(data, fh)
        results = agent_orchestrator.run_agents(1)
        key = list(results.keys())[0]
        self.assertIsInstance(key, int)

    def test_path_uses_agent_id(self):
        data = [{"agent_id": "3", "command": "echo x"}]
        with open("branches/1/tasks.json", "w") as fh:
            json.dump(data, fh)
        agent_orchestrator.run_agents(1)
        self.assertTrue(os.path.exists("branches/1/agents/agent-3.json"))


if __name__ == "__main__":
    unittest.main()
