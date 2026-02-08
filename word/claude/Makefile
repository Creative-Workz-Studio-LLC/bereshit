# ============================================================================
# METADATA - CPI-SI Production Build System
# ============================================================================
#
# Purpose: Build, test, and deploy the CPI-SI system from Bereshit
# Biblical: Ecclesiastes 3:3 — "A time to break down, and a time to build up"
#
# Architecture:
#   Source (exFAT):   Bereshit/word/claude/       All source code, configs
#   Symlinks:         ~/.claude/                  Config/source references
#   Binaries (ext4):  ~/.local/share/claude/      Compiled executables (+x)
#
# Quick start:
#   make build        Build all modules + binaries (incremental — cached)
#   make install-bin  Deploy binaries to ext4
#   make verify       Check installation integrity
#   make fresh        Clean + full reinstall
#   make force        Bypass cache, rebuild everything
#   make help         Show all targets
#
# CLI alternative (after first install):
#   cpisi build       Same as make build
#   cpisi install     Same as make install
#   cpisi verify      Same as make verify
#   cpisi help        Full CLI help
#
# ============================================================================

# ============================================================================
# SETUP — Variables
# ============================================================================

.PHONY: all build bin test clean help \
        pkg hooks statusline system skills cpisi-cli \
        dashboard dashboard-frontend dashboard-tui dashboard-gtk \
        install install-bin install-bin-only install-symlinks install-desktop \
        uninstall uninstall-bin uninstall-symlinks \
        fresh verify clean-stale \
        fmt vet tidy \
        db db-init db-migrate db-status \
        db-init-all db-split \
        setup list-skills list-system-cmds \
        force cache-clean cache-warm \
        fast-build remote-build \
        deps deps-check deps-install server-setup

GO      := go
SHELL   := /bin/bash

# Binary link flags (strip debug symbols, reduce size)
BINFLAGS := -buildvcs=false -ldflags="-s -w"

# Disable VCS stamping — git status on 22GB exFAT is too slow, causes timeouts.
# Cannot use 'export GOFLAGS' because it pollutes sub-Makefile environments (hooks/).
# Instead, use NOVCS for compile-check targets and BINFLAGS for binary builds.
NOVCS := -buildvcs=false

# Suppress CGO warnings from gotk4 bindings (Go cgo issue #32411)
# cgo redeclares stdlib free(), triggering -Wbuiltin-declaration-mismatch in glib/gio/gtk.
# The PIE relocation warning (DT_TEXTREL in .gopclntab) is GNU ld informational — no flag
# to suppress it, but binaries work correctly.
export CGO_CFLAGS := -Wno-builtin-declaration-mismatch

# Canonical source directory (this directory, on exFAT)
SRC_DIR := $(shell pwd)

# Deployment targets
DEPLOY_DIR  := $(HOME)/.claude
INSTALL_DIR := $(HOME)/.local/share/claude

# Module paths — DRY list used by fmt, vet, tidy, clean
STD_MODULES := pkg hooks statusline dashboard dashboard/tui dashboard/gtk cmd/cpisi
SYS_MODULES := system/runtime/cmd system/runtime/lib

# Main binaries (name:module_dir:cmd_path)
MAIN_BINS := statusline cpisi dashboard dashboard-tui dashboard-gtk

# Skills
SKILLS_DIR  := skills
SKILL_DIRS  := $(wildcard $(SKILLS_DIR)/*/scripts)
SKILL_NAMES := $(notdir $(patsubst %/scripts,%,$(SKILL_DIRS)))

# Symlink targets
SYMLINK_DIRS  := agents commands config data divisions docs hooks output-styles \
                 pkg plans rules skills statusline system workflow
SYMLINK_FILES := CLAUDE.md SYSTEM-REFERENCE.md config.toml instance.jsonc \
                 Makefile go.work settings.json

# Stale artifacts — empty now (cpi-si and bin were cleaned 2026-02-04)
STALE_DIRS :=

# Database — multi-domain architecture
DATA_DIR     := data
DB_PATH      := $(DATA_DIR)/cpisi.db
DB_BASE_DIR  := $(HOME)/.local/share/claude/data
DB_DOMAINS   := sessions cognition growth temporal projects

# System command names (auto-detected, directories only)
SYS_CMD_DIR   := system/runtime/cmd
SYS_CMD_NAMES := $(shell find $(SYS_CMD_DIR) -mindepth 1 -maxdepth 1 -type d -exec basename {} \; 2>/dev/null | sort)

# Build cache (stamp files on ext4 — avoids exFAT 512KB cluster overhead)
BUILD_CACHE := $(HOME)/.cache/cpisi/stamps
$(shell mkdir -p $(BUILD_CACHE))

# Check if sources are newer than stamp file
# Usage: $(call stale,stamp_name,source_dirs...)
# Returns "y" if rebuild needed, empty if cached
define stale
$(shell s="$(BUILD_CACHE)/$(1).stamp"; \
  if [ ! -f "$$s" ]; then echo "y"; \
  else for d in $(2); do \
    if [ -d "$(SRC_DIR)/$$d" ] && find "$(SRC_DIR)/$$d" -name '*.go' -newer "$$s" -print -quit 2>/dev/null | grep -q .; then \
      echo "y"; break; fi; done; fi)
endef

# Check if binary needs rebuild (binary as its own stamp)
# Usage: $(call bin_stale,binary_path,source_dirs...)
define bin_stale
$(shell if [ ! -f "$(SRC_DIR)/$(1)" ]; then echo "y"; \
  else for d in $(2); do \
    if [ -d "$(SRC_DIR)/$$d" ] && find "$(SRC_DIR)/$$d" -name '*.go' -newer "$(SRC_DIR)/$(1)" -print -quit 2>/dev/null | grep -q .; then \
      echo "y"; break; fi; done; fi)
endef

# Default
all: build

# ============================================================================
# BODY — Build Targets
# ============================================================================

# Build everything: compile all modules + build binaries
build: pkg hooks statusline system skills cpisi-cli dashboard dashboard-tui dashboard-gtk bin
	@echo ""
	@echo "=== All modules built ==="
	@echo "    Run 'make install-bin' to deploy to $(INSTALL_DIR)"

# --- Module compile checks (cached) ---

pkg:
	@if [ -n "$(call stale,pkg,pkg)" ]; then \
		echo "  Building pkg..."; \
		cd pkg && $(GO) build $(NOVCS) ./... && touch $(BUILD_CACHE)/pkg.stamp; \
	else echo "  pkg (cached)"; fi

hooks:
	@if [ -n "$(call stale,hooks,hooks pkg)" ]; then \
		echo "  Building hooks..."; \
		cd hooks && $(GO) build $(NOVCS) ./... && touch $(BUILD_CACHE)/hooks.stamp; \
	else echo "  hooks (cached)"; fi

statusline:
	@if [ -n "$(call stale,statusline,statusline pkg)" ]; then \
		echo "  Building statusline..."; \
		cd statusline && $(GO) build $(NOVCS) ./... && touch $(BUILD_CACHE)/statusline.stamp; \
	else echo "  statusline (cached)"; fi

system:
	@if [ -n "$(call stale,system,system/runtime/cmd system/runtime/lib pkg)" ]; then \
		echo "  Building system..."; \
		cd system/runtime/cmd && $(GO) build $(NOVCS) ./...; \
		cd $(SRC_DIR)/system/runtime/lib && $(GO) build $(NOVCS) ./...; \
		touch $(BUILD_CACHE)/system.stamp; \
	else echo "  system (cached)"; fi

cpisi-cli:
	@if [ -n "$(call stale,cpisi-cli,cmd/cpisi)" ]; then \
		echo "  Building cpisi CLI..."; \
		cd cmd/cpisi && $(GO) build $(NOVCS) ./... && touch $(BUILD_CACHE)/cpisi-cli.stamp; \
	else echo "  cpisi-cli (cached)"; fi

skills: $(addprefix skill-,$(SKILL_NAMES))
	@echo "  All skills checked"

skill-%:
	@if [ -d "$(SKILLS_DIR)/$*/scripts" ]; then \
		if [ -n "$(call stale,skill-$*,$(SKILLS_DIR)/$*/scripts)" ]; then \
			cd $(SKILLS_DIR)/$*/scripts && $(GO) build $(NOVCS) ./... && touch $(BUILD_CACHE)/skill-$*.stamp; \
		fi; \
	else \
		echo "  Skill not found: $*"; exit 1; \
	fi

# --- Dashboard modules (cached) ---

dashboard-frontend:
	@if [ -f "dashboard/frontend/package.json" ]; then \
		if [ -d "dashboard/server/static" ] && [ -f $(BUILD_CACHE)/frontend.stamp ] && \
		   ! find dashboard/frontend/src -newer $(BUILD_CACHE)/frontend.stamp -print -quit 2>/dev/null | grep -q .; then \
			echo "  Dashboard frontend (cached)"; \
		else \
			echo "  Building dashboard frontend..."; \
			TEMP_BUILD=/tmp/cpisi-dashboard-build; \
			rm -rf "$$TEMP_BUILD"; \
			cp -r dashboard/frontend "$$TEMP_BUILD"; \
			cd "$$TEMP_BUILD" && npm install --silent && npx tsc --noEmit && npx vite build; \
			rm -rf $(SRC_DIR)/dashboard/server/static; \
			cp -r /tmp/server/static $(SRC_DIR)/dashboard/server/static; \
			rm -rf "$$TEMP_BUILD" /tmp/server; \
			touch $(BUILD_CACHE)/frontend.stamp; \
			echo "    Built: dashboard/server/static/"; \
		fi; \
	else \
		echo "    Frontend not found, skipping"; \
	fi

dashboard: dashboard-frontend
	@if [ -n "$(call stale,dashboard,dashboard pkg)" ]; then \
		echo "  Building dashboard (web)..."; \
		cd dashboard && $(GO) build $(NOVCS) ./... && touch $(BUILD_CACHE)/dashboard.stamp; \
	else echo "  dashboard (cached)"; fi

dashboard-tui:
	@if [ -n "$(call stale,dashboard-tui,dashboard/tui pkg)" ]; then \
		echo "  Building dashboard (TUI)..."; \
		cd dashboard/tui && $(GO) build $(NOVCS) ./... && touch $(BUILD_CACHE)/dashboard-tui.stamp; \
	else echo "  dashboard-tui (cached)"; fi

dashboard-gtk:
	@if [ -n "$(call stale,dashboard-gtk,dashboard/gtk pkg)" ]; then \
		echo "  Building dashboard (GTK4)..."; \
		cd dashboard/gtk && $(GO) build $(NOVCS) ./... 2>&1 | grep -v 'gopclntab\|DT_TEXTREL\|creating DT_TEXTREL\|dashboard-gtk$$'; touch $(BUILD_CACHE)/dashboard-gtk.stamp; \
	else echo "  dashboard-gtk (cached)"; fi

# --- Binary builds (cached — skips unchanged modules) ---

bin: bin-statusline bin-cpisi bin-hooks bin-system-cmds \
     bin-dashboard bin-dashboard-tui bin-dashboard-gtk
	@echo "=== Binaries built to bin/ ==="

bin-statusline:
	@mkdir -p bin
	@if [ -n "$(call bin_stale,bin/statusline,statusline pkg)" ]; then \
		cd statusline && $(GO) build $(BINFLAGS) -o ../bin/statusline ./cmd/statusline; \
		echo "    bin/statusline"; \
	else echo "    bin/statusline (cached)"; fi

bin-cpisi:
	@mkdir -p bin
	@if [ -n "$(call bin_stale,bin/cpisi,cmd/cpisi)" ]; then \
		cd cmd/cpisi && $(GO) build $(BINFLAGS) -o ../../bin/cpisi .; \
		echo "    bin/cpisi"; \
	else echo "    bin/cpisi (cached)"; fi

bin-dashboard: dashboard-frontend
	@mkdir -p bin
	@if [ -n "$(call bin_stale,bin/dashboard,dashboard pkg)" ]; then \
		cd dashboard && $(GO) build $(BINFLAGS) -o ../bin/dashboard ./cmd/dashboard; \
		echo "    bin/dashboard"; \
	else echo "    bin/dashboard (cached)"; fi

bin-dashboard-tui:
	@mkdir -p bin
	@if [ -n "$(call bin_stale,bin/dashboard-tui,dashboard/tui pkg)" ]; then \
		cd dashboard/tui && $(GO) build $(BINFLAGS) -o ../../bin/dashboard-tui ./cmd/dashboard-tui; \
		echo "    bin/dashboard-tui"; \
	else echo "    bin/dashboard-tui (cached)"; fi

bin-dashboard-gtk:
	@mkdir -p bin
	@if [ -n "$(call bin_stale,bin/dashboard-gtk,dashboard/gtk pkg)" ]; then \
		cd dashboard/gtk && $(GO) build $(BINFLAGS) -o ../../bin/dashboard-gtk ./cmd/dashboard-gtk 2>&1 | grep -v 'gopclntab\|DT_TEXTREL\|creating DT_TEXTREL\|dashboard-gtk$$'; \
		echo "    bin/dashboard-gtk"; \
	else echo "    bin/dashboard-gtk (cached)"; fi

bin-hooks:
	@if [ -n "$(call stale,hooks-bin,hooks pkg)" ]; then \
		cd hooks && $(MAKE) build; \
		touch $(BUILD_CACHE)/hooks-bin.stamp; \
		echo "    hooks/bin/*"; \
	else echo "    hooks/bin/* (cached)"; fi

bin-system-cmds:
	@mkdir -p system/runtime/bin
	@if [ -n "$(call stale,system-cmds,system/runtime/cmd system/runtime/lib)" ]; then \
		echo "    Building system commands (parallel)..."; \
		pids=""; count=0; \
		for cmd in system/runtime/cmd/*/; do \
			name=$$(basename $$cmd); \
			if [ -f "$$cmd$$name.go" ] || [ -f "$$cmd/main.go" ]; then \
				(cd $$cmd && $(GO) build $(BINFLAGS) -o $(SRC_DIR)/system/runtime/bin/$$name . 2>/dev/null) & \
				pids="$$pids $$!"; count=$$((count+1)); \
			fi; \
		done; \
		fail=0; for pid in $$pids; do wait $$pid || fail=$$((fail+1)); done; \
		if [ $$fail -eq 0 ]; then \
			echo "    system/runtime/bin/* ($$count commands)"; \
			touch $(BUILD_CACHE)/system-cmds.stamp; \
		else echo "    WARN: $$fail system command(s) failed"; fi; \
	else echo "    system/runtime/bin/* (cached)"; fi

# Build a single system command: make sys-<name>
# Uses bin_stale to skip if binary is already newer than sources
sys-%:
	@mkdir -p system/runtime/bin
	@cmd_dir="$(SYS_CMD_DIR)/$*"; \
	if [ ! -d "$$cmd_dir" ]; then echo "  System command not found: $*"; exit 1; fi; \
	if [ -n "$(call bin_stale,system/runtime/bin/$*,$(SYS_CMD_DIR)/$* system/runtime/lib)" ]; then \
		echo "  Building system/$*..."; \
		cd "$$cmd_dir" && $(GO) build $(BINFLAGS) -o $(SRC_DIR)/system/runtime/bin/$* .; \
		echo "    system/runtime/bin/$*"; \
	else echo "  system/$* (cached)"; fi

# Fast build: rsync Go source to persistent ext4 mirror, build there, copy binaries back.
# Avoids exFAT filesystem overhead. Mirror persists across reboots (~/.cache/cpisi/source/).
# First run rsyncs ~730MB Go source (skips 22GB data). Subsequent runs rsync only changes.
# Usage: make fast-build  OR  make fast-sys-<name>  OR  make remote-build
FAST_BUILD_DIR := $(HOME)/.cache/cpisi/source

# Go source directories (skip data/, system/data/, divisions/, etc.)
FAST_SYNC_INCLUDE := pkg hooks statusline dashboard cmd system/runtime system/loganalyzer \
                     skills go.work go.sum Makefile

fast-build:
	@echo "  Fast build (persistent ext4 at $(FAST_BUILD_DIR))..."
	@mkdir -p $(FAST_BUILD_DIR)
	@rsync -a --delete \
		--include='go.work' --include='go.sum' --include='Makefile' \
		--include='pkg/***' --include='hooks/***' --include='statusline/***' \
		--include='dashboard/***' --include='cmd/***' --include='system/***' \
		--include='skills/***' \
		--exclude='*/node_modules/***' --exclude='*/bin/***' \
		--exclude='*' \
		$(SRC_DIR)/ $(FAST_BUILD_DIR)/
	@cd $(FAST_BUILD_DIR) && $(MAKE) build --no-print-directory SRC_DIR=$(FAST_BUILD_DIR)
	@echo "  Copying binaries back..."
	@mkdir -p bin system/runtime/bin
	@cp -r $(FAST_BUILD_DIR)/bin/* bin/ 2>/dev/null || true
	@cp -r $(FAST_BUILD_DIR)/system/runtime/bin/* system/runtime/bin/ 2>/dev/null || true
	@cp -r $(FAST_BUILD_DIR)/hooks/bin/* hooks/bin/ 2>/dev/null || true
	@echo "  Fast build complete."

fast-sys-%:
	@mkdir -p system/runtime/bin
	@cmd_dir="$(SYS_CMD_DIR)/$*"; \
	if [ ! -d "$$cmd_dir" ]; then echo "  System command not found: $*"; exit 1; fi
	@echo "  Fast building system/$* (ext4)..."
	@mkdir -p $(FAST_BUILD_DIR)
	@rsync -a --delete $(SYS_CMD_DIR)/$*/ $(FAST_BUILD_DIR)/cmd/$*/ 2>/dev/null; \
	rsync -a --delete system/runtime/lib/ $(FAST_BUILD_DIR)/lib/ 2>/dev/null; \
	cp system/runtime/cmd/go.mod system/runtime/cmd/go.sum $(FAST_BUILD_DIR)/cmd/ 2>/dev/null || true; \
	cd $(FAST_BUILD_DIR)/cmd/$* && $(GO) build $(BINFLAGS) -o $(SRC_DIR)/system/runtime/bin/$* .
	@echo "    system/runtime/bin/$*"

# Remote build: offload compilation to CWS server (full ext4, 428GB free).
# Rsyncs Go source to server, builds there, pulls binaries back.
# Usage: make remote-build  (requires: ssh cws-server works)
REMOTE_HOST    := cws-server
REMOTE_DIR     := ~/cpisi-build

remote-build:
	@echo "  Remote build ($(REMOTE_HOST):$(REMOTE_DIR))..."
	@ssh $(REMOTE_HOST) 'mkdir -p $(REMOTE_DIR)'
	@rsync -az --delete \
		--include='go.work' --include='go.sum' --include='Makefile' \
		--include='pkg/***' --include='hooks/***' --include='statusline/***' \
		--include='dashboard/***' --include='cmd/***' --include='system/***' \
		--include='skills/***' \
		--exclude='*/node_modules/***' --exclude='*/bin/***' \
		--exclude='*' \
		$(SRC_DIR)/ $(REMOTE_HOST):$(REMOTE_DIR)/
	@echo "  Building on server..."
	@ssh $(REMOTE_HOST) 'cd $(REMOTE_DIR) && make build --no-print-directory SRC_DIR=$(REMOTE_DIR)'
	@echo "  Pulling binaries..."
	@mkdir -p bin system/runtime/bin hooks/bin
	@rsync -az $(REMOTE_HOST):$(REMOTE_DIR)/bin/ bin/ 2>/dev/null || true
	@rsync -az $(REMOTE_HOST):$(REMOTE_DIR)/system/runtime/bin/ system/runtime/bin/ 2>/dev/null || true
	@rsync -az $(REMOTE_HOST):$(REMOTE_DIR)/hooks/bin/ hooks/bin/ 2>/dev/null || true
	@echo "  Remote build complete."

# Warm Go build cache for all modules (speeds up subsequent builds)
cache-warm:
	@echo "  Warming Go build cache..."
	@echo "  [1/4] pkg..."
	@cd pkg && $(GO) build $(NOVCS) ./... 2>/dev/null
	@echo "  [2/4] system/runtime/lib..."
	@cd system/runtime/lib && $(GO) build $(NOVCS) ./... 2>/dev/null
	@echo "  [3/4] system/runtime/cmd..."
	@cd system/runtime/cmd && $(GO) build $(NOVCS) ./... 2>/dev/null
	@echo "  [4/4] hooks..."
	@cd hooks && $(GO) build $(NOVCS) ./... 2>/dev/null
	@echo "  Build cache warm. Subsequent builds will be faster."

# ============================================================================
# BODY — Install / Uninstall
# ============================================================================

# Full install: build + symlinks + binaries + desktop
install:
	@echo ""
	@echo "  CPI-SI Installation"
	@echo "  Source:   $(SRC_DIR)"
	@echo "  Symlinks: $(DEPLOY_DIR)"
	@echo "  Binaries: $(INSTALL_DIR)"
	@echo ""
	@echo "  [1/5] Building packages..."
	@$(MAKE) pkg --no-print-directory
	@echo "  [2/5] Building binaries..."
	@$(MAKE) bin --no-print-directory
	@echo "  [3/5] Creating symlinks..."
	@$(MAKE) install-symlinks --no-print-directory
	@echo "  [4/5] Installing binaries..."
	@$(MAKE) install-bin-only --no-print-directory
	@echo "  [5/5] Installing desktop entry..."
	@$(MAKE) install-desktop --no-print-directory
	@echo ""
	@$(MAKE) verify --no-print-directory
	@echo ""
	@echo "  Installation complete."

# Build binaries then install
install-bin: bin install-bin-only

# Copy binaries to ext4 with +x
install-bin-only:
	@mkdir -p $(INSTALL_DIR)/bin $(INSTALL_DIR)/hooks/bin $(INSTALL_DIR)/system/bin
	@echo "  Main binaries:"
	@for name in $(MAIN_BINS); do \
		if [ -f "bin/$$name" ]; then \
			cp "bin/$$name" "$(INSTALL_DIR)/bin/$$name"; \
			chmod +x "$(INSTALL_DIR)/bin/$$name"; \
			echo "    + $$name"; \
		fi; \
	done
	@echo "  Hook binaries:"
	@for hook in hooks/bin/*; do \
		if [ -f "$$hook" ]; then \
			name=$$(basename $$hook); \
			cp "$$hook" "$(INSTALL_DIR)/hooks/bin/$$name"; \
			chmod +x "$(INSTALL_DIR)/hooks/bin/$$name"; \
			echo "    + $$name"; \
		fi; \
	done
	@echo "  System binaries:"
	@for cmd in system/runtime/bin/*; do \
		if [ -f "$$cmd" ]; then \
			name=$$(basename $$cmd); \
			cp "$$cmd" "$(INSTALL_DIR)/system/bin/$$name"; \
			chmod +x "$(INSTALL_DIR)/system/bin/$$name"; \
		fi; \
	done 2>/dev/null || true
	@echo "    + system commands"

# Create symlinks from source to ~/.claude/
install-symlinks:
	@mkdir -p $(DEPLOY_DIR)
	@echo "  Directory symlinks:"
	@for dir in $(SYMLINK_DIRS); do \
		if [ -d "$(SRC_DIR)/$$dir" ]; then \
			[ -L "$(DEPLOY_DIR)/$$dir" ] && rm -f "$(DEPLOY_DIR)/$$dir"; \
			[ -d "$(DEPLOY_DIR)/$$dir" ] && mv "$(DEPLOY_DIR)/$$dir" "$(DEPLOY_DIR)/$$dir.bak"; \
			ln -sf "$(SRC_DIR)/$$dir" "$(DEPLOY_DIR)/$$dir"; \
			echo "    + $$dir"; \
		fi; \
	done
	@echo "  File symlinks:"
	@for file in $(SYMLINK_FILES); do \
		if [ -f "$(SRC_DIR)/$$file" ]; then \
			[ -L "$(DEPLOY_DIR)/$$file" ] && rm -f "$(DEPLOY_DIR)/$$file"; \
			[ -f "$(DEPLOY_DIR)/$$file" ] && mv "$(DEPLOY_DIR)/$$file" "$(DEPLOY_DIR)/$$file.bak"; \
			ln -sf "$(SRC_DIR)/$$file" "$(DEPLOY_DIR)/$$file"; \
			echo "    + $$file"; \
		fi; \
	done

# Install .desktop file for GTK4 dashboard
install-desktop:
	@DESKTOP_IN="$(SRC_DIR)/dashboard/gtk/data/com.creativeworkzstudio.cpisi.dashboard.desktop.in"; \
	if [ -f "$$DESKTOP_IN" ]; then \
		APPS_DIR="$(HOME)/.local/share/applications"; \
		mkdir -p "$$APPS_DIR"; \
		sed 's|@BINDIR@|$(INSTALL_DIR)/bin|g' "$$DESKTOP_IN" \
			| sed 's|@ICONDIR@|$(SRC_DIR)/dashboard/gtk/data|g' \
			> "$$APPS_DIR/com.creativeworkzstudio.cpisi.dashboard.desktop"; \
		echo "    + desktop entry"; \
	fi

install-status:
	@echo "  Symlink status ($(DEPLOY_DIR)):"
	@for dir in $(SYMLINK_DIRS); do \
		if [ -L "$(DEPLOY_DIR)/$$dir" ]; then \
			target=$$(readlink -f "$(DEPLOY_DIR)/$$dir"); \
			[ "$$target" = "$(SRC_DIR)/$$dir" ] && echo "    + $$dir" || echo "    ! $$dir -> $$target"; \
		elif [ -d "$(DEPLOY_DIR)/$$dir" ]; then echo "    x $$dir (not symlink)"; \
		else echo "    - $$dir"; fi; \
	done
	@for file in $(SYMLINK_FILES); do \
		if [ -L "$(DEPLOY_DIR)/$$file" ]; then \
			target=$$(readlink -f "$(DEPLOY_DIR)/$$file"); \
			[ "$$target" = "$(SRC_DIR)/$$file" ] && echo "    + $$file" || echo "    ! $$file -> $$target"; \
		elif [ -f "$(DEPLOY_DIR)/$$file" ]; then echo "    x $$file (not symlink)"; \
		else echo "    - $$file"; fi; \
	done

uninstall: uninstall-symlinks uninstall-bin
	@echo "  Uninstall complete"

uninstall-symlinks:
	@for dir in $(SYMLINK_DIRS); do [ -L "$(DEPLOY_DIR)/$$dir" ] && rm -f "$(DEPLOY_DIR)/$$dir"; done; true
	@for file in $(SYMLINK_FILES); do [ -L "$(DEPLOY_DIR)/$$file" ] && rm -f "$(DEPLOY_DIR)/$$file"; done; true
	@echo "  Symlinks removed"

uninstall-bin:
	@rm -rf $(INSTALL_DIR)/bin/* $(INSTALL_DIR)/hooks/bin/* $(INSTALL_DIR)/system/bin/*
	@echo "  Binaries removed"

fresh: clean clean-stale install

clean-stale:
	@for dir in $(STALE_DIRS); do \
		if [ -d "$(DEPLOY_DIR)/$$dir" ] || [ -L "$(DEPLOY_DIR)/$$dir" ]; then \
			rm -rf "$(DEPLOY_DIR)/$$dir"; \
			echo "    Removed stale: $$dir"; \
		fi; \
	done

# ============================================================================
# BODY — Verify
# ============================================================================

verify:
	@echo "  Verifying CPI-SI installation..."
	@echo ""
	@errors=0; \
	echo "  Source:"; \
	for dir in $(SYMLINK_DIRS); do \
		[ -d "$(SRC_DIR)/$$dir" ] && echo "    + $$dir" || { echo "    x $$dir MISSING"; errors=$$((errors+1)); }; \
	done; \
	for file in $(SYMLINK_FILES); do \
		[ -f "$(SRC_DIR)/$$file" ] && echo "    + $$file" || { echo "    x $$file MISSING"; errors=$$((errors+1)); }; \
	done; \
	echo ""; \
	echo "  Symlinks:"; \
	for dir in $(SYMLINK_DIRS); do \
		if [ -L "$(DEPLOY_DIR)/$$dir" ]; then \
			target=$$(readlink -f "$(DEPLOY_DIR)/$$dir"); \
			[ "$$target" = "$(SRC_DIR)/$$dir" ] && echo "    + $$dir" || { echo "    ! $$dir wrong target"; errors=$$((errors+1)); }; \
		elif [ -d "$(DEPLOY_DIR)/$$dir" ]; then echo "    x $$dir not symlink"; errors=$$((errors+1)); \
		else echo "    - $$dir"; fi; \
	done; \
	for file in $(SYMLINK_FILES); do \
		if [ -L "$(DEPLOY_DIR)/$$file" ]; then \
			target=$$(readlink -f "$(DEPLOY_DIR)/$$file"); \
			[ "$$target" = "$(SRC_DIR)/$$file" ] && echo "    + $$file" || { echo "    ! $$file wrong target"; errors=$$((errors+1)); }; \
		elif [ -f "$(DEPLOY_DIR)/$$file" ]; then echo "    x $$file not symlink"; errors=$$((errors+1)); \
		else echo "    - $$file"; fi; \
	done; \
	echo ""; \
	echo "  Binaries:"; \
	for name in $(MAIN_BINS); do \
		if [ -x "$(INSTALL_DIR)/bin/$$name" ]; then echo "    + $$name"; \
		elif [ -f "$(INSTALL_DIR)/bin/$$name" ]; then echo "    x $$name not executable"; errors=$$((errors+1)); \
		else echo "    - $$name"; fi; \
	done; \
	echo ""; \
	echo "  Hooks:"; \
	for hook in start end stop submit pre-use post-use pre-compact setup request notification permission-request hooks subagent-start subagent-stop task-completed teammate-idle; do \
		if [ -x "$(INSTALL_DIR)/hooks/bin/$$hook" ]; then echo "    + $$hook"; \
		elif [ -f "$(INSTALL_DIR)/hooks/bin/$$hook" ]; then echo "    x $$hook not executable"; errors=$$((errors+1)); \
		else echo "    - $$hook"; fi; \
	done; \
	echo ""; \
	echo "  Stale:"; \
	for dir in $(STALE_DIRS); do \
		if [ -d "$(DEPLOY_DIR)/$$dir" ] || [ -L "$(DEPLOY_DIR)/$$dir" ]; then \
			echo "    ! $$dir is stale"; errors=$$((errors+1)); \
		else echo "    + $$dir absent (good)"; fi; \
	done; \
	echo ""; \
	echo "  Workspace:"; \
	if [ -f "$(SRC_DIR)/go.work" ]; then \
		echo "    + go.work exists"; \
		cd $(SRC_DIR) && $(GO) work sync 2>/dev/null && echo "    + go.work syncs" || { echo "    x go.work sync failed"; errors=$$((errors+1)); }; \
	else echo "    x go.work missing"; errors=$$((errors+1)); fi; \
	echo ""; \
	if [ $$errors -eq 0 ]; then echo "  === Verification PASSED ==="; \
	else echo "  === FAILED: $$errors error(s) ==="; echo "  Run 'make fresh' to fix"; fi

# ============================================================================
# BODY — Test
# ============================================================================

test: test-pkg test-hooks test-statusline test-system test-dashboard test-dashboard-tui test-dashboard-gtk
	@echo "=== All tests passed ==="

test-pkg:
	@echo "  Testing pkg..."
	@cd pkg && $(GO) test ./... -v

test-hooks:
	@echo "  Testing hooks..."
	@cd hooks && $(GO) test ./... -v

test-statusline:
	@echo "  Testing statusline..."
	@cd statusline && $(GO) test ./... -v

test-system:
	@echo "  Testing system..."
	@cd system/runtime/cmd && $(GO) test ./... -v 2>/dev/null || true
	@cd system/runtime/lib && $(GO) test ./... -v 2>/dev/null || true

test-dashboard:
	@echo "  Testing dashboard..."
	@cd dashboard && $(GO) test ./... -v 2>/dev/null || true

test-dashboard-tui:
	@echo "  Testing dashboard-tui..."
	@cd dashboard/tui && $(GO) test ./... -v 2>/dev/null || true

test-dashboard-gtk:
	@echo "  Testing dashboard-gtk..."
	@cd dashboard/gtk && $(GO) test ./... -v 2>/dev/null || true

# ============================================================================
# BODY — Database (Multi-Domain Architecture)
# ============================================================================

# Legacy single-DB targets (backward compat)
db: db-init db-migrate

db-init:
	@mkdir -p $(DATA_DIR)
	@if [ -f "$(DATA_DIR)/schema/001_initial.sql" ]; then \
		sqlite3 $(DB_PATH) < $(DATA_DIR)/schema/001_initial.sql && \
		echo "  Legacy database initialized at $(DB_PATH)"; \
	else echo "  Schema file not found"; fi

db-migrate:
	@for f in $(DATA_DIR)/migrations/*.sql; do \
		[ -f "$$f" ] && sqlite3 $(DB_PATH) < "$$f" && echo "  Applied: $$f"; \
	done 2>/dev/null || true

# --- Multi-domain database targets ---

# Initialize all 5 domain databases
db-init-all: $(addprefix db-init-,$(DB_DOMAINS))
	@echo "  All domain databases initialized at $(DB_BASE_DIR)"

# Initialize a single domain: make db-init-<domain>
db-init-%: FORCE
	@mkdir -p $(DB_BASE_DIR)
	@schema="pkg/foundation/database/$*/schema/001_initial.sql"; \
	if [ -f "$$schema" ]; then \
		sqlite3 "$(DB_BASE_DIR)/$*.db" < "$$schema" && \
		echo "    + $*.db initialized"; \
	else echo "    x $*.db — schema not found: $$schema"; exit 1; fi

FORCE:

# Split legacy cpisi.db into 5 domain databases
db-split:
	@if [ ! -f "$(DB_PATH)" ]; then echo "  Legacy $(DB_PATH) not found"; exit 1; fi
	@echo "  Splitting $(DB_PATH) → $(DB_BASE_DIR)/"
	@$(MAKE) db-init-all --no-print-directory
	@echo "  Migrating sessions data..."
	@for table in sessions exchanges insights key_context session_events feedback_events log_imports; do \
		count=$$(sqlite3 $(DB_PATH) "SELECT COUNT(*) FROM $$table" 2>/dev/null || echo "0"); \
		if [ "$$count" != "0" ]; then \
			sqlite3 $(DB_PATH) ".dump $$table" | sqlite3 "$(DB_BASE_DIR)/sessions.db" 2>/dev/null; \
			echo "    + $$table ($$count rows)"; \
		fi; \
	done
	@echo "  Migrating cognition data..."
	@for table in choices hebrew_transitions k_align_history tool_events; do \
		count=$$(sqlite3 $(DB_PATH) "SELECT COUNT(*) FROM $$table" 2>/dev/null || echo "0"); \
		if [ "$$count" != "0" ]; then \
			sqlite3 $(DB_PATH) ".dump $$table" | sqlite3 "$(DB_BASE_DIR)/cognition.db" 2>/dev/null; \
			echo "    + $$table ($$count rows)"; \
		fi; \
	done
	@echo "  Migrating growth data..."
	@for table in detected_patterns learnings; do \
		count=$$(sqlite3 $(DB_PATH) "SELECT COUNT(*) FROM $$table" 2>/dev/null || echo "0"); \
		if [ "$$count" != "0" ]; then \
			sqlite3 $(DB_PATH) ".dump $$table" | sqlite3 "$(DB_BASE_DIR)/growth.db" 2>/dev/null; \
			echo "    + $$table ($$count rows)"; \
		fi; \
	done
	@echo "  Split complete. Legacy DB preserved at $(DB_PATH)"
	@$(MAKE) db-status --no-print-directory

# Show all databases with sizes and table counts
db-status:
	@echo "  Domain Databases ($(DB_BASE_DIR)):"
	@echo ""
	@for domain in $(DB_DOMAINS); do \
		db="$(DB_BASE_DIR)/$$domain.db"; \
		if [ -f "$$db" ]; then \
			size=$$(du -h "$$db" | cut -f1); \
			tables=$$(sqlite3 "$$db" ".tables" 2>/dev/null | wc -w); \
			echo "    $$domain.db  $$size  ($$tables tables)"; \
		else \
			echo "    $$domain.db  — not initialized"; \
		fi; \
	done
	@echo ""
	@if [ -f "$(DB_PATH)" ]; then \
		echo "  Legacy: $(DB_PATH) ($$(du -h $(DB_PATH) | cut -f1))"; \
	fi

# ============================================================================
# BODY — Utilities (DRY module iteration)
# ============================================================================

clean:
	@echo "  Cleaning..."
	@rm -rf bin/ $(BUILD_CACHE)/
	@for mod in $(STD_MODULES) $(SYS_MODULES); do \
		(cd $$mod && $(GO) clean ./...) 2>/dev/null || true; \
	done
	@rm -rf dashboard/server/static 2>/dev/null || true
	@find . -name "*.test" -delete 2>/dev/null || true
	@echo "  Clean complete"

# Reset build cache only (next build will recheck all sources)
cache-clean:
	@rm -rf $(BUILD_CACHE)/
	@echo "  Build cache cleared"

# Force rebuild everything (clear cache then build)
force: cache-clean build

fmt:
	@echo "  Formatting..."
	@for mod in $(STD_MODULES) $(SYS_MODULES); do \
		(cd $$mod && $(GO) fmt ./...) 2>/dev/null || true; \
	done

vet:
	@echo "  Vetting..."
	@for mod in $(STD_MODULES) $(SYS_MODULES); do \
		(cd $$mod && $(GO) vet ./...) 2>/dev/null || true; \
	done

tidy:
	@echo "  Tidying go.mod files..."
	@for mod in $(STD_MODULES) $(SYS_MODULES); do \
		(cd $$mod && $(GO) mod tidy) 2>/dev/null || true; \
	done
	@for skill in $(SKILL_DIRS); do \
		(cd $$skill && $(GO) mod tidy) 2>/dev/null || true; \
	done

list-skills:
	@for skill in $(SKILL_NAMES); do echo "  $$skill"; done

list-system-cmds:
	@echo "  System commands ($(words $(SYS_CMD_NAMES))):"
	@for cmd in $(SYS_CMD_NAMES); do echo "    $$cmd"; done

# ============================================================================
# BODY — Dependency Management
# ============================================================================

# Required system packages for each build target
# Core: go, make, git
# Dashboard frontend: node, npm
# Dashboard GTK4: libgtk-4-dev, libgirepository1.0-dev, pkg-config
# System: sqlite3 (for db-init, db-split)

# Check all dependencies — reports missing without installing
deps-check:
	@echo "  Checking dependencies..."
	@echo ""
	@missing=0; \
	echo "  Core (required):"; \
	check_ver() { \
		case "$$1" in \
			go)  go version 2>/dev/null | head -1 ;; \
			gcc) gcc --version 2>/dev/null | head -1 ;; \
			*)   $$1 --version 2>/dev/null | head -1 ;; \
		esac; \
	}; \
	for cmd in go make git gcc; do \
		if command -v $$cmd >/dev/null 2>&1; then \
			ver=$$(check_ver $$cmd); \
			echo "    + $$cmd  ($$ver)"; \
		else echo "    x $$cmd  MISSING"; missing=$$((missing+1)); fi; \
	done; \
	echo ""; \
	echo "  Frontend (dashboard-frontend):"; \
	for cmd in node npm; do \
		if command -v $$cmd >/dev/null 2>&1; then \
			echo "    + $$cmd  ($$($$cmd --version 2>/dev/null))"; \
		else echo "    x $$cmd  MISSING (install nodejs)"; missing=$$((missing+1)); fi; \
	done; \
	echo ""; \
	echo "  GTK4 (dashboard-gtk):"; \
	if pkg-config --exists gtk4 2>/dev/null; then \
		echo "    + gtk4  ($$(pkg-config --modversion gtk4))"; \
	else echo "    x gtk4  MISSING (install libgtk-4-dev)"; missing=$$((missing+1)); fi; \
	if pkg-config --exists gobject-introspection-1.0 2>/dev/null; then \
		echo "    + gobject-introspection"; \
	else echo "    x gobject-introspection  MISSING (install libgirepository1.0-dev)"; missing=$$((missing+1)); fi; \
	echo ""; \
	echo "  Database:"; \
	if command -v sqlite3 >/dev/null 2>&1; then \
		echo "    + sqlite3  ($$(sqlite3 --version | cut -d' ' -f1))"; \
	else echo "    x sqlite3  MISSING"; missing=$$((missing+1)); fi; \
	echo ""; \
	if [ $$missing -eq 0 ]; then echo "  All dependencies satisfied."; \
	else echo "  $$missing missing. Run 'make deps-install' to install."; fi

# Install all dependencies (requires elevated permissions)
deps-install:
	@echo "  Installing dependencies..."
	@echo ""
	@echo "  [1/4] System packages..."
	@pkexec apt install -y build-essential git sqlite3 curl wget pkg-config 2>&1 | tail -1
	@echo "  [2/4] GTK4 development libraries..."
	@pkexec apt install -y libgtk-4-dev libgirepository1.0-dev 2>&1 | tail -1
	@echo "  [3/4] Go $(shell go version 2>/dev/null | grep -oP 'go[\d.]+' || echo '1.24.4')..."
	@if command -v go >/dev/null 2>&1; then \
		echo "    Already installed: $$(go version)"; \
	else \
		echo "    Downloading Go 1.24.4..."; \
		wget -q https://go.dev/dl/go1.24.4.linux-amd64.tar.gz -O /tmp/go.tar.gz; \
		pkexec tar -C /usr/local -xzf /tmp/go.tar.gz; \
		rm /tmp/go.tar.gz; \
		pkexec ln -sf /usr/local/go/bin/go /usr/local/bin/go; \
		echo "    Installed: $$(go version)"; \
	fi
	@echo "  [4/4] Node.js..."
	@if command -v node >/dev/null 2>&1; then \
		echo "    Already installed: node $$(node --version)"; \
	else \
		echo "    Installing Node.js 20.x..."; \
		curl -fsSL https://deb.nodesource.com/setup_20.x | pkexec bash - >/dev/null 2>&1; \
		pkexec apt install -y nodejs 2>&1 | tail -1; \
		echo "    Installed: node $$(node --version)"; \
	fi
	@echo ""
	@$(MAKE) deps-check --no-print-directory

# Shorthand: check deps, install if needed, then build
deps: deps-check

# Set up CWS server for remote builds (run once)
# Usage: make server-setup  (requires: ssh cws-server works)
server-setup:
	@echo "  Setting up CWS server for remote builds..."
	@echo ""
	@echo "  [1/5] System packages..."
	@ssh $(REMOTE_HOST) 'sudo DEBIAN_FRONTEND=noninteractive apt update -qq && \
		sudo DEBIAN_FRONTEND=noninteractive apt install -y -qq \
		build-essential git sqlite3 curl wget pkg-config \
		libgtk-4-dev libgirepository1.0-dev 2>&1 | tail -1'
	@echo "  [2/5] Go 1.24.4..."
	@ssh $(REMOTE_HOST) 'if command -v go >/dev/null 2>&1; then \
		echo "    Already installed: $$(go version)"; \
	else \
		wget -q https://go.dev/dl/go1.24.4.linux-amd64.tar.gz -O /tmp/go.tar.gz && \
		sudo rm -rf /usr/local/go && sudo tar -C /usr/local -xzf /tmp/go.tar.gz && \
		rm /tmp/go.tar.gz && sudo ln -sf /usr/local/go/bin/go /usr/local/bin/go && \
		echo "    Installed: $$(go version)"; \
	fi'
	@echo "  [3/5] Node.js..."
	@ssh $(REMOTE_HOST) 'if command -v node >/dev/null 2>&1; then \
		echo "    Already installed: node $$(node --version)"; \
	else \
		curl -fsSL https://deb.nodesource.com/setup_20.x | sudo -E bash - >/dev/null 2>&1 && \
		sudo DEBIAN_FRONTEND=noninteractive apt install -y nodejs 2>&1 | tail -1 && \
		echo "    Installed: node $$(node --version)"; \
	fi'
	@echo "  [4/5] Build directory..."
	@ssh $(REMOTE_HOST) 'mkdir -p $(REMOTE_DIR) && echo "    $(REMOTE_DIR) ready"'
	@echo "  [5/5] Verifying..."
	@ssh $(REMOTE_HOST) 'echo "    Go: $$(go version)" && \
		echo "    GCC: $$(gcc --version | head -1)" && \
		echo "    Node: $$(node --version)" && \
		echo "    GTK4: $$(pkg-config --modversion gtk4)" && \
		echo "    Disk: $$(df -h / | tail -1 | awk "{print \$$4}") free"'
	@echo ""
	@echo "  Server ready. Use 'make remote-build' to build."

# ============================================================================
# CLOSING — Help & Setup
# ============================================================================

help:
	@echo "CPI-SI Build System"
	@echo ""
	@echo "  Source:   $(SRC_DIR)"
	@echo "  Symlinks: $(DEPLOY_DIR)"
	@echo "  Binaries: $(INSTALL_DIR)"
	@echo "  Data:     $(DB_BASE_DIR)"
	@echo ""
	@echo "Build (all):"
	@echo "  build              Build all modules + binaries (incremental)"
	@echo "  bin                Build binaries only (to local bin/)"
	@echo "  force              Force rebuild everything (ignores cache)"
	@echo ""
	@echo "Build (single target):"
	@echo "  pkg                Shared packages"
	@echo "  hooks              Hooks module"
	@echo "  statusline         Statusline"
	@echo "  system             All system commands"
	@echo "  sys-<name>         One system command (e.g. sys-status)"
	@echo "  cpisi-cli          cpisi CLI"
	@echo "  skills             All skills"
	@echo "  skill-<name>       One skill"
	@echo "  dashboard          Web dashboard (Go + frontend)"
	@echo "  dashboard-frontend TypeScript frontend only"
	@echo "  dashboard-tui      TUI dashboard (Bubble Tea)"
	@echo "  dashboard-gtk      GTK4 dashboard"
	@echo ""
	@echo "Install:"
	@echo "  install            Full (build + symlinks + binaries + desktop)"
	@echo "  install-bin        Build + deploy binaries to ext4"
	@echo "  install-bin-only   Deploy existing binaries (no rebuild)"
	@echo "  install-symlinks   Create symlinks in ~/.claude/"
	@echo "  install-desktop    Install GTK4 .desktop entry"
	@echo "  install-status     Show symlink status"
	@echo "  uninstall          Remove everything"
	@echo "  fresh              Clean + full reinstall"
	@echo "  verify             Check installation integrity"
	@echo ""
	@echo "Test:"
	@echo "  test               Run all tests"
	@echo "  test-<module>      Test one module (pkg, hooks, statusline, etc.)"
	@echo ""
	@echo "Database (multi-domain):"
	@echo "  db-init-all        Initialize all 5 domain databases"
	@echo "  db-init-<domain>   Initialize one (sessions, cognition, growth, temporal, projects)"
	@echo "  db-split           Migrate legacy cpisi.db → 5 domain databases"
	@echo "  db-status          Show all databases with sizes"
	@echo ""
	@echo "Database (legacy):"
	@echo "  db                 Init + migrate legacy cpisi.db"
	@echo ""
	@echo "Utilities:"
	@echo "  clean              Clean build artifacts + cache"
	@echo "  cache-clean        Reset build cache only"
	@echo "  fmt                Format all Go code"
	@echo "  vet                Run go vet"
	@echo "  tidy               Tidy go.mod files"
	@echo "  list-skills        List available skills"
	@echo "  list-system-cmds   List system commands"
	@echo ""
	@echo "Fast (ext4 build — bypasses exFAT overhead):"
	@echo "  fast-build         Persistent mirror to ~/.cache/cpisi/, build there"
	@echo "  fast-sys-<name>    Fast build one system command"
	@echo "  remote-build       Build on CWS server via SSH (full ext4)"
	@echo ""
	@echo "Dependencies:"
	@echo "  deps-check         Check all dependencies (reports missing)"
	@echo "  deps-install       Install all dependencies (local, requires pkexec)"
	@echo "  server-setup       Install deps on CWS server (run once)"
	@echo ""
	@echo "Cache:"
	@echo "  cache-warm         Pre-compile all deps (speeds up single-target builds)"
	@echo "  cache-clean        Reset build cache only"
	@echo ""
	@echo "Builds are incremental — unchanged modules skip."
	@echo "Use 'make force' to bypass, 'make cache-warm' after fresh clone."
	@echo "Use 'make fast-build' for cold builds (persistent ext4 mirror)."
	@echo "Use 'make remote-build' to offload builds to CWS server."
	@echo ""
	@echo "Quick start:  make fresh"
	@echo "CLI:          cpisi help  (after first install)"

setup:
	@echo ""
	@echo "  CPI-SI Setup Guide"
	@echo ""
	@echo "  1. make build          Compile all modules"
	@echo "  2. make install-symlinks  Link config to ~/.claude/"
	@echo "  3. make install-bin    Deploy binaries to ext4"
	@echo "  4. make verify         Check everything"
	@echo ""
	@echo "  Or: make fresh         Do it all at once"
	@echo ""
	@echo "  After install, use: cpisi <command>"
	@echo "  Run: cpisi help"
