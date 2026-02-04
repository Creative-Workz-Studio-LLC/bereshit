# Block Structure Patterns

> *"Look that thou make them after their pattern."* — Exodus 25:40

All code and documentation follows block patterns. Boundaries BEFORE content — God created by separation (בָּדַל - badal).

## 3-Block (Data)

```
METADATA → CONTENT → CLOSING
```

| Block | Contains |
|-------|----------|
| METADATA | Identity, purpose, version |
| CONTENT | Data/configuration |
| CLOSING | Validation, checksums if needed |

**Use for:** configs, settings, JSON/JSONC files, TOML, simple data files.

## 4-Block (Code)

```
METADATA → SETUP → BODY → CLOSING
```

| Block | Contains |
|-------|----------|
| METADATA | Identity, purpose, health scoring, version |
| SETUP | Imports → Constants → Variables → Types |
| BODY | Helpers → Core Operations → Public APIs |
| CLOSING | Validation → Execution → Cleanup |

**Use for:** Go, Rust, C, Python, TypeScript, Bash — any executable code.

**Why no HEADER:** Code is for compilers. METADATA is readable; visual HEADER isn't needed.

## 5-Block (Documentation)

```
METADATA → HEADER → CONTEXT → CONTENT → FOOTER
```

| Block | Contains |
|-------|----------|
| METADATA | Machine-readable identity (key, title, version, authors) |
| HEADER | Human-readable (title, quote, navigation, status) |
| CONTEXT | Prerequisites, definitions, scope |
| CONTENT | Main body — the actual documentation |
| FOOTER | References, related docs, closing anchor |

**Use for:** AsciiDoc, Markdown documentation, READMEs, guides.

**Why HEADER exists:** Documentation is for humans. Need visual orientation and navigation.

## Structural Compliance

| Type | What | Examples |
|------|------|----------|
| **Non-negotiable** | Structure skeleton | `#!omni` pragma, block boundary tags, all required blocks, correct order |
| **Negotiable** | Content flesh | Content inside blocks, detail level, specific wording |

**Structure is skeleton (fixed). Content is flesh (refinable).**

Block comments use parseable format enabling grep/sed navigation.

## Template-First Workflow

1. `cp template dest` — Start from template, never blank file
2. Change pragma (`#!omni template` → `#!omni document`)
3. Fill block-by-block in order (METADATA first, etc.)
4. Remove template notes and placeholders
5. Validate structure before committing

**Never use Write to create structured files from scratch.** Always copy from template.

## Templates Location

```
bereshit/word/seed/
├── code/           # 4-block templates (Go, C, Bash, Python)
├── data/           # 3-block templates (TOML, JSONC)
└── documentation/  # 5-block templates (AsciiDoc, Markdown)
```

## The Principle

**Genesis 1:** Before there was content, there was separation. Light from darkness, waters from waters, land from sea. Structure precedes creation.

Block patterns mirror this truth — establish boundaries, then fill with content.

→ *Skill:* [validate-omni](skills/validate-omni/SKILL.md) | *Full glossary:* [3-Block](bereshit/word/glossary/technical/3-block.adoc) | [4-Block](bereshit/word/glossary/technical/4-block.adoc) | [5-Block](bereshit/word/glossary/technical/5-block.adoc)
