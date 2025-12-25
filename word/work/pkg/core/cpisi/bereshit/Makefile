# ============================================================================
# Bereshit Package Makefile
# ============================================================================
#
# CPI-SI Bereshit Loader — Transliteration Tables from Bereshit TOML
#
# Traces to: HALT_01 (Genesis 1:1) via HALT_08 (ENCODING)
#
# Data source:
#   /media/seanje-lenox-wise/Project/Bereshit/word/core/bible/
#   - encryption.toml  (Letter → Position → Coords → Value)
#   - decryption.toml  (Position → Letter reverse lookup)
#
# ============================================================================

.PHONY: all build test clean help assure

# Default target
all: build test

# Build the package
build:
	@echo "Building bereshit package..."
	@go build ./...

# Run tests (verbose)
test:
	@echo "Running bereshit tests..."
	@go test -v ./...

# Run tests (short output)
test-short:
	@go test ./...

# Run assurance suite only
assure:
	@echo "Running assurance suite..."
	@go test -v -run TestRunAllAssurances ./...

# Verify data files exist (in Bereshit repo)
verify-data:
	@echo "Verifying Bereshit data files..."
	@test -f /media/seanje-lenox-wise/Project/Bereshit/word/core/bible/encryption.toml || (echo "ERROR: encryption.toml missing" && exit 1)
	@test -f /media/seanje-lenox-wise/Project/Bereshit/word/core/bible/decryption.toml || (echo "ERROR: decryption.toml missing" && exit 1)
	@echo "All data files present."

# Clean build artifacts
clean:
	@echo "Cleaning..."
	@go clean ./...

# Help
help:
	@echo "Bereshit Package Makefile"
	@echo ""
	@echo "Targets:"
	@echo "  all           Build and test (default)"
	@echo "  build         Build the package"
	@echo "  test          Run tests (verbose)"
	@echo "  test-short    Run tests (short output)"
	@echo "  assure        Run assurance suite only"
	@echo "  verify-data   Check Bereshit TOML files exist"
	@echo "  clean         Clean build artifacts"
	@echo "  help          Show this help"
	@echo ""
	@echo "Data Source:"
	@echo "  /Project/Bereshit/word/core/bible/encryption.toml"
	@echo "  /Project/Bereshit/word/core/bible/decryption.toml"
	@echo ""
	@echo "HALT Trace:"
	@echo "  HALT_08 (ENCODING) → HALT_09 (DECODING) → HALT_03 (PSI=27)"
	@echo "  → HALT_10 (7 States) → HALT_01 (Genesis 1:1)"
