<div align="center">

# OmniCode Ecosystem

**OmniCode structural patterns highlighted across all your languages**

![Version](https://img.shields.io/badge/Version-0.2.1-blue?style=flat)
![Status](https://img.shields.io/badge/Status-Active-success?style=flat)
![License](https://img.shields.io/badge/License-Proprietary-informational?style=flat)

*Block boundaries, section tags, and operator conventions — everywhere*

</div>

---

## What This Does

When you use OmniCode conventions inside comments of other languages, this extension highlights them. Block boundaries (`═══`), section tags (`[METADATA]`), section addressing (`P.1`, `M.1`), and pragma shebangs (`#!omni`) light up in Go, C, Rust, TypeScript, Python, TOML, JSONC, and more.

It also provides the **default color palette** for all OmniCode scopes, used by both this extension and the companion OmniCode Language extension.

---

## Supported Languages

Injection grammars activate in comments of:

| Language | Scope |
|----------|-------|
| Go | `source.go` |
| C | `source.c` |
| C++ | `source.cpp` |
| Rust | `source.rust` |
| TypeScript | `source.ts`, `source.tsx` |
| JavaScript | `source.js`, `source.jsx` |
| Python | `source.python` |
| Shell/Bash | `source.shell` |
| Makefile | `source.makefile` |
| TOML | `source.toml` |
| YAML | `source.yaml` |
| JSONC | `source.json.comments` |
| CSS | `source.css` |
| C# | `source.csharp` |

Additionally, JSONC and JSON files get highlighting for OmniCode meta fields (keys prefixed with `_`).

---

## What Gets Highlighted

### In Comments (All Languages)

| Pattern | Example | What It Is |
|---------|---------|------------|
| Block boundaries | `// ═══════════` | Block separators |
| Section boundaries | `// ───────────` | Section separators |
| Block headers | `// METADATA BLOCK` | Block names |
| Block ends | `// END METADATA` | Block terminators |
| Section headers | `// P.1 CORE` | Section addressing |
| Tags | `// [PRAGMA]` | Block/section tags |
| Section addresses | `P.1`, `M.1`, `X.1`, `Cc`, `Co`, `Cv` | Section references |
| Pragma in comments | `// #!omni document --md` | Embedded pragmas |

### In JSONC/JSON Files

| Pattern | Example | What It Is |
|---------|---------|------------|
| Meta keys | `"_pragma"`, `"_P1_key"` | OmniCode underscore-prefix fields |
| Block boundaries | `"═══════════"` | Boundary values in strings |
| Section boundaries | `"───────────"` | Boundary values in strings |
| Tags | `"[METADATA]"` | Tag values in strings |

---

## Color Palette

This extension provides 30+ default token color rules covering all OmniCode scopes:

| Element | Color | Hex | Style |
|---------|-------|-----|-------|
| Block boundaries (`═══`) | Green | `#6A9955` | **bold** |
| Section boundaries (`───`) | Dark green | `#4E7A46` | normal |
| Block names | Teal | `#4EC9B0` | **bold** |
| Tags (`[METADATA]`) | Yellow | `#DCDCAA` | **bold** |
| Section addresses | Light blue | `#9CDCFE` | **bold** |
| Pragma shebang | Purple | `#C586C0` | **bold** |
| Directives (`@key`) | Purple | `#C586C0` | normal |
| Flags (`--toml`) | Sand | `#CE9178` | normal |
| Anchors (`#anchor`) | Gold | `#FFD700` | **bold** |
| Type binding (`::`) | Light gray | `#D4D4D4` | **bold** |
| Containment (`>>`, `=>`, `<-`) | Blue | `#569CD6` | **bold** |
| Control flow (`-->`, `->|`) | Purple | `#C586C0` | **bold** |
| Hebrew states | Gold | `#FFD700` | normal |
| CPI-SI keywords | Bright cyan | `#4FC1FF` | normal |
| Ternary values (`+1`, `-1`) | Sage | `#B5CEA8` | **bold** |
| Meta fields (`_prefix`) | Gray | `#808080` | *italic* |
| Affirm (`YES`, `CONFIRMED`) | Green | `#6A9955` | normal |
| Negate (`NO`, `DENIED`) | Red | `#F44747` | normal |
| Attention critical (`>!!!`) | Red | `#F44747` | **bold** |
| Attention warning (`>!!`) | Orange | `#FF8C00` | **bold** |
| Emphasis (`BAM`, `IMPORTANT`) | Orange | `#FF8C00` | **bold** |

Colors are optimized for dark themes and integrate naturally with VSCode's default Dark+ theme.

---

## Installation

### From Source (Development)

```bash
cd Bereshit/word/omni/tooling/vscode
make install-ecosystem
```

Reload VSCode: `Ctrl+Shift+P` > "Developer: Reload Window"

### From VSIX (Distribution)

```bash
make package-ecosystem
code --install-extension dist/omnicode-ecosystem-0.2.1.vsix
```

---

## Companion Extension

**[OmniCode Language](https://github.com/Creative-Workz-Studio-LLC/workspace)** — Full language support for `.omni` and `.ofd` files including syntax highlighting, structural alignment linting via cws-struct, health scoring, and QuickFix code actions. Install both for the complete OmniCode experience.

---

## Example: Go File with OmniCode Comments

```go
// ═══════════════════════════════════════════════════════════════════
// METADATA BLOCK                                             [PRAGMA]
// ═══════════════════════════════════════════════════════════════════

package main

// ───────────────────────────────────────────────────────────────────
// P.1 CORE — Package Identity
// ───────────────────────────────────────────────────────────────────

import "fmt"

// ═══════════════════════════════════════════════════════════════════
// BODY BLOCK                                                [CONTENT]
// ═══════════════════════════════════════════════════════════════════

func main() {
    fmt.Println("Hello, Kingdom Technology")
}

// ═══════════════════════════════════════════════════════════════════
// END BODY                                                     [END]
// ═══════════════════════════════════════════════════════════════════
```

All the `═══`, `METADATA BLOCK`, `[PRAGMA]`, `P.1 CORE`, etc. light up with OmniCode colors while the Go code retains its native highlighting.

---

<div align="center">

**CreativeWorkzStudio LLC** | Kingdom Technology

*Built with intentional design for the glory of God*

*"Look that thou make them after their pattern."* — Exodus 25:40

</div>
