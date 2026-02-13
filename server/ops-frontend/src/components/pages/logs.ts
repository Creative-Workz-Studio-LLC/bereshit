/**
 * Logs page — stream and view server logs.
 * Service selector, line count, live streaming via SSE.
 */

const SERVICES = [
  'cws-server', 'cloudflared', 'uptime-kuma', 'litestream',
  'prometheus', 'node-exporter', 'grafana-server',
];

export function renderLogs(): HTMLElement {
  const page = document.createElement('div');
  let eventSource: EventSource | null = null;

  page.innerHTML = `
    <div class="page-header">
      <h2>Logs</h2>
      <p>Stream and search server logs in real time</p>
    </div>

    <div class="card mb-4">
      <div style="display: flex; gap: var(--space-3); align-items: center; flex-wrap: wrap;">
        <div>
          <label class="text-muted" style="font-size: var(--font-size-xs); display: block; margin-bottom: var(--space-1);">Service</label>
          <select id="log-service" class="input" style="min-width: 180px;">
            ${SERVICES.map(s => `<option value="${s}">${s}</option>`).join('')}
          </select>
        </div>
        <div>
          <label class="text-muted" style="font-size: var(--font-size-xs); display: block; margin-bottom: var(--space-1);">Lines</label>
          <select id="log-lines" class="input">
            <option value="25">25</option>
            <option value="50" selected>50</option>
            <option value="100">100</option>
            <option value="200">200</option>
          </select>
        </div>
        <div style="align-self: flex-end;">
          <button id="log-fetch" class="btn">Load Logs</button>
          <button id="log-stream" class="btn">Stream Live</button>
          <button id="log-stop" class="btn btn-danger" style="display: none;">Stop</button>
        </div>
        <div style="align-self: flex-end; margin-left: auto;">
          <button id="log-clear" class="btn btn-sm">Clear</button>
        </div>
      </div>
    </div>

    <div class="terminal" id="log-output" style="height: 60vh; overflow-y: auto;"></div>
  `;

  const serviceEl = page.querySelector<HTMLSelectElement>('#log-service')!;
  const linesEl = page.querySelector<HTMLSelectElement>('#log-lines')!;
  const outputEl = page.querySelector<HTMLDivElement>('#log-output')!;
  const fetchBtn = page.querySelector<HTMLButtonElement>('#log-fetch')!;
  const streamBtn = page.querySelector<HTMLButtonElement>('#log-stream')!;
  const stopBtn = page.querySelector<HTMLButtonElement>('#log-stop')!;
  const clearBtn = page.querySelector<HTMLButtonElement>('#log-clear')!;

  fetchBtn.addEventListener('click', async () => {
    const service = serviceEl.value;
    const lines = linesEl.value;
    fetchBtn.disabled = true;
    fetchBtn.textContent = 'Loading...';

    try {
      const resp = await fetch(`/api/ops/logs?service=${service}&lines=${lines}`);
      const data = await resp.json();
      if (data.logs) {
        outputEl.textContent = data.logs;
        outputEl.scrollTop = outputEl.scrollHeight;
      }
    } catch (err) {
      outputEl.textContent = `Error: ${err}`;
    } finally {
      fetchBtn.disabled = false;
      fetchBtn.textContent = 'Load Logs';
    }
  });

  streamBtn.addEventListener('click', () => {
    stopStreaming();

    const service = serviceEl.value;
    const lines = linesEl.value;
    outputEl.textContent = `Streaming ${service} logs...\n\n`;

    streamBtn.style.display = 'none';
    stopBtn.style.display = '';

    eventSource = new EventSource(`/api/ops/logs/stream?service=${service}&lines=${lines}`);
    eventSource.onmessage = (e) => {
      outputEl.textContent += e.data + '\n';
      outputEl.scrollTop = outputEl.scrollHeight;
    };
    eventSource.onerror = () => {
      outputEl.textContent += '\n[Connection closed]\n';
      stopStreaming();
    };
  });

  stopBtn.addEventListener('click', () => stopStreaming());
  clearBtn.addEventListener('click', () => { outputEl.textContent = ''; });

  function stopStreaming() {
    if (eventSource) {
      eventSource.close();
      eventSource = null;
    }
    streamBtn.style.display = '';
    stopBtn.style.display = 'none';
  }

  Object.assign(page, {
    cleanup: () => stopStreaming(),
  });

  return page;
}
