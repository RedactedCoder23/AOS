import unittest

from scripts.ai_providers.loader import get_provider
from scripts.ai_providers.echo_provider import EchoProvider


class LoaderTest(unittest.TestCase):
    def test_load_echo(self):
        prov = get_provider("echo")
        self.assertIsInstance(prov, EchoProvider)

    def test_unknown(self):
        with self.assertRaises(ValueError):
            get_provider("nonexistent")


if __name__ == "__main__":
    unittest.main()
