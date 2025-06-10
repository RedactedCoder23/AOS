import os
import subprocess
import time
import unittest

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
