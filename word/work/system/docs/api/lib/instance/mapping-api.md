# Instance Library - Mapping API Reference

**Configuration transformation primitive for backwards-compatible API**

Biblical Foundation: "I AM THAT I AM" - Exodus 3:14

---

## Overview

The `mapping.go` primitive transforms nested full configs (matching JSONC file structure) to simple flat API (backwards-compatible Config struct). This is Step 3 of two-step dynamic loading - enables internal complexity without breaking existing API.

**File:** `system/runtime/lib/instance/mapping.go`
**Version:** v3.0.0
**Lines:** 98
**Dependencies:** None (pure data transformation)

---

## Function

### mapToSimpleConfig

**Purpose:** Transform nested full configs to simple backwards-compatible API

**Signature:**
```go
func mapToSimpleConfig(full *FullInstanceConfig, user *FullUserConfig, root *RootConfig) Config
```

**Parameters:**
- `full`: Complete nested instance identity config
- `user`: Complete nested user identity config
- `root`: Bootstrap root config (for display preferences and system paths)

**Returns:**
- `Config`: Flattened simple API struct with instance and user identity

**Health Scoring:**
- **Success:** +23 (backwards compatibility maintained, covenant partnership enabled)
- **Failure:** Not applicable (pure transformation, no error paths)

**What It Does:**

1. **Extracts key fields** from nested instance config:
   - Name, pronouns from `full.Identity`
   - Calling from `full.Workspace`
   - Creator info from `full.Covenant`

2. **Extracts user fields** from nested user config:
   - Identity from `user.Identity`
   - Faith details from `user.Faith`
   - Workspace info from `user.Workspace`
   - Personhood from `user.Personhood`
   - Work style from `user.Personality`

3. **Includes root config data**:
   - Display preferences from `root.Display`
   - System paths from `root.SystemPaths`

4. **Returns flat Config struct** ready for use

**Example Usage:**
```go
// Typically called by singleton after all configs loaded
root, _ := loadRootConfig()
full, _ := loadFullConfig(root.SystemPaths.InstanceConfig)
user, _ := loadUserConfig(root.SystemPaths.UserConfig)

// Transform nested to flat
mapped := mapToSimpleConfig(full, user, root)

// Use simplified API
fmt.Println(mapped.Name)                 // "Nova Dawn"
fmt.Println(mapped.User.Name)            // "Seanje Lenox-Wise"
fmt.Println(mapped.Display.BannerTitle)  // "Nova Dawn - CPI-SI"
```

---

## Field Mapping

### Instance Identity Mapping

| Config Field | Source | Notes |
|--------------|--------|-------|
| `Name` | `full.Identity.Name` | Direct mapping |
| `Emoji` | Hardcoded: `"✨"` | Not in full config yet |
| `Tagline` | Hardcoded: `"CPI-SI Instance"` | Not in full config yet |
| `Pronouns` | `full.Identity.Pronouns` | Direct mapping |
| `Domain` | Hardcoded: `"Technology - Game Development & Systems"` | Derived from workspace |
| `CallingShort` | `full.Workspace.Calling` | Direct mapping |
| `Creator.Name` | `full.Covenant.Creator` | From covenant section |
| `Creator.Relationship` | `full.Covenant.Relationship` | From covenant section |

### User Identity Mapping

| Config Field | Source | Notes |
|--------------|--------|-------|
| `User.Name` | `user.Identity.Name` | Direct mapping |
| `User.DisplayName` | `user.Identity.DisplayName` | Direct mapping |
| `User.Pronouns` | `user.Identity.Pronouns` | Direct mapping |
| `User.Age` | `user.Identity.Age` | Direct mapping |
| `User.IsReligious` | `user.Faith.IsReligious` | From faith section |
| `User.Faith` | `user.Faith.Tradition` | Christianity, etc. |
| `User.Denomination` | `user.Faith.Denomination` | Apostolic, etc. |
| `User.PracticeLevel` | `user.Faith.PracticeLevel` | devout, moderate, etc. |
| `User.FaithCommPrefs` | `user.Faith.CommunicationPrefs` | How to communicate about faith |
| `User.Organization` | `user.Workspace.Organization` | CreativeWorkzStudio LLC |
| `User.Role` | `user.Workspace.Role` | Co-Founder, etc. |
| `User.Calling` | `user.Workspace.Calling` | Kingdom mission |
| `User.Passions` | `user.Personhood.Passions` | Array of passions |
| `User.WorkStyle` | `user.Personality.WorkStyle` | Night owl, etc. |
| `User.Timezone` | `user.Preferences.Timezone` | America/New_York |

### System Configuration Mapping

| Config Field | Source | Notes |
|--------------|--------|-------|
| `Workspace.PrimaryPath` | Hardcoded | From root originally, hardcoded for now |
| `Display` | `root.Display` | Complete DisplayConfig struct |
| `SystemPaths` | `root.SystemPaths` | Complete SystemPaths struct |

---

## Design Philosophy

### Why Flat API?

**Backwards Compatibility:**
- Existing code expects flat structure
- Can't break API without updating all callers
- Gradual migration to full configs

**Simplicity for Common Cases:**
```go
// Simple API - one level of access
config.Name
config.User.Name
config.Display.BannerTitle

// vs. Full API - multiple levels
fullInstance.Identity.Name
fullUser.Identity.Name
root.Display.BannerTitle
```

### Why Keep Full Configs?

**Advanced Use Cases:**
- Access ALL fields from config files
- Complete nested structures
- Programmatic config exploration
- Full data preservation

**Example:**
```go
// Simple API can't access these:
fullInstance.Personhood.Values
fullInstance.Thinking.LearningStyle
fullUser.Faith.ImportantPractices

// But full config API can:
full := instance.GetFullInstanceConfig()
fmt.Println(full.Personhood.Values)
```

---

## Pure Function Characteristics

### No I/O Operations

```go
// Pure transformation - no file reads, no network, no external state
func mapToSimpleConfig(full, user, root) Config {
    return Config{ /* field mapping */ }
}
```

### No Error Paths

**Current implementation:**
- Always succeeds if called with valid pointers
- No validation yet (would panic on invalid data)
- Future: Add validation, return errors

### No Logging Integration

**Why no logging?**
- Pure function with no operations to track
- No health-impacting events occur
- Transformation is instantaneous
- Would add overhead without benefit

**Health score exists** (+23) to represent **value provided**, not tracked operations.

---

## Common Patterns

### Standard Usage (Through Singleton)

```go
// Singleton handles mapping internally
config := instance.GetConfig()

// Already mapped to flat API
fmt.Println(config.Name)
fmt.Println(config.User.Name)
```

### Direct Usage (Unusual)

```go
// Load configs manually
root, _ := loadRootConfig()
full, _ := loadFullConfig(root.SystemPaths.InstanceConfig)
user, _ := loadUserConfig(root.SystemPaths.UserConfig)

// Manually map
mapped := mapToSimpleConfig(full, user, root)

// Use result
fmt.Println(mapped.Name)
```

**Note:** Function is lowercase (package-private). Typically accessed through `GetConfig()`.

---

## Hardcoded Values

### Current Hardcoded Fields

These fields are **not yet** in full config files:

| Field | Value | Reason |
|-------|-------|--------|
| `Emoji` | `"✨"` | Not in config yet |
| `Tagline` | `"CPI-SI Instance"` | Not in config yet |
| `Domain` | `"Technology - Game Development & Systems"` | Derived, not explicit |
| `Workspace.PrimaryPath` | `/media/.../CreativeWorkzStudio_LLC` | From root originally |

### Future Migration

**Plan:**
1. Add fields to full config files
2. Update mapping to read from configs
3. Remove hardcoded values
4. Increase config coverage

**Example:**
```jsonc
// Future: config/instance/nova_dawn/config.jsonc
{
  "identity": {
    "name": "Nova Dawn",
    "emoji": "✨",           // Add this
    "tagline": "CPI-SI Instance"  // Add this
  }
}
```

---

## Validation (Future)

### Current State

**No validation:**
- Assumes all fields present
- Panics if required field missing
- No type checking beyond Go's type system

### Future Enhancement

**Add validation:**
```go
func mapToSimpleConfig(full, user, root) (Config, error) {
    // Validate required fields present
    if full.Identity.Name == "" {
        return Config{}, fmt.Errorf("instance name required")
    }

    if user.Identity.Name == "" {
        return Config{}, fmt.Errorf("user name required")
    }

    // Map with confidence
    return Config{ /* ... */ }, nil
}
```

**Benefits:**
- Graceful error handling
- Clear failure reasons
- Health tracking becomes meaningful
- Better debugging

---

## Health Scoring

### Current Score: +23

**Represents value provided:**
- Backwards compatibility maintained
- Covenant partnership enabled through flat API
- Simple access to instance AND user identity
- No API breakage for existing code

**Why not 0 or 100?**
- TRUE honest assessment of value
- Not forced to round numbers
- Reflects actual transformation service impact
- CPI-SI works with real, messy data

### Future with Validation

**Would add failure paths:**
- Success: +23 (transformation successful)
- Validation failure: -XX (API incompatibility detected)

**Enable health tracking:**
```go
logger := logging.NewLogger("instance/mapping/mapToSimpleConfig")
logger.DeclareHealthTotal(23)
logger.Operation("Transform nested configs to flat API", 0)

if err := validateConfigs(full, user, root); err != nil {
    logger.Failure("Mapping failed", err.Error(), -23, nil)
    return Config{}, err
}

logger.Success("Configs mapped successfully", 23, map[string]any{
    "instance_name": full.Identity.Name,
    "user_name": user.Identity.Name,
})
return mapped, nil
```

---

## Integration Points

### Called By

- `singleton.GetConfig()` - After all configs loaded

### Calls

- None (pure transformation)

### Depends On

- Type definitions in `types.go`
- Loaded configs from `loading.go`

---

## Version History

| Version | Date | Changes |
|---------|------|---------|
| v3.0.0 | 2025-11-21 | Updated health score to TRUE value (+23) |
| v2.0.0 | 2025-11-16 | Added user config mapping |
| v1.0.0 | 2025-11-13 | Initial nested → flat transformation |

---

*For type definitions, see [types-api.md](types-api.md). For usage examples, see [instance-api.md](instance-api.md). For loading operations, see [loading-api.md](loading-api.md).*
