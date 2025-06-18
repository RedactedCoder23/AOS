# AOS — tests/python/test_security_branch_owner.py — Purpose: Python module 
# AOS — ./tests/python/test_security_branch_owner.py
# (c) 2025 RedactedCoder23
# Brief: Source file.
import os
import sys
import unittest
from unittest import mock

sys.path.insert(0, os.path.abspath(os.path.join(os.path.dirname(__file__), "../..")))
from scripts import branch_ui  # noqa: E402


class BranchOwnerTest(unittest.TestCase):
    def setUp(self):
        branch_ui.app.config["TESTING"] = True
        self.client = branch_ui.app.test_client()
        branch_ui.service.branches.clear()

    @mock.patch("scripts.branch_ui.subprocess.Popen")
    @mock.patch("scripts.branch_ui.subprocess.check_output", return_value="")
    def test_syscall_owner_enforced(self, _chk, popen):
        popen.return_value = mock.Mock(pid=1)
        bid = branch_ui.service.create(uid=1000)
        res = branch_ui.service.merge(bid, uid=1001)
        self.assertIn("error", res)

    def test_http_owner_enforced(self):
        branch_ui.service.branches[1] = {
            "pid": 1,
            "sock": "s",
            "status": "RUNNING",
            "owner_uid": 1000,
        }
        res = self.client.post(
            "/branches/1/snapshot", environ_base={"REMOTE_USER": "1001"}
        )
        self.assertEqual(res.status_code, 403)
        self.assertIn("permission", res.get_data(as_text=True))


if __name__ == "__main__":
    unittest.main()
