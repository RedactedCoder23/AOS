# AOS — ./tests/python/test_plugins_cli.py
# (c) 2025 RedactedCoder23
# Brief: Source file.
import os
import sys
import json
import tempfile
import subprocess
import unittest
from unittest import mock
from pathlib import Path

sys.path.insert(0, os.path.abspath(os.path.join(os.path.dirname(__file__), "../..")))

from scripts import plugins_cli  # noqa: E402
from scripts.ai_providers import loader  # noqa: E402


FIXTURE = (Path(__file__).resolve().parents[1] / "fixtures" / "echo_plugin").resolve()


class PluginsCliTest(unittest.TestCase):
    def _run_add(self, registry, store):
        argv = [
            "plugins_cli.py",
            "add",
            str(FIXTURE),
            "--registry",
            str(registry),
            "--store",
            str(store),
        ]
        with mock.patch.object(sys, "argv", argv), mock.patch.object(
            subprocess, "check_call"
        ) as cc, mock.patch("docker.from_env"):
            plugins_cli.main()
            self.assertTrue(cc.called)

    def test_add_and_load(self):
        with tempfile.TemporaryDirectory() as tmp:
            reg = Path(tmp) / "providers.json"
            reg.write_text("{}")
            store = Path(tmp) / "store"
            store.mkdir()
            self._run_add(reg, store)
            data = json.loads(reg.read_text())
            self.assertIn("echo", data)
            info = data["echo"]
            self.assertIn("image", info)

            cont = mock.Mock()
            cont.wait.return_value = None
            cont.logs.return_value = b"hi"
            client = mock.Mock()
            client.containers.run.return_value = cont
            with mock.patch("docker.from_env", return_value=client):
                orig_cfg = loader._CFG
                orig_mtime = loader._mtime
                orig_map = loader.PROVIDERS.copy()
                try:
                    loader._CFG = str(reg)
                    loader._mtime = None
                    loader.PROVIDERS.clear()
                    prov = loader.get_provider("echo")
                    out = prov.generate("hi")
                    self.assertEqual(out, "hi")
                finally:
                    loader._CFG = orig_cfg
                    loader._mtime = orig_mtime
                    loader.PROVIDERS = orig_map


if __name__ == "__main__":
    unittest.main()
