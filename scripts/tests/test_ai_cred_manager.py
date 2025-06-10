import os
import subprocess
import tempfile
import json
import socket
import sys
import time
import unittest

BIN = os.path.join("scripts", "ai_cred_manager.py")


class CredManagerTest(unittest.TestCase):
    def setUp(self):
        self.tmp = tempfile.TemporaryDirectory()
        self.env = os.environ.copy()
        self.env["AICRED_DB"] = os.path.join(self.tmp.name, "db")
        self.env["AICRED_KEY"] = os.path.join(self.tmp.name, "key")
        self.env["AICRED_SOCK"] = os.path.join(self.tmp.name, "sock")

    def tearDown(self):
        self.tmp.cleanup()

    def run_cmd(self, args):
        return subprocess.check_output([sys.executable, BIN] + args, env=self.env)

    def test_set_get_list_delete(self):
        self.run_cmd(["set", "--service", "openai", "--key", "SK-1"])
        out = self.run_cmd(["get", "--service", "openai"]).decode().strip()
        self.assertEqual(out, "SK-1")
        out = self.run_cmd(["list"]).decode().strip()
        self.assertIn("openai", out)
        self.run_cmd(["delete", "--service", "openai"])
        out = self.run_cmd(["list"]).decode().strip()
        self.assertEqual(out, "")

    def test_socket_rpc(self):
        proc = subprocess.Popen([sys.executable, BIN, "daemon"], env=self.env)
        time.sleep(0.2)
        self.run_cmd(["set", "--service", "openai", "--key", "SK-2"])
        with socket.socket(socket.AF_UNIX, socket.SOCK_STREAM) as s:
            s.connect(self.env["AICRED_SOCK"])
            s.sendall(
                json.dumps({"method": "get", "params": {"service": "openai"}}).encode()
            )
            data = s.recv(1024)
        proc.terminate()
        resp = json.loads(data.decode())
        self.assertEqual(resp.get("result"), "SK-2")


if __name__ == "__main__":
    unittest.main()
