# AOS — ./tests/python/test_orchestrator_scaling.py
# (c) 2025 RedactedCoder23
# Brief: Source file.
import os
import sys
import json
import tempfile
import unittest

sys.path.insert(0, os.path.abspath(os.path.join(os.path.dirname(__file__), "../..")))

from scripts import agent_orchestrator  # noqa: E402


class ScalingTest(unittest.TestCase):
    def setUp(self):
        self.tmp = tempfile.TemporaryDirectory()
        self.orig = os.getcwd()
        os.chdir(self.tmp.name)
        os.makedirs("branches/1", exist_ok=True)

    def tearDown(self):
        os.chdir(self.orig)
        self.tmp.cleanup()

    def _write_tasks(self, n: int):
        data = [{"agent_id": f"t{i}", "command": f"echo {i}"} for i in range(n)]
        with open("branches/1/tasks.json", "w") as fh:
            json.dump(data, fh)

    def test_scaling_metrics(self):
        self._write_tasks(6)
        list(agent_orchestrator.run_tasks(1))
        m = agent_orchestrator.METRICS.get(1, {})
        self.assertGreaterEqual(m.get("agents_spawned", 1), 2)


if __name__ == "__main__":
    unittest.main()
