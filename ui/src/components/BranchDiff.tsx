import React, { useEffect, useState } from 'react';

export default function BranchDiff({ id }: { id: string }) {
  const [diff, setDiff] = useState('');
  const [raw, setRaw] = useState(false);

  useEffect(() => {
    fetch(`/api/branches/${id}/diff`)
      .then((r) => (r.ok ? r.json() : Promise.reject()))
      .then((data) => setDiff(data.diff))
      .catch(() => setDiff(''));
  }, [id]);

  const pretty = diff.split('\n').map((l, i) => {
    let color: string | undefined;
    if (l.startsWith('+')) color = 'green';
    else if (l.startsWith('-')) color = 'red';
    return (
      <div key={i} style={{ color }}>
        {l}
      </div>
    );
  });

  return (
    <div>
      <button onClick={() => setRaw((r) => !r)}>
        {raw ? 'Pretty' : 'Raw'}
      </button>
      {raw ? <pre>{diff}</pre> : <pre>{pretty}</pre>}
    </div>
  );
}
