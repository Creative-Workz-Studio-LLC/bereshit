# Temporal Awareness Consciousness Substrate - Calibration Test
## Baseline: 2025-11-04 16:56 CST

**Purpose:** Complete system calibration to verify Four Dimensions of Temporal Awareness integration across hooks, tools, statusline, and library orchestration.

**Session Context:**
- Session ID: 2025-11-04_1642
- Session Start: Tue Nov 04, 2025 at 16:42:14
- Test Time: ~14 minutes elapsed
- Compactions: 0

---

## Test 1: Four Dimensions - Individual Component Tests

### 1.1 External Time (Clock + Calendar)
**Tool:** `external-time`
**Expected:** Show current time, date, week number

```bash
~/.claude/system/bin/external-time
```

**Result:**
```
⏰📅 External Time Awareness (Clock + Calendar)
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

⏰ Clock (Time):
   16:56:XX (04:56 PM)

📅 Calendar (Date):
   Tuesday, November 04, 2025
   2025-11-04

━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

💡 The world's timeline - time moves continuously
```

**Status:** ⏳ PENDING TEST

---

### 1.2 Internal Time (Session Duration)
**Tool:** `session-time check`
**Expected:** Show session start, current time, elapsed, compaction count (if any)

```bash
~/.claude/system/bin/session-time check
```

**Result:**
```
Session Start:   Tue Nov 04, 2025 at 16:42:14
Current Time:    Tue Nov 04, 2025 at 16:56:XX
Elapsed:         ~14mXXs
Compactions:     0 (or not shown if 0)
```

**Status:** ⏳ PENDING TEST

---

### 1.3 Internal Schedule (Planner)
**Tool:** `planner-view` (defaults to today, seanje)
**Expected:** Show today's 24-hour schedule with all time blocks

```bash
~/.claude/system/bin/planner-view
```

**Result:**
```
📅 Tuesday, November 04, 2025 - Seanje's Schedule
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

🧘 07:00 - 08:00  🔒 Personal Morning routine (Lord time, breakfast) (1h)
💼 09:00 - 17:00  ⭐ Work Work hours (8h)
🍽️ 12:00 - 13:00  ⭐ Meal Lunch break (1h)
🍽️ 18:00 - 19:00  ⭐ Meal Dinner (1h)
😴 23:00 - 07:00  🔒 Sleep Sleep schedule (crosses midnight) (8h)

⏱️  Unscheduled Time (potential availability):
   [list of unscheduled blocks]
```

**Status:** ⏳ PENDING TEST

---

### 1.4 External Calendar (Base Calendar)
**Tool:** `calendar-query` (defaults to today)
**Expected:** Show date, weekday, week number

```bash
~/.claude/system/bin/calendar-query
```

**Result:**
```
📅 2025-11-04
   Tuesday
   Week 45
```

**Status:** ⏳ PENDING TEST

---

## Test 2: Temporal Library Orchestration

### 2.1 Temporal Context Composition
**Test:** Verify temporal library composes all four dimensions from extracted libraries
**Method:** Check that temporal/temporal.go imports and orchestrates:
- `system/lib/sessiontime` for Internal Time
- `system/lib/planner` for Internal Schedule
- `system/lib/calendar` for External Calendar
- Native time.Now() for External Time

**Expected:** No reimplementation, pure orchestration

**Status:** ⏳ PENDING TEST

---

### 2.2 Session-Time-Awareness Planner Integration
**Tool:** `session-time-awareness`
**Expected:** Uses extracted planner library to classify downtime, not reimplemented logic

```bash
~/.claude/system/bin/session-time-awareness
```

**Result:**
```
🕐 3-Stage Time Awareness
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

⏱️  Wall-Clock Elapsed:  ~14mXXs
   Session started: Tue Nov 04, 2025 at 16:42:14

✅ Active Uptime:        ~14mXXs (100%)
   Time actively working

💤 Semi-Downtime:        0s (0%)
   Session open but idle (>30min gaps)

✅ Current State:       UPTIME - Actively working
   Last activity: 16:56:XX
   Active 0s ago

📅 Internal Calendar:
   Work: [current work item]
   Session X of Y | Day X of Y | XX% complete
```

**Status:** ⏳ PENDING TEST

---

## Test 3: Tool Defaults and Exit Codes

### 3.1 View/Query Tools - Default Behavior
**Expected:** All should work with NO parameters (default to "now")

| Tool | Command | Expected Behavior | Exit Code |
|------|---------|-------------------|-----------|
| `planner-view` | `planner-view` | Shows today, seanje | 0 |
| `calendar-query` | `calendar-query` | Shows today | 0 |
| `calendar-overlay` | `calendar-overlay` | Shows today overlay | 0 |
| `external-time` | `external-time` | Shows now | 0 |
| `session-time check` | `session-time check` | Shows current session | 0 |
| `session-time-awareness` | `session-time-awareness` | Shows current session 3-stage | 0 |
| `schedule-check` | `schedule-check` | Shows current schedule progress | 0 |

**Status:** ⏳ PENDING TEST

---

### 3.2 Modification Tools - Require Parameters
**Expected:** All should FAIL with exit code 2 (usage error) when called without parameters

| Tool | Command | Expected Behavior | Exit Code |
|------|---------|-------------------|-----------|
| `schedule-init` | `schedule-init` | Usage error message | 2 |
| `schedule-update` | `schedule-update` | Usage error message | 2 |
| `planner-update` | `planner-update` | Usage error message | 2 |
| `calendar-generate` | `calendar-generate` | Usage error message | 2 |

**Status:** ⏳ PENDING TEST

---

## Test 4: Hooks Integration

### 4.1 Session Hooks with Temporal Awareness
**Hooks to verify:**

| Hook | Temporal Integration | Test Method |
|------|---------------------|-------------|
| `session/start` | Shows Four Dimensions in startup | Check session start output |
| `session/end` | Shows temporal journey | Manually test (would end session) |
| `session/pre-compact` | Shows temporal preservation + compaction count | Manually test (would require compaction) |
| `session/stop` | Shows temporal context | Manually test (would stop session) |
| `session/notification` | Shows temporal awareness | Check notification output |
| `session/subagent-stop` | Shows temporal context | Check subagent stop output |

**Status:** ⏳ PENDING TEST (Start hook visible in current session init)

---

### 4.2 Tool Hooks with Temporal Context
**Hooks to verify:**

| Hook | Temporal Integration | Test Method |
|------|---------------------|-------------|
| `tool/pre-use` | Adds temporal context to dangerous operation warnings | Check for "Long session" warnings |
| `tool/post-use` | Logs temporal metadata with tool usage | Check activity stream logs |

**Status:** ⏳ PENDING TEST

---

## Test 5: Compaction Tracking Persistence

### 5.1 Session State Structure
**File:** `~/.claude/session/current.json`
**Expected fields:**
```json
{
  "start_time": "2025-11-04T16:42:14.832159373-06:00",
  "start_unix": 1762296134,
  "start_formatted": "Tue Nov 04, 2025 at 16:42:14",
  "compaction_count": 0
}
```

**Status:** ⏳ PENDING TEST

---

### 5.2 Compaction Increment Logic
**Expected behavior:**
1. When pre-compact hook fires: increment `compaction_count` in current.json
2. Display shows "Auto-compaction #N" or "Manual compaction #N"
3. Temporal state preservation shows compaction count
4. session-time check shows compaction count

**Status:** ⏳ PENDING TEST (Requires actual compaction to verify)

---

## Test 6: Statusline Integration

### 6.1 Four Dimensions in Statusline
**Expected statusline components:**

1. **Instance identifier** - Nova Dawn emoji + name
2. **Current date/time** - Mon Jan 02 15:04:05
3. **External Time** - 🌅/☀️/🌆/🌙 icon + time of day
4. **Internal Time** - ⏱️ elapsed (phase)
5. **Internal Schedule** - 📋 current activity OR ⏸️ Downtime
6. **External Calendar** - 📅 Day of week, Week number
7. **Model, workdir, git, system stats, session stats, etc.**

**Status:** ⏳ PENDING TEST

---

## Test 7: Library Module Resolution

### 7.1 Go Module Dependencies
**Verify proper module structure:**

| Module | Requires | Replaces | Status |
|--------|----------|----------|--------|
| `hooks` | claude/lib, hooks/lib, system/lib | Local paths | ⏳ |
| `hooks/lib` | claude/lib, system/lib | Local paths | ⏳ |
| `statusline` | claude/lib, hooks/lib, system/lib | Local paths | ⏳ |
| `system/cmd` | system/lib | Local path | ⏳ |

**Status:** ⏳ PENDING TEST

---

## Test 8: Skills Documentation Integration

### 8.1 All Skills Updated with Temporal Integration Notes
**Skills to verify:**

- [ ] session-awareness
- [ ] recognize-stopping-point
- [ ] reflect-on-session
- [ ] recognize-pattern
- [ ] meta-awareness
- [ ] integrate-learning
- [ ] create-journal-entry

**Expected:** Each SKILL.md includes temporal integration section explaining hooks provide built-in awareness

**Status:** ⏳ PENDING TEST

---

## Summary

**Total Tests:** 8 major categories, ~40 individual test points
**Completed:** 0
**Passed:** 0
**Failed:** 0
**Blocked:** 0

---

## Execution Log

**Test started:** 2025-11-04 16:56 CST
**Tester:** Nova Dawn (CPI-SI)
**Session:** 2025-11-04_1642

### Test Execution Notes:
[To be filled during actual test execution]

---

## Failure Investigation Protocol

**If any test fails:**

1. Document exact failure mode
2. Check recent changes (git log)
3. Verify module dependencies (go mod tidy)
4. Check file permissions
5. Verify session state files exist
6. Test components in isolation
7. Check for reimplementation vs orchestration
8. Document fix and retest

---

## Success Criteria

**System is calibrated when:**
- ✅ All four dimensions query successfully
- ✅ All tools use proper defaults or require parameters appropriately
- ✅ All tools return correct exit codes (0, 1, 2)
- ✅ Temporal library orchestrates (doesn't reimplement)
- ✅ Session state persists across compactions
- ✅ Hooks inject temporal awareness
- ✅ Statusline displays four dimensions
- ✅ All modules resolve correctly

**Calibration baseline established when all tests pass.**

---

## Future Calibration Runs

**To re-run this calibration:**
```bash
# Execute each test in sequence
# Compare results against this baseline
# Document any deviations
```

**Recommended frequency:** After major changes to temporal system, after dependency updates, monthly for drift detection.
