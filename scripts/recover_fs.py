import json
import os


def recover(log_path: str = ".journal/log") -> None:
    if not os.path.exists(log_path):
        return
    with open(log_path, "r", encoding="utf-8") as fh:
        for line in fh:
            try:
                entry = json.loads(line)
            except json.JSONDecodeError:
                continue
            if not entry.get("committed"):
                continue


if __name__ == "__main__":
    recover()
