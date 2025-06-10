#!/usr/bin/env python3
"""Checkpoint utility using CBOR"""
import sys
import struct
import time

try:
    import cbor2
except Exception:  # pragma: no cover - optional
    cbor2 = None

MAGIC = 0x43484B50  # CHKP
HDR_FMT = "<I H Q Q I"


def read_checkpoint(path: str) -> int:
    with open(path, "rb") as f:
        raw = f.read()
    hdr_size = struct.calcsize(HDR_FMT)
    magic, ver, bid, ts, plen = struct.unpack(HDR_FMT, raw[:hdr_size])
    if magic != MAGIC:
        print("bad magic", file=sys.stderr)
        return 1
    payload = raw[hdr_size : hdr_size + plen]  # noqa: E203
    entries = cbor2.loads(payload)
    for e in entries:
        if e.get("dir"):
            print(f"D {e['path']}")
        else:
            print(f"F {e['path']} {e['data'].hex()}")
    return 0


def write_checkpoint(path: str, lines) -> int:
    entries = []
    for line in lines:
        line = line.strip()
        if not line:
            continue
        typ, rest = line[0], line[2:]
        if typ == "D":
            entries.append({"path": rest, "dir": True})
        elif typ == "F":
            p, hexdata = rest.split(" ", 1)
            entries.append({"path": p, "dir": False, "data": bytes.fromhex(hexdata)})
    payload = cbor2.dumps(entries)
    hdr = struct.pack(HDR_FMT, MAGIC, 1, 0, int(time.time()), len(payload))
    with open(path, "wb") as f:
        f.write(hdr)
        f.write(payload)
    return 0


def main() -> int:
    if len(sys.argv) < 3:
        print("usage: aos_checkpoint.py <read|write> <file>", file=sys.stderr)
        return 1
    if cbor2 is None:
        print("cbor2 package missing", file=sys.stderr)
        return 1
    cmd, file = sys.argv[1], sys.argv[2]
    if cmd == "read":
        return read_checkpoint(file)
    if cmd == "write":
        return write_checkpoint(file, sys.stdin)
    print("invalid command", file=sys.stderr)
    return 1


if __name__ == "__main__":
    sys.exit(main())
