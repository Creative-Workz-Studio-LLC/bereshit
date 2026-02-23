# ============================================================================
# METADATA
# ============================================================================
#
# file:    Makefile
# key:     B-omni-tooling-vscode-makefile
# title:   OmniCode VSCode Extension Suite — Build Orchestration
# type:    Build (Makefile)
# version: a-01.00
# created: 2026-02-21
# authors: Nova Dawn (CPI-SI)
# purpose: Build, test, and install OmniCode VSCode extensions.
#          Handles exFAT limitation: node_modules on ext4 cache,
#          source on project drive, install to ~/.vscode/extensions/.
#
# ============================================================================

# ============================================================================
# SETUP
# ============================================================================

# Directories
VSCODE_ROOT := $(shell pwd)
EXT_DIR     := $(HOME)/.vscode/extensions
CACHE_DIR   := $(HOME)/.cache/omnicode/vscode
LANG_SRC    := extensions/omnicode-language
ECO_SRC     := extensions/omnicode-ecosystem

# Tools (from ext4 cache)
NPM         := npm
ESBUILD     := $(CACHE_DIR)/node_modules/.bin/esbuild
TSC         := $(CACHE_DIR)/node_modules/.bin/tsc
VSCE        := $(CACHE_DIR)/node_modules/.bin/vsce

# Output
DIST_DIR    := dist

# Extension install names
LANG_INSTALL := omnicode-language
ECO_INSTALL  := omnicode-ecosystem

# esbuild flags
ESBUILD_FLAGS := --bundle \
	--outfile=$(LANG_SRC)/out/extension.js \
	--external:vscode \
	--format=cjs \
	--platform=node \
	--target=node18

# ============================================================================
# BODY
# ============================================================================

# ---------------------------------------------------------------------------
# Default target
# ---------------------------------------------------------------------------

.PHONY: all
all: build

# ---------------------------------------------------------------------------
# Dependencies — install to ext4 cache (exFAT can't do node_modules/.bin/)
# ---------------------------------------------------------------------------

.PHONY: deps
deps:
	@echo "Installing dependencies to ext4 cache..."
	@mkdir -p $(CACHE_DIR)
	@test -f $(CACHE_DIR)/package.json || (cd $(CACHE_DIR) && $(NPM) init -y >/dev/null 2>&1)
	@cd $(CACHE_DIR) && $(NPM) install --save-dev \
		@types/node@^18.0.0 \
		@types/vscode@^1.80.0 \
		esbuild@^0.24.0 \
		typescript@^5.7.0 \
		@vscode/vsce@latest \
		2>&1 | tail -5
	@echo "Dependencies installed at $(CACHE_DIR)/node_modules/"

# ---------------------------------------------------------------------------
# Build — esbuild bundle for omnicode-language
# ---------------------------------------------------------------------------

.PHONY: build
build: $(LANG_SRC)/out/extension.js

$(LANG_SRC)/out/extension.js: $(LANG_SRC)/src/*.ts
	@echo "Building omnicode-language..."
	@mkdir -p $(LANG_SRC)/out
	@$(ESBUILD) $(LANG_SRC)/src/extension.ts $(ESBUILD_FLAGS)
	@echo "Built: $(LANG_SRC)/out/extension.js"

# ---------------------------------------------------------------------------
# Type check (optional — esbuild doesn't type-check)
# ---------------------------------------------------------------------------

.PHONY: typecheck
typecheck:
	@echo "Type-checking omnicode-language..."
	@cd $(LANG_SRC) && $(TSC) --noEmit \
		--paths '{"vscode": ["$(CACHE_DIR)/node_modules/@types/vscode"]}'
	@echo "Type check passed"

# ---------------------------------------------------------------------------
# Install — copy extensions to ~/.vscode/extensions/
# ---------------------------------------------------------------------------

.PHONY: install
install: install-language install-ecosystem

.PHONY: install-language
install-language: build
	@echo "Installing omnicode-language..."
	@rm -rf $(EXT_DIR)/$(LANG_INSTALL)
	@mkdir -p $(EXT_DIR)/$(LANG_INSTALL)
	@cp $(LANG_SRC)/package.json $(EXT_DIR)/$(LANG_INSTALL)/
	@cp $(LANG_SRC)/language-configuration.json $(EXT_DIR)/$(LANG_INSTALL)/
	@cp $(LANG_SRC)/omnifold-language-configuration.json $(EXT_DIR)/$(LANG_INSTALL)/
	@cp -r $(LANG_SRC)/syntaxes $(EXT_DIR)/$(LANG_INSTALL)/
	@mkdir -p $(EXT_DIR)/$(LANG_INSTALL)/out
	@cp $(LANG_SRC)/out/extension.js $(EXT_DIR)/$(LANG_INSTALL)/out/
	@cp $(LANG_SRC)/icon.png $(EXT_DIR)/$(LANG_INSTALL)/ 2>/dev/null || true
	@cp $(LANG_SRC)/icon.svg $(EXT_DIR)/$(LANG_INSTALL)/ 2>/dev/null || true
	@cp $(LANG_SRC)/ofd-icon.png $(EXT_DIR)/$(LANG_INSTALL)/ 2>/dev/null || true
	@cp $(LANG_SRC)/ofd-icon.svg $(EXT_DIR)/$(LANG_INSTALL)/ 2>/dev/null || true
	@cp $(LANG_SRC)/README.md $(EXT_DIR)/$(LANG_INSTALL)/ 2>/dev/null || true
	@echo "Installed: $(EXT_DIR)/$(LANG_INSTALL)/"

.PHONY: install-ecosystem
install-ecosystem:
	@echo "Installing omnicode-ecosystem..."
	@rm -rf $(EXT_DIR)/$(ECO_INSTALL)
	@mkdir -p $(EXT_DIR)/$(ECO_INSTALL)
	@cp $(ECO_SRC)/package.json $(EXT_DIR)/$(ECO_INSTALL)/
	@cp -r $(ECO_SRC)/syntaxes $(EXT_DIR)/$(ECO_INSTALL)/
	@cp $(ECO_SRC)/icon.png $(EXT_DIR)/$(ECO_INSTALL)/ 2>/dev/null || true
	@cp $(ECO_SRC)/icon.svg $(EXT_DIR)/$(ECO_INSTALL)/ 2>/dev/null || true
	@cp $(ECO_SRC)/README.md $(EXT_DIR)/$(ECO_INSTALL)/ 2>/dev/null || true
	@echo "Installed: $(EXT_DIR)/$(ECO_INSTALL)/"

# ---------------------------------------------------------------------------
# Dev — build + install in one step
# ---------------------------------------------------------------------------

.PHONY: dev
dev: install
	@echo ""
	@echo "Extensions installed. Reload VSCode window to activate."
	@echo "  Ctrl+Shift+P → 'Developer: Reload Window'"

# ---------------------------------------------------------------------------
# Package — build .vsix files for distribution
# ---------------------------------------------------------------------------

.PHONY: package
package: package-language package-ecosystem
	@echo ""
	@echo "Packages ready in $(DIST_DIR)/"
	@ls -lh $(DIST_DIR)/*.vsix 2>/dev/null

.PHONY: package-language
package-language: build
	@echo "Packaging omnicode-language..."
	@mkdir -p $(DIST_DIR)
	@cd $(LANG_SRC) && $(VSCE) package --no-dependencies -o ../../$(DIST_DIR)/
	@echo "Packaged: $(DIST_DIR)/omnicode-language-*.vsix"

.PHONY: package-ecosystem
package-ecosystem:
	@echo "Packaging omnicode-ecosystem..."
	@mkdir -p $(DIST_DIR)
	@cd $(ECO_SRC) && $(VSCE) package --no-dependencies -o ../../$(DIST_DIR)/
	@echo "Packaged: $(DIST_DIR)/omnicode-ecosystem-*.vsix"

# ---------------------------------------------------------------------------
# Publish — push to VSCode Marketplace (requires PAT)
# ---------------------------------------------------------------------------

.PHONY: publish
publish: package
	@echo "Publishing omnicode-language..."
	@cd $(LANG_SRC) && $(VSCE) publish --no-dependencies
	@echo "Publishing omnicode-ecosystem..."
	@cd $(ECO_SRC) && $(VSCE) publish --no-dependencies
	@echo "Published to VSCode Marketplace"

# ---------------------------------------------------------------------------
# Clean
# ---------------------------------------------------------------------------

.PHONY: clean
clean:
	@rm -rf $(LANG_SRC)/out $(DIST_DIR)
	@echo "Cleaned build artifacts and packages"

.PHONY: clean-all
clean-all: clean
	@rm -rf $(CACHE_DIR)
	@echo "Cleaned ext4 cache"

# ---------------------------------------------------------------------------
# Help
# ---------------------------------------------------------------------------

.PHONY: help
help:
	@echo "OmniCode VSCode Extension Suite"
	@echo ""
	@echo "  make deps              Install npm dependencies (ext4 cache)"
	@echo "  make build             Build omnicode-language (esbuild)"
	@echo "  make install           Install both extensions to VSCode"
	@echo "  make install-language  Install omnicode-language only"
	@echo "  make install-ecosystem Install omnicode-ecosystem only"
	@echo "  make dev               Build + install + reload hint"
	@echo "  make package           Build .vsix packages for distribution"
	@echo "  make publish           Publish to VSCode Marketplace"
	@echo "  make typecheck         Type-check with tsc (no emit)"
	@echo "  make clean             Remove build artifacts + packages"
	@echo "  make clean-all         Remove artifacts + ext4 cache"

# ============================================================================
# CLOSING
# ============================================================================
#
# Build flow:
#   Source (exFAT) → deps (ext4 cache) → build (esbuild) → install (~/.vscode/)
#
# The exFAT project drive can't set execute permissions, so node_modules
# with .bin/ scripts live on ext4. Build output (extension.js) is just a
# file — no execute permission needed — so it goes back to source.
#
# "Look that thou make them after their pattern." — Exodus 25:40
# ============================================================================
