<div align="center">

# 🔧 CPI-SI Interactive Terminal System

**Seamless terminal operations for CPI-SI instances through Claude Code CLI**

![Bash](https://img.shields.io/badge/Bash-4EAA25?style=flat&logo=gnubash&logoColor=white)
![Linux](https://img.shields.io/badge/Linux-FCC624?style=flat&logo=linux&logoColor=black)
![License](https://img.shields.io/badge/License-Proprietary-red)

*Covenant Partnership Intelligence ⊗ Structured Intelligence*

[Overview](#overview) • [Architecture](#architecture) • [Features](#features) • [Installation](#installation) • [Reference](#reference)

</div>

---

## Table of Contents

- [🔧 CPI-SI Interactive Terminal System](#-cpi-si-interactive-terminal-system)
  - [Table of Contents](#table-of-contents)
  - [Overview](#overview)
    - [Design Principles](#design-principles)
  - [Philosophy](#philosophy)
    - [Covenant Partnership Model](#covenant-partnership-model)
    - [Safety Boundaries](#safety-boundaries)
  - [Architecture](#architecture)
    - [Component Overview](#component-overview)
    - [Directory Layout](#directory-layout)
  - [Features](#features)
    - [Safe Operations (Passwordless)](#safe-operations-passwordless)
    - [Protected Operations (Password Required)](#protected-operations-password-required)
  - [System Tools](#system-tools)
  - [Installation](#installation)
    - [Quick Start](#quick-start)
    - [Current Installation Status](#current-installation-status)
    - [Manual Integration](#manual-integration)
  - [Usage](#usage)
    - [Testing the Setup](#testing-the-setup)
    - [Common Operations](#common-operations)
  - [Troubleshooting](#troubleshooting)
  - [Reference](#reference)
    - [Sudoers Configuration](#sudoers-configuration)
    - [Environment Variables](#environment-variables)
    - [Integration Points](#integration-points)
  - [Further Documentation](#further-documentation)
  - [Status](#status)

---

## Overview

The Interactive Terminal System enables Claude Code CLI to function as a native terminal environment with seamless handling of interactive programs, sudo operations, and system management while maintaining safety boundaries.

**What this means:**

- Run `sudo` commands without typing your password (for safe operations)
- Install packages, manage services, change file permissions - all automated
- Protected from accidentally breaking your system
- Everything documented with clear examples

> [!NOTE]
> This system transforms Claude Code from a constrained CLI tool into a full development environment while maintaining responsible safeguards aligned with the covenant partnership model: trust with responsibility, autonomy with safety.

**Quick orientation:**

| If you want to... | Go to... |
|-------------------|----------|
| Check if it's working | [Usage → Testing the Setup](#testing-the-setup) |
| Install from scratch | [Installation → Quick Start](#quick-start) |
| Fix something not working | [Troubleshooting](#troubleshooting) |
| Understand how it works | [Architecture](#architecture) |
| See what commands are available | [System Tools](#system-tools) |

### Design Principles

| Principle | Implementation |
|-----------|----------------|
| **Autonomous Safe Operations** | Passwordless sudo for development work - no friction |
| **Responsibility with Trust** | Explicit safeguards against destructive operations |
| **Transparency** | Clear documentation of capabilities and boundaries |
| **Linux-First** | Built for Ubuntu/Debian systems, standard Unix tools |

---

## Philosophy

### Covenant Partnership Model

This system embodies the CPI-SI covenant partnership model:

- **Trust:** Autonomous operation for safe development work
- **Responsibility:** Explicit safety boundaries prevent destructive operations
- **Transparency:** Clear documentation of capabilities and limits

**Genesis 1:1 principle:** Order from chaos. This system brings order to terminal interaction, enabling focused development work without friction.

### Safety Boundaries

**Single hard constraint:** Do not brick the laptop

<details>
<summary><b>Safe operations (passwordless sudo)</b></summary>

- ✅ Package management (install, update, upgrade)
- ✅ Development tool installation
- ✅ Service management (start, stop, restart)
- ✅ File permissions (chmod, chown on user/project files)
- ✅ Network configuration

</details>

<details>
<summary><b>Protected operations (password required)</b></summary>

- 🔒 Essential package removal (kernel, systemd, grub)
- 🔒 Critical system service modification
- 🔒 Filesystem operations (mkfs, fdisk, dd)
- 🔒 Bootloader modifications

</details>

---

## Architecture

### Component Overview

```bash
Interactive Terminal System
│
├── Sudoers Configuration
│   ├── Safe command whitelist (NOPASSWD)
│   ├── Protected command defaults (password required)
│   └── Explicit deny rules (destructive patterns)
│
├── Environment Configuration
│   ├── Non-interactive defaults (DEBIAN_FRONTEND, etc.)
│   ├── Tool-specific configurations
│   └── Shell integration
│
├── Wrapper Infrastructure (optional)
│   ├── Interactive program handlers
│   ├── Prompt automation scripts
│   └── Fallback mechanisms
│
└── Documentation & Integration
    ├── CLAUDE.md updates (system awareness)
    ├── settings.json updates (environment vars)
    └── Usage documentation
```

### Directory Layout

```tree
system/
├── README.md                          # This document
├── go.mod                             # Go module definition
│
├── bin/                               # Compiled system tools (4 binaries, ~12MB)
│   ├── status                         # Quick health check (2.9MB)
│   ├── validate                       # Validate installation (2.9MB)
│   ├── test                           # Test operations (2.9MB)
│   └── diagnose                       # Detailed diagnostics (2.9MB)
│
├── cmd/                               # Command source code
│   ├── README.md                      # Commands documentation
│   ├── status/
│   ├── validate/
│   ├── test/
│   └── diagnose/
│
├── lib/                               # System libraries
│   ├── README.md                      # Libraries documentation
│   ├── logging/                       # Health tracking and logging
│   ├── sudoers/                       # Sudoers validation
│   ├── environment/                   # Environment validation
│   ├── operations/                    # Operation testing
│   └── display/                       # Formatted output
│
├── scripts/                           # Installation and build scripts
│   ├── README.md                      # Scripts documentation
│   ├── build.sh                       # Build all binaries
│   ├── sudoers/
│   │   └── install.sh                 # Install sudoers configuration
│   └── env/
│       └── integrate.sh               # Integrate environment variables
│
├── sudoers/
│   ├── README.md                      # Sudoers component guide
│   └── 90-cpi-si-safe-operations     # Sudoers drop-in configuration
│
├── env/
│   ├── README.md                      # Environment component guide
│   └── non-interactive.conf           # Environment variable definitions
│
├── wrappers/                          # Reserved for future use
│   └── README.md                      # Wrapper philosophy
│
├── logs/                              # Operation logs with health tracking (10 active logs)
│   ├── README.md                      # Logging documentation
│   ├── commands/                      # Command execution logs (validate, test, status, diagnose)
│   ├── scripts/                       # Build and system scripts (build.log ~82KB)
│   ├── libraries/                     # Library component logs (operations, sudoers, environment)
│   └── system/                        # System-level logs (env-integrate, sudoers-install)
│
└── docs/
    ├── README.md                      # Documentation index
    ├── architecture.md                # Complete design document
    └── operations-reference.md        # Complete operations catalog
```

---

## Features

> [!TIP]
> **GUI Password Prompts:** Use `pkexec` instead of `sudo` for graphical password dialogs (desktop environments). Both methods work identically - `pkexec` shows a GUI dialog while `sudo` prompts in terminal. See [GUI Password Prompts](docs/gui-sudo-prompts.md) for details.

### Safe Operations (Passwordless)

<details>
<summary><b>Package Management</b></summary>

**Enabled Commands:**

- `sudo apt update` - Update package lists
- `sudo apt upgrade` - Upgrade installed packages
- `sudo apt install <package>` - Install any package
- `sudo dpkg -i <file>` - Install manual packages

**Safety Boundaries:**

- Essential package removal requires password
- Prevents removing: systemd, linux-image, grub, network-manager

</details>

<details>
<summary><b>Service Management</b></summary>

**Enabled Commands:**

- `sudo systemctl start/stop/restart <service>` - Manage services
- `sudo systemctl enable/disable <service>` - Configure autostart
- `sudo systemctl status <service>` - Check service status

**Safety Boundaries:**

- Critical service disabling requires password
- Prevents disabling: systemd core services

</details>

<details>
<summary><b>File Permissions</b></summary>

**Enabled Commands:**

- `sudo chmod <mode> <file>` - Change file permissions
- `sudo chown <user:group> <file>` - Change file ownership
- File operations in user/project directories

**Safety Boundaries:**

- Critical system directories protected
- `/etc/`, `/.git/config`, `/.ssh/`, `/boot/` require password

</details>

<details>
<summary><b>Network Configuration</b></summary>

**Enabled Commands:**

- Network service restarts
- Configuration file updates
- DNS configuration changes

</details>

### Protected Operations (Password Required)

<details>
<summary><b>Essential Package Removal</b></summary>

Operations that could break the system require authentication:

- `sudo apt remove systemd*`
- `sudo apt remove linux-image*`
- `sudo apt remove grub*`
- `sudo apt remove network-manager*`

</details>

<details>
<summary><b>Filesystem Operations</b></summary>

Destructive operations require authentication:

- `sudo mkfs*` - Format filesystems
- `sudo fdisk*` - Partition management
- `sudo dd*` - Low-level disk operations

</details>

<details>
<summary><b>Bootloader Modifications</b></summary>

Boot-critical operations require authentication:

- `sudo grub-install`
- `sudo update-grub`
- UEFI modifications

</details>

---

## System Tools

> [!NOTE]
> **Sudoers Validation Behavior:** The `visudo -c` validation command itself requires a password (protected operation), so validation checks may show as "failed" even when sudo operations work correctly. This is expected - the actual passwordless operations (like `sudo apt update`) function properly.

**Quick health check:**

```bash
cd ~/.claude/system
./bin/status
```

Shows immediate system status and available capabilities.

**Validate installation:**

```bash
./bin/validate
```

Comprehensive validation of sudoers and environment configuration.

**Test operations:**

```bash
./bin/test --quick          # Quick operation tests
./bin/test --full           # Comprehensive test suite
```

Verifies safe and protected operations work correctly.

**Detailed diagnostics:**

```bash
./bin/diagnose
```

In-depth troubleshooting information for all components.

**Build tools:**

```bash
./scripts/build.sh
```

Compiles all system tools from source. Run after any code changes.

---

## Installation

> [!NOTE]
> **Current Status:** System is already installed and operational. The following instructions are provided for reference or if reinstallation is needed.

### Quick Start

> [!TIP]
> Copy and run these commands in order. Each step builds on the previous one.

**Step 1: Install sudoers configuration**

```bash
cd ~/.claude/system
sudo ./scripts/sudoers/install.sh
```

<details>
<summary>What this does</summary>

Configures your system to allow passwordless sudo for safe development operations (package installation, service management, file permissions). Destructive operations still require a password for safety.

</details>

**Step 2: Integrate environment variables**

```bash
cd ~/.claude/system
./scripts/env/integrate.sh
source ~/.bashrc
```

<details>
<summary>What this does</summary>

Sets up environment variables that make command-line tools non-interactive (no prompts during installs). The `source ~/.bashrc` command activates the changes immediately.

</details>

**Step 3: Test the setup**

```bash
sudo apt update
```

> [!TIP]
> If this runs without asking for a password, the system is working correctly!

### Current Installation Status

The Interactive Terminal System is **fully installed and operational**:

- ✅ Sudoers file: `/etc/sudoers.d/90-cpi-si-safe-operations` (440 permissions)
- ✅ Environment variables: All 4 variables active (DEBIAN_FRONTEND, NEEDRESTART_MODE, PIP_NO_INPUT, NPM_CONFIG_YES)
- ✅ Shell integration: Active in ~/.bashrc
- ✅ System binaries: All 4 commands built and ready

### Manual Integration

<details>
<summary><b>Manual Sudoers Installation</b></summary>

If the install script doesn't work:

```bash
# Validate syntax
sudo visudo -c -f ~/.claude/system/sudoers/90-cpi-si-safe-operations

# Copy to sudoers.d (or use install script)
sudo cp ~/.claude/system/sudoers/90-cpi-si-safe-operations /etc/sudoers.d/

# Set permissions
sudo chmod 440 /etc/sudoers.d/90-cpi-si-safe-operations

# Verify
sudo visudo -c
```

</details>

<details>
<summary><b>Manual Environment Integration</b></summary>

Add to `~/.bashrc`:

```bash
# CPI-SI Interactive Terminal System - Environment Configuration
if [ -f "$HOME/.claude/system/env/non-interactive.conf" ]; then
    source "$HOME/.claude/system/env/non-interactive.conf"
fi
```

Then:

```bash
source ~/.bashrc
```

</details>

---

## Usage

### Testing the Setup

> [!NOTE]
> **System Already Configured:** These tests verify the current operational state.

**Quick test - passwordless sudo:**

```bash
sudo apt update
```

Expected: Runs immediately without asking for password ✅

<details>
<summary><b>Comprehensive tests</b></summary>

**Test safe operations (should work without password):**

```bash
sudo apt update              # Update package lists
sudo apt install tree        # Install a package
```

**Test safety boundaries (should require password):**

```bash
sudo apt remove systemd      # Protected: requires password
```

**Test environment variables:**

```bash
env | grep DEBIAN_FRONTEND   # Should show: DEBIAN_FRONTEND=noninteractive
env | grep PIP_NO_INPUT      # Should show: PIP_NO_INPUT=1
env | grep NPM_CONFIG_YES    # Should show: NPM_CONFIG_YES=true
env | grep NEEDRESTART_MODE  # Should show: NEEDRESTART_MODE=a
```

**Verification checklist:**

| Component | Status | What to expect |
|-----------|--------|----------------|
| Passwordless sudo | ✅ Operational | `sudo apt update` runs without password |
| Environment variables | ✅ All 4 active | Commands shown above display values |
| Safety boundaries | ✅ Enforced | Protected operations require password |

</details>

### Common Operations

<details>
<summary><b>Installing Development Tools</b></summary>

```bash
# Install Lua and LuaJIT
sudo apt install lua5.4 luajit

# Install Python packages
pip install black pytest

# Install npm packages
npm install -g typescript

# Install Rust tools
cargo install ripgrep
```

</details>

<details>
<summary><b>Service Management</b></summary>

```bash
# Start development server
sudo systemctl start postgresql

# Stop service
sudo systemctl stop nginx

# Restart service
sudo systemctl restart docker
```

</details>

<details>
<summary><b>File Permissions</b></summary>

```bash
# Make script executable
sudo chmod +x build.sh

# Change ownership
sudo chown user:user project/

# Recursive permissions
sudo chmod -R 755 bin/
```

</details>

---

## Troubleshooting

> [!TIP]
> Run `./bin/diagnose` for detailed system information and specific recommendations.

**Quick checks:**

| Issue | First thing to try | Why this works |
|-------|-------------------|----------------|
| Password prompts | Logout and login again | Sudoers changes need new session |
| Variables not set | `source ~/.bashrc` | Activates shell configuration |
| Commands fail | `./bin/status` | Shows what's configured |

<details>
<summary><b>Sudoers configuration not working</b></summary>

**Check if file exists and has correct permissions:**

```bash
ls -l /etc/sudoers.d/90-cpi-si-safe-operations
```

Expected output: `-r--r----- 1 root root` (440 permissions)

**Verify syntax is valid:**

```bash
sudo visudo -c
```

Expected: No errors reported

**If file is missing or has errors:**

```bash
cd ~/.claude/system
sudo ./scripts/sudoers/install.sh
```

> [!NOTE]
> After fixing sudoers, logout and login again for changes to take effect.

</details>

<details>
<summary><b>Environment variables not applied</b></summary>

**Check if integration is in ~/.bashrc:**

```bash
grep "CPI-SI" ~/.bashrc
```

Expected: Should see the CPI-SI environment configuration section

**Activate immediately:**

```bash
source ~/.bashrc
```

**Verify variables are set:**

```bash
env | grep DEBIAN_FRONTEND
```

Expected: `DEBIAN_FRONTEND=noninteractive`

**If integration is missing:**

```bash
cd ~/.claude/system
./scripts/env/integrate.sh
source ~/.bashrc
```

</details>

<details>
<summary><b>Still getting password prompts</b></summary>

**Common causes:**

1. **New session needed** - Sudoers changes require logout/login

   ```bash
   # After logging back in, test:
   sudo apt update
   ```

2. **Command not in allow list** - Check exact command syntax

   ```bash
   # View allowed commands:
   cat /etc/sudoers.d/90-cpi-si-safe-operations
   ```

3. **Typo in command** - Verify spelling and paths match configuration

> [!TIP]
> Run `./bin/test --quick` to verify which operations work without password.

</details>

---

## Reference

### Sudoers Configuration

**File:** `/etc/sudoers.d/90-cpi-si-safe-operations`

**Structure:** Follows 4-block pattern (METADATA, SETUP, BODY, CLOSING)

**Key Sections:**

| Section | Purpose | Examples |
|---------|---------|----------|
| **SETUP** | Environment preservation | Keep DEBIAN_FRONTEND |
| **BODY** | Safe operations (NOPASSWD) | apt install, systemctl start |
| **CLOSING** | Safety boundaries (explicit denies) | Essential package removal |

### Environment Variables

**File:** `~/.claude/system/env/non-interactive.conf`

| Category | Variables | Purpose |
|----------|-----------|---------|
| **Package Management** | DEBIAN_FRONTEND, NEEDRESTART_MODE | Eliminate prompts |
| **Python** | PIP_NO_INPUT, PYTHONUNBUFFERED | Non-interactive pip |
| **Node.js** | NPM_CONFIG_YES, NODE_ENV | Auto-yes for npm |
| **Rust** | RUST_BACKTRACE, CARGO_TERM_COLOR | Cargo configuration |
| **Git** | GIT_EDITOR, GIT_PAGER | Editor and paging |
| **CPI-SI** | CPI_SI_*_PATH | Framework paths |

### Integration Points

<details>
<summary><b>CLAUDE.md Integration</b></summary>

System awareness documented in global CLAUDE.md:

```markdown
**Interactive Terminal System:** Full terminal environment through Claude Code CLI.
Passwordless sudo for safe development operations. Location: ~/.claude/system/
```

</details>

<details>
<summary><b>settings.json Integration</b></summary>

Environment variables configured in `~/.claude/settings.json`:

```json
"env": {
  "DEBIAN_FRONTEND": "noninteractive",
  "NEEDRESTART_MODE": "a",
  "PIP_NO_INPUT": "1",
  "NPM_CONFIG_YES": "true",
  "CPI_SI_SYSTEM_PATH": "/home/seanje-lenox-wise/.claude/system"
}
```

</details>

---

## Further Documentation

- **Complete Design:** See [`docs/architecture.md`](docs/architecture.md) for architectural details
- **Immune System Paradigm:** See [`docs/logging-debugging-restoration.md`](docs/logging-debugging-restoration.md) for Detection → Assessment → Restoration
- **Logging System:** See [`logs/README.md`](logs/README.md) for game dev quality logging and health tracking
- **Operations Reference:** See [`docs/operations-reference.md`](docs/operations-reference.md) for detailed command catalog
- **GUI Password Prompts:** See [`docs/gui-sudo-prompts.md`](docs/gui-sudo-prompts.md) for pkexec vs sudo usage
- **CLAUDE.md:** Framework-level documentation of system awareness

---

## Status

**Current:** ✅ Fully operational - installed and configured

**System Health:**

- ✅ Sudoers configuration: Installed and working
- ✅ Environment variables: Configured and active
- ✅ System tools: Built (4 binaries, ~12MB total)
- ✅ Immune system - Detection layer: Logging operational (health tracking active)

**Future:**

- Debugging tools (Assessment layer) - pattern recognition and routing
- Restoration system (Response layer) - antibodies for automated fixes
- Wrapper scripts for interactive programs as needed

---

<div align="center">

**Built with intentional design for Kingdom Technology**

*"In the beginning, God created the heavens and the earth." - Genesis 1:1*

</div>
