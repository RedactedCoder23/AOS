import os
import sys
import json
import importlib
import unittest

sys.path.insert(0, os.path.abspath(os.path.join(os.path.dirname(__file__), "../..")))

from scripts import ai_backend, agent_orchestrator  # noqa: E402


class ProviderPluginTest(unittest.TestCase):
    def test_discovery_and_generate(self):
        # reload modules to pick up providers.json
        importlib.reload(ai_backend)
        importlib.reload(agent_orchestrator)
        ai_backend._load_providers()
        agent_orchestrator._load_providers()
        provs = ai_backend.PROVIDERS
        self.assertIn("openai", provs)
        self.assertIn("local-llama", provs)
        self.assertEqual(provs["openai"].generate("hi"), "hi")
        self.assertEqual(provs["local-llama"].generate("hi"), "mock-response")


if __name__ == "__main__":
    unittest.main()
