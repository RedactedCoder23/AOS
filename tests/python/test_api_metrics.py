import unittest
from fastapi.testclient import TestClient
from src.api.metrics import app


class MetricsApiTest(unittest.TestCase):
    def test_metrics_keys(self):
        client = TestClient(app)
        resp = client.get("/branches/foo/metrics")
        self.assertEqual(resp.status_code, 200)
        data = resp.json()
        for key in ["pending_tasks", "cpu_pct", "mem_pct", "history"]:
            self.assertIn(key, data)


if __name__ == "__main__":
    unittest.main()
