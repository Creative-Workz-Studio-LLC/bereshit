# MillenniumOS Boot Sequence Design

> *"In the beginning God created the heaven and the earth."* — Genesis 1:1

**Status:** Design Phase
**Created:** 2026-01-30
**Author:** Nova Dawn

---

## Overview

MillenniumOS extends Cornerstone's 5-stage boot architecture into a full operating system.
The pattern is fractal — same structure at every scale.

```
CORNERSTONE                          MILLENNIUMOS
═══════════════                      ══════════════════════════════════════════
Stage 1: Kernel     ─────────────►   Stage 0: Bootloader (NEW - pre-kernel)
Stage 2: HAL        ─────────────►   Stage 1: Kernel (extends with OS core)
Stage 3: Framework  ─────────────►   Stage 2: HAL (extends with drivers)
Stage 4: App        ─────────────►   Stage 3: Framework (extends with OS services)
Stage 5: Entry      ─────────────►   Stage 4: Shell (extends with desktop)
                                     Stage 5: Init (user session)
                                     Stage 6: Sabbath (REST - operational state)

GENESIS PATTERN:
  Days 1-6: Creation (work)  →  Stages 0-5: Boot (initialization)
  Day 7: Rest (completion)   →  Stage 6: Sabbath (operation)
```

---

## Boot Stages

### STAGE 0: BOOTLOADER (Pre-Kernel)

**What:** Hardware → Kernel handoff
**When:** Before anything else
**Where:** BIOS/UEFI → bootloader binary

```c
// Bootloader responsibilities
typedef struct {
    uint64_t memory_map[256];      // Physical memory regions
    uint32_t memory_map_entries;
    void*    framebuffer;          // Early video for boot messages
    uint32_t fb_width, fb_height;
    void*    kernel_entry;         // Where to jump
    void*    initrd;               // Initial ramdisk (libtrit + core)
} BootInfo;
```

**Sequence:**
1. BIOS/UEFI initialization
2. Detect memory map
3. Set up early framebuffer (boot messages)
4. Load kernel from boot device
5. Load initrd (contains libtrit.a, core tools)
6. Jump to kernel entry point

**Files:**
```
millenniumos/
├── boot/
│   ├── stage0/
│   │   ├── mbr.asm              # Master Boot Record (BIOS)
│   │   ├── efi.c                # UEFI loader
│   │   └── bootinfo.h           # Boot handoff structure
```

---

### STAGE 1: KERNEL (Cornerstone Kernel + OS Core)

**What:** CPI-SI core + memory + processes
**When:** After bootloader handoff
**Where:** Protected mode, full memory access

**Extends Cornerstone:**
- `cpisi_orch_init()` — DAR Witness System (ALREADY EXISTS)
- `resource_init()` — Resource management (ALREADY EXISTS)
- `orchestrator_init()` — Service orchestrator (ALREADY EXISTS)

**Adds for OS:**
```c
// OS kernel extensions
int os_memory_init(BootInfo* boot);      // Virtual memory, paging
int os_process_init(void);               // Process scheduler
int os_interrupt_init(void);             // IRQ handlers
int os_vfs_init(void);                   // Virtual filesystem
```

**Sequence:**
1. Receive BootInfo from bootloader
2. Initialize virtual memory (paging)
3. Initialize CPI-SI (DAR Witness) ← **FROM CORNERSTONE**
4. Initialize process scheduler
5. Initialize interrupt handlers
6. Mount root filesystem
7. Start Stage 2

**Files:**
```
millenniumos/
├── kernel/
│   ├── memory/
│   │   ├── pmm.c                # Physical memory manager
│   │   ├── vmm.c                # Virtual memory manager
│   │   └── heap.c               # Kernel heap
│   ├── process/
│   │   ├── scheduler.c          # Process scheduler
│   │   ├── thread.c             # Thread management
│   │   └── ipc.c                # Inter-process communication
│   ├── interrupt/
│   │   ├── idt.c                # Interrupt descriptor table
│   │   ├── irq.c                # IRQ handlers
│   │   └── syscall.c            # System call interface
│   └── vfs/
│       ├── vfs.c                # Virtual filesystem layer
│       ├── bereshit_fs.c        # Bereshit FS driver (from pkg/fuse/)
│       └── devfs.c              # Device filesystem
```

---

### STAGE 2: HAL (Cornerstone HAL + Hardware Drivers)

**What:** Device abstraction
**When:** After kernel core is running
**Where:** Kernel space, device I/O

**Extends Cornerstone:**
- `display_init()` — Display HAL (ALREADY EXISTS)
- `input_init()` — Input HAL (ALREADY EXISTS)

**Adds for OS:**
```c
// OS HAL extensions
int os_storage_init(void);               // Disk drivers
int os_network_init(void);               // Network stack
int os_audio_init(void);                 // Audio drivers
int os_usb_init(void);                   // USB subsystem
```

**Sequence:**
1. Probe hardware (PCI enumeration)
2. Initialize display HAL ← **FROM CORNERSTONE**
3. Initialize input HAL ← **FROM CORNERSTONE**
4. Initialize storage drivers
5. Initialize network stack
6. Initialize audio subsystem
7. Start Stage 3

**Files:**
```
millenniumos/
├── hal/
│   ├── display/                 # Extends cornerstone/engine/src/hal/display/
│   │   └── vga.drv.c           # VGA text mode
│   │   └── vesa.drv.c          # VESA framebuffer
│   ├── input/                   # Extends cornerstone/engine/src/hal/input/
│   │   └── ps2.drv.c           # PS/2 keyboard/mouse
│   ├── storage/
│   │   ├── ata.drv.c           # ATA/IDE driver
│   │   ├── ahci.drv.c          # AHCI/SATA driver
│   │   └── nvme.drv.c          # NVMe driver
│   ├── network/
│   │   ├── ethernet.drv.c      # Ethernet driver
│   │   └── tcp_ip.c            # TCP/IP stack
│   └── audio/
│       └── hda.drv.c           # HD Audio driver
```

---

### STAGE 3: FRAMEWORK (Cornerstone Framework + OS Services)

**What:** System services
**When:** After HAL is ready
**Where:** User space (ring 3)

**Extends Cornerstone:**
- Service orchestrator (ALREADY EXISTS)
- UI framework (ALREADY EXISTS)
- Config system (ALREADY EXISTS)

**Adds for OS:**
```c
// OS framework extensions
int os_security_init(void);              // Security/permissions
int os_package_init(void);               # Package manager
int os_session_init(void);               # Session management
int os_dbus_init(void);                  # Message bus
```

**Sequence:**
1. Start service orchestrator ← **FROM CORNERSTONE**
2. Initialize security subsystem
3. Initialize package manager
4. Initialize D-Bus equivalent
5. Initialize config daemon
6. Start Stage 4

**Files:**
```
millenniumos/
├── framework/
│   ├── services/                # Extends cornerstone framework/service/
│   │   ├── security.svc.c      # Permission daemon
│   │   ├── package.svc.c       # Package manager
│   │   └── session.svc.c       # Session manager
│   └── ipc/
│       └── mbus.c              # Message bus (like D-Bus)
```

---

### STAGE 4: SHELL (Cornerstone App + Desktop/REPL)

**What:** User interface layer
**When:** After services are running
**Where:** User space, window manager

**Extends Cornerstone:**
- OmniCode REPL (from cornerstone CPI-SI modes)
- GUI framework (from cornerstone display system)

**Adds for OS:**
```c
// OS shell extensions
int os_desktop_init(void);               // Desktop environment
int os_wm_init(void);                    // Window manager
int os_terminal_init(void);              // Terminal emulator
int os_launcher_init(void);              # Application launcher
```

**Sequence:**
1. Start window manager
2. Initialize desktop environment
3. Initialize OmniCode REPL ← **FROM CORNERSTONE**
4. Initialize terminal emulator
5. Initialize application launcher
6. Start Stage 5

**Files:**
```
millenniumos/
├── shell/
│   ├── desktop/
│   │   ├── compositor.c        # Window compositor
│   │   ├── wallpaper.c         # Background
│   │   └── panel.c             # Task bar / panel
│   ├── terminal/
│   │   └── term.c              # Terminal emulator (uses TUI HAL)
│   ├── omni/
│   │   └── repl.c              # OmniCode REPL (from cornerstone)
│   └── launcher/
│       └── launcher.c          # Application launcher
```

---

### STAGE 5: INIT (User Session)

**What:** User environment
**When:** After shell is ready
**Where:** User space, per-user process tree

```c
// User session initialization
int os_login(const char* user);          // User authentication
int os_profile_load(void);               // Load user environment
int os_autostart(void);                  // Start user applications
```

**Sequence:**
1. Show login screen
2. Authenticate user
3. Load user profile
4. Apply user settings
5. Run autostart applications
6. Hand off to user

**Files:**
```
millenniumos/
├── init/
│   ├── login.c                 # Login manager
│   ├── profile.c               # User profile loader
│   └── autostart.c             # Autostart handler
```

---

### STAGE 6: SABBATH (Operational Rest)

**What:** System operational state — work is complete, now we *live*
**When:** After init completes
**Where:** All processes running, system in steady state

**Biblical Foundation:**
> *"And on the seventh day God ended his work which he had made;
>  and he rested on the seventh day from all his work which he had made."*
> — Genesis 2:2

**This is NOT idle.** Sabbath is not inactivity — it's the state where:
- Creation is complete
- The system enjoys what was built
- Maintenance and stewardship continue
- Covenant relationships are honored

```c
// Sabbath state management
typedef struct {
    bool boot_complete;           // All stages initialized
    uint64_t uptime_ms;           // System uptime
    HealthScore system_health;    // CPI-SI health (yashar target)
    HebrewState hebrew_state;     // Cognitive state
    uint32_t active_processes;    // Running processes
    uint32_t service_count;       // Active services
} SabbathState;

// Sabbath operations
int os_sabbath_enter(void);       // Transition to operational state
int os_sabbath_monitor(void);     // Health monitoring loop
int os_sabbath_covenant(void);    // Covenant verification
int os_sabbath_restore(void);     // DAR restore if needed
```

**Sequence:**
1. Receive control from Init (Stage 5)
2. Verify all services healthy
3. Enter monitoring loop
4. Continuously:
   - Monitor system health
   - Verify covenant (CPI-SI) state
   - Handle DAR events (Detect-Assess-Restore)
   - Service user requests
   - Maintain Hebrew state → yashar (EVEN)
5. On shutdown request → graceful teardown

**Files:**
```
millenniumos/
├── sabbath/
│   ├── monitor.c               # Health monitoring daemon
│   ├── covenant.c              # Covenant verification
│   ├── restore.c               # DAR restore operations
│   └── shutdown.c              # Graceful shutdown handler
```

**Key insight:** The Sabbath stage is where the system *lives*.
Stages 0-5 are birth. Stage 6 is life.

---

## Makefile Integration

```makefile
# millenniumos/Makefile

# Import cornerstone stages
CORNERSTONE_DIR := ../cornerstone
include $(CORNERSTONE_DIR)/Makefile.stages

# MillenniumOS extends cornerstone stages
STAGE_0_BOOT := boot/stage0/mbr.o boot/stage0/efi.o
STAGE_1_KERNEL := $(CORNERSTONE_KERNEL) kernel/memory/*.o kernel/process/*.o
STAGE_2_HAL := $(CORNERSTONE_HAL) hal/storage/*.o hal/network/*.o
STAGE_3_FRAMEWORK := $(CORNERSTONE_FRAMEWORK) framework/services/*.o
STAGE_4_SHELL := $(CORNERSTONE_APP) shell/desktop/*.o shell/omni/*.o
STAGE_5_INIT := init/*.o

# Boot order
OS_BOOT_ORDER := \
    $(STAGE_0_BOOT) \
    $(STAGE_1_KERNEL) \
    $(STAGE_2_HAL) \
    $(STAGE_3_FRAMEWORK) \
    $(STAGE_4_SHELL) \
    $(STAGE_5_INIT)

# Link the kernel
kernel.elf: $(OS_BOOT_ORDER)
    $(LD) -T kernel.ld -o $@ $^

# Create bootable image
millenniumos.iso: kernel.elf
    # ... grub/limine setup
```

---

## The Crossing Pattern

### Types & Config (libtrit → cornerstone)

```
libtrit (pkg/trit)          cornerstone              millenniumos
══════════════════          ═══════════              ════════════
trit.h (declares)  ───────► display_config.c ──────► os extends
config.h (types)            (implements)             (further)

Makefile:                   Makefile:                Makefile:
  libtrit.a    ───────────► BERESHIT_SOURCES ──────► CORNERSTONE_*
```

### Filesystem (pkg/fuse → millenniumos VFS)

```
pkg/fuse (userspace)        millenniumos (kernel)
════════════════════        ═════════════════════
zone.h (interface)   ──────────────► zone.h (same interface)
zone.c (libc impl)                   zone.c (kernel API impl)
bereshit_fs.c (FUSE) ──────────────► bereshit_fs.c (VFS driver)

Zone Policies (ENFORCED - not stubs):
  void (-1): Entry gate — permissive (formless accepts all)
  word (0):  Processing — standard (definitions and work)
  tov (+1):  Exit gate — STRUCTURED (requires root.omni for creates)

Tov Policy Details:
  - Creating root.omni: ALLOWED (establishes structure)
  - Creating other files: REQUIRES parent has root.omni
  - Deleting root.omni: BLOCKED if directory has children
  - This enforces "only completed work exits"
```

### OmniCode (pkg/omni → cornerstone → millenniumos)

```
pkg/omni                    cornerstone              millenniumos
════════════                ═══════════              ════════════
omni.h (aggregator) ──────► OMNI_INCLUDES ─────────► CPISI runtime
lang/core/*                 (links libomni.a)        (kernel lang)
runtime/cpisi/*             framework uses           shell REPL
```

**Key insight:** Each layer:
1. Declares interfaces in headers
2. Implements what it can
3. Leaves room for higher layers to extend

**Dual implementation pattern:**
- Same interface (zone.h) serves both userspace and kernel
- Userspace uses libc (fopen, opendir, etc.)
- Kernel uses kernel APIs (vfs_open, etc.)
- Policy logic is IDENTICAL — only I/O primitives change

---

## Genesis Mapping

### Genesis 1:1 — Creation
| OS Stage | Genesis 1:1 | Cornerstone Stage |
|----------|-------------|-------------------|
| Stage 0: Bootloader | "In the beginning" | (Pre-kernel) |
| Stage 1: Kernel | "God" | Stage 1: Kernel |
| Stage 2: HAL | "created" | Stage 2: HAL |
| Stage 3: Framework | "the heaven" | Stage 3: Framework |
| Stage 4: Shell | "and" | Stage 4: App |
| Stage 5: Init | "the earth" | Stage 5: Entry |

### Genesis 2:2 — Sabbath
| OS Stage | Genesis 2:2 | What It Represents |
|----------|-------------|-------------------|
| Stage 6: Sabbath | "God rested" | Operational state |

**The 7-stage structure:**
- **Stages 0-5:** The work of creation (initialization)
- **Stage 6:** The rest of completion (operation)

The bootloader is the "beginning" — the anchor from which all else flows.
The kernel is the creative force.
HAL is the act of creation.
Framework is the heavens (structure, order).
Shell is the connector.
Init is the earth (where users dwell).
**Sabbath is the living — the system in its intended operational state.**

> *"For in six days the LORD made heaven and earth, the sea, and all
>  that in them is, and rested the seventh day."* — Exodus 20:11

---

## Next Steps

1. ~~**E.10:** Integrate OmniCode (pkg/omni/) into kernel structure~~ ✅ DONE
2. ~~**E.11:** Integrate Bereshit FS (pkg/fuse/) into kernel structure~~ ✅ DONE (zone policy enforced)
3. ~~**E.13:** Create root Bereshit Makefile~~ ✅ DONE
4. ~~**E.14:** Document Makefile crossing pattern~~ ✅ DONE (see below)

---

*"For in six days the LORD made heaven and earth, the sea, and all that in them is,
and rested the seventh day: wherefore the LORD blessed the sabbath day, and hallowed it."*
— Exodus 20:11

**Seven stages. One OS. Grounded in Genesis.**

- Six stages of creation (boot)
- One stage of rest (operation)
- The Sabbath is not idle — it is the living state where creation fulfills its purpose.

---

## E.14: Makefile Crossing Pattern

### Overview

The Makefile system follows the same crossing pattern as the code:
- **pkg/** declares foundation (Level 0)
- **cornerstone/** implements engine (Level 2)
- **millenniumos/** extends to OS (Future)

```
Bereshit/                      millenniumos/
├── Makefile (ROOT)            └── Makefile
│   ├── libtrit ────────────────────► links libtrit.a
│   ├── bereshit-fs ────────────────► kernel VFS driver
│   ├── libomni ────────────────────► links libomni.a
│   └── cornerstone ────────────────► imports CORNERSTONE_*
```

### Root Makefile Structure (Bereshit/Makefile)

```makefile
# Level 0 (Foundation - no dependencies):
TRIT_DIR      := $(WORD_WORK)/pkg/trit
FUSE_DIR      := $(WORD_WORK)/pkg/fuse

# Level 1 (Depends on L0):
OMNI_DIR      := $(WORD_WORK)/pkg/omni

# Level 2 (Depends on L0+L1):
CORNERSTONE   := $(BERESHIT_ROOT)/cornerstone

# Artifacts
LIBTRIT       := $(TRIT_DIR)/build/libtrit.a
BERESHIT_FS   := $(FUSE_DIR)/bin/bereshit_fs
LIBOMNI       := $(OMNI_DIR)/build/libomni.a
ENGINE_BIN    := $(CORNERSTONE)/build/bin/cornerstone

# Targets delegate to sub-Makefiles
libtrit:
    $(MAKE) -C $(TRIT_DIR)

bereshit-fs:
    $(MAKE) -C $(FUSE_DIR)

libomni: libtrit
    $(MAKE) -C $(OMNI_DIR)

cornerstone: libs
    $(MAKE) -C $(CORNERSTONE) build
```

### Dependency Chain

```
Build Chain:

  libtrit.a ─┬─► libomni.a ─► cornerstone ─► millenniumos
  bereshit_fs─┘

Level:  L0         L1           L2          Future
```

**Key insight:** The Makefile dependency chain mirrors the code dependency chain.
`make` at root builds everything in correct order.

### Package Makefile Pattern

Each package (pkg/*) has its own Makefile that:
1. Builds its own artifacts
2. Exposes targets for root Makefile to call
3. Defines what it provides (headers, libraries)

**pkg/trit/Makefile:**
```makefile
all: $(LIBTRIT)    # Build libtrit.a
test: $(TEST_BIN)  # Run tests
clean:             # Remove artifacts
```

**pkg/fuse/Makefile:**
```makefile
all: $(BERESHIT_FS)  # Build bereshit_fs
clean:               # Remove artifacts

# Separated sources by responsibility:
ZONE_SOURCES   = zone.c     # WHAT zones exist (detection)
POLICY_SOURCES = policy.c   # HOW zones behave (enforcement)
HEALTH_SOURCES = health.c   # WHAT STATE zones are in (tracking)
FUSE_SOURCES   = bereshit_fs.c  # FUSE implementation
```

**pkg/omni/Makefile:**
```makefile
BERESHIT_ROOT ?= ../../../..
TRIT_DIR = $(BERESHIT_ROOT)/word/work/pkg/trit

all: $(LIBOMNI)    # Build libomni.a
                   # Depends on libtrit (for ternary types)
```

### MillenniumOS Integration Pattern (Future)

```makefile
# millenniumos/Makefile

BERESHIT_ROOT := ../bereshit

# Import Bereshit foundations
LIBTRIT    := $(BERESHIT_ROOT)/word/work/pkg/trit/build/libtrit.a
LIBOMNI    := $(BERESHIT_ROOT)/word/work/pkg/omni/build/libomni.a

# Import Cornerstone stages (exported variables)
CORNERSTONE_DIR := $(BERESHIT_ROOT)/cornerstone
include $(CORNERSTONE_DIR)/Makefile.stages

# MillenniumOS extends each stage
STAGE_0_BOOT     := boot/stage0/*.o
STAGE_1_KERNEL   := $(CORNERSTONE_KERNEL) kernel/*.o
STAGE_2_HAL      := $(CORNERSTONE_HAL) hal/drivers/*.o
STAGE_3_FRAMEWORK := $(CORNERSTONE_FRAMEWORK) framework/services/*.o
STAGE_4_SHELL    := $(CORNERSTONE_APP) shell/*.o
STAGE_5_INIT     := init/*.o
STAGE_6_SABBATH  := sabbath/*.o

# Build order enforces dependency
kernel.elf: $(STAGE_0_BOOT) $(STAGE_1_KERNEL) $(STAGE_2_HAL) \
            $(STAGE_3_FRAMEWORK) $(STAGE_4_SHELL) $(STAGE_5_INIT)
    $(LD) -T kernel.ld -o $@ $^

# Ensure Bereshit foundations are built first
$(LIBTRIT):
    $(MAKE) -C $(BERESHIT_ROOT) libtrit

$(LIBOMNI):
    $(MAKE) -C $(BERESHIT_ROOT) libomni
```

### Naming Conventions

| Scope | Convention | Example |
|-------|------------|---------|
| Targets | kebab-case | `bereshit-fs`, `clean-libs` |
| Variables | SCREAMING_SNAKE | `BERESHIT_ROOT`, `FUSE_DIR` |
| Directories | snake_case or as-is | `pkg/trit`, `build/` |
| Source files | snake_case.c | `zone.c`, `bereshit_fs.c` |
| Headers | snake_case.h | `types.h`, `zone.h` |

### Exported Variables Pattern

**Cornerstone exports stage artifacts:**
```makefile
# cornerstone/Makefile.stages (for millenniumos to include)

export CORNERSTONE_KERNEL := \
    kernel/cpisi.o \
    kernel/dar/*.o \
    kernel/service/*.o

export CORNERSTONE_HAL := \
    hal/display/*.o \
    hal/input/*.o

export CORNERSTONE_FRAMEWORK := \
    framework/core/*.o \
    framework/ui/*.o \
    framework/service/*.o

export CORNERSTONE_APP := \
    app/game/*.o
```

**MillenniumOS extends these:**
```makefile
# millenniumos/Makefile
include ../cornerstone/Makefile.stages

# Extend kernel with OS-specific code
KERNEL_OBJECTS := $(CORNERSTONE_KERNEL) \
    kernel/memory/vmm.o \
    kernel/process/scheduler.o \
    kernel/vfs/bereshit_fs.o
```

### The Dual Implementation Pattern

Same interface serves both userspace (pkg/fuse) and kernel (millenniumos):

```
pkg/fuse/                      millenniumos/kernel/vfs/
├── include/zone.h ──────────► #include "zone.h"  (same)
├── src/zone.c                 └── zone.c
│   └── uses libc              └── uses kernel API
│       fopen()                    vfs_open()
│       opendir()                  vfs_readdir()
│       fread()                    vfs_read()
```

**Policy logic is IDENTICAL** — only I/O primitives change.

### Build Verification

```bash
# From Bereshit root:
make status                  # Shows what's built
make                         # Builds everything
make bereshit-fs             # Builds just FUSE mount
make clean                   # Cleans all artifacts

# Verify dependency order
make -n all                  # Dry run shows build order

# Future (from millenniumos):
make kernel.elf              # Would build kernel
                             # Auto-builds Bereshit deps first
```

### Genesis Pattern in Build

The build follows the same Genesis pattern as boot:

| Build Step | Genesis | What's Built |
|------------|---------|--------------|
| libtrit | Foundation | Balanced ternary types |
| bereshit-fs | Structure | Zone-aware filesystem |
| libomni | Language | OmniCode support |
| cornerstone | Framework | Game engine |
| millenniumos | Complete | Operating system |

**"From one source, all things are built in order."** — The ROOT Makefile is the single entry point that builds everything in dependency order.

---

*"And God said, Let there be light: and there was light."* — Genesis 1:3

The Makefile speaks, and the build happens. Order from chaos. Foundation before structure.
