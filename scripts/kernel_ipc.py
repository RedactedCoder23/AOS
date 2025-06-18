# AOS — ./scripts/kernel_ipc.py
# (c) 2025 RedactedCoder23
# Brief: Source file.
import argparse
import json
import os
import sys

SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))
REPO_ROOT = os.path.dirname(SCRIPT_DIR)
sys.path.insert(0, REPO_ROOT)
os.environ.setdefault("AOS_IPC_LOCAL", "1")

from scripts import branch_ui  # type: ignore  # noqa: E402


def _error(msg: str, code: int = 404) -> None:
    print(json.dumps({"error": msg, "code": code}))


def create_branch(_):
    bid = branch_ui.kernel_ipc("create")
    print(bid)


def list_branches(_):
    data = branch_ui.kernel_ipc("list")
    if isinstance(data, str):
        print(data)
    else:
        # ensure JSON output
        print(json.dumps(data))


def merge_branch(args):
    rc = branch_ui.kernel_ipc("merge", {"branch_id": args.branch_id})
    if isinstance(rc, int) and rc < 0:
        _error("branch not found")
    else:
        print(json.dumps({"merge_job_id": rc}))


def snapshot_branch(args):
    rc = branch_ui.kernel_ipc("snapshot", {"branch_id": args.branch_id})
    if isinstance(rc, int) and rc < 0:
        _error("branch not found")
    else:
        print(json.dumps({"snapshot_id": rc}))


def delete_branch(args):
    rc = branch_ui.kernel_ipc("delete", {"branch_id": args.branch_id})
    if isinstance(rc, int) and rc < 0:
        _error("branch not found")
    else:
        print(json.dumps({"result": "deleted"}))


def main(argv=None):
    p = argparse.ArgumentParser(prog="kernel-ipc")
    sub = p.add_subparsers(dest="cmd", required=True)

    sub.add_parser("create-branch").set_defaults(func=create_branch)
    sub.add_parser("list-branches").set_defaults(func=list_branches)
    m = sub.add_parser("merge-branch")
    m.add_argument("branch_id", type=int)
    m.set_defaults(func=merge_branch)
    s = sub.add_parser("snapshot-branch")
    s.add_argument("branch_id", type=int)
    s.set_defaults(func=snapshot_branch)
    d = sub.add_parser("delete-branch")
    d.add_argument("branch_id", type=int)
    d.set_defaults(func=delete_branch)

    args = p.parse_args(argv)
    args.func(args)


if __name__ == "__main__":
    main()
