#!/usr/bin/env python3
"""Simple credential vault prototype."""
import argparse
import json
import os
import socket
import struct
import sys
from cryptography.fernet import Fernet
import pwd
import grp
import urllib.request
import urllib.error

from scripts import aos_audit
from scripts.aos_audit import log_entry

try:
    import keyring
except Exception:  # pragma: no cover - optional
    keyring = None

DB_PATH = os.environ.get("AICRED_DB", "/var/lib/ai-cred/creds.db")
KEY_PATH = os.environ.get("AICRED_KEY", "/etc/ai-cred/master.key")
SOCK_PATH = os.environ.get("AICRED_SOCK", "/run/ai-cred.sock")
DROP_USER = os.environ.get("AICRED_DROP_USER", "branch-manager")
DROP_GROUP = os.environ.get("AICRED_DROP_GROUP", "branchd")
ACL_PATH = os.environ.get("AICRED_ACL", "/etc/ai-cred/acl.json")
VAULT_ADDR = os.environ.get("VAULT_ADDR")
ENV_PATH = os.path.expanduser("~/.aos/secrets.env")


class CredentialsUnavailableError(RuntimeError):
    """Raised when the credential store cannot be found."""


def _load_acl():
    if not os.path.exists(ACL_PATH):
        return {}
    try:
        with open(ACL_PATH, "r") as fh:
            return json.load(fh)
    except Exception:
        return {}


def _check_acl(action: str, uid: int | None = None) -> bool:
    """Return True if uid is allowed for action."""
    acl = _load_acl()
    rule = acl.get(action, {})
    uid = uid if uid is not None else os.getuid()
    gids = []
    try:
        user = pwd.getpwuid(uid)
        gids = os.getgrouplist(user.pw_name, user.pw_gid)
    except Exception:
        pass
    if uid in rule.get("users", []):
        return True
    if any(g in rule.get("groups", []) for g in gids):
        return True
    return False


def _check_group(group: str | None, uid: int | None = None) -> bool:
    if not group:
        return True
    uid = uid if uid is not None else os.getuid()
    try:
        user = pwd.getpwuid(uid)
        gids = os.getgrouplist(user.pw_name, user.pw_gid)
        return grp.getgrnam(group).gr_gid in gids
    except Exception:
        return False


def _env_load() -> dict:
    if not os.path.exists(ENV_PATH):
        return {}
    with open(ENV_PATH, "r", encoding="utf-8") as fh:
        data = {}
        for line in fh:
            if "=" in line:
                k, v = line.strip().split("=", 1)
                data[k] = v
        return data


def _env_save(data: dict) -> None:
    os.makedirs(os.path.dirname(ENV_PATH), exist_ok=True)
    with open(ENV_PATH, "w", encoding="utf-8") as fh:
        for k, v in data.items():
            fh.write(f"{k}={v}\n")


def _vault_get(key: str) -> str | None:
    if not VAULT_ADDR:
        return None
    url = f"{VAULT_ADDR}/v1/secret/data/{key}"
    try:
        with urllib.request.urlopen(url) as resp:
            data = json.load(resp)
            return data.get("data", {}).get("data", {}).get("value")
    except Exception:
        return None


def _vault_set(key: str, value: str) -> bool:
    if not VAULT_ADDR:
        return False
    url = f"{VAULT_ADDR}/v1/secret/data/{key}"
    req = urllib.request.Request(
        url,
        method="POST",
        data=json.dumps({"data": {"value": value}}).encode(),
        headers={"Content-Type": "application/json"},
    )
    try:
        with urllib.request.urlopen(req):
            return True
    except Exception:
        return False


def vault_get(key: str) -> str:
    val = _vault_get(key)
    if val is not None:
        return val
    env = _env_load()
    return env.get(key, "")


def vault_set(key: str, value: str) -> None:
    if not _vault_set(key, value):
        data = _env_load()
        data[key] = value
        _env_save(data)


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
        aos_audit.warn("credential store missing")
        raise CredentialsUnavailableError("credential store missing")
    with open(DB_PATH, "rb") as fh:
        data = db_f.decrypt(fh.read())
        return json.loads(data.decode())


def _save(db_f, data):
    os.makedirs(os.path.dirname(DB_PATH), exist_ok=True)
    with open(DB_PATH, "wb") as fh:
        fh.write(db_f.encrypt(json.dumps(data).encode()))


def cmd_set(args):
    if not _check_acl("set") or not _check_group(args.group):
        log_entry(pwd.getpwuid(os.getuid()).pw_name, "cred_set", args.service, "denied")
        print("Permission denied")
        return
    f = _fernet()
    data = _load(f)
    data[args.service] = args.key
    _save(f, data)
    log_entry(pwd.getpwuid(os.getuid()).pw_name, "cred_set", args.service, "success")


def cmd_get(args):
    if not _check_acl("get") or not _check_group(args.group):
        log_entry(pwd.getpwuid(os.getuid()).pw_name, "cred_get", args.service, "denied")
        print("Permission denied")
        return
    f = _fernet()
    data = _load(f)
    val = data.get(args.service, "")
    print(val)
    log_entry(pwd.getpwuid(os.getuid()).pw_name, "cred_get", args.service, "success")


def cmd_list(args):
    if not _check_acl("list") or not _check_group(args.group):
        log_entry(pwd.getpwuid(os.getuid()).pw_name, "cred_list", "*", "denied")
        print("Permission denied")
        return
    f = _fernet()
    data = _load(f)
    print(" ".join(data.keys()))
    log_entry(pwd.getpwuid(os.getuid()).pw_name, "cred_list", "*", "success")


def cmd_delete(args):
    if not _check_acl("delete"):
        log_entry(
            pwd.getpwuid(os.getuid()).pw_name, "cred_delete", args.service, "denied"
        )
        print("Permission denied")
        return
    f = _fernet()
    data = _load(f)
    data.pop(args.service, None)
    _save(f, data)
    log_entry(pwd.getpwuid(os.getuid()).pw_name, "cred_delete", args.service, "success")


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
    log_entry(pwd.getpwuid(os.getuid()).pw_name, "cred_rotate", "*", "success")


def cmd_daemon(_args):
    _health_check()
    f = _fernet()
    os.makedirs(os.path.dirname(SOCK_PATH), exist_ok=True)
    if os.path.exists(SOCK_PATH):
        os.unlink(SOCK_PATH)
    with socket.socket(socket.AF_UNIX, socket.SOCK_STREAM) as s:
        s.bind(SOCK_PATH)
        try:
            import grp as grp_mod

            gid = grp_mod.getgrnam(DROP_GROUP).gr_gid
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
                data = conn.recv(4096)
                if not _check_acl("rpc", uid):
                    conn.sendall(json.dumps({"error": "Permission denied."}).encode())
                    log_entry(pwd.getpwuid(uid).pw_name, "cred_rpc", "*", "denied")
                    continue
                try:
                    req = json.loads(data.decode())
                    method = req.get("method")
                    params = req.get("params", {})
                    store = _load(f)
                    if method == "get":
                        allowed = _check_acl("get", uid)
                        if allowed:
                            result = store.get(params.get("service", ""), "")
                            conn.sendall(json.dumps({"result": result}).encode())
                            log_entry(
                                pwd.getpwuid(uid).pw_name,
                                "cred_get",
                                params.get("service", ""),
                                "success",
                            )
                        else:
                            conn.sendall(
                                json.dumps({"error": "Permission denied."}).encode()
                            )
                            log_entry(
                                pwd.getpwuid(uid).pw_name,
                                "cred_get",
                                params.get("service", ""),
                                "denied",
                            )
                    elif method == "set":
                        allowed = _check_acl("set", uid)
                        if allowed:
                            store[params["service"]] = params["key"]
                            _save(f, store)
                            conn.sendall(json.dumps({"result": "ok"}).encode())
                            log_entry(
                                pwd.getpwuid(uid).pw_name,
                                "cred_set",
                                params.get("service"),
                                "success",
                            )
                        else:
                            conn.sendall(
                                json.dumps({"error": "Permission denied."}).encode()
                            )
                            log_entry(
                                pwd.getpwuid(uid).pw_name,
                                "cred_set",
                                params.get("service"),
                                "denied",
                            )
                    elif method == "list":
                        allowed = _check_acl("list", uid)
                        if allowed:
                            conn.sendall(
                                json.dumps({"result": list(store.keys())}).encode()
                            )
                            log_entry(
                                pwd.getpwuid(uid).pw_name, "cred_list", "*", "success"
                            )
                        else:
                            conn.sendall(
                                json.dumps({"error": "Permission denied."}).encode()
                            )
                            log_entry(
                                pwd.getpwuid(uid).pw_name, "cred_list", "*", "denied"
                            )
                    elif method == "delete":
                        allowed = _check_acl("delete", uid)
                        if allowed:
                            store.pop(params.get("service"), None)
                            _save(f, store)
                            conn.sendall(json.dumps({"result": "ok"}).encode())
                            log_entry(
                                pwd.getpwuid(uid).pw_name,
                                "cred_delete",
                                params.get("service"),
                                "success",
                            )
                        else:
                            conn.sendall(
                                json.dumps({"error": "Permission denied."}).encode()
                            )
                            log_entry(
                                pwd.getpwuid(uid).pw_name,
                                "cred_delete",
                                params.get("service"),
                                "denied",
                            )
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
    s.add_argument("--group")
    s.set_defaults(func=cmd_set)

    g = sub.add_parser("get")
    g.add_argument("--service", required=True)
    g.add_argument("--group")
    g.set_defaults(func=cmd_get)

    lp = sub.add_parser("list")
    lp.add_argument("--group")
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
