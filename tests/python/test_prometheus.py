import unittest
from fastapi.testclient import TestClient

from src.observability.prom_exporter import app


class PrometheusExporterTest(unittest.TestCase):
    def test_metrics_keys(self):
        client = TestClient(app)
        res = client.get("/metrics")
        self.assertEqual(res.status_code, 200)
        data = {}
        for line in res.text.splitlines():
            if line.startswith("#") or not line.strip():
                continue
            k, _ = line.split(None, 1)
            data[k] = True
        self.assertIn("queue_len", data)
        self.assertIn("job_latency", data)
        self.assertIn("worker_count", data)


if __name__ == "__main__":
    unittest.main()
