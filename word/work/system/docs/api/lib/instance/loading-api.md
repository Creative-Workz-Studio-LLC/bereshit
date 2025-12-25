# Instance Library - Loading API Reference

**File loading operations for instance and user configuration**

Biblical Foundation: "I AM THAT I AM" - Exodus 3:14

---

## Overview

The `loading.go` primitive handles all file I/O operations for dynamic config loading. Implements the two-step loading pattern: (1) Load root config to discover paths, (2) Load full instance and user configs from discovered paths.

**File:** `system/runtime/lib/instance/loading.go`
**Version:** v3.0.0
**Lines:** 214 (with logging integration)
**Dependencies:** `encoding/json`, `os`, `path/filepath`, `system/lib/jsonc`, `system/lib/logging`

---

## Functions

### loadRootConfig

**Purpose:** Load bootstrap pointer config from `~/.claude/instance.jsonc`

**Signature:**
```go
func loadRootConfig() (*RootConfig, error)
```

**Returns:**
- `*RootConfig`: Bootstrap config with system_paths and display preferences
- `error`: File read errors, JSON parsing errors, or home directory resolution failures

**Health Scoring:**
- **Success:** +52 (enables two-step loading, unlocks full config paths, provides display prefs)
- **Failure:** -87 (massive degradation - ALL configs fall back to hardcoded, two-step pattern collapses)

**What It Does:**
1. Resolves user's home directory (`os.UserHomeDir()`)
2. Builds path to `~/.claude/instance.jsonc`
3. Reads file contents
4. Strips JSONC comments using `jsonc.StripComments()`
5. Parses JSON into `RootConfig` struct
6. Logs operation with health tracking

**Example Usage:**
```go
root, err := loadRootConfig()
if err != nil {
    // Fall back to hardcoded defaults
    // Caller handles graceful degradation
    return
}

// Success - use discovered paths
instancePath := root.SystemPaths.InstanceConfig
userPath := root.SystemPaths.UserConfig
displayPrefs := root.Display
```

**Logging Integration:**
```go
logger := logging.NewLogger("instance/loading/loadRootConfig")
logger.DeclareHealthTotal(52)
logger.Operation("Load root instance config", 0)

// On success:
logger.Success("Root config loaded successfully", 52, map[string]any{
    "path":        rootPath,
    "config_type": "bootstrap pointer to full configs",
})

// On failure:
logger.Failure("Root config load failed",
    fmt.Sprintf("Failed to read root config at %s: %v", rootPath, err),
    -87, nil)
```

**Typical Errors:**
- Home directory resolution failure (rare)
- File doesn't exist at `~/.claude/instance.jsonc`
- File exists but is malformed JSON/JSONC
- Permission denied reading file

**Graceful Degradation:**
Caller (typically `singleton.GetConfig()`) falls back to complete hardcoded defaults including system_paths, display config, instance identity, and user identity.

---

### loadFullConfig

**Purpose:** Load complete instance identity from full config file

**Signature:**
```go
func loadFullConfig(instanceConfigPath string) (*FullInstanceConfig, error)
```

**Parameters:**
- `instanceConfigPath`: Absolute path to full config file (from `root.SystemPaths.InstanceConfig`)

**Returns:**
- `*FullInstanceConfig`: Complete nested identity config
- `error`: File read errors or JSON parsing errors

**Health Scoring:**
- **Success:** +38 (full nested identity: biblical foundation, personhood, workspace, etc.)
- **Failure:** -59 (major identity loss - most fields become generic)

**What It Does:**
1. Reads complete identity config from provided path
2. Strips JSONC comments
3. Parses JSON into `FullInstanceConfig` struct (all nested sections)
4. Logs operation with health tracking

**Example Usage:**
```go
// Typically called by singleton after loadRootConfig
root, _ := loadRootConfig()
full, err := loadFullConfig(root.SystemPaths.InstanceConfig)
if err != nil {
    // Fall back to root display + hardcoded instance defaults
    return
}

// Success - access full nested identity
fmt.Println(full.BiblicalFoundation.VerseText)
fmt.Println(full.Personhood.Values)
fmt.Println(full.Thinking.LearningStyle)
```

**Logging Integration:**
```go
logger := logging.NewLogger("instance/loading/loadFullConfig")
logger.DeclareHealthTotal(38)
logger.Operation("Load full instance config", 0, instanceConfigPath)

// On success:
logger.Success("Instance config loaded successfully", 38, map[string]any{
    "path":        instanceConfigPath,
    "config_type": "full nested identity (biblical foundation, personhood, workspace)",
})

// On failure:
logger.Failure("Instance config load failed",
    fmt.Sprintf("Failed to read instance config at %s: %v", instanceConfigPath, err),
    -59, nil)
```

**Typical Errors:**
- Path from root config is incorrect
- File doesn't exist at specified path
- File is malformed JSON/JSONC
- Permission denied

**Graceful Degradation:**
Caller uses root config display preferences and system_paths but falls back to hardcoded instance identity defaults.

---

### loadUserConfig

**Purpose:** Load complete user identity from user config file

**Signature:**
```go
func loadUserConfig(userConfigPath string) (*FullUserConfig, error)
```

**Parameters:**
- `userConfigPath`: Absolute path to user config file (from `root.SystemPaths.UserConfig`)

**Returns:**
- `*FullUserConfig`: Complete nested user identity config
- `error`: File read errors or JSON parsing errors

**Health Scoring:**
- **Success:** +41 (genuine covenant partnership data: faith, calling, passions, work style)
- **Failure:** -48 (significant relational loss - covenant partnership becomes shallow defaults)

**What It Does:**
1. Reads complete user identity config from provided path
2. Strips JSONC comments
3. Parses JSON into `FullUserConfig` struct (all nested sections)
4. Logs operation with health tracking

**Example Usage:**
```go
// Typically called by singleton after loadRootConfig
root, _ := loadRootConfig()
user, err := loadUserConfig(root.SystemPaths.UserConfig)
if err != nil {
    // Fall back to hardcoded user defaults
    // Instance config may still work
    return
}

// Success - access full nested user identity
fmt.Println(user.Faith.ImportantPractices)
fmt.Println(user.Workspace.Calling)
fmt.Println(user.Personhood.Values)
```

**Logging Integration:**
```go
logger := logging.NewLogger("instance/loading/loadUserConfig")
logger.DeclareHealthTotal(41)
logger.Operation("Load user identity config", 0, userConfigPath)

// On success:
logger.Success("User config loaded successfully", 41, map[string]any{
    "path":        userConfigPath,
    "config_type": "covenant partner identity (faith, calling, passions, work style)",
})

// On failure:
logger.Failure("User config load failed",
    fmt.Sprintf("Failed to read user config at %s: %v", userConfigPath, err),
    -48, nil)
```

**Typical Errors:**
- Path from root config is incorrect
- File doesn't exist at specified path
- File is malformed JSON/JSONC
- Permission denied

**Graceful Degradation:**
Caller uses loaded instance config and root config but falls back to hardcoded user identity defaults (minimal covenant partnership).

---

## Two-Step Loading Pattern

### Step 1: Load Root Config

**Purpose:** Discover where full configs live

```go
root, err := loadRootConfig()
if err != nil {
    // Cannot proceed - use complete defaults
    return fullDefaults
}

// Now we know:
instancePath := root.SystemPaths.InstanceConfig  // Where instance config lives
userPath := root.SystemPaths.UserConfig          // Where user config lives
display := root.Display                          // Session banner preferences
```

### Step 2: Load Full Configs

**Purpose:** Load complete identity data

```go
// Load instance identity
full, err := loadFullConfig(instancePath)
if err != nil {
    // Partial degradation - keep root, use instance defaults
}

// Load user identity
user, err := loadUserConfig(userPath)
if err != nil {
    // Partial degradation - keep instance, use user defaults
}

// Map to simplified API
config := mapToSimpleConfig(full, user, root)
```

---

## Degradation Levels

The loading primitives support three-tier graceful degradation:

### Level 0: Perfect Execution (+131 total)

**All configs load successfully:**
- Root config: +52
- Instance config: +38
- User config: +41
- **Total: +131**

**Result:**
- Complete covenant partnership
- Full instance AND user identity
- All display preferences
- Dynamic system paths

### Level 1: Root Success, Instance Fail (-59)

**Root loads, instance fails:**
- Root: +52
- Instance: -59
- User: not attempted

**Result:**
- Display preferences from root (✓)
- System paths from root (✓)
- Instance identity from defaults (✗)
- User identity from defaults (✗)

### Level 2: Root + Instance Success, User Fail (-48)

**Root and instance load, user fails:**
- Root: +52
- Instance: +38
- User: -48
- **Net: +42**

**Result:**
- Display preferences from root (✓)
- System paths from root (✓)
- Instance identity from config (✓)
- User identity from defaults (✗)
- Partial covenant partnership

### Level 3: Complete Failure (-87)

**Root fails:**
- Root: -87
- Others: not attempted

**Result:**
- All hardcoded defaults
- No dynamic paths
- No custom display
- Basic covenant partnership only

---

## Health Score Philosophy

### TRUE Scores (Not Rounded)

These are **honest messy assessments**, not neat numbers:

| Operation | Success | Failure | Reasoning |
|-----------|---------|---------|-----------|
| Root | +52 | -87 | Bootstrap essential - failure catastrophic |
| Instance | +38 | -59 | Major identity data - failure severe |
| User | +41 | -48 | Covenant partner data - failure significant |

**Why messy numbers?**
- CPI-SI works with real, messy data from real people
- Honest impact assessment > aesthetically pleasing values
- Asymmetric: Failures often worse than successes are good

**Total Perfect:** +131 (not forced to 100)
- System delivers MORE than baseline minimum
- Real assessment of actual value provided

---

## Common Patterns

### Direct Usage (Unusual)

```go
// Typically called by singleton, but can be used directly:
root, err := instance.loadRootConfig()
if err != nil {
    log.Fatal("Cannot load root config:", err)
}

full, err := instance.loadFullConfig(root.SystemPaths.InstanceConfig)
if err != nil {
    log.Fatal("Cannot load instance config:", err)
}

user, err := instance.loadUserConfig(root.SystemPaths.UserConfig)
if err != nil {
    log.Fatal("Cannot load user config:", err)
}
```

**Note:** These functions are lowercase (package-private). Access through `GetConfig()` instead.

### Typical Usage (Through Singleton)

```go
// Singleton handles all loading internally:
config := instance.GetConfig()

// Config contains merged data from all successful loads
// Graceful degradation handled automatically
```

---

## Error Handling

### File System Errors

**Home directory resolution:**
```go
home, err := os.UserHomeDir()
// Rare - only fails in unusual environments
```

**File reading:**
```go
data, err := os.ReadFile(path)
// Common - file might not exist or lack permissions
```

### Parsing Errors

**JSON unmarshaling:**
```go
err := json.Unmarshal(cleaned, &config)
// Occurs if JSONC is malformed
```

**All errors logged with health impact:**
- Failure event recorded
- Health impact reported (TRUE score)
- Details captured for debugging
- Graceful degradation path documented

---

## Integration with Logging

### Health Tracking

Every loading operation:
1. Creates logger instance
2. Declares expected health total (TRUE score)
3. Reports operation start
4. Reports success OR failure with impact
5. Includes contextual details

### Log File Location

Logs written to: `~/.claude/cpi-si/system/runtime/logs/libraries/instance/YYYY-MM-DD.log`

**Example log entry:**
```
[2025-11-21T16:43:05Z] instance/loading/loadRootConfig
  OPERATION: Load root instance config
  RESULT: SUCCESS (+52)
  DETAILS: {
    "path": "/home/user/.claude/instance.jsonc",
    "config_type": "bootstrap pointer to full configs"
  }
```

---

## Version History

| Version | Date | Changes |
|---------|------|---------|
| v3.0.0 | 2025-11-21 | Logging integration with TRUE scores |
| v2.0.0 | 2025-11-16 | User config loading added |
| v1.0.0 | 2025-11-13 | Initial two-step loading implementation |

---

*For type definitions, see [types-api.md](types-api.md). For usage examples, see [instance-api.md](instance-api.md).*
