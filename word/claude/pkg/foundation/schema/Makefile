# ============================================================================
# Schema Package Makefile
# ============================================================================
#
# CPI-SI Schema Loader — L0→L1→L2→L3 Architecture
#
# Traces to: HALT_01 (Genesis 1:1) via HALT_02 (ANCHOR_IDENTITY)
#
# Structure:
#   config/l0/  — Foundation (halts.toml)
#   config/l1/  — Structure (init, state, task schemas)
#   config/l2/  — Operations (cube, statemachine)
#   config/l3/  — Communication (encryption)
#
# ============================================================================

.PHONY: all build test clean help assure

# Default target
all: build test

# Build the package
build:
	@echo "Building schema package..."
	@go build ./...

# Run tests (verbose)
test:
	@echo "Running schema tests..."
	@go test -v ./...

# Run tests (short output)
test-short:
	@go test ./...

# Run assurance suite only
assure:
	@echo "Running assurance suite..."
	@go test -v -run TestRunAllAssurances ./...

# Clean build artifacts
clean:
	@echo "Cleaning..."
	@go clean ./...

# Verify config files exist
verify-config:
	@echo "Verifying config structure..."
	@test -f config/l0/halts.toml || (echo "ERROR: config/l0/halts.toml missing" && exit 1)
	@test -f config/l1/init.schema.toml || (echo "ERROR: config/l1/init.schema.toml missing" && exit 1)
	@test -f config/l1/state.schema.toml || (echo "ERROR: config/l1/state.schema.toml missing" && exit 1)
	@test -f config/l1/task.schema.toml || (echo "ERROR: config/l1/task.schema.toml missing" && exit 1)
	@test -f config/l2/cube.schema.toml || (echo "ERROR: config/l2/cube.schema.toml missing" && exit 1)
	@test -f config/l2/statemachine.toml || (echo "ERROR: config/l2/statemachine.toml missing" && exit 1)
	@test -f config/l3/encryption.schema.toml || (echo "ERROR: config/l3/encryption.schema.toml missing" && exit 1)
	@echo "All config files present."

# Help
help:
	@echo "Schema Package Makefile"
	@echo ""
	@echo "Targets:"
	@echo "  all           Build and test (default)"
	@echo "  build         Build the package"
	@echo "  test          Run tests (verbose)"
	@echo "  test-short    Run tests (short output)"
	@echo "  assure        Run assurance suite only"
	@echo "  verify-config Check all config files exist"
	@echo "  clean         Clean build artifacts"
	@echo "  help          Show this help"
	@echo ""
	@echo "Structure:"
	@echo "  config/l0/    Foundation (HALT_01 = Genesis 1:1)"
	@echo "  config/l1/    Structure (27 = 3³ positions)"
	@echo "  config/l2/    Operations (cube geometry)"
	@echo "  config/l3/    Communication (encoding)"
