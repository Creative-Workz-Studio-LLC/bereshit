<div align="center">

# 💻 System Commands

**Command-line tools for Interactive Terminal System**

![Go](https://img.shields.io/badge/Go-00ADD8?style=flat&logo=go&logoColor=white)
![Linux](https://img.shields.io/badge/Linux-FCC624?style=flat&logo=linux&logoColor=black)

*Source code for validation, testing, and diagnostics*

[Commands](#commands) • [Building](#building) • [Development](#development)

</div>

---

## Table of Contents

- [💻 System Commands](#-system-commands)
  - [Table of Contents](#table-of-contents)
  - [Overview](#overview)
  - [Commands](#commands)
    - [status](#status)
    - [validate](#validate)
    - [test](#test)
    - [diagnose](#diagnose)
  - [Structure](#structure)
  - [Architecture Principles](#architecture-principles)
    - [The Ladder and Baton Model](#the-ladder-and-baton-model)
  - [Health Scoring System](#health-scoring-system)
    - [The HEALTH SCORING MAP](#the-health-scoring-map)
    - [Every Action Has Value](#every-action-has-value)
    - [Visual Health Indicators](#visual-health-indicators)
    - [Implementation Pattern](#implementation-pattern)
    - [Health Scoring as Documentation](#health-scoring-as-documentation)
  - [Building](#building)
    - [Build All Commands](#build-all-commands)
    - [Build Single Command](#build-single-command)
  - [Development](#development)
    - [Testing Without Building](#testing-without-building)
    - [Adding New Commands](#adding-new-commands)
  - [Dependencies](#dependencies)
    - [Logging as Rails vs Libraries as Rungs](#logging-as-rails-vs-libraries-as-rungs)

---

## Overview

**What are commands?** Tools you run to check, test, and diagnose the system. Think of them as the interface - you type a command, it shows you what's happening.

**Quick reference:**

| Command | What it does | When to use |
|---------|--------------|-------------|
| `status` | Quick health check | "Is everything working?" |
| `validate` | Detailed configuration check | "What exactly is wrong?" |
| `test` | Try actual operations | "Does it really work?" |
| `diagnose` | Full troubleshooting info | "I need to debug something" |

> [!TIP]
> Run any command from the system directory: `./bin/status` or `./bin/validate`

---

> [!NOTE]
> These are compiled Go programs - the source code is here, the ready-to-run programs are in `../bin/`

---

## Commands

### status

**Quick health check showing system status**

```bash
./bin/status
```

**Purpose:**

- Shows immediate operational status
- Displays available capabilities
- Provides next steps for installation

**What you get:**

- ✓ or ✗ for each configuration
- Exact commands to fix issues
- Next steps if something is wrong

**Use when:**

- Quick system health check
- Verifying installation status
- Getting installation instructions

<details>
<summary><b>Example output</b></summary>

**System not configured:**

```bash
──────────────────────────────────────
 CPI-SI Interactive Terminal System
──────────────────────────────────────

─────────────────
 System Status
─────────────────
Sudoers Configuration     ✗ Issues detected
Environment Variables     ✗ Issues detected

Details:
  Sudoers:            Run: cd ~/.claude/system && sudo ./scripts/sudoers/install.sh
  Environment:        Run: cd ~/.claude/system && ./scripts/env/integrate.sh && source ~/.bashrc


Available Capabilities:
  ◯ Passwordless sudo (not configured)
  ◯ Non-interactive defaults (not configured)


Next Steps:

1. Install sudoers configuration:
   cd ~/.claude/system
   sudo ./scripts/sudoers/install.sh

2. Integrate environment variables:
   cd ~/.claude/system
   ./scripts/env/integrate.sh
   source ~/.bashrc

Then run './bin/validate' to verify installation
```

</details>

<details>
<summary><b>Technical details</b></summary>

**Health tracking:** Reaches 100 health on successful execution (logs every check and display action)

</details>

---

### validate

**Comprehensive installation validation**

```bash
./bin/validate
```

**Purpose:**

- Validates sudoers configuration
- Checks environment variables
- Confirms proper installation

**What you get:**

- Detailed check for each component
- Specific issues found
- Exact fix recommendations

**Use when:**

- After installing sudoers
- After integrating environment
- Troubleshooting installation issues

<details>
<summary><b>Example output</b></summary>

**System not configured:**

```bash
───────────────────────────────────────────────────
 CPI-SI Interactive Terminal System - Validation
───────────────────────────────────────────────────

Sudoers Configuration:
  ◯ Configuration file exists
  ◯ Correct permissions ()
  ◯ Syntax validation passed

⚠ Run: cd ~/.claude/system && sudo ./scripts/sudoers/install.sh

Environment Configuration:
  ◯ Shell integration (.bashrc)
  ◯ DEBIAN_FRONTEND (not set)
  ◯ NEEDRESTART_MODE (not set)
  ◯ PIP_NO_INPUT (not set)
  ◯ NPM_CONFIG_YES (not set)

⚠ Run: cd ~/.claude/system && ./scripts/env/integrate.sh && source ~/.bashrc

──────────────────────
 Validation Summary
──────────────────────
✗ Interactive Terminal System has issues

⚠ Sudoers configuration needs attention
⚠ Environment configuration needs attention

ℹ Run './bin/diagnose' for detailed troubleshooting
```

</details>

**Exit codes:**

- `0` - All validation passed
- `1` - Validation issues detected

<details>
<summary><b>Technical details</b></summary>

**Health tracking:**

- Setup (15): Initialize, start, snapshot, display header
- Sudoers validation (35): Library access, file check, permissions, syntax, status
- Environment validation (35): Library access, shell integration, variables check
- Summary (15): Status determination, display message, recommendations
- Final health at -20 (issues detected) vs 100 (fully operational)

</details>

---

### test

**Operation testing for safe and protected operations**

```bash
./bin/test --quick          # Quick operation tests
./bin/test --full           # Comprehensive test suite
```

**Purpose:**

- Tests safe operations (should work without password)
- Tests protected operations (should require password)
- Verifies safety boundaries

**What you get:**

- ✓ or ✗ for each operation type
- Quick mode: Essential tests only
- Full mode: Comprehensive test suite

**Use when:**

- After sudoers installation
- Verifying permissions work correctly
- Testing system configuration

<details>
<summary><b>Example output</b></summary>

**Sudoers not configured:**

```bash
────────────────────────────────────────────────────────
 CPI-SI Interactive Terminal System - Operation Tests
────────────────────────────────────────────────────────
ℹ Running quick operation tests...

Testing: sudo apt update (safe operation)...
✗ Update package lists
  Error:            exit status 1

Testing: sudo systemctl status (safe operation)...
✗ Check service status
  Error:            exit status 1


────────────────
 Test Results
────────────────
✗ Update package lists
  Error:            exit status 1

✗ Check service status
  Error:            exit status 1


Summary:
  Total Tests:        2
  Passed:             0
  Failed:             2

⚠ 2 tests failed - system may have issues

ℹ Run './bin/diagnose' for detailed troubleshooting
```

</details>

**Exit codes:**

- `0` - Command executed successfully (not tests passed)
- Note: Command exits 0 even when tests fail - test failures are reported, not command errors

<details>
<summary><b>Technical details</b></summary>

**Health tracking:**

- Setup (20): Logger, header, args parsing, operation start, snapshot
- Test Execution (50): Initiate suite, execute tests, collect results
- Results Processing (30): Display results, count, summary, log final, exit code
- Final health at 90 (command succeeded, tests failed) vs 100 (all tests passed)

</details>

---

### diagnose

**Detailed diagnostics and troubleshooting**

```bash
./bin/diagnose
```

**Purpose:**

- System information gathering
- Sudoers configuration diagnosis
- Environment variable diagnosis
- File system checks
- Binary verification
- Troubleshooting recommendations

**What you get:**

- Complete system information
- Detailed configuration status
- File-level checks
- Specific recommendations for fixes

**Use when:**

- Installation not working
- Need detailed system information
- Troubleshooting configuration issues

<details>
<summary><b>Example output (partial)</b></summary>

**System not configured:**

```bash
────────────────────────────────────────────────────
 CPI-SI Interactive Terminal System - Diagnostics
────────────────────────────────────────────────────

System Information:
  User:               seanje-lenox-wise
  Home:               /home/seanje-lenox-wise
  Shell:              /bin/bash
  Working Dir:        /home/seanje-lenox-wise/.claude/system


Sudoers Diagnosis:
  Config Path:        /etc/sudoers.d/90-cpi-si-safe-operations
  File Exists:        false
  Source File:        Found
  Source Valid:       true
  Groups:             seanje-lenox-wise adm cdrom sudo dip plugdev users lpadmin docker


ℹ Run: cd ~/.claude/system && sudo ./scripts/sudoers/install.sh


Environment Diagnosis:
  Config Path:        /home/seanje-lenox-wise/.claude/system/env/non-interactive.conf
  Config File:        Found
  Shell RC:           /home/seanje-lenox-wise/.bashrc
  Integrated:         false

Environment Variables:
  ✗ DEBIAN_FRONTEND
    Expected:       noninteractive
    Actual:         (not set)
  ✗ NEEDRESTART_MODE
    Expected:       a
    Actual:         (not set)
  ✗ PIP_NO_INPUT
    Expected:       1
    Actual:         (not set)
  ✗ NPM_CONFIG_YES
    Expected:       true
    Actual:         (not set)

  Integration Script: /home/seanje-lenox-wise/.claude/system/scripts/env/integrate.sh

ℹ Run: cd ~/.claude/system && ./scripts/env/integrate.sh && source ~/.bashrc


File System Check:
  ◉ /home/seanje-lenox-wise/.claude/system
  ◉ /home/seanje-lenox-wise/.claude/system/sudoers
  ◉ /home/seanje-lenox-wise/.claude/system/env
  ◉ /home/seanje-lenox-wise/.claude/system/wrappers
  ◉ /home/seanje-lenox-wise/.claude/system/docs
  ◉ /home/seanje-lenox-wise/.claude/system/bin


Binary Check:
  ◉ validate (0775)
  ◉ test (0775)
  ◉ status (0775)
  ◉ diagnose (0775)


───────────────────────────────────
 Troubleshooting Recommendations
───────────────────────────────────
Common issues and solutions:

1. Sudoers not working:
   • Logout and login again
   • Check file permissions: sudo visudo -c
   • Verify ownership: ls -l /etc/sudoers.d/90-cpi-si-safe-operations

2. Environment variables not set:
   • Source .bashrc: source ~/.bashrc
   • Check integration: grep 'CPI-SI' ~/.bashrc
   • Restart shell or Claude Code session

3. Binaries not found:
   • Run build script: cd ~/.claude/system && ./scripts/build.sh
   • Check build errors in output
```

</details>

<details>
<summary><b>Technical details</b></summary>

**Health tracking:**

- Setup (15): Logger, operation start, snapshot, display header
- Diagnostic Actions (70): System info, sudoers diagnosis, environment diagnosis, paths check, binaries check
- Results & Guidance (15): Troubleshooting display, log completion
- Final health at 100 (command executed all diagnostics successfully)

</details>

---

## Structure

Each command follows this pattern:

```bash
cmd/
└── <command-name>/
    └── <command-name>.go    # Main program file
```

**All commands:**

- Use `package main`
- Follow 4-block structure
- Implement comprehensive health scoring (0 to 100)
- Connect to logging rails (not passed through hierarchy)
- Import from `system/lib/` for shared functionality
- Compile to `../bin/<command-name>`

---

## Architecture Principles

### The Ladder and Baton Model

Commands exist within a hierarchical system (the ladder), but logging is special.

**The Ladder (Dependency Hierarchy):**

```bash
Commands (validate, test, status, diagnose)
    ↓ orchestrate
Libraries (sudoers, environment, operations, display)
    ↓ use
Core utilities
```

**The Rails (Immune System Infrastructure):**

Logging, debugging, and restoration don't sit on rungs - they're the **rails** that hold the ladder together. Every component at every level connects directly to this immune system infrastructure.

**The Trilogy - Detection → Assessment → Restoration:**

- **Logging (Detection)**: Captures everything with complete context
- **Debugging (Assessment)**: Analyzes problems, classifies, routes responses
- **Restoration (Response)**: Automated fixes (antibodies) + human intervention

> [!NOTE]
> **Complete paradigm:** See [`docs/logging-debugging-restoration.md`](../docs/logging-debugging-restoration.md) for comprehensive explanation of the immune system design.

```bash
║                              ║  ← Immune system rails (Detection → Assessment → Restoration)
║  [Commands]                  ║
║      ↓ orchestrate           ║
║  [Libraries]                 ║
║      ↓ use                   ║
║  [Utilities]                 ║
║                              ║
```

**Why this matters:**

- The immune system (Detection → Assessment → Restoration) is **orthogonal to the dependency hierarchy**
- Every component creates its logger at initialization (attaches to Detection rails)
- Health tracking is unified across all components in an execution
- Components don't pass loggers down - they all attach to the same rail system
- Future: Debugging (Assessment) and Restoration will also work orthogonally

**Example:**

```go
// cmd/validate creates logger
logger := logging.NewLogger("validate")
logger.Check("sudoers-checked", true, 20, details)

// lib/sudoers also creates logger
logger := logging.NewLogger("sudoers")
logger.Check("file-found", true, 5, details)
```

Both attach to the logging rails. Both contribute to the system's health tracking. Neither depends on the other for logging access.

---

## Health Scoring System

Every command implements **comprehensive health scoring** from 0 to 100, tracking every action that contributes to the command's goal.

### The HEALTH SCORING MAP

Every command includes a health scoring map in its METADATA block:

```go
// HEALTH SCORING MAP (Total = 100):
// ----------------------------------
// Setup (15 points):
//   - Initialize logger: +5
//   - Display header: +2
//   - Parse arguments: +5
//   - Log operation start: +5
//   - Snapshot state: +3
//
// Main Actions (70 points):
//   - [Action 1]: +X
//   - [Action 2]: +Y
//   - [Action 3]: +Z
//
// Results (15 points):
//   - Display results: +10
//   - Log completion: +5
```

This map serves as:

1. **Documentation** - What the command does and why
2. **Contract** - How success is measured
3. **Debug aid** - Where health comes from

### Every Action Has Value

Health scoring isn't arbitrary. It's based on a simple principle:

**Perfect execution of ALL actions = +100**
**Complete failure of ALL actions = -100**
**Real execution = sum of successes (+) and failures (-)**

**Example from build.sh:**

```bash
Goal: Build all 4 binaries successfully

Actions:
  Setup (10 pts):      Source logging +3, Create bin +2, Init context +5
  Per binary (20 pts): Find source +4, Compiler +2, Start +3, Complete +8,
                       Written +2, Executable +1
  Verification (10):   Count +3, Log +2, Display +5

Perfect execution: 10 + (20×4) + 10 = 100 health
```

### Visual Health Indicators

Every log entry includes visual health status:

```bash
HEALTH: 85 (Δ+20) 💙 [██████████████████░░]
        ↑    ↑    ↑   ↑
        │    │    │   └─ Health bar (normalized -100 to +100)
        │    │    └───── Emoji indicator (11 states)
        │    └────────── Delta from previous state
        └─────────────── Current session health
```

**Emoji Health States:**

| Range | Emoji | State | Meaning |
|-------|-------|-------|---------|
| 90-100 | 💚 | Excellent | Flawless operation |
| 70-89 | 💙 | Good | Minor issues handled |
| 50-69 | 💛 | Adequate | Warning zone |
| 30-49 | 🧡 | Degraded | Reduced functionality |
| 10-29 | ❤️ | Troubled | Multiple problems |
| -10 to +10 | 🤍 | Neutral | Baseline state |
| -30 to -11 | 💔 | Declining | Clear problems |
| -50 to -31 | 🩹 | Damaged | Losing capability |
| -70 to -51 | ⚠️ | Critical | Major failures |
| -90 to -71 | ☠️ | Severe | Near breakdown |
| ≤-91 | 💀 | Dead | Total failure |

### Implementation Pattern

All commands follow this health tracking pattern:

```go
func main() {
    // Setup Action 1/4: Initialize logger (+5)
    logger := logging.NewLogger("command")
    logger.Check("logger-initialized", true, 5, map[string]interface{}{
        "component": "command",
    })

    // Setup Action 2/4: Display header (+2)
    fmt.Print(display.Header("Command Title"))
    logger.Check("header-displayed", true, 2, map[string]interface{}{
        "header": "command",
    })

    // Main Action: Do work (+X or -X)
    success := doWork()
    healthImpact := 20
    if !success {
        healthImpact = -20
    }
    logger.Check("work-completed", success, healthImpact, details)

    // Results Action: Log completion (+5)
    logger.Success("Command completed", 5, details)
}
```

**Key points:**

1. Every action logs with its health impact
2. Failures use negative values (mirror of success)
3. Health impacts sum to 100 for perfect execution
4. Comments show action number and point value

### Health Scoring as Documentation

The health scoring map documents **what the command actually does**:

- Setup actions show initialization requirements
- Main actions show the command's purpose
- Point values show relative importance
- Total of 100 forces intentional value assignment

Reading the health map tells you:

- What the command's goal is
- What steps it takes to achieve that goal
- Which actions are most critical
- How success is measured

This makes health scoring both a runtime diagnostic tool and documentation for developers.

---

## Building

### Build All Commands

```bash
cd ~/.claude/system
./scripts/build.sh
```

The build script:

- Compiles all 4 commands
- Creates `bin/` directory if needed
- Reports success/failure for each
- Shows available binaries

### Build Single Command

```bash
cd ~/.claude/system
go build -o bin/status ./cmd/status
```

Useful during development when modifying a single command.

---

## Development

### Testing Without Building

```bash
cd ~/.claude/system/cmd/status
go run status.go
```

**Advantages:**

- Faster iteration during development
- No binary to clean up
- Immediate execution

**Limitations:**

- Slower than compiled binary
- Not suitable for production use

### Adding New Commands

1. **Create directory:** `cmd/<new-command>/`
2. **Create source file:** `<new-command>.go` with 4-block structure
3. **Define the health scoring map:**
   - What's the command's goal?
   - What actions contribute to that goal?
   - Distribute 100 points across all actions
4. **Use `package main`**
5. **Import required libraries:**

   ```go
   import (
       "system/lib/logging"
       "system/lib/display"
       // etc.
   )
   ```

6. **Implement health tracking throughout execution**
7. **Add to build script:** Update `scripts/build.sh` COMMANDS array
8. **Build and test:**

   ```bash
   ./scripts/build.sh
   ./bin/<new-command>
   ```

9. **Verify health progression reaches 100:** Check `logs/<command>.log`
10. **Update this README**

**Example structure with health scoring:**

```go
// ============================================================================
// METADATA
// ============================================================================
// New Command - CPI-SI Interactive Terminal System
// Purpose: What this command does
// Non-blocking: How it behaves
// Usage: ./bin/new-command [options]
//
// HEALTH SCORING MAP (Total = 100):
// ----------------------------------
// Setup (20 points):
//   - Initialize logger: +5
//   - Parse arguments: +5
//   - Log operation start: +5
//   - Snapshot state: +3
//   - Display header: +2
//
// Main Work (70 points):
//   - Action 1: +25
//   - Action 2: +25
//   - Action 3: +20
//
// Results (10 points):
//   - Display results: +5
//   - Log completion: +5

package main

// ============================================================================
// SETUP
// ============================================================================

import (
    "fmt"
    "system/lib/display"
    "system/lib/logging"
)

// ============================================================================
// BODY
// ============================================================================

func doWork() bool {
    // Implementation
    return true
}

// ============================================================================
// CLOSING
// ============================================================================

func main() {
    // Setup Action 1/5: Initialize logger (+5)
    logger := logging.NewLogger("new-command")
    logger.Check("logger-initialized", true, 5, map[string]interface{}{
        "component": "new-command",
    })

    // Setup Action 2/5: Parse arguments (+5)
    // ... parse args ...
    logger.Check("args-parsed", true, 5, map[string]interface{}{
        "args": "parsed",
    })

    // Setup Action 3/5: Log operation start (+5)
    logger.Operation("new-command", 5, "doing something")

    // Setup Action 4/5: Snapshot state (+3)
    logger.SnapshotState("before-work", 3)

    // Setup Action 5/5: Display header (+2)
    fmt.Print(display.Header("New Command"))
    logger.Check("header-displayed", true, 2, map[string]interface{}{
        "header": "new command",
    })

    // Main Work Action 1/3: Do work (+25 or -25)
    success := doWork()
    healthImpact := 25
    if !success {
        healthImpact = -25
    }
    logger.Check("work-done", success, healthImpact, map[string]interface{}{
        "result": success,
    })

    // Results Action 1/2: Display results (+5)
    fmt.Println(display.Success("Work completed"))
    logger.Check("results-displayed", true, 5, map[string]interface{}{
        "displayed": "results",
    })

    // Results Action 2/2: Log completion (+5)
    logger.Success("Command completed", 5, map[string]interface{}{
        "final_health": "should be 100 on perfect execution",
    })
}
```

**Health scoring checklist for new commands:**

- [ ] Health scoring map in METADATA totals exactly 100 points
- [ ] Every action logs with `logger.Check()` or appropriate method
- [ ] Failures use negative mirror values of successes
- [ ] Comments show action number and health value (e.g., "Setup Action 1/4: (+5)")
- [ ] Perfect execution reaches health 100
- [ ] Logs show clear health progression with visual indicators
- [ ] Health map documents what the command actually does

---

## Dependencies

Commands depend on these libraries:

| Library | Purpose | Example Usage | Relationship |
|---------|---------|---------------|--------------|
| `system/lib/logging` | Health tracking & logs | `logging.NewLogger()` | **Rail** - orthogonal to hierarchy |
| `system/lib/sudoers` | Sudoers validation | `sudoers.Check()` | **Rung** - orchestrated by commands |
| `system/lib/environment` | Environment validation | `environment.Check()` | **Rung** - orchestrated by commands |
| `system/lib/operations` | Operation testing | `operations.TestSafeOperation()` | **Rung** - orchestrated by commands |
| `system/lib/display` | Formatted output | `display.Success()` | **Rung** - orchestrated by commands |

**All libraries follow the same 4-block structure and provide focused, reusable functionality.**

### Logging as Rails vs Libraries as Rungs

**Logging (`system/lib/logging`)** is special:

- Not passed through the hierarchy
- Every component connects directly
- Provides the health tracking infrastructure
- Acts as the **rails** holding the system together

**Other libraries** follow normal dependency patterns:

- Commands orchestrate libraries
- Libraries provide focused functionality
- Clear hierarchical relationships
- Act as **rungs** in the ladder

This distinction is architectural - logging provides cross-cutting infrastructure that all components attach to, while other libraries provide domain-specific functionality that commands orchestrate.

---

<div align="center">

**Built with intentional design for Kingdom Technology**

[Back to System Documentation](../README.md)

</div>
