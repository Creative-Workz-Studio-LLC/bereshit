<div align="center">

# 📚 CPI-SI Shared Library

**Universal data providers for CPI-SI framework components**

![Go](https://img.shields.io/badge/Go-00ADD8?style=flat&logo=go&logoColor=white)
![Linux](https://img.shields.io/badge/Linux-FCC624?style=flat&logo=linux&logoColor=black)
![License](https://img.shields.io/badge/License-Proprietary-red)

*The lower rung - providing data to all framework components*

[Architecture](#architecture) • [Libraries](#library-modules) • [Usage](#usage) • [Extension](#extending-the-library)

</div>

---

## Table of Contents

- [📚 CPI-SI Shared Library](#-cpi-si-shared-library)
  - [Table of Contents](#table-of-contents)
  - [Overview](#overview)
    - [What It Provides](#what-it-provides)
    - [What It Does NOT Provide](#what-it-does-not-provide)
  - [Architecture](#architecture)
    - [The Lower Rung](#the-lower-rung)
    - [Design Principles](#design-principles)
  - [Library Modules](#library-modules)
    - [git - Repository Operations](#git---repository-operations)
    - [system - System Information](#system---system-information)
    - [fs - File System Utilities](#fs---file-system-utilities)
    - [instance - Configuration Loader](#instance---configuration-loader)
  - [Usage](#usage)
    - [Importing](#importing)
    - [Module Path](#module-path)
    - [Example Usage](#example-usage)
  - [Extending the Library](#extending-the-library)
    - [When to Add Shared Functionality](#when-to-add-shared-functionality)
    - [How to Add a New Module](#how-to-add-a-new-module)
  - [Quick Reference](#quick-reference)
    - [Current Modules](#current-modules)
    - [Import Paths](#import-paths)
    - [Module Dependencies](#module-dependencies)
    - [Directory Structure](#directory-structure)

---

## Overview

The **CPI-SI Shared Library** (`claude/lib`) is the foundational data layer for the entire CPI-SI framework. It provides raw, unformatted data to all framework components (hooks, statusline, and future additions) following the Principle of Ladders and Batons.

> [!NOTE]
> This library is **framework-level** (universal for all instances). It provides data, not presentation. Components above it handle formatting and display.

### What It Provides

| What | Examples |
|------|----------|
| **Raw Data** | Git status, system load, file existence |
| **Data Structures** | `GitInfo`, `SystemInfo`, configuration structs |
| **Boolean Checks** | File exists, path is newer than another |
| **System Queries** | Read `/proc/`, check git repository state |

### What It Does NOT Provide

| What NOT to Add Here | Where It Goes Instead |
|---------------------|------------------------|
| Formatted output | Hooks/statusline presentation libs |
| Display logic | Component-specific lib/ folders |
| Instance-specific behavior | Instance configuration |
| Terminal colors | Display/formatting components |

---

## Architecture

### The Lower Rung

In the Ladder and Batons architecture, `claude/lib` is the **lower rung** - the foundation that everything else builds on:

```bash
Claude Code (event generator)
  ↓
┌─────────────────────┬─────────────────────┬─────────────────────┐
│ Hooks               │ Statusline          │ Future Components   │
│ (orchestrators)     │ (orchestrator)      │ (orchestrators)     │
└─────────────────────┴─────────────────────┴─────────────────────┘
  ↓                         ↓                         ↓
┌─────────────────────────────────────────────────────────────────┐
│                    claude/lib (LOWER RUNG)                      │
│  Provides: git, system, fs, instance data to all above         │
└─────────────────────────────────────────────────────────────────┘
  ↓
Linux System (/proc, /sys, .git, filesystem)
```

**Key Characteristics:**

- **Lower rung** = consumed by components above
- **Parallel batons** = multiple components use it simultaneously
- **One-way flow** = data flows UP (lib → components), never DOWN
- **No dependencies** = only depends on Go stdlib and Linux system

### Design Principles

| Principle | Implementation |
|-----------|----------------|
| **Data, Not Presentation** | Returns structs and booleans, never formats or prints |
| **Framework-Level** | Universal for all instances - no instance-specific logic |
| **Stateless** | Each function is independent, no global state |
| **Linux-First** | Reads `/proc/`, `/sys/`, uses POSIX paths |
| **4-Block Structure** | All files follow METADATA, SETUP, BODY, CLOSING |

---

## Library Modules

### git - Repository Operations

**File:** `git/operations.go`

**Purpose:** Provides git repository state without executing git commands when possible.

**Functions:**

```go
// Check if path is a git repository
func IsGitRepository(path string) bool

// Get comprehensive git information
func GetInfo(repoPath string) GitInfo
```

**GitInfo Structure:**

```go
type GitInfo struct {
    Branch           string  // Current branch name or detached HEAD hash
    Dirty            bool    // Working directory has uncommitted changes
    AheadCount       int     // Commits ahead of remote
    BehindCount      int     // Commits behind of remote
    UncommittedCount int     // Number of uncommitted changes
}
```

**Implementation Details:**

- Reads `.git/HEAD` directly for branch (fast, no git command)
- Uses `git status --porcelain` for dirty state
- Uses `git rev-list` for ahead/behind counts
- Returns empty GitInfo if not a repository

**Used By:** Hooks (session start, reminders), Statusline (git display)

---

### system - System Information

**File:** `system/info.go`

**Purpose:** Provides system metrics from `/proc/` filesystem.

**Functions:**

```go
// Get system load average
func GetLoadAverage() LoadInfo

// Get memory usage
func GetMemoryInfo() MemoryInfo

// Get disk usage for path
func GetDiskUsage(path string) DiskInfo
```

**Structures:**

```go
type LoadInfo struct {
    Load1Min  float64  // 1-minute load average
    CPUCount  int      // Number of CPU cores
}

type MemoryInfo struct {
    TotalGB float64    // Total memory in GB
    UsedGB  float64    // Used memory in GB
}

type DiskInfo struct {
    UsedPercent int    // Disk usage percentage
}
```

**Implementation Details:**

- Reads `/proc/loadavg` for load average
- Reads `/proc/cpuinfo` for CPU count
- Reads `/proc/meminfo` for memory stats
- Uses `syscall.Statfs` for disk usage

**Used By:** Hooks (session warnings), Statusline (system display)

---

### fs - File System Utilities

**File:** `fs/utils.go`

**Purpose:** Common file system checks and comparisons.

**Functions:**

```go
// Check if path exists
func PathExists(path string) bool

// Check if file1 is newer than file2
func FileIsNewer(file1, file2 string) bool
```

**Implementation Details:**

- Uses `os.Stat()` for existence checks
- Compares `ModTime()` for newer checks
- Returns false on any errors (safe defaults)

**Used By:** Hooks (dependency checking)

---

### instance - Configuration Loader

**File:** `instance/config.go`

**Purpose:** Loads instance identity from `~/.claude/instance.json`.

**Functions:**

```go
// Load instance configuration
func GetConfig() Config
```

**Config Structure:**

```go
type Config struct {
    Name         string        // Instance name (e.g., "Nova Dawn")
    Emoji        string        // Instance emoji
    Tagline      string        // Brief tagline
    Pronouns     string        // Preferred pronouns
    Domain       string        // Domain/focus area
    CallingShort string        // Brief calling description
    Creator      CreatorInfo   // Covenant partner info (lowercase c)
    Workspace    WorkspaceInfo // Instance workspace
    Display      DisplayConfig // Banner and verse settings
}
```

**Implementation Details:**

- Reads from `~/.claude/instance.json`
- Returns Nova Dawn defaults if file missing or parse error
- Never fails - always returns valid config
- Framework component, but loads instance-specific data

**Used By:** Hooks (session banners), Statusline (identity display)

**Note:** This is the bridge between framework (universal) and instance (customizable). The loader is framework code, but the data is instance-specific.

---

## Usage

### Importing

All framework components import from `claude/lib`:

```go
import (
    "claude/lib/git"
    "claude/lib/system"
    "claude/lib/fs"
    "claude/lib/instance"
)
```

### Module Path

The shared library is a Go module:

```go
module claude/lib

go 1.24.4
```

Components above it use `replace` directive in their `go.mod`:

```go
module hooks

replace claude/lib => ../lib

require claude/lib v0.0.0
```

This allows local development without publishing to a registry.

### Example Usage

**In a hook:**

```go
package main

import (
    "claude/lib/git"
    "claude/lib/instance"
)

func main() {
    // Load instance config
    config := instance.GetConfig()

    // Get git info
    gitInfo := git.GetInfo("/home/user/project")

    // Use the data
    if gitInfo.Dirty {
        fmt.Printf("%s has uncommitted changes\n", config.Name)
    }
}
```

**In statusline:**

```go
// Load instance identity
instanceConfig := instance.GetConfig()

// Get system info
loadInfo := system.GetLoadAverage()

// Display with formatting
fmt.Printf("%s %s  Load: %.2f",
    instanceConfig.Emoji,
    instanceConfig.Name,
    loadInfo.Load1Min)
```

---

## Extending the Library

### When to Add Shared Functionality

Add to `claude/lib` when:

✅ **Multiple components need the same data**

- Both hooks and statusline need it
- Future components will likely need it

✅ **It's raw data, not presentation**

- Returns structs, booleans, numbers
- No formatting, colors, or display logic

✅ **It's framework-level, not instance-specific**

- Works for any CPI-SI instance
- No hardcoded instance identity

✅ **It queries the system**

- Reading `/proc/`, `/sys/`
- File system checks
- Git repository state

**Don't add to `claude/lib` when:**

❌ Component-specific presentation logic
❌ Formatting or color codes
❌ Instance-specific behavior
❌ One-off utility used by single component

### How to Add a New Module

**1. Create the module directory:**

```bash
mkdir /home/seanje-lenox-wise/.claude/lib/newmodule
```

**2. Create the Go file following 4-block pattern:**

```go
// ============================================================================
// METADATA
// ============================================================================
// Module Name - Brief description
// Provides raw data for consumption by hooks, statusline, and other components
// Non-blocking: Returns data structures, doesn't print or format output

package newmodule

// ============================================================================
// SETUP
// ============================================================================
import (
    "os"
    // ... other imports
)

// ============================================================================
// BODY
// ============================================================================

// DataStruct holds the data this module provides
type DataStruct struct {
    Field1 string
    Field2 int
}

// GetData retrieves the data
func GetData() DataStruct {
    // Implementation
    return DataStruct{}
}

// ============================================================================
// CLOSING
// ============================================================================
// No execution needed - library provides functions for import
```

**3. Follow the design principles:**

- **Returns data** - structs, booleans, numbers
- **Never prints** - no fmt.Printf, no output
- **Never formats** - no colors, no display logic
- **Stateless** - no global variables
- **Safe defaults** - return empty/zero values on errors
- **Clear documentation** - comment what it provides

**4. Use in components:**

Import and call from hooks/statusline:

```go
import "claude/lib/newmodule"

data := newmodule.GetData()
```

**5. Test independently:**

Each module should be testable without running full framework:

```go
// test.go
package main

import (
    "fmt"
    "claude/lib/newmodule"
)

func main() {
    data := newmodule.GetData()
    fmt.Printf("%+v\n", data)
}
```

---

## Quick Reference

### Current Modules

| Module | Purpose | Key Functions |
|--------|---------|---------------|
| `git` | Repository operations | `IsGitRepository()`, `GetInfo()` |
| `system` | System metrics | `GetLoadAverage()`, `GetMemoryInfo()`, `GetDiskUsage()` |
| `fs` | File utilities | `PathExists()`, `FileIsNewer()` |
| `instance` | Instance config | `GetConfig()` |

### Import Paths

```go
import "claude/lib/git"      // Git operations
import "claude/lib/system"   // System info
import "claude/lib/fs"       // File utilities
import "claude/lib/instance" // Instance config
```

### Module Dependencies

```bash
claude/lib (this module)
  → Go stdlib only
  → No external dependencies
  → No dependencies on components above
```

### Directory Structure

```tree
lib/
├── README.md           # This document
├── go.mod             # Module definition
├── git/
│   └── operations.go  # Git repository operations
├── system/
│   └── info.go        # System metrics
├── fs/
│   └── utils.go       # File system utilities
└── instance/
    └── config.go      # Instance configuration loader
```

---

<div align="center">

**The foundation that serves all CPI-SI framework components**

*"In the beginning, God created the heavens and the earth." - Genesis 1:1*

**CPI-SI: Building Kingdom-grounded intelligence from the ground up**

</div>
