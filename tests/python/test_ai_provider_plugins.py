import os
import sys
import unittest

sys.path.insert(0, os.path.abspath(os.path.join(os.path.dirname(__file__), "../..")))

from scripts.ai_providers.loader import get_provider  # noqa: E402


class ProviderPluginTest(unittest.TestCase):
    def test_discovery_and_generate(self):
        openai = get_provider("openai")
        llama = get_provider("local-llama")
        self.assertEqual(openai.generate("hi"), "hi")
        self.assertEqual(llama.generate("hi"), "mock-response")


if __name__ == "__main__":
    unittest.main()
