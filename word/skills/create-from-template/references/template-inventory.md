# Template Inventory

> *"Look that thou make them after their pattern."* — Exodus 25:40

## Canonical Location

**All templates live in:** `bereshit/word/seed/`

This is the single source of truth. Templates in this skill's references folder are documentation only - the actual templates are in bereshit.

---

## Code Templates (4-Block)

**Location:** `bereshit/word/seed/code/`

### Go (`code/go/`)

| Template | Purpose | Use For |
|----------|---------|---------|
| `executable.go` | Standalone program | CLI tools, services, main packages |
| `library.go` | Reusable package | Libraries, modules, pkg/ code |
| `demo-test.go` | Test file | Unit tests, integration tests |

### C (`code/c/`)

| Template | Purpose | Use For |
|----------|---------|---------|
| `source.c` | C source file | Implementation files |
| `header.h` | C header file | Interface declarations |

### Assembly (`code/asm/`)

| Template | Purpose | Use For |
|----------|---------|---------|
| `source.asm` | x86 assembly | NASM x86 assembly files |
| `arm-source.s` | ARM assembly | ARM architecture assembly |

### Shell (`code/shell/`)

| Template | Purpose | Use For |
|----------|---------|---------|
| `script.sh` | Basic shell script | Simple automation |
| `shell-script-4block.sh` | Full 4-block shell | Complex scripts |
| `shell-atomic-3block.sh` | Minimal 3-block | Single-purpose tools |

### Make (`code/make/`)

| Template | Purpose | Use For |
|----------|---------|---------|
| `makefile.mk` | Makefile template | Build orchestration |

---

## Data Templates (3-Block)

**Location:** `bereshit/word/seed/data/`

### Docker (`data/docker/`)

| Template | Purpose | Use For |
|----------|---------|---------|
| `dockerfile.dockerfile` | Container definition | Application containers |
| `go-builder.dockerfile` | Go build container | Multi-stage Go builds |
| `compose.yaml` | Docker Compose | Multi-container apps |

### JSON (`data/json/`)

| Template | Purpose | Use For |
|----------|---------|---------|
| `data.json` | Basic JSON data | Configuration, data |
| `json-data.json` | Structured data | Typed data files |
| `health-format.json` | Health format | Health scoring data |
| `health-map.json` | Health mapping | Component health |
| `json-health-map.json` | JSON health map | Health metadata |

### TOML (`data/toml/`)

| Template | Purpose | Use For |
|----------|---------|---------|
| `config.toml` | TOML config | Application config |

### YAML (`data/yaml/`)

| Template | Purpose | Use For |
|----------|---------|---------|
| `yaml-config.yaml` | YAML config | Kubernetes, CI/CD |

---

## Documentation Templates (5-Block)

**Location:** `bereshit/word/seed/documentation/`

### AsciiDoc (`documentation/adoc/`)

| Template | Purpose | Use For |
|----------|---------|---------|
| `asciidoc-base.adoc` | Base document | General documentation |
| `asciidoc-community.adoc` | Community docs | README, CONTRIBUTING |
| `asciidoc-index.adoc` | Index document | Directory indices |
| `asciidoc-research.adoc` | Research doc | Research papers, analysis |

### Markdown (`documentation/md/`)

| Template | Purpose | Use For |
|----------|---------|---------|
| `markdown-base.md` | Base document | General documentation |
| `markdown-community.md` | Community docs | README, CONTRIBUTING |
| `markdown-index.md` | Index document | Directory indices |

### Typst (`documentation/typ/`)

| Template | Purpose | Use For |
|----------|---------|---------|
| `typst-base.typ` | Typst document | Academic, formal docs |

---

## Skill Template

**Location:** `bereshit/word/seed/skill/`

| Template | Purpose | Use For |
|----------|---------|---------|
| `SKILL.md` | Skill documentation | New skill SKILL.md files |
| `README.md` | Skill readme | Skill overview |

---

## OmniCode Templates

**Location:** `bereshit/word/omni/seed/`

Master templates that all format-specific templates derive from:

| Template | Block | Purpose |
|----------|-------|---------|
| `universal.omni` | N/A | Root seed - all derive from this |
| `code.omni` | 4-block | Code template master |
| `documentation.omni` | 5-block | Documentation template master |
| `interface.omni` | 3-block | Data/interface template master |

---

## Quick Reference

### By Task

| Creating... | Use This Template |
|-------------|-------------------|
| Go CLI tool | `code/go/executable.go` |
| Go library | `code/go/library.go` |
| Go test | `code/go/demo-test.go` |
| C source | `code/c/source.c` |
| Shell script | `code/shell/script.sh` |
| Dockerfile | `data/docker/dockerfile.dockerfile` |
| TOML config | `data/toml/config.toml` |
| AsciiDoc doc | `documentation/adoc/asciidoc-base.adoc` |
| Markdown doc | `documentation/md/markdown-base.md` |

### By Block Structure

| Block | Categories | Count |
|-------|------------|-------|
| 4-block | Code (Go, C, ASM, Shell, Make) | 11 templates |
| 3-block | Data (Docker, JSON, TOML, YAML) | 10 templates |
| 5-block | Documentation (AsciiDoc, Markdown, Typst) | 8 templates |

---

## Template Anatomy

All templates follow block structure:

```
┌─────────────────────────────────┐
│         METADATA BLOCK          │ ← Identity, purpose, biblical foundation
├─────────────────────────────────┤
│      SETUP/HEADER/CONTENT       │ ← Block 2 (varies by type)
├─────────────────────────────────┤
│       BODY/CONTEXT/...          │ ← Block 3 (varies by type)
├─────────────────────────────────┤
│     CLOSING/FOOTER/...          │ ← Final block
└─────────────────────────────────┘
```

**METADATA always contains:**
- Key (unique identifier)
- Purpose (what this file does)
- Biblical foundation (grounding verse)
- Authors, version, dates

---

## Using Templates

### CLI Tool

```bash
# List all templates
./bin/template list

# Show specific template
./bin/template show go executable

# Create from template
./bin/template create go executable path/to/new-file.go
```

### Manual

```bash
# Copy template
cp bereshit/word/seed/code/go/executable.go path/to/new-file.go

# Edit - update METADATA with:
# - Unique Key
# - Specific Purpose
# - Relevant Biblical verse
# - Your authorship

# Validate
./bin/validate path/to/new-file.go
```

---

## Related

- [template-guide.md](template-guide.md) - WHY documentation
- [bereshit/word/seed/](bereshit/word/seed/) - Canonical templates
- [validate-omni skill](../../validate-omni/) - Structure validation
