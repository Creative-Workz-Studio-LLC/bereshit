# Statusline

> *"Let all things be done decently and in order."* — 1 Corinthians 14:40

**CPI-SI Statusline display system.**

Assembles formatted statusline output from Claude Code session data.

---

## Structure

```bash
statusline/
├── cmd/
│   └── statusline/
│       └── main.go           # Entry point orchestrator
├── lib/
│   ├── display/              # Terminal colors and formatting
│   ├── features/             # Conditional display logic
│   ├── format/               # Path and text formatting
│   ├── git/                  # Git repository status
│   ├── session/              # Session statistics
│   ├── system/               # System health metrics
│   ├── temporal/             # Time awareness
│   └── types/                # Data type definitions
├── bin/                      # Built executables
├── go.mod
├── Makefile
└── README.md
```

---

## Build

```bash
make build
```

---

## Test

```bash
echo '{"session_id":"test","model":{"display_name":"Sonnet"},"cwd":"/tmp"}' | ./bin/statusline
```

---

## Display Elements

| Element | Library | What It Shows |
|---------|---------|---------------|
| Instance | - | Nova Dawn identity |
| Date/Time | - | Current timestamp |
| Time of Day | temporal | Morning/afternoon/evening awareness |
| Model | format | Claude model name |
| Directory | format | Shortened working directory |
| Git | git | Branch name and dirty status |
| System Load | system | CPU load average |
| Memory | system | Memory usage percentage |
| Lines | session | Lines added/removed |
| Duration | session | Session duration |
| Cost | session | API cost |
| Version | - | Claude Code version |
| Kingdom | features | Occasional reminder |

---

## Integration

Called by Claude Code hooks via stdin JSON:

```bash
# Hook invocation
echo "$CLAUDE_SESSION_JSON" | ./statusline
```

---

## Libraries

Each library in `lib/` handles a specific display concern:

| Library | Purpose |
|---------|---------|
| **types** | SessionContext data contract |
| **display** | ANSI color codes |
| **format** | Path shortening, model names |
| **temporal** | Time of day, session phase |
| **git** | Git status display |
| **session** | Lines, duration, cost |
| **system** | Load, memory, disk |
| **features** | Conditional display decisions |

---

> *"A word fitly spoken is like apples of gold in pictures of silver."* — Proverbs 25:11
