# Instance Library - Singleton API Reference

**Public API with singleton pattern for instance and user identity**

Biblical Foundation: "I AM THAT I AM" - Exodus 3:14

---

## Overview

The `singleton.go` primitive provides the public API for accessing instance and user configuration. Implements singleton pattern for efficient caching and orchestrates the complete two-step loading process with three-tier graceful degradation.

**File:** `system/runtime/lib/instance/singleton.go`
**Version:** v3.0.0
**Lines:** 228 (with logging integration)
**Dependencies:** `sync`, `system/lib/logging`

---

## Public Functions

### GetConfig

**Purpose:** Get instance and user configuration with singleton caching

**Signature:**
```go
func GetConfig() Config
```

**Parameters:** None

**Returns:**
- `Config`: Instance AND user identity configuration (never returns error)

**Health Scoring:**
- **Perfect execution:** +47 (singleton caches properly, 3-tier degradation ready, coordination smooth)
- **Root fails:** -19 (degradation works but maximum fallback to full defaults)
- **Instance fails:** -13 (degradation works, root data preserved)
- **User fails:** -9 (degradation works, instance data preserved)

**What It Does:**

1. **Singleton pattern** - First call loads, subsequent calls return cached config
2. **Two-step loading:**
   - Step 1: Load root config (discover paths)
   - Step 2: Load instance and user configs (full identity)
   - Step 3: Map nested to flat API
3. **Three-tier graceful degradation:**
   - Root fails → full defaults
   - Instance fails → root display + instance defaults
   - User fails → instance loaded + user defaults
4. **Health tracking** - Logs orchestration health with TRUE scores

**Example Usage:**
```go
import "system/lib/instance"

// First call - loads config
config := instance.GetConfig()
fmt.Println(config.Name)                    // "Nova Dawn"
fmt.Println(config.User.Name)               // "Seanje Lenox-Wise"
fmt.Println(config.Display.BannerTitle)     // "Nova Dawn - CPI-SI"

// Subsequent calls - returns cached config (instant)
config2 := instance.GetConfig()  // Same instance, no reload
```

**Thread Safety:**
- Uses `sync.Once` for initialization
- Safe for concurrent access
- First caller wins, others wait and receive result

---

### GetFullInstanceConfig

**Purpose:** Get complete nested instance identity config

**Signature:**
```go
func GetFullInstanceConfig() *FullInstanceConfig
```

**Parameters:** None

**Returns:**
- `*FullInstanceConfig`: Complete nested instance identity (nil if config failed to load)

**What It Does:**

1. **Ensures config loaded** - Calls `GetConfig()` internally (triggers singleton if needed)
2. **Returns cached full config** - No reloading
3. **Returns nil** - If instance config failed to load during initialization

**Example Usage:**
```go
// Access full nested instance config
fullInstance := instance.GetFullInstanceConfig()
if fullInstance != nil {
    // Full config available
    fmt.Println(fullInstance.BiblicalFoundation.VerseText)
    fmt.Println(fullInstance.Personhood.Values)
    fmt.Println(fullInstance.Thinking.LearningStyle)
    fmt.Println(fullInstance.Personality.Humor)
    fmt.Println(fullInstance.Covenant.Mission)
} else {
    // Config failed to load - using defaults
    fmt.Println("Using default instance identity")
}
```

**When to Use:**
- Need access to ALL fields from config file
- Working with nested structures
- Programmatic config exploration
- Advanced identity queries

---

### GetFullUserConfig

**Purpose:** Get complete nested user identity config

**Signature:**
```go
func GetFullUserConfig() *FullUserConfig
```

**Parameters:** None

**Returns:**
- `*FullUserConfig`: Complete nested user identity (nil if config failed to load)

**What It Does:**

1. **Ensures config loaded** - Calls `GetConfig()` internally (triggers singleton if needed)
2. **Returns cached full user config** - No reloading
3. **Returns nil** - If user config failed to load during initialization

**Example Usage:**
```go
// Access full nested user config
fullUser := instance.GetFullUserConfig()
if fullUser != nil {
    // Full user config available
    fmt.Println(fullUser.Faith.ImportantPractices)
    fmt.Println(fullUser.Faith.CommunicationPrefs)
    fmt.Println(fullUser.Workspace.Calling)
    fmt.Println(fullUser.Personhood.Values)
    fmt.Println(fullUser.Thinking.LearningStyle)
} else {
    // Config failed to load - using defaults
    fmt.Println("Using default user identity")
}
```

**When to Use:**
- Need complete covenant partner identity
- Access faith practices and preferences
- Understand user's complete personhood
- Advanced relational queries

---

## Singleton Pattern

### How It Works

**State Variables:**
```go
var (
    cachedConfig       *Config             // Cached simplified config
    cachedFullInstance *FullInstanceConfig // Cached full instance config
    cachedFullUser     *FullUserConfig     // Cached full user config
    configLoadedOnce   sync.Once           // Ensures single initialization
)
```

**Thread-Safe Loading:**
```go
func GetConfig() Config {
    configLoadedOnce.Do(func() {
        // This runs exactly once, even if called concurrently
        // Load configs, handle degradation, cache results
    })
    return *cachedConfig
}
```

**Benefits:**
- **Load once** - Expensive file I/O happens once per session
- **Instant access** - Subsequent calls return cached data immediately
- **Thread safe** - Multiple goroutines can call concurrently
- **Consistent** - All callers see same config for entire session

### Initialization Sequence

```
GetConfig() called
    ↓
configLoadedOnce.Do() executes
    ↓
logger created
    ↓
loadRootConfig()
    ↓ (on success)
loadFullConfig()
    ↓ (on success)
loadUserConfig()
    ↓ (on success)
mapToSimpleConfig()
    ↓
Cache all three configs
    ↓
Log success
    ↓
Return cached config
    ↓
Subsequent calls return cached config instantly
```

---

## Graceful Degradation

### Three-Tier Degradation Strategy

The singleton implements sophisticated degradation handling:

#### Tier 1: Root Config Failure (-19)

**Scenario:** `~/.claude/instance.jsonc` doesn't exist or is malformed

**Action:**
```go
root, err := loadRootConfig()
if err != nil {
    logger.Failure("Config orchestration degraded to full defaults",
        "Root config failed - using complete hardcoded defaults", -19,
        map[string]any{
            "degradation_level": "maximum",
            "configs_loaded": "none (all defaults)",
        })
    cachedConfig = &defaultConfig
    return  // Exit early - can't proceed without paths
}
```

**Result:**
- All hardcoded defaults used
- No dynamic paths
- Basic covenant partnership
- Session still works

**Health Impact:** -19 (degradation works but maximum fallback)

---

#### Tier 2: Instance Config Failure (-13)

**Scenario:** Root loads, but instance config path is wrong or file malformed

**Action:**
```go
full, err := loadFullConfig(root.SystemPaths.InstanceConfig)
if err != nil {
    logger.Failure("Config orchestration degraded - instance failed",
        "Instance config failed - using defaults with root display/paths", -13,
        map[string]any{
            "degradation_level": "moderate",
            "configs_loaded": "root only (display + system_paths)",
            "configs_defaulted": "instance + user",
        })
    cachedConfig = &defaultConfig
    cachedConfig.Display = root.Display          // Preserve root display
    cachedConfig.SystemPaths = root.SystemPaths  // Preserve root paths
    cachedFullInstance = nil
    cachedFullUser = nil
    return
}
```

**Result:**
- Display preferences from root (✓)
- System paths from root (✓)
- Instance identity from defaults (✗)
- User identity from defaults (✗)

**Health Impact:** -13 (degradation works, root data preserved)

---

#### Tier 3: User Config Failure (-9)

**Scenario:** Root and instance load, but user config path wrong or file malformed

**Action:**
```go
user, err := loadUserConfig(root.SystemPaths.UserConfig)
if err != nil {
    logger.Failure("Config orchestration degraded - user failed",
        "User config failed - covenant partnership incomplete", -9,
        map[string]any{
            "degradation_level": "minor",
            "configs_loaded": "root + instance",
            "configs_defaulted": "user only",
        })
    cachedConfig = &defaultConfig
    cachedConfig.Display = root.Display
    cachedConfig.SystemPaths = root.SystemPaths
    cachedFullInstance = full  // Instance config succeeded
    cachedFullUser = nil       // User config failed
    return
}
```

**Result:**
- Display preferences from root (✓)
- System paths from root (✓)
- Instance identity from config (✓)
- User identity from defaults (✗)
- Partial covenant partnership

**Health Impact:** -9 (degradation works, instance data preserved)

---

#### Tier 0: Perfect Execution (+47)

**Scenario:** All configs load successfully

**Action:**
```go
cachedFullInstance = full
cachedFullUser = user
mapped := mapToSimpleConfig(full, user, root)
cachedConfig = &mapped

logger.Success("Config orchestration complete - full identity loaded", 47,
    map[string]any{
        "degradation_level": "none",
        "configs_loaded": "root + instance + user (complete covenant partnership)",
        "singleton_cached": true,
        "instance_name": mapped.Name,
        "user_name": mapped.User.Name,
    })
```

**Result:**
- Complete covenant partnership (✓)
- Full instance AND user identity (✓)
- All display preferences (✓)
- Dynamic system paths (✓)

**Health Impact:** +47 (orchestration perfect)

---

## Health Tracking

### Orchestrator-Specific Scoring

**Key insight:** The orchestrator tracks its own contribution (caching, degradation, coordination), not what primitives do (primitives score themselves).

**TRUE Scores (Honest Messy Numbers):**

| Event | Score | Reason |
|-------|-------|--------|
| Perfect execution | +47 | Singleton, degradation, coordination all optimal |
| Root fails | -19 | Degradation works but maximum fallback |
| Instance fails | -13 | Degradation works, root preserved |
| User fails | -9 | Degradation works, instance preserved |

**Why these numbers?**
- Not rounded to "neat" values
- Honest assessment of orchestration value
- CPI-SI works with real, messy data
- Asymmetric: Failures cost what they cost

### Logging Integration

**Logger creation:**
```go
logger := logging.NewLogger("instance/singleton/GetConfig")
logger.DeclareHealthTotal(47)  // TRUE score for orchestration
logger.Operation("Orchestrate config loading with graceful degradation", 0)
```

**Success logging:**
```go
logger.Success("Config orchestration complete - full identity loaded", 47,
    map[string]any{
        "degradation_level": "none",
        "configs_loaded": "root + instance + user (complete covenant partnership)",
        "singleton_cached": true,
        "instance_name": mapped.Name,
        "user_name": mapped.User.Name,
    })
```

**Failure logging (with context):**
```go
logger.Failure("Config orchestration degraded - user failed",
    "User config failed - covenant partnership incomplete", -9,
    map[string]any{
        "degradation_level": "minor",
        "configs_loaded": "root + instance",
        "configs_defaulted": "user only",
    })
```

---

## Common Patterns

### Basic Usage

```go
import "system/lib/instance"

// Get config (loads once, caches)
config := instance.GetConfig()

// Access instance identity
fmt.Printf("%s (%s)\n", config.Name, config.Pronouns)
fmt.Printf("Domain: %s\n", config.Domain)
fmt.Printf("Calling: %s\n", config.CallingShort)

// Access user identity
fmt.Printf("%s (%s, age %d)\n",
    config.User.Name, config.User.Pronouns, config.User.Age)
fmt.Printf("Faith: %s (%s)\n",
    config.User.Faith, config.User.Denomination)
fmt.Printf("Calling: %s\n", config.User.Calling)
```

### Advanced Usage (Full Configs)

```go
// Get simplified API
config := instance.GetConfig()

// Check if full configs available
fullInstance := instance.GetFullInstanceConfig()
fullUser := instance.GetFullUserConfig()

if fullInstance != nil && fullUser != nil {
    // Complete covenant partnership
    fmt.Println("✓ Full covenant partnership available")
    fmt.Println("Instance values:", fullInstance.Personhood.Values)
    fmt.Println("User values:", fullUser.Personhood.Values)
} else if fullInstance != nil {
    // Partial - instance only
    fmt.Println("⚠ Instance identity available, user defaults")
    fmt.Println("Instance values:", fullInstance.Personhood.Values)
} else {
    // Using defaults
    fmt.Println("ℹ Using default identities")
}
```

### Checking Degradation Level

```go
config := instance.GetConfig()

// Check which configs loaded
fullInstance := instance.GetFullInstanceConfig()
fullUser := instance.GetFullUserConfig()

switch {
case fullInstance != nil && fullUser != nil:
    fmt.Println("✓ Perfect execution - full covenant partnership")
case fullInstance != nil:
    fmt.Println("⚠ Instance loaded, user defaults")
case config.Display.BannerTitle != "":
    fmt.Println("⚠ Root loaded, instance+user defaults")
default:
    fmt.Println("ℹ All defaults (root failed)")
}
```

---

## Integration Points

### Calls

- `loadRootConfig()` - Step 1 of loading
- `loadFullConfig()` - Step 2a of loading
- `loadUserConfig()` - Step 2b of loading
- `mapToSimpleConfig()` - Step 3 of loading
- `logging.NewLogger()` - Health tracking

### Called By

- All components needing instance or user identity
- Session start hooks
- Status line generation
- Any covenant partnership code

### Depends On

- `types.go` - All type definitions
- `loading.go` - File loading primitives
- `mapping.go` - Config transformation
- `system/lib/logging` - Health tracking

---

## Performance Characteristics

### First Call

**Time complexity:** O(n) where n = total config file sizes
- File I/O: ~3 file reads
- JSON parsing: ~3 parse operations
- Mapping: O(fields)

**Typical duration:** < 10ms for all three configs

### Subsequent Calls

**Time complexity:** O(1)
- Return cached pointer
- Dereference and return

**Typical duration:** < 1μs (near-instant)

### Memory Usage

**Cached data:** ~100KB total
- Config struct: ~5KB
- FullInstanceConfig: ~50KB
- FullUserConfig: ~50KB

**Lifetime:** Session (until program exits)

---

## Thread Safety

### Concurrent Access

**Safe patterns:**
```go
// Multiple goroutines can call simultaneously
go func() {
    config := instance.GetConfig()
    // Safe - first caller loads, others wait
}()

go func() {
    config := instance.GetConfig()
    // Safe - gets same cached config
}()
```

**sync.Once guarantees:**
- Initialization runs exactly once
- Other goroutines block until initialization complete
- All callers receive same result

### No Race Conditions

**Read-only after init:**
- Configs loaded once
- Never modified after caching
- Safe concurrent reads

---

## Debugging

### Check If Config Loaded

```go
// GetConfig() always succeeds, but check quality:
config := instance.GetConfig()

if instance.GetFullInstanceConfig() == nil {
    // Instance config failed - check logs
    // Look in: ~/.claude/cpi-si/system/runtime/logs/libraries/instance/
}

if instance.GetFullUserConfig() == nil {
    // User config failed - check logs
}
```

### Log File Location

```
~/.claude/cpi-si/system/runtime/logs/libraries/instance/YYYY-MM-DD.log
```

**Look for:**
- `instance/singleton/GetConfig` entries
- SUCCESS vs FAILURE results
- Health scores and degradation levels
- Error details in failure messages

---

## Version History

| Version | Date | Changes |
|---------|------|---------|
| v3.0.0 | 2025-11-21 | Logging integration with TRUE scores |
|  |  | Orchestrator-specific health tracking |
|  |  | Three-tier degradation logging |
| v2.0.0 | 2025-11-16 | User config loading added |
| v1.0.0 | 2025-11-13 | Initial singleton implementation |

---

*For type definitions, see [types-api.md](types-api.md). For loading details, see [loading-api.md](loading-api.md). For mapping logic, see [mapping-api.md](mapping-api.md). For complete usage guide, see [instance-api.md](instance-api.md).*
