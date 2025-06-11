import React, { useEffect, useState } from 'react';
import { LineChart, Line, XAxis, YAxis } from 'recharts';
import { useParams } from 'react-router-dom';
import BranchDiff from '../components/BranchDiff';

export default function BranchDetail() {
  const { id } = useParams();
  const [data, setData] = useState<any[]>([]);
  const [status, setStatus] = useState<string>('unknown');

  useEffect(() => {
    if (!id) return;
    const es = new EventSource(`/events`);
    const handler = (e: MessageEvent) => {
      try {
        const ev = JSON.parse(e.data);
        if (ev.branch_id && String(ev.branch_id) === String(id)) {
          if ('cpu_pct' in ev) {
            setData((d) => [
              ...d.slice(-19),
              { ts: Date.now(), cpu: ev.cpu_pct, coverage: ev.coverage || 0 },
            ]);
          }
        }
      } catch {}
    };
    es.addEventListener('stats', handler);
    return () => {
      es.removeEventListener('stats', handler);
      es.close();
    };
  }, [id]);

  useEffect(() => {
    if (!id) return;
    const ws = new WebSocket(`ws://${window.location.host}/ws/branches/${id}`);
    ws.onmessage = (ev) => {
      try {
        const msg = JSON.parse(ev.data);
        if (msg.status) setStatus(msg.status);
      } catch {}
    };
    return () => ws.close();
  }, [id]);

  const rollback = async () => {
    if (!id) return;
    if (!confirm('Rollback branch?')) return;
    await fetch(`/branches/${id}/rollback`, { method: 'POST' });
  };

  return (
    <div>
      <h2>Branch {id}</h2>
      <p>Status: {status}</p>
      <button onClick={rollback}>Rollback</button>
      <LineChart width={300} height={200} data={data}>
        <Line type="monotone" dataKey="cpu" stroke="#8884d8" />
        <Line type="monotone" dataKey="coverage" stroke="#82ca9d" />
        <XAxis dataKey="ts" hide />
        <YAxis />
      </LineChart>
      {id && <BranchDiff id={id} />}
    </div>
  );
}
