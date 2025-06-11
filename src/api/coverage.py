import glob
import json
import os
from fastapi import FastAPI, Response
import yaml
from scripts import aos_audit as audit
from src.service.queue import load_status

app = FastAPI()


def _load_threshold() -> float:
    if os.path.exists("config.yml"):
        try:
            with open("config.yml", "r", encoding="utf-8") as fh:
                data = yaml.safe_load(fh) or {}
                return float(data.get("coverage_threshold", 0))
        except Exception:
            return 0.0
    return 0.0


def _load_history(branch_id: str):
    base = os.path.expanduser(os.path.join("~/.aos/branches", str(branch_id)))
    hist = []
    for path in sorted(glob.glob(os.path.join(base, "coverage-*.json"))):
        date = os.path.basename(path)[9:17]
        try:
            with open(path, "r", encoding="utf-8") as fh:
                data = json.load(fh)
            lines = float(data.get("totals", {}).get("percent_covered", 0))
            branches = float(data.get("totals", {}).get("percent_branches", 0))
        except Exception:
            lines = branches = 0.0
        hist.append({"date": date, "lines": lines, "branches": branches})
    return hist


@app.get("/branches/{id}/coverage-history")
def get_coverage(id: str, response: Response):
    """Return coverage history for branch ``id``."""
    hist = _load_history(id)
    threshold = _load_threshold()
    if hist and threshold and hist[-1]["lines"] < threshold:
        response.headers["X-Coverage-Below-Threshold"] = "true"
    status = load_status(id).get("status")
    audit.log("get_coverage", branch=id, user="api")
    data = {"coverage": hist, "threshold": threshold}
    if status is not None:
        data["status"] = status
    return data
