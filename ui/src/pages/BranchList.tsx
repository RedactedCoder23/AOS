import React, { useEffect, useState } from 'react';
import api from '../services/api';

export default function BranchList() {
  const [branches, setBranches] = useState<any[]>([]);
  const [cursor, setCursor] = useState<string | null>(null);

  const load = async () => {
    const res = await api.getBranches(10, cursor ?? undefined);
    setBranches((prev) => [...prev, ...res.items]);
    setCursor(res.next || null);
  };

  useEffect(() => {
    if (typeof fetch !== 'undefined') {
      load();
    }
  }, []);

  return (
    <div>
      <h2>Branches</h2>
      <ul>
        {branches.map((b) => (
          <li key={b.branch_id}>
            <a href={`/branches/${b.branch_id}`}>{b.branch_id}</a>
          </li>
        ))}
      </ul>
      {cursor && <button onClick={load}>More</button>}
    </div>
  );
}
