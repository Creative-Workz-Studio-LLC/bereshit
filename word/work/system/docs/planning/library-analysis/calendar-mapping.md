# Calendar Library - Complete Mapping

**File:** `system/runtime/lib/calendar/calendar.go`
**Lines:** 110
**Complexity:** LOW
**Status:** 🔴 CRITICAL PATH MISMATCH

---

## Current State

### **What It Does:**

Loads calendar data from JSON files organized by year/month. Provides functions to:

- Load calendar for specific month
- Get events for specific date
- Retrieve milestone information

### **Functions Provided:**

1. `LoadCalendar(year, month)` - Loads monthly calendar JSON file
2. `GetEventsForDate(date)` - Retrieves events for specific date
3. `GetMilestones()` - Gets milestone data

---

## Data Dependencies

### **Data It Needs:**

**Location (EXPECTED by code):**

```bash
~/.claude/cpi-si/system/calendar/base/2025/01-january.json
~/.claude/cpi-si/system/calendar/base/2025/02-february.json
... (12 months)
```

**Location (ACTUAL data location):**

```bash
~/.claude/cpi-si/system/data/temporal/appointed/base/2025/01-january.json
~/.claude/cpi-si/system/data/temporal/appointed/base/2025/02-february.json
... (12 months)
```

**🔴 CRITICAL MISMATCH: Code expects `/system/calendar/` but data is in `/system/data/temporal/appointed/`**

### **Schemas:**

- `system/data/temporal/appointed/schemas/calendar.json` - Main calendar structure
- `system/data/temporal/appointed/schemas/date.json` - Date entry structure
- `system/data/temporal/appointed/schemas/event.json` - Event structure
- `system/data/temporal/appointed/schemas/milestone.json` - Milestone definitions

### **Data Structure (JSON):**

```json
{
  "year": 2025,
  "month": 1,
  "month_name": "January",
  "days": [
    {
      "date": "2025-01-01",
      "day_of_week": "Wednesday",
      "events": [
        {
          "time": "00:00",
          "title": "New Year's Day",
          "type": "holiday"
        }
      ]
    }
  ],
  "milestones": []
}
```

---

## Config Dependencies

### **Currently Uses:**

- ❌ **NONE** - All paths hardcoded

### **Hardcoded Values:**

```go
// Line ~50-60 (approximate):
basePath := filepath.Join(homeDir, ".claude", "cpi-si", "system", "calendar", "base")
// WRONG PATH - should be "system/data/temporal/appointed/base"
```

### **Config Needed:**

**From:** `system/config/temporal.toml`

```toml
[calendar]
base_path = "system/data/temporal/appointed/base"
personal_path = "system/data/temporal/appointed/personal"
shared_path = "system/data/temporal/appointed/shared"
projects_path = "system/data/temporal/appointed/projects"
schema_dir = "system/data/temporal/appointed/schemas"
default_year = 2025
```

**OR From:** `system/config/paths.toml`

```toml
[temporal]
calendar_base = "system/data/temporal/appointed/base"
calendar_personal = "system/data/temporal/appointed/personal"
calendar_shared = "system/data/temporal/appointed/shared"
calendar_projects = "system/data/temporal/appointed/projects"
calendar_schemas = "system/data/temporal/appointed/schemas"
```

---

## Code Analysis

### **Current Implementation (WRONG):**

```go
func LoadCalendar(year int, month int) (*Calendar, error) {
    homeDir, _ := os.UserHomeDir()
    // WRONG PATH:
    basePath := filepath.Join(homeDir, ".claude", "cpi-si", "system", "calendar", "base")

    monthFile := fmt.Sprintf("%02d-%s.json", month, monthNames[month-1])
    calendarPath := filepath.Join(basePath, fmt.Sprintf("%d", year), monthFile)

    data, err := os.ReadFile(calendarPath)
    if err != nil {
        return nil, err
    }

    var calendar Calendar
    json.Unmarshal(data, &calendar)
    return &calendar, nil
}
```

### **Fixed Implementation (CONFIG-DRIVEN):**

```go
func LoadCalendar(year int, month int) (*Calendar, error) {
    // Load paths from config
    paths := config.LoadPaths()  // From paths.toml
    homeDir, _ := os.UserHomeDir()

    // Use correct path from config
    basePath := filepath.Join(homeDir, ".claude", "cpi-si", paths.Temporal.CalendarBase)

    // Rest stays the same
    monthFile := fmt.Sprintf("%02d-%s.json", month, monthNames[month-1])
    calendarPath := filepath.Join(basePath, fmt.Sprintf("%d", year), monthFile)

    data, err := os.ReadFile(calendarPath)
    if err != nil {
        return nil, err
    }

    var calendar Calendar
    json.Unmarshal(data, &calendar)
    return &calendar, nil
}
```

---

## Gaps Identified

### **🔴 CRITICAL:**

1. **Path mismatch** - Code expects wrong directory
2. **No config integration** - All paths hardcoded
3. **No graceful fallback** - Fails if data missing

### **🟡 IMPORTANT:**

1. **No schema validation** - Loads JSON without validating structure
2. **No error details** - Generic error messages
3. **Hardcoded calendar types** - Only loads "base", not personal/shared/projects

### **🟢 NICE-TO-HAVE:**

1. **No caching** - Re-reads file every call
2. **No health scoring** - Doesn't track load success/failure
3. **Limited API** - Could provide more query functions

---

## Refactoring Plan

### **Phase 1: Fix Critical Path Mismatch (IMMEDIATE)**

```go
// Step 1: Create paths.toml with correct path
[temporal]
calendar_base = "system/data/temporal/appointed/base"

// Step 2: Update calendar.go to load from config
paths := config.LoadPaths()
basePath := filepath.Join(homeDir, ".claude", "cpi-si", paths.Temporal.CalendarBase)

// Step 3: Test calendar loading
calendar, err := LoadCalendar(2025, 1)
// Should successfully load from correct location
```

**Estimated Effort:** 1-2 hours
**Risk:** LOW (straightforward path fix)

---

### **Phase 2: Add Config-Driven Calendar Types**

```go
// Load all calendar types from config
func LoadCalendarWithType(year, month int, calendarType string) (*Calendar, error) {
    paths := config.LoadPaths()

    var basePath string
    switch calendarType {
    case "base":
        basePath = paths.Temporal.CalendarBase
    case "personal":
        basePath = paths.Temporal.CalendarPersonal
    case "shared":
        basePath = paths.Temporal.CalendarShared
    case "projects":
        basePath = paths.Temporal.CalendarProjects
    default:
        return nil, fmt.Errorf("unknown calendar type: %s", calendarType)
    }

    // ... rest of loading logic
}
```

**Estimated Effort:** 2-3 hours
**Risk:** LOW (adds functionality, doesn't break existing)

---

### **Phase 3: Add Schema Validation & Health Scoring**

```go
func LoadCalendar(year, month int) (*Calendar, error) {
    logger := logging.NewLogger("calendar")
    logger.DeclareHealthTotal(100)

    logger.Operation("load-calendar", 0, fmt.Sprintf("year=%d month=%d", year, month))

    // Path resolution (+10)
    paths := config.LoadPaths()
    calendarPath := constructPath(paths, year, month)
    logger.Success("Path resolved", +10, map[string]any{"path": calendarPath})

    // File read (+40)
    data, err := os.ReadFile(calendarPath)
    if err != nil {
        logger.Failure("Calendar file not found", err.Error(), -90, map[string]any{"path": calendarPath})
        return nil, err
    }
    logger.Success("File read", +40, map[string]any{"bytes": len(data)})

    // Schema validation (+25)
    if err := validateCalendarSchema(data, paths.Temporal.CalendarSchemas); err != nil {
        logger.Failure("Schema validation failed", err.Error(), -25, nil)
        return nil, err
    }
    logger.Success("Schema valid", +25, nil)

    // JSON parse (+25)
    var calendar Calendar
    if err := json.Unmarshal(data, &calendar); err != nil {
        logger.Failure("JSON parse failed", err.Error(), -25, nil)
        return nil, err
    }
    logger.Success("Calendar loaded", +25, map[string]any{"days": len(calendar.Days)})

    return &calendar, nil
}
```

**Estimated Effort:** 4-5 hours
**Risk:** MEDIUM (adds complexity, needs testing)

---

## Dependencies

### **Imports Used:**

- `os` - File operations
- `path/filepath` - Path construction
- `encoding/json` - JSON parsing

### **Should Add:**

- `system/lib/logging` - Health tracking
- `system/lib/config` - Path loading
- Schema validation library (or custom)

### **Called By:**

- `temporal/temporal.go` - Date awareness
- User commands (future) - Direct calendar queries

### **Calls:**

- File system (os.ReadFile)
- JSON parser (json.Unmarshal)

---

## Testing Requirements

### **Unit Tests Needed:**

1. `TestLoadCalendar_Success` - Loads valid calendar
2. `TestLoadCalendar_FileNotFound` - Handles missing file gracefully
3. `TestLoadCalendar_InvalidJSON` - Handles malformed JSON
4. `TestLoadCalendar_SchemaValidation` - Validates against schema
5. `TestGetEventsForDate` - Retrieves events correctly
6. `TestGetMilestones` - Retrieves milestones correctly

### **Integration Tests Needed:**

1. Load from all calendar types (base, personal, shared, projects)
2. Load across multiple months/years
3. Config-driven path resolution
4. Graceful fallback when calendar missing

---

## Health Scoring Map

**Total: 100 points**

| Action | Points | Success | Failure |
|--------|--------|---------|---------|
| Path resolution | 10 | +10 | -10 |
| File read | 40 | +40 | -90 (critical - can't proceed) |
| Schema validation | 25 | +25 | -25 |
| JSON parse | 25 | +25 | -25 |

**Perfect execution:** +100
**File not found:** -90 (critical failure, can't load)
**Schema invalid:** +50 (loaded but invalid structure)
**Parse error:** +50 (loaded but corrupt data)

---

## Success Criteria

### **Phase 1 Complete:**

- [ ] Path mismatch fixed
- [ ] Calendar loads from correct data location
- [ ] All calendar types accessible (base, personal, shared, projects)

### **Phase 2 Complete:**

- [ ] Config-driven path resolution
- [ ] No hardcoded paths in code
- [ ] Graceful fallback for missing calendars

### **Phase 3 Complete:**

- [ ] Schema validation integrated
- [ ] Health scoring implemented
- [ ] Comprehensive error messages
- [ ] Full test coverage

---

This mapping provides surgical precision for updating calendar library to be fully config-driven and production-ready.
