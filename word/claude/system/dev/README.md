# CPI-SI Development Folder

**Purpose:** Development-specific data, tooling, and output for CPI-SI system

**Location:** `~/.claude/cpi-si/system/dev/`

---

## Overview

The `dev/` folder contains development artifacts, debug output, demonstrations, and templates that support the development and troubleshooting of the CPI-SI system.

**Key Principle:** Separate development artifacts from operational outputs.

```
dev/                    →  Development-specific content
outputs/                →  Operational outputs (logs, journals)
```

---

## Folder Structure

```
dev/
├── README.md           # This file
├── templates/          # Format templates (logging, debugging)
├── debug/              # Debug output files
├── demo/               # Demonstration programs
├── docs/               # API documentation and guides
└── wrappers/           # Wrapper documentation (intentionally empty)
```

---

## Templates

**Location:** `dev/templates/`

**Purpose:** Define structured formats for logging and debugging to avoid hardcoding in runtime

### Why Templates?

Templates externalize format definitions so the runtime doesn't have hardcoded output formats. This enables:
- Format changes without code changes
- Consistency across all components
- Easy extensibility
- Clear separation of concerns (logic vs presentation)

### Template Files

| File | Purpose |
|------|---------|
| `logging/log-format.jsonc` | Structured logging format for all CPI-SI components |
| `debugging/debug-format.jsonc` | Structured debugging output for all CPI-SI components |

**Example: Before templates (hardcoded)**
```go
// BAD: Format hardcoded in runtime
log.Printf("[%s] %s | %s | %s@%s:%d | HEALTH: %d%% ...",
    timestamp, level, component, user, host, pid, health)
```

**Example: With templates**
```go
// GOOD: Runtime reads format from template
template := LoadLogTemplate("system/dev/templates/logging/log-format.jsonc")
log := template.Format(entry)
```

→ **Full documentation:** [templates/README.md](templates/README.md)

---

## Debug Output

**Location:** `dev/debug/`

**Purpose:** Store debug session output files organized by component

### Structure

```
debug/
├── display/            # Display component debug files
├── environment/        # Environment component debug files
├── status/             # Status component debug files
├── sudoers/            # Sudoers component debug files
├── test/               # Test component debug files
├── unix-safe/          # Unix-safe component debug files
└── validate/           # Validate component debug files
```

### Debug File Format

**Current (minimal):**
- Simple header with component name, context ID, PID, timestamp
- Example: `display-1762306131.debug`

**Future (template-based):**
- Structured output using `templates/debugging/debug-format.jsonc`
- Header → Context → Initial State → Events → Final State → Analysis → Footer
- Multiple debug levels: BASIC, DETAILED, VERBOSE

### Debug Levels

| Level | Sections | Use Case |
|-------|----------|----------|
| **BASIC** | Header, Context, Initial/Final State, Footer | Quick state snapshot |
| **DETAILED** | + Event Timeline | Standard debugging |
| **VERBOSE** | + Automated Analysis | Deep troubleshooting |

---

## Demonstrations

**Location:** `dev/demo/`

**Purpose:** Educational demonstration programs

### Current Demos

| File | Purpose |
|------|---------|
| `rails_correlation_demo.go` | Demonstrates the "rails" concept (orthogonal logging infrastructure) |

**Rails concept:** Logging is NOT a rung on the ladder (dependency) - it's the rails that all rungs attach to. Every component creates its own logger directly, never passing loggers as parameters.

---

## Documentation

**Location:** `dev/docs/`

**Purpose:** API documentation and guides for CPI-SI development tools

### Structure

```
docs/
├── README.md              # Navigation guide for all dev documentation
├── demos/                 # API docs for demonstration programs
├── templates/             # Deep documentation for template formats
├── debug/                 # Debug tooling documentation
└── guides/                # Development guides and how-tos
```

### Documentation Pattern

**Code + API Documentation = Reinforced Learning**

| Location | Contents | Purpose |
|----------|----------|---------|
| **Code** | Standalone practical teaching with docstrings and inline comments | Learn by reading implementation |
| **API Docs** | Deep philosophy, design rationale, extensive explanations | Learn the "why" and architectural decisions |

**Both exist because people learn differently** - not redundancy, intentional multi-path teaching.

**Example (Rails Demo):**
- Code: `demo/rails_correlation_demo.go` (905 lines, self-contained and teachable)
- API Doc: `docs/demos/rails-correlation-demo-api.md` (757 lines, deep philosophy)
- Code reduced 10% by extracting lengthy explanations to API doc
- Both serve learning, different modalities

→ **Full documentation navigation:** [docs/README.md](docs/README.md)

---

## Wrappers

**Location:** `dev/wrappers/`

**Purpose:** Documentation of intentional restraint (why wrappers folder exists but is empty)

### Philosophy

**Comprehensive README documents:** "Build what we need, not what we think we might need"

**Key insight:** Environment variables (DEBIAN_FRONTEND=noninteractive, APT_LISTCHANGES_FRONTEND=none, etc.) handle all interactive scenarios. No wrapper layer needed.

**This is good documentation:** Explaining WHY something doesn't exist prevents future developers from creating unnecessary abstractions.

→ **Full explanation:** [wrappers/README.md](wrappers/README.md)

---

## Connection to Outputs Folder

**Dev folder vs Outputs folder:**

```
dev/                        →  Development artifacts
├── templates/              →  Format definitions
├── debug/                  →  Debug session output
├── demo/                   →  Demonstrations
└── wrappers/               →  Documentation

outputs/                    →  Operational outputs
├── logs/                   →  System logs (formatted per templates/logging/)
└── journals/               →  User journals (Bible study, personal, instance)
```

**Relationship:**

- **Templates (dev/)** define structure
- **Logs (outputs/)** use that structure
- **Debug (dev/)** stores troubleshooting output
- **Journals (outputs/)** have separate structure (not template-based)

**Flow:**

```
Component executes
    ↓
Runtime reads template (dev/templates/)
    ↓
Formats output according to template
    ↓
Writes to:
    - outputs/logs/ (operational logs)
    - dev/debug/ (debug sessions)
```

---

## Development Workflow

### Current State

✅ **Templates defined** - Format structures documented in JSONC

🚧 **Runtime uses hardcoded formats** - Not yet reading templates

🚧 **Debug output minimal** - Simple headers, not using template structure

### Next Steps

**Runtime Integration:**
1. Create template parser library
2. Update logging library to read `templates/logging/log-format.jsonc`
3. Update debugging library to read `templates/debugging/debug-format.jsonc`
4. Refactor components to use template-based formatting
5. Remove hardcoded format strings from runtime

**Benefits after integration:**
- Format changes without code changes or rebuilding
- Consistent output across all components
- Easy to extend with new fields or levels
- Clear separation: runtime = logic, templates = presentation

---

## Design Philosophy

### Separation of Development from Operations

| Type | Location | Purpose |
|------|----------|---------|
| **Development artifacts** | system/dev/ | Debugging, testing, development tools |
| **Operational outputs** | outputs/ | Logs, journals, production data |

**Why separate:**
- Clear boundary between dev and production concerns
- Dev artifacts can be cleaned/regenerated
- Operational outputs are precious data (logs, journals)
- Different backup/retention policies

### Template-Based Formatting

**Core principle:** Don't hardcode presentation in runtime logic

**Benefits:**
- **Maintainability** - Change format without touching code
- **Consistency** - All components use same structure
- **Testability** - Test logic separately from formatting
- **Documentation** - Template IS the format documentation

### Intentional Restraint

**Wrappers folder example:** Document WHY something doesn't exist to prevent unnecessary future work.

**Principle:** Build what you need, document why you don't need what you don't.

---

## Health Scoring Integration

Both logging and debugging templates include health scoring:

**Logging template:**
- Health score in every log entry
- Format: `HEALTH: {percentage}% (raw: {score}, Δ{delta}) {emoji} [{bar}]`
- Visual indicators: 💚 (excellent) → ☠️ (failed)

**Debugging template:**
- Initial state health
- Event-by-event health impact
- Final state health with delta
- Health status assessment

**This enables:** System-wide health analysis via `debugger` command (Assessment layer of immune system)

---

## Integration with System Architecture

**Dev folder fits into CPI-SI architecture:**

```
Ladder (Hierarchical Dependencies)
    ↓
Components execute
    ↓
Attach to Rails (Logging)
    ↓
Use templates (dev/templates/)
    ↓
Write output (outputs/logs/ or dev/debug/)
    ↓
Debugger analyzes (Assessment layer)
```

**The immune system paradigm:**

| Stage | Component | Dev Folder Role |
|-------|-----------|-----------------|
| Detection | Logging | Templates define log structure |
| Assessment | Debugging | Templates define debug structure, debug files enable analysis |
| Restoration | Response | Future - automated fixes based on debug patterns |

---

## Adding New Development Tools

**When to add to dev/:**

- ✅ Development-specific demonstrations
- ✅ Debug output templates
- ✅ Testing utilities
- ✅ Development documentation
- ✅ Format specifications

**When NOT to add to dev/:**

- ❌ Operational tools (those go in system/bin/)
- ❌ Libraries (those go in system/lib/)
- ❌ Logs (those go in outputs/logs/)
- ❌ User data (that goes in outputs/journals/ or data/)

**The boundary:** If it's development-specific and not part of operational runtime, it belongs in dev/.

---

## Biblical Foundation

**Order and structure honor God:**

"Let all things be done decently and in order." (1 Corinthians 14:40)

**Development folder demonstrates:**
- **Intentional structure** - Everything has its place
- **Documented decisions** - Why things are as they are
- **Separation of concerns** - Development vs operations
- **Excellence through restraint** - Building what's needed, documenting what's not

**Template philosophy aligns with Kingdom principles:**
- Consistency reflects Creator's order
- Separation enables testing (stewardship)
- Documentation serves others
- Extensibility without disruption

---

*Development artifacts support operational excellence.*

*Last Updated: November 9, 2025*
