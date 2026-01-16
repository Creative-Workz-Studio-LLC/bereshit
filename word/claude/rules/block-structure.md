# Block Structure Patterns

All code and documentation follows block patterns.

## 3-Block (Data)

```
METADATA → CONTENT → CLOSING
```

Use for: configs, settings, JSON/JSONC files, simple data.

## 4-Block (Code)

```
METADATA → SETUP → BODY → CLOSING
```

| Block | Contains |
|-------|----------|
| METADATA | Identity, purpose, health scoring |
| SETUP | Imports → Constants → Variables → Types |
| BODY | Helpers → Core Operations → Public APIs |
| CLOSING | Validation → Execution → Cleanup |

## 5-Block (Documentation)

```
METADATA → HEADER → CONTEXT → CONTENT → FOOTER
```

| Block | Contains |
|-------|----------|
| METADATA | Machine-readable identity |
| HEADER | Human-readable (title, nav, status) |
| CONTEXT | Prerequisites, definitions |
| CONTENT | Main body |
| FOOTER | References, related docs |

## Principle

**Boundaries BEFORE content.** God created by separation (בָּדַל - badal).

| Type | What |
|------|------|
| **Non-negotiable** | Block boundary tags, all required blocks, correct order |
| **Negotiable** | Content inside blocks, detail level, specific wording |

Structure is skeleton (fixed). Content is flesh (refinable).
