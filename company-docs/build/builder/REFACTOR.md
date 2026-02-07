# Builder Consolidation — Post-Implementation Roadmap

Makefile refactor and TS builder expansion. Based on patterns proven through
four editorial passes (Cover, TOC, Dedication, Preface) and the editorial style
guide crystallized to b-01.05 with 24+ anti-patterns.

**Completed:** Phases 1-5 (2026-02-07)
**Status:** All phases complete

## Architecture

Two-tier separation — proven and unchanged:

| System | Role | Interface |
|--------|------|-----------|
| **Makefile** | Editorial CLI, PDF orchestration, caching, dependency graphs | `make <target>` |
| **TS Builder** | Intelligence engine — parsing, validation, multi-format publishing | `cws-build <command>` |
| **build.config.yaml** | Single source of truth for behavior | Drives both systems |

**Make is the orchestrator. Builder is the intelligence.**

## What Was Done

### Phase 1: Core Consolidation (lint + toc delegation)

Replaced ~390 lines of Makefile grep chains with ~50 lines of delegation + ~850 lines
of structured TypeScript.

| Before | After |
|--------|-------|
| 155 lines of grep chains for lint | 4-line delegation to `cws-build lint` |
| 130 lines of shell parsing for toc-status | 4-line delegation to `cws-build toc` |
| 100 lines of shell parsing for toc-parse | 4-line delegation to `cws-build toc` |
| 8 lint checks (grep-based, uncategorized) | 17 checks in TS (categorized, extensible) |
| Makefile 1842 lines | Makefile ~1520 lines |

**Files created/modified:**
- `validate/editorial.ts` — All 17 lint checks with categories
- `validate/index.ts` — Exports for editorial module
- `cli.ts` — Wired `--editorial`, `--category`, display function
- `Makefile` — Replaced grep chains, updated help/deps/.PHONY

### Phase 2: Editorial Guide Integration

All 17 checks implemented from the start in `validate/editorial.ts`, organized
into 4 categories matching editorial style guide anti-patterns.

### Phase 3: Format Expansion

Added docbook-intermediate pipeline for pandoc-based format conversion.

| Component | Change |
|-----------|--------|
| `config/types.ts` | Added `pipeline?: string` to `FormatConfig` |
| `publish/executor.ts` | Implemented `buildViaPipeline()` for 2-step conversion |
| `build.config.yaml` | Added markdown and docx format definitions |
| `Makefile` | Added `markdown`, `docx`, `convert` targets + deps/help update |

**Pipeline:** AsciiDoc → DocBook XML (asciidoctor) → target format (pandoc).
Pandoc's DocBook reader is excellent; its AsciiDoc reader is not.

## Editorial Lint — 17 Checks in 4 Categories

### Category: `editorial` (Checks 1-8)

Migrated from Makefile grep chains. Core editorial standards.

| # | Check | What It Finds |
|:-:|-------|---------------|
| 1 | `shall` | "shall" in governance text (skips Bible quotes, comments) |
| 2 | `spacers` | `{empty} +` spacer count per file |
| 3 | `page-markers` | Page marker integrity — gaps, missing banners |
| 4 | `page-sequence` | Page marker sequence — numbering order |
| 5 | `old-comments` | Old-format `// --- PAGE BREAK` comments |
| 6 | `lead-density` | `[.lead]` count > 2 per file |
| 7 | `title-page` | `:title-page:` in standalone header |
| 8 | `book-heading` | `[discrete]` without `ifndef::book[]` guard |

### Category: `style` (Checks 9-13)

Anti-patterns from editorial style guide. Style consistency.

| # | Check | What It Finds |
|:-:|-------|---------------|
| 9 | `company-name` | "Creative Workz Studio" (spaced — should be "CreativeWorkzStudio") |
| 10 | `chapter-language` | "Chapter" instead of "Article"/"Section" |
| 11 | `all-caps` | ALL CAPS for emphasis (4+ consecutive uppercase) |
| 12 | `gold-text` | `[.gold]` role on text content |
| 13 | `register-mixing` | "shall" + "must" in same paragraph |

### Category: `structure` (Checks 14-15)

Structural integrity and cross-reference validation.

| # | Check | What It Finds |
|:-:|-------|---------------|
| 14 | `unlabeled-breaks` | `<<<` without `// PDF p.N` within 5 lines |
| 15 | `xref-resolution` | `<<anchor>>` without `[[anchor]]` defined |

### Category: `conversion` (Checks 16-17)

Conversion readiness and frontmatter rules.

| # | Check | What It Finds |
|:-:|-------|---------------|
| 16 | `icon-macros` | `icon:` macro usage (conversion checklist) |
| 17 | `admonition-frontmatter` | NOTE/TIP/IMPORTANT in A-frontmatter scope |

### Usage

```bash
make lint                          # All 17 checks, all files
make lint FILE=A-COVER             # All checks, single file
make lint SCOPE=A                  # All checks, frontmatter scope
make lint CHECK=editorial          # Checks 1-8 only
make lint CHECK=style              # Checks 9-13 only
make lint CHECK=structure          # Checks 14-15 only
make lint CHECK=conversion         # Checks 16-17 only
make lint CHECK=all                # Default — all categories
make lint FILE=A-COVER CHECK=style # Combined: single file + category
```

## Format Conversion Pipeline

Two new formats via `docbook-intermediate` pipeline:

| Format | Target | Command |
|--------|--------|---------|
| Markdown | GitHub-Flavored Markdown (.md) | `make markdown` |
| Word | OOXML document (.docx) | `make docx` |
| Generic | Any configured format | `make convert FORMAT=<name>` |

**Pipeline steps:**
1. AsciiDoc → DocBook XML (via `asciidoctor -b docbook5`)
2. DocBook XML → target format (via `pandoc --from docbook --to <format>`)

**Adding new formats:** Define in `build.config.yaml` with `pipeline: docbook-intermediate`,
set command/args for pandoc, add Makefile target. No code changes needed.

## What Stays in Make

Make keeps what Make is good at:

| Stays | Why |
|-------|-----|
| PDF generation (`asciidoctor-pdf`) | File dependencies and caching |
| Cascading builds (section → article → part → book) | Make's dependency graph |
| `watch-pdf` file monitoring | entr integration |
| `status` dashboard | PDF timestamps + build state |
| `clean` / `remove` | File operations |
| `deps` prerequisite check | System tool detection |
| `pages` extraction via qpdf | Shell tool delegation |
| `section` / `article` / `part` / `book` targets | The editorial build interface |
| `validate` structural checks | Skeleton validation |
| Asset generation (diagrams, grayscale, palette) | 3-phase with caching |

## Stability Zones — What Does NOT Change

| Component | Why |
|-----------|-----|
| Resolution engine (`resolve_dir`, `resolve_file`) | Elegant 3-mode resolution, proven |
| Cache engine (`check_cache`) | Timestamp-based, handles all cases |
| Error code system (E0-E4x) | Consistent across Make + TS |
| Brand colors, part discovery variables | Used by assets |
| Asset generation target | 3-phase with caching, works |
| All build targets (section, article, part, spine, book, create, pages) | Core editorial flow |
| Remove, status, watch-pdf, setup, clean | Working infrastructure |

### Phase 3.5: Config Schema Alignment (v3.0.0)

Aligned TypeScript types with build.config.yaml v3.0.0 (10 sections).

| Component | Change |
|-----------|--------|
| `config/types.ts` | Added §7 ScriptureConfig, §8 PathsConfig, §9 ScopeDefinition/ScopesConfig, §10 BrandColorsConfig/BrandConfig |
| `config/types.ts` | Fixed AssetsConfig.logos from `LogoEntry[]` to `LogosConfig` (directory + items) |
| `config/types.ts` | Added DerivedAssetsConfig (grayscale, palette, mockup) |
| `config/loader.ts` | Updated `computePaths()` to read from config.paths when available |
| `config/loader.ts` | Added query functions: getScopeKeys, getScopeConfig, getBrandColors, getScriptureBasePath, getDefaultTranslation |
| `cli.ts` | Added `cws-build config` subcommand (--show, --validate, --make, --json) |
| `config/index.ts` | Exported all new types and functions |
| `index.ts` | Root barrel exports updated |
| `package.json` | Version bumped 2.0.0 → 3.0.0 |

**Contract:** build.config.yaml (truth) → TypeScript interfaces (contract) → builder logic (execution)

## Phase 4: Pipeline Features + Cleanup (Completed 2026-02-07)

### 4a: TOC Pipeline Commands

| Command | Purpose |
|---------|---------|
| `cws-build toc --fill` | Fill `p. __` placeholders with computed page numbers |
| `cws-build toc --fill --dry-run` | Report fills without writing |
| `cws-build toc --verify` | Check existing page numbers against computed values |
| `make toc-fill` | Makefile delegation to `--fill` |
| `make toc-fill DRYRUN=1` | Makefile delegation to `--fill --dry-run` |
| `make toc-verify` | Makefile delegation to `--verify` |

**Data flow:**
```
buildSectionFileMap() → sectionFileMap (§ ref → path)
         ↓
scanSpine() → ordered traversal (A-files + content)
         ↓
countFilePages() → marker count per file
         ↓
computePageMap() → cumulative offsets → § ref → book page
         ↓
runTocFill() / runTocCheck() → fill placeholders / detect stale
```

**Files:** `validate/pages.ts` (+250 lines), `cli.ts` (+95 lines)

**Makefile:** Consolidated `toc-status` + `toc-parse` (identical) into single `toc` target.
Added `toc-fill` and `toc-verify` targets.

### 4b: PDF Cross-Validation

Replaced stub in `checkPageMarkers()` with actual `qpdf --show-npages` comparison.
Reports MATCH or MISMATCH between source marker count and rendered PDF page count.

**Files:** `validate/editorial.ts` — `getPdfPageCount()` + cross-validation logic

### 4c: Dead Code Audit

**Makefile:**
- No orphaned grep patterns (lint delegation clean)
- `.PHONY` list aligned with all defined targets
- Consolidated `toc-status`/`toc-parse` → `toc` (were identical)

**TS Builder — dead exports (intentionally kept as API surface):**
- `runPageStatus()` — delegates to `runPageParse()`, kept for API compat
- 5 spine helper functions — `getPartIds`, `getPart`, `getScopeDir`, `scopeFromDir`, `resolveBookDir`
- 6 config query functions — `getIcon`, `getScopeKeys`, `getScopeConfig`, `getBrandColors`, `getScriptureBasePath`, `getDefaultTranslation`
- 3 error utilities — `ErrorCollector`, `sectionTypeLabel`, `detectScope`

These are API surface for external consumers and future features. Not removed.

### 4d: Documentation

- REFACTOR.md updated with Phase 4 completion
- Makefile help section updated with new TOC targets

## Phase 5: Folder Restructure (Completed 2026-02-07)

Deflattened `src/` directory to match system growth. cli.ts (1277 lines) split into
focused modules.

### 5a: CLI Directory

Split monolithic `cli.ts` into `cli/` directory:

| Component | Lines | Purpose |
|-----------|:-----:|---------|
| `cli.ts` | 8 | Entry point shim (preserves `dist/cli.js` path) |
| `cli/index.ts` | 129 | Commander program setup + command registration |
| `cli/utils.ts` | 146 | exitWithError, delegateToMake, checkPrereqs, parseFormat |
| `cli/commands/build.ts` | 137 | Default action — multi-format publishing |
| `cli/commands/editorial.ts` | 96 | Make-delegated commands (7 targets) |
| `cli/commands/lint.ts` | 76 | Editorial lint + Bible + TOC validation |
| `cli/commands/toc.ts` | 60 | TOC parse, fill, verify |
| `cli/commands/validate.ts` | 54 | Structural validation |
| `cli/commands/spine.ts` | 42 | Spine + inventory |
| `cli/commands/assets.ts` | 49 | Asset generation |
| `cli/commands/config.ts` | 151 | Config management |

**Pattern:** Each command module exports `register*Command(program, config, paths)`.
The main `cli/index.ts` loads config, creates the Commander program, and registers
all commands. This keeps each file focused on one concern.

### 5b: Display Extraction

Extracted result display functions from cli.ts into `display/` directory:

| File | Functions | Source |
|------|-----------|--------|
| `display/validation.ts` | `displayFindings()` | Structure validation |
| `display/editorial.ts` | `displayEditorialResults()` | Editorial lint |
| `display/bible.ts` | `displayBibleFindings()` | Bible validation |
| `display/toc.ts` | `displayTocResults()`, `displayTocFillResult()`, `displayTocCheckResult()` | TOC pipeline |
| `display/spine.ts` | `displaySpineTree()` | Spine tree |
| `display/assets-report.ts` | `displayAssetReport()` | Asset generation |

### 5c: Barrel Updates

- `display/index.ts` — re-exports all 6 new display modules
- `src/index.ts` — exports new display functions + validate/pages types + editorial types
- `package.json` — bin entry unchanged (`dist/cli.js` preserved by shim)

### Impact

| Metric | Before | After |
|--------|--------|-------|
| `cli.ts` lines | 1,277 | 8 (shim) |
| CLI module total | 1 file | 11 files (948 lines) |
| `display/` files | 5 | 11 |
| Total source files | 35 | 51 |
| Directories | 6 | 8 (added `cli/`, `cli/commands/`) |
| Max file size | 1,277 (cli.ts) | 151 (config command) |

## Net Effect

| Metric | Before | After |
|--------|--------|-------|
| Makefile lines | 1842 | ~1549 |
| Builder source files | 26 | 51 |
| Builder directories | 5 | 8 |
| Max file size | 1,277 lines (cli.ts) | 151 lines (config command) |
| Lint checks | 8 (grep chains) | 17 (TS, categorized) |
| Check categories | 0 | 4 (editorial, style, structure, conversion) |
| Format targets | 4+3 disabled | 4+5 disabled (+markdown, +docx) |
| Make↔Builder duplication | 6 checks | 0 |
| Conversion formats | 0 | 2 (markdown, docx via pandoc pipeline) |
| Error handling | Mixed | Consistent E-codes across both systems |
| TOC pipeline | Manual placeholders | Computed fill + verify + cross-validation |
| PDF validation | Markers only | Markers + PDF cross-validation (qpdf) |
