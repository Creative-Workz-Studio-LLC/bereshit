# #!omni data --makefile
# #!omni meta.key = B-tov-demo-phase-3-fuse-mount-makefile
# #!omni meta.at = a-01.00
#
# Makefile — FUSE Mount Demo

# =============================================================================
# METADATA
# =============================================================================

# Key: B-tov-demo-phase-3-fuse-mount-makefile
# Title: FUSE Mount Demo Makefile
# Type: Build Configuration

# =============================================================================
# CONTENT
# =============================================================================

# Project root
BERESHIT = /media/seanje-lenox-wise/Project/Bereshit

# Directories
FUSE_PKG = $(BERESHIT)/word/work/pkg/fuse
BUILD_DIR = build
BIN_DIR = bin

# Compiler settings
CC = gcc
CFLAGS = -Wall -Wextra -Werror -std=c11 -O2
CFLAGS += -I$(FUSE_PKG)/include

# Source files
SOURCES = main.c
ZONE_SRC = $(FUSE_PKG)/src/zone.c

OBJECTS = $(BUILD_DIR)/main.o $(BUILD_DIR)/zone.o

# Target
TARGET = $(BIN_DIR)/demo-fuse-mount

# =============================================================================
# TARGETS
# =============================================================================

.PHONY: all clean run help

all: $(TARGET)

$(TARGET): $(OBJECTS) | $(BIN_DIR)
	$(CC) $(OBJECTS) -o $@
	@echo "Built: $@"

$(BUILD_DIR)/main.o: main.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/zone.o: $(ZONE_SRC) | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR):
	mkdir -p $@

$(BIN_DIR):
	mkdir -p $@

clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)
	@echo "Cleaned"

run: $(TARGET)
	./$(TARGET)

help:
	@echo "FUSE Mount Demo"
	@echo ""
	@echo "Targets:"
	@echo "  all     Build demo (default)"
	@echo "  run     Build and run demo"
	@echo "  clean   Remove build artifacts"
	@echo "  help    Show this help"

# =============================================================================
# CLOSING
# =============================================================================

# "And God saw that it was good" — Genesis 1:10
