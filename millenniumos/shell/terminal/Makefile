# =============================================================================
# METADATA [METADATA]
# =============================================================================
#
# Key:          MOS-SHELL-TERMINAL-MAKEFILE
# Title:        MillenniumOS Terminal Shell Makefile
# Type:         Build
# Component:    Shell / Terminal
#
# Authors:      Seanje Lenox-Wise (Architect), Nova Dawn (Implementation)
# Organization: CreativeWorkzStudio LLC
#
# Status:       Active
# Version:      a-01.00
# Created:      2026-01-31
#
# =============================================================================

# =============================================================================
# SETUP [SETUP]
# =============================================================================

# Directories
SHELL_DIR    := $(dir $(lastword $(MAKEFILE_LIST)))
SHELL_SRC    := $(SHELL_DIR)src
SHELL_INC    := $(SHELL_DIR)include
SHELL_BUILD  := $(SHELL_DIR)build

# Source files
SHELL_SRCS   := $(SHELL_SRC)/shell.c \
                $(SHELL_SRC)/parser.c \
                $(SHELL_SRC)/dispatch.c \
                $(SHELL_SRC)/builtins/core.c \
                $(SHELL_SRC)/builtins/boot.c \
                $(SHELL_SRC)/builtins/dar.c \
                $(SHELL_SRC)/builtins/mem.c

# Object files
SHELL_OBJS   := $(patsubst $(SHELL_SRC)/%.c,$(SHELL_BUILD)/%.o,$(SHELL_SRCS))

# Output
SHELL_LIB    := $(SHELL_BUILD)/libshell.a

# Compiler settings (inherit from parent or set defaults)
CC           ?= gcc
AR           ?= ar
CFLAGS       ?= -Wall -Wextra -Werror -ffreestanding -nostdlib -O2

# Include paths
SHELL_CFLAGS := $(CFLAGS) \
                -I$(SHELL_INC) \
                -I../../kernel/include \
                -I../../boot/include

# =============================================================================
# END SETUP
# =============================================================================

# =============================================================================
# BODY [BODY]
# =============================================================================

.PHONY: all clean

all: $(SHELL_LIB)

# Create static library
$(SHELL_LIB): $(SHELL_OBJS)
	@mkdir -p $(dir $@)
	@echo "[AR] Creating $@"
	@$(AR) rcs $@ $^

# Compile source files
$(SHELL_BUILD)/%.o: $(SHELL_SRC)/%.c
	@mkdir -p $(dir $@)
	@echo "[CC] Compiling $<"
	@$(CC) $(SHELL_CFLAGS) -c $< -o $@

# Clean
clean:
	@echo "[CLEAN] Removing shell build directory"
	@rm -rf $(SHELL_BUILD)

# =============================================================================
# END BODY
# =============================================================================

# =============================================================================
# CLOSING [CLOSING]
# =============================================================================

# Show help
help:
	@echo "MillenniumOS Terminal Shell Build"
	@echo ""
	@echo "Targets:"
	@echo "  all     - Build libshell.a"
	@echo "  clean   - Remove build artifacts"
	@echo "  help    - Show this help"
	@echo ""
	@echo "Output: $(SHELL_LIB)"

# =============================================================================
# END CLOSING
# =============================================================================
