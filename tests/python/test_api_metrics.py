/* AOS — tests/python/test_api_metrics.py — Purpose: Python module */
# AOS — ./tests/python/test_api_metrics.py
# (c) 2025 RedactedCoder23
# Brief: Source file.
import unittest
from fastapi.testclient import TestClient
from src.api.metrics import app


class MetricsApiTest(unittest.TestCase):
    def test_metrics_keys(self):
        client = TestClient(app)
        resp = client.get("/branches/foo/metrics")
        self.assertEqual(resp.status_code, 404)


if __name__ == "__main__":
    unittest.main()
