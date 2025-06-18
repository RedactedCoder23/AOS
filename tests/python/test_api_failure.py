/* AOS — tests/python/test_api_failure.py — Purpose: Python module */
# AOS — ./tests/python/test_api_failure.py
# (c) 2025 RedactedCoder23
# Brief: Source file.
import unittest
from unittest import mock
from fastapi.testclient import TestClient

from src.api.metrics import app
import scripts.agent_orchestrator as ao


class MetricsCircuitBreakerTest(unittest.TestCase):
    def test_cached_on_failure(self):
        ao._stats_cache.clear()
        ao._stats_cache["1"] = {"v": 1}
        with mock.patch("src.api.metrics.get_stats", side_effect=RuntimeError("boom")):
            client = TestClient(app)
            fail = client.get("/branches/1/metrics")
            self.assertEqual(fail.status_code, 502)
            self.assertEqual(fail.json(), {"error": "boom", "data": {"v": 1}})


if __name__ == "__main__":
    unittest.main()
