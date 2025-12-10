; %error "TEMPLATE: Remove this line when ready to assemble"
; ═══════════════════════════════════════════════════════════════════════════
; TEMPLATE: Assembly Source File (4-Block Structure)
; Key: B-word-seed-code-asm-source
; ═══════════════════════════════════════════════════════════════════════════
;
; DEPENDENCY CLASSIFICATION: [PURE/DEPENDED] ([deps if DEPENDED])
;   - PURE: Self-contained - no external dependencies beyond CPU
;   - DEPENDED: Needs external symbols - list them: (needs: kernel_main, vga_init)
;
; This is a TEMPLATE file - copy and modify for new assembly source files.
; Replace all [bracketed] placeholders with actual content.
; Rename to appropriate name (e.g., entry.asm, boot.asm).
; Remove the "%error" line above when ready to assemble.
;
; Assembler: NASM (Netwide Assembler)
; derives_from: bereshit/word/omni/seed/code.omni
; Derived from: Kingdom Technology 4-block code structure
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
; ────────────────────────────────────────────────────────────────
; INTERFACE (Expected)
; ────────────────────────────────────────────────────────────────
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
; ────────────────────────────────────────────────────────────────
; OPERATIONAL (Contextual)
; ────────────────────────────────────────────────────────────────
;
; # Blocking Status
;
; [Blocking/Non-blocking]: [Brief explanation]
;
; Mitigation: [How blocking/failures handled]
;
; # Health Scoring
;
; [OMIT: Assembly modules are foundations - health tracked at higher levels]
;
; Note: Assembly modules typically don't track health - they're foundations.
; Health scoring occurs in the C/Go code that calls these routines.
;
; ────────────────────────────────────────────────────────────────
; METADATA Omission Guide
; ────────────────────────────────────────────────────────────────
;
; Tier 1 (CORE IDENTITY): Never omit - every file needs these.
;
; Tier 2 (INTERFACE): May omit with [OMIT: reason] notation.
;   - Dependencies: Required for ASM - documents CPU features and extern symbols
;   - Usage & Integration: Required - shows assemble commands and formats
;
; Tier 3 (OPERATIONAL): Include when applicable to file type.
;   - Blocking Status: Include for boot code, interrupt handlers
;   - Health Scoring: [OMIT: Assembly is foundational - health at higher levels]
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
; Section order: Directives → Constants → External Symbols → Data → BSS → Debug Infrastructure
; This flows: assembler config → values → dependencies → initialized → uninitialized → infrastructure
;
; IMPORTANT: All sections MUST be present, even if empty or reserved.
; For empty sections, use: ; [Reserved: Brief reason why not needed]
;
; -----------------------------------------------------------------------------
; SETUP Sections Overview
; -----------------------------------------------------------------------------
;
; 1. ASSEMBLER DIRECTIVES (Dependencies)
;    Purpose: Configuration telling NASM how to assemble this file
;    Subsections: Mode Selection → Origin → CPU Features
;
; 2. CONSTANTS (EQU Definitions)
;    Purpose: Fixed values that never change (compile-time, no memory used)
;    Subsections: Category Constants → Hardware Constants → Size Constants
;
; 3. EXTERNAL SYMBOLS (Type Behaviors)
;    Purpose: Symbols defined elsewhere or exported to other modules
;    Subsections: External Dependencies (EXTERN) → Exported Symbols (GLOBAL)
;
; 4. DATA SECTION (Variables - Initialized)
;    Purpose: Data with initial values, loaded into memory at runtime
;    Subsections: String Data → Numeric Data → Structured Data
;
; 5. BSS SECTION (Variables - Uninitialized)
;    Purpose: Space reserved for runtime data, does not increase binary size
;    Subsections: Buffers → Stack → Heap Area
;
; 6. DEBUG INFRASTRUCTURE (Rails Pattern)
;    Purpose: Debug output, logging markers, diagnostic symbols
;    Subsections: Debug Strings → Diagnostic Symbols

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

; ────────────────────────────────────────────────────────────────
; Debug Infrastructure (Rails Pattern)
; ────────────────────────────────────────────────────────────────
;
; Debug output strings and diagnostic symbols for development.
; Use conditional assembly to exclude from release builds.
;
; See: standards/code/patterns/CWS-PATTERN-003-CODE-rails.md

;--- Debug Strings ---
; Debug messages for development (exclude in release builds).

; %ifdef DEBUG
; section .data
; dbg_prefix:     db "[component] ", 0
; dbg_init:       db "Initializing...", 10, 0
; dbg_done:       db "Done.", 10, 0
; %endif

;--- Diagnostic Symbols ---
; Symbols for debugger breakpoints and diagnostic tools.

; [Reserved: Add debug entry points as component develops]

; -----------------------------------------------------------------------------
; SETUP Omission Guide
; -----------------------------------------------------------------------------
;
; ALL sections MUST be present. Content may be reserved with reason:
;
;   - Assembler Directives: Rarely reserved - most files need BITS/ORG
;   - Constants: [Reserved: No fixed values needed]
;   - External Symbols: [Reserved: Self-contained, no external dependencies]
;   - Data Section: [Reserved: No initialized data needed]
;   - BSS Section: [Reserved: No runtime buffers needed]
;   - Debug Infrastructure: [Reserved: Minimal component - no debug output]
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
; -----------------------------------------------------------------------------
; BODY Sections Overview
; -----------------------------------------------------------------------------
;
; 1. ORGANIZATIONAL CHART (Internal Structure)
;    Purpose: Map label dependencies and execution flow within this module
;    Subsections: Label Structure → Execution Flow → APUs
;
; 2. HELPERS/UTILITIES (Internal Support)
;    Purpose: Foundation routines - simple, focused, reusable
;    Subsections: Pure Computations → Hardware Access Helpers
;
; 3. CORE OPERATIONS (Business Logic)
;    Purpose: Module-specific functionality implementing primary purpose
;    Subsections: [Category 1] → [Category 2] → ... (organized by concern)
;
; 4. ERROR HANDLING (Safety Patterns)
;    Purpose: Error responses - halt, return codes, exception handling
;    Subsections: Design Principle → Error Strategy → halt_error
;
; 5. PUBLIC ENTRY POINTS (Exported Interface)
;    Purpose: Labels exported via GLOBAL for external callers
;    Subsections: [Category 1] → [Category 2] → ... (organized by purpose)
;
; Section order: Org Chart → Helpers → Core Operations → Error Handling → Entry Points
; This flows: understand structure → build foundations → implement logic → handle errors → expose interface
;
; Universal mapping (see standards for cross-language patterns):
;   Organizational Chart ≈ Dependency/Flow Documentation
;   Helpers/Utilities ≈ Internal Routines (not exported)
;   Core Operations ≈ Business Logic (the work)
;   Error Handling ≈ halt_error, return codes
;   Public Entry Points ≈ GLOBAL labels (what others call)

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

; -----------------------------------------------------------------------------
; BODY Omission Guide
; -----------------------------------------------------------------------------
;
; ALL five sections MUST be present. Content may be reserved with reason:
;
;   - Organizational Chart: Rarely reserved - label structure benefits from map
;   - Helpers/Utilities: [Reserved: No internal subroutines - uses library calls]
;   - Core Operations: Rarely reserved - contains primary instruction sequences
;   - Error Handling: [Reserved: Uses CPU exceptions, no custom error handlers]
;   - Public Entry Points: [Reserved: Internal module - no GLOBAL exports]
;
; Unlike METADATA (sections omitted entirely with [OMIT:]), BODY preserves
; all section headers with [Reserved:] notation for unused sections.
;
; For multi-file assembly projects:
;   - Entry file: Contains _start/main, GLOBAL exports, orchestration
;   - Module files: Contains internal routines, EXTERN imports
;   - Document linkage with [Reserved: Calls helpers from utils.asm via EXTERN]

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
; GROUP 1: CODING (Operations - Verify, Execute, Clean)
;
; 1. CODE VALIDATION (Testing & Verification)
;    Purpose: Prove correctness - assemble, link, test
;    Subsections: Assembly Verification → Link Testing → Runtime Testing
;
; 2. CODE EXECUTION (Entry Points & Flow)
;    Purpose: Entry point(s) and execution orchestration
;    Subsections: Entry Point → Execution Flow → Exit Codes → Exception Handling
;
; 3. CODE CLEANUP (Resource Management)
;    Purpose: Register preservation, stack cleanup, resource release
;    Subsections: Register Preservation → Stack Management → Resource Cleanup
;
; GROUP 2: FINAL DOCUMENTATION (Synthesis - Reference Back to Earlier Blocks)
;
; 4. MODULE OVERVIEW (Summary with Back-References)
;    Purpose: High-level summary pointing back to METADATA for details
;    References: METADATA "Purpose & Function", "Key Features"
;
; 5. MODIFICATION POLICY (Safe/Careful/Never)
;    Purpose: Guide future maintainers on what's safe to change
;    Subsections: Safe to Modify → Modify with Care → Never Modify
;
; 6. LADDER AND BATON FLOW (Back-Reference to BODY)
;    Purpose: Point to BODY Organizational Chart for label structure
;    References: BODY "Organizational Chart - Internal Structure"
;
; 7. SURGICAL UPDATE POINTS (Back-Reference to BODY)
;    Purpose: Point to BODY for adding new routines
;    References: BODY section categories for routine placement
;
; 8. PERFORMANCE CONSIDERATIONS (Cycle Counts)
;    Purpose: Instruction timing, pipeline considerations, cache behavior
;    References: BODY routine comments with cycle counts
;
; 9. TROUBLESHOOTING GUIDE (Debug Patterns)
;    Purpose: Common assembly issues and debugging techniques
;    Subsections: Register Corruption → Stack Misalignment → Memory Access
;
; 10. RELATED COMPONENTS (Module Dependencies)
;     Purpose: Point to related assembly modules and includes
;     References: METADATA "Dependencies" - EXTERN symbols, includes
;
; 11. FUTURE EXPANSIONS (Module Roadmap)
;     Purpose: Planned routines, optimizations, platform support
;     Subsections: Planned Routines → Optimizations → Platform Support
;
; 12. CONTRIBUTION GUIDELINES (Adding Routines)
;     Purpose: How to add new routines to this module
;     Subsections: Routine Structure → Register Convention → Documentation
;
; 13. QUICK REFERENCE (Assembly Patterns)
;     Purpose: Copy-paste ready patterns for common operations
;     Subsections: Function Prologue/Epilogue → System Calls → Memory Operations
;
; Section order: Validation → Execution → Cleanup → Overview → Policy → Ladder/Baton →
;                Surgical → Performance → Troubleshooting → Related → Future → Contribution → Reference
; This flows: verify → run → clean → document → guide future work

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

; -----------------------------------------------------------------------------
; CLOSING Omission Guide
; -----------------------------------------------------------------------------
;
; ALL thirteen sections MUST be present. Content may be reserved with reason:
;
; GROUP 1: CODING
;   - Code Validation: Assembly and link verification
;   - Code Execution: Entry point and execution flow
;   - Code Cleanup: Register preservation and stack management
;
; GROUP 2: FINAL DOCUMENTATION (mostly back-references)
;   - Module Overview: Summary of what this module provides
;   - Modification Policy: Guide for modifying routines safely
;   - Ladder and Baton Flow: Back-reference to BODY label structure
;   - Surgical Update Points: Where to add new routines
;   - Performance Considerations: Cycle counts and timing
;   - Troubleshooting Guide: Common assembly issues
;   - Related Components: Related modules and includes
;   - Future Expansions: [Reserved: Module complete, no planned routines]
;   - Contribution Guidelines: How to add new routines
;   - Quick Reference: Common assembly patterns
;
; Unlike BODY (which uses [Reserved:] inline), CLOSING sections can be
; entirely replaced with back-references to avoid duplication.
;
; The key principle: CLOSING synthesizes, METADATA/SETUP/BODY contain details.
; Don't repeat - reference back to where the information lives.

; ============================================================================
; END CLOSING
; ============================================================================
