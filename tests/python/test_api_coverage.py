# AOS — tests/python/test_api_coverage.py — Purpose: Python module 
# AOS — ./tests/python/test_api_coverage.py
# (c) 2025 RedactedCoder23
# Brief: Source file.
import unittest
from fastapi.testclient import TestClient
from src.api.coverage import app


class CoverageApiTest(unittest.TestCase):
    def test_coverage_keys(self):
        client = TestClient(app)
        resp = client.get("/branches/foo/coverage-history")
        self.assertEqual(resp.status_code, 200)
        data = resp.json()
        for key in ["coverage", "threshold"]:
            self.assertIn(key, data)


if __name__ == "__main__":
    unittest.main()
