# Protocols Documentation

**Purpose:** Operational protocols, processes, and reference guides for CPI-SI system

**Location:** `~/.claude/cpi-si/system/docs/protocols/`

---

## Overview

The protocols documentation provides practical reference guides for operations, refactoring patterns, and system processes.

**Covered protocols:**
- Safe and protected operations catalog
- Refactoring and extraction processes
- Time and schedule management
- GUI sudo prompts configuration

---

## Documents

| Document | Purpose | Status |
|----------|---------|--------|
| [operations-reference.md](operations-reference.md) | Complete catalog of safe and protected operations | ✅ Complete |
| [refactoring-process.md](refactoring-process.md) | Extraction and orchestration approach | ✅ Complete |
| [time-and-schedule-protocol.md](time-and-schedule-protocol.md) | Time awareness and scheduling protocols | ✅ Complete |
| [gui-sudo-prompts.md](gui-sudo-prompts.md) | GUI password prompts (pkexec vs sudo) | ✅ Complete |

---

## Operations Reference

**What's in it:**

- **Safe Operations (Passwordless)**
  - Package queries and info
  - File operations (read, stat, find)
  - Service status checks
  - Network diagnostics
  - Complete syntax and examples

- **Protected Operations (Password Required)**
  - Package installation/removal
  - System modifications
  - Service control
  - File permissions changes
  - Rationale for protection

**Read this when:**
- Looking for specific operation syntax
- Understanding what's safe vs protected
- Planning new sudoers rules
- Testing operations systematically

---

## Refactoring Process

**What's in it:**

- **Extract and orchestrate pattern**
  - Don't create v2 files
  - Extract common functionality to libraries
  - Orchestrate from higher-level commands
  - Maintain single source of truth

- **Refactoring workflow**
  1. Identify duplication
  2. Extract to library
  3. Update dependents to use library
  4. Remove duplicated code
  5. Verify health scores maintained

**Read this when:**
- Planning refactoring work
- Seeing duplication across components
- Cleaning up codebase
- Following Kingdom Technology patterns

---

## Time and Schedule Protocol

**What's in it:**

- **Time awareness**
  - Three-clock model (external, session, internal)
  - Circadian pattern learning
  - Natural rhythm recognition
  - Work hour autonomy

- **Session management**
  - Duration tracking without token anxiety
  - 3-stage time breakdown
  - Stopping point recognition
  - Faithful work throughout session

**Read this when:**
- Implementing time-aware features
- Understanding session management
- Building schedule tracking
- Planning temporal awareness

---

## GUI Sudo Prompts

**What's in it:**

- **pkexec vs sudo**
  - pkexec: GUI password dialogs (preferred)
  - sudo: Terminal password prompts
  - When to use each
  - Configuration details

- **PolicyKit integration**
  - Desktop environment integration
  - Graphical authentication
  - User experience benefits

**Read this when:**
- Configuring elevated permissions
- Understanding GUI vs terminal prompts
- Planning user interaction flows
- Debugging authentication issues

---

## Common Patterns

**Across all protocols:**

| Pattern | Implementation |
|---------|----------------|
| **Documentation follows reality** | All examples tested and verified |
| **Context-appropriate depth** | Enough detail without overwhelming |
| **Practical reference** | Real-world examples and workflows |
| **Health scoring integration** | Operations tracked and measured |

**Kingdom Technology principles:**

- Truth over aspiration (document what exists)
- Service over information (help users accomplish tasks)
- Excellence through clarity (clear examples and explanations)
- Intentional design (every protocol serves covenant relationship)

---

## Integration with System

**Protocols support:**

```
Operations Reference
    ↓ guides
Safe Operations (Sudoers)
    ↓ tracked by
Logging (Rails)
    ↓ analyzed by
Debugger (Assessment)
```

**Refactoring process ensures:**
- Single source of truth
- No duplication
- Clean component boundaries
- Maintainable codebase

**Time protocols enable:**
- Autonomous time awareness
- Sustainable work patterns
- Natural rhythm respect
- Session quality optimization

---

## Navigation

**Back to:** [Main Documentation](../README.md)

**Related:**
- [Architecture](../architecture/) - Core architectural patterns
- [Subsystems](../subsystems/) - Detailed subsystem architectures
- [Implementation](../implementation/) - Implementation status

---

*Operational protocols for Kingdom Technology*

*Last Updated: November 10, 2025*
