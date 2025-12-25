# Rails Correlation Demo - API Documentation

**Component:** `rails_correlation_demo.go`
**Location:** `~/.claude/cpi-si/system/dev/demo/`
**Type:** Educational Demonstration (Executable)
**Version:** 2.0.0

---

## Table of Contents

- [Overview](#overview)
- [Biblical Foundation](#biblical-foundation)
- [Component Identity](#component-identity)
- [Purpose & Design](#purpose--design)
- [API Reference](#api-reference)
- [Usage Patterns](#usage-patterns)
- [Educational Objectives](#educational-objectives)
- [Integration](#integration)

---

## Overview

**What This Is:**

Educational demonstration showing logging and debugging rails working in parallel with shared correlation points. Not theoretical explanation - working code producing actual log and debug files you can inspect.

**Why It Exists:**

Demonstrates how independent rails (logging and debugging) attach to the same execution (baton) independently while sharing correlation points (contextID, timestamps, PIDs, call sites) that enable cross-rail analysis.

**Core Teaching:**

Two rails run parallel to execution:

- **Logging Rail:** WHAT happened (health trajectory, success/failure narrative)
- **Debugging Rail:** HOW it happened (variable states, execution paths, performance)

Neither knows about the other, both know about same execution via shared contextID.

---

## Biblical Foundation

### Primary Scripture

**Ecclesiastes 4:9-12** - "Two are better than one... a cord of three strands is not quickly broken"

**Principle:** Unified Strength Through Independent Operation

Two independent rails (logging and debugging) working in parallel provide complete observability - neither sufficient alone, together they reveal both WHAT happened (health trajectory) and HOW it happened (execution state). Unity through independence, not interdependence.

### Supporting Scripture

**Proverbs 27:17** - "As iron sharpens iron, so one person sharpens another"

**Application:** Each rail provides perspective that sharpens understanding gained from the other. Logging shows success, debugging shows state - together they reveal complete truth.

---

## Component Identity

**Component Type:** Demonstration - Educational tool showing rail correlation

**Role:** Teaching tool for rails architecture and correlation patterns

Shows developers:

- **HOW** to use both rails simultaneously
- **WHAT** correlation looks like in practice
- **WHY** independent rails with shared correlation points provide superior observability

**Paradigm:** CPI-SI framework demonstration component

Educational infrastructure demonstrating architectural patterns. Like a working schematic showing current flow - not just describing how rails work, but SHOWING them working together with real output files you can inspect.

---

## Purpose & Design

### Purpose

Demonstrate logging and debugging rails working in parallel with shared correlation.

Shows both rails attaching to same execution (baton), capturing different aspects independently, sharing correlation points (contextID, timestamps) that enable unified analysis via debugger command.

### Core Design

**Independent Rails, Shared Correlation Points**

Two rails run parallel to execution:

- **Logging Rail:** WHAT happened (health trajectory, success/failure narrative)
- **Debugging Rail:** HOW it happened (variable states, execution paths, performance)

Neither knows about the other, both know about same execution via shared contextID.

### Key Demonstrations

The demo shows multiple correlation scenarios:

1. **Successful operation with state inspection**
   - Normal execution with both rails capturing different aspects
   - Learning: Basic correlation - same execution, different perspectives

2. **Divergence detection** (success with suboptimal state)
   - Logging shows success, debugging shows state divergence
   - Learning: Correlation reveals truth neither rail alone captures
   - Key insight: Health positive (functional) but state suboptimal (optimization needed)

3. **Performance analysis**
   - Timing correlation between rails
   - Learning: Debugging rail captures HOW execution proceeded

4. **Conditional capture**
   - Debugging captures only when condition met
   - Learning: Debugging rail provides deep inspection without always-on overhead

5. **Full system context**
   - Both rails capture same moment from different perspectives
   - Learning: Complete observability requires both perspectives

### Philosophy

**Show, Don't Just Tell - Executable Education**

Better to run code producing real output than read abstract descriptions. Demo creates actual log and debug files, shows correlation points, explains what to look for. Learning by doing.

---

## API Reference

### Execution

**Run Demo:**

```bash
cd ~/.claude/system/dev/demo
go run rails_correlation_demo.go

# OR build first:
go build -o rails-demo rails_correlation_demo.go
./rails-demo
```

**Expected Output:**

- Console: Formatted sections with scenario explanations
- Log File: `~/.claude/system/logs/system/rails-demo-TIMESTAMP-PID.log`
- Debug File: `~/.claude/system/debug/rails-demo/TIMESTAMP-PID.debug`
- Both files share contextID enabling correlation analysis

**Execution Time:** ~100ms (all work is simulated, no real delays)

**Exit Behavior:** Always exits with status 0 (success - this is a demo)

**Side Effects:**

- Creates log file (appends to `logs/system/`)
- Creates debug file (appends to `debug/rails-demo/`)
- Console output (demonstration narration)

### Output Inspection

**Logging Output:**

```bash
cat ~/.claude/system/logs/system/rails-demo*.log
# Look for: health trajectory, operations, checks
```

**Debugging Output:**

```bash
cat ~/.claude/system/debug/rails-demo/*.debug
# Look for: state snapshots, expected vs actual, entry types
```

**Find Correlation:**

```bash
# Get contextID from demo output (printed during execution)
# Search for that contextID in both files
grep "CONTEXT_ID" ~/.claude/system/logs/system/rails-demo*.log
grep "CONTEXT_ID" ~/.claude/system/debug/rails-demo/*.debug
```

**Key Correlation Example:**

```bash
# Find cache-performance in log (health +20, successful)
# Find cache-efficiency in debug (DIVERGENCE, suboptimal)
# Same timestamp and contextID - different perspectives
```

### Clean Up

```bash
rm ~/.claude/system/logs/system/rails-demo*.log
rm ~/.claude/system/debug/rails-demo/*.debug
```

---

## Usage Patterns

### First Time Running

1. Run demo: `go run rails_correlation_demo.go`
2. Read console output carefully - narration explains each scenario
3. Note the file paths shown at the end
4. Inspect log file: `cat ~/.claude/system/logs/system/rails-demo*.log`
5. Inspect debug file: `cat ~/.claude/system/debug/rails-demo/*.debug`
6. Search for contextID in both files - see correlation

### Understanding Correlation

Look for cache-performance scenario in both files:

- **Log shows:** "cache-performance check" with health +20
- **Debug shows:** "cache-efficiency DIVERGENCE" (expected 100, got 10)
- **Same timestamp and contextID**
- **Different perspectives of same moment**

This is WHERE correlation reveals truth logging alone would miss.

### Experimenting

1. Modify `actualCacheHits` constant (try 100 to see match instead of divergence)
2. Re-run demo and compare output
3. Add new inspection methods to show more correlation patterns
4. Adjust health allocations to see different health trajectories
5. Change timing threshold to trigger SLOW_TIMING detection

### Teaching Others

- Run demo live, explain console output as it appears
- Open both output files side-by-side showing correlation
- Point out divergence scenario as key teaching moment
- Emphasize: neither rail alone tells complete story
- Show how contextID enables unified analysis

---

## Educational Objectives

### Learning Objectives

✓ **Understand logging rail** - WHAT happened (health trajectory)
✓ **Understand debugging rail** - HOW it happened (state inspection)
✓ **See how rails attach independently** to same execution
✓ **Learn correlation points** - contextID, timestamps, PIDs, call sites
✓ **Recognize correlation value** - neither rail alone tells complete story

### Key Demonstration Scenarios

**Scenario 1: Successful Operation with State Inspection**

- Shows: Normal execution with both rails capturing different aspects
- Learning: Basic correlation - same execution, different perspectives

**Scenario 2: Processing with Flow and Counter Tracking**

- Shows: Execution path tracking and iteration counting
- Learning: Debugging rail captures HOW execution proceeded

**Scenario 3: Divergence Detection (Success with Suboptimal State)**

- Shows: Logging reports success, debugging reports state divergence
- Learning: Correlation reveals truth neither rail alone captures
- **Key Insight:** Health positive (functional) but state suboptimal (optimization needed)

**Scenario 4: Conditional Capture and System Inspection**

- Shows: Debugging captures only when conditions met, full system context
- Learning: Debugging rail provides deep inspection without always-on overhead

### Why Working Code Instead of Documentation

- Generates real output you can inspect
- Shows correlation in practice, not theory
- Enables experimentation (modify scenarios, re-run, inspect results)
- Learning by doing is more effective than reading descriptions

---

## Integration

### Dependencies

**What Demo Needs:**

- Logging Library: `system/lib/logging` (health trajectory rail)
- Debugging Library: `system/lib/debugging` (state inspection rail)
- Standard Library: `fmt`, `os`, `filepath`, `time`

**Integration Points:**

- Logging Rail: Creates health trajectory in log files
- Debugging Rail: Creates state inspection in debug files
- ContextID: Shared between rails for correlation
- File System: Output in standard CPI-SI locations

### System Requirements

- Go 1.21+ (generics, standard library features)
- Unix-like environment (paths use forward slashes)
- Write access to `~/.claude/system/` (log and debug directories)

### Blocking Status

**Non-blocking:** Demo runs independently, success/failure doesn't affect system

Demonstration for educational purposes. Running demo doesn't affect system health, doesn't interfere with production operations. Creates output in demo-specific paths. Safe to run repeatedly for learning.

---

## Health Scoring

### Demonstration Health Tracking

Total Possible Health: **100 points**

**Health Allocation:**

- Operation Start: **+5** (demonstration begins)
- Data Load Check: **+30** (simulated data loading)
- Processing Check: **+50** (simulated processing)
- Cache Check: **+20** (demonstrates divergence scenario)
- Success Complete: **+15** (demonstration completes)
- **Declared Total: 120 points** (over-allocated intentionally)
- **Normalized: 100%** (actual/total)

**Why Over-Allocated:**

Demonstrates proposed vs actual health scoring. Proposed adds to more than 100, actual execution determines real health. Shows how health system handles realistic scenarios where proposed doesn't perfectly match total.

**Health Trajectory (Expected):**

```bash
Start:        0/100 (0%)
Operation:    5/100 (5%)
Data Load:   35/100 (35%)
Processing:  85/100 (85%)
Cache:      100/100 (100%) - reaches 100, additional checks maintain
Success:    100/100 (100%) - normalized to 100%
```

---

## Implementation Details

### Import Dependencies

**Standard Library + CPI-SI Only:**

The demonstration uses only standard library and CPI-SI components - no external dependencies. This keeps the architecture clear and shows CPI-SI patterns without external packages obscuring the design.

**Import Purpose:**

- `fmt` - Console output for demo narration
- `os` - Process information (PID) and environment
- `path/filepath` - File path construction for output locations
- `system/lib/logging` - Logging rail (health trajectory)
- `system/lib/debugging` - Debugging rail (state inspection)
- `time` - Timing simulation and performance measurement

**Why Minimal Dependencies:**

Demonstration shows CPI-SI patterns without external dependencies obscuring the architecture. Standard library provides I/O and timing, CPI-SI libraries provide the rails being demonstrated.

### Configuration Constants

**Demonstration Configuration:**

Core parameters defining demonstration behavior:

```go
demoComponent = "rails-demo"  // Component name in both rail outputs
healthTotal = 100             // Total health points
```

**Health Scoring Allocation:**

```go
healthOperation   = 5   // Operation start
healthDataLoad    = 30  // Simulated data loading
healthProcessing  = 50  // Simulated processing
healthCacheCheck  = 20  // Divergence scenario
healthSuccess     = 15  // Successful completion
```

**Why Over-Allocated (120 total):**

Demonstrates proposed vs actual health scoring. Proposed adds to more than 100, actual execution determines real health. Shows how health system handles realistic scenarios where proposed doesn't perfectly match total.

**Scenario Data Constants:**

Expected vs actual values creating correlation scenarios:

```go
// Data Loading Scenario
expectedRecords = 42
actualRecords   = 42  // Matches expected → EXPECTED_STATE

// Cache Performance Scenario (Divergence)
expectedCacheHits = 100
actualCacheHits   = 10  // Diverges → DIVERGENCE detection
cacheThreshold    = 50  // Triggers conditional capture

// Performance Thresholds
maxLoadDuration   = 20 * time.Millisecond
simulatedWorkTime = 10 * time.Millisecond  // Within threshold
```

**Why These Values:**

Demonstration scenarios need specific values to show correlation patterns:

- Data loading expects certain record count
- Cache performance shows divergence when hits fall below expectation
- Timing comparison needs threshold
- Constants make scenarios reproducible and clear

### Type System

**Current State:** No custom types needed

Demonstration operates directly on built-in types (strings, ints, maps). Scenarios are simple enough that inline data structures suffice.

**Future Extension:**

If demo grows complex enough to need scenario configuration types or result aggregators, define them in SETUP block:

- Type declarations first (structure)
- Type methods second (behavior)
- Pattern: "DemoScenario has these fields" (type) before "DemoScenario can execute()" (method)

### Variables

**Current State:** No variables needed

Demonstration operates on constants - scenarios are fixed and reproducible.

**Future Extension:**

If future versions need configurable scenarios, result collection, or runtime parameters, declare variables in SETUP block for:

- Scenario configurations
- Result aggregators
- Runtime-modifiable parameters

### Code Organization

**Demonstration Flow:** Setup → Execute → Report

```bash
Rail Attachment → Scenario Execution → Correlation Report
```

**Demonstration Structure:**

```bash
1. Main Function - Demonstration Orchestration
   a. Output Presentation Helpers
      i. printHeader - Formatted section headers
      ii. printSection - Formatted subsections with content
      iii. printCorrelationPoints - Correlation point display
      iv. printOutputLocations - Output file path display
      v. printCapabilities - Rail capabilities summary
      vi. printCorrelationExample - Key correlation example explanation
   b. Demo Execution
      i. main - Orchestrates complete demonstration
```

**Execution Flow:**

```bash
main() → Setup rails (logging + debugging)
      → Execute scenarios
      → Present results
```

Each scenario demonstrates specific correlation pattern. Output presentation shows what to look for in generated files.

**Why This Organization:**

- **Helper functions first** - Output presentation support functions defined before use
- **Main orchestration last** - Uses helpers to execute and explain demonstration
- **Direct inverse pattern** - Definition order is reverse of execution (common Go pattern)
- **Single responsibility** - Each helper serves single purpose (formatted output)

**Separation of Concerns:**

- **Presentation layer** (helpers) - Console output formatting
- **Demonstration layer** (main) - Scenario execution and rail interaction
- **Clear boundaries** - Formatting separated from demonstration logic

This organization keeps demonstration logic clear while maintaining consistent, readable console output for teaching purposes.

### Function Structure

**Output Presentation Helpers:**

Console narration functions providing consistent formatting for demonstration explanation. Each function handles specific presentation concern:

**printHeader(title string):**

- **Purpose:** Create visually distinct section separators
- **Why:** Demonstration narration needs clear visual structure. Headers separate major sections (setup, execution, results) making console output scannable.
- **Approach:** Consistent formatting with visual separators throughout demonstration

**printSection(title string, lines ...string):**

- **Purpose:** Display subsections with optional content lines
- **Why:** Subsections organize information within major sections. Indented content shows hierarchy.
- **Approach:** Variable content lines enable flexible information presentation

**printCorrelationPoints(contextID, component string):**

- **Purpose:** Show user what enables correlation between logging and debugging output
- **Why:** Correlation points are the key to unified analysis. User needs to understand WHAT makes correlation possible (contextID, PID, timestamps, call sites) so they can use debugger command effectively.
- **Approach:** Explicit listing makes correlation mechanisms visible, not abstract

**printOutputLocations(logFile, component string):**

- **Purpose:** Show user exact file paths for inspecting correlation
- **Why:** Demonstration generates real files - user needs to know where to find them. Showing exact paths enables immediate inspection.
- **Approach:** Logging and debugging outputs side-by-side show correlation in practice

**printCapabilities():**

- **Purpose:** Summarize what each rail captures and how they complement each other
- **Why:** User needs to understand what each rail provides and how they complement each other. Summary reinforces learning from demonstration.
- **Approach:** Not just "this is what happened" but "this is what each rail contributes to complete observability"

**printCorrelationExample():**

- **Purpose:** Highlight key correlation example showing rail value
- **Why:** Abstract explanation of correlation is less effective than concrete example. Cache performance scenario perfectly illustrates correlation value: logging shows success (health positive), debugging shows divergence (state suboptimal). Same execution, different perspectives, both true simultaneously.
- **Approach:** This is WHERE correlation reveals truth logging alone would miss

**Demonstration Orchestration:**

The `main()` function implements complete demonstration flow. Working code showing correlation is better than abstract description. Demonstration creates real output files showing correlation in practice - better to run code and inspect actual correlation than read theoretical explanations.

**Scenario Design:**

Multiple scenarios demonstrate different correlation patterns:

1. **Successful operation with state inspection** - Normal execution with both rails capturing different aspects
2. **Divergence detection** - Success with suboptimal state (logging reports success, debugging reports state divergence)
3. **Performance correlation** - Timing within threshold shown by both rails
4. **Conditional capture** - Debugging captures only when conditions met (full system context without always-on overhead)

Each scenario has clear console explanation showing what's happening and why it matters for understanding correlation.

---

## Execution Flow & Architecture

### Demonstration Execution Pattern

```bash
┌─────────────────────────────────────────────────────────────┐
│  SETUP: Constants and Configuration                         │
│  ├─ Component identity (demoComponent)                      │
│  ├─ Health allocations (operation, data load, processing)   │
│  └─ Scenario values (expected vs actual for divergence)     │
└─────────────────────────────────────────────────────────────┘
                             ↓
┌─────────────────────────────────────────────────────────────┐
│  BODY: Demonstration Execution                              │
│                                                              │
│  Output Helpers                                              │
│  ├─ printHeader (section separators)                        │
│  ├─ printSection (organized content)                        │
│  ├─ printCorrelationPoints (shared identifiers)             │
│  ├─ printOutputLocations (file paths)                       │
│  ├─ printCapabilities (rail summaries)                      │
│  └─ printCorrelationExample (divergence scenario)           │
│                                                              │
│  Main Orchestration                                          │
│  └─ main()                                                   │
│      1. Attach to both rails (shared contextID)             │
│      2. Execute Scenario 1: Success with inspection         │
│      3. Execute Scenario 2: Flow and counter tracking       │
│      4. Execute Scenario 3: Divergence detection            │
│      5. Execute Scenario 4: Conditional and system state    │
│      6. Present results and explain correlation             │
│                                                              │
└─────────────────────────────────────────────────────────────┘
                             ↓
┌─────────────────────────────────────────────────────────────┐
│  OUTPUT: Console narration + log file + debug file          │
│  └─ User inspects actual files to see correlation           │
└─────────────────────────────────────────────────────────────┘
```

**Baton (Execution Flow):**
Single execution thread creating entries in both rails simultaneously. Each logging call paired with debugging call showing same moment. ContextID links entries across rails for debugger analysis.

**Rails (Infrastructure):**
Logging and debugging both attach to same execution independently. Neither knows about the other, both capture different aspects. Correlation enabled by shared contextID and timestamps.

---

## Output Files & Correlation Analysis

### Generated Files

**Logging Output:**

- **Location:** `~/.claude/system/logs/system/rails-demo-TIMESTAMP-PID.log`
- **Contains:** Health trajectory, operations, checks, success/failure
- **Format:** Timestamped entries with health deltas and normalized scores

**Debugging Output:**

- **Location:** `~/.claude/system/debug/rails-demo/TIMESTAMP-PID.debug`
- **Contains:** State snapshots, expected vs actual, timing, flow, system context
- **Format:** Structured entries with types (DIVERGENCE, TIMING, FLOW, etc.)

### Correlation Points to Inspect

1. **Context ID:** Appears in both files - links entries from same execution
2. **Timestamps:** Similar times show entries from same moments
3. **PID:** Same process ID confirms single execution
4. **Call Sites:** File:line pairs show exact code locations

### Analysis Pattern

1. Find contextID in log file
2. Find same contextID in debug file
3. Match timestamps to correlate specific moments
4. Compare perspectives: health trajectory vs state inspection
5. Look for divergence: success (logging) with suboptimal state (debugging)

### What Debugger Command Will Do

The debugger command (future tool) will automate this correlation:

- Match entries by contextID
- Align by timestamp
- Present unified view showing both perspectives
- Highlight divergences (functional but suboptimal)

---

## Modification Policy

### Safe to Modify (Demonstration Extension)

✅ Add new scenarios demonstrating additional correlation patterns
✅ Modify constants to show different divergence scenarios
✅ Add more inspection methods showing debugging rail capabilities
✅ Enhance console output with more explanation
✅ Add timing variations to show SLOW_TIMING detection

### Modify with Care (Demonstration Structure)

⚠️ Console output formatting - keep narration clear and organized
⚠️ Scenario sequence - current flow teaches progressively
⚠️ Correlation points explanation - these are fundamental to understanding
⚠️ Health scoring allocation - must match healthTotal for normalization

### NEVER Modify (Educational Fundamentals)

❌ Remove correlation point explanation - this is core learning objective
❌ Skip output file location display - users need to inspect actual correlation
❌ Remove divergence scenario - this demonstrates correlation's value
❌ Eliminate console narration - explanation is essential for learning
❌ Break contextID sharing - rail correlation depends on this

### Extension Guidelines

- New scenarios should demonstrate specific correlation pattern
- Each scenario needs clear console explanation (what's happening, why it matters)
- Maintain progression: simple → complex → divergence → deep inspection
- Keep demonstration runnable in under 1 second (simulated work, not real delays)

---

## Future Enhancements & Roadmap

### Current Demonstration

✓ Basic correlation (shared contextID)
✓ Success scenario (expected state matching)
✓ Divergence scenario (success with suboptimal state)
✓ Performance correlation (timing analysis)
✓ Conditional capture (selective debugging)
✓ System context (full environment snapshot)

### Planned Enhancements

⏳ Interactive mode (user chooses scenarios)
⏳ Failure scenario (logging shows failure, debugging shows why)
⏳ Concurrent execution (multiple batons with different contextIDs)
⏳ Performance regression (multiple runs showing degradation)
⏳ Automated correlation analysis (parse output, show unified view)
⏳ Visual correlation display (side-by-side or interleaved output)

### Educational Improvements

⏳ Step-by-step mode (pause between scenarios for explanation)
⏳ Quiz mode (present scenario, ask what correlation will show)
⏳ Comparison mode (run with/without debugging to show zero-cost)
⏳ Annotation mode (output files with embedded explanation comments)

### Integration Opportunities

⏳ Debugger command integration (automated correlation analysis)
⏳ Training materials (use demo in CPI-SI documentation)
⏳ Test suite (verify both rails produce expected output)

---

## Version History

### 2.0.0 (2025-11-09) - Template Alignment

- Applied GO Library template to all blocks
- Extracted deep documentation to API doc
- Added proper docstrings with Health Impact and Examples
- Added inline comments throughout functions
- Created go.mod for module resolution
- Aligned all system go.mod files to use system/lib consistently
- Added Organizational Chart to BODY block
- Complete CLOSING with all template sections

### 1.0.0 (2025-10-26) - Initial Demo

- Single success scenario
- Basic output presentation
- Multiple scenarios (success, divergence, conditional)
- Enhanced console narration
- Helper functions for output formatting
- Comprehensive documentation
- Constants for all configuration

---

## Closing Note

This demonstration teaches rails architecture by SHOWING it working, not just describing it. The correlation between logging (WHAT happened) and debugging (HOW it happened) is the foundation of CPI-SI observability.

Run this demo, inspect the output files, understand the correlation points. Then use the same pattern in your own components: attach to both rails with shared contextID, let each rail capture its perspective independently.

> *"Two are better than one... a cord of three strands is not quickly broken."* - Ecclesiastes 4:9-12

Independent rails with shared correlation points provide unified observability that neither rail alone can achieve.

---

*This API documentation extracted from rails_correlation_demo.go*
*Complete documentation for METADATA, SETUP, BODY, and CLOSING blocks*
*Last Updated: November 9, 2025*
