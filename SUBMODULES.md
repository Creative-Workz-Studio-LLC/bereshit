# Submodule Reference

## What Are Submodules?

Git submodules are **repos inside repos, tracked by pointer**.

When you add a submodule, git stores:
1. The URL of the external repo (in `.gitmodules`)
2. The specific commit SHA it points to (in the parent's index)

**The files are physically present.** Once cloned, `cornerstone/` contains real files
you can compile, reference, and use like any other directory. The "pointer" is just
how git tracks it — from the filesystem's perspective, it's all real files.

---

## This Repository's Submodules

| Submodule | Path | Source | Purpose |
|-----------|------|--------|---------|
| Cornerstone | `cornerstone/` | `../Cornerstone` | Game engine (systems, templates, demo) |

---

## Common Operations

### First Time Setup (After Cloning)

```bash
git clone <this-repo>
cd Bereshit

# Initialize and fetch submodule content
git submodule init
git submodule update

# Or in one step:
git clone --recurse-submodules <this-repo>
```

### Update Submodule to Latest

```bash
# Go into submodule, get latest
cd cornerstone
git pull origin main
cd ..

# Commit the new pointer
git add cornerstone
git commit -m "[bereshit] SYNC: Update cornerstone submodule"
```

### Check Submodule Status

```bash
git submodule status
# Shows: commit-sha path (branch info)
```

### See What Changed in Submodule

```bash
cd cornerstone
git log --oneline -5
```

---

## How Builds Use Submodules

The Makefile compiles using submodule paths:

```makefile
CFLAGS = -I. -Icornerstone    # Include path for headers
SOURCES = cornerstone/engine/core/cornerstone.c ...
```

Code includes headers from submodule:
```c
#include "engine/core/include/scene.h"  // Resolved via -Icornerstone
```

**Key:** From the build's perspective, these are just regular files in a directory.

---

## The Mental Model

Think of it like a **bookmark** in a book:
- The bookmark (submodule reference) points to a specific page (commit)
- The book (Cornerstone) can have new pages added
- Your bookmark stays where you put it until you move it
- Different repos (TAL, Bereshit) can have bookmarks at different pages

**Why this matters:**
- TAL can stay on a stable engine version
- Cornerstone can be developed independently
- Update when you choose, not when changes happen

---

## Troubleshooting

### Submodule directory is empty
```bash
git submodule init
git submodule update
```

### "Not a git repository" errors in submodule
```bash
git submodule update --init --recursive
```

### Need to change submodule URL
Edit `.gitmodules`, then:
```bash
git submodule sync
git submodule update --init
```
