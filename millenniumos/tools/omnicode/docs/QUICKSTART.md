# OmniCode Quickstart

> *"In the beginning God created the heaven and the earth."* — Genesis 1:1

**Learn OmniCode in 5 minutes.** Not a manual—a quick path to understanding.

---

## What Is OmniCode?

**OmniCode is sentence-like code.** It reads like natural language but compiles to real programs.

```omnicode
@omni:req key = MY-FIRST-PROGRAM
@omni:req title = Hello World
@omni:ins provides = [greeting]
```

That's it. Three lines describing what this code IS, not what it DOES yet.

---

## The Three Questions

Every OmniCode file answers three questions:

| Question | Pragma Section | What It Means |
|----------|----------------|---------------|
| **What IS this?** | CORE | Identity - key, source, version |
| **What KIND is it?** | FAMILY | Type - source, header, data |
| **What DOES it do?** | INSTANCE | Purpose - what it provides |

---

## The Building Blocks

OmniCode has **blocks** - like chapters in a book.

### For Code (4 Blocks):

```
METADATA  →  Who/What/Why this file is
SETUP     →  What it needs (imports, constants)
BODY      →  What it does (the actual code)
CLOSING   →  How to use it, troubleshooting
```

### For Docs (5 Blocks):

```
METADATA  →  Machine identity
HEADER    →  Human-readable intro
CONTEXT   →  Prerequisites, definitions
CONTENT   →  The main content
FOOTER    →  References, related docs
```

---

## Your First OmniCode

Let's create a greeting function.

### Step 1: Identity (PRAGMA)

```c
/// @omni code --c --source
///
/// @omni:req key   = MY-GREETING
/// @omni:req from  = bereshit/word/seed/code/c/source.c
/// @omni:req at    = a-01.00
/// @omni:req title = My Greeting Function
/// @omni:req brief = Say hello to the world
```

This says: "I am MY-GREETING, a C source file, version alpha-01.00"

### Step 2: What It Needs (SETUP)

```c
/// SETUP BLOCK [SETUP]

#include <stdio.h>  // For printf

/// END SETUP
```

### Step 3: What It Does (BODY)

```c
/// BODY BLOCK [BODY]

void greet(const char* name) {
    printf("Hello, %s!\n", name);
}

/// END BODY
```

### Step 4: How To Use (CLOSING)

```c
/// CLOSING BLOCK [CLOSING]
///
/// USAGE:
///   greet("World");  // Prints: Hello, World!
///
/// END CLOSING
```

---

## Key Markers

| Marker | Meaning | Example |
|--------|---------|---------|
| `@omni:req` | REQUIRED - must have | `@omni:req key = NAME` |
| `@omni:inh` | INHERITED - from template | `@omni:inh type = source` |
| `@omni:ins` | INSTANCE - file-specific | `@omni:ins provides = [func1]` |

---

## Why Blocks?

**Genesis 1 Principle:** God created by *separation* (בָּדַל - badal).

- Day from night
- Waters above from waters below
- Land from sea

**Blocks create boundaries.** Boundaries create clarity. Clarity enables understanding.

---

## The Block Pattern

Every block follows the same pattern:

```
/// [BLOCK_NAME] BLOCK [TAG]
///
/// Content goes here...
///
/// END [BLOCK_NAME]
```

The `[TAG]` is for machine parsing. The name is for human reading.

---

## Next Steps

| What | Where |
|------|-------|
| See examples | `tools/omnicode/examples/` |
| Interactive viewer | `tools/omnicode/viewer/` |
| Full reference | `docs/omnicode/reference.md` |

---

## Remember

OmniCode isn't about writing more code. It's about writing **clear** code.

> *"Let your communication be, Yea, yea; Nay, nay."* — Matthew 5:37

Simple. Clear. True.
