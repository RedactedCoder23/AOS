# AOS — ./tests/python/test_auth_login.py
# (c) 2025 RedactedCoder23
# Brief: Source file.
import unittest
import sys
import os

sys.path.insert(0, os.path.abspath(os.path.join(os.path.dirname(__file__), "../..")))
from scripts import branch_ui  # noqa: E402


class AuthLoginTest(unittest.TestCase):
    def setUp(self):
        branch_ui.app.config["TESTING"] = True
        self.client = branch_ui.app.test_client()

    def test_login_token(self):
        res = self.client.post(
            "/auth/login", json={"username": "test", "password": "test"}
        )
        self.assertEqual(res.status_code, 200)
        data = res.get_json()
        self.assertIn("access", data)
        self.assertIn("refresh", data)


if __name__ == "__main__":
    unittest.main()
