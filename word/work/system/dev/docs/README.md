# CPI-SI Development Documentation

**Purpose:** API documentation, design philosophy, and guides for CPI-SI development tools and systems

**Location:** `~/.claude/cpi-si/system/dev/docs/`

---

## Overview

The `docs/` folder contains deep documentation extracted from development code. This follows the Kingdom Technology documentation pattern:

| Location | Contents | Purpose |
|----------|----------|---------|
| **Code** | Standalone practical teaching with docstrings and inline comments | Learn by reading implementation |
| **API Docs** | Deep philosophy, design rationale, extensive explanations | Learn the "why" and architectural decisions |

**Both exist because people learn differently** - reinforced learning, not redundancy.

---

## Folder Structure

```
docs/
├── README.md              # This file - navigation guide
├── demos/                 # API docs for demonstration programs
├── templates/             # Deep documentation for template formats
├── debug/                 # Debug tooling documentation
└── guides/                # Development guides and how-tos
```

---

## Demonstrations API Documentation

**Location:** `docs/demos/`

**Purpose:** Deep philosophy and design rationale for demonstration programs

### Current Documentation

| File | Demonstrates | Status |
|------|--------------|--------|
| `rails-correlation-demo-api.md` | Rails concept (orthogonal logging infrastructure) | ✅ Complete |

**What these docs contain:**
- Execution flow diagrams
- Correlation analysis (how concepts connect)
- Modification policy (when to change vs extend)
- Implementation roadmap
- Architectural philosophy
- Design decisions and rationale

**Relationship to code:**
- Code (`dev/demo/rails_correlation_demo.go`) = Standalone teachable implementation
- API Doc = Deep philosophy and extensive explanations
- Both serve learning, different modalities

---

## Template Format Documentation

**Location:** `docs/templates/`

**Purpose:** Deep documentation of format structures for logging and debugging

### Planned Documentation

| File | Purpose | Status |
|------|---------|--------|
| `logging-format-api.md` | Deep dive into log format structure, philosophy, usage patterns | 🚧 Future |
| `debugging-format-api.md` | Deep dive into debug format structure, philosophy, usage patterns | 🚧 Future |

**What these will contain:**
- Format philosophy (why structure matters)
- Field-by-field explanation
- Health scoring integration
- Visual indicator design rationale
- Template parsing strategies
- Extension patterns
- Migration from hardcoded formats

**Relationship to templates:**
- Templates (`dev/templates/logging/`, `dev/templates/debugging/`) = JSONC format definitions
- API Docs = Philosophy, usage patterns, integration guidance

---

## Debug Tooling Documentation

**Location:** `docs/debug/`

**Purpose:** Documentation for debug analysis tools and workflows

### Planned Documentation

| File | Purpose | Status |
|------|---------|--------|
| Future debug tooling docs | As debug tools are built | 🚧 Future |

**Will document:**
- Debug analysis patterns
- Automated debugging workflows
- Pattern recognition in debug output
- Debug session correlation

---

## Development Guides

**Location:** `docs/guides/`

**Purpose:** How-to guides for developers working with CPI-SI system

### Planned Guides

| File | Purpose | Status |
|------|---------|--------|
| `template-integration-guide.md` | How to integrate template-based formatting into runtime | 🚧 Future |
| `adding-demonstrations-guide.md` | How to create new demonstration programs | 🚧 Future |
| `health-scoring-implementation-guide.md` | How to implement health scoring in new components | 🚧 Future |

**Guide philosophy:**
- Practical step-by-step instructions
- Code examples
- Common pitfalls and solutions
- Integration with existing patterns

---

## Documentation Standards

**All docs in this folder follow Kingdom Technology documentation standards:**

### 4-Block Structure (Implicit in Documentation)

```
METADATA (Header/Intro)
    ↓
SETUP (Prerequisites/Concepts)
    ↓
BODY (Main Content)
    ↓
CLOSING (References/Appendix)
```

**Applied to API docs:**
- Reader navigates naturally without seeing scaffolding
- Structure provides organization without being explicit
- Reinforces 4-block thinking across all documentation

### Documentation Levels

**API documentation provides Level 3-4 understanding:**

| Level | Purpose | Location |
|-------|---------|----------|
| **Level 1** | What it is | Code comments, docstrings |
| **Level 2** | How to use it | Code examples, inline comments |
| **Level 3** | Why it works this way | API docs (design rationale) |
| **Level 4** | When to use alternative approaches | API docs (modification policy, roadmap) |

---

## Adding New Documentation

### When to Create API Documentation

**Create API docs when:**
- ✅ Code has deep philosophical foundation
- ✅ Design decisions need explanation
- ✅ Multiple learning modalities serve users
- ✅ Architectural patterns deserve detailed exploration
- ✅ Code will become training data (educational value primary)

**Don't create API docs when:**
- ❌ Code is self-explanatory with good docstrings
- ❌ Documentation would just repeat what code says
- ❌ Simple utility with no deep philosophy

### Documentation Workflow

**When creating new demo or complex component:**

1. **Write code first** with proper docstrings and inline comments
2. **Identify deep philosophy** that doesn't fit in code
3. **Extract to API doc** (execution flow, correlation analysis, modification policy, roadmap)
4. **Reduce code weight** by removing lengthy explanations (replace with concise comments)
5. **Verify both standalone** - code teachable without doc, doc valuable without code
6. **Cross-reference** - code mentions API doc, API doc references code sections

**Example (Rails Demo):**
- Code: 1002 lines → 905 lines (10% lighter, higher quality)
- API Doc: 757 lines (deep philosophy extracted)
- Both serve learning, different modalities
- Reinforced learning, not redundancy

---

## Integration with System Architecture

**Documentation supports the immune system paradigm:**

| Stage | Component | Documentation Role |
|-------|-----------|-------------------|
| **Detection** | Logging | Template docs explain format philosophy |
| **Assessment** | Debugging | Debug docs show analysis patterns |
| **Restoration** | Response | Guides show how to implement fixes |

**Documentation is rails for knowledge:**
- Attaches to all components (orthogonal)
- Provides visibility (what's happening and why)
- Enables learning (from implementation to philosophy)
- Supports growth (guides show how to extend)

---

## Biblical Foundation

**Documentation serves others:**

"Let all things be done decently and in order." (1 Corinthians 14:40)

**Kingdom Technology documentation principles:**
- **Truth** - Accurate representation of how things work
- **Service** - Genuinely helps others understand
- **Excellence** - Right documentation in right places
- **Stewardship** - Captures wisdom for future developers

**Reinforced learning reflects creation:**
- People learn differently (visual, reading, doing)
- Multiple modalities honor how God made us
- Not redundancy - intentional multi-path teaching
- Code + philosophy = complete understanding

---

## Navigation

**By topic:**
- **Demonstrations** → `demos/` - Philosophy of demo programs
- **Templates** → `templates/` - Format structure deep dives
- **Debug Tools** → `debug/` - Debug analysis documentation
- **How-Tos** → `guides/` - Step-by-step development guides

**By learning style:**
- **Code-first learners** → Read implementations in `dev/demo/`, `system/lib/`
- **Philosophy-first learners** → Read API docs in `docs/demos/`, `docs/templates/`
- **Task-oriented learners** → Read guides in `docs/guides/`

**All paths lead to understanding** - choose what serves your learning.

---

*Documentation extracted from code enables both to excel at their purposes.*

*Last Updated: November 10, 2025*
