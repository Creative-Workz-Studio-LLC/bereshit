# Calendar Library Demo

**Purpose:** Demonstrate proper usage of the calendar library by importing and using it as a real consumer would.

**Location:** `system/runtime/demos/calendar/`

---

## What This Demonstrates

This is NOT a test - this is a **demo** showing real usage patterns for the calendar library.

### 7 Demos Included

1. **Loading current month calendar** - Basic library usage
2. **Querying specific date information** - Date lookup operations
3. **Retrieving month metadata** - Month-level data access
4. **Finding all holidays in month** - Data iteration and filtering
5. **Analyzing weekends in month** - Practical use case
6. **Error handling demonstration** - How library handles invalid data
7. **Loading calendar from different year** - Cross-year operations

---

## Running the Demo

```bash
cd ~/.claude/cpi-si/system/runtime/demos/calendar
go run demo.go
```

---

## What This Proves

✅ Calendar library works when imported as a module
✅ JSONC files are parsed correctly
✅ All three public functions work (LoadMonthCalendar, GetDateInfo, GetMonthInfo)
✅ Error handling works for invalid dates
✅ Data structure matches actual calendar files
✅ **Config-driven paths work** - Loads from paths.toml (no hardcoded paths)
✅ **Paths library integration** - Uses system/lib/paths for path resolution

---

## Educational Value

This demo serves as:

- **Reference implementation** for how to use calendar library
- **Usage examples** for each public function
- **Proof of functionality** that library works as intended
- **Documentation supplement** showing real code, not just descriptions

---

## Key Learnings

### Library Functions

```go
// Load full month calendar
cal, err := calendar.LoadMonthCalendar(2025, 11)

// Query specific date
dateInfo, err := calendar.GetDateInfo(2025, 11, 12)

// Get month metadata
monthInfo, err := calendar.GetMonthInfo(2025, 11)
```

### Data Access Patterns

```go
// Iterate over all dates in month
for _, date := range cal.Dates {
    if date.IsHoliday {
        fmt.Println(date.Holiday)
    }
}

// Access month metadata
fmt.Println(monthInfo.Name)        // "November"
fmt.Println(monthInfo.DaysInMonth) // 30
```

### Error Handling

```go
// Library returns errors for invalid dates
_, err := calendar.GetDateInfo(2025, 11, 99)
if err != nil {
    // Handle error - date not found
}
```

---

## What This Is NOT

❌ Not a unit test (doesn't assert specific values)
❌ Not a benchmark (doesn't measure performance)
❌ Not comprehensive coverage (shows common use cases)

This is a **demonstration** of how consumers should use the library.

---

## Related Files

- **Library:** `system/runtime/lib/calendar/calendar.go`
- **Data:** `system/data/temporal/chronological/calendar/YYYY/MM-month.jsonc`
- **Config:** `system/config/temporal.toml`, `system/config/paths.toml`

---

**Created:** 2025-11-12
**Purpose:** Verify calendar library path fix and demonstrate proper usage
**Educational:** Reference for future calendar library consumers
