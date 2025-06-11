import unittest
import os
import json
import tempfile
from fastapi.testclient import TestClient
from src.api.metrics import app as metrics_app
from src.api.coverage import app as coverage_app
from src.api.branches import app as branches_app
from scripts import agent_orchestrator


class AuditActionsTest(unittest.TestCase):
    def setUp(self):
        self.tmp = tempfile.NamedTemporaryFile(delete=False)
        os.environ["AOS_AUDIT_LOG"] = self.tmp.name
        agent_orchestrator.branch_stats.clear()
        agent_orchestrator.branch_stats["1"] = {
            "pending_tasks": 0,
            "cpu_pct": 0.0,
            "mem_pct": 0.0,
            "history": [{"timestamp": "t"}],
        }
        base = os.path.expanduser("~/.aos/branches/1")
        os.makedirs(base, exist_ok=True)
        with open(os.path.join(base, "coverage-20250101.json"), "w") as fh:
            json.dump({"totals": {"percent_covered": 70}}, fh)
        with open("config.yml", "w") as fh:
            fh.write("coverage_threshold: 75.0\n")

    def tearDown(self):
        os.unlink(self.tmp.name)
        if os.path.exists("config.yml"):
            os.unlink("config.yml")

    def _read_log(self):
        with open(self.tmp.name) as fh:
            return [json.loads(line) for line in fh if line.strip()]

    def test_audit_entries(self):
        TestClient(metrics_app).get("/branches/1/metrics")
        TestClient(coverage_app).get("/branches/1/coverage-history")
        TestClient(branches_app).post("/branches/1/merge")
        actions = [e["action"] for e in self._read_log()]
        self.assertIn("get_metrics", actions)
        self.assertIn("get_coverage", actions)
        self.assertIn("merge_blocked", actions)


if __name__ == "__main__":
    unittest.main()
