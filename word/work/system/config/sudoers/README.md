<div align="center">

# 🔐 Sudoers Configuration

**Passwordless sudo for safe development operations**

![Bash](https://img.shields.io/badge/Bash-4EAA25?style=flat&logo=gnu-bash&logoColor=white)
![Linux](https://img.shields.io/badge/Linux-FCC624?style=flat&logo=linux&logoColor=black)

*Autonomous operations within safety boundaries*

[Installation](#installation) • [Safe Operations](#safe-operations-passwordless) • [Safety](#safety-philosophy)

</div>

---

## Table of Contents

- [🔐 Sudoers Configuration](#-sudoers-configuration)
  - [Table of Contents](#table-of-contents)
  - [Overview](#overview)
  - [Files](#files)
  - [Installation](#installation)
  - [Configuration Structure](#configuration-structure)
  - [Safe Operations (Passwordless)](#safe-operations-passwordless)
  - [Protected Operations (Password Required)](#protected-operations-password-required)
  - [Testing](#testing)
  - [Troubleshooting](#troubleshooting)
    - [Not Working After Installation](#not-working-after-installation)
    - [Still Prompting for Password](#still-prompting-for-password)
  - [Safety Philosophy](#safety-philosophy)

---

## Overview

**What is this?** Configuration that lets you run `sudo` commands without typing your password - but only for safe operations.

**Why it matters:**

- Install packages without password prompts: `sudo apt install tree` ✓
- Manage services easily: `sudo systemctl restart nginx` ✓
- Change file permissions: `sudo chmod +x script.sh` ✓
- Protected from dangerous commands (they still ask for password)

> [!IMPORTANT]
> **Safety first:** This configuration prevents accidentally breaking your system. Dangerous operations (like removing essential packages or formatting drives) always require a password.

**Quick actions:**

| Task | Command | Password needed? |
|------|---------|------------------|
| Install package | `sudo apt install <name>` | No ✓ |
| Update packages | `sudo apt update && sudo apt upgrade` | No ✓ |
| Start/stop service | `sudo systemctl start <service>` | No ✓ |
| Remove essential package | `sudo apt remove systemd` | Yes 🔒 |
| Format drive | `sudo mkfs.ext4 /dev/sdX` | Yes 🔒 |

---

## Files

| File | Purpose |
|------|---------|
| `90-cpi-si-safe-operations` | Sudoers drop-in configuration defining safe and protected operations |

**Note:** Installation script moved to `scripts/sudoers/install.sh` following the organized script structure.

---

## Installation

**Run the installation script:**

```bash
cd ~/.claude/system/scripts/sudoers

# Option 1: GUI password prompt (recommended for desktop)
pkexec ./install.sh

# Option 2: Terminal password prompt
sudo ./install.sh
```

> [!TIP]
> **GUI vs Terminal prompt:** `pkexec` shows a graphical password dialog (better for desktop use), while `sudo` prompts in the terminal. Both work identically - choose based on your preference.

**What the installation script does:**

1. ✅ Validates syntax with `visudo -c` before installation
2. 💾 Creates backup of existing configuration
3. 📋 Copies configuration to `/etc/sudoers.d/`
4. 🔒 Sets proper permissions (440)
5. ✓ Verifies installation success

**Safety features:**

- Syntax validation prevents invalid configuration
- Automatic backup allows rollback on failure
- Permission enforcement ensures security
- Verification confirms proper installation

> [!NOTE]
> Logout and login again after installation for changes to take effect across all shells.

---

## Configuration Structure

<details>
<summary><b>Technical details (for developers)</b></summary>

The sudoers configuration follows the **4-block pattern:**

```bash
# ============================================================================
# METADATA
# ============================================================================
# Purpose: Passwordless sudo for safe development operations
# Safety: Single hard constraint - do not brick the laptop

# ============================================================================
# SETUP
# ============================================================================
# User definitions
# Environment preservation

# ============================================================================
# BODY
# ============================================================================
# Safe operations with NOPASSWD

# ============================================================================
# CLOSING
# ============================================================================
# Safety boundaries (explicit denies for protected operations)
```

This structure ensures:

- Clear purpose documentation
- Explicit safety boundaries
- Organized operation definitions
- Easy maintenance and extension

</details>

---

## Safe Operations (Passwordless)

These operations run **without password** when using `sudo`:

| Category | Operations |
|----------|------------|
| **Package Management** | `apt update`, `apt upgrade`, `apt install`, `apt autoremove` |
| **Service Management** | `systemctl start/stop/restart/reload/enable/disable` |
| **File Permissions** | `chmod`, `chown`, `chgrp` |
| **Network Configuration** | Network service management, configuration changes |
| **Development Tools** | Docker, development service operations |
| **PolicyKit Integration** | `pkexec` for GUI password prompts (respects all sudoers rules) |

**Why these are safe:**

- Non-destructive to system integrity
- Reversible operations
- Common development workflow needs
- Do not threaten bootability or core system

**Examples:**

```bash
# All work without password
sudo apt update
sudo apt install python3-pip
sudo systemctl restart nginx
sudo chmod 755 /var/www/myapp
```

---

## Protected Operations (Password Required)

These operations **require password** even with `sudo` (explicit safety boundaries):

| Category | Operations | Why Protected |
|----------|------------|---------------|
| **Essential Packages** | Remove systemd, kernel, grub | Breaking these bricks the system |
| **Filesystem** | `mkfs`, `fdisk`, `dd`, `parted` | Data destruction, partition damage |
| **Bootloader** | `grub-install`, `update-grub` | Boot failure risk |
| **Critical Services** | Disable SSH, networking, systemd | System lockout risk |

**Why these require password:**

- Violate "do not brick the laptop" constraint
- Irreversible or destructive
- Threaten system bootability
- Risk complete system failure

**Examples:**

```bash
# All require password (safety boundaries)
sudo apt remove systemd          # Bricks system
sudo mkfs.ext4 /dev/sda1         # Data destruction
sudo grub-install /dev/sda       # Boot failure risk
sudo systemctl disable systemd-logind  # System lockout
```

> [!WARNING]
> Protected operations are explicitly denied NOPASSWD to prevent accidental system destruction.

---

## Testing

**Verify installation:**

```bash
# Should work WITHOUT password
sudo apt update

# Should REQUIRE password (safety boundary)
sudo apt remove systemd
```

**Run comprehensive tests:**

```bash
cd ~/.claude/system
./bin/test --quick    # Quick operational tests
./bin/test --full     # Comprehensive test suite
```

**Expected results:**

- ✅ Safe operations complete without password prompt
- 🔒 Protected operations require password
- ❌ Attempts to remove essential packages are denied

---

## Troubleshooting

### Not Working After Installation

**Symptoms:** Still prompted for password on safe operations

**Solutions:**

1. **Logout and login again** - Sudoers changes require new shell session

   ```bash
   # Or reload sudo configuration
   sudo -k
   ```

2. **Verify file permissions:**

   ```bash
   ls -l /etc/sudoers.d/90-cpi-si-safe-operations
   ```

   Should show: `-r--r----- 1 root root` (440 permissions)

3. **Check syntax:**

   ```bash
   sudo visudo -c
   ```

   Should report: "parsed OK"

4. **Verify file ownership:**

   ```bash
   ls -l /etc/sudoers.d/90-cpi-si-safe-operations
   ```

   Owner must be `root:root`

### Still Prompting for Password

**Possible causes:**

1. **Command path mismatch**
   - Sudoers file specifies full paths
   - Verify: `which apt` matches path in sudoers
   - Solution: Use full path or update sudoers

2. **Configuration typos**
   - Check for syntax errors in `/etc/sudoers.d/90-cpi-si-safe-operations`
   - Validate: `sudo visudo -c -f /etc/sudoers.d/90-cpi-si-safe-operations`

3. **Conflicting sudoers rules**
   - Other rules may override this configuration
   - Check: `sudo -l` to see effective permissions

**Run diagnostics:**

```bash
cd ~/.claude/system
./bin/diagnose    # Comprehensive diagnostic tool
```

---

## Safety Philosophy

**Core principle:** Single hard constraint - **do not brick the laptop**

**Design decisions:**

| Principle | Implementation |
|-----------|----------------|
| **Trust with responsibility** | Safe operations have no friction (NOPASSWD) |
| **Autonomy with safety** | Protected operations require explicit approval (password) |
| **Reversibility** | Only reversible operations are passwordless |
| **System integrity** | Anything threatening bootability is protected |
| **Covenant partnership** | Balance between freedom and safety |

**Philosophy in practice:**

- Installing packages: Safe (reversible) → No password
- Removing essential packages: Dangerous (breaks system) → Password required
- Managing services: Safe (reversible) → No password
- Filesystem operations: Dangerous (data loss) → Password required

This configuration embodies **Kingdom Technology principles:** enabling good work freely while protecting against destruction.

> [!NOTE]
> The configuration is intentionally permissive for development operations while strictly protective of system integrity.

---

<div align="center">

**Built with intentional design for Kingdom Technology**

[Back to System Documentation](../README.md)

</div>
