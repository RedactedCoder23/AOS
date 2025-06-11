import json
import time
import unittest

from scripts.ai_providers import loader


class ProviderHotReloadTest(unittest.TestCase):
    def test_reload_on_change(self):
        orig = json.load(open(loader._CFG, "r", encoding="utf-8"))
        try:
            loader.load_providers()
            self.assertIsNone(loader.get_provider("alias2"))
            data = orig.copy()
            data["alias2"] = {"module": "mock_provider", "class": "MockProvider"}
            with open(loader._CFG, "w", encoding="utf-8") as fh:
                json.dump(data, fh)
            time.sleep(1)
            prov = loader.get_provider("alias2")
            self.assertIsNotNone(prov)
            self.assertEqual(prov.generate("hi"), "mock-response")
        finally:
            with open(loader._CFG, "w", encoding="utf-8") as fh:
                json.dump(orig, fh)
            loader.PROVIDERS.clear()
            loader.load_providers()


if __name__ == "__main__":
    unittest.main()
