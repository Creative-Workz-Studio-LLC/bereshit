<div align="center">

# 🔧 System Scripts

**Shell scripts for Interactive Terminal System**

![Bash](https://img.shields.io/badge/Bash-4EAA25?style=flat&logo=gnu-bash&logoColor=white)
![Linux](https://img.shields.io/badge/Linux-FCC624?style=flat&logo=linux&logoColor=black)

*Build, installation, and integration automation*

[Scripts](#scripts) • [Standards](#script-standards) • [Development](#adding-new-scripts)

</div>

---

## Table of Contents

- [🔧 System Scripts](#-system-scripts)
  - [Table of Contents](#table-of-contents)
  - [Overview](#overview)
  - [Script Organization](#script-organization)
  - [Scripts](#scripts)
    - [build.sh](#buildsh)
    - [sudoers/install.sh](#sudoersinstallsh)
    - [env/integrate.sh](#envintegratesh)
  - [Script Standards](#script-standards)
    - [4-Block Structure](#4-block-structure)
    - [Error Handling](#error-handling)
    - [Color Output](#color-output)
    - [Path Handling](#path-handling)
  - [Adding New Scripts](#adding-new-scripts)

---

## Overview

**What are scripts?** Automated commands that set up or build parts of the system. Instead of typing multiple commands manually, run one script.

**Quick reference:**

| Script | What it does | When to use |
|--------|--------------|-------------|
| `build.sh` | Compiles all system tools | After code changes or updates |
| `sudoers/install.sh` | Sets up passwordless sudo | Initial installation or if sudo breaks |
| `env/integrate.sh` | Configures shell environment | Initial installation or new shell |

> [!NOTE]
> All scripts are safe to run multiple times - they won't break things if you run them again.

---

## Script Organization

```bash
scripts/
├── build.sh              # Build all system binaries
├── sudoers/
│   └── install.sh        # Install sudoers configuration
└── env/
    └── integrate.sh      # Integrate environment variables
```

**Pattern:** Scripts are organized by domain. Build scripts at root, installation scripts in subdirectories matching their purpose (sudoers, env).

---

## Scripts

### build.sh

**Compiles all system tools from source code**

```bash
cd ~/.claude/system
./scripts/build.sh
```

**What to expect:**
- Script shows progress: "Building validate... ✓ Success"
- Takes 5-10 seconds to compile 4 binaries
- Creates `bin/` directory with: validate, test, status, diagnose
- Green checkmarks = success, red X = failure

**When to run:**
- After code changes
- After pulling updates
- If binaries are missing or outdated

<details>
<summary><b>Technical details</b></summary>

**What it does:**
- Compiles all commands from `cmd/` to `bin/`
- Shows build progress with color-coded output
- Reports success/failure for each binary
- Creates `bin/` directory if needed

**Output format:**
- Yellow headers for sections
- Green ✓ for successful builds
- Red ✗ for failures

**Exit codes:**
- `0` = All binaries built successfully
- `1` = One or more builds failed

</details>

---

### sudoers/install.sh

**Sets up passwordless sudo for safe operations**

```bash
cd ~/.claude/system
sudo ./scripts/sudoers/install.sh
```

> [!IMPORTANT]
> Requires `sudo` to run (will ask for password once)

**What to expect:**
- Validates configuration before installing (safety check)
- Creates backup if file already exists
- Installs to `/etc/sudoers.d/90-cpi-si-safe-operations`
- Shows green "✓ Installed successfully" when done
- After this, safe `sudo` commands won't ask for password

**Safety features:**

| Protection | What it does |
|-----------|--------------|
| Syntax validation | Won't install if configuration is invalid |
| Automatic backup | Saves existing configuration first |
| Auto-restore on failure | Rolls back if something goes wrong |
| Permission check | Ensures correct file permissions (440) |

<details>
<summary><b>Technical details</b></summary>

**What it does:**
- Validates syntax with `visudo -c` before installation
- Creates timestamped backup of existing configuration
- Copies configuration to `/etc/sudoers.d/`
- Sets permissions to 440 (read-only, root-owned)
- Verifies installation with `visudo -c`

**Exit codes:**
- `0` = Installation successful
- `1` = Validation failed or installation error

**File location:**
- Source: `~/.claude/system/sudoers/90-cpi-si-safe-operations`
- Installed: `/etc/sudoers.d/90-cpi-si-safe-operations`
- Backup: `/etc/sudoers.d/90-cpi-si-safe-operations.backup.TIMESTAMP`

</details>

---

### env/integrate.sh

**Configures your shell to use non-interactive defaults**

```bash
cd ~/.claude/system
./scripts/env/integrate.sh
source ~/.bashrc
```

**What to expect:**
- Checks if already configured (won't duplicate)
- Adds a few lines to your `~/.bashrc` file
- Shows what was added
- Run `source ~/.bashrc` to activate immediately

> [!TIP]
> Safe to run multiple times - it checks if already configured and skips if so.

**What this does:**
Makes command-line tools non-interactive (no prompts during installs). For example:
- `apt install` won't ask "Do you want to continue? [Y/n]"
- `pip install` won't prompt for confirmations
- Tools restart automatically when needed

<details>
<summary><b>What gets added to ~/.bashrc</b></summary>

**Integration code:**

```bash
# CPI-SI Interactive Terminal System - Non-Interactive Environment Defaults
if [ -f "$HOME/.claude/system/env/non-interactive.conf" ]; then
    source "$HOME/.claude/system/env/non-interactive.conf"
fi
```

This loads environment variables from `~/.claude/system/env/non-interactive.conf` whenever you open a terminal.

**Why it's safe:**
- Only loads if the config file exists
- Doesn't override existing environment variables
- Easy to remove (just delete these lines from ~/.bashrc)

</details>

---

## Script Standards

<details>
<summary><b>Standards for script developers</b></summary>

All scripts in this directory follow consistent standards for reliability, safety, and maintainability.

### 4-Block Structure

Every script follows the 4-block pattern:

```bash
# ============================================================================
# METADATA
# ============================================================================
# Script Name - CPI-SI Interactive Terminal System
# Purpose: What this script does
# Blocking: Does it require user interaction or run automatically?

# ============================================================================
# SETUP
# ============================================================================

# Variables, paths, colors
# Environment preparation

# ============================================================================
# BODY
# ============================================================================

# Main logic and functions
# Core functionality

# ============================================================================
# CLOSING
# ============================================================================

# Execute main logic
# Exit with appropriate code
```

### Error Handling

**Standard practices:**

- `set -e` for immediate exit on error
- Proper exit codes (0 = success, 1 = failure)
- Clear, actionable error messages
- Validation before destructive operations
- Cleanup on failure where applicable

### Color Output

**Consistent color scheme:**

| Color | Usage | Example |
|-------|-------|---------|
| Yellow | Headers, info | Build progress, section titles |
| Green | Success | Successful builds, installations |
| Red | Errors | Failed operations, validation errors |
| Blue | Details | File paths, technical info |

**Implementation:**

```bash
YELLOW='\033[1;33m'
GREEN='\033[0;32m'
RED='\033[0;31m'
BLUE='\033[0;34m'
NC='\033[0m'  # No Color
```

### Path Handling

**Best practices:**

- Use `SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"` for script location
- Build absolute paths from known locations
- Validate paths exist before operations
- Use proper quoting for paths with spaces
- Check write permissions before attempting changes

</details>

### Health Scoring

**TRUE SCORE Philosophy:** Every action has genuine impact value, not artificially divided points.

Scripts use **TRUE SCORE** to track health:
- Each action scored by REAL impact (not divided to sum to 100)
- Total Possible = sum of all action values
- Normalization converts to percentage: `(cumulative / total_possible) × 100`
- Creates unique health fingerprints for debugging

**Example: build.sh**

```bash
# Total Possible: 612 points

Setup (10 points):
  - Source logging: +5 or -5
  - Create bin/: +3 or -3
  - Initialize: +2

Per binary (148 points × 4 = 592 points):
  - Check source exists: +15 or -15
  - Go compiler available: +20 or -20
  - Compilation starts: +5
  - Compilation completes: +90 or -90  (CRITICAL)
  - Binary written: +10 or -10
  - Binary executable: +8 or -8

Verification (10 points):
  - Count results: +3
  - Log final state: +5 or -5
  - Display output: +2 or -2

# Perfect execution: 612/612 = 100%
# 3 binaries fail compilation: (612 - 270)/612 = 56%
```

**Why TRUE SCORE matters:**

- **Debugging:** "Health: 56%, raw: 342/612" tells you exactly what happened
- **Accuracy:** Values reflect actual importance (compilation = 90 points, not arbitrary 8)
- **Fingerprints:** Unique raw scores create identifiable patterns
- **Transparency:** See real impact, not normalized-first artificial scores

**All scripts follow this pattern:**
- `build.sh`: 612 total points
- `env/integrate.sh`: 150 total points
- `sudoers/install.sh`: 170 total points

**Verified execution examples:**

```bash
# build.sh - Perfect execution (all 4 binaries built)
HEALTH: 100% (raw: 612, Δ+2) 💚

# env/integrate.sh - Already integrated (early exit)
HEALTH: 33% (raw: 50, Δ0) 💔

# sudoers/install.sh - Complete installation
HEALTH: 100% (raw: 170, Δ+48) 💚
```

**Technical note - Sudo context logging:**

Scripts run with `sudo` override `LOG_BASE_DIR` after sourcing logger because `$HOME` becomes `/root` in sudo context. This ensures logs write to the correct user directory:

```bash
source "$SYSTEM_DIR/lib/logging/logger.sh"
LOG_BASE_DIR="$SYSTEM_DIR/logs"  # Override for sudo context
declare_health_total 170
```

</details>

---

<details>
<summary><b>Adding New Scripts (for developers)</b></summary>

## Adding New Scripts

**Process:**

1. **Determine category** - Build, install, configure, test?
2. **Create in appropriate location**
   - Build scripts → `scripts/`
   - Installation scripts → `scripts/<domain>/`
3. **Implement 4-block structure**
   - Start with METADATA block
   - Define SETUP (variables, colors, paths)
   - Implement BODY (main logic)
   - Add CLOSING (execution, exit codes)
4. **Add error handling**
   - Use `set -e`
   - Validate inputs
   - Provide clear error messages
5. **Apply color scheme**
   - Yellow for headers
   - Green for success
   - Red for errors
6. **Make executable**

   ```bash
   chmod +x scripts/new-script.sh
   ```

7. **Test thoroughly**
   - Test success path
   - Test failure cases
   - Verify idempotency if applicable
   - Check exit codes
8. **Update this README**
   - Add script documentation
   - Update organization tree
   - Document usage and purpose

**Template:**

```bash
#!/bin/bash
# ============================================================================
# METADATA
# ============================================================================
# Script Name - CPI-SI Interactive Terminal System
# Purpose: Brief description
# Blocking: Yes/No - explanation

# ============================================================================
# SETUP
# ============================================================================

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

# Colors
YELLOW='\033[1;33m'
GREEN='\033[0;32m'
RED='\033[0;31m'
NC='\033[0m'

# ============================================================================
# BODY
# ============================================================================

main() {
    # Implementation here
    echo -e "${GREEN}Success${NC}"
}

# ============================================================================
# CLOSING
# ============================================================================

main
exit 0
```

</details>

---

<div align="center">

**Built with intentional design for Kingdom Technology**

[Back to System Documentation](../README.md)

</div>
