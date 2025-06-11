import os
import sys
import unittest

sys.path.insert(0, os.path.abspath(os.path.join(os.path.dirname(__file__), "../.")))

<<<<<< codex/add-echo-and-openai-provider-plugins
from scripts import ai_backend  # noqa: E402
from scripts.ai_providers.openai_provider import OpenAIProvider  # noqa: E402


class ProviderPluginTest(unittest.TestCase):
    def test_get_provider(self):
        # reload to pick up providers.json
        importlib.reload(ai_backend)
        prov = ai_backend.get_provider("echo")
        assert prov is not None
        self.assertEqual(prov.generate("hi"), "hi")
        self.assertIsInstance(ai_backend.get_provider("openai"), OpenAIProvider)
=======
from scripts.ai_providers.loader import get_provider  # noqa: E402


class ProviderPluginTest(unittest.TestCase):
    def test_discovery_and_generate(self):
        openai = get_provider("openai")
        llama = get_provider("local-llama")
        self.assertEqual(openai.generate("hi"), "hi")
        self.assertEqual(llama.generate("hi"), "mock-response")
>>>>>> main


if __name__ == "__main__":
    unittest.main()
