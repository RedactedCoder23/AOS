# AOS — tests/python/conftest.py — Purpose: test fixtures
# (c) 2025 RedactedCoder23
# Brief: Source file.
import os
import sys

# Ensure the repository root is on the path so `import scripts.*` works
sys.path.insert(
    0, os.path.abspath(os.path.join(os.path.dirname(__file__), "../.."))
)  # noqa: E402

import importlib  # noqa: E402
import inspect  # noqa: E402
import pkgutil  # noqa: E402
import warnings  # noqa: E402
from scripts.ai_providers.base import AIProvider  # noqa: E402


def pytest_configure(config):
    provider_path = os.path.join(
        os.path.dirname(__file__), "..", "..", "scripts", "ai_providers"
    )
    modules = []
    for _, modname, _ in pkgutil.iter_modules([provider_path]):
        try:
            modules.append(importlib.import_module(f"scripts.ai_providers.{modname}"))
        except Exception:
            continue
    missing = []
    for mod in modules:
        for _, obj in inspect.getmembers(mod, inspect.isclass):
            if issubclass(obj, AIProvider) and obj is not AIProvider:
                if obj.generate is AIProvider.generate:
                    missing.append(obj.__name__)
    if missing:
        warnings.warn(f"missing generate() in: {', '.join(missing)}")
