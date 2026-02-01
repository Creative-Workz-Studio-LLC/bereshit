# =============================================================================
# METADATA [METADATA]
# =============================================================================
#
# Key:         MOS-build-makefile
# Title:       MillenniumOS Build System
# Type:        Makefile
# Component:   Build Infrastructure
# Role:        Build bootloader, kernel, and create bootable image
#
# Status:      Active
# Version:     a-02.00
# Created:     2025-01-30
# Updated:     2025-01-31
#
# Authors:     Seanje Lenox-Wise (Architect), Nova Dawn (Author)
# Organization: CreativeWorkzStudio LLC
#
# Grounding:   Genesis 1:1 — "In the beginning God created"
#              The Makefile creates the OS from source.
#
# =============================================================================
# END METADATA
# =============================================================================

# =============================================================================
# SETUP [SETUP]
# =============================================================================

# Verbosity control
Q := @
ECHO := @echo

# Directories
ROOT_DIR     := $(shell pwd)
BUILD_DIR    := $(ROOT_DIR)/build
BOOT_DIR     := $(ROOT_DIR)/boot/stage0
KERNEL_DIR   := $(ROOT_DIR)/kernel
ISO_DIR      := $(BUILD_DIR)/iso

# Bereshit paths (libtrit foundation)
BERESHIT_DIR := $(ROOT_DIR)/..
LIBTRIT_DIR  := $(BERESHIT_DIR)/word/work/pkg/trit
LIBTRIT_INC  := $(LIBTRIT_DIR)/include
LIBTRIT_LIB  := $(LIBTRIT_DIR)/build/libtrit.a

# Output files
STAGE0A      := $(BUILD_DIR)/stage0a.bin
STAGE0B      := $(BUILD_DIR)/stage0b.bin
BOOTLOADER   := $(BUILD_DIR)/boot.bin
KERNEL_ELF   := $(BUILD_DIR)/kernel.elf
KERNEL_BIN   := $(BUILD_DIR)/kernel.bin
OS_IMAGE     := $(BUILD_DIR)/millenniumos.img
ISO_IMAGE    := $(BUILD_DIR)/millenniumos.iso

# UEFI output files
UEFI_BOOT_DIR    := $(ROOT_DIR)/boot/uefi
UEFI_BOOTLOADER  := $(BUILD_DIR)/BOOTX64.EFI
UEFI_IMAGE       := $(BUILD_DIR)/millenniumos-uefi.img
ESP_DIR          := $(BUILD_DIR)/esp

# Tools
NASM         := nasm
CC           := gcc
LD           := ld
OBJCOPY      := objcopy
QEMU         := qemu-system-x86_64
DD           := dd
AWK          := awk
# ISO tools (xorriso preferred, genisoimage fallback)
MKISOFS      := $(shell which xorriso 2>/dev/null || which genisoimage 2>/dev/null || echo "missing-iso-tool")
CDRECORD     := $(shell which wodim 2>/dev/null || which cdrecord 2>/dev/null || echo "missing-burn-tool")

# UEFI firmware (OVMF)
OVMF_DIR     := /usr/share/OVMF
OVMF_CODE    := $(OVMF_DIR)/OVMF_CODE_4M.fd
OVMF_VARS    := $(OVMF_DIR)/OVMF_VARS_4M.fd
OVMF_VARS_CP := $(BUILD_DIR)/OVMF_VARS_4M.fd

# Flags
NASMFLAGS    := -f bin
NASMFLAGS64  := -f elf64 -I$(ROOT_DIR)/   # Include root for bootinfo.inc access

# C flags for freestanding kernel
# Include paths follow kernel ladder structure (include/ separate from src/)
KERNEL_INC   := $(KERNEL_DIR)/include
KERNEL_SRC   := $(KERNEL_DIR)/src

CFLAGS       := -ffreestanding \
                -fno-stack-protector \
                -fno-pic \
                -mno-red-zone \
                -mno-mmx \
                -mno-sse \
                -mno-sse2 \
                -mcmodel=kernel \
                -m64 \
                -Wall \
                -Wextra \
                -Werror \
                -O2 \
                -g \
                -I$(ROOT_DIR) \
                -I$(KERNEL_INC) \
                -I$(KERNEL_INC)/core \
                -I$(KERNEL_INC)/witness \
                -I$(KERNEL_INC)/hal \
                -I$(KERNEL_INC)/services \
                -I$(KERNEL_INC)/diagnostic \
                -I$(LIBTRIT_INC) \
                -I$(ROOT_DIR)/shell/terminal/include

# Linker flags
LDFLAGS      := -nostdlib \
                -T $(KERNEL_DIR)/link.ld \
                -z max-page-size=0x1000

# UEFI build flags (gnu-efi)
GNUEFI_DIR   := /usr/include/efi
GNUEFI_LIB   := /usr/lib
GNUEFI_CRT   := $(GNUEFI_LIB)/crt0-efi-x86_64.o
GNUEFI_LDS   := $(GNUEFI_LIB)/elf_x86_64_efi.lds

UEFI_CFLAGS  := -ffreestanding \
                -fno-stack-protector \
                -fshort-wchar \
                -mno-red-zone \
                -fpic \
                -fno-strict-aliasing \
                -DEFI_FUNCTION_WRAPPER \
                -DGNU_EFI_USE_MS_ABI \
                -I$(GNUEFI_DIR) \
                -I$(GNUEFI_DIR)/x86_64 \
                -I$(GNUEFI_DIR)/protocol \
                -Wall -Wextra -O2

UEFI_LDFLAGS := -nostdlib \
                -shared \
                -Bsymbolic \
                -T $(GNUEFI_LDS) \
                -L$(GNUEFI_LIB) \
                $(GNUEFI_CRT)

# =============================================================================
# END SETUP
# =============================================================================

# =============================================================================
# BODY [BODY]
# =============================================================================

# Source files
STAGE0A_SRC  := $(BOOT_DIR)/stage0a.asm
STAGE0B_SRC  := $(BOOT_DIR)/stage0b.asm

# Assembly sources (src/ directory, ladder structure)
KERNEL_ASM_SRCS := $(KERNEL_SRC)/entry.asm \
                   $(KERNEL_SRC)/core/interrupt/interrupt.asm \
                   $(KERNEL_SRC)/core/process/context_switch.asm

# C sources (src/ directory, kernel ladder structure)
# LAYER 0: Core (memory, interrupt, process)
# LAYER 1: Witness (DAR foundation)
# LAYER 2: HAL (hardware abstraction)
# LAYER 3: Services (vfs, graphics, ui, cpisi, omnicode, installer)
# LAYER 4: Diagnostic (probes)
# LAYER 5: Shell (terminal interface)
KERNEL_C_SRCS := $(wildcard $(KERNEL_SRC)/*.c) \
                 $(wildcard $(KERNEL_SRC)/core/memory/*.c) \
                 $(wildcard $(KERNEL_SRC)/core/interrupt/*.c) \
                 $(wildcard $(KERNEL_SRC)/core/process/*.c) \
                 $(wildcard $(KERNEL_SRC)/core/timer/*.c) \
                 $(wildcard $(KERNEL_SRC)/core/clock/*.c) \
                 $(wildcard $(KERNEL_SRC)/witness/*.c) \
                 $(wildcard $(KERNEL_SRC)/hal/*.c) \
                 $(wildcard $(KERNEL_SRC)/hal/input/*.c) \
                 $(wildcard $(KERNEL_SRC)/hal/input/config/*.c) \
                 $(wildcard $(KERNEL_SRC)/services/vfs/*.c) \
                 $(wildcard $(KERNEL_SRC)/services/graphics/*.c) \
                 $(wildcard $(KERNEL_SRC)/services/ui/*.c) \
                 $(wildcard $(KERNEL_SRC)/services/cpisi/*.c) \
                 $(wildcard $(KERNEL_SRC)/services/cpisi/config/*.c) \
                 $(wildcard $(KERNEL_SRC)/services/cpisi/format/*.c) \
                 $(wildcard $(KERNEL_SRC)/services/cpisi/time/*.c) \
                 $(wildcard $(KERNEL_SRC)/services/omnicode/*.c) \
                 $(wildcard $(KERNEL_SRC)/services/installer/*.c) \
                 $(wildcard $(KERNEL_SRC)/diagnostic/*.c) \
                 $(wildcard $(ROOT_DIR)/shell/terminal/src/*.c) \
                 $(wildcard $(ROOT_DIR)/shell/terminal/src/builtins/*.c)

# Object files
KERNEL_ASM_OBJS := $(patsubst $(KERNEL_SRC)/%.asm,$(BUILD_DIR)/%.o,$(KERNEL_ASM_SRCS))
# Kernel C sources (under kernel/src/)
KERNEL_C_ONLY   := $(filter $(KERNEL_SRC)/%,$(KERNEL_C_SRCS))
KERNEL_C_OBJS   := $(patsubst $(KERNEL_SRC)/%.c,$(BUILD_DIR)/%.o,$(KERNEL_C_ONLY))
# Shell C sources (under shell/terminal/src/)
SHELL_C_ONLY    := $(filter $(ROOT_DIR)/shell/%,$(KERNEL_C_SRCS))
SHELL_C_OBJS    := $(patsubst $(ROOT_DIR)/shell/%.c,$(BUILD_DIR)/shell/%.o,$(SHELL_C_ONLY))
# Combined objects
KERNEL_OBJS     := $(KERNEL_ASM_OBJS) $(KERNEL_C_OBJS) $(SHELL_C_OBJS)

# -----------------------------------------------------------------------------
# B.1 Primary Targets
# -----------------------------------------------------------------------------

.PHONY: all image iso uefi-image uefi-image-simple run run-dev run-rebuild run-bios debug debug-nowait debug-bios gdb gdb-bios clean help burn-check burn iso-verify run-iso test test-hw test-memory test-cpu test-storage run-uefi run-uefi-simple run-uefi-debug debug-uefi

## all: Build everything (default) - both UEFI and BIOS images
all: image uefi-image-simple
	$(ECHO) "MillenniumOS build complete!"
	$(ECHO) "  BIOS image: $(OS_IMAGE)"
	$(ECHO) "  UEFI ESP:   $(ESP_DIR)/"
	$(ECHO) ""
	$(ECHO) "Run with:"
	$(ECHO) "  make run       → UEFI mode (default, GOP graphics)"
	$(ECHO) "  make run-bios  → BIOS mode (legacy, VESA graphics)"

## image: Build bootable disk image
image: $(OS_IMAGE)
	$(ECHO) "[BUILD] Created: $(OS_IMAGE)"

## iso: Build bootable ISO image
iso: $(ISO_IMAGE)
	$(ECHO) "[BUILD] Created: $(ISO_IMAGE)"

## iso-verify: Verify ISO image structure and bootability
iso-verify: $(ISO_IMAGE)
	$(ECHO) "[VERIFY] Checking ISO image structure..."
	$(Q)file $(ISO_IMAGE) | grep -q "ISO 9660" && echo "  ✓ Valid ISO 9660 format" || echo "  ✗ Invalid format"
	$(Q)isoinfo -d -i $(ISO_IMAGE) 2>/dev/null | grep -E "^(Volume|Boot)" || true
	$(Q)$(MKISOFS) -as mkisofs -report-el-torito -indev $(ISO_IMAGE) 2>/dev/null | head -20 || true
	$(ECHO) "[VERIFY] ISO verification complete"

## run-iso: Run ISO image in QEMU (CD-ROM boot test)
run-iso: $(ISO_IMAGE)
	$(ECHO) "[QEMU] Booting from ISO image..."
	$(Q)$(QEMU) -cdrom $(ISO_IMAGE) \
		-m 128M \
		-boot d \
		-serial stdio

## burn-check: Check for CD/DVD burning capability
burn-check:
	$(ECHO) "[BURN] Checking burning capability..."
ifeq ($(CDRECORD),missing-burn-tool)
	$(ECHO) "  ✗ No burning tool found"
	$(ECHO) "    Install wodim (recommended) or cdrecord"
	$(ECHO) "    Ubuntu/Debian: sudo apt install wodim"
	$(ECHO) "    Fedora: sudo dnf install wodim"
else
	$(ECHO) "  ✓ Burn tool: $(CDRECORD)"
	$(ECHO) "  Available drives:"
	$(Q)$(CDRECORD) --devices 2>/dev/null | grep -E "^\s+[0-9]" || echo "    No drives detected (or requires root)"
endif

## burn: Burn ISO to CD/DVD (requires root for some drives)
burn: $(ISO_IMAGE) burn-check
ifeq ($(CDRECORD),missing-burn-tool)
	$(error No burning tool available. See 'make burn-check' for installation instructions)
endif
	$(ECHO) "[BURN] Ready to burn $(ISO_IMAGE)"
	$(ECHO) "  Insert blank CD/DVD and run:"
	$(ECHO) "    sudo $(CDRECORD) -v dev=/dev/sr0 $(ISO_IMAGE)"
	$(ECHO) "  Or for dry-run (no actual burn):"
	$(ECHO) "    $(CDRECORD) -dummy -v dev=/dev/sr0 $(ISO_IMAGE)"

# -----------------------------------------------------------------------------
# B.1d Hardware Compatibility Tests
# -----------------------------------------------------------------------------

## test: Run all hardware compatibility tests
test: test-hw

## test-hw: Run hardware compatibility test suite
test-hw: $(OS_IMAGE) $(ISO_IMAGE)
	$(ECHO) "[TEST] Running hardware compatibility tests..."
	$(Q)./test/hardware/test-profiles.sh all

## test-memory: Test memory configurations only
test-memory: $(OS_IMAGE)
	$(Q)./test/hardware/test-profiles.sh memory

## test-cpu: Test CPU configurations only
test-cpu: $(OS_IMAGE)
	$(Q)./test/hardware/test-profiles.sh cpu

## test-storage: Test storage configurations only
test-storage: $(OS_IMAGE) $(ISO_IMAGE)
	$(Q)./test/hardware/test-profiles.sh storage

## test-quick: Quick smoke test (minimal hardware profile)
test-quick: $(OS_IMAGE)
	$(ECHO) "[TEST] Running quick smoke test..."
	$(Q)timeout 5 $(QEMU) -drive format=raw,file=$(OS_IMAGE) \
		-m 64M -display none -serial stdio -no-reboot 2>&1 | head -50 || true
	$(ECHO) "[TEST] Quick test complete"

# -----------------------------------------------------------------------------
# B.2 Build Rules
# -----------------------------------------------------------------------------

## Build directories (mirrors kernel ladder structure)
$(BUILD_DIR):
	$(Q)mkdir -p $(BUILD_DIR)
	@# LAYER 0: Core
	$(Q)mkdir -p $(BUILD_DIR)/core/memory
	$(Q)mkdir -p $(BUILD_DIR)/core/interrupt
	$(Q)mkdir -p $(BUILD_DIR)/core/process
	$(Q)mkdir -p $(BUILD_DIR)/core/timer
	$(Q)mkdir -p $(BUILD_DIR)/core/clock
	@# LAYER 1: Witness
	$(Q)mkdir -p $(BUILD_DIR)/witness
	@# LAYER 2: HAL
	$(Q)mkdir -p $(BUILD_DIR)/hal
	@# LAYER 3: Services
	$(Q)mkdir -p $(BUILD_DIR)/services/vfs
	$(Q)mkdir -p $(BUILD_DIR)/services/graphics
	$(Q)mkdir -p $(BUILD_DIR)/services/ui
	$(Q)mkdir -p $(BUILD_DIR)/services/cpisi/config
	$(Q)mkdir -p $(BUILD_DIR)/services/cpisi/format
	$(Q)mkdir -p $(BUILD_DIR)/services/cpisi/time
	$(Q)mkdir -p $(BUILD_DIR)/services/omnicode
	$(Q)mkdir -p $(BUILD_DIR)/services/installer
	@# LAYER 4: Diagnostic
	$(Q)mkdir -p $(BUILD_DIR)/diagnostic

$(ISO_DIR)/boot:
	$(Q)mkdir -p $(ISO_DIR)/boot

## Stage 0a (Boot Sector - 512 bytes)
$(STAGE0A): $(STAGE0A_SRC) | $(BUILD_DIR)
	$(ECHO) "[NASM] Assembling Stage 0a (boot sector)..."
	$(Q)$(NASM) $(NASMFLAGS) -o $@ $<

## Stage 0b (Loader - sets up CPU and loads kernel)
$(STAGE0B): $(STAGE0B_SRC) | $(BUILD_DIR)
	$(ECHO) "[NASM] Assembling Stage 0b (loader)..."
	$(Q)$(NASM) $(NASMFLAGS) -o $@ $<

## Combined bootloader (Stage 0a + Stage 0b)
$(BOOTLOADER): $(STAGE0A) $(STAGE0B) | $(BUILD_DIR)
	$(ECHO) "[BOOT] Combining bootloader stages..."
	$(Q)cat $(STAGE0A) $(STAGE0B) > $@

## Kernel assembly objects (generic pattern - from src/)
$(BUILD_DIR)/%.o: $(KERNEL_SRC)/%.asm | $(BUILD_DIR)
	$(ECHO) "[NASM] Assembling $<..."
	$(Q)mkdir -p $(dir $@)
	$(Q)$(NASM) $(NASMFLAGS64) -o $@ $<

## Kernel C objects (generic pattern - from src/)
$(BUILD_DIR)/%.o: $(KERNEL_SRC)/%.c | $(BUILD_DIR)
	$(ECHO) "[CC] Compiling $<..."
	$(Q)mkdir -p $(dir $@)
	$(Q)$(CC) $(CFLAGS) -c -o $@ $<

## Shell C objects (from shell/terminal/src/)
$(BUILD_DIR)/shell/%.o: $(ROOT_DIR)/shell/%.c | $(BUILD_DIR)
	$(ECHO) "[CC] Compiling $<..."
	$(Q)mkdir -p $(dir $@)
	$(Q)$(CC) $(CFLAGS) -c -o $@ $<

## libtrit dependency
$(LIBTRIT_LIB):
	$(ECHO) "[LIBTRIT] Building libtrit..."
	$(Q)$(MAKE) -C $(LIBTRIT_DIR)

## Kernel ELF (linked with libtrit)
$(KERNEL_ELF): $(KERNEL_OBJS) $(LIBTRIT_LIB) | $(BUILD_DIR)
	$(ECHO) "[LD] Linking kernel..."
	$(Q)$(LD) $(LDFLAGS) -o $@ $(KERNEL_OBJS) $(LIBTRIT_LIB)

## Kernel binary (flat binary from ELF)
$(KERNEL_BIN): $(KERNEL_ELF)
	$(ECHO) "[OBJCOPY] Creating kernel binary..."
	$(Q)$(OBJCOPY) -O binary $< $@

## Disk image (bootloader stages + kernel)
## Layout: Sector 0 = Stage0a, Sectors 1-32 = Stage0b, Sector 34+ = Kernel
##
## Size: 16MB (32768 sectors) - room for 448KB kernel + future modules
## This is a proper HD image, not floppy-constrained.
##
$(OS_IMAGE): $(BOOTLOADER) $(KERNEL_BIN)
	$(ECHO) "[IMAGE] Creating 16MB disk image..."
	$(Q)$(DD) if=/dev/zero of=$@ bs=512 count=32768 2>/dev/null
	$(Q)$(DD) if=$(BOOTLOADER) of=$@ conv=notrunc 2>/dev/null
	$(Q)$(DD) if=$(KERNEL_BIN) of=$@ seek=34 conv=notrunc 2>/dev/null
	$(Q)ls -lh $@ | $(AWK) '{print "[IMAGE] Size: "$$5}'
	$(ECHO) "[IMAGE] MillenniumOS image ready"

## ISO image (for CD-ROM boot)
## Uses El Torito specification for bootable CD/DVD
$(ISO_IMAGE): $(BOOTLOADER) $(KERNEL_BIN) | $(ISO_DIR)/boot
	$(ECHO) "[ISO] Creating bootable ISO image..."
	$(Q)cp $(BOOTLOADER) $(ISO_DIR)/boot/boot.bin
	$(Q)cp $(KERNEL_BIN) $(ISO_DIR)/boot/kernel.bin
ifeq ($(findstring xorriso,$(MKISOFS)),xorriso)
	$(Q)$(MKISOFS) -as mkisofs \
		-R -J \
		-V "MILLENNIUMOS" \
		-b boot/boot.bin \
		-no-emul-boot \
		-boot-load-size 4 \
		-boot-info-table \
		-o $@ $(ISO_DIR)
else ifeq ($(findstring genisoimage,$(MKISOFS)),genisoimage)
	$(Q)$(MKISOFS) -R -J \
		-V "MILLENNIUMOS" \
		-b boot/boot.bin \
		-no-emul-boot \
		-boot-load-size 4 \
		-boot-info-table \
		-o $@ $(ISO_DIR)
else
	$(error No ISO creation tool found. Install xorriso or genisoimage)
endif
	$(ECHO) "[ISO] Created: $@"

# -----------------------------------------------------------------------------
# B.2b Hybrid UEFI+BIOS Image
# -----------------------------------------------------------------------------
#
# Creates a single bootable image that works with both:
#   - UEFI firmware (primary) - uses GOP graphics
#   - BIOS firmware (fallback) - uses VESA graphics
#
# Structure:
#   - GPT partition table (for UEFI)
#   - Protective MBR with BIOS boot code (for legacy)
#   - EFI System Partition with BOOTX64.EFI
#   - Kernel accessible to both boot paths
#

HYBRID_IMAGE := $(BUILD_DIR)/millenniumos.img

## hybrid-image: Build hybrid UEFI+BIOS bootable image (default target)
.PHONY: hybrid-image
hybrid-image: $(BOOTLOADER) $(KERNEL_BIN) uefi-image-simple
	$(ECHO) "[HYBRID] Creating hybrid UEFI+BIOS image (64MB)..."
	@# Create 64MB image
	$(Q)$(DD) if=/dev/zero of=$(HYBRID_IMAGE) bs=1M count=64 2>/dev/null
	@#
	@# Write BIOS boot code to first 446 bytes (MBR boot code area)
	@# This allows BIOS systems to boot using traditional MBR
	$(Q)$(DD) if=$(STAGE0A) of=$(HYBRID_IMAGE) bs=446 count=1 conv=notrunc 2>/dev/null
	@#
	@# Create GPT partition table (preserves first 446 bytes of MBR)
	$(Q)parted -s $(HYBRID_IMAGE) mklabel gpt 2>/dev/null || true
	@#
	@# Create partitions:
	@#   1. BIOS Boot partition (for GRUB-style BIOS boot on GPT) - 1MB
	@#   2. EFI System Partition - 32MB
	@#   3. Boot partition (kernel) - remaining
	$(Q)parted -s $(HYBRID_IMAGE) mkpart bios_boot 1MiB 2MiB 2>/dev/null
	$(Q)parted -s $(HYBRID_IMAGE) set 1 bios_grub on 2>/dev/null
	$(Q)parted -s $(HYBRID_IMAGE) mkpart ESP fat32 2MiB 34MiB 2>/dev/null
	$(Q)parted -s $(HYBRID_IMAGE) set 2 esp on 2>/dev/null
	$(Q)parted -s $(HYBRID_IMAGE) mkpart boot 34MiB 100% 2>/dev/null
	@#
	@# Write BIOS stage0b to BIOS boot partition (sector 2048 = 1MB)
	$(Q)$(DD) if=$(STAGE0B) of=$(HYBRID_IMAGE) bs=512 seek=2048 conv=notrunc 2>/dev/null
	@#
	@# Write kernel to boot partition (sector 69632 = 34MB)
	$(Q)$(DD) if=$(KERNEL_BIN) of=$(HYBRID_IMAGE) bs=512 seek=69632 conv=notrunc 2>/dev/null
	@#
	@# Format and populate ESP (requires sudo for loop mount)
	$(ECHO) "[HYBRID] Populating EFI System Partition..."
	$(Q)LOOP=$$(sudo losetup --find --show --partscan $(HYBRID_IMAGE)) && \
		sudo mkfs.vfat -F32 $${LOOP}p2 2>/dev/null && \
		sudo mkdir -p /mnt/mos-esp && \
		sudo mount $${LOOP}p2 /mnt/mos-esp && \
		sudo mkdir -p /mnt/mos-esp/EFI/BOOT && \
		sudo cp $(ESP_DIR)/EFI/BOOT/BOOTX64.EFI /mnt/mos-esp/EFI/BOOT/ 2>/dev/null || \
			echo "  Note: UEFI bootloader not built yet (run make uefi-bootloader)" && \
		sudo cp $(KERNEL_BIN) /mnt/mos-esp/kernel.bin && \
		sync && \
		sudo umount /mnt/mos-esp && \
		sudo losetup -d $$LOOP
	@#
	$(ECHO) "[HYBRID] Created: $(HYBRID_IMAGE)"
	$(ECHO) "  UEFI boot: EFI/BOOT/BOOTX64.EFI → kernel.bin"
	$(ECHO) "  BIOS boot: MBR → stage0b → kernel"

## hybrid-image-nosudo: Build hybrid image without sudo (ESP not formatted)
## Use this for development - run with QEMU's FAT emulation
.PHONY: hybrid-image-nosudo
hybrid-image-nosudo: $(BOOTLOADER) $(KERNEL_BIN) uefi-image-simple
	$(ECHO) "[HYBRID] Creating hybrid image (no sudo - dev mode)..."
	$(Q)$(DD) if=/dev/zero of=$(HYBRID_IMAGE) bs=1M count=64 2>/dev/null
	$(Q)$(DD) if=$(STAGE0A) of=$(HYBRID_IMAGE) bs=446 count=1 conv=notrunc 2>/dev/null
	$(Q)$(DD) if=$(STAGE0B) of=$(HYBRID_IMAGE) bs=512 seek=1 conv=notrunc 2>/dev/null
	$(Q)$(DD) if=$(KERNEL_BIN) of=$(HYBRID_IMAGE) bs=512 seek=34 conv=notrunc 2>/dev/null
	$(ECHO) "[HYBRID] Created: $(HYBRID_IMAGE) (BIOS-only, use --uefi with ESP_DIR)"

# -----------------------------------------------------------------------------
# B.2c UEFI Build Rules
# -----------------------------------------------------------------------------
#
# Graphics Tripwire: UEFI GOP (primary) → VESA (BIOS fallback) → VGA Text
# This builds the UEFI bootloader and creates a GPT disk image with ESP.
#

## UEFI bootloader object
$(BUILD_DIR)/uefi_boot.o: $(UEFI_BOOT_DIR)/uefi_boot.c | $(BUILD_DIR)
	$(ECHO) "[UEFI-CC] Compiling UEFI bootloader..."
	$(Q)$(CC) $(UEFI_CFLAGS) -c -o $@ $<

## UEFI bootloader shared object
$(BUILD_DIR)/uefi_boot.so: $(BUILD_DIR)/uefi_boot.o
	$(ECHO) "[UEFI-LD] Linking UEFI bootloader..."
	$(Q)$(LD) $(UEFI_LDFLAGS) -o $@ $< -lefi -lgnuefi

## UEFI bootloader PE/COFF executable
$(UEFI_BOOTLOADER): $(BUILD_DIR)/uefi_boot.so
	$(ECHO) "[UEFI-OBJCOPY] Creating BOOTX64.EFI..."
	$(Q)$(OBJCOPY) -j .text -j .sdata -j .data -j .dynamic \
		-j .dynsym -j .rel -j .rela -j .rel.* -j .rela.* -j .reloc \
		--target=efi-app-x86_64 $< $@

## UEFI ESP directory structure
$(ESP_DIR)/EFI/BOOT: | $(BUILD_DIR)
	$(Q)mkdir -p $@

## UEFI disk image (GPT with EFI System Partition)
## 64MB image: ESP for bootloader + kernel
$(UEFI_IMAGE): $(UEFI_BOOTLOADER) $(KERNEL_BIN) | $(ESP_DIR)/EFI/BOOT
	$(ECHO) "[UEFI-IMAGE] Creating UEFI bootable disk image..."
	@# Create 64MB disk image
	$(Q)$(DD) if=/dev/zero of=$@ bs=1M count=64 2>/dev/null
	@# Create GPT partition table with EFI System Partition
	$(Q)parted -s $@ mklabel gpt
	$(Q)parted -s $@ mkpart ESP fat32 1MiB 63MiB
	$(Q)parted -s $@ set 1 esp on
	@# Set up loop device and format ESP
	$(ECHO) "[UEFI-IMAGE] Formatting EFI System Partition..."
	$(Q)LOOP=$$(sudo losetup --find --show --partscan $@) && \
		sudo mkfs.vfat -F32 $${LOOP}p1 && \
		sudo mkdir -p /mnt/mos-esp && \
		sudo mount $${LOOP}p1 /mnt/mos-esp && \
		sudo mkdir -p /mnt/mos-esp/EFI/BOOT && \
		sudo cp $(UEFI_BOOTLOADER) /mnt/mos-esp/EFI/BOOT/BOOTX64.EFI && \
		sudo cp $(KERNEL_BIN) /mnt/mos-esp/kernel.bin && \
		sudo umount /mnt/mos-esp && \
		sudo losetup -d $$LOOP
	$(ECHO) "[UEFI-IMAGE] Created: $@"

## uefi-image: Build UEFI bootable disk image
.PHONY: uefi-image
uefi-image: $(UEFI_IMAGE)
	$(ECHO) "[BUILD] UEFI image ready: $(UEFI_IMAGE)"

## uefi-image-simple: Build UEFI image without sudo (copies files only)
## Use with OVMF which can read raw directory as FAT
.PHONY: uefi-image-simple
uefi-image-simple: $(UEFI_BOOTLOADER) $(KERNEL_BIN) | $(ESP_DIR)/EFI/BOOT
	$(ECHO) "[UEFI] Setting up ESP directory structure..."
	$(Q)cp $(UEFI_BOOTLOADER) $(ESP_DIR)/EFI/BOOT/BOOTX64.EFI
	$(Q)cp $(KERNEL_BIN) $(ESP_DIR)/kernel.bin
	$(ECHO) "[UEFI] ESP directory ready at: $(ESP_DIR)"
	$(ECHO) "[UEFI] Use with: make run-uefi-dir"

# -----------------------------------------------------------------------------
# B.3 Run Targets
# -----------------------------------------------------------------------------
#
# Boot Mode Ladder (scripts aligned to 4-block structure):
#
#   ┌────────────────────────────────────────────────────────────┐
#   │  run-uefi.sh        │ PRODUCTION — Full GPT + ESP image   │
#   ├────────────────────────────────────────────────────────────┤
#   │  run-uefi-simple.sh │ DEVELOPMENT — QEMU vvfat            │
#   ├────────────────────────────────────────────────────────────┤
#   │  run-uefi-debug.sh  │ DEBUG — GDB server enabled          │
#   ├────────────────────────────────────────────────────────────┤
#   │  run-bios.sh        │ LEGACY — SeaBIOS fallback           │
#   └────────────────────────────────────────────────────────────┘
#
# All scripts are config-driven from scripts/qemu/config/uefi.cfg
#

## run: Run in QEMU (UEFI production mode - default)
## Uses proper GPT disk with FAT32 ESP partition
run: image uefi-image-simple
	$(ECHO) "[QEMU] Starting MillenniumOS (UEFI production mode)..."
	$(Q)$(ROOT_DIR)/scripts/qemu/uefi/run-uefi.sh

## run-dev: Run in QEMU (UEFI development mode - vvfat)
## Uses QEMU's vvfat driver for rapid iteration (no disk image creation)
run-dev: uefi-image-simple
	$(ECHO) "[QEMU] Starting MillenniumOS (UEFI development mode)..."
	$(Q)$(ROOT_DIR)/scripts/qemu/uefi/run-uefi-simple.sh

## run-bios: Run in QEMU with BIOS firmware (legacy mode)
## Uses SeaBIOS with VESA graphics fallback chain
run-bios: image
	$(ECHO) "[QEMU] Starting MillenniumOS (BIOS legacy mode)..."
	$(Q)$(ROOT_DIR)/scripts/qemu/bios/run-bios.sh

## run-rebuild: Run in UEFI mode with forced ESP rebuild
## Forces recreation of ESP image even if it exists
run-rebuild: image uefi-image-simple
	$(ECHO) "[QEMU] Starting MillenniumOS (UEFI, forced rebuild)..."
	$(Q)$(ROOT_DIR)/scripts/qemu/uefi/run-uefi.sh --rebuild

## debug: Run in QEMU with debug output (UEFI debug mode)
## Enables GDB server on :1234 and waits for debugger connection
debug: image uefi-image-simple
	$(ECHO) "[QEMU] Starting MillenniumOS (UEFI debug mode)..."
	$(Q)$(ROOT_DIR)/scripts/qemu/uefi/run-uefi-debug.sh

## debug-nowait: Run in debug mode without waiting for GDB
## Enables GDB server but starts immediately
debug-nowait: image uefi-image-simple
	$(ECHO) "[QEMU] Starting MillenniumOS (UEFI debug, no GDB wait)..."
	$(Q)$(ROOT_DIR)/scripts/qemu/uefi/run-uefi-debug.sh --no-wait

## debug-bios: Run BIOS mode with debug output
## Uses SeaBIOS with debug tracing enabled
debug-bios: image
	$(ECHO) "[QEMU] Starting MillenniumOS (BIOS debug mode)..."
	$(Q)$(ROOT_DIR)/scripts/qemu/bios/run-bios.sh --debug

## gdb: Run with GDB server (alias for debug)
gdb: debug

## gdb-bios: Run BIOS mode with GDB server
gdb-bios: image
	$(ECHO) "[QEMU] Starting MillenniumOS (BIOS with GDB)..."
	$(Q)$(ROOT_DIR)/scripts/qemu/bios/run-bios.sh --gdb

# -----------------------------------------------------------------------------
# B.3b UEFI Run Targets
# -----------------------------------------------------------------------------
#
# UEFI boot path using OVMF firmware.
# Primary development path for 64-bit native boot.
#
# Delegates to aligned scripts in scripts/qemu/uefi/ which:
#   - Load config from scripts/qemu/config/uefi.cfg
#   - Follow 4-block structure (METADATA → SETUP → BODY → CLOSING)
#   - Document their position in boot mode ladder
#   - Handle OVMF detection (EDK2-built or system package)
#

## Copy OVMF_VARS to build dir (UEFI needs writable vars)
$(OVMF_VARS_CP): | $(BUILD_DIR)
	$(Q)cp $(OVMF_VARS) $@

## run-uefi: Run UEFI image in QEMU (EDK2-compatible GPT disk)
## Uses proper GPT disk with FAT32 ESP partition (same as `make run`)
## PRODUCTION mode in boot mode ladder
run-uefi: image uefi-image-simple
	$(ECHO) "[QEMU/UEFI] Starting MillenniumOS (production mode)..."
	$(Q)$(ROOT_DIR)/scripts/qemu/uefi/run-uefi.sh

## run-uefi-simple: Run with QEMU's vvfat driver (development fallback)
## Uses QEMU's built-in FAT driver - simpler but less compatible
## DEVELOPMENT mode in boot mode ladder
run-uefi-simple: uefi-image-simple
	$(ECHO) "[QEMU/UEFI] Starting MillenniumOS (development mode)..."
	$(Q)$(ROOT_DIR)/scripts/qemu/uefi/run-uefi-simple.sh

## run-uefi-debug: Run UEFI with GDB server enabled
## Pauses at startup waiting for GDB connection on :1234
## DEBUG mode in boot mode ladder
run-uefi-debug: image uefi-image-simple
	$(ECHO) "[QEMU/UEFI] Starting MillenniumOS (debug mode)..."
	$(Q)$(ROOT_DIR)/scripts/qemu/uefi/run-uefi-debug.sh

## run-uefi-bios: Run BIOS image in QEMU with UEFI firmware (fallback test)
run-uefi-bios: $(OS_IMAGE) $(OVMF_VARS_CP)
	$(ECHO) "[QEMU/UEFI] Starting MillenniumOS (BIOS image via UEFI)..."
	$(ECHO) "  Note: Tests CSM/legacy boot path"
	$(Q)$(QEMU) \
		-drive if=pflash,format=raw,readonly=on,file=$(OVMF_CODE) \
		-drive if=pflash,format=raw,file=$(OVMF_VARS_CP) \
		-drive format=raw,file=$(OS_IMAGE) \
		-m 512M \
		-vga std \
		-display gtk,window-close=on,show-cursor=on \
		-serial mon:stdio \
		-no-reboot

## run-uefi-shell: Run UEFI shell only (no disk, for testing OVMF)
run-uefi-shell: $(OVMF_VARS_CP)
	$(ECHO) "[QEMU/UEFI] Starting UEFI shell (OVMF only)..."
	$(Q)$(QEMU) \
		-drive if=pflash,format=raw,readonly=on,file=$(OVMF_CODE) \
		-drive if=pflash,format=raw,file=$(OVMF_VARS_CP) \
		-m 256M \
		-serial stdio

## debug-uefi: Alias for run-uefi-debug (GDB enabled)
debug-uefi: run-uefi-debug

## uefi-check: Verify OVMF and gnu-efi are available
uefi-check:
	$(ECHO) "[UEFI] Checking UEFI development environment..."
	$(ECHO) ""
	$(ECHO) "OVMF (UEFI firmware for QEMU):"
	@if [ -f "$(OVMF_CODE)" ]; then \
		echo "  ✓ OVMF_CODE: $(OVMF_CODE)"; \
	else \
		echo "  ✗ OVMF_CODE not found at $(OVMF_CODE)"; \
		echo "    Install: sudo apt install ovmf"; \
	fi
	@if [ -f "$(OVMF_VARS)" ]; then \
		echo "  ✓ OVMF_VARS: $(OVMF_VARS)"; \
	else \
		echo "  ✗ OVMF_VARS not found at $(OVMF_VARS)"; \
	fi
	$(ECHO) ""
	$(ECHO) "gnu-efi (UEFI development library):"
	@if [ -d "$(GNUEFI_DIR)" ]; then \
		echo "  ✓ gnu-efi headers: $(GNUEFI_DIR)"; \
	else \
		echo "  ✗ gnu-efi headers not found at $(GNUEFI_DIR)"; \
		echo "    Install: sudo apt install gnu-efi"; \
	fi
	@if [ -f "$(GNUEFI_CRT)" ]; then \
		echo "  ✓ gnu-efi CRT: $(GNUEFI_CRT)"; \
	else \
		echo "  ✗ gnu-efi CRT not found at $(GNUEFI_CRT)"; \
	fi
	@if [ -f "$(GNUEFI_LDS)" ]; then \
		echo "  ✓ gnu-efi linker script: $(GNUEFI_LDS)"; \
	else \
		echo "  ✗ gnu-efi linker script not found at $(GNUEFI_LDS)"; \
	fi
	$(ECHO) ""
	$(ECHO) "[UEFI] To install all UEFI dependencies:"
	$(ECHO) "  sudo apt install ovmf gnu-efi"

# -----------------------------------------------------------------------------
# B.4 Utility Targets
# -----------------------------------------------------------------------------

## clean: Remove build artifacts
clean:
	$(ECHO) "[CLEAN] Removing build directory..."
	$(Q)rm -rf $(BUILD_DIR)

## info: Show build configuration
info:
	$(ECHO) "MillenniumOS Build Configuration"
	$(ECHO) "================================"
	$(ECHO) "Root:        $(ROOT_DIR)"
	$(ECHO) "Build:       $(BUILD_DIR)"
	$(ECHO) "libtrit:     $(LIBTRIT_DIR)"
	$(ECHO) ""
	$(ECHO) "Outputs:"
	$(ECHO) "  Bootloader: $(BOOTLOADER)"
	$(ECHO) "  Kernel:     $(KERNEL_BIN)"
	$(ECHO) "  Image:      $(OS_IMAGE)"
	$(ECHO) ""
	$(ECHO) "Kernel Assembly Sources:"
	$(Q)echo "$(KERNEL_ASM_SRCS)" | tr ' ' '\n' | sed 's/^/  /'
	$(ECHO) ""
	$(ECHO) "Kernel C Sources:"
	$(Q)echo "$(KERNEL_C_SRCS)" | tr ' ' '\n' | sed 's/^/  /'

# -----------------------------------------------------------------------------
# B.5 Installation Targets
# -----------------------------------------------------------------------------
#
# Install MillenniumOS to system or user directory:
#   - Launcher script (millenniumos-run)
#   - OS image
#   - Desktop entry
#   - Icons
#

# Installation paths (can be overridden)
# NOTE: Use = (deferred) not := (immediate) so install-user PREFIX override works
PREFIX          ?= /usr/local
BINDIR          = $(PREFIX)/bin
DATADIR         = $(PREFIX)/share/millenniumos
APPLICATIONSDIR = $(PREFIX)/share/applications
ICONDIR         = $(PREFIX)/share/icons/hicolor

# Resource paths
RESOURCES_DIR   := $(ROOT_DIR)/resources
DESKTOP_DIR     := $(RESOURCES_DIR)/desktop
ICONS_DIR       := $(RESOURCES_DIR)/icons
SCRIPTS_DIR     := $(ROOT_DIR)/scripts

.PHONY: install install-user uninstall uninstall-user update-user

## install: Install MillenniumOS to system (PREFIX=/usr/local)
## Installs both BIOS and UEFI boot images
install: image uefi-image-simple
	$(ECHO) "Installing MillenniumOS..."
	$(ECHO) "  PREFIX: $(PREFIX)"
	@#
	@# ─── Phase 1: Directories ───
	@#
	$(Q)mkdir -p $(DESTDIR)$(BINDIR)
	$(Q)mkdir -p $(DESTDIR)$(DATADIR)
	$(Q)mkdir -p $(DESTDIR)$(APPLICATIONSDIR)
	$(Q)mkdir -p $(DESTDIR)$(ICONDIR)/32x32/apps
	$(Q)mkdir -p $(DESTDIR)$(ICONDIR)/48x48/apps
	$(Q)mkdir -p $(DESTDIR)$(ICONDIR)/64x64/apps
	$(Q)mkdir -p $(DESTDIR)$(ICONDIR)/128x128/apps
	$(Q)mkdir -p $(DESTDIR)$(ICONDIR)/256x256/apps
	@#
	@# ─── Phase 2: Launcher Script ───
	@# Update paths for installed location, UEFI default
	@#
	$(Q)sed -e 's|ROOT_DIR=".*"|ROOT_DIR="$(DATADIR)"|g' \
	        -e 's|BUILD_DIR=".*"|BUILD_DIR="$(DATADIR)"|g' \
	        -e 's|OS_IMAGE=".*"|OS_IMAGE="$(DATADIR)/millenniumos.img"|g' \
	        -e 's|ESP_DIR=".*"|ESP_DIR="$(DATADIR)/esp"|g' \
	        -e 's|OVMF_VARS_CP=".*"|OVMF_VARS_CP="$(DATADIR)/OVMF_VARS.fd"|g' \
	        $(SCRIPTS_DIR)/run.sh > $(DESTDIR)$(BINDIR)/millenniumos-run
	$(Q)chmod 755 $(DESTDIR)$(BINDIR)/millenniumos-run
	$(ECHO) "  Launcher: $(DESTDIR)$(BINDIR)/millenniumos-run"
	@#
	@# ─── Phase 3: OS Images (BIOS + UEFI) ───
	@#
	$(Q)cp $(OS_IMAGE) $(DESTDIR)$(DATADIR)/millenniumos.img
	$(ECHO) "  BIOS Image: $(DESTDIR)$(DATADIR)/millenniumos.img"
	@# Copy ESP directory structure for UEFI boot
	$(Q)mkdir -p $(DESTDIR)$(DATADIR)/esp/EFI/BOOT
	$(Q)cp $(ESP_DIR)/EFI/BOOT/BOOTX64.EFI $(DESTDIR)$(DATADIR)/esp/EFI/BOOT/ 2>/dev/null || true
	$(Q)cp $(KERNEL_BIN) $(DESTDIR)$(DATADIR)/esp/kernel.bin 2>/dev/null || true
	$(ECHO) "  UEFI ESP:   $(DESTDIR)$(DATADIR)/esp/"
	@# Copy OVMF_VARS for writable UEFI variables
	$(Q)cp $(OVMF_VARS) $(DESTDIR)$(DATADIR)/OVMF_VARS.fd 2>/dev/null || true
	$(ECHO) "  OVMF Vars:  $(DESTDIR)$(DATADIR)/OVMF_VARS.fd"
	@#
	@# ─── Phase 4: Desktop Entry ───
	@#
	$(Q)sed -e 's|@BINDIR@|$(BINDIR)|g' \
	        -e 's|@DATADIR@|$(DATADIR)|g' \
	        $(DESKTOP_DIR)/millenniumos.desktop.in > $(DESTDIR)$(APPLICATIONSDIR)/millenniumos.desktop
	$(Q)chmod 644 $(DESTDIR)$(APPLICATIONSDIR)/millenniumos.desktop
	$(ECHO) "  Desktop:  $(DESTDIR)$(APPLICATIONSDIR)/millenniumos.desktop"
	@#
	@# ─── Phase 5: Icons ───
	@#
	$(Q)cp $(ICONS_DIR)/millenniumos-32.png $(DESTDIR)$(ICONDIR)/32x32/apps/millenniumos.png
	$(Q)cp $(ICONS_DIR)/millenniumos-48.png $(DESTDIR)$(ICONDIR)/48x48/apps/millenniumos.png
	$(Q)cp $(ICONS_DIR)/millenniumos-64.png $(DESTDIR)$(ICONDIR)/64x64/apps/millenniumos.png
	$(Q)cp $(ICONS_DIR)/millenniumos-128.png $(DESTDIR)$(ICONDIR)/128x128/apps/millenniumos.png
	$(Q)cp $(ICONS_DIR)/millenniumos-256.png $(DESTDIR)$(ICONDIR)/256x256/apps/millenniumos.png
	$(ECHO) "  Icons:    $(DESTDIR)$(ICONDIR)/*/apps/millenniumos.png"
	@#
	@# ─── Phase 6: Update Caches ───
	@#
	$(Q)if [ "$(DESTDIR)" = "" ]; then \
		if [ -x "$$(which update-desktop-database 2>/dev/null)" ]; then \
			update-desktop-database $(APPLICATIONSDIR) 2>/dev/null || true; \
		fi; \
		if [ -x "$$(which gtk-update-icon-cache 2>/dev/null)" ]; then \
			gtk-update-icon-cache -f -t $(ICONDIR) 2>/dev/null || true; \
		fi; \
	fi
	@#
	$(ECHO) ""
	$(ECHO) "✓ Installation complete"
	$(ECHO) ""
	$(ECHO) "  Desktop app starts in UEFI mode (GOP graphics)"
	$(ECHO) "  For BIOS mode: millenniumos-run --bios"
	$(ECHO) ""

## install-user: Install to user directory (~/.local)
install-user: PREFIX=$(HOME)/.local
install-user: APPLICATIONSDIR=$(HOME)/.local/share/applications
install-user: ICONDIR=$(HOME)/.local/share/icons/hicolor
install-user: install
	$(Q)update-desktop-database $(HOME)/.local/share/applications/ 2>/dev/null || true
	$(Q)gtk-update-icon-cache -f -t $(HOME)/.local/share/icons/hicolor/ 2>/dev/null || true
	$(ECHO) "✓ Installed to user's local directory"
	$(ECHO) "  Launcher: $(HOME)/.local/bin/millenniumos-run"
	$(ECHO) "  Desktop entry installed — search 'MillenniumOS' in applications"

## uninstall: Remove MillenniumOS from system
uninstall:
	$(ECHO) "Uninstalling MillenniumOS..."
	$(Q)rm -f $(DESTDIR)$(BINDIR)/millenniumos-run
	$(Q)rm -rf $(DESTDIR)$(DATADIR)
	$(Q)rm -f $(DESTDIR)$(APPLICATIONSDIR)/millenniumos.desktop
	$(Q)rm -f $(DESTDIR)$(ICONDIR)/32x32/apps/millenniumos.png
	$(Q)rm -f $(DESTDIR)$(ICONDIR)/48x48/apps/millenniumos.png
	$(Q)rm -f $(DESTDIR)$(ICONDIR)/64x64/apps/millenniumos.png
	$(Q)rm -f $(DESTDIR)$(ICONDIR)/128x128/apps/millenniumos.png
	$(Q)rm -f $(DESTDIR)$(ICONDIR)/256x256/apps/millenniumos.png
	$(Q)if [ "$(DESTDIR)" = "" ]; then \
		update-desktop-database $(APPLICATIONSDIR) 2>/dev/null || true; \
		gtk-update-icon-cache -f -t $(ICONDIR) 2>/dev/null || true; \
	fi
	$(ECHO) "✓ Uninstall complete"

## uninstall-user: Remove from user directory (~/.local)
uninstall-user: PREFIX=$(HOME)/.local
uninstall-user: APPLICATIONSDIR=$(HOME)/.local/share/applications
uninstall-user: ICONDIR=$(HOME)/.local/share/icons/hicolor
uninstall-user: uninstall
	$(ECHO) "✓ Uninstalled from user's local directory"

## update-user: Rebuild and reinstall to user directory
update-user: clean image
	@$(MAKE) --no-print-directory uninstall-user 2>/dev/null || true
	@$(MAKE) --no-print-directory install-user
	$(ECHO) ""
	$(ECHO) "✓ Update complete — desktop app now uses latest build"

# -----------------------------------------------------------------------------
# B.6 Help Target
# -----------------------------------------------------------------------------

## help: Show available targets
help:
	$(ECHO) ""
	$(ECHO) "╔═══════════════════════════════════════════════════════════════════╗"
	$(ECHO) "║              MillenniumOS Build System — a-02.00                  ║"
	$(ECHO) "║          Redeeming Technology for the Kingdom                     ║"
	$(ECHO) "╚═══════════════════════════════════════════════════════════════════╝"
	$(ECHO) ""
	$(ECHO) "Build targets:"
	$(ECHO) "  make              Build hybrid UEFI+BIOS image (default)"
	$(ECHO) "  make hybrid-image Build hybrid image with sudo (full ESP)"
	$(ECHO) "  make image        Build BIOS-only disk image"
	$(ECHO) "  make iso          Build bootable ISO image"
	$(ECHO) "  make clean        Remove build artifacts"
	$(ECHO) ""
	$(ECHO) "╭───────────────────────────────────────────────────────────────────╮"
	$(ECHO) "│                      Boot Mode Ladder                             │"
	$(ECHO) "├───────────────────────────────────────────────────────────────────┤"
	$(ECHO) "│  make run           PRODUCTION — Full GPT + ESP image             │"
	$(ECHO) "│  make run-dev       DEVELOPMENT — QEMU vvfat (fast iteration)     │"
	$(ECHO) "│  make debug         DEBUG — GDB server enabled, waits for attach  │"
	$(ECHO) "│  make run-bios      LEGACY — SeaBIOS fallback (VESA graphics)     │"
	$(ECHO) "╰───────────────────────────────────────────────────────────────────╯"
	$(ECHO) ""
	$(ECHO) "Run targets:"
	$(ECHO) "  make run           Run UEFI production (GPT + ESP disk)"
	$(ECHO) "  make run-dev       Run UEFI development (vvfat, no disk rebuild)"
	$(ECHO) "  make run-rebuild   Run UEFI with forced ESP rebuild"
	$(ECHO) "  make run-bios      Run BIOS mode (SeaBIOS + VESA)"
	$(ECHO) "  make run-iso       Run from ISO image"
	$(ECHO) ""
	$(ECHO) "Debug targets:"
	$(ECHO) "  make debug         UEFI debug mode (GDB waits for connection)"
	$(ECHO) "  make debug-nowait  UEFI debug mode (GDB enabled, starts immediately)"
	$(ECHO) "  make debug-bios    BIOS debug mode (tracing enabled)"
	$(ECHO) "  make gdb           Alias for debug"
	$(ECHO) "  make gdb-bios      BIOS mode with GDB server"
	$(ECHO) ""
	$(ECHO) "Graphics (automatic based on boot mode):"
	$(ECHO) "  UEFI: GOP framebuffer (modern hardware)"
	$(ECHO) "  BIOS: VESA 1024x768 → 800x600 → 640x480 → VGA Text"
	$(ECHO) ""
	$(ECHO) "UEFI tools:"
	$(ECHO) "  make run-uefi-shell  Run UEFI shell only (test OVMF)"
	$(ECHO) "  make uefi-check      Verify OVMF/gnu-efi installation"
	$(ECHO) ""
	$(ECHO) "ISO/CD-ROM targets:"
	$(ECHO) "  make iso-verify  Verify ISO image structure"
	$(ECHO) "  make burn-check  Check CD/DVD burning capability"
	$(ECHO) "  make burn        Show burn instructions for ISO"
	$(ECHO) ""
	$(ECHO) "Test targets:"
	$(ECHO) "  make test        Run all hardware compatibility tests"
	$(ECHO) "  make test-quick  Quick smoke test"
	$(ECHO) "  make test-memory Test memory configurations"
	$(ECHO) "  make test-cpu    Test CPU configurations"
	$(ECHO) "  make test-storage Test storage configurations"
	$(ECHO) ""
	$(ECHO) "Install targets:"
	$(ECHO) "  make install       Install to system (PREFIX=/usr/local)"
	$(ECHO) "  make install-user  Install to ~/.local (no sudo)"
	$(ECHO) "  make uninstall     Remove from system"
	$(ECHO) "  make uninstall-user Remove from ~/.local"
	$(ECHO) "  make update-user   Rebuild and reinstall to ~/.local"
	$(ECHO) ""
	$(ECHO) "Info targets:"
	$(ECHO) "  make info     Show build configuration"
	$(ECHO) "  make help     Show this help"
	$(ECHO) ""
	$(ECHO) "\"In the beginning God created the heaven and the earth.\" — Genesis 1:1"
	$(ECHO) ""

# =============================================================================
# END BODY
# =============================================================================

# =============================================================================
# CLOSING [CLOSING]
# =============================================================================
#
# MillenniumOS Build System — Version a-02.00
#
# Build chain:
#   1. libtrit (if needed)
#   2. boot.asm → boot.bin (bootloader)
#   3. Assembly sources:
#      - entry.asm → entry.o (kernel entry)
#      - interrupt/interrupt.asm → interrupt.o (ISR/IRQ handlers)
#      - process/context_switch.asm → context_switch.o (register save/restore)
#   4. C sources (kernel subsystems):
#      - kernel.c → kernel.o (main kernel)
#      - memory/memory.c → memory.o (frame allocator, heap)
#      - interrupt/interrupt.c → interrupt.o (IDT, PIC, handlers)
#      - process/process.c → process.o (scheduler, PCB)
#      - vfs/vfs.c → vfs.o (virtual filesystem)
#   5. Link kernel.elf with libtrit
#   6. Create kernel.bin (flat binary)
#   7. Combine boot.bin + kernel.bin → millenniumos.img
#
# QEMU Scripts (aligned to 4-block structure, config-driven):
#
#   Boot Mode Ladder:
#   ┌────────────────────────────────────────────────────────────┐
#   │  run-uefi.sh        │ PRODUCTION — Full GPT + ESP image   │
#   ├────────────────────────────────────────────────────────────┤
#   │  run-uefi-simple.sh │ DEVELOPMENT — QEMU vvfat            │
#   ├────────────────────────────────────────────────────────────┤
#   │  run-uefi-debug.sh  │ DEBUG — GDB server enabled          │
#   ├────────────────────────────────────────────────────────────┤
#   │  run-bios.sh        │ LEGACY — SeaBIOS fallback           │
#   └────────────────────────────────────────────────────────────┘
#
#   Scripts load config from: scripts/qemu/config/uefi.cfg
#   Documentation at:         scripts/qemu/README.adoc
#
# Ternary concepts used throughout:
#   - Types (trit5, trit9, trit27) from libtrit
#   - 13 spare metadata values (243-255) for error states
#   - Zone semantics (void/word/tov)
#   - Hebrew cognitive states
#
# Kingdom Technology Operating System
# CreativeWorkzStudio LLC
#
# "In the beginning God created the heaven and the earth."
# — Genesis 1:1
#
# =============================================================================
# END CLOSING
# =============================================================================
