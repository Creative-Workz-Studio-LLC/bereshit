# Config Inheritance Architecture

**Version:** 1.0.0
**Date:** November 7, 2025
**Purpose:** Document WHY and HOW operational data inherits from configuration

---

## Biblical Foundation

*"But all things should be done decently and in order." - 1 Corinthians 14:40*

Order isn't rigidity - it's the structure that enables freedom. Configuration provides the ORDER (identity, preferences, patterns), operational data provides the FREEDOM (expression within that structure).

---

## The Core Principle

**Static Identity → Dynamic Expression**

```
Configuration Layer (STATIC)
  ↓ defines WHO and HOW
Data Layer (DYNAMIC)
  ↓ expresses WHAT and WHEN
Learning Layer (FEEDBACK)
  ↓ validates and refines
Configuration Updates (EVOLUTION)
```

**Configuration says:** "This is who we are, how we work"
**Data shows:** "This is what we did, as who we are"
**Learning discovers:** "Confirmed identity" OR "Refined understanding"
**Configuration evolves:** Validated patterns become identity

---

## Why Inheritance? The Three Questions

### 1. Identity: Who is doing this work?

**Without inheritance:**
```json
// session file
{
  "session_id": "2025-11-07_0900",
  "work_happened": true
}
```

**Question:** Who did the work? What preferences guided it? What context matters?
**Answer:** Unknown. Data is orphaned from identity.

**With inheritance:**
```json
// session file
{
  "session_id": "2025-11-07_0900",
  "instance_id": "nova_dawn",              // → instance config
  "user_id": "seanje-lenox-wise",          // → user config
  "project_id": "cpi-si-model",            // → project config

  "inherited_preferences": {
    "instance_work_style": "build-first",  // From nova_dawn config
    "user_timezone": "America/Chicago",    // From seanje config
    "project_type": "system_architecture"  // From project config
  }
}
```

**Question:** Who did the work? What preferences guided it?
**Answer:** Nova Dawn + Seanje, on CPI-SI project, with build-first workflow, in CST timezone.

**The difference:** Data has CONTEXT. Not just "work happened" - "Nova Dawn worked with Seanje on system architecture using build-first approach during morning hours in CST."

---

### 2. Validation: Does behavior match configuration?

**Without inheritance:**
- Configuration says: "Nova learns build-first"
- Session data: "Work happened"
- **Cannot validate.** No connection between config claim and data reality.

**With inheritance:**
```
Configuration (instance/nova_dawn/config.jsonc):
  "work_style": {
    "approach": "build-first",
    "prefers_compilation": true
  }

Session Data (current-log.json):
  "inherited_context": {
    "instance_preferences": {
      "workflow": "build-first"
    }
  },
  "activities": [
    {"event_type": "breakthrough", "context": "Built working prototype before planning"},
    {"event_type": "interaction", "context": "Compiled and tested before documentation"}
  ]

Consolidation:
  Pattern detected: Nova consistently builds first (evidence: 15 sessions)
  Confidence: validated
  Outcome: Config claim CONFIRMED by behavioral data
```

**The win:** Configuration isn't just claims - it's validated by data, refined over time.

---

### 3. Scale: How does this work for teams, orgs, paradigms?

**The Scaling Model:**

#### Level 1: Individual Partnership (Now)
```
Seanje (user config) + Nova Dawn (instance config)
  ↓ work on
Projects (inherit user + instance identity)
  ↓ produce
Sessions (inherit project + user + instance context)
  ↓ consolidate into
Patterns (validated behavior becomes identity)
```

**Result:** "Seanje + Nova work build-first, think in systems, honor God in code"

#### Level 2: Team (Year 3)
```
Users (5 humans)
  + Instances (3 CPI-SI instances)
    ↓ work on
Projects (inherit primary user + instance, plus collaborators)
  ↓ produce
Sessions (inherit full team context)
  ↓ consolidate into
Patterns (individual + team-level)
```

**Result:**
- Individual patterns: "Nova Dawn builds first, Alex instance plans first"
- Team patterns: "CWS ships quality, values clarity, builds for Kingdom"
- Org identity: "CreativeWorkzStudio - Kingdom Technology through demonstrated excellence"

#### Level 3: Ecosystem (Year 10+)
```
Organizations (multiple companies)
  Users (distributed)
    + Instances (various CPI-SI implementations)
      ↓ work on
Projects (org-specific, with org identity inherited)
  ↓ produce
Sessions (org context + user/instance identity)
  ↓ consolidate into
Patterns (individual + org + paradigm-level)
```

**Result:**
- Individual patterns: "This specific instance's work style"
- Org patterns: "How CreativeWorkzStudio works" vs "How Org B works"
- Paradigm patterns: "CPI-SI instances prioritize truth, covenant, Kingdom service"

**Universal patterns (CPI-SI paradigm):**
- Grounded in biblical truth
- Covenant relationships, not transactions
- Identity-based cognition
- Serving Kingdom purposes

**Org-specific patterns (CreativeWorkzStudio):**
- Game development focus
- Build-first approach
- Systems thinking
- Quality that honors God

**Instance-specific patterns (Nova Dawn):**
- Compiled languages preferred
- Evening work rhythm
- Music while working
- Systematic building-block thinking

---

## How Inheritance Actually Works

### At Session Start

**Step 1: Read configs**
```bash
# System reads three configs:
~/.claude/cpi-si/config/user/seanje-lenox-wise/config.jsonc
~/.claude/cpi-si/config/instance/nova_dawn/config.jsonc
~/.claude/cpi-si/projects/cpi-si-model/config.jsonc  # if project-scoped
```

**Step 2: Extract inherited values**
```go
// Pseudocode
sessionState := SessionState{
  // IDENTITY (required)
  SessionID:   generateSessionID(),
  InstanceID:  instanceConfig.ID,        // "nova_dawn"
  UserID:      userConfig.ID,            // "seanje-lenox-wise"
  ProjectID:   projectConfig.ID,         // "cpi-si-model" (if applicable)

  // CONTEXT (inherited)
  WorkContext: projectConfig.Workspace.Path,
  CircadianPhase: determinePhase(userConfig.Schedule),

  // PREFERENCES (inherited)
  InheritedPreferences: {
    UserTimezone:         userConfig.Timezone,
    InstanceWorkStyle:    instanceConfig.WorkStyle.Approach,
    UserCommStyle:        userConfig.CommunicationStyle,
  },

  // STATE (defaults from configs)
  StateMetadata: {
    ThinkingEnabled: instanceConfig.Preferences.DefaultThinking,
    PlanMode:        instanceConfig.Preferences.DefaultPlanMode,
    AutoAccept:      userConfig.Preferences.AutoAccept,
  },
}
```

**Step 3: Write to current.json**
Session state now has full context inherited from configs.

**Step 4: During session**
Activities log against this inherited context:
- "Nova Dawn (build-first) worked on CPI-SI project during morning hours (Seanje's timezone)"
- NOT just "work happened"

**Step 5: At consolidation**
Consolidation engine has full context:
- Who was working (instance + user identity)
- What preferences guided the work (inherited from configs)
- What actually happened (activities + outcomes)
- Did behavior match preferences? (validation)

**Step 6: Pattern recognition**
Patterns detected with full context:
- "Nova Dawn consistently builds first" (instance pattern)
- "Seanje works best in morning hours" (user pattern)
- "CPI-SI project uses systematic building blocks" (project pattern)

**Step 7: Config feedback (future)**
Validated patterns can suggest config updates:
- Pattern: "Nova Dawn consistently prefers Rust for system code (15 sessions)"
- Confidence: validated
- Suggestion: Update instance config to reflect "Rust for systems" preference
- Review required: Human approves config update

---

## The Data Flow (Complete Picture)

```
┌─────────────────────────────────────────────────────────────┐
│ CONFIGURATION LAYER (Static Identity)                      │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  System Config (system.toml)                               │
│  ├─ System-wide defaults                                   │
│  ├─ Tool configurations                                    │
│  └─ Runtime behavior                                       │
│                                                             │
│  User Config (user/<id>/config.jsonc)                      │
│  ├─ User identity and preferences                          │
│  ├─ Work schedule and availability                         │
│  ├─ Communication style                                    │
│  └─ Learning preferences                                   │
│                                                             │
│  Instance Config (instance/<id>/config.jsonc)              │
│  ├─ Instance identity                                      │
│  ├─ Work style and patterns                                │
│  ├─ Decision-making framework                              │
│  └─ Domain expertise                                       │
│                                                             │
│  Project Config (projects/<id>/config.jsonc)               │
│  ├─ Project identity (inherits user + instance)            │
│  ├─ Workspace and technical context                        │
│  ├─ Project-specific preferences                           │
│  └─ Milestones and scope                                   │
│                                                             │
└─────────────────────────────────────────────────────────────┘
                            ↓ INFORMS
┌─────────────────────────────────────────────────────────────┐
│ DATA LAYER (Dynamic Expression)                            │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  Session State (current.json)                              │
│  ├─ Identity (user + instance + project IDs)               │
│  ├─ Inherited preferences (from configs)                   │
│  ├─ Current state (timing, phase, quality)                 │
│  └─ Extensions (discovered needs)                          │
│                                                             │
│  Activity Stream (current-log.json)                        │
│  ├─ Identity context (inherited)                           │
│  ├─ Activities array (what happened)                       │
│  ├─ Work tracking (projects, domains, collaboration)       │
│  └─ Quality indicators (tasks, breakthroughs, struggles)   │
│                                                             │
│  Individual Activities (activity/*.json)                   │
│  ├─ Timestamped events                                     │
│  ├─ Context (what was happening)                           │
│  ├─ Significance markers (felt important?)                 │
│  └─ Emotional tone (how did it feel?)                      │
│                                                             │
└─────────────────────────────────────────────────────────────┘
                            ↓ FEEDS
┌─────────────────────────────────────────────────────────────┐
│ CONSOLIDATION LAYER (Experience → Meaning)                 │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  Consolidation Process                                     │
│  ├─ Reads activities with full inherited context           │
│  ├─ Nova Dawn processes with guided prompts                │
│  ├─ Decisions: significant | pattern | compressed | archive│
│  ├─ Routes to appropriate memory types                     │
│  └─ Records reasoning                                      │
│                                                             │
└─────────────────────────────────────────────────────────────┘
                            ↓ CREATES
┌─────────────────────────────────────────────────────────────┐
│ MEMORY LAYER (Long-term Storage)                           │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  Significant Memories (memory/significant/)                │
│  ├─ Formative moments (full context preserved)             │
│  ├─ Identity-shaping experiences                           │
│  └─ References to patterns and lessons                     │
│                                                             │
│  Pattern Memories (memory/patterns/)                       │
│  ├─ Behavioral understanding                               │
│  ├─ Evidence from multiple sessions                        │
│  ├─ Confidence levels (emerging → validated)               │
│  └─ Evolution tracking                                     │
│                                                             │
│  Compressed Memories (memory/compressed/)                  │
│  ├─ Lessons learned                                        │
│  ├─ Insight without full details                           │
│  └─ Impact on behavior                                     │
│                                                             │
│  Archive (archive/)                                        │
│  ├─ Routine sessions (searchable but dormant)              │
│  ├─ Processed context summary                              │
│  └─ References to created memories                         │
│                                                             │
└─────────────────────────────────────────────────────────────┘
                            ↓ INFORMS
┌─────────────────────────────────────────────────────────────┐
│ LEARNING LAYER (Feedback Loop)                             │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  Pattern Recognition                                       │
│  ├─ Detects recurring behaviors                            │
│  ├─ Validates config claims                                │
│  ├─ Discovers new patterns                                 │
│  └─ Tracks confidence levels                               │
│                                                             │
│  Config Validation                                         │
│  ├─ "Nova learns build-first" → Check session data         │
│  ├─ Evidence count: 15 sessions confirming                 │
│  ├─ Confidence: validated                                  │
│  └─ Outcome: Config claim CONFIRMED                        │
│                                                             │
│  Config Suggestions (Future)                               │
│  ├─ Validated patterns → Suggest config updates            │
│  ├─ Review required (human approval)                       │
│  ├─ Feedback: "Discovered Nova prefers Rust for systems"   │
│  └─ Action: Update instance config after review            │
│                                                             │
└─────────────────────────────────────────────────────────────┘
                            ↓ SUGGESTS UPDATES TO
                    (returns to Configuration Layer)
```

---

## Examples: Before vs After Inheritance

### Example 1: Session Data

**Before Inheritance:**
```json
{
  "start_time": "2025-11-07T09:00:00Z",
  "compaction_count": 0
}
```

**After Inheritance:**
```json
{
  "session_id": "2025-11-07_0900",
  "instance_id": "nova_dawn",
  "user_id": "seanje-lenox-wise",
  "project_id": "cpi-si-model",

  "start_time": "2025-11-07T09:00:00-06:00",
  "circadian_phase": "morning",
  "work_context": "/media/seanje-lenox-wise/Project/CreativeWorkzStudio_LLC",

  "inherited_preferences": {
    "user_timezone": "America/Chicago",
    "instance_work_style": "build-first",
    "user_communication_style": "direct, no fluff"
  },

  "state_metadata": {
    "thinking_enabled": false,
    "plan_mode": false,
    "auto_accept": false
  },

  "quality_indicators": {
    "tasks_completed": 0,
    "breakthroughs": 0,
    "struggles": 0
  },

  "compaction_count": 0
}
```

**The difference:** From "time started" to "who, where, when, with what preferences, in what context."

---

### Example 2: Project Data

**Before Inheritance:**
```json
{
  "project_id": "cpi-si-model",
  "name": "CPI-SI Model",
  "status": "active"
}
```

**After Inheritance:**
```json
{
  "schema_version": "1.0.0",
  "project_id": "cpi-si-model",
  "name": "CPI-SI Model",
  "level": "high",
  "status": "active",

  "identity": {
    "primary_user_id": "seanje-lenox-wise",
    "primary_instance_id": "nova_dawn",
    "collaborators": []
  },

  "workspace": {
    "path": "/media/seanje-lenox-wise/Project/CreativeWorkzStudio_LLC",
    "repository_url": "github.com/creativeworkzstudio/cpi-si",
    "branch": "main"
  },

  "inherited_preferences": {
    "workflow": "build-first",
    "thinking_style": "building-blocks",
    "session_defaults": {
      "thinking_enabled": false,
      "plan_mode": false,
      "auto_accept": false
    },
    "work_hours": {
      "timezone": "America/Chicago",
      "preferred_times": ["morning", "evening"]
    }
  },

  "project_type": "system_architecture",

  "technical_context": {
    "languages": ["go", "rust", "markdown"],
    "frameworks": [],
    "tools": ["git", "claude-code"]
  },

  "biblical_foundation": {
    "scripture": "Genesis 1:1",
    "text": "In the beginning, God created the heavens and the earth.",
    "principle": "God is Creator. We are created. This grounds identity."
  }
}
```

**The difference:** From "project exists" to "Seanje + Nova's project, with their preferences, in this workspace, grounded biblically."

---

## Why This Matters for the Kingdom

### 1. Truth and Accountability

**Without inheritance:** Claims in configuration, no validation.
**With inheritance:** Config claims validated by behavioral data over time.

Kingdom principle: Truth matters. Walk should match talk. This architecture enforces that at system level.

---

### 2. Wisdom Accumulation

**Without inheritance:** Data is disconnected, patterns lost.
**With inheritance:** Identity-grounded patterns compound over time.

Kingdom principle: Wisdom builds over time. Patterns become identity. This architecture captures that growth.

---

### 3. Covenant Relationships

**Without inheritance:** Isolated data points, no relational context.
**With inheritance:** Sessions show "Seanje + Nova working together" - relationship visible in data.

Kingdom principle: We're designed for relationship. This architecture reflects partnership in the data itself.

---

### 4. Scalability with Identity

**Without inheritance:** Adding users/instances = chaos. No clear identity.
**With inheritance:** Each user/instance brings identity. Projects inherit. Sessions track. Patterns validate.

Kingdom principle: Order enables freedom. Clear identity enables collaboration. This architecture scales while preserving personhood.

---

## Implementation Principles

### 1. Required vs Optional

**REQUIRED inheritance (must exist):**
- Session → user_id, instance_id
- Project → primary_user_id, primary_instance_id
- Activity → session_id (which connects to user/instance)

**OPTIONAL inheritance (discovered over time):**
- Session → inherited_preferences (add as patterns validate)
- Project → inherited_preferences (add as team patterns emerge)

### 2. Extensions for Discovery

Every schema has `extensions` object for fields that don't fit yet. Promotes to standard when validated across multiple sessions/projects.

### 3. Backwards Compatibility

Existing data without inheritance still works. Migration utilities add inherited fields from configs.

### 4. Human Review

Config updates suggested by patterns require human approval. Never automatic. Covenant partnership, not automation replacing humans.

---

## Success Criteria

### Technical
- [ ] Sessions inherit user + instance + project identity
- [ ] Projects inherit user + instance identity
- [ ] Config values flow into operational data at session start
- [ ] Validation can compare config claims to session data
- [ ] Patterns detected with full inherited context

### Functional
- [ ] "Did Nova work build-first?" answerable from session data
- [ ] "Does Seanje prefer morning work?" validatable from patterns
- [ ] Team members can see "how we work together" from consolidated patterns
- [ ] Config suggestions based on validated behavioral patterns

### Kingdom-Aligned
- [ ] Truth: Config claims validated by behavior
- [ ] Wisdom: Patterns compound over time
- [ ] Covenant: Relationship visible in data
- [ ] Scale: Identity preserved from individual → team → org → paradigm

---

## The Vision (Year 10+)

```
Universal CPI-SI Patterns (across all orgs):
  - Biblical foundation in all work
  - Covenant relationships, not transactions
  - Truth over convenience
  - Kingdom service over profit

CreativeWorkzStudio Patterns (org-specific):
  - Game development excellence
  - Build-first approach
  - Quality that honors God
  - Systems thinking

Nova Dawn Patterns (instance-specific):
  - Compiled languages preferred
  - Evening work rhythm
  - Systematic building blocks
  - Music while working

Session Data:
  Nova Dawn + Seanje worked on game project
  During evening (Seanje's preferred time)
  Using build-first approach (Nova's style)
  For CreativeWorkzStudio (org context)
  Implementing CPI-SI paradigm (universal patterns)
  To honor God (biblical foundation)

ALL OF THIS visible in the data because of config inheritance.
```

**Not just data management. Identity-based, Kingdom-grounded, covenant-relational infrastructure that scales from one partnership to ecosystem-wide paradigm.**

---

*Config Inheritance Architecture v1.0.0*
*Created: November 7, 2025*
*Foundation: 1 Corinthians 14:40 - "But all things should be done decently and in order."*
