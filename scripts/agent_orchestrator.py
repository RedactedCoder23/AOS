import json
import os
import subprocess
import shlex
from typing import Iterable, Dict, List

try:
    import yaml  # type: ignore
except Exception:  # pragma: no cover - handle optional dep
    yaml = None


def _load_spec(branch_id: int) -> List[Dict[str, str]]:
    """Load tasks spec from branches/<id>/tasks.yaml or .json."""
    base = os.path.join("branches", str(branch_id))
    yaml_path = os.path.join(base, "tasks.yaml")
    json_path = os.path.join(base, "tasks.json")
    if os.path.exists(yaml_path):
        if yaml is None:
            raise RuntimeError("PyYAML required for yaml specs")
        with open(yaml_path, "r", encoding="utf-8") as fh:
            data = yaml.safe_load(fh)
    elif os.path.exists(json_path):
        with open(json_path, "r", encoding="utf-8") as fh:
            data = json.load(fh)
    else:
        raise FileNotFoundError("task spec not found")
    if not isinstance(data, list):
        raise ValueError("spec must be a list of tasks")
    tasks = []
    for item in data:
        if (
            not isinstance(item, dict)
            or "agent_id" not in item
            or "command" not in item
        ):
            raise ValueError("invalid task entry")
        tasks.append({"agent_id": str(item["agent_id"]), "command": item["command"]})
    return tasks


def _run_agent(
    branch_id: int, task: Dict[str, str], timeout: int = 60
) -> Dict[str, str]:
    """Execute a single agent task and write its result."""
    cmd = task["command"]
    if isinstance(cmd, str):
        cmd = shlex.split(cmd)
    proc = subprocess.Popen(
        cmd,
        cwd=os.path.join("branches", str(branch_id)),
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        text=True,
    )
    try:
        out, err = proc.communicate(timeout=timeout)
        status = "success" if proc.returncode == 0 else "failed"
    except subprocess.TimeoutExpired:
        proc.kill()
        out, err = proc.communicate()
        status = "failed"
    result = {
        "agent_id": task["agent_id"],
        "status": status,
        "stdout": out or "",
        "stderr": err or "",
    }
    agents_dir = os.path.join("branches", str(branch_id), "agents")
    os.makedirs(agents_dir, exist_ok=True)
    path = os.path.join(agents_dir, f"agent-{task['agent_id']}.json")
    with open(path, "w", encoding="utf-8") as fh:
        json.dump(result, fh)
    return result


def _run_quality(branch_id: int) -> float:
    """Run lint, tests and coverage for *branch_id* returning coverage %."""
    cmd = (
        "flake8 src/ && pytest --maxfail=1 --disable-warnings -q && "
        "coverage run -m pytest && coverage json -o coverage.json"
    )
    base = os.path.join("branches", str(branch_id))
    subprocess.run(cmd, shell=True, cwd=base)
    cov_file = os.path.join(base, "coverage.json")
    percent = 0.0
    if os.path.exists(cov_file):
        with open(cov_file, "r", encoding="utf-8") as fh:
            try:
                data = json.load(fh)
                percent = float(data.get("totals", {}).get("percent_covered", 0))
            except Exception:
                percent = 0.0
    hist_path = os.path.join(base, "history.json")
    history = []
    if os.path.exists(hist_path):
        try:
            with open(hist_path, "r", encoding="utf-8") as fh:
                history = json.load(fh)
        except Exception:
            history = []
    history.append(percent)
    with open(hist_path, "w", encoding="utf-8") as fh:
        json.dump(history, fh)
    return percent


def run_tasks(branch_id: int) -> Iterable[Dict[str, str]]:
    """Run all tasks for *branch_id* yielding result dicts."""
    for task in _load_spec(branch_id):
        yield _run_agent(branch_id, task)
    _run_quality(branch_id)


def main(branch_id: int) -> None:
    for result in run_tasks(branch_id):
        print(json.dumps(result))


if __name__ == "__main__":
    import sys

    if len(sys.argv) != 2:
        print("Usage: agent_orchestrator.py <branch_id>")
        sys.exit(1)
    main(int(sys.argv[1]))
