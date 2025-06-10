export function listBranches() {
  return fetch('/branches').then(r => r.json());
}
export function createBranch() {
  return fetch('/branches', {method:'POST'}).then(r => r.json());
}
export function mergeBranch(id) {
  return fetch(`/branches/${id}/merge`, {method:'POST'}).then(r => r.json());
}
