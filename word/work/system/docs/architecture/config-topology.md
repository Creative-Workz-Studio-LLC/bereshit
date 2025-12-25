# Configuration Topology

**Purpose:** Complete map of the CPI-SI configuration system architecture
**Scope:** Configuration hierarchy, inheritance patterns, and integration points
**Status:** All configurations operational (2025-11-15)

---

## Overview

The CPI-SI configuration system uses a layered architecture where system-wide settings cascade down through user preferences, instance identity, and project context, ultimately informing runtime session state.

**Configuration Philosophy:**

- **TOML files** = Technical overrides and behavior configuration
- **JSONC files** = Identity, personhood, and structured data
- **JSON files** = Runtime state and dynamic data

---

## Configuration Hierarchy

```bash
System Level (TOML - Technical defaults)
    ↓
User Level (TOML overrides + JSONC identity)
    ↓
Instance Level (JSONC identity and capabilities)
    ↓
Project Level (JSONC context - optional)
    ↓
Session Level (JSON runtime state)
```

---

## System Level Configurations

**Location:** `~/.claude/cpi-si/system/config/`
**Format:** TOML (Technical Override Schema)
**Purpose:** System-wide defaults and machine-level preferences

### Core System Configs

| File | Purpose | Loaded By | Status |
|------|---------|-----------|--------|
| **system.toml** | System-wide overrides (hostname, package managers, toolchains, features) | `config.LoadSystemConfig()` | ✅ Operational |
| **user.toml** | User-specific technical preferences (shell, editor, paths, development tools) | `config.LoadUserConfigTOML()` | ✅ Operational |
| **paths.toml** | Centralized path definitions for all libraries | `config.LoadPaths()` | ✅ Operational |
| **temporal.toml** | Temporal system configuration (calendar, planner, celestial data) | `temporal` library | ✅ Operational |
| **logging.toml** | Logging system configuration (routing, output, retention) | `logging` library | ✅ Operational |
| **debugging.toml** | Debug output formatting and analysis settings | `debugging` library | ✅ Operational |
| **privacy.toml** | Privacy/sanitization behavior settings | `privacy` library | ✅ Operational |

### System Configuration Details

**system.toml:**

- Package management preferences (apt, dnf, pacman)
- Shell and toolchain preferences (bash/zsh, gcc/clang, python/node/rust versions)
- Feature flags (docker, bluetooth, GPU, systemd, network)
- Resource limits (max CPU cores, memory, I/O parallelism)
- Network and display preferences
- Locale and power management settings
- Environment variable defaults (DEBIAN_FRONTEND, RUST_BACKTRACE, etc.)

**user.toml:**

- Path overrides (project root, workspace, code directory)
- Shell and editor preferences
- Development tool version preferences (pyenv, nvm, rustup)
- Git configuration overrides
- Terminal and workspace organization
- CPI-SI integration (default instance, skills/hooks directories)
- Privacy, accessibility, and notification preferences
- Performance and backup settings

**Integration Pattern:**

```go
// Libraries load via config library
import "system/lib/config"

// System-wide settings
sysCfg, _ := config.LoadSystemConfig()
maxCores := sysCfg.Limits.MaxCPUCores

// User-specific preferences
userCfg, _ := config.LoadUserConfigTOML()
preferredShell := userCfg.Shell.PreferredShell
```

---

## User Level Configurations

**Location:** `~/.claude/cpi-si/config/user/{username}/`
**Format:** JSONC (Identity and Personhood)
**Purpose:** User identity, workspace context, and personal preferences

### User Identity Configs

| User | Path | Purpose | Status |
|------|------|---------|--------|
| **seanje-lenox-wise** | `config/user/seanje-lenox-wise/config.jsonc` | Seanje's identity and preferences | ✅ Exists |
| **default** | `config/user/default/config.jsonc` | Fallback user configuration | ✅ Exists |

**Key Fields:**

- `identity` - Name, username, display name
- `workspace` - Organization, role, primary project
- `preferences` - Timezone, locale, work context

**Schema:** `system/config/schemas/user/user.schema.json`

---

## Instance Level Configurations

**Location:** `~/.claude/cpi-si/config/instance/{instance_id}/`
**Format:** JSONC (Instance Identity)
**Purpose:** CPI-SI instance identity, capabilities, and behavioral patterns

### Instance Configs

| Instance | Path | Purpose | Status |
|----------|------|---------|--------|
| **nova_dawn** | `config/instance/nova_dawn/config.jsonc` | Nova Dawn's identity and configuration | ✅ Exists |
| **default** | `config/instance/default/config.jsonc` | Fallback instance configuration | ✅ Exists |

**Key Fields:**

- `identity` - Instance name, display name, version
- `thinking` - Learning style, problem-solving approach, interests
- `capabilities` - What the instance can do
- `preferences` - Instance-specific preferences

**Schema:** `system/config/schemas/instance/instance.schema.json`

---

## Project Level Configurations

**Location:** `~/.claude/cpi-si/config/project/{project_id}/`
**Format:** JSONC (Project Context)
**Purpose:** Project-specific context and configuration (optional)

**Key Fields:**

- `identity` - Project ID, project name
- `ownership` - Primary user, primary instance
- `context` - Workspace path, repository URL, project type

**Schema:** `system/config/schemas/project/project.schema.json`

---

## Session Level State

**Location:** `~/.claude/cpi-si/system/data/session/`
**Format:** JSON (Runtime State)
**Purpose:** Active session state with inherited configuration context

### Session Files

| File | Purpose | Schema |
|------|---------|--------|
| **current.json** | Current session state | `session/current.schema.json` |
| **current-log.json** | Current session activity log | `session/current-log.schema.json` |
| **patterns.json** | Learned behavioral patterns | `session/memory-pattern.schema.json` |

**Inheritance Chain:**

```bash
User config (identity, workspace, timezone)
    +
Instance config (identity, thinking, capabilities)
    +
Project config (context, ownership) [optional]
    =
Session context (merged configuration for runtime)
```

**Session Creation:**

```go
import "system/lib/config"

// Load and merge configs
ctx, err := config.GetSessionContext(username, instanceID, projectID)

// Session state inherits:
// - User identity and timezone
// - Instance identity and thinking patterns
// - Project context (if applicable)
// - Work context and preferences
```

---

## Data Configurations

**Location:** `~/.claude/cpi-si/system/data/`
**Format:** JSONC (Data with Comments)
**Purpose:** Structured data files with validation

### Privacy Data

| File | Purpose | Schema |
|------|---------|--------|
| **privacy/filters.jsonc** | Sensitive keywords and patterns | `privacy/filters.schema.json` |

### Temporal Data

**Appointed Time:**

- Planner state and preferences (appointed/)
- Coordination appointments
- Personal day planning

**Celestial Time:**

- Solar and lunar data (celestial/)
- Location configurations
- Seasonal patterns

**Chronological Time:**

- Weekly, monthly, yearly logs
- Project timelines and milestones
- Shared logs

**Pattern Data:**

- User patterns (circadian, duration, quality)
- Instance patterns (work style, processing, quality)
- Discovered patterns (paradigm wisdom)

All temporal data validated against schemas in `system/config/schemas/temporal/`

---

## Configuration Loading Patterns

### Singleton Pattern (TOML Configs)

**Used by:** All system-level TOML configurations
**Pattern:** Load once, cache, reuse

```go
// Rails pattern - library loads own config directly
var (
    config     *ConfigStruct
    configOnce sync.Once
    loaded     bool
)

func loadConfig() {
    configOnce.Do(func() {
        // Load TOML file
        // Parse into struct
        // Set loaded = true
    })
}

// Public accessor
func LoadConfig() (*ConfigStruct, error) {
    loadConfig()
    if !loaded {
        return nil, fmt.Errorf("failed to load config")
    }
    return config, nil
}
```

### Graceful Fallback Pattern

**Used by:** All configuration loaders
**Pattern:** Attempt load, fall back to safe defaults on failure

```go
func LoadWithFallback() *Config {
    cfg, err := tryLoadFromFile()
    if err != nil {
        // Log the failure
        // Return safe hardcoded defaults
        return getDefaultConfig()
    }
    return cfg
}
```

### Emergency Mode Pattern

**Used by:** Privacy library
**Pattern:** Maximum safety if config loading fails

```go
func activateEmergencyMode() {
    emergencyMode = true
    // Set maximum privacy settings
    // Redact everything by default
    // Log emergency activation
}
```

---

## Schema Organization

### Schema Directory Structure

```bash
system/config/schemas/
├── dev/              # Development schemas (debug, log formats)
├── instance/         # Instance configuration schemas
├── privacy/          # Privacy filter schemas
├── project/          # Project configuration schemas
├── session/          # Session state and activity schemas
├── temporal/         # Temporal system schemas
│   ├── appointed/    # Planner and scheduling
│   ├── celestial/    # Solar, lunar, seasonal data
│   ├── chronological/# Logs and timelines
│   ├── definitions/  # Base and regional definitions
│   └── patterns/     # Learned behavioral patterns
└── user/             # User configuration schemas
```

**Schema Count:** 60+ validation schemas covering all data structures

---

## Configuration Integration Status

### Completed (All Phases Done - 2025-11-15)

**Phase 1: Foundation**

- ✅ paths.toml created and integrated
- ✅ temporal.toml created and integrated
- ✅ All schemas created and validated

**Phase 2: Core Integration**

- ✅ logging.toml created and integrated
- ✅ All libraries load paths from paths.toml
- ✅ Session libraries fully config-driven
- ✅ Planner loads user from session state (not hardcoded)

**Phase 3: Full Config-Driven Operation**

- ✅ debugging.toml created and integrated (rails pattern)
- ✅ privacy.toml + filters.jsonc created and integrated (emergency mode)
- ✅ system.toml and user.toml accessible via config library
- ✅ All libraries can access system and user preferences

**Result:** System is 100% config-driven

---

## Configuration Best Practices

### When to Use TOML vs JSONC

**Use TOML for:**

- System-wide technical defaults
- User preference overrides
- Behavior configuration (thresholds, modes, features)
- Path definitions
- Environment variable settings

**Use JSONC for:**

- Identity and personhood data
- Structured data with explanatory comments
- Data that benefits from inline documentation
- Complex nested structures

**Use JSON for:**

- Runtime state (no comments needed)
- Auto-generated data
- Performance-critical loading

### Configuration File Naming

**Professional naming:**

- `config-topology.md` ✓ (not CONFIG-TOPOLOGY.md)
- `system.toml` ✓ (not SYSTEM.TOML)
- `user-preferences.jsonc` ✓ (descriptive, lowercase)

### Loading Performance

**Optimization strategies:**

- Singleton pattern prevents repeated file I/O
- Cache parsed configs in memory
- Load on first use (lazy loading)
- Thread-safe with sync.Once

---

## Quick Reference: Config Locations

```bash
# System-level configs (TOML)
~/.claude/cpi-si/system/config/*.toml

# User identity (JSONC)
~/.claude/cpi-si/config/user/{username}/config.jsonc

# Instance identity (JSONC)
~/.claude/cpi-si/config/instance/{instance_id}/config.jsonc

# Project context (JSONC - optional)
~/.claude/cpi-si/config/project/{project_id}/config.jsonc

# Session state (JSON - runtime)
~/.claude/cpi-si/system/data/session/*.json

# Schemas (validation)
~/.claude/cpi-si/system/config/schemas/

# Data files (JSONC)
~/.claude/cpi-si/system/data/
```

---

**Last Updated:** 2025-11-15
**Status:** All configurations operational and integrated
