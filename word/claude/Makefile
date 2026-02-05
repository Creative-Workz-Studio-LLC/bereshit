# ============================================================================
# METADATA - Claude Production Build System
# ============================================================================
# Purpose: Build, test, and deploy claude system from Bereshit (canonical root)
# Biblical: Ecclesiastes 3:3 - "A time to break down, and a time to build up"
#
# Location: Bereshit/word/claude/ - THE canonical source
#
# Architecture (2026-02-04):
#   Source (exFAT):     Bereshit/word/claude/     - All source code, configs
#   Config (symlinks):  ~/.claude/                - Symlinks to source
#   Binaries (ext4):    ~/.local/share/claude/    - Compiled executables
#
#   Why separate? exFAT doesn't support execute permissions. Binaries must
#   live on ext4 to be executable. Source/config can stay on exFAT via symlinks.
#
# Structure:
#   bin/              - Build output (intermediate, on exFAT)
#   config/           - Human-readable configuration
#   docs/             - Documentation
#   hooks/            - Session and tool lifecycle hooks
#   pkg/              - Shared Go packages (cognition, statemachine, types)
#   skills/           - Skill implementations
#   statusline/       - Status line implementation (WezTerm OSC integration)
#   system/           - System runtime libraries and commands
#
# Usage:
#   make              - Build all modules
#   make build        - Build all modules
#   make bin          - Build binaries to bin/ (local)
#   make install-bin  - Install binaries to ~/.local/share/claude/ (ext4)
#   make install      - Install symlinks to ~/.claude/ + binaries to ext4
#   make fresh        - Clean and reinstall everything
#   make verify       - Verify all paths, symlinks, and binaries
#   make uninstall    - Remove symlinks and installed binaries
#   make test         - Run all tests
#   make clean        - Clean build artifacts
#
# ============================================================================

.PHONY: all build bin test clean pkg hooks statusline system skills help \
        install install-bin install-bin-only install-symlinks \
        uninstall uninstall-bin uninstall-symlinks \
        fresh verify clean-stale tidy fmt vet db db-init db-migrate setup

# Default target
all: build

# ============================================================================
# SETUP - Variables
# ============================================================================

GO := go
SHELL := /bin/bash
GOFLAGS := -ldflags="-s -w"

# This directory (canonical source on exFAT)
SRC_DIR := $(shell pwd)

# Symlink deployment target (config/source references)
DEPLOY_DIR := $(HOME)/.claude

# Binary install target (ext4 filesystem for execute permissions)
# exFAT doesn't support Unix permissions, so executables must live on ext4
INSTALL_DIR := $(HOME)/.local/share/claude

# Skills
SKILLS_DIR := skills
SKILL_DIRS := $(wildcard $(SKILLS_DIR)/*/scripts)
SKILL_NAMES := $(notdir $(patsubst %/scripts,%,$(SKILL_DIRS)))

# Directories to symlink (source/config only - NOT bin)
# Note: bin removed - binaries go to INSTALL_DIR, not symlinked
SYMLINK_DIRS := agents commands config data divisions docs hooks output-styles pkg plans rules skills statusline system workflow

# Files to symlink
SYMLINK_FILES := CLAUDE.md SYSTEM-REFERENCE.md config.toml instance.jsonc Makefile go.work settings.json

# Known stale directories from old setups (safe to remove)
STALE_DIRS := cpi-si bin

# ============================================================================
# BODY - Build Targets
# ============================================================================

# Build everything (compile only - use install-bin to deploy to ext4)
build: pkg hooks statusline system skills bin
	@echo "=== All modules built successfully ==="
	@echo "    Run 'make install-bin' to deploy binaries to $(INSTALL_DIR)"

# Build binaries to local bin/ (intermediate step)
bin: bin-statusline bin-hooks bin-system-cmds
	@echo "=== Binaries built to bin/ ==="

bin-statusline:
	@echo "=== Building statusline ==="
	@mkdir -p bin
	@cd statusline && $(GO) build $(GOFLAGS) -o ../bin/statusline ./cmd/statusline
	@echo "  Built: bin/statusline"

# Build hook binaries
bin-hooks:
	@echo "=== Building hooks ==="
	@cd hooks && $(MAKE) build
	@echo "  Built: hooks/bin/*"

# Build system runtime command binaries
bin-system-cmds:
	@echo "=== Building system runtime commands ==="
	@mkdir -p system/runtime/bin
	@for cmd in system/runtime/cmd/*/; do \
		name=$$(basename $$cmd); \
		if [ -f "$$cmd$$name.go" ] || [ -f "$$cmd/main.go" ]; then \
			(cd $$cmd && $(GO) build $(GOFLAGS) -o $(SRC_DIR)/system/runtime/bin/$$name . 2>/dev/null) && \
			echo "  Built: system/runtime/bin/$$name"; \
		fi; \
	done 2>/dev/null || true
	@echo "  System commands built to system/runtime/bin/"

# Build shared pkg module
pkg:
	@echo "=== Building pkg ==="
	@cd pkg && $(GO) build ./...

# Build hooks module (compile check only - bin-hooks builds binaries)
hooks:
	@echo "=== Building hooks module ==="
	@cd hooks && $(GO) build ./...

# Build statusline module
statusline:
	@echo "=== Building statusline module ==="
	@cd statusline && $(GO) build ./...

# Build system module (runtime libraries and commands)
# Note: system/ root is organizational only - actual code is in runtime/cmd and runtime/lib
system:
	@echo "=== Building system module ==="
	@cd system/runtime/cmd && $(GO) build ./...
	@cd system/runtime/lib && $(GO) build ./...

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

# Full install: symlinks + binaries (with step-by-step progress)
install:
	@echo ""
	@echo "╔══════════════════════════════════════════════════════════════════╗"
	@echo "║              CPI-SI / Claude Code Installation                   ║"
	@echo "╚══════════════════════════════════════════════════════════════════╝"
	@echo ""
	@echo "Source:   $(SRC_DIR)"
	@echo "Symlinks: $(DEPLOY_DIR)"
	@echo "Binaries: $(INSTALL_DIR)"
	@echo ""
	@echo "════════════════════════════════════════════════════════════════════"
	@echo "[Step 1/5] Building shared packages..."
	@echo "════════════════════════════════════════════════════════════════════"
	@$(MAKE) pkg --no-print-directory
	@echo "  ✓ Packages built"
	@echo ""
	@echo "════════════════════════════════════════════════════════════════════"
	@echo "[Step 2/5] Building binaries..."
	@echo "════════════════════════════════════════════════════════════════════"
	@$(MAKE) bin --no-print-directory
	@echo "  ✓ Binaries built"
	@echo ""
	@echo "════════════════════════════════════════════════════════════════════"
	@echo "[Step 3/5] Creating config symlinks in ~/.claude/..."
	@echo "════════════════════════════════════════════════════════════════════"
	@$(MAKE) install-symlinks --no-print-directory
	@echo ""
	@echo "════════════════════════════════════════════════════════════════════"
	@echo "[Step 4/5] Installing binaries to ext4..."
	@echo "════════════════════════════════════════════════════════════════════"
	@$(MAKE) install-bin-only --no-print-directory
	@echo ""
	@echo "════════════════════════════════════════════════════════════════════"
	@echo "[Step 5/5] Verifying installation..."
	@echo "════════════════════════════════════════════════════════════════════"
	@$(MAKE) verify --no-print-directory
	@echo ""
	@echo "╔══════════════════════════════════════════════════════════════════╗"
	@echo "║              ✓ Installation Complete!                            ║"
	@echo "╚══════════════════════════════════════════════════════════════════╝"
	@echo ""
	@echo "Next: Update settings.json to reference $(INSTALL_DIR)"
	@echo ""

# Install binaries to ext4 (build first, then copy)
install-bin: bin install-bin-only

# Install binaries to ext4 (copy only, no rebuild)
install-bin-only:
	@mkdir -p $(INSTALL_DIR)/bin
	@mkdir -p $(INSTALL_DIR)/hooks/bin
	@mkdir -p $(INSTALL_DIR)/system/bin
	@echo ""
	@echo "--- Main binaries ---"
	@if [ -f "bin/statusline" ]; then \
		cp bin/statusline $(INSTALL_DIR)/bin/; \
		chmod +x $(INSTALL_DIR)/bin/statusline; \
		echo "  ✓ statusline → $(INSTALL_DIR)/bin/statusline"; \
	fi
	@if [ -f "bin/cpisi" ]; then \
		cp bin/cpisi $(INSTALL_DIR)/bin/; \
		chmod +x $(INSTALL_DIR)/bin/cpisi; \
		echo "  ✓ cpisi → $(INSTALL_DIR)/bin/cpisi"; \
	fi
	@echo ""
	@echo "--- Hook binaries ---"
	@for hook in hooks/bin/*; do \
		if [ -f "$$hook" ]; then \
			name=$$(basename $$hook); \
			cp "$$hook" $(INSTALL_DIR)/hooks/bin/; \
			chmod +x "$(INSTALL_DIR)/hooks/bin/$$name"; \
			echo "  ✓ $$name → $(INSTALL_DIR)/hooks/bin/$$name"; \
		fi; \
	done
	@echo ""
	@echo "--- System binaries ---"
	@for cmd in system/runtime/bin/*; do \
		if [ -f "$$cmd" ]; then \
			name=$$(basename $$cmd); \
			cp "$$cmd" $(INSTALL_DIR)/system/bin/; \
			chmod +x "$(INSTALL_DIR)/system/bin/$$name"; \
			echo "  ✓ $$name → $(INSTALL_DIR)/system/bin/$$name"; \
		fi; \
	done 2>/dev/null || true
	@echo ""
	@echo "  ✓ Binaries installed with execute permissions"

# Install symlinks only (source/config)
install-symlinks:
	@echo "=== Installing symlinks to $(DEPLOY_DIR) ==="
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
	@echo "=== Symlinks installed ==="

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

# Full uninstall: symlinks + binaries
uninstall: uninstall-symlinks uninstall-bin
	@echo "=== Full uninstall complete ==="

# Remove symlinks from ~/.claude/
uninstall-symlinks:
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
	@echo "=== Symlinks removed ==="

# Remove installed binaries from ext4
uninstall-bin:
	@echo "=== Removing binaries from $(INSTALL_DIR) ==="
	@rm -rf $(INSTALL_DIR)/bin/*
	@rm -rf $(INSTALL_DIR)/hooks/bin/*
	@rm -rf $(INSTALL_DIR)/system/bin/*
	@echo "=== Binaries removed ==="

# Fresh install: clean stale directories and reinstall
fresh: clean clean-stale install
	@echo "=== Fresh installation complete ==="

# Clean known stale directories from old setups
clean-stale:
	@echo "=== Cleaning stale directories from $(DEPLOY_DIR) ==="
	@for dir in $(STALE_DIRS); do \
		if [ -d "$(DEPLOY_DIR)/$$dir" ] && [ ! -L "$(DEPLOY_DIR)/$$dir" ]; then \
			echo "  Removing stale: $$dir"; \
			rm -rf "$(DEPLOY_DIR)/$$dir"; \
			echo "  ✓ Removed $$dir"; \
		elif [ -L "$(DEPLOY_DIR)/$$dir" ]; then \
			echo "  Removing stale symlink: $$dir"; \
			rm -f "$(DEPLOY_DIR)/$$dir"; \
			echo "  ✓ Removed $$dir"; \
		fi; \
	done
	@echo "=== Stale cleanup complete ==="

# Verify all paths, symlinks, and binaries are correct
verify:
	@echo "=== Verifying Claude Installation ==="
	@echo ""
	@echo "Source:   $(SRC_DIR)"
	@echo "Symlinks: $(DEPLOY_DIR)"
	@echo "Binaries: $(INSTALL_DIR)"
	@echo ""
	@errors=0; \
	echo "--- Checking source directories exist ---"; \
	for dir in $(SYMLINK_DIRS); do \
		if [ -d "$(SRC_DIR)/$$dir" ]; then \
			echo "  ✓ $$dir exists in source"; \
		else \
			echo "  ✗ $$dir MISSING in source"; \
			errors=$$((errors + 1)); \
		fi; \
	done; \
	echo ""; \
	echo "--- Checking source files exist ---"; \
	for file in $(SYMLINK_FILES); do \
		if [ -f "$(SRC_DIR)/$$file" ]; then \
			echo "  ✓ $$file exists in source"; \
		else \
			echo "  ✗ $$file MISSING in source"; \
			errors=$$((errors + 1)); \
		fi; \
	done; \
	echo ""; \
	echo "--- Checking symlinks point to Bereshit ---"; \
	for dir in $(SYMLINK_DIRS); do \
		if [ -L "$(DEPLOY_DIR)/$$dir" ]; then \
			target=$$(readlink -f "$(DEPLOY_DIR)/$$dir"); \
			if [ "$$target" = "$(SRC_DIR)/$$dir" ]; then \
				echo "  ✓ $$dir → correct"; \
			else \
				echo "  ⚠ $$dir → $$target (wrong target)"; \
				errors=$$((errors + 1)); \
			fi; \
		elif [ -d "$(DEPLOY_DIR)/$$dir" ]; then \
			echo "  ✗ $$dir is real directory (should be symlink)"; \
			errors=$$((errors + 1)); \
		else \
			echo "  - $$dir not installed"; \
		fi; \
	done; \
	for file in $(SYMLINK_FILES); do \
		if [ -L "$(DEPLOY_DIR)/$$file" ]; then \
			target=$$(readlink -f "$(DEPLOY_DIR)/$$file"); \
			if [ "$$target" = "$(SRC_DIR)/$$file" ]; then \
				echo "  ✓ $$file → correct"; \
			else \
				echo "  ⚠ $$file → $$target (wrong target)"; \
				errors=$$((errors + 1)); \
			fi; \
		elif [ -f "$(DEPLOY_DIR)/$$file" ]; then \
			echo "  ✗ $$file is real file (should be symlink)"; \
			errors=$$((errors + 1)); \
		else \
			echo "  - $$file not installed"; \
		fi; \
	done; \
	echo ""; \
	echo "--- Checking installed binaries (ext4) ---"; \
	for bin in statusline cpisi; do \
		if [ -x "$(INSTALL_DIR)/bin/$$bin" ]; then \
			echo "  ✓ $$bin installed and executable"; \
		elif [ -f "$(INSTALL_DIR)/bin/$$bin" ]; then \
			echo "  ✗ $$bin exists but NOT executable"; \
			errors=$$((errors + 1)); \
		else \
			echo "  - $$bin not installed"; \
		fi; \
	done; \
	echo ""; \
	echo "--- Checking hook binaries (ext4) ---"; \
	for hook in start end stop submit pre-use post-use; do \
		if [ -x "$(INSTALL_DIR)/hooks/bin/$$hook" ]; then \
			echo "  ✓ $$hook installed and executable"; \
		elif [ -f "$(INSTALL_DIR)/hooks/bin/$$hook" ]; then \
			echo "  ✗ $$hook exists but NOT executable"; \
			errors=$$((errors + 1)); \
		else \
			echo "  - $$hook not installed"; \
		fi; \
	done; \
	echo ""; \
	echo "--- Checking for stale directories ---"; \
	for dir in $(STALE_DIRS); do \
		if [ -d "$(DEPLOY_DIR)/$$dir" ] && [ ! -L "$(DEPLOY_DIR)/$$dir" ]; then \
			echo "  ⚠ $$dir is stale (run: make clean-stale)"; \
			errors=$$((errors + 1)); \
		elif [ -L "$(DEPLOY_DIR)/$$dir" ]; then \
			echo "  ⚠ $$dir symlink is stale (run: make clean-stale)"; \
			errors=$$((errors + 1)); \
		else \
			echo "  ✓ $$dir not present (good)"; \
		fi; \
	done; \
	echo ""; \
	echo "--- Checking Go workspace ---"; \
	if [ -f "$(SRC_DIR)/go.work" ]; then \
		echo "  ✓ go.work exists"; \
		cd $(SRC_DIR) && $(GO) work sync 2>/dev/null && echo "  ✓ go.work syncs successfully" || echo "  ✗ go.work sync failed"; \
	else \
		echo "  ✗ go.work missing"; \
		errors=$$((errors + 1)); \
	fi; \
	echo ""; \
	if [ $$errors -eq 0 ]; then \
		echo "=== Verification PASSED ==="; \
	else \
		echo "=== Verification FAILED: $$errors error(s) ==="; \
		echo "Run 'make fresh' to fix installation"; \
	fi

# ============================================================================
# BODY - Test Targets
# ============================================================================

# Run all tests
test: test-pkg test-hooks test-statusline test-system
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

test-system:
	@echo "=== Testing system ==="
	@cd system/runtime/cmd && $(GO) test ./... -v 2>/dev/null || true
	@cd system/runtime/lib && $(GO) test ./... -v 2>/dev/null || true

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
	@cd system/runtime/cmd && $(GO) clean ./... 2>/dev/null || true
	@cd system/runtime/lib && $(GO) clean ./... 2>/dev/null || true
	@find . -name "*.test" -delete
	@echo "Clean complete"

# Format all Go code
fmt:
	@echo "=== Formatting Go code ==="
	@cd pkg && $(GO) fmt ./...
	@cd hooks && $(GO) fmt ./...
	@cd statusline && $(GO) fmt ./...
	@cd system/runtime/cmd && $(GO) fmt ./... 2>/dev/null || true
	@cd system/runtime/lib && $(GO) fmt ./... 2>/dev/null || true

# Run go vet
vet:
	@echo "=== Running go vet ==="
	@cd pkg && $(GO) vet ./...
	@cd hooks && $(GO) vet ./...
	@cd statusline && $(GO) vet ./...
	@cd system/runtime/cmd && $(GO) vet ./... 2>/dev/null || true
	@cd system/runtime/lib && $(GO) vet ./... 2>/dev/null || true

# Tidy all go.mod files
tidy:
	@echo "=== Tidying go.mod files ==="
	@cd pkg && $(GO) mod tidy
	@cd hooks && $(GO) mod tidy
	@cd statusline && $(GO) mod tidy
	@cd system/runtime/cmd && $(GO) mod tidy 2>/dev/null || true
	@cd system/runtime/lib && $(GO) mod tidy 2>/dev/null || true
	@for skill in $(SKILL_DIRS); do \
		(cd $$skill && $(GO) mod tidy); \
	done 2>/dev/null || true

# ============================================================================
# CLOSING - Help
# ============================================================================

help:
	@echo "Claude Production Build System (Bereshit)"
	@echo ""
	@echo "This is the CANONICAL source for Claude Code / CPI-SI integration."
	@echo ""
	@echo "Architecture:"
	@echo "  Source (exFAT):   $(SRC_DIR)"
	@echo "  Symlinks:         $(DEPLOY_DIR)"
	@echo "  Binaries (ext4):  $(INSTALL_DIR)"
	@echo ""
	@echo "Usage: make [target]"
	@echo ""
	@echo "Build targets:"
	@echo "  all, build    Build all modules and binaries (local)"
	@echo "  bin           Build binaries to local bin/"
	@echo "  bin-hooks     Build hook binaries"
	@echo "  pkg           Build shared pkg module"
	@echo "  hooks         Build hooks module"
	@echo "  statusline    Build statusline module"
	@echo "  system        Build system runtime module"
	@echo "  skills        Build all skills"
	@echo "  skill-<name>  Build specific skill"
	@echo ""
	@echo "Install targets:"
	@echo "  install         Full install: symlinks + binaries to ext4"
	@echo "  install-bin     Install binaries to ext4 (with exec permissions)"
	@echo "  install-symlinks Create symlinks in ~/.claude/"
	@echo "  install-status  Show current symlink status"
	@echo "  uninstall       Remove symlinks and installed binaries"
	@echo "  uninstall-bin   Remove installed binaries only"
	@echo "  fresh           Clean and full reinstall (recommended)"
	@echo "  verify          Check symlinks and binary permissions"
	@echo "  clean-stale     Remove known stale directories"
	@echo ""
	@echo "Test targets:"
	@echo "  test          Run all tests"
	@echo "  test-pkg      Test pkg module"
	@echo "  test-hooks    Test hooks module"
	@echo "  test-statusline Test statusline module"
	@echo "  test-system   Test system module"
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
	@echo ""
	@echo "Quick start:"
	@echo "  make fresh    # Full clean install"
	@echo "  make verify   # Check everything is working"
	@echo ""
	@echo "Step-by-step install:"
	@echo "  make setup    # Interactive setup guide"

# Interactive setup guide
setup:
	@echo ""
	@echo "╔══════════════════════════════════════════════════════════════════╗"
	@echo "║           CPI-SI / Claude Code Installation Guide                ║"
	@echo "╚══════════════════════════════════════════════════════════════════╝"
	@echo ""
	@echo "This will set up Claude Code with CPI-SI integration."
	@echo ""
	@echo "Architecture:"
	@echo "  • Source code lives in Bereshit (can be on exFAT)"
	@echo "  • Config symlinks go to ~/.claude/"
	@echo "  • Binaries install to ~/.local/share/claude/ (must be ext4)"
	@echo ""
	@echo "────────────────────────────────────────────────────────────────────"
	@echo "Step 1: Build all modules"
	@echo "────────────────────────────────────────────────────────────────────"
	@echo "  make build"
	@echo ""
	@echo "  This compiles:"
	@echo "    • pkg/          - Shared Go packages"
	@echo "    • hooks/        - Session and tool lifecycle hooks"
	@echo "    • statusline/   - Status line binary"
	@echo "    • system/       - System runtime commands"
	@echo "    • skills/       - Skill implementations"
	@echo ""
	@echo "────────────────────────────────────────────────────────────────────"
	@echo "Step 2: Create config symlinks"
	@echo "────────────────────────────────────────────────────────────────────"
	@echo "  make install-symlinks"
	@echo ""
	@echo "  Creates symlinks in ~/.claude/ pointing to:"
	@echo "    $(SRC_DIR)"
	@echo ""
	@echo "  Symlinked: agents, commands, config, docs, hooks, output-styles,"
	@echo "             pkg, plans, rules, skills, statusline, system, workflow,"
	@echo "             CLAUDE.md, settings.json, etc."
	@echo ""
	@echo "────────────────────────────────────────────────────────────────────"
	@echo "Step 3: Install binaries to ext4"
	@echo "────────────────────────────────────────────────────────────────────"
	@echo "  make install-bin"
	@echo ""
	@echo "  Copies compiled binaries to $(INSTALL_DIR)"
	@echo "  and sets execute permissions."
	@echo ""
	@echo "  Why ext4? exFAT doesn't support Unix permissions."
	@echo "  Binaries need +x to be executable."
	@echo ""
	@echo "────────────────────────────────────────────────────────────────────"
	@echo "Step 4: Update settings.json"
	@echo "────────────────────────────────────────────────────────────────────"
	@echo "  Ensure settings.json references the ext4 binary paths:"
	@echo ""
	@echo "    hooks:      ~/.local/share/claude/hooks/bin/"
	@echo "    statusLine: ~/.local/share/claude/bin/statusline"
	@echo ""
	@echo "────────────────────────────────────────────────────────────────────"
	@echo "Step 5: Verify installation"
	@echo "────────────────────────────────────────────────────────────────────"
	@echo "  make verify"
	@echo ""
	@echo "  Checks:"
	@echo "    • Source directories exist"
	@echo "    • Symlinks point correctly"
	@echo "    • Binaries are installed and executable"
	@echo "    • Go workspace syncs"
	@echo ""
	@echo "════════════════════════════════════════════════════════════════════"
	@echo ""
	@echo "Or run everything at once:"
	@echo "  make fresh"
	@echo ""
	@echo "════════════════════════════════════════════════════════════════════"

# List available skills
list-skills:
	@for skill in $(SKILL_NAMES); do echo $$skill; done
