@ .error "TEMPLATE: Remove this line when ready to assemble"
@ ═══════════════════════════════════════════════════════════════════════════
@ TEMPLATE: ARM Assembly Source File (4-Block Structure)
@ Key: B-word-seed-code-asm-arm-source
@ ═══════════════════════════════════════════════════════════════════════════
@
@ DEPENDENCY CLASSIFICATION: [PURE/DEPENDED] ([deps if DEPENDED])
@   - PURE: Self-contained - no external dependencies beyond CPU
@   - DEPENDED: Needs external symbols - list them: (needs: kernel_main, uart_init)
@
@ This is a TEMPLATE file - copy and modify for new ARM assembly source files.
@ Replace all [bracketed] placeholders with actual content.
@ Rename to appropriate name (e.g., entry.s, boot.s, vectors.s).
@ Remove the ".error" line above when ready to assemble.
@
@ Assembler: GNU AS (Gas) for ARM
@ derives_from: bereshit/word/omni/seed/code.omni
@ Derived from: Kingdom Technology 4-block code structure
@ See: standards/code/4-block/ for complete documentation
@
@ ARM Assembly Format Notes:
@   - GNU AS syntax (unified ARM/Thumb)
@   - Comments use at-sign (@) or C-style (/* */)
@   - Labels end with colon (:)
@   - Directives start with period (.)
@   - ARM uses load/store architecture (can't operate directly on memory)
@
@ ARM Architecture Targets:
@   - ARMv6: Raspberry Pi 1, embedded
@   - ARMv7: Raspberry Pi 2/3 (32-bit), most embedded
@   - ARMv8/AArch64: Raspberry Pi 3/4 (64-bit), modern ARM
@
@ ═══════════════════════════════════════════════════════════════════════════

@ [brief description of what this assembly file implements].
@
@ [Module Name] - CPI-SI [Project/System Name]
@
@ ============================================================================
@ METADATA
@ ============================================================================
@
@ ────────────────────────────────────────────────────────────────
@ CORE IDENTITY (Required)
@ ────────────────────────────────────────────────────────────────
@
@ # Biblical Foundation
@
@ Scripture: [Relevant verse grounding this module's purpose]
@
@ Principle: [Kingdom principle this module demonstrates]
@
@ Anchor: [Supporting verse reinforcing the principle]
@
@ # CPI-SI Identity
@
@ Component Type: [Ladder/Baton/Rails - see CWS-STD-004 for explanations]
@
@ Role: [Specific responsibility in system architecture]
@
@ Paradigm: CPI-SI framework component
@
@ # Authorship & Lineage
@
@   - Architect: [Who designed the approach and requirements]
@   - Implementation: [Who wrote the code and verified it works]
@   - Created: [YYYY-MM-DD]
@   - Version: [MAJOR.MINOR.PATCH]
@   - Modified: [YYYY-MM-DD - what changed]
@
@ Version History:
@
@   - [X.Y.Z] ([YYYY-MM-DD]) - [Brief description of changes]
@   - [X.Y.Z] ([YYYY-MM-DD]) - [Brief description of changes]
@
@ # Purpose & Function
@
@ Purpose: [What problem does this module solve?]
@
@ Core Design: [Architectural pattern or paradigm]
@
@ Key Features:
@
@   - [What it provides - major capabilities]
@   - [What it enables - what others can build with this]
@   - [What problems it solves - specific use cases]
@
@ Philosophy: [Guiding principle for how this module works]
@
@ ────────────────────────────────────────────────────────────────
@ INTERFACE (Expected)
@ ────────────────────────────────────────────────────────────────
@
@ # Dependencies
@
@ What This Needs:
@
@   - CPU Features: [list required features - e.g., NEON, VFP]
@   - External Symbols: [None | list extern symbols]
@   - Memory Layout: [Any assumptions about memory layout]
@
@ What Uses This:
@
@   - Linker: [How this gets linked]
@   - Bootloader: [If applicable]
@   - C code: [If called from C]
@
@ Integration Points:
@
@   - [How other systems connect - AAPCS calling convention]
@   - [Cross-component interactions]
@   - [Register state expectations]
@
@ # Usage & Integration
@
@ Assemble:
@
@   arm-none-eabi-as [source].s -o [output].o
@   arm-none-eabi-as -march=[arch] [source].s -o [output].o
@
@ Common architectures:
@   - armv6:    ARMv6 (Raspberry Pi 1)
@   - armv7-a:  ARMv7-A (Cortex-A series, RPi 2/3 32-bit)
@   - armv8-a:  ARMv8-A AArch32 mode
@
@ Link:
@   arm-none-eabi-ld -T linker.ld -o [output].elf [objects...]
@
@ ────────────────────────────────────────────────────────────────
@ OPERATIONAL (Contextual)
@ ────────────────────────────────────────────────────────────────
@
@ # Blocking Status
@
@ [Blocking/Non-blocking]: [Brief explanation]
@
@ Mitigation: [How blocking/failures handled]
@
@ # Health Scoring
@
@ [OMIT: Assembly modules are foundations - health tracked at higher levels]
@
@ Note: Assembly modules typically don't track health - they're foundations.
@ Health scoring occurs in the C/Go code that calls these routines.
@
@ ────────────────────────────────────────────────────────────────
@ METADATA Omission Guide
@ ────────────────────────────────────────────────────────────────
@
@ Tier 1 (CORE IDENTITY): Never omit - every file needs these.
@
@ Tier 2 (INTERFACE): May omit with [OMIT: reason] notation.
@   - Dependencies: Required for ARM ASM - documents CPU features and extern symbols
@   - Usage & Integration: Required - shows assemble/link commands
@
@ Tier 3 (OPERATIONAL): Include when applicable to file type.
@   - Blocking Status: Include for boot code, interrupt handlers
@   - Health Scoring: [OMIT: Assembly is foundational - health at higher levels]
@
@ Unlike SETUP (all sections required), METADATA omission signals component characteristics.

@ ============================================================================
@ END METADATA
@ ============================================================================

@ ============================================================================
@ SETUP
@ ============================================================================
@
@ For SETUP structure explanation, see: standards/code/4-block/CWS-STD-006-CODE-setup-block.md
@
@ Section order: Directives → Constants → External Symbols → Data → BSS → Debug Infrastructure
@ This flows: assembler config → values → dependencies → initialized → uninitialized → infrastructure
@
@ IMPORTANT: All sections MUST be present, even if empty or reserved.
@ For empty sections, use: @ [Reserved: Brief reason why not needed]
@
@ -----------------------------------------------------------------------------
@ SETUP Sections Overview
@ -----------------------------------------------------------------------------
@
@ 1. ASSEMBLER DIRECTIVES (Dependencies)
@    Purpose: Configuration telling GNU AS how to assemble this file
@    Subsections: Architecture Selection → CPU Selection → FPU Selection → Syntax
@
@ 2. CONSTANTS (.equ Definitions)
@    Purpose: Fixed values that never change (compile-time, no memory used)
@    Subsections: Category Constants → Hardware Constants → Size Constants
@
@ 3. EXTERNAL SYMBOLS (Type Behaviors)
@    Purpose: Symbols defined elsewhere or exported to other modules
@    Subsections: External Dependencies (.extern) → Exported Symbols (.global)
@
@ 4. DATA SECTION (Variables - Initialized)
@    Purpose: Data with initial values, loaded into memory at runtime
@    Subsections: String Data → Numeric Data → Structured Data
@
@ 5. BSS SECTION (Variables - Uninitialized)
@    Purpose: Space reserved for runtime data, does not increase binary size
@    Subsections: Buffers → Stack → Heap Area
@
@ 6. DEBUG INFRASTRUCTURE (Rails Pattern)
@    Purpose: Debug output, logging markers, diagnostic symbols
@    Subsections: Debug Strings → Diagnostic Symbols

@ ────────────────────────────────────────────────────────────────
@ Assembler Directives
@ ────────────────────────────────────────────────────────────────
@
@ Configuration telling GNU AS how to assemble this file.
@ Must come before any code or data.
@
@ See: standards/code/4-block/sections/setup/CWS-SECTION-SETUP-001-imports.md

@--- Architecture Selection ---
@ .arch armv6          @ ARMv6 (Raspberry Pi 1)
@ .arch armv7-a        @ ARMv7-A (Cortex-A series)
@ .arch armv8-a        @ ARMv8-A (64-bit capable, 32-bit mode)

@--- CPU Selection ---
@ .cpu cortex-a7       @ Specific CPU (RPi 2)
@ .cpu cortex-a53      @ Specific CPU (RPi 3/4)

@--- FPU/SIMD Selection ---
@ .fpu vfp             @ Vector Floating Point
@ .fpu neon            @ NEON SIMD

@--- Instruction Set ---
@ .arm                 @ ARM (32-bit) instruction set
@ .thumb               @ Thumb (16-bit compact) instruction set
@ .syntax unified      @ Unified ARM/Thumb syntax

@ ────────────────────────────────────────────────────────────────
@ Constants (EQU Definitions)
@ ────────────────────────────────────────────────────────────────
@
@ Named values that never change. Magic numbers given meaningful names.
@ .equ creates compile-time constants (no memory used).
@
@ See: standards/code/4-block/sections/setup/CWS-SECTION-SETUP-002-constants.md

@--- [Category Name] Constants ---
@ [Brief explanation of this group and their purpose]

@ @ [CONSTANT_NAME] [brief description].
@ @
@ @ Set to [value] based on [reasoning].
@ .equ [CONSTANT_NAME], [value]
@
@ @ [ANOTHER_CONSTANT] [brief description].
@ .equ [ANOTHER_CONSTANT], [value]

@--- Hardware Base Addresses (BCM2835/BCM2836/BCM2837) ---
@ .equ PERIPHERAL_BASE, 0x20000000    @ RPi 1
@ .equ PERIPHERAL_BASE, 0x3F000000    @ RPi 2/3
@ .equ GPIO_BASE,       (PERIPHERAL_BASE + 0x200000)
@ .equ UART0_BASE,      (PERIPHERAL_BASE + 0x201000)

@--- Size Constants ---
@ .equ STACK_SIZE,      16384         @ 16KB stack
@ .equ PAGE_SIZE,       4096          @ 4KB page

@--- ARM Register Aliases ---
@ @ Standard AAPCS register names
@ .equ sp, r13                        @ Stack pointer
@ .equ lr, r14                        @ Link register
@ .equ pc, r15                        @ Program counter

@ ────────────────────────────────────────────────────────────────
@ External Symbols
@ ────────────────────────────────────────────────────────────────
@
@ Symbols defined in other files that this module needs.
@ .extern declares symbols to be resolved at link time.
@ .global exports symbols for other modules to use.
@
@ See: standards/code/4-block/sections/setup/CWS-SECTION-SETUP-005-type-methods.md

@--- External Dependencies ---
@ Symbols this module needs from other files.

@ .extern [symbol_name]     @ [Purpose - what this symbol provides]

@--- Exported Symbols ---
@ Symbols this module provides to other files.

@ .global [symbol_name]     @ [Purpose - what this symbol does]

@ ────────────────────────────────────────────────────────────────
@ Data Section (Initialized Data)
@ ────────────────────────────────────────────────────────────────
@
@ Data with initial values. Loaded into memory at runtime.
@ Use sparingly - increases binary size.
@
@ See: standards/code/4-block/sections/setup/CWS-SECTION-SETUP-003-variables.md

@ .section .data
@ .align 4

@--- String Data ---
@ @ [string_name] [brief description].
@ [string_name]:    .asciz "[string content]"      @ Null-terminated
@ [string_name]:    .ascii "[string content]\n\0"  @ With newline

@--- Numeric Data ---
@ @ [var_name] [brief description].
@ [var_name]:       .byte [value]      @ Byte (8-bit)
@ [var_name]:       .hword [value]     @ Half-word (16-bit)
@ [var_name]:       .word [value]      @ Word (32-bit)
@ [var_name]:       .quad [value]      @ Quad (64-bit, AArch64)

@ ────────────────────────────────────────────────────────────────
@ BSS Section (Uninitialized Data)
@ ────────────────────────────────────────────────────────────────
@
@ Space reserved for runtime data. Does not increase binary size.
@ Use .space/.skip to reserve space.
@
@ See: standards/code/4-block/sections/setup/CWS-SECTION-SETUP-003-variables.md

@ .section .bss
@ .align 4

@--- Buffers ---
@ @ [buffer_name] [brief description].
@ [buffer_name]:    .space [size]      @ Reserve [size] bytes

@--- Stack ---
@ @ Stack space for this module.
@ stack_bottom:
@     .space STACK_SIZE                @ Reserve stack space
@ stack_top:

@ ────────────────────────────────────────────────────────────────
@ Debug Infrastructure (Rails Pattern)
@ ────────────────────────────────────────────────────────────────
@
@ Debug output strings and diagnostic symbols for development.
@ Use conditional assembly to exclude from release builds.
@
@ See: standards/code/patterns/CWS-PATTERN-003-CODE-rails.md

@--- Debug Strings ---
@ Debug messages for development (exclude in release builds).

@ .ifdef DEBUG
@ .section .rodata
@ dbg_prefix:     .asciz "[component] "
@ dbg_init:       .asciz "Initializing...\n"
@ dbg_done:       .asciz "Done.\n"
@ .endif

@--- Diagnostic Symbols ---
@ Symbols for debugger breakpoints and diagnostic tools.

@ [Reserved: Add debug entry points as component develops]

@ -----------------------------------------------------------------------------
@ SETUP Omission Guide
@ -----------------------------------------------------------------------------
@
@ ALL sections MUST be present. Content may be reserved with reason:
@
@   - Assembler Directives: Rarely reserved - most files need .arch/.cpu
@   - Constants: [Reserved: No fixed values needed]
@   - External Symbols: [Reserved: Self-contained, no external dependencies]
@   - Data Section: [Reserved: No initialized data needed]
@   - BSS Section: [Reserved: No runtime buffers needed]
@   - Debug Infrastructure: [Reserved: Minimal component - no debug output]
@
@ Unlike METADATA (sections omitted entirely with [OMIT:]), SETUP preserves
@ all section headers with [Reserved:] notation for unused sections.

@ ============================================================================
@ END SETUP
@ ============================================================================

@ ============================================================================
@ BODY
@ ============================================================================
@
@ For BODY structure explanation, see: standards/code/4-block/CWS-STD-007-CODE-body-block.md
@
@ -----------------------------------------------------------------------------
@ BODY Sections Overview
@ -----------------------------------------------------------------------------
@
@ 1. ORGANIZATIONAL CHART (Internal Structure)
@    Purpose: Map label dependencies and execution flow within this module
@    Subsections: Label Structure → Execution Flow → APUs
@
@ 2. HELPERS/UTILITIES (Internal Support)
@    Purpose: Foundation routines - simple, focused, reusable
@    Subsections: Pure Computations → Hardware Access Helpers
@
@ 3. CORE OPERATIONS (Business Logic)
@    Purpose: Module-specific functionality implementing primary purpose
@    Subsections: [Category 1] → [Category 2] → ... (organized by concern)
@
@ 4. ERROR HANDLING (Safety Patterns)
@    Purpose: Error responses - halt, return codes, exception handling
@    Subsections: Design Principle → Error Strategy → halt_error
@
@ 5. PUBLIC ENTRY POINTS (Exported Interface)
@    Purpose: Labels exported via .global for external callers
@    Subsections: [Category 1] → [Category 2] → ... (organized by purpose)
@
@ Section order: Org Chart → Helpers → Core Operations → Error Handling → Entry Points
@ This flows: understand structure → build foundations → implement logic → handle errors → expose interface
@
@ Universal mapping (see standards for cross-language patterns):
@   Organizational Chart ≈ Dependency/Flow Documentation
@   Helpers/Utilities ≈ Internal Routines (not exported)
@   Core Operations ≈ Business Logic (the work)
@   Error Handling ≈ halt_error, wfi loops
@   Public Entry Points ≈ .global labels (what others call)

@ ────────────────────────────────────────────────────────────────
@ Organizational Chart - Internal Structure
@ ────────────────────────────────────────────────────────────────
@ Maps bidirectional dependencies and execution flow within this module.
@
@ See: standards/code/4-block/sections/body/CWS-SECTION-BODY-001-organizational-chart.md
@
@ Label Structure:
@
@   Entry Points (GLOBAL)
@   ├── [entry_label] → sets up, calls [helper_label]
@   └── [another_entry] → uses [utility_label]
@
@   Helpers (internal)
@   ├── [helper_label] → pure computation
@   └── [utility_label] → hardware access
@
@ Execution Flow:
@
@   External call → [entry_label]
@     ↓
@   [setup code]
@     ↓
@   [helper_label]
@     ↓
@   [cleanup/return]
@
@ APUs (Available Processing Units):
@ - [X] labels total
@ - [X] entry points (global)
@ - [X] helper routines (internal)

@ ────────────────────────────────────────────────────────────────
@ Text Section (Code)
@ ────────────────────────────────────────────────────────────────

.section .text

@ ────────────────────────────────────────────────────────────────
@ Helpers/Utilities - Internal Support
@ ────────────────────────────────────────────────────────────────
@ Foundation routines used throughout this module. Usually not exported.
@
@ See: standards/code/4-block/sections/body/CWS-SECTION-BODY-002-helpers.md
@
@ [Reserved: Additional helpers will emerge as module develops]

@ [helper_name] [does what]
@
@ What It Does:
@ [Brief explanation - helpers are usually simple and focused]
@
@ Parameters (registers):
@   r0: [Purpose and expected values]
@   r1: [Purpose and expected values]
@
@ Returns:
@   r0: [What's returned]
@
@ Clobbers: [registers modified]
@
@ [helper_name]:
@     @ Implementation
@     bx lr              @ Return (branch to link register)

@ ────────────────────────────────────────────────────────────────
@ Core Operations - Main Logic
@ ────────────────────────────────────────────────────────────────
@ Module-specific functionality implementing primary purpose.
@
@ See: standards/code/4-block/sections/body/CWS-SECTION-BODY-003-core-operations.md

@ ────────────────────────────────────────────────────────────────
@ [Category Name] - [Purpose]
@ ────────────────────────────────────────────────────────────────
@ What These Do:
@ [High-level description of this category of operations]
@
@ Calling Convention: AAPCS (ARM Architecture Procedure Call Standard)
@   - r0-r3:   Arguments and return values (caller-saved)
@   - r4-r11:  Callee-saved (must preserve)
@   - r12:     Intra-procedure scratch (IP)
@   - r13/sp:  Stack pointer
@   - r14/lr:  Link register (return address)
@   - r15/pc:  Program counter

@ [operation_name] [does what]
@
@ What It Does:
@ [Detailed explanation of operation purpose and behavior]
@
@ Parameters (registers):
@   r0: [Purpose and expected values]
@   r1: [Purpose and expected values]
@
@ Returns:
@   r0: [What's returned and meaning]
@
@ Clobbers: r0-r3, r12 (per AAPCS)
@
@ Stack Usage: [bytes used, if any]
@
@ Example usage:
@
@     ldr r0, =[value]
@     bl [operation_name]
@     @ Result in r0
@
@ [operation_name]:
@     @ Save callee-saved registers if needed
@     push {r4-r7, lr}
@
@     @ Implementation
@
@     @ Restore and return
@     pop {r4-r7, pc}      @ Pop lr directly into pc to return

@ ────────────────────────────────────────────────────────────────
@ Error Handling
@ ────────────────────────────────────────────────────────────────
@ Assembly error handling is typically simple - halt or return error code.
@
@ See: standards/code/4-block/sections/body/CWS-SECTION-BODY-004-error-handling.md
@
@ Design Principle: [Blocking/Non-blocking]
@
@ Error Handling Strategy:
@   - Invalid input: [Return error code / halt / ignore]
@   - Hardware fault: [Exception handler]
@   - Unrecoverable: [Infinite loop / WFI]

@ halt_error halts the CPU.
@
@ Used for unrecoverable errors. CPU will wait for interrupt.
@
@ halt_error:
@     cpsid i            @ Disable IRQ
@     cpsid f            @ Disable FIQ
@ .loop:
@     wfi                @ Wait for interrupt (low power halt)
@     b .loop            @ Loop if spurious wakeup

@ ────────────────────────────────────────────────────────────────
@ Public Entry Points - Exported Interface
@ ────────────────────────────────────────────────────────────────
@ Entry points exported via .global for external callers.
@
@ See: standards/code/4-block/sections/body/CWS-SECTION-BODY-005-public-apis.md

@ ═══ [Category Name] ═══

@ [entry_point_name] [does what at high level]
@
@ What It Does:
@ [Detailed explanation of complete operation]
@
@ Parameters (registers):
@   r0: [Purpose and expected values]
@
@ Returns:
@   r0: [What's returned and meaning]
@
@ Clobbers: r0-r3, r12 (per AAPCS)
@
@ Calling Convention: AAPCS
@
@ Example usage (from C):
@
@     extern void [entry_point_name](void);
@     [entry_point_name]();
@
@ Example usage (from assembly):
@
@     bl [entry_point_name]
@
@ .global [entry_point_name]
@ [entry_point_name]:
@     @ Setup
@
@     @ Core operation
@
@     @ Cleanup and return
@     bx lr

@ -----------------------------------------------------------------------------
@ BODY Omission Guide
@ -----------------------------------------------------------------------------
@
@ ALL five sections MUST be present. Content may be reserved with reason:
@
@   - Organizational Chart: Rarely reserved - label structure benefits from map
@   - Helpers/Utilities: [Reserved: No internal subroutines - uses library calls]
@   - Core Operations: Rarely reserved - contains primary instruction sequences
@   - Error Handling: [Reserved: Uses CPU exceptions, no custom error handlers]
@   - Public Entry Points: [Reserved: Internal module - no .global exports]
@
@ Unlike METADATA (sections omitted entirely with [OMIT:]), BODY preserves
@ all section headers with [Reserved:] notation for unused sections.
@
@ For multi-file ARM assembly projects:
@   - Entry file: Contains _start/main, .global exports, orchestration
@   - Module files: Contains internal routines, .extern imports
@   - Document linkage with [Reserved: Calls helpers from utils.s via .extern]

@ ============================================================================
@ END BODY
@ ============================================================================

@ ============================================================================
@ CLOSING
@ ============================================================================
@
@ For CLOSING structure explanation, see: standards/code/4-block/CWS-STD-008-CODE-closing-block.md
@
@ -----------------------------------------------------------------------------
@ CLOSING Sections Overview
@ -----------------------------------------------------------------------------
@
@ GROUP 1: CODING (Operations - Verify, Execute, Clean)
@
@ 1. CODE VALIDATION (Testing & Verification)
@    Purpose: Prove correctness - assemble, link, test
@    Subsections: Assembly Verification → Link Testing → Runtime Testing
@
@ 2. CODE EXECUTION (Entry Points & Flow)
@    Purpose: Entry point(s) and execution orchestration
@    Subsections: Entry Point → Execution Flow → Exit Codes → Exception Handling
@
@ 3. CODE CLEANUP (Resource Management)
@    Purpose: Register preservation, stack cleanup, resource release
@    Subsections: Register Preservation → Stack Management → Resource Cleanup
@
@ GROUP 2: FINAL DOCUMENTATION (Synthesis - Reference Back to Earlier Blocks)
@
@ 4. MODULE OVERVIEW (Summary with Back-References)
@    Purpose: High-level summary pointing back to METADATA for details
@    References: METADATA "Purpose & Function", "Key Features"
@
@ 5. MODIFICATION POLICY (Safe/Careful/Never)
@    Purpose: Guide future maintainers on what's safe to change
@    Subsections: Safe to Modify → Modify with Care → Never Modify
@
@ 6. LADDER AND BATON FLOW (Back-Reference to BODY)
@    Purpose: Point to BODY Organizational Chart for label structure
@    References: BODY "Organizational Chart - Internal Structure"
@
@ 7. SURGICAL UPDATE POINTS (Back-Reference to BODY)
@    Purpose: Point to BODY for adding new routines
@    References: BODY section categories for routine placement
@
@ 8. PERFORMANCE CONSIDERATIONS (Cycle Counts)
@    Purpose: Instruction timing, pipeline considerations, cache behavior
@    References: BODY routine comments with cycle counts
@
@ 9. TROUBLESHOOTING GUIDE (Debug Patterns)
@    Purpose: Common ARM assembly issues and debugging techniques
@    Subsections: Register Corruption → Stack Misalignment → Memory Access
@
@ 10. RELATED COMPONENTS (Module Dependencies)
@     Purpose: Point to related assembly modules and includes
@     References: METADATA "Dependencies" - .extern symbols, includes
@
@ 11. FUTURE EXPANSIONS (Module Roadmap)
@     Purpose: Planned routines, optimizations, platform support
@     Subsections: Planned Routines → Optimizations → Platform Support
@
@ 12. CONTRIBUTION GUIDELINES (Adding Routines)
@     Purpose: How to add new routines to this module
@     Subsections: Routine Structure → Register Convention → Documentation
@
@ 13. QUICK REFERENCE (Assembly Patterns)
@     Purpose: Copy-paste ready patterns for common operations
@     Subsections: Function Prologue/Epilogue → System Calls → Memory Operations
@
@ Section order: Validation → Execution → Cleanup → Overview → Policy → Ladder/Baton →
@                Surgical → Performance → Troubleshooting → Related → Future → Contribution → Reference
@ This flows: verify → run → clean → document → guide future work

@ ────────────────────────────────────────────────────────────────
@ Code Validation: [moduleName]
@ ────────────────────────────────────────────────────────────────
@ For Code Validation section explanation, see: standards/code/4-block/sections/closing/CWS-SECTION-CLOSING-001-code-validation.md
@
@ Build Verification:
@   arm-none-eabi-as -march=armv7-a [source].s -o [output].o
@   # Must assemble without errors or warnings
@
@ Link Verification:
@   arm-none-eabi-ld -T linker.ld -o [binary].elf [objects...]
@   # Must link without undefined symbol errors
@
@ Binary Verification:
@   arm-none-eabi-objdump -d [binary].elf | head
@   # Verify expected instructions at expected addresses
@
@ Testing Requirements:
@   - Verify [specific behavior] in emulator (QEMU ARM)
@   - Check [register state] after execution
@   - Confirm [memory layout] correct
@
@ Example validation commands:
@
@     # Assemble
@     arm-none-eabi-as -march=armv7-a -o [module].o [module].s
@
@     # Link
@     arm-none-eabi-ld -T linker.ld -o [binary].elf [module].o
@
@     # Create binary
@     arm-none-eabi-objcopy [binary].elf -O binary [binary].bin
@
@     # Test in emulator
@     qemu-system-arm -M [machine] -kernel [binary].elf

@ ────────────────────────────────────────────────────────────────
@ Code Execution
@ ────────────────────────────────────────────────────────────────
@ For Code Execution section explanation, see: standards/code/4-block/sections/closing/CWS-SECTION-CLOSING-002-code-execution.md
@
@ Entry Point: [label name]
@
@ Execution Flow:
@   1. [First step - e.g., set up stack]
@   2. [Second step - e.g., call C function]
@   3. [Third step - e.g., handle return]
@   4. [Final step - e.g., halt]
@
@ Register State on Entry:
@   [register]: [Expected value/state]
@
@ Register State on Exit:
@   [register]: [Value/state after execution]

@ ────────────────────────────────────────────────────────────────
@ Code Cleanup
@ ────────────────────────────────────────────────────────────────
@ For Code Cleanup section explanation, see: standards/code/4-block/sections/closing/CWS-SECTION-CLOSING-003-code-cleanup.md
@
@ Resource Management:
@   - Registers: [Which are preserved/restored]
@   - Stack: [How stack is managed]
@   - Memory: [Any allocations to free]
@
@ AAPCS Register Usage:
@   - r0-r3:   Arguments/results (caller-saved, can clobber)
@   - r4-r11:  Callee-saved (must preserve if used)
@   - r12/ip:  Intra-procedure scratch (can clobber)
@   - r13/sp:  Stack pointer (must restore)
@   - r14/lr:  Link register (must preserve for nested calls)
@   - r15/pc:  Program counter

@ ════════════════════════════════════════════════════════════════
@ FINAL DOCUMENTATION
@ ════════════════════════════════════════════════════════════════

@ ────────────────────────────────────────────────────────────────
@ Module Overview & Usage Summary
@ ────────────────────────────────────────────────────────────────
@ For Module Overview section explanation, see: standards/code/4-block/sections/closing/CWS-SECTION-CLOSING-004-library-overview.md
@
@ Purpose: See METADATA "Purpose & Function" section above
@
@ Provides: See METADATA "Key Features" list above
@
@ Quick summary:
@   - [1-2 sentence overview of what this module does]
@
@ Architecture: See METADATA "CPI-SI Identity" section above

@ ────────────────────────────────────────────────────────────────
@ Modification Policy
@ ────────────────────────────────────────────────────────────────
@ For Modification Policy section explanation, see: standards/code/4-block/sections/closing/CWS-SECTION-CLOSING-005-modification-policy.md
@
@ Safe to Modify (Extension Points):
@   [tick] Add new helper routines (follow existing patterns)
@   [tick] Add new constants (use .equ)
@   [tick] Extend data section
@
@ Modify with Extreme Care (Breaking Changes):
@   [warn] Entry point labels - breaks all callers
@   [warn] Register conventions - breaks calling code
@   [warn] Memory layout assumptions
@
@ NEVER Modify (Foundational Rails):
@   [x] 4-block structure (METADATA, SETUP, BODY, CLOSING)
@   [x] AAPCS calling convention
@   [x] Hardware interface contracts

@ ────────────────────────────────────────────────────────────────
@ Ladder and Baton Flow
@ ────────────────────────────────────────────────────────────────
@ For Ladder and Baton Flow section explanation, see: standards/code/4-block/sections/closing/CWS-SECTION-CLOSING-006-ladder-baton-flow.md
@
@ See BODY "Organizational Chart" for complete structure.
@
@ Quick summary:
@ - Ladder: [Dependencies - what this needs]
@ - Baton: [Execution flow - entry to exit]

@ ────────────────────────────────────────────────────────────────
@ Performance Considerations
@ ────────────────────────────────────────────────────────────────
@ For Performance Considerations section explanation, see: standards/code/4-block/sections/closing/CWS-SECTION-CLOSING-008-performance-considerations.md
@
@ Assembly is the performance baseline - no abstraction overhead.
@
@ ARM-specific optimizations:
@   - Use conditional execution (IT blocks in Thumb-2)
@   - Exploit barrel shifter (free shifts in ALU ops)
@   - Use NEON for SIMD operations
@   - Align data to cache line boundaries
@
@ Pipeline considerations:
@   - Avoid data hazards (register dependencies)
@   - Branch prediction (prefer conditional execution)
@   - Memory alignment (word-aligned loads fastest)

@ ────────────────────────────────────────────────────────────────
@ Troubleshooting Guide
@ ────────────────────────────────────────────────────────────────
@ For Troubleshooting Guide section explanation, see: standards/code/4-block/sections/closing/CWS-SECTION-CLOSING-009-troubleshooting-guide.md
@
@ Problem: "undefined reference to [symbol]"
@   - Cause: Symbol not exported with .global
@   - Solution: Add ".global [symbol]" before label
@
@ Problem: "Error: selected processor does not support [instruction]"
@   - Cause: Instruction not available in target architecture
@   - Solution: Use .arch directive or select appropriate -march
@
@ Problem: Data abort / Prefetch abort
@   - Cause: Unaligned access, invalid address, MMU fault
@   - Solution: Check alignment, verify address range
@
@ Problem: Undefined instruction exception
@   - Cause: Executing data, wrong instruction set mode
@   - Solution: Check .arm/.thumb directives, verify code section

@ ────────────────────────────────────────────────────────────────
@ Related Components & Dependencies
@ ────────────────────────────────────────────────────────────────
@ For Related Components section explanation, see: standards/code/4-block/sections/closing/CWS-SECTION-CLOSING-010-related-components.md
@
@ See METADATA "Dependencies" section above for complete information.
@
@ Quick summary:
@ - Dependencies: [Key external symbols needed]
@ - Dependents: [What uses this module]

@ ────────────────────────────────────────────────────────────────
@ Future Expansions & Roadmap
@ ────────────────────────────────────────────────────────────────
@ For Future Expansions section explanation, see: standards/code/4-block/sections/closing/CWS-SECTION-CLOSING-011-future-expansions.md
@
@ Planned Features:
@   [check] [Completed feature] - COMPLETED
@   [pending] [Planned feature 1]
@   [pending] [Planned feature 2]
@
@ Research Areas:
@   - AArch64 (64-bit ARM) support
@   - NEON SIMD optimization
@   - TrustZone integration

@ ────────────────────────────────────────────────────────────────
@ Closing Note
@ ────────────────────────────────────────────────────────────────
@ For Closing Note section explanation, see: standards/code/4-block/sections/closing/CWS-SECTION-CLOSING-012-closing-note.md
@
@ This module is [architectural role - what it provides].
@ [Explain its place in the ecosystem].
@
@ ARM assembly is clean and regular - RISC philosophy shows.
@ Test thoroughly in QEMU before hardware.
@
@ "[Relevant Scripture verse]" - [Reference]

@ ────────────────────────────────────────────────────────────────
@ Quick Reference: Usage Examples
@ ────────────────────────────────────────────────────────────────
@ For Quick Reference section explanation, see: standards/code/4-block/sections/closing/CWS-SECTION-CLOSING-013-quick-reference.md
@
@ Assemble (ARMv7-A):
@
@     arm-none-eabi-as -march=armv7-a [module].s -o [module].o
@
@ Assemble (Raspberry Pi):
@
@     arm-none-eabi-as -mcpu=cortex-a7 [module].s -o [module].o
@
@ Link:
@
@     arm-none-eabi-ld -T linker.ld -o kernel.elf entry.o kernel.o
@
@ Create binary:
@
@     arm-none-eabi-objcopy kernel.elf -O binary kernel.img
@
@ Call from C:
@
@     extern void [entry_point](void);
@     [entry_point]();
@
@ Run in QEMU:
@
@     qemu-system-arm -M raspi2 -kernel kernel.elf -serial stdio

@ -----------------------------------------------------------------------------
@ CLOSING Omission Guide
@ -----------------------------------------------------------------------------
@
@ ALL thirteen sections MUST be present. Content may be reserved with reason:
@
@ GROUP 1: CODING
@   - Code Validation: Assembly and link verification
@   - Code Execution: Entry point and execution flow
@   - Code Cleanup: Register preservation and stack management
@
@ GROUP 2: FINAL DOCUMENTATION (mostly back-references)
@   - Module Overview: Summary of what this module provides
@   - Modification Policy: Guide for modifying routines safely
@   - Ladder and Baton Flow: Back-reference to BODY label structure
@   - Surgical Update Points: Where to add new routines
@   - Performance Considerations: Cycle counts and timing
@   - Troubleshooting Guide: Common ARM assembly issues
@   - Related Components: Related modules and includes
@   - Future Expansions: [Reserved: Module complete, no planned routines]
@   - Contribution Guidelines: How to add new routines
@   - Quick Reference: Common ARM assembly patterns
@
@ Unlike BODY (which uses [Reserved:] inline), CLOSING sections can be
@ entirely replaced with back-references to avoid duplication.
@
@ The key principle: CLOSING synthesizes, METADATA/SETUP/BODY contain details.
@ Don't repeat - reference back to where the information lives.

@ ============================================================================
@ END CLOSING
@ ============================================================================
