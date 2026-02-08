# ── CWS Server Makefile ──────────────────────────────────────────────
# Build, run, and manage the CWS Server.
#
# Targets:
#   make build      — Build server binary
#   make run        — Build and run server (dev mode)
#   make check      — Run go vet and build check
#   make clean      — Remove build artifacts
#   make help       — Show this help
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

# Production server (Dell Inspiron)
PROD_HOST := seanje@192.168.1.99
PROD_BIN  := /usr/local/bin/cws-server
PROD_DIR  := /home/seanje/cws
DASHBOARD := $(realpath $(ROOT)/../company-docs/build/dashboard/src-web)
CONFIG    := $(realpath $(ROOT)/../company-docs/build/build.config.yaml)

# ── Build ───────────────────────────────────────────────────────────

.PHONY: build
build: ## Build server binary
	cd $(CMD) && go build -o $(BIN) .
	@echo "Built: $(BIN)"

.PHONY: build-release
build-release: ## Build optimized release binary
	cd $(CMD) && go build -ldflags="-s -w" -o $(BIN) .
	@echo "Built (release): $(BIN)"

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

# ── Deploy ─────────────────────────────────────────────────────────

.PHONY: deploy
deploy: build-release ## Deploy to production server (Dell)
	@echo "Deploying to $(PROD_HOST)..."
	scp $(BIN) $(PROD_HOST):/tmp/cws-server
	ssh $(PROD_HOST) "sudo install -m 755 /tmp/cws-server $(PROD_BIN) && rm /tmp/cws-server"
	@echo "Binary deployed"
	rsync -az --delete $(DASHBOARD)/ $(PROD_HOST):$(PROD_DIR)/build/dashboard/src-web/
	@echo "Frontend synced"
	scp $(CONFIG) $(PROD_HOST):$(PROD_DIR)/build/build.config.yaml
	@echo "Config synced"
	ssh $(PROD_HOST) "sudo systemctl restart cws-server"
	@echo "Service restarted"
	@sleep 2
	@curl -sf https://dashboard.creativeworkzstudio.com/healthz > /dev/null && \
		echo "Deploy complete — https://dashboard.creativeworkzstudio.com is live" || \
		echo "Warning: health check failed"

.PHONY: deploy-binary
deploy-binary: build-release ## Deploy only the server binary
	scp $(BIN) $(PROD_HOST):/tmp/cws-server
	ssh $(PROD_HOST) "sudo install -m 755 /tmp/cws-server $(PROD_BIN) && rm /tmp/cws-server && sudo systemctl restart cws-server"
	@echo "Binary deployed and service restarted"

.PHONY: deploy-frontend
deploy-frontend: ## Deploy only frontend + config
	rsync -az --delete $(DASHBOARD)/ $(PROD_HOST):$(PROD_DIR)/build/dashboard/src-web/
	scp $(CONFIG) $(PROD_HOST):$(PROD_DIR)/build/build.config.yaml
	@echo "Frontend and config deployed (no restart needed — static files)"

.PHONY: prod-status
prod-status: ## Check production server status
	@ssh $(PROD_HOST) "sudo systemctl status cws-server --no-pager -l | head -12"
	@echo ""
	@curl -sf https://dashboard.creativeworkzstudio.com/api/version | python3 -m json.tool 2>/dev/null || echo "Server unreachable"

.PHONY: prod-logs
prod-logs: ## Tail production server logs
	ssh $(PROD_HOST) "sudo journalctl -u cws-server -f"

# ── Clean ───────────────────────────────────────────────────────────

.PHONY: clean
clean: ## Remove build artifacts
	rm -rf $(ROOT)/bin/

# ── Help ────────────────────────────────────────────────────────────

.PHONY: help
help: ## Show available targets
	@echo "CWS Server — Build Targets"
	@echo "──────────────────────────────────────────────"
	@grep -E '^[a-zA-Z_-]+:.*##' $(MAKEFILE_LIST) | \
		awk 'BEGIN {FS = ":.*##"}; {printf "  \033[36m%-16s\033[0m %s\n", $$1, $$2}'
	@echo ""
	@echo "Variables:"
	@echo "  PORT=3847         Server port"
	@echo "  BUILDER_DIR=...   Path to company-docs/"
	@echo ""
	@echo "  PROD_HOST=...     Production server SSH target"
	@echo ""
	@echo "Examples:"
	@echo "  make run            # Build and start dev server"
	@echo "  make run PORT=8080  # Custom port"
	@echo "  make check          # Verify code compiles"
	@echo "  make deploy         # Build + deploy to production"
	@echo "  make prod-status    # Check production health"
	@echo "  make prod-logs      # Tail production logs"
