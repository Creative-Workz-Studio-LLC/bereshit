# ── CWS Server Makefile ──────────────────────────────────────────────
# Build, run, deploy, and manage the CWS Server.
#
# Targets:
#   make build         — Build server binary
#   make run           — Build and run server (dev mode)
#   make check         — Run go vet and build check
#   make deploy        — Full production deploy (build + compile + sync + restart)
#   make deploy-binary — Deploy only the Go binary
#   make deploy-dash   — Deploy dashboard frontend + config
#   make deploy-site   — Deploy website static files
#   make deploy-builder— Deploy builder CLI + deps
#   make prod-status   — Check production health
#   make prod-logs     — Tail production logs
#   make help          — Show this help
# ────────────────────────────────────────────────────────────────────

SHELL     := /bin/bash
.DEFAULT_GOAL := help

# ── Paths ───────────────────────────────────────────────────────────

ROOT      := $(shell pwd)
# Build to native filesystem (exfat can't set execute permissions)
BIN       := /home/seanje-lenox-wise/.local/bin/cws-server
CMD       := $(ROOT)/cmd/cws-server

# Default project directory (auto-detected if not set)
BUILDER_DIR ?= $(realpath $(ROOT)/../company-docs)

# Server options
PORT      ?= 3847

# ── Source Paths (workstation) ──────────────────────────────────────

DASHBOARD_SRC  := $(BUILDER_DIR)/build/dashboard/src-web
DASHBOARD_ROOT := $(BUILDER_DIR)/build/dashboard
BUILDER_DIST   := $(BUILDER_DIR)/build/builder/dist
BUILDER_PKG    := $(BUILDER_DIR)/build/builder/package.json
CONFIG         := $(BUILDER_DIR)/build/build.config.yaml
THEMES         := $(BUILDER_DIR)/build/themes
COMPANY_MK     := $(BUILDER_DIR)/Makefile
WEBSITE_SRC    := $(ROOT)/website

# ── Production Targets ────────────────────────────────────────────────
#
# PROD_TARGET controls which server receives deployments.
#   make deploy                    → deploys to current PROD_TARGET (oracle)
#   make deploy PROD_TARGET=dell   → deploys to Dell (LAN fallback)

PROD_TARGET ?= oracle

# Oracle Cloud (primary — ARM64)
ORACLE_HOST := seanje@163.192.118.124
# Dell Inspiron (fallback — x86_64, LAN only)
DELL_HOST   := seanje@192.168.1.99

ifeq ($(PROD_TARGET),dell)
  PROD_HOST := $(DELL_HOST)
else
  PROD_HOST := $(ORACLE_HOST)
endif

PROD_BIN  := /usr/local/bin/cws-server
PROD_DIR  := /home/seanje/cws

# ── Build ───────────────────────────────────────────────────────────

.PHONY: build
build: ## Build server binary
	cd $(CMD) && go build -o $(BIN) .
	@echo "Built: $(BIN)"

.PHONY: build-release
build-release: ## Build optimized release binary
	cd $(CMD) && go build -ldflags="-s -w" -o $(BIN) .
	@echo "Built (release): $(BIN)"

.PHONY: build-arm64
build-arm64: ## Cross-compile for linux/arm64 (Oracle Cloud)
	cd $(CMD) && CGO_ENABLED=0 GOOS=linux GOARCH=arm64 go build -ldflags="-s -w" -o $(BIN)-arm64 .
	@echo "Built (arm64): $(BIN)-arm64"

.PHONY: frontend
frontend: ## Compile dashboard TypeScript
	cd $(DASHBOARD_ROOT) && npx tsc -p tsconfig.web.json
	@echo "Dashboard TypeScript compiled"

# ── Run ─────────────────────────────────────────────────────────────

.PHONY: run
run: build ## Build and run server (dev mode)
	$(BIN) --dev --port $(PORT) --builder-dir $(BUILDER_DIR)

.PHONY: run-quick
run-quick: ## Run without rebuilding (must build first)
	$(BIN) --dev --port $(PORT) --builder-dir $(BUILDER_DIR)

# ── Check ───────────────────────────────────────────────────────────

.PHONY: check
check: ## Run go vet and build check
	cd $(ROOT) && go vet ./...
	cd $(CMD) && go build -o /dev/null .
	@echo "All checks passed"

.PHONY: tidy
tidy: ## Run go mod tidy
	cd $(ROOT) && go mod tidy

# ── Deploy ──────────────────────────────────────────────────────────
#
# Full deploy: build-release → compile TS → sync everything → restart
#
# Components deployed:
#   1. Go binary         → /usr/local/bin/cws-server
#   2. Dashboard frontend → $(PROD_DIR)/build/dashboard/src-web/
#   3. Builder CLI dist  → $(PROD_DIR)/build/builder/dist/
#   4. Builder deps      → $(PROD_DIR)/build/builder/node_modules/
#   5. Website           → $(PROD_DIR)/website/
#   6. Config + themes   → $(PROD_DIR)/build/
#   7. Makefile          → $(PROD_DIR)/Makefile
# ────────────────────────────────────────────────────────────────────

# Choose the right binary for the target architecture.
ifeq ($(PROD_TARGET),dell)
  DEPLOY_BIN := $(BIN)
  BUILD_DEP  := build-release
else
  DEPLOY_BIN := $(BIN)-arm64
  BUILD_DEP  := build-arm64
endif

.PHONY: deploy
deploy: $(BUILD_DEP) frontend ## Full production deploy
	@echo "═══════════════════════════════════════════════"
	@echo "  CWS Server — Full Production Deploy ($(PROD_TARGET))"
	@echo "═══════════════════════════════════════════════"
	@echo ""
	@# ── Binary ──
	@echo "▸ Deploying binary..."
	scp -q $(DEPLOY_BIN) $(PROD_HOST):/tmp/cws-server
	ssh $(PROD_HOST) "sudo install -m 755 /tmp/cws-server $(PROD_BIN) && rm /tmp/cws-server"
	@echo "  ✓ Binary installed"
	@echo ""
	@# ── Dashboard frontend ──
	@echo "▸ Syncing dashboard frontend..."
	rsync -az --delete \
		--exclude='*.ts' \
		--exclude='tsconfig*.json' \
		--exclude='.tsbuildinfo' \
		$(DASHBOARD_SRC)/ $(PROD_HOST):$(PROD_DIR)/build/dashboard/src-web/
	@echo "  ✓ Dashboard synced"
	@echo ""
	@# ── Builder CLI ──
	@echo "▸ Syncing builder CLI..."
	rsync -az --delete $(BUILDER_DIST)/ $(PROD_HOST):$(PROD_DIR)/build/builder/dist/
	@# Only npm install if package.json changed (compare checksums).
	@LOCAL_SUM=$$(md5sum $(BUILDER_PKG) | cut -d' ' -f1); \
	REMOTE_SUM=$$(ssh $(PROD_HOST) "md5sum $(PROD_DIR)/build/builder/package.json 2>/dev/null | cut -d' ' -f1" || echo "none"); \
	if [ "$$LOCAL_SUM" != "$$REMOTE_SUM" ]; then \
		echo "  package.json changed — running npm install..."; \
		scp -q $(BUILDER_PKG) $(PROD_HOST):$(PROD_DIR)/build/builder/package.json; \
		ssh $(PROD_HOST) "cd $(PROD_DIR)/build/builder && npm install --omit=dev --no-bin-links --silent 2>&1 | tail -1"; \
	else \
		echo "  package.json unchanged — skipping npm install"; \
	fi
	@echo "  ✓ Builder CLI synced"
	@echo ""
	@# ── Website ──
	@echo "▸ Syncing website..."
	rsync -az --delete $(WEBSITE_SRC)/ $(PROD_HOST):$(PROD_DIR)/website/
	@echo "  ✓ Website synced"
	@echo ""
	@# ── Config, themes, Makefile ──
	@echo "▸ Syncing config & themes..."
	scp -q $(CONFIG) $(PROD_HOST):$(PROD_DIR)/build/build.config.yaml
	rsync -az $(THEMES)/ $(PROD_HOST):$(PROD_DIR)/build/themes/
	scp -q $(COMPANY_MK) $(PROD_HOST):$(PROD_DIR)/Makefile
	@echo "  ✓ Config, themes, Makefile synced"
	@echo ""
	@# ── Restart & verify ──
	@echo "▸ Restarting service..."
	ssh $(PROD_HOST) "sudo systemctl restart cws-server"
	@sleep 2
	@echo ""
	@curl -sf https://dashboard.creativeworkzstudio.com/healthz > /dev/null && \
		echo "═══════════════════════════════════════════════" && \
		echo "  Deploy complete" && \
		echo "  https://creativeworkzstudio.com" && \
		echo "  https://dashboard.creativeworkzstudio.com" && \
		echo "═══════════════════════════════════════════════" || \
		(echo "⚠  Warning: health check failed — check prod-logs" && exit 1)

# ── Partial Deploy Targets ──────────────────────────────────────────

.PHONY: deploy-binary
deploy-binary: $(BUILD_DEP) ## Deploy only the Go binary + restart
	scp -q $(DEPLOY_BIN) $(PROD_HOST):/tmp/cws-server
	ssh $(PROD_HOST) "sudo install -m 755 /tmp/cws-server $(PROD_BIN) && rm /tmp/cws-server && sudo systemctl restart cws-server"
	@echo "Binary deployed and service restarted"

.PHONY: deploy-dash
deploy-dash: frontend ## Deploy dashboard frontend + config + themes
	rsync -az --delete \
		--exclude='*.ts' \
		--exclude='tsconfig*.json' \
		--exclude='.tsbuildinfo' \
		$(DASHBOARD_SRC)/ $(PROD_HOST):$(PROD_DIR)/build/dashboard/src-web/
	scp -q $(CONFIG) $(PROD_HOST):$(PROD_DIR)/build/build.config.yaml
	rsync -az $(THEMES)/ $(PROD_HOST):$(PROD_DIR)/build/themes/
	@echo "Dashboard, config, and themes deployed (no restart needed)"

.PHONY: deploy-site
deploy-site: ## Deploy website to Oracle + Cloudflare Pages
	rsync -az --delete $(WEBSITE_SRC)/ $(PROD_HOST):$(PROD_DIR)/website/
	@echo "  ✓ Website synced to Oracle"
	npx wrangler pages deploy $(WEBSITE_SRC) --project-name cws-website
	@echo "  ✓ Website deployed to Cloudflare Pages"

.PHONY: deploy-builder
deploy-builder: ## Deploy builder CLI dist + deps
	rsync -az --delete $(BUILDER_DIST)/ $(PROD_HOST):$(PROD_DIR)/build/builder/dist/
	@LOCAL_SUM=$$(md5sum $(BUILDER_PKG) | cut -d' ' -f1); \
	REMOTE_SUM=$$(ssh $(PROD_HOST) "md5sum $(PROD_DIR)/build/builder/package.json 2>/dev/null | cut -d' ' -f1" || echo "none"); \
	if [ "$$LOCAL_SUM" != "$$REMOTE_SUM" ]; then \
		echo "package.json changed — running npm install..."; \
		scp -q $(BUILDER_PKG) $(PROD_HOST):$(PROD_DIR)/build/builder/package.json; \
		ssh $(PROD_HOST) "cd $(PROD_DIR)/build/builder && npm install --omit=dev --no-bin-links --silent 2>&1 | tail -1"; \
	else \
		echo "package.json unchanged — skipping npm install"; \
	fi
	scp -q $(COMPANY_MK) $(PROD_HOST):$(PROD_DIR)/Makefile
	@echo "Builder CLI and Makefile deployed"

.PHONY: deploy-fly
deploy-fly: ## Deploy to Fly.io (standby/failover)
	@echo "═══════════════════════════════════════════════"
	@echo "  CWS Server — Fly.io Standby Deploy"
	@echo "═══════════════════════════════════════════════"
	cd $(ROOT)/.. && $(HOME)/.fly/bin/flyctl deploy \
		--app cws-server \
		--config server/fly.toml \
		--dockerfile server/Dockerfile \
		--remote-only
	@echo ""
	@curl -sf https://cws-server.fly.dev/healthz > /dev/null && \
		echo "Fly.io deploy complete — https://cws-server.fly.dev" || \
		echo "⚠  Warning: Fly.io health check failed"

.PHONY: fly-status
fly-status: ## Check Fly.io standby status
	@$(HOME)/.fly/bin/flyctl status --app cws-server 2>/dev/null || echo "flyctl not found"
	@echo ""
	@curl -sf https://cws-server.fly.dev/api/version | python3 -m json.tool 2>/dev/null || echo "Fly.io standby unreachable (may be auto-stopped)"

.PHONY: deploy-scripts
deploy-scripts: ## Deploy monitoring scripts to production
	ssh $(PROD_HOST) "mkdir -p $(PROD_DIR)/scripts $(PROD_DIR)/logs"
	rsync -az $(ROOT)/scripts/ $(PROD_HOST):$(PROD_DIR)/scripts/
	ssh $(PROD_HOST) "chmod 755 $(PROD_DIR)/scripts/*.sh"
	@echo "Scripts deployed"

# ── Production Management ───────────────────────────────────────────

.PHONY: prod-status
prod-status: ## Check production server status
	@ssh $(PROD_HOST) "sudo systemctl status cws-server --no-pager -l | head -12"
	@echo ""
	@curl -sf https://dashboard.creativeworkzstudio.com/api/version | python3 -m json.tool 2>/dev/null || echo "Server unreachable"

.PHONY: prod-logs
prod-logs: ## Tail production server logs
	ssh $(PROD_HOST) "sudo journalctl -u cws-server -f"

.PHONY: prod-restart
prod-restart: ## Restart production service
	ssh $(PROD_HOST) "sudo systemctl restart cws-server"
	@sleep 2
	@curl -sf https://dashboard.creativeworkzstudio.com/healthz > /dev/null && \
		echo "Service restarted — healthy" || \
		echo "⚠  Warning: health check failed after restart"

.PHONY: prod-health
prod-health: ## View watchdog health log
	@ssh $(PROD_HOST) "cat $(PROD_DIR)/logs/health.log 2>/dev/null || echo 'No health events logged (all checks passing)'"

.PHONY: prod-ssh
prod-ssh: ## SSH into production server
	ssh $(PROD_HOST)

# ── Clean ───────────────────────────────────────────────────────────

.PHONY: clean
clean: ## Remove build artifacts
	rm -rf $(ROOT)/bin/

# ── Help ────────────────────────────────────────────────────────────

.PHONY: help
help: ## Show available targets
	@echo "CWS Server — Build & Deploy"
	@echo "──────────────────────────────────────────────"
	@grep -E '^[a-zA-Z_-]+:.*##' $(MAKEFILE_LIST) | \
		awk 'BEGIN {FS = ":.*##"}; {printf "  \033[36m%-18s\033[0m %s\n", $$1, $$2}'
	@echo ""
	@echo "Variables:"
	@echo "  PORT=3847              Server port (dev)"
	@echo "  BUILDER_DIR=...        Path to company-docs/"
	@echo "  PROD_TARGET=oracle     Deploy target: oracle (default) or dell"
	@echo ""
	@echo "Deploy workflow:"
	@echo "  make deploy                  Full deploy to Oracle (ARM64)"
	@echo "  make deploy PROD_TARGET=dell Full deploy to Dell (x86_64)"
	@echo "  make deploy-binary           Go binary only (restarts service)"
	@echo "  make deploy-dash             Dashboard + config + themes (no restart)"
	@echo "  make deploy-site             Website → Oracle + Cloudflare Pages"
	@echo "  make deploy-builder          Builder CLI + deps + Makefile"
	@echo ""
	@echo "Production:"
	@echo "  make prod-status    Health check"
	@echo "  make prod-logs      Tail journalctl"
	@echo "  make prod-restart   Restart service"
	@echo "  make prod-ssh       SSH into server"
