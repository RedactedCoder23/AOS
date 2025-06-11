import os
import subprocess
import tempfile
import json
import socket
import sys
import time
import unittest
import pwd
import grp

sys.path.insert(0, os.path.abspath(os.path.join(os.path.dirname(__file__), "../..")))

BIN = os.path.join("scripts", "ai_cred_manager.py")


class CredManagerTest(unittest.TestCase):
    def setUp(self):
        self.tmp = tempfile.TemporaryDirectory()
        self.env = os.environ.copy()
        self.env["AICRED_DB"] = os.path.join(self.tmp.name, "db")
        self.env["AICRED_KEY"] = os.path.join(self.tmp.name, "key")
        self.env["AICRED_SOCK"] = os.path.join(self.tmp.name, "sock")
        self.env["AICRED_DROP_USER"] = pwd.getpwuid(os.getuid()).pw_name
        self.env["AICRED_DROP_GROUP"] = grp.getgrgid(os.getgid()).gr_name
        self.audit_log = os.path.join(self.tmp.name, "audit.log")
        self.env["AOS_AUDIT_LOG"] = self.audit_log
        # Ensure subprocesses can import the local 'scripts' package
        repo_root = os.path.abspath(os.path.join(os.path.dirname(__file__), "../.."))
        self.env["PYTHONPATH"] = repo_root
        acl = {
            "get": {"users": [os.getuid()]},
            "set": {"users": [os.getuid()]},
            "list": {"users": [os.getuid()]},
            "delete": {"users": [os.getuid()]},
            "rpc": {"users": [os.getuid()]},
        }
        self.env["AICRED_ACL"] = os.path.join(self.tmp.name, "acl.json")
        with open(self.env["AICRED_ACL"], "w") as fh:
            json.dump(acl, fh)

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
        with open(self.audit_log) as fh:
            log = fh.read()
        self.assertIn("cred_set", log)

    def test_rotate(self):
        self.run_cmd(["set", "--service", "anthropic", "--key", "KEY"])
        self.run_cmd(["rotate"])
        out = self.run_cmd(["get", "--service", "anthropic"]).decode().strip()
        self.assertEqual(out, "KEY")

    def test_socket_rpc(self):
        proc = subprocess.Popen([sys.executable, BIN, "daemon"], env=self.env)
        for _ in range(20):
            if os.path.exists(self.env["AICRED_SOCK"]):
                break
            time.sleep(0.1)
        with socket.socket(socket.AF_UNIX, socket.SOCK_STREAM) as s:
            s.connect(self.env["AICRED_SOCK"])
            s.sendall(
                json.dumps(
                    {"method": "set", "params": {"service": "openai", "key": "SK-2"}}
                ).encode()
            )
            s.recv(1024)
        with socket.socket(socket.AF_UNIX, socket.SOCK_STREAM) as s:
            s.connect(self.env["AICRED_SOCK"])
            s.sendall(
                json.dumps({"method": "get", "params": {"service": "openai"}}).encode()
            )
            data = s.recv(1024)
        with socket.socket(socket.AF_UNIX, socket.SOCK_STREAM) as s:
            s.connect(self.env["AICRED_SOCK"])
            s.sendall(json.dumps({"method": "list", "params": {}}).encode())
            lst = s.recv(1024)
        with socket.socket(socket.AF_UNIX, socket.SOCK_STREAM) as s:
            s.connect(self.env["AICRED_SOCK"])
            s.sendall(
                json.dumps(
                    {"method": "delete", "params": {"service": "openai"}}
                ).encode()
            )
            s.recv(1024)
        proc.terminate()
        resp = json.loads(data.decode())
        self.assertEqual(resp.get("result"), "SK-2")
        lres = json.loads(lst.decode())
        self.assertIn("openai", lres.get("result", []))

    def test_permission_denied(self):
        proc = subprocess.Popen([sys.executable, BIN, "daemon"], env=self.env)
        for _ in range(20):
            if os.path.exists(self.env["AICRED_SOCK"]):
                break
            time.sleep(0.1)
        cmd = (
            "import socket, json; s=socket.socket(socket.AF_UNIX);"
            f"s.connect('{self.env['AICRED_SOCK']}');"
            "s.sendall(json.dumps({'method':'get','params':{'service':'x'}}).encode());"
            "print(s.recv(1024).decode())"
        )
        res = subprocess.run(
            ["sudo", "-u", "nobody", "/usr/bin/python3", "-c", cmd],
            env=self.env,
            stdout=subprocess.PIPE,
            stderr=subprocess.STDOUT,
            text=True,
        )
        proc.terminate()
        self.assertTrue(
            "Permission denied" in res.stdout or "PermissionError" in res.stdout
        )
        if os.path.exists(self.audit_log):
            with open(self.audit_log) as fh:
                log = fh.read()
            self.assertIn("denied", log)


if __name__ == "__main__":
    unittest.main()
