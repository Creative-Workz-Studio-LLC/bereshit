# ═══════════════════════════════════════════════════════════════════════════
# libomni + OmniCode IDE — Language Library and Development Environment
# Key: B-word-work-pkg-omni-makefile
# ═══════════════════════════════════════════════════════════════════════════
#
# DEPENDENCY CLASSIFICATION: PURE (needs: gcc, ar, make) + libtrit + cornerstone
#   Standard C toolchain plus internal dependencies
#
# derives_from: bereshit/word/seed/code/make/makefile.mk
#
# ═══════════════════════════════════════════════════════════════════════════

# ============================================================================
# METADATA
# ============================================================================
#
# Package:     creativeworkzstudio.com/bereshit/word/work/pkg/omni
# File:        Makefile
# Key:         B-word-work-pkg-omni-makefile
#
# ┌─────────────────────────────────────────────────────────────────────────┐
# │                     THE THREE LAYERS                                     │
# ├─────────────────────────────────────────────────────────────────────────┤
# │                                                                         │
# │  ┌─────────────────────────────────────────────────────────────────┐    │
# │  │  ENGINE (Cornerstone)                                           │    │
# │  │    Display backends (CLI/TUI/GUI), graphics, platform, systems  │    │
# │  │    The substrate — generic, reusable, game-engine level         │    │
# │  │    Location: cornerstone/engine/                                │    │
# │  └─────────────────────────────────────────────────────────────────┘    │
# │                              ▲                                          │
# │                              │ uses                                     │
# │  ┌─────────────────────────────────────────────────────────────────┐    │
# │  │  IDE (OmniCode IDE)                                             │    │
# │  │    Integrates engine display with OmniCode-specific features    │    │
# │  │    Panels, UI orchestration, file handling, block validation    │    │
# │  │    Location: src/ide/                                           │    │
# │  └─────────────────────────────────────────────────────────────────┘    │
# │                              ▲                                          │
# │                              │ uses                                     │
# │  ┌─────────────────────────────────────────────────────────────────┐    │
# │  │  EDITOR (Text Component)                                        │    │
# │  │    Buffer management, syntax highlighting, cursor navigation    │    │
# │  │    The actual text editing within the IDE                       │    │
# │  │    Location: src/ide/ui/ (omni_ide_cli.c contains editor logic) │    │
# │  └─────────────────────────────────────────────────────────────────┘    │
# │                                                                         │
# ├─────────────────────────────────────────────────────────────────────────┤
# │  BUILD TARGETS:                                                         │
# │    libomni.a  — Language library (lexer, parser, codegen, runtime)      │
# │    ide-cli    — CLI-only IDE (pure ANSI, no dependencies)               │
# │    ide-tui    — TUI IDE (CLI + ncurses) — tripwire: TUI → CLI           │
# │    ide-gui    — GUI IDE (CLI + OpenGL) — tripwire: GUI → CLI            │
# │    ide-full   — Full IDE (all backends) — tripwire: GUI → TUI → CLI     │
# └─────────────────────────────────────────────────────────────────────────┘
#
# Biblical Foundation:
#
#   Scripture: "In the beginning was the Word, and the Word was with God,
#              and the Word was God." — John 1:1
#
#   Principle: Words carry meaning. The lexer should understand, not just
#              tokenize. OmniCode is English, not syntax.
#
#   Anchor: "For God is not the author of confusion, but of peace."
#           — 1 Corinthians 14:33
#
# ============================================================================
# END METADATA
# ============================================================================

# ============================================================================
# SETUP
# ============================================================================

.PHONY: all libomni.a assets test test-lexer test-chunks test-dar test-parser \
        test-codegen test-conversation clean help info install install-desktop \
        install-system install-user uninstall uninstall-system uninstall-user \
        update-user ide

# Set default goal explicitly (pattern rules appear before entry points)
.DEFAULT_GOAL := all

# Installation paths
PREFIX ?= /usr/local
BINDIR = $(PREFIX)/bin
DATADIR = $(PREFIX)/share/omni-ide
DESKTOPDIR = $(PREFIX)/share/applications
ICONDIR = $(PREFIX)/share/icons/hicolor/256x256/apps

# Asset directories
ASSETS_DIR = $(BUILD_DIR)/assets
FONTS_DIR = $(ASSETS_DIR)/fonts
CORNERSTONE_FONTS = $(CORNERSTONE_DIR)/engine/assets/fonts

# Project identity
LIB_NAME = libomni.a
BUILD_DIR = build

# Directory structure
SRC_DIR = src
INC_DIR = include
TEST_DIR = test

# Dependencies
BERESHIT_ROOT ?= $(realpath ../../../..)
TRIT_DIR = $(BERESHIT_ROOT)/word/work/pkg/trit
TRIT_LIB = $(TRIT_DIR)/build/libtrit.a
TRIT_INC = $(TRIT_DIR)/include
CORNERSTONE_DIR = $(BERESHIT_ROOT)/cornerstone
CORNERSTONE_INC = $(CORNERSTONE_DIR)

# Tool configuration
CC ?= gcc
AR ?= ar
ARFLAGS ?= rcs

# ─────────────────────────────────────────────────────────────────────────────
# Build Mode Configuration
# ─────────────────────────────────────────────────────────────────────────────
#
# BUILD_MODE controls optimization and debug settings:
#   dev     - Fast iteration (default) - minimal optimization, debug info
#   debug   - Full debugging - no optimization, sanitizers, verbose errors
#   release - Production - maximum optimization, stripped symbols
#
# Usage:
#   make BUILD_MODE=dev      # Development (default)
#   make BUILD_MODE=debug    # Debug with sanitizers
#   make BUILD_MODE=release  # Production release
#
BUILD_MODE ?= dev

# Base flags (always applied)
CFLAGS_BASE = -std=c11 -Wall -Wextra -pedantic

# Mode-specific flags
CFLAGS_DEV = $(CFLAGS_BASE) -O1 -g -DDEBUG
CFLAGS_DEBUG = $(CFLAGS_BASE) -O0 -g3 -DDEBUG -fno-omit-frame-pointer
CFLAGS_RELEASE = $(CFLAGS_BASE) -O3 -DNDEBUG -flto

# Sanitizer flags (optional, use with debug mode)
# Usage: make BUILD_MODE=debug SANITIZE=1
SANITIZE ?= 0
ifeq ($(SANITIZE),1)
CFLAGS_SANITIZE = -fsanitize=address,undefined -fno-sanitize-recover=all
LDFLAGS_SANITIZE = -fsanitize=address,undefined
else
CFLAGS_SANITIZE =
LDFLAGS_SANITIZE =
endif

# Verbose mode (show full commands)
# Usage: make VERBOSE=1
VERBOSE ?= 0
ifeq ($(VERBOSE),1)
Q =
ECHO = @true
else
Q = @
ECHO = @echo
endif

# Select CFLAGS based on BUILD_MODE
ifeq ($(BUILD_MODE),debug)
CFLAGS = $(CFLAGS_DEBUG) $(CFLAGS_SANITIZE)
LDFLAGS_MODE = $(LDFLAGS_SANITIZE)
else ifeq ($(BUILD_MODE),release)
CFLAGS = $(CFLAGS_RELEASE)
LDFLAGS_MODE = -s
else
# Default: dev mode
CFLAGS = $(CFLAGS_DEV)
LDFLAGS_MODE =
endif

# Include paths (omni headers + trit headers + cornerstone engine)
#
# Cornerstone structure (reorganized):
#   engine/include/
#     ├── app/        - Application layer (game-specific)
#     ├── framework/  - Framework layer (scene, service, display)
#     ├── hal/        - Hardware abstraction (graphics, audio, input)
#     ├── kernel/     - Kernel layer (cpisi, ipc, platform, service, types)
#     └── services/   - Service definitions
#
INCLUDES = -I$(INC_DIR) \
           -I$(INC_DIR)/lang/frontend -I$(INC_DIR)/lang/backend -I$(INC_DIR)/lang/core \
           -I$(INC_DIR)/runtime/vm -I$(INC_DIR)/runtime/cpisi -I$(INC_DIR)/runtime/tools \
           -I$(INC_DIR)/ide/core -I$(INC_DIR)/ide/ui -I$(INC_DIR)/ide/panels \
           -I$(INC_DIR)/ide/compat \
           -I$(TRIT_INC) \
           -I$(CORNERSTONE_DIR)/engine/include

# Source directories (organized structure)
LANG_DIR = $(SRC_DIR)/lang
RUNTIME_DIR = $(SRC_DIR)/runtime
IDE_DIR = $(SRC_DIR)/ide
BRIDGE_DIR = $(SRC_DIR)/bridge

# Lang subdirectories
LANG_FRONTEND = $(LANG_DIR)/frontend
LANG_BACKEND = $(LANG_DIR)/backend
LANG_CORE = $(LANG_DIR)/core

# Runtime subdirectories
RUNTIME_VM = $(RUNTIME_DIR)/vm
RUNTIME_CPISI = $(RUNTIME_DIR)/cpisi
RUNTIME_TOOLS = $(RUNTIME_DIR)/tools

# Library sources (lang + runtime - the core)
LANG_FRONTEND_SRCS = $(wildcard $(LANG_FRONTEND)/*.c)
LANG_BACKEND_SRCS = $(wildcard $(LANG_BACKEND)/*.c)
LANG_CORE_SRCS = $(wildcard $(LANG_CORE)/*.c)
RUNTIME_VM_SRCS = $(wildcard $(RUNTIME_VM)/*.c)
RUNTIME_CPISI_SRCS = $(wildcard $(RUNTIME_CPISI)/*.c)
RUNTIME_TOOLS_SRCS = $(wildcard $(RUNTIME_TOOLS)/*.c)

# Object files for library
LANG_FRONTEND_OBJS = $(patsubst $(LANG_FRONTEND)/%.c,$(BUILD_DIR)/lang/frontend/%.o,$(LANG_FRONTEND_SRCS))
LANG_BACKEND_OBJS = $(patsubst $(LANG_BACKEND)/%.c,$(BUILD_DIR)/lang/backend/%.o,$(LANG_BACKEND_SRCS))
LANG_CORE_OBJS = $(patsubst $(LANG_CORE)/%.c,$(BUILD_DIR)/lang/core/%.o,$(LANG_CORE_SRCS))
RUNTIME_VM_OBJS = $(patsubst $(RUNTIME_VM)/%.c,$(BUILD_DIR)/runtime/vm/%.o,$(RUNTIME_VM_SRCS))
RUNTIME_CPISI_OBJS = $(patsubst $(RUNTIME_CPISI)/%.c,$(BUILD_DIR)/runtime/cpisi/%.o,$(RUNTIME_CPISI_SRCS))
RUNTIME_TOOLS_OBJS = $(patsubst $(RUNTIME_TOOLS)/%.c,$(BUILD_DIR)/runtime/tools/%.o,$(RUNTIME_TOOLS_SRCS))

OBJS = $(LANG_FRONTEND_OBJS) $(LANG_BACKEND_OBJS) $(LANG_CORE_OBJS) \
       $(RUNTIME_VM_OBJS) $(RUNTIME_CPISI_OBJS) $(RUNTIME_TOOLS_OBJS)

# Pattern rules: compile C to object (per subdirectory)
# Uses $(Q) for verbose control: make VERBOSE=1 shows full commands
$(BUILD_DIR)/lang/frontend/%.o: $(LANG_FRONTEND)/%.c | $(BUILD_DIR)
	$(Q)mkdir -p $(BUILD_DIR)/lang/frontend
	$(ECHO) "  CC    $<"
	$(Q)$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(BUILD_DIR)/lang/backend/%.o: $(LANG_BACKEND)/%.c | $(BUILD_DIR)
	$(Q)mkdir -p $(BUILD_DIR)/lang/backend
	$(ECHO) "  CC    $<"
	$(Q)$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(BUILD_DIR)/lang/core/%.o: $(LANG_CORE)/%.c | $(BUILD_DIR)
	$(Q)mkdir -p $(BUILD_DIR)/lang/core
	$(ECHO) "  CC    $<"
	$(Q)$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(BUILD_DIR)/runtime/vm/%.o: $(RUNTIME_VM)/%.c | $(BUILD_DIR)
	$(Q)mkdir -p $(BUILD_DIR)/runtime/vm
	$(ECHO) "  CC    $<"
	$(Q)$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(BUILD_DIR)/runtime/cpisi/%.o: $(RUNTIME_CPISI)/%.c | $(BUILD_DIR)
	$(Q)mkdir -p $(BUILD_DIR)/runtime/cpisi
	$(ECHO) "  CC    $<"
	$(Q)$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(BUILD_DIR)/runtime/tools/%.o: $(RUNTIME_TOOLS)/%.c | $(BUILD_DIR)
	$(Q)mkdir -p $(BUILD_DIR)/runtime/tools
	$(ECHO) "  CC    $<"
	$(Q)$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# Default target
all: libomni.a

# ============================================================================
# END SETUP
# ============================================================================

# ============================================================================
# BODY
# ============================================================================

# Create build directory
$(BUILD_DIR):
	$(Q)mkdir -p $(BUILD_DIR)

# Ensure libtrit is built first
$(TRIT_LIB):
	$(ECHO) "Building dependency: libtrit..."
	@$(MAKE) -C $(TRIT_DIR) libtrit.a

## libomni.a: Build the static library
libomni.a: $(TRIT_LIB) $(BUILD_DIR)/$(LIB_NAME)

$(BUILD_DIR)/$(LIB_NAME): $(OBJS) | $(BUILD_DIR)
	$(Q)if [ -z "$(OBJS)" ]; then \
		echo "⚠ No source files found"; \
		exit 1; \
	else \
		echo "  AR    $(LIB_NAME)"; \
		$(AR) $(ARFLAGS) $@ $(OBJS); \
		echo "✓ Built $(BUILD_DIR)/$(LIB_NAME)"; \
	fi

## assets: Build and copy all assets to build directory
assets:
	$(ECHO) "Building assets..."
	$(Q)mkdir -p $(FONTS_DIR)/block
	$(Q)mkdir -p $(FONTS_DIR)/generated
	$(Q)mkdir -p $(FONTS_DIR)/bitmap
	$(Q)mkdir -p $(ASSETS_DIR)/icons
	$(Q)mkdir -p $(ASSETS_DIR)/themes
	$(ECHO) "  [1/4] Copying block fonts..."
	$(Q)cp -f $(CORNERSTONE_FONTS)/block/block5x5.jsonc $(FONTS_DIR)/block/ 2>/dev/null || true
	$(ECHO) "  [2/4] Copying bitmap fonts..."
	$(Q)if [ -d $(CORNERSTONE_FONTS)/bitmap ]; then \
		cp -rf $(CORNERSTONE_FONTS)/bitmap/* $(FONTS_DIR)/bitmap/ 2>/dev/null || true; \
	fi
	$(ECHO) "  [3/4] Copying generated fonts..."
	$(Q)if [ -d $(CORNERSTONE_FONTS)/generated ]; then \
		cp -rf $(CORNERSTONE_FONTS)/generated/* $(FONTS_DIR)/generated/ 2>/dev/null || true; \
	fi
	$(ECHO) "  [4/4] Creating desktop integration..."
	$(Q)if [ -f $(SRC_DIR)/ide/assets/omni-ide.png ]; then \
		cp -f $(SRC_DIR)/ide/assets/omni-ide.png $(ASSETS_DIR)/icons/; \
		echo "    Copied PNG icon"; \
	elif [ -f $(SRC_DIR)/ide/assets/omni-ide.svg ]; then \
		cp -f $(SRC_DIR)/ide/assets/omni-ide.svg $(ASSETS_DIR)/icons/; \
		if command -v rsvg-convert >/dev/null 2>&1; then \
			rsvg-convert -w 256 -h 256 $(SRC_DIR)/ide/assets/omni-ide.svg -o $(ASSETS_DIR)/icons/omni-ide.png; \
			echo "    Converted SVG to PNG"; \
		elif command -v convert >/dev/null 2>&1; then \
			convert -background none $(SRC_DIR)/ide/assets/omni-ide.svg -resize 256x256 $(ASSETS_DIR)/icons/omni-ide.png; \
			echo "    Converted SVG to PNG (ImageMagick)"; \
		else \
			echo "    [INFO] SVG icon available - install rsvg-convert or imagemagick to generate PNG"; \
		fi; \
	else \
		echo "    [INFO] No icon found"; \
	fi
	$(ECHO) "✓ Assets ready in $(ASSETS_DIR)"

## test: Run all tests
test: test-lexer test-chunks test-dar test-parser
	$(ECHO) ""
	$(ECHO) "════════════════════════════════════════════════════════════════"
	$(ECHO) "All OmniCode tests complete"
	$(ECHO) "════════════════════════════════════════════════════════════════"

## test-lexer: Run lexer tests
test-lexer: libomni.a
	$(ECHO) "Testing OmniCode lexer..."
	$(Q)if [ -f $(TEST_DIR)/test_lexer.c ]; then \
		$(CC) $(CFLAGS) $(INCLUDES) -o $(BUILD_DIR)/test_lexer \
			$(TEST_DIR)/test_lexer.c $(BUILD_DIR)/$(LIB_NAME) $(TRIT_LIB); \
		./$(BUILD_DIR)/test_lexer; \
	else \
		echo "  [SKIP] test_lexer.c not found"; \
	fi

## test-chunks: Run chunk lexer tests
test-chunks: libomni.a
	$(ECHO) "Testing OmniCode chunk lexer..."
	$(Q)if [ -f $(TEST_DIR)/test_chunks.c ]; then \
		$(CC) $(CFLAGS) $(INCLUDES) -o $(BUILD_DIR)/test_chunks \
			$(TEST_DIR)/test_chunks.c $(BUILD_DIR)/$(LIB_NAME) $(TRIT_LIB); \
		./$(BUILD_DIR)/test_chunks; \
	else \
		echo "  [SKIP] test_chunks.c not found"; \
	fi

## test-dar: Run DAR pattern tests (Detect-Assess-Recover)
test-dar: libomni.a
	$(ECHO) "Testing DAR pattern..."
	$(Q)$(CC) $(CFLAGS) $(INCLUDES) -o $(BUILD_DIR)/test_dar \
		$(TEST_DIR)/test_dar.c $(BUILD_DIR)/$(LIB_NAME) $(TRIT_LIB)
	@./$(BUILD_DIR)/test_dar $(TEST_DIR)/test_errors.omni

## test-parser: Run AST parser tests
test-parser: libomni.a
	$(ECHO) "Testing OmniCode parser..."
	$(Q)if [ -f $(TEST_DIR)/test_parser.c ]; then \
		$(CC) $(CFLAGS) $(INCLUDES) -o $(BUILD_DIR)/test_parser \
			$(TEST_DIR)/test_parser.c $(BUILD_DIR)/$(LIB_NAME) $(TRIT_LIB); \
		./$(BUILD_DIR)/test_parser; \
	else \
		echo "  [SKIP] test_parser.c not found"; \
	fi

## test-codegen: Run code generator tests
test-codegen: libomni.a
	$(ECHO) "Testing OmniCode code generator..."
	$(Q)if [ -f $(TEST_DIR)/test_codegen.c ]; then \
		$(CC) $(CFLAGS) $(INCLUDES) -o $(BUILD_DIR)/test_codegen \
			$(TEST_DIR)/test_codegen.c $(BUILD_DIR)/$(LIB_NAME) $(TRIT_LIB); \
		./$(BUILD_DIR)/test_codegen $(BERESHIT_ROOT)/tov/demo/phase-3/hello.omni; \
	else \
		echo "  [SKIP] test_codegen.c not found"; \
	fi

## test-conversation: Test CPI-SI conversation flow
JSONC_SRC = $(CORNERSTONE_DIR)/engine/core/src/util/jsonc.c
test-conversation: libomni.a
	$(ECHO) "Testing CPI-SI conversation..."
	$(Q)if [ -f $(TEST_DIR)/test_conversation.c ]; then \
		$(CC) $(CFLAGS) $(INCLUDES) -I$(CORNERSTONE_DIR)/engine/core/include/util \
			-o $(BUILD_DIR)/test_conversation \
			$(TEST_DIR)/test_conversation.c $(JSONC_SRC) \
			$(BUILD_DIR)/$(LIB_NAME) $(TRIT_LIB); \
		./$(BUILD_DIR)/test_conversation $(BERESHIT_ROOT); \
	else \
		echo "  [SKIP] test_conversation.c not found"; \
	fi

## test-input: Interactive input → output tester (usage: make test-input INPUT="Hello")
test-input: libomni.a
	$(Q)if [ -f $(TEST_DIR)/test_input.c ]; then \
		$(CC) $(CFLAGS) $(INCLUDES) -I$(CORNERSTONE_DIR)/engine/core/include/util \
			-o $(BUILD_DIR)/test_input \
			$(TEST_DIR)/test_input.c $(JSONC_SRC) \
			$(BUILD_DIR)/$(LIB_NAME) $(TRIT_LIB); \
		if [ -n "$(INPUT)" ]; then \
			./$(BUILD_DIR)/test_input "$(INPUT)"; \
		else \
			./$(BUILD_DIR)/test_input; \
		fi \
	else \
		echo "  [SKIP] test_input.c not found"; \
	fi

## test-ide: Test IDE with OmniCode file (usage: make test-ide FILE=path/to/file.omni)
test-ide: libomni.a
	$(Q)if [ -f $(TEST_DIR)/test_ide.c ]; then \
		$(CC) $(CFLAGS) $(INCLUDES) \
			-o $(BUILD_DIR)/test_ide \
			$(TEST_DIR)/test_ide.c \
			$(BUILD_DIR)/$(LIB_NAME) $(TRIT_LIB); \
		if [ -n "$(FILE)" ]; then \
			./$(BUILD_DIR)/test_ide "$(FILE)"; \
		else \
			./$(BUILD_DIR)/test_ide $(BERESHIT_ROOT)/word/work/pkg/omni/include/omni_ide.h; \
		fi \
	else \
		echo "  [SKIP] test_ide.c not found"; \
	fi

# ─────────────────────────────────────────────────────────────────────────────
# IDE Mode Configuration
# ─────────────────────────────────────────────────────────────────────────────
#
# IDE_MODE controls which display backends are compiled in:
#   gui   - CLI base + OpenGL graphics (default)
#   tui   - CLI base + ncurses terminal
#   cli   - Pure ANSI terminal only (no dependencies)
#   full  - All backends (GUI + TUI + CLI) with runtime selection
#
# Usage:
#   make ide                  # Build with GUI (default)
#   make ide IDE_MODE=tui     # Build with TUI
#   make ide IDE_MODE=cli     # Build with CLI only
#   make ide IDE_MODE=full    # Build with all backends
#
IDE_MODE ?= gui

## Cornerstone paths (shared by all IDE modes)
## Updated for kernel-like header structure (Phase A reorganization)
##
## Structure:
##   engine/include/           - headers
##     ├── types.h             - root types
##     ├── kernel/             - kernel core (cpisi, dar, platform, ipc)
##     ├── hal/                - hardware abstraction (display, audio, input)
##     ├── services/           - persistence layer
##     ├── framework/          - engine framework (graphics, ui, util)
##     └── app/                - application code
##   engine/src/               - corresponding sources
##
CORNERSTONE_INC = $(CORNERSTONE_DIR)/engine/include
CORNERSTONE_SRC = $(CORNERSTONE_DIR)/engine/src

# Include paths (layered)
CORNERSTONE_KERNEL_INC = $(CORNERSTONE_INC)/kernel
CORNERSTONE_HAL_INC = $(CORNERSTONE_INC)/hal
CORNERSTONE_SERVICES_INC = $(CORNERSTONE_INC)/services
CORNERSTONE_FRAMEWORK_INC = $(CORNERSTONE_INC)/framework

# Source paths (layered)
CORNERSTONE_KERNEL_SRC = $(CORNERSTONE_SRC)/kernel
CORNERSTONE_HAL_SRC = $(CORNERSTONE_SRC)/hal
CORNERSTONE_SERVICES_SRC = $(CORNERSTONE_SRC)/services
CORNERSTONE_FRAMEWORK_SRC = $(CORNERSTONE_SRC)/framework

# Specific component paths (for IDE compilation)
CORNERSTONE_DISPLAY_INC = $(CORNERSTONE_HAL_INC)/display
CORNERSTONE_DISPLAY_SRC = $(CORNERSTONE_HAL_SRC)/display/hal
CORNERSTONE_DISPLAY_DRV = $(CORNERSTONE_HAL_SRC)/display/driver
CORNERSTONE_GRAPHICS_INC = $(CORNERSTONE_FRAMEWORK_INC)/graphics
CORNERSTONE_GRAPHICS_SRC = $(CORNERSTONE_FRAMEWORK_SRC)/graphics
CORNERSTONE_PLATFORM_INC = $(CORNERSTONE_KERNEL_INC)/platform
CORNERSTONE_PLATFORM_SRC = $(CORNERSTONE_KERNEL_SRC)/platform/hal
CORNERSTONE_DAR_INC = $(CORNERSTONE_KERNEL_INC)/dar
CORNERSTONE_DAR_SRC = $(CORNERSTONE_KERNEL_SRC)/dar
CORNERSTONE_UTIL_INC = $(CORNERSTONE_FRAMEWORK_INC)/util
CORNERSTONE_UTIL_SRC = $(CORNERSTONE_FRAMEWORK_SRC)/util
CORNERSTONE_UI_INC = $(CORNERSTONE_FRAMEWORK_INC)/ui
CORNERSTONE_UI_SRC = $(CORNERSTONE_FRAMEWORK_SRC)/ui
CORNERSTONE_CONFIG_SRC = $(CORNERSTONE_FRAMEWORK_SRC)/config
CORNERSTONE_SESSION_SRC = $(CORNERSTONE_SERVICES_SRC)/session

# Cornerstone Libraries (Phase A structure)
# Link against Cornerstone stage libraries using same pattern as cornerstone.c
# Key: --start-group/--end-group handles circular deps, --whole-archive for stage3
CORNERSTONE_LIB_DIR = $(CORNERSTONE_DIR)/build/lib

# Cornerstone link flags (matches Cornerstone's own linking pattern)
# Order: stage4 → stage3 (whole archive) → stage2 → stage1 → bereshit
CORNERSTONE_LINK_FLAGS = \
	-L$(CORNERSTONE_LIB_DIR) \
	-Wl,--start-group \
	-lstage4_app \
	-Wl,--whole-archive -lstage3_framework -Wl,--no-whole-archive \
	-lstage2_hal -lstage1_kernel -lbereshit \
	-Wl,--end-group

# External libraries required by Cornerstone (from LDFLAGS)
CORNERSTONE_EXT_LIBS = -lGL -lepoxy -lglfw -lpng -lncurses -lm -ldl -lutil -lsqlite3

# Base CFLAGS (always needed for IDE)
# Includes follow the kernel-like layer hierarchy:
#   types.h (root) → kernel/ → hal/ → services/ → framework/
IDE_CFLAGS_BASE = $(CFLAGS) $(INCLUDES) \
	-I$(CORNERSTONE_DIR) \
	-I$(CORNERSTONE_INC) \
	-I$(CORNERSTONE_KERNEL_INC) \
	-I$(CORNERSTONE_KERNEL_INC)/cpisi \
	-I$(CORNERSTONE_HAL_INC) \
	-I$(CORNERSTONE_HAL_INC)/input/device \
	-I$(CORNERSTONE_FRAMEWORK_INC) \
	-I$(CORNERSTONE_DISPLAY_INC)/hal \
	-I$(CORNERSTONE_PLATFORM_INC)/hal \
	-I$(CORNERSTONE_DAR_INC) \
	-I$(CORNERSTONE_UTIL_INC)/system/util \
	-I$(CORNERSTONE_UTIL_INC)/graphics/util \
	-DIDE_CLI_BASE

# Mode-specific CFLAGS additions
ifeq ($(IDE_MODE),cli)
IDE_CFLAGS = $(IDE_CFLAGS_BASE)
IDE_LIBS =
IDE_MODE_DESC = CLI only (pure ANSI)
else ifeq ($(IDE_MODE),tui)
IDE_CFLAGS = $(IDE_CFLAGS_BASE) -DIDE_TUI_ENABLED
IDE_LIBS = -lncurses
IDE_MODE_DESC = TUI (CLI + ncurses)
else ifeq ($(IDE_MODE),full)
IDE_CFLAGS = $(IDE_CFLAGS_BASE) -I$(CORNERSTONE_GRAPHICS_INC)/render/manager -DIDE_TUI_ENABLED -DIDE_GUI_ENABLED
IDE_LIBS = -lncurses -lglfw -lGL -lepoxy -lpng -lm
IDE_MODE_DESC = Full (CLI + TUI + GUI)
else
# Default: gui
IDE_CFLAGS = $(IDE_CFLAGS_BASE) -I$(CORNERSTONE_GRAPHICS_INC)/render/manager -DIDE_GUI_ENABLED
IDE_LIBS = -lglfw -lGL -lepoxy -lpng -lm
IDE_MODE_DESC = GUI (CLI + OpenGL)
endif

## ide: Build OmniCode IDE
## Single target with IDE_MODE variable controlling what gets built
## Always outputs: build/omni-ide
ide: libomni.a assets cornerstone-libs
	$(ECHO) "Building OmniCode IDE [$(IDE_MODE_DESC)]..."
	$(Q)rm -rf $(BUILD_DIR)/ide && mkdir -p $(BUILD_DIR)/ide
	$(ECHO) "  [Core] omni_ide_main.c"
	$(Q)$(CC) $(IDE_CFLAGS) -c -o $(BUILD_DIR)/ide/main.o $(IDE_DIR)/main/omni_ide_main.c
	$(ECHO) "  [Core] omni_ide.c"
	$(Q)$(CC) $(IDE_CFLAGS) -c -o $(BUILD_DIR)/ide/core.o $(IDE_DIR)/core/omni_ide.c
	$(ECHO) "  [Core] omni_ide_panel.c"
	$(Q)$(CC) $(IDE_CFLAGS) -c -o $(BUILD_DIR)/ide/panel.o $(IDE_DIR)/panels/omni_ide_panel.c
	$(ECHO) "  [UI] omni_ide_cli.c"
	$(Q)$(CC) $(IDE_CFLAGS) -c -o $(BUILD_DIR)/ide/cli_ui.o $(IDE_DIR)/ui/omni_ide_cli.c
ifeq ($(IDE_MODE),tui)
	$(ECHO) "  [UI] omni_ide_tui.c"
	$(Q)$(CC) $(IDE_CFLAGS) -c -o $(BUILD_DIR)/ide/tui_ui.o $(IDE_DIR)/ui/omni_ide_tui.c
endif
ifeq ($(IDE_MODE),gui)
	$(ECHO) "  [UI] omni_ide_gui.c"
	$(Q)$(CC) $(IDE_CFLAGS) -c -o $(BUILD_DIR)/ide/gui_ui.o $(IDE_DIR)/ui/omni_ide_gui.c
endif
ifeq ($(IDE_MODE),full)
	$(ECHO) "  [UI] omni_ide_tui.c"
	$(Q)$(CC) $(IDE_CFLAGS) -c -o $(BUILD_DIR)/ide/tui_ui.o $(IDE_DIR)/ui/omni_ide_tui.c
	$(ECHO) "  [UI] omni_ide_gui.c"
	$(Q)$(CC) $(IDE_CFLAGS) -c -o $(BUILD_DIR)/ide/gui_ui.o $(IDE_DIR)/ui/omni_ide_gui.c
endif
	$(ECHO) "  [Config] config.c"
	$(Q)$(CC) $(IDE_CFLAGS) -c -o $(BUILD_DIR)/ide/config.o $(TRIT_DIR)/src/config.c
	$(ECHO) "  [Config] toml.c"
	$(Q)$(CC) $(IDE_CFLAGS) -c -o $(BUILD_DIR)/ide/toml.o $(TRIT_DIR)/src/toml.c
	$(ECHO) "  [Bridge] engine_bridge.c"
	$(Q)$(CC) $(IDE_CFLAGS) -c -o $(BUILD_DIR)/ide/engine_bridge.o $(IDE_DIR)/bridge/engine_bridge.c
	$(ECHO) "  [Link] omni-ide (linking against Cornerstone stage libraries)"
	$(Q)$(CC) -o $(BUILD_DIR)/omni-ide \
		$(BUILD_DIR)/ide/*.o \
		$(BUILD_DIR)/$(LIB_NAME) $(TRIT_LIB) \
		$(CORNERSTONE_LINK_FLAGS) \
		$(CORNERSTONE_EXT_LIBS) \
		$(IDE_LIBS) -lpthread
	$(ECHO) "✓ Built $(BUILD_DIR)/omni-ide [$(IDE_MODE_DESC)]"
	$(ECHO) ""
ifeq ($(IDE_MODE),cli)
	$(ECHO) "Pure ANSI terminal editor - works anywhere"
	$(ECHO) "No ncurses, no OpenGL - just escape codes"
else ifeq ($(IDE_MODE),tui)
	$(ECHO) "Display modes:"
	$(ECHO) "  --cli   Pure ANSI terminal"
	$(ECHO) "  --tui   Terminal interface (ncurses) - DEFAULT"
else ifeq ($(IDE_MODE),full)
	$(ECHO) "Display modes:"
	$(ECHO) "  --cli   Pure ANSI terminal"
	$(ECHO) "  --tui   Terminal interface (ncurses)"
	$(ECHO) "  --gui   Graphical interface (OpenGL) - DEFAULT"
else
	$(ECHO) "Display modes:"
	$(ECHO) "  --cli   Pure ANSI terminal"
	$(ECHO) "  --gui   Graphical interface (OpenGL) - DEFAULT"
endif
	$(ECHO) ""
	$(ECHO) "Usage: ./build/omni-ide [options] <file>"

## cornerstone-libs: Ensure Cornerstone libraries are built
## IDE links against pre-built Cornerstone libraries (Phase A architecture)
cornerstone-libs:
	$(Q)if [ ! -f "$(CORNERSTONE_LIB_DIR)/libstage1_kernel.a" ]; then \
		echo "Building Cornerstone libraries..."; \
		$(MAKE) -C $(CORNERSTONE_DIR) libs; \
	else \
		echo "  Cornerstone libraries up-to-date"; \
	fi

## install: Install omni-ide (use PREFIX=~/.local for user-local, or install-system for system-wide)
## Uses IDE_MODE=full by default for maximum compatibility
install:
	$(Q)$(MAKE) ide IDE_MODE=full
	$(ECHO) "Installing OmniCode IDE..."
	$(ECHO) ""
	$(ECHO) "  [1/5] Installing binary..."
	$(Q)install -d $(BINDIR)
	$(Q)install -m 755 $(BUILD_DIR)/omni-ide $(BINDIR)/omni-ide
	$(ECHO) "  [2/5] Installing assets..."
	$(Q)install -d $(DATADIR)
	$(Q)install -d $(DATADIR)/fonts/block
	$(Q)install -d $(DATADIR)/fonts/generated
	$(Q)install -d $(DATADIR)/fonts/bitmap
	$(Q)cp -rf $(ASSETS_DIR)/fonts/* $(DATADIR)/fonts/ 2>/dev/null || true
	$(ECHO) "  [3/5] Installing desktop entry..."
	$(Q)install -d $(DESKTOPDIR)
	$(Q)sed -e 's|@BINDIR@|$(BINDIR)|g' \
	        -e 's|@DATADIR@|$(DATADIR)|g' \
	        $(SRC_DIR)/ide/assets/omni-ide.desktop.in > $(DESKTOPDIR)/omni-ide.desktop
	$(Q)chmod 644 $(DESKTOPDIR)/omni-ide.desktop
	$(ECHO) "  [4/5] Installing icon..."
	$(Q)install -d $(ICONDIR)
	$(Q)install -d $(PREFIX)/share/icons/hicolor/scalable/apps
	$(Q)if [ -f $(SRC_DIR)/ide/assets/omni-ide.png ]; then \
		install -m 644 $(SRC_DIR)/ide/assets/omni-ide.png $(ICONDIR)/omni-ide.png; \
	elif [ -f $(ASSETS_DIR)/icons/omni-ide.png ]; then \
		install -m 644 $(ASSETS_DIR)/icons/omni-ide.png $(ICONDIR)/omni-ide.png; \
	elif [ -f $(SRC_DIR)/ide/assets/omni-ide.svg ]; then \
		install -m 644 $(SRC_DIR)/ide/assets/omni-ide.svg $(PREFIX)/share/icons/hicolor/scalable/apps/omni-ide.svg; \
		echo "    Installed SVG icon (scalable)"; \
	else \
		echo "    [WARN] Icon not found - install manually"; \
	fi
	$(ECHO) "  [5/5] Updating desktop database..."
	$(Q)update-desktop-database $(DESKTOPDIR) 2>/dev/null || true
	$(Q)gtk-update-icon-cache -f -t $(PREFIX)/share/icons/hicolor 2>/dev/null || true
	$(ECHO) ""
	$(ECHO) "✓ OmniCode IDE installed successfully"
	$(ECHO) ""
	$(ECHO) "  Binary:  $(BINDIR)/omni-ide"
	$(ECHO) "  Assets:  $(DATADIR)/"
	$(ECHO) "  Desktop: $(DESKTOPDIR)/omni-ide.desktop"
	$(ECHO) ""
	$(ECHO) "Usage: omni-ide [--cli|--tui|--gui] <file>"
	$(ECHO) "       omni-ide --help"

## install-desktop: Install only the desktop entry and icon (after install)
install-desktop:
	$(ECHO) "Installing desktop integration..."
	$(Q)install -d $(DESKTOPDIR)
	$(Q)sed -e 's|@BINDIR@|$(BINDIR)|g' \
	        -e 's|@DATADIR@|$(DATADIR)|g' \
	        $(SRC_DIR)/ide/assets/omni-ide.desktop.in > $(DESKTOPDIR)/omni-ide.desktop
	$(Q)chmod 644 $(DESKTOPDIR)/omni-ide.desktop
	$(Q)install -d $(ICONDIR)
	$(Q)if [ -f $(SRC_DIR)/ide/assets/omni-ide.png ]; then \
		install -m 644 $(SRC_DIR)/ide/assets/omni-ide.png $(ICONDIR)/omni-ide.png; \
	fi
	$(Q)update-desktop-database $(DESKTOPDIR) 2>/dev/null || true
	$(Q)gtk-update-icon-cache -f -t $(PREFIX)/share/icons/hicolor 2>/dev/null || true
	$(ECHO) "✓ Desktop integration installed"

## install-system: Install omni-ide system-wide with graphical password prompt (pkexec)
## Builds as user, then uses pkexec for privileged installation
install-system:
	$(Q)$(MAKE) ide IDE_MODE=full
	$(ECHO) "Installing OmniCode IDE system-wide (will prompt for password)..."
	$(ECHO) ""
	$(Q)pkexec env \
		BUILD_DIR="$(CURDIR)/$(BUILD_DIR)" \
		SRC_DIR="$(CURDIR)/$(SRC_DIR)" \
		ASSETS_DIR="$(CURDIR)/$(ASSETS_DIR)" \
		sh -c '\
		echo "  [1/5] Installing binary..."; \
		install -d /usr/local/bin; \
		install -m 755 $$BUILD_DIR/omni-ide /usr/local/bin/omni-ide; \
		echo "  [2/5] Installing assets..."; \
		install -d /usr/local/share/omni-ide; \
		install -d /usr/local/share/omni-ide/fonts/block; \
		install -d /usr/local/share/omni-ide/fonts/generated; \
		install -d /usr/local/share/omni-ide/fonts/bitmap; \
		cp -rf $$ASSETS_DIR/fonts/* /usr/local/share/omni-ide/fonts/ 2>/dev/null || true; \
		echo "  [3/5] Installing desktop entry..."; \
		install -d /usr/local/share/applications; \
		sed -e "s|@BINDIR@|/usr/local/bin|g" \
		    -e "s|@DATADIR@|/usr/local/share/omni-ide|g" \
		    $$SRC_DIR/ide/assets/omni-ide.desktop.in > /usr/local/share/applications/omni-ide.desktop; \
		chmod 644 /usr/local/share/applications/omni-ide.desktop; \
		echo "  [4/5] Installing icons..."; \
		for size in 16 24 32 48 64 128 256; do \
			install -d /usr/share/icons/hicolor/$${size}x$${size}/apps; \
			if [ -f $$SRC_DIR/ide/assets/icons/omni-ide-$${size}.png ]; then \
				install -m 644 $$SRC_DIR/ide/assets/icons/omni-ide-$${size}.png /usr/share/icons/hicolor/$${size}x$${size}/apps/omni-ide.png; \
			fi; \
		done; \
		if [ -f $$SRC_DIR/ide/assets/omni-ide.svg ]; then \
			install -d /usr/share/icons/hicolor/scalable/apps; \
			install -m 644 $$SRC_DIR/ide/assets/omni-ide.svg /usr/share/icons/hicolor/scalable/apps/omni-ide.svg; \
		fi; \
		echo "  [5/5] Updating desktop database..."; \
		update-desktop-database /usr/local/share/applications 2>/dev/null || true; \
		gtk-update-icon-cache -f -t /usr/share/icons/hicolor 2>/dev/null || true; \
	'
	$(ECHO) ""
	$(ECHO) "✓ OmniCode IDE installed system-wide"
	$(ECHO) ""
	$(ECHO) "  Binary:  /usr/local/bin/omni-ide"
	$(ECHO) "  Assets:  /usr/local/share/omni-ide/"
	$(ECHO) "  Desktop: /usr/local/share/applications/omni-ide.desktop"
	$(ECHO) ""
	$(ECHO) "Usage: omni-ide [--cli|--tui|--gui] <file>"

## uninstall-system: Remove omni-ide from system-wide installation (pkexec)
uninstall-system:
	$(ECHO) "Removing OmniCode IDE system-wide (will prompt for password)..."
	$(Q)pkexec sh -c '\
		rm -f /usr/local/bin/omni-ide; \
		rm -rf /usr/local/share/omni-ide; \
		rm -f /usr/local/share/applications/omni-ide.desktop; \
		for size in 16 24 32 48 64 128 256; do \
			rm -f /usr/share/icons/hicolor/$${size}x$${size}/apps/omni-ide.png; \
		done; \
		rm -f /usr/share/icons/hicolor/scalable/apps/omni-ide.svg; \
		update-desktop-database /usr/local/share/applications 2>/dev/null || true; \
		gtk-update-icon-cache -f -t /usr/share/icons/hicolor 2>/dev/null || true; \
	'
	$(ECHO) "✓ OmniCode IDE uninstalled from system"

## uninstall: Remove omni-ide from system completely
uninstall:
	$(ECHO) "Removing OmniCode IDE..."
	$(Q)rm -f $(BINDIR)/omni-ide
	$(Q)rm -rf $(DATADIR)
	$(Q)rm -f $(DESKTOPDIR)/omni-ide.desktop
	$(Q)rm -f $(ICONDIR)/omni-ide.png
	$(Q)update-desktop-database $(DESKTOPDIR) 2>/dev/null || true
	$(ECHO) "✓ OmniCode IDE uninstalled"

# ─────────────────────────────────────────────────────────────────────────────
# User-Local Installation (aligned with Cornerstone/MillenniumOS pattern)
# ─────────────────────────────────────────────────────────────────────────────

## install-user: Install to user's local directory (~/.local)
install-user: PREFIX=$(HOME)/.local
install-user: DESKTOPDIR=$(HOME)/.local/share/applications
install-user: ICONDIR=$(HOME)/.local/share/icons/hicolor/256x256/apps
install-user: install
	@# Force desktop cache refresh for user install
	$(Q)update-desktop-database $(HOME)/.local/share/applications 2>/dev/null || true
	$(Q)gtk-update-icon-cache -f -t $(HOME)/.local/share/icons/hicolor 2>/dev/null || true
	$(ECHO) ""
	$(ECHO) "  Desktop entry installed — search 'OmniCode' in applications"

## uninstall-user: Remove from user's local directory (~/.local)
uninstall-user: PREFIX=$(HOME)/.local
uninstall-user: DESKTOPDIR=$(HOME)/.local/share/applications
uninstall-user: ICONDIR=$(HOME)/.local/share/icons/hicolor/256x256/apps
uninstall-user: uninstall
	$(ECHO) "✓ Uninstalled from user's local directory"

## update-user: Rebuild and reinstall to user's local directory
update-user: clean ide
	@$(MAKE) --no-print-directory uninstall-user 2>/dev/null || true
	@$(MAKE) --no-print-directory install-user
	$(ECHO) ""
	$(ECHO) "✓ OmniCode IDE updated in ~/.local"

## clean: Remove build artifacts
clean:
	$(ECHO) "Cleaning..."
	$(Q)rm -rf $(BUILD_DIR)
	$(Q)rm -f *.o
	$(ECHO) "✓ Clean complete"

## help: Show available targets and build options
help:
	$(ECHO) "════════════════════════════════════════════════════════════════════════════════"
	$(ECHO) "  OmniCode IDE + libomni - Build System"
	$(ECHO) "════════════════════════════════════════════════════════════════════════════════"
	$(ECHO) ""
	$(ECHO) "USAGE: make [target] [OPTIONS]"
	$(ECHO) ""
	$(ECHO) "────────────────────────────────────────────────────────────────────────────────"
	$(ECHO) "BUILD TARGETS"
	$(ECHO) "────────────────────────────────────────────────────────────────────────────────"
	$(ECHO) "  all            Build libomni.a (default)"
	$(ECHO) "  libomni.a      Build the static library"
	$(ECHO) "  assets         Copy fonts and icons to build directory"
	$(ECHO) ""
	$(ECHO) "────────────────────────────────────────────────────────────────────────────────"
	$(ECHO) "IDE TARGET"
	$(ECHO) "────────────────────────────────────────────────────────────────────────────────"
	$(ECHO) "  ide                    Build OmniCode IDE (uses IDE_MODE)"
	$(ECHO) ""
	$(ECHO) "  IDE_MODE=gui           CLI + OpenGL graphics (default)"
	$(ECHO) "  IDE_MODE=tui           CLI + ncurses terminal"
	$(ECHO) "  IDE_MODE=cli           Pure ANSI terminal only"
	$(ECHO) "  IDE_MODE=full          All backends with runtime selection"
	$(ECHO) ""
	$(ECHO) "────────────────────────────────────────────────────────────────────────────────"
	$(ECHO) "TEST TARGETS"
	$(ECHO) "────────────────────────────────────────────────────────────────────────────────"
	$(ECHO) "  test           Run all tests"
	$(ECHO) "  test-lexer     Test lexer"
	$(ECHO) "  test-parser    Test AST parser"
	$(ECHO) "  test-chunks    Test chunk lexer"
	$(ECHO) "  test-dar       Test DAR pattern (Detect-Assess-Recover)"
	$(ECHO) "  test-codegen   Test code generator"
	$(ECHO) "  test-conversation  Test CPI-SI conversation flow"
	$(ECHO) "  test-input INPUT=\"text\"  Interactive input tester"
	$(ECHO) "  test-ide FILE=path   Test IDE with file"
	$(ECHO) ""
	$(ECHO) "────────────────────────────────────────────────────────────────────────────────"
	$(ECHO) "INSTALL TARGETS"
	$(ECHO) "────────────────────────────────────────────────────────────────────────────────"
	$(ECHO) "  install          Install to PREFIX (default: /usr/local or ~/.local)"
	$(ECHO) "  install-system   System-wide install with pkexec (graphical password prompt)"
	$(ECHO) "  install-desktop  Install desktop entry and icon only"
	$(ECHO) "  uninstall        Remove from PREFIX location"
	$(ECHO) "  uninstall-system Remove system-wide install with pkexec"
	$(ECHO) ""
	$(ECHO) "────────────────────────────────────────────────────────────────────────────────"
	$(ECHO) "MAINTENANCE TARGETS"
	$(ECHO) "────────────────────────────────────────────────────────────────────────────────"
	$(ECHO) "  clean          Remove build artifacts"
	$(ECHO) "  info           Show build configuration"
	$(ECHO) ""
	$(ECHO) "────────────────────────────────────────────────────────────────────────────────"
	$(ECHO) "BUILD MODES"
	$(ECHO) "────────────────────────────────────────────────────────────────────────────────"
	$(ECHO) "  BUILD_MODE=dev       Fast iteration (default) — -O1 -g"
	$(ECHO) "  BUILD_MODE=debug     Full debugging — -O0 -g3, verbose errors"
	$(ECHO) "  BUILD_MODE=release   Production — -O3 -DNDEBUG, stripped"
	$(ECHO) ""
	$(ECHO) "────────────────────────────────────────────────────────────────────────────────"
	$(ECHO) "BUILD OPTIONS"
	$(ECHO) "────────────────────────────────────────────────────────────────────────────────"
	$(ECHO) "  SANITIZE=1     Enable address/undefined sanitizers (use with debug)"
	$(ECHO) "  VERBOSE=1      Show full compilation commands"
	$(ECHO) "  CC=clang       Use alternative compiler"
	$(ECHO) ""
	$(ECHO) "────────────────────────────────────────────────────────────────────────────────"
	$(ECHO) "DISPLAY MODES (runtime flags)"
	$(ECHO) "────────────────────────────────────────────────────────────────────────────────"
	$(ECHO) "  --cli          Pure ANSI terminal"
	$(ECHO) "  --tui          Terminal interface (ncurses)"
	$(ECHO) "  --gui          Graphical interface (OpenGL) — default for ide-full"
	$(ECHO) ""
	$(ECHO) "────────────────────────────────────────────────────────────────────────────────"
	$(ECHO) "EXAMPLES"
	$(ECHO) "────────────────────────────────────────────────────────────────────────────────"
	$(ECHO) "  make ide                             # Build GUI IDE (default)"
	$(ECHO) "  make ide IDE_MODE=full               # Build with all backends"
	$(ECHO) "  make ide IDE_MODE=cli                # Build CLI-only version"
	$(ECHO) "  make ide BUILD_MODE=release          # Production build"
	$(ECHO) "  make ide BUILD_MODE=debug SANITIZE=1 # Debug with sanitizers"
	$(ECHO) "  make test VERBOSE=1                  # Run tests with verbose output"
	$(ECHO) "  ./build/omni-ide --gui file.omni    # Run GUI with file"
	$(ECHO) "  ./build/omni-ide --cli              # Run CLI mode"
	$(ECHO) ""
	$(ECHO) "Current: IDE_MODE=$(IDE_MODE), BUILD_MODE=$(BUILD_MODE), SANITIZE=$(SANITIZE)"
	$(ECHO) ""

## info: Show build configuration
info:
	$(ECHO) "════════════════════════════════════════════════════════════════════════════════"
	$(ECHO) "  OmniCode Build Configuration"
	$(ECHO) "════════════════════════════════════════════════════════════════════════════════"
	$(ECHO) ""
	$(ECHO) "IDENTITY"
	$(ECHO) "  LIB_NAME:      $(LIB_NAME)"
	$(ECHO) "  BUILD_DIR:     $(BUILD_DIR)"
	$(ECHO) ""
	$(ECHO) "BUILD MODE"
	$(ECHO) "  BUILD_MODE:    $(BUILD_MODE)"
	$(ECHO) "  SANITIZE:      $(SANITIZE)"
	$(ECHO) "  VERBOSE:       $(VERBOSE)"
	$(ECHO) ""
	$(ECHO) "TOOLCHAIN"
	$(ECHO) "  CC:            $(CC)"
	$(ECHO) "  CFLAGS:        $(CFLAGS)"
	$(ECHO) "  LDFLAGS_MODE:  $(LDFLAGS_MODE)"
	$(ECHO) ""
	$(ECHO) "PATHS"
	$(ECHO) "  BERESHIT_ROOT: $(BERESHIT_ROOT)"
	$(ECHO) "  TRIT_DIR:      $(TRIT_DIR)"
	$(ECHO) "  CORNERSTONE:   $(CORNERSTONE_DIR)"
	$(ECHO) ""
	$(ECHO) "SOURCES"
	$(ECHO) "  Lang Frontend: $(words $(LANG_FRONTEND_SRCS)) files"
	$(ECHO) "  Lang Backend:  $(words $(LANG_BACKEND_SRCS)) files"
	$(ECHO) "  Lang Core:     $(words $(LANG_CORE_SRCS)) files"
	$(ECHO) "  Runtime VM:    $(words $(RUNTIME_VM_SRCS)) files"
	$(ECHO) "  Runtime CPI-SI:$(words $(RUNTIME_CPISI_SRCS)) files"
	$(ECHO) "  Runtime Tools: $(words $(RUNTIME_TOOLS_SRCS)) files"
	$(ECHO) "  Total Objects: $(words $(OBJS)) files"
	$(ECHO) ""

# ============================================================================
# END BODY
# ============================================================================

# ============================================================================
# CLOSING
# ============================================================================
#
# Validation:
#   make clean && make        # Fresh build
#   make test                 # Run tests
#   make test-dar             # Test error detection
#
# Related Components:
#   - word/work/pkg/trit/ (dependency: libtrit)
#   - word/omni/syntax.omni (language specification)
#
# "In the beginning was the Word." — John 1:1
#
# ============================================================================
# END CLOSING
# ============================================================================
