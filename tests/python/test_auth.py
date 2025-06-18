/* AOS — tests/python/test_auth.py — Purpose: Python module */
# AOS — ./tests/python/test_auth.py
# (c) 2025 RedactedCoder23
# Brief: Source file.
import unittest
import sys
import os
import jwt

sys.path.insert(0, os.path.abspath(os.path.join(os.path.dirname(__file__), "../..")))
from scripts import branch_ui  # noqa: E402


class AuthFlowTest(unittest.TestCase):
    def setUp(self):
        branch_ui.app.config["TESTING"] = True
        self.client = branch_ui.app.test_client()

    def login(self, user="test", pwd="test"):
        return self.client.post(
            "/auth/login", json={"username": user, "password": pwd}
        ).get_json()

    def test_refresh(self):
        data = self.login("test", "test")
        res = self.client.post("/auth/refresh", json={"token": data["refresh"]})
        self.assertEqual(res.status_code, 200)
        new_data = res.get_json()
        self.assertIn("access", new_data)
        self.assertNotEqual(data["access"], new_data["access"])
        payload = jwt.decode(
            new_data["access"], branch_ui.JWT_SECRET, algorithms=["HS256"]
        )
        self.assertEqual(payload["role"], "admin")

    def test_plugin_install_rbac(self):
        admin = self.login("test", "test")
        viewer = self.login("viewer", "viewer")

        res = self.client.post(
            "/plugins/install",
            json={"repo": "x"},
            headers={"Authorization": f"Bearer {viewer['access']}"},
        )
        self.assertEqual(res.status_code, 403)

        res = self.client.post(
            "/plugins/install",
            json={"repo": "x"},
            headers={"Authorization": f"Bearer {admin['access']}"},
        )
        self.assertEqual(res.status_code, 200)


if __name__ == "__main__":
    unittest.main()
