/* global d3 */
import {listBranches, createBranch, mergeBranch} from './api.js';

async function loadGraph() {
  const resp = await fetch('/graph');
  return await resp.json();
}

function setupGraph(data) {
  const width = document.getElementById('graph').clientWidth;
  const height = document.getElementById('graph').clientHeight;
  const svg = d3.select('#graph').append('svg')
    .attr('width', width)
    .attr('height', height);

  const nodes = data.branches.map(b => Object.assign({}, b));
  const links = data.edges.map(e => ({source: e[0], target: e[1]}));

  const simulation = d3.forceSimulation(nodes)
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
    .on('dblclick', d => openTab(d.name));

  const labels = svg.append('g')
    .selectAll('text')
    .data(nodes)
    .enter().append('text')
    .text(d => d.name)
    .attr('text-anchor', 'middle')
    .attr('dy', 4)
    .style('pointer-events', 'none');

  simulation.on('tick', () => {
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
    if (!event.active) simulation.alphaTarget(0.3).restart();
    event.subject.fx = event.subject.x;
    event.subject.fy = event.subject.y;
  }

  function dragged(event) {
    event.subject.fx = event.x;
    event.subject.fy = event.y;
  }

  function dragended(event) {
    if (!event.active) simulation.alphaTarget(0);
    event.subject.fx = null;
    event.subject.fy = null;
  }
}

function openTab(name) {
  const bar = document.getElementById('tab-bar');
  const content = document.getElementById('tab-content');
  const tab = document.createElement('div');
  tab.className = 'tab';
  tab.textContent = name;
  const pane = document.createElement('div');
  pane.className = 'pane';
  pane.textContent = `Shell for ${name}`;

  tab.onclick = () => setActive(tab, pane);
  tab.oncontextmenu = e => { e.preventDefault(); bar.removeChild(tab); content.removeChild(pane); };

  bar.appendChild(tab);
  content.appendChild(pane);
  setActive(tab, pane);
}

function setActive(tab, pane) {
  document.querySelectorAll('#tab-bar .tab').forEach(t => t.classList.remove('active'));
  document.querySelectorAll('#tab-content .pane').forEach(p => p.classList.remove('active'));
  tab.classList.add('active');
  pane.classList.add('active');
}

async function refresh() {
  const g = await listBranches();
  setupGraph(g);
}

loadGraph().then(setupGraph);
