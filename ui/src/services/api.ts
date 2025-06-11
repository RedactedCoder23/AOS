let token: string | null = null;
let refreshToken: string | null = null;
let role: string | null = null;

export function setToken(t: string, r?: string, rt?: string) {
  token = t;
  if (rt) refreshToken = rt;
  if (r) {
    role = r;
    sessionStorage.setItem('role', r);
  }
}

export function getRole() {
  return role;
}

function headers() {
  return token ? { Authorization: `Bearer ${token}` } : {};
}

export async function login(username: string, password: string) {
  const res = await fetch('/auth/login', {
    method: 'POST',
    headers: { 'Content-Type': 'application/json' },
    body: JSON.stringify({ username, password }),
  });
  if (!res.ok) throw new Error('login failed');
  const data = await res.json();
  setToken(data.access, data.role, data.refresh);
}

export async function refresh() {
  if (!refreshToken) throw new Error('no refresh token');
  const res = await fetch('/auth/refresh', {
    method: 'POST',
    headers: { 'Content-Type': 'application/json' },
    body: JSON.stringify({ token: refreshToken }),
  });
  if (!res.ok) throw new Error('refresh failed');
  const data = await res.json();
  setToken(data.access, data.role, data.refresh);
}

export async function getBranches(limit = 10, cursor?: string) {
  const params = new URLSearchParams();
  params.set('limit', String(limit));
  if (cursor) params.set('cursor', cursor);
  const res = await fetch(`/api/branches?${params}`, { headers: headers() });
  if (!res.ok) throw new Error('list failed');
  return res.json();
}

export async function installPlugin(repo: string) {
  const res = await fetch("/plugins/install", {
    method: "POST",
    headers: { ...headers(), "Content-Type": "application/json" },
    body: JSON.stringify({ repo }),
  });
  if (!res.ok) throw new Error("install failed");
  return res.json();
}

export default { login, refresh, getBranches, installPlugin, getRole };
