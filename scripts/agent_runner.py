import json
import sys
import time
import threading
import subprocess
import shlex

HEARTBEAT_INTERVAL = 1


def main() -> int:
    if len(sys.argv) < 4:
        print("usage: agent_runner.py <cmd> <hb_file> <result_file>")
        return 1
    cmd = sys.argv[1]
    hb = sys.argv[2]
    result = sys.argv[3]
    if cmd.startswith("["):
        args = json.loads(cmd)
    else:
        args = shlex.split(cmd)
    proc = subprocess.Popen(
        args, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True
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
