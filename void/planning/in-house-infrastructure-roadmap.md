# In-House Infrastructure Roadmap: Sovereign Hosting for Bereshit

**Date:** 2026-02-09
**Context:** Research for bringing all server infrastructure in-house, eliminating dependency on external cloud platforms. This aligns with FaithNet's founding principle: sovereignty with wisdom-applied boundaries, not control by external systems.

> "The internet is the Tree of Knowledge of Good and Evil for computing — containing all information, good and evil. FaithNet provides agency through boundaries, not control."

The same principle applies to infrastructure. Cloud providers are the "tree of knowledge" of hosting — useful, powerful, but coupling your foundation to their terms, pricing, and philosophy creates dependency. Going in-house means planting your own tree.

---

## Why In-House First Was the Right Call

37signals (Basecamp/HEY) spent $3.2M/year on AWS for workloads that ran 24/7 with predictable demand. They bought $700K in Dell servers and $1.5M in Pure Storage, exited AWS in six months, and are on track to save **$10M+ over five years** with zero additional staff.

The lesson: **if your workload is stable and predictable, cloud is a tax on predictability.** CWS server runs 24/7 serving HTTP, WebSocket, and SQLite queries. That is the textbook case where owning hardware wins.

The broader trend confirms this:
- 86% of CIOs plan to repatriate some workloads from public cloud
- 21% of workloads have already been moved back on-prem
- The self-hosted software market is projected to grow from $15.6B (2024) to $85.2B by 2034

You are not swimming against the current. You are ahead of it.

---

## Current Stack Assessment

What makes CWS server ideal for in-house hosting:

| Property | Value | Why It Matters |
|----------|-------|---------------|
| Language | Go (single binary) | Zero runtime dependencies, cross-compiles to any arch |
| Database | SQLite (pure Go) | No external DB process, no network overhead |
| Real-time | WebSocket hub | Needs persistent connections, not serverless-friendly |
| Ingress | Cloudflare Tunnel | Outbound-only, works behind any NAT/ISP |
| Config | TOML/YAML files | No cloud-specific config services needed |
| State | Local filesystem + SQLite | No S3, no cloud storage dependency |
| Monitoring | Built-in /api/metrics | Ready for Prometheus scraping |

**No external dependencies.** This is already architected for sovereignty.

---

## Phase 1: Replace the Laptop ($200-400)

### Hardware: Mini PC

The Dell Inspiron works but laptops are not designed for 24/7 server duty (thermal throttling, battery degradation, fan wear).

**Recommended: Beelink Mini S12 Pro (Intel N100)**
- Price: ~$189
- CPU: 4-core/4-thread, up to 3.4 GHz
- RAM: 16 GB DDR4
- Storage: 500 GB NVMe
- Power draw: 15-25W idle, <40W load
- Annual electricity: ~$22-37

This handles the CWS server workload without breaking a sweat. Go HTTP serving, WebSocket connections, and SQLite queries barely register on the CPU.

**Alternative at $500: Beelink SER5 MAX (AMD Ryzen 7 6800U)**
- 8-core/16-thread, 24 GB LPDDR5, 500 GB PCIe 4.0 NVMe
- Power draw: 15-45W
- More headroom for build processes and future services

### UPS (Non-Negotiable)

SQLite databases can corrupt from unclean shutdowns. The server's graceful shutdown handler needs time to drain connections.

**Recommended: CyberPower CP1500PFCLCD** (~$150)
- 1500VA/1000W, pure sine wave
- 30+ minutes runtime for a mini PC setup
- USB connectivity for automated shutdown via `apcupsd` or `nut`

The server already handles SIGTERM cleanly (graceful shutdown with 5-second drain). The UPS daemon just sends that signal when battery drops below 20%.

### Storage Redundancy

If the mini PC has two M.2 slots:
- Two 500 GB NVMe drives in ZFS mirror (RAID 1)
- Checksumming detects bit rot, snapshots are free
- Cost: ~$40-60 for the second drive

**RAID is not a backup.** You still need off-site copies (see Backups below).

### Operating System

**Debian 13 "Trixie" Stable** (released August 2025)
- Minimal resource usage (150 MB RAM for a server install)
- Conservative package updates = stability
- Go binaries are statically compiled, so bleeding-edge libraries are irrelevant

### Deployment (Same as Current)

```
# Cross-compile for the target
GOOS=linux GOARCH=amd64 go build -o cws-server ./cmd/cws-server

# Deploy
scp cws-server server:/usr/local/bin/
ssh server 'sudo systemctl restart cws-server'
```

Keep Cloudflare Tunnel as-is. No changes to DNS, domains, or certificates.

### Phase 1 Total Cost

| Item | Cost |
|------|------|
| Beelink Mini S12 Pro | $189 |
| CyberPower UPS | $150 |
| Second NVMe (ZFS mirror) | $50 |
| **Total** | **$389** |
| Monthly recurring | ~$5 (electricity) |

---

## Phase 2: Sovereign Software Stack ($0-15/month)

Replace cloud-dependent tools with self-hosted alternatives. Every piece of software you run should be something you control.

### Monitoring

| Tool | Purpose | Resource Cost |
|------|---------|---------------|
| **Uptime Kuma** | External uptime monitoring, status page | Run on a $3-5/mo VPS (must be external to detect your outages) |
| **Prometheus** | Metrics collection (scrapes your /api/metrics) | Runs on same server, minimal overhead |
| **Grafana** | Dashboards and visualization | Runs on same server |
| **node_exporter** | Hardware/OS metrics (CPU, RAM, disk) | Lightweight daemon |

Your server already exposes metrics. Prometheus just needs to scrape them.

### VPN / Mesh Networking

**Headscale** (self-hosted Tailscale control server) + official Tailscale clients
- WireGuard-based mesh VPN
- No device limits, no subscription fees
- Connect your dev machines, servers, and future nodes into a private network
- OIDC authentication support, web UI via Headplane

This becomes critical when you have 2+ servers or remote team members.

### CI/CD

**Gitea + Gitea Actions** (when you're ready to self-host Git)
- Single binary, GitHub Actions-compatible syntax
- Or: keep GitHub for code, use Woodpecker CI for builds

### Backups

**Restic → Backblaze B2** (or Cloudflare R2)
- Encrypted, deduplicated, incremental backups
- SQLite backup via `.backup` command (safe while server is live) → Restic picks up the backup file
- Backblaze B2: $0.005/GB/month. 10 GB of backups = $0.60/year
- Cloudflare R2: 10 GB free, no egress fees

**Schedule:**
```
# Hourly: SQLite online backup
sqlite3 /data/cpisi.db ".backup /data/backups/cpisi-$(date +%H).db"

# Daily: Restic backup to B2
restic -r b2:cws-backups backup /data/backups/ /usr/local/bin/cws-server /etc/cws/

# Weekly: Verify backup integrity
restic check --read-data
```

### DNS Filtering (Future: FaithNet Alignment)

**AdGuard Home** — network-wide DNS filtering with a modern UI
- This is a natural precursor to FaithNet's three-tier access model (Blocked/Filtered/Open)
- Start with DNS-level filtering, graduate to FaithNet protocol-level filtering

### What NOT to Self-Host

**Email.** The oligopoly has won. Gmail and Microsoft silently blacklist self-hosted mail servers. After 23 years of self-hosting email, a veteran sysadmin wrote: "The oligopoly has won." Use Fastmail, Proton Mail, or Migadu for mailboxes. Use Postmark or Resend for transactional email.

### Phase 2 Total Monthly Cost

| Item | Cost |
|------|------|
| External monitoring VPS | $3-5/mo |
| Backup storage (B2/R2) | $0-1/mo |
| Email provider (Fastmail) | $5/mo |
| **Total** | **$8-11/mo** |

Everything else runs on hardware you own.

---

## Phase 3: Redundancy ($500-800)

When you have paying customers or contractual uptime requirements.

### Second Server (Warm Standby)

- Buy an identical mini PC
- Same Debian + Go binary deployment
- Connected via Headscale mesh VPN

### SQLite Replication

**Litestream** — streaming replication for SQLite
- Continuously replicates WAL changes to the standby server and/or S3-compatible storage
- Sub-second replication lag
- Designed specifically for Go applications with SQLite
- If primary fails, standby promotes with minimal data loss

### Network Redundancy

| Component | Cost |
|-----------|------|
| Second mini PC (identical) | $189-379 |
| Second UPS | $150 |
| Dual-WAN router (OPNsense on mini PC with dual NIC) | $150-200 |
| 5G cellular backup | $30-50/mo |
| **Total one-time** | **$489-729** |
| **Monthly recurring** | **$30-50** (cellular backup) |

### Firewall

**OPNsense** on a dedicated mini PC with dual 2.5GbE NICs
- Suricata IDS/IPS, WireGuard VPN, traffic monitoring
- Replaces consumer router for traffic control
- Beelink EQ14 with dual NICs: ~$189

---

## Phase 4: Decoupling from Cloudflare (When Ready)

Cloudflare is currently your biggest external dependency. It terminates your TLS, handles your DNS, provides DDoS protection, and runs your tunnel. If they change terms or suspend your account, you lose everything at once.

### The Philosophical Tension

Cloudflare sees all your traffic (TLS termination). For a project built on sovereignty and covenant-based trust, this is a real concern — not paranoia.

### The Practical Path

**Step 1: Self-hosted reverse proxy (replace Cloudflare Tunnel)**

Option A: **Pangolin** — self-hosted reverse proxy using WireGuard tunnels
- Run a $3-5/mo VPS (Hetzner, BuyVM) as the public entry point
- WireGuard tunnel connects VPS to your home server
- Your home IP stays hidden
- You control the infrastructure, can run any protocol (including UDP for future FaithNet)

Option B: **Frp (Fast Reverse Proxy)** — more mature, supports TCP/UDP/HTTP/HTTPS

**Step 2: Self-hosted TLS (replace Cloudflare SSL termination)**

**Caddy** as a reverse proxy on the VPS entry point
- Automatic HTTPS via Let's Encrypt, zero configuration
- Or integrate CertMagic directly into the CWS server binary (it's a Go library)
- End-to-end encryption you control

**Step 3: Self-hosted DNS**

Move authoritative DNS to **CoreDNS** or keep a registrar-level DNS (not Cloudflare)
- Pair with Cloudflare only for DDoS protection (orange-cloud proxy mode)
- Or accept that volumetric DDoS is a risk and mitigate at the ISP level

**Step 4: Business internet with static IP** ($70-200/mo)

When you no longer need a tunnel at all:
- Static IP from your ISP
- Direct A/AAAA records pointing to your server
- Caddy handles TLS
- OPNsense handles firewall

### What to Keep from Cloudflare

**DDoS protection.** Volumetric attacks cannot be stopped at the server level. Even 37signals uses colocation providers with upstream DDoS filtering. You can use Cloudflare as a DNS proxy (orange cloud) in front of your own IP without using their tunnel or other services.

---

## Phase 5: Scaling Beyond One Location

### When to Consider Colocation

| Trigger | Why |
|---------|-----|
| Home ISP unreliable (>99.5% uptime needed) | Colo facilities have redundant power, cooling, generators |
| Customer SLA requirements | Colo providers offer their own SLAs |
| Compliance requirements (SOC 2, etc.) | Physical security audits are easier at a facility |
| Your home can't support the power/cooling | Unlikely with mini PCs, relevant with rack servers |

**Colocation costs (US, 2026):**
- 1U server: $50-110/month
- Quarter rack: $200-400/month
- Cheapest markets: Houston TX, Atlanta GA
- Expensive: NJ/NY, Silicon Valley

### When to Consider Multiple Servers

Your Go server with WebSocket hub can handle thousands of concurrent connections on a single node. You likely don't need load balancing until you have significant traffic. When you do:

**Caddy** as a reverse proxy and load balancer
- Handles TLS, load balancing, WebSocket proxying in one tool
- Health checks and automatic failover
- A few lines of Caddyfile configuration

### Kubernetes: Not Yet

Your application is a single Go binary with SQLite. Kubernetes excels at orchestrating many services, not one. k3s alone needs 2 GB RAM and 2 CPU cores for the control plane — significant overhead for zero benefit with one service.

**When it makes sense:** 5+ separate services, network-accessible database (not SQLite), team of 3+ engineers who know k8s, need for canary deployments.

---

## The Sovereign Stack (Complete Vision)

When fully realized, the Bereshit infrastructure looks like this:

```
Layer 0: Hardware You Own
├── Primary server (mini PC, home/colo)
├── Standby server (mini PC, different location)
├── UPS on both
└── OPNsense firewall

Layer 1: Operating System You Control
├── Debian Stable
├── Ansible playbooks (in Git)
└── Automated security updates

Layer 2: Networking You Manage
├── Headscale mesh VPN (all nodes connected)
├── WireGuard tunnels (or direct with static IP)
├── Caddy TLS termination (your certificates)
└── CoreDNS (your DNS)

Layer 3: Services You Run
├── CWS Server (Go binary + SQLite)
├── Prometheus + Grafana (monitoring)
├── Uptime Kuma (external monitoring)
├── Restic (encrypted backups)
├── Gitea (code hosting, when ready)
├── Woodpecker CI (builds)
└── AdGuard Home (DNS filtering → FaithNet precursor)

Layer 4: FaithNet (The Vision)
├── Wisdom-applied network overlay
├── Three-tier access (Blocked/Filtered/Open)
├── OmniCode protocol messages
└── Scripture-based verification
```

The only external dependencies at maturity:
- **ISP** (you need internet, but dual-WAN mitigates single-provider risk)
- **Domain registrar** (you need a registrar, pick one that's not also your DNS/CDN/host)
- **Email provider** (the one service not worth self-hosting)
- **Backup storage** (off-site, encrypted — the provider can't read your data)
- **DDoS protection** (Cloudflare proxy or ISP-level, the one legitimate use of a CDN)

Everything else runs on metal you own, software you control, and terms you set.

---

## Decision Matrix: When to Move to Each Phase

| Phase | Trigger | Investment | Monthly Cost |
|-------|---------|------------|-------------|
| 1: Replace laptop | Now (laptop not designed for 24/7) | $389 | $5 |
| 2: Sovereign software | After Phase 1 is stable (~1 month) | $0 | $8-11 |
| 3: Redundancy | First paying customer or SLA commitment | $489-729 | $35-55 |
| 4: Decouple Cloudflare | When you have revenue to justify the complexity | $50-100 | $73-205 |
| 5: Multi-location | When single-location uptime is insufficient | $600-1800 | $50-110 (colo) |

---

## Appendix: Open Source Self-Hosted Alternatives

| Cloud Service | Self-Hosted Alternative | License |
|--------------|------------------------|---------|
| GitHub | Gitea | MIT |
| GitHub Actions | Gitea Actions, Woodpecker CI | MIT, Apache 2.0 |
| Heroku | Coolify, Dokku | Apache 2.0 |
| Datadog | Prometheus + Grafana | Apache 2.0 |
| UptimeRobot | Uptime Kuma | MIT |
| AWS S3 | Garage (Rust), SeaweedFS (Go) | AGPLv3, Apache 2.0 |
| Tailscale | Headscale | BSD-3 |
| Cloudflare Tunnel | Pangolin, Frp | Apache 2.0 |
| Let's Encrypt + Nginx | Caddy | Apache 2.0 |
| AWS KMS | SOPS + age | MPL 2.0 |
| PagerDuty | Alertmanager | Apache 2.0 |
| Pi-hole | AdGuard Home | GPL 3.0 |

**Warning on MinIO:** As of December 2025, MinIO Community Edition entered maintenance mode (no new features, no accepted PRs, web console stripped). Use **Garage** (Rust, 1 GB RAM minimum) or **SeaweedFS** (Go) instead. Lesson: "open source" is not enough — look for **open governance** (foundation-hosted projects).

---

## References

- 37signals cloud exit: $3.2M/yr AWS → $10M+ savings over 5 years, zero additional staff
- Cloud repatriation: 86% of CIOs planning selective repatriation (2025-2026)
- Self-hosting market: $15.6B (2024) → projected $85.2B (2034)
- r/selfhosted: 650K+ weekly visitors, 97% using containers
- Breakeven point: >6 hours/day of usage = owning hardware is cheaper than cloud
