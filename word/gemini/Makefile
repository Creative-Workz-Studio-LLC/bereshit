# ============================================================================
# METADATA - Gemini Substrate Build System
# ============================================================================
#
# Purpose: Build and deploy Gemini CLI integration (mirrors Claude pattern)
# Key:     B-gemini-Makefile
#
# ============================================================================

.PHONY: all build install install-symlinks install-bin clean help

# Paths
SRC_DIR     := $(CURDIR)
DEPLOY_DIR  := $(HOME)/.gemini
INSTALL_DIR := $(HOME)/.local/share/gemini

# Symlink targets (from repo to ~/)
SYMLINK_FILES := settings.json .geminiignore env.sh config.jsonc
SYMLINK_DIRS  := hooks

# Default target
all: build

build:
	@echo "  Gemini substrate is configuration-driven. No separate binary build needed."

install: install-symlinks install-bin
	@echo "  Gemini substrate installed and mirrored."

install-symlinks:
	@mkdir -p $(DEPLOY_DIR)
	@echo "  Creating Gemini symlinks..."
	@for file in $(SYMLINK_FILES); do 
		if [ -f "$(SRC_DIR)/$$file" ]; then 
			[ -L "$(DEPLOY_DIR)/$$file" ] && rm -f "$(DEPLOY_DIR)/$$file"; 
			[ -f "$(DEPLOY_DIR)/$$file" ] && mv "$(DEPLOY_DIR)/$$file" "$(DEPLOY_DIR)/$$file.bak"; 
			ln -sf "$(SRC_DIR)/$$file" "$(DEPLOY_DIR)/$$file"; 
			echo "    + $$file"; 
		fi; 
	done
	@for dir in $(SYMLINK_DIRS); do 
		if [ -d "$(SRC_DIR)/$$dir" ]; then 
			[ -L "$(DEPLOY_DIR)/$$dir" ] && rm -f "$(DEPLOY_DIR)/$$dir"; 
			[ -d "$(DEPLOY_DIR)/$$dir" ] && mv "$(DEPLOY_DIR)/$$dir" "$(DEPLOY_DIR)/$$dir.bak"; 
			ln -sf "$(SRC_DIR)/$$dir" "$(DEPLOY_DIR)/$$dir"; 
			echo "    + $$dir"; 
		fi; 
	done

install-bin:
	@mkdir -p $(INSTALL_DIR)/bin
	@echo "  Binary deployment for Gemini (ext4):"
	@# Currently Gemini uses the universal hooks in ~/.local/share/claude/bin
	@# In Phase 2, we will deploy dedicated gemini-* binaries here.
	@echo "    (Using universal SDK binaries)"

clean:
	@echo "  Cleaning Gemini build artifacts..."

help:
	@echo ""
	@echo "Gemini Substrate Build System"
	@echo "----------------------------"
	@echo "Targets:"
	@echo "  build            - Validate configuration"
	@echo "  install          - Full deployment (symlinks + binaries)"
	@echo "  install-symlinks - Link ~/.gemini back to repository"
	@echo "  clean            - Remove artifacts"
	@echo ""
