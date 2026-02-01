# #!omni data --makefile
# #!omni meta.key = B-word-work-pkg-fuse-makefile
# #!omni meta.at = a-02.00
#
# Makefile — Bereshit FUSE Filesystem
# Build the zone-aware filesystem mount

# =============================================================================
# METADATA
# =============================================================================

# Key: B-word-work-pkg-fuse-makefile
# Title: FUSE Package Makefile
# Type: Build Configuration
# Version: a-02.00 (separated zone/policy/health)

# =============================================================================
# CONTENT
# =============================================================================

# Directories
SRC_DIR = src
INC_DIR = include
BUILD_DIR = build
BIN_DIR = bin

# Compiler settings
CC = gcc
CFLAGS = -Wall -Wextra -Werror -std=c11 -O2
CFLAGS += -I$(INC_DIR)
CFLAGS += $(shell pkg-config --cflags fuse3)

LDFLAGS = $(shell pkg-config --libs fuse3)

# -----------------------------------------------------------------------------
# Source Files (separated by responsibility)
# -----------------------------------------------------------------------------

# Core zone detection
ZONE_SOURCES = $(SRC_DIR)/zone.c

# Policy enforcement
POLICY_SOURCES = $(SRC_DIR)/policy.c

# Health tracking
HEALTH_SOURCES = $(SRC_DIR)/health.c

# FUSE implementation
FUSE_SOURCES = $(SRC_DIR)/bereshit_fs.c

# All sources
SOURCES = $(ZONE_SOURCES) $(POLICY_SOURCES) $(HEALTH_SOURCES) $(FUSE_SOURCES)
OBJECTS = $(SOURCES:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)

# Target
TARGET = $(BIN_DIR)/bereshit_fs

# =============================================================================
# TARGETS
# =============================================================================

.PHONY: all clean help install test

all: $(TARGET)

$(TARGET): $(OBJECTS) | $(BIN_DIR)
	$(CC) $(OBJECTS) -o $@ $(LDFLAGS)
	@echo "Built: $@"
	@echo ""
	@echo "Source files compiled:"
	@echo "  - zone.c    (zone detection)"
	@echo "  - policy.c  (policy enforcement)"
	@echo "  - health.c  (health tracking)"
	@echo "  - bereshit_fs.c (FUSE implementation)"

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR):
	mkdir -p $@

$(BIN_DIR):
	mkdir -p $@

clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)
	@echo "Cleaned build artifacts"

help:
	@echo "Bereshit FUSE Filesystem"
	@echo ""
	@echo "Structure:"
	@echo "  zone.c      - Zone detection (WHAT zones exist)"
	@echo "  policy.c    - Policy enforcement (HOW zones behave)"
	@echo "  health.c    - Health tracking (WHAT STATE zones are in)"
	@echo "  bereshit_fs.c - FUSE implementation"
	@echo ""
	@echo "Targets:"
	@echo "  all     Build bereshit_fs (default)"
	@echo "  clean   Remove build artifacts"
	@echo "  help    Show this help"
	@echo ""
	@echo "Usage:"
	@echo "  ./bin/bereshit_fs --source=/path/to/Bereshit /mnt/bereshit"
	@echo ""
	@echo "Zone Semantics:"
	@echo "  void (-1)  Entry gate - permissive (formless accepts all)"
	@echo "  word (0)   Processing - standard (definitions and work)"
	@echo "  tov (+1)   Exit gate - structured (requires root.omni)"

# =============================================================================
# CLOSING
# =============================================================================

# "And God saw that it was good" — Genesis 1:10
#
# The separation follows Genesis 1 pattern:
#   - zone.c: Naming and identification (Day 1 - light from darkness)
#   - policy.c: Rules and boundaries (Day 2 - firmament divides)
#   - health.c: State and monitoring (Day 3 - fruitfulness)
