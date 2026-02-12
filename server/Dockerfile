# ═══════════════════════════════════════════════════════════════════════════
# METADATA
# ═══════════════════════════════════════════════════════════════════════════
#
# Key: cws-server-dockerfile
# Purpose: Multi-stage Docker build for CWS Server (Fly.io standby)
# Biblical: Nehemiah 4:6 — "The people had a mind to work"
# Authors: Nova Dawn
# Version: 1.1.0
# Created: 2026-02-11
# Updated: 2026-02-12
#
# Build context: repo root (Bereshit/)
#   docker build -f server/Dockerfile -t cws-server .
#
# The server binary is statically linked (CGO_ENABLED=0). The runtime
# image includes Litestream for restoring SQLite databases from R2
# on startup (Oracle → R2 → Fly.io failover chain).
#
# Environment variables (set via fly secrets):
#   LITESTREAM_ACCESS_KEY_ID     — R2 API token access key
#   LITESTREAM_SECRET_ACCESS_KEY — R2 API token secret key
#
# ═══════════════════════════════════════════════════════════════════════════

# ── STAGE 1: Build ────────────────────────────────────────────────────────

FROM golang:1.24-alpine AS builder

WORKDIR /src

# Copy go.mod/go.sum first for dependency caching.
# Local replace modules need their go.mod too.
COPY server/go.mod server/go.sum ./server/
COPY word/claude/dashboard/go.mod word/claude/dashboard/go.sum* ./word/claude/dashboard/
COPY word/claude/pkg/go.mod word/claude/pkg/go.sum* ./word/claude/pkg/

WORKDIR /src/server
RUN go mod download

# Copy full source.
WORKDIR /src
COPY server/ ./server/
COPY word/claude/dashboard/ ./word/claude/dashboard/
COPY word/claude/pkg/ ./word/claude/pkg/

# Build statically linked binary.
WORKDIR /src/server/cmd/cws-server
RUN CGO_ENABLED=0 GOOS=linux go build -ldflags="-s -w" -o /cws-server .

# ── STAGE 2: Litestream ─────────────────────────────────────────────────────
# Grab Litestream binary for SQLite restore-from-R2 on startup.

FROM litestream/litestream:0.3.13 AS litestream

# ── STAGE 3: Runtime ──────────────────────────────────────────────────────

FROM alpine:3.21

RUN apk add --no-cache ca-certificates tzdata bash

# Non-root user.
RUN adduser -D -h /home/cws cws

# Binaries.
COPY --from=builder /cws-server /usr/local/bin/cws-server
COPY --from=litestream /usr/local/bin/litestream /usr/local/bin/litestream

# Litestream config for restore.
COPY server/litestream-fly.yml /etc/litestream/litestream.yml

# Startup script: restore DBs from R2, then start server.
COPY --chmod=755 server/run.sh /usr/local/bin/run.sh

# Website static files.
COPY --chown=cws:cws server/website/ ./website/

# Persistent data lives on a Fly volume mounted at /home/cws/data.
USER cws
WORKDIR /home/cws
RUN mkdir -p /home/cws/data /home/cws/data/databases

EXPOSE 3847

HEALTHCHECK --interval=30s --timeout=5s --retries=3 \
    CMD wget -qO- http://localhost:3847/healthz || exit 1

ENTRYPOINT ["/usr/local/bin/run.sh"]
