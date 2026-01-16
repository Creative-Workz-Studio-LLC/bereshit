; ═══════════════════════════════════════════════════════════════════════════
; TEMPLATE: Assembly Include File (4-Block Structure)
; Key: CODE-ASM-002
; ═══════════════════════════════════════════════════════════════════════════
;
; DEPENDENCY CLASSIFICATION: [PURE/DEPENDED] ([deps if DEPENDED])
;   - PURE: Self-contained - no external include dependencies
;   - DEPENDED: Needs other includes - list them: (needs: config.inc, types.inc)
;
; This is a TEMPLATE file - copy and modify for new assembly include files.
; Replace all [bracketed] placeholders with actual content.
; Rename to appropriate name (e.g., config.inc, types.inc, constants.inc).
;
; Assembler: NASM (Netwide Assembler)
; Derived from: templates/code/c/CODE-C-002-C-header.h
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
; For METADATA structure explanation, see: standards/code/4-block/CWS-STD-004-CODE-metadata-block.md
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
; # Blocking Status
;
; Non-blocking: Include files are compile-time only, no execution
;
; Mitigation: N/A - preprocessor definitions
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
; # Health Scoring
;
; Health Impact: None - compile-time definitions only
;
; Note: Include files don't track health. Errors here cause build failures.

; ============================================================================
; END METADATA
; ============================================================================

; ============================================================================
; SETUP
; ============================================================================
;
; For SETUP structure explanation, see: standards/code/4-block/CWS-STD-006-CODE-setup-block.md
;
; Section order: Include Guard → Required Includes
; This flows: protection from multiple inclusion → dependencies
;
; Why this order: Include guard must come first to prevent redefinition.
; Dependencies must be resolved before using their definitions.
;
; IMPORTANT: All sections MUST be present, even if empty or reserved.
; A lean section comment is better than absence. Why:
;   - Consistent structure across all files (navigation)
;   - Clear extension points (where to add when needed)
;   - Intentional vs forgotten (a reserved section is deliberate)
;
; For empty sections, use: ; [Reserved: Brief reason why not needed]

; ────────────────────────────────────────────────────────────────
; Include Guard
; ────────────────────────────────────────────────────────────────
;
; Prevents multiple inclusion errors. NASM uses %ifndef/%define pattern.
; Guard name should be unique - typically: [PROJECT]_[PATH]_[NAME]_INC
;
; See: standards/code/4-block/sections/setup/CWS-SECTION-SETUP-001-imports.md

%ifndef [INCLUDE_GUARD_NAME]_INC
%define [INCLUDE_GUARD_NAME]_INC

; ────────────────────────────────────────────────────────────────
; Required Includes
; ────────────────────────────────────────────────────────────────
;
; Other include files this one depends on. Order matters - include
; dependencies before using their definitions.
;
; See: standards/code/4-block/sections/setup/CWS-SECTION-SETUP-001-imports.md

;--- Project Includes ---
; Other project includes this depends on.

; %include "[path]/[dependency].inc"  ; [Purpose - what definitions needed]

;--- Standard Includes ---
; [Reserved: Assembly has no standard includes - bare metal]

;--- External Includes ---
; [Reserved: No external assembly includes in kernel]

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

; ============================================================================
; END BODY
; ============================================================================

; ============================================================================
; CLOSING
; ============================================================================
;
; For CLOSING structure explanation, see: standards/code/4-block/CWS-STD-008-CODE-closing-block.md

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

; ============================================================================
; END CLOSING
; ============================================================================

%endif ; [INCLUDE_GUARD_NAME]_INC
