# Structuring — Processed

**Date:** 2026-02-11
**Status:** Complete — all files transferred to proper L0-L5 locations

---

## Round 1 — TOML/JSON/Tools (2026-02-10)

Three directories containing the OmniCode 3-block alignment work:

| Directory | Contents | Files |
|-----------|----------|:-----:|
| `toml/` | Aligned TOML data files + seed templates | 68 |
| `json/` | Aligned JSON/JSONC files | 77 |
| `tools/` | Linters + transformers (Deno/TS) | 5 |

**Total: 153 files (3 seed templates + 65 TOML data + 77 JSON/JSONC + 5 tools + 3 root-files)**

### Seed Templates (3)
| File | Destination |
|------|-------------|
| `data/config.toml` | `b-word/seed/data/L0/toml/config.toml` |
| `code/config.toml` | `b-word/seed/code/L0/toml/config.toml` |
| `code/compiler/config.toml` | `b-word/seed/code/L0/toml/compiler/config.toml` |

### Tools (5)
All moved to `c-tov/cwe-creator-tools/`:
- `lint-toml.ts` (v a-02.00)
- `lint-json.ts`
- `transform-toml.ts`
- `transform-json.ts`
- `transform-bible.ts`

### TOML Data Files (65)
| Layer | Destination | Files |
|-------|-------------|:-----:|
| L0 index | `b-word/L0-universal/foundation/` | 1 |
| L0 bible | `b-word/L0-universal/foundation/bible/` | 8 |
| L0 math | `b-word/L0-universal/foundation/math/` | 5 |
| L0 types | `b-word/L0-universal/foundation/types/` | 35 |
| L1 language | `b-word/L1-omnicode/language/` | 2 |
| L2 filesystem | `b-word/L2-platform/filesystem/` | 2 |
| L2 os/health | `b-word/L2-platform/os/health/` | 6 |
| L2 os/permission | `b-word/L2-platform/os/permission/` | 1 |
| L3 identity | `b-word/L3-cpisi/identity/` | 2 |
| L4 network | `b-word/L4-faithnet/network/` | 3 |

### JSON/JSONC Files (77)
| Category | Destination | Files |
|----------|-------------|:-----:|
| Base 3-block templates | `b-word/seed/data/L0/json/` | 2 |
| Base format templates | `b-word/seed/data/L0/json/formats/` | 3 |
| Seed utility | `b-word/seed/data/L0/` | 1 |
| Cornerstone game data | `b-word/L5-applications/cornerstone/data/` | 24 |
| Tov temporal/session | `c-tov/` (definitions, patterns, projects, session, temporal) | 47 |

### Root Files (3)
| File | Destination |
|------|-------------|
| `omnicode-operators.toml` | `b-word/L1-omnicode/` |
| `Makefile.template` | `b-word/build/` |
| `DEVELOPMENT-ENVIRONMENT.md` | `b-word/build/` |

---

## Round 2 — Makefiles/EditorConfigs/Seed L0-L5 (2026-02-11)

### Makefile Templates (aligned)
| File | From | To |
|------|------|----|
| `game-project.mk` | `structuring/makefiles/` | `b-word/seed/code/L0/make/` |
| `layer.mk` | `structuring/makefiles/` | `b-word/seed/code/L0/make/` |
| `project.mk` | `structuring/makefiles/` | `b-word/seed/code/L0/make/` |

### EditorConfig Templates (aligned)
| File | From | To |
|------|------|----|
| `compiler-project.editorconfig` | `structuring/editorconfigs/` | `b-word/seed/data/L0/dotfiles/` |
| `root.editorconfig` | `structuring/editorconfigs/` | `b-word/seed/data/L0/dotfiles/` |

### Seed L0-L5 Reorganization

All raw template folders in `b-word/seed/` moved into their L0 layers:

**data/**:
| From | To |
|------|----|
| `data/dotfiles/` | `data/L0/dotfiles/` |
| `data/json/` | `data/L0/json/` |
| `data/toml/` | removed (duplicate of `data/L0/toml/`) |
| `data/health-map.json` | `data/L0/health-map.json` |
| — | Created: `data/L1/`, `data/L2/`, `data/L4/`, `data/L5/` |

**code/**:
| From | To |
|------|----|
| `code/asm/` | `code/L0/asm/` |
| `code/c/` | `code/L0/c/` |
| `code/cmake/` | `code/L0/cmake/` |
| `code/go/` | `code/L0/go/` |
| `code/linker/` | `code/L0/linker/` |
| `code/make/` | `code/L0/make/` |
| `code/shell/` | `code/L0/shell/` |
| `code/config/` | `code/L0/config/` |
| `code/toml/` | `code/L0/toml/` |

**documentation/**:
| From | To |
|------|----|
| `documentation/adoc/` | `documentation/L0/adoc/` |
| `documentation/md/` | `documentation/L0/md/` |
| `documentation/typ/` | `documentation/L0/typ/` |

### cws-struct Tool
Created unified structuring tool at `c-tov/cws-struct/`:
- `mod.ts` — CLI entrypoint (lint, transform, verify, formats, help)
- `lib/types.ts` — Shared types (LintResult, FormatHandler, CliOptions)
- `lib/output.ts` — Terminal output formatting
- `lib/discovery.ts` — File walking and matching
- `lib/formats/registry.ts` — Format handler registry
- `lib/formats/toml.ts` — TOML 3-block linter
- `lib/verify/env.ts` — Environment verification (16 tools, 7 categories)
- `deno.jsonc` — Project config (3-block JSONC template)

### Setup Script
Updated `c-tov/scripts/setup-dev-environment.sh`:
- `pkexec` with `sudo` fallback
- `--verify-only` mode with proper `if/then/else`
- All current tools: Go, Rust, Deno, Node, Bun, Python, CUDA
- Points to `cws-struct verify env` for structured verification

### Root Files
- Updated `root-files/development-environment.adoc` — real paths, accurate versions
- Removed `root-files/DEVELOPMENT-ENVIRONMENT.md` (replaced by .adoc)

## Remaining

`wait/` — Queued templates not yet aligned (C headers, base AsciiDoc/Markdown, dotfiles, makefile base).

---

> *"To every thing there is a season, and a time to every purpose under the heaven."* — Ecclesiastes 3:1
