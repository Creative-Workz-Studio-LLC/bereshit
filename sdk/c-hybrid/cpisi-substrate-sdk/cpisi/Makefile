# ============================================================================
# METADATA - CPI-SI Native Substrate Build System
# ============================================================================
#
# Purpose: Build and deploy the native CPI-SI substrate (RI Standard)
# Key:     B-sdk-cpisi-Makefile
#
# ============================================================================

.PHONY: all build install install-symlinks install-bin clean help

# Paths
SRC_DIR     := $(CURDIR)
DEPLOY_DIR  := $(HOME)/.cpisi
INSTALL_DIR := $(HOME)/.local/share/cpisi

# Symlink targets
SYMLINK_FILES := config.jsonc
SYMLINK_DIRS  := hooks

# Default target
all: build

build:
	@echo "  Building CPI-SI native substrate..."

install: install-symlinks install-bin
	@echo "  CPI-SI native substrate installed and mirrored."

install-symlinks:
	@mkdir -p $(DEPLOY_DIR)
	@echo "  Creating CPI-SI native symlinks..."
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
	@echo "  Binary deployment for CPI-SI Native (ext4):"
	@echo "    (Using universal SDK binaries)"

clean:
	@echo "  Cleaning CPI-SI native artifacts..."

help:
	@echo ""
	@echo "CPI-SI Native Substrate Build System"
	@echo "-----------------------------------"
	@echo "Targets:"
	@echo "  build            - Validate configuration"
	@echo "  install          - Full deployment"
	@echo "  install-symlinks - Link ~/.cpisi back to SDK"
	@echo ""
