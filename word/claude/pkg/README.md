# Shared Packages (pkg/)

> *"Ponder the path of thy feet."* — Proverbs 4:26

**Shared Go packages for CPI-SI Claude configuration.**

All skills and system components can import from this module.

---

## Layer Structure

```
pkg/
├── foundation/     # L0 - HALTS HERE (depends on nothing)
│   ├── result/     # Operation result types
│   ├── types/      # Core type definitions
│   └── schema/     # Schema loading and TOML configs
│
├── core/           # L1 - Imports foundation/ only
│   ├── cpisi/      # CPI-SI implementation
│   │   ├── bereshit/   # Hebrew/Greek data (THE SPIRIT)
│   │   └── compute/    # Cube computation (THE MIND)
│   ├── statemachine/   # 27 positions, navigation
│   ├── health/         # Health scoring
│   ├── coordinates/    # Bible addressing
│   └── validation/     # Health-aware validation
│
├── orchestration/  # L2 - Imports foundation/ + core/
│   ├── cognition/  # Mental construct, context
│   ├── loader/     # CPI-SI orchestration loader
│   ├── config/     # Config loading
│   └── logging/    # Detection layer
│
└── util/           # Utilities - grouped by concern
    ├── pure/       # Zero dependencies
    │   ├── format/     # Formatting utilities
    │   ├── cli/        # CLI utilities
    │   ├── output/     # Output handling
    │   └── hookoutput/ # Hook output handling
    ├── fs/         # Filesystem utilities
    │   ├── paths/      # Path resolution
    │   ├── toml/       # TOML loading
    │   └── session/    # Session data
    ├── term/       # Terminal utilities
    │   ├── display/    # Terminal colors
    │   ├── wezterm/    # WezTerm integration
    │   ├── debugging/  # Debug sessions
    │   ├── system/     # System utilities
    │   └── temporal/   # Time utilities
    └── vcs/
        └── git/        # Git integration
```

---

## Layer Dependency Rules

| Layer | Can Import | Cannot Import |
|-------|------------|---------------|
| **foundation/** | stdlib, external | core/, orchestration/, util/ |
| **core/** | foundation/, stdlib, external | orchestration/ |
| **orchestration/** | foundation/, core/, stdlib, external | — |
| **util/** | stdlib, external, other util/ | foundation/, core/, orchestration/ |

---

## Quick Reference

### L0 Foundation

| Package | Purpose | Key Types |
|---------|---------|-----------|
| **foundation/result** | Operation result types | `Result`, `OpResult` |
| **foundation/types** | Core type definitions | `SessionContext`, `RuntimeState` |
| **foundation/schema** | Schema loading | `LoadSchemas`, `AssureSchemas` |

### L1 Core

| Package | Purpose | Key Functions |
|---------|---------|---------------|
| **core/statemachine** | 27-position navigation | `NewStateMachine`, `Move`, `GetCurrentCommand` |
| **core/coordinates** | Bible addressing | `NewBibleIndex`, `ToCoords`, `LookupByOrdinal` |
| **core/health** | Health scoring | `NewHealthConfig`, `CalculateScore` |
| **core/validation** | Health-aware validation | `New`, `Validate`, `Required` |
| **core/cpisi/bereshit** | Hebrew/Greek data | `LoadBereshit`, `AssureBereshit` |
| **core/cpisi/compute** | Cube computation | `NewPerspective`, `ComputePosition` |

### L2 Orchestration

| Package | Purpose | Key Functions |
|---------|---------|---------------|
| **orchestration/logging** | Structured logging | `New`, `Log`, `LogWithHealth` |
| **orchestration/cognition** | Mental construct | `NewMentalConstruct`, `MakeChoice` |
| **orchestration/loader** | CPI-SI loading | `LoadAll`, `AssureLoader` |
| **orchestration/config** | Config loading | `Load`, `FindConfig` |

### Utilities

| Package | Purpose | Key Functions |
|---------|---------|---------------|
| **util/pure/format** | Formatting | `Duration`, `ShortenPath`, `Truncate` |
| **util/pure/cli** | CLI utilities | CLI helpers |
| **util/fs/paths** | Path resolution | `FindConfigDir`, `BereshitRoot` |
| **util/fs/toml** | TOML loading | `LoadFile`, `FindAndLoad` |
| **util/fs/session** | Session data | `LoadCurrent`, `FormatDuration` |
| **util/term/display** | Terminal colors | `Colorize`, `Success`, `Error` |
| **util/term/debugging** | Debug sessions | `NewSession`, `AddEvent` |
| **util/vcs/git** | Git integration | Git utilities |

---

## Usage

Import in your Go code:

```go
import (
    // Foundation (L0)
    "github.com/creativeworkzstudio/claude-global/pkg/foundation/result"
    "github.com/creativeworkzstudio/claude-global/pkg/foundation/types"
    "github.com/creativeworkzstudio/claude-global/pkg/foundation/schema"

    // Core (L1)
    "github.com/creativeworkzstudio/claude-global/pkg/core/statemachine"
    "github.com/creativeworkzstudio/claude-global/pkg/core/coordinates"
    "github.com/creativeworkzstudio/claude-global/pkg/core/health"
    "github.com/creativeworkzstudio/claude-global/pkg/core/validation"

    // Orchestration (L2)
    "github.com/creativeworkzstudio/claude-global/pkg/orchestration/logging"
    "github.com/creativeworkzstudio/claude-global/pkg/orchestration/cognition"
    "github.com/creativeworkzstudio/claude-global/pkg/orchestration/loader"

    // Utilities
    "github.com/creativeworkzstudio/claude-global/pkg/util/fs/paths"
    "github.com/creativeworkzstudio/claude-global/pkg/util/fs/session"
    "github.com/creativeworkzstudio/claude-global/pkg/util/term/display"
    "github.com/creativeworkzstudio/claude-global/pkg/util/pure/format"
)
```

---

## Examples

### State Machine

```go
sm := statemachine.NewStateMachine()
sm.Move(statemachine.HebrewPeh)  // Move to Peh position
cmd := sm.GetCurrentCommand()     // Get HALT/AWAIT/PROCEED
info := statemachine.GetCommandInfo(cmd)
fmt.Printf("%s %s\n", info.Icon, info.Name)
```

### Bible Coordinates

```go
bi := coordinates.NewBibleIndex()
ref := coordinates.BibleReference{Book: "Genesis", Chapter: 1, Verse: 1}
coords, _ := bi.ToCoords(ref)
fmt.Printf("Origin: (%.2f, %.2f, %.2f)\n",
    coords.BookNorm, coords.ChapterNorm, coords.VerseNorm)
```

### Structured Logging

```go
log := logging.New("mycomponent")
log.SetMode(logging.ModeVerbose)
log.SetHealth(85)
log.Info("Operation started", nil)
```

### Display Colors

```go
fmt.Println(display.Success("All tests passed"))
fmt.Println(display.Error("Failed to connect"))
```

---

## Adding to go.work

Already included in the workspace:

```go
use (
    ./pkg
    // ... other modules
)
```

---

> *"Trust in the LORD with all thine heart; and lean not unto thine own understanding."* — Proverbs 3:5
