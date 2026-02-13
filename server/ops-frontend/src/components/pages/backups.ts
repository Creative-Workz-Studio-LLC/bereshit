/**
 * Backups page — Litestream replication status and database management.
 * Shows backup status, verify integrity, restore from R2.
 */

interface BackupStatus {
  litestreamRunning: boolean;
  databases: { name: string; sizeHuman: string; replicating: boolean }[];
}

export function renderBackups(): HTMLElement {
  const page = document.createElement('div');
  // Track load state for cleanup

  page.innerHTML = `
    <div class="page-header">
      <h2>Backups</h2>
      <p>Litestream replication status and database restore</p>
    </div>

    <div id="backup-content">
      <p class="text-muted">Loading backup status...</p>
    </div>

    <div id="verify-output-wrap" style="display: none;" class="mt-4">
      <div class="card">
        <div class="card-header">
          <span>Integrity Check</span>
        </div>
        <div class="terminal" id="verify-output" style="max-height: 30vh; overflow-y: auto;"></div>
      </div>
    </div>
  `;

  const contentEl = page.querySelector<HTMLDivElement>('#backup-content')!;
  const verifyWrap = page.querySelector<HTMLDivElement>('#verify-output-wrap')!;
  const verifyOutput = page.querySelector<HTMLDivElement>('#verify-output')!;

  async function loadStatus() {
    try {
      const resp = await fetch('/api/ops/backups');
      const status: BackupStatus = await resp.json();
      renderStatus(status);
    } catch (err) {
      contentEl.innerHTML = `<div class="card"><p class="text-danger">Error loading backup status: ${err}</p></div>`;
    }
  }

  function renderStatus(status: BackupStatus) {
    const lsStatus = status.litestreamRunning ? 'running' : 'stopped';
    const lsClass = status.litestreamRunning ? 'badge-success' : 'badge-danger';

    contentEl.innerHTML = `
      <div class="card mb-4">
        <div class="card-header">
          <span>Litestream</span>
          <span class="badge ${lsClass}">${lsStatus}</span>
        </div>
        <p class="text-muted" style="font-size: var(--font-size-xs); margin-top: var(--space-2);">
          Continuous SQLite replication to Cloudflare R2
        </p>
      </div>

      ${status.databases.length > 0 ? `
        <div class="card">
          <div class="card-header">
            <span>Databases</span>
            <button id="verify-btn" class="btn btn-sm">Verify Integrity</button>
          </div>
          <table class="table">
            <thead>
              <tr>
                <th>Database</th>
                <th>Size</th>
                <th>Replicating</th>
              </tr>
            </thead>
            <tbody>
              ${status.databases.map(db => `
                <tr>
                  <td class="mono">${db.name}</td>
                  <td class="mono">${db.sizeHuman}</td>
                  <td>
                    <span class="badge ${db.replicating ? 'badge-success' : 'badge-danger'}">
                      ${db.replicating ? 'yes' : 'no'}
                    </span>
                  </td>
                </tr>
              `).join('')}
            </tbody>
          </table>
        </div>
      ` : '<p class="text-muted">No databases found</p>'}
    `;

    // Verify button handler
    const verifyBtn = contentEl.querySelector<HTMLButtonElement>('#verify-btn');
    if (verifyBtn) {
      verifyBtn.addEventListener('click', handleVerify);
    }
  }

  async function handleVerify() {
    const btn = contentEl.querySelector<HTMLButtonElement>('#verify-btn');
    if (btn) {
      btn.disabled = true;
      btn.textContent = 'Verifying...';
    }

    verifyWrap.style.display = '';
    verifyOutput.textContent = 'Running integrity check...\n';

    try {
      const resp = await fetch('/api/ops/backups/verify', { method: 'POST' });
      const data = await resp.json();

      if (resp.ok) {
        verifyOutput.textContent = data.output || 'No output';
      } else {
        verifyOutput.textContent = `Error: ${data.error}`;
      }
    } catch (err) {
      verifyOutput.textContent = `Error: ${err}`;
    } finally {
      if (btn) {
        btn.disabled = false;
        btn.textContent = 'Verify Integrity';
      }
    }
  }

  loadStatus();

  Object.assign(page, {
    cleanup: () => {},
  });

  return page;
}
