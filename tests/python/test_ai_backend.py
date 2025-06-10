import os
import sys
import unittest
from types import SimpleNamespace
import unittest.mock
import importlib

SCRIPT = os.path.join("scripts", "ai_backend.py")


class AiBackendTest(unittest.TestCase):
    def setUp(self):
        path = os.path.join(os.getcwd(), "scripts")
        if path not in sys.path:
            sys.path.insert(0, path)

    def test_offline(self):
        mod = importlib.reload(importlib.import_module("ai_backend"))
        os.environ["AOS_AI_OFFLINE"] = "1"
        sys.argv = ["ai_backend.py", "hi"]
        try:
            self.assertEqual(mod.main(), 0)
        finally:
            del os.environ["AOS_AI_OFFLINE"]

    def test_success(self):
        mod = importlib.reload(importlib.import_module("ai_backend"))

        def fake_get(_svc):
            return "k"

        fake_openai = SimpleNamespace(
            OpenAI=lambda api_key: SimpleNamespace(
                chat=SimpleNamespace(
                    completions=SimpleNamespace(
                        create=lambda **_: SimpleNamespace(
                            choices=[
                                SimpleNamespace(message=SimpleNamespace(content="ok"))
                            ]
                        )
                    )
                )
            )
        )

        with unittest.mock.patch.object(
            mod, "get_api_key", fake_get
        ), unittest.mock.patch.object(mod, "openai", fake_openai):
            sys.argv = ["ai_backend.py", "hi"]
            self.assertEqual(mod.main(), 0)

    def test_no_args(self):
        mod = importlib.reload(importlib.import_module("ai_backend"))
        sys.argv = ["ai_backend.py"]
        self.assertEqual(mod.main(), 1)

    def test_openai_missing(self):
        mod = importlib.reload(importlib.import_module("ai_backend"))

        def fake_get(_):
            return "k"

        with unittest.mock.patch.object(mod, "get_api_key", fake_get):
            mod.openai = None
            sys.argv = ["ai_backend.py", "hi"]
            self.assertEqual(mod.main(), 2)

    def test_openai_error(self):
        mod = importlib.reload(importlib.import_module("ai_backend"))

        def fake_get(_svc):
            return "k"

        class BadClient:
            def __init__(self, api_key):
                pass

            class chat:
                class completions:
                    @staticmethod
                    def create(**_):
                        raise RuntimeError("boom")

        fake_openai = SimpleNamespace(OpenAI=BadClient)

        with unittest.mock.patch.object(
            mod, "get_api_key", fake_get
        ), unittest.mock.patch.object(mod, "openai", fake_openai):
            sys.argv = ["ai_backend.py", "hi"]
            self.assertEqual(mod.main(), 3)

    def test_missing_key(self):
        mod = importlib.reload(importlib.import_module("ai_backend"))

        def bad_get(_svc):
            raise KeyError("openai")

        with unittest.mock.patch.object(mod, "get_api_key", bad_get):
            sys.argv = ["ai_backend.py", "hi"]
            self.assertEqual(mod.main(), 1)


if __name__ == "__main__":
    unittest.main()
