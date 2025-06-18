# AOS — ./tests/python/test_ai_backend.py
# (c) 2025 RedactedCoder23
# Brief: Source file.
import os
import sys
import unittest
import unittest.mock
import importlib

sys.path.insert(0, os.path.abspath(os.path.join(os.path.dirname(__file__), "../..")))

from scripts import ai_backend  # noqa: E402

SCRIPT = os.path.join("scripts", "ai_backend.py")


class AiBackendTest(unittest.TestCase):

    def test_offline(self):
        mod = importlib.reload(ai_backend)
        os.environ["AOS_AI_OFFLINE"] = "1"
        sys.argv = ["ai_backend.py", "hi"]
        try:
            self.assertEqual(mod.main(), 0)
        finally:
            del os.environ["AOS_AI_OFFLINE"]

    def test_success(self):
        mod = importlib.reload(ai_backend)
        os.environ["AOS_AI_PROVIDER"] = "echo"
        sys.argv = ["ai_backend.py", "hello"]
        try:
            self.assertEqual(mod.main(), 0)
        finally:
            del os.environ["AOS_AI_PROVIDER"]

    def test_no_args(self):
        mod = importlib.reload(ai_backend)
        sys.argv = ["ai_backend.py"]
        self.assertEqual(mod.main(), 1)


if __name__ == "__main__":
    unittest.main()
