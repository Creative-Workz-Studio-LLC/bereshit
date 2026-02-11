# Server Hosting Research

**Date:** 2026-02-09
**Context:** Evaluating free/cheap server hosting alternatives to the Dell Inspiron (192.168.1.99) for the CWS server. Currently using Cloudflare free plan for web hosting + Cloudflare Tunnel.

## Current Server Profile

The CWS server has lightweight requirements:

- Go single-binary with embedded SQLite (pure Go, no CGO)
- WebSocket streaming for real-time dashboard
- No GPU, no external database, no heavy compute
- Storage: SQLite files + logs (a few GB)
- Ingress via Cloudflare Tunnel (no port forwarding)

## Recommendation: Oracle Cloud Free Tier

**Best option for $0/month, permanently free.**

| Resource | Free Allowance |
|---|---|
| ARM Compute (Ampere A1) | 4 OCPUs, 24 GB RAM (up to 4 VMs) |
| x86 Compute (AMD) | 2 VMs, 1/8 OCPU + 1 GB RAM each |
| Block Storage | 200 GB total |
| Object Storage | 20 GB |
| Outbound Bandwidth | 10 TB/month |
| Autonomous Databases | 2x (1 OCPU, 20 GB each) |
| Load Balancer | 1 flexible LB (10 Mbps) |

### Why This Works

- Go cross-compiles to linux/arm64 natively
- SQLite pure-Go driver (modernc.org/sqlite) runs on ARM without CGO
- Cloudflare Tunnel works the same way (install cloudflared on the VM)
- systemd service management identical to current Dell Inspiron setup
- 24 GB RAM is far more than the server currently needs
- 10 TB/month bandwidth is more than sufficient

### Migration Path

1. Sign up at https://www.oracle.com/cloud/free/ (credit card required, never charged)
2. Provision Ampere A1 VM (e.g., 2 OCPUs, 12 GB RAM)
3. Install Go binary: `GOOS=linux GOARCH=arm64 go build`
4. Install cloudflared and configure tunnel
5. Set up systemd service (same as current cws-server.service)
6. Update Cloudflare tunnel to point to new VM
7. Keep Dell Inspiron as fallback/dev server

### Optional: Install Coolify

[Coolify](https://coolify.io/) is an open-source self-hosted PaaS that runs on the Oracle VM.
Provides: git-push deploys, automatic SSL, web dashboard, database management.
Setup guide: https://coolify.io/docs/knowledge-base/server/oracle-cloud

### Caveats

- No SLA on free tier, forum-only support
- ARM instance availability can be limited in popular regions (retry or pick less popular region)
- Account requires valid credit card (not debit/prepaid in some cases)

## Alternative Options

### Expand Cloudflare (Already In Use)

- Workers: 100K requests/day free, but 10ms CPU limit and no WebSockets on free tier
- D1: SQLite-compatible, 5 GB free
- R2: Object storage, 10 GB free
- Paid tier ($5/mo) removes most limits
- **Verdict:** Good supplement but cannot replace the server due to WebSocket requirement

### Google Cloud e2-micro (Free Forever)

- 0.25 vCPU, 1 GB RAM, 30 GB disk
- US regions only
- 1 GB outbound/month (very limited)
- **Verdict:** Works but Oracle is 24x the RAM for the same price ($0)

### Fly.io (~$2-5/month)

- Small shared-cpu VMs with global edge deployment
- Full Docker support
- No free tier for new users (2-hour trial only)
- **Verdict:** Cheap and good for scaling later, not free

### Hetzner Cloud (~$4/month)

- Excellent performance-per-dollar
- European data centers (US available via Ashburn)
- **Verdict:** Best budget VPS when free tiers are outgrown

## Growth Strategy

```
Phase 1 (Now, Free):
  Oracle Cloud ARM VM + Cloudflare Tunnel + Cloudflare free CDN/DNS

Phase 2 (Growing, ~$5/mo):
  Add Cloudflare Workers paid tier for edge API routes
  Keep Oracle VM for WebSocket/stateful workloads

Phase 3 (Scaling, ~$10-15/mo):
  Add Hetzner or Fly.io nodes as secondary servers
  Load balance across regions if needed
```

## Options to Avoid

| Provider | Reason |
|---|---|
| AWS (new accounts) | 6-month credit window, then account closure unless upgraded |
| Railway | No free tier, one-time $5 credit only |
| Render free DB | PostgreSQL deleted after 90 days |
| Fly.io free trial | Only 2 hours of VM runtime |
