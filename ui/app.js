/* global d3, React, ReactDOM, Recharts */
import {
  listBranches,
  createBranch,
  mergeBranch,
  snapshotBranch,
  deleteBranch,
  getCoverageHistory,
} from './api.js';

// Simple toast queue managed via React
function ToastContainer({ toasts }) {
  return React.createElement(
    'div',
    { id: 'toast-root' },
    toasts.map((t) =>
      React.createElement('div', { key: t.id, className: 'toast-item' }, t.msg)
    )
  );
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

let svg;
let linkGroup;
let nodeGroup;
let labelGroup;
let sim;

function updateGraph(nodes, links) {
  const width = document.getElementById('graph').clientWidth;
  const height = document.getElementById('graph').clientHeight;
  if (!svg) {
    svg = d3.select('#graph').append('svg');
    linkGroup = svg
      .append('g')
      .attr('stroke', '#999')
      .attr('stroke-opacity', 0.6);
    nodeGroup = svg
      .append('g')
      .attr('stroke', '#fff')
      .attr('stroke-width', 1.5);
    labelGroup = svg.append('g');
    sim = d3
      .forceSimulation()
      .force(
        'link',
        d3
          .forceLink()
          .id((d) => d.id)
          .distance(80)
      )
      .force('charge', d3.forceManyBody().strength(-200));
  }
  svg.attr('width', width).attr('height', height);
  sim.force('center', d3.forceCenter(width / 2, height / 2));

  sim.nodes(nodes);
  sim.force('link').links(links);

  const linkSel = linkGroup
    .selectAll('line')
    .data(links, (d) => `${d.source.id}-${d.target.id}`);
  linkSel.exit().remove();
  linkSel.enter().append('line').attr('stroke-width', 2);

  const nodeSel = nodeGroup.selectAll('circle').data(nodes, (d) => d.id);
  nodeSel.exit().remove();
  nodeSel
    .enter()
    .append('circle')
    .attr('r', 20)
    .attr('fill', '#69b')
    .call(
      d3
        .drag()
        .on('start', dragstarted)
        .on('drag', dragged)
        .on('end', dragended)
    )
    .on('click', (event, d) => showMenu(event, d));

  const labelSel = labelGroup.selectAll('text').data(nodes, (d) => d.id);
  labelSel.exit().remove();
  labelSel
    .enter()
    .append('text')
    .attr('text-anchor', 'middle')
    .attr('dy', 4)
    .style('pointer-events', 'none');

  labelGroup.selectAll('text').text((d) => d.name);

  sim.on('tick', () => {
    linkGroup
      .selectAll('line')
      .attr('x1', (d) => d.source.x)
      .attr('y1', (d) => d.source.y)
      .attr('x2', (d) => d.target.x)
      .attr('y2', (d) => d.target.y);

    nodeGroup
      .selectAll('circle')
      .attr('cx', (d) => d.x)
      .attr('cy', (d) => d.y);

    labelGroup
      .selectAll('text')
      .attr('x', (d) => d.x)
      .attr('y', (d) => d.y);
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
  const [running, setRunning] = React.useState(false);
  const start = React.useCallback(() => {
    if (running) return;
    const es = new EventSource(`/branches/${branchId}/agents/stream`);
    setRunning(true);
    es.onmessage = (e) => {
      try {
        const data = JSON.parse(e.data);
        setAgents((a) => [
          ...a.filter((x) => x.agent_id !== data.agent_id),
          data,
        ]);
      } catch {
        // ignore
      }
    };
    es.onerror = () => es.close();
  }, [branchId, running]);

  return React.createElement(
    'div',
    null,
    React.createElement(
      'button',
      { onClick: start, disabled: running },
      'Run Agents'
    ),
    React.createElement(
      'table',
      null,
      React.createElement(
        'tbody',
        null,
        agents.map((a) =>
          React.createElement(
            'tr',
            { key: a.agent_id },
            React.createElement('td', null, a.agent_id),
            React.createElement('td', null, a.status)
          )
        )
      )
    )
  );
}

function CoverageChart({ branchId }) {
  const [data, setData] = React.useState([]);
  React.useEffect(() => {
    getCoverageHistory(branchId)
      .then((h) => setData(h.map((v, i) => ({ i, v }))))
      .catch(() => {});
  }, [branchId]);
  if (!data.length) return null;
  return React.createElement(
    Recharts.LineChart,
    { width: 100, height: 40, data },
    React.createElement(Recharts.Line, {
      type: 'monotone',
      dataKey: 'v',
      stroke: '#8884d8',
      dot: false,
    })
  );
}

function MetricsPanel({ branchId }) {
  const [metrics, setMetrics] = React.useState(null);
  React.useEffect(() => {
    fetch(`/branches/${branchId}/metrics`)
      .then((r) => r.json())
      .then(setMetrics)
      .catch(() => {});
  }, [branchId]);
  if (!metrics) return React.createElement('div', null, 'Loading metrics...');
  return React.createElement(
    'div',
    { className: 'metrics-panel' },
    React.createElement('div', null, `Agents: ${metrics.agents_spawned}`),
    React.createElement(
      'div',
      null,
      `Success: ${Math.round(metrics.success_rate * 100)}%`
    ),
    React.createElement(
      'div',
      null,
      `Avg time: ${metrics.avg_runtime.toFixed(2)}s`
    )
  );
}

function BranchNode({ data, onMerge, onSnapshot, onDelete }) {
  return React.createElement(
    'div',
    { className: 'branch-node' },
    React.createElement('img', {
      src: `/branches/${data.id}/thumbnail.png`,
      width: 80,
      height: 60,
    }),
    `${data.name} `,
    React.createElement('button', { onClick: onSnapshot }, 'Checkpoint'),
    React.createElement('button', { onClick: onMerge }, 'Merge'),
    React.createElement('button', { onClick: onDelete }, 'Delete'),
    React.createElement(CoverageChart, { branchId: data.id }),
    React.createElement(AgentPanel, { branchId: data.id }),
    React.createElement(MetricsPanel, { branchId: data.id })
  );
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
        setToasts((t) => t.filter((x) => x.id !== id));
      }, 3000);
    };
  }, []);

  async function refresh() {
    setGraphLoading(true);
    const graphEl = document.getElementById('graph');
    if (graphEl) graphEl.textContent = 'Loading...';
    const g = await listBranches();
    const nodes = g.branches || [];
    const links = (g.edges || []).map((e) => ({ source: e[0], target: e[1] }));
    setBranches(nodes);
    updateGraph(
      nodes.map((n) => ({ ...n })),
      links
    );
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

  async function handleSnapshot(id) {
    if (loading) return;
    setLoading(true);
    try {
      await snapshotBranch(id);
      if (window.showToast) window.showToast('Checkpoint saved.');
      await refresh();
    } catch {
      if (window.showToast) window.showToast('Snapshot failed.');
    }
    setLoading(false);
  }

  async function handleDelete(id) {
    if (loading) return;
    setLoading(true);
    try {
      await deleteBranch(id);
      if (window.showToast) window.showToast('Branch deleted.');
      await refresh();
    } catch {
      if (window.showToast) window.showToast('Delete failed.');
    }
    setLoading(false);
  }

  return React.createElement(
    React.Fragment,
    null,
    React.createElement(
      'button',
      { onClick: handleCreate, disabled: loading },
      'New Branch'
    ),
    branches.map((b) =>
      React.createElement(BranchNode, {
        key: b.id,
        data: b,
        onMerge: () => handleMerge(b.id),
        onSnapshot: () => handleSnapshot(b.id),
        onDelete: () => handleDelete(b.id),
      })
    ),
    React.createElement(ToastContainer, { toasts })
  );
}

ReactDOM.render(
  React.createElement(ErrorBoundary, null, React.createElement(App)),
  document.getElementById('controls')
);
