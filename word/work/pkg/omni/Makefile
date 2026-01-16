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
        test-codegen test-conversation clean help info install install-desktop uninstall ide

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

# Include paths (omni headers + trit headers + cornerstone)
# Organized includes mirror source structure
INCLUDES = -I$(INC_DIR) \
           -I$(INC_DIR)/lang/frontend -I$(INC_DIR)/lang/backend -I$(INC_DIR)/lang/core \
           -I$(INC_DIR)/runtime/vm -I$(INC_DIR)/runtime/cpisi -I$(INC_DIR)/runtime/tools \
           -I$(INC_DIR)/ide/core -I$(INC_DIR)/ide/ui -I$(INC_DIR)/ide/panels \
           -I$(TRIT_INC) -I$(CORNERSTONE_INC) \
           -I$(CORNERSTONE_INC)/engine/core/include/util

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
CORNERSTONE_DISPLAY_INC = $(CORNERSTONE_DIR)/engine/display/include
CORNERSTONE_DISPLAY_SRC = $(CORNERSTONE_DIR)/engine/display/src
CORNERSTONE_CORE_INC = $(CORNERSTONE_DIR)/engine/core/include
CORNERSTONE_CORE_SRC = $(CORNERSTONE_DIR)/engine/core/src
CORNERSTONE_PLATFORM_SRC = $(CORNERSTONE_DIR)/engine/platform/linux
CORNERSTONE_GRAPHICS_INC = $(CORNERSTONE_DIR)/engine/graphics/include
CORNERSTONE_GRAPHICS_SRC = $(CORNERSTONE_DIR)/engine/graphics/src

# Base CFLAGS (always needed for IDE)
IDE_CFLAGS_BASE = $(CFLAGS) $(INCLUDES) \
	-I$(CORNERSTONE_DIR) \
	-I$(CORNERSTONE_DISPLAY_INC) \
	-I$(CORNERSTONE_CORE_INC)/engine -I$(CORNERSTONE_CORE_INC)/scene -I$(CORNERSTONE_CORE_INC)/util \
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
IDE_CFLAGS = $(IDE_CFLAGS_BASE) -I$(CORNERSTONE_GRAPHICS_INC) -DIDE_TUI_ENABLED -DIDE_GUI_ENABLED
IDE_LIBS = -lncurses -lglfw -lGL -lepoxy -lpng -lm
IDE_MODE_DESC = Full (CLI + TUI + GUI)
else
# Default: gui
IDE_CFLAGS = $(IDE_CFLAGS_BASE) -I$(CORNERSTONE_GRAPHICS_INC) -DIDE_GUI_ENABLED
IDE_LIBS = -lglfw -lGL -lepoxy -lpng -lm
IDE_MODE_DESC = GUI (CLI + OpenGL)
endif

## ide: Build OmniCode IDE
## Single target with IDE_MODE variable controlling what gets built
## Always outputs: build/omni-ide
ide: libomni.a assets
	$(ECHO) "Building OmniCode IDE [$(IDE_MODE_DESC)]..."
	$(Q)mkdir -p $(BUILD_DIR)/ide
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
	$(ECHO) "  [Core] omni_screenshot.c"
	$(Q)$(CC) $(IDE_CFLAGS) -c -o $(BUILD_DIR)/ide/screenshot.o $(IDE_DIR)/core/omni_screenshot.c
endif
ifeq ($(IDE_MODE),full)
	$(ECHO) "  [UI] omni_ide_tui.c"
	$(Q)$(CC) $(IDE_CFLAGS) -c -o $(BUILD_DIR)/ide/tui_ui.o $(IDE_DIR)/ui/omni_ide_tui.c
	$(ECHO) "  [UI] omni_ide_gui.c"
	$(Q)$(CC) $(IDE_CFLAGS) -c -o $(BUILD_DIR)/ide/gui_ui.o $(IDE_DIR)/ui/omni_ide_gui.c
	$(ECHO) "  [Core] omni_screenshot.c"
	$(Q)$(CC) $(IDE_CFLAGS) -c -o $(BUILD_DIR)/ide/screenshot.o $(IDE_DIR)/core/omni_screenshot.c
endif
	$(ECHO) "  [Display] display.c"
	$(Q)$(CC) $(IDE_CFLAGS) -c -o $(BUILD_DIR)/ide/display.o $(CORNERSTONE_DISPLAY_SRC)/display.c
	$(ECHO) "  [Backend] cli.c"
	$(Q)$(CC) $(IDE_CFLAGS) -c -o $(BUILD_DIR)/ide/cli_backend.o $(CORNERSTONE_DISPLAY_SRC)/backends/cli.c
ifeq ($(IDE_MODE),tui)
	$(ECHO) "  [Backend] tui.c"
	$(Q)$(CC) $(IDE_CFLAGS) -c -o $(BUILD_DIR)/ide/tui_backend.o $(CORNERSTONE_DISPLAY_SRC)/backends/tui.c
endif
ifeq ($(IDE_MODE),gui)
	$(ECHO) "  [Backend] gui.c"
	$(Q)$(CC) $(IDE_CFLAGS) -c -o $(BUILD_DIR)/ide/gui_backend.o $(CORNERSTONE_DISPLAY_SRC)/backends/gui.c
	$(ECHO) "  [Graphics] renderer.c"
	$(Q)$(CC) $(IDE_CFLAGS) -c -o $(BUILD_DIR)/ide/renderer.o $(CORNERSTONE_GRAPHICS_SRC)/renderer.c
	$(ECHO) "  [Graphics] texture.c"
	$(Q)$(CC) $(IDE_CFLAGS) -c -o $(BUILD_DIR)/ide/texture.o $(CORNERSTONE_GRAPHICS_SRC)/texture.c
	$(ECHO) "  [Graphics] text.c"
	$(Q)$(CC) $(IDE_CFLAGS) -c -o $(BUILD_DIR)/ide/text.o $(CORNERSTONE_GRAPHICS_SRC)/text.c
	$(ECHO) "  [Graphics] fontgen.c"
	$(Q)$(CC) $(IDE_CFLAGS) -c -o $(BUILD_DIR)/ide/fontgen.o $(CORNERSTONE_GRAPHICS_SRC)/fontgen.c
	$(ECHO) "  [Graphics] blockfont.c"
	$(Q)$(CC) $(IDE_CFLAGS) -c -o $(BUILD_DIR)/ide/blockfont.o $(CORNERSTONE_CORE_SRC)/util/blockfont.c
	$(ECHO) "  [Platform] platform_linux.c"
	$(Q)$(CC) $(IDE_CFLAGS) -c -o $(BUILD_DIR)/ide/platform.o $(CORNERSTONE_PLATFORM_SRC)/platform_linux.c
endif
ifeq ($(IDE_MODE),full)
	$(ECHO) "  [Backend] tui.c"
	$(Q)$(CC) $(IDE_CFLAGS) -c -o $(BUILD_DIR)/ide/tui_backend.o $(CORNERSTONE_DISPLAY_SRC)/backends/tui.c
	$(ECHO) "  [Backend] gui.c"
	$(Q)$(CC) $(IDE_CFLAGS) -c -o $(BUILD_DIR)/ide/gui_backend.o $(CORNERSTONE_DISPLAY_SRC)/backends/gui.c
	$(ECHO) "  [Graphics] renderer.c"
	$(Q)$(CC) $(IDE_CFLAGS) -c -o $(BUILD_DIR)/ide/renderer.o $(CORNERSTONE_GRAPHICS_SRC)/renderer.c
	$(ECHO) "  [Graphics] texture.c"
	$(Q)$(CC) $(IDE_CFLAGS) -c -o $(BUILD_DIR)/ide/texture.o $(CORNERSTONE_GRAPHICS_SRC)/texture.c
	$(ECHO) "  [Graphics] text.c"
	$(Q)$(CC) $(IDE_CFLAGS) -c -o $(BUILD_DIR)/ide/text.o $(CORNERSTONE_GRAPHICS_SRC)/text.c
	$(ECHO) "  [Graphics] fontgen.c"
	$(Q)$(CC) $(IDE_CFLAGS) -c -o $(BUILD_DIR)/ide/fontgen.o $(CORNERSTONE_GRAPHICS_SRC)/fontgen.c
	$(ECHO) "  [Graphics] blockfont.c"
	$(Q)$(CC) $(IDE_CFLAGS) -c -o $(BUILD_DIR)/ide/blockfont.o $(CORNERSTONE_CORE_SRC)/util/blockfont.c
	$(ECHO) "  [Platform] platform_linux.c"
	$(Q)$(CC) $(IDE_CFLAGS) -c -o $(BUILD_DIR)/ide/platform.o $(CORNERSTONE_PLATFORM_SRC)/platform_linux.c
endif
	$(ECHO) "  [Util] cpisi_log.c"
	$(Q)$(CC) $(IDE_CFLAGS) -c -o $(BUILD_DIR)/ide/cpisi_log.o $(CORNERSTONE_CORE_SRC)/util/cpisi_log.c
	$(ECHO) "  [Util] jsonc.c"
	$(Q)$(CC) $(IDE_CFLAGS) -c -o $(BUILD_DIR)/ide/jsonc.o $(CORNERSTONE_CORE_SRC)/util/jsonc.c
	$(ECHO) "  [Config] config.c"
	$(Q)$(CC) $(IDE_CFLAGS) -c -o $(BUILD_DIR)/ide/config.o $(TRIT_DIR)/src/config.c
	$(ECHO) "  [Config] toml.c"
	$(Q)$(CC) $(IDE_CFLAGS) -c -o $(BUILD_DIR)/ide/toml.o $(TRIT_DIR)/src/toml.c
	$(ECHO) "  [Link] omni-ide"
	$(Q)$(CC) -o $(BUILD_DIR)/omni-ide \
		$(BUILD_DIR)/ide/*.o \
		$(BUILD_DIR)/$(LIB_NAME) $(TRIT_LIB) $(IDE_LIBS)
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

## install: Install omni-ide to system with all assets (requires sudo)
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
	$(Q)install -m 644 $(SRC_DIR)/ide/assets/omni-ide.desktop $(DESKTOPDIR)/omni-ide.desktop
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
	$(Q)install -m 644 $(SRC_DIR)/ide/assets/omni-ide.desktop $(DESKTOPDIR)/omni-ide.desktop
	$(Q)install -d $(ICONDIR)
	$(Q)if [ -f $(SRC_DIR)/ide/assets/omni-ide.png ]; then \
		install -m 644 $(SRC_DIR)/ide/assets/omni-ide.png $(ICONDIR)/omni-ide.png; \
	fi
	$(Q)update-desktop-database $(DESKTOPDIR) 2>/dev/null || true
	$(Q)gtk-update-icon-cache -f -t $(PREFIX)/share/icons/hicolor 2>/dev/null || true
	$(ECHO) "✓ Desktop integration installed"

## uninstall: Remove omni-ide from system completely
uninstall:
	$(ECHO) "Removing OmniCode IDE..."
	$(Q)rm -f $(BINDIR)/omni-ide
	$(Q)rm -rf $(DATADIR)
	$(Q)rm -f $(DESKTOPDIR)/omni-ide.desktop
	$(Q)rm -f $(ICONDIR)/omni-ide.png
	$(Q)update-desktop-database $(DESKTOPDIR) 2>/dev/null || true
	$(ECHO) "✓ OmniCode IDE uninstalled"

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
	$(ECHO) "  install          Install omni-ide to system (IDE_MODE=full)"
	$(ECHO) "  install-desktop  Install desktop entry and icon only"
	$(ECHO) "  uninstall        Remove from system"
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
