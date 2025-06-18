# AOS — ./tests/fixtures/echo_plugin/run_plugin.py
# (c) 2025 RedactedCoder23
# Brief: Source file.
import os
import importlib

entry = os.environ["ENTRY_CLASS"]
prompt = os.environ["PROMPT"]
module_name, class_name = entry.rsplit(".", 1)
cls = getattr(importlib.import_module(module_name), class_name)
print(cls().generate(prompt))
