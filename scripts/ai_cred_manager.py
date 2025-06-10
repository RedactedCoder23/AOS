#!/usr/bin/env python3
"""Simple credential vault prototype."""
import argparse
import json
import os
import socket
from cryptography.fernet import Fernet

DB_PATH = os.environ.get("AICRED_DB", "/var/lib/ai-cred/creds.db")
KEY_PATH = os.environ.get("AICRED_KEY", "/var/lib/ai-cred/key")
SOCK_PATH = os.environ.get("AICRED_SOCK", "/run/ai-cred.sock")


def _fernet():
    os.makedirs(os.path.dirname(KEY_PATH), exist_ok=True)
    if not os.path.exists(KEY_PATH):
        key = Fernet.generate_key()
        with open(KEY_PATH, "wb") as fh:
            os.chmod(KEY_PATH, 0o600)
            fh.write(key)
    else:
        with open(KEY_PATH, "rb") as fh:
            key = fh.read()
    return Fernet(key)


def _load(db_f):
    if not os.path.exists(DB_PATH):
        return {}
    with open(DB_PATH, "rb") as fh:
        data = db_f.decrypt(fh.read())
        return json.loads(data.decode())


def _save(db_f, data):
    os.makedirs(os.path.dirname(DB_PATH), exist_ok=True)
    with open(DB_PATH, "wb") as fh:
        fh.write(db_f.encrypt(json.dumps(data).encode()))


def cmd_set(args):
    f = _fernet()
    data = _load(f)
    data[args.service] = args.key
    _save(f, data)


def cmd_get(args):
    f = _fernet()
    data = _load(f)
    print(data.get(args.service, ""))


def cmd_list(args):
    f = _fernet()
    data = _load(f)
    print(" ".join(data.keys()))


def cmd_delete(args):
    f = _fernet()
    data = _load(f)
    data.pop(args.service, None)
    _save(f, data)


def cmd_daemon(_args):
    f = _fernet()
    os.makedirs(os.path.dirname(SOCK_PATH), exist_ok=True)
    if os.path.exists(SOCK_PATH):
        os.unlink(SOCK_PATH)
    with socket.socket(socket.AF_UNIX, socket.SOCK_STREAM) as s:
        s.bind(SOCK_PATH)
        os.chmod(SOCK_PATH, 0o660)
        s.listen()
        while True:
            conn, _ = s.accept()
            with conn:
                data = conn.recv(1024)
                try:
                    req = json.loads(data.decode())
                    if req.get("method") == "get":
                        d = _load(f)
                        result = d.get(req["params"]["service"], "")
                        conn.sendall(json.dumps({"result": result}).encode())
                    else:
                        conn.sendall(json.dumps({"error": "bad method"}).encode())
                except Exception as e:  # pragma: no cover - error path
                    conn.sendall(json.dumps({"error": str(e)}).encode())


def main():
    p = argparse.ArgumentParser()
    sub = p.add_subparsers(dest="cmd", required=True)
    s = sub.add_parser("set")
    s.add_argument("--service", required=True)
    s.add_argument("--key", required=True)
    s.set_defaults(func=cmd_set)

    g = sub.add_parser("get")
    g.add_argument("--service", required=True)
    g.set_defaults(func=cmd_get)

    l = sub.add_parser("list")
    l.set_defaults(func=cmd_list)

    d = sub.add_parser("delete")
    d.add_argument("--service", required=True)
    d.set_defaults(func=cmd_delete)

    sub.add_parser("daemon").set_defaults(func=cmd_daemon)

    args = p.parse_args()
    args.func(args)


if __name__ == "__main__":
    main()
