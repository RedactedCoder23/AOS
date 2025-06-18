/* AOS — src/api/branches.py — Purpose: Python module */
# AOS — ./src/api/branches.py
# (c) 2025 RedactedCoder23
# Brief: Source file.
import glob
import json
import os
import shutil
from fastapi import FastAPI, HTTPException
from src.security.permissions import requires_permission
from src.branch.merge import three_way_merge
from src.db.branch_repo import get_branch
from src.branch.policy import allowed_merge
from src.service.security import apply_security_headers
import yaml
from scripts import aos_audit as audit
from src.service import queue
from src.api.errors import install as install_errors
from src.api.profiler import install as install_profiler

app = FastAPI()
apply_security_headers(app)
install_errors(app)
install_profiler(app)


def _threshold() -> float:
    if os.path.exists("config.yml"):
        try:
            with open("config.yml", "r", encoding="utf-8") as fh:
                data = yaml.safe_load(fh) or {}
                return float(data.get("coverage_threshold", 0))
        except Exception:
            return 0.0
    return 0.0


def _latest_coverage(branch_id: str) -> float:
    base = os.path.expanduser(os.path.join("~/.aos/branches", str(branch_id)))
    files = sorted(glob.glob(os.path.join(base, "coverage-*.json")))
    if not files:
        return 100.0
    path = files[-1]
    try:
        with open(path, "r", encoding="utf-8") as fh:
            data = json.load(fh)
        return float(data.get("totals", {}).get("percent_covered", 0))
    except Exception:
        return 0.0


@app.post("/branches/{id}/merge")
@requires_permission("write")
def merge_branch(id: str):
    cov = _latest_coverage(id)
    thresh = _threshold()
    if thresh and cov < thresh:
        audit.log("merge_blocked", branch=id, user="api", reason="low_coverage")
        detail = f"coverage {cov:.1f}% below threshold {thresh}%"
        raise HTTPException(status_code=412, detail=detail)
    branch = get_branch(int(id))
    if branch and not allowed_merge(branch, "main"):
        raise HTTPException(status_code=403, detail="merge policy violation")
    current = os.path.expanduser(os.path.join("~/.aos", "branches.json"))
    backup = current + ".bak"
    if os.path.exists(current):
        try:
            shutil.copy(current, backup)
        except Exception:
            pass
    audit.log("merge_branch", branch=id, user="api")
    return {"merged": True}


@app.get("/branches/{id}/conflicts")
@requires_permission("view")
def merge_conflicts(id: str):
    base = ""
    ours = ""
    theirs = ""
    merged, conflicts = three_way_merge(base, ours, theirs)
    formatted = [
        {
            "line": c["line"],
            "ours": c["ours"],
            "theirs": c["theirs"],
            "suggestions": ["accept ours", "accept theirs", "manual"],
        }
        for c in conflicts
    ]
    return {"conflicts": formatted, "merged": merged}


@app.post("/branches/{id}/rollback")
@requires_permission("write")
def rollback_branch(id: str):
    """Restore last passing branch JSON and re-queue its job."""
    base = os.path.expanduser(os.path.join("~/.aos", "branches.json"))
    backup = base + ".bak"
    if not os.path.exists(backup):
        raise HTTPException(status_code=404, detail="backup missing")
    try:
        shutil.copy(backup, base)
    except Exception as exc:  # pragma: no cover - file error
        raise HTTPException(status_code=500, detail=str(exc))
    params = queue.job_params(id)
    if params:
        provider, prompt = params
        queue.enqueue_provider_job(id, provider, prompt)
    audit.log("rollback_branch", branch=id, user="api")
    return {"rolled_back": True}
