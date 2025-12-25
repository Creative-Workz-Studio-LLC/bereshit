# Runtime Library Mapping - COMPLETE

**Date:** 2025-11-12
**Scope:** Complete Data-to-Code-to-Config mapping for system/runtime/lib
**Purpose:** Foundation work for config-driven autonomous operation
**Status:** ✅ MAPPING COMPLETE

---

## What This Mapping Contains

This comprehensive exploration mapped the entire runtime library system - **19 libraries, their data dependencies, config requirements, and identified gaps.** This is foundation work to make the system fully config-driven and autonomous.

---

## Documentation Structure

### **📋 Core Documents:**

1. **[LIBRARY-MAPPING-OVERVIEW.md](./LIBRARY-MAPPING-OVERVIEW.md)**
   - High-level summary of all 19 libraries
   - Tier organization (Rails, Core Services, Domain Libraries)
   - Current state assessment
   - Complexity analysis

2. **[DATA-TO-CODE-MATRIX.md](./DATA-TO-CODE-MATRIX.md)**
   - Complete mapping: Data ↔ Code ↔ Config
   - What data connects to what code
   - What configs are needed
   - Critical mismatches identified

3. **[CONFIG-TOPOLOGY.md](./CONFIG-TOPOLOGY.md)**
   - How all configs interconnect
   - Inheritance chain (System → User → Instance → Session)
   - Missing configs catalog
   - Config loading patterns

4. **[GAPS-AND-REQUIREMENTS.md](./GAPS-AND-REQUIREMENTS.md)**
   - Complete gaps inventory (by priority)
   - Phased implementation plan (3 phases, 8-11 days)
   - Success criteria
   - Risk assessment

### **📂 Library-Specific Analysis:**

1. **[library-analysis/calendar-mapping.md](./library-analysis/calendar-mapping.md)**
   - 🔴 CRITICAL PATH MISMATCH
   - Code expects wrong directory
   - Fix: Update to correct data location

2. **[library-analysis/instance-mapping.md](./library-analysis/instance-mapping.md)**
   - 🔴 CRITICAL DEFAULT CONFIG BUG
   - Hardcoded struct instead of loading file
   - Fix: Load from default/config.jsonc

---

## Critical Findings (Action Required)

### **🔴 CRITICAL ISSUES (Fix Immediately):**

1. **Calendar Path Mismatch**
   - Code: `system/calendar/base/2025/`
   - Data: `system/data/temporal/appointed/base/2025/`
   - **Impact:** System cannot load calendars
   - **Fix:** Update calendar.go path (1-2 hours)

2. **Instance Default Not Loaded**
   - File exists: `config/instance/default/config.jsonc`
   - Code has: Hardcoded struct (ignores file changes)
   - **Impact:** Config changes ignored
   - **Fix:** Load from file instead of hardcoding (1-2 hours)

3. **Missing paths.toml**
   - **Impact:** ALL 19 libraries use hardcoded paths
   - **Blocks:** Config-driven autonomous operation
   - **Fix:** Create paths.toml with all path definitions (8-10 hours total)

4. **Missing temporal.toml**
   - **Impact:** User hardcoded as "seanje" in planner loading
   - **Blocks:** Multi-user operation
   - **Fix:** Create temporal.toml, load user from config (2-3 hours)

5. **Missing Planner Schemas**
   - Data exists: `system/planner/templates/*.json`
   - Schemas missing: No validation schemas
   - **Impact:** Cannot validate planner templates
   - **Fix:** Create 3 schemas (template, daily, weekly) (4-5 hours)

---

## System Health Summary

### **✅ What Works:**

- **validation/formatter.go** - ✅ Fully config-driven (exemplar)
- **config/config.go** - ✅ Rich inheritance system
- **sessiontime/** - ✅ Session state management
- **patterns/** - ✅ Pattern memory
- **logging/** - ✅ Health scoring, comprehensive logging
- **debugging/** - ✅ State inspection

### **⚠️ What's Partial:**

- **calendar/** - ⚠️ Reads data but wrong path
- **planner/** - ⚠️ Loads templates but hardcoded paths
- **temporal/** - ⚠️ Orchestrates but hardcoded user
- **instance/** - ⚠️ Loads configs but broken default

### **❌ What's Hardcoded:**

- **All libraries** - ❌ Paths hardcoded (need paths.toml)
- **logging/** - ❌ Component routing hardcoded
- **privacy/** - ❌ Filter patterns hardcoded

---

## Implementation Roadmap

### **🔴 Phase 1: Critical Foundation (Week 1) - 20-25 hours**

**Priority:** HIGHEST - Fixes broken functionality

**Day 1-2:**

- [ ] Create `system/config/paths.toml` with all path definitions
- [ ] Create `config/paths.go` to load paths.toml
- [ ] Test path loading independently

**Day 3:**

- [ ] Create `system/config/temporal.toml` with user config
- [ ] Fix calendar path mismatch (update calendar.go)
- [ ] Test calendar loading from correct location

**Day 4:**

- [ ] Fix instance default loading (update instance/config.go)
- [ ] Test default config loading from file

**Day 5:**

- [ ] Create planner schemas (template, daily, weekly)
- [ ] Test planner schema validation

**Deliverable:** Critical bugs fixed, foundation configs in place

---

### **🟡 Phase 2: Systematic Refactoring (Week 2-3) - 30-40 hours**

**Priority:** HIGH - Makes system config-driven

**Week 2:**

- [ ] Update sessiontime/ to load paths from config
- [ ] Update patterns/ to load paths from config
- [ ] Update calendar/ (if not done in Phase 1)
- [ ] Update planner/ to load paths from config
- [ ] Test session and temporal systems

**Week 3:**

- [ ] Create `system/config/logging.toml` with routing config
- [ ] Refactor logging/logger.go to load routing from config
- [ ] Test logging with config-driven routing
- [ ] Update temporal.go to load user from config
- [ ] Create user/instance config schemas

**Deliverable:** Core libraries config-driven, logging maintainable

---

### **🟢 Phase 3: Polish & Integration (Week 4) - 15-20 hours**

**Priority:** MEDIUM - System polish and completeness

- [ ] Create `system/config/debugging.toml` and integrate
- [ ] Create `system/config/privacy.toml` + filters.json and integrate
- [ ] Create system-wide config loader for system.toml/user.toml
- [ ] Add remaining validation schemas
- [ ] Full system integration testing
- [ ] Documentation updates

**Deliverable:** Complete config-driven autonomous operation

---

## Quick Start: Fix Critical Issues

### **1. Fix Calendar Path Mismatch (30 minutes)**

```bash
# Edit calendar/calendar.go
# Find line ~50-60:
basePath := filepath.Join(homeDir, ".claude", "cpi-si", "system", "calendar", "base")

# Change to:
basePath := filepath.Join(homeDir, ".claude", "cpi-si", "system", "data", "temporal", "appointed", "base")

# Test:
cd system/runtime/lib/calendar
go test -run TestLoadCalendar
```

---

### **2. Fix Instance Default Loading (30 minutes)**

```bash
# Edit instance/config.go
# Find GetDefaultConfig() function (currently returns hardcoded struct)

# Replace with:
func LoadDefaultConfigFromFile() (*InstanceConfig, error) {
    homeDir, _ := os.UserHomeDir()
    defaultPath := filepath.Join(homeDir, ".claude", "cpi-si", "config", "instance", "default", "config.jsonc")

    data, err := os.ReadFile(defaultPath)
    if err != nil {
        return getTrueFallback(), nil  // Only if file missing
    }

    var config InstanceConfig
    json.Unmarshal(data, &config)
    return &config, nil
}

# Update LoadInstanceConfig() to use this function

# Test:
cd system/runtime/lib/instance
go test -run TestLoadDefault
```

---

### **3. Create paths.toml (1-2 hours)**

```bash
# Create system/config/paths.toml
cat > system/config/paths.toml << 'EOF'
[base]
system_dir = "system"
config_dir = "config"
data_dir = "system/data"
logs_dir = "system/logs"

[session]
state_file = "system/data/session/current.json"
patterns_file = "system/data/session/patterns.json"
schema_dir = "system/data/session/schemas"

[temporal]
calendar_base = "system/data/temporal/appointed/base"
calendar_personal = "system/data/temporal/appointed/personal"
calendar_shared = "system/data/temporal/appointed/shared"
calendar_projects = "system/data/temporal/appointed/projects"
calendar_schemas = "system/data/temporal/appointed/schemas"
planner_templates = "system/planner/templates"
planner_schemas = "system/data/temporal/planner/schemas"

[config]
validation_dir = "system/data/config/validation"
formatters = "system/data/config/validation/formatters.jsonc"
schemas = "system/data/config/validation/schemas"

[logging]
base_dir = "system/logs"
commands_dir = "system/logs/commands"
libraries_dir = "system/logs/libraries"
scripts_dir = "system/logs/scripts"
system_dir = "system/logs/system"

[privacy]
filters_file = "system/data/privacy/filters.json"
schema = "system/data/privacy/schemas/filters.json"
EOF

# Create config/paths.go to load this
# ... (see CONFIG-TOPOLOGY.md for implementation)
```

---

## Library Inventory (All 19)

### **Tier 1: Infrastructure Rails**

1. **logging/** (3950 lines) - Detection layer, health scoring
2. **debugging/** (2026 lines) - Assessment layer, state inspection

### **Tier 2: Core Services**

1. **config/** (560 lines) - Config inheritance system
2. **sessiontime/** (456 lines) - Session state management
3. **patterns/** (425 lines) - Pattern memory
4. **temporal/** (282 lines) - Time orchestration
5. **privacy/** (~200 lines) - Privacy filtering

### **Tier 3: Domain Libraries**

1. **validation/** (1170 lines) - Multi-language formatting ✅ Config-driven
2. **calendar/** (110 lines) - Calendar data access 🔴 Path mismatch
3. **planner/** (121 lines) - Planner templates ⚠️ Hardcoded paths
4. **instance/** (~200 lines) - Instance configs 🔴 Default not loaded
5. **fs/** (~150 lines) - Filesystem utilities
6. **git/** (~150 lines) - Git operations
7. **system/** (~100 lines) - System information
8. **display/** (~200 lines) - Formatted output
9. **sudoers/** (~150 lines) - Sudoers validation
10. **environment/** (~150 lines) - Environment checking
11. **operations/** (~200 lines) - Operation testing
12. *(Other small utilities)*

---

## Data Inventory

### **Session Data** (`system/data/session/`)

- `current.json` - Session state ✅
- `patterns.json` - Pattern memory ✅
- 7 schemas ✅

### **Temporal Data** (`system/data/temporal/`)

**Appointed (Calendars):**

- `base/2025/` - Monthly calendars ✅
- `personal/`, `shared/`, `projects/` - User calendars ✅
- 7 schemas ✅

**Patterns:**

- `circadian/`, `seasonal/`, `work-patterns/` ✅
- 3 schemas ✅

### **Config Validation** (`system/data/config/validation/`)

- `formatters.jsonc` ✅
- 1 schema ✅

### **Planner** (`system/planner/`)

- `templates/*.json` ✅
- ❌ **Schemas missing** (need 3)

---

## Config Inventory

### **System Level** (`system/config/`)

- `system.toml` ✅ (exists, not loaded)
- `user.toml` ✅ (exists, not loaded)
- ❌ `paths.toml` (CRITICAL - MISSING)
- ❌ `temporal.toml` (CRITICAL - MISSING)
- ❌ `logging.toml` (IMPORTANT - MISSING)
- ❌ `debugging.toml` (nice-to-have)
- ❌ `privacy.toml` (nice-to-have)

### **User Level** (`config/user/`)

- `seanje-lenox-wise/config.jsonc` ✅
- `default/config.jsonc` ✅

### **Instance Level** (`config/instance/`)

- `nova_dawn/config.jsonc` ✅
- `default/config.jsonc` ✅ (exists but not loaded! 🔴)

---

## Success Metrics

### **Phase 1 Complete When:**

- [ ] Calendar loads from correct data location
- [ ] Instance default loads from file (not hardcoded)
- [ ] paths.toml exists and works
- [ ] temporal.toml exists and works
- [ ] Planner schemas exist and validate

### **Phase 2 Complete When:**

- [ ] No hardcoded paths in any library
- [ ] Logging routing config-driven
- [ ] Temporal user config-driven
- [ ] All core libraries config-driven

### **Phase 3 Complete When:**

- [ ] All .toml configs created
- [ ] All schemas exist
- [ ] System fully autonomous
- [ ] Complete integration testing

---

## Estimated Effort

| Phase | Hours | Days (8hr/day) |
|-------|-------|----------------|
| Phase 1: Critical Foundation | 20-25 | 2.5-3 |
| Phase 2: Systematic Refactoring | 30-40 | 4-5 |
| Phase 3: Polish & Integration | 15-20 | 2-2.5 |
| **TOTAL** | **65-85** | **8-11** |

---

## Next Actions

### **IMMEDIATE (Today):**

1. Read GAPS-AND-REQUIREMENTS.md for complete gap inventory
2. Fix calendar path mismatch (30 min fix)
3. Fix instance default loading (30 min fix)

### **THIS WEEK (Phase 1):**

1. Create paths.toml (foundation for everything)
2. Create temporal.toml (fixes user hardcoding)
3. Create planner schemas (enables validation)
4. Test all critical fixes

### **NEXT 2 WEEKS (Phase 2):**

1. Systematic refactoring of all libraries to load paths from config
2. Create logging.toml and refactor routing
3. Full config-driven operation

---

## Documentation Navigation

**Start Here:**

1. Read this README (you are here)
2. Review [GAPS-AND-REQUIREMENTS.md](./GAPS-AND-REQUIREMENTS.md) for priorities
3. Check [DATA-TO-CODE-MATRIX.md](./DATA-TO-CODE-MATRIX.md) for specific library connections
4. Reference [CONFIG-TOPOLOGY.md](./CONFIG-TOPOLOGY.md) for config structure
5. See library-specific analyses for surgical updates

**For Specific Libraries:**

- [calendar-mapping.md](./library-analysis/calendar-mapping.md) - Calendar path fix
- [instance-mapping.md](./library-analysis/instance-mapping.md) - Instance default fix

---

## Conclusion

This comprehensive mapping provides **surgical precision** for transforming the runtime library system from hardcoded to fully config-driven. All pieces exist - they need systematic wiring through configuration.

**Foundation work complete. Ready for systematic refactoring.**

---

**Mapping completed by:** Nova Dawn
**Date:** 2025-11-12
**Thoroughness level:** VERY THOROUGH (as requested)
**Status:** ✅ COMPLETE - Ready for implementation
