# AOS — tests/python/test_provider_contract.py — Purpose: Python module 
# AOS — ./tests/python/test_provider_contract.py
# (c) 2025 RedactedCoder23
# Brief: Source file.
import importlib
import inspect
import os
import pkgutil
import unittest

from scripts.ai_providers.base import AIProvider


class ProviderContract(unittest.TestCase):
    def test_generate_overridden(self):
        provider_path = os.path.join(
            os.path.dirname(__file__), "..", "..", "scripts", "ai_providers"
        )
        modules = []
        for _, modname, _ in pkgutil.iter_modules([provider_path]):
            try:
                modules.append(
                    importlib.import_module(f"scripts.ai_providers.{modname}")
                )
            except Exception:
                continue
        missing = []
        for mod in modules:
            for _, obj in inspect.getmembers(mod, inspect.isclass):
                if issubclass(obj, AIProvider) and obj is not AIProvider:
                    if obj.generate is AIProvider.generate:
                        missing.append(obj.__name__)
        self.assertFalse(missing, f"providers missing generate(): {', '.join(missing)}")


if __name__ == "__main__":
    unittest.main()
