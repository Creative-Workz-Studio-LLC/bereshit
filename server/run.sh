#!/bin/bash
# ── CWS Server Startup (Fly.io) ────────────────────────────────────
# Restores SQLite databases from R2 via Litestream, then starts
# the CWS server. Used as Docker ENTRYPOINT on Fly.io standby.
#
# On first boot (empty volume): restore creates the databases.
# On subsequent boots (volume has data): restore is a no-op (fast).
#
# Litestream replicate wraps the server process — if Oracle is down
# and Fly.io becomes primary, writes are captured and replicated
# back to R2 so recovery doesn't lose data.
# ────────────────────────────────────────────────────────────────────

set -e

DATA_DIR="/home/cws/data"
DB_DIR="${DATA_DIR}/databases"

# Ensure directories exist (volume may be fresh).
mkdir -p "${DB_DIR}"

echo "==> Restoring databases from R2..."

# Restore each database. -if-db-not-exists skips restore when the
# database already exists on the volume (normal restart, not failover).
# -if-replica-exists prevents errors when no replica exists yet.

DATABASES=(
  "${DATA_DIR}/cpisi.db"
  "${DB_DIR}/sessions.db"
  "${DB_DIR}/cognition.db"
  "${DB_DIR}/growth.db"
  "${DB_DIR}/temporal.db"
  "${DB_DIR}/projects.db"
)

for db in "${DATABASES[@]}"; do
  name=$(basename "$db" .db)
  if [ -f "$db" ]; then
    echo "    ${name}: exists on volume, skipping restore"
  else
    echo "    ${name}: restoring from R2..."
    litestream restore -config /etc/litestream/litestream.yml -if-replica-exists "$db" 2>&1 || {
      echo "    ${name}: no replica found (new database), skipping"
    }
  fi
done

echo "==> Restore complete"
echo "==> Starting CWS Server..."

# Run server under Litestream replicate — captures any writes if
# Fly.io becomes the active primary during failover.
exec litestream replicate -exec "cws-server --port 3847 --website-dir /home/cws/website --data-dir /home/cws/data" -config /etc/litestream/litestream.yml
