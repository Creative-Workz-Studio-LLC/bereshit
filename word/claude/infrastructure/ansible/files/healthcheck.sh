#!/bin/bash
# ── CWS Server Health Watchdog ──────────────────────────────────────
# Checks server health, restarts if unresponsive, monitors cloudflared.
# Run via cron: */5 * * * * /home/seanje/cws/scripts/healthcheck.sh
# ────────────────────────────────────────────────────────────────────

set -euo pipefail

# ── Config ──────────────────────────────────────────────────────────

HEALTH_URL="http://localhost:3847/healthz"
TIMEOUT=5
MAX_LOG_SIZE=$((10 * 1024 * 1024))  # 10 MB
LOG_DIR="/home/seanje/cws/logs"
LOG_FILE="${LOG_DIR}/health.log"
SERVICES=("cws-server" "cloudflared")

# ── Setup ───────────────────────────────────────────────────────────

mkdir -p "${LOG_DIR}"

log() {
    echo "[$(date '+%Y-%m-%d %H:%M:%S')] $1" >> "${LOG_FILE}"
}

# ── Log Rotation ────────────────────────────────────────────────────

if [ -f "${LOG_FILE}" ]; then
    size=$(stat -c%s "${LOG_FILE}" 2>/dev/null || echo 0)
    if [ "${size}" -gt "${MAX_LOG_SIZE}" ]; then
        mv "${LOG_FILE}" "${LOG_FILE}.old"
        log "Log rotated (previous: ${size} bytes)"
    fi
fi

# ── Health Check ────────────────────────────────────────────────────

if curl -sf --max-time ${TIMEOUT} "${HEALTH_URL}" > /dev/null 2>&1; then
    # Server healthy — quiet success (only log failures/restarts)
    :
else
    log "WARN: Health check failed — ${HEALTH_URL} unresponsive"

    # Check if service is running at all
    if systemctl is-active --quiet cws-server; then
        log "WARN: Service is active but not responding — restarting"
        sudo systemctl restart cws-server
        sleep 3

        # Verify restart worked
        if curl -sf --max-time ${TIMEOUT} "${HEALTH_URL}" > /dev/null 2>&1; then
            log "OK: Service restarted successfully — health check passed"
        else
            log "ERROR: Service restart did not resolve health check failure"
        fi
    else
        log "WARN: Service is not active — starting"
        sudo systemctl start cws-server
        sleep 3

        if curl -sf --max-time ${TIMEOUT} "${HEALTH_URL}" > /dev/null 2>&1; then
            log "OK: Service started successfully — health check passed"
        else
            log "ERROR: Service start did not resolve health check failure"
        fi
    fi
fi

# ── Service Status Checks ──────────────────────────────────────────

for svc in "${SERVICES[@]}"; do
    if ! systemctl is-active --quiet "${svc}"; then
        log "WARN: ${svc} is not active — attempting restart"
        sudo systemctl restart "${svc}"
        sleep 2
        if systemctl is-active --quiet "${svc}"; then
            log "OK: ${svc} restarted successfully"
        else
            log "ERROR: ${svc} failed to restart"
        fi
    fi
done
