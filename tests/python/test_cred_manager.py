# AOS — tests/python/test_cred_manager.py — Purpose: Python module 
# AOS — ./tests/python/test_cred_manager.py
# (c) 2025 RedactedCoder23
# Brief: Source file.
import os
import tempfile
import unittest
from unittest import mock

from scripts import ai_cred_manager as cm


class CredManagerTest(unittest.TestCase):
    def test_load_roundtrip(self):
        with tempfile.TemporaryDirectory() as tmp:
            db = os.path.join(tmp, "creds.db")
            key = os.path.join(tmp, "master.key")
            with mock.patch.object(cm, "DB_PATH", db), mock.patch.object(
                cm, "KEY_PATH", key
            ):
                f = cm._fernet()
                cm._save(f, {"svc": "token"})
                data = cm._load(f)
                self.assertEqual(data, {"svc": "token"})

    def test_missing_vault_raises(self):
        with tempfile.TemporaryDirectory() as tmp:
            db = os.path.join(tmp, "creds.db")  # not created
            key = os.path.join(tmp, "master.key")
            with mock.patch.object(cm, "DB_PATH", db), mock.patch.object(
                cm, "KEY_PATH", key
            ), mock.patch.object(cm.aos_audit, "warn") as warn:
                f = cm._fernet()
                with self.assertRaises(cm.CredentialsUnavailableError):
                    cm._load(f)
                warn.assert_called_once()


if __name__ == "__main__":
    unittest.main()
