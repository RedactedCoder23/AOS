import unittest
import sys
import os

sys.path.insert(0, os.path.abspath(os.path.join(os.path.dirname(__file__), "../..")))
from scripts import branch_ui  # noqa: E402


class BranchPagingTest(unittest.TestCase):
    def setUp(self):
        branch_ui.app.config["TESTING"] = True
        self.client = branch_ui.app.test_client()
        branch_ui.service.branches.clear()
        branch_ui.service.branches[1] = {"pid": 1, "status": "RUNNING", "owner_uid": 0}
        branch_ui.service.branches[2] = {"pid": 2, "status": "RUNNING", "owner_uid": 0}

    def test_paged_list(self):
        res = self.client.get("/api/branches?limit=1")
        self.assertEqual(res.status_code, 200)
        data = res.get_json()
        self.assertEqual(len(data["items"]), 1)
        self.assertIn("next", data)


if __name__ == "__main__":
    unittest.main()
