import os
from fastapi import FastAPI, HTTPException
from src.service.security import apply_security_headers
from scripts import aos_audit as audit
import pygit2

app = FastAPI()
apply_security_headers(app)


def _get_repo() -> pygit2.Repository:
    path = pygit2.discover_repository(os.getcwd())
    if not path:
        raise HTTPException(status_code=500, detail="repo not found")
    return pygit2.Repository(path)


@app.get("/branches/{id}/diff")
def branch_diff(id: str) -> dict:
    """Return unified diff for *id* vs its previous commit."""
    repo = _get_repo()
    br = repo.lookup_branch(id)
    if br is None:
        raise HTTPException(status_code=404, detail="branch not found")
    commit = repo.get(br.target)
    if not commit.parents:
        return {"diff": ""}
    parent = commit.parents[0]
    diff = repo.diff(parent, commit)
    audit.log("branch_diff", branch=id, user="api")
    return {"diff": diff.patch}
