# Linux-First Development

## Core Principles

| Principle | Practice |
|-----------|----------|
| **Line endings** | LF only. Never CRLF. |
| **Encoding** | UTF-8 always |
| **Paths** | Forward slashes, case-sensitive |
| **Permissions** | Set appropriate file permissions |
| **Shell** | Bash-compatible scripts |

## Filesystem Operations

Prefer filesystem commands over load-then-write:

```bash
# Good
cp template.md destination.md
mv old.go new.go
mkdir -p path/to/dir

# Avoid
read file → modify → write file (when filesystem ops work)
```

## Targeted Retrieval

| Priority | Mode | How |
|:--------:|------|-----|
| 1️⃣ | **Target** | `grep -n "section" file` → Read with offset |
| 2️⃣ | **Expand** | If first target insufficient, gather related sections |
| 3️⃣ | **Full Load** | Last resort - only when structure doesn't exist |

## The Test

Would this work on any Linux system without modification?
