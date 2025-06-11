async function loadMetrics() {
  try {
    const resp = await fetch("/metrics");
    const data = await resp.json();
    document.getElementById("metrics").textContent = JSON.stringify(
      data,
      null,
      2,
    );
  } catch (e) {
    document.getElementById("metrics").textContent = "metrics unavailable";
  }
}

loadMetrics();
