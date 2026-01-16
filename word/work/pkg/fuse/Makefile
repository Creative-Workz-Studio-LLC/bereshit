# #!omni data --makefile
# #!omni meta.key = B-word-work-pkg-fuse-makefile
# #!omni meta.at = a-01.00
#
# Makefile — Bereshit FUSE Filesystem
# Build the zone-aware filesystem mount

# =============================================================================
# METADATA
# =============================================================================

# Key: B-word-work-pkg-fuse-makefile
# Title: FUSE Package Makefile
# Type: Build Configuration

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

# Source files
SOURCES = $(SRC_DIR)/bereshit_fs.c $(SRC_DIR)/zone.c
OBJECTS = $(SOURCES:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)

# Target
TARGET = $(BIN_DIR)/bereshit_fs

# =============================================================================
# TARGETS
# =============================================================================

.PHONY: all clean help install

all: $(TARGET)

$(TARGET): $(OBJECTS) | $(BIN_DIR)
	$(CC) $(OBJECTS) -o $@ $(LDFLAGS)
	@echo "Built: $@"

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
	@echo "Targets:"
	@echo "  all     Build bereshit_fs (default)"
	@echo "  clean   Remove build artifacts"
	@echo "  help    Show this help"
	@echo ""
	@echo "Usage:"
	@echo "  ./bin/bereshit_fs --source=/path/to/Bereshit /mnt/bereshit"

# =============================================================================
# CLOSING
# =============================================================================

# "And God saw that it was good" — Genesis 1:10
