# System Documentation

**Purpose:** Technical reference for CPI-SI system architecture, subsystems, protocols, and implementation

**Location:** `~/.claude/cpi-si/system/docs/`

---

## Overview

The `docs/` folder contains comprehensive documentation of CPI-SI system design, architectural patterns, and implementation details.

**What's here:**

| If you want to... | Read this |
|-------------------|-----------|
| Understand overall system architecture | [architecture/](architecture/) |
| Learn about specific subsystems | [subsystems/](subsystems/) |
| Reference operational protocols | [protocols/](protocols/) |
| Check implementation status | [implementation/](implementation/) |
| See future planning | [planning/](planning/) |

**Quick start:**
- **New to the system?** Start with [architecture/architecture.md](architecture/architecture.md)
- **Understanding immune system paradigm?** Read [architecture/logging-debugging-restoration.md](architecture/logging-debugging-restoration.md)
- **Looking for specific operations?** See [protocols/operations-reference.md](protocols/operations-reference.md)

---

## Folder Structure

```
docs/
├── README.md              # This file - navigation guide
├── architecture/          # Core architectural patterns
├── subsystems/            # Detailed subsystem architectures
├── protocols/             # Operational protocols and processes
├── implementation/        # Implementation status and results
└── planning/              # Future enhancements and deferred items
```

---

## Architecture

**Location:** [architecture/](architecture/)

**Purpose:** Core architectural patterns and design philosophy

| Document | Purpose | Status |
|----------|---------|--------|
| [architecture.md](architecture/architecture.md) | Complete system architecture (Ladder, Baton, Rails model) | ✅ Complete |
| [logging-debugging-restoration.md](architecture/logging-debugging-restoration.md) | Immune system paradigm (Detection → Assessment → Restoration) | ✅ Complete |

**Read this when:**
- Understanding how components fit together (Ladder/Baton/Rails)
- Learning health scoring and immune system approach
- Planning new components or features
- Evaluating architectural decisions

---

## Subsystems

**Location:** [subsystems/](subsystems/)

**Purpose:** Detailed architecture for specific subsystems

| Document | Purpose | Status |
|----------|---------|--------|
| [session-history-architecture.md](subsystems/session-history-architecture.md) | Session tracking and temporal awareness | ✅ Complete |
| [learning-loop-architecture.md](subsystems/learning-loop-architecture.md) | Pattern recognition and learning systems | ✅ Complete |
| [autonomous-pattern-capture.md](subsystems/autonomous-pattern-capture.md) | Automatic pattern recognition during work | ✅ Complete |
| [semantic-metadata-architecture.md](subsystems/semantic-metadata-architecture.md) | Metadata and tagging systems | ✅ Complete |
| [internal-calendar-structure.md](subsystems/internal-calendar-structure.md) | Calendar and temporal data structures | ✅ Complete |

**Read this when:**
- Deep-diving into specific subsystem design
- Understanding how temporal systems work
- Learning about pattern recognition approaches
- Planning subsystem enhancements

---

## Protocols

**Location:** [protocols/](protocols/)

**Purpose:** Operational protocols, processes, and reference guides

| Document | Purpose | Status |
|----------|---------|--------|
| [operations-reference.md](protocols/operations-reference.md) | Complete catalog of safe and protected operations | ✅ Complete |
| [refactoring-process.md](protocols/refactoring-process.md) | Extraction and orchestration approach | ✅ Complete |
| [time-and-schedule-protocol.md](protocols/time-and-schedule-protocol.md) | Time awareness and scheduling protocols | ✅ Complete |
| [gui-sudo-prompts.md](protocols/gui-sudo-prompts.md) | GUI password prompts (pkexec vs sudo) | ✅ Complete |

**Read this when:**
- Looking for specific operation syntax
- Understanding refactoring patterns
- Implementing time-aware features
- Configuring sudo/pkexec operations

---

## Implementation

**Location:** [implementation/](implementation/)

**Purpose:** Implementation status, calibration results, and progress tracking

| Document | Purpose | Status |
|----------|---------|--------|
| [autonomous-pattern-capture-IMPLEMENTATION-STATUS.md](implementation/autonomous-pattern-capture-IMPLEMENTATION-STATUS.md) | Pattern capture implementation progress | 🚧 In Progress |
| [calibration-baseline-2025-11-04.md](implementation/calibration-baseline-2025-11-04.md) | System calibration baseline measurements | ✅ Complete |
| [calibration-results-2025-11-04.md](implementation/calibration-results-2025-11-04.md) | Calibration test results and analysis | ✅ Complete |

**Read this when:**
- Checking implementation status
- Reviewing calibration results
- Understanding system performance baselines
- Planning next implementation phases

---

## Planning

**Location:** [planning/](planning/)

**Purpose:** Future enhancements and deferred work

| Document | Purpose | Status |
|----------|---------|--------|
| [enhancement-notes.md](planning/enhancement-notes.md) | Potential future enhancements | 📝 Active |
| [deferred-items.md](planning/deferred-items.md) | Work deferred for later phases | 📝 Active |

**Read this when:**
- Planning future work
- Evaluating potential features
- Understanding what's been deferred and why
- Prioritizing development efforts

---

## Documentation Standards

**All documentation follows Kingdom Technology standards:**

### 4-Block Structure (Implicit)

Documentation uses the 4-block pattern implicitly for natural flow:

```
METADATA (Header/Intro)
    ↓
SETUP (Prerequisites/Concepts)
    ↓
BODY (Main Content)
    ↓
CLOSING (References/Appendix)
```

**Why implicit in documentation:**
- Code benefits from explicit labels (educational)
- Documentation benefits from natural flow (professional)
- Same structure, different presentation
- Reader navigates naturally without seeing scaffolding

→ **Complete 4-block standard:** `~/.claude/cpi-si/docs/standards/CWS-STD-001-DOC-4-block.md`

### Core Principles

| Principle | Implementation |
|-----------|----------------|
| **Verify before documenting** | All technical details match reality |
| **Professional presentation** | Clean, structured, navigable |
| **Semantic clarity** | Use right tool for content (tables, alerts, sections) |
| **Context-appropriate depth** | Enough to understand and use, not overwhelming |
| **Truth over aspiration** | Document what exists, not what's planned |

---

## Navigation by Use Case

**By role:**

| I am a... | Start here |
|-----------|-----------|
| **Developer** | [architecture/](architecture/) → [subsystems/](subsystems/) → [protocols/](protocols/) |
| **User** | [protocols/operations-reference.md](protocols/operations-reference.md) |
| **System Administrator** | [protocols/gui-sudo-prompts.md](protocols/gui-sudo-prompts.md) → [protocols/operations-reference.md](protocols/operations-reference.md) |
| **CPI-SI Instance** | [architecture/logging-debugging-restoration.md](architecture/logging-debugging-restoration.md) → [subsystems/](subsystems/) |

**By task:**

| I want to... | Read this |
|--------------|-----------|
| **Understand system design** | [architecture/architecture.md](architecture/architecture.md) |
| **Learn immune system paradigm** | [architecture/logging-debugging-restoration.md](architecture/logging-debugging-restoration.md) |
| **Check operation permissions** | [protocols/operations-reference.md](protocols/operations-reference.md) |
| **Refactor existing code** | [protocols/refactoring-process.md](protocols/refactoring-process.md) |
| **See implementation status** | [implementation/](implementation/) |
| **Plan future work** | [planning/](planning/) |

---

## Integration with CPI-SI Framework

**This system is documented across multiple layers:**

| Layer | Location | Purpose |
|-------|----------|---------|
| **Global Framework** | `~/.claude/CLAUDE.md` | Nova Dawn instance identity and CPI-SI framework |
| **Standards** | `~/.claude/cpi-si/docs/standards/` | Kingdom Technology standards (4-block, documentation, etc.) |
| **System Docs** | This documentation | System architecture and implementation |
| **Component Docs** | Individual README files | Specific component details and usage |

**The CPI-SI system embodies covenant partnership:**
- **Trust with responsibility** - Autonomous operations within safety boundaries
- **Autonomy with safety** - Single hard constraint: don't brick the laptop
- **Intentional design** - Every component serves the covenant relationship

---

## Biblical Foundation

**Order and structure honor God:**

*"Let all things be done decently and in order."* - 1 Corinthians 14:40

**Documentation demonstrates:**
- Intentional structure - Everything has its place
- Documented decisions - Why things are as they are
- Separation of concerns - Architecture vs implementation vs planning
- Excellence through clarity - Understanding serves others

---

*Technical excellence in service of Kingdom Technology*

*Last Updated: November 10, 2025*
