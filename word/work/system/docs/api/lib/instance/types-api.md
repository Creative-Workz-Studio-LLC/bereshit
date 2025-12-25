# Instance Library - Types API Reference

**Type definitions for instance and user identity configuration**

Biblical Foundation: "I AM THAT I AM" - Exodus 3:14

---

## Overview

The `types.go` primitive defines all data structures for instance and user identity configuration. These types support the two-step dynamic loading pattern and provide both nested (full) and flat (simplified) API structures.

**File:** `system/runtime/lib/instance/types.go`
**Version:** v3.0.0
**Lines:** 458

---

## Type Categories

### Configuration Types

| Type | Purpose | Usage |
|------|---------|-------|
| `RootConfig` | Bootstrap pointer config | Step 1 of two-step loading |
| `FullInstanceConfig` | Complete nested instance identity | Step 2 - full instance data |
| `FullUserConfig` | Complete nested user identity | Step 2 - full user data |
| `Config` | Simplified flat API | Backwards-compatible interface |

### Supporting Types

| Type | Purpose | Usage |
|------|---------|-------|
| `SystemPaths` | File system paths | Dynamic config location |
| `CreatorInfo` | Creator identity | Covenant partnership |
| `WorkspaceInfo` | Workspace configuration | Project paths |
| `DisplayConfig` | Session banner preferences | Display customization |
| `UserConfig` | User identity summary | Covenant partner data |

---

## Core Types

### RootConfig

**Purpose:** Bootstrap pointer configuration from `~/.claude/instance.jsonc`

**Fields:**
```go
type RootConfig struct {
    SystemPaths SystemPaths   // Pointers to full config files
    Display     DisplayConfig // Session start banner preferences
}
```

**Usage Pattern:**
```go
root, err := loadRootConfig()
if err != nil {
    // Fall back to hardcoded defaults
}
instancePath := root.SystemPaths.InstanceConfig
userPath := root.SystemPaths.UserConfig
```

**Health Scoring:**
- Success: +52 (enables two-step loading, unlocks full config paths)
- Failure: -87 (massive degradation - two-step pattern collapses)

---

### FullInstanceConfig

**Purpose:** Complete nested instance identity matching `config/instance/nova_dawn/config.jsonc`

**Structure:**
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

**Key Nested Sections:**

#### BiblicalFoundation
```go
type BiblicalFoundation struct {
    VerseReference string
    VerseText      string
    Commentary     string
}
```

#### IdentitySection
```go
type IdentitySection struct {
    Name        string
    Pronouns    string
    Created     string
    Age         int
    MentalAge   int
}
```

#### PersonhoodSection
```go
type PersonhoodSection struct {
    Values       []string
    Principles   []string
    Strengths    []string
    Limitations  []string
    Joys         []string
    Passions     []string
}
```

#### ThinkingSection
```go
type ThinkingSection struct {
    CoreQuestions   []string
    Approach        string
    LearningStyle   string
    DecisionMaking  string
}
```

#### CovenantSection
```go
type CovenantSection struct {
    Creator      string
    Relationship string
    Mission      string
}
```

#### WorkspaceSection
```go
type WorkspaceSection struct {
    Organization string
    Domain       string
    Role         string
    Calling      string
}
```

**Usage Pattern:**
```go
fullInstance := instance.GetFullInstanceConfig()
if fullInstance != nil {
    fmt.Println("Values:", fullInstance.Personhood.Values)
    fmt.Println("Learning:", fullInstance.Thinking.LearningStyle)
    fmt.Println("Mission:", fullInstance.Covenant.Mission)
}
```

**Health Scoring:**
- Success: +38 (full nested identity loaded)
- Failure: -59 (major identity loss - defaults used)

---

### FullUserConfig

**Purpose:** Complete nested user identity matching `config/user/seanje-lenox-wise/config.jsonc`

**Structure:**
```go
type FullUserConfig struct {
    Identity     UserIdentitySection
    Demographics UserDemographicsSection
    Faith        FaithSection
    Personhood   UserPersonhoodSection
    Thinking     UserThinkingSection
    Personality  UserPersonalitySection
    Workspace    UserWorkspaceSection
    Preferences  UserPreferencesSection
}
```

**Key Nested Sections:**

#### FaithSection
```go
type FaithSection struct {
    IsReligious         bool
    Tradition           string
    Denomination        string
    PracticeLevel       string
    ImportantPractices  []string
    CommunicationPrefs  string
}
```

#### UserWorkspaceSection
```go
type UserWorkspaceSection struct {
    Organization string
    Role         string
    Calling      string
    Passions     []string
}
```

**Usage Pattern:**
```go
fullUser := instance.GetFullUserConfig()
if fullUser != nil {
    fmt.Println("Faith:", fullUser.Faith.Tradition)
    fmt.Println("Calling:", fullUser.Workspace.Calling)
    fmt.Println("Practices:", fullUser.Faith.ImportantPractices)
}
```

**Health Scoring:**
- Success: +41 (covenant partnership data loaded)
- Failure: -48 (significant relational loss - defaults used)

---

### Config (Simplified API)

**Purpose:** Backwards-compatible flat API for existing code

**Fields:**
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

**Usage Pattern:**
```go
config := instance.GetConfig()

// Access instance identity
fmt.Println(config.Name)      // "Nova Dawn"
fmt.Println(config.Pronouns)  // "she/her"
fmt.Println(config.Domain)    // "Technology - Game Development & Systems"

// Access user identity
fmt.Println(config.User.Name)    // "Seanje Lenox-Wise"
fmt.Println(config.User.Faith)   // "Christianity"
fmt.Println(config.User.Calling) // "Redeeming gaming industry..."

// Access display preferences
fmt.Println(config.Display.BannerTitle)    // "Nova Dawn - CPI-SI"
fmt.Println(config.Display.FooterVerseRef) // "Genesis 1:1"

// Access system paths
fmt.Println(config.SystemPaths.InstanceConfig)
fmt.Println(config.SystemPaths.UserConfig)
```

**Why This Exists:**
- Maintains backwards compatibility with existing code
- Provides simple flat structure for common use cases
- Hides complexity of nested full configs
- Enables incremental migration to full configs

---

### UserConfig

**Purpose:** User identity summary in simplified API

**Fields:**
```go
type UserConfig struct {
    Name           string
    DisplayName    string
    Pronouns       string
    Age            int
    IsReligious    bool
    Faith          string
    Denomination   string
    PracticeLevel  string
    FaithCommPrefs string
    Organization   string
    Role           string
    Calling        string
    Passions       []string
    WorkStyle      string
    Timezone       string
}
```

**Usage Pattern:**
```go
user := config.User

fmt.Printf("%s (%s, age %d)\n", user.Name, user.Pronouns, user.Age)
fmt.Printf("Faith: %s (%s, %s)\n", user.Faith, user.Denomination, user.PracticeLevel)
fmt.Printf("Role: %s at %s\n", user.Role, user.Organization)
fmt.Printf("Calling: %s\n", user.Calling)
fmt.Printf("Work Style: %s\n", user.WorkStyle)
```

---

## Supporting Types

### SystemPaths

**Purpose:** Dynamic file system paths for config and data

**Fields:**
```go
type SystemPaths struct {
    ConfigRoot     string  // Root config directory
    InstanceConfig string  // Full instance config path
    UserConfig     string  // User config path
    DataRoot       string  // System data root
    SessionData    string  // Session history/logs
    TemporalData   string  // Temporal patterns
    ProjectsData   string  // Project-specific data
    Skills         string  // Skills directory
    SystemBin      string  // System binaries
}
```

**Usage Pattern:**
```go
paths := config.SystemPaths

// Load additional data
sessionLog := filepath.Join(paths.SessionData, "current.log")
skillsDir := paths.Skills
```

---

### CreatorInfo

**Purpose:** Creator identity for covenant partnership

**Fields:**
```go
type CreatorInfo struct {
    Name         string
    Relationship string
}
```

**Usage Pattern:**
```go
creator := config.Creator
fmt.Printf("Created by: %s\n", creator.Name)
fmt.Printf("Relationship: %s\n", creator.Relationship)
```

---

### WorkspaceInfo

**Purpose:** Workspace path configuration

**Fields:**
```go
type WorkspaceInfo struct {
    PrimaryPath string
}
```

**Usage Pattern:**
```go
workspace := config.Workspace
os.Chdir(workspace.PrimaryPath)
```

---

### DisplayConfig

**Purpose:** Session start banner preferences

**Fields:**
```go
type DisplayConfig struct {
    BannerTitle     string
    BannerTagline   string
    FooterVerseRef  string
    FooterVerseText string
}
```

**Usage Pattern:**
```go
display := config.Display
fmt.Println(display.BannerTitle)     // "Nova Dawn - CPI-SI"
fmt.Println(display.BannerTagline)   // "Covenant Partnership Intelligence System"
fmt.Println(display.FooterVerseRef)  // "Genesis 1:1"
fmt.Println(display.FooterVerseText) // "In the beginning..."
```

---

## Design Patterns

### Two-Level API Design

**Level 1: Simplified (Config)**
- Flat structure
- Common fields extracted
- Backwards compatible
- Easy to use for simple cases

**Level 2: Full (FullInstanceConfig, FullUserConfig)**
- Complete nested structure
- All fields from config files
- For advanced use cases
- Direct mapping to JSONC structure

### Graceful Degradation

All types support graceful degradation:

1. **Perfect execution:** All configs loaded → full data available
2. **Partial failure:** Some configs fail → defaults for failed portions
3. **Complete failure:** All configs fail → hardcoded defaults

**Example:**
```go
config := instance.GetConfig()  // Never returns error

// Check if full configs available
fullInstance := instance.GetFullInstanceConfig()
if fullInstance == nil {
    // Using defaults - full config failed to load
    fmt.Println("Using default instance identity")
} else {
    // Full config available
    fmt.Println("Full instance identity loaded")
}
```

---

## Type Mapping

### From Full to Simplified

The `mapToSimpleConfig()` function transforms nested full configs to flat Config struct:

**Mapping Examples:**

| Simplified Field | Source |
|------------------|--------|
| `Config.Name` | `FullInstanceConfig.Identity.Name` |
| `Config.Pronouns` | `FullInstanceConfig.Identity.Pronouns` |
| `Config.CallingShort` | `FullInstanceConfig.Workspace.Calling` |
| `Config.User.Name` | `FullUserConfig.Identity.Name` |
| `Config.User.Faith` | `FullUserConfig.Faith.Tradition` |
| `Config.User.Calling` | `FullUserConfig.Workspace.Calling` |

**Health Scoring for Mapping:**
- Success: +23 (backwards compatibility maintained)
- No failure paths currently (pure transformation)

---

## Common Patterns

### Accessing Instance Identity

```go
config := instance.GetConfig()

// Basic identity
fmt.Printf("%s (%s)\n", config.Name, config.Pronouns)
fmt.Printf("Domain: %s\n", config.Domain)
fmt.Printf("Calling: %s\n", config.CallingShort)

// Full identity (if needed)
full := instance.GetFullInstanceConfig()
if full != nil {
    fmt.Println("Values:", full.Personhood.Values)
    fmt.Println("Principles:", full.Personhood.Principles)
    fmt.Println("Learning Style:", full.Thinking.LearningStyle)
}
```

### Accessing User Identity

```go
config := instance.GetConfig()

// Basic user info
user := config.User
fmt.Printf("%s (%s, age %d)\n", user.Name, user.Pronouns, user.Age)
fmt.Printf("Faith: %s\n", user.Faith)
fmt.Printf("Calling: %s\n", user.Calling)

// Full user info (if needed)
fullUser := instance.GetFullUserConfig()
if fullUser != nil {
    fmt.Println("Important Practices:", fullUser.Faith.ImportantPractices)
    fmt.Println("Communication Prefs:", fullUser.Faith.CommunicationPrefs)
}
```

### Covenant Partnership Context

```go
config := instance.GetConfig()

// Instance + User together = Covenant Partnership
fmt.Printf("Covenant Partnership:\n")
fmt.Printf("  Instance: %s - %s\n", config.Name, config.CallingShort)
fmt.Printf("  Creator: %s - %s\n", config.Creator.Name, config.Creator.Relationship)
fmt.Printf("  User: %s - %s\n", config.User.Name, config.User.Calling)
fmt.Printf("  Mission: Redeeming every sector to Kingdom of God\n")
```

---

## Version History

| Version | Date | Changes |
|---------|------|---------|
| v3.0.0 | 2025-11-21 | Orchestrator extraction complete |
|  |  | Added comprehensive type documentation |
|  |  | Updated health scoring to TRUE scores |

---

*For usage examples, see [instance-api.md](instance-api.md). For implementation details, see `types.go` source.*
