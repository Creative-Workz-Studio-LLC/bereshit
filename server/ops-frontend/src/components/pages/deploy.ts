/**
 * Deploy page — deploy code, config, and assets to production.
 * Target cards with streaming output terminal.
 */

const TARGETS: { id: string; label: string; desc: string }[] = [
  { id: 'all', label: 'Full Deploy', desc: 'Build, compile, sync everything, restart' },
  { id: 'binary', label: 'Binary Only', desc: 'Go binary + restart service' },
  { id: 'dash', label: 'Dashboard', desc: 'Frontend + config + themes (no restart)' },
  { id: 'site', label: 'Website', desc: 'Static site to Oracle + Cloudflare Pages' },
  { id: 'builder', label: 'Builder CLI', desc: 'Builder dist + deps + Makefile' },
  { id: 'fly', label: 'Fly.io Standby', desc: 'Deploy to Fly.io failover' },
  { id: 'monitoring', label: 'Monitoring', desc: 'Prometheus + Grafana configs' },
];

export function renderDeploy(): HTMLElement {
  const page = document.createElement('div');
  let eventSource: EventSource | null = null;
  let deploying = false;

  page.innerHTML = `
    <div class="page-header">
      <h2>Deploy</h2>
      <p>Deploy code, config, and assets to production</p>
    </div>

    <div class="card-grid mb-4">
      ${TARGETS.map(t => `
        <div class="card" style="cursor: pointer;" data-target="${t.id}">
          <div class="stat-label">${t.label}</div>
          <div class="text-muted" style="font-size: var(--font-size-xs); margin-top: var(--space-1);">${t.desc}</div>
        </div>
      `).join('')}
    </div>

    <div id="deploy-output-wrap" style="display: none;">
      <div class="card mb-4">
        <div class="card-header">
          <span id="deploy-title">Deploy Output</span>
          <button id="deploy-cancel" class="btn btn-sm btn-danger" style="display: none;">Cancel</button>
        </div>
        <div class="terminal" id="deploy-output" style="height: 40vh; overflow-y: auto;"></div>
      </div>
    </div>
  `;

  const outputWrap = page.querySelector<HTMLDivElement>('#deploy-output-wrap')!;
  const outputEl = page.querySelector<HTMLDivElement>('#deploy-output')!;
  const titleEl = page.querySelector<HTMLSpanElement>('#deploy-title')!;
  const cancelBtn = page.querySelector<HTMLButtonElement>('#deploy-cancel')!;

  // Card click handlers
  TARGETS.forEach(t => {
    const card = page.querySelector(`[data-target="${t.id}"]`);
    if (card) {
      card.addEventListener('click', () => startDeploy(t));
    }
  });

  cancelBtn.addEventListener('click', () => {
    if (eventSource) {
      eventSource.close();
      eventSource = null;
    }
    deploying = false;
    outputEl.textContent += '\n[Cancelled by user]\n';
    cancelBtn.style.display = 'none';
    enableCards(true);
  });

  function startDeploy(target: typeof TARGETS[0]) {
    if (deploying) return;

    const confirmed = confirm(`Deploy ${target.label} to production?`);
    if (!confirmed) return;

    deploying = true;
    enableCards(false);
    outputWrap.style.display = '';
    titleEl.textContent = `Deploying: ${target.label}`;
    cancelBtn.style.display = '';
    outputEl.textContent = '';

    // Use POST with SSE via fetch + ReadableStream (EventSource only does GET)
    fetch(`/api/ops/deploy/${target.id}`, { method: 'POST' })
      .then(async resp => {
        if (!resp.ok) {
          const err = await resp.json();
          outputEl.textContent = `Error: ${err.error}`;
          return;
        }

        const reader = resp.body!.getReader();
        const decoder = new TextDecoder();
        let buffer = '';

        while (true) {
          const { done, value } = await reader.read();
          if (done) break;

          buffer += decoder.decode(value, { stream: true });
          const lines = buffer.split('\n');
          buffer = lines.pop()!; // Keep incomplete line in buffer

          for (const line of lines) {
            if (line.startsWith('data: ')) {
              outputEl.textContent += line.slice(6) + '\n';
              outputEl.scrollTop = outputEl.scrollHeight;
            } else if (line.startsWith('event: done')) {
              outputEl.textContent += '\nDeploy complete.\n';
            }
          }
        }
      })
      .catch(err => {
        outputEl.textContent += `\nError: ${err}\n`;
      })
      .finally(() => {
        deploying = false;
        cancelBtn.style.display = 'none';
        enableCards(true);
      });
  }

  function enableCards(enabled: boolean) {
    page.querySelectorAll<HTMLDivElement>('[data-target]').forEach(card => {
      card.style.opacity = enabled ? '1' : '0.5';
      card.style.pointerEvents = enabled ? 'auto' : 'none';
    });
  }

  Object.assign(page, {
    cleanup: () => {
      if (eventSource) eventSource.close();
    },
  });

  return page;
}
