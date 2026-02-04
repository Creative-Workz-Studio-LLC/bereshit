# Skill Template

> *"Look that thou make them after their pattern."* — Exodus 25:40

## Structure

```
skill-name/
├── SKILL.md           # Main skill documentation (5-block, YAML frontmatter first)
├── scripts/           # Go code helpers
│   ├── go.mod         # Go module
│   ├── main.go        # Entry point (if standalone tool)
│   └── [helpers].go   # Additional Go files
└── references/        # Supporting documentation
    └── [docs].md      # Reference materials
```

## Creating a New Skill

1. Copy this template folder to `claude-global/skills/[skill-name]/`
2. Rename and fill SKILL.md following 5-block structure
3. Add Go scripts if skill needs programmatic support
4. Add references if skill needs supporting docs

## SKILL.md Requirements

### YAML Frontmatter (MUST be first)

```yaml
---
name: skill-name
description: One-line description for skill discovery
version: "1.0.0"
allowed-tools: "Read,Write,Edit,Glob,Grep,Bash"
---
```

### 5-Block Structure

| Block | Purpose |
|-------|---------|
| METADATA | Machine-readable identity (key, title, category, etc.) |
| HEADER | Human-readable identity (title, nav, biblical quote) |
| CONTEXT | Scope, prerequisites, key terms |
| CONTENT | When to use, how it works, scripts, references, examples |
| FOOTER | Related documents, closing |

## Skill Categories

| Category | Skills | Purpose |
|----------|--------|---------|
| **creation** | create-from-template, create-journal-entry, create-whitepaper | File/content creation |
| **omnicode** | format-lookup, propagate-change, validate-omni | OmniCode operations |
| **session** | session-awareness, recognize-stopping-point | Time/rhythm awareness |
| **learning** | meta-awareness, recognize-pattern, reflect-on-session, integrate-learning | Growth and learning loop |

## Scripts Guidelines

Scripts provide programmatic support for skills. They should:

- Be Go code (compiled, quality assurance)
- Follow 4-block structure
- Have clear input/output
- Be buildable and testable

```go
// Example script structure
package main

import (
    "fmt"
    "os"
)

func main() {
    // Script logic
}
```

## References Guidelines

References provide supporting documentation:

- Lookup tables, mappings
- Detailed specifications
- Examples and templates
- External documentation summaries
