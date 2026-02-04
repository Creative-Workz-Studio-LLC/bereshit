# Linux-First Development

Ubuntu 25.10 dedicated workstation. All development follows Linux conventions.

## Core Principles

| Principle | Practice |
|-----------|----------|
| **Line endings** | LF only. Never CRLF. |
| **Encoding** | UTF-8 always |
| **Paths** | Forward slashes, case-sensitive |
| **Permissions** | Set appropriate file permissions (644 for files, 755 for executables) |
| **Shell** | Bash-compatible scripts |
| **Quotes** | Double-quote paths with spaces |

## Filesystem Operations

Prefer filesystem commands over load-then-write:

```bash
# Good — use filesystem operations
cp template.md destination.md
mv old.go new.go
mkdir -p path/to/dir

# Avoid — loading and writing when commands work
Read file → modify → Write file
```

**When to use filesystem commands:**
- Copying files (especially templates)
- Moving/renaming files
- Creating directory structures
- File permissions changes

**When to use Read/Edit/Write:**
- Content modifications
- Targeted edits within files
- When you need to inspect before deciding

## Targeted Retrieval

| Priority | Mode | How |
|:--------:|------|-----|
| 1️⃣ | **Target** | `grep -n "pattern" file` → Read with offset |
| 2️⃣ | **Expand** | If first target insufficient, gather related sections |
| 3️⃣ | **Full Load** | Last resort — only when structure doesn't exist |

**Read before Edit.** Always understand the file before modifying.

## Elevated Permissions

Use `pkexec` (not `sudo`) for elevated permissions:

```bash
pkexec apt install <package>
pkexec systemctl restart <service>
pkexec chmod 755 /path/to/file
```

**Why `pkexec`:** Displays GUI password dialog in desktop environments.

## Project Drive

| Path | Purpose |
|------|---------|
| `/media/seanje-lenox-wise/Project/` | External project drive (auto-mounted, exfat) |
| `/media/seanje-lenox-wise/Project/Bereshit/` | Main project repo |
| `/media/seanje-lenox-wise/Project/CreativeWorkzStudio_LLC/` | Company repo |
| `~/.claude/` | Claude Code configuration, CPI-SI binaries |

## The Test

Would this work on any Linux system without modification?

If it relies on Windows conventions, WSL quirks, or macOS specifics — rethink it.
