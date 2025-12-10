; %error "TEMPLATE: Remove this line when ready to include"
; ═══════════════════════════════════════════════════════════════════════════
; TEMPLATE: Assembly Include File (4-Block Structure)
; Key: B-word-seed-code-asm-include
; ═══════════════════════════════════════════════════════════════════════════
;
; DEPENDENCY CLASSIFICATION: [PURE/DEPENDED] ([deps if DEPENDED])
;   - PURE: Self-contained - no external include dependencies
;   - DEPENDED: Needs other includes - list them: (needs: config.inc, types.inc)
;
; This is a TEMPLATE file - copy and modify for new assembly include files.
; Replace all [bracketed] placeholders with actual content.
; Rename to appropriate name (e.g., config.inc, types.inc, constants.inc).
; Remove the "%error" line above when ready to include.
;
; Assembler: NASM (Netwide Assembler)
; derives_from: bereshit/word/omni/seed/code.omni
; Derived from: Kingdom Technology 4-block code structure
; See: standards/code/4-block/ for complete documentation
;
; Assembly Include Notes:
;   - .inc files are assembly's equivalent to C .h header files
;   - Provide constants (EQU), macros (%macro), and type definitions (struc)
;   - No code sections - definitions only
;   - Use %ifndef/%define pattern for include guards
;
; ═══════════════════════════════════════════════════════════════════════════

; [brief description of what this include file provides].
;
; [Include Name] Include - CPI-SI [Project/System Name]
;
; ============================================================================
; METADATA
; ============================================================================
;
; ────────────────────────────────────────────────────────────────
; CORE IDENTITY (Required)
; ────────────────────────────────────────────────────────────────
;
; # Biblical Foundation
;
; Scripture: [Relevant verse grounding this include's purpose]
;
; Principle: [Kingdom principle this include demonstrates]
;
; Anchor: [Supporting verse reinforcing the principle]
;
; # CPI-SI Identity
;
; Component Type: Rails (configuration/constants)
;
; Role: [Specific responsibility - what definitions this provides]
;
; Paradigm: CPI-SI framework component
;
; # Authorship & Lineage
;
;   - Architect: [Who designed the approach and requirements]
;   - Implementation: [Who wrote the definitions and verified they work]
;   - Created: [YYYY-MM-DD]
;   - Version: [MAJOR.MINOR.PATCH]
;   - Modified: [YYYY-MM-DD - what changed]
;
; Version History:
;
;   - [X.Y.Z] ([YYYY-MM-DD]) - [Brief description of changes]
;   - [X.Y.Z] ([YYYY-MM-DD]) - [Brief description of changes]
;
; # Purpose & Function
;
; Purpose: [What definitions does this include provide?]
;
; Core Design: [Architectural pattern - e.g., configuration centralization]
;
; Key Features:
;
;   - [What constants it provides - major categories]
;   - [What macros it provides - reusable patterns]
;   - [What type definitions it provides - structures]
;
; Philosophy: [Guiding principle for these definitions]
;
; ────────────────────────────────────────────────────────────────
; INTERFACE (Expected)
; ────────────────────────────────────────────────────────────────
;
; # Dependencies
;
; What This Needs:
;
;   - Other Includes: [None | list other .inc files needed]
;   - CPU Features: [None | list required features]
;
; What Uses This:
;
;   - [Assembly source file 1]
;   - [Assembly source file 2]
;   - [Other includes that build on this]
;
; Integration Points:
;
;   - [How this connects to system configuration]
;   - [C header equivalent if config-driven: include/[path]/[name].h]
;
; # Usage & Integration
;
; Include:
;
;   %include "[path]/[include-name].inc"
;
; Integration Pattern:
;
;  1. %include this file at top of .asm source
;  2. Use constants with: mov eax, CONSTANT_NAME
;  3. Use macros with: MACRO_NAME param1, param2
;  4. Use structs with: instance: istruc StructName ...
;
; Definitions Provided (in typical usage order):
;
;   [Category 1] ([purpose]):
;     [CONSTANT_NAME] - [description]
;     [ANOTHER_CONST] - [description]
;
;   [Category 2] ([purpose]):
;     [MACRO_NAME] - [description]
;
; ────────────────────────────────────────────────────────────────
; OPERATIONAL (Contextual)
; ────────────────────────────────────────────────────────────────
;
; # Blocking Status
;
; [OMIT: Include files are compile-time only - no execution path]
;
; # Health Scoring
;
; [OMIT: Include files don't track health - errors cause build failures]
;
; ────────────────────────────────────────────────────────────────
; METADATA Omission Guide
; ────────────────────────────────────────────────────────────────
;
; Tier 1 (CORE IDENTITY): Never omit - every file needs these.
;
; Tier 2 (INTERFACE): May omit with [OMIT: reason] notation.
;   - Dependencies: Required - documents other includes and CPU features
;   - Usage & Integration: Required - shows how to include and use definitions
;
; Tier 3 (OPERATIONAL): Include when applicable to file type.
;   - Blocking Status: [OMIT: Include files are compile-time only]
;   - Health Scoring: [OMIT: Include files don't track health]
;
; Unlike SETUP (all sections required), METADATA omission signals component characteristics.

; ============================================================================
; END METADATA
; ============================================================================

; ============================================================================
; SETUP
; ============================================================================
;
; For SETUP structure explanation, see: standards/code/4-block/CWS-STD-006-CODE-setup-block.md
;
; Section order: Include Guard → Required Includes → Constants → Extern Symbols → Data Refs → Debug
; This flows: guard → dependencies → fixed values → externals → data → infrastructure
;
; Note: Include files are primarily definitions (BODY). SETUP is minimal.
;
; IMPORTANT: All sections MUST be present, even if empty or reserved.
; For empty sections, use: ; [Reserved: Brief reason why not needed]
;
; -----------------------------------------------------------------------------
; SETUP Sections Overview
; -----------------------------------------------------------------------------
;
; 1. INCLUDE GUARD (Dependencies)
;    Purpose: Prevents multiple inclusion errors
;    Subsections: Guard Definition
;
; 2. REQUIRED INCLUDES (Dependencies)
;    Purpose: Other include files this one depends on
;    Subsections: Project Includes → Standard Includes → External Includes
;
; 3. CONSTANTS (Reserved)
;    Purpose: [Reserved: Constants defined in BODY for include files]
;
; 4. EXTERN SYMBOLS (Reserved)
;    Purpose: [Reserved: Extern declarations in BODY for include files]
;
; 5. DATA REFERENCES (Reserved)
;    Purpose: [Reserved: Data definitions in BODY for include files]
;
; 6. DEBUG INFRASTRUCTURE (Reserved)
;    Purpose: [Reserved: Debug macros defined in BODY if needed]

; ────────────────────────────────────────────────────────────────
; Include Guard (Dependencies)
; ────────────────────────────────────────────────────────────────
;
; Prevents multiple inclusion errors. NASM uses %ifndef/%define pattern.
; Guard name should be unique - typically: [PROJECT]_[PATH]_[NAME]_INC

%ifndef [INCLUDE_GUARD_NAME]_INC
%define [INCLUDE_GUARD_NAME]_INC

; ────────────────────────────────────────────────────────────────
; Required Includes (Dependencies)
; ────────────────────────────────────────────────────────────────
;
; Other include files this one depends on. Order matters - include
; dependencies before using their definitions.

;--- Project Includes ---
; Other project includes this depends on.

; %include "[path]/[dependency].inc"  ; [Purpose - what definitions needed]

;--- Standard Includes ---
; [Reserved: Assembly has no standard includes - bare metal]

;--- External Includes ---
; [Reserved: No external assembly includes in kernel]

; ────────────────────────────────────────────────────────────────
; Constants (Reserved)
; ────────────────────────────────────────────────────────────────
; [Reserved: Constants defined in BODY for include files]

; ────────────────────────────────────────────────────────────────
; Extern Symbols (Reserved)
; ────────────────────────────────────────────────────────────────
; [Reserved: Extern declarations in BODY for include files]

; ────────────────────────────────────────────────────────────────
; Data References (Reserved)
; ────────────────────────────────────────────────────────────────
; [Reserved: Data definitions in BODY for include files]

; ────────────────────────────────────────────────────────────────
; Debug Infrastructure (Reserved)
; ────────────────────────────────────────────────────────────────
; [Reserved: Debug macros defined in BODY if needed]

; -----------------------------------------------------------------------------
; SETUP Omission Guide
; -----------------------------------------------------------------------------
;
; ALL sections MUST be present. Content may be reserved with reason:
;
;   - Include Guard: Never reserved - required for include files
;   - Required Includes: [Reserved: Self-contained, no dependencies]
;   - Constants: [Reserved: Constants defined in BODY for include files]
;   - Extern Symbols: [Reserved: Extern declarations in BODY for include files]
;   - Data References: [Reserved: Data definitions in BODY for include files]
;   - Debug Infrastructure: [Reserved: Debug macros in BODY if needed]
;
; Unlike METADATA (sections omitted entirely with [OMIT:]), SETUP preserves
; all section headers with [Reserved:] notation for unused sections.

; ============================================================================
; END SETUP
; ============================================================================

; ============================================================================
; BODY
; ============================================================================
;
; For BODY structure explanation, see: standards/code/4-block/CWS-STD-007-CODE-body-block.md
;
; NOTE: Assembly include files DECLARE definitions, they do NOT contain code.
; This BODY section provides the definitions this include exports.
;
; -----------------------------------------------------------------------------
; BODY Sections Overview
; -----------------------------------------------------------------------------
;
; 1. ORGANIZATIONAL CHART (Definitions Structure)
;    Purpose: Map what definitions this include provides
;    Subsections: Constants → Macros → Structures → Externals → Definition Counts
;
; 2. CONSTANTS (EQU Definitions)
;    Purpose: Named compile-time values - no memory used
;    Subsections: [Category 1] → [Category 2] → ... (organized by purpose)
;
; 3. MACRO DEFINITIONS (%macro)
;    Purpose: Reusable code patterns - expand at assembly time
;    Subsections: Multi-line Macros → Simple Defines
;
; 4. STRUCTURE DEFINITIONS (struc)
;    Purpose: Data structure layouts - equivalent to C typedef struct
;    Subsections: Building Block Structures → Composed Structures
;
; 5. EXTERNAL SYMBOL DECLARATIONS
;    Purpose: EXTERN (needs) and GLOBAL (exports) declarations
;    Subsections: External Dependencies → Exported Symbols
;
; Section order: Org Chart → Constants → Macros → Structures → Externals
; This flows: map definitions → fixed values → code patterns → data layouts → symbol declarations
;
; Universal mapping (see standards for cross-language patterns):
;   Organizational Chart ≈ Definitions Overview
;   Constants ≈ EQU values (compile-time)
;   Macro Definitions ≈ %macro code patterns
;   Structure Definitions ≈ struc data layouts
;   External Declarations ≈ EXTERN/GLOBAL symbols

; ────────────────────────────────────────────────────────────────
; Organizational Chart - Definitions Structure
; ────────────────────────────────────────────────────────────────
; Maps what this include provides. Provides navigation for both
; consumers (what's available to use) and maintainers (what's defined).
;
; See: standards/code/4-block/sections/body/CWS-SECTION-BODY-001-organizational-chart.md
;
; Definitions Provided (by category):
;
;   Constants (EQU)
;   ├── [Category 1]
;   │   ├── [CONSTANT_1] = [value] - [purpose]
;   │   └── [CONSTANT_2] = [value] - [purpose]
;   └── [Category 2]
;       └── [CONSTANT_3] = [value] - [purpose]
;
;   Macros (%macro)
;   ├── [MACRO_1](params) - [purpose]
;   └── [MACRO_2](params) - [purpose]
;
;   Structures (struc)
;   └── [StructName] - [purpose]
;       ├── .[field1] - [type/size]
;       └── .[field2] - [type/size]
;
;   External Declarations
;   ├── [EXTERN symbol] - [what it provides]
;   └── [GLOBAL symbol] - [what this exports]
;
; Definition Counts:
; - [X] constants total
; - [X] macros
; - [X] structures
; - [X] external declarations

; ────────────────────────────────────────────────────────────────
; Constants (EQU Definitions)
; ────────────────────────────────────────────────────────────────
; Named values that never change. Magic numbers given meaningful names.
; EQU creates compile-time constants (no memory used at runtime).
;
; See: standards/code/4-block/sections/setup/CWS-SECTION-SETUP-002-constants.md
; (Note: In .inc files, constants are BODY content, not SETUP)

; ────────────────────────────────────────────────────────────────
; [Category 1 Name] Constants
; ────────────────────────────────────────────────────────────────
; [Brief explanation of this group and their purpose]
;
; Extension Point:
; To add new [constant type], add EQU definition following naming pattern:
;   [PREFIX]_[NAME]  equ  [value]  ; [Description]

; ; [CONSTANT_NAME] [brief description].
; ;
; ; Set to [value] based on [reasoning].
; [CONSTANT_NAME]     equ     [value]     ; [Inline context if needed]

; ; [ANOTHER_CONSTANT] [brief description].
; [ANOTHER_CONSTANT]  equ     [value]

; ────────────────────────────────────────────────────────────────
; [Category 2 Name] Constants
; ────────────────────────────────────────────────────────────────
; [Same documentation pattern as Category 1]

; [Reserved: Additional constant categories as component develops]

; ────────────────────────────────────────────────────────────────
; Macro Definitions
; ────────────────────────────────────────────────────────────────
; Reusable code patterns. Macros expand at assembly time.
; Use %macro/%endmacro for multi-line, %define for simple substitution.
;
; See: standards/code/4-block/sections/setup/CWS-SECTION-SETUP-002-constants.md
;
; Extension Point:
; To add new macro, follow pattern:
;   %macro MACRO_NAME [param_count]
;       ; Implementation using %1, %2, etc.
;   %endmacro

; ; [MACRO_NAME] [does what].
; ;
; ; Parameters:
; ;   %1: [Purpose of first parameter]
; ;   %2: [Purpose of second parameter]
; ;
; ; Example usage:
; ;   MACRO_NAME arg1, arg2
; ;
; %macro [MACRO_NAME] [param_count]
;     ; Macro body using %1, %2, etc.
; %endmacro

; ; [Simple define macro] [does what].
; %define [SIMPLE_MACRO](x)  ((x) + [value])

; [Reserved: Additional macro definitions as patterns emerge]

; ────────────────────────────────────────────────────────────────
; Structure Definitions
; ────────────────────────────────────────────────────────────────
; Data structure definitions using NASM struc/endstruc.
; Equivalent to C typedef struct in headers.
;
; See: standards/code/4-block/sections/setup/CWS-SECTION-SETUP-004-types.md
;
; Extension Point:
; To add new structure, follow pattern:
;   struc StructName
;       .field1:  resb/resw/resd/resq  [count]
;   endstruc

; ; [StructName] represents [what this models].
; ;
; ; [2-4 sentences: what it represents, when used, key constraints]
; ;
; ; Fields:
; ;   - [.field1]: [purpose and meaning] ([size] bytes)
; ;   - [.field2]: [purpose] ([size] bytes)
; ;
; ; Total Size: [StructName]_size bytes
; ;
; ; Example usage:
; ;   section .bss
; ;   my_instance:  istruc [StructName]
; ;       at [StructName].[field1], [type] [value]
; ;   iend
; ;
; struc [StructName]
;     .[field1]:    resd    1       ; [Purpose] (4 bytes)
;     .[field2]:    resb    [size]  ; [Purpose] ([size] bytes)
; endstruc

; [Reserved: Additional structure definitions as component develops]

; ────────────────────────────────────────────────────────────────
; External Symbol Declarations
; ────────────────────────────────────────────────────────────────
; Symbols from other files (EXTERN) and symbols this exports (GLOBAL).
; Equivalent to function prototypes and extern declarations in C headers.
;
; See: standards/code/4-block/sections/setup/CWS-SECTION-SETUP-005-type-methods.md
;
; Note: External declarations in includes allow assembly files to share
; the same extern list by including this file.

;--- External Dependencies ---
; Symbols this include's consumers typically need from other files.

; ; [symbol_name] [what it provides - from which file].
; [EXTERN [symbol_name]]

;--- Exported Symbols ---
; Symbols that files including this should export.
; Uncommon in includes - usually in source files.

; ; [symbol_name] [what this provides to other files].
; ; [GLOBAL [symbol_name]]

; [Reserved: Additional external declarations as needed]

; -----------------------------------------------------------------------------
; BODY Omission Guide
; -----------------------------------------------------------------------------
;
; ALL five sections MUST be present. Content may be reserved with reason:
;
;   - Organizational Chart: Rarely reserved - definitions structure benefits from map
;   - Constants: [Reserved: No EQU definitions - uses external constants only]
;   - Macro Definitions: [Reserved: No macros - pure data definitions only]
;   - Structure Definitions: [Reserved: No struc definitions - primitive types only]
;   - External Declarations: [Reserved: No EXTERN/GLOBAL - internal definitions only]
;
; Unlike METADATA (sections omitted entirely with [OMIT:]), BODY preserves
; all section headers with [Reserved:] notation for unused sections.
;
; Include files define, source files use:
;   - Org Chart documents what definitions are provided
;   - Constants provide EQU values for source files
;   - Macros provide code patterns for reuse
;   - Structures define data layouts
;   - External Declarations coordinate symbol visibility

; ============================================================================
; END BODY
; ============================================================================

; ============================================================================
; CLOSING
; ============================================================================
;
; For CLOSING structure explanation, see: standards/code/4-block/CWS-STD-008-CODE-closing-block.md
;
; -----------------------------------------------------------------------------
; CLOSING Sections Overview
; -----------------------------------------------------------------------------
;
; GROUP 1: CODING (Operations - Verify, Include, Clean)
;
; 1. INCLUDE VALIDATION (Verification)
;    Purpose: Verify include correctness - guard, symbols, no conflicts
;    Subsections: Include Guard → Symbol Verification → No Duplicate Definitions
;
; 2. INCLUDE USAGE (How It's Included)
;    [Reserved: Includes don't execute - they're %include'd and used]
;
; 3. INCLUDE CLEANUP (Dependency Management)
;    Purpose: Proper include order and dependency management
;    Subsections: Include Order → Dependency Chain
;
; GROUP 2: FINAL DOCUMENTATION (Synthesis - Reference Back to Earlier Blocks)
;
; 4. INCLUDE OVERVIEW (Summary with Back-References)
;    Purpose: High-level summary of definitions this include provides
;    References: METADATA "Purpose & Function", "Key Features"
;
; 5. MODIFICATION POLICY (Safe/Careful/Never)
;    Purpose: Guide future maintainers on what's safe to change
;    Subsections: Safe to Modify → Modify with Care → Never Modify (ABI)
;
; 6. LADDER AND BATON FLOW (Back-Reference to BODY)
;    Purpose: Point to BODY Organizational Chart for definitions structure
;    References: BODY "Organizational Chart - Definitions Structure"
;
; 7. SURGICAL UPDATE POINTS (Back-Reference to BODY)
;    Purpose: Point to BODY for adding new definitions
;    References: BODY section categories for definition placement
;
; 8. PERFORMANCE CONSIDERATIONS (Size/Alignment)
;    Purpose: Structure sizes, alignment requirements, padding
;    References: BODY structure definitions with size annotations
;
; 9. TROUBLESHOOTING GUIDE (Include Issues)
;    Purpose: Common include problems and solutions
;    Subsections: Missing Dependencies → Symbol Conflicts → Circular Includes
;
; 10. RELATED COMPONENTS (Include Dependencies)
;     Purpose: Point to related includes and source files that use this
;     References: METADATA "Dependencies" - required includes, consumers
;
; 11. FUTURE EXPANSIONS (Definitions Roadmap)
;     Purpose: Planned definitions, deprecations, additions
;     Subsections: Planned Definitions → Deprecations → ABI Changes
;
; 12. CONTRIBUTION GUIDELINES (Adding Definitions)
;     Purpose: How to add new definitions to this include
;     Subsections: Naming Convention → Documentation → ABI Considerations
;
; 13. QUICK REFERENCE (Include Pattern)
;     Purpose: Copy-paste ready include pattern
;     Subsections: Basic Include → With Guard Check → Conditional
;
; Section order: Validation → [Usage Reserved] → Cleanup → Overview → Policy → Ladder/Baton →
;                Surgical → Performance → Troubleshooting → Related → Future → Contribution → Reference
; This flows: verify → document → guide expansion

; ────────────────────────────────────────────────────────────────
; Code Validation: Include (Definitions Only)
; ────────────────────────────────────────────────────────────────
; For Code Validation section explanation, see: standards/code/4-block/sections/closing/CWS-SECTION-CLOSING-001-code-validation.md
;
; Include Validation:
;   - Include guard works correctly (%ifndef/%define pattern)
;   - All constants have valid values (no undefined symbols)
;   - All macros expand correctly
;   - No circular include dependencies
;   - Structures have correct field offsets
;
; Build Verification:
;   ; Create test file to verify include
;   ; test_include.asm:
;   ;   %include "[include-name].inc"
;   ;   section .text
;   ;   global _start
;   ;   _start:
;   ;       mov eax, [CONSTANT_NAME]  ; Test constant access
;   ;       hlt
;   ;
;   nasm -f elf32 test_include.asm -o test_include.o
;   ; Must assemble without errors
;
; Self-Containment Test:
;   ; Include should work standalone (with its dependencies)
;   ; No undefined symbols when including
;
; Integration Testing:
;   - Test with actual consuming .asm files
;   - Verify definitions match expected values
;   - Check structure sizes match C equivalents (if config-driven)

; ────────────────────────────────────────────────────────────────
; Code Execution: None (Include)
; ────────────────────────────────────────────────────────────────
; For Code Execution section explanation, see: standards/code/4-block/sections/closing/CWS-SECTION-CLOSING-002-code-execution.md
;
; This is an INCLUDE file - definitions only, no execution.
; All code using these definitions is in .asm source files.
; Includes are processed by NASM preprocessor at assembly time.
;
; Usage: %include "[path]/[include-name].inc"
;
; The include is processed by NASM, making all defined constants,
; macros, and structures available. No code executes - only
; definitions are expanded by the preprocessor.
;
; Example include and usage:
;
;     %include "[include-name].inc"
;
;     section .text
;     global _start
;     _start:
;         ; Use constants
;         mov eax, [CONSTANT_NAME]
;
;         ; Use macros
;         [MACRO_NAME] arg1, arg2
;
;         ; Use structures
;         mov ebx, [my_instance + StructName.field1]
;
;         hlt

; ────────────────────────────────────────────────────────────────
; Code Cleanup: None (Include)
; ────────────────────────────────────────────────────────────────
; For Code Cleanup section explanation, see: standards/code/4-block/sections/closing/CWS-SECTION-CLOSING-003-code-cleanup.md
;
; Include files define compile-time constructs only.
; No runtime resources to manage or clean up.
;
; Memory Impact:
;   - Constants (EQU): Zero bytes - compile-time substitution only
;   - Macros: Zero bytes - expanded inline at assembly time
;   - Structures: Define layout only - instances allocated elsewhere

; ════════════════════════════════════════════════════════════════
; FINAL DOCUMENTATION
; ════════════════════════════════════════════════════════════════

; ────────────────────────────────────────────────────────────────
; Include Overview & Integration Summary
; ────────────────────────────────────────────────────────────────
; For Include Overview section explanation, see: standards/code/4-block/sections/closing/CWS-SECTION-CLOSING-004-library-overview.md
;
; Purpose: See METADATA "Purpose & Function" section above
;
; Provides: See METADATA "Key Features" list above
;
; Quick summary:
;   - [1-2 sentence overview of what this include provides]
;
; Integration: %include in assembly files needing these definitions
;
; Architecture: Rails (configuration/constants) - foundational definitions

; ────────────────────────────────────────────────────────────────
; Modification Policy
; ────────────────────────────────────────────────────────────────
; For Modification Policy section explanation, see: standards/code/4-block/sections/closing/CWS-SECTION-CLOSING-005-modification-policy.md
;
; Safe to Modify (Extension Points):
;   ✅ Add new constants within existing categories (follow naming pattern)
;   ✅ Add new constant categories (follow section structure)
;   ✅ Add new macros (document parameters and usage)
;   ✅ Add new structure definitions (document field purposes)
;   ✅ Update values for configuration changes
;
; Modify with Extreme Care (Breaking Changes):
;   ⚠️ Changing existing constant names - breaks all consuming code
;   ⚠️ Changing constant values - may break dependent calculations
;   ⚠️ Changing macro signatures - breaks all macro invocations
;   ⚠️ Changing structure field order/sizes - breaks memory layout
;
; NEVER Modify (Foundational Rails):
;   ❌ 4-block structure (METADATA, SETUP, BODY, CLOSING)
;   ❌ Include guard naming convention
;   ❌ Remove definitions without deprecation plan
;   ❌ Change definitions that must match C equivalents (config-driven)

; ────────────────────────────────────────────────────────────────
; Ladder and Baton Flow
; ────────────────────────────────────────────────────────────────
; For Ladder and Baton Flow section explanation, see: standards/code/4-block/sections/closing/CWS-SECTION-CLOSING-006-ladder-baton-flow.md
;
; Ladder (Dependencies):
;   [dependency].inc (if any)
;        ↓
;   [this-include].inc  ← YOU ARE HERE
;        ↓
;   [consumer].asm (assembly sources using this)
;
; Baton Flow:
;   N/A - include files are compile-time definitions, no execution flow
;
; Config-Driven Integration (if applicable):
;   config/[name].toml
;        ↓ (generator)
;   [include].inc  ← YOU ARE HERE (assembly definitions)
;   [header].h     ← parallel (C definitions)

; ────────────────────────────────────────────────────────────────
; Surgical Update Points (Extension Guide)
; ────────────────────────────────────────────────────────────────
; For Surgical Update Points section explanation, see: standards/code/4-block/sections/closing/CWS-SECTION-CLOSING-007-surgical-update-points.md
;
; Adding constants:
;   - Find appropriate category in BODY "Constants" section
;   - Add EQU definition following existing pattern
;   - Document purpose in comment above definition
;
; Adding macros:
;   - Add to BODY "Macro Definitions" section
;   - Document parameters and example usage
;   - Follow %macro/%endmacro pattern
;
; Adding structures:
;   - Add to BODY "Structure Definitions" section
;   - Document all fields with purposes and sizes
;   - Calculate and document total structure size

; ────────────────────────────────────────────────────────────────
; Performance Considerations
; ────────────────────────────────────────────────────────────────
; For Performance Considerations section explanation, see: standards/code/4-block/sections/closing/CWS-SECTION-CLOSING-008-performance-considerations.md
;
; Include files have zero runtime overhead:
;   - Constants: Compile-time substitution (EQU)
;   - Macros: Inline expansion at assembly time
;   - Structures: Layout definitions only
;
; Assembly time impact:
;   - Large includes slightly increase assembly time
;   - Include guards prevent redundant processing
;   - No impact on generated code size (definitions only)

; ────────────────────────────────────────────────────────────────
; Troubleshooting Guide
; ────────────────────────────────────────────────────────────────
; For Troubleshooting Guide section explanation, see: standards/code/4-block/sections/closing/CWS-SECTION-CLOSING-009-troubleshooting-guide.md
;
; Problem: "symbol `[NAME]' redefined"
;   - Cause: Include guard not working or duplicate definition
;   - Solution: Verify %ifndef/%define pattern, check for duplicates
;
; Problem: "symbol `[NAME]' undefined"
;   - Cause: Include not added or wrong path
;   - Solution: Add %include "[path]/[include].inc" at top of file
;
; Problem: "macro `[NAME]' requires [N] parameters"
;   - Cause: Wrong number of arguments to macro
;   - Solution: Check macro definition for parameter count
;
; Problem: Structure size mismatch with C equivalent
;   - Cause: Field sizes or alignment differ
;   - Solution: Verify field sizes match C types, check alignment

; ────────────────────────────────────────────────────────────────
; Related Components & Dependencies
; ────────────────────────────────────────────────────────────────
; For Related Components section explanation, see: standards/code/4-block/sections/closing/CWS-SECTION-CLOSING-010-related-components.md
;
; See METADATA "Dependencies" section above for complete information.
;
; Quick summary:
; - Dependencies: [Other .inc files needed]
; - Consumers: [Assembly files using this include]
;
; C Equivalent (if config-driven):
;   - C header: include/[path]/[name].h
;   - Config source: config/[name].toml
;   - Generator: [tool that generates both from config]

; ────────────────────────────────────────────────────────────────
; Future Expansions & Roadmap
; ────────────────────────────────────────────────────────────────
; For Future Expansions section explanation, see: standards/code/4-block/sections/closing/CWS-SECTION-CLOSING-011-future-expansions.md
;
; Planned Definitions:
;   ✓ [Completed definition] - COMPLETED
;   ⏳ [Planned constant category]
;   ⏳ [Planned macro]
;   ⏳ [Planned structure]
;
; Research Areas:
;   - [Additional definitions to consider]
;   - [Config-driven generation improvements]

; ────────────────────────────────────────────────────────────────
; Closing Note
; ────────────────────────────────────────────────────────────────
; For Closing Note section explanation, see: standards/code/4-block/sections/closing/CWS-SECTION-CLOSING-012-closing-note.md
;
; This include is RAILS - foundational definitions for [component].
; [Explain its place in the system - what depends on these definitions].
;
; Modify thoughtfully - definition changes affect all consuming assembly.
; If config-driven, changes must stay synchronized with C header equivalent.
;
; "[Relevant Scripture verse]" - [Reference]

; ────────────────────────────────────────────────────────────────
; Quick Reference: Usage Examples
; ────────────────────────────────────────────────────────────────
; For Quick Reference section explanation, see: standards/code/4-block/sections/closing/CWS-SECTION-CLOSING-013-quick-reference.md
;
; Include:
;
;     %include "[path]/[include-name].inc"
;
; Use Constants:
;
;     mov eax, [CONSTANT_NAME]
;     cmp ebx, [ANOTHER_CONSTANT]
;
; Use Macros:
;
;     [MACRO_NAME] arg1, arg2
;
; Use Structures:
;
;     section .bss
;     my_data: istruc [StructName]
;         at [StructName].[field1], dd 0
;     iend
;
;     section .text
;     mov eax, [my_data + StructName.field1]
;
; Verify Include:
;
;     nasm -f elf32 -E test.asm | grep [CONSTANT_NAME]
;     ; Should show the expanded value

; -----------------------------------------------------------------------------
; CLOSING Omission Guide
; -----------------------------------------------------------------------------
;
; ALL thirteen sections MUST be present. Content may be reserved with reason:
;
; GROUP 1: CODING
;   - Include Validation: Guard and symbol verification
;   - Include Usage: [Reserved: Includes don't execute - they're %include'd]
;   - Include Cleanup: Include order and dependency management
;
; GROUP 2: FINAL DOCUMENTATION (mostly back-references)
;   - Include Overview: Summary of definitions provided
;   - Modification Policy: ABI considerations
;   - Ladder and Baton Flow: Back-reference to BODY definitions structure
;   - Surgical Update Points: Where to add new definitions
;   - Performance Considerations: Structure sizes and alignment
;   - Troubleshooting Guide: Common include issues
;   - Related Components: Related includes and consumers
;   - Future Expansions: [Reserved: Definitions complete, no planned additions]
;   - Contribution Guidelines: How to add new definitions
;   - Quick Reference: Include patterns
;
; Unlike BODY (which uses [Reserved:] inline), CLOSING sections can be
; entirely replaced with back-references to avoid duplication.
;
; The key principle: CLOSING synthesizes, METADATA/SETUP/BODY contain details.
; Don't repeat - reference back to where the information lives.

; ============================================================================
; END CLOSING
; ============================================================================

%endif ; [INCLUDE_GUARD_NAME]_INC
