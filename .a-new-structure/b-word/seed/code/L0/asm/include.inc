; %error "TEMPLATE: Remove this line when ready to include"
; @file include.inc
; @omni template --asm-include
;
; ═══════════════════════════════════════════════════════════════════════════════
; OMNICODE PRAGMA [PRAGMA]
; ═══════════════════════════════════════════════════════════════════════════════
;
; @legend
;   :req  REQUIRED   — must exist, validation fails without
;   :inh  INHERITED  — from template, override allowed
;   :ins  INSTANCE   — file-specific, unique values
; @endlegend
;
; ───────────────────────────────────────────────────────────────────────────────
; P.1 CORE — Identity [CORE]
; ───────────────────────────────────────────────────────────────────────────────
;
; @omni:req  key        = B-word-seed-code-asm-include
; @omni:req  from       = bereshit/word/omni/seed/B-word-omni-seed-code.omni
; @omni:req  at         = template
;
; ───────────────────────────────────────────────────────────────────────────────
; P.2 FAMILY — Classification [FAMILY]
; ───────────────────────────────────────────────────────────────────────────────
;
; @omni:inh  type       = include
; @omni:inh  subtype    = template
; @omni:inh  role       = seed
; @omni:inh  structure  = 4-block
;
; ───────────────────────────────────────────────────────────────────────────────
; P.3 INSTANCE — File Details [INSTANCE]
; ───────────────────────────────────────────────────────────────────────────────
;
; @omni:ins  component  = seed
; @omni:ins  layer      = code/asm
; @omni:ins  includes   = none
; @omni:ins  provides   = [ASM_INCLUDE_TEMPLATE]
;
; ───────────────────────────────────────────────────────────────────────────────
; P.4 ARCHITECTURE — Scaling Context [ARCH]
; ───────────────────────────────────────────────────────────────────────────────
;
; @omni:ins  layers     = [0:kernel, 1:system, 2:runtime, 3:framework,
;                          4:health, 5:network, 6:storage, 7:engine, 8:app]
; @omni:ins  scale      = 3^n [1, 3, 9, 27, 81, 243, 729, 2187, 6561]
; @omni:ins  pattern    = 4+1+4 [below:0-3, center:4, above:5-8]
; @omni:ins  anchor     = genesis_1_1 -> void(0) -> unity(1) -> derivation
;
; ───────────────────────────────────────────────────────────────────────────────
; P.5 SUMMARY — Human Identity [SUMMARY]
; ───────────────────────────────────────────────────────────────────────────────
;
; @omni:req  title      = Assembly Include File Template
; @omni:req  brief      = 4-block structure template for NASM assembly include files
;
; ═══════════════════════════════════════════════════════════════════════════════
; END PRAGMA [END]
; ═══════════════════════════════════════════════════════════════════════════════
;
; USAGE: cp include.inc dest.inc → update pragma → fill sections
;
; DEPENDENCY CLASSIFICATION: [PURE/DEPENDED] ([deps if DEPENDED])
;   - PURE: Self-contained — no external include dependencies
;   - DEPENDED: Needs other includes — list them: (needs: config.inc, types.inc)
;
; Assembly Include Notes:
;   - .inc files are assembly's equivalent to C .h header files
;   - Provide constants (EQU), macros (%macro), and type definitions (struc)
;   - No code sections — definitions only
;   - Use %ifndef/%define pattern for include guards
;
; [brief description of what this include file provides].
;
; [Include Name] Include — CPI-SI [Project/System Name]
;
; ═══════════════════════════════════════════════════════════════════════════════
; METADATA BLOCK [METADATA]
; ═══════════════════════════════════════════════════════════════════════════════
;
; @brief Identity and context for this include file.
;
; STRUCTURE: M.1-M.10 sections, grouped for readability:
;   - M.1 IDENTITY:    Core identity (file, brief, key, title, type, component, role)
;   - M.2-M.6:         State, Attribution, Location, Derivation, Classification
;   - M.7-M.10:        Intent, Grounding, Dependencies, Roadmap
;
; ═══════════════════════════════════════════════════════════════════════════════

; ═══════════════════════════════════════════════════════════════════════════════
; M.1 IDENTITY — Core Identity [IDENTITY]
; ═══════════════════════════════════════════════════════════════════════════════
;
; @file      seed_include.inc
; @brief     [Include Name] — [One-line description].
;
; Key:       [PROJECT-INCLUDE-NAME]
; Title:     [Human-Readable Title]
; Type:      Include (Definitions)
; Component: Rails — configuration/constants
; Role:      [Specific responsibility — what definitions this provides]

; ═══════════════════════════════════════════════════════════════════════════════
; M.2 STATE — Lifecycle State [STATE]
; ═══════════════════════════════════════════════════════════════════════════════
;
; @version   a-XX.XX
; @date      YYYY-MM-DD
;
; Status:    Active
; Created:   YYYY-MM-DD
; Updated:   YYYY-MM-DD

; ═══════════════════════════════════════════════════════════════════════════════
; M.3 ATTRIBUTION — Authorship & Rights [ATTRIBUTION]
; ═══════════════════════════════════════════════════════════════════════════════
;
; @author    SeedCallback (Architect)
; @author    SeedCallback (Implementation)
; @copyright © YYYY [Organization]. All rights reserved.

; ═══════════════════════════════════════════════════════════════════════════════
; M.4 LOCATION — File Position [LOCATION]
; ═══════════════════════════════════════════════════════════════════════════════
;
; Path:      [project/path/to/file.inc]

; ═══════════════════════════════════════════════════════════════════════════════
; M.5 DERIVATION — Template Lineage [DERIVATION]
; ═══════════════════════════════════════════════════════════════════════════════
;
; Derives:   bereshit/word/seed/code/asm/include.inc (template)

; ═══════════════════════════════════════════════════════════════════════════════
; M.6 CLASSIFICATION — Categorization [CLASSIFICATION]
; ═══════════════════════════════════════════════════════════════════════════════
;
; Tags:      [asm, nasm, include, constants, kernel, boot]

; ═══════════════════════════════════════════════════════════════════════════════
; M.7 INTENT — Purpose Statement [INTENT]
; ═══════════════════════════════════════════════════════════════════════════════
;
; Purpose:   [What definitions does this include provide?]
;
; Core Design: [Architectural pattern — e.g., configuration centralization]
;
; Key Features:
;   - [What constants it provides — major categories]
;   - [What macros it provides — reusable patterns]
;   - [What type definitions it provides — structures]

; ═══════════════════════════════════════════════════════════════════════════════
; M.8 GROUNDING — Biblical Foundation [GROUNDING]
; ═══════════════════════════════════════════════════════════════════════════════
;
; Scripture: [Book Chapter:Verse] — "[Quote or theme]"
; Principle: [Kingdom principle demonstrated]

; ═══════════════════════════════════════════════════════════════════════════════
; M.9 DEPENDENCIES — Required Components [DEPENDENCIES]
; ═══════════════════════════════════════════════════════════════════════════════
;
; Other Includes:
;   - [None | %include "path/to/other.inc" — purpose]
;
; C Header Equivalent:
;   - [None | include/path/to/name.h — if config-driven]
;
; Used by:
;   - [Assembly source files that include this]
;
; Usage:
;   %include "[path]/[include-name].inc"

; ═══════════════════════════════════════════════════════════════════════════════
; M.10 ROADMAP — Version History [ROADMAP]
; ═══════════════════════════════════════════════════════════════════════════════
;
; History:
;   a-XX.XX (YYYY-MM-DD) — [Initial creation or significant change]
;   a-XX.XX (YYYY-MM-DD) — [Subsequent change]

; ═══════════════════════════════════════════════════════════════════════════════
; END METADATA [END]
; ═══════════════════════════════════════════════════════════════════════════════

; ═══════════════════════════════════════════════════════════════════════════════
; SETUP BLOCK [SETUP]
; ═══════════════════════════════════════════════════════════════════════════════
;
; @brief Configuration before definitions — primarily include guard.
;
; 4-Block Code Structure: METADATA → SETUP → BODY → CLOSING
;
; NOTE: Include files are primarily definitions (BODY). SETUP is minimal.
;
; CONTAINS:
;   - S.1 GUARD     — Include guard (%ifndef/%define)
;   - S.2 INCLUDES  — Required include dependencies
;
; Section order: Guard → Includes
; Flow: prevent multiple inclusion → resolve dependencies
;
; ═══════════════════════════════════════════════════════════════════════════════

; ═══════════════════════════════════════════════════════════════════════════════
; S.1 GUARD — Include Guard [GUARD]
; ═══════════════════════════════════════════════════════════════════════════════
;
; @brief Prevents multiple inclusion errors.
;
; PURPOSE: NASM uses %ifndef/%define pattern for include guards.
;          Guard name should be unique: [PROJECT]_[PATH]_[NAME]_INC
;
; ═══════════════════════════════════════════════════════════════════════════════

%ifndef [INCLUDE_GUARD_NAME]_INC
%define [INCLUDE_GUARD_NAME]_INC

; ═══════════════════════════════════════════════════════════════════════════════
; S.2 INCLUDES — Required Include Dependencies [INCLUDES]
; ═══════════════════════════════════════════════════════════════════════════════
;
; @brief Other include files this one depends on.
;
; PURPOSE: Order matters — include dependencies before using their definitions.
;
; ═══════════════════════════════════════════════════════════════════════════════

; ───────────────────────────────────────────────────────────────────────────────
; S.2a PROJECT — Project Includes [PROJECT]
; ───────────────────────────────────────────────────────────────────────────────

; %include "[path]/[dependency].inc"  ; [Purpose — what definitions needed]

; ───────────────────────────────────────────────────────────────────────────────
; S.2b EXTERNAL — External Includes [EXTERNAL]
; ───────────────────────────────────────────────────────────────────────────────

; [Reserved: No external assembly includes in kernel]

; ═══════════════════════════════════════════════════════════════════════════════
; END SETUP [END]
; ═══════════════════════════════════════════════════════════════════════════════

; ═══════════════════════════════════════════════════════════════════════════════
; BODY BLOCK [BODY]
; ═══════════════════════════════════════════════════════════════════════════════
;
; @brief The actual work — definitions (includes don't contain code).
;
; 4-Block Code Structure: METADATA → SETUP → BODY → CLOSING
;
; NOTE: Assembly include files DECLARE definitions, they do NOT contain code.
;       This BODY section provides the definitions this include exports.
;
; CONTAINS:
;   - B.1 ORG CHART   — Definitions structure (what this provides)
;   - B.2 CONSTANTS   — EQU definitions (compile-time values)
;   - B.3 MACROS      — Reusable code patterns (%macro)
;   - B.4 STRUCTURES  — Data structure layouts (struc)
;   - B.5 EXTERNAL    — Symbol declarations (EXTERN/GLOBAL)
;
; Section order: Org Chart → Constants → Macros → Structures → External
; Flow: map definitions → values → patterns → layouts → symbols
;
; ═══════════════════════════════════════════════════════════════════════════════

; ═══════════════════════════════════════════════════════════════════════════════
; B.1 ORG CHART — Definitions Structure [ORGCHART]
; ═══════════════════════════════════════════════════════════════════════════════
;
; @brief Map what this include provides.
;
; WHY: Navigation for consumers (what's available) and maintainers (what's defined).
;
; Definitions Provided (by category):
;
;   Constants (EQU)
;   ├── [Category 1]
;   │   ├── [CONSTANT_1] = [value] — [purpose]
;   │   └── [CONSTANT_2] = [value] — [purpose]
;   └── [Category 2]
;       └── [CONSTANT_3] = [value] — [purpose]
;
;   Macros (%macro)
;   ├── [MACRO_1](params) — [purpose]
;   └── [MACRO_2](params) — [purpose]
;
;   Structures (struc)
;   └── [StructName] — [purpose]
;       ├── .[field1] — [type/size]
;       └── .[field2] — [type/size]
;
; Definition Counts:
;   [X] constants total
;   [X] macros
;   [X] structures
;
; ═══════════════════════════════════════════════════════════════════════════════

; ═══════════════════════════════════════════════════════════════════════════════
; B.2 CONSTANTS — EQU Definitions [CONSTANTS]
; ═══════════════════════════════════════════════════════════════════════════════
;
; @brief Named compile-time values — no memory used at runtime.
;
; PURPOSE: Magic numbers with meaning. EQU creates compile-time constants.
;
; Extension Point:
;   To add new constant, follow naming pattern:
;     [PREFIX]_[NAME]  equ  [value]  ; [Description]
;
; ═══════════════════════════════════════════════════════════════════════════════

; ───────────────────────────────────────────────────────────────────────────────
; B.2a [Category 1 Name] Constants [CATEGORY1]
; ───────────────────────────────────────────────────────────────────────────────
; [Brief explanation of this group and their purpose]

; [CONSTANT_NAME]     equ     [value]     ; [brief description]
; [ANOTHER_CONSTANT]  equ     [value]     ; [brief description]

; ───────────────────────────────────────────────────────────────────────────────
; B.2b [Category 2 Name] Constants [CATEGORY2]
; ───────────────────────────────────────────────────────────────────────────────

; [Reserved: Additional constant categories as component develops]

; ═══════════════════════════════════════════════════════════════════════════════
; B.3 MACROS — Reusable Code Patterns [MACROS]
; ═══════════════════════════════════════════════════════════════════════════════
;
; @brief Macros expand at assembly time — code patterns for reuse.
;
; PURPOSE: Use %macro/%endmacro for multi-line, %define for simple substitution.
;
; Extension Point:
;   %macro MACRO_NAME [param_count]
;       ; Implementation using %1, %2, etc.
;   %endmacro
;
; ═══════════════════════════════════════════════════════════════════════════════

; [MACRO_NAME] [does what]
;
; @brief  [Brief explanation]
;
; Parameters:
;   %1: [Purpose of first parameter]
;   %2: [Purpose of second parameter]
;
; Example usage:
;   MACRO_NAME arg1, arg2
;
; %macro [MACRO_NAME] [param_count]
;     ; Macro body using %1, %2, etc.
; %endmacro

; [Reserved: Additional macro definitions as patterns emerge]

; ═══════════════════════════════════════════════════════════════════════════════
; B.4 STRUCTURES — Data Structure Layouts [STRUCTURES]
; ═══════════════════════════════════════════════════════════════════════════════
;
; @brief Data structure definitions using NASM struc/endstruc.
;
; PURPOSE: Equivalent to C typedef struct in headers.
;
; Extension Point:
;   struc StructName
;       .field1:  resb/resw/resd/resq  [count]
;   endstruc
;
; ═══════════════════════════════════════════════════════════════════════════════

; [StructName] represents [what this models]
;
; @brief  [2-4 sentences: what it represents, when used, key constraints]
;
; Fields:
;   .[field1]: [purpose and meaning] ([size] bytes)
;   .[field2]: [purpose] ([size] bytes)
;
; Total Size: [StructName]_size bytes
;
; Example usage:
;   section .bss
;   my_instance:  istruc [StructName]
;       at [StructName].[field1], dd 0
;   iend
;
; struc [StructName]
;     .[field1]:    resd    1       ; [Purpose] (4 bytes)
;     .[field2]:    resb    [size]  ; [Purpose] ([size] bytes)
; endstruc

; [Reserved: Additional structure definitions as component develops]

; ═══════════════════════════════════════════════════════════════════════════════
; B.5 EXTERNAL — Symbol Declarations [EXTERNAL]
; ═══════════════════════════════════════════════════════════════════════════════
;
; @brief Symbols from other files (EXTERN) and exports (GLOBAL).
;
; PURPOSE: Equivalent to function prototypes and extern declarations in C headers.
;          External declarations in includes allow assembly files to share
;          the same extern list by including this file.
;
; ═══════════════════════════════════════════════════════════════════════════════

; ───────────────────────────────────────────────────────────────────────────────
; B.5a EXTERN — External Dependencies [EXTERN]
; ───────────────────────────────────────────────────────────────────────────────

; extern [symbol_name]           ; [what it provides — from which file]

; ───────────────────────────────────────────────────────────────────────────────
; B.5b GLOBAL — Exported Symbols [GLOBAL]
; ───────────────────────────────────────────────────────────────────────────────

; [Reserved: Uncommon in includes — usually in source files]

; ═══════════════════════════════════════════════════════════════════════════════
; END BODY [END]
; ═══════════════════════════════════════════════════════════════════════════════

; ═══════════════════════════════════════════════════════════════════════════════
; CLOSING BLOCK [CLOSING]
; ═══════════════════════════════════════════════════════════════════════════════
;
; @brief Synthesis, verification, and forward guidance.
;
; 4-Block Code Structure: METADATA → SETUP → BODY → CLOSING
;
; GROUP 1 — OPERATIONS (Verify, Execute, Clean):
;   - X.1 VALIDATION — Include guard and symbol verification
;   - X.2 EXECUTION  — [Reserved: Includes don't execute]
;   - X.3 CLEANUP    — [Reserved: Compile-time only]
;
; GROUP 2 — DOCUMENTATION (Synthesis and References):
;   - X.4 OVERVIEW    — Include summary (references METADATA)
;   - X.5 POLICY      — Modification guidelines
;   - X.6 FLOW        — Ladder and baton (references BODY)
;   - X.7 PERF        — Performance considerations
;   - X.8 DEBUG       — Troubleshooting guide
;   - X.9 RELATED     — Related components
;   - X.10 ROADMAP    — Future expansions
;   - X.11 REFERENCE  — Quick reference patterns
;
; ═══════════════════════════════════════════════════════════════════════════════

; ═══════════════════════════════════════════════════════════════════════════════
; X.1 VALIDATION — Include Guard & Symbol Verification [VALIDATION]
; ═══════════════════════════════════════════════════════════════════════════════
;
; @brief Prove correctness — guard, symbols, no conflicts.
;
; Include Guard Verification:
;   - %ifndef/%define pattern at SETUP S.1 GUARD
;   - Symbol matches filename pattern [UPPER_SNAKE]_INC
;   - %endif present at end of file
;
; Symbol Verification:
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
; Integration Testing:
;   - Test with actual consuming .asm files
;   - Verify definitions match expected values
;   - Check structure sizes match C equivalents (if config-driven)

; ═══════════════════════════════════════════════════════════════════════════════
; X.2 EXECUTION — [Reserved: Include] [EXECUTION]
; ═══════════════════════════════════════════════════════════════════════════════
;
; @brief [Reserved: Includes don't execute — preprocessor definitions only]
;
; This is an INCLUDE file — definitions only, no execution.
; All code using these definitions is in .asm source files.
; Includes are processed by NASM preprocessor at assembly time.
;
; Usage: %include "[path]/[include-name].inc"
;
; The include is processed by NASM, making all defined constants,
; macros, and structures available. No code executes — only
; definitions are expanded by the preprocessor.

; ═══════════════════════════════════════════════════════════════════════════════
; X.3 CLEANUP — [Reserved: Compile-Time Only] [CLEANUP]
; ═══════════════════════════════════════════════════════════════════════════════
;
; @brief [Reserved: Include files define compile-time constructs only]
;
; No runtime resources to manage or clean up.
;
; Memory Impact:
;   - Constants (EQU): Zero bytes — compile-time substitution only
;   - Macros: Zero bytes — expanded inline at assembly time
;   - Structures: Define layout only — instances allocated elsewhere

; ═══════════════════════════════════════════════════════════════════════════════
; X.4 OVERVIEW — Include Summary [OVERVIEW]
; ═══════════════════════════════════════════════════════════════════════════════
;
; @brief High-level summary — references METADATA for details.
;
; Purpose:  See METADATA M.7 INTENT section above
; Provides: See METADATA M.7 INTENT "Key Features" list above
;
; Quick summary:
;   [1-2 sentence overview of what this include provides]
;
; Integration: %include in assembly files needing these definitions
;
; Architecture: Rails (configuration/constants) — foundational definitions

; ═══════════════════════════════════════════════════════════════════════════════
; X.5 POLICY — Modification Guidelines [POLICY]
; ═══════════════════════════════════════════════════════════════════════════════
;
; @brief Guide future maintainers on what's safe to change.
;
; Safe to Modify (Extension Points):
;   ✅ Add new constants within existing categories (follow naming pattern)
;   ✅ Add new constant categories (follow section structure)
;   ✅ Add new macros (document parameters and usage)
;   ✅ Add new structure definitions (document field purposes)
;   ✅ Update values for configuration changes
;
; Modify with Extreme Care (Breaking Changes):
;   ⚠️ Changing existing constant names — breaks all consuming code
;   ⚠️ Changing constant values — may break dependent calculations
;   ⚠️ Changing macro signatures — breaks all macro invocations
;   ⚠️ Changing structure field order/sizes — breaks memory layout
;
; NEVER Modify (Foundational Rails):
;   ❌ 4-block structure (METADATA, SETUP, BODY, CLOSING)
;   ❌ Include guard naming convention
;   ❌ Remove definitions without deprecation plan
;   ❌ Change definitions that must match C equivalents (if config-driven)

; ═══════════════════════════════════════════════════════════════════════════════
; X.6 FLOW — Ladder & Baton [FLOW]
; ═══════════════════════════════════════════════════════════════════════════════
;
; @brief Include chain — references BODY B.1 ORG CHART for details.
;
; See BODY B.1 ORG CHART for complete structure.
;
; Ladder (Dependencies):
;   [dependency].inc (if any)
;        ↓
;   [this-include].inc  ← YOU ARE HERE
;        ↓
;   [consumer].asm (assembly sources using this)
;
; Baton Flow:
;   N/A — include files are compile-time definitions, no execution flow
;
; Config-Driven Integration (if applicable):
;   config/[name].toml
;        ↓ (generator)
;   [include].inc  ← YOU ARE HERE (assembly definitions)
;   [header].h     ← parallel (C definitions)

; ═══════════════════════════════════════════════════════════════════════════════
; X.7 PERF — Performance Considerations [PERF]
; ═══════════════════════════════════════════════════════════════════════════════
;
; @brief Zero runtime overhead — compile-time only.
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

; ═══════════════════════════════════════════════════════════════════════════════
; X.8 DEBUG — Troubleshooting Guide [DEBUG]
; ═══════════════════════════════════════════════════════════════════════════════
;
; @brief Common include issues and debugging techniques.
;
; Problem: "symbol `[NAME]' redefined"
;   Cause:    Include guard not working or duplicate definition
;   Solution: Verify %ifndef/%define pattern, check for duplicates
;
; Problem: "symbol `[NAME]' undefined"
;   Cause:    Include not added or wrong path
;   Solution: Add %include "[path]/[include].inc" at top of file
;
; Problem: "macro `[NAME]' requires [N] parameters"
;   Cause:    Wrong number of arguments to macro
;   Solution: Check macro definition for parameter count
;
; Problem: Structure size mismatch with C equivalent
;   Cause:    Field sizes or alignment differ
;   Solution: Verify field sizes match C types, check alignment

; ═══════════════════════════════════════════════════════════════════════════════
; X.9 RELATED — Related Components [RELATED]
; ═══════════════════════════════════════════════════════════════════════════════
;
; @brief Related includes and dependencies — references METADATA M.9.
;
; See METADATA M.9 DEPENDENCIES section above for complete information.
;
; Quick summary:
;   Dependencies: [Other .inc files needed]
;   Consumers:    [Assembly files using this include]
;
; C Equivalent (if config-driven):
;   - C header: include/[path]/[name].h
;   - Config source: config/[name].toml
;   - Generator: [tool that generates both from config]

; ═══════════════════════════════════════════════════════════════════════════════
; X.10 ROADMAP — Future Expansions [ROADMAP]
; ═══════════════════════════════════════════════════════════════════════════════
;
; @brief Planned definitions, deprecations, additions.
;
; Planned Definitions:
;   ✓ [Completed definition] — COMPLETED
;   ⏳ [Planned constant category]
;   ⏳ [Planned macro]
;   ⏳ [Planned structure]
;
; Research Areas:
;   - [Additional definitions to consider]
;   - [Config-driven generation improvements]

; ═══════════════════════════════════════════════════════════════════════════════
; X.11 REFERENCE — Quick Reference [REFERENCE]
; ═══════════════════════════════════════════════════════════════════════════════
;
; @brief Copy-paste ready patterns for common operations.
;
; Include:
;     %include "[path]/[include-name].inc"
;
; Use Constants:
;     mov eax, [CONSTANT_NAME]
;     cmp ebx, [ANOTHER_CONSTANT]
;
; Use Macros:
;     [MACRO_NAME] arg1, arg2
;
; Use Structures:
;     section .bss
;     my_data: istruc [StructName]
;         at [StructName].[field1], dd 0
;     iend
;
;     section .text
;     mov eax, [my_data + StructName.field1]
;
; Verify Include:
;     nasm -f elf32 -E test.asm | grep [CONSTANT_NAME]
;     ; Should show the expanded value
;
; Closing Note:
;   This include is RAILS — foundational definitions for [component].
;   [Explain its place in the system — what depends on these definitions].
;
;   Modify thoughtfully — definition changes affect all consuming assembly.
;   If config-driven, changes must stay synchronized with C header equivalent.
;
;   "[Relevant Scripture verse]" — [Reference]

; ═══════════════════════════════════════════════════════════════════════════════
; END CLOSING [END]
; ═══════════════════════════════════════════════════════════════════════════════

%endif ; [INCLUDE_GUARD_NAME]_INC
