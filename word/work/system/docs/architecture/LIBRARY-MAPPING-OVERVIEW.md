# Runtime Library System - Complete Mapping Overview

**Created:** 2025-11-12
**Purpose:** Complete topology map of runtime/lib system - Data → Code → Config relationships
**Scope:** Foundation work for config-driven autonomous operation

---

## Executive Summary

The runtime library system contains **19 Go libraries** organized into three tiers:

### **Tier 1: Infrastructure Rails** (Orthogonal - All Components Attach)

- **logging/** (3950 lines) - Detection layer, health scoring, complete event narrative
- **debugging/** (2026 lines) - Assessment layer, state inspection, execution details

### **Tier 2: Core Services** (Foundational - Used by Higher Tiers)

- **config/** - Config inheritance (User → Instance → Project → Session)
- **sessiontime/** - Session state management, timing, compaction tracking
- **patterns/** - Pattern memory (cognitive, identity, relational patterns)
- **temporal/** - Orchestrates time awareness (4 dimensions)
- **privacy/** - Privacy filtering for sensitive data

### **Tier 3: Domain Libraries** (Specific Capabilities)

- **calendar/** - Calendar data access (base/personal/shared calendars)
- **planner/** - Planner template loading (daily/weekly patterns)
- **fs/** - Filesystem utilities
- **git/** - Git operations
- **system/** - System information
- **instance/** - Instance configuration loading
- **display/** - Formatted output with ANSI colors
- **validation/** - Multi-language code formatting (syntax + formatter)
- **sudoers/** - Sudoers validation
- **environment/** - Environment checking
- **operations/** - Operation testing

---

## Current State Assessment

### ✅ **What Exists and Works**

**Fully Config-Driven:**

- `validation/formatter.go` - Loads formatters.jsonc, graceful fallback
- `config/config.go` - JSONC parsing, inheritance chain
- `sessiontime/` - Reads/writes current.json session state
- `patterns/` - Reads/writes patterns.json

**Partially Config-Driven:**

- `calendar/` - Reads JSON calendar data (hardcoded path)
- `planner/` - Reads JSON planner templates (hardcoded path)
- `temporal/` - Orchestrates existing libraries (hardcoded user)

**Hardcoded but Functional:**

- `logging/` - Hardcoded paths, component routing lists
- `debugging/` - Hardcoded paths, format constants
- `instance/` - Hardcoded default config, fallback logic
- `privacy/` - Hardcoded filter patterns

---

## Data Layer Inventory

### **Session Data** (`system/data/session/`)

- `current.json` - Active session state (SessionState struct)
- `patterns.json` - Pattern memory (PatternData struct)
- **Schemas:** 7 JSON schemas defining structure

### **Temporal Data** (`system/data/temporal/`)

**Appointed (Calendars):**

- `base/2025/` - Monthly calendar JSON files
- `personal/`, `shared/`, `projects/` - User/project calendars
- **Schemas:** 7 schemas (calendar, date, event, milestone, etc.)

**Patterns:**

- `circadian/`, `seasonal/`, `work-patterns/` - Pattern data
- **Schemas:** 3 schemas

### **Planner Data** (`system/planner/`)

- `templates/` - User planner templates (JSON)

### **Config Validation** (`system/data/config/validation/`)

- `formatters.jsonc` - Multi-language formatter configurations

---

## Config Layer Topology

### **System Configs** (`system/config/`)

- `system.toml` - System-wide configuration
- `user.toml` - User-specific overrides
- **Schemas:** None currently

### **User/Instance Configs** (`config/`)

**User Configs:**

- `user/seanje-lenox-wise/config.jsonc`
- `user/default/config.jsonc`

**Instance Configs:**

- `instance/nova_dawn/config.jsonc`
- `instance/default/config.jsonc`

**Inheritance Chain:**

```bash
User Config → Instance Config → Project Config (optional)
      ↓              ↓                   ↓
SessionState (current.json with inherited context)
```

---

## Critical Gaps Identified

### 🔴 **Missing Data**

1. No centralized paths configuration file
2. No component routing configuration (logging uses hardcoded lists)
3. No system-wide environment variables registry
4. No planner schema files (templates exist, no validation schemas)

### 🔴 **Missing Configs**

1. **temporal.toml** - Temporal system configuration (user mappings, calendar paths)
2. **logging.toml** - Log routing rules, health visualization config
3. **debugging.toml** - Debug output configuration
4. **privacy.toml** - Privacy filter patterns, redaction rules
5. **paths.toml** - Centralized path configuration for all libraries

### 🔴 **Code Needing Config-Driven Updates**

1. **calendar.go** - Hardcoded `$HOME/.claude/cpi-si/system/calendar/base/` path
2. **planner.go** - Hardcoded `$HOME/.claude/cpi-si/system/planner/templates/` path
3. **temporal.go** - Hardcoded `"seanje"` user for planner loading
4. **logging/logger.go** - Hardcoded component routing lists in variables
5. **instance/config.go** - Hardcoded default config, should load from default/config.jsonc
6. **privacy/privacy.go** - Hardcoded filter patterns

---

## Architectural Patterns Observed

### **Rails Pattern** (Logging + Debugging)

- Orthogonal infrastructure - all components attach directly
- No passing through function parameters
- Each component creates own logger/inspector via `New*()` functions
- Correlation via shared `contextID`

### **Building Block Method** (Systematic Layering)

- Foundation → Layer 1 → Layer 2 → Conclusion
- Example: `config.go` - JSONC parsing → User config → Instance config → Session context

### **4-Block Structure** (Every File)

```bash
METADATA - Who, what, why, health scoring map
SETUP    - Imports, types, constants, variables
BODY     - Business logic (Direct Inverse organization)
CLOSING  - Validation, execution, cleanup, documentation
```

### **Direct Inverse Organization** (Definition = Reverse of Execution)

- Helpers first (called by everyone)
- Infrastructure second (orchestrates helpers)
- Public API last (entry points for external callers)
- Reading bottom-to-top shows dependencies
- Reading top-to-bottom shows usage

---

## Dependencies and Relationships

### **Logging Rail Dependencies:**

```bash
logging/logger.go (rails - no library deps)
  ↓
display/ (formatting)
validation/ (optional - code formatting)
```

### **Session Management Chain:**

```bash
config/config.go
  ↓
sessiontime/sessiontime.go
  ↓
patterns/patterns.go
```

### **Temporal Orchestration:**

```bash
temporal/temporal.go
  ├─→ sessiontime/ (session duration)
  ├─→ planner/ (schedule awareness)
  └─→ calendar/ (date information)
```

### **Instance Identity:**

```bash
instance/config.go
  ↓
config/config.go (for richer instance configs)
```

---

## Health Scoring Integration

**Base100 System** (Used by logging):

- All actions total 100 points
- Perfect execution = +100
- Complete failure = -100
- Real execution = sum of actual results
- Normalized health = (cumulative / total possible) × 100

**Integration Points:**

- Logging METADATA declares health scoring map
- formatters.go has scoring for config load/parse/format operations
- Most other libraries don't explicitly declare health maps yet

---

## Next Steps for Full Config-Driven Operation

### **Phase 1: Create Missing Configs**

1. Create `paths.toml` with all library paths
2. Create `temporal.toml` for temporal system config
3. Create `logging.toml` for routing and visualization
4. Create `privacy.toml` for filter patterns

### **Phase 2: Update Libraries to Load Configs**

1. Refactor `calendar.go` to load paths from config
2. Refactor `planner.go` to load paths from config
3. Refactor `temporal.go` to load user from config
4. Refactor `logging/logger.go` for config-driven routing
5. Refactor `instance/config.go` to use default config files

### **Phase 3: Create Missing Data Structures**

1. Create planner schemas (matching formatters.jsonc pattern)
2. Create centralized environment registry
3. Create system-wide paths registry

---

## Library Complexity Analysis

| Library | Lines | Complexity | Config Status |
|---------|-------|-----------|---------------|
| logging/ | 3950 | Very High | Hardcoded |
| debugging/ | 2026 | High | Hardcoded |
| validation/ | 1170 | High | ✅ Config-driven |
| config/ | 560 | Medium | ✅ Foundation |
| sessiontime/ | 456 | Medium | ✅ Data-driven |
| patterns/ | 425 | Medium | ✅ Data-driven |
| temporal/ | 282 | Medium | Partial |
| calendar/ | 110 | Low | Partial |
| planner/ | 121 | Low | Partial |
| privacy/ | ~200 | Low | Hardcoded |
| Others | <150 | Low | Mixed |

---

## Conclusion

The runtime library system is architecturally sound but needs **systematic config-driven refactoring** to achieve autonomous operation. Key work:

1. **Create 5 missing .toml configs** (paths, temporal, logging, debugging, privacy)
2. **Refactor 6 hardcoded libraries** to load from configs
3. **Add missing schemas** for planner data validation

All pieces exist - they need wiring through configuration rather than hardcoded paths.

See individual library analysis files for surgical update points.
