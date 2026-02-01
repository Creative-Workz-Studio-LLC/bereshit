# ============================================================================
# METADATA - Claude Production Build System
# ============================================================================
# Purpose: Build, test, and deploy claude system from Bereshit (canonical root)
# Biblical: Ecclesiastes 3:3 - "A time to break down, and a time to build up"
#
# Location: Bereshit/word/claude/ - THE canonical source
# Deploy to: ~/.claude/ via symlinks
#
# Structure (2026-01-29):
#   bin/              - Compiled binaries (statusline)
#   config/           - Human-readable configuration
#   docs/             - Documentation
#   hooks/            - Session and tool lifecycle hooks
#   pkg/              - Shared Go packages
#   skills/           - Skill implementations
#   statusline/       - Status line implementation
#
# Usage:
#   make              - Build all modules
#   make build        - Build all modules
#   make bin          - Build binaries to bin/
#   make install      - Install symlinks to ~/.claude/
#   make uninstall    - Remove symlinks from ~/.claude/
#   make test         - Run all tests
#   make clean        - Clean build artifacts
#
# ============================================================================

.PHONY: all build bin test clean pkg hooks statusline skills help install uninstall tidy fmt vet db db-init db-migrate

# Default target
all: build

# ============================================================================
# SETUP - Variables
# ============================================================================

GO := go
SHELL := /bin/bash

# This directory (canonical source)
SRC_DIR := $(shell pwd)

# Deployment target
DEPLOY_DIR := $(HOME)/.claude

# Skills
SKILLS_DIR := skills
SKILL_DIRS := $(wildcard $(SKILLS_DIR)/*/scripts)
SKILL_NAMES := $(notdir $(patsubst %/scripts,%,$(SKILL_DIRS)))

# Directories to symlink
SYMLINK_DIRS := agents bin commands config data divisions docs hooks output-styles pkg plans rules skills statusline system workflow

# Files to symlink
SYMLINK_FILES := CLAUDE.md SYSTEM-REFERENCE.md config.toml instance.jsonc Makefile

# ============================================================================
# BODY - Build Targets
# ============================================================================

# Build everything
build: pkg hooks statusline skills bin
	@echo "=== All modules built successfully ==="

# Build binaries to bin/
bin: bin-statusline
	@echo "=== Binaries built to bin/ ==="

bin-statusline:
	@echo "=== Building statusline ==="
	@mkdir -p bin
	@cd statusline && $(GO) build -o ../bin/statusline ./cmd/statusline
	@echo "Built: bin/statusline"

# Build shared pkg module
pkg:
	@echo "=== Building pkg ==="
	@cd pkg && $(GO) build ./...

# Build hooks module
hooks:
	@echo "=== Building hooks ==="
	@cd hooks && $(GO) build ./...

# Build statusline module
statusline:
	@echo "=== Building statusline module ==="
	@cd statusline && $(GO) build ./...

# Build all skills
skills: $(addprefix skill-,$(SKILL_NAMES))
	@echo "=== All skills built ==="

# Pattern rule for individual skills
skill-%:
	@echo "=== Building skill: $* ==="
	@if [ -d "$(SKILLS_DIR)/$*/scripts" ]; then \
		cd $(SKILLS_DIR)/$*/scripts && $(GO) build ./...; \
	else \
		echo "Skill not found: $*"; \
		exit 1; \
	fi

# ============================================================================
# BODY - Install/Uninstall Targets
# ============================================================================

# Install symlinks to ~/.claude/
install: build
	@echo "=== Installing to $(DEPLOY_DIR) ==="
	@echo ""
	@mkdir -p $(DEPLOY_DIR)
	@echo "--- Creating directory symlinks ---"
	@for dir in $(SYMLINK_DIRS); do \
		if [ -d "$(SRC_DIR)/$$dir" ]; then \
			if [ -L "$(DEPLOY_DIR)/$$dir" ]; then \
				rm -f "$(DEPLOY_DIR)/$$dir"; \
			elif [ -d "$(DEPLOY_DIR)/$$dir" ]; then \
				echo "  ⚠ Backing up existing $$dir to $$dir.bak"; \
				mv "$(DEPLOY_DIR)/$$dir" "$(DEPLOY_DIR)/$$dir.bak"; \
			fi; \
			ln -sf "$(SRC_DIR)/$$dir" "$(DEPLOY_DIR)/$$dir"; \
			echo "  ✓ $$dir → $(SRC_DIR)/$$dir"; \
		fi; \
	done
	@echo ""
	@echo "--- Creating file symlinks ---"
	@for file in $(SYMLINK_FILES); do \
		if [ -f "$(SRC_DIR)/$$file" ]; then \
			if [ -L "$(DEPLOY_DIR)/$$file" ]; then \
				rm -f "$(DEPLOY_DIR)/$$file"; \
			elif [ -f "$(DEPLOY_DIR)/$$file" ]; then \
				echo "  ⚠ Backing up existing $$file to $$file.bak"; \
				mv "$(DEPLOY_DIR)/$$file" "$(DEPLOY_DIR)/$$file.bak"; \
			fi; \
			ln -sf "$(SRC_DIR)/$$file" "$(DEPLOY_DIR)/$$file"; \
			echo "  ✓ $$file → $(SRC_DIR)/$$file"; \
		fi; \
	done
	@echo ""
	@echo "=== Installation complete ==="
	@echo ""
	@echo "Source: $(SRC_DIR)"
	@echo "Target: $(DEPLOY_DIR)"

# Show current symlink status
install-status:
	@echo "=== Symlink Status ==="
	@echo ""
	@echo "Target: $(DEPLOY_DIR)"
	@echo "Source: $(SRC_DIR)"
	@echo ""
	@echo "Directories:"
	@for dir in $(SYMLINK_DIRS); do \
		if [ -L "$(DEPLOY_DIR)/$$dir" ]; then \
			target=$$(readlink -f "$(DEPLOY_DIR)/$$dir"); \
			if [ "$$target" = "$(SRC_DIR)/$$dir" ]; then \
				echo "  ✓ $$dir → $$target"; \
			else \
				echo "  ⚠ $$dir → $$target (not pointing to Bereshit)"; \
			fi; \
		elif [ -d "$(DEPLOY_DIR)/$$dir" ]; then \
			echo "  ✗ $$dir (real directory, not symlink)"; \
		else \
			echo "  - $$dir (not present)"; \
		fi; \
	done
	@echo ""
	@echo "Files:"
	@for file in $(SYMLINK_FILES); do \
		if [ -L "$(DEPLOY_DIR)/$$file" ]; then \
			target=$$(readlink -f "$(DEPLOY_DIR)/$$file"); \
			if [ "$$target" = "$(SRC_DIR)/$$file" ]; then \
				echo "  ✓ $$file → $$target"; \
			else \
				echo "  ⚠ $$file → $$target (not pointing to Bereshit)"; \
			fi; \
		elif [ -f "$(DEPLOY_DIR)/$$file" ]; then \
			echo "  ✗ $$file (real file, not symlink)"; \
		else \
			echo "  - $$file (not present)"; \
		fi; \
	done

# Remove symlinks from ~/.claude/
uninstall:
	@echo "=== Removing symlinks from $(DEPLOY_DIR) ==="
	@for dir in $(SYMLINK_DIRS); do \
		if [ -L "$(DEPLOY_DIR)/$$dir" ]; then \
			rm -f "$(DEPLOY_DIR)/$$dir"; \
			echo "  ✓ Removed $$dir"; \
		fi; \
	done
	@for file in $(SYMLINK_FILES); do \
		if [ -L "$(DEPLOY_DIR)/$$file" ]; then \
			rm -f "$(DEPLOY_DIR)/$$file"; \
			echo "  ✓ Removed $$file"; \
		fi; \
	done
	@echo "=== Uninstall complete ==="

# ============================================================================
# BODY - Test Targets
# ============================================================================

# Run all tests
test: test-pkg test-hooks test-statusline
	@echo "=== All tests passed ==="

test-pkg:
	@echo "=== Testing pkg ==="
	@cd pkg && $(GO) test ./... -v

test-hooks:
	@echo "=== Testing hooks ==="
	@cd hooks && $(GO) test ./... -v

test-statusline:
	@echo "=== Testing statusline ==="
	@cd statusline && $(GO) test ./... -v

# ============================================================================
# BODY - Database Targets
# ============================================================================

DATA_DIR := data
DB_PATH := $(DATA_DIR)/cpisi.db

db-init:
	@echo "=== Initializing database ==="
	@mkdir -p $(DATA_DIR)
	@if command -v sqlite3 >/dev/null 2>&1; then \
		if [ -f "$(DATA_DIR)/schema/001_initial.sql" ]; then \
			sqlite3 $(DB_PATH) < $(DATA_DIR)/schema/001_initial.sql && \
			echo "Database initialized at $(DB_PATH)"; \
		else \
			echo "Schema file not found"; \
		fi; \
	else \
		echo "sqlite3 CLI not found. Database will be initialized by Go code."; \
	fi

db-migrate:
	@echo "=== Running database migrations ==="
	@for f in $(DATA_DIR)/migrations/*.sql; do \
		[ -f "$$f" ] && sqlite3 $(DB_PATH) < "$$f" && echo "Applied: $$f"; \
	done 2>/dev/null || true
	@echo "Migrations complete"

db-status:
	@echo "=== Database Status ==="
	@if [ -f $(DB_PATH) ]; then \
		echo "Database: $(DB_PATH)"; \
		echo "Size: $$(du -h $(DB_PATH) | cut -f1)"; \
		sqlite3 $(DB_PATH) ".tables" 2>/dev/null || true; \
	else \
		echo "Database not initialized. Run: make db-init"; \
	fi

db: db-init db-migrate

# ============================================================================
# BODY - Utility Targets
# ============================================================================

# Clean build artifacts
clean:
	@echo "=== Cleaning build artifacts ==="
	@rm -rf bin/
	@cd pkg && $(GO) clean ./... 2>/dev/null || true
	@cd hooks && $(GO) clean ./... 2>/dev/null || true
	@cd statusline && $(GO) clean ./... 2>/dev/null || true
	@find . -name "*.test" -delete
	@echo "Clean complete"

# Format all Go code
fmt:
	@echo "=== Formatting Go code ==="
	@cd pkg && $(GO) fmt ./...
	@cd hooks && $(GO) fmt ./...
	@cd statusline && $(GO) fmt ./...

# Run go vet
vet:
	@echo "=== Running go vet ==="
	@cd pkg && $(GO) vet ./...
	@cd hooks && $(GO) vet ./...
	@cd statusline && $(GO) vet ./...

# Tidy all go.mod files
tidy:
	@echo "=== Tidying go.mod files ==="
	@cd pkg && $(GO) mod tidy
	@cd hooks && $(GO) mod tidy
	@cd statusline && $(GO) mod tidy
	@for skill in $(SKILL_DIRS); do \
		(cd $$skill && $(GO) mod tidy); \
	done 2>/dev/null || true

# ============================================================================
# CLOSING - Help
# ============================================================================

help:
	@echo "Claude Production Build System (Bereshit)"
	@echo ""
	@echo "This is the CANONICAL source for Claude Code integration."
	@echo "Location: $(SRC_DIR)"
	@echo ""
	@echo "Usage: make [target]"
	@echo ""
	@echo "Build targets:"
	@echo "  all, build    Build all modules and binaries"
	@echo "  bin           Build binaries to bin/"
	@echo "  pkg           Build shared pkg module"
	@echo "  hooks         Build hooks module"
	@echo "  statusline    Build statusline module"
	@echo "  skills        Build all skills"
	@echo "  skill-<name>  Build specific skill"
	@echo ""
	@echo "Install targets:"
	@echo "  install       Create symlinks in ~/.claude/ pointing here"
	@echo "  install-status Show current symlink status"
	@echo "  uninstall     Remove symlinks from ~/.claude/"
	@echo ""
	@echo "Test targets:"
	@echo "  test          Run all tests"
	@echo "  test-pkg      Test pkg module"
	@echo "  test-hooks    Test hooks module"
	@echo "  test-statusline Test statusline module"
	@echo ""
	@echo "Database targets:"
	@echo "  db            Initialize and migrate database"
	@echo "  db-init       Initialize database schema"
	@echo "  db-migrate    Run pending migrations"
	@echo "  db-status     Show database status"
	@echo ""
	@echo "Utility targets:"
	@echo "  clean         Clean build artifacts"
	@echo "  fmt           Format all Go code"
	@echo "  vet           Run go vet on all modules"
	@echo "  tidy          Tidy all go.mod files"
	@echo "  help          Show this help"

# List available skills
list-skills:
	@for skill in $(SKILL_NAMES); do echo $$skill; done
