# AOS — tests/python/test_metrics_failure.py — Purpose: Python module 
# AOS — ./tests/python/test_metrics_failure.py
# (c) 2025 RedactedCoder23
# Brief: Source file.
import unittest
from unittest import mock
from fastapi.testclient import TestClient

from src.api.metrics import app


class MetricsFailureTest(unittest.TestCase):
    def test_metrics_error(self):
        with mock.patch("src.api.metrics.get_stats", side_effect=RuntimeError("boom")):
            client = TestClient(app)
            resp = client.get("/branches/1/metrics")
            self.assertEqual(resp.status_code, 502)
            self.assertEqual(resp.json()["error"], "boom")


if __name__ == "__main__":
    unittest.main()
