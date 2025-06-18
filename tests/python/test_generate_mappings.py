# AOS — tests/python/test_generate_mappings.py — Purpose: Python module 
# AOS — ./tests/python/test_generate_mappings.py
# (c) 2025 RedactedCoder23
# Brief: Source file.
import os
import subprocess
import tempfile
import sys
import unittest

sys.path.insert(0, os.path.abspath(os.path.join(os.path.dirname(__file__), "../..")))

SCRIPT = os.path.join("scripts", "generate_aos_mappings.py")


class GenerateMappingsTest(unittest.TestCase):
    def test_validate(self):
        res = subprocess.run(
            ["python3", SCRIPT, "--validate"],
            capture_output=True,
        )
        self.assertEqual(res.returncode, 0, res.stdout.decode())

    def test_generate_outdir(self):
        with tempfile.TemporaryDirectory() as tmp:
            res = subprocess.run(
                ["python3", SCRIPT, "--outdir", tmp],
                capture_output=True,
            )
            self.assertEqual(res.returncode, 0, res.stderr.decode())
            self.assertTrue(os.path.exists(os.path.join(tmp, "command_map.c")))
            self.assertTrue(os.path.exists(os.path.join(tmp, "commands.c")))
            self.assertTrue(os.path.exists(os.path.join(tmp, "category_defs.h")))


if __name__ == "__main__":
    unittest.main()
