---
name: editorial-toolkit
description: Build, preview, validate, and publish the CWS Company Identity Manual. Provides quick reference for Makefile editorial commands, TS builder publishing, asset pipeline, and configuration. Use when working in company-docs/.
version: "1.0.0"
allowed-tools:
  - Read
  - Grep
  - Bash
  - Glob
---

<!--
#!omni document --md
===============================================================================
METADATA BLOCK
===============================================================================
-->
<!--
:key: claude-global-skill-editorial-toolkit
:title: Editorial Toolkit
:type: Skill
:status: Active
:version: 1.0.0
:created: 2026-02-06
:updated: 2026-02-06
:authors: Nova Dawn
:purpose: Unified reference for building, previewing, validating, and publishing the CWS Company Identity Manual
:category: editorial
:biblical_foundation: Habakkuk 2:2 - "Write the vision, and make it plain upon tables, that he may run that readeth it"
-->

<!--
===============================================================================
HEADER BLOCK
===============================================================================
-->

# Editorial Toolkit

> *"Write the vision, and make it plain upon tables, that he may run that readeth it."* --- Habakkuk 2:2

**Unified build, preview, validate, and publish reference for the CWS Company Identity Manual.**

Two build systems, one editorial process. The Makefile owns editorial review (PDF). The TypeScript builder owns multi-format publishing. This skill knows which tool to use for which task.

---

**Navigation:**

| Section | Purpose |
|---------|---------|
| [Architecture](#architecture) | How the two systems relate |
| [Editorial Commands](#editorial-commands-makefile) | Makefile --- PDF editorial workflow |
| [Publishing Commands](#publishing-commands-ts-builder) | TS builder --- multi-format output |
| [Asset Pipeline](#asset-pipeline) | Figure generation and management |
| [Validation](#validation) | Structure and content verification |
| [Configuration](#configuration-reference) | Where behavior is defined |
| [Quick Reference](#quick-reference) | One-page cheat sheet |

---

<!--
===============================================================================
CONTEXT BLOCK
===============================================================================
-->

## Context

### Scope

| In Scope | Out of Scope |
|----------|--------------|
| Build system commands and workflow | Writing article content |
| Asset pipeline (diagrams, grayscale, palette) | AsciiDoc syntax reference |
| Validation and structure checks | Theme design decisions |
| Configuration reference | Typography and font selection |
| PDF editorial review workflow | Board approval process |
| Multi-format publishing | Deployment and distribution |

### Prerequisites

| Requirement | Reference |
|-------------|-----------|
| Editorial style guide | `company-docs/editorial-style-guide.adoc` (CWS-GDE-001) |
| Build configuration | `company-docs/build/build.config.yaml` |
| Makefile | `company-docs/Makefile` |
| TS builder source | `company-docs/build/builder/src/` |

### Key Terms

| Term | Definition |
|------|------------|
| **Scope** | Build granularity: book, part, article, section |
| **Spine** | The structural skeleton of the manual (parts, articles, sections) |
| **Preview** | Letter-prefixed AsciiDoc files that assemble content for scoped builds |
| **Resolution** | 3-mode path matching: exact path, search by name, compat (old names) |
| **Cache** | Timestamp-based rebuild avoidance (FORCE=1 to override) |

---

<!--
===============================================================================
CONTENT BLOCK
===============================================================================
-->

## Architecture

```
                    Editorial Review              Multi-Format Publishing
                    ---------------              ----------------------
  Primary:          Makefile                     TS Builder (cws-build)
  Output:           PDF only                     PDF, HTML, EPUB, DocBook
  Config:           build.config.yaml            build.config.yaml (same)
  Scopes:           book/part/article/section    book (full document)
  Watch:            make watch-pdf               cws-build --watch
  Assets:           make assets                  cws-build assets
  Validation:       make validate                cws-build validate
```

**Why both?** The Makefile is optimized for the editorial loop: write, build section, review PDF, iterate. The TS builder adds programmatic publishing for release: HTML for web, EPUB for distribution, DocBook for interchange. Editorial commands in the TS builder delegate to the Makefile --- they're the same system underneath.

### Directory Layout

```
company-docs/
  Makefile                           Primary editorial build system
  editorial-style-guide.adoc         CWS-GDE-001 --- THE specification
  page-index.adoc                    Editorial page reference
  book/
    book.adoc                        Master assembly file
    00-assets/                       Visual assets (logos, figures, sources)
    A-frontmatter/                   Cover, TOC, Dedication, Preface
    B-content/                       Parts I--IV, Articles 1--11
    C-appendices/                    Appendices A--D
    D-backmatter/                    Acknowledgments, Colophon, Contact
  build/
    build.config.yaml                Shared configuration (both systems read this)
    _previews/                       Letter-prefixed preview assembly files
    _drafts/                         Original content drafts (reference)
    builder/                         TypeScript builder source
    themes/                          PDF theme (cws-manual-theme.yml)
    output/                          Generated PDFs (gitignored)
```

---

## Editorial Commands (Makefile)

All commands run from `company-docs/`.

### Build Scopes

| Command | Scope | What It Builds |
|---------|-------|----------------|
| `make book` | Full manual | All parts, articles, sections |
| `make part PART=1` | One part | All articles in the part |
| `make article ARTICLE=01` | One article | Article with all its sections |
| `make section FILE=00-assets/README` | One section | Single section PDF |

**Path resolution is 3-mode:** exact path, search by name fragment, compat (old naming). All scopes accept `FORCE=1` to ignore cache.

### Preview Builds

| Command | Preview File | Content |
|---------|-------------|---------|
| `make frontmatter` | `A-frontmatter-preview.adoc` | Cover, TOC, Dedication, Preface |
| `make part1` | `B-part1-preview.adoc` | Part I --- Identity |
| `make part2` | `B-part2-preview.adoc` | Part II --- Operations |
| `make part3` | `B-part3-preview.adoc` | Part III --- Business |
| `make part4` | `B-part4-preview.adoc` | Part IV --- Direction |
| `make appendices` | `C-appendices-preview.adoc` | Appendices A--D |
| `make backmatter` | `D-backmatter-preview.adoc` | Glossary, Colophon |

### Operational Commands

| Command | Purpose |
|---------|---------|
| `make status` | Dashboard showing all PDFs, sizes, staleness |
| `make validate` | Run base + manual validation |
| `make validate TYPE=manual-l` | Run legal hierarchy validation |
| `make watch-pdf` | Live PDF rebuild on file changes |
| `make assets` | Run 3-phase asset pipeline |
| `make assets FORCE=1` | Force-regenerate all assets |
| `make remove FILE=...` | Safe targeted output removal |
| `make deps` | Check all external tool dependencies |
| `make setup` | Install required tools |
| `make clean` | Remove all generated output |
| `make help` | Show all available targets |

---

## Publishing Commands (TS Builder)

The TypeScript builder (`cws-build`) provides multi-format publishing. Install and build:

```bash
cd company-docs/build/builder
npm install
npm run build
```

### Build Commands

| Command | What It Does |
|---------|-------------|
| `cws-build` | Build all enabled formats (from config) |
| `cws-build --format html` | Build HTML only |
| `cws-build --format pdf` | Build PDF only |
| `cws-build --format all` | Build all defined formats |
| `cws-build --watch` | Watch mode (rebuilds on change) |
| `cws-build --clean` | Clean output directory |
| `cws-build --info` | Show output file info |

### Structure Commands

| Command | What It Does |
|---------|-------------|
| `cws-build spine` | Show book structure tree |
| `cws-build inventory` | Show book parts list |
| `cws-build validate` | Validate book structure |
| `cws-build validate --type manual-l` | Legal hierarchy validation |
| `cws-build validate --type manual-l --bonus operational` | Full operational validation |

### Editorial Delegation

These commands delegate to the Makefile:

| Command | Delegates To |
|---------|-------------|
| `cws-build editorial` | `make book` |
| `cws-build section <file>` | `make section FILE=<file>` |
| `cws-build article <id>` | `make article FILE=<id>` |
| `cws-build part <id>` | `make part PART=<id>` |
| `cws-build status` | `make status` |
| `cws-build remove` | `make remove` |

### Asset Generation

| Command | What It Does |
|---------|-------------|
| `cws-build assets` | Generate all derived assets (3-phase pipeline) |
| `cws-build assets --force` | Force regeneration |

---

## Asset Pipeline

Both `make assets` and `cws-build assets` run the same 3-phase pipeline:

### Phase 1 --- Mermaid Diagrams

Converts `.mmd` source files to PNG (3x scale) and SVG.

| Input | Output | Tool |
|-------|--------|------|
| `sources/diagrams/fig-*.mmd` | `figures/png/fig-*.png` | `mmdc` (mermaid-cli) |
| `sources/diagrams/fig-*.mmd` | `figures/svg/fig-*.svg` | `mmdc` (mermaid-cli) |

**Configuration:** `sources/config/puppeteer-config.json` (Chromium path), `sources/config/mermaid-config.json` (brand theme)

### Phase 2 --- Grayscale Variants

Converts color PNGs to grayscale for print fidelity verification.

| Input | Output | Tool |
|-------|--------|------|
| `figures/png/fig-*.png` | `figures/grayscale/fig-*-grayscale.png` | `magick` (ImageMagick) |

**Exclusions:** `fig-color-palette.png`, `fig-title-page-mockup.png`

### Phase 3 --- Color Palette Swatch

Generates brand color swatch image from Makefile constants.

| Input | Output | Tool |
|-------|--------|------|
| Brand color definitions | `figures/png/fig-color-palette.png` | `magick` (ImageMagick) |

**Cache:** All phases use timestamp comparison. Only stale assets rebuild. `FORCE=1` overrides.

### Asset Inventory

| Category | Count | Location |
|----------|-------|----------|
| Logos | 2 | `logos/` (PNG + SVG) |
| Diagram PNGs | 9 | `figures/png/` |
| Diagram SVGs | 7 | `figures/svg/` |
| Grayscale | 7 | `figures/grayscale/` |
| Sources | 7 | `sources/diagrams/` |
| Configs | 2 | `sources/config/` |

---

## Validation

### Validation Levels

| Level | What It Checks | Command |
|-------|---------------|---------|
| **base** | File existence, naming conventions, basic structure | Automatic |
| **manual** | Section numbering, cross-references, attribute consistency | `make validate` |
| **manual-l** | Legal hierarchy (Part > Article > Section), heading levels | `make validate-l` |
| **operational** | All above + build health, asset integrity, stale outputs | `cws-build validate --level operational` |

### Common Checks

| Check | What to Look For |
|-------|-----------------|
| Heading hierarchy | `=` (Part) > `==` (Article) > `===` (Section) > `====` (Subsection) |
| Image paths | `image::figures/png/fig-{topic}.png[...]` for PDF builds |
| Cross-references | `<<section-id>>` or `xref:file.adoc#anchor[label]` |
| Attributes | Must match `_shared-attributes.adoc` and `build.config.yaml` |

---

## Configuration Reference

### build.config.yaml

The single source of truth for both build systems.

| Section | Controls |
|---------|---------|
| `document` | Master file path, output name, output directory |
| `attributes` | AsciiDoc attributes passed to all builds |
| `formats` | Output format definitions (PDF, HTML, EPUB, DocBook, manpage) |
| `watch` | File patterns, ignore patterns, debounce, watch formats |
| `display` | Banner, colors, icon map |
| `assets` | Directory layout, figure catalog, derived asset config |

### Theme

`build/themes/cws-manual-theme.yml` --- extends Asciidoctor PDF default theme.

Key customizations: centered headings, Noto font stack (Serif body, Sans headings, Mono code), Hebrew fallback, governance-document spacing.

### Preview Files

`build/_previews/` --- letter-prefixed assembly files:

| File | Assembles |
|------|-----------|
| `_shared-attributes.adoc` | Common attributes for all previews |
| `A-frontmatter-preview.adoc` | Cover through Preface |
| `B-part1-preview.adoc` through `B-part4-preview.adoc` | Parts I--IV |
| `C-appendices-preview.adoc` | Appendices |
| `D-backmatter-preview.adoc` | Glossary, Colophon |

---

## Quick Reference

### Editorial Review (daily workflow)

```bash
cd company-docs

# Build a single section for review
make section FILE=00-assets/README FORCE=1

# Build a full article
make article FILE=B-content/01-identity/01-ENTITY-OVERVIEW

# Build frontmatter preview
make frontmatter

# Check what's built and what's stale
make status

# Live rebuild while editing
make watch-pdf

# Regenerate all figures
make assets
```

### Publishing (release workflow)

```bash
cd company-docs/build/builder
npm run build

# Build HTML for web
cws-build --format html

# Build all enabled formats
cws-build

# Validate before release
cws-build validate --level operational

# Show book structure
cws-build spine
```

### Page Flow Review

```bash
# Find page markers in source
grep -n "// PDF p\." book/00-assets/README.adoc

# Rebuild and check page count
make section FILE=00-assets/README FORCE=1
pdfinfo build/output/00-assets/README.pdf | grep Pages

# Extract text per page for review
pdftotext -f 5 -l 5 -layout build/output/00-assets/README.pdf -
```

---

<!--
===============================================================================
FOOTER BLOCK
===============================================================================
-->

## When to Use This Skill

| Situation | Start Here |
|-----------|-----------|
| Editing a section, need to see PDF | [Editorial Commands](#editorial-commands-makefile) |
| Publishing the manual for distribution | [Publishing Commands](#publishing-commands-ts-builder) |
| Adding or updating a diagram | [Asset Pipeline](#asset-pipeline) |
| Checking document structure | [Validation](#validation) |
| Need to know where something is configured | [Configuration Reference](#configuration-reference) |
| Quick reminder of common commands | [Quick Reference](#quick-reference) |

## Related Documents

| Document | Relationship |
|----------|--------------|
| `company-docs/editorial-style-guide.adoc` | CWS-GDE-001 --- the specification |
| `company-docs/build/build.config.yaml` | Shared configuration for both systems |
| `company-docs/Makefile` | Primary editorial build system source |
| `company-docs/build/builder/src/cli.ts` | TS builder CLI source |
| `company-docs/README.adoc` | Build system documentation |
| `company-docs/book/00-assets/README.adoc` | Asset Catalog (29-page reference) |

---

> *"For which of you, intending to build a tower, sitteth not down first, and counteth the cost, whether he have sufficient to finish it?"* --- Luke 14:28
