# AOS — tests/python/test_api_coverage_live.py — Purpose: Python module 
# AOS — ./tests/python/test_api_coverage_live.py
# (c) 2025 RedactedCoder23
# Brief: Source file.
import unittest
import os
import json
import tempfile
from fastapi.testclient import TestClient
from src.api.coverage import app
from scripts import coverage_recorder
import sys


class CoverageLiveAPITest(unittest.TestCase):
    def setUp(self):
        self.tmp = tempfile.TemporaryDirectory()
        self.orig_home = os.environ.get("HOME")
        os.environ["HOME"] = self.tmp.name
        self.orig_cwd = os.getcwd()
        os.chdir(self.tmp.name)
        with open(os.path.join(self.tmp.name, "config.yml"), "w") as fh:
            fh.write("coverage_threshold: 75.0\n")
        data = {"totals": {"percent_covered": 83.4, "percent_branches": 79.1}}
        with open("coverage.json", "w") as fh:
            json.dump(data, fh)
        self.orig_argv = list(sys.argv)
        sys.argv = ["coverage_recorder.py", "0"]
        coverage_recorder.main()

    def tearDown(self):
        if self.orig_home is not None:
            os.environ["HOME"] = self.orig_home
        os.chdir(self.orig_cwd)
        self.tmp.cleanup()
        if os.path.exists("coverage.json"):
            os.unlink("coverage.json")
        sys.argv = self.orig_argv

    def test_history_endpoint(self):
        client = TestClient(app)
        resp = client.get("/branches/0/coverage-history")
        self.assertEqual(resp.status_code, 200)
        data = resp.json()
        self.assertTrue(len(data["coverage"]) >= 1)
        self.assertEqual(data["threshold"], 75.0)


if __name__ == "__main__":
    unittest.main()
