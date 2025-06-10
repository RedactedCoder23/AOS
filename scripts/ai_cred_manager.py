#!/usr/bin/env python3
"""Simple credential vault prototype."""
import argparse
import json
import os
import socket
import struct
import sys
from cryptography.fernet import Fernet

try:
    import keyring
except Exception:  # pragma: no cover - optional
    keyring = None

DB_PATH = os.environ.get("AICRED_DB", "/var/lib/ai-cred/creds.db")
KEY_PATH = os.environ.get("AICRED_KEY", "/etc/ai-cred/master.key")
SOCK_PATH = os.environ.get("AICRED_SOCK", "/run/ai-cred.sock")
DROP_USER = os.environ.get("AICRED_DROP_USER", "branch-manager")
DROP_GROUP = os.environ.get("AICRED_DROP_GROUP", "branchd")


def _fernet():
    key = None
    if keyring is not None:
        try:
            val = keyring.get_password("ai-cred", "master")
            if val:
                key = val.encode()
        except Exception:  # pragma: no cover - keyring failures
            pass

    if key is None and os.path.exists(KEY_PATH):
        with open(KEY_PATH, "rb") as fh:
            key = fh.read()

    if key is None:
        key = Fernet.generate_key()
        os.makedirs(os.path.dirname(KEY_PATH), exist_ok=True)
        with open(KEY_PATH, "wb") as fh:
            os.fchmod(fh.fileno(), 0o400)
            fh.write(key)
        if keyring is not None:
            try:
                keyring.set_password("ai-cred", "master", key.decode())
            except Exception:
                pass

    return Fernet(key)


def _health_check():
    ok = False
    if keyring is not None:
        try:
            keyring.get_password("ai-cred", "master")
            ok = True
        except Exception:
            pass
    if not ok and os.path.exists(KEY_PATH) and os.access(KEY_PATH, os.R_OK):
        ok = True
    if not ok:
        print(
            "WARNING: master key not accessible; a new key will be generated",
            file=sys.stderr,
        )


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


def cmd_rotate(_args):
    old_f = _fernet()
    data = _load(old_f)
    new_key = Fernet.generate_key()
    os.makedirs(os.path.dirname(KEY_PATH), exist_ok=True)
    with open(KEY_PATH, "wb") as fh:
        os.fchmod(fh.fileno(), 0o400)
        fh.write(new_key)
    if keyring is not None:
        try:
            keyring.set_password("ai-cred", "master", new_key.decode())
        except Exception:
            pass
    new_f = Fernet(new_key)
    _save(new_f, data)


def cmd_daemon(_args):
    _health_check()
    f = _fernet()
    os.makedirs(os.path.dirname(SOCK_PATH), exist_ok=True)
    if os.path.exists(SOCK_PATH):
        os.unlink(SOCK_PATH)
    with socket.socket(socket.AF_UNIX, socket.SOCK_STREAM) as s:
        s.bind(SOCK_PATH)
        try:
            import grp

            gid = grp.getgrnam(DROP_GROUP).gr_gid
        except Exception:
            gid = os.getgid()
        os.chown(SOCK_PATH, 0, gid)
        os.chmod(SOCK_PATH, 0o660)
        if os.getuid() == 0:
            try:
                import pwd

                uid = pwd.getpwnam(DROP_USER).pw_uid
                os.setgid(gid)
                os.setuid(uid)
            except Exception:
                pass
        s.listen()
        while True:
            conn, _ = s.accept()
            with conn:
                try:
                    creds = conn.getsockopt(
                        socket.SOL_SOCKET, socket.SO_PEERCRED, struct.calcsize("3i")
                    )
                    _, uid, _ = struct.unpack("3i", creds)
                except Exception:  # pragma: no cover - platform fallback
                    uid = 0
                allowed = {0}
                try:
                    import pwd

                    allowed.add(pwd.getpwnam(DROP_USER).pw_uid)
                except Exception:
                    pass
                data = conn.recv(4096)
                if uid not in allowed:
                    conn.sendall(json.dumps({"error": "Permission denied."}).encode())
                    continue
                try:
                    req = json.loads(data.decode())
                    method = req.get("method")
                    params = req.get("params", {})
                    store = _load(f)
                    if method == "get":
                        result = store.get(params.get("service", ""), "")
                        conn.sendall(json.dumps({"result": result}).encode())
                    elif method == "set":
                        store[params["service"]] = params["key"]
                        _save(f, store)
                        conn.sendall(json.dumps({"result": "ok"}).encode())
                    elif method == "list":
                        conn.sendall(
                            json.dumps({"result": list(store.keys())}).encode()
                        )
                    elif method == "delete":
                        store.pop(params.get("service"), None)
                        _save(f, store)
                        conn.sendall(json.dumps({"result": "ok"}).encode())
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

    lp = sub.add_parser("list")
    lp.set_defaults(func=cmd_list)

    d = sub.add_parser("delete")
    d.add_argument("--service", required=True)
    d.set_defaults(func=cmd_delete)

    sub.add_parser("rotate").set_defaults(func=cmd_rotate)

    sub.add_parser("daemon").set_defaults(func=cmd_daemon)

    args = p.parse_args()
    args.func(args)


if __name__ == "__main__":
    main()
