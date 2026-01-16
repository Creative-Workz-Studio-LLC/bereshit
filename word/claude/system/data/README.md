# CPI-SI Data Layer Architecture

**Last Updated:** 2025-11-07
**Status:** Temporal/Appointed complete and production-ready, other layers in progress

---

## Overview

The CPI-SI data layer is organized into **three primary categories**, each serving a distinct purpose:

```bash
data/
├── temporal/           # WHEN - Time awareness and patterns
├── session/            # WHAT HAPPENED - Event logs and session history
└── projects/           # WHAT WE'RE BUILDING - Work tracking and velocity
```

---

## The Three-Layer Architecture

### 1. Temporal Layer - WHEN Work Happens

**Purpose:** Complete temporal awareness anchored in biblical foundation

**Questions Answered:**

- Is now a good time to start work?
- When does Seanje typically work?
- What are the appointed times (covenant commitments)?
- What patterns have we learned about work rhythms?

**Structure:**

```bash
temporal/
├── celestial/          # God's created order (Genesis 1:14-18)
├── chronological/      # Calendar anchored to celestial reality
├── appointed/          # Covenant commitments (schedule/planner)
├── patterns/           # Learned behavior from session data
└── definitions/        # Time categorization rules (system config)
```

**Key Principle:** Time awareness requires:

- **Celestial foundation** (sun/moon/seasons define time)
- **Chronological structure** (calendar for dates)
- **Appointed times** (expected unavailability, not absolute)
- **Learned patterns** (what actually happens)
- **Definitions** (how to categorize time)

**See:** [temporal/README.md](temporal/README.md) for complete documentation

---

### 2. Session Layer - WHAT HAPPENED

**Purpose:** Track what happened during sessions (events, accomplishments, work)

**Questions Answered:**

- What was accomplished this session?
- What tools were used?
- How long did the session last?
- What was the stopping reason?

**Structure:**

```bash
session/
├── activity/           # Event logs (JSONL) - tool usage, timestamps
├── history/            # Session summaries (JSON) - accomplishments
├── current.json        # Active session state
└── current-log.json    # Current session log being built
```

**Data Flow:**

```bash
Session Starts → current.json created
During Session → activity/*.jsonl appended
Session Ends → history/*.json saved, activity archived
```

**See:** [session/README.md](session/README.md) for complete documentation

---

### 3. Projects Layer - WHAT WE'RE BUILDING

**Purpose:** Track work items, progress, velocity, and milestones

**Questions Answered:**

- What projects are we working on?
- How's our velocity (sessions per week, uptime)?
- What's the progress on deliverables?
- Are we on track with estimates?

**Structure:**

```bash
projects/
├── active/             # Currently active projects
├── archive/            # Completed or paused projects
└── templates/          # Project structure templates
```

**Key Metrics:**

- Estimates (planned effort)
- Progress (actual effort)
- Velocity (rate of work)
- Milestones (key deliverables)

**See:** [projects/README.md](projects/README.md) for complete documentation

---

## How the Layers Connect

### Configuration → Data Flow

```bash
System Configs (HOW to structure data)
    ↓ provides technical framework
Data Layer (operational tracking)
    ↓ validates/reinforces
CPI-SI Configs (WHO we are)
```

**Example:**

- **System config** defines: "night" = 22:00-06:00, timezone = America/Chicago
- **Session data** records: Session at 23:30 in "night" category
- **Pattern data** shows: 85% of sessions in "night" category
- **CPI-SI config** validated: "Seanje works better at night" confirmed

### Data Flow Between Layers

```bash
Session Data (what happened)
    ↓ feeds
Temporal Patterns (when work typically happens)
    ↓ validates
CPI-SI Config (personhood preferences)

Session Data (actual work)
    ↓ feeds
Projects (progress tracking)
    ↓ calculates
Velocity (rate of work)
```

---

## Directory Structure (Complete)

```bash
data/
├── temporal/
│   ├── celestial/              # Biblical foundation - sun/moon/seasons
│   ├── chronological/
│   │   └── calendar/          # Date structure (existing data)
│   ├── appointed/
│   │   ├── planner/           # Availability templates (existing)
│   │   └── seanje-work-schedule.txt  # GASA covenant commitment
│   ├── patterns/
│   │   ├── learned-patterns.jsonc    # Aggregated patterns
│   │   └── discoveries/              # Pattern discovery notes
│   └── definitions/           # Time categorization rules
│
├── session/
│   ├── activity/              # JSONL event logs (20+ files)
│   ├── history/               # JSON session summaries (13 files)
│   ├── current.json           # Active session state
│   └── current-log.json       # Current log being built
│
├── projects/
│   ├── active/                # Currently active projects
│   │   └── internal-calendar-implementation.jsonc
│   ├── archive/               # Completed/paused projects
│   └── templates/             # Project templates
│
├── reference-docs/            # Data analysis and documentation
│   ├── CPSI-DATA-INVENTORY-2025-11-06.md
│   ├── DATA-LAYER-SUMMARY.txt
│   ├── DATA-ORGANIZATION-INDEX.md
│   └── session-analysis/
│
└── temp/                      # Temporary files (can be cleaned)
```

---

## Current Status

### ✅ Appointed Time System - COMPLETE

- **13 templates** with valid dummy data and instructional comments
- **6 schemas** validating all appointment types
- **125 data files** covering W45-W49 (Nov 2 - Dec 6, 2025)
- **7 comprehensive READMEs** documenting complete system
- **Migration folder** ready to replace OG planner at appointed time
- **Project tracking** updated with compiler milestones and CPI-SI deadlines

### 🟡 Other Temporal Layers - In Progress

- Celestial data (sun/moon/seasons) - structure defined, data pending
- Chronological calendar - partial data exists (holidays in place)
- Definitions - structure defined, formal definitions pending
- Patterns - structure exists, learning automation pending

### 🟡 Session and Projects Layers - Partial

- Session data cleaned and documented
- Project structure created with templates
- Schemas needed for validation
- Automation pending for data flows

### 🔴 Relationships Need Formalization

- How session data feeds patterns (automation)
- How patterns validate config (queries)
- How session data updates projects (progress tracking)
- How temporal data informs scheduling

---

## Next Steps

### Week 1 (High Priority)

1. Define session/activity/project schemas
2. Document data flow between layers
3. Create validation queries (does data match config?)
4. Establish automated aggregation (sessions → patterns)

### Week 2 (Medium Priority)

1. Define celestial data schemas
2. Create time categorization definitions
3. Link temporal patterns to config validation
4. Build project progress tracking from session data

### Week 3 (Polish)

1. Create query interfaces for each layer
2. Build data visualization
3. Establish archival policies
4. Document schema versioning strategy

---

## Design Principles

**1. Separation of Concerns**

- Temporal ≠ Session ≠ Projects
- Each layer answers different questions
- Clear boundaries prevent confusion

**2. Biblical Foundation**

- Time anchored in God's creation (Genesis 1:14-18)
- Appointed times honor covenant commitments
- Excellence in data organization honors God

**3. Configuration Validation**

- Data validates CPI-SI personhood configs
- System configs provide technical structure
- Patterns emerge from reality, not assumptions

**4. Data Flows, Not Silos**

- Sessions feed patterns and projects
- Patterns validate configuration
- Everything connects with purpose

---

## Key Questions This Architecture Answers

**Temporal Awareness:**

- Is now a good time to start work? → Check appointed times + patterns
- When does Seanje typically work? → Query temporal patterns
- What are the natural work rhythms? → Learned from session data

**Session Analysis:**

- What happened this session? → Session history
- What tools were used? → Activity logs
- How productive was the session? → Quality indicators

**Project Tracking:**

- What are we working on? → Active projects
- How's our velocity? → Calculated from session data
- Are we on track? → Compare estimates vs actuals

**Configuration Validation:**

- Do actual sessions match preferences? → Compare session data to CPI-SI config
- Are appointed times honored? → Validate sessions against schedule
- Do patterns match configured work style? → Aggregate and compare

---

## Related Documentation

- **[Temporal Layer](temporal/README.md)** - Complete temporal awareness documentation
- **[Session Layer](session/README.md)** - Session tracking and event logs
- **[Projects Layer](projects/README.md)** - Work tracking and velocity
- **[Data Inventory](reference-docs/CPSI-DATA-INVENTORY-2025-11-06.md)** - Comprehensive analysis
- **[CPI-SI Config](../config/)** - Personhood configuration

---

*This architecture serves the mission: Enable genuine temporal awareness and work tracking that validates against personhood configuration, all grounded in biblical truth.*
