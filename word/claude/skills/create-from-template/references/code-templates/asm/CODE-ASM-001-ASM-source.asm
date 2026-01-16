; ═══════════════════════════════════════════════════════════════════════════
; TEMPLATE: Assembly Source File (4-Block Structure)
; Key: CODE-ASM-001
; ═══════════════════════════════════════════════════════════════════════════
;
; DEPENDENCY CLASSIFICATION: [PURE/DEPENDED] ([deps if DEPENDED])
;   - PURE: Self-contained - no external dependencies beyond CPU
;   - DEPENDED: Needs external symbols - list them: (needs: kernel_main, vga_init)
;
; This is a TEMPLATE file - copy and modify for new assembly source files.
; Replace all [bracketed] placeholders with actual content.
; Rename to appropriate name (e.g., entry.asm, boot.asm).
;
; Assembler: NASM (Netwide Assembler)
; Derived from: templates/code/c/CODE-C-001-C-source.c
; See: standards/code/4-block/ for complete documentation
;
; Assembly Format Notes:
;   - NASM syntax (Intel style, not AT&T)
;   - Comments use semicolons (;)
;   - Labels end with colon (:)
;   - Directives use brackets for memory [addr]
;
; ═══════════════════════════════════════════════════════════════════════════

; [brief description of what this assembly file implements].
;
; [Module Name] - CPI-SI [Project/System Name]
;
; For METADATA structure explanation, see: standards/code/4-block/CWS-STD-004-CODE-metadata-block.md
;
; # Biblical Foundation
;
; Scripture: [Relevant verse grounding this module's purpose]
;
; Principle: [Kingdom principle this module demonstrates]
;
; Anchor: [Supporting verse reinforcing the principle]
;
; # CPI-SI Identity
;
; Component Type: [Ladder/Baton/Rails - see CWS-STD-004 for explanations]
;
; Role: [Specific responsibility in system architecture]
;
; Paradigm: CPI-SI framework component
;
; # Authorship & Lineage
;
;   - Architect: [Who designed the approach and requirements]
;   - Implementation: [Who wrote the code and verified it works]
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
; Purpose: [What problem does this module solve?]
;
; Core Design: [Architectural pattern or paradigm]
;
; Key Features:
;
;   - [What it provides - major capabilities]
;   - [What it enables - what others can build with this]
;   - [What problems it solves - specific use cases]
;
; Philosophy: [Guiding principle for how this module works]
;
; # Blocking Status
;
; [Blocking/Non-blocking]: [Brief explanation]
;
; Mitigation: [How blocking/failures handled]
;
; # Usage & Integration
;
; Assemble:
;
;   nasm -f [format] [source].asm -o [output].[o/bin]
;
; Formats:
;   - elf32: 32-bit ELF object (Linux, link with ld)
;   - elf64: 64-bit ELF object (Linux x64)
;   - bin:   Flat binary (bootloaders, bare metal)
;   - win32: Windows 32-bit object
;   - win64: Windows 64-bit object
;
; # Dependencies
;
; What This Needs:
;
;   - CPU Features: [list required features - e.g., protected mode, SSE]
;   - External Symbols: [None | list extern symbols]
;   - Memory Layout: [Any assumptions about memory layout]
;
; What Uses This:
;
;   - Linker: [How this gets linked]
;   - Bootloader: [If applicable]
;   - C code: [If called from C]
;
; Integration Points:
;
;   - [How other systems connect - calling convention, symbol names]
;   - [Cross-component interactions]
;   - [Memory/register state expectations]
;
; # Health Scoring
;
; Health Impact: [None for low-level code | describe if applicable]
;
; Note: Assembly modules typically don't track health - they're foundations.

; ============================================================================
; END METADATA
; ============================================================================

; ============================================================================
; SETUP
; ============================================================================
;
; For SETUP structure explanation, see: standards/code/4-block/CWS-STD-006-CODE-setup-block.md
;
; Section order: Directives → Constants → External Symbols → Data → BSS
; This flows: assembler config → values → dependencies → initialized data → uninitialized data
;
; Why this order: NASM processes top-to-bottom. Directives must come first,
; constants before use, externs before call, data before reference.
;
; IMPORTANT: All sections MUST be present, even if empty or reserved.
; A lean section comment is better than absence. Why:
;   - Consistent structure across all files (navigation)
;   - Clear extension points (where to add when needed)
;   - Intentional vs forgotten (a reserved section is deliberate)
;
; For empty sections, use: ; [Reserved: Brief reason why not needed]

; ────────────────────────────────────────────────────────────────
; Assembler Directives
; ────────────────────────────────────────────────────────────────
;
; Configuration telling NASM how to assemble this file.
; Must come before any code or data.
;
; See: standards/code/4-block/sections/setup/CWS-SECTION-SETUP-001-imports.md

;--- Mode Selection ---
; [BITS 16]          ; Real mode (bootloaders, BIOS)
; [BITS 32]          ; Protected mode (32-bit kernels)
; [BITS 64]          ; Long mode (64-bit kernels)

;--- Origin (for flat binaries) ---
; [ORG 0x7C00]       ; Bootloader origin
; [ORG 0x100000]     ; Kernel load address

;--- CPU Feature Requirements ---
; [CPU 386]          ; Minimum 386 instructions
; [CPU X64]          ; 64-bit instructions

; ────────────────────────────────────────────────────────────────
; Constants (EQU Definitions)
; ────────────────────────────────────────────────────────────────
;
; Named values that never change. Magic numbers given meaningful names.
; EQU creates compile-time constants (no memory used).
;
; See: standards/code/4-block/sections/setup/CWS-SECTION-SETUP-002-constants.md

;--- [Category Name] Constants ---
; [Brief explanation of this group and their purpose]

; ; [CONSTANT_NAME] [brief description].
; ;
; ; Set to [value] based on [reasoning].
; [CONSTANT_NAME] equ [value]
;
; ; [ANOTHER_CONSTANT] [brief description].
; [ANOTHER_CONSTANT] equ [value]

;--- Hardware Constants ---
; VGA_MEMORY      equ 0xB8000    ; VGA text mode buffer address
; VGA_WIDTH       equ 80         ; Characters per row
; VGA_HEIGHT      equ 25         ; Rows on screen

;--- Size Constants ---
; STACK_SIZE      equ 16384      ; 16KB stack
; SECTOR_SIZE     equ 512        ; Disk sector size

; ────────────────────────────────────────────────────────────────
; External Symbols
; ────────────────────────────────────────────────────────────────
;
; Symbols defined in other files that this module needs.
; EXTERN declares symbols to be resolved at link time.
; GLOBAL exports symbols for other modules to use.
;
; See: standards/code/4-block/sections/setup/CWS-SECTION-SETUP-005-type-methods.md

;--- External Dependencies ---
; Symbols this module needs from other files.

; [EXTERN symbol_name]     ; [Purpose - what this symbol provides]

;--- Exported Symbols ---
; Symbols this module provides to other files.

; [GLOBAL symbol_name]     ; [Purpose - what this symbol does]

; ────────────────────────────────────────────────────────────────
; Data Section (Initialized Data)
; ────────────────────────────────────────────────────────────────
;
; Data with initial values. Loaded into memory at runtime.
; Use sparingly - increases binary size.
;
; See: standards/code/4-block/sections/setup/CWS-SECTION-SETUP-003-variables.md

; section .data
; align [alignment]

;--- String Data ---
; ; [string_name] [brief description].
; [string_name]:    db "[string content]", 0    ; Null-terminated
; [string_name]:    db "[string content]", 10, 0 ; With newline

;--- Numeric Data ---
; ; [var_name] [brief description].
; [var_name]:       db [value]     ; Byte (8-bit)
; [var_name]:       dw [value]     ; Word (16-bit)
; [var_name]:       dd [value]     ; Double word (32-bit)
; [var_name]:       dq [value]     ; Quad word (64-bit)

; ────────────────────────────────────────────────────────────────
; BSS Section (Uninitialized Data)
; ────────────────────────────────────────────────────────────────
;
; Space reserved for runtime data. Does not increase binary size.
; Use RESB/RESW/RESD/RESQ to reserve space.
;
; See: standards/code/4-block/sections/setup/CWS-SECTION-SETUP-003-variables.md

; section .bss
; align [alignment]

;--- Buffers ---
; ; [buffer_name] [brief description].
; [buffer_name]:    resb [size]    ; Reserve [size] bytes

;--- Stack ---
; ; Stack space for this module.
; stack_bottom:
;     resb STACK_SIZE              ; Reserve stack space
; stack_top:

; ============================================================================
; END SETUP
; ============================================================================

; ============================================================================
; BODY
; ============================================================================
;
; For BODY structure explanation, see: standards/code/4-block/CWS-STD-007-CODE-body-block.md

; ────────────────────────────────────────────────────────────────
; Organizational Chart - Internal Structure
; ────────────────────────────────────────────────────────────────
; Maps bidirectional dependencies and execution flow within this module.
;
; See: standards/code/4-block/sections/body/CWS-SECTION-BODY-001-organizational-chart.md
;
; Label Structure:
;
;   Entry Points (GLOBAL)
;   ├── [entry_label] → sets up, calls [helper_label]
;   └── [another_entry] → uses [utility_label]
;
;   Helpers (internal)
;   ├── [helper_label] → pure computation
;   └── [utility_label] → hardware access
;
; Execution Flow:
;
;   External call → [entry_label]
;     ↓
;   [setup code]
;     ↓
;   [helper_label]
;     ↓
;   [cleanup/return]
;
; APUs (Available Processing Units):
; - [X] labels total
; - [X] entry points (global)
; - [X] helper routines (internal)

; ────────────────────────────────────────────────────────────────
; Text Section (Code)
; ────────────────────────────────────────────────────────────────

section .text

; ────────────────────────────────────────────────────────────────
; Helpers/Utilities - Internal Support
; ────────────────────────────────────────────────────────────────
; Foundation routines used throughout this module. Usually not exported.
;
; See: standards/code/4-block/sections/body/CWS-SECTION-BODY-002-helpers.md
;
; [Reserved: Additional helpers will emerge as module develops]

; [helper_name] [does what]
;
; What It Does:
; [Brief explanation - helpers are usually simple and focused]
;
; Parameters (registers):
;   [register]: [Purpose and expected values]
;
; Returns:
;   [register]: [What's returned]
;
; Clobbers: [registers modified]
;
; [helper_name]:
;     ; Implementation
;     ret

; ────────────────────────────────────────────────────────────────
; Core Operations - Main Logic
; ────────────────────────────────────────────────────────────────
; Module-specific functionality implementing primary purpose.
;
; See: standards/code/4-block/sections/body/CWS-SECTION-BODY-003-core-operations.md

; ────────────────────────────────────────────────────────────────
; [Category Name] - [Purpose]
; ────────────────────────────────────────────────────────────────
; What These Do:
; [High-level description of this category of operations]
;
; Calling Convention:
; [Describe register usage, stack expectations]

; [operation_name] [does what]
;
; What It Does:
; [Detailed explanation of operation purpose and behavior]
;
; Parameters (registers):
;   [register]: [Purpose and expected values]
;
; Returns:
;   [register]: [What's returned and meaning]
;
; Clobbers: [registers modified]
;
; Stack Usage: [bytes used, if any]
;
; Example usage:
;
;     mov eax, [value]
;     call [operation_name]
;     ; Result in [register]
;
; [operation_name]:
;     ; Save callee-saved registers if needed
;     push ebx
;     push esi
;     push edi
;
;     ; Implementation
;
;     ; Restore and return
;     pop edi
;     pop esi
;     pop ebx
;     ret

; ────────────────────────────────────────────────────────────────
; Error Handling
; ────────────────────────────────────────────────────────────────
; Assembly error handling is typically simple - halt or return error code.
;
; See: standards/code/4-block/sections/body/CWS-SECTION-BODY-004-error-handling.md
;
; Design Principle: [Blocking/Non-blocking]
;
; Error Handling Strategy:
;   - Invalid input: [Return error code / halt / ignore]
;   - Hardware fault: [Halt with error message / interrupt handler]
;   - Unrecoverable: [cli; hlt loop]

; halt_error halts the CPU with interrupts disabled.
;
; Used for unrecoverable errors. CPU will not continue.
;
; halt_error:
;     cli                 ; Disable interrupts
; .loop:
;     hlt                 ; Halt CPU
;     jmp .loop           ; Loop if spurious wakeup

; ────────────────────────────────────────────────────────────────
; Public Entry Points - Exported Interface
; ────────────────────────────────────────────────────────────────
; Entry points exported via GLOBAL for external callers.
;
; See: standards/code/4-block/sections/body/CWS-SECTION-BODY-005-public-apis.md

; ═══ [Category Name] ═══

; [entry_point_name] [does what at high level]
;
; What It Does:
; [Detailed explanation of complete operation]
;
; Parameters (registers):
;   [register]: [Purpose and expected values]
;
; Returns:
;   [register]: [What's returned and meaning]
;
; Clobbers: [registers modified]
;
; Calling Convention: [cdecl/stdcall/custom]
;
; Example usage (from C):
;
;     extern void [entry_point_name](void);
;     [entry_point_name]();
;
; Example usage (from assembly):
;
;     call [entry_point_name]
;
; global [entry_point_name]
; [entry_point_name]:
;     ; Setup
;
;     ; Core operation
;
;     ; Cleanup and return
;     ret

; ============================================================================
; END BODY
; ============================================================================

; ============================================================================
; CLOSING
; ============================================================================
;
; For CLOSING structure explanation, see: standards/code/4-block/CWS-STD-008-CODE-closing-block.md

; ────────────────────────────────────────────────────────────────
; Code Validation: [moduleName]
; ────────────────────────────────────────────────────────────────
; For Code Validation section explanation, see: standards/code/4-block/sections/closing/CWS-SECTION-CLOSING-001-code-validation.md
;
; Build Verification:
;   nasm -f [format] [source].asm -o [output]
;   # Must assemble without errors or warnings
;
; Link Verification (if object file):
;   ld -m elf_i386 -o [binary] [objects...]
;   # Must link without undefined symbol errors
;
; Binary Verification (if flat binary):
;   hexdump -C [binary] | head
;   # Verify expected bytes at expected offsets
;
; Testing Requirements:
;   - Verify [specific behavior] in emulator (QEMU, Bochs)
;   - Check [register state] after execution
;   - Confirm [memory layout] correct
;
; Example validation commands:
;
;     # Assemble
;     nasm -f elf32 -o [module].o [module].asm
;
;     # Link with C code
;     ld -m elf_i386 -o [binary] [module].o [other].o
;
;     # Test in emulator
;     qemu-system-i386 -kernel [binary]

; ────────────────────────────────────────────────────────────────
; Code Execution
; ────────────────────────────────────────────────────────────────
; For Code Execution section explanation, see: standards/code/4-block/sections/closing/CWS-SECTION-CLOSING-002-code-execution.md
;
; Entry Point: [label name]
;
; Execution Flow:
;   1. [First step - e.g., set up stack]
;   2. [Second step - e.g., call C function]
;   3. [Third step - e.g., handle return]
;   4. [Final step - e.g., halt]
;
; Register State on Entry:
;   [register]: [Expected value/state]
;
; Register State on Exit:
;   [register]: [Value/state after execution]

; ────────────────────────────────────────────────────────────────
; Code Cleanup
; ────────────────────────────────────────────────────────────────
; For Code Cleanup section explanation, see: standards/code/4-block/sections/closing/CWS-SECTION-CLOSING-003-code-cleanup.md
;
; Resource Management:
;   - Registers: [Which are preserved/restored]
;   - Stack: [How stack is managed]
;   - Memory: [Any allocations to free]
;
; Callee-Saved Registers (must preserve):
;   - 32-bit cdecl: EBX, ESI, EDI, EBP
;   - 64-bit SysV: RBX, RBP, R12-R15
;
; Caller-Saved Registers (can clobber):
;   - 32-bit cdecl: EAX, ECX, EDX
;   - 64-bit SysV: RAX, RCX, RDX, RSI, RDI, R8-R11

; ════════════════════════════════════════════════════════════════
; FINAL DOCUMENTATION
; ════════════════════════════════════════════════════════════════

; ────────────────────────────────────────────────────────────────
; Module Overview & Usage Summary
; ────────────────────────────────────────────────────────────────
; For Module Overview section explanation, see: standards/code/4-block/sections/closing/CWS-SECTION-CLOSING-004-library-overview.md
;
; Purpose: See METADATA "Purpose & Function" section above
;
; Provides: See METADATA "Key Features" list above
;
; Quick summary:
;   - [1-2 sentence overview of what this module does]
;
; Architecture: See METADATA "CPI-SI Identity" section above

; ────────────────────────────────────────────────────────────────
; Modification Policy
; ────────────────────────────────────────────────────────────────
; For Modification Policy section explanation, see: standards/code/4-block/sections/closing/CWS-SECTION-CLOSING-005-modification-policy.md
;
; Safe to Modify (Extension Points):
;   [tick] Add new helper routines (follow existing patterns)
;   [tick] Add new constants (use EQU)
;   [tick] Extend data section
;
; Modify with Extreme Care (Breaking Changes):
;   [warn] Entry point labels - breaks all callers
;   [warn] Register conventions - breaks calling code
;   [warn] Memory layout assumptions
;
; NEVER Modify (Foundational Rails):
;   [x] 4-block structure (METADATA, SETUP, BODY, CLOSING)
;   [x] Calling convention (cdecl/stdcall/etc)
;   [x] Hardware interface contracts

; ────────────────────────────────────────────────────────────────
; Ladder and Baton Flow
; ────────────────────────────────────────────────────────────────
; For Ladder and Baton Flow section explanation, see: standards/code/4-block/sections/closing/CWS-SECTION-CLOSING-006-ladder-baton-flow.md
;
; See BODY "Organizational Chart" for complete structure.
;
; Quick summary:
; - Ladder: [Dependencies - what this needs]
; - Baton: [Execution flow - entry to exit]

; ────────────────────────────────────────────────────────────────
; Performance Considerations
; ────────────────────────────────────────────────────────────────
; For Performance Considerations section explanation, see: standards/code/4-block/sections/closing/CWS-SECTION-CLOSING-008-performance-considerations.md
;
; Assembly is the performance baseline - no abstraction overhead.
;
; Key optimizations:
;   - [Optimization 1 - e.g., register usage over memory]
;   - [Optimization 2 - e.g., aligned memory access]
;   - [Optimization 3 - e.g., branch prediction hints]
;
; Cycle counts (approximate):
;   - [Operation]: ~[N] cycles
;   - [Operation]: ~[N] cycles

; ────────────────────────────────────────────────────────────────
; Troubleshooting Guide
; ────────────────────────────────────────────────────────────────
; For Troubleshooting Guide section explanation, see: standards/code/4-block/sections/closing/CWS-SECTION-CLOSING-009-troubleshooting-guide.md
;
; Problem: "undefined reference to [symbol]"
;   - Cause: Symbol not exported with GLOBAL
;   - Solution: Add "global [symbol]" before label
;
; Problem: "relocation truncated to fit"
;   - Cause: Address doesn't fit in instruction
;   - Solution: Use indirect addressing or different format
;
; Problem: Triple fault / immediate reboot
;   - Cause: Invalid instruction, bad memory access, stack overflow
;   - Solution: Debug with emulator, check stack setup

; ────────────────────────────────────────────────────────────────
; Related Components & Dependencies
; ────────────────────────────────────────────────────────────────
; For Related Components section explanation, see: standards/code/4-block/sections/closing/CWS-SECTION-CLOSING-010-related-components.md
;
; See METADATA "Dependencies" section above for complete information.
;
; Quick summary:
; - Dependencies: [Key external symbols needed]
; - Dependents: [What uses this module]

; ────────────────────────────────────────────────────────────────
; Future Expansions & Roadmap
; ────────────────────────────────────────────────────────────────
; For Future Expansions section explanation, see: standards/code/4-block/sections/closing/CWS-SECTION-CLOSING-011-future-expansions.md
;
; Planned Features:
;   [check] [Completed feature] - COMPLETED
;   [pending] [Planned feature 1]
;   [pending] [Planned feature 2]
;
; Research Areas:
;   - [Research direction 1]
;   - [Research direction 2]

; ────────────────────────────────────────────────────────────────
; Closing Note
; ────────────────────────────────────────────────────────────────
; For Closing Note section explanation, see: standards/code/4-block/sections/closing/CWS-SECTION-CLOSING-012-closing-note.md
;
; This module is [architectural role - what it provides].
; [Explain its place in the ecosystem].
;
; Modify thoughtfully - assembly errors are subtle and hard to debug.
; Test thoroughly in emulator before hardware.
;
; "[Relevant Scripture verse]" - [Reference]

; ────────────────────────────────────────────────────────────────
; Quick Reference: Usage Examples
; ────────────────────────────────────────────────────────────────
; For Quick Reference section explanation, see: standards/code/4-block/sections/closing/CWS-SECTION-CLOSING-013-quick-reference.md
;
; Assemble (ELF32 object):
;
;     nasm -f elf32 [module].asm -o [module].o
;
; Assemble (flat binary):
;
;     nasm -f bin [module].asm -o [module].bin
;
; Link with C code:
;
;     ld -m elf_i386 -T linker.ld -o kernel.elf entry.o kernel.o
;
; Call from C:
;
;     extern void [entry_point](void);
;     [entry_point]();

; ============================================================================
; END CLOSING
; ============================================================================
