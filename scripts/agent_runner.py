import argparse
import json
import sys
import time
import threading
import subprocess
import shlex
import os

HEARTBEAT_INTERVAL = 1


def main(argv: list[str] | None = None) -> int:
    parser = argparse.ArgumentParser(description="run a command with heartbeat")
    parser.add_argument("cmd", help="command to execute")
    parser.add_argument("hb_file", help="heartbeat file")
    parser.add_argument("result_file", help="result json file")
    parser.add_argument("--provider", help="AI provider override")
    args = parser.parse_args(argv)

    cmd = args.cmd
    hb = args.hb_file
    result = args.result_file
    if cmd.startswith("["):
        proc_args = json.loads(cmd)
    else:
        proc_args = shlex.split(cmd)
    env = os.environ.copy()
    if args.provider:
        env["AOS_TASK_META"] = json.dumps({"provider": args.provider})
    proc = subprocess.Popen(
        proc_args, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True, env=env
    )

    def heartbeat() -> None:
        while proc.poll() is None:
            with open(hb, "w", encoding="utf-8") as fh:
                fh.write(str(time.time()))
            time.sleep(HEARTBEAT_INTERVAL)

    th = threading.Thread(target=heartbeat, daemon=True)
    th.start()
    out, err = proc.communicate()
    with open(result, "w", encoding="utf-8") as fh:
        json.dump({"returncode": proc.returncode, "stdout": out, "stderr": err}, fh)
    return proc.returncode


if __name__ == "__main__":
    sys.exit(main())
