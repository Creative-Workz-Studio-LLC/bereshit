; %error "TEMPLATE: Remove this line when ready to assemble"
; @file source.asm
; @omni template --asm-source
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
; @omni:req  key        = B-word-seed-code-asm-source
; @omni:req  from       = bereshit/word/omni/seed/B-word-omni-seed-code.omni
; @omni:req  at         = template
;
; ───────────────────────────────────────────────────────────────────────────────
; P.2 FAMILY — Classification [FAMILY]
; ───────────────────────────────────────────────────────────────────────────────
;
; @omni:inh  type       = source
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
; @omni:ins  provides   = [ASM_SOURCE_TEMPLATE]
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
; @omni:req  title      = Assembly Source File Template
; @omni:req  brief      = 4-block structure template for NASM assembly source files
;
; ═══════════════════════════════════════════════════════════════════════════════
; END PRAGMA [END]
; ═══════════════════════════════════════════════════════════════════════════════
;
; USAGE: cp source.asm dest.asm → update pragma → fill sections
;
; DEPENDENCY CLASSIFICATION: [PURE/DEPENDED] ([deps if DEPENDED])
;   - PURE: Self-contained - no external dependencies beyond CPU
;   - DEPENDED: Needs external symbols - list them: (needs: kernel_main, vga_init)
;
; Assembly Format Notes:
;   - NASM syntax (Intel style, not AT&T)
;   - Comments use semicolons (;)
;   - Labels end with colon (:)
;   - Directives use brackets for memory [addr]
;
; [brief description of what this assembly file implements].
;
; [Module Name] - CPI-SI [Project/System Name]
;
; ═══════════════════════════════════════════════════════════════════════════════
; METADATA BLOCK [METADATA]
; ═══════════════════════════════════════════════════════════════════════════════
;
; @brief Identity and context for this component.
;
; STRUCTURE: M.1-M.10 sections, grouped for readability:
;   - M.1 IDENTITY:    Core identity (file, brief, key, title, type, component, role)
;   - M.2-M.6:         State, Attribution, Location, Derivation, Classification
;   - M.7-M.10:        Intent, Grounding, Dependencies, Roadmap
;
; DOMAIN FILES: Use condensed M.2-M.6 and M.7-M.10 groupings.
; FOUNDATION FILES: May expand individual M.X sections as needed.
;
; ═══════════════════════════════════════════════════════════════════════════════

; ═══════════════════════════════════════════════════════════════════════════════
; M.1 IDENTITY — Core Identity [IDENTITY]
; ═══════════════════════════════════════════════════════════════════════════════
;
; @file      seed_component.asm
; @brief     [Component Name] — [One-line description].
;
; Key:       [PROJECT-COMPONENT-NAME]
; Title:     [Human-Readable Title]
; Type:      Source ([Bootloader|Kernel|Library|Executable])
; Component: [Ladder|Baton|Rails] — [architectural role]
; Role:      [Specific responsibility in system]

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
; Path:      [project/path/to/file.asm]

; ═══════════════════════════════════════════════════════════════════════════════
; M.5 DERIVATION — Template Lineage [DERIVATION]
; ═══════════════════════════════════════════════════════════════════════════════
;
; Derives:   bereshit/word/seed/code/asm/source.asm (template)

; ═══════════════════════════════════════════════════════════════════════════════
; M.6 CLASSIFICATION — Categorization [CLASSIFICATION]
; ═══════════════════════════════════════════════════════════════════════════════
;
; Tags:      [asm, nasm, kernel, boot, x86, x64]

; ═══════════════════════════════════════════════════════════════════════════════
; M.7 INTENT — Purpose Statement [INTENT]
; ═══════════════════════════════════════════════════════════════════════════════
;
; Purpose:   [What this enables for whom]
;
; Core Design: [Architectural pattern or paradigm]
;
; Key Features:
;   - [What it provides — major capabilities]
;   - [What it enables — what others can build with this]
;   - [What problems it solves — specific use cases]

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
; CPU Features:
;   - [list required features — e.g., protected mode, long mode, SSE]
;
; External Symbols:
;   - [None | EXTERN symbol_name — purpose]
;
; Memory Layout:
;   - [Any assumptions about memory layout]
;
; Used by:
;   - [What depends on this — linker, bootloader, C code]
;
; Assemble:
;   nasm -f [format] [source].asm -o [output].[o/bin]
;
; Formats:
;   - bin:   Flat binary (bootloaders, bare metal)
;   - elf64: 64-bit ELF object (Linux x64)
;   - elf32: 32-bit ELF object (Linux)

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
; @brief Configuration and declarations before body.
;
; 4-Block Code Structure: METADATA → SETUP → BODY → CLOSING
;
; CONTAINS:
;   - S.1 DIRECTIVES — Assembler configuration (BITS, ORG, CPU)
;   - S.2 CONSTANTS  — EQU definitions (compile-time values)
;   - S.3 EXTERNAL   — Symbol declarations (EXTERN, GLOBAL)
;   - S.4 DATA       — Initialized data section
;   - S.5 BSS        — Uninitialized data section
;   - S.6 DEBUG      — Debug infrastructure
;
; Section order: Directives → Constants → External → Data → BSS → Debug
; Flow: assembler config → values → symbols → initialized → uninitialized → debug
;
; ═══════════════════════════════════════════════════════════════════════════════

; ═══════════════════════════════════════════════════════════════════════════════
; S.1 DIRECTIVES — Assembler Configuration [DIRECTIVES]
; ═══════════════════════════════════════════════════════════════════════════════
;
; @brief Configuration telling NASM how to assemble this file.
;
; PURPOSE: Set CPU mode, origin address, and feature requirements.
;          Must come before any code or data.
;
; CONTAINS:
;   - S.1a MODE   — CPU bit mode (16/32/64)
;   - S.1b ORIGIN — Load address for flat binaries
;   - S.1c CPU    — Minimum CPU feature requirements
;
; Builds FROM: SETUP block (configuration context)
; Builds TO:   S.2 CONSTANTS (assembler now knows target mode)
;
; ═══════════════════════════════════════════════════════════════════════════════

; ───────────────────────────────────────────────────────────────────────────────
; S.1a MODE — CPU Bit Mode [MODE]
; ───────────────────────────────────────────────────────────────────────────────

; [BITS 16]          ; Real mode (bootloaders, BIOS)
; [BITS 32]          ; Protected mode (32-bit kernels)
; [BITS 64]          ; Long mode (64-bit kernels)

; ───────────────────────────────────────────────────────────────────────────────
; S.1b ORIGIN — Load Address [ORIGIN]
; ───────────────────────────────────────────────────────────────────────────────

; [ORG 0x7C00]       ; Boot sector origin
; [ORG 0x7E00]       ; Stage 0b origin (after boot sector)
; [ORG 0x100000]     ; Kernel load address (1MB)

; ───────────────────────────────────────────────────────────────────────────────
; S.1c CPU — Feature Requirements [CPU]
; ───────────────────────────────────────────────────────────────────────────────

; [CPU 386]          ; Minimum 386 instructions
; [CPU X64]          ; 64-bit long mode instructions

; ═══════════════════════════════════════════════════════════════════════════════
; S.2 CONSTANTS — EQU Definitions [CONSTANTS]
; ═══════════════════════════════════════════════════════════════════════════════
;
; @brief Named compile-time values — no memory used at runtime.
;
; PURPOSE: Define component-specific constants. Magic numbers with meaning.
;
; CONTAINS:
;   - S.2a SIZES    — Buffer and array sizes
;   - S.2b HARDWARE — Memory addresses, port numbers
;   - S.2c LIMITS   — Min, max, default values
;
; Builds FROM: S.1 DIRECTIVES (mode affects address sizes)
; Builds TO:   S.3 EXTERNAL (constants used in symbol declarations)
;
; ═══════════════════════════════════════════════════════════════════════════════

; ───────────────────────────────────────────────────────────────────────────────
; S.2a SIZES — Buffer and Array Sizes [SIZES]
; ───────────────────────────────────────────────────────────────────────────────

; STACK_SIZE      equ 16384      ; 16KB stack
; SECTOR_SIZE     equ 512        ; Disk sector size
; BUFFER_SIZE     equ 4096       ; General buffer size

; ───────────────────────────────────────────────────────────────────────────────
; S.2b HARDWARE — Memory and Port Constants [HARDWARE]
; ───────────────────────────────────────────────────────────────────────────────

; VGA_MEMORY      equ 0xB8000    ; VGA text mode buffer address
; VGA_WIDTH       equ 80         ; Characters per row
; VGA_HEIGHT      equ 25         ; Rows on screen
; COM1_PORT       equ 0x3F8      ; Serial port 1

; ───────────────────────────────────────────────────────────────────────────────
; S.2c LIMITS — Value Bounds [LIMITS]
; ───────────────────────────────────────────────────────────────────────────────

; [CONSTANT_NAME] equ [value]    ; [brief description]

; ═══════════════════════════════════════════════════════════════════════════════
; S.3 EXTERNAL — Symbol Declarations [EXTERNAL]
; ═══════════════════════════════════════════════════════════════════════════════
;
; @brief Symbols from other modules (EXTERN) and exports (GLOBAL).
;
; PURPOSE: Coordinate symbol visibility across compilation units.
;
; CONTAINS:
;   - S.3a EXTERN — Symbols this module needs from other files
;   - S.3b GLOBAL — Symbols this module provides to other files
;
; Builds FROM: S.2 CONSTANTS (constants may define symbol addresses)
; Builds TO:   S.4 DATA (external symbols may reference data)
;
; ═══════════════════════════════════════════════════════════════════════════════

; ───────────────────────────────────────────────────────────────────────────────
; S.3a EXTERN — External Dependencies [EXTERN]
; ───────────────────────────────────────────────────────────────────────────────

; extern kernel_main            ; C kernel entry point
; extern gdt_descriptor         ; GDT from linker

; ───────────────────────────────────────────────────────────────────────────────
; S.3b GLOBAL — Exported Symbols [GLOBAL]
; ───────────────────────────────────────────────────────────────────────────────

; global _start                 ; Entry point for linker
; global boot_info              ; Boot information structure

; ═══════════════════════════════════════════════════════════════════════════════
; S.4 DATA — Initialized Data [DATA]
; ═══════════════════════════════════════════════════════════════════════════════
;
; @brief Data with initial values — loaded into memory at runtime.
;
; PURPOSE: Strings, lookup tables, initial values.
;          Use sparingly — increases binary size.
;
; CONTAINS:
;   - S.4a STRINGS  — Null-terminated strings
;   - S.4b NUMERIC  — Initialized variables
;   - S.4c TABLES   — Lookup tables, arrays
;
; Builds FROM: S.3 EXTERNAL (data may be exported)
; Builds TO:   S.5 BSS (initialized before uninitialized)
;
; ═══════════════════════════════════════════════════════════════════════════════

; section .data
; align 8

; ───────────────────────────────────────────────────────────────────────────────
; S.4a STRINGS — String Data [STRINGS]
; ───────────────────────────────────────────────────────────────────────────────

; msg_hello:      db "Hello, World!", 0
; msg_error:      db "Error!", 10, 0    ; With newline

; ───────────────────────────────────────────────────────────────────────────────
; S.4b NUMERIC — Initialized Variables [NUMERIC]
; ───────────────────────────────────────────────────────────────────────────────

; boot_drive:     db 0           ; Byte (8-bit)
; sector_count:   dw 0           ; Word (16-bit)
; load_address:   dd 0           ; Double word (32-bit)
; tsc_value:      dq 0           ; Quad word (64-bit)

; ───────────────────────────────────────────────────────────────────────────────
; S.4c TABLES — Lookup Tables [TABLES]
; ───────────────────────────────────────────────────────────────────────────────

; [Reserved: Add lookup tables as component develops]

; ═══════════════════════════════════════════════════════════════════════════════
; S.5 BSS — Uninitialized Data [BSS]
; ═══════════════════════════════════════════════════════════════════════════════
;
; @brief Space reserved for runtime data — does not increase binary size.
;
; PURPOSE: Buffers, stack, heap areas.
;          Use RESB/RESW/RESD/RESQ to reserve space.
;
; CONTAINS:
;   - S.5a BUFFERS — General purpose buffers
;   - S.5b STACK   — Stack space reservation
;
; Builds FROM: S.4 DATA (BSS follows initialized data)
; Builds TO:   S.6 DEBUG (all data sections complete)
;
; ═══════════════════════════════════════════════════════════════════════════════

; section .bss
; align 16

; ───────────────────────────────────────────────────────────────────────────────
; S.5a BUFFERS — General Purpose Buffers [BUFFERS]
; ───────────────────────────────────────────────────────────────────────────────

; temp_buffer:    resb 4096      ; 4KB temporary buffer
; read_buffer:    resb 512       ; Sector read buffer

; ───────────────────────────────────────────────────────────────────────────────
; S.5b STACK — Stack Space [STACK]
; ───────────────────────────────────────────────────────────────────────────────

; stack_bottom:
;     resb STACK_SIZE            ; Reserve stack space
; stack_top:

; ═══════════════════════════════════════════════════════════════════════════════
; S.6 DEBUG — Debug Infrastructure [DEBUG]
; ═══════════════════════════════════════════════════════════════════════════════
;
; @brief Debug output strings and diagnostic symbols.
;
; PURPOSE: Development aids — excluded from release builds.
;          Use conditional assembly (%ifdef DEBUG).
;
; CONTAINS:
;   - S.6a STRINGS — Debug message strings
;   - S.6b SYMBOLS — Diagnostic entry points
;
; Builds FROM: S.5 BSS (all runtime data defined)
; Builds TO:   END SETUP (configuration complete)
;
; ═══════════════════════════════════════════════════════════════════════════════

; ───────────────────────────────────────────────────────────────────────────────
; S.6a STRINGS — Debug Message Strings [DBG_STRINGS]
; ───────────────────────────────────────────────────────────────────────────────

; %ifdef DEBUG
; section .data
; dbg_prefix:     db "[component] ", 0
; dbg_init:       db "Initializing...", 10, 0
; dbg_done:       db "Complete.", 10, 0
; %endif

; ───────────────────────────────────────────────────────────────────────────────
; S.6b SYMBOLS — Diagnostic Entry Points [DBG_SYMBOLS]
; ───────────────────────────────────────────────────────────────────────────────

; [Reserved: Add debug entry points as component develops]

; ═══════════════════════════════════════════════════════════════════════════════
; END SETUP [END]
; ═══════════════════════════════════════════════════════════════════════════════

; ═══════════════════════════════════════════════════════════════════════════════
; BODY BLOCK [BODY]
; ═══════════════════════════════════════════════════════════════════════════════
;
; @brief The actual work — implementations (assembly routines).
;
; 4-Block Code Structure: METADATA → SETUP → BODY → CLOSING
;
; SECTION ORDER (Assembly-optimized):
;   B.1 ORG CHART      — Map first, reader knows the terrain
;   B.2 CORE OPS       — WHAT it does: main execution flow
;   B.3 HELPERS        — HOW it supports: utility subroutines
;   B.4 ERROR HANDLING — Exceptional paths, failure handlers
;   B.5 PUBLIC APIS    — Exported symbols (GLOBAL declarations)
;
; WHY THIS ORDER:
;   Assembly doesn't require forward declarations like C. NASM resolves forward
;   references automatically. So we organize for READING ORDER: show WHAT the
;   code does (CORE OPS) before HOW the utilities work (HELPERS).
;
; CONTAINS:
;   - B.1 ORG CHART      — Label hierarchy, baton flow, statistics
;   - B.2 CORE OPS       — Main execution flow, business logic
;   - B.3 HELPERS        — Utility subroutines (pure, hardware, mode-specific)
;   - B.4 ERROR HANDLING — Error handlers, halt loops, failure paths
;   - B.5 PUBLIC APIS    — Exported entry points (GLOBAL)
;
; ═══════════════════════════════════════════════════════════════════════════════

; ═══════════════════════════════════════════════════════════════════════════════
; B.1 ORG CHART — Label Hierarchy [ORGCHART]
; ═══════════════════════════════════════════════════════════════════════════════
;
; @brief Map internal structure — label hierarchy, baton flow, statistics.
;
; WHY: Understanding the file structure before diving into details.
;      Org chart provides the mental model for navigation.
;
; CONTAINS:
;   - B.1a STRUCTURE — Labels grouped by section (CORE OPS, HELPERS, ERRORS)
;   - B.1b FLOW      — Baton execution path diagram
;   - B.1c COUNTS    — Statistics (labels per section)
;
; ═══════════════════════════════════════════════════════════════════════════════

; ───────────────────────────────────────────────────────────────────────────────
; B.1a STRUCTURE — Label Hierarchy by Section [STRUCTURE]
; ───────────────────────────────────────────────────────────────────────────────
;
; ┌─────────────────────────────────────────────────────────────────────────────┐
; │ B.2 CORE OPS — Main Execution Flow (WHAT it does)                           │
; └─────────────────────────────────────────────────────────────────────────────┘
;
; B.2a [Category] — [Purpose]
; ├── [core_op_label]   → [what it does]
; └── [another_op]      → [what it does]
;
; ┌─────────────────────────────────────────────────────────────────────────────┐
; │ B.3 HELPERS — Utility Subroutines (HOW it supports)                         │
; └─────────────────────────────────────────────────────────────────────────────┘
;
; B.3a PURE — Pure Computation
; ├── [helper_label]    → [pure computation]
; └── [utility_label]   → [stateless utility]
;
; B.3b HARDWARE — Hardware Access
; └── [hw_helper]       → [hardware I/O]
;
; ┌─────────────────────────────────────────────────────────────────────────────┐
; │ B.4 ERROR HANDLING — Failure Paths                                          │
; └─────────────────────────────────────────────────────────────────────────────┘
;
; B.4a HANDLERS — Error Handlers
; ├── [error_handler]   → [recovery/cleanup]
; └── halt_error        → [cli; hlt loop]
;
; ┌─────────────────────────────────────────────────────────────────────────────┐
; │ B.5 PUBLIC APIS — Exported Symbols                                          │
; └─────────────────────────────────────────────────────────────────────────────┘
;
; B.5a [Category] — Exported Entry Points
; ├── [entry_label]     → GLOBAL, called externally
; └── [another_entry]   → GLOBAL, public API

; ───────────────────────────────────────────────────────────────────────────────
; B.1b FLOW — Baton Execution Path [FLOW]
; ───────────────────────────────────────────────────────────────────────────────
;
; External call → [B.5: entry_label]
;   ↓
; [setup code]
;   ↓
; [B.2: core_op_label] → [B.3: helper_label]
;   ↓                        ↓ on error
; [cleanup/return]      [B.4: error_handler]

; ───────────────────────────────────────────────────────────────────────────────
; B.1c COUNTS — Label Statistics by Section [COUNTS]
; ───────────────────────────────────────────────────────────────────────────────
;
; ┌───────────────────────────────────────────────────────────────────────────┐
; │ Section          │ Count │ Labels                                        │
; ├───────────────────────────────────────────────────────────────────────────┤
; │ B.2 CORE OPS     │  [X]  │ [list main operation labels]                  │
; │ B.3 HELPERS      │  [X]  │ [list helper labels]                          │
; │ B.4 ERROR HANDLING│ [X]  │ [list error handler labels]                   │
; │ B.5 PUBLIC APIS  │  [X]  │ [list exported labels]                        │
; └───────────────────────────────────────────────────────────────────────────┘
;
; TOTAL: [X] primary labels

; ═══════════════════════════════════════════════════════════════════════════════
; B.2 CORE OPS — Main Execution Flow [COREOPS]
; ═══════════════════════════════════════════════════════════════════════════════
;
; @brief WHAT the module does — main execution flow and business logic.
;
; PURPOSE: The actual work this module does.
;          Organized by concern/category.
;          Reader sees WHAT before HOW (helpers).
;
; CONTAINS:
;   - B.2a [Category1] — [description]
;   - B.2b [Category2] — [description]
;
; Builds FROM: B.1 ORG CHART (structure understanding)
; Builds TO:   B.3 HELPERS (utilities called by core ops)
;
; ═══════════════════════════════════════════════════════════════════════════════

section .text

; ───────────────────────────────────────────────────────────────────────────────
; B.2a [Category Name] — [Purpose] [CATEGORY1]
; ───────────────────────────────────────────────────────────────────────────────
;
; What These Do:
;   [High-level description of this category of operations]
;
; Calling Convention:
;   [Describe register usage, stack expectations]

; [operation_name] [does what]
;
; @brief  [Detailed explanation of operation purpose and behavior]
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

; ═══════════════════════════════════════════════════════════════════════════════
; B.3 HELPERS — Utility Subroutines [HELPERS]
; ═══════════════════════════════════════════════════════════════════════════════
;
; @brief HOW it supports — utility subroutines called by core ops.
;
; PURPOSE: Pure computations and utility functions.
;          Usually not exported (internal to module).
;          Reader sees these AFTER understanding WHAT the code does.
;
; CONTAINS:
;   - B.3a PURE      — Pure computation routines
;   - B.3b HARDWARE  — Hardware access helpers
;
; Builds FROM: B.2 CORE OPS (called by main execution)
; Builds TO:   B.4 ERROR HANDLING (helpers may need error paths)
;
; ═══════════════════════════════════════════════════════════════════════════════

; ───────────────────────────────────────────────────────────────────────────────
; B.3a PURE — Pure Computation Routines [PURE]
; ───────────────────────────────────────────────────────────────────────────────

; [helper_name] [does what]
;
; @brief  [Brief explanation — helpers are usually simple and focused]
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

; ───────────────────────────────────────────────────────────────────────────────
; B.3b HARDWARE — Hardware Access Helpers [HARDWARE]
; ───────────────────────────────────────────────────────────────────────────────

; [Reserved: Add hardware access helpers as component develops]

; ═══════════════════════════════════════════════════════════════════════════════
; B.4 ERROR HANDLING — Failure Paths [ERRORS]
; ═══════════════════════════════════════════════════════════════════════════════
;
; @brief Error responses — halt, return codes, exception handling.
;
; PURPOSE: Handle failure cases gracefully or halt safely.
;          Assembly error handling is typically simple.
;          Separated from happy path for clarity.
;
; CONTAINS:
;   - B.4a STRATEGY — Error handling approach
;   - B.4b HANDLERS — Error handler routines
;
; Builds FROM: B.3 HELPERS (errors may arise from utilities)
; Builds TO:   B.5 PUBLIC APIS (public API uses error handling)
;
; ═══════════════════════════════════════════════════════════════════════════════

; ───────────────────────────────────────────────────────────────────────────────
; B.4a STRATEGY — Error Handling Approach [STRATEGY]
; ───────────────────────────────────────────────────────────────────────────────
;
; Design Principle: [Blocking/Non-blocking]
;
; Error Handling Strategy:
;   - Invalid input: [Return error code / halt / ignore]
;   - Hardware fault: [Halt with error message / interrupt handler]
;   - Unrecoverable: [cli; hlt loop]

; ───────────────────────────────────────────────────────────────────────────────
; B.4b HANDLERS — Error Handler Routines [HANDLERS]
; ───────────────────────────────────────────────────────────────────────────────

; halt_error halts the CPU with interrupts disabled.
;
; @brief  Used for unrecoverable errors. CPU will not continue.
;
; halt_error:
;     cli                 ; Disable interrupts
; .loop:
;     hlt                 ; Halt CPU
;     jmp .loop           ; Loop if spurious wakeup

; ═══════════════════════════════════════════════════════════════════════════════
; B.5 PUBLIC APIS — Exported Entry Points [PUBLICAPI]
; ═══════════════════════════════════════════════════════════════════════════════
;
; @brief Entry points exported via GLOBAL for external callers.
;
; PURPOSE: The public interface — what other modules call.
;          These orchestrate core ops and helpers.
;          For flat binaries (bootloaders), this section may be empty.
;
; CONTAINS:
;   - B.5a [Category1] — [description]
;   - B.5b [Category2] — [description]
;
; Builds FROM: B.4 ERROR HANDLING (error handling integrated)
; Builds TO:   END BODY (implementation complete)
;
; ═══════════════════════════════════════════════════════════════════════════════

; ───────────────────────────────────────────────────────────────────────────────
; B.5a [Category Name] — [Purpose] [ENTRY1]
; ───────────────────────────────────────────────────────────────────────────────

; [entry_point_name] [does what at high level]
;
; @brief  [Detailed explanation of complete operation]
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
;     extern void [entry_point_name](void);
;     [entry_point_name]();
;
; Example usage (from assembly):
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
;   - X.1 VALIDATION — Build and test verification
;   - X.2 EXECUTION  — Entry point and execution flow
;   - X.3 CLEANUP    — Register and resource management
;
; GROUP 2 — DOCUMENTATION (Synthesis and References):
;   - X.4 OVERVIEW    — Module summary (references METADATA)
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
; X.1 VALIDATION — Build & Test Verification [VALIDATION]
; ═══════════════════════════════════════════════════════════════════════════════
;
; @brief Prove correctness — assemble, link, test.
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

; ═══════════════════════════════════════════════════════════════════════════════
; X.2 EXECUTION — Entry Point & Flow [EXECUTION]
; ═══════════════════════════════════════════════════════════════════════════════
;
; @brief Entry point(s) and execution orchestration.
;
; Entry Point: [label name]
;
; Execution Flow:
;   1. [First step — e.g., set up stack]
;   2. [Second step — e.g., call C function]
;   3. [Third step — e.g., handle return]
;   4. [Final step — e.g., halt]
;
; Register State on Entry:
;   [register]: [Expected value/state]
;
; Register State on Exit:
;   [register]: [Value/state after execution]

; ═══════════════════════════════════════════════════════════════════════════════
; X.3 CLEANUP — Register & Resource Management [CLEANUP]
; ═══════════════════════════════════════════════════════════════════════════════
;
; @brief Register preservation, stack cleanup, resource release.
;
; Resource Management:
;   - Registers: [Which are preserved/restored]
;   - Stack: [How stack is managed]
;   - Memory: [Any allocations to free]
;
; Callee-Saved Registers (must preserve):
;   - 32-bit cdecl: EBX, ESI, EDI, EBP
;   - 64-bit SysV:  RBX, RBP, R12-R15
;
; Caller-Saved Registers (can clobber):
;   - 32-bit cdecl: EAX, ECX, EDX
;   - 64-bit SysV:  RAX, RCX, RDX, RSI, RDI, R8-R11

; ═══════════════════════════════════════════════════════════════════════════════
; X.4 OVERVIEW — Module Summary [OVERVIEW]
; ═══════════════════════════════════════════════════════════════════════════════
;
; @brief High-level summary — references METADATA for details.
;
; Purpose:  See METADATA M.7 INTENT section above
; Provides: See METADATA M.7 INTENT "Key Features" list above
;
; Quick summary:
;   [1-2 sentence overview of what this module does]
;
; Architecture: See METADATA M.1 IDENTITY "Component" field above

; ═══════════════════════════════════════════════════════════════════════════════
; X.5 POLICY — Modification Guidelines [POLICY]
; ═══════════════════════════════════════════════════════════════════════════════
;
; @brief Guide future maintainers on what's safe to change.
;
; Safe to Modify (Extension Points):
;   ✅ Add new helper routines (follow existing patterns)
;   ✅ Add new constants (use EQU)
;   ✅ Extend data section
;
; Modify with Extreme Care (Breaking Changes):
;   ⚠️ Entry point labels — breaks all callers
;   ⚠️ Register conventions — breaks calling code
;   ⚠️ Memory layout assumptions
;
; NEVER Modify (Foundational Rails):
;   ❌ 4-block structure (METADATA, SETUP, BODY, CLOSING)
;   ❌ Calling convention (cdecl/stdcall/SysV)
;   ❌ Hardware interface contracts

; ═══════════════════════════════════════════════════════════════════════════════
; X.6 FLOW — Ladder & Baton [FLOW]
; ═══════════════════════════════════════════════════════════════════════════════
;
; @brief Execution flow — references BODY B.1 ORG CHART for details.
;
; See BODY B.1 ORG CHART for complete structure.
;
; Quick summary:
;   Ladder: [Dependencies — what this needs]
;   Baton:  [Execution flow — entry to exit]

; ═══════════════════════════════════════════════════════════════════════════════
; X.7 PERF — Performance Considerations [PERF]
; ═══════════════════════════════════════════════════════════════════════════════
;
; @brief Instruction timing, pipeline, cache behavior.
;
; Assembly is the performance baseline — no abstraction overhead.
;
; Key optimizations:
;   - [Optimization 1 — e.g., register usage over memory]
;   - [Optimization 2 — e.g., aligned memory access]
;   - [Optimization 3 — e.g., branch prediction hints]
;
; Cycle counts (approximate):
;   - [Operation]: ~[N] cycles
;   - [Operation]: ~[N] cycles

; ═══════════════════════════════════════════════════════════════════════════════
; X.8 DEBUG — Troubleshooting Guide [DEBUG]
; ═══════════════════════════════════════════════════════════════════════════════
;
; @brief Common assembly issues and debugging techniques.
;
; Problem: "undefined reference to [symbol]"
;   Cause:    Symbol not exported with GLOBAL
;   Solution: Add "global [symbol]" before label
;
; Problem: "relocation truncated to fit"
;   Cause:    Address doesn't fit in instruction
;   Solution: Use indirect addressing or different format
;
; Problem: Triple fault / immediate reboot
;   Cause:    Invalid instruction, bad memory access, stack overflow
;   Solution: Debug with emulator, check stack setup

; ═══════════════════════════════════════════════════════════════════════════════
; X.9 RELATED — Related Components [RELATED]
; ═══════════════════════════════════════════════════════════════════════════════
;
; @brief Related modules and dependencies — references METADATA M.9.
;
; See METADATA M.9 DEPENDENCIES section above for complete information.
;
; Quick summary:
;   Dependencies: [Key external symbols needed]
;   Dependents:   [What uses this module]

; ═══════════════════════════════════════════════════════════════════════════════
; X.10 ROADMAP — Future Expansions [ROADMAP]
; ═══════════════════════════════════════════════════════════════════════════════
;
; @brief Planned routines, optimizations, platform support.
;
; Planned Features:
;   ✓ [Completed feature] — COMPLETED
;   ⏳ [Planned feature 1]
;   ⏳ [Planned feature 2]
;
; Research Areas:
;   - [Research direction 1]
;   - [Research direction 2]

; ═══════════════════════════════════════════════════════════════════════════════
; X.11 REFERENCE — Quick Reference [REFERENCE]
; ═══════════════════════════════════════════════════════════════════════════════
;
; @brief Copy-paste ready patterns for common operations.
;
; Assemble (ELF64 object):
;     nasm -f elf64 [module].asm -o [module].o
;
; Assemble (flat binary):
;     nasm -f bin [module].asm -o [module].bin
;
; Link with C code:
;     ld -m elf_x86_64 -T linker.ld -o kernel.elf entry.o kernel.o
;
; Call from C:
;     extern void [entry_point](void);
;     [entry_point]();
;
; Closing Note:
;   This module is [architectural role — what it provides].
;   [Explain its place in the ecosystem].
;
;   Modify thoughtfully — assembly errors are subtle and hard to debug.
;   Test thoroughly in emulator before hardware.
;
;   "[Relevant Scripture verse]" — [Reference]

; ═══════════════════════════════════════════════════════════════════════════════
; END CLOSING [END]
; ═══════════════════════════════════════════════════════════════════════════════
