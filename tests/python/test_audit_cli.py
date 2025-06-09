import os
import subprocess
import unittest

SCRIPT = os.path.join("scripts", "audit_cli.py")


class AuditCliTest(unittest.TestCase):
    def setUp(self):
        if os.path.exists("AOS-CHECKLIST.log"):
            os.remove("AOS-CHECKLIST.log")

    def test_clear_and_show(self):
        res = subprocess.run(
            ["python3", SCRIPT, "clear"], capture_output=True, text=True
        )
        self.assertEqual(res.returncode, 0)
        res = subprocess.run(["python3", SCRIPT, "log"], capture_output=True, text=True)
        self.assertEqual(res.stdout.strip(), "")

    def test_rollback_invalid(self):
        res = subprocess.run(
            [
                "python3",
                SCRIPT,
                "rollback",
                "deadbeef",
            ],
            capture_output=True,
            text=True,
        )
        self.assertNotEqual(res.returncode, 0)
        self.assertIn("rollback failed", res.stderr)


if __name__ == "__main__":
    unittest.main()
