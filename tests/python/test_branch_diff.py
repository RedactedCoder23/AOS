/* AOS — tests/python/test_branch_diff.py — Purpose: Python module */
# AOS — ./tests/python/test_branch_diff.py
# (c) 2025 RedactedCoder23
# Brief: Source file.
import os
import subprocess
import tempfile
import unittest
from fastapi.testclient import TestClient
from src.api.branch_diff import app


class BranchDiffAPITest(unittest.TestCase):
    def setUp(self):
        self.tmp = tempfile.TemporaryDirectory()
        self.orig_cwd = os.getcwd()
        os.chdir(self.tmp.name)
        subprocess.run(["git", "init"], check=True)
        subprocess.run(["git", "config", "user.email", "a@b"], check=True)
        subprocess.run(["git", "config", "user.name", "tester"], check=True)
        with open("f.txt", "w") as fh:
            fh.write("base\n")
        subprocess.run(["git", "add", "f.txt"], check=True)
        subprocess.run(["git", "commit", "-m", "base"], check=True)
        subprocess.run(["git", "checkout", "-b", "main"], check=True)
        with open("f.txt", "a") as fh:
            fh.write("prev\n")
        subprocess.run(["git", "commit", "-am", "prev"], check=True)
        with open("f.txt", "a") as fh:
            fh.write("head\n")
        subprocess.run(["git", "commit", "-am", "head"], check=True)

    def tearDown(self):
        os.chdir(self.orig_cwd)
        self.tmp.cleanup()

    def test_diff_endpoint(self):
        client = TestClient(app)
        resp = client.get("/branches/main/diff")
        self.assertEqual(resp.status_code, 200)
        diff = resp.json()["diff"]
        self.assertIn("head", diff)
        self.assertIn("prev", diff)


if __name__ == "__main__":
    unittest.main()
