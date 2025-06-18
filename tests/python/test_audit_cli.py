# AOS — tests/python/test_audit_cli.py — Purpose: Python module 
# AOS — ./tests/python/test_audit_cli.py
# (c) 2025 RedactedCoder23
# Brief: Source file.
import os
import subprocess
import sys
import tempfile
import json
import unittest

sys.path.insert(0, os.path.abspath(os.path.join(os.path.dirname(__file__), "../..")))

SCRIPT = os.path.join("scripts", "aos_audit.py")


class AuditCliFilters(unittest.TestCase):
    def setUp(self):
        self.tmp = tempfile.NamedTemporaryFile(delete=False)
        self.log_path = self.tmp.name
        lines = [
            json.dumps(
                {
                    "user": "alice",
                    "action": "branch_create",
                    "resource": "branch:1",
                    "result": "success",
                }
            ),
            json.dumps(
                {
                    "user": "bob",
                    "action": "branch_delete",
                    "resource": "branch:2",
                    "result": "fail",
                }
            ),
        ]
        with open(self.log_path, "w") as fh:
            for line in lines:
                fh.write(line + "\n")

    def tearDown(self):
        os.unlink(self.log_path)

    def _run(self, *args):
        cmd = [sys.executable, SCRIPT, "show", "--file", self.log_path] + list(args)
        return subprocess.run(cmd, capture_output=True, text=True)

    def test_filter_user(self):
        res = self._run("--user", "alice")
        self.assertEqual(res.returncode, 0)
        data = json.loads(res.stdout.strip())
        self.assertEqual(data["user"], "alice")

    def test_filter_action(self):
        res = self._run("--action", "branch_delete")
        self.assertEqual(res.returncode, 0)
        data = json.loads(res.stdout.strip())
        self.assertEqual(data["action"], "branch_delete")

    def test_filter_resource(self):
        res = self._run("--resource", "branch:2")
        self.assertEqual(res.returncode, 0)
        data = json.loads(res.stdout.strip())
        self.assertEqual(data["resource"], "branch:2")


if __name__ == "__main__":
    unittest.main()
