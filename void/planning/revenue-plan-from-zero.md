# Revenue Plan: From Zero

**Date:** 2026-02-09
**Context:** A practical revenue plan built from what actually exists in this
codebase, what CWS can demonstrably do today, and what it takes to build a
brand from nothing.

---

## What I Actually See in This Repo

Before talking about revenue, I need to be honest about what's here and what
isn't. This isn't a pitch deck — it's an inventory.

### What's Built and Working

1. **A production-grade Go server** serving four services from a single binary,
   deployed to real hardware, accessible at real URLs, with health monitoring,
   WebSocket streaming, graceful shutdown, and a Makefile deploy pipeline that
   would impress engineers at companies ten times your size.

2. **A professional marketing website** with custom CSS (no framework), premium
   typography (Playfair Display + Inter), responsive design, and a cohesive
   visual identity (deep blue + gold). This is not a template — it's crafted
   work that demonstrates genuine design sensibility.

3. **A full editorial publishing system** — 57-point lint, 5 PDF themes, 4 build
   scopes, multi-format output (PDF/HTML/EPUB/DocBook), asset generation
   pipeline, Bible verse validation, and a TypeScript builder with real-time
   WebSocket streaming. This system publishes a board-level governance document
   with 4 Parts, 11 Articles, and 87 Sections. That's not a toy.

4. **CPI-SI** — a working state machine system with SQLite + FTS5, 21 API
   endpoints, real-time WebSocket updates, journal management, pattern
   detection, and analytics. Built on a theological architecture that traces
   every component to Genesis 1:1.

5. **A 73KB Makefile** for editorial workflow and a **45KB Makefile** for the
   full stack build system. These alone represent weeks of careful engineering.

6. **Complete company governance documentation** — entity overview, publishing
   model, services catalog, audience definition, business model, core values,
   mission/vision, team structure, timeline, and strategic vision. This is a
   real company with real thinking behind it.

### What's NOT Built Yet

- FaithNet (architecture documented, code not started)
- OmniCode compiler (lexer/parser in progress)
- MillenniumOS (foundation stage)
- Cornerstone game engine (functional but no published titles)
- No public-facing products with paying customers
- No email list
- No social media presence
- No blog or content
- No client testimonials or case studies
- No pricing on the website
- No call-to-action on the website beyond "Contact Us"

### The Gap

The gap isn't skill. The codebase proves the skill is there — full-stack
engineering, system architecture, design, editorial systems, infrastructure,
deployment. The gap is that **none of this skill is visible to anyone who
isn't reading the source code.**

A church in St. Louis looking for a website doesn't know this exists. A
creative needing publishing support doesn't know this exists. A family worried
about internet safety doesn't know FaithNet is coming.

Revenue starts with visibility. Visibility starts with showing the work.

---

## The Honest Starting Position

CWS is at what startups call "pre-revenue." But it's not pre-work. The
difference matters. Many pre-revenue companies have a slide deck and a dream.
CWS has:

- A registered LLC
- A leadership team of 6
- A production server running live
- A working product (the publishing system)
- A complete business model document
- A company identity manual
- A professional website

The problem isn't "we have nothing." The problem is "we have everything
except customers." That's a marketing problem, not a capability problem.
Marketing problems are solvable.

---

## Revenue Stream 1: Web Development Services

### Why This Is First

It's first because it can generate revenue this week with zero new code.

The website at creativeworkzstudio.com is itself a portfolio piece. Anyone who
visits it sees clean, professional, responsive design with thoughtful typography
and a cohesive brand. That's exactly what a church, a ministry, or a small
business needs for their own site.

The Go server demonstrates backend capability. The Makefile deploy system
demonstrates DevOps maturity. The editorial publishing system demonstrates
attention to detail. A potential client doesn't need to see any of that — they
just need to see the website and know you can build one for them.

### What You Can Sell Today

| Service                              | Price           | Delivery  |
|--------------------------------------|-----------------|-----------|
| Church/ministry website (5-8 pages)  | $2,000 - $3,500 | 2-3 weeks |
| Small business website (5-10 pages)  | $2,500 - $5,000 | 2-4 weeks |
| Landing page (single page)          | $500 - $1,000   | 3-5 days  |
| Website redesign                     | $1,500 - $4,000 | 2-3 weeks |
| Monthly maintenance + hosting        | $150 - $300/mo  | Ongoing   |
| Technical consultation               | $100/hr         | As needed |

### Why These Prices

These are mid-market for the St. Louis area and the faith-based niche. You're
not competing with Fiverr ($200 template installs) or with agencies ($15,000+
custom builds). You're in the sweet spot where churches and small ministries
actually have budget: $2,000-5,000 for a site that looks professional and
works well.

The maintenance retainer is where recurring revenue starts. A church that pays
you $2,500 for a website and then $200/month for hosting, updates, and support
is worth $4,900 in year one and $2,400 every year after. Five retainer clients
= $1,000/month recurring before you touch anything else.

### How to Get the First Client

This is the hard part, and I won't pretend it isn't. From zero means zero
referrals, zero testimonials, zero portfolio beyond your own site. Here's the
practical path:

**Week 1-2: The warm network**

Seanje works at Guardian Angel Settlement Association. That's a nonprofit with
a physical location, staff, community connections, and almost certainly a
website that could be better. This isn't a cold call — it's a conversation
with colleagues:

- "I build websites for churches and nonprofits through my company. Does
  Guardian Angel need any web work?"
- "Do you know any churches looking for a new website?"
- "I'm offering a discounted rate for my first few clients to build my
  portfolio."

The leadership team has 6 people. Each of them attends a church, knows people
in ministries, has family in small businesses. That's potentially 30-50 warm
contacts before you make a single cold outreach.

**Week 3-4: The first project (discounted or pro bono)**

Take the first project at 50% off or even free if it's for a ministry that
genuinely can't pay. The goal isn't revenue — it's:

1. A completed project you can show to the next client
2. A testimonial from a real organization
3. A case study for your website
4. Practice scoping, quoting, and delivering client work

**Month 2-3: Full price, referral-driven**

With one completed project:

- Add a portfolio section to creativeworkzstudio.com showing the work
- Ask the first client for a testimonial and referral
- Post in local church Facebook groups: "I build websites for churches"
- Reach out to 5 churches whose websites look dated (look them up, be specific
  about what you'd improve — "I noticed your site isn't mobile-friendly")

### Revenue Target

| Milestone              | Timeline   | Monthly Revenue |
|------------------------|------------|-----------------|
| First client (discounted) | Month 1 | $0-1,000        |
| First full-price client   | Month 2-3 | $2,000-3,500  |
| First retainer            | Month 3   | $150-300        |
| Steady state (2 projects + 3 retainers) | Month 6+ | $2,500-5,000 |

---

## Revenue Stream 2: The Publishing System as a Service

### Why This Matters

Most people reading this plan will skip to FaithNet. But the publishing system
is the hidden gem in this codebase, and it's the most natural revenue path
for a company that calls itself a publishing house.

What you've built:

- A 57-point editorial lint system with 7 categories
- 5 professional PDF themes (manual, legal, manual-legal, plain, informal)
- Multi-format output (PDF, HTML, EPUB, DocBook XML)
- Asset generation (Mermaid diagrams, color palette generation, grayscale
  conversion for print)
- Table of Contents verification with page number validation
- Bible verse validation against KJV/WEB
- Live rebuild with file watching
- Configuration-driven everything (single YAML source of truth)

This system publishes a board-level governance document. That same system can
publish:

- Church constitutions and bylaws
- Ministry handbooks and policy manuals
- Nonprofit annual reports
- Small business employee handbooks
- Self-published books (devotionals, Bible studies, memoirs)

### What You Can Sell

| Service                                    | Price            |
|--------------------------------------------|------------------|
| Church constitution/bylaws (formatted PDF)  | $500 - $1,500   |
| Ministry handbook (design + formatting)     | $800 - $2,000   |
| Nonprofit annual report (design + layout)   | $1,000 - $3,000 |
| Employee handbook (small business)          | $600 - $1,500   |
| Book formatting + layout (self-publishing)  | $500 - $2,000   |
| Ongoing editorial support (retainer)        | $200 - $500/mo  |

### The Pitch

"We have a professional publishing system that produces corporate-quality
documents with consistent branding, multiple output formats, and editorial
validation. We use the same system to publish our own board governance manual."

Then you show them the output — their church bylaws in a professionally
themed PDF with proper typography, table of contents, and consistent styling.
That's not something they can do in Microsoft Word.

### Who Needs This

- **Churches rewriting bylaws** (happens more than you'd think — leadership
  transitions, denomination changes, legal updates)
- **Nonprofits producing annual reports** (every grant-funded nonprofit needs
  these)
- **Small businesses creating employee handbooks** (every business with 5+
  employees should have one)
- **Self-publishing authors** (the faith-based self-publishing market is large
  and underserved on the formatting side)

### Revenue Target

| Milestone                    | Timeline    | Revenue        |
|------------------------------|-------------|----------------|
| First document project       | Month 2-3   | $500-1,500     |
| Steady state (1-2/month)     | Month 6+    | $1,000-3,000   |
| Editorial retainers (2-3)    | Month 6+    | $400-1,500/mo  |

---

## Revenue Stream 3: Digital Products

### Why Digital Products

They sell while you sleep. Once created, a digital product generates revenue
with zero marginal cost per sale. Even $200/month in passive income changes the
economics of a bootstrapped startup.

### What You Can Create From What Already Exists

**Product 1: "A Biblical Guide to Internet Safety for Families"**
- Price: Free (lead magnet for FaithNet waitlist)
- Purpose: Email capture, audience building, authority establishment
- Content: You already have the theological framework (FaithNet spec).
  Translate it into practical family advice.
- Format: PDF, 15-25 pages, designed with your publishing system

**Product 2: "Church Website Planning Guide"**
- Price: $9.99
- Purpose: Lead generation for web dev services + standalone revenue
- Content: How to plan a church website, what pages you need, how to write
  your About page, SEO basics for churches, choosing a domain
- Format: PDF, 20-30 pages

**Product 3: Church Website Template**
- Price: $79 - $149
- Purpose: Revenue + demonstration of design skill
- Content: A clean, responsive HTML/CSS template based on your existing
  design system (the deep blue + gold palette works perfectly for churches).
  No JavaScript required. Include: homepage, about, sermons, events,
  contact, staff directory.
- Format: ZIP file with HTML/CSS/assets + setup instructions

**Product 4: "Starting a Faith-Based Tech Company" Guide**
- Price: $19.99
- Purpose: Revenue + thought leadership in the faith-tech space
- Content: You're literally living this. Document the journey: LLC formation,
  choosing tech stack, building in margins, sovereignty principles, what
  worked, what didn't. Your company-docs drafts are half the content already.
- Format: PDF, 30-50 pages

**Product 5: Professional Document Templates**
- Price: $29 - $49 per template pack
- Purpose: Revenue from the publishing system
- Content: Church bylaws template, ministry handbook template, nonprofit
  annual report template — all pre-themed with your 5 PDF themes
- Format: AsciiDoc/Markdown source + PDF samples + build instructions

### Where to Sell

**Gumroad** — zero monthly cost, 10% transaction fee. Simple, trusted,
handles payments and delivery. Perfect for a from-zero startup because there's
no upfront investment.

Later, sell from your own site (Stripe integration, higher margins).

### Revenue Target

| Milestone                        | Timeline   | Monthly Revenue |
|----------------------------------|------------|-----------------|
| Free lead magnet live            | Month 1    | $0 (but captures emails) |
| First paid product listed        | Month 2    | $50-200         |
| 3-5 products listed              | Month 4    | $200-500        |
| Steady state (SEO + audience)    | Month 8+   | $300-800        |

---

## Revenue Stream 4: FaithNet Founding Members

### Why Pre-Sell Before the Product Exists

Three reasons:

1. **Validation.** If no one pays, the product idea needs rethinking before
   you invest months building it. If people do pay, you have confirmation
   that the problem is real and your framing resonates.

2. **Revenue.** Pre-sales generate cash flow during development. Even $5,000
   from founding members funds 6+ months of hosting and buys the mini PC.

3. **Community.** Founding members become your beta testers, your word-of-mouth
   evangelists, and your most forgiving early users. They're invested in your
   success because they paid for it before it existed.

### The Founding Member Program

| Tier               | Price              | What They Get                          | Target |
|--------------------|--------------------|----------------------------------------|--------|
| Founding Family    | $49.99/yr locked   | Lifetime 50% off, first beta access, name on "Founders Wall" | 100 families |
| Founding Church    | $99.99/yr locked   | Lifetime 40% off, priority support, input on church features  | 20 churches |
| Founding Partner   | $199.99/yr locked  | Everything above + quarterly call with founder, custom categories | 10 partners |

"Locked" means the price never increases for them. Ever. This is a covenant
commitment, not a promotional trick. It aligns with your values: "covenant,
not contract."

### What You Need Before Launching This

1. A dedicated page at creativeworkzstudio.com/faithnet
2. A clear explanation of the three-tier model (Blocked/Filtered/Open)
3. The "agency through boundaries" messaging (already written in the FaithNet spec)
4. An email capture form (Buttondown, free tier)
5. A payment processor (Gumroad or Stripe)
6. An honest timeline: "Beta expected [date]. Founding members get first access."

You do NOT need a working product. You need a compelling vision, a credible
team (you have one), and transparency about where things stand.

### Revenue Target

| Milestone                     | Timeline   | Revenue          |
|-------------------------------|------------|------------------|
| Page live, first signups      | Month 2    | $0 (waitlist)    |
| Open founding member payments | Month 3    | $500-2,000       |
| 50 founding families          | Month 6    | $2,500           |
| Full founding cohort          | Month 9-12 | $5,000-15,000    |

---

## Building a Brand From Complete Zero

### The Reality of Zero

Zero means:

- Nobody knows you exist
- You have no social proof
- You have no content indexed by Google
- You have no email list
- You have no followers on any platform
- You have no case studies
- You have no testimonials

This is not a bad position. It's an honest one. Most startups pretend they're
further along than they are. CWS's values (integrity, transparency,
covenant-based relationships) mean starting from zero honestly is more
aligned than faking it.

### Step 1: Your Website Is Your Storefront (Fix It First)

The current site is beautiful but passive. A visitor reads about CWS and
leaves. There is no path from "interested" to "engaged" to "customer."

**What needs to change (not a redesign, just additions):**

Add these pages as static HTML files in `server/website/`:

| Page | Purpose | CTA |
|------|---------|-----|
| `/services` | Web development offerings + pricing | "Get a Free Consultation" (contact form) |
| `/faithnet` | FaithNet vision + waitlist + founding members | "Join the Waitlist" (email capture) |
| `/publishing` | Publishing services for creators | "Start a Conversation" (contact form) |
| `/blog` | Articles on internet safety, church tech, building in public | "Subscribe for Updates" (email capture) |

Every page ends with a next step. No dead ends.

**What to change on the existing homepage:**

The "Tools" section currently links to Builder Dashboard and CPI-SI Console.
These are internal tools — they mean nothing to an external visitor and they
expose internal infrastructure. Replace this section with:

- A preview of services ("What We Can Do For You")
- A FaithNet teaser ("Coming Soon: Internet Safety for Families of Faith")
- A blog preview (latest 2-3 articles)

Keep the internal tool links in a separate `/internal` page or behind auth.

### Step 2: Content Is Your Proof of Expertise (Start Writing)

You can't buy credibility at zero. You have to create it. The fastest path
is content that demonstrates you know what you're talking about.

**The first 10 articles (in order of impact):**

| # | Title | Purpose |
|---|-------|---------|
| 1 | "Why Your Church Needs a Better Website (And What That Actually Means)" | Directly markets web dev services to your target audience |
| 2 | "Internet Safety for Christian Families: A Practical Guide" | Seeds FaithNet, captures emails, establishes authority |
| 3 | "What We're Building and Why: The CWS Story" | Founder story, builds trust, humanizes the brand |
| 4 | "5 Things Every Church Website Needs (That Most Are Missing)" | SEO, practical value, markets services |
| 5 | "Why We Chose to Build Our Own Server Instead of Using AWS" | Tech credibility, sovereignty narrative, attracts builders |
| 6 | "Setting Up Internet Filtering for Your Family (Free Options)" | Pure value, no sell — builds trust with FaithNet audience |
| 7 | "The Case for Owning Your Technology" | Thought leadership, aligns with values |
| 8 | "How We Published a 87-Section Governance Manual With Our Own Tools" | Demonstrates publishing capability |
| 9 | "A Developer's Guide to Building for the Kingdom" | Attracts technical community, establishes CWS in faith-tech |
| 10 | "What FaithNet Is and Why We're Building It" | Official announcement, drives waitlist signups |

**Frequency:** 2 per month. That's one article every two weeks. Publish on
your own blog (you control it), share to relevant communities.

**Distribution for each article:**

1. Publish on creativeworkzstudio.com/blog
2. Share in 2-3 relevant communities (church tech Facebook groups, r/Christianity, local church newsletters)
3. Send to your email list (even if it's 12 people)
4. Post a teaser on LinkedIn with link to full article

### Step 3: Email List Is Your Foundation (Own Your Audience)

Social media is rented land. An email list is yours.

**Tool:** Buttondown (free up to 100 subscribers, $9/month up to 1,000).
Simple, privacy-respecting, no bloat.

**How to get the first 100 subscribers:**

| Source | Expected Signups | Method |
|--------|------------------|--------|
| Personal network | 15-25 | Direct ask: "I'm starting a newsletter about faith and technology" |
| Lead magnet (internet safety PDF) | 20-40 | Give away free guide in exchange for email |
| Blog articles | 10-20 | "Subscribe" CTA at bottom of every article |
| FaithNet waitlist | 10-30 | Anyone interested in FaithNet gives their email |
| Church communities | 5-15 | Share in church groups, local contacts |

**Timeline to 100 subscribers:** 2-3 months with consistent effort.

100 subscribers isn't a vanity metric. It's:
- 100 people who hear about every product launch
- 100 potential beta testers for FaithNet
- 100 people who might refer a client
- A statistically meaningful sample for feedback

### Step 4: Social Proof (Earn It Honestly)

From zero, you can't fake testimonials. You earn them:

**Month 1-2: The first testimonial**

Deliver the first web project (even discounted). At the end, ask:

> "Would you be willing to write 2-3 sentences about what it was like working
> with us? It would really help other churches find us."

Put it on the homepage. One real testimonial from a real church is worth more
than fifty generic quotes.

**Month 3-4: The case study**

Write up the first project as a case study:
- What the client needed
- What you built
- Before/after screenshots
- The client's own words

Put it on the /services page. Now you have proof, not just claims.

**Month 4-6: Pastoral endorsements**

For FaithNet specifically, a pastor's endorsement carries enormous weight in
the faith community. You don't need a megachurch pastor. You need 3-5 local
pastors who:

1. Understand the internet safety problem
2. Believe in the "agency through boundaries" approach
3. Are willing to put their name behind it

Approach them with the free internet safety PDF, explain the FaithNet vision,
and ask if they'd support it publicly.

### Step 5: Local Before Global

CWS is in Missouri. The first clients, the first endorsements, the first
community — they're all local. This is not a limitation. It's a strength.

**Local advantages:**

- Face-to-face meetings build trust faster than any website
- Church networks in a metro area are densely connected (one referral spreads)
- Local nonprofits prefer local vendors
- Guardian Angel Settlement Association is a physical nexus of community connections
- You can attend church events, ministry meetings, business mixers in person

**The local play:**

1. Attend 1-2 local church technology meetups or ministry leader gatherings per month
2. Offer a free 30-minute "church website audit" to any church in the area
3. Volunteer tech help at church events (sound, livestream, projector setup — be
   useful, not salesy)
4. Leave business cards at church offices with a simple pitch: "Websites for
   Churches — creativeworkzstudio.com"

None of this requires a marketing budget. It requires presence and patience.

---

## The Combined Revenue Path

### Month 1-3: Foundation

| Activity | Revenue | Purpose |
|----------|---------|---------|
| First web dev client (discounted) | $0-1,500 | Portfolio piece + testimonial |
| Free internet safety PDF | $0 | Email list building (lead magnet) |
| Website updates (add /services, /faithnet pages) | $0 | Enable revenue capture |
| First 2-4 blog articles published | $0 | SEO + credibility |
| Email list: 30-50 subscribers | $0 | Foundation for everything |

**Monthly revenue: $0-500**

This is the hard part. You're investing time with little return. But you're
building the assets (portfolio, testimonials, email list, content) that
generate returns in months 4-12.

### Month 4-6: Traction

| Activity | Revenue | Purpose |
|----------|---------|---------|
| Second web dev client (full price) | $2,000-4,000 | Real revenue |
| First maintenance retainer (2 clients) | $300-600/mo | Recurring revenue starts |
| First digital product on Gumroad | $50-200/mo | Passive income starts |
| First publishing service project | $500-1,500 | Second revenue stream opens |
| FaithNet founding member page live | $500-2,000 | Pre-sales begin |
| Email list: 100-150 subscribers | — | Growing audience |
| 6-10 blog articles indexed by Google | — | Organic discovery begins |

**Monthly revenue: $1,500-4,000**

### Month 7-12: Momentum

| Activity | Revenue | Purpose |
|----------|---------|---------|
| Steady web dev work (1-2 projects/month) | $2,000-5,000/mo | Primary income |
| Maintenance retainers (3-5 clients) | $450-1,500/mo | Stable recurring base |
| Digital products (3-5 listed) | $200-500/mo | Growing passive income |
| Publishing services (1-2/month) | $500-2,000/mo | Second service line |
| FaithNet founding members (50-100) | $2,500-10,000 cumulative | Pre-launch funding |
| Email list: 250-500 subscribers | — | Ready for FaithNet beta launch |

**Monthly revenue: $3,000-7,000**

### Year 2: Sustainability Transition

This is where the business model document's "Phase 2: Transition" becomes
real. If month 7-12 averages $4,000-5,000/month:

- Annual revenue: ~$48,000-60,000
- Hosting costs: ~$108/year (the $9/month interim stack)
- Business expenses: ~$2,000-3,000/year (domains, email, tools, occasional
  software)
- Net before taxes: ~$45,000-57,000

That's approaching the point where Seanje can consider transitioning from
Guardian Angel to CWS full-time. Not yet — but approaching. The full
transition happens when revenue sustains for 6+ consecutive months and
there's 3-6 months of reserve.

---

## Why Revenue Is a Mission Requirement, Not a Compromise

The business model document says: "We're not a nonprofit relying on donations,
nor a startup chasing exits. We're a business building for the long term."

But it also says Phase 1 is "building in the margins" with "minimal expenses,
maximum foundation building."

Here's the tension: you can't build FaithNet in the margins. You can't build
MillenniumOS in the margins. You can't build a publishing house that serves
creatives across every sector in the margins. The margins are where you lay
foundations. Revenue is how you move from foundation to construction.

The company docs describe five phases:

| Phase | Name | Revenue Role |
|-------|------|-------------|
| 1 | Foundation | Minimal — building in margins |
| 2 | Transition | First products, growing toward sustainability |
| 3 | Sustainability | CWS as primary income, able to hire |
| 4 | Growth | Multiple products, many partnerships |
| 5 | Kingdom Economy | Mark-free commerce operational |

**You are at the boundary between Phase 1 and Phase 2.** The foundation is
laid. The stack exists. The server runs. The website is live. The governance
docs are written. The business model is documented. The team is defined.

What Phase 2 needs is customers. Customers need to know you exist. Knowing you
exist requires visibility. Visibility requires content, presence, and delivered
work.

Revenue isn't about "becoming corporate." It's about removing the dependency
on a day job so the Kingdom work becomes the primary work. The same sovereignty
principle drives this as drives the in-house hosting plan: dependency on
external systems (including an employer) for mission-critical resources is
a fragility you should eliminate as soon as practically possible.

The web dev services aren't a distraction from FaithNet. They're the funding
mechanism. Every church website you build is a relationship with a future
FaithNet customer, a potential publishing partner, and a node in the trade
network the business model describes.

Every blog article about internet safety is a future FaithNet user finding
you through Google. Every founding member payment is validation that the
market exists. Every email subscriber is someone who will hear about the
launch.

Revenue, audience, and product development are not competing priorities.
They're the same work viewed from different angles.

---

## The Minimum Viable Brand

From zero, you don't need a full brand overhaul. You need five things:

### 1. A Clear One-Sentence Pitch

> "We build websites and technology for churches and Kingdom-aligned
> organizations."

Not: "A publishing house and technology company building Genesis-patterned
infrastructure for the Millennium." That's the vision. The pitch is for
someone at a church mixer who asks "So what do you do?"

Save the full vision for the second conversation. Get the first conversation
by being immediately understandable.

### 2. A Website That Converts

You have the design. You need the pages:
- `/services` — what you do, what it costs, how to start
- `/faithnet` — what's coming, how to be early
- `/blog` — proof you know what you're talking about
- Email capture on every page

### 3. A Portfolio (Even If It's Just Your Own Work)

Right now your portfolio is:
- creativeworkzstudio.com (your own site)
- The Builder Dashboard (shows engineering depth)
- The Company Identity Manual (shows publishing capability)

After your first client project, add that. After the second, add that.
The portfolio grows with every delivery.

### 4. A Consistent Voice

Read through the company docs — the voice is already there. It's direct,
grounded, values-driven, and doesn't pander. Sentences like "We serve with
intention, not indiscriminately" and "Your IP stays yours" and "We advise,
you decide" — that's a brand voice. Use it in blog posts, emails, and
client conversations.

### 5. A Way to Be Found

- Google: blog articles optimized for "church website design" + "internet
  safety for families" + "faith-based technology"
- Local: church networks, ministry events, nonprofit connections
- Referral: every completed project is asked for a referral
- Email: the list grows and every subscriber is a distribution channel

---

## 90-Day Action Plan (Practical, Ordered)

### Week 1-2

- [ ] Add `/services` page to the website with pricing and contact CTA
- [ ] Add `/faithnet` page with waitlist email capture
- [ ] Set up Buttondown for email list
- [ ] Remove internal tool links from public homepage
- [ ] Ask 15-20 people in personal network to subscribe to email list
- [ ] Identify 3 churches in local network that might need web work
- [ ] Reach out to Guardian Angel about their web presence

### Week 3-4

- [ ] Write and publish first blog article (church website topic)
- [ ] Create the free internet safety PDF (lead magnet)
- [ ] Set up Gumroad account
- [ ] Start conversations with potential first web dev client
- [ ] Join 2-3 church tech or ministry leader Facebook groups

### Week 5-8 (Month 2)

- [ ] Publish 2 more blog articles
- [ ] Send first email newsletter (even to 15 people)
- [ ] List first paid digital product on Gumroad ($9.99 guide)
- [ ] Scope and quote first web dev project
- [ ] Begin first web dev project if client secured
- [ ] Design the church website template for Gumroad

### Week 9-12 (Month 3)

- [ ] Deliver first web dev project (or close to delivery)
- [ ] Collect first testimonial and add to website
- [ ] Open FaithNet founding member payments
- [ ] Publish 2 more blog articles (6 total)
- [ ] List church website template on Gumroad ($79-149)
- [ ] Email list target: 50+ subscribers
- [ ] Begin second web dev client outreach

---

## What Success Looks Like at 12 Months

If this plan executes well, at 12 months CWS has:

- **5-8 completed web projects** in the portfolio
- **3-5 maintenance retainer clients** ($450-1,500/month recurring)
- **3-5 digital products** on Gumroad ($200-500/month passive)
- **50-100 FaithNet founding members** ($2,500-10,000 in pre-sales)
- **250-500 email subscribers** (ready for FaithNet beta launch)
- **20+ blog articles** indexed by Google (organic discovery)
- **3-5 testimonials** from real organizations
- **2-3 pastoral endorsements** for FaithNet
- **Monthly revenue of $3,000-7,000**
- **Clear path to Phase 3** (sustainability, full-time transition)

And most importantly: **a brand that exists in the world, not just in a Git
repository.**

---

## Summary

The skill is already here. The architecture is already here. The vision,
the values, the governance, the team — all here. What's missing is the
bridge between what you've built and the people who need it.

That bridge is built with:
1. A website that has a next step (services, waitlist, email capture)
2. Content that proves expertise (blog articles, guides)
3. Delivered work that earns trust (client projects, testimonials)
4. A growing audience that hears about what's coming (email list)
5. Pre-sales that validate and fund the product (founding members)

None of this requires new technology. It requires showing the work that
already exists to the people who need it. The code is written. The server
runs. The site is live. Now it's time to let people know.
