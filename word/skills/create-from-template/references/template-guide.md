# Template Creation Guide

> *"Look that thou make them after their pattern."* — Exodus 25:40

## Why Templates Exist

Templates embody the **building block method** in code form. They are **Foundation (0)** - what you build FROM, not create.

### The Problem Without Templates

Without templates, code creation devolves into:
- **Throwing code at the wall** - No structure, just trial and error
- **Inconsistent structure** - Every file different, hard to navigate
- **Missing essentials** - Forgotten imports, missing error handling
- **No health scoring** - Can't track code quality systematically

### The Solution With Templates

Templates provide:
- **Structural consistency** - Every file follows block patterns
- **Health scoring readiness** - METADATA enables quality tracking
- **Biblical grounding** - Purpose and foundation for every file
- **Cognitive scaffolding** - Structure guides thought, prevents drift

---

## Template Hierarchy

```
bereshit/word/
├── omni/seed/           ← MASTER: OmniCode templates
│   ├── universal.omni   ← Root (everything derives)
│   ├── code.omni        ← 4-block code master
│   ├── documentation.omni ← 5-block doc master
│   └── interface.omni   ← 3-block data master
└── seed/                ← FORMAT-SPECIFIC: Ready to use
    ├── code/            ← Go, C, ASM, Shell, Make
    │   ├── go/          ← executable.go, library.go, demo-test.go
    │   ├── c/           ← source.c, header.h
    │   ├── asm/         ← source.asm, arm-source.s
    │   ├── shell/       ← script.sh, shell-script-4block.sh
    │   └── make/        ← makefile.mk
    ├── data/            ← Docker, JSON, TOML, YAML
    │   ├── docker/      ← dockerfile.dockerfile, compose.yaml
    │   ├── json/        ← data.json, health-map.json
    │   ├── toml/        ← config.toml
    │   └── yaml/        ← yaml-config.yaml
    └── documentation/   ← AsciiDoc, Markdown, Typst
        ├── adoc/        ← asciidoc-base.adoc, asciidoc-community.adoc
        ├── md/          ← markdown-base.md, markdown-community.md
        └── typ/         ← typst-base.typ
```

**Key insight:** OmniCode templates are the master. Format-specific templates are transpiled/derived expressions of the master.

---

## Block Structures

### 3-Block (Data/Configuration)

```
METADATA → CONTENT → CLOSING
```

**Use for:** Configs, settings, JSON, TOML, YAML, Dockerfiles

**Structure:**
- **METADATA** - Identity, purpose, biblical foundation
- **CONTENT** - The actual data/configuration
- **CLOSING** - Comments, notes, version info

### 4-Block (Code/Executables)

```
METADATA → SETUP → BODY → CLOSING
```

**Use for:** Go, C, ASM, Shell, Makefiles

**Structure:**
- **METADATA** - Identity, purpose, health scoring
- **SETUP** - Imports → Constants → Variables → Types → Type Methods
- **BODY** - Org Chart → Helpers → Core Operations → Error Handling → Public APIs
- **CLOSING** - Validation → Execution → Cleanup → Documentation

### 5-Block (Documentation)

```
METADATA → HEADER → CONTEXT → CONTENT → FOOTER
```

**Use for:** AsciiDoc, Markdown, Typst documents

**Structure:**
- **METADATA** - Machine-readable identity (often in comments)
- **HEADER** - Human-readable identity (title, navigation)
- **CONTEXT** - Prerequisites, definitions, scope
- **CONTENT** - Main body
- **FOOTER** - References, related documents

---

## Building Block Method in Templates

Templates ARE the building block method encoded:

### Foundation (0) - The Template Itself

The template is your anchor. You cannot violate its structure:
- Block order is fixed
- Required sections must exist
- Markers must be present

### Build Up (+1) - Filling In

From the template foundation, you layer content:
1. METADATA - Fill with file-specific identity
2. SETUP - Add your imports, types, constants
3. BODY - Implement your logic
4. CLOSING - Add validation, cleanup, execution

### Break Down (-1) - Verification

Trace back to verify:
- Does structure match template?
- Are all blocks present?
- Can validate-omni confirm structure?

---

## Template Selection Guide

| If Creating... | Category | Block | Template |
|----------------|----------|-------|----------|
| Go CLI tool | code | 4-block | `go/executable.go` |
| Go library | code | 4-block | `go/library.go` |
| Go test file | code | 4-block | `go/demo-test.go` |
| C implementation | code | 4-block | `c/source.c` |
| C header | code | 4-block | `c/header.h` |
| x86 assembly | code | 4-block | `asm/source.asm` |
| ARM assembly | code | 4-block | `asm/arm-source.s` |
| Shell script | code | 4-block | `shell/script.sh` |
| Makefile | code | 4-block | `make/makefile.mk` |
| Dockerfile | data | 3-block | `docker/dockerfile.dockerfile` |
| Docker Compose | data | 3-block | `docker/compose.yaml` |
| TOML config | data | 3-block | `toml/config.toml` |
| YAML config | data | 3-block | `yaml/yaml-config.yaml` |
| JSON data | data | 3-block | `json/data.json` |
| AsciiDoc doc | documentation | 5-block | `adoc/asciidoc-base.adoc` |
| Markdown doc | documentation | 5-block | `md/markdown-base.md` |
| Typst doc | documentation | 5-block | `typ/typst-base.typ` |

---

## Placeholders in Templates

Templates contain placeholders to replace:

| Placeholder | Example | Replace With |
|-------------|---------|--------------|
| `{{KEY}}` | `my-new-module` | Unique file identifier |
| `{{PURPOSE}}` | `Handle user authentication` | What this file does |
| `{{BIBLICAL}}` | `Proverbs 4:26` | Grounding verse |
| `{{VERSION}}` | `1.0.0` | Initial version |
| `{{DATE}}` | `2025-12-09` | Creation date |
| `{{AUTHORS}}` | `Nova Dawn` | Who created it |

---

## When NOT to Use Templates

- **Editing existing files** - Use Edit tool, preserve existing structure
- **Third-party code** - Respect their conventions
- **Temporary/scratch files** - No structure needed
- **Files with established structure** - Don't overwrite

---

## Disciplined Creation Process

### 1. Select Template

```bash
./bin/template list code
./bin/template show go executable
```

### 2. Create File

```bash
./bin/template create go executable path/to/new-file.go
```

### 3. Fill METADATA

Update with file-specific values:
```go
// Key: my-new-module
// Purpose: Handle user authentication for API
// Biblical: Proverbs 4:26 - "Ponder the path of thy feet"
// Version: 1.0.0
// Created: 2025-12-09
```

### 4. Implement

Fill SETUP, BODY, CLOSING following block patterns.

### 5. Validate

```bash
./bin/validate path/to/new-file.go
```

---

## Common Mistakes

| Mistake | Why Wrong | Fix |
|---------|-----------|-----|
| Skipping METADATA | No identity, can't health score | Always fill METADATA first |
| Wrong block order | Violates structure | Follow template exactly |
| Missing blocks | Incomplete structure | Include all required blocks |
| Ignoring biblical foundation | Lose grounding | Add relevant verse |
| No validation | Drift undetected | Run validate-omni |

---

## Related Resources

- [template-inventory.md](template-inventory.md) - Full template catalog
- [bereshit/word/seed/](bereshit/word/seed/) - Canonical templates
- [validate-omni skill](../../validate-omni/) - Structure validation
- [format-lookup skill](../../format-lookup/) - Format mappings
