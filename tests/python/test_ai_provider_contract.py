# AOS — tests/python/test_ai_provider_contract.py — Purpose: Python module 
# AOS — ./tests/python/test_ai_provider_contract.py
# (c) 2025 RedactedCoder23
# Brief: Source file.
import inspect
import importlib
import os
import pkgutil
import unittest

from scripts.ai_providers.base import AIProvider


class ProviderContractTest(unittest.TestCase):
    def test_generate_overridden(self):
        provider_path = os.path.join(
            os.path.dirname(__file__), "..", "..", "scripts", "ai_providers"
        )
        modules = []
        for _, modname, _ in pkgutil.iter_modules([provider_path]):
            if modname.startswith("__"):
                continue
            try:
                modules.append(
                    importlib.import_module(f"scripts.ai_providers.{modname}")
                )
            except Exception:
                continue
        subclasses = set()
        for mod in modules:
            for name, obj in inspect.getmembers(mod, inspect.isclass):
                if issubclass(obj, AIProvider) and obj is not AIProvider:
                    subclasses.add(obj)
        for cls in subclasses:
            self.assertIsNot(
                cls.generate,
                AIProvider.generate,
                f"{cls.__name__} must override generate()",
            )


if __name__ == "__main__":
    unittest.main()
