import React, { useEffect, useState } from "react";
import { LineChart, Line, XAxis, YAxis } from "recharts";
import { useParams } from "react-router-dom";
import BranchDiff from "../components/BranchDiff";

export default function BranchDetail() {
  const { id } = useParams();
  const [data, setData] = useState<any[]>([]);

  useEffect(() => {
    if (!id) return;
    const es = new EventSource(`/events`);
    const handler = (e: MessageEvent) => {
      try {
        const ev = JSON.parse(e.data);
        if (ev.branch_id && String(ev.branch_id) === String(id)) {
          if ("cpu_pct" in ev) {
            setData((d) => [
              ...d.slice(-19),
              { ts: Date.now(), cpu: ev.cpu_pct, coverage: ev.coverage || 0 },
            ]);
          }
        }
      } catch {}
    };
    es.addEventListener("stats", handler);
    return () => {
      es.removeEventListener("stats", handler);
      es.close();
    };
  }, [id]);

  return (
    <div>
      <h2>Branch {id}</h2>
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
