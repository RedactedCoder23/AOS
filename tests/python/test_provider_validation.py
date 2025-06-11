import unittest
import tempfile
import json
import os
import sys
from scripts.ai_providers import loader
from scripts.ai_providers.base import AIProvider
import importlib


class ProviderValidationTest(unittest.TestCase):
    def test_missing_generate(self):
        tmp = tempfile.TemporaryDirectory()
        sys.path.insert(0, tmp.name)
        with open(os.path.join(tmp.name, "badprov.py"), "w") as fh:
            fh.write("from scripts.ai_providers.base import AIProvider\nclass Bad(AIProvider):\n    pass\n")
        cfg = os.path.join(tmp.name, "providers.json")
        with open(cfg, "w") as fh:
            json.dump({"bad": "badprov.Bad"}, fh)
        orig_cfg = loader._CFG
        orig_mtime = loader._mtime
        orig_map = loader.PROVIDERS.copy()
        loader._CFG = cfg
        loader._mtime = None
        loader.PROVIDERS.clear()
        with self.assertRaises(loader.ProviderImplementationError):
            loader.load_providers()
        loader._CFG = orig_cfg
        loader._mtime = orig_mtime
        loader.PROVIDERS = orig_map
        sys.path.remove(tmp.name)
        tmp.cleanup()


if __name__ == "__main__":
    unittest.main()
