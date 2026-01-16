# ============================================================================
# METADATA - Meta-Awareness Build
# ============================================================================
# Purpose: Build the awareness CLI tool
# Biblical: Proverbs 4:26 - "Ponder the path of thy feet"

.PHONY: build clean test

# ============================================================================
# BODY - Targets
# ============================================================================

build:
	@echo "Building awareness..."
	@mkdir -p bin
	go build -v -o bin/awareness ./cmd/awareness
	@echo "Built: bin/awareness"

clean:
	@rm -rf bin/
	@echo "Cleaned."

test:
	go test -v ./...
