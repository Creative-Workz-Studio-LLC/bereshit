# Runtime Library Documentation Index

**Created:** 2025-11-12
**Total Documentation:** 7 files, ~3,100 lines
**Purpose:** Complete mapping of runtime library system for config-driven refactoring

---

## Core Documentation (Start Here)

### 1. **README-MAPPING-COMPLETE.md** (320 lines)

**Purpose:** Executive summary and quick-start guide
**Read this first:** Overview of findings, critical issues, quick fixes, roadmap

**Key Sections:**

- Critical findings (4 major issues)
- System health summary
- 3-phase implementation roadmap (8-11 days)
- Quick start fixes (30 min each)
- Library inventory (all 19)
- Success metrics

**When to read:** Start here for high-level understanding

---

### 2. **LIBRARY-MAPPING-OVERVIEW.md** (200 lines)

**Purpose:** High-level summary of all 19 libraries
**Organized by:** Tier (Rails → Core Services → Domain Libraries)

**Key Sections:**

- Executive summary (3 tiers)
- Current state assessment (what works, what's partial, what's hardcoded)
- Data layer inventory
- Config layer topology
- Critical gaps identified
- Architectural patterns observed
- Dependencies and relationships
- Health scoring integration
- Next steps

**When to read:** Need architectural overview and library organization

---

### 3. **DATA-TO-CODE-MATRIX.md** (270 lines)

**Purpose:** Complete mapping of Data ↔ Code ↔ Config relationships
**Format:** Three-column relationship tables

**Key Sections:**

- Session management stack (SessionState, PatternMemory)
- Temporal system stack (Calendar, Planner)
- Config validation stack (Formatters)
- Instance/User configuration stack
- Logging/Debugging stack
- Privacy filtering stack
- Summary matrix (all data → code → config)
- Critical data mismatches
- Config-driven refactoring priority

**When to read:** Need to know which data connects to which code and what configs are needed

---

### 4. **CONFIG-TOPOLOGY.md** (430 lines)

**Purpose:** Map complete configuration system topology
**Focus:** Config hierarchy, inheritance, and interconnections

**Key Sections:**

- Config hierarchy overview (System → User → Instance → Session)
- Current config files (what exists)
- Missing configs (5 critical .toml files)
- Config inheritance chain
- Schema relationships
- Config loading patterns
- Config topology visualization
- Integration priorities

**When to read:** Need to understand how configs interconnect and what's missing

---

### 5. **GAPS-AND-REQUIREMENTS.md** (370 lines)

**Purpose:** Comprehensive gaps inventory and implementation plan
**Organization:** By layer (Data, Config, Code) and priority (Critical, Important, Nice-to-Have)

**Key Sections:**

- 🔴 Critical gaps (5 major issues)
- 🟡 Important gaps (7 high-priority items)
- 🟢 Nice-to-have gaps (4 polish items)
- Summary by layer (Data, Config, Code)
- Phased implementation plan (3 phases, detailed)
- Success criteria
- Risk assessment
- Estimated effort (65-85 hours)

**When to read:** Need complete gap inventory and implementation roadmap

---

## Library-Specific Analysis

### 6. **library-analysis/calendar-mapping.md** (220 lines)

**Library:** `system/runtime/lib/calendar/calendar.go`
**Status:** 🔴 CRITICAL PATH MISMATCH
**Lines:** 110
**Complexity:** LOW

**Key Sections:**

- Current state (what it does, functions provided)
- Data dependencies (expected vs actual location)
- Config dependencies (needs temporal.toml)
- Code analysis (current vs fixed implementation)
- Gaps identified (critical, important, nice-to-have)
- Refactoring plan (3 phases)
- Health scoring map
- Testing requirements
- Success criteria

**When to read:** Fixing calendar path mismatch or updating calendar library

---

### 7. **library-analysis/instance-mapping.md** (290 lines)

**Library:** `system/runtime/lib/instance/config.go`
**Status:** 🔴 CRITICAL - Hardcoded default config
**Lines:** ~200
**Complexity:** MEDIUM

**Key Sections:**

- Current state (what it does, the bug)
- Data dependencies (default/config.jsonc exists but not loaded)
- Config dependencies (integration with config/config.go)
- Code analysis (current broken vs fixed)
- Relationship with config/config.go
- Gaps identified
- Refactoring plan (3 phases)
- Health scoring map
- Testing requirements
- Before/after examples

**When to read:** Fixing instance default loading or understanding config inheritance

---

## Quick Reference by Use Case

### **"I need to fix the calendar path mismatch"**

→ Read: [calendar-mapping.md](./library-analysis/calendar-mapping.md)
→ Quick fix: Update calendar.go line ~50-60, change path to `/system/data/temporal/appointed/base`

### **"I need to fix instance default loading"**

→ Read: [instance-mapping.md](./library-analysis/instance-mapping.md)
→ Quick fix: Update instance/config.go `GetDefaultConfig()` to load from file

### **"I need to create paths.toml"**

→ Read: [CONFIG-TOPOLOGY.md](./CONFIG-TOPOLOGY.md) section "Missing Configs"
→ Template: See paths.toml structure in GAPS-AND-REQUIREMENTS.md

### **"I need to understand what data connects to what code"**

→ Read: [DATA-TO-CODE-MATRIX.md](./DATA-TO-CODE-MATRIX.md)
→ Tables: Session, Temporal, Config Validation, Instance/User stacks

### **"I need to see all the gaps and priorities"**

→ Read: [GAPS-AND-REQUIREMENTS.md](./GAPS-AND-REQUIREMENTS.md)
→ Focus: Critical gaps section, phased implementation plan

### **"I need a complete implementation roadmap"**

→ Read: [README-MAPPING-COMPLETE.md](./README-MAPPING-COMPLETE.md) section "Implementation Roadmap"
→ Also see: GAPS-AND-REQUIREMENTS.md "Phased Implementation Plan"

### **"I need architectural overview of all libraries"**

→ Read: [LIBRARY-MAPPING-OVERVIEW.md](./LIBRARY-MAPPING-OVERVIEW.md)
→ Focus: Tier organization, dependencies, architectural patterns

### **"I need to understand config inheritance"**

→ Read: [CONFIG-TOPOLOGY.md](./CONFIG-TOPOLOGY.md) section "Config Inheritance Chain"
→ Visualization: Config topology diagram

---

## Documentation Statistics

| Document | Lines | Size | Purpose |
|----------|-------|------|---------|
| README-MAPPING-COMPLETE.md | 320 | 14K | Executive summary, quick start |
| LIBRARY-MAPPING-OVERVIEW.md | 200 | 8.9K | High-level library summary |
| DATA-TO-CODE-MATRIX.md | 270 | 12K | Data ↔ Code ↔ Config mapping |
| CONFIG-TOPOLOGY.md | 430 | 19K | Config interconnections |
| GAPS-AND-REQUIREMENTS.md | 370 | 16K | Gap inventory, implementation plan |
| calendar-mapping.md | 220 | 9.7K | Calendar library analysis |
| instance-mapping.md | 290 | 13K | Instance library analysis |
| **TOTAL** | **~3,100** | **~92K** | **Complete system mapping** |

---

## Key Findings Summary

### **Critical Issues (Fix Immediately):**

1. 🔴 Calendar path mismatch (code expects wrong directory)
2. 🔴 Instance default not loaded (hardcoded struct ignores file)
3. 🔴 Missing paths.toml (all libraries use hardcoded paths)
4. 🔴 Missing temporal.toml (user hardcoded in planner)
5. 🔴 Missing planner schemas (cannot validate templates)

### **Architectural Strengths:**

- ✅ 4-Block structure consistent across all files
- ✅ Rails pattern (logging/debugging) clean and orthogonal
- ✅ Direct Inverse organization predictable
- ✅ Health scoring integrated in logging layer
- ✅ Config inheritance system (config/config.go) solid

### **System State:**

- **19 libraries** mapped completely
- **All 19** use hardcoded paths (need refactoring)
- **1 library** (validation/formatter.go) fully config-driven (exemplar)
- **2 critical bugs** identified (calendar, instance)
- **5 missing configs** needed (.toml files)
- **Effort estimate:** 65-85 hours (8-11 days) for full refactoring

---

## Reading Order (Recommended)

### **First Pass (1 hour):**

1. README-MAPPING-COMPLETE.md (overview, critical issues)
2. GAPS-AND-REQUIREMENTS.md (gaps, priorities)

### **Second Pass (2 hours):**

1. DATA-TO-CODE-MATRIX.md (understand connections)
2. CONFIG-TOPOLOGY.md (understand config system)

### **Third Pass (As Needed):**

1. LIBRARY-MAPPING-OVERVIEW.md (architectural context)
2. calendar-mapping.md (when fixing calendar)
3. instance-mapping.md (when fixing instance)

---

## Implementation Priority

### **Today (30 min each):**

- [ ] Fix calendar path mismatch
- [ ] Fix instance default loading

### **This Week (Phase 1: 20-25 hours):**

- [ ] Create paths.toml
- [ ] Create temporal.toml
- [ ] Create planner schemas
- [ ] Test critical fixes

### **Next 2 Weeks (Phase 2: 30-40 hours):**

- [ ] Refactor all libraries to load paths from config
- [ ] Create logging.toml
- [ ] Create config schemas

### **Week 4 (Phase 3: 15-20 hours):**

- [ ] Create remaining .toml configs
- [ ] Full integration testing
- [ ] Documentation updates

---

## Success Criteria

### **Mapping Complete:** ✅

- [x] All 19 libraries analyzed
- [x] Data ↔ Code ↔ Config relationships mapped
- [x] All gaps identified
- [x] Implementation plan created
- [x] Critical issues documented

### **Refactoring Complete (Future):**

- [ ] No hardcoded paths in any library
- [ ] All .toml configs created
- [ ] All schemas exist
- [ ] System fully config-driven
- [ ] Autonomous operation achieved

---

## Contact & Maintenance

**Created by:** Nova Dawn
**Date:** 2025-11-12
**For:** Seanje Lenox-Wise
**Purpose:** Foundation work for config-driven autonomous operation
**Status:** ✅ MAPPING COMPLETE - Ready for implementation

**Maintenance Notes:**

- Update this index when adding new library analyses
- Keep success criteria current as implementation progresses
- Add new use cases to Quick Reference section as patterns emerge

---

**End of Documentation Index**
