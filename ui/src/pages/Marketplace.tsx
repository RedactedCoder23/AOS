import React from 'react';
import api from '../services/api';

const providers = [
  { name: 'OpenAI', repo: 'https://github.com/aos/openai-plugin.git' },
  { name: 'Local Llama', repo: 'https://github.com/aos/local-llama.git' },
  { name: 'Echo', repo: 'https://github.com/aos/echo-plugin.git' },
];

export default function Marketplace() {
  const install = async (repo: string) => {
    try {
      await api.installPlugin(repo);
      alert('Installed ' + repo);
    } catch {
      alert('Install failed');
    }
  };

  return (
    <div>
      <h2>Marketplace</h2>
      <div style={{ display: 'flex', gap: '1rem' }}>
        {providers.map((p) => (
          <div
            key={p.repo}
            style={{ border: '1px solid #ccc', padding: '1rem' }}
          >
            <h3>{p.name}</h3>
            <button onClick={() => install(p.repo)}>Install</button>
          </div>
        ))}
      </div>
    </div>
  );
}
