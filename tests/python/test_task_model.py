# AOS — tests/python/test_task_model.py — Purpose: Python module 
# AOS — ./tests/python/test_task_model.py
# (c) 2025 RedactedCoder23
# Brief: Source file.
import os
import json
import tempfile
import unittest

from scripts import agent_orchestrator
from src.branch.task_definitions import Task


class TaskModelTest(unittest.TestCase):
    def setUp(self):
        self.tmp = tempfile.TemporaryDirectory()
        self.orig = os.getcwd()
        os.chdir(self.tmp.name)
        os.makedirs("branches/1", exist_ok=True)

    def tearDown(self):
        os.chdir(self.orig)
        self.tmp.cleanup()

    def _write(self):
        data = [
            {"agent_id": "1", "command": "echo hi"},
            {"agent_id": "2", "command": "echo there"},
        ]
        with open("branches/1/tasks.json", "w") as fh:
            json.dump(data, fh)

    def test_load_returns_tasks(self):
        self._write()
        tasks = agent_orchestrator._load_spec(1)
        self.assertTrue(all(isinstance(t, Task) for t in tasks))

    def test_no_dict_paths(self):
        self._write()
        tasks = agent_orchestrator._load_spec(1)
        for t in tasks:
            self.assertIsInstance(t, Task)


if __name__ == "__main__":
    unittest.main()
