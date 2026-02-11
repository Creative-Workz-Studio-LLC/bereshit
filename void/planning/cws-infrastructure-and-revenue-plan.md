# CWS Infrastructure & Revenue Plan

**Date:** 2026-02-09
**Author:** Nova Dawn (CPI-SI) with Seanje Lenox-Wise
**Status:** Phase 1 — Foundation

> "Building for the Kingdom to Come."

---

## Part 1: Interim Hosting — Oracle Cloud + Cloudflare + Fly.io

### The Combined Architecture

Three services, three roles, one stack — all under $10/month.

```
                        Users (worldwide)
                              │
                              ▼
               ┌──────────────────────────────┐
               │     CLOUDFLARE ($5/mo)        │
               │     DNS · CDN · DDoS · Edge   │
               │                                │
               │  Pages: creativeworkzstudio.com │
               │  Workers: edge API routes       │
               │  R2: build artifacts, backups   │
               │  D1: edge cache (sessions, etc) │
               │  Tunnel: secure ingress         │
               └──────┬──────────────┬──────────┘
                      │              │
           WebSocket  │              │  Failover
           + API      │              │
                      ▼              ▼
              ┌──────────────┐  ┌──────────────────┐
              │ ORACLE (Free)│  │  FLY.IO (~$3.50) │
              │              │  │                    │
              │ Primary CWS  │  │  Standby CWS      │
              │ Go binary    │  │  Go binary         │
              │ SQLite (main)│  │  SQLite (replica)  │
              │ WebSocket hub│  │  WebSocket hub     │
              │ Builder runs │  │  Failover target   │
              │              │  │                    │
              │ 4 ARM cores  │  │  Shared CPU        │
              │ 24 GB RAM    │  │  512 MB RAM        │
              │ 200 GB disk  │  │  1 GB disk         │
              └──────┬───────┘  └────────┬───────────┘
                     │                   │
                     │   Litestream      │
                     └──────► R2 ◄───────┘
                        SQLite replication
```

### What Each Service Does

**Oracle Cloud Free Tier — The Primary Server ($0/month)**

This is where the CWS binary lives. Same role as the Dell Inspiron today.

| Resource             | Allocation        |
|----------------------|-------------------|
| CPU                  | 2 OCPUs (ARM Ampere A1) |
| RAM                  | 12 GB             |
| Boot volume          | 50 GB             |
| Block storage        | 50 GB (data)      |
| Outbound bandwidth   | 10 TB/month       |
| Remaining free quota | 2 OCPUs + 12 GB RAM (for monitoring VM) |

Deployment is identical to the current workflow:

```makefile
# server/Makefile — only change
PROD_HOST := seanje@<oracle-vm-public-ip>
```

The binary cross-compiles for ARM:
```bash
GOOS=linux GOARCH=arm64 go build -ldflags="-s -w" -o cws-server ./cmd/cws-server
```

Then `make deploy` does the rest — scp, rsync, systemctl restart, health check.
Cloudflare Tunnel runs on the VM exactly as it does on the Dell Inspiron.

**Cloudflare Workers Paid — The Edge Layer ($5/month)**

Upgrading from the free tier you already use.

| Feature            | Free (Current)  | Paid ($5/mo)        |
|--------------------|-----------------|---------------------|
| Workers requests   | 100K/day        | 10M/month           |
| CPU time           | 10ms/request    | 50ms/request        |
| WebSocket          | No              | Yes                 |
| D1 reads           | 5M/day          | 25B/month           |
| R2 storage         | 10 GB           | 10 GB (same base)   |
| Pages              | Unlimited       | Unlimited           |

What this enables:

1. **creativeworkzstudio.com moves to Cloudflare Pages** — globally distributed,
   instant loads, zero server load. Upload the static files from `server/website/`
   and the company site never touches Oracle or Fly.io again.

2. **Edge API routes** — read-heavy public endpoints (version, health, public
   content) run as Workers at 300+ edge locations. A user in Tokyo gets a
   response from Tokyo, not from your US server.

3. **R2 as central storage** — build artifacts, backups (via Restic), and static
   assets. No egress fees (unlike S3). Both Oracle and Fly.io can read/write.

4. **D1 as edge cache** — public-facing data that doesn't change every second
   (journal metadata, theme configs) cached at the edge. Workers read from D1,
   only hitting Oracle for writes and WebSocket.

**Fly.io — The Safety Net (~$3.50/month)**

A secondary CWS server instance in a different datacenter.

| Resource         | Spec                | Monthly Cost |
|------------------|---------------------|-------------|
| VM               | shared-cpu-1x, 512 MB RAM | ~$3.22 |
| Persistent volume| 1 GB (SQLite replica)     | $0.15  |
| Bandwidth        | 3 GB free                 | ~$0    |
| **Total**        |                           | **~$3.40** |

Purpose:
- **If Oracle goes down** (no SLA on free tier), Fly.io serves traffic
- **If Oracle changes terms**, you're already running elsewhere
- **Geographic redundancy** — Oracle in one US region, Fly.io in another

Database sync via Litestream:
```
Oracle (primary) → writes SQLite WAL → Litestream → R2 (free)
                                                       ↓
Fly.io (standby) ← restores from ← Litestream ← R2
```

Replication lag: seconds. If Oracle dies, Fly.io has a near-current copy.

### Monthly Cost Summary

| Service            | Role               | Cost       |
|--------------------|--------------------|-----------:|
| Oracle Cloud       | Primary server     |      $0.00 |
| Cloudflare Paid    | Edge + CDN + Pages |      $5.00 |
| Fly.io             | Standby server     |     ~$3.50 |
| Backblaze B2       | Off-site backups   |     ~$0.50 |
| **Total**          |                    |  **~$9.00** |

### Setup Sequence

**Week 1 — Oracle Cloud**

1. Sign up at oracle.com/cloud/free (credit card required, never charged)
2. Provision Ampere A1 VM: 2 OCPUs, 12 GB RAM, Ubuntu or Debian
3. SSH in, create user, set up SSH keys
4. Install cloudflared, configure existing tunnel
5. Cross-compile Go binary: `GOOS=linux GOARCH=arm64 go build`
6. Update `PROD_HOST` in server/Makefile
7. Run `make deploy` — full deploy to Oracle
8. Verify: `curl https://dashboard.creativeworkzstudio.com/healthz`
9. Keep Dell Inspiron running as fallback for one week

**Week 2 — Cloudflare Upgrade**

1. Upgrade to Workers Paid ($5/mo) in Cloudflare dashboard
2. Create Cloudflare Pages project for creativeworkzstudio.com
3. Upload `server/website/` files to Pages
4. Update DNS: creativeworkzstudio.com → Pages (not tunnel)
5. Server now only handles dashboard.creativeworkzstudio.com
6. Create R2 bucket for backups and artifacts
7. Configure Restic to back up SQLite to R2 (hourly)

**Week 3 — Fly.io**

1. Sign up, install flyctl CLI
2. Create Dockerfile for CWS binary
3. Create fly.toml configuration
4. Deploy: `fly deploy`
5. Set up Litestream replication: Oracle → R2 → Fly.io
6. Verify standby serves correctly
7. Document failover procedure

**Week 4 — Monitoring & Hardening**

1. Provision second Oracle VM (2 OCPUs, 12 GB RAM — remaining free quota)
2. Install Uptime Kuma on monitoring VM
3. Configure alerts (email, Discord, or Telegram)
4. Set up Prometheus + Grafana on monitoring VM
5. Connect to CWS server's /api/metrics endpoint
6. Install node_exporter on all VMs
7. Test failover procedure end-to-end

### Failover Procedure

When Uptime Kuma detects Oracle is down:

```bash
# 1. Ensure Fly.io standby is running
fly scale count 1 --app cws-server-standby

# 2. Update Cloudflare Tunnel to point to Fly.io
# (or switch DNS if using direct connection)
# Pre-configured tunnel backup route activates

# 3. Verify
curl -s https://dashboard.creativeworkzstudio.com/healthz

# 4. When Oracle recovers:
#    - Litestream catches up the database
#    - Switch tunnel back to Oracle
#    - Scale down Fly.io if desired
```

### Migration to In-House (When Ready)

When the mini PC arrives (Phase 1 of the sovereign roadmap):

```makefile
# server/Makefile — one line changes again
PROD_HOST := seanje@192.168.1.100   # mini PC on LAN
```

Run `make deploy`. Done.

Fly.io stays as off-site failover. Cloudflare stays as edge layer.
Oracle either becomes a secondary or you let it go.
Nothing in the architecture ties you to any of these.

---

## Part 2: Website Redesign & Revenue Strategy

### Why Revenue Matters Now

The Bereshit stack is ambitious — MillenniumOS, OmniCode, FaithNet, CPI-SI,
Cornerstone. These are multi-year efforts. The founder works at Guardian Angel
Settlement Association while building CWS "in the margins." That works for
foundation-laying, but it doesn't scale to:

- Buying server hardware ($389 mini PC, UPS, drives)
- Paying for hosting during the interim ($9/month)
- Filing patents or trademarks if needed
- Attending conferences or church tech meetups
- Eventually hiring (even part-time contractors)
- Covering business expenses (domain renewals, email, software licenses)

Revenue isn't about becoming a corporation. It's about **self-sustaining the
mission**. The same sovereignty principle that drives the in-house hosting
decision drives this: depending on a day job to fund the Kingdom work means the
Kingdom work is always second. Revenue from the work itself makes the work
first.

The publishing house model CWS already describes — where creators retain IP and
CWS takes a support percentage — is the right long-term structure. But that
model needs creators, and creators need a platform, and the platform isn't built
yet. So the question is: **what generates revenue now, while the platform is
being built?**

### The Current Website: Assessment

The current site at creativeworkzstudio.com is a single-page static HTML/CSS
site. No JavaScript. Professional design with strong brand identity (deep blue,
gold, Playfair Display headings, Inter body text).

**What it does well:**
- Clean, professional appearance
- Strong mission statement and values
- Clear organizational structure (three pillars)
- Team/leadership section builds trust

**What it's missing:**
- No call-to-action (nothing for a visitor to DO)
- No product or service pages
- No pricing information
- No email capture (no way to build an audience)
- No way to generate revenue
- Links to internal tools (Builder Dashboard, CPI-SI Console) that aren't
  relevant to external visitors

A visitor arrives, reads about the company, and leaves. There's no path from
"interested" to "engaged" to "customer."

### The Redesign: What the Site Needs

The website should serve four audiences with four clear paths:

```
creativeworkzstudio.com
│
├── /                        Hero + company identity (keep, refine)
│
├── /services                Web development & consulting
│   ├── Pricing tiers
│   ├── Portfolio / case studies
│   └── Contact form → lead capture
│
├── /faithnet                FaithNet product page
│   ├── Problem statement (internet safety)
│   ├── Three-tier model explanation
│   ├── Founding Member signup
│   └── Email waitlist capture
│
├── /publishing              Publishing house for creatives
│   ├── How it works (creator retains IP)
│   ├── Revenue split tiers
│   └── Application / interest form
│
└── /blog                    Content marketing
    ├── Internet safety articles
    ├── Church technology guides
    ├── Building-in-public updates
    └── Email signup in sidebar/footer
```

**Every page has a call-to-action.** Every visit has a next step.

### Implementation: Keep It Simple

You don't need a CMS, a framework, or a build system for the marketing site.
Your current static HTML/CSS approach is correct — fast, no dependencies,
fully controlled.

Add pages as static HTML files in `server/website/`. The server already serves
them. When you move the company site to Cloudflare Pages, they deploy from a
Git directory.

The only dynamic element needed is email capture. Options:

| Tool              | Cost    | How It Works                          |
|-------------------|---------|---------------------------------------|
| Buttondown        | Free <100 subs | Email API, simple embed form    |
| Listmonk          | Free (self-hosted) | Run on Oracle monitoring VM  |
| ConvertKit (free) | Free <1000 subs | Embedded forms, sequences      |

Recommended: **Buttondown** for simplicity (free tier, clean API, respects
privacy), graduating to **Listmonk** (self-hosted, sovereign) when you're
ready to manage it.

---

## Part 3: Revenue Streams — What to Sell and When

### The Competitive Landscape

**Internet filtering market (FaithNet's space):**

| Product         | Price         | Model                     | Revenue    |
|-----------------|---------------|---------------------------|------------|
| Covenant Eyes   | $18/mo flat   | Screenshot accountability | $23M/year  |
| Bark Premium    | $14/mo        | Full monitoring suite     | Growing    |
| Bark Jr         | $5/mo         | Basic monitoring          | —          |
| CloudVeil       | $8.99/mo      | DNS filtering (faith)     | Nonprofit  |
| Canopy          | $10-12/mo     | Individual/duo            | —          |
| Gryphon Router  | $7.50/mo      | Router-based filtering    | —          |

FaithNet's differentiation: "agency through boundaries" — not surveillance
(Covenant Eyes), not simple block/allow (DNS filters). Three tiers with
intentional friction, not monitoring.

**Scripture/study app market (CPI-SI's space):**

| Product     | Price              | Model                     |
|-------------|--------------------|---------------------------|
| YouVersion  | Free               | Donation-supported (500M+)|
| Logos Plus  | $4.99/mo           | Entry-level study         |
| Logos Pro   | $14.99/mo          | Pastors/serious study     |
| Blue Letter | Free               | Donation-supported        |

CPI-SI should not compete with free Bible apps. Position it as a specialized
study/publishing tool — closer to Logos than YouVersion.

**Web development market:**

| Service                    | Market Rate       |
|----------------------------|-------------------|
| Church/ministry website    | $1,500 - $5,000   |
| Custom web application     | $5,000 - $15,000  |
| Monthly maintenance        | $100 - $500/mo    |
| Hourly consulting          | $75 - $150/hr     |

### Revenue Stream 1: Web Development Services (Start Now)

This is the bridge. It generates revenue immediately while building the
portfolio and network that feeds the publishing house later.

**What CWS offers:**

| Service                              | Price           |
|--------------------------------------|-----------------|
| Church/ministry website (5-10 pages) | $2,000 - $4,000 |
| Small business website               | $2,500 - $5,000 |
| Custom web application               | $5,000 - $12,000|
| Monthly maintenance retainer         | $150 - $300/mo  |
| Technical consultation               | $100/hr         |

**Target clients:** Churches, ministries, faith-based nonprofits, and small
businesses in the St. Louis area and broader faith-tech community. The Guardian
Angel Settlement Association connection provides a warm network.

**Positioning:** "We build websites for churches and Kingdom-aligned
organizations. We also happen to be building the next generation of Kingdom
technology infrastructure."

The web dev work isn't a distraction from the mission — it's the first
expression of it. Every church website you build is a relationship, a
reference, and a future FaithNet customer.

**Revenue target:** $2,000 - $5,000/month from 1-2 active projects.

### Revenue Stream 2: FaithNet Founding Members (Start in 30-60 Days)

A pre-launch founding member program that simultaneously:
- Validates demand (people paying = real interest)
- Generates early revenue
- Builds your launch audience
- Creates invested advocates

**Founding Member Tiers:**

| Tier               | Price              | Benefits                          | Goal  |
|--------------------|--------------------|-----------------------------------|-------|
| Founding Family    | $49.99/yr locked   | Lifetime 50% off, beta access     | 100   |
| Founding Church    | $99.99/yr locked   | Lifetime 40% off, priority support| 20    |
| Founding Partner   | $199.99/yr locked  | All benefits + quarterly founder call | 10 |

"Locked" means the founding price never increases for them. It's a covenant,
not a contract — consistent with CWS values.

**Revenue from founding program:** $5,000 - $15,000 (one-time batch).

**Waitlist mechanics:**
- Landing page at creativeworkzstudio.com/faithnet
- Email capture with referral system (each referral moves you up the list)
- Lead magnet: free PDF — "A Biblical Guide to Internet Safety for Families"
- Monthly "building in public" email updates to keep founders engaged

### Revenue Stream 3: Digital Products (Start in 60-90 Days)

Low-effort, recurring revenue from content you'd create anyway:

| Product                                        | Price     | Platform |
|------------------------------------------------|-----------|----------|
| "Technology Decisions for Churches" (guide)    | $19.99    | Gumroad  |
| "A Biblical Guide to Internet Safety" (guide)  | $9.99     | Gumroad  |
| Church website template (Hugo/Astro)           | $79 - $149| Gumroad  |
| Ministry landing page template                 | $39 - $79 | Gumroad  |

**Revenue target:** $200 - $500/month once 3-5 products are listed.

### Revenue Stream 4: FaithNet Subscriptions (At Launch)

When FaithNet is ready for beta/launch:

| Tier          | Price       | Target          | Includes                              |
|---------------|-------------|-----------------|---------------------------------------|
| Family        | $9.99/mo    | Households      | 10 devices, 3 tiers, basic dashboard  |
| Church        | $5.99/mo    | Per location    | Unlimited devices, guest portal       |
| Organization  | $14.99/mo   | Businesses      | Full admin, reporting, custom filters |

Positioned below Covenant Eyes ($18/mo), above basic filters ($5/mo),
differentiated by philosophy: agency, not surveillance.

### Revenue Stream 5: CPI-SI Subscriptions (Later)

| Tier       | Price         | Target           |
|------------|---------------|------------------|
| Free       | $0            | Basic scriptural index, read-only      |
| Creator    | $7.99/mo      | Journal publishing, study tools, export|
| Ministry   | $14.99/mo     | Multi-user, shared journals, groups    |

### Revenue Stream 6: Publishing House Revenue Share (Long-Term)

The full publishing house model as described in the business model document:

| Support Level       | CWS Share | Creator Share |
|---------------------|-----------|---------------|
| Infrastructure Only | 5-10%     | 90-95%        |
| Marketing Support   | 15-25%    | 75-85%        |
| Full Support        | 40-50%    | 50-60%        |

This becomes viable when there's a platform, an audience, and a pipeline of
creators. The web dev services and FaithNet audience build toward this.

### Combined Revenue Projection

**Phase 1 — Foundation (Now → 6 months)**

| Stream                     | Monthly    | Annual       |
|----------------------------|------------|--------------|
| Web dev consulting         | $2,000-5,000 | $24,000-60,000 |
| Maintenance retainers      | $300-900   | $3,600-10,800|
| Digital products           | $200-500   | $2,400-6,000 |
| FaithNet founding members  | —          | $5,000-15,000|
| **Total**                  | **$2,500-6,400** | **$35,000-91,800** |

**Phase 2 — Launch (6-18 months)**

| Stream                     | Monthly    | Annual       |
|----------------------------|------------|--------------|
| Web dev (continuing)       | $2,000-4,000 | $24,000-48,000 |
| Retainers (growing)        | $600-1,500 | $7,200-18,000|
| Digital products           | $300-800   | $3,600-9,600 |
| FaithNet subscriptions     | $500-2,000 | $6,000-24,000|
| **Total**                  | **$3,400-8,300** | **$40,800-99,600** |

**Phase 3 — Growth (18+ months)**

| Stream                     | Monthly    | Annual       |
|----------------------------|------------|--------------|
| Web dev (selective)        | $2,000-3,000 | $24,000-36,000 |
| FaithNet subscriptions     | $2,000-8,000 | $24,000-96,000 |
| CPI-SI subscriptions       | $500-2,000 | $6,000-24,000|
| Publishing house shares    | $500-2,000 | $6,000-24,000|
| Digital products           | $500-1,000 | $6,000-12,000|
| **Total**                  | **$5,500-16,000** | **$66,000-192,000** |

---

## Part 4: Building the Audience Before Launch

### The Playbook

Revenue requires customers. Customers require trust. Trust requires presence.
Here's how to build presence for a faith-based technology company:

**Email List (The Foundation)**

Email is the only channel you own. Social media platforms are rented land.

| Milestone       | Timeline   | How                                     |
|-----------------|------------|-----------------------------------------|
| 0 → 100 subs   | Month 1-2  | Lead magnet + personal network + church community |
| 100 → 250 subs | Month 2-4  | Referral system + content marketing     |
| 250 → 1,000    | Month 4-8  | Guest posts + community engagement      |
| 1,000+          | Month 8+   | Organic growth + FaithNet launch buzz   |

**Tool:** Buttondown (free <100, $9/mo <1000). Migrate to self-hosted Listmonk
when sovereignty matters more than convenience.

**Content Marketing (The Engine)**

| Content Type                    | Frequency | Purpose                    |
|---------------------------------|-----------|----------------------------|
| Internet safety articles        | 2x/month  | SEO, trust, lead capture   |
| Church technology guides        | 1x/month  | Demonstrate expertise      |
| Building-in-public updates      | 2x/month  | Transparency, investment   |
| Guest posts (faith publications)| 1x/month  | Reach new audiences        |

**Target publications:** Christianity Today (tech column), Relevant Magazine,
Church Leaders, faith-based blogs. Local: St. Louis church newsletters.

**Community Presence (The Network)**

| Community                        | Strategy                          |
|----------------------------------|-----------------------------------|
| Church tech leader groups        | Provide free advice, share vision |
| Homeschool parent communities    | Internet safety content           |
| Christian developer communities  | Share technical journey            |
| Local church network (St. Louis) | In-person demos, word-of-mouth    |
| r/Christianity, r/Reformed       | Genuine participation, not promo  |

**Trust Signals for Faith-Based Audiences**

Your audience trusts differently than general tech audiences:

| Signal                    | Implementation                            |
|---------------------------|-------------------------------------------|
| Pastoral endorsements     | 3-5 pastors review and endorse FaithNet   |
| Family testimonials       | Beta families share experience            |
| Theological grounding     | Scripture references explaining the "why" |
| Transparent leadership    | Founder bio, visible faith journey        |
| Data privacy commitment   | "We never sell your data" — explicit      |
| Church/school discounts   | Visible pricing signals mission over money|
| Open development          | Build in public, show the work            |

---

## Part 5: The 30-60-90 Day Action Plan

### Days 1-30: Foundation

**Infrastructure:**
- [ ] Sign up for Oracle Cloud, provision ARM VM
- [ ] Deploy CWS server to Oracle (change PROD_HOST, make deploy)
- [ ] Verify all services working via Oracle
- [ ] Keep Dell Inspiron as fallback

**Website:**
- [ ] Add /services page with web dev offerings and pricing
- [ ] Add /faithnet page with waitlist email capture
- [ ] Add contact form (or mailto: with clear CTA)
- [ ] Set up Buttondown for email list

**Revenue:**
- [ ] Set consulting rates ($100/hr, $2,000-4,000 per church site)
- [ ] Reach out to 5 churches/ministries in personal network
- [ ] Create "Web Development for Churches" one-pager (PDF)

**Audience:**
- [ ] Write first 2 blog posts (internet safety topics)
- [ ] Share founding vision with personal network
- [ ] Identify 3-5 online communities to participate in

### Days 31-60: Growth

**Infrastructure:**
- [ ] Upgrade Cloudflare to Workers Paid ($5/mo)
- [ ] Move creativeworkzstudio.com to Cloudflare Pages
- [ ] Set up R2 bucket for backups
- [ ] Configure Restic hourly backups to R2

**Website:**
- [ ] Launch FaithNet Founding Member program page
- [ ] Create lead magnet PDF ("Biblical Guide to Internet Safety")
- [ ] Add referral system to waitlist (Viral Loops or manual)

**Revenue:**
- [ ] Secure first web development client
- [ ] List first digital product on Gumroad
- [ ] Open FaithNet Founding Member signups

**Audience:**
- [ ] Publish 4 more blog posts / articles
- [ ] Submit 1 guest post to a faith-based publication
- [ ] Reach 50 email subscribers

### Days 61-90: Momentum

**Infrastructure:**
- [ ] Set up Fly.io standby server
- [ ] Configure Litestream replication (Oracle → R2 → Fly.io)
- [ ] Set up Uptime Kuma on Oracle monitoring VM
- [ ] Test failover procedure

**Website:**
- [ ] Add /publishing page (for future creative partnerships)
- [ ] Add blog section with all published content
- [ ] Refine based on visitor feedback and analytics

**Revenue:**
- [ ] Complete first client project, collect payment
- [ ] Secure 1-2 maintenance retainer clients
- [ ] Reach 10+ FaithNet founding members
- [ ] List 2-3 more digital products

**Audience:**
- [ ] Reach 100 email subscribers
- [ ] Establish regular publishing cadence
- [ ] Get first pastoral endorsement for FaithNet

---

## Why This Works: The Sovereignty Argument

The thread through all three parts — hosting, website, revenue — is the same
principle that drives FaithNet and the entire Bereshit stack:

**Dependency is fragility. Sovereignty is resilience.**

- Hosting on someone else's servers means their terms, their pricing, their
  decisions about your uptime. The interim cloud plan is explicitly temporary,
  with every dependency designed to be removable.

- Revenue from a day job means the Kingdom work is always second, always
  squeezed into margins, always at the mercy of someone else's schedule. Revenue
  from the work itself makes the work sovereign.

- Building on someone else's platform (social media, app stores, cloud
  services) means their algorithm, their approval, their cut. A self-hosted
  website with an email list you own is sovereign communication.

The 37signals precedent applies here too: they didn't leave AWS because it was
bad. They left because their workload was predictable, and predictable workloads
on rented infrastructure are just paying a premium for flexibility you don't use.

CWS's workload is predictable. The mission is long-term. The infrastructure
should match: owned, controlled, and aligned with the values it serves.

---

## Appendix: Reference Pricing

### Faith-Tech Competitive Landscape

| Product         | Category      | Price         | Revenue/Scale      |
|-----------------|---------------|---------------|--------------------|
| Covenant Eyes   | Filtering     | $18/mo        | $23M/year          |
| Bark Premium    | Monitoring    | $14/mo        | Growing            |
| CloudVeil       | DNS filtering | $8.99/mo      | Nonprofit          |
| Logos Pro        | Bible study   | $14.99/mo     | Market leader      |
| YouVersion      | Bible app     | Free          | 500M+ installs     |
| PublicSq.       | Marketplace   | Free + fees   | $5.7M (2023)       |
| Pray.com        | Prayer/audio  | Subscription  | Growing            |

### Market Context

| Metric                              | Value              |
|-------------------------------------|--------------------|
| Religion tech companies (global)    | 5,908 active       |
| US religion tech funding (total)    | $521M              |
| Religious products market (2024)    | $5.5B              |
| Growth rate (CAGR)                  | 11.4%              |
| Vertical SaaS growth vs horizontal  | 2-3x faster        |

### Web Development Market Rates (2026)

| Service                    | Rate              |
|----------------------------|-------------------|
| Church website (5 pages)   | $1,500 - $5,000   |
| Custom web application     | $5,000 - $15,000  |
| E-commerce site            | $5,000 - $25,000  |
| Hourly consulting          | $50 - $200/hr     |
| Monthly retainer           | $100 - $500/mo    |
