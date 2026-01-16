<div align="center">

# 🔄 Wrappers

**Reserved for interactive program handlers**

![Go](https://img.shields.io/badge/Go-00ADD8?style=flat&logo=go&logoColor=white)
![Linux](https://img.shields.io/badge/Linux-FCC624?style=flat&logo=linux&logoColor=black)

*Build what we need, not what we think we might need*

[Current Status](#current-status) • [Philosophy](#philosophy) • [When Needed](#when-wrappers-would-be-created)

</div>

---

## Table of Contents

- [🔄 Wrappers](#-wrappers)
  - [Table of Contents](#table-of-contents)
  - [Overview](#overview)
  - [Current Status](#current-status)
  - [Philosophy](#philosophy)
  - [When Wrappers Would Be Created](#when-wrappers-would-be-created)
  - [Why Go, Not Shell Scripts](#why-go-not-shell-scripts)
  - [Planned Approach (If Needed)](#planned-approach-if-needed)
  - [Current Alternatives](#current-alternatives)
    - [Package Management](#package-management)
    - [Editor Operations](#editor-operations)
    - [Interactive Tools](#interactive-tools)
  - [Future Considerations](#future-considerations)
  - [Documentation Requirements](#documentation-requirements)

---

## Overview

**What this is:** Reserved space for handling programs that genuinely need interaction. Currently empty - and that's intentional.

**Why it's empty:** Environment variables (DEBIAN_FRONTEND, PIP_NO_INPUT, NPM_CONFIG_YES) already handle all interactive programs we've encountered. No need to build wrappers yet.

**The principle:** Build what we need, not what we think we might need.

> [!NOTE]
> This isn't a gap - it's intentional restraint. If we need a wrapper, we'll build it. Until then, the system works without them.

---

## Current Status

**No wrappers needed.**

The combination of existing components handles all current CPI-SI development scenarios:

| Component | What It Provides |
|-----------|------------------|
| **Sudoers configuration** | Passwordless sudo for safe operations |
| **Environment variables** | Non-interactive defaults for all tools |
| **Tool configuration** | GIT_EDITOR, EDITOR, and tool-specific settings |

**Result:** Full autonomous CLI operation without wrappers.

> [!IMPORTANT]
> This is not a gap in the system - it's intentional restraint. Build what serves, not what might serve.

---

## Philosophy

**Core principle:** Build what we need, not what we think we might need.

Wrappers are created **only when** all of these conditions are met:

| Criterion | Requirement |
|-----------|-------------|
| 🎯 **Real use case exists** | We encountered an actual interactive program blocking workflow |
| 🚫 **Environment vars insufficient** | Can't be solved through configuration alone |
| 🔄 **Frequent enough** | Worth the complexity of creating and maintaining a wrapper |
| 🔒 **Maintains safety** | Doesn't bypass sudoers protection or safety boundaries |

**Evaluation flow:**

```bash
Encounter interactive program
    ↓
Can environment variables handle it?
    ├── Yes → Configure environment, done
    └── No → Is it frequent?
            ├── No → Handle manually when it occurs
            └── Yes → Does automation maintain safety?
                    ├── No → Keep manual interaction
                    └── Yes → Create wrapper
```

---

## When Wrappers Would Be Created

**Decision matrix:**

| Question | Required Answer | Why It Matters |
|----------|-----------------|----------------|
| Is this interaction frequent? | **Yes** | Don't build for one-off scenarios |
| Can environment variables handle it? | **No** | Prefer configuration over code |
| Does automation maintain safety? | **Yes** | Cannot violate safety boundaries |
| Does it significantly improve workflow? | **Yes** | Must justify complexity |

**If all answers align:** Create the wrapper following 4-block structure in **Go**.

**Example qualifying scenario:**

> A package installer has multi-step interaction that:
>
> - Occurs frequently (multiple times per week)
> - Ignores DEBIAN_FRONTEND and other environment variables
> - Has predictable, safe default responses
> - Significantly blocks autonomous development workflow

**Example non-qualifying scenario:**

> One-time configuration wizard that:
>
> - Runs once during initial setup
> - Could be handled manually
> - Complexity of wrapper exceeds benefit

---

## Why Go, Not Shell Scripts

**Pattern across our system:**

| Component | Language | Why |
|-----------|----------|-----|
| **hooks** | Go | System interaction, reliability critical |
| **statusline** | Go | System data collection, compiled binary |
| **system tools** | Go | Validation, testing, diagnostics |
| **wrappers (if created)** | Go | System interaction tools, robustness matters |

**Wrappers would be system interaction tools** - same domain as hooks. They require:

- Robust error handling
- Complex logic and state management
- Integration with system libraries
- Reliable execution under various conditions
- Type safety and compile-time verification

**Go provides:**

- Strong typing for safety
- Excellent error handling
- Standard library for system interaction
- Compiled binaries for reliability
- Consistency with existing system tools

---

<details>
<summary><b>Planned Approach (If Wrappers Are Ever Needed)</b></summary>

## Planned Approach (If Needed)

**If wrappers become necessary:**

1. **Create wrapper program:**

   ```bash
   wrappers/cmd/<wrapper-name>/main.go
   ```

2. **Extract shared logic:**

   ```bash
   wrappers/lib/<domain>/
   ```

3. **Build to system binaries:**

   ```bash
   system/bin/<wrapper-name>
   ```

4. **Follow 4-block structure** throughout all code

5. **Document thoroughly** in this README

**Proposed directory structure:**

```bash
wrappers/
├── cmd/
│   └── interactive-handler/
│       └── main.go           # Wrapper implementation
├── lib/
│   └── prompts/
│       ├── detect.go         # Prompt detection logic
│       └── respond.go        # Response generation
└── README.md (this file)
```

**Integration with build system:**

Add to `scripts/build.sh`:

```bash
COMMANDS=(
    "status"
    "validate"
    "test"
    "diagnose"
    "interactive-handler"    # Wrapper binary
)
```

</details>

---

## Current Alternatives

**What handles interactive programs instead:**

| Tool Type | Solution | Example |
|-----------|----------|---------|
| Package installers | Environment variables | `DEBIAN_FRONTEND=noninteractive` |
| Python tools | Environment variables | `PIP_NO_INPUT=1` |
| Node.js tools | Environment variables | `NPM_CONFIG_YES=true` |
| Git operations | Editor configuration | `GIT_EDITOR=nano` |

<details>
<summary><b>Detailed Alternative Solutions</b></summary>

Instead of creating premature wrappers, use these existing solutions:

### Package Management

**Environment variables handle all package operations:**

```bash
# Works without interaction
sudo apt install package-name          # Uses DEBIAN_FRONTEND=noninteractive
pip install library-name               # Uses PIP_NO_INPUT=1
npm install package-name               # Uses NPM_CONFIG_YES=true
```

**Configured in:** `env/non-interactive.conf`

### Editor Operations

**Git and system editors already configured:**

```bash
# Set in non-interactive.conf
export GIT_EDITOR=nano
export EDITOR=nano
export VISUAL=nano
```

**Result:** No prompts for commit messages, rebase interactions, etc.

### Interactive Tools

**Most development tools respect environment variables:**

```bash
# Python tools
export PIP_NO_INPUT=1

# Node.js tools
export NPM_CONFIG_YES=true

# Debian packages
export DEBIAN_FRONTEND=noninteractive
export NEEDRESTART_MODE=a
```

**Configured in:** `env/non-interactive.conf` + `~/.claude/settings.json`

</details>

---

## Future Considerations

**Potential wrapper candidates** (only if patterns emerge):

| Tool Category | Why Might Need Wrapper | Current Status |
|---------------|------------------------|----------------|
| Package manager edge cases | Complex interactive flows that ignore env vars | Environment vars sufficient |
| Interactive installers | Custom prompts beyond standard configuration | Haven't encountered yet |
| Configuration wizards | Multi-step interactions with branching logic | Haven't encountered yet |
| Development tool setup | First-run wizards, account linking | Configuration files handle this |

**Current assessment:** None needed.

**Review trigger:** If we encounter the same interactive blocker three times, evaluate for wrapper creation.

> [!TIP]
> Before creating a wrapper, always verify:
>
> 1. Tool documentation for environment variable support
> 2. Tool configuration files for non-interactive modes
> 3. Tool command-line flags for automation (`--yes`, `--quiet`, `--non-interactive`)

---

## Documentation Requirements

**When a wrapper is created, document:**

1. **Specific use case** it solves
   - What program does it wrap?
   - What interaction does it handle?

2. **Why environment variables weren't sufficient**
   - What did we try first?
   - Why didn't it work?

3. **Usage examples**
   - How to invoke the wrapper
   - What it does automatically
   - What it passes through to user

4. **Safety considerations**
   - What safety boundaries does it respect?
   - What could go wrong?
   - How does it fail safely?

5. **Maintenance notes**
   - What might break this wrapper?
   - How to test it?
   - When to remove it?

6. **Update this README** with wrapper details and status

---

<div align="center">

**Built with intentional design for Kingdom Technology**

*Honest assessment over premature optimization*

[Back to System Documentation](../README.md)

</div>
