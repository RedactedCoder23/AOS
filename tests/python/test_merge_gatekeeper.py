import unittest
import os
import json
import tempfile
from fastapi.testclient import TestClient
from src.api.branches import app


class MergeGatekeeperTest(unittest.TestCase):
    def setUp(self):
        self.tmp = tempfile.TemporaryDirectory()
        self.orig_home = os.environ.get("HOME")
        os.environ["HOME"] = self.tmp.name
        self.orig_cwd = os.getcwd()
        os.chdir(self.tmp.name)
        os.makedirs(os.path.join(self.tmp.name, ".aos/branches/foo"), exist_ok=True)
        with open(os.path.join(self.tmp.name, "config.yml"), "w") as fh:
            fh.write("coverage_threshold: 75.0\n")
        data = {"totals": {"percent_covered": 70.0}}
        with open(
            os.path.join(
                self.tmp.name,
                ".aos/branches/foo/coverage-20250101.json",
            ),
            "w",
        ) as fh:
            json.dump(data, fh)

    def tearDown(self):
        if self.orig_home is not None:
            os.environ["HOME"] = self.orig_home
        os.chdir(self.orig_cwd)
        self.tmp.cleanup()

    def test_merge_block(self):
        client = TestClient(app)
        resp = client.post("/branches/foo/merge")
        self.assertEqual(resp.status_code, 412)
        self.assertIn("threshold", resp.json()["detail"])


if __name__ == "__main__":
    unittest.main()
