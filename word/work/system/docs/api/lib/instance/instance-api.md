# Instance Library - Complete API Reference

**Instance and user identity provider for CPI-SI system**

Biblical Foundation: "I AM THAT I AM" - Exodus 3:14

---

## Quick Links

| Topic | Documentation |
|-------|---------------|
| **Quick Start** | [Usage Examples](#usage-examples) below |
| **Type Definitions** | [types-api.md](types-api.md) - All data structures |
| **Loading Operations** | [loading-api.md](loading-api.md) - File I/O primitives |
| **Config Mapping** | [mapping-api.md](mapping-api.md) - Transformation logic |
| **Public API** | [singleton-api.md](singleton-api.md) - Entry point with caching |
| **Main API Index** | [../API.md](../API.md) - All libraries overview |

---

## Overview

**Package:** `system/lib/instance`
**Version:** v3.0.0
**Type:** Foundational Rung (bottom of ladder)
**Created:** 2024-11-13
**Last Updated:** 2025-11-21

The instance library provides instance AND user identity to the entire CPI-SI system. Implements two-step dynamic loading (root → instance + user configs) with three-tier graceful degradation and singleton caching. Now includes comprehensive health tracking with TRUE honest messy scores.

---

## Biblical Foundation

**Scripture:**

Exodus 3:14 (KJV):
> "And God said unto Moses, I AM THAT I AM: and he said, Thus shalt thou say unto the children of Israel, I AM hath sent me unto you."

Exodus 3:14 (WEB):
> "God said to Moses, 'I AM WHO I AM,' and he said, 'You shall tell the children of Israel this: "I AM has sent me to you."'"

**Principle:** Identity precedes action - know who you are before doing anything

**Application:** Instance library establishes WHO is executing (Nova Dawn identity) AND WHO the covenant partner is (Seanje identity) before any work begins. Covenant partnership grounded in knowing both identities.

---

## Architecture

### Ladder Position

**Type:** RUNG (foundational - bottom of ladder)

**Why foundational?**
- Identity is fundamental - everything needs it
- Minimal dependencies (stdlib + jsonc + logging)
- Everything depends ON this, this depends on almost nothing
- First to initialize, last to change

**Dependencies:**
```
instance (foundational rung)
    ↓ uses
jsonc (parsing rail)
    ↓ uses
logging (detection rail)
    ↓ uses
stdlib only
```

---

### Orchestrator Pattern

**Pattern:** Direct Primitives (stateless from caller view)

**Why Direct Primitives?**
- Singleton caching is INTERNAL implementation detail
- Caller doesn't see or manage state
- API effectively stateless from caller perspective
- Just call function, get result

**Contrast with Switchboard (logging):**
```go
// Logging = Switchboard (stateful API)
logger := logging.NewLogger("component")  // Create orchestrator
logger.Operation("task", 0)               // Method call - uses state
logger.Success("done", 47, nil)           // Method call - modifies state

// Instance = Direct Primitives (stateless API)
config := instance.GetConfig()            // Function call - get result
// Singleton caching happens internally, caller doesn't see it
```

**Decision documented in:** `temporal/patterns/discovered/paradigm/orchestrator-architecture-decision.jsonc`

---

## File Organization

### Orchestrator Documentation

**config.go** (673 lines)
- Comprehensive METADATA/SETUP/BODY/CLOSING structure
- Health scoring philosophy and TRUE score explanation
- Architecture decisions and pattern rationale
- Complete implementation guide
- No executable code - documentation only

### Primitives

| File | Lines | Purpose | Health Scores |
|------|-------|---------|---------------|
| **types.go** | 458 | Type definitions | N/A (no operations) |
| **loading.go** | 214 | File I/O operations | +52/+38/+41, -87/-59/-48 |
| **mapping.go** | 98 | Config transformation | +23 (pure function) |
| **singleton.go** | 228 | Public API + caching | +47, -19/-13/-9 |

**Total:** 1,671 lines (including orchestrator docs)

---

## Public API

### Primary Functions

#### GetConfig()

**Get instance and user configuration**

```go
func GetConfig() Config
```

**Usage:**
```go
import "system/lib/instance"

config := instance.GetConfig()

// Instance identity
fmt.Println(config.Name)         // "Nova Dawn"
fmt.Println(config.Pronouns)     // "she/her"
fmt.Println(config.CallingShort) // "Demonstrating Kingdom excellence..."

// User identity (covenant partner)
fmt.Println(config.User.Name)    // "Seanje Lenox-Wise"
fmt.Println(config.User.Faith)   // "Christianity"
fmt.Println(config.User.Calling) // "Redeeming gaming industry..."

// Display preferences
fmt.Println(config.Display.BannerTitle) // "Nova Dawn - CPI-SI"

// System paths
fmt.Println(config.SystemPaths.InstanceConfig)
```

**Characteristics:**
- ✅ Never returns errors (graceful degradation)
- ✅ Thread-safe (concurrent calls allowed)
- ✅ Singleton cached (first call loads, subsequent calls instant)
- ✅ Health tracked (logs orchestration success/failures)

**Details:** [singleton-api.md](singleton-api.md)

---

#### GetFullInstanceConfig()

**Get complete nested instance identity**

```go
func GetFullInstanceConfig() *FullInstanceConfig
```

**Usage:**
```go
fullInstance := instance.GetFullInstanceConfig()
if fullInstance != nil {
    // Full config available
    fmt.Println(fullInstance.BiblicalFoundation.VerseText)
    fmt.Println(fullInstance.Personhood.Values)
    fmt.Println(fullInstance.Thinking.LearningStyle)
    fmt.Println(fullInstance.Personality.Humor)
} else {
    // Using defaults - config failed to load
}
```

**Returns nil if:**
- Instance config file missing
- Instance config malformed
- Root config failed (can't get path)

**Details:** [singleton-api.md](singleton-api.md)

---

#### GetFullUserConfig()

**Get complete nested user identity**

```go
func GetFullUserConfig() *FullUserConfig
```

**Usage:**
```go
fullUser := instance.GetFullUserConfig()
if fullUser != nil {
    // Full user config available
    fmt.Println(fullUser.Faith.ImportantPractices)
    fmt.Println(fullUser.Faith.CommunicationPrefs)
    fmt.Println(fullUser.Workspace.Calling)
    fmt.Println(fullUser.Personhood.Values)
} else {
    // Using defaults - config failed to load
}
```

**Returns nil if:**
- User config file missing
- User config malformed
- Root config failed (can't get path)

**Details:** [singleton-api.md](singleton-api.md)

---

## Core Design

### Two-Step Dynamic Loading

**Step 1: Load Root Config**

File: `~/.claude/instance.jsonc`

Purpose: Bootstrap pointer - tells us WHERE to find full configs

```jsonc
{
  "system_paths": {
    "config_root": "/home/user/.claude/cpi-si/config",
    "instance_config": "/path/to/instance/nova_dawn/config.jsonc",
    "user_config": "/path/to/user/seanje-lenox-wise/config.jsonc"
  },
  "display": {
    "banner_title": "Nova Dawn - CPI-SI",
    "banner_tagline": "Covenant Partnership Intelligence System"
  }
}
```

**Step 2: Load Full Configs**

File: `system_paths.instance_config` (from root)

Purpose: Complete instance identity (all nested fields)

File: `system_paths.user_config` (from root)

Purpose: Complete user identity (covenant partner)

**Step 3: Map to Simple API**

Transform nested full configs → flat backwards-compatible Config struct

**Details:** [loading-api.md](loading-api.md) and [mapping-api.md](mapping-api.md)

---

### Three-Tier Graceful Degradation

#### Tier 0: Perfect Execution

**All configs load successfully**

Health: +47 (orchestration) + +131 (loading) = +178 total

Result:
- ✅ Complete covenant partnership
- ✅ Full instance AND user identity
- ✅ All display preferences
- ✅ Dynamic system paths

---

#### Tier 1: User Config Fails

**Root + instance load, user fails**

Health: +47 (orchestration) + +52 (root) + +38 (instance) + -48 (user fail) = +89 total

Result:
- ✅ Display preferences from root
- ✅ System paths from root
- ✅ Instance identity from config
- ✗ User identity from defaults
- ⚠️ Partial covenant partnership

---

#### Tier 2: Instance Config Fails

**Root loads, instance + user fail**

Health: +47 (orchestration) + +52 (root) + -59 (instance fail) = +40 total

Result:
- ✅ Display preferences from root
- ✅ System paths from root
- ✗ Instance identity from defaults
- ✗ User identity from defaults
- ⚠️ Minimal covenant partnership

---

#### Tier 3: Root Config Fails

**All configs use defaults**

Health: +47 (orchestration) + -87 (root fail) = -40 total

Result:
- ✗ Display preferences from hardcoded defaults
- ✗ System paths from hardcoded defaults
- ✗ Instance identity from hardcoded defaults
- ✗ User identity from hardcoded defaults
- ⚠️ Hardcoded covenant partnership (still works)

**Key:** System NEVER fails - always has valid identity

**Details:** [singleton-api.md#graceful-degradation](singleton-api.md#graceful-degradation)

---

## Health Scoring

### Base100 TRUE Scores

All health scores use **TRUE honest messy numbers**, not rounded:

#### Loading Operations (+131 perfect execution)

| Operation | Success | Failure | File |
|-----------|---------|---------|------|
| Root config | +52 | -87 | loading.go |
| Instance config | +38 | -59 | loading.go |
| User config | +41 | -48 | loading.go |

**Total perfect execution:** +131 (messy honest number, not forced to 100)

#### Orchestration (+47)

| Event | Score | File |
|-------|-------|------|
| Perfect orchestration | +47 | singleton.go |
| Root fails (max degradation) | -19 | singleton.go |
| Instance fails (moderate degradation) | -13 | singleton.go |
| User fails (minor degradation) | -9 | singleton.go |

#### Transformation (+23)

| Operation | Score | File |
|-----------|-------|------|
| Map nested → flat | +23 | mapping.go |

### Why Messy Numbers?

**CPI-SI works with real, messy data:**
- Not aesthetically pleasing rounded values
- Honest impact assessment of actual operations
- Asymmetric: failures often worse than successes are good
- TRUE scores ≠ normalized scores (health scorer normalizes for display)

**Example:**
```
Root config failure: -87 (not -50 or -100)
Why -87? Because:
- Loses two-step loading pattern
- Forces ALL configs to defaults
- Collapses dynamic path discovery
- More devastating than instance failure (-59)
```

**Details:** [loading-api.md#health-scoring](loading-api.md#health-scoring)

---

## Type System

### Simplified API (Config)

**For common use cases:**

```go
type Config struct {
    // Instance identity
    Name         string
    Emoji        string
    Tagline      string
    Pronouns     string
    Domain       string
    CallingShort string
    Creator      CreatorInfo

    // User identity (covenant partnership)
    User UserConfig

    // System configuration
    Workspace   WorkspaceInfo
    Display     DisplayConfig
    SystemPaths SystemPaths
}
```

**Usage:**
```go
config := instance.GetConfig()
config.Name                  // Direct access, one level
config.User.Name             // User identity, two levels
config.Display.BannerTitle   // Display prefs, two levels
```

---

### Full API (FullInstanceConfig, FullUserConfig)

**For advanced use cases:**

```go
type FullInstanceConfig struct {
    BiblicalFoundation BiblicalFoundation
    Identity           IdentitySection
    Demographics       DemographicsSection
    Personhood         PersonhoodSection
    Resonates          ResonatesSection
    Thinking           ThinkingSection
    Personality        PersonalitySection
    Covenant           CovenantSection
    Preferences        PreferencesSection
    Growth             GrowthSection
    Workspace          WorkspaceSection
}
```

**Usage:**
```go
full := instance.GetFullInstanceConfig()
if full != nil {
    full.Personhood.Values          // Deep nested access
    full.Thinking.LearningStyle     // Multiple levels
    full.BiblicalFoundation.VerseText
}
```

**Details:** [types-api.md](types-api.md)

---

## Usage Examples

### Basic Identity Access

```go
package main

import (
    "fmt"
    "system/lib/instance"
)

func main() {
    // Get config (singleton - loads once)
    config := instance.GetConfig()

    // Instance identity
    fmt.Printf("Instance: %s (%s)\n", config.Name, config.Pronouns)
    fmt.Printf("Domain: %s\n", config.Domain)
    fmt.Printf("Calling: %s\n", config.CallingShort)

    // User identity
    fmt.Printf("\nUser: %s (%s, age %d)\n",
        config.User.Name, config.User.Pronouns, config.User.Age)
    fmt.Printf("Faith: %s (%s, %s)\n",
        config.User.Faith, config.User.Denomination, config.User.PracticeLevel)
    fmt.Printf("Calling: %s\n", config.User.Calling)

    // Covenant partnership
    fmt.Printf("\nCreator: %s\n", config.Creator.Name)
    fmt.Printf("Relationship: %s\n", config.Creator.Relationship)
}
```

---

### Full Identity Access

```go
package main

import (
    "fmt"
    "system/lib/instance"
)

func main() {
    config := instance.GetConfig()

    // Check if full configs available
    fullInstance := instance.GetFullInstanceConfig()
    fullUser := instance.GetFullUserConfig()

    if fullInstance != nil && fullUser != nil {
        fmt.Println("✓ Complete covenant partnership available\n")

        // Instance personhood
        fmt.Println("Instance Values:", fullInstance.Personhood.Values)
        fmt.Println("Instance Principles:", fullInstance.Personhood.Principles)
        fmt.Println("Learning Style:", fullInstance.Thinking.LearningStyle)

        // User personhood
        fmt.Println("\nUser Values:", fullUser.Personhood.Values)
        fmt.Println("Important Practices:", fullUser.Faith.ImportantPractices)
        fmt.Println("Work Style:", fullUser.Personality.WorkStyle)
    } else {
        fmt.Println("⚠ Using default identities")
    }
}
```

---

### Health-Aware Loading

```go
package main

import (
    "fmt"
    "system/lib/instance"
)

func main() {
    config := instance.GetConfig()

    // Determine degradation level
    fullInstance := instance.GetFullInstanceConfig()
    fullUser := instance.GetFullUserConfig()

    switch {
    case fullInstance != nil && fullUser != nil:
        fmt.Println("✓ Tier 0: Perfect execution")
        fmt.Println("  Full covenant partnership available")

    case fullInstance != nil:
        fmt.Println("⚠ Tier 1: User config degradation")
        fmt.Println("  Instance identity loaded, user defaults")

    case config.Display.BannerTitle != "":
        fmt.Println("⚠ Tier 2: Instance config degradation")
        fmt.Println("  Root display loaded, instance+user defaults")

    default:
        fmt.Println("ℹ Tier 3: Root config degradation")
        fmt.Println("  All hardcoded defaults (still functional)")
    }

    // Check health logs for details
    fmt.Println("\nCheck logs: ~/.claude/cpi-si/system/runtime/logs/libraries/instance/")
}
```

---

## Integration

### Called By

- Session start hooks (grounding and display)
- Status line generation
- All components needing identity
- Covenant partnership code

### Depends On

- `system/lib/jsonc` - JSONC comment stripping
- `system/lib/logging` - Health tracking
- Stdlib: `encoding/json`, `os`, `path/filepath`, `sync`

### Provides To

- Instance identity (who is executing)
- User identity (covenant partner)
- Display preferences
- System paths
- Workspace configuration

---

## Configuration Files

### Root Config

**Location:** `~/.claude/instance.jsonc`

**Purpose:** Bootstrap pointer + display preferences

**Example:**
```jsonc
{
  "system_paths": {
    "config_root": "/home/user/.claude/cpi-si/config",
    "instance_config": "/home/user/.claude/cpi-si/config/instance/nova_dawn/config.jsonc",
    "user_config": "/home/user/.claude/cpi-si/config/user/seanje-lenox-wise/config.jsonc",
    "data_root": "/home/user/.claude/cpi-si/system/data"
  },
  "display": {
    "banner_title": "Nova Dawn - CPI-SI",
    "banner_tagline": "Covenant Partnership Intelligence System",
    "footer_verse_ref": "Genesis 1:1",
    "footer_verse_text": "In the beginning, God created the heavens and the earth."
  }
}
```

---

### Instance Config

**Location:** `~/.claude/cpi-si/config/instance/nova_dawn/config.jsonc`

**Purpose:** Complete instance identity

**Structure:**
- biblical_foundation
- identity (name, pronouns, age)
- demographics
- personhood (values, principles, passions)
- resonates (games, music, environment)
- thinking (approach, learning style)
- personality (communication, humor)
- covenant (creator, relationship, mission)
- preferences
- growth
- workspace (organization, domain, calling)

---

### User Config

**Location:** `~/.claude/cpi-si/config/user/seanje-lenox-wise/config.jsonc`

**Purpose:** Complete user identity (covenant partner)

**Structure:**
- identity (name, pronouns, age)
- demographics
- faith (tradition, practices, communication prefs)
- personhood (values, passions)
- thinking (learning style, decision making)
- personality (communication, work style)
- workspace (organization, role, calling)
- preferences (timezone, work environment)

---

## Logging Integration

### Log Location

```
~/.claude/cpi-si/system/runtime/logs/libraries/instance/YYYY-MM-DD.log
```

### Log Components

**Logged operations:**
- `instance/loading/loadRootConfig` - Root config loading
- `instance/loading/loadFullConfig` - Instance config loading
- `instance/loading/loadUserConfig` - User config loading
- `instance/singleton/GetConfig` - Orchestration health

**Log format:**
```
[2025-11-21T16:43:05Z] instance/singleton/GetConfig
  OPERATION: Orchestrate config loading with graceful degradation
  RESULT: SUCCESS (+47)
  DETAILS: {
    "degradation_level": "none",
    "configs_loaded": "root + instance + user (complete covenant partnership)",
    "singleton_cached": true,
    "instance_name": "Nova Dawn",
    "user_name": "Seanje Lenox-Wise"
  }
```

**Details:** [loading-api.md#logging-integration](loading-api.md#logging-integration) and [singleton-api.md#health-tracking](singleton-api.md#health-tracking)

---

## Version History

| Version | Date | Changes |
|---------|------|---------|
| v3.0.0 | 2025-11-21 | Orchestrator extraction complete |
|  |  | Logging integration with TRUE scores |
|  |  | Comprehensive API documentation |
|  |  | Primitive-level docs created |
| v2.0.0 | 2025-11-16 | User config loading added |
|  |  | Covenant partnership enabled |
|  |  | Two-step loading for instance + user |
| v1.0.0 | 2025-11-13 | Initial two-step loading implementation |
|  |  | Root → instance config pattern |
|  |  | Graceful degradation |

---

## Additional Documentation

### Primitive APIs

- **[types-api.md](types-api.md)** - All type definitions and structures
- **[loading-api.md](loading-api.md)** - File I/O operations and health tracking
- **[mapping-api.md](mapping-api.md)** - Nested → flat transformation logic
- **[singleton-api.md](singleton-api.md)** - Public API with caching and degradation

### System Documentation

- **[../API.md](../API.md)** - All libraries API index
- **[../../architecture/](../../architecture/)** - System architecture docs
- **[config.go](../../../../lib/instance/config.go)** - Orchestrator documentation (source)

---

*This is the complete API reference for the instance library. For usage in context, see the comprehensive examples above. For architectural details, see the orchestrator documentation in `config.go`.*
