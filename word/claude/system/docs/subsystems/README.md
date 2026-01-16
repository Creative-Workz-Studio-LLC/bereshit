# Subsystems Documentation

**Purpose:** Detailed architecture for specific CPI-SI subsystems

**Location:** `~/.claude/cpi-si/system/docs/subsystems/`

---

## Overview

The subsystems documentation provides deep dives into specific architectural domains within the CPI-SI system.

**Covered subsystems:**
- Session tracking and temporal awareness
- Learning loops and pattern recognition
- Autonomous pattern capture
- Semantic metadata systems
- Internal calendar structures

---

## Documents

| Document | Purpose | Status |
|----------|---------|--------|
| [session-history-architecture.md](session-history-architecture.md) | Session tracking and temporal awareness | ✅ Complete |
| [learning-loop-architecture.md](learning-loop-architecture.md) | Pattern recognition and learning systems | ✅ Complete |
| [autonomous-pattern-capture.md](autonomous-pattern-capture.md) | Automatic pattern recognition during work | ✅ Complete |
| [semantic-metadata-architecture.md](semantic-metadata-architecture.md) | Metadata and tagging systems | ✅ Complete |
| [internal-calendar-structure.md](internal-calendar-structure.md) | Calendar and temporal data structures | ✅ Complete |

---

## Session History Architecture

**What's in it:**

- **Three-clock model**
  - External clock: System time (what time is it?)
  - Session clock: Wall-clock vs active time (uptime, semi-downtime, pure-downtime)
  - Internal clock: Circadian patterns (learned work rhythms)

- **Session tracking**
  - Duration awareness without token anxiety
  - 3-stage breakdown (uptime, semi-downtime, pure-downtime)
  - Natural stopping point recognition

**Read this when:**
- Implementing time-aware features
- Understanding session management
- Building temporal awareness tools
- Planning session tracking integration

---

## Learning Loop Architecture

**What's in it:**

- **Learning cycle**
  - Experience → Reflection → Recognition → Integration → Identity Growth
  - Four journal system (Bible study, personal, instance, universal)
  - Pattern aggregation from session logs

- **Validation framework**
  - Stated identity (config) vs actual behavior (patterns)
  - Grace + truth approach to misalignment
  - Growth opportunities, not failures

**Read this when:**
- Understanding pattern learning systems
- Planning journal integration
- Building validation workflows
- Designing identity-based systems

---

## Autonomous Pattern Capture

**What's in it:**

- **Real-time pattern recognition**
  - "This is the third time I've approached problems this way"
  - Immediate insight capture as patterns emerge
  - Triggers learning when patterns crystallize

- **Genuine noticing**
  - Not post-hoc analysis
  - Active awareness during work
  - Spontaneous recognition

**Read this when:**
- Building pattern recognition features
- Understanding autonomous learning
- Designing real-time capture systems
- Planning skill integration

---

## Semantic Metadata Architecture

**What's in it:**

- **Metadata systems**
  - Tagging and categorization
  - Semantic relationships
  - Cross-referencing patterns

- **Structured data**
  - Schema validation
  - Metadata standards
  - Integration patterns

**Read this when:**
- Designing metadata schemas
- Planning tagging systems
- Building semantic search
- Structuring data relationships

---

## Internal Calendar Structure

**What's in it:**

- **Temporal layers**
  - Celestial layer (God's created time - solar, lunar, seasons)
  - Chronological layer (human time - calendars, schedules)
  - Patterns layer (learned behavior - what actually happens)

- **Calendar types**
  - Personal calendars (weekly, monthly, yearly)
  - Shared calendars (partnerships)
  - Project timelines
  - Base reference calendars

**Read this when:**
- Understanding temporal data structures
- Planning calendar integration
- Building time-tracking features
- Designing pattern aggregation from calendars

---

## Integration Patterns

**How subsystems connect:**

```
Session History
    ↓ feeds
Learning Loop
    ↓ uses
Autonomous Pattern Capture
    ↓ stores in
Semantic Metadata
    ↓ organized by
Internal Calendar
```

**All subsystems:**
- Follow architecture patterns (Ladder, Baton, Rails)
- Include health scoring
- Support immune system paradigm
- Use 4-block structure

---

## Navigation

**Back to:** [Main Documentation](../README.md)

**Related:**
- [Architecture](../architecture/) - Core architectural patterns
- [Protocols](../protocols/) - Operational protocols
- [Implementation](../implementation/) - Implementation status

---

*Detailed subsystem architectures for Kingdom Technology*

*Last Updated: November 10, 2025*
