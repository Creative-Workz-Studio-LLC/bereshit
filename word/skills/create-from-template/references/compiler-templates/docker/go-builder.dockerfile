# ═══════════════════════════════════════════════════════════════════════════
# TEMPLATE: Dockerfile for Go Projects (4-Block Structure)
# Key: CODE-DOCKER-001
# ═══════════════════════════════════════════════════════════════════════════
#
# This is a TEMPLATE file - copy and modify for new Go project Dockerfiles.
# Replace all [bracketed] placeholders with actual content.
# Rename to Dockerfile (remove CODE-DOCKER-001-DOCKER- prefix).
#
# Derived from: verified Go build process (go build, go test)
# See: standards/code/4-block/ for complete documentation
# See: https://docs.docker.com/reference/dockerfile/
#
# ═══════════════════════════════════════════════════════════════════════════

# =============================================================================
# METADATA
# =============================================================================
#
# # Biblical Foundation
#
# Scripture: "For which of you, intending to build a tower, sitteth not down
# first, and counteth the cost, whether he have sufficient to finish it?"
# - Luke 14:28
#
# Principle: Docker images are like building a tower - we define each layer
# intentionally, count the cost (image size, build time), and construct
# reproducible builds that work the same everywhere.
#
# Anchor: "Except the LORD build the house, they labour in vain that build it."
# - Psalm 127:1
#
# # CPI-SI Identity
#
# Component Type: Rails
#
# Dockerfiles provide infrastructure for reproducible builds. They don't
# transform data (Baton) or provide foundations (Ladder), but ensure
# consistent build environments across all systems.
#
# Role: Container image definition for Go application builds
#
# Paradigm: CPI-SI framework component - build infrastructure
#
# # Authorship & Lineage
#
#   - Architect: [Who designed this Dockerfile]
#   - Implementation: [Who created and maintains this file]
#   - Created: [YYYY-MM-DD]
#   - Version: [MAJOR.MINOR.PATCH]
#   - Modified: [YYYY-MM-DD - what changed]
#
# # Purpose & Function
#
# Purpose: Build Go applications in isolated, reproducible containers
#
# Core Design: Multi-stage build for minimal final image
#
# Key Features:
#
#   - Multi-stage build (builder + runtime)
#   - Minimal final image (distroless or scratch)
#   - Build cache optimization
#   - Non-root user for security
#
# # Health Scoring
#
# Build Impact: Failed builds block deployment
#
# Validation Behavior:
#   - Syntax error: Build fails immediately
#   - Missing file: COPY fails with error
#   - Test failure: Build aborts (if tests enabled)
#
# =============================================================================
# END METADATA
# =============================================================================

# =============================================================================
# SETUP
# =============================================================================
#
# File Identity:
#   - Key: [PROJECT]-DOCKER-###
#   - Consumer: Docker Engine (docker build)
#   - Loader: Dockerfile parser
#
# Related Files:
#   - .dockerignore - Exclude files from build context
#   - docker-compose.yaml - Service orchestration
#   - Makefile - Build automation
#
# Build Commands:
#   - Build: docker build -t [image-name] .
#   - Build with arg: docker build --build-arg GO_VERSION=1.23 -t [name] .
#   - Run: docker run --rm [image-name]
#
# Build Arguments:
#   - GO_VERSION: Go compiler version (default: 1.23)
#   - CGO_ENABLED: C interop (0=pure Go, 1=allow cgo)
#
# =============================================================================
# END SETUP
# =============================================================================

# =============================================================================
# BODY
# =============================================================================

# -----------------------------------------------------------------------------
# Stage 1: Builder - Compile the Go application
# -----------------------------------------------------------------------------
# Uses official Go image for building. This stage is discarded in final image.

ARG GO_VERSION=1.23
FROM golang:${GO_VERSION}-bookworm AS builder

# Build arguments
ARG CGO_ENABLED=0
ARG GOPROXY=https://proxy.golang.org,direct

# Set environment
ENV CGO_ENABLED=${CGO_ENABLED}
ENV GOPROXY=${GOPROXY}
ENV GOOS=linux
ENV GOARCH=amd64

# Set working directory
WORKDIR /build

# Copy go.mod and go.sum first for better layer caching
# Only re-download dependencies when these files change
COPY go.mod go.sum ./
RUN go mod download && go mod verify

# Copy source code
COPY . .

# Build the application
# Replace [binary-name] and [main-path] with actual values
RUN go build -v -ldflags="-w -s" -o /app/[binary-name] [main-path]

# Optional: Run tests during build
# Uncomment to enforce tests pass before image is created
# RUN go test -v ./...

# -----------------------------------------------------------------------------
# Stage 2: Runtime - Minimal final image
# -----------------------------------------------------------------------------
# Uses distroless for security (no shell, minimal attack surface)
# Alternative: FROM scratch (even smaller, but no debugging tools)

FROM gcr.io/distroless/static-debian12:nonroot AS runtime

# Labels for image metadata
LABEL org.opencontainers.image.title="[Application Name]"
LABEL org.opencontainers.image.description="[Application description]"
LABEL org.opencontainers.image.source="https://github.com/Creative-Workz-Studio-LLC/[repo]"
LABEL org.opencontainers.image.vendor="CreativeWorkzStudio LLC"

# Copy binary from builder stage
COPY --from=builder /app/[binary-name] /app/[binary-name]

# Set working directory
WORKDIR /app

# Run as non-root user (nonroot user is UID 65532 in distroless)
USER nonroot:nonroot

# Expose port if application is a server
# EXPOSE [port]

# Set entrypoint
ENTRYPOINT ["/app/[binary-name]"]

# Default command arguments (can be overridden at runtime)
# CMD ["--help"]

# =============================================================================
# END BODY
# =============================================================================

# =============================================================================
# CLOSING
# =============================================================================
#
# -----------------------------------------------------------------------------
# Validation
# -----------------------------------------------------------------------------
#
# Verify Dockerfile is working:
#
#   # Lint Dockerfile
#   docker run --rm -i hadolint/hadolint < Dockerfile
#
#   # Build image
#   docker build -t [name]:test .
#
#   # Check image size
#   docker images [name]:test
#
#   # Test run
#   docker run --rm [name]:test --version
#
# Common Issues:
#
#   "COPY failed" - Check file exists in build context
#   "go mod download failed" - Check go.mod/go.sum are valid
#   "exec format error" - Check GOOS/GOARCH match target platform
#   "permission denied" - Check USER directive and file permissions
#
# -----------------------------------------------------------------------------
# Image Size Optimization
# -----------------------------------------------------------------------------
#
# Tips for minimal images:
#
#   - Use multi-stage builds (done in this template)
#   - Use -ldflags="-w -s" to strip debug info
#   - Use CGO_ENABLED=0 for static binaries
#   - Use distroless or scratch as base
#   - Order COPY by change frequency (least frequent first)
#   - Use .dockerignore to exclude unnecessary files
#
# Typical sizes:
#   - golang base: ~800MB (builder stage, discarded)
#   - distroless: ~20MB
#   - scratch: ~5MB (just the binary)
#
# -----------------------------------------------------------------------------
# Security Considerations
# -----------------------------------------------------------------------------
#
#   ✅ Non-root user (distroless nonroot)
#   ✅ Minimal base image (no shell, minimal tools)
#   ✅ Multi-stage build (build tools not in final image)
#   ✅ Static binary (no runtime dependencies)
#
#   Consider adding:
#   - HEALTHCHECK for container orchestration
#   - Read-only filesystem at runtime
#   - Resource limits in docker-compose/k8s
#
# -----------------------------------------------------------------------------
# Modification Policy
# -----------------------------------------------------------------------------
#
# Safe to Modify:
#   ✅ Change Go version (ARG GO_VERSION)
#   ✅ Add build flags to go build
#   ✅ Add environment variables
#   ✅ Change exposed ports
#   ✅ Add HEALTHCHECK directive
#
# Modify with Care:
#   ⚠️ Changing base image (affects security/size)
#   ⚠️ Adding RUN commands (each creates a layer)
#   ⚠️ Changing COPY order (affects caching)
#
# NEVER Modify:
#   ❌ Remove multi-stage build (bloats image)
#   ❌ Run as root in production
#   ❌ 4-block documentation structure
#
# -----------------------------------------------------------------------------
# .dockerignore Companion
# -----------------------------------------------------------------------------
#
# Create .dockerignore to exclude files from build context:
#
#   .git/
#   .gitignore
#   .env
#   *.md
#   Makefile
#   docs/
#   bin/
#   build/
#   vendor/
#   **/*_test.go
#
# -----------------------------------------------------------------------------
# Closing Note
# -----------------------------------------------------------------------------
#
# This Dockerfile builds Go applications in reproducible containers,
# following Kingdom Technology principles of intentional design.
#
# "For which of you, intending to build a tower, sitteth not down first,
# and counteth the cost?" - Luke 14:28
#
# =============================================================================
# END CLOSING
# =============================================================================
