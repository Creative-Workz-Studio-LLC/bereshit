@ ═══════════════════════════════════════════════════════════════════════════
@ TEMPLATE: ARM Assembly Source File (4-Block Structure)
@ Key: CODE-ARM-001
@ ═══════════════════════════════════════════════════════════════════════════
@
@ DEPENDENCY CLASSIFICATION: [PURE/DEPENDED] ([deps if DEPENDED])
@   - PURE: Self-contained - no external dependencies beyond CPU
@   - DEPENDED: Needs external symbols - list them: (needs: kernel_main, uart_init)
@
@ This is a TEMPLATE file - copy and modify for new ARM assembly source files.
@ Replace all [bracketed] placeholders with actual content.
@ Rename to appropriate name (e.g., entry.s, boot.s, vectors.s).
@
@ Assembler: GNU AS (Gas) for ARM
@ Derived from: templates/code/asm/CODE-ASM-001-ASM-source.asm
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
@ For METADATA structure explanation, see: standards/code/4-block/CWS-STD-004-CODE-metadata-block.md
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
@ # Blocking Status
@
@ [Blocking/Non-blocking]: [Brief explanation]
@
@ Mitigation: [How blocking/failures handled]
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
@ # Health Scoring
@
@ Health Impact: [None for low-level code | describe if applicable]
@
@ Note: Assembly modules typically don't track health - they're foundations.

@ ============================================================================
@ END METADATA
@ ============================================================================

@ ============================================================================
@ SETUP
@ ============================================================================
@
@ For SETUP structure explanation, see: standards/code/4-block/CWS-STD-006-CODE-setup-block.md
@
@ Section order: Directives → Constants → External Symbols → Data → BSS
@ This flows: assembler config → values → dependencies → initialized data → uninitialized data
@
@ Why this order: GNU AS processes top-to-bottom. Directives must come first,
@ constants before use, externs before call, data before reference.
@
@ IMPORTANT: All sections MUST be present, even if empty or reserved.
@ A lean section comment is better than absence. Why:
@   - Consistent structure across all files (navigation)
@   - Clear extension points (where to add when needed)
@   - Intentional vs forgotten (a reserved section is deliberate)
@
@ For empty sections, use: @ [Reserved: Brief reason why not needed]

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

@ ============================================================================
@ END SETUP
@ ============================================================================

@ ============================================================================
@ BODY
@ ============================================================================
@
@ For BODY structure explanation, see: standards/code/4-block/CWS-STD-007-CODE-body-block.md

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

@ ============================================================================
@ END BODY
@ ============================================================================

@ ============================================================================
@ CLOSING
@ ============================================================================
@
@ For CLOSING structure explanation, see: standards/code/4-block/CWS-STD-008-CODE-closing-block.md

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

@ ============================================================================
@ END CLOSING
@ ============================================================================
