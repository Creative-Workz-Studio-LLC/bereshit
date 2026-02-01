# OmniCode Learning System

> *"For precept must be upon precept, line upon line."* — Isaiah 28:10

**Learn OmniCode through seeing, not just reading.**

---

## Quick Start

1. Read [QUICKSTART.md](docs/QUICKSTART.md) - 5 minutes to understand OmniCode
2. Explore [examples/](examples/) - See real OmniCode files
3. Run the viewer - Watch blocks light up, see connections form

---

## Components

### Documentation (`docs/`)

| File | Purpose |
|------|---------|
| [QUICKSTART.md](docs/QUICKSTART.md) | 5-minute introduction |
| [SYNTAX.md](docs/SYNTAX.md) | Complete syntax reference |

### Examples (`examples/`)

| File | What It Teaches |
|------|-----------------|
| [example_01_minimal.omni](examples/example_01_minimal.omni) | Smallest valid OmniCode file |
| [example_01_minimal.generated.c](examples/example_01_minimal.generated.c) | What gets generated |
| [example_02_health.omni](examples/example_02_health.omni) | Header structure with types |

### Viewer (`viewer/`)

| File | Purpose |
|------|---------|
| [viewer.h](viewer/viewer.h) | OmniCode structure viewer |
| [viewer.c](viewer/viewer.c) | Parse and display with learning mode |
| [progress.h](viewer/progress.h) | Transpilation progress visualization |
| [progress.c](viewer/progress.c) | "Watch specs become code" |

---

## Learning Modes

### 1. Structure View

See the building blocks:

```
┌──────────────────────────────────────────────────────────────┐
│  METADATA   │   SETUP   │    BODY    │   CLOSING            │
│  (blue)     │   (cyan)  │   (plum)   │   (steel)            │
└──────────────────────────────────────────────────────────────┘
```

Each block is color-coded. Size shows relative content.

### 2. Pragma View

See the identity fields:

```
[req] key    = MY-FILE
[inh] type   = source
[ins] layer  = kernel
```

Color-coded by requirement level:
- 🔴 req (Required) - Must have
- 🟡 inh (Inherited) - From template
- 🟢 ins (Instance) - File-specific
- 🔵 opt (Optional) - Can omit

### 3. Learning Mode

Select any block or field to see explanations:

```
BODY is where the work happens.

Contains:
- Functions and implementations
- The actual logic
- Helper functions first
- Public API last

This is the heart of the file.
```

### 4. Progress View

Watch transpilation happen:

```
SOURCE → PARSE → VALIDATE → TRANSFORM → EMIT → COMPLETE
  ■        ■        ▶━━━━━       □         □        □

Building Blocks:
  [METADATA] → [SETUP] → [BODY] → [CLOSING]
                            ▲
                         (current focus)
```

---

## The Genesis Principle

OmniCode structure comes from Genesis 1:

**"And God divided the light from the darkness."** (Genesis 1:4)

God created by *separation* (בָּדַל - badal):
- Light from darkness
- Waters above from below
- Land from sea

**Blocks create separation.** Separation creates clarity. Clarity enables understanding.

---

## Controls

| Key | Action |
|-----|--------|
| S | Structure view |
| P | Pragma view |
| L | Learning mode |
| 1-8 | Select block |
| T | Toggle step/auto mode |
| SPACE | Step (in step mode) |
| R | Reset progress |
| ESC | Exit |

---

## Next Steps

1. ✅ Read the quickstart
2. ✅ Study the examples
3. ⬜ Create your first OmniCode file
4. ⬜ Watch it transpile to C
5. ⬜ Compile and run!

---

*"And God said, Let there be light: and there was light."* — Genesis 1:3

The word becomes reality. Your specification becomes code.
