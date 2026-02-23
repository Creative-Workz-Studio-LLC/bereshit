<div align="center">

# OmniCode Language

**Full language support for OmniCode (.omni) and OmniFold (.ofd) files**

![Version](https://img.shields.io/badge/Version-0.3.0-blue?style=flat)
![Status](https://img.shields.io/badge/Status-Active-success?style=flat)
![License](https://img.shields.io/badge/License-Proprietary-informational?style=flat)
![cws--struct](https://img.shields.io/badge/cws--struct-integrated-4EC9B0?style=flat)

*Syntax highlighting, structural alignment linting, health scoring, and QuickFix code actions*

</div>

---

## What is OmniCode?

OmniCode is a block-structured metadata and document architecture format developed by [CreativeWorkzStudio LLC](https://github.com/Creative-Workz-Studio-LLC). It uses a sentence-like syntax with structural operators for describing file hierarchies, document schemas, and system relationships.

---

## Features

### Structural Alignment Linting

Powered by **cws-struct** — the DAR (Detect/Align/Report) engine:

- Inline diagnostics on save and open
- Supports **8 formats**: Go, Rust, TOML, JSON, JSONC, Makefile, OmniCode, OmniFold
- **Ternary lint policy**: strict (-1) / balanced (0) / growth (+1)
- Automatic binary detection with fallback chain

### Health Scoring

Ternary health score displayed in the status bar:

| Range | State | Color |
|-------|-------|-------|
| 50 to +100 | tov (aligned) | Green |
| 0 to 49 | yashar (stable) | Yellow |
| -100 to -1 | shavar (misaligned) | Red |

Click the status bar item for a full health report with per-block breakdown.

### QuickFix Code Actions

When cws-struct detects structural issues with available fixes:

- **Remove** — Delete misaligned lines
- **Insert** — Add missing structural elements
- **Replace** — Correct content in place

One-click fixes from the lightbulb menu or `Ctrl+.`.

### Syntax Highlighting

Full TextMate grammar covering 25+ pattern categories:

| Category | Elements |
|----------|----------|
| **Pragma** | `#!omni` shebang with type and flags |
| **Block Structure** | `═══` boundaries, `METADATA BLOCK`, `END METADATA` markers |
| **Section Structure** | `───` boundaries, `P.1`/`M.1`/`X.1`/`Cc`/`Co`/`Cv` addressing |
| **Tags** | `[PRAGMA]`, `[METADATA]`, `[CONTENT]`, `[CLOSING]`, `[END]` |
| **Directives** | `@key`, `@type`, `@version`, `@authors` |
| **Anchors** | `#anchor` declarations with values |
| **Containment** | `>>` (child), `=>` (provides), `<-` (derives) |
| **Type Binding** | `::` operator for type declarations |
| **Control Flow** | `-->` (proceed), `->|` (halt), `\->` (await), `<--` (backward) |
| **CPI-SI Keywords** | Health states, DAR operations, witness declarations |
| **Hebrew States** | `shavar`, `chaser`, `ratsah`, `yashar`, `tamim`, `shalem`, `tov` |

### Commands

| Command | Description |
|---------|-------------|
| `OmniCode: Lint Current File` | Manually trigger lint on active file |
| `OmniCode: Lint All Open Files` | Lint every open document |
| `OmniCode: Show Health Details` | Full health report in output channel |
| `OmniCode: Show Output Channel` | View extension logs and errors |

---

## Settings

All settings under the `omnicode.*` namespace:

| Setting | Default | Description |
|---------|---------|-------------|
| `omnicode.cwsStructPath` | `"cws-struct"` | Path to cws-struct binary |
| `omnicode.lintOnSave` | `true` | Lint on file save |
| `omnicode.lintOnOpen` | `true` | Lint when opening files |
| `omnicode.policy` | `"balanced"` | Ternary lint policy |
| `omnicode.debounceMs` | `300` | Debounce delay before linting |

---

## Installation

### From Source (Development)

```bash
cd Bereshit/word/omni/tooling/vscode
make deps    # Install build dependencies (first time)
make dev     # Build + install + reload hint
```

### From VSIX (Distribution)

```bash
make package
code --install-extension dist/omnicode-language-0.3.0.vsix
```

### Prerequisites

- Visual Studio Code 1.80.0+
- **cws-struct** binary (install from [cws-struct source](https://github.com/Creative-Workz-Studio-LLC/workspace))

---

## Companion Extension

**[OmniCode Ecosystem](https://github.com/Creative-Workz-Studio-LLC/workspace)** — Highlights OmniCode structural patterns inside comments of Go, C, Rust, TypeScript, Python, TOML, JSONC, and 10+ other languages. Provides the default color palette. Install both for the complete OmniCode experience.

---

## Sample

```
#!omni folder --toml --jsonc

// ═══════════════════════════════════════════
// METADATA BLOCK                       [PRAGMA]
// ═══════════════════════════════════════════

@key    b-word-core
@type   folder
@status active

// ───────────────────────────────────────────
// P.1 CORE — Identity
// ───────────────────────────────────────────

#anchor Genesis_1_1

>> types/ :: data
>> bible/ :: data

=> [CONFIG_TEMPLATES]
<- b-word/seed/data/toml/config.toml

// ═══════════════════════════════════════════
// END METADATA                          [END]
// ═══════════════════════════════════════════
```

---

<div align="center">

**CreativeWorkzStudio LLC** | Kingdom Technology

*Built with intentional design for the glory of God*

*"Look that thou make them after their pattern."* — Exodus 25:40

</div>
