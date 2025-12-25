# Session Data - What Happened

**Purpose:** Track WHAT HAPPENED during sessions (event logs, accomplishments, work done)

## Distinction from Other Layers

| Layer | Focus | Questions Answered |
|-------|-------|-------------------|
| **Temporal** | WHEN work happens | Is now a good time? When are typical work hours? |
| **Session** | WHAT HAPPENED | What was accomplished? What tools were used? How did the session go? |
| **Projects** | WHAT WE'RE BUILDING | What projects exist? What's the progress? What's the velocity? |

## Structure

```
session/
├── activity/           # Event logs (JSONL) - tool usage, timestamps, results
├── history/            # Session summaries (JSON) - what was accomplished
├── current.json        # Active session state
├── current-log.json    # Current session log entry
└── README.md          # This file
```

## What Belongs Here

### Activity Logs (JSONL)
**Format:** Line-delimited JSON (one event per line)
**Lifecycle:** Created during session, archived after completion
**Purpose:** Capture every action during a session

**Example line:**
```json
{"ts":"2025-11-04T17:56:42.501570475-06:00","tool":"PromptSubmit","ctx":"length:0","result":"success"}
```

**Contains:**
- Timestamp (timezone-aware)
- Tool used
- Context
- Result

### History Summaries (JSON)
**Format:** Structured JSON
**Lifecycle:** One file per session, permanent
**Purpose:** Session-level summary for querying and pattern analysis

**Example:**
```json
{
  "session_id": "2025-11-04_1642",
  "start_time": "2025-11-04T16:42:14.833382065-06:00",
  "end_time": "2025-11-04T17:20:48.207889285-06:00",
  "duration_minutes": 38,
  "day_of_week": "Tuesday",
  "time_of_day_category": "afternoon",
  "work_context": "/media/seanje-lenox-wise/Project/CreativeWorkzStudio_LLC",
  "stopping_reason": "Normal session end",
  "quality_indicators": { "tasks_completed": 0 }
}
```

**Contains:**
- Session identification
- Temporal context (when, how long)
- Work context (where, what)
- Stopping reason
- Quality indicators

### Current Session State
**Files:**
- `current.json` - Minimal session state (start time, compaction count)
- `current-log.json` - Session log entry being built

**Lifecycle:** Ephemeral - cleared/moved when session ends
**Purpose:** Track active session before it becomes history

## Data Flow

```
Session Starts
    ↓
current.json created (session state)
current-log.json created (building summary)
    ↓
During Session
    ↓
activity/YYYY-MM-DD_HHMM.jsonl appended (events logged)
    ↓
Session Ends
    ↓
current-log.json → history/YYYY-MM-DD_HHMM.json (summary saved)
activity/YYYY-MM-DD_HHMM.jsonl → archived
current.json cleared
```

## Relationship to Other Layers

### Feeds Temporal Patterns
Session history → aggregated into learned patterns:
- When sessions happen
- How long they last
- Natural stopping points
- Time-of-day quality

### Feeds Project Progress
Session data → project tracking:
- Sessions completed
- Actual uptime hours
- Velocity calculation

### Validated by Configuration
CPI-SI config preferences → session data shows reality:
- "Works better at night" → validate against session times
- "Build-first learner" → validate against work patterns

## Schemas Needed

🔴 **session.schema.json** - Structure for history summary files
🔴 **activity.schema.json** - Structure for JSONL event lines
🔴 **current-session.schema.json** - Structure for current.json state

## Files to Review

⚠️ `paradigm-vs-project-analysis.md` - One-off analysis, should move to reference docs
⚠️ `temp-sleep-log-2025-11-06.txt` - Temporary file, needs proper home or cleanup

---

*Status: Data exists, needs schema formalization and cleanup*
