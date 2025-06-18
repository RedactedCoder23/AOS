/* AOS — tests/python/test_api_metrics_live.py — Purpose: Python module */
# AOS — ./tests/python/test_api_metrics_live.py
# (c) 2025 RedactedCoder23
# Brief: Source file.
import unittest
from fastapi.testclient import TestClient
from src.api.metrics import app
from scripts import agent_orchestrator


class MetricsLiveAPITest(unittest.TestCase):
    def setUp(self):
        agent_orchestrator.branch_stats.clear()
        agent_orchestrator.branch_stats["foo"] = {
            "pending_tasks": 2,
            "cpu_pct": 10.0,
            "mem_pct": 30.0,
            "history": [{"timestamp": "t"}],
        }

    def test_live_metrics(self):
        client = TestClient(app)
        resp = client.get("/branches/foo/metrics")
        self.assertEqual(resp.status_code, 200)
        data = resp.json()
        self.assertIn("cpu_pct", data)
        self.assertGreaterEqual(len(data["history"]), 1)


if __name__ == "__main__":
    unittest.main()
