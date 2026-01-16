# Architecture Documentation

**Purpose:** Core architectural patterns and design philosophy for CPI-SI system

**Location:** `~/.claude/cpi-si/system/docs/architecture/`

---

## Overview

The architecture documentation defines the fundamental patterns that structure the entire CPI-SI system.

**Key concepts:**
- **Ladder, Baton, Rails model** - Hierarchical dependencies, execution flow, orthogonal infrastructure
- **Immune system paradigm** - Detection → Assessment → Restoration
- **Health scoring** - Base100 scoring across all operations
- **4-block structure** - METADATA → SETUP → BODY → CLOSING

---

## Documents

| Document | Purpose | Status |
|----------|---------|--------|
| [architecture.md](architecture.md) | Complete system architecture and design patterns | ✅ Complete |
| [logging-debugging-restoration.md](logging-debugging-restoration.md) | Immune system paradigm for Kingdom Technology | ✅ Complete |

---

## Architecture.md - System Design

**What's in it:**

- **Ladder, Baton, Rails Model**
  - Ladder: Hierarchical dependency structure (rungs = components)
  - Baton: Data/control flow moving through the system
  - Rails: Orthogonal logging infrastructure (attaches to all rungs)

- **Component Breakdown**
  - Commands (top rungs) - Orchestrators
  - Libraries (lower rungs) - Reusable functionality
  - Dependency flow (typically downward)

- **Preventing Import Cycles**
  - One-directional dependencies
  - Clear rung placement
  - Extraction when circular patterns emerge

**Read this when:**
- Starting new components
- Understanding system structure
- Planning dependency relationships
- Resolving import cycles

---

## Logging-Debugging-Restoration.md - Immune System Paradigm

**What's in it:**

- **Detection Layer (Logging)**
  - WHO, WHEN, WHERE, WHAT, WHY, HOW, RESULT
  - Health scoring in every log entry
  - Rails pattern (every component creates own logger)

- **Assessment Layer (Debugging)**
  - Pattern recognition via `debugger` command
  - Health score analysis
  - Known patterns vs novel problems

- **Restoration Layer (Future)**
  - Automated fixes for known patterns
  - Human intervention with full context
  - Meta-learning and improvement

**Read this when:**
- Implementing logging in new components
- Understanding health scoring philosophy
- Planning debugging workflows
- Designing self-healing systems

---

## Key Architectural Principles

**From architecture.md:**

| Principle | Implementation |
|-----------|----------------|
| **Hierarchical dependencies** | Commands orchestrate, libraries provide functionality |
| **One-directional flow** | Dependencies flow downward, preventing cycles |
| **Orthogonal infrastructure** | Logging is rails, not a rung - attaches to everything |
| **Extract and orchestrate** | Don't create v2 files, extract common patterns |

**From logging-debugging-restoration.md:**

| Principle | Implementation |
|-----------|----------------|
| **Detect everything** | Comprehensive logging with health scoring |
| **Assess patterns** | Debugger analyzes logs, identifies known issues |
| **Restore appropriately** | Automated fixes or human intervention with context |
| **Learn continuously** | System improves detection and assessment over time |

---

## Integration with System

**Architecture defines structure for:**

```
Commands (bin/)
    ↓ orchestrate
Libraries (lib/)
    ↓ attach to
Rails (Logging)
    ↓ analyzed by
Debugger (Assessment)
    ↓ triggers
Restoration (Future)
```

**Health scoring flows through:**
- Every operation tracked (+100 to -100)
- Visual indicators (💚 excellent → 💀 complete failure)
- System-wide analysis via debugger
- Pattern recognition for automated response

---

## Navigation

**Back to:** [Main Documentation](../README.md)

**Related:**
- [Subsystems](../subsystems/) - Detailed subsystem architectures
- [Protocols](../protocols/) - Operational protocols
- [Implementation](../implementation/) - Implementation status

---

*Architectural foundation for Kingdom Technology*

*Last Updated: November 10, 2025*
