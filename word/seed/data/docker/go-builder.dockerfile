# syntax=docker/dockerfile:1
#!omni template --docker
# ═══════════════════════════════════════════════════════════════════════════
# TEMPLATE: Dockerfile for Go Projects (4-Block Structure)
# Key: B-word-seed-data-docker-go-builder
# ═══════════════════════════════════════════════════════════════════════════
#
# TEMPLATE: This file is a template - copy and modify before using.
# Replace all [bracketed] placeholders with actual content.
# Remove the "TEMPLATE" markers when ready to build.
#
# DEPENDENCY CLASSIFICATION: DEPENDED (needs: golang base image)
#   - Go builder requires golang:X.XX-bookworm base image
#   - Multi-stage build: builder stage discarded, only binary in final image
#
# Derives from: B-word-omni-seed-data-4block
#               (bereshit/word/omni/seed/data-4block.omni)
# See: standards/code/4-block/ for complete documentation
# See: https://docs.docker.com/reference/dockerfile/
#
# ═══════════════════════════════════════════════════════════════════════════

# =============================================================================
# METADATA
# =============================================================================
#
# Package:     [organization/project-name]
# File:        Dockerfile
# Key:         [PROJECT-DOCKER-###] (Go application container)
#
# -----------------------------------------------------------------------------
# CORE IDENTITY (Required)
# -----------------------------------------------------------------------------
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
# Component Type: Rails (build infrastructure)
#
# Role: Container image definition for Go application builds
#
# Paradigm: CPI-SI framework component - build infrastructure
#
# # Authorship & Lineage
#
#   Architect: [Who designed this Dockerfile]
#   Implementation: [Who created and maintains this file]
#   Created: [YYYY-MM-DD]
#   Version: [MAJOR.MINOR.PATCH]
#   Modified: [YYYY-MM-DD - what changed]
#
# Version History:
#
#   [X.Y.Z] ([YYYY-MM-DD]) - [Brief description of changes]
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
# Philosophy: Intentional layers, minimal attack surface, reproducible builds
#
# -----------------------------------------------------------------------------
# INTERFACE (Expected)
# -----------------------------------------------------------------------------
#
# # Dependencies
#
# What This Needs:
#
#   - Base Image: golang:X.XX-bookworm (builder stage)
#   - Runtime Base: gcr.io/distroless/static-debian12:nonroot
#   - Source: go.mod, go.sum, *.go files
#
# What Uses This:
#
#   - docker-compose.yaml (service definition)
#   - CI/CD pipelines (automated builds)
#   - Developers (local development)
#
# Integration Points:
#
#   - Build args: GO_VERSION, CGO_ENABLED
#   - Output: Single static binary at /app/[binary-name]
#
# # Usage & Integration
#
# Build Commands:
#
#   docker build -t [image-name] .
#   docker build --build-arg GO_VERSION=1.23 -t [name] .
#
# Run Commands:
#
#   docker run --rm [image-name]
#   docker run --rm [image-name] --help
#
# -----------------------------------------------------------------------------
# OPERATIONAL (Contextual)
# -----------------------------------------------------------------------------
#
# # Blocking Status
#
# [OMIT: Dockerfile - build errors stop image creation, no runtime blocking concern]
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
# -----------------------------------------------------------------------------
# METADATA Omission Guide
# -----------------------------------------------------------------------------
#
# Tier 1 (CORE IDENTITY): Never omit - every file needs these.
#
# Tier 2 (INTERFACE): May omit with [OMIT: reason] notation.
#   - Dependencies: Required - documents Go version and base images
#   - Usage & Integration: Required - shows docker build/run commands
#
# Tier 3 (OPERATIONAL): Include when applicable to file type.
#   - Blocking Status: [OMIT: Dockerfile - build-time only]
#   - Health Scoring: Include build validation behavior
#
# Unlike SETUP (all sections required), METADATA omission signals component characteristics.
#
# =============================================================================
# END METADATA
# =============================================================================

# =============================================================================
# SETUP
# =============================================================================
#
# For SETUP structure explanation, see: standards/code/4-block/CWS-STD-006-CODE-setup-block.md
#
# -----------------------------------------------------------------------------
# SETUP Sections Overview
# -----------------------------------------------------------------------------
#
# 1. FILE IDENTITY (Dependencies)
#    Purpose: Establish what consumes this Dockerfile and how it's loaded
#    Subsections: Consumer → Loader → Build Context
#
# 2. CONSTANTS
#    [Reserved: Go Dockerfiles define constants inline via ENV in builder stage]
#
# 3. VARIABLES
#    [Reserved: ARG directives (GO_VERSION, CGO_ENABLED) defined inline in BODY]
#
# 4. RELATED FILES (Types)
#    Purpose: Document related configuration files
#    Subsections: Build Context → Go Modules → Orchestration
#
# 5. TYPE BEHAVIORS
#    [Reserved: Build commands (go build) defined inline in builder stage]
#
# 6. USAGE/BEHAVIOR (Rails Infrastructure)
#    Purpose: How this Dockerfile is consumed and built
#    Subsections: Build Commands → Run Commands → Build Arguments
#
# Section order: File Identity → [Reserved] → [Reserved] → Related Files → [Reserved] → Usage/Behavior
# This flows: what uses this → (constants inline) → (variables inline) → related files → (behaviors inline) → how it's built
#
# Universal mapping (see standards for cross-language patterns):
#   File Identity ≈ Dependencies (who consumes this)
#   Constants ≈ Constants [Reserved: ENV defined in builder stage]
#   Variables ≈ Variables [Reserved: ARG defined in builder stage]
#   Related Files ≈ Types (what files relate to this)
#   Type Behaviors ≈ Type Methods [Reserved: go build defined in stage]
#   Usage/Behavior ≈ Package-Level State (build/run pattern)
#
# -----------------------------------------------------------------------------
# File Identity (Dependencies)
# -----------------------------------------------------------------------------
#
# Key: [PROJECT]-DOCKER-### (unique identifier for this Go builder Dockerfile)
# Consumer: Docker Engine (docker build, docker buildx)
# Loader: Dockerfile parser (BuildKit recommended for multi-stage)
# =============================================================================

# -----------------------------------------------------------------------------
# Constants
# -----------------------------------------------------------------------------
#
# [Reserved: Go builder Dockerfiles define environment constants via ENV.
#  GOOS, GOARCH, CGO_ENABLED are set inline in builder stage in BODY
#  because they're specific to the compilation environment.]
# =============================================================================

# -----------------------------------------------------------------------------
# Variables
# -----------------------------------------------------------------------------
#
# [Reserved: Build-time variables (GO_VERSION, CGO_ENABLED, GOPROXY) are
#  defined via ARG inline in BODY where they're used. ARGs have stage scope.]
# =============================================================================

# -----------------------------------------------------------------------------
# Related Files (Types)
# -----------------------------------------------------------------------------
#
# Build Context:
#   - .dockerignore: Exclude files from build context
#   - go.mod, go.sum: Go module dependencies (copied first for caching)
#
# Orchestration:
#   - docker-compose.yaml: Service orchestration using this image
#   - Makefile: Build automation scripts
# =============================================================================

# -----------------------------------------------------------------------------
# Type Behaviors
# -----------------------------------------------------------------------------
#
# [Reserved: Go build commands are defined inline in builder stage:
#   - go mod download (dependency resolution)
#   - go build (compilation)
#   - go test (optional validation)
#  These belong in BODY because they're stage-specific operations.]
# =============================================================================

# -----------------------------------------------------------------------------
# Usage/Behavior (Rails Infrastructure)
# -----------------------------------------------------------------------------
#
# Build Commands:
#   docker build -t [image-name] .
#   docker build --build-arg GO_VERSION=1.23 -t [name] .
#   docker buildx build --platform linux/amd64,linux/arm64 -t [name] .
#
# Run Commands:
#   docker run --rm [image-name]
#   docker run --rm [image-name] --help
#
# Build Arguments (defined via ARG in builder stage):
#   - GO_VERSION: Go compiler version (default: 1.23)
#   - CGO_ENABLED: C interop (0=pure Go, 1=allow cgo)
#   - GOPROXY: Go module proxy (default: https://proxy.golang.org,direct)
# =============================================================================

# -----------------------------------------------------------------------------
# SETUP Omission Guide
# -----------------------------------------------------------------------------
#
# Unlike METADATA (where sections can be omitted with [OMIT: reason]),
# ALL six SETUP sections must be present for structural alignment.
#
# If a section has no content for this file:
#   - Keep the section header
#   - Add [Reserved: reason] comment explaining why empty
#   - This maintains the 6-section structure across all templates
#
# Go builder Dockerfile-specific guidance:
#   - File Identity: Always required (key, consumer, loader)
#   - Constants: [Reserved: GOOS/GOARCH/CGO_ENABLED in builder stage]
#   - Variables: [Reserved: ARG GO_VERSION in builder stage]
#   - Related Files: Required - documents go.mod, go.sum, .dockerignore
#   - Type Behaviors: [Reserved: go build commands in builder stage]
#   - Usage/Behavior: Required - shows docker build/run commands
#
# The goal is structural consistency - every CONFIG template has the same
# 6-section SETUP structure, making navigation and understanding predictable.
# =============================================================================

# =============================================================================
# END SETUP
# =============================================================================

# =============================================================================
# BODY
# =============================================================================
#
# For BODY structure explanation, see: standards/code/4-block/CWS-STD-007-CODE-body-block.md
#
# -----------------------------------------------------------------------------
# BODY Sections Overview
# -----------------------------------------------------------------------------
#
# 1. CONFIGURATION STRUCTURE MAP (Internal Organization)
#    Purpose: Document the multi-stage build organization and layer strategy
#    Subsections: Stage Overview → Layer Flow → Cache Strategy
#
# 2. HELPERS/UTILITIES
#    [Reserved: Declarative config - Dockerfiles don't have helper functions.
#     Build logic is expressed through instruction ordering and stage design.]
#
# 3. CONFIGURATION CONTENT (Core Data)
#    Purpose: The actual Dockerfile instructions organized by build stage
#    Subsections: Stage 1 (Builder) → Stage 2 (Runtime) → Labels/Metadata
#
# 4. ERROR HANDLING
#    [Reserved: Declarative config - Docker build validates at instruction level.
#     Build failures stop immediately with descriptive error messages.]
#
# 5. PUBLIC INTERFACE
#    [Reserved: Declarative config - exposed interface (EXPOSE, CMD, ENTRYPOINT)
#     is defined inline within Configuration Content as part of the runtime stage.]
#
# Section order: Configuration Structure Map → [Reserved] → Content → [Reserved] → [Reserved]
# This flows: how it's organized → (no helpers) → what it does → (build validates) → (interface inline)
#
# Universal mapping (see standards for cross-language patterns):
#   Configuration Structure Map ≈ Organizational Chart (internal structure)
#   Helpers/Utilities ≈ Helper Functions [Reserved: declarative config]
#   Configuration Content ≈ Core Operations (primary functionality)
#   Error Handling ≈ Error Handling [Reserved: docker build validates]
#   Public Interface ≈ Public APIs [Reserved: EXPOSE/CMD/ENTRYPOINT inline]
#
# -----------------------------------------------------------------------------
# 1. Configuration Structure Map (Internal Organization)
# -----------------------------------------------------------------------------
#
# Stage Architecture:
#
#   Stage 1 (builder):
#     Base: golang:X.XX-bookworm
#     Purpose: Compile Go source to static binary
#     Output: /app/[binary-name]
#     Discarded: Yes (not in final image)
#
#   Stage 2 (runtime):
#     Base: gcr.io/distroless/static-debian12:nonroot
#     Purpose: Minimal secure container for running binary
#     Output: Final published image
#     Discarded: No (this is the result)
#
# Layer Cache Strategy:
#   1. ARG/FROM (base image selection)
#   2. COPY go.mod, go.sum (dependencies - changes less often)
#   3. RUN go mod download (cached if deps unchanged)
#   4. COPY . (source code - changes most often)
#   5. RUN go build (recompile when source changes)
#
# This ordering maximizes cache hits - dependencies before source code.
#
# -----------------------------------------------------------------------------
# 2. Helpers/Utilities
# -----------------------------------------------------------------------------
#
# [Reserved: Go builder Dockerfiles are declarative instruction sequences.
#  There are no helper functions - build logic is expressed through:
#  - Multi-stage design (builder vs runtime separation)
#  - Instruction ordering (cache optimization)
#  - Build arguments (parameterization)]
#
# -----------------------------------------------------------------------------
# 3. Configuration Content (Core Data)
# -----------------------------------------------------------------------------
#
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

# -----------------------------------------------------------------------------
# 4. Error Handling
# -----------------------------------------------------------------------------
#
# [Reserved: Go builder Dockerfiles validate at instruction execution time.
#  Docker build stops immediately on any error with descriptive messages:
#  - Syntax errors detected by Dockerfile parser
#  - COPY failures when files don't exist in build context
#  - RUN failures when commands return non-zero exit codes
#  - FROM failures when base image can't be pulled
#  No programmatic error handling needed - the build system handles it.]
#
# -----------------------------------------------------------------------------
# 5. Public Interface
# -----------------------------------------------------------------------------
#
# [Reserved: The public interface of a Docker image is defined inline:
#  - EXPOSE: Declares which ports the container listens on
#  - CMD/ENTRYPOINT: Defines how the container executes
#  - LABEL: Provides metadata about the image
#  These are part of Configuration Content (Section 3) because they're
#  integral to the runtime stage definition, not separate exports.]
#
# -----------------------------------------------------------------------------
# BODY Omission Guide
# -----------------------------------------------------------------------------
#
# Unlike METADATA (where sections can be omitted with [OMIT: reason]),
# ALL five BODY sections must be present for structural alignment.
#
# If a section has no content for this file:
#   - Keep the section header
#   - Add [Reserved: reason] comment explaining why empty
#   - This maintains the 5-section structure across all templates
#
# Go builder Dockerfile-specific guidance:
#
#   1. Configuration Structure Map: Required - documents stage architecture and cache strategy
#   2. Helpers/Utilities: [Reserved: declarative config - no helper functions]
#   3. Configuration Content: Required - the actual Dockerfile instructions
#   4. Error Handling: [Reserved: docker build validates at instruction level]
#   5. Public Interface: [Reserved: EXPOSE/CMD/ENTRYPOINT defined inline in content]
#
# The goal is structural consistency - every CONFIG template has the same
# 5-section BODY structure, making navigation and understanding predictable.
#
# =============================================================================
# END BODY
# =============================================================================

# =============================================================================
# CLOSING
# =============================================================================
#
# For CLOSING structure explanation, see: standards/code/4-block/CWS-STD-008-CODE-closing-block.md
#
# -----------------------------------------------------------------------------
# CLOSING Sections Overview
# -----------------------------------------------------------------------------
#
# ┌─────────────────────────────────────────────────────────────────────────────┐
# │ GROUP 1: VALIDATION (Build-time concerns)                                   │
# ├─────────────────────────────────────────────────────────────────────────────┤
# │ 1. Validation           │ How to verify Dockerfile correctness              │
# │ 2. Build Commands       │ [Reserved: in SETUP - Usage/Behavior section]     │
# │ 3. Cleanup              │ [Reserved: multi-stage build discards builder]    │
# ├─────────────────────────────────────────────────────────────────────────────┤
# │ GROUP 2: FINAL DOCUMENTATION (Reference sections)                           │
# ├─────────────────────────────────────────────────────────────────────────────┤
# │ 4. Performance          │ Image size optimization and layer caching tips    │
# │ 5. Security             │ Security considerations for container images      │
# │ 6. Dependencies         │ [Reserved: in METADATA - Dependencies section]    │
# │ 7. Integration          │ .dockerignore and related file guidance           │
# │ 8. Known Issues         │ [Reserved: template - no known issues to document]│
# │ 9. Changelog            │ [Reserved: in METADATA - Version History]         │
# │10. See Also             │ [Reserved: in METADATA - references]              │
# │11. Modification Policy  │ What's safe to modify, what requires care         │
# │12. Closing Note         │ Final guidance and biblical connection            │
# │13. Attribution          │ [Reserved: in METADATA - Authorship section]      │
# └─────────────────────────────────────────────────────────────────────────────┘
#
# Section order follows EXECUTION → DOCUMENTATION flow:
#   Validation → [Reserved] → [Reserved] → Performance → Security →
#   [Reserved] → Integration → [Reserved] → [Reserved] → [Reserved] →
#   Modification Policy → Closing Note → [Reserved]
#
# Universal mapping (see standards for cross-language patterns):
#   Validation ≈ Validation (verify correctness)
#   Build Commands ≈ Execution [Reserved: in SETUP]
#   Cleanup ≈ Cleanup [Reserved: multi-stage handles this]
#   Performance ≈ Performance Considerations
#   Security ≈ Security Considerations
#   Dependencies ≈ Dependencies [Reserved: in METADATA]
#   Integration ≈ Integration Notes
#   Known Issues ≈ Known Issues [Reserved: template]
#   Changelog ≈ Changelog [Reserved: in METADATA]
#   See Also ≈ See Also [Reserved: in METADATA]
#   Modification Policy ≈ Modification Policy
#   Closing Note ≈ Closing Note
#   Attribution ≈ Attribution [Reserved: in METADATA]
#
# =============================================================================
# GROUP 1: VALIDATION
# =============================================================================
#
# -----------------------------------------------------------------------------
# 1. Validation
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
# 2. Build Commands (Execution)
# -----------------------------------------------------------------------------
#
# [Reserved: Build commands are documented in SETUP - Usage/Behavior section.
#  This avoids duplication and keeps execution information in one place.
#  See SETUP for: docker build, docker buildx, docker run commands.]
#
# -----------------------------------------------------------------------------
# 3. Cleanup
# -----------------------------------------------------------------------------
#
# [Reserved: Multi-stage builds handle cleanup automatically.
#  The builder stage (with all build tools, source, intermediate files) is
#  discarded entirely. Only the final runtime stage becomes the image.
#  No explicit cleanup needed - this is the power of multi-stage builds.]
#
# =============================================================================
# GROUP 2: FINAL DOCUMENTATION
# =============================================================================
#
# -----------------------------------------------------------------------------
# 4. Performance Considerations (Image Size Optimization)
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
# 5. Security Considerations
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
# 6. Dependencies
# -----------------------------------------------------------------------------
#
# [Reserved: Dependencies are documented in METADATA - Dependencies section.
#  This avoids duplication. See METADATA for: base images, source files,
#  what uses this Dockerfile, and integration points.]
#
# -----------------------------------------------------------------------------
# 7. Integration Notes (.dockerignore Companion)
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
# Related files that work with this Dockerfile:
#   - docker-compose.yaml: Service orchestration
#   - Makefile: Build automation (make docker-build, make docker-run)
#   - CI/CD config: Automated builds (.github/workflows/, .gitlab-ci.yml)
#
# -----------------------------------------------------------------------------
# 8. Known Issues
# -----------------------------------------------------------------------------
#
# [Reserved: This is a template file - no known issues to document.
#  When implementing for a specific project, document any known issues
#  with the build process, base image compatibility, or runtime behavior.]
#
# -----------------------------------------------------------------------------
# 9. Changelog
# -----------------------------------------------------------------------------
#
# [Reserved: Version history is documented in METADATA - Authorship section.
#  This avoids duplication and keeps version information in one place.]
#
# -----------------------------------------------------------------------------
# 10. See Also
# -----------------------------------------------------------------------------
#
# [Reserved: References are documented in METADATA header and SETUP.
#  See: standards/code/4-block/ for documentation standards
#  See: https://docs.docker.com/reference/dockerfile/ for Dockerfile reference]
#
# -----------------------------------------------------------------------------
# 11. Modification Policy
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
# 12. Closing Note
# -----------------------------------------------------------------------------
#
# This Dockerfile builds Go applications in reproducible containers,
# following Kingdom Technology principles of intentional design.
#
# Multi-stage builds embody the Kingdom principle of intentional layering:
# each stage serves a specific purpose, the builder provides what's needed
# for construction, and the runtime contains only what's needed for operation.
#
# "For which of you, intending to build a tower, sitteth not down first,
# and counteth the cost?" - Luke 14:28
#
# This template counts the cost: minimal image size, maximal security,
# reproducible builds that work the same everywhere.
#
# -----------------------------------------------------------------------------
# 13. Attribution
# -----------------------------------------------------------------------------
#
# [Reserved: Attribution is documented in METADATA - Authorship section.
#  This avoids duplication. See METADATA for: Architect, Implementation,
#  Created date, Version, and Version History.]
#
# -----------------------------------------------------------------------------
# CLOSING Omission Guide
# -----------------------------------------------------------------------------
#
# Unlike METADATA (where sections can be omitted with [OMIT: reason]),
# ALL thirteen CLOSING sections must be present for structural alignment.
#
# If a section has no content for this file:
#   - Keep the section header
#   - Add [Reserved: reason] comment explaining why empty
#   - This maintains the 13-section structure across all templates
#
# Go builder Dockerfile-specific guidance:
#
# GROUP 1: VALIDATION
#   1. Validation: Required - lint, build, test commands
#   2. Build Commands: [Reserved: in SETUP - Usage/Behavior section]
#   3. Cleanup: [Reserved: multi-stage build discards builder stage]
#
# GROUP 2: FINAL DOCUMENTATION
#   4. Performance: Required - image size and layer caching tips
#   5. Security: Required - container security considerations
#   6. Dependencies: [Reserved: in METADATA - Dependencies section]
#   7. Integration: Required - .dockerignore and related files
#   8. Known Issues: [Reserved: template - no known issues]
#   9. Changelog: [Reserved: in METADATA - Version History]
#  10. See Also: [Reserved: in METADATA - references]
#  11. Modification Policy: Required - safe/careful/never modify guidance
#  12. Closing Note: Required - final guidance and biblical connection
#  13. Attribution: [Reserved: in METADATA - Authorship section]
#
# The goal is structural consistency - every CONFIG template has the same
# 13-section CLOSING structure, making navigation and understanding predictable.
#
# =============================================================================
# END CLOSING
# =============================================================================
