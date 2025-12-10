---
# ═══════════════════════════════════════════════════════════════════════════════
# METADATA BLOCK
# ═══════════════════════════════════════════════════════════════════════════════
#
# THINKING: This is YAML front matter - parseable by tools, minimally rendered.
# For COMMUNITY documents: Establishes identity for welcoming, root-level docs.
# COMMUNITY documents are the "front door" - README, CONTRIBUTING, CODE_OF_CONDUCT.
# They live at project root and are often the first thing newcomers see.
#
# COMMUNITY documents typically have type: Guide or Policy and purpose focused on
# welcoming, onboarding, or establishing community standards.
# Delete these comments when filling in, or keep for the next person.
#
# ═══════════════════════════════════════════════════════════════════════════════
#
# ─────────────────────────────────────────────────────────────────────────────
# IDENTITY - What IS this document
# ─────────────────────────────────────────────────────────────────────────────
key: B-word-seed-doc-markdown-community
                        # Bereshit keying: ORIGIN-FOLDER-DOMAIN-NAME
title:                  # Document title - clear, welcoming
                        # Examples: "Welcome to [Project]", "How to Contribute"
type:                   # Document type: Guide | Policy (most common for community docs)
                        # README = Guide, CONTRIBUTING = Guide, CODE_OF_CONDUCT = Policy
#
# ─────────────────────────────────────────────────────────────────────────────
# STATE - Where is this in its lifecycle
# ─────────────────────────────────────────────────────────────────────────────
status:                 # Lifecycle state: Draft | Active | Deprecated | Archived
version:                # SemVer (Semantic Versioning): X.Y.Z (major.minor.patch)
                        # Major = breaking changes, Minor = additions, Patch = fixes
#
# ─────────────────────────────────────────────────────────────────────────────
# TEMPORAL - When was this created and updated
# ─────────────────────────────────────────────────────────────────────────────
created:                # Creation date: YYYY-MM-DD (ISO 8601 format)
updated:                # Last update date: YYYY-MM-DD (ISO 8601 format)
#
# ─────────────────────────────────────────────────────────────────────────────
# ATTRIBUTION - Who created and maintains this
# ─────────────────────────────────────────────────────────────────────────────
authors: []             # List of authors: [Name1, Name2]
                        # For community docs: often the core maintainers
#
# ─────────────────────────────────────────────────────────────────────────────
# LOCATION - Where does this live
# ─────────────────────────────────────────────────────────────────────────────
path:                   # For COMMUNITY docs: usually project root (/)
                        # README.md, CONTRIBUTING.md, etc. live at root by convention
#
# ─────────────────────────────────────────────────────────────────────────────
# CLASSIFICATION - How is this categorized for discovery
# ─────────────────────────────────────────────────────────────────────────────
tags: []                # Categorization tags: [community, welcome, contributing, guide]
                        # COMMUNITY docs often tagged: community, onboarding
#
# ─────────────────────────────────────────────────────────────────────────────
# INTENT - Why does this document exist
# ─────────────────────────────────────────────────────────────────────────────
purpose:                # One-line purpose statement - for COMMUNITY: who it welcomes
                        # Pattern: "Welcomes X to Y" or "Guides X to participate in Y"
#
# ─────────────────────────────────────────────────────────────────────────────
# DERIVATION - What template does this document implement
# ─────────────────────────────────────────────────────────────────────────────
derives_from: bereshit/word/seed/documentation/B-word-seed-doc-markdown-base.md
                        # Path to parent template (variant of markdown-base)
#
# ─────────────────────────────────────────────────────────────────────────────
# GROUNDING - What biblical truth anchors this work
# ─────────────────────────────────────────────────────────────────────────────
biblical_foundation:    # Scripture reference (e.g., Romans 15:7)
                        # COMMUNITY docs: hospitality, welcome, unity
#
# ─────────────────────────────────────────────────────────────────────────────
# STRICTNESS - How strictly should this template be followed
# ─────────────────────────────────────────────────────────────────────────────
strictness:             # T (Tight) = Follow exactly, structure is required
                        # G (Guided) = Adapt thoughtfully, structure is recommended
                        # F (Free) = Principles only, structure is flexible
#
# ═══════════════════════════════════════════════════════════════════════════════
---
<!-- #!omni template --md -->

<!-- ═══════════════════════════════════════════════════════════════════════════════
     HEADER BLOCK - The Cover
     ═══════════════════════════════════════════════════════════════════════════════

     THINKING: For COMMUNITY: The WELCOME readers see first.
     In 5 seconds, readers should feel: I'm in the right place. I'm welcome here.

     For COMMUNITY documents:
     - Title (H1): Welcoming, clear - "Welcome to X" or "How to Contribute"
     - Tagline: Invitation, not just description - draw people in
     - Badges: Keep minimal - community docs should feel approachable, not technical
     - Grounding quote: Mission/vision that inspires belonging
     - Quick nav: Path to common newcomer needs

     COMMUNITY HEADER focuses on: "You belong here" not just "What is this?"

     ═══════════════════════════════════════════════════════════════════════════════ -->

<div align="center">

<!-- THINKING: Logo/branding creates instant recognition
     COMMUNITY docs often benefit from project logo
<img src="path/to/logo.png" alt="[Project] Logo" width="120">
-->

# [Welcome Title]

<!-- THINKING: For COMMUNITY: Welcoming titles work better than technical ones.
     Examples:
     - "Welcome to [Project]" (README)
     - "How to Contribute" (CONTRIBUTING)
     - "Our Community Standards" (CODE_OF_CONDUCT)
     - "Meet the Team" (AUTHORS)
     Should match the 'title' field in METADATA. -->

**[Welcoming tagline - what makes this community special]**

<!-- THINKING: For COMMUNITY: Taglines should INVITE, not just describe.
     Pattern: "Join us in [mission]" or "[What we do] together"
     Examples:
     - "Building Kingdom Technology together"
     - "Join us in redeeming every sector through excellence" -->

<!-- THINKING: For COMMUNITY: Fewer badges = more approachable.
     Consider: just status, or status + contributor count
-->
![Status](https://img.shields.io/badge/Status-Active-brightgreen?style=flat)

<!-- THINKING: Optional community badges - choose what fits:
![Contributors](https://img.shields.io/github/contributors/[org]/[repo]?style=flat)
![License](https://img.shields.io/badge/License-[type]-blue?style=flat)
-->

<!-- THINKING: For COMMUNITY: Mission statement creates belonging.
     This is where biblical grounding can shine authentically. -->
*[Mission statement that invites participation and establishes purpose]*

---

<!-- THINKING: For COMMUNITY: Navigation to newcomer paths.
     What do new people need most?
     - README: Quick start, what is this, how to use
     - CONTRIBUTING: How to help, guidelines, getting started
     - CODE_OF_CONDUCT: Values, expectations, reporting
-->

<kbd>[What is This?](#overview)</kbd> <kbd>[Get Started](#getting-started)</kbd> <kbd>[Get Involved](#get-involved)</kbd> <kbd>[Community](#community)</kbd>

<!-- THINKING: Alternative for simpler community docs:
**[About](#overview)** • **[Quick Start](#quick-start)** • **[Contributing](#contributing)** • **[Support](#support)**
-->

</div>

---

<!-- ═══════════════════════════════════════════════════════════════════════════════
     CONTEXT BLOCK - The Introduction
     ═══════════════════════════════════════════════════════════════════════════════

     THINKING: For COMMUNITY: What newcomers need to feel ORIENTED.
     Prepares understanding - make them feel at home before diving in.

     For COMMUNITY documents:
     - Overview: Who we are and why this matters (welcoming tone)
     - Core values: What makes this community special
     - Quick Start: Fast path for eager participants
     - What You'll Find Here: Guide to the community/project
     - Table of Contents: Navigation (keep it simple)

     Layer for COMMUNITY readers:
     - Scan: "I understand what this is about"
     - Skim: "I see how I can participate"
     - Read: "I'm ready to get involved"

     ═══════════════════════════════════════════════════════════════════════════════ -->

## Overview

<!-- THINKING: For COMMUNITY: Welcoming description - who we are, why this matters.
     Keep it 2-4 sentences. Answer: What is this community? Why would I want to be here?
     Avoid jargon. Speak to newcomers, not insiders. -->

[Welcoming description of the project/community and why it matters]

We're building [what] because [why it matters]. You're welcome here.

> [!TIP]
> **New here?** Start with [most helpful resource for newcomers](#getting-started), then [next step](#get-involved).

<!-- THINKING: For COMMUNITY: Guide newcomers immediately.
     > [!TIP] is friendlier than > [!IMPORTANT] for welcoming docs. -->

> [!NOTE]
> **Everyone is welcome.** Whether you're here to [use/contribute/learn], you belong.

<!-- THINKING: For COMMUNITY: Explicit welcome. Remove barriers to participation. -->

---

### Getting Started

<!-- THINKING: For COMMUNITY: Fast path for eager newcomers.
     What's the EASIEST way to get started? Lower the barrier. -->

**Ready to dive in?**

1. **[Action 1]** — [Brief description of first step]
2. **[Action 2]** — [Brief description of second step]
3. **[Action 3]** — [Brief description of third step]

<!-- THINKING: For COMMUNITY: Numbered steps are clearer than code blocks.
     Save code blocks for technical documentation. Keep this accessible. -->

---

### What You'll Find Here

<!-- THINKING: For COMMUNITY: Orient newcomers to the community/project.
     What do they need to know? Where should they go?
     Use friendly, inviting language. -->

| | If You Want To... | Go Here |
|:----:|-------------------|---------|
| 🚀 | **Get started quickly** | [Quick Start Guide](#getting-started) |
| 🤝 | **Contribute to the project** | [Contributing Guide](#get-involved) |
| 💬 | **Join the conversation** | [Community](#community) |
| 📖 | **Learn more about us** | [About](#about) |

<!-- THINKING: For COMMUNITY: "What do you want to do?" format is newcomer-friendly.
     Direct people to what they need, not what we want to show them. -->

---

### Table of Contents

<!-- THINKING: For COMMUNITY: Keep ToC simple and focused on newcomer paths.
     Auto-generate with: doctoc filename.md --notitle
     For community docs, the structure should feel inviting, not overwhelming. -->

<!-- START doctoc generated TOC please keep comment here to allow auto update -->
<!-- DON'T EDIT THIS SECTION, INSTEAD RE-RUN doctoc TO UPDATE -->

- [\[Welcome Title\]](#welcome-title)
  - [Overview](#overview)
    - [Getting Started](#getting-started)
    - [What You'll Find Here](#what-youll-find-here)
    - [Table of Contents](#table-of-contents)
  - [About](#about)
    - [Our Mission](#our-mission)
    - [What We're Building](#what-were-building)
  - [Get Involved](#get-involved)
    - [Ways to Contribute](#ways-to-contribute)
    - [First Contribution](#first-contribution)
  - [Community](#community)
    - [Values](#values)
    - [Getting Help](#getting-help)
  - [Recognition](#recognition)
    - [Contributors](#contributors)
    - [Special Thanks](#special-thanks)
  - [Toolkit for Community Documents](#toolkit-for-community-documents)
  - [Biblical Foundation](#biblical-foundation)
  - [References](#references)
    - [Community Documents](#community-documents)
    - [Getting Started Resources](#getting-started-resources)
    - [Stay Connected](#stay-connected)
  - [See Also](#see-also)
    - [Thank you for being here](#thank-you-for-being-here)

<!-- END doctoc generated TOC please keep comment here to allow auto update -->

[↑ Back to Top](#welcome-title)

---

<!-- ═══════════════════════════════════════════════════════════════════════════════
     CONTENT BLOCK - The Chapters
     ═══════════════════════════════════════════════════════════════════════════════

     THINKING: For COMMUNITY: The welcoming content - organized for newcomers.
     This is the heart of community documents - inviting participation.

     For COMMUNITY documents:
     - H2 Sections: About, Get Involved, Community, etc.
     - H3 Subsections: Ways to contribute, Values, Getting help
     - Tables: Simple, scannable - avoid overwhelming newcomers
     - <details open>: Core information everyone needs
     - <details>: Additional details for those who want depth
     - Callouts: > [!TIP] for helpful guidance (friendly tone)
     - [Back to Top]: Navigation aids

     For COMMUNITY, prefer:
     - Friendly, inviting language (not technical jargon)
     - Clear paths to participation
     - Explicit welcomes and encouragement
     - Simple structures over complex ones

     Layer for COMMUNITY readers:
     - Scan: "I see what this community is about"
     - Read: "I understand how to participate"
     - Deep dive: "I'm ready to contribute meaningfully"

     ═══════════════════════════════════════════════════════════════════════════════ -->

## About

<!-- THINKING: For COMMUNITY: Who we are and what we're building.
     This is the "meet us" section - make it personal and inviting. -->

[Brief, welcoming description of the project/organization]

### Our Mission

<!-- THINKING: For COMMUNITY: Why does this project exist?
     What's the heart behind the work? -->

[Mission statement - what we're building and why it matters]

### What We're Building

<!-- THINKING: For COMMUNITY: What does the project do?
     Keep it accessible - avoid jargon. -->

| | What | Why |
|:----:|------|-----|
| 🎯 | **[Core thing 1]** | [Why it matters] |
| 🛠️ | **[Core thing 2]** | [Why it matters] |
| 🌱 | **[Core thing 3]** | [Why it matters] |

[↑ Back to Top](#welcome-title)

---

## Get Involved

<!-- THINKING: For COMMUNITY: How to participate.
     Make contributing feel achievable, not intimidating. -->

We'd love your help! Here's how you can contribute.

### Ways to Contribute

<!-- THINKING: For COMMUNITY: Multiple entry points for different people.
     Not everyone codes - welcome all contributions. -->

| | Contribution | How to Start |
|:----:|--------------|--------------|
| 💻 | **Code** | [Link to contributing guide or good-first-issues] |
| 📖 | **Documentation** | [Link to docs that need help] |
| 🐛 | **Bug reports** | [Link to issue template] |
| 💡 | **Ideas & feedback** | [Link to discussions or feature requests] |
| 🤝 | **Community support** | [Link to help others in discussions] |

> [!TIP]
> **Not sure where to start?** Check out our [good first issues](#) or [ask in discussions](#).

### First Contribution

<!-- THINKING: For COMMUNITY: Hand-holding for first-time contributors.
     Make the first step as easy as possible. -->

**Never contributed before? No problem!**

1. **Fork** the repository
2. **Clone** your fork locally
3. **Make** your changes
4. **Submit** a pull request

[Detailed contributing guide →](#)

> [!NOTE]
> **We review every contribution.** Don't worry about being perfect - we're here to help!

[↑ Back to Top](#welcome-title)

---

## Community

<!-- THINKING: For COMMUNITY: How we treat each other.
     Establish a welcoming, respectful culture. -->

### Values

<!-- THINKING: For COMMUNITY: What we stand for.
     These should feel genuine, not corporate. -->

We believe in:

- **Welcoming everyone** — All backgrounds, experience levels, and perspectives
- **Kindness first** — Respectful communication in all interactions
- **Learning together** — No question is too basic
- **Excellence with grace** — High standards, patient support

### Getting Help

<!-- THINKING: For COMMUNITY: How to get support.
     Make it clear where to go for different needs. -->

| Need Help With... | Go Here |
|-------------------|---------|
| 🐛 **Bugs or issues** | [GitHub Issues](#) |
| 💬 **Questions** | [Discussions](#) |
| 📖 **Documentation** | [Docs](#) |
| 🤝 **Community chat** | [Discord/Slack](#) |

> [!TIP]
> **Before asking:** Check if your question has been answered in [FAQ](#) or [existing discussions](#).

[↑ Back to Top](#welcome-title)

---

## Recognition

<!-- THINKING: For COMMUNITY: Thank contributors.
     People love being recognized - it encourages more participation.
     Keep it warm and genuine - this builds community. -->

We're grateful for everyone who helps make [Project] better.

### Contributors

<!-- THINKING: Options for recognizing contributors:
     1. All-contributors bot (automated)
     2. Manual list in README
     3. Link to GitHub contributors page
     4. Dedicated CONTRIBUTORS.md file -->

Thanks to everyone who has contributed!

<!-- Use one of these approaches:

Option 1 - Link to GitHub:
[See all contributors](https://github.com/[org]/[repo]/graphs/contributors)

Option 2 - All-contributors badge:
[![All Contributors](https://img.shields.io/badge/all_contributors-X-orange.svg)](#contributors)

Option 3 - Manual table (for smaller projects):
| | Contributor | Contributions |
|:--:|-------------|---------------|
| 👤 | **[@username](link)** | Code, docs |
| 👤 | **[@username](link)** | Design |

-->

### Special Thanks

<!-- THINKING: Acknowledge specific help, inspiration, or support -->

- **[Person/Organization]** — [What they contributed or inspired]
- **[Person/Organization]** — [What they contributed or inspired]

[↑ Back to Top](#welcome-title)

---

## Toolkit for Community Documents

<!-- THINKING: For COMMUNITY: Lighter toolkit focused on welcoming elements.
     Points to G-000 for the complete Markdown reference.
     Keeps only what's most useful for community docs. -->

> [!TIP]
> **→ For complete Markdown toolkit:** See [Markdown Base Template](B-word-seed-doc-markdown-base.md) — full text formatting, diagrams, advanced features.

This section covers elements especially useful for community documents.

<details open>
<summary><h3>Welcoming Callouts</h3></summary>

<!-- THINKING: Callouts that make newcomers feel welcome -->

> [!NOTE]
> **For context:** Use NOTE to provide helpful background information.

---

> [!TIP]
> **New here?** Use TIP to guide newcomers to the right place.

---

> [!IMPORTANT]
> **Key concept:** Use IMPORTANT for things everyone should know.

---

> [!WARNING]
> **Before proceeding:** Use WARNING for things to watch out for.

</details>

<details open>
<summary><h3>Badges for Status</h3></summary>

<!-- THINKING: Badges make READMEs look professional and provide quick info -->

**Common badges:**

```markdown
![Status](https://img.shields.io/badge/Status-Active-brightgreen)
![License](https://img.shields.io/badge/License-MIT-blue)
![Contributors](https://img.shields.io/github/contributors/org/repo)
```

**Result:**
![Status](https://img.shields.io/badge/Status-Active-brightgreen)
![License](https://img.shields.io/badge/License-MIT-blue)

**Custom badges:**

```markdown
![Custom](https://img.shields.io/badge/[LABEL]-[MESSAGE]-[COLOR])
```

Colors: `brightgreen`, `green`, `yellowgreen`, `yellow`, `orange`, `red`, `blue`, `lightgrey`

</details>

<details open>
<summary><h3>Quick Navigation</h3></summary>

<!-- THINKING: Help readers find what they need fast -->

**Inline navigation (for top of README):**

```markdown
**[Get Started](#getting-started)** • **[Contribute](#contribute)** • **[Community](#community)**
```

**Back to top links (end of sections):**

```markdown
[↑ Back to Top](#welcome-title)
```

</details>

<details open>
<summary><h3>Contribution Steps</h3></summary>

<!-- THINKING: Common pattern for CONTRIBUTING docs -->

**Numbered steps with commands:**

````markdown
1. **Fork** the repository
   ```bash
   # Click "Fork" on GitHub
   ```

2. **Clone** your fork

   ```bash
   git clone https://github.com/YOUR-USERNAME/repo.git
   ```

3. **Create** a branch

   ```bash
   git checkout -b feature/your-feature
   ```
````

</details>

<details>
<summary><h3>Collapsible Sections</h3></summary>

<!-- THINKING: Keep README clean while providing depth -->

**Syntax:**

```markdown
<details>
<summary><b>Click to expand</b></summary>

Hidden content here...

</details>
```

**Use for:**

- FAQ sections
- Detailed installation options
- Troubleshooting guides
- Long lists that would clutter the main document

</details>

[↑ Back to Top](#welcome-title)

---

<!-- ═══════════════════════════════════════════════════════════════════════════════
     FOOTER BLOCK - The Appendix
     ═══════════════════════════════════════════════════════════════════════════════

     THINKING: For COMMUNITY documents, the FOOTER sends readers off with warmth.
     They should feel:
     - Welcome to stay and contribute
     - Connected to the larger community
     - Grounded in why this community exists

     Contains:
     - Biblical Foundation: Grounding in hospitality/community/unity
     - References: Links to other community docs, getting started resources
     - Centered closing: Welcome back, stay connected
     - Cross-references: Other places to engage

     Send readers out with:
     - Invitation to participate (you belong here)
     - Clear next steps (how to stay connected)
     - Warm, welcoming tone (come back anytime)

     ═══════════════════════════════════════════════════════════════════════════════ -->

## Biblical Foundation

<!-- THINKING: For COMMUNITY documents, focus on hospitality, welcome, unity.
     Scripture that grounds community values: Romans 15:7, Hebrews 13:2,
     Colossians 3:12-14, 1 Peter 4:9, Galatians 6:2 -->

> [!NOTE]
> **Community rooted in hospitality** — We welcome everyone because we were first welcomed.

*"Wherefore receive ye one another, as Christ also received us to the glory of God."* — **Romans 15:7** (KJV)

**Applied:** We build inclusive communities not because it's trendy, but because hospitality is a Kingdom principle. Every contribution matters. Every person belongs. We welcome others as we have been welcomed.

[↑ Back to Top](#welcome-title)

---

## References

<!-- THINKING: For COMMUNITY: Focus on resources that help people participate.
     Keep it friendly and accessible - not exhaustive lists. -->

### Community Documents

<!-- THINKING: The standard community files at project root -->

| | Document | Purpose |
|:----:|----------|---------|
| 📖 | **[README](README.md)** | Project overview and getting started |
| 🤝 | **[Contributing Guide](CONTRIBUTING.md)** | How to contribute |
| 📜 | **[Code of Conduct](CODE_OF_CONDUCT.md)** | Community standards |
| 🔒 | **[Security Policy](SECURITY.md)** | Reporting vulnerabilities |
| 📄 | **[License](LICENSE)** | Usage rights and terms |

### Getting Started Resources

<!-- THINKING: Resources to help newcomers succeed -->

| Resource | Purpose |
|----------|---------|
| **[Documentation](#)** | Learn how things work |
| **[Examples](#)** | See it in action |
| **[FAQ](#)** | Common questions answered |

### Stay Connected

<!-- THINKING: Ways to stay engaged with the community -->

| Channel | Purpose |
|---------|---------|
| **[GitHub Discussions](#)** | Ask questions, share ideas |
| **[Issue Tracker](#)** | Report bugs, request features |
| **[Discord/Slack](#)** | Real-time chat |

---

## See Also

<!-- THINKING: For COMMUNITY: Other places to explore.
     Keep it inviting - these are doorways, not obligations. -->

- **[Project Website](#)** — Learn more about [Project]
- **[Blog/Updates](#)** — Latest news and announcements
- **[Roadmap](#)** — Where we're headed

---

<div align="center">

<!-- THINKING: For COMMUNITY: Centered footer is WARM and INVITING.
     Welcome them back, invite them to stay, remind them they belong. -->

**[↑ Back to Top](#welcome-title)**

---

### Thank you for being here

We're glad you're part of this community.

**Questions?** [Open a discussion](#) • **Found a bug?** [Report it](#) • **Want to help?** [Start contributing](#)

---

*"Wherefore receive ye one another, as Christ also received us to the glory of God."* — **Romans 15:7**

**Building together, welcoming everyone.**

---

<!-- THINKING: For COMMUNITY: Keep version/status light or omit.
     The focus is on relationship, not bureaucracy.

**Version:** [X.Y.Z] • **Updated:** [YYYY-MM-DD]
-->

</div>

<!-- ═══════════════════════════════════════════════════════════════════════════════
     REFERENCE-STYLE LINKS
     ═══════════════════════════════════════════════════════════════════════════════

     THINKING: For COMMUNITY: Keep references simple and accessible.
     Format: [ref-name]: url "optional title"

     ═══════════════════════════════════════════════════════════════════════════════ -->

<!-- Template references (add as needed) -->

<!-- Standard references (if needed) -->

<!-- Community document links (customize per project) -->
<!-- [discussions]: https://github.com/[org]/[repo]/discussions "GitHub Discussions" -->
<!-- [issues]: https://github.com/[org]/[repo]/issues "Issue Tracker" -->
<!-- [contributing]: CONTRIBUTING.md "Contributing Guide" -->

<!-- ═══════════════════════════════════════════════════════════════════════════════
     TEMPLATE NOTES - DELETE WHEN USING
     ═══════════════════════════════════════════════════════════════════════════════

     C-000: COMMUNITY TEMPLATE
     Purpose: Welcoming, accessible, root-level documents

     This template is for:
     - README.md          (Project overview, first impression)
     - CONTRIBUTING.md    (How to contribute)
     - CODE_OF_CONDUCT.md (Community standards)
     - SECURITY.md        (Reporting vulnerabilities)
     - SUPPORT.md         (Getting help)

     ─────────────────────────────────────────────────────────────────
     KEY PRINCIPLE: WELCOME FIRST
     ─────────────────────────────────────────────────────────────────

     COMMUNITY documents are often the FIRST thing someone sees.
     Every element should make newcomers feel:
     - "I'm in the right place"
     - "I'm welcome here"
     - "I can contribute"
     - "I know what to do next"

     ─────────────────────────────────────────────────────────────────
     FORMAT × PURPOSE: WHERE C-000 FITS
     ─────────────────────────────────────────────────────────────────

     The Template Matrix has two axes:

              │ General (teach) │ Index (navigate) │ Community (welcome) │
     ─────────┼─────────────────┼──────────────────┼─────────────────────│
     Markdown │      G-000      │      T-000       │       C-000         │
     AsciiDoc │      A-000      │        —         │         —           │
     Typst    │      F-000      │        —         │         —           │

     C-000 shares Markdown syntax with G-000 and T-000.
     C-000 has a UNIQUE PURPOSE: welcoming newcomers.

     → For complete Markdown toolkit: See B-word-seed-doc-markdown-base.md
     → For navigation/index focus: See B-word-seed-doc-markdown-index.md

     ─────────────────────────────────────────────────────────────────
     COMMUNITY-SPECIFIC GUIDANCE
     ─────────────────────────────────────────────────────────────────

     Language:
     - Use "you" and "we" (inclusive, conversational)
     - Avoid jargon unless defined
     - Keep sentences short and clear
     - Use action words: "Get started", "Join us", "Contribute"

     Tone:
     - Friendly, not formal
     - Encouraging, not demanding
     - Patient, not rushed
     - Grateful, not entitled

     Structure:
     - Lead with value (what they get, not what we want)
     - Clear next steps early
     - FAQ or common questions
     - Multiple paths for different users

     Callouts for COMMUNITY:
     > [!TIP]       - Guide newcomers ("New here? Start with...")
     > [!NOTE]      - Provide context ("Everyone is welcome")
     > [!IMPORTANT] - Key info ("Please read our Code of Conduct")

     ─────────────────────────────────────────────────────────────────
     WHEN FILLING THIS TEMPLATE
     ─────────────────────────────────────────────────────────────────

     1. METADATA → Establish welcoming identity
     2. HEADER → The warm welcome (badges optional but helpful)
     3. CONTEXT → What this is, why it matters, how to get started
     4. CONTENT → About, Get Involved, Community, Recognition
     5. FOOTER → References, stay connected, thank you
     6. Delete THINKING comments (or keep as guidance)
     7. Delete/adapt Toolkit section (keep as reference or remove)
     8. Update links to your actual resources
     9. Test: Would a newcomer feel welcome?

     FINAL CHECK: Read as if you've never seen the project.
     - Do I know what this is in 5 seconds?
     - Do I feel welcome?
     - Do I know what to do next?

     If yes to all three → your COMMUNITY document is ready.

     ═══════════════════════════════════════════════════════════════════════════════ -->
