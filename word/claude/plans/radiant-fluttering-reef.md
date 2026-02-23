# Plan: OmniCode Extension Suite — Linter Integration + Suite Design

> *"Look that thou make them after their pattern."* — Exodus 25:40

---

## Context

Both OmniCode VSCode extensions (Language v0.2.0, Ecosystem v0.2.0) are **100% declarative** — no TypeScript, no main entry, no activationEvents. They only provide TextMate grammars and language configuration. Meanwhile, `cws-struct` (our DAR engine) is production-ready with 260 tests, rich JSON output including health scoring, fix suggestions, and support for Go/Rust/TOML/JSON/Makefile/dotfiles/OmniCode/OmniFold formats.

The extensions live directly at `~/.vscode/extensions/` with no separate source directory, no build system, and no version control. We need to:
1. **Wire cws-struct diagnostics** into the Language extension (immediate value)
2. **Establish proper source management** with build/install workflow (sustainable development)
3. **Design the full suite architecture** for future growth

---

## Architecture Decisions

| Decision | Choice | Rationale |
|----------|--------|-----------|
| **Source location** | `Bereshit/word/omni/tooling/vscode/` | INSTALL.md already references this; `word/omni/` is the OmniCode home |
| **Repo structure** | Monorepo (npm workspaces) | Shared build infra, coordinated versions, single Makefile |
| **Build tool** | esbuild (not webpack) | 10-100x faster, simpler config, VSCode standard |
| **cws-struct relationship** | External process (spawn) | 95MB binary — spawn `cws-struct lint <fmt> <file> --json`, like ESLint pattern |
| **exFAT handling** | node_modules on ext4 cache | `~/.cache/omnicode/vscode/` for executables; JS output back to exFAT source |
| **API version** | `vscode ^1.80.0` (unchanged) | All needed APIs available; system has VSCode 1.96+ |
| **Ecosystem ext** | Stays declarative | No TypeScript needed — injection grammars + themes only |

---

## Source Layout

```
Bereshit/word/omni/tooling/vscode/
  Makefile                          # Build, test, install orchestration
  package.json                      # npm workspace root (private)
  tsconfig.base.json                # Shared TS config
  .gitignore                        # node_modules/, out/, *.vsix

  extensions/
    omnicode-language/              # Gets TypeScript
      package.json                  # + main, activationEvents, configuration
      tsconfig.json
      src/
        extension.ts                # activate/deactivate entry point
        diagnostics.ts              # DiagnosticCollection + cws-struct spawn
        statusbar.ts                # Health score StatusBarItem
        codeactions.ts              # CodeActionProvider for fix suggestions
        config.ts                   # Settings management
        types.ts                    # cws-struct JSON output types
      out/extension.js              # esbuild bundle (gitignored)
      test/fixtures/                # Sample files for testing
      syntaxes/                     # Existing grammars (unchanged)
      language-configuration.json   # Existing (unchanged)
      omnifold-language-configuration.json
      icon.png, ofd-icon.png

    omnicode-ecosystem/             # Stays declarative (no build)
      package.json                  # Existing (unchanged)
      syntaxes/                     # Existing grammars (unchanged)
      icon.png
```

---

## Extension API Design

### Settings (namespace: `omnicode`)

| Setting | Type | Default | Purpose |
|---------|------|---------|---------|
| `omnicode.cwsStructPath` | string | `"cws-struct"` | Binary path (PATH → `~/.local/share/claude/bin/`) |
| `omnicode.lintOnSave` | boolean | `true` | Lint on file save |
| `omnicode.lintOnOpen` | boolean | `true` | Lint when opening files |
| `omnicode.policy` | enum | `"balanced"` | Ternary lint policy: strict/balanced/growth |
| `omnicode.debounceMs` | number | `300` | Debounce delay before linting |

### Diagnostic Mapping

| cws-struct | VSCode |
|------------|--------|
| `"error"` | `DiagnosticSeverity.Error` |
| `"warn"` | `DiagnosticSeverity.Warning` |
| `"info"` | `DiagnosticSeverity.Information` |
| `line: N` | `Range(N-1, 0, N-1, MAX)` (full line highlight) |
| `line: null` | `Range(0, 0, 0, 0)` (file-level diagnostic) |

### Format Auto-Detection

```
.go → go, .rs → rust, .toml → toml, .json/.jsonc → json
.omni → omni, .ofd → ofd, Makefile → makefile
.gitignore/.editorconfig → dotfiles
```

### Code Actions (QuickFix)

When `fix.actions` exists on a result:
- `"remove"` action → `WorkspaceEdit.delete(Range(startLine-1..endLine))`
- `"insert"` action → `WorkspaceEdit.insert(Position(afterLine, 0), content.join('\n'))`
- Title from `fix.description`

### Health Status Bar

- Right-aligned: `"OmniCode: +75"` with color coding (green 50+, yellow 0-49, red <0)
- Tooltip: per-block breakdown
- Click: opens output channel with full health report
- Hidden when active file has no lint data

---

## Phased Implementation

### Phase 1: Source Migration + Build Pipeline

**Goal:** Move sources to Bereshit, establish build/install. Zero new functionality.

1. Create `Bereshit/word/omni/tooling/vscode/` directory structure
2. Copy existing extension files from `~/.vscode/extensions/`
3. Create root `package.json` (workspaces), `tsconfig.base.json`, `.gitignore`
4. Create `Makefile` with targets: `deps`, `build`, `install`, `clean`, `dev`
5. Set up ext4 build cache at `~/.cache/omnicode/vscode/`
6. Create stub `src/extension.ts` (empty activate/deactivate)
7. Configure esbuild → `out/extension.js`
8. **Verify:** build → install → extension loads → grammars still work

### Phase 2: Diagnostics Integration

**Goal:** cws-struct inline diagnostics on save/open.

1. Implement `config.ts` — settings + binary resolution
2. Implement `types.ts` — mirror cws-struct JSON output types
3. Implement `diagnostics.ts`:
   - Binary path resolution (setting → PATH → default)
   - Spawn `cws-struct lint <format> <file> --json --policy <policy>`
   - Parse JSON stdout, map to `vscode.Diagnostic[]`
   - Debounce mechanism, DiagnosticCollection management
4. Wire `onDidSaveTextDocument` + `onDidOpenTextDocument`
5. Add output channel for cws-struct errors
6. **Verify:** Open .go/.toml/.omni file → see inline diagnostics

### Phase 3: Status Bar + Code Actions

**Goal:** Health visibility and one-click fixes.

1. Implement `statusbar.ts` — health score StatusBarItem
2. Implement `codeactions.ts` — QuickFix from fix suggestions
3. **Verify:** Status bar shows health → click for details → QuickFix available

### Phase 4: Future Growth (not this session)

- OmniCode Studio (webview for `cws-struct studio`)
- Template scaffolding (`cws-struct create` via command palette)
- OmniFold tree view (directory navigation)
- Full LSP (eventual migration from spawn-per-lint)

---

## CPI-SI Family Dispatch

For implementation, dispatch family agents as teammates:

| Agent | Role | Phase |
|-------|------|-------|
| **Nehemiah Shamar** (infrastructure) | Build pipeline, Makefile, ext4 cache setup | Phase 1 |
| **Ezra Matthan** (architecture) | Extension TypeScript architecture, module boundaries | Phase 1-2 |
| **Nathan Emet** (testing) | Test fixtures, verify diagnostics mapping | Phase 2-3 |
| **Joanna Elara** (patterns) | Ensure consistency with existing CPI-SI patterns | Phase 1-2 |
| **Deborah Saphar** (documentation) | README, INSTALL.md, extension descriptions | Phase 3 |

Agents should journal during work (create-journal-entry), update their patterns, and post-journal when complete.

---

## Critical Files

| File | Purpose |
|------|---------|
| `~/.vscode/extensions/omnicode-language/package.json` | Existing manifest to extend |
| `~/.vscode/extensions/omnicode-0.1.0/package.json` | Ecosystem manifest (copy as-is) |
| `Bereshit/.a-new-structure/c-tov/cws-struct/lib/foundation/types.ts` | Authoritative type definitions |
| `Bereshit/.a-new-structure/c-tov/cws-struct/lib/foundation/health.ts` | Health scoring types |
| `Bereshit/.a-new-structure/c-tov/cws-struct/mod.ts` | JSON output contract (emitJson) |
| `~/.vscode/extensions/omnicode-language/syntaxes/*.tmLanguage.json` | Grammars to preserve |

---

## Build Flow

```
Source (exFAT)                    Build Cache (ext4)                Install (ext4)
Bereshit/word/omni/               ~/.cache/omnicode/vscode/         ~/.vscode/extensions/
tooling/vscode/                   node_modules/ (.bin/)             omnicode-language/
                                                                    omnicode-ecosystem/
   make deps ──────────────────→ npm install
   make build ─── esbuild ──→ out/extension.js (back to source)
   make install ───────────────────────────────────────────────→ cp -r (strip src/)
```

---

## Verification

1. **Build pipeline:** `make deps && make build` produces `out/extension.js`
2. **Install:** `make install` copies to `~/.vscode/extensions/`, grammars work
3. **Diagnostics:** Open a Go file with known block structure issues → see inline errors/warnings
4. **Health:** Status bar shows ternary health score for active file
5. **Code actions:** Hover a diagnostic with fix → QuickFix available
6. **Ecosystem:** Injection grammars still work in all 16 host languages
7. **No regression:** Existing .omni/.ofd syntax highlighting unchanged
