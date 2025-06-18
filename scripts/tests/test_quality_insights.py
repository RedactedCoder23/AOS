# AOS — scripts/tests/test_quality_insights.py — Purpose: Python module 
# AOS — ./scripts/tests/test_quality_insights.py
# (c) 2025 RedactedCoder23
# Brief: Source file.
import os
import json
import tempfile
import unittest
import sys

sys.path.insert(0, os.path.abspath(os.path.join(os.path.dirname(__file__), "../..")))

from scripts import branch_ui  # noqa: E402


class QualityInsightsTest(unittest.TestCase):
    def setUp(self):
        branch_ui.app.config["TESTING"] = True
        self.client = branch_ui.app.test_client()
        branch_ui.service.branches.clear()
        self.tmp = tempfile.TemporaryDirectory()
        self.orig = os.getcwd()
        os.chdir(self.tmp.name)

    def tearDown(self):
        os.chdir(self.orig)
        self.tmp.cleanup()
        if os.path.exists("rules.yaml"):
            os.unlink("rules.yaml")

    def test_merge_rejects_low_coverage(self):
        os.makedirs("branches/1", exist_ok=True)
        with open("branches/1/coverage.json", "w") as fh:
            json.dump({"totals": {"percent_covered": 75}}, fh)
        with open("rules.yaml", "w") as fh:
            fh.write("coverage_threshold: 80\n")
        branch_ui.service.branches[1] = {
            "pid": 1,
            "sock": "s",
            "status": "RUNNING",
            "owner_uid": os.getuid(),
        }
        res = self.client.post(
            "/branches/1/merge",
            environ_base={"REMOTE_USER": str(os.getuid())},
        )
        self.assertEqual(res.status_code, 422)
        data = json.loads(res.data)
        self.assertEqual(data["code"], 422)
        self.assertIn("coverage below", data["error"])

    def test_coverage_history(self):
        os.makedirs("branches/1", exist_ok=True)
        with open("branches/1/history.json", "w") as fh:
            json.dump([70, 80, 90], fh)
        res = self.client.get("/branches/1/coverage-history")
        self.assertEqual(res.status_code, 200)
        data = json.loads(res.data)
        self.assertEqual(data, [70, 80, 90])


if __name__ == "__main__":
    unittest.main()
