# ============================================================================
# METADATA - Claude Global Build System
# ============================================================================
# Purpose: Build, test, and manage all claude-global modules
# Biblical: Ecclesiastes 3:3 - "A time to break down, and a time to build up"
#
# Structure (2025-12-22):
#   bin/              - Compiled binaries (cpisi, statusline)
#   config/           - Human-readable configuration
#   docs/             - Documentation
#   entrypoints/      - Go main() packages
#   hooks/            - Session and tool lifecycle hooks
#   pkg/              - Shared Go packages
#   skills/           - Skill implementations
#   statusline/       - Status line implementation
#
# Usage:
#   make          - Build all modules
#   make build    - Build all modules
#   make bin      - Build binaries (cpisi, statusline) to bin/
#   make test     - Run all tests
#   make clean    - Clean build artifacts
#   make pkg      - Build shared pkg module
#   make hooks    - Build hooks module
#   make statusline - Build statusline module
#   make skills   - Build all skills
#   make skill-<name> - Build specific skill (e.g., make skill-format-lookup)
#
# ============================================================================

.PHONY: all build bin test clean pkg hooks statusline skills help sync sync-code sync-status deploy db db-init db-migrate tidy

# Default target
all: build

# ============================================================================
# SETUP - Variables
# ============================================================================

GO := go
SKILLS_DIR := skills
SKILL_DIRS := $(wildcard $(SKILLS_DIR)/*/scripts)
SKILL_NAMES := $(notdir $(patsubst %/scripts,%,$(SKILL_DIRS)))

# ============================================================================
# BODY - Build Targets
# ============================================================================

# Build everything
build: pkg hooks statusline skills bin
	@echo "=== All modules built successfully ==="

# Build binaries to bin/
bin:
	@echo "=== Building binaries to bin/ ==="
	@mkdir -p bin
	@$(GO) build -o bin/cpisi ./entrypoints/cpisi
	@$(GO) build -o bin/statusline ./statusline/cmd/statusline
	@echo "Built: bin/cpisi, bin/statusline"

# Build shared pkg module
pkg:
	@echo "=== Building pkg ==="
	@$(GO) build ./pkg/...

# Build hooks module
hooks:
	@echo "=== Building hooks ==="
	@$(GO) build ./hooks/...

# Build statusline module
statusline:
	@echo "=== Building statusline ==="
	@$(GO) build ./statusline/...

# Build all skills
skills: $(addprefix skill-,$(SKILL_NAMES))
	@echo "=== All skills built ==="

# Pattern rule for individual skills
skill-%:
	@echo "=== Building skill: $* ==="
	@if [ -d "$(SKILLS_DIR)/$*/scripts" ]; then \
		$(GO) build ./$(SKILLS_DIR)/$*/scripts/...; \
	else \
		echo "Skill not found: $*"; \
		exit 1; \
	fi

# ============================================================================
# BODY - Test Targets
# ============================================================================

# Run all tests
test: test-pkg test-hooks test-statusline test-skills
	@echo "=== All tests passed ==="

test-pkg:
	@echo "=== Testing pkg ==="
	@$(GO) test ./pkg/... -v

test-hooks:
	@echo "=== Testing hooks ==="
	@$(GO) test ./hooks/... -v

test-statusline:
	@echo "=== Testing statusline ==="
	@$(GO) test ./statusline/... -v

test-skills:
	@echo "=== Testing skills ==="
	@for skill in $(SKILL_DIRS); do \
		echo "Testing $$skill..."; \
		$(GO) test ./$$skill/... -v || exit 1; \
	done

# ============================================================================
# BODY - Deployment Targets
# ============================================================================

# Production locations (relative paths for portability)
# From claude-global/, PROD_DIR is sibling under CreativeWorkzStudio_LLC
PROD_DIR := ../bereshit/word/claude
DEPLOY_DIR := $(HOME)/.claude

# Core directories to sync (Go modules with code)
SYNC_CODE_DIRS := hooks pkg statusline skills

# Configuration and documentation directories
SYNC_CONFIG_DIRS := config agents commands docs output-styles plans rules system

# Data directories (schema and migrations, not runtime data)
SYNC_DATA_DIRS := data/schema data/migrations

# Files to sync at root level
# NOTE: go.work is NOT synced - each location (dev/prod) has its own workspace config
# NOTE: settings.jsonc moved to holding (was outdated reference doc)
SYNC_ROOT_FILES := instance.jsonc settings.json config.toml CLAUDE.md

# Sync development to production - comprehensive
sync: build
	@echo "=== Syncing claude-global → bereshit/word/claude ==="
	@echo ""
	@echo "--- Syncing binaries ---"
	@echo "  ↳ bin/"
	@rsync -av --delete bin/ $(PROD_DIR)/bin/ 2>/dev/null || true
	@echo ""
	@echo "--- Syncing Go modules (code) ---"
	@for dir in $(SYNC_CODE_DIRS); do \
		echo "  ↳ $$dir/"; \
		rsync -av --delete $$dir/ $(PROD_DIR)/$$dir/ --exclude 'bin/' --exclude '*.test' 2>/dev/null || true; \
	done
	@echo ""
	@echo "--- Syncing config/docs ---"
	@for dir in $(SYNC_CONFIG_DIRS); do \
		if [ -d "$$dir" ]; then \
			echo "  ↳ $$dir/"; \
			rsync -av --delete $$dir/ $(PROD_DIR)/$$dir/ 2>/dev/null || true; \
		fi; \
	done
	@echo ""
	@echo "--- Syncing data directories ---"
	@for dir in $(SYNC_DATA_DIRS); do \
		if [ -d "$$dir" ]; then \
			echo "  ↳ $$dir/"; \
			mkdir -p $(PROD_DIR)/$$dir; \
			rsync -av $$dir/ $(PROD_DIR)/$$dir/ 2>/dev/null || true; \
		fi; \
	done
	@echo ""
	@echo "--- Syncing root files ---"
	@for file in $(SYNC_ROOT_FILES); do \
		if [ -f "$$file" ]; then \
			echo "  ↳ $$file"; \
			cp -f $$file $(PROD_DIR)/$$file 2>/dev/null || true; \
		fi; \
	done
	@echo ""
	@echo "=== Sync complete ==="

# Quick sync - only code directories (faster)
sync-code: build
	@echo "=== Quick sync (code only) ==="
	@for dir in $(SYNC_CODE_DIRS); do \
		rsync -av --delete $$dir/ $(PROD_DIR)/$$dir/ --exclude 'bin/' --exclude '*.test' 2>/dev/null || true; \
	done
	@echo "=== Quick sync complete ==="

# Sync status - show what would be synced
sync-status:
	@echo "=== Sync Status ==="
	@echo ""
	@echo "Source: $(CURDIR)"
	@echo "Target: $(PROD_DIR)"
	@echo ""
	@echo "Code directories:"
	@for dir in $(SYNC_CODE_DIRS); do \
		if [ -d "$$dir" ]; then \
			count=$$(find $$dir -name "*.go" 2>/dev/null | wc -l); \
			echo "  ✓ $$dir/ ($$count .go files)"; \
		else \
			echo "  ✗ $$dir/ (missing)"; \
		fi; \
	done
	@echo ""
	@echo "Config directories:"
	@for dir in $(SYNC_CONFIG_DIRS); do \
		if [ -d "$$dir" ]; then \
			echo "  ✓ $$dir/"; \
		else \
			echo "  ✗ $$dir/ (missing)"; \
		fi; \
	done
	@echo ""
	@echo "Root files:"
	@for file in $(SYNC_ROOT_FILES); do \
		if [ -f "$$file" ]; then \
			echo "  ✓ $$file"; \
		else \
			echo "  ✗ $$file (missing)"; \
		fi; \
	done

# Deploy: sync to bereshit/word/claude + rebuild hooks
# NOTE: ~/.claude/ uses symlinks to bereshit/word/claude/, so sync is sufficient.
#       No separate deploy-home needed - symlinks handle it automatically.
deploy: sync
	@echo "=== Rebuilding hooks in production ==="
	@cd $(PROD_DIR)/hooks && make all && make sync 2>/dev/null || true
	@echo "=== Full deployment complete ==="
	@echo ""
	@echo "Deployment chain:"
	@echo "  claude-global/ ─[rsync]─> bereshit/word/claude/ <─[symlinks]─ ~/.claude/"

# ============================================================================
# BODY - Database Targets
# ============================================================================

# Database directory
DATA_DIR := data
DB_PATH := $(DATA_DIR)/cpisi.db

# Initialize database (create if not exists, run migrations)
# Note: If sqlite3 CLI is not available, the Go code handles initialization
# via the Migrate() method when NewSQLiteRepository is first called.
db-init:
	@echo "=== Initializing database ==="
	@mkdir -p $(DATA_DIR)
	@if command -v sqlite3 >/dev/null 2>&1; then \
		sqlite3 $(DB_PATH) < $(DATA_DIR)/schema/001_initial.sql && \
		echo "Database initialized at $(DB_PATH)"; \
	else \
		echo "sqlite3 CLI not found. Database will be initialized by Go code."; \
		echo "Run any hook or tool that uses the database to auto-initialize."; \
	fi

# Run pending migrations
db-migrate:
	@echo "=== Running database migrations ==="
	@for f in $(DATA_DIR)/migrations/*.sql; do \
		[ -f "$$f" ] && sqlite3 $(DB_PATH) < "$$f" && echo "Applied: $$f"; \
	done 2>/dev/null || true
	@echo "Migrations complete"

# Database status
db-status:
	@echo "=== Database Status ==="
	@if [ -f $(DB_PATH) ]; then \
		echo "Database: $(DB_PATH)"; \
		echo "Size: $$(du -h $(DB_PATH) | cut -f1)"; \
		echo "Tables:"; \
		sqlite3 $(DB_PATH) ".tables"; \
	else \
		echo "Database not initialized. Run: make db-init"; \
	fi

# Combined db target
db: db-init db-migrate
	@echo "=== Database ready ==="

# Open database in GUI browser
db-browse:
	@echo "=== Opening DB Browser ==="
	@sqlitebrowser $(DB_PATH) &

# ============================================================================
# BODY - Utility Targets
# ============================================================================

# Clean build artifacts
clean:
	@echo "=== Cleaning build artifacts ==="
	@$(GO) clean ./...
	@find . -name "*.test" -delete
	@find . -type d -name "bin" -exec rm -rf {} + 2>/dev/null || true
	@echo "Clean complete"

# Format all Go code
fmt:
	@echo "=== Formatting Go code ==="
	@$(GO) fmt ./...

# Run go vet
vet:
	@echo "=== Running go vet ==="
	@$(GO) vet ./pkg/...
	@$(GO) vet ./hooks/...
	@$(GO) vet ./statusline/...
	@for skill in $(SKILL_DIRS); do \
		$(GO) vet ./$$skill/...; \
	done

# Tidy all go.mod files
tidy:
	@echo "=== Tidying go.mod files ==="
	@cd pkg && $(GO) mod tidy
	@cd hooks && $(GO) mod tidy
	@cd statusline && $(GO) mod tidy
	@for skill in $(SKILL_DIRS); do \
		cd $$skill && $(GO) mod tidy && cd -; \
	done

# ============================================================================
# CLOSING - Help
# ============================================================================

help:
	@echo "Claude Global Build System"
	@echo ""
	@echo "Usage: make [target]"
	@echo ""
	@echo "Build targets:"
	@echo "  all, build    Build all modules and binaries"
	@echo "  bin           Build binaries (cpisi, statusline) to bin/"
	@echo "  pkg           Build shared pkg module"
	@echo "  hooks         Build hooks module"
	@echo "  statusline    Build statusline module"
	@echo "  skills        Build all skills"
	@echo "  skill-<name>  Build specific skill"
	@echo ""
	@echo "Deployment targets:"
	@echo "  sync          Full sync to bereshit/word/claude (code, config, docs, bin)"
	@echo "  sync-code     Quick sync (code directories only)"
	@echo "  sync-status   Show what would be synced"
	@echo "  deploy        Full deploy: sync + rebuild hooks (~/.claude uses symlinks)"
	@echo ""
	@echo "Test targets:"
	@echo "  test          Run all tests"
	@echo "  test-pkg      Test pkg module"
	@echo "  test-hooks    Test hooks module"
	@echo "  test-statusline Test statusline module"
	@echo "  test-skills   Test all skills"
	@echo ""
	@echo "Database targets:"
	@echo "  db            Initialize and migrate database"
	@echo "  db-init       Initialize database schema"
	@echo "  db-migrate    Run pending migrations"
	@echo "  db-status     Show database status"
	@echo "  db-browse     Open database in GUI browser"
	@echo ""
	@echo "Utility targets:"
	@echo "  clean         Clean build artifacts"
	@echo "  fmt           Format all Go code"
	@echo "  vet           Run go vet on all modules"
	@echo "  tidy          Tidy all go.mod files"
	@echo "  help          Show this help"
	@echo ""
	@echo "Available skills:"
	@for skill in $(SKILL_NAMES); do echo "  skill-$$skill"; done

# List all skills (useful for scripting)
list-skills:
	@for skill in $(SKILL_NAMES); do echo $$skill; done
