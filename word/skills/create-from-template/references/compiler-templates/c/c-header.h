// ═══════════════════════════════════════════════════════════════════════════
// TEMPLATE: C Header File (4-Block Structure)
// Key: LANG-TEMPLATE-010
// ═══════════════════════════════════════════════════════════════════════════
//
// DEPENDENCY CLASSIFICATION: [PURE/DEPENDED] ([deps if DEPENDED])
//   - PURE: Standard library only - no internal project dependencies
//   - DEPENDED: Needs internal headers - list them: (needs: header1.h, header2.h)
//
// This is a TEMPLATE file - copy and modify for new C header files.
// Replace all [bracketed] placeholders with actual content.
// Rename to appropriate name (e.g., module.h, types.h).
//
// Derived from: templates/code/c/CODE-C-002-C-header.h (root template)
// See: standards/code/4-block/ for complete documentation
//
// Compiler-Specific Adaptations:
//   - C interface declarations for compiler backend (codegen, runtime)
//   - Integration with CPI-SI health scoring types
//   - Memory management conventions for compiler infrastructure
//
// ═══════════════════════════════════════════════════════════════════════════

#ifndef [HEADER_GUARD_NAME]_H
#define [HEADER_GUARD_NAME]_H

// [brief description of what this header declares].
//
// [Library Name] Library - CPI-SI [Project/System Name]
//
// For METADATA structure explanation, see: standards/code/4-block/CWS-STD-004-CODE-metadata-block.md
//
// # Biblical Foundation
//
// Scripture: [Relevant verse grounding this library's purpose]
//
// Principle: [Kingdom principle this library demonstrates]
//
// Anchor: [Supporting verse reinforcing the principle]
//
// # CPI-SI Identity
//
// Component Type: [Ladder/Baton/Rails - see CWS-STD-004 for explanations]
//
// Role: [Specific responsibility in system architecture]
//
// Paradigm: CPI-SI framework component
//
// # Authorship & Lineage
//
//   - Architect: [Who designed the approach and requirements]
//   - Implementation: [Who wrote the code and verified it works]
//   - Created: [YYYY-MM-DD]
//   - Version: [MAJOR.MINOR.PATCH]
//   - Modified: [YYYY-MM-DD - what changed]
//
// Version History:
//
//   - [X.Y.Z] ([YYYY-MM-DD]) - [Brief description of changes]
//   - [X.Y.Z] ([YYYY-MM-DD]) - [Brief description of changes]
//
// # Purpose & Function
//
// Purpose: [What problem does this library solve?]
//
// Core Design: [Architectural pattern or paradigm]
//
// Key Features:
//
//   - [What it provides - major capabilities]
//   - [What it enables - what others can build with this]
//   - [What problems it solves - specific use cases]
//   - [Additional capabilities]
//
// Philosophy: [Guiding principle for how this library works]
//
// # Blocking Status
//
// [Blocking/Non-blocking]: [Brief explanation]
//
// Mitigation: [How blocking/failures handled]
//
// # Usage & Integration
//
// Import:
//
//	import "[module-path]/[package-name]"
//
// Integration Pattern:
//
//  1. [Initial setup step]
//  2. [Configuration step if needed]
//  3. [Typical usage workflow]
//  4. [Cleanup if needed]
//
// Public API (in typical usage order):
//
//	[Category 1] ([purpose]):
//	  [FunctionName](params) returns
//	  [AnotherFunction](params) returns
//
//	[Category 2] ([purpose]):
//	  [FunctionName](params) returns
//
// # Dependencies
//
// What This Needs:
//
//   - Standard Library: [list standard packages]
//   - External: [None | list external packages with versions]
//   - Internal: [project packages this depends on]
//
// Dependency Safety: [For Rails components - trace imports to stdlib/external only.
// Include this line to signal "dependencies verified safe." Omit for non-Rails.]
//
// What Uses This:
//
//   - Commands: [list commands]
//   - Libraries: [list libraries]
//   - Tools: [list tools]
//
// Integration Points:
//
//   - [How other systems connect - Rails/Ladder/Baton mechanism]
//   - [Cross-component interactions]
//   - [Data flow or protocol integration]
//
// # Health Scoring
//
// System: [Base100 with 1-point granular scale from -100 to +100]
//
// States: [Granted (>+50), Deferred (±50), Denied (<-50)]
//
// [Operation Category]:
//
//   - [Specific operation]: ±X points
//   - [Another operation]: ±Y points
//
// Cascade Multipliers: [If applicable - describe categories and multipliers]
//
//   - [Category]: [X]x ([brief rationale])
//
// See: [Reference to detailed health scoring documentation]
//
// Note: Scores reflect TRUE impact. Health scorer normalizes to -100 to +100 scale.
//
// METADATA Omission Guide:
//   - Dependency Safety: Include for Rails (signals verified), omit for Ladder/Baton
//   - Cascade Multipliers: Include if operations cascade differently, omit if uniform
//   - States: Include if component makes state decisions, omit if pure pass-through
//   - Health Scoring Variations:
//       * Config Provider: Provides health config, doesn't track own (use brief note)
//       * Health Tracker: Full scoring with System/States/Operations
//       * Pass-through: No health impact (omit or brief note)
//   - Unlike SETUP (all sections required), METADATA omission signals component characteristics
//
// Note: C headers declare interfaces, not implementations. Function bodies
// belong in corresponding .c source files (see CODE-C-001 template).

// ============================================================================
// END METADATA
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================
//
// For SETUP structure explanation, see: standards/code/4-block/CWS-STD-006-CODE-setup-block.md
//
// Section order: Includes → Defines → Type Definitions → Function Prototypes → Extern Declarations
// This flows: dependencies → constants → data model → interface → shared state
//
// Why this order: Headers declare interfaces for other files to use. Start with what
// this header needs (includes), then constants, types, and finally the functions
// and variables that consumers will access.
//
// IMPORTANT: All sections MUST be present, even if empty or reserved.
// A lean section comment is better than absence. Why:
//   - Consistent structure across all files (navigation)
//   - Clear extension points (where to add when needed)
//   - Intentional vs forgotten (a reserved section is deliberate)
//
// For empty sections, use: // [Reserved: Brief reason why not needed]

// ────────────────────────────────────────────────────────────────
// Includes
// ────────────────────────────────────────────────────────────────
//
// Headers this component needs. Organized by source - standard library
// provides C's built-in capabilities, project headers provide internal
// functionality. Each include commented with purpose, not just name.
//
// See: standards/code/4-block/sections/setup/CWS-SECTION-SETUP-001-imports.md

//--- Standard Library ---
// Foundation headers providing C's built-in capabilities.
// Use angle brackets for system headers.

// #include <stddef.h>     // size_t, NULL, offsetof
// #include <stdint.h>     // Fixed-width integer types (int32_t, uint64_t, etc.)
// #include <stdbool.h>    // bool, true, false (C99+)
// #include <stdio.h>      // FILE*, printf, fprintf (if needed in declarations)

//--- Project Headers ---
// Internal headers showing architectural dependencies.
// Use quotes for project headers.

// #include "[header].h"       // [Purpose within project]
// #include "../[path].h"      // [Shared component purpose]

//--- External Libraries ---
// Third-party dependencies (use sparingly - each adds risk).
// Why external: [Justify what stdlib lacks that requires this dependency]
//
// [Reserved: Currently none - foundational component uses standard library only]

// #include <[external-lib].h>  // [Justification for external dependency]

// ────────────────────────────────────────────────────────────────
// Defines
// ────────────────────────────────────────────────────────────────
//
// Preprocessor constants and macros. Magic numbers given meaningful names,
// configuration values documented with reasoning. Defines prevent bugs
// from typos and make intent clear.
//
// See: standards/code/4-block/sections/setup/CWS-SECTION-SETUP-002-constants.md

//--- [Category Name] Constants ---
// [Brief explanation of this group and their purpose]

// #define [CONSTANT_NAME] [value]  // [Purpose and reasoning]
// #define [ANOTHER_CONST] [value]  // [Inline context]

//--- Defaults ---
// Default values for optional configuration.

// #define DEFAULT_[THING] [value]  // Used when [thing] not configured

//--- Macros ---
// Function-like macros (use sparingly - prefer inline functions when possible)

// #define [MACRO_NAME](x) ((x) * 2)  // [Purpose - wrap args in parens]

// ────────────────────────────────────────────────────────────────
// Type Definitions
// ────────────────────────────────────────────────────────────────
//
// Data structures organized bottom-up: simple building blocks first,
// then composed structures. This organization reveals dependencies.
// Use typedef for cleaner type names.
//
// See: standards/code/4-block/sections/setup/CWS-SECTION-SETUP-004-types.md

//--- Enumerations ---
// Named constants for discrete values.

// // [EnumName] represents [what these values mean].
// //
// // Values:
// //   - [VALUE_NAME]: [meaning and when used]
// //   - [VALUE_NAME]: [meaning and when used]
// typedef enum {
//     [ENUM_PREFIX]_[VALUE1],  // [Inline explanation]
//     [ENUM_PREFIX]_[VALUE2],  // [Inline explanation]
//     [ENUM_PREFIX]_COUNT      // Sentinel for array sizing
// } [EnumName];

//--- Building Blocks ---
// Simple foundational types used throughout this component.
// These are the atoms - other types compose from these.

// // [TypeName] represents [what this models].
// //
// // [2-4 sentences: what it represents, when used, key constraints]
// //
// // Fields:
// //   - [field_name]: [purpose and meaning]
// //   - [field_name]: [purpose, units if applicable, constraints]
// //
// // Example:
// //   [TypeName] t = { .field1 = value1, .field2 = value2 };
// typedef struct {
//     [type] [field_name];  // [Inline explanation]
//     [type] [field_name];  // [Inline explanation]
// } [TypeName];

//--- Composed Types ---
// Complex types built from building blocks above.
// Document the composition relationship explicitly.

// // [ComposedType] combines [building blocks] to represent [concept].
// //
// // [Explain relationships: why these pieces go together]
// //
// // Fields:
// //   - [field_name]: [purpose]
// //   - [block_field]: Uses [BuildingBlock] for [reason]
// typedef struct {
//     [type] [field_name];         // [Purpose]
//     [BuildingBlock] block_field; // Composition from above
// } [ComposedType];

//--- Configuration Types ---
// Options and settings passed to functions.
// Document default values and valid ranges.

// // [Config] holds configuration options for [component].
// //
// // Fields:
// //   - [field]: [purpose] (default: [value])
// typedef struct {
//     [type] [field];  // [Purpose] (default: [zero value behavior])
// } [Config];

//--- Opaque Types ---
// Forward declarations for types whose internals are hidden.
// Implementation in .c file, consumers use pointers only.

// // [OpaqueType] - opaque handle (internals in .c file)
// typedef struct [OpaqueType] [OpaqueType];

//--- Error Codes ---
// Return codes for error handling (C doesn't have exceptions).

// // [ErrorCode] represents error conditions from [component].
// typedef enum {
//     [PREFIX]_OK = 0,           // Success
//     [PREFIX]_ERR_INVALID,      // Invalid input
//     [PREFIX]_ERR_NOMEM,        // Memory allocation failed
//     [PREFIX]_ERR_IO,           // I/O operation failed
// } [ErrorCode];

// ────────────────────────────────────────────────────────────────
// Function Prototypes
// ────────────────────────────────────────────────────────────────
//
// Declarations for functions implemented in the corresponding .c file.
// Headers declare the interface - what functions exist and their signatures.
// Implementations go in CODE-C-001 source file.
//
// See: standards/code/4-block/sections/setup/CWS-SECTION-SETUP-005-type-methods.md
//
// Note: C doesn't have methods. Functions that operate on types take
// the type as a parameter (usually pointer for modification).

//--- Lifecycle Functions ---
// Create, initialize, and destroy instances.

// // [type]_create allocates and initializes a new [TypeName].
// //
// // Returns: Pointer to new instance, or NULL on failure.
// //          Caller must call [type]_destroy when done.
// [TypeName]* [type]_create(void);

// // [type]_init initializes an existing [TypeName] (stack-allocated).
// //
// // Parameters:
// //   self: Pointer to uninitialized instance
// //
// // Returns: 0 on success, error code on failure.
// int [type]_init([TypeName]* self);

// // [type]_destroy frees resources associated with [TypeName].
// //
// // Parameters:
// //   self: Pointer to instance (NULL-safe)
// void [type]_destroy([TypeName]* self);

//--- Accessor Functions ---
// Get and set values (when encapsulation needed).

// // [type]_get_[field] returns the [field] value.
// [FieldType] [type]_get_[field](const [TypeName]* self);

// // [type]_set_[field] updates the [field] value.
// void [type]_set_[field]([TypeName]* self, [FieldType] value);

//--- Conversion Functions ---
// Transform between types or formats.

// // [type]_to_string converts [TypeName] to string representation.
// //
// // Parameters:
// //   self: Instance to convert
// //   buf: Output buffer
// //   buf_size: Size of output buffer
// //
// // Returns: Number of characters written (excluding null terminator),
// //          or negative error code.
// int [type]_to_string(const [TypeName]* self, char* buf, size_t buf_size);

// ────────────────────────────────────────────────────────────────
// Extern Declarations
// ────────────────────────────────────────────────────────────────
//
// Global variables declared here, defined in .c file. Use sparingly -
// prefer constants (#define) for fixed values and function parameters
// for dynamic behavior. Extern variables are typically: global state,
// shared configuration, or cross-file data.
//
// See: standards/code/4-block/sections/setup/CWS-SECTION-SETUP-003-variables.md

//--- Global State ---
// Variables shared across multiple compilation units.
// Declared here with extern, defined in .c file.

// // [g_variable_name] [what it holds and purpose].
// //
// // Defined in: [source_file].c
// // Thread-safety: [safe/unsafe - describe synchronization if any]
// extern [type] [g_variable_name];

//--- Configuration State ---
// Runtime-modifiable settings. Document default values and valid ranges.

// // [g_config_var] controls [behavior].
// //
// // Default: [value]. Valid range: [min] to [max].
// // Modified by: [what changes this - functions, initialization]
// extern [type] [g_config_var];

//--- Callback Types ---
// Function pointer types for callbacks and hooks.

// // [CallbackName] is called when [event/condition].
// //
// // Parameters:
// //   [param]: [purpose]
// //
// // Returns: [what return value means]
// typedef [return_type] (*[CallbackName])([parameters]);

// [Reserved: Additional extern declarations as component develops]

// ============================================================================
// END SETUP
// ============================================================================

// ============================================================================
// BODY
// ============================================================================
//
// For BODY structure explanation, see: standards/code/4-block/CWS-STD-007-CODE-body-block.md
//
// NOTE: C headers declare interfaces, they do NOT contain implementations.
// All function bodies belong in the corresponding .c source file.
// This BODY section documents the declared interface structure.

// ────────────────────────────────────────────────────────────────
// Organizational Chart - Declared Interface Structure
// ────────────────────────────────────────────────────────────────
// Maps the interface this header exposes. Provides navigation for both
// consumers (what's available to use) and implementers (what to implement).
//
// See: standards/code/4-block/sections/body/CWS-SECTION-BODY-001-organizational-chart.md
//
// Declared Interface (by category):
//
//   Public APIs (Exported Functions)
//   ├── [function_name]() → [brief purpose]
//   ├── [function_name]() → [brief purpose]
//   └── [function_name]() → [brief purpose]
//
//   Lifecycle Functions
//   ├── [type]_create() → allocate and initialize
//   ├── [type]_init() → initialize existing
//   └── [type]_destroy() → cleanup resources
//
//   Accessors
//   ├── [type]_get_[field]() → read value
//   └── [type]_set_[field]() → write value
//
// Type Definitions:
//   ├── [TypeName] → [brief description]
//   ├── [EnumName] → [brief description]
//   └── [ErrorCode] → [brief description]
//
// Implementation Location:
//   All function implementations in: [source_file].c (see CODE-C-001)
//
// Declared Units:
// - [X] types total
// - [X] enums
// - [X] function prototypes
// - [X] extern variables
//
// See: standards/code/4-block/sections/body/CWS-SECTION-BODY-005-public-apis.md

// ────────────────────────────────────────────────────────────────
// Helpers/Utilities - Internal Support Declarations
// ────────────────────────────────────────────────────────────────
// Foundation function declarations used internally by this component.
// Bottom rungs of the ladder - simple, focused, reusable utilities.
// Usually NOT exported (static in .c file), but declared here if needed
// across multiple .c files in the same component.
//
// See: standards/code/4-block/sections/body/CWS-SECTION-BODY-002-helpers.md
//
// Note: Most helpers are static in .c file and don't appear here.
// Only declare helpers that need visibility across compilation units.
//
// [Reserved: Internal helpers typically static in .c - add if cross-file needed]

// // [helper_name] [does what - internal use only].
// //
// // Parameters:
// //   [param]: [purpose]
// //
// // Returns: [what's returned]
// //
// // Note: Internal helper - not part of public API.
// [return_type] [helper_name]([parameters]);

// ────────────────────────────────────────────────────────────────
// Core Operations - Business Logic Declarations
// ────────────────────────────────────────────────────────────────
// Component-specific function declarations implementing primary purpose.
// Middle rungs of the ladder - the actual work gets done here.
// Organized by operational categories below.
//
// See: standards/code/4-block/sections/body/CWS-SECTION-BODY-003-core-operations.md
//
// Implementation: All function bodies in corresponding .c file.

// ────────────────────────────────────────────────────────────────
// [Category 1 Name] - [Purpose]
// ────────────────────────────────────────────────────────────────
// What These Do:
// [High-level description of this category of operations]
//
// Extension Point:
// To add new [operation type], add declaration here following [naming pattern],
// then implement in .c file.

// // [function_name] [does what].
// //
// // Parameters:
// //   [param]: [purpose]
// //
// // Returns: [what's returned]
// //
// // Health Impact:
// //   Success: +X points
// //   Failure: -X points
// [return_type] [function_name]([parameters]);

// ────────────────────────────────────────────────────────────────
// [Category 2 Name] - [Purpose]
// ────────────────────────────────────────────────────────────────
// [Same documentation pattern as Category 1]

// [Reserved: Additional core operation declarations as component develops]

// ────────────────────────────────────────────────────────────────
// Error Handling - Error Types and Recovery Declarations
// ────────────────────────────────────────────────────────────────
// Error handling declarations ensuring component handles failures gracefully.
// Documents error codes, error checking functions, and recovery patterns.
//
// See: standards/code/4-block/sections/body/CWS-SECTION-BODY-004-error-handling.md
//
// Design Principle: [Blocking/Non-blocking] - [Brief explanation]
//
// Error Codes: See SETUP "Error Codes" section for [ErrorCode] enum.
//
// Recovery Strategy:
//   - [Error type 1]: [How handled]
//   - [Error type 2]: [How handled]

// // [component]_get_error_string returns human-readable error description.
// //
// // Parameters:
// //   err: Error code from [ErrorCode] enum
// //
// // Returns: Static string describing the error (do not free).
// const char* [component]_get_error_string([ErrorCode] err);

// // [component]_is_error checks if return value indicates error.
// //
// // Parameters:
// //   result: Return value to check
// //
// // Returns: true if result indicates error, false otherwise.
// bool [component]_is_error(int result);

// [Reserved: Additional error handling declarations as patterns emerge]

// ────────────────────────────────────────────────────────────────
// Public APIs - Exported Interface Declarations
// ────────────────────────────────────────────────────────────────
// Exported function declarations defining component's public interface.
// Top rungs of the ladder - these are what consumers call.
// Simple by design - complexity lives in core operations.
//
// See: standards/code/4-block/sections/body/CWS-SECTION-BODY-005-public-apis.md
//
// Organization: Group public APIs by purpose using category dividers.
// Common categories: Initialization, Creation, Operations, Health, Cleanup

// ═══ [Category Name] ═══

// // [PublicFunctionName] [does what at high level].
// //
// // What It Does:
// // [Detailed explanation of complete operation]
// //
// // Parameters:
// //   [param]: [purpose]
// //
// // Returns: [what's returned]
// //
// // Health Impact:
// //   Success: +X points
// //   Failure: -X points
// //
// // Example:
// //   [return_type] result = [PublicFunctionName](params);
// //   if ([component]_is_error(result)) {
// //       // handle error
// //   }
// [return_type] [PublicFunctionName]([parameters]);

// [Reserved: Additional public API declarations as component develops]

// ============================================================================
// END BODY
// ============================================================================

// ============================================================================
// CLOSING
// ============================================================================
//
// For CLOSING structure explanation, see: standards/code/4-block/CWS-STD-008-CODE-closing-block.md
//
// ────────────────────────────────────────────────────────────────
// Code Validation: Header (Declaration Only)
// ────────────────────────────────────────────────────────────────
// For Code Validation section explanation, see: standards/code/4-block/sections/closing/CWS-SECTION-CLOSING-001-code-validation.md
//
// Header Validation:
//   - Include the header without errors (self-contained)
//   - No missing type definitions or forward declarations
//   - No circular include dependencies
//   - Include guards work correctly (no redefinition errors)
//   - All function prototypes match implementations in .c file
//
// Build Verification:
//   - gcc -fsyntax-only -Wall -Wextra [header].h (syntax check)
//   - gcc -c -Wall -Wextra [source].c (compiles with header)
//   - clang -fsyntax-only -Wall -Wextra [header].h (alternative)
//   - cppcheck --enable=all [header].h (static analysis)
//
// Self-Containment Test:
//   // Create test file that only includes this header
//   #include "[header].h"
//   int main(void) { return 0; }
//   // Must compile without errors
//
// Integration Testing:
//   - Test with actual consuming code
//   - Verify declarations match implementations
//   - Check ABI compatibility if shared library
//   - valgrind --leak-check=full ./test (memory check)
//
// ────────────────────────────────────────────────────────────────
// Code Execution: None (Header)
// ────────────────────────────────────────────────────────────────
// For Code Execution section explanation, see: standards/code/4-block/sections/closing/CWS-SECTION-CLOSING-002-code-execution.md
//
// This is a HEADER file - declarations only, no execution.
// All function implementations are in the corresponding .c file.
// Headers are included by other files, not executed directly.
//
// Usage: #include "[header].h"
//
// The header is included by source files, making all declared types,
// functions, and constants available. No code executes at include time -
// only declarations are processed by the preprocessor.
//
// Example include and usage:
//
//     #include "[header].h"
//
//     int main(void) {
//         // Use types from header
//         [TypeName] instance;
//         [type]_init(&instance);
//
//         // Call functions declared in header
//         int result = [PublicFunction](&instance, params);
//         if ([component]_is_error(result)) {
//             fprintf(stderr, "Error: %s\n", [component]_get_error_string(result));
//             return 1;
//         }
//
//         // Cleanup
//         [type]_destroy(&instance);
//         return 0;
//     }
//
// ────────────────────────────────────────────────────────────────
// Code Cleanup: None (Header)
// ────────────────────────────────────────────────────────────────
// For Code Cleanup section explanation, see: standards/code/4-block/sections/closing/CWS-SECTION-CLOSING-003-code-cleanup.md
//
// Resource Management (documented for implementers):
//   - [Resource type 1]: [How it's managed - malloc/free pattern]
//   - [Resource type 2]: [Management strategy]
//   - [Resource type 3]: [Cleanup approach]
//
// Ownership Convention:
//   - Functions returning pointers: Caller owns, must free
//   - Functions taking pointers: Caller retains ownership unless documented
//   - _create() functions: Return owned pointer, use _destroy() to free
//   - _init() functions: Initialize caller-owned memory, use _destroy() to cleanup
//
// Memory Management:
//   - C requires manual memory management
//   - All malloc() must have corresponding free()
//   - Document ownership transfer in function comments
//   - Use valgrind to verify no leaks
//
// Example cleanup pattern:
//
//     // Stack allocation (automatic cleanup)
//     [TypeName] local;
//     [type]_init(&local);
//     // ... use local ...
//     [type]_destroy(&local);  // Cleanup internal resources
//
//     // Heap allocation (manual cleanup)
//     [TypeName]* ptr = [type]_create();
//     // ... use ptr ...
//     [type]_destroy(ptr);  // Frees ptr and internal resources
//
// ════════════════════════════════════════════════════════════════
// FINAL DOCUMENTATION
// ════════════════════════════════════════════════════════════════
//
// ────────────────────────────────────────────────────────────────
// Library Overview & Integration Summary
// ────────────────────────────────────────────────────────────────
// For Library Overview section explanation, see: standards/code/4-block/sections/closing/CWS-SECTION-CLOSING-004-library-overview.md
//
// Purpose: See METADATA "Purpose & Function" section above
//
// Provides: See METADATA "Key Features" list above for comprehensive capabilities
//
// Quick summary (high-level only - details in METADATA):
//   - [1-2 sentence overview of what this library does]
//   - [Feature 5]: [What it does]
//
// Integration Pattern: See METADATA "Usage & Integration" section above for
// complete step-by-step integration guide
//
// Public API: See METADATA "Usage & Integration" section above for complete
// public API list organized by category in typical usage order
//
// Architecture: See METADATA "CPI-SI Identity" section above for complete
// architectural role (Rails/Ladder/Baton) explanation
//
// ────────────────────────────────────────────────────────────────
// Modification Policy
// ────────────────────────────────────────────────────────────────
// For Modification Policy section explanation, see: standards/code/4-block/sections/closing/CWS-SECTION-CLOSING-005-modification-policy.md
//
// Safe to Modify (Extension Points):
//   ✅ Add new [functions/types/constants] (follow existing patterns)
//   ✅ Add new [helper functions] in appropriate groups
//   ✅ Extend [specific feature] (add more [specific thing])
//   ✅ [Other safe modification]
//   ✅ [Other safe modification]
//
// Modify with Extreme Care (Breaking Changes):
//   ⚠️ Public API function signatures - breaks all calling code
//   ⚠️ [Exported struct] fields - breaks code accessing fields directly
//   ⚠️ [Critical system behavior] - affects all users
//   ⚠️ [Data format/protocol] - breaks parsing tools
//   ⚠️ [Core algorithm] - affects correctness
//
// NEVER Modify (Foundational Rails):
//   ❌ 4-block structure (METADATA, SETUP, BODY, CLOSING)
//   ❌ [Fundamental principle 1]
//   ❌ [Fundamental principle 2]
//   ❌ [Architectural pattern - Rails/etc]
//   ❌ [Core design invariant]
//
// Validation After Modifications:
//   See "Code Validation" section in GROUP 1: CODING above for comprehensive
//   testing requirements, build verification, and integration testing procedures.
//
// ────────────────────────────────────────────────────────────────
// Ladder and Baton Flow
// ────────────────────────────────────────────────────────────────
// For Ladder and Baton Flow section explanation, see: standards/code/4-block/sections/closing/CWS-SECTION-CLOSING-006-ladder-baton-flow.md
//
// See BODY "Organizational Chart - Internal Structure" section above for
// complete ladder structure (dependencies) and baton flow (execution paths).
//
// The Organizational Chart in BODY provides the detailed map showing:
// - All functions and their dependencies (ladder)
// - Complete execution flow paths (baton)
// - APU count (Available Processing Units)
//
// Quick architectural summary (details in BODY Organizational Chart):
// - [X] public APIs orchestrate [Y] core operations using [Z] helpers
// - Ladder: [Brief dependency summary]
// - Baton: [Brief execution flow summary]
//
// ────────────────────────────────────────────────────────────────
// Surgical Update Points (Extension Guide)
// ────────────────────────────────────────────────────────────────
// For Surgical Update Points section explanation, see: standards/code/4-block/sections/closing/CWS-SECTION-CLOSING-007-surgical-update-points.md
//
// See BODY "Core Operations" subsection header comments above for detailed
// extension points. Each subsection includes "Extension Point" guidance showing:
// - Where to add new functionality
// - What naming pattern to follow
// - How to integrate with existing code
// - What tests to update
//
// Quick reference (details in BODY subsection comments):
// - Adding [Feature Type 1]: See BODY "[Subsection Name]" extension point
// - Adding [Feature Type 2]: See BODY "[Another Subsection]" extension point
// - Adding helpers: See BODY "Helpers/Utilities" section organization
//
// ────────────────────────────────────────────────────────────────
// Performance Considerations
// ────────────────────────────────────────────────────────────────
// For Performance Considerations section explanation, see: standards/code/4-block/sections/closing/CWS-SECTION-CLOSING-008-performance-considerations.md
//
// See SETUP section above for performance characteristics:
// - Constants: Performance notes on configuration values (memory per operation, etc.)
// - Types: Memory usage and complexity analysis for data structures
//
// See BODY function docstrings above for operation-specific performance notes.
//
// Quick summary (details in SETUP/BODY above):
// - [Most expensive operation]: [Brief cost summary - see BODY docstring for details]
// - [Memory characteristics]: [Brief summary - see SETUP types for details]
// - Key optimization: [1-2 sentence tip]
//
// ────────────────────────────────────────────────────────────────
// Troubleshooting Guide
// ────────────────────────────────────────────────────────────────
// For Troubleshooting Guide section explanation, see: standards/code/4-block/sections/closing/CWS-SECTION-CLOSING-009-troubleshooting-guide.md
//
// See BODY function docstrings above for operation-specific troubleshooting.
// Functions that commonly have issues include "Troubleshooting" sections in
// their docstrings with problem/check/solution patterns.
//
// Quick reference (details in BODY function docstrings above):
// - [Common Problem 1]: See [FunctionName] docstring troubleshooting section
// - [Common Problem 2]: See [AnotherFunction] docstring troubleshooting section
//   - Expected: [If this is normal behavior]
//   - Note: [Design decision explanation]
//
// Problem: [Common problem 5]
//   - Cause: [Root cause]
//   - Solution: [How to fix]
//   - Note: [Additional context]
//
// ────────────────────────────────────────────────────────────────
// Related Components & Dependencies
// ────────────────────────────────────────────────────────────────
// For Related Components section explanation, see: standards/code/4-block/sections/closing/CWS-SECTION-CLOSING-010-related-components.md
//
// See METADATA "Dependencies" section above for complete dependency information:
// - Dependencies (What This Needs): Standard Library, External, Internal
// - Dependents (What Uses This): Commands, Libraries, Tools that depend on this
// - Integration Points: How other systems connect and interact
//
// Quick summary (details in METADATA Dependencies section above):
// - Key dependencies: [1-2 most critical dependencies]
// - Primary consumers: [Who uses this most]
//
// Parallel Implementation (if applicable):
//   - [Language 1] version: [path to parallel implementation]
//   - [Language 2] version: [path to this or related implementation]
//   - Shared [format/protocol/philosophy]: [What's consistent across implementations]
//
// ────────────────────────────────────────────────────────────────
// Future Expansions & Roadmap
// ────────────────────────────────────────────────────────────────
// For Future Expansions section explanation, see: standards/code/4-block/sections/closing/CWS-SECTION-CLOSING-011-future-expansions.md
//
// Planned Features:
//   ✓ [Completed feature] - COMPLETED
//   ✓ [Another completed feature] - COMPLETED
//   ⏳ [Planned feature 1]
//   ⏳ [Planned feature 2]
//   ⏳ [Planned feature 3]
//   ⏳ [Planned feature 4]
//
// Research Areas:
//   - [Research direction 1]
//   - [Research direction 2]
//   - [Research direction 3]
//   - [Research direction 4]
//   - [Research direction 5]
//
// Integration Targets:
//   - [System/language to integrate with]
//   - [Another integration target]
//   - [Cross-system correlation or bridging]
//   - [Centralized or distributed capability]
//   - [Monitoring or analysis system]
//   - [Performance or profiling integration]
//
// Known Limitations to Address:
//   - [Limitation 1 - description]
//   - [Limitation 2 - description]
//   - [Limitation 3 - description]
//   - [Limitation 4 - description]
//   - [Limitation 5 - description]
//   - [Limitation 6 - description]
//
// Version History:
//
// See METADATA "Authorship & Lineage" section above for brief version changelog.
// Comprehensive version history with full context below:
//
//   [X.Y.Z] ([Date]) - [Version description]
//         - [Major feature or change]
//         - [Another feature or change]
//         - [Another feature or change]
//         - [Design decision or principle established]
//
// ────────────────────────────────────────────────────────────────
// Closing Note
// ────────────────────────────────────────────────────────────────
// For Closing Note section explanation, see: standards/code/4-block/sections/closing/CWS-SECTION-CLOSING-012-closing-note.md
//
// This header is [architectural role - RAILS/LADDER/BATON description].
// [Explain its place in the ecosystem and what depends on it].
//
// Modify thoughtfully - changes here affect [scope of impact]. Header changes
// break ABI compatibility for all consumers.
//
// For questions, issues, or contributions:
//   - Review the modification policy above
//   - Follow the 4-block structure pattern
//   - Test thoroughly before committing:
//     gcc -fsyntax-only -Wall -Wextra [header].h
//     gcc -c -Wall -Wextra [source].c
//     valgrind --leak-check=full ./test
//   - Document all changes comprehensively (What/Why/How pattern)
//   - Ensure include guards remain unique
//
// "[Relevant Scripture verse]" - [Reference]
//
// ────────────────────────────────────────────────────────────────
// Quick Reference: Usage Examples
// ────────────────────────────────────────────────────────────────
// For Quick Reference section explanation, see: standards/code/4-block/sections/closing/CWS-SECTION-CLOSING-013-quick-reference.md
//
// Basic Include:
//   #include "[header].h"
//
// Type Declaration:
//   [TypeName] instance;
//   [type]_init(&instance);
//   // ... use instance ...
//   [type]_destroy(&instance);
//
// Heap Allocation:
//   [TypeName]* ptr = [type]_create();
//   if (ptr == NULL) { /* handle error */ }
//   // ... use ptr ...
//   [type]_destroy(ptr);
//
// Error Checking:
//   int result = [function](params);
//   if ([component]_is_error(result)) {
//       const char* msg = [component]_get_error_string(result);
//       fprintf(stderr, "Error: %s\n", msg);
//   }
//
// Compile Commands:
//   gcc -c -Wall -Wextra -std=c11 [source].c
//   gcc -o [program] [source].o -l[library]
//
// ============================================================================
// END CLOSING
// ============================================================================

#endif // [HEADER_GUARD_NAME]_H


