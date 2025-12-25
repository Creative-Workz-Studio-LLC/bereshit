# Text Buffer Architecture Plan: Config-Driven Split

## Overview

Transform the monolithic `pkg/textbuffer/` into a modular, config-driven system that can scale.

## Current State Analysis

### What We Have
```
pkg/textbuffer/
├── buffer.go   # Core gap buffer (TextBuffer struct, constants)
├── cursor.go   # Movement methods on *TextBuffer
└── undo.go     # UndoStack + TextBufferWithUndo wrapper
```

### The Problem
- `cursor.go` and `undo.go` directly access private fields (`tb.gapStart`, `tb.gapEnd`, `tb.buffer`)
- Constants hardcoded (`defaultGapSize=256`, `minGapSize=64`)
- No configuration layer - can't tune behavior without code changes
- Tightly coupled - can't swap implementations

## Proposed Architecture

### 1. Configuration Layer

Following established patterns from health-scorer and compiler:

```go
// pkg/config/config.go
type TextBufferConfig struct {
    Buffer BufferConfig `toml:"buffer"`
    Cursor CursorConfig `toml:"cursor"`
    Undo   UndoConfig   `toml:"undo"`
}

type BufferConfig struct {
    DefaultGapSize int `toml:"default_gap_size"` // Default: 256
    MinGapSize     int `toml:"min_gap_size"`     // Default: 64
}

type CursorConfig struct {
    WordBoundaryMode string `toml:"word_boundary_mode"` // "unicode" | "vim" | "simple"
}

type UndoConfig struct {
    MaxHistorySize int  `toml:"max_history_size"` // Default: 1000, 0 = unlimited
    GroupInserts   bool `toml:"group_inserts"`    // Group consecutive chars
}
```

Plus: `Default()`, `Load()`, accessor functions following established pattern.

### 2. Interface-Based Package Separation

The key insight: To split packages cleanly, buffer must expose accessors for its internals.

```go
// pkg/buffer/buffer.go - Core buffer with interface
type Buffer interface {
    // Content operations
    Insert(r rune)
    InsertString(s string)
    Delete() bool
    DeleteForward() bool

    // Query operations
    String() string
    Length() int
    RuneAt(pos int) rune
    LineCount() int

    // Cursor position (gap position)
    CursorPosition() int
    MoveGapTo(pos int)

    // For cursor/undo packages - expose controlled access
    GapStart() int
    GapEnd() int
    BufferSlice(start, end int) []rune
}
```

### 3. Module Structure

```
text-buffer/
├── go.mod                    # Main module
├── pkg/
│   ├── config/
│   │   └── config.go         # TextBufferConfig + all sub-configs
│   ├── buffer/
│   │   ├── buffer.go         # Buffer interface
│   │   ├── gap.go            # GapBuffer implementation
│   │   └── buffer_test.go
│   ├── cursor/
│   │   ├── cursor.go         # Cursor operations (takes Buffer interface)
│   │   └── cursor_test.go
│   ├── undo/
│   │   ├── stack.go          # UndoStack (pure data structure)
│   │   ├── tracked.go        # TrackedBuffer wrapper
│   │   └── undo_test.go
│   └── textbuffer/
│       └── textbuffer.go     # Convenience package combining all
└── demos/
    └── ...
```

### 4. Rails Integration Points

Config-driven system creates natural attachment points:

| Rail | Purpose | Example |
|------|---------|---------|
| **Logging** | Buffer operations logging | Each insert/delete can log if enabled |
| **Health Scoring** | Track buffer health | Gap efficiency, undo stack size |
| **Metrics** | Performance tracking | Operation counts, timing |

## Implementation Steps

### Phase 1: Config Foundation
1. Create `pkg/config/config.go` with TextBufferConfig
2. Implement Default(), Load(), accessor pattern
3. Update buffer.go to use config values instead of constants

### Phase 2: Interface Extraction
1. Define Buffer interface in `pkg/buffer/buffer.go`
2. Add accessor methods to GapBuffer (GapStart(), GapEnd(), BufferSlice())
3. Verify existing functionality still works

### Phase 3: Package Split
1. Move cursor operations to `pkg/cursor/` - accept Buffer interface
2. Move undo operations to `pkg/undo/` - accept Buffer interface
3. Create `pkg/textbuffer/` convenience package that wires it all together

### Phase 4: Testing & Demos
1. Update existing tests to work with new structure
2. Update demos to show config usage
3. Add integration tests

## Design Decisions

### Why Interface, Not Exported Fields?
- Controlled access - buffer knows who's accessing what
- Swappable implementations (gap buffer now, rope later?)
- Clear API boundaries
- Testability with mocks

### Why Single Config Package?
- One place to define all tunable behavior
- Single TOML file for users
- Follows health-scorer and compiler patterns
- Easy to extend

### What's NOT Configurable
- Core algorithm (gap buffer mechanics)
- Rune-based operation (not byte-based)
- Unicode support (always on)

These are architectural choices, not user preferences.

## Success Criteria

1. **Modularity**: Each package can be imported independently
2. **Configurability**: Behavior tunable via TOML without code changes
3. **Testability**: Each package testable in isolation
4. **Compatibility**: Existing demos and tests work after refactor
5. **Rails-Ready**: Natural attachment points for logging, health, metrics
