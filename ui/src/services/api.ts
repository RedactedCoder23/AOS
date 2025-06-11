let token: string | null = null;

export function setToken(t: string) {
  token = t;
}

function headers() {
  return token ? { Authorization: `Bearer ${token}` } : {};
}

export async function login(username: string, password: string) {
  const res = await fetch("/auth/login", {
    method: "POST",
    headers: { "Content-Type": "application/json" },
    body: JSON.stringify({ username, password }),
  });
  if (!res.ok) throw new Error("login failed");
  const data = await res.json();
  setToken(data.token);
}

export async function getBranches(limit = 10, cursor?: string) {
  const params = new URLSearchParams();
  params.set("limit", String(limit));
  if (cursor) params.set("cursor", cursor);
  const res = await fetch(`/api/branches?${params}`, { headers: headers() });
  if (!res.ok) throw new Error("list failed");
  return res.json();
}

export default { login, getBranches };
