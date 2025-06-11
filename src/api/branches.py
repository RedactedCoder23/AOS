import glob
import json
import os
from fastapi import FastAPI, HTTPException
from src.service.security import apply_security_headers
import yaml
from scripts import aos_audit as audit

app = FastAPI()
apply_security_headers(app)


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
def merge_branch(id: str):
    cov = _latest_coverage(id)
    thresh = _threshold()
    if thresh and cov < thresh:
        audit.log("merge_blocked", branch=id, user="api", reason="low_coverage")
        detail = f"coverage {cov:.1f}% below threshold {thresh}%"
        raise HTTPException(status_code=412, detail=detail)
    audit.log("merge_branch", branch=id, user="api")
    return {"merged": True}
