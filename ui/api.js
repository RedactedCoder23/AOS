export async function listBranches() {
  const resp = await fetch('/branches');
  if (!resp.ok) throw new Error('failed to list branches');
  return await resp.json();
}

export async function createBranch() {
  const resp = await fetch('/branches', { method: 'POST' });
  if (!resp.ok) throw new Error('failed to create branch');
  return await resp.json();
}

export async function mergeBranch(id) {
  const resp = await fetch(`/branches/${id}/merge`, { method: 'POST' });
  if (!resp.ok) throw new Error('failed to merge branch');
  return await resp.json();
}

export async function snapshotBranch(id) {
  const resp = await fetch(`/branches/${id}/snapshot`, { method: 'POST' });
  if (!resp.ok) throw new Error('failed to snapshot branch');
  return await resp.json();
}

export async function deleteBranch(id) {
  const resp = await fetch(`/branches/${id}`, { method: 'DELETE' });
  if (!resp.ok) throw new Error('failed to delete branch');
  return await resp.json();
}
