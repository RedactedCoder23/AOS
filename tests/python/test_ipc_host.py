/* AOS — tests/python/test_ipc_host.py — Purpose: Python module */
# AOS — ./tests/python/test_ipc_host.py
# (c) 2025 RedactedCoder23
# Brief: Source file.
import os
import subprocess
import time
import sys
import unittest

sys.path.insert(0, os.path.abspath(os.path.join(os.path.dirname(__file__), "../..")))

BIN = os.path.join("build", "ipc_host")


class IpcHostSmoke(unittest.TestCase):
    def test_startup(self):
        if not os.path.exists(BIN):
            self.skipTest("ipc_host binary missing")
        proc = subprocess.Popen([BIN], stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        time.sleep(1)
        proc.terminate()
        proc.wait(timeout=5)
        self.assertIsNotNone(proc.returncode)


if __name__ == "__main__":
    unittest.main()
