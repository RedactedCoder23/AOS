import json
import os
import queue
import subprocess
import threading
import time
import shlex
from typing import Iterable, Dict, List
import importlib

from .ai_providers.base import AIProvider

MAX_AGENTS = int(os.environ.get("MAX_AGENTS", "4"))
METRICS: Dict[int, Dict[str, float]] = {}
PROVIDERS: Dict[str, AIProvider] = {}

try:
    import yaml  # type: ignore
except Exception:  # pragma: no cover - handle optional dep
    yaml = None


def _cpu_usage(pid: int) -> float:
    """Return CPU usage percentage for *pid* over a short interval."""
    try:
        with open(f"/proc/{pid}/stat", "r") as fh:
            fields = fh.read().split()
            utime = int(fields[13])
            stime = int(fields[14])
    except Exception:  # pragma: no cover - /proc not available
        return 0.0
    time.sleep(0.1)
    try:
        with open(f"/proc/{pid}/stat", "r") as fh:
            fields = fh.read().split()
            utime2 = int(fields[13])
            stime2 = int(fields[14])
    except Exception:  # pragma: no cover - /proc not available
        return 0.0
    clk = os.sysconf(os.sysconf_names.get("SC_CLK_TCK", 100))
    delta = (utime2 + stime2) - (utime + stime)
    return delta / clk * 1000.0  # approx percentage


def _load_providers() -> None:
    global PROVIDERS
    if PROVIDERS:
        return
    cfg = os.path.join(os.path.dirname(os.path.dirname(__file__)), "providers.json")
    try:
        with open(cfg, "r", encoding="utf-8") as fh:
            data = json.load(fh)
    except Exception:  # pragma: no cover - missing config
        data = {}
    for name, info in data.items():
        try:
            mod = importlib.import_module(f"scripts.ai_providers.{info['module']}")
            cls = getattr(mod, info["class"])
            PROVIDERS[name] = cls(name)
        except Exception:  # pragma: no cover - plugin errors
            continue


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
        tasks.append(
            {
                "agent_id": str(item["agent_id"]),
                "command": item["command"],
                "depends_on": list(item.get("depends_on", [])),
                "provider": item.get("provider"),
            }
        )
    return tasks


def _run_agent(
    branch_id: int,
    task: Dict[str, str],
    emit: callable,
    timeout: int = 60,
) -> Dict[str, str]:
    """Execute a single agent task and write its result."""
    _load_providers()
    provider = task.get("provider")
    attempts = 0
    start_time = time.perf_counter()
    while attempts < 3:
        attempts += 1
        try:
            if provider:
                prov = PROVIDERS.get(provider)
                if prov is None:
                    raise RuntimeError(f"provider {provider} not found")
                out = prov.generate(task["command"])
                err = ""
                status = "success"
            else:
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
                out, err = proc.communicate(timeout=timeout)
                status = "success" if proc.returncode == 0 else "failed"
        except subprocess.TimeoutExpired:
            proc.kill()
            out, err = proc.communicate()
            status = "failed"
        except Exception as exc:
            out = ""
            err = str(exc)
            status = "failed"
        if status == "success" or attempts >= 3:
            break
        # retry notification
        event = {
            "agent_id": task["agent_id"],
            "status": "retrying",
            "attempt": attempts,
        }
        emit(event)
    runtime = time.perf_counter() - start_time
    result = {
        "agent_id": task["agent_id"],
        "status": status,
        "stdout": out or "",
        "stderr": err or "",
        "runtime": runtime,
    }
    agents_dir = os.path.join("branches", str(branch_id), "agents")
    os.makedirs(agents_dir, exist_ok=True)
    path = os.path.join(agents_dir, f"agent-{task['agent_id']}.json")
    with open(path, "w", encoding="utf-8") as fh:
        json.dump(result, fh)
    emit(result)
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
    tasks = _load_spec(branch_id)
    results: Dict[str, Dict[str, str]] = {}
    waiting = {t["agent_id"]: t for t in tasks if t.get("depends_on")}
    queue_tasks: queue.Queue = queue.Queue()
    for t in tasks:
        if not t.get("depends_on"):
            queue_tasks.put(t)

    res_queue: queue.Queue = queue.Queue()

    def emit(ev):
        res_queue.put(ev)

    workers: List[threading.Thread] = []

    def worker():
        while True:
            try:
                task = queue_tasks.get(timeout=1)
            except queue.Empty:
                if stop_event.is_set():
                    break
                continue
            res = _run_agent(branch_id, task, emit)
            results[task["agent_id"]] = res
            for wid in list(waiting.keys()):
                w = waiting[wid]
                if all(
                    d in results and results[d].get("status") == "success"
                    for d in w["depends_on"]
                ):
                    queue_tasks.put(w)
                    waiting.pop(wid)
            queue_tasks.task_done()

    stop_event = threading.Event()

    # initial workers
    initial = 1 + (queue_tasks.qsize() // 5)
    initial = min(initial, MAX_AGENTS)
    for _ in range(initial):
        t = threading.Thread(target=worker)
        workers.append(t)
        t.start()
    last_idle = None

    while True:
        # scaling logic
        if (queue_tasks.qsize() > 5 or _cpu_usage(os.getpid()) > 70) and len(
            workers
        ) < MAX_AGENTS:
            t = threading.Thread(target=worker)
            workers.append(t)
            t.start()
        if queue_tasks.empty() and not waiting:
            if last_idle is None:
                last_idle = time.time()
            if (
                time.time() - last_idle > 30
                and len(workers) > 1
                and _cpu_usage(os.getpid()) < 10
            ):
                # shut down one worker
                stop_event.set()
                workers.pop().join()
                stop_event.clear()
                last_idle = time.time()
        else:
            last_idle = None

        if queue_tasks.empty() and not waiting:
            break

        try:
            ev = res_queue.get(timeout=0.1)
            yield ev
        except queue.Empty:
            pass

    stop_event.set()
    for w in workers:
        w.join()

    METRICS[branch_id] = {
        "agents_spawned": len(workers),
        "success_rate": (
            sum(1 for e in results.values() if e.get("status") == "success")
            / len(tasks)
            if tasks
            else 0.0
        ),
        "avg_runtime": (
            sum(e.get("runtime", 0.0) for e in results.values()) / len(results)
            if results
            else 0.0
        ),
    }

    _run_quality(branch_id)

    while not res_queue.empty():
        yield res_queue.get()


def main(branch_id: int) -> None:
    for result in run_tasks(branch_id):
        print(json.dumps(result))


if __name__ == "__main__":
    import sys

    if len(sys.argv) != 2:
        print("Usage: agent_orchestrator.py <branch_id>")
        sys.exit(1)
    main(int(sys.argv[1]))
