import argparse
import datetime
import os
import shutil
import sys


def main() -> int:
    p = argparse.ArgumentParser()
    p.add_argument("branch_id")
    p.add_argument("--src", default="coverage.json")
    args = p.parse_args()

    if not os.path.exists(args.src):
        print("coverage file not found", file=sys.stderr)
        return 1
    base = os.path.expanduser(os.path.join("~/.aos/branches", str(args.branch_id)))
    os.makedirs(base, exist_ok=True)
    date = datetime.date.today().strftime("%Y%m%d")
    dst = os.path.join(base, f"coverage-{date}.json")
    shutil.move(args.src, dst)
    return 0


if __name__ == "__main__":
    sys.exit(main())
