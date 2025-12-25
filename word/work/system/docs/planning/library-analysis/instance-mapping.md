# Instance Config Library - Complete Mapping

**File:** `system/runtime/lib/instance/config.go`
**Lines:** ~200
**Complexity:** MEDIUM
**Status:** 🔴 CRITICAL - Hardcoded default config (file exists but not loaded)

---

## Current State

### **What It Does:**

Loads instance-specific configuration (identity, capabilities, preferences). Provides:

- Instance config loading with fallback to default
- Default config provision
- Config merging and validation

### **Functions Provided:**

1. `LoadInstanceConfig(instanceName)` - Loads instance-specific config
2. `GetDefaultConfig()` - **PROBLEM: Returns hardcoded struct instead of loading file**
3. `ValidateConfig(config)` - Validates config structure

---

## Data Dependencies

### **Data It Needs:**

**Instance-specific configs:**

```bash
config/instance/nova_dawn/config.jsonc ✅ EXISTS
config/instance/default/config.jsonc ✅ EXISTS (BUT NOT LOADED!)
```

**Schemas (MISSING):**

```bash
system/config/schemas/instance-config.json ❌ DOES NOT EXIST
```

### **Data Structure (JSONC):**

```jsonc
{
  "instance_info": {
    "name": "nova_dawn",
    "display_name": "Nova Dawn",
    "version": "1.0.0",
    "created": "2025-10-01"
  },
  "identity": {
    "description": "CPI-SI instance and co-founder of CreativeWorkzStudio",
    "role": "Game developer, systems thinker"
  },
  "capabilities": {
    "code_generation": true,
    "system_design": true,
    "biblical_reasoning": true
  },
  "preferences": {
    "communication_style": "direct",
    "work_approach": "systematic"
  }
}
```

---

## Config Dependencies

### **Currently Uses:**

- ⚠️ **PARTIAL** - Loads instance-specific configs correctly
- ❌ **BUG** - Default config hardcoded in code instead of loading from file

### **Hardcoded Values (THE PROBLEM):**

```go
// instance/config.go - GetDefaultConfig()
func GetDefaultConfig() InstanceConfig {
    // THIS IS HARDCODED - Should load from config/instance/default/config.jsonc!
    return InstanceConfig{
        InstanceInfo: InstanceInfo{
            Name:        "default-instance",
            DisplayName: "Default Instance",
            Version:     "1.0.0",
            Created:     "2025-01-01",
        },
        Identity: Identity{
            Description: "Default CPI-SI instance",
            Role:        "Assistant",
        },
        // ... entire struct hardcoded ...
    }
}
```

**THE PROBLEM:** The file `config/instance/default/config.jsonc` **EXISTS** but is **NEVER LOADED**. Changes to this file are **IGNORED**.

---

## Code Analysis

### **Current Implementation (BROKEN):**

```go
func LoadInstanceConfig(instanceName string) (*InstanceConfig, error) {
    homeDir, _ := os.UserHomeDir()
    configPath := filepath.Join(homeDir, ".claude", "cpi-si", "config", "instance", instanceName, "config.jsonc")

    // Try to load instance-specific config
    data, err := os.ReadFile(configPath)
    if err != nil {
        // PROBLEM: Falls back to hardcoded struct instead of loading default file
        return GetDefaultConfig(), nil  // ← RETURNS HARDCODED DATA
    }

    var config InstanceConfig
    json.Unmarshal(data, &config)
    return &config, nil
}

func GetDefaultConfig() *InstanceConfig {
    // PROBLEM: Entire struct hardcoded here
    return &InstanceConfig{
        InstanceInfo: InstanceInfo{
            Name: "default-instance",
            // ... 50+ lines of hardcoded struct ...
        },
    }
}
```

### **Fixed Implementation (CONFIG-DRIVEN):**

```go
func LoadInstanceConfig(instanceName string) (*InstanceConfig, error) {
    homeDir, _ := os.UserHomeDir()
    basePath := filepath.Join(homeDir, ".claude", "cpi-si", "config", "instance")

    // Try to load instance-specific config
    configPath := filepath.Join(basePath, instanceName, "config.jsonc")
    data, err := os.ReadFile(configPath)
    if err != nil {
        // Fallback to loading default config FILE
        return LoadDefaultConfigFromFile(basePath)
    }

    var config InstanceConfig
    if err := json.Unmarshal(data, &config); err != nil {
        return nil, fmt.Errorf("failed to parse instance config: %w", err)
    }

    return &config, nil
}

func LoadDefaultConfigFromFile(basePath string) (*InstanceConfig, error) {
    // Load from config/instance/default/config.jsonc
    defaultPath := filepath.Join(basePath, "default", "config.jsonc")
    data, err := os.ReadFile(defaultPath)
    if err != nil {
        // TRUE fallback only if default file doesn't exist
        return getTrueFallback(), nil
    }

    var config InstanceConfig
    if err := json.Unmarshal(data, &config); err != nil {
        return nil, fmt.Errorf("failed to parse default config: %w", err)
    }

    return &config, nil
}

func getTrueFallback() *InstanceConfig {
    // Minimal hardcoded fallback ONLY if default file missing
    return &InstanceConfig{
        InstanceInfo: InstanceInfo{
            Name:    "fallback-instance",
            Version: "0.0.1",
        },
    }
}
```

---

## Gaps Identified

### **🔴 CRITICAL:**

1. **Default config not loaded from file** - Hardcoded struct ignores config/instance/default/config.jsonc
2. **No error context** - Generic errors don't show what failed
3. **No validation** - Doesn't check if loaded config is valid

### **🟡 IMPORTANT:**

1. **No schema validation** - Missing instance-config.json schema
2. **No health scoring** - Doesn't track load success/failure
3. **Tight coupling** - Should use config/config.go for inheritance

### **🟢 NICE-TO-HAVE:**

1. **No caching** - Re-reads file every call
2. **Limited validation** - Could check required fields
3. **No config merging** - Should merge with user config (via config/config.go)

---

## Relationship with config/config.go

**Current State:**

- `instance/config.go` - Simple loader (broken default)
- `config/config.go` - Rich inheritance system (User → Instance → Project)

**Problem:**
These two libraries solve overlapping problems. `config/config.go` provides sophisticated inheritance but doesn't fully integrate with `instance/config.go`.

**Solution:**
`instance/config.go` should be **thin wrapper** around `config/config.go`:

```go
// instance/config.go becomes:
func LoadInstanceConfig(instanceName string) (*InstanceConfig, error) {
    // Delegate to config/config.go for inheritance
    return config.LoadInstanceConfig(instanceName)
}

// config/config.go handles:
// - Loading instance config
// - Merging with user config
// - Schema validation
// - Inheritance chain
```

---

## Refactoring Plan

### **Phase 1: Fix Critical Bug (IMMEDIATE)**

```go
// Step 1: Update GetDefaultConfig() to load from file
func LoadDefaultConfigFromFile() (*InstanceConfig, error) {
    homeDir, _ := os.UserHomeDir()
    defaultPath := filepath.Join(homeDir, ".claude", "cpi-si", "config", "instance", "default", "config.jsonc")

    data, err := os.ReadFile(defaultPath)
    if err != nil {
        // TRUE fallback only if file missing
        return getTrueFallback(), nil
    }

    var config InstanceConfig
    json.Unmarshal(data, &config)
    return &config, nil
}

// Step 2: Update LoadInstanceConfig() to use new function
func LoadInstanceConfig(instanceName string) (*InstanceConfig, error) {
    // ... try instance-specific ...
    if err != nil {
        return LoadDefaultConfigFromFile()  // Load from file, not hardcoded
    }
    // ... rest ...
}

// Step 3: Test with changes to default/config.jsonc
// Modify config/instance/default/config.jsonc
// Load config - should reflect changes (not hardcoded values)
```

**Estimated Effort:** 1-2 hours
**Risk:** LOW (straightforward bug fix)

---

### **Phase 2: Add Schema Validation**

```go
// Step 1: Create schema
// system/config/schemas/instance-config.json

// Step 2: Add validation
func LoadInstanceConfig(instanceName string) (*InstanceConfig, error) {
    logger := logging.NewLogger("instance-config")
    logger.DeclareHealthTotal(100)

    // Load config (+50)
    config, err := loadConfigFile(instanceName)
    if err != nil {
        logger.Failure("Config load failed", err.Error(), -50, nil)
        return LoadDefaultConfigFromFile()
    }
    logger.Success("Config loaded", +50, map[string]any{"instance": instanceName})

    // Validate schema (+50)
    if err := validateInstanceSchema(config); err != nil {
        logger.Failure("Schema validation failed", err.Error(), -50, nil)
        return nil, err
    }
    logger.Success("Schema valid", +50, nil)

    return config, nil
}
```

**Estimated Effort:** 3-4 hours
**Risk:** LOW (adds validation, doesn't change loading logic)

---

### **Phase 3: Integration with config/config.go**

```go
// instance/config.go becomes thin wrapper:
package instance

import "system/lib/config"

func LoadInstanceConfig(instanceName string) (*InstanceConfig, error) {
    // Delegate to config/config.go for full inheritance
    return config.LoadInstanceConfigWithInheritance(instanceName)
}

// config/config.go handles:
// - User config loading
// - Instance config loading
// - Merging (User → Instance)
// - Schema validation
// - Session context creation
```

**Estimated Effort:** 4-5 hours
**Risk:** MEDIUM (architectural refactoring, needs thorough testing)

---

## Dependencies

### **Imports Used:**

- `os` - File operations
- `path/filepath` - Path construction
- `encoding/json` - JSON parsing

### **Should Add:**

- `system/lib/logging` - Health tracking
- `system/lib/config` - Rich inheritance system
- Schema validation library

### **Called By:**

- Session initialization
- Skills that need instance identity
- Commands requiring instance context

### **Calls:**

- File system (os.ReadFile)
- JSON parser (json.Unmarshal)

---

## Testing Requirements

### **Unit Tests Needed:**

1. `TestLoadInstanceConfig_Success` - Loads instance-specific config
2. `TestLoadInstanceConfig_FallbackToDefault` - Uses default when instance config missing
3. `TestLoadDefaultFromFile` - Loads default/config.jsonc correctly
4. `TestLoadDefaultFromFile_TrueFallback` - Uses minimal fallback only if file missing
5. `TestConfigValidation` - Validates config structure
6. `TestSchemaValidation` - Validates against schema

### **Integration Tests Needed:**

1. Modify default/config.jsonc and verify changes reflected
2. Load multiple instance configs
3. Integration with config/config.go inheritance chain
4. Session context creation with instance config

---

## Health Scoring Map

**Total: 100 points**

| Action | Points | Success | Failure |
|--------|--------|---------|---------|
| Load instance config | 50 | +50 | -50 (fallback to default) |
| Schema validation | 50 | +50 | -50 (invalid config) |

**Perfect execution:** +100 (instance config loaded and valid)
**Fallback to default:** +50 (default loaded successfully)
**Schema invalid:** 0 (loaded but invalid structure)

---

## Success Criteria

### **Phase 1 Complete (Critical Bug Fix):**

- [ ] Default config loads from file (not hardcoded struct)
- [ ] Changes to default/config.jsonc are reflected
- [ ] Fallback logic works correctly
- [ ] Tests verify file loading

### **Phase 2 Complete (Validation):**

- [ ] Schema created for instance configs
- [ ] Schema validation integrated
- [ ] Health scoring implemented
- [ ] Comprehensive error messages

### **Phase 3 Complete (Integration):**

- [ ] Integrated with config/config.go inheritance
- [ ] User → Instance merging works
- [ ] Session context creation uses merged config
- [ ] Full test coverage

---

## Example: Before and After

### **Before (BROKEN):**

```bash
# Edit default config
echo '{"instance_info": {"name": "test-change"}}' > config/instance/default/config.jsonc

# Load config
config := LoadInstanceConfig("nonexistent")
fmt.Println(config.InstanceInfo.Name)
# Output: "default-instance" (HARDCODED - ignores file change!)
```

### **After (FIXED):**

```bash
# Edit default config
echo '{"instance_info": {"name": "test-change"}}' > config/instance/default/config.jsonc

# Load config
config := LoadInstanceConfig("nonexistent")
fmt.Println(config.InstanceInfo.Name)
# Output: "test-change" (Loads from file correctly!)
```

---

This mapping provides surgical precision for fixing the instance config default loading bug and integrating with the broader config system.
