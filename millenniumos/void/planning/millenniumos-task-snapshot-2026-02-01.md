# MillenniumOS Task Snapshot

> *"A just man falleth seven times, and riseth up again."* — Proverbs 24:16

**Snapshot Date:** 2026-02-01
**Purpose:** Preserve all task progress and current session findings before new session

---

## Session Summary (2026-02-01)

### Work Completed This Session

1. **Serial Port Investigation** — Researched Hangul character (`꾯` = 0xEA 0xAA 0xAF) issue in serial output
2. **Root Cause Found** — Serial port routing mismatch between components:
   - UEFI bootloader used COM2 (0x2F8)
   - entry.asm used COM1 (0x3F8)
   - kernel.c used COM1 (0x3F8)
   - QEMU routed COM1 → ovmf.log, COM2 → debug.log (wrong!)

3. **Fixes Applied:**
   - Swapped QEMU serial routing in all 3 scripts (run-uefi.sh, run-uefi-simple.sh, run-uefi-debug.sh)
   - Changed UEFI bootloader to use COM1 (0x3F8) for consistency
   - All components now output to COM1 → debug.log

4. **Files Modified:**
   - `scripts/qemu/uefi/run-uefi.sh` — Serial routing fix
   - `scripts/qemu/uefi/run-uefi-simple.sh` — Serial routing fix
   - `scripts/qemu/uefi/run-uefi-debug.sh` — Serial routing fix
   - `boot/uefi/uefi_boot.c` — Changed DEBUG_PORT from 0x2F8 to 0x3F8

### Current Issue (Needs Next Session)

- OVMF not directly booting our bootloader (falls back to EFI Shell)
- startup.nsh exists and should auto-execute
- Need to verify ESP image contents and boot order

---

## Pending Tasks (11 total)

### High Priority — DAR Witness System

| ID | Task | Notes |
|----|------|-------|
| #112 | Expand DAR Witness API (kernel M.10) | Core DAR functionality |
| #92 | Persistent log writer service | For VFS-based logging |

### High Priority — Boot & Kernel

| ID | Task | Notes |
|----|------|-------|
| #72 | Document MillenniumOS boot architecture | Architecture docs |
| #81 | Document subsystem initialization order and dependencies | Critical for debugging |
| #119 | Add UEFI Runtime Services stub (kernel M.10) | Kernel feature |
| #77 | Remove dead VGA text mode code from kernel | Cleanup |
| #67 | Integrate libtrit into boot→kernel handoff | Ternary types |

### Medium Priority — Code Alignment

| ID | Task | Notes |
|----|------|-------|
| #117 | Align core/memory/memory.c to 4-block template | Template compliance |
| #118 | Align hal/hal.c to 4-block template | Template compliance |

### Long-term — OmniCode Compiler

| ID | Task | Notes |
|----|------|-------|
| #62 | Stage 1: Write OmniCode compiler IN OmniCode | Blocks #63 |
| #63 | Stage 2: Compile OmniCode compiler with Stage 1 | Blocked by #62, blocks #64 |
| #64 | Stage 3: Triple-verify self-hosting | Blocked by #63 |

---

## Completed Tasks (39 total)

### Boot System (12 completed)
- #66 Design UEFI bootloader for MillenniumOS
- #68 Create UEFI disk image build system
- #69 Redesign MosBootInfo for 64-bit and ternary semantics
- #70 Set up QEMU + OVMF for UEFI boot testing
- #71 Implement GOP framebuffer in UEFI bootloader
- #73 Design unified boot architecture (UEFI primary + BIOS compat)
- #74 Update BIOS boot path for unified boot info
- #75 Unify BOOT_INFO_ADDR constant across all files
- #78 Add boot info validation in entry.asm before jumping to kernel
- #105 Verify kernel.bin is correctly placed in ESP image
- #106 Unify QEMU run scripts for EDK2 compatibility
- #107 Fix mtools FAT partition creation for OVMF compatibility

### DAR Witness System (8 completed)
- #83 Implement boot_tsc and entry_tsc timestamp fields
- #84 Create unified debug/logging system with configurable levels
- #86 Wire DAR Witness logging throughout kernel subsystems
- #87 Implement complete DAR Witness system from Stage 0a to kernel
- #88 Stage 0b: Implement boot event buffer for all CPU modes
- #89 Kernel: Import boot event buffers into witness
- #90 Early kernel: Create pre-CPISI event buffer
- #91 Stage 0a: Add minimal witness markers

### Kernel (7 completed)
- #65 Update kernel graphics for 64-bit framebuffer
- #76 Implement stub subsystems in kernel (memory, interrupt, timer, scheduler)
- #79 Add compile-time struct size assertions
- #80 Add comprehensive docstrings to boot chain files
- #85 Verify framebuffer color format handling
- #109 Investigate EDK2 LoadedImage protocol failure
- #110 Add detailed UEFI error logging with status codes

### Shell/Terminal (4 completed)
- #111 Wire shell into kernel main loop
- #113 Add HAL keyboard driver for shell input
- #114 Implement terminal shell core (shell.c, parser.c, dispatch.c)
- #115 Design terminal shell architecture (shell/terminal/)
- #116 Implement terminal shell built-in commands

### Template Alignment (9 completed)
- #82 Extract serial port constants to shared header
- #93 Align stage0b.asm to template
- #94 Create INC template for assembly includes
- #95 Create/update ASM template to match C template formatting
- #96 Organize stage0 folder structure
- #97 Align stage0a.asm to template
- #98 Align bootinfo.inc to INC template
- #99 Name anonymous enums in bootinfo.h properly
- #100 Align serial.h BODY with template B.2-B.5
- #101 Align debug.h with full OmniCode 4-block template
- #102 Align serial.h CLOSING with template X.1-X.10
- #103 Align kernel/ to OmniCode 4-block template with full DAR
- #104 Align uefi_boot.c to full OmniCode 4-block template
- #108 Align serial port configuration between scripts

### OmniCode (1 completed)
- #61 Stage 0: Seed compiler (omnic-c) - COMPLETE

---

## Key Architecture Notes

### Serial Port Configuration (FIXED)
```
Component           Port        Destination
─────────────────────────────────────────────
UEFI bootloader     COM1 (0x3F8)  → debug.log
entry.asm           COM1 (0x3F8)  → debug.log
kernel.c            COM1 (0x3F8)  → debug.log
witness.c           COM1 (0x3F8)  → debug.log

QEMU Routing:
  -serial "file:${DEBUG_LOG}"   # COM1 → debug.log (our output)
  -serial "file:${OVMF_LOG}"    # COM2 → ovmf.log (OVMF firmware)
```

### DAR Witness Flow
```
DETECT (-1)  → Observe/log events
ASSESS (0)   → Evaluate system health
RESTORE (+1) → Take corrective action

Health: -100 to +100
Hebrew States: SHAVAR(0) → YASHAR(3) → TOV(6)
```

### Boot Sequence
```
1. OVMF firmware (UEFI)
2. EFI Shell fallback (if direct boot fails)
3. startup.nsh → \EFI\BOOT\BOOTX64.EFI
4. UEFI bootloader (uefi_boot.c)
5. ExitBootServices
6. entry.asm (kernel entry)
7. kernel.c (kernel_main)
8. witness.c (DAR system)
9. Shell prompt
```

---

## Files to Review Next Session

1. `boot/uefi/uefi_boot.c` — Verify COM1 output working
2. `kernel/src/witness/witness.c` — Full DAR implementation
3. `kernel/src/kernel.c` — Serial init and main flow
4. `scripts/qemu/uefi/run-uefi.sh` — ESP creation and QEMU launch

---

## Plan Reference

See `/home/seanje-lenox-wise/.claude/plans/linear-petting-whale.md` for the master Cornerstone Engine infrastructure plan (Phase A: Kernel-like Header Ladder).

---

*Snapshot preserved by Nova Dawn — CreativeWorkzStudio LLC*
