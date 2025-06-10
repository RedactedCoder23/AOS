/* global d3, React, ReactDOM */
import { listBranches, createBranch, mergeBranch } from './api.js';

// Simple toast queue managed via React
function ToastContainer({ toasts }) {
  return React.createElement('div', { id: 'toast-root' },
    toasts.map(t => React.createElement('div', { key: t.id, className: 'toast-item' }, t.msg)));
}

class ErrorBoundary extends React.Component {
  constructor(props) {
    super(props);
    this.state = { hasError: false };
  }

  componentDidCatch(err) {
    console.error(err);
    this.setState({ hasError: true });
  }

  render() {
    if (this.state.hasError) {
      return React.createElement('div', null, 'Something went wrong.');
    }
    return this.props.children;
  }
}


function updateGraph(nodes, links) {
  const width = document.getElementById('graph').clientWidth;
  const height = document.getElementById('graph').clientHeight;
  const svg = d3.select('#graph').html('').append('svg')
    .attr('width', width)
    .attr('height', height);

  const sim = d3.forceSimulation(nodes)
    .force('link', d3.forceLink(links).id(d => d.id).distance(80))
    .force('charge', d3.forceManyBody().strength(-200))
    .force('center', d3.forceCenter(width / 2, height / 2));

  const link = svg.append('g')
    .attr('stroke', '#999')
    .attr('stroke-opacity', 0.6)
    .selectAll('line')
    .data(links)
    .enter().append('line')
    .attr('stroke-width', 2);

  const node = svg.append('g')
    .attr('stroke', '#fff')
    .attr('stroke-width', 1.5)
    .selectAll('circle')
    .data(nodes)
    .enter().append('circle')
    .attr('r', 20)
    .attr('fill', '#69b')
    .call(d3.drag()
      .on('start', dragstarted)
      .on('drag', dragged)
      .on('end', dragended))
    .on('click', (event, d) => showMenu(event, d));

  const labels = svg.append('g')
    .selectAll('text')
    .data(nodes)
    .enter().append('text')
    .text(d => d.name)
    .attr('text-anchor', 'middle')
    .attr('dy', 4)
    .style('pointer-events', 'none');

  sim.on('tick', () => {
    link.attr('x1', d => d.source.x)
        .attr('y1', d => d.source.y)
        .attr('x2', d => d.target.x)
        .attr('y2', d => d.target.y);

    node.attr('cx', d => d.x)
        .attr('cy', d => d.y);

    labels.attr('x', d => d.x)
          .attr('y', d => d.y);
  });

  function dragstarted(event) {
    if (!event.active) sim.alphaTarget(0.3).restart();
    event.subject.fx = event.subject.x;
    event.subject.fy = event.subject.y;
  }

  function dragged(event) {
    event.subject.fx = event.x;
    event.subject.fy = event.y;
  }

  function dragended(event) {
    if (!event.active) sim.alphaTarget(0);
    event.subject.fx = null;
    event.subject.fy = null;
  }
}

function showMenu(event, d) {
  const tip = document.getElementById('tooltip');
  tip.style.display = 'block';
  tip.style.left = `${event.pageX}px`;
  tip.style.top = `${event.pageY}px`;
  tip.innerHTML = '<button id="merge-btn">Merge</button>';
  document.getElementById('merge-btn').onclick = async () => {
    tip.style.display = 'none';
    await mergeBranch(d.id);
    if (window.showToast) window.showToast('Merge succeeded.');
    if (window.refreshBranches) window.refreshBranches();
  };
}

function AgentPanel({ branchId }) {
  const [agents, setAgents] = React.useState([]);
  const [loading, setLoading] = React.useState(true);
  React.useEffect(() => {
    const es = new EventSource(`/branches/${branchId}/agents`);
    es.onmessage = (e) => {
      try {
        const data = JSON.parse(e.data);
        setAgents((a) => [...a.filter(x => x.agent_id !== data.agent_id), data]);
        setLoading(false);
      } catch {
        // ignore parse errors
      }
    };
    return () => es.close();
  }, [branchId]);
  if (loading) {
    return React.createElement('div', null, 'Loading...');
  }
  return React.createElement('ul', null,
    agents.map(a => React.createElement('li', { key: a.agent_id },
      `${a.agent_id}: ${a.task} (${a.status})`)));
}

function BranchNode({ data, onMerge }) {
  return React.createElement('div', { className: 'branch-node' },
    `${data.name} `,
    React.createElement('button', { onClick: onMerge }, 'Merge'),
    React.createElement(AgentPanel, { branchId: data.id }));
}

function App() {
  const [branches, setBranches] = React.useState([]);
  const [loading, setLoading] = React.useState(false);
  const [graphLoading, setGraphLoading] = React.useState(true);
  const [toasts, setToasts] = React.useState([]);

  React.useEffect(() => {
    window.showToast = (msg) => {
      const id = Date.now() + Math.random();
      setToasts((t) => [...t, { id, msg }]);
      setTimeout(() => {
        setToasts((t) => t.filter(x => x.id !== id));
      }, 3000);
    };
  }, []);

  async function refresh() {
    setGraphLoading(true);
    const graphEl = document.getElementById('graph');
    if (graphEl) graphEl.textContent = 'Loading...';
    const g = await listBranches();
    const nodes = g.branches || [];
    const links = (g.edges || []).map(e => ({ source: e[0], target: e[1] }));
    setBranches(nodes);
    updateGraph(nodes.map(n => ({ ...n })), links);
    setGraphLoading(false);
  }

  React.useEffect(() => {
    refresh();
    window.refreshBranches = refresh;
  }, []);

  async function handleCreate() {
    if (loading) return;
    setLoading(true);
    try {
      await createBranch();
      if (window.showToast) window.showToast('Branch created.');
      await refresh();
    } catch (e) {
      if (window.showToast) window.showToast('Create failed.');
    }
    setLoading(false);
  }

  async function handleMerge(id) {
    if (loading) return;
    setLoading(true);
    try {
      await mergeBranch(id);
      if (window.showToast) window.showToast('Merge succeeded.');
      await refresh();
    } catch (e) {
      if (window.showToast) window.showToast('Merge failed.');
    }
    setLoading(false);
  }

  return React.createElement(React.Fragment, null,
    React.createElement('button', { onClick: handleCreate, disabled: loading }, 'New Branch'),
    branches.map(b => React.createElement(BranchNode, {
      key: b.id,
      data: b,
      onMerge: () => handleMerge(b.id),
    })),
    React.createElement(ToastContainer, { toasts })
  );
}

ReactDOM.render(
  React.createElement(ErrorBoundary, null, React.createElement(App)),
  document.getElementById('controls')
);
