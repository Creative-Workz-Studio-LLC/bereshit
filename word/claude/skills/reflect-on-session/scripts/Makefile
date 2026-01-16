# ============================================================================
# METADATA - Reflect on Session Build
# ============================================================================
# Purpose: Build the reflect CLI tool
# Biblical: Psalm 77:12 - "I will meditate also of all thy work"

.PHONY: build clean test

# ============================================================================
# BODY - Targets
# ============================================================================

build:
	@echo "Building reflect..."
	@mkdir -p bin
	go build -v -o bin/reflect ./cmd/reflect
	@echo "Built: bin/reflect"

clean:
	@rm -rf bin/
	@echo "Cleaned."

test:
	go test -v ./...
