/* global d3 */

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
  const links = data.edges.map(e => ({source:e[0], target:e[1]}));
  const sim = d3.forceSimulation(nodes)
    .force('link', d3.forceLink(links).id(d=>d.id).distance(80))
    .force('charge', d3.forceManyBody().strength(-200))
    .force('center', d3.forceCenter(width/2,height/2));
  const link = svg.append('g')
    .selectAll('line')
    .data(links).enter().append('line')
    .attr('stroke','#999');
  const node = svg.append('g')
    .selectAll('circle')
    .data(nodes).enter().append('circle')
    .attr('r',20).attr('fill','#69b')
    .call(d3.drag()
      .on('start', dragstarted)
      .on('drag', dragged)
      .on('end', dragended));
  const lbl = svg.append('g').selectAll('text')
    .data(nodes).enter().append('text')
    .text(d=>d.name).attr('dy',4).attr('text-anchor','middle');
  sim.on('tick',()=>{
    link.attr('x1',d=>d.source.x).attr('y1',d=>d.source.y)
        .attr('x2',d=>d.target.x).attr('y2',d=>d.target.y);
    node.attr('cx',d=>d.x).attr('cy',d=>d.y);
    lbl.attr('x',d=>d.x).attr('y',d=>d.y);
  });
  function dragstarted(event){if(!event.active) sim.alphaTarget(0.3).restart(); event.subject.fx=event.subject.x; event.subject.fy=event.subject.y;}
  function dragged(event){event.subject.fx=event.x; event.subject.fy=event.y;}
  function dragended(event){if(!event.active) sim.alphaTarget(0); event.subject.fx=null; event.subject.fy=null;}
}

 
async function sendAI(){
  const input=document.getElementById('prompt');
  const msg=input.value.trim(); if(!msg) return;
  input.value='';
  const div=document.getElementById('messages');
  const m=document.createElement('div');
  m.textContent='> '+msg; div.appendChild(m);
  const r=await fetch('/ai?q='+encodeURIComponent(msg));
  const data=await r.json();
  const resp=document.createElement('div');
  resp.textContent=data.reply||data.error;
  div.appendChild(resp);
  div.scrollTop=div.scrollHeight;
}

loadGraph().then(setupGraph);

