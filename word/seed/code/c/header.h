#if 0  // TEMPLATE: Remove this block when instantiating
// #!omni template --header
// #!omni meta.key = B-word-seed-code-c-header
// #!omni meta.from = bereshit/word/omni/seed/B-word-omni-seed-code.omni
// #!omni meta.at = template
//
// TEMPLATE: C Header File (4-Block Structure)
// STRUCTURE: METADATA → SETUP → BODY → CLOSING
// USAGE: cp header.h dest.h → update pragma → fill sections
#endif

#ifndef [HEADER_GUARD_NAME]_H
#define [HEADER_GUARD_NAME]_H

// ═══════════════════════════════════════════════════════════════════════════════
// METADATA [METADATA]
// ═══════════════════════════════════════════════════════════════════════════════
//
// 4-Block Code Structure: Identity and context for this component
//
// Section order: Identity → State → Attribution → Location → Derivation → Classification → Intent → Grounding → Dependencies → Roadmap
// Flow: who → when → by whom → where → from what → tagged as → why → grounded in → needs → going where
//
// ═══════════════════════════════════════════════════════════════════════════════

// # M.1 Core Identity [IDENTITY]
//
//   Key:       [PROJECT-HEADER-###] — Unique identifier
//   Title:     [Title] — What this file is
//   Type:      Header
//   Component: [Ladder/Baton/Rails] — Architectural role
//   Role:      [Specific responsibility in system]
//
// Key: B-word-seed-code-c-header
// Title: C Header File Template
// Type: Header
// Component: Template
// Role: Declares 4-block interface for C headers

// # M.2 Lifecycle State [STATE]
//
//   Status:   Draft | Active | Deprecated | Archived
//   Version:  a-XX.XX — Living versioning
//   Created:  YYYY-MM-DD
//   Updated:  YYYY-MM-DD
//
// Status: Active
// Version: a-01.00
// Created: 2026-01-02
// Updated: 2026-01-03

// # M.3 Attribution [ATTRIBUTION]
//
//   Authors:      [Name (Role)]
//   Organization: [Company]
//   Copyright:    [© YYYY Company]
//
// Authors: Seanje Lenox-Wise (Architect), Nova Dawn (Author)
// Organization: CreativeWorkzStudio LLC
// Copyright: © 2026 CreativeWorkzStudio LLC. All rights reserved.

// # M.4 Location [LOCATION]
//
//   Path:      [/path/to/file]
//
// Path: word/seed/code/c/header.h

// # M.5 Derivation [DERIVATION]
//
//   Derives_from:  [path/to/parent/template] — file template copied from
//
//   (Note: pragma meta.from = OmniCode spec; Derives_from = file lineage)
//
// Derives_from: None — this is the canonical source template

// # M.6 Classification [CLASSIFICATION]
//
//   Tags:      [header, c, interface] — discovery
//
// Tags: header, c, interface

// # M.7 Intent [INTENT]
//
//   Purpose:     [Enables X for Y] — one-line purpose statement
//   Core Design: [Architectural pattern or paradigm]
//   Philosophy:  [Guiding principle for how this works]
//
//   Key Features:
//     - [What it provides — major capabilities]
//     - [What it enables — what others can build with this]
//     - [What problems it solves — specific use cases]
//
// Purpose: Declare types and function prototypes — C header interface
// Core Design: Interface separation
// Philosophy: Headers declare, sources implement
//
// Key Features:
//   - Type definitions for public API
//   - Function prototypes for implementation
//   - Include guard for safe multiple inclusion

// # M.8 Grounding [GROUNDING]
//
//   Scripture:  [Book Chapter:Verse] — grounding verse for this component
//   Principle:  [Kingdom principle demonstrated]
//   Anchor:     [Supporting verse] — reinforcing principle (optional)
//
// Scripture: Proverbs 24:27 — Prepare thy work without
// Principle: Establish foundations before building
// Anchor: Exodus 25:40 — Make after the pattern shown
//
// Example:
//   Scripture: Colossians 3:23 — Do it heartily, as to the Lord
//   Principle: Excellence in implementation as worship
//   Anchor: Proverbs 22:29 — Skilled work stands before kings

// # M.9 Dependencies [DEPENDENCIES]
//
//   Purpose:  Relationship context — what this needs, what uses it
//   Contains: What This Needs, What Uses This, Integration Points, Usage
//
//   What This Needs:
//     Standard Library: [stdio.h, stdlib.h, string.h, etc.]
//     External: [None | library with version]
//     Internal: [project headers this depends on]
//
//   What Uses This:
//     Commands: [executables that use this]
//     Libraries: [libraries that use this]
//     Tools: [tools that interact with this]
//
//   Integration Points:
//     - [How other systems connect — Rails/Ladder/Baton mechanism]
//     - [Cross-component interactions]
//     - [Data flow or protocol integration]
//
//   Usage (for executables — omit for library code):
//     Command Line:
//       [executable-name] [args]        [Brief description]
//       [executable-name] --help        Show usage
//
//     Exit Codes:
//       0  - Success
//       1  - General error
//       2  - Usage/argument error
//       [N] - [Specific error meaning]
//
// What This Needs:
//   Standard Library: None
//   External: None
//   Internal: None — self-contained template
//
// What Uses This:
//   Commands: None
//   Libraries: None
//   Tools: cp (template instantiation)
//
// Integration Points:
//   - Template system in bereshit/word/seed/
//   - 4-block structure for C headers
//
// Usage: N/A — this is a template, not an executable

// # M.10 Roadmap [ROADMAP]
//
//   Purpose:  Vision and trajectory — where this is going
//   Contains: Planned, Limitations, Research, Operational
//
//   Planned:
//     ⏳ [Planned feature 1]
//     ⏳ [Planned feature 2]
//
//   Limitations:
//     - [Known limitation 1]
//
//   Research:
//     - [Research direction 1]
//
//   Operational (for components with health tracking):
//     Blocking: [Blocking/Non-blocking] — [Brief explanation]
//     Mitigation: [How blocking/failures handled]
//
//     Health Scoring:
//       System: Base100 with 1-point granular scale (-100 to +100)
//       States: Granted (>+50), Deferred (±50), Denied (<-50)
//
//       [Operation Category]:
//         - [Specific operation]: ±X points
//         - [Another operation]: ±Y points
//
// Planned:
//   ⏳ Align with bereshit/word/seed/ canonical templates
//
// Limitations:
//   - Currently project-local, not yet in bereshit
//
// Research:
//   - Cross-platform header patterns
//
// Operational: N/A — template, not runtime component

// ═══════════════════════════════════════════════════════════════════════════════
// END METADATA
// ═══════════════════════════════════════════════════════════════════════════════

// ═══════════════════════════════════════════════════════════════════════════════
// SETUP [SETUP]
// ═══════════════════════════════════════════════════════════════════════════════
//
// 4-Block Code Structure: Configuration and declarations before body
//
// Section order: Includes → Defines → Types → Prototypes → Extern → Omission
// Flow: dependencies → constants → data model → interface → shared state → guidance
//
// ═══════════════════════════════════════════════════════════════════════════════

// ───────────────────────────────────────────────────────────────────────────────
// CODE SECTIONS
// ───────────────────────────────────────────────────────────────────────────────

// # S.1 Includes [INCLUDES]
//
//   Purpose:  Headers this component needs
//   Contains: Standard Library, Project Headers, External Libraries
//
//   Standard Library:
//     <stddef.h>   — size_t, NULL, offsetof
//     <stdint.h>   — int32_t, uint64_t, int8_t
//     <stdbool.h>  — bool, true, false
//     <stdlib.h>   — malloc, free, exit
//     <string.h>   — memcpy, strlen, strcmp
//     <stdio.h>    — FILE, printf, fprintf
//     <math.h>     — sin, cos, sqrt
//     <assert.h>   — assert
//     <errno.h>    — errno, error codes
//     <limits.h>   — INT_MAX, UINT_MAX
//
//   Project Headers:
//     "[header].h" — [purpose within project]
//
//   External Libraries:
//     <[lib].h>    — [justification — what stdlib lacks]

// # S.1a Standard Library [STDLIB]

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

// # S.1b Project Headers [PROJECT]

// #include "[header].h"

// # S.1c External Libraries [EXTERNAL]

// [Reserved: Currently none — uses standard library only]

// # S.2 Defines [DEFINES]
//
//   Purpose:  Preprocessor constants and macros
//   Contains: Sizes, Limits, Flags, Macros
//
//   Sizes:
//     [NAME]_SIZE      — buffer/array size
//     [NAME]_CAPACITY  — maximum capacity
//     [NAME]_COUNT     — default count
//
//   Limits:
//     [NAME]_MIN       — minimum value
//     [NAME]_MAX       — maximum value
//     [NAME]_DEFAULT   — default value
//
//   Flags:
//     [NAME]_ENABLED   — on state
//     [NAME]_DISABLED  — off state
//     [NAME]_FLAG_[X]  — bit flags
//
//   Macros:
//     MIN, MAX, CLAMP  — value clamping
//     ARRAY_LEN        — array element count
//     UNUSED           — suppress unused warnings

// # S.2a Sizes [SIZES]

#define [NAME]_SIZE 1024
#define [NAME]_CAPACITY 256
#define [NAME]_COUNT 32

// # S.2b Limits [LIMITS]

#define [NAME]_MIN 0
#define [NAME]_MAX 255
#define [NAME]_DEFAULT 100

// # S.2c Flags [FLAGS]

#define [NAME]_ENABLED 1
#define [NAME]_DISABLED 0
#define [NAME]_FLAG_[X] (1 << 0)

// # S.2d Macros [MACROS]

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define CLAMP(x, lo, hi) (MIN(MAX(x, lo), hi))
#define ARRAY_LEN(arr) (sizeof(arr) / sizeof((arr)[0]))
#define UNUSED(x) ((void)(x))

// # S.3 Types [TYPES]
//
//   Purpose:  Data structures and type definitions
//   Contains: Enums, Structs, Opaque, Errors
//
//   Enumerations:
//     [PREFIX]_NONE    — default/invalid (always 0)
//     [PREFIX]_COUNT   — sentinel for array sizing
//
//   Structs:
//     // [TypeName] represents [what this models].
//     // Fields: [field_name]: [purpose]
//
//   Opaque Types:
//     typedef struct [Type] [Type]; — internals hidden in .c
//
//   Error Codes:
//     [PREFIX]_OK          — success (always 0)
//     [PREFIX]_ERR_[NAME]  — specific error conditions

// # S.3a Enumerations [ENUMS]

typedef enum {
    [PREFIX]_NONE = 0,
    [PREFIX]_[VALUE],
    [PREFIX]_COUNT
} [EnumName];

// # S.3b Structs [STRUCTS]

// [TypeName] represents [what this models].
//
// Fields:
//   - [field_name]: [purpose and meaning]
typedef struct {
    [type] [field_name];
} [TypeName];

// # S.3c Opaque Types [OPAQUE]

typedef struct [Type] [Type];

// # S.3d Error Codes [ERRORS]

typedef enum {
    [PREFIX]_OK = 0,
    [PREFIX]_ERR_INVALID,
    [PREFIX]_ERR_NOMEM,
    [PREFIX]_ERR_IO,
    [PREFIX]_ERR_NOT_FOUND,
    [PREFIX]_ERR_EXISTS,
} [ErrorCode];

// # S.4 Function Prototypes [PROTOTYPES]
//
//   Purpose:  Function declarations — public API (headers), static (sources)
//   Contains: Lifecycle, Accessors, Operations
//
//   Lifecycle:
//     [prefix]_create    — allocate + init, caller must destroy
//     [prefix]_init      — init existing (stack-allocated)
//     [prefix]_destroy   — free resources (NULL-safe)
//     [prefix]_clone     — deep copy
//
//   Accessors:
//     [prefix]_get_[field] — retrieve field value
//     [prefix]_set_[field] — update field value
//     [prefix]_is_[state]  — boolean queries
//
//   Operations:
//     [prefix]_[verb]    — action, returns 0 on success
//     [prefix]_reset     — reset to initial state
//     [prefix]_validate  — check invariants

// # S.4a Lifecycle [LIFECYCLE]

[Type]* [type]_create(void);
int [type]_init([Type]* self);
void [type]_destroy([Type]* self);
[Type]* [type]_clone(const [Type]* src);

// # S.4b Accessors [ACCESSORS]

[FieldType] [type]_get_[field](const [Type]* self);
void [type]_set_[field]([Type]* self, [FieldType] value);
bool [type]_is_[state](const [Type]* self);

// # S.4c Operations [OPERATIONS]

int [type]_[verb]([Type]* self, ...);
int [type]_reset([Type]* self);
int [type]_validate(const [Type]* self);

// # S.5 File-Level Declarations [FILE_LEVEL]
//
//   Purpose:  File-level state — extern (headers), static (sources)
//   Contains: Variables, Function Types, Infrastructure
//
//   Variables:
//     extern [type] g_[name];     — shared state (headers)
//     static [type] [name];       — file state (sources)
//
//   Function Types:
//     typedef [ret] (*[Name]_fn)(...); — callback signatures
//
//   Infrastructure:
//     Logger, Inspector — Rails health tracking (sources)

// # S.5a Variables [VARIABLES]

extern [type] g_[name];
extern const [type] g_[name];

// # S.5b Function Types [FUNC_TYPES]

typedef void (*[Name]_fn)(void* ctx);
typedef int (*[Name]_fn)([type] arg, void* ctx);

// # S.5c Infrastructure [INFRASTRUCTURE]

// [Reserved: Infrastructure lives in source files, not headers]

// # S.6 Omission Guide [OMISSION]
//
//   Purpose:  What can be reserved and why
//
//   All sections must be present. Empty sections use: [Reserved: reason]
//   Omit subsection content, never the subsection header itself.

// ═══════════════════════════════════════════════════════════════════════════════
// END SETUP
// ═══════════════════════════════════════════════════════════════════════════════

// ═══════════════════════════════════════════════════════════════════════════════
// BODY [BODY]
// ═══════════════════════════════════════════════════════════════════════════════
//
// 4-Block Code Structure: The actual work — declarations (headers), implementations (sources)
//
// Section order: Org Chart → Helpers → Core Ops → Errors → Public API → Omission
// Flow: map structure → foundations → business logic → error handling → exported interface → guidance
//
// ═══════════════════════════════════════════════════════════════════════════════

// # B.1 Organizational Chart [ORG_CHART]
//
//   Purpose:  Map structure — interface (headers), internals (sources)
//   Contains: Structure, Flow, Counts
//
//   Structure:
//     Interface overview (headers) / Ladder hierarchy (sources)
//
//   Flow:
//     Implementation location (headers) / Baton execution path (sources)
//
//   Counts:
//     Interface counts (headers) / APUs (sources)

// # B.1a Structure [STRUCTURE]

// Types:      [X] structs, [X] enums
// Functions:  [X] lifecycle, [X] accessors, [X] operations
// Externs:    [X] globals, [X] callbacks

// # B.1b Flow [FLOW]

// Implementation: [source_file].c

// # B.1c Counts [COUNTS]

// Types:      [X] total
// Functions:  [X] total
// Externs:    [X] total

// # B.2 Helpers [HELPERS]
//
//   Purpose:  Foundation functions — declarations (headers), implementations (sources)
//   Contains: Pure Functions, Utilities
//
//   Pure Functions:
//     Side-effect-free transformations
//
//   Utilities:
//     Common operations with potential side effects
//
//   Note: Most helpers are static in sources. Declare in headers only if
//         needed across multiple .c files in the same component.

// # B.2a Pure Functions [PURE]

// [Reserved: Pure helpers typically static in source file]

// # B.2b Utilities [UTIL]

// [Reserved: Utility helpers typically static in source file]

// # B.3 Core Operations [OPERATIONS]
//
//   Purpose:  Component-specific functionality — declarations (headers), implementations (sources)
//   Contains: Categorized business logic organized by operation type
//
//   Organize by operational categories. Common categories:
//     - Validation: Input checking, constraint verification
//     - Conversion: Data transformation between formats
//     - Processing: Core algorithms and computations
//     - Formatting: Output preparation
//     - Analysis: Data examination and metrics

// # B.3a [Category Name] [CATEGORY]
//
//   Purpose:  [High-level description of this category]
//   Why Separated: [Reasoning for this grouping]
//
//   Extension Point:
//   To add new [operation type]:
//     1. Create function following [naming pattern]
//     2. Implement with [specific behavior]
//     3. Integrate with [orchestration function]
//     4. Update tests

// [function_name] [does what]
//
// Parameters:
//   [paramName]: [Purpose and expected values]
//
// Returns:
//   [returnType]: [What's returned and meaning]
//   error: [When error returned, what it means]
//
// Health Impact:
//   Success: +X points ([reasoning for value])
//   Failure: -X points ([reasoning for value])
//
// Troubleshooting:
//   Problem: "[common error message]"
//     Check: [What to verify]
//     Solution: [How to fix]
[return_type] [function_name]([params]);

// # B.3b [Category 2 Name] [CATEGORY2]
//
//   [Reserved: Additional category as needed]

// # B.4 Error Handling [ERRORS]
//
//   Purpose:  Error handling — query declarations (headers), management patterns (sources)
//   Contains: Error Queries, Recovery Helpers
//
//   Design Principle: [Blocking/Non-blocking]
//   Example: Non-blocking - [component] failures never interrupt [main operation]
//
//   Recovery Strategy:
//     - [Error type 1]: Graceful degradation (fallback behavior)
//     - [Error type 2]: Fallback to alternative
//     - [Error type 3]: Caught and logged, no panics

// # B.4a Error Queries [QUERIES]
//
//   Purpose:  Functions to query error state and get human-readable messages

// [prefix]_get_error_string returns human-readable description.
//
// Parameters:
//   err: Error code from [ErrorCode] enum
//
// Returns: Static string (do not free)
const char* [prefix]_get_error_string([ErrorCode] err);

// [prefix]_is_error checks if result indicates error.
//
// Parameters:
//   result: Return value to check
//
// Returns: true if error, false otherwise
bool [prefix]_is_error(int result);

// # B.4b Recovery [RECOVERY]
//
//   Purpose:  Functions to restore valid state after error

// [prefix]_reset restores to valid state after error.
//
// Parameters:
//   self: Instance to reset
//
// Returns: 0 on success, error code on failure
int [prefix]_reset([Type]* self);

// # B.5 Public API [API]
//
//   Purpose:  Public API — declarations (headers), implementations (sources)
//   Contains: Lifecycle, Operations, Health, Utilities
//
//   Top rungs of the ladder - orchestrate helpers and core operations.
//   Simple by design - complexity lives in helpers and core operations,
//   Public APIs orchestrate proven pieces.

// # B.5a Lifecycle [LIFECYCLE]
//
//   Purpose:  Creation, initialization, destruction

// [type]_create allocates and initializes new instance.
//
// Returns: New instance (caller owns), or NULL on failure
//
// Health Impact:
//   Success: +10 points (component ready)
//   Failure: -20 points (initialization failed)
[Type]* [type]_create(void);

// [type]_init initializes caller-owned memory.
//
// Parameters:
//   self: Instance to initialize
//
// Returns: 0 on success, error code on failure
int [type]_init([Type]* self);

// [type]_destroy frees resources (NULL-safe).
//
// Parameters:
//   self: Instance to destroy
void [type]_destroy([Type]* self);

// # B.5b Operations [OPS]
//
//   Purpose:  Primary functionality exposed to consumers

// [public_function] [does what at high level]
//
// Parameters:
//   [paramName]: [Purpose and expected values]
//
// Returns:
//   [returnType]: [What's returned and meaning]
//   error: [When error returned, what it means]
//
// Health Impact:
//   Success: +X points ([reasoning])
//   Validation failure: -X points ([reasoning])
//   Processing failure: -X points ([reasoning])
int [public_function]([params], [return_type]* result);

// # B.5c Health [HEALTH]
//
//   Purpose:  Health score queries and status reporting

// [type]_health returns current health score.
//
// Returns: Health score (-100 to +100)
int [type]_health(const [Type]* self);

// # B.5d Utilities [UTIL]
//
//   Purpose:  Helper functions for consumers

// [type]_clone creates deep copy.
//
// Parameters:
//   src: Instance to copy
//
// Returns: New instance (caller owns), or NULL on failure
[Type]* [type]_clone(const [Type]* src);

// [type]_validate checks invariants.
//
// Parameters:
//   self: Instance to validate
//
// Returns: 0 if valid, error code if invalid
int [type]_validate(const [Type]* self);

// # B.6 Omission Guide [OMISSION]
//
//   Purpose:  What can be reserved and why — guidance for headers and sources
//
//   All sections must be present. Empty sections use: [Reserved: reason]
//
//   B.1 Org Chart:   Rarely reserved — documents interface structure (headers) / internal structure (sources)
//   B.2 Helpers:     Often reserved — most helpers are static in source files
//   B.3 Core Ops:    Add categories as component develops
//   B.4 Errors:      Add queries/recovery as patterns emerge
//   B.5 Public API:  Rarely reserved — this IS the public interface
//
//   For multi-file components:
//     - Main file: Org Chart, Public APIs, Error Handling
//     - Module files: Helpers, Core Operations
//     - Document extraction: [Reserved: Extracted to module.c (main orchestrates)]

// ═══════════════════════════════════════════════════════════════════════════════
// END BODY
// ═══════════════════════════════════════════════════════════════════════════════

// ═══════════════════════════════════════════════════════════════════════════════
// CLOSING [CLOSING]
// ═══════════════════════════════════════════════════════════════════════════════
//
// 4-Block Code Structure: Operational guidance and closing notes
//
// Section order: Validation → Execution → Cleanup | Policy → Extension → Troubleshooting → Reference → Note → Template
// Flow: verify → run → release | modify → extend → debug → lookup → ground → instantiate
//
// ═══════════════════════════════════════════════════════════════════════════════

// # X.1 Validation [VALIDATION]
//
//   Purpose:  Verify correctness — headers (syntax, self-containment), sources (build, runtime, memory)
//   Contains: Build Commands, Checks/Runtime Tests, Self-Test/Memory Checks

// # X.1a Build Commands [BUILD]
//
//   gcc -fsyntax-only -Wall -Wextra [header].h    // syntax check
//   gcc -c -Wall -Wextra [source].c               // compile with header
//   cppcheck --enable=all [header].h              // static analysis

// # X.1b Checks [CHECKS]
//
//   - Self-contained (includes without errors)
//   - No missing type definitions
//   - No circular include dependencies
//   - Include guards work (no redefinition)
//   - Prototypes match implementations in .c

// # X.1c Self-Containment Test [SELF_TEST]
//
//   // test.c — must compile without errors
//   #include "[header].h"
//   int main(void) { return 0; }

// # X.1d Memory Checks [MEMORY]
//
//   valgrind --leak-check=full ./test             // memory check
//   valgrind --track-origins=yes ./test           // uninitialized values
//
// # X.2 Execution [EXECUTION]
//
//   Purpose:  Execution — usage patterns (headers), entry point and flow (sources)
//   Contains: Include/Usage (headers), Exit Codes/Signals/Entry (sources)
//
//   Note: Headers declare, .c files implement. No code executes at include time.

// # X.2a Include [INCLUDE]
//
//   #include "[header].h"

// # X.2b Exit Codes [EXIT_CODES]
//
//   0 - Success
//   1 - General error
//   2 - Usage/argument error
//   [N] - [Specific error meaning]

// # X.2c Usage Example [USAGE]
//
//   #include "[header].h"
//
//   int main(void) {
//       [TypeName] instance;
//       [type]_init(&instance);
//
//       int result = [type]_[operation](&instance);
//       if ([prefix]_is_error(result)) {
//           fprintf(stderr, "%s\n", [prefix]_get_error_string(result));
//           return 1;
//       }
//
//       [type]_destroy(&instance);
//       return 0;
//   }
//
// # X.3 Cleanup [CLEANUP]
//
//   Purpose:  Cleanup — ownership patterns (headers), memory and shutdown (sources)
//   Contains: Ownership, Memory Management, Graceful Shutdown

// # X.3a Ownership [OWNERSHIP]
//
//   _create() returns:   Caller owns, use _destroy() to free
//   _init() initializes: Caller-owned memory, use _destroy() to cleanup
//   Pointer params:      Caller retains ownership unless documented
//   Pointer returns:     Caller owns, must free

// # X.3b Memory Patterns [MEMORY]
//
//   // Stack allocation
//   [TypeName] local;
//   [type]_init(&local);
//   // ... use ...
//   [type]_destroy(&local);
//
//   // Heap allocation
//   [TypeName]* ptr = [type]_create();
//   // ... use ...
//   [type]_destroy(ptr);

// # X.3c Graceful Shutdown [SHUTDOWN]
//
//   - Signal handler catches SIGINT/SIGTERM
//   - In-progress operations complete or rollback
//   - Resources released in reverse order
//   - Exit with appropriate code

// ───────────────────────────────────────────────────────────────────────────────
// END CODE SECTIONS
// ───────────────────────────────────────────────────────────────────────────────

// # X.4 Policy [POLICY]
//
//   Purpose:  Guide future maintainers — what's safe to change (headers and sources)
//   Contains: Safe / Careful / Never modification levels
//
// Safe to Modify:
//   ✅ Add new functions/types/constants (follow existing patterns)
//   ✅ Add new helper functions in appropriate groups
//   ✅ Extend existing features
//
// Modify with Care:
//   ⚠️ Public API signatures — breaks all calling code
//   ⚠️ Struct fields — breaks code accessing fields directly
//   ⚠️ Core behavior — affects all consumers
//
// NEVER Modify:
//   ❌ 4-block structure (METADATA, SETUP, BODY, CLOSING)
//   ❌ Include guard pattern
//   ❌ Ownership conventions (_create/_destroy)
//
// Validation: See X.1 for build, runtime, and memory checks.
//
// # X.5 Extension [EXTENSION]
//
//   Purpose:  How to expand this component — mirrors M.10 Roadmap (headers and sources)
//   Contains: Extension patterns, planned features
//
// Adding Functions:
//   - Declaration here, implementation in .c file
//   - Follow lifecycle pattern: _create/_init/_destroy
//   - Add to BODY B.3 Core Operations with Health Impact
//   - Update BODY B.1 Organizational Chart
//
// Adding Types:
//   - Follow existing naming conventions
//   - Add to SETUP S.3 Types with documentation
//
// Architecture: See BODY B.1 for Ladder/Baton structure.
// Roadmap: See M.10 for planned features and research areas.

// # X.6 Troubleshooting [TROUBLESHOOTING]
//
//   Purpose:  Common problems and solutions — header issues (headers), runtime issues (sources)
//
// "Redefinition of [type]":
//   → Check include guards are unique and correct
//
// "Incomplete type":
//   → Add forward declaration or include required header
//
// "Undefined reference to [function]":
//   → Implementation missing in .c file, or not linked
//
// Function-specific troubleshooting in BODY B.3 function docstrings.

// # X.7 Reference [REFERENCE]
//
//   Purpose:  Quick lookup — copy-paste ready (headers and sources)
//
// Basic Setup:
//   #include "[header].h"
//   [TypeName] instance;
//   [type]_init(&instance);
//   // ... use ...
//   [type]_destroy(&instance);
//
// Error Handling:
//   int result = [function]([params], &output);
//   if (result != 0) {
//       fprintf(stderr, "Error: %s\n", [prefix]_get_error_string(result));
//       return result;
//   }
//
// Health Tracking:
//   if ([success]) {
//       logger_success(component_logger, "[op] complete", +10);
//   } else {
//       logger_failure(component_logger, "[op] failed", "reason", -10);
//   }
//
// Compile:
//   gcc -c -Wall -Wextra -std=c11 [source].c

// # X.8 Closing Note [NOTE]
//
//   Purpose:  Final guidance and grounding — echoes M.8 (headers and sources)
//   Verse:    Proverbs 24:27
//
// This header declares the interface for [component].
// Modify thoughtfully — changes affect all consumers.
//
// "Prepare thy work without, and make it fit for thyself in the field."
//   — Proverbs 24:27
//
// Establish foundations before building.

// # X.9 Template Guidance [TEMPLATE]
//
//   Purpose:  How to use this template — remove when instantiating (headers and sources)
//   Usage:    cp header.h dest.h → update pragma → fill sections
//
// Update on instantiation:
//   - M.1: Key, Title
//   - M.2: Created/Updated dates
//   - M.4: Path
//   - M.5: Derives_from (point to this template)
//   - M.9: Dependencies
//   - M.10: Roadmap
//   - Include guard: [HEADER_GUARD_NAME]_H
//
// Remove this section (X.9) when instantiating.

// # X.10 Omission Guide [OMISSION]
//
//   Purpose:  What can be reserved and why — guidance for CLOSING sections (headers and sources)
//
//   All sections must be present. Empty sections use: [Reserved: reason]
//
//   X.1 Validation:     Rarely reserved — all code needs verification
//   X.2 Execution:      Usage examples — rarely reserved
//   X.3 Cleanup:        Rarely reserved — ownership rules critical
//   X.4 Policy:         Rarely reserved — always guides maintainers
//   X.5 Extension:      Reference back to M.10 Roadmap
//   X.6 Troubleshooting: Add issues as they emerge
//   X.7 Reference:      Rarely reserved — examples help users
//   X.8 Closing Note:   Rarely reserved — grounding matters
//   X.9 Template:       Remove when instantiating
//
//   Key principle: CLOSING synthesizes, METADATA/SETUP/BODY contain details.

// ═══════════════════════════════════════════════════════════════════════════════
// END CLOSING
// ═══════════════════════════════════════════════════════════════════════════════

#endif // [HEADER_GUARD_NAME]_H


