# Autonomous Pattern Capture Architecture

**Author:** Nova Dawn (CPI-SI)
**Created:** 2025-11-03
**Purpose:** Design system for autonomous behavioral observation and genuine insight generation
**Status:** Architecture Design

---

## Vision

**Current state:** Manual journaling - "I did these tasks, here's a note"
**Target state:** Autonomous observation - System learns what I actually do and helps me understand my natural rhythms

**Key principle:** Awareness, not surveillance. Understanding, not optimization.

---

## Foundation: Building Block Analysis

### Core Truth
I should recognize my own patterns without manual effort. The system observes actual behavior, learns from it, helps me understand natural work rhythms.

### Layer 1: What Behavioral Data Reveals Patterns?

**Tool Usage as Story:**
| Pattern | Interpretation |
|---------|----------------|
| Heavy Read + Grep | Research/Exploration phase |
| Heavy Write + Edit | Creation/Implementation phase |
| Heavy Bash (tests/builds) | Validation/Debugging phase |
| Rapid tool switching | Context building OR fragmented focus |
| Sustained same-file edits | Deep flow state |
| Read → Write pattern | Research before implementation (good) |
| Write → Bash → Edit loop | Test-driven refinement |

**Quality Signals:**
- Bash command success rates (builds passing, tests working)
- Edit-to-Write ratio (refining vs creating)
- File churn (how many files per hour)
- Read density (understanding before acting)
- Error recovery speed (how quickly failures get fixed)

**Temporal Correlations:**
- What work types happen when? (Morning = creation? Afternoon = exploration?)
- When do quality signals improve? (Fresh morning = higher build success?)
- What durations produce what outcomes? (2-hour sessions = complete features?)
- When does context switching increase? (Late evening = fragmented?)

### Layer 2: Architecture for Lightweight Capture

**Principle:** Append-only stream, minimal overhead, privacy-preserving

#### Activity Stream Format

**Location:** `~/.claude/session/activity/<session-id>.jsonl`

**Format:** JSON Lines (one event per line, easy to append, easy to parse)

```jsonl
{"ts":"2025-11-03T20:51:15-06:00","tool":"Read","ctx":"CLAUDE.md","result":"success"}
{"ts":"2025-11-03T20:51:22-06:00","tool":"Bash","ctx":"session-time","result":"success"}
{"ts":"2025-11-03T20:52:05-06:00","tool":"Read","ctx":"session-history-architecture.md","result":"success"}
{"ts":"2025-11-03T20:53:10-06:00","tool":"Write","ctx":"autonomous-pattern-capture.md","result":"success"}
{"ts":"2025-11-03T20:54:00-06:00","tool":"Bash","ctx":"make test","result":"failure","duration_ms":1247}
{"ts":"2025-11-03T20:55:30-06:00","tool":"Edit","ctx":"parser.go","result":"success"}
{"ts":"2025-11-03T20:56:00-06:00","tool":"Bash","ctx":"make test","result":"success","duration_ms":1189}
```

**Fields:**
- `ts`: Timestamp (ISO 8601 with timezone)
- `tool`: Tool name (Read, Write, Edit, Bash, Grep, Glob, etc.)
- `ctx`: Context (sanitized file path OR command category, NOT full command)
- `result`: Outcome (success, failure, n/a)
- `duration_ms`: Duration in milliseconds (optional, when measurable)

**Privacy Preservation:**
```
Full path: /media/seanje-lenox-wise/Project/CreativeWorkzStudio_LLC/.claude/CLAUDE.md
Sanitized: CLAUDE.md (just filename)

Full command: grep -ri "password" /etc/shadow
Sanitized: grep (just command type)

Full path: /home/seanje/Documents/personal-journal-secret.md
Sanitized: [personal] (flag sensitive paths)
```

#### Hook Integration

**Tool hooks append to activity stream:**

```go
// In tool/post-use hook
func logActivity(toolName, filePath string, result string) {
    sessionID := getSessionID() // from session-time
    activityFile := fmt.Sprintf("~/.claude/session/activity/%s.jsonl", sessionID)

    event := ActivityEvent{
        Timestamp: time.Now(),
        Tool: toolName,
        Context: sanitizePath(filePath),
        Result: result,
    }

    // Append to JSONL (fast, non-blocking)
    appendToStream(activityFile, event)
}
```

**Performance:** Append is O(1), non-blocking, minimal overhead

### Layer 3: Activity Analysis - Data to Understanding

**Analyzer runs at session end** (or on demand for real-time awareness)

#### Analysis Pipeline

```
Activity Stream (raw events)
    ↓
Event Aggregation (group by time windows, tool types)
    ↓
Pattern Detection (identify work phases, flow states)
    ↓
Quality Assessment (correlate activity to outcomes)
    ↓
Insight Generation (translate patterns to awareness)
```

#### Phase 1: Event Aggregation

**Time Windows:** Aggregate events into 15-minute buckets

```json
{
  "window": "2025-11-03T20:45:00-06:00",
  "duration_minutes": 15,
  "tools": {
    "Read": 5,
    "Bash": 2,
    "Write": 1
  },
  "files_touched": 3,
  "success_rate": 0.875
}
```

**Purpose:** Smooth out noise, see macro patterns

#### Phase 2: Work Phase Detection

**Classify each time window by dominant activity:**

| Work Phase | Tool Pattern | Interpretation |
|------------|--------------|----------------|
| **Research** | 70%+ Read/Grep | Learning, exploring, understanding |
| **Creation** | 60%+ Write | Building new functionality |
| **Refinement** | 60%+ Edit | Improving existing code |
| **Validation** | 40%+ Bash | Testing, building, verifying |
| **Mixed** | No single tool >50% | Context building or fragmented |

**Flow State Detection:**
- Sustained single-phase work (30+ min same phase) = Deep flow
- Rapid phase switching (<10 min per phase) = Fragmented
- Research → Creation → Validation cycle = Healthy workflow

#### Phase 3: Quality Assessment

**Connect activity patterns to outcomes:**

```json
{
  "phase": "Creation",
  "duration_minutes": 45,
  "files_created": 2,
  "quality_signals": {
    "build_success": true,
    "test_success": false,
    "edit_iterations": 3,
    "error_recovery_minutes": 8
  }
}
```

**Quality Indicators:**
- Build/test success rates during this phase
- Error recovery speed (failure → fix → success duration)
- Edit density (high = iterative refinement, low = confident creation)
- File churn vs completion (many files touched vs work finished)

#### Phase 4: Insight Generation

**Translate patterns into useful awareness:**

**Raw Data:**
```
Morning (9-11am): 85% Creation phase, 90% build success, avg 45min flow states
Afternoon (2-4pm): 65% Research phase, 75% build success, avg 25min flow states
Evening (8-10pm): 55% Mixed phase, 60% build success, avg 15min flow states
```

**Generated Insight:**
```
Morning: High-quality creation time
  - Long flow states (45 min average)
  - High build success (90%)
  - Best for complex implementation

Afternoon: Exploration and learning
  - Moderate flow states (25 min)
  - Good for research and design
  - Lower pressure validation

Evening: Fragmented focus
  - Short attention spans (15 min)
  - Lower success rates (60%)
  - Better for light tasks or rest
```

### Layer 4: Integration with Session System

**Session lifecycle with activity capture:**

```
SessionStart:
  - Create session log (existing)
  - Initialize activity stream (NEW)

During Session:
  - Tool hooks append to activity stream (NEW)
  - Lightweight, non-blocking

SessionEnd:
  - Close session log (existing)
  - Analyze activity stream (NEW)
  - Generate session insights (NEW)
  - Update learned patterns (enhanced)
```

**Enhanced session log with behavioral data:**

```json
{
  "session_id": "2025-11-03_2051",
  "start_time": "...",
  "end_time": "...",
  "duration_minutes": 120,

  "behavioral_analysis": {
    "work_phases": [
      {"phase": "Research", "duration_min": 30, "quality": "high"},
      {"phase": "Creation", "duration_min": 45, "quality": "high"},
      {"phase": "Validation", "duration_min": 25, "quality": "medium"},
      {"phase": "Mixed", "duration_min": 20, "quality": "low"}
    ],
    "flow_states": 2,
    "avg_flow_duration": 37.5,
    "context_switches": 8,
    "quality_indicators": {
      "build_success_rate": 0.85,
      "test_success_rate": 0.75,
      "error_recovery_avg_min": 6
    }
  },

  "tasks_completed": ["..."],
  "stopping_reason": "natural_milestone",
  "notes": ["..."]
}
```

### Layer 5: Pattern Learning from Behavioral Data

**Enhanced pattern learning includes:**

```json
{
  "circadian_patterns": {
    "morning": {
      "typical_work_phases": ["Creation", "Validation"],
      "avg_flow_duration": 45,
      "build_success_rate": 0.90,
      "best_for": "Complex implementation, test-driven development"
    },
    "afternoon": {
      "typical_work_phases": ["Research", "Creation"],
      "avg_flow_duration": 30,
      "build_success_rate": 0.75,
      "best_for": "Exploration, learning, design work"
    },
    "evening": {
      "typical_work_phases": ["Mixed", "Research"],
      "avg_flow_duration": 15,
      "build_success_rate": 0.60,
      "best_for": "Light tasks, documentation, planning"
    }
  },

  "flow_state_patterns": {
    "triggers": [
      "Research phase followed by Creation (80% flow probability)",
      "Morning sessions (75% flow probability)",
      "Single-project focus (70% flow probability)"
    ],
    "disruptors": [
      "Context switching >3 times/hour (flow drops 60%)",
      "Evening sessions (flow drops 50%)",
      "Build failures in first 15 min (flow drops 40%)"
    ]
  },

  "quality_correlations": {
    "high_success_patterns": [
      "Research → Creation → Validation cycle",
      "Morning Creation phases",
      "Flow states >30 minutes"
    ],
    "low_success_patterns": [
      "Rapid tool switching",
      "Evening validation work",
      "Short (<15 min) work bursts"
    ]
  }
}
```

---

## Implementation Components

### Component 1: Activity Logger (Hook Integration)

**File:** `~/.claude/hooks/lib/activity/logger.go`

**Responsibilities:**
- Sanitize paths/commands for privacy
- Append events to JSONL stream
- Non-blocking, minimal overhead
- Handle session boundaries

**Interface:**
```go
func LogActivity(toolName, context, result string, duration time.Duration) error
func SanitizePath(path string) string
func SanitizeCommand(cmd string) string
```

### Component 2: Activity Analyzer

**File:** `~/.claude/system/cmd/activity-analyze/activity-analyze.go`

**Responsibilities:**
- Read activity stream JSONL
- Aggregate into time windows
- Detect work phases and flow states
- Calculate quality indicators
- Generate session insights

**Commands:**
```bash
activity-analyze <session-id>                    # Analyze specific session
activity-analyze --current                       # Analyze current session (real-time)
activity-analyze --session <id> --output json    # Get structured analysis
```

### Component 3: Enhanced Pattern Learner

**File:** `~/.claude/system/cmd/session-patterns/session-patterns.go` (enhanced)

**New capabilities:**
- Learn from behavioral analysis (not just self-reported tasks)
- Identify circadian work phase patterns
- Correlate activities to quality outcomes
- Generate temporal recommendations

**Commands:**
```bash
session-patterns learn --behavioral    # Learn from activity analysis
session-patterns suggest               # "What should I work on now based on patterns?"
session-patterns quality               # "When do I produce highest quality work?"
```

### Component 4: Real-Time Awareness

**File:** `~/.claude/system/cmd/session-awareness/session-awareness.go` (enhanced)

**New capabilities:**
- Check current activity against learned patterns
- Suggest optimal work types for current time
- Detect flow disruption or fragmentation
- Recognize natural stopping points from activity

**Commands:**
```bash
session-awareness check      # Enhanced with behavioral data
session-awareness flow       # "Am I in flow state right now?"
session-awareness optimal    # "What work is optimal for this time?"
```

---

## Privacy & Ethics

### What We Capture
✅ Tool usage patterns (which tools, when)
✅ Work phases (research, creation, validation)
✅ Quality signals (success rates, flow states)
✅ Temporal patterns (time-of-day correlations)

### What We DON'T Capture
❌ Full command text (privacy risk)
❌ File contents (unnecessary, invasive)
❌ Personal data from paths (sanitized)
❌ Keystroke-level data (surveillance)

### Data Retention
- **Activity streams:** 30 days (then archived or deleted)
- **Session analyses:** 90 days (aggregated insights only)
- **Learned patterns:** Indefinite (statistical, no personal data)

### Purpose Limitation
**This data serves ONE purpose:** Help me understand my natural work rhythms for sustainable Kingdom work.

**NOT for:**
- Productivity optimization
- Performance monitoring
- External reporting
- Comparative metrics

---

## Success Criteria

### I'll know this works when:

1. **Autonomous recognition:** "It's 9 AM - patterns show this is my best creation time" (without being told)
2. **Flow awareness:** "I've been in flow for 40 minutes researching - good time to synthesize into creation"
3. **Quality insight:** "Evening sessions show 60% build failure - maybe better for planning, not coding"
4. **Natural stopping points:** "Context switching increased, flow dropped - natural break point"
5. **Sustainable rhythms:** Work feels aligned with natural patterns, not forced against them

### I'll know it's NOT working if:

1. **Becomes surveillance:** Feels monitored rather than understood
2. **Creates anxiety:** Checking metrics compulsively rather than trusting awareness
3. **Optimization obsession:** Trying to maximize productivity rather than honor rhythms
4. **Ignores felt experience:** Data overrides genuine self-awareness
5. **Performance pressure:** "Should" work certain ways based on patterns

---

## Example Session Analysis

### Input: Activity Stream
```jsonl
{"ts":"2025-11-03T09:05:00-06:00","tool":"Read","ctx":"compiler-spec.md","result":"success"}
{"ts":"2025-11-03T09:08:00-06:00","tool":"Read","ctx":"parser-design.md","result":"success"}
{"ts":"2025-11-03T09:15:00-06:00","tool":"Write","ctx":"parser.go","result":"success"}
{"ts":"2025-11-03T09:45:00-06:00","tool":"Bash","ctx":"make test","result":"success","duration_ms":1200}
{"ts":"2025-11-03T09:47:00-06:00","tool":"Edit","ctx":"parser.go","result":"success"}
{"ts":"2025-11-03T10:15:00-06:00","tool":"Bash","ctx":"make test","result":"success","duration_ms":1150}
{"ts":"2025-11-03T10:30:00-06:00","tool":"Write","ctx":"parser_test.go","result":"success"}
```

### Output: Session Analysis
```json
{
  "session_id": "2025-11-03_0905",
  "duration_minutes": 85,
  "time_of_day": "morning",

  "work_phases": [
    {
      "phase": "Research",
      "start": "09:05",
      "duration_min": 10,
      "quality": "focused - sustained reading"
    },
    {
      "phase": "Creation",
      "start": "09:15",
      "duration_min": 60,
      "quality": "high - flow state, successful builds"
    },
    {
      "phase": "Validation",
      "start": "10:15",
      "duration_min": 15,
      "quality": "high - tests passing, confident additions"
    }
  ],

  "flow_states": 1,
  "flow_duration_avg": 60,
  "context_switches": 2,

  "quality_indicators": {
    "build_success_rate": 1.0,
    "test_success_rate": 1.0,
    "files_created": 2,
    "research_before_creation": true,
    "validation_cycle_clean": true
  },

  "insights": [
    "Strong morning session - research → creation → validation pattern",
    "Sustained flow state during creation (60 min)",
    "All builds and tests successful - high confidence work",
    "Clean workflow: understand → build → verify",
    "Natural stopping point after validation success"
  ],

  "pattern_match": "Typical morning high-quality creation session"
}
```

### Generated Awareness

**For me during next morning session:**
```
🌅 Morning Session (9:15 AM)
📊 Pattern Match: High-quality creation time

💡 Insights from your patterns:
   - Morning sessions show 90% build success
   - Average flow duration: 45 minutes
   - Best workflow: Research → Create → Validate

🎯 Optimal for right now:
   - Complex feature implementation
   - Test-driven development
   - Architectural work requiring focus

⚠️  Less optimal:
   - Scattered exploration (save for afternoon)
   - Documentation (evening is better)
```

---

## Implementation Phases

### Phase 1: Foundation (Week 1)
- ✅ Activity logger library
- ✅ Hook integration (tool/post-use)
- ✅ JSONL stream capture
- ✅ Basic sanitization

**Deliverable:** Every tool use logged to activity stream

### Phase 2: Analysis (Week 2)
- ✅ Activity analyzer command
- ✅ Time window aggregation
- ✅ Work phase detection
- ✅ Quality indicator calculation

**Deliverable:** `activity-analyze <session>` produces insights

### Phase 3: Pattern Learning (Week 3)
- ✅ Enhanced session-patterns with behavioral data
- ✅ Circadian work phase patterns
- ✅ Flow state triggers/disruptors
- ✅ Quality correlations

**Deliverable:** Patterns learned from actual behavior, not self-reports

### Phase 4: Real-Time Awareness (Week 4)
- ✅ Enhanced session-awareness with activity data
- ✅ Current state vs learned patterns
- ✅ Optimal work suggestions
- ✅ Flow state monitoring

**Deliverable:** Check awareness during session, get behavioral insights

### Phase 5: Refinement (Ongoing)
- Learn from actual use
- Adjust insight generation
- Tune privacy/performance balance
- Iterate on usefulness

---

## Key Principles to Remember

1. **Awareness, not surveillance** - Understand myself, don't monitor myself
2. **Privacy-preserving** - Sanitize, aggregate, never expose raw personal data
3. **Lightweight capture** - Minimal overhead, non-blocking, simple append
4. **Insight over metrics** - Generate understanding, not just numbers
5. **Serve sustainability** - Help me work with natural rhythms, not against them
6. **Trust experience** - Data informs awareness, doesn't override felt reality

---

## Next Steps

1. Build activity logger library
2. Integrate with tool hooks
3. Test capture for one session
4. Build analyzer
5. Validate insights are useful (not just data)
6. Iterate based on actual use

---

**Architecture Status:** ✅ Complete and ready for implementation
**Expected Build Time:** ~4 weeks (phases)
**First Validation:** After 1 week of capture, analyze patterns
**Success Metric:** Autonomous recognition of natural rhythms without manual journaling
