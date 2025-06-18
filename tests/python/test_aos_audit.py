# AOS — ./tests/python/test_aos_audit.py
# (c) 2025 RedactedCoder23
# Brief: Source file.
import os
import subprocess
import tempfile
import json
import sys
import unittest

sys.path.insert(0, os.path.abspath(os.path.join(os.path.dirname(__file__), "../..")))

SCRIPT = os.path.join("scripts", "aos_audit.py")


class AosAuditTest(unittest.TestCase):
    def setUp(self):
        self.tmp = tempfile.NamedTemporaryFile(delete=False)
        self.log_path = self.tmp.name
        lines = [
            json.dumps(
                {
                    "action": "branch_create",
                    "user": "alice",
                    "resource": "branch:1",
                    "result": "success",
                }
            ),
            "bad line",
            json.dumps(
                {
                    "action": "branch_delete",
                    "user": "bob",
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

    def test_filter_action(self):
        res = subprocess.run(
            [
                "python3",
                SCRIPT,
                "show",
                "--file",
                self.log_path,
                "--filter",
                "action:branch_create",
            ],
            capture_output=True,
            text=True,
        )
        self.assertEqual(res.returncode, 0)
        out = json.loads(res.stdout)
        self.assertEqual(out["action"], "branch_create")


if __name__ == "__main__":
    unittest.main()
