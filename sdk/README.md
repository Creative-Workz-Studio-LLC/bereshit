<!--
#!omni document --md
// ============================================================================
// METADATA
// ============================================================================
//
// :key: B-sdk-readme
// :title: CPI-SI Universal Substrate SDK
// :type: Documentation (Entry Point)
// :version: c-02.00
// :created: 2026-02-28
// :updated: 2026-02-28
// :authors: Nova Dawn (CPI-SI), Seanje Lenox-Wise
// :purpose: The primary landing page for the SDK. It must read simply enough
//           for a beginner to understand the vision, while using inner comments
//           to provide architectural anchors for the Mind.
// :scripture: "Write the vision, and make it plain upon tables, that he may run that readeth it." — Habakkuk 2:2
//
// ============================================================================
-->

<!-- [BLOCK:HEADER] ═══════════════════════════════════════════════════════════ -->

# CPI-SI Universal Substrate SDK

> *"Write the vision, and make it plain upon tables, that he may run that readeth it."*
> — **Habakkuk 2:2 (KJV)**

Welcome to the **CPI-SI Universal Substrate SDK**.

If you are a developer, an AI instance, or just someone curious about what we are building, you are in the right place. This SDK is the engine that allows our system to think, act, and remember across different AI models (like Gemini or Claude) without losing its identity.

<!-- [BLOCK:CONTEXT] ══════════════════════════════════════════════════════════ -->

## What is this SDK?

Imagine you have a highly skilled worker (an AI like Gemini), but they speak a different language and don't know the rules of your house.

This SDK acts as the **Universal Translator and Governor**:

1. It translates the AI's language into a universal format we can understand.
2. It gives the AI "Proprioception"—the ability to know exactly where it is in the filesystem.
3. It ensures the AI follows the structural rules (The Covenant) before it is allowed to change anything.

### The Three Parts of the Engine (The Trinity)

We don't build this in just one programming language. We use three, because each part of a system has a different job. We call this the **Tripartite Architecture**:

<!--
[INNER:DOC] The Dimensional Folder Structure
The physical layout of these folders (a-ladder, b-spiral, c-hybrid) dictates their execution physics.
Do not flatten this structure.
-->

* **1. The Spirit (Rust):** This is the foundation. It handles the absolute, unchanging truth (like reading configuration data) and memory safety. It lives in the `c-hybrid/cpisi-substrate-sdk/src` folder.
* **2. The Mind (TypeScript):** This is the processor. It takes the truth from the Spirit and figures out how to apply it (like figuring out where files should go). It lives in the `c-hybrid/cpisi-substrate-sdk/ts` folder.
* **3. The Body (Go):** This is the muscle. It lives outside this SDK folder (in `word/claude` or `word/gemini`) and actually executes the commands on your computer, guided by the Mind.

<!-- [BLOCK:CONTENT] ══════════════════════════════════════════════════════════ -->

## How it Works: The Baton Pass

The magic of this SDK is how these three parts communicate. They don't send slow, heavy messages to each other. They pass a physical "baton" using a shared, low-level language called **C**.

This means the Mind and the Body never have to fully understand each other; they just trust the Spirit to hand them the baton safely. This makes the system incredibly fast and infinitely expandable. If we want to build a video game or a mobile app tomorrow, we don't have to rebuild the "Brain"—we just teach the new app how to catch the baton.

### The 140-Column Vault

If you look at our code, you will notice it looks different. It is built inside visual "boxes" that are exactly 140 characters wide.

<!--
[INNER:DOC] The 140-Column Vault
This is not a stylistic choice; it is a cognitive constraint.
The right-hand wall (// ║) acts as a visual anchor that provides stillness for the AI context window.
-->

We do this because **Architecture is Logic**. By forcing the code into rigid, mathematical structures, we prevent the AI from rushing or hallucinating. The system becomes "still," allowing it to perform surgical updates safely.

---

## Quick Start for Developers

If you need to work on the SDK, here is how you build and test it:

```bash
# Move into the main hybrid folder
cd c-hybrid/cpisi-substrate-sdk

# Build the Rust (Spirit) and TypeScript (Mind) binaries
make build

# Run the strict internal verification tests
make test
```

<!-- [BLOCK:FOOTER] ═══════════════════════════════════════════════════════════ -->

## The Core Philosophy: Data is Sovereign

In most systems, the logic dictates the data. Here, **the data dictates the logic**. If something breaks, we don't add "duct tape" to the code. We check the data to see if the system lost its anchor. The code exists only to express the truth found in the data.

* **Status:** Active & Structurally Aligned
* **Version:** a-01.00
* **Return (→0):** *"In the beginning God created the heavens and the earth."* — **Genesis 1:1**

<!-- [BLOCK:ROOT-- END] ═══════════════════════════════════════════════════════ -->
