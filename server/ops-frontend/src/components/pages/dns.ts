/**
 * DNS page — view DNS configuration.
 * Cloudflare DNS management requires API token setup on the server side.
 * For now, shows current DNS configuration and links to Cloudflare dashboard.
 */

const DNS_RECORDS = [
  { type: 'CNAME', name: 'creativeworkzstudio.com', content: 'Cloudflare Pages', proxied: true },
  { type: 'CNAME', name: 'www', content: 'Cloudflare Pages', proxied: true },
  { type: 'CNAME', name: 'dashboard', content: 'Cloudflare Tunnel', proxied: true },
  { type: 'CNAME', name: 'cpisi', content: 'Cloudflare Tunnel', proxied: true },
  { type: 'CNAME', name: 'status', content: 'Cloudflare Tunnel', proxied: true },
  { type: 'CNAME', name: 'grafana', content: 'Cloudflare Tunnel', proxied: true },
];

export function renderDNS(): HTMLElement {
  const page = document.createElement('div');

  page.innerHTML = `
    <div class="page-header">
      <h2>DNS</h2>
      <p>Cloudflare DNS records for creativeworkzstudio.com</p>
    </div>

    <div class="card mb-4">
      <div class="card-header">
        <span>DNS Records</span>
        <a href="https://dash.cloudflare.com" target="_blank" class="btn btn-sm">
          Open Cloudflare Dashboard
        </a>
      </div>
      <table class="table">
        <thead>
          <tr>
            <th>Type</th>
            <th>Name</th>
            <th>Content</th>
            <th>Proxy</th>
          </tr>
        </thead>
        <tbody>
          ${DNS_RECORDS.map(r => `
            <tr>
              <td><span class="badge">${r.type}</span></td>
              <td class="mono">${r.name}</td>
              <td class="text-muted">${r.content}</td>
              <td>
                <span class="badge ${r.proxied ? 'badge-success' : 'badge-warning'}">
                  ${r.proxied ? 'proxied' : 'DNS only'}
                </span>
              </td>
            </tr>
          `).join('')}
        </tbody>
      </table>
    </div>

    <div class="card">
      <div class="card-header">
        <span>Architecture</span>
      </div>
      <div style="padding: var(--space-3); font-size: var(--font-size-sm);">
        <pre class="mono text-muted" style="line-height: 1.8;">User → Cloudflare CDN → Tunnel → Oracle VM (:3847)
                              ├── dashboard.creativeworkzstudio.com  →  Go server
                              ├── cpisi.creativeworkzstudio.com      →  Go server
                              ├── status.creativeworkzstudio.com     →  Uptime Kuma (:3001)
                              └── grafana.creativeworkzstudio.com    →  Grafana (:3000)

User → Cloudflare Pages  →  creativeworkzstudio.com (static site)
User → Fly.io (standby)  →  cws-server.fly.dev (auto-stopped)</pre>
      </div>
    </div>
  `;

  return page;
}
