# AOS — scripts/agent_orchestrator.py — Purpose: Python module 
# AOS — ./scripts/agent_orchestrator.py
# (c) 2025 RedactedCoder23
# Brief: Source file.
import json
import os
import queue
import subprocess
import threading
import time
from collections import OrderedDict
import shlex
import importlib
from dataclasses import dataclass
from math import ceil
from typing import Iterable, Dict, List

from scripts.ai_providers import loader
from scripts.ai_providers.base import AIProvider
from src.api import events
from src.branch.task_definitions import Task


@dataclass
class LoadStats:
    pending_tasks: int
    active_agents: int
    cpu_pct: float
    mem_pct: float


MAX_AGENTS = int(os.environ.get("MAX_AGENTS", "4"))
TASKS_PER_AGENT = int(os.environ.get("TASKS_PER_AGENT", "3"))
METRICS: Dict[int, Dict[str, float]] = {}
branch_stats: Dict[str, Dict] = {}
_stats_lock = threading.Lock()
_stats_cache: "OrderedDict[str, Dict]" = OrderedDict()
_cache_limit = 20
_failure_count = 0
_break_until = 0.0


def get_stats(branch_id: str) -> Dict | None:
    """Return latest stats for ``branch_id`` copied from memory.

    Implements a simple circuit breaker to avoid repeated failures.
    Successful calls populate an LRU cache used on failure paths.
    """
    global _failure_count, _break_until

    now = time.time()
    if now < _break_until:
        raise RuntimeError("metrics temporarily unavailable")
    try:
        with _stats_lock:
            data = branch_stats.get(str(branch_id))
            result = json.loads(json.dumps(data)) if data else None
        if result is not None:
            if len(_stats_cache) >= _cache_limit:
                _stats_cache.popitem(last=False)
            _stats_cache[str(branch_id)] = result
        _failure_count = 0
        return result
    except Exception as exc:
        _failure_count += 1
        if _failure_count >= 3:
            _break_until = now + 5
        raise RuntimeError(str(exc))


def get_cached_stats(branch_id: str) -> Dict | None:
    """Return cached stats for ``branch_id`` if available."""
    return _stats_cache.get(str(branch_id))


try:
    psutil = (
        importlib.import_module("psutil")
        if importlib.util.find_spec("psutil")
        else None
    )
except Exception:  # pragma: no cover - optional dependency
    psutil = None
PROVIDERS: Dict[str, AIProvider] = loader.PROVIDERS


try:
    import yaml  # type: ignore
except Exception:  # pragma: no cover - handle optional dep
    yaml = None


def _cpu_pct() -> float:
    """Return system CPU utilisation percentage."""
    if psutil is None:  # pragma: no cover - optional
        return 0.0
    try:
        return psutil.cpu_percent(interval=None)
    except Exception:  # pragma: no cover - psutil error
        return 0.0


def _mem_pct() -> float:
    """Return system memory utilisation percentage."""
    if psutil is None:  # pragma: no cover - optional
        return 0.0
    try:
        return psutil.virtual_memory().percent
    except Exception:  # pragma: no cover - psutil error
        return 0.0


def calc_desired_agents(stats: LoadStats) -> int:
    """Calculate desired agent count from load statistics."""
    desired = (
        ceil(stats.pending_tasks / TASKS_PER_AGENT)
        if TASKS_PER_AGENT
        else stats.pending_tasks
    )
    desired = min(desired, MAX_AGENTS)
    if desired > stats.active_agents + 1:
        desired = stats.active_agents + 1
    return desired


def _load_providers() -> None:
    """Compatibility wrapper around :func:`loader.load_providers`."""
    loader.load_providers()


def _load_spec(branch_id: int) -> List[Task]:
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
    tasks: List[Task] = []
    for item in data:
        if (
            not isinstance(item, dict)
            or "agent_id" not in item
            or "command" not in item
        ):
            raise ValueError("invalid task entry")
        tasks.append(
            Task(
                agent_id=str(item["agent_id"]),
                command=item["command"],
                depends_on=list(item.get("depends_on", [])),
                priority=int(item.get("priority", 100)),
                provider=item.get("provider"),
            )
        )
    return tasks


def _run_agent(
    branch_id: int,
    task: Task,
    timeout: int = 60,
) -> Dict[str, str]:
    """Execute a single agent task and write its result."""
    provider = task.provider
    attempts = 0
    start_time = time.perf_counter()
    while attempts < 3:
        attempts += 1
        try:
            if provider:
                prov = loader.get_provider(provider)
                out = prov.generate(task.command)
                err = ""
                status = "success"
            else:
                cmd = task.command
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
        events.emit(
            {
                "agent_id": task.agent_id,
                "status": "retrying",
                "attempt": attempts,
            }
        )
    runtime = time.perf_counter() - start_time
    result = {
        "agent_id": task.agent_id,
        "status": status,
        "stdout": out or "",
        "stderr": err or "",
        "runtime": runtime,
        "restart_count": attempts - 1,
    }
    agents_dir = os.path.join("branches", str(branch_id), "agents")
    os.makedirs(agents_dir, exist_ok=True)
    path = os.path.join(agents_dir, f"agent-{task.agent_id}.json")
    with open(path, "w", encoding="utf-8") as fh:
        json.dump(result, fh)
    if status != "success" and attempts >= 3:
        events.emit({"type": "agent_failed", "id": task.agent_id})
    events.emit(result)
    return result


def _run_quality(branch_id: int) -> float:
    """Run lint, tests and coverage for *branch_id* returning coverage %."""
    if os.environ.get("AOS_SKIP_QUALITY"):
        return 0.0
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
    waiting = {t.agent_id: t for t in tasks if t.depends_on}
    queue_tasks: queue.Queue[Task | None] = queue.Queue()
    ready = [t for t in tasks if not t.depends_on]
    ready.sort(key=lambda x: x.priority)
    for t in ready:
        queue_tasks.put(t)

    res_queue: queue.Queue = queue.Queue()

    def _collector(ev):
        res_queue.put(ev)

    events.register(_collector)

    workers: List[threading.Thread] = []

    def worker():
        while True:
            try:
                task = queue_tasks.get(timeout=1)
            except queue.Empty:
                continue
            if task is None:
                break
            res = _run_agent(branch_id, task)
            results[task.agent_id] = res
            for wid in list(waiting.keys()):
                w = waiting[wid]
                if all(
                    d in results and results[d].get("status") == "success"
                    for d in w.depends_on
                ):
                    queue_tasks.put(w)
                    waiting.pop(wid)
            queue_tasks.task_done()

    def spawn_agent():
        t = threading.Thread(target=worker)
        workers.append(t)
        t.start()

    def kill_agent():
        queue_tasks.put(None)

    spawn_agent()
    desired_prev = 1
    events.emit({"type": "scaling", "desired": desired_prev, "active": len(workers)})
    spawned = len(workers)

    while True:
        pending = queue_tasks.qsize() + len(waiting)
        cpu_val = _cpu_pct()
        mem_val = _mem_pct()
        stats = LoadStats(
            pending_tasks=pending,
            active_agents=len(workers),
            cpu_pct=cpu_val,
            mem_pct=mem_val,
        )
        with _stats_lock:
            rec = branch_stats.setdefault(str(branch_id), {"history": []})
            rec["pending_tasks"] = pending
            rec["cpu_pct"] = cpu_val
            rec["mem_pct"] = mem_val
            rec.setdefault("history", []).append(
                {
                    "timestamp": time.strftime("%Y-%m-%dT%H:%M:%SZ", time.gmtime()),
                    "pending_tasks": pending,
                    "cpu_pct": cpu_val,
                    "mem_pct": mem_val,
                }
            )
            if len(rec["history"]) > 20:
                rec["history"].pop(0)
        events.emit(
            {
                "type": "stats",
                "branch_id": branch_id,
                "cpu_pct": cpu_val,
                "mem_pct": mem_val,
            }
        )
        desired = calc_desired_agents(stats)
        if desired != desired_prev:
            events.emit({"type": "scaling", "desired": desired, "active": len(workers)})
        while len(workers) < desired:
            spawn_agent()
            spawned += 1
        while len(workers) > desired:
            kill_agent()
            workers.pop()
        desired_prev = desired

        if pending == 0 and queue_tasks.empty():
            break

        try:
            ev = res_queue.get(timeout=0.1)
            yield ev
        except queue.Empty:
            pass

    for _ in workers:
        kill_agent()
    for w in workers:
        w.join()

    METRICS[branch_id] = {
        "agents_spawned": spawned,
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

    coverage_pct = _run_quality(branch_id)
    events.emit(
        {
            "type": "stats",
            "branch_id": branch_id,
            "coverage": coverage_pct,
        }
    )

    while not res_queue.empty():
        yield res_queue.get()

    events.unregister(_collector)


def run_agents(branch_id: int) -> Dict[int, Dict[str, str]]:
    """Helper returning results dictionary for branch ``branch_id``."""
    results: Dict[int, Dict[str, str]] = {}
    for ev in run_tasks(branch_id):
        if "status" in ev and ev.get("agent_id") is not None:
            try:
                key = int(ev["agent_id"])
            except ValueError:
                continue
            results[key] = ev
    return results


def main(branch_id: int) -> None:
    for result in run_tasks(branch_id):
        print(json.dumps(result))


if __name__ == "__main__":
    import sys

    if len(sys.argv) != 2:
        print("Usage: agent_orchestrator.py <branch_id>")
        sys.exit(1)
    main(int(sys.argv[1]))
