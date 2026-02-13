// ═══════════════════════════════════════════════════════════════════════════
// METADATA
// ═══════════════════════════════════════════════════════════════════════════
//
// Key: cws-ops-backups
// Purpose: Litestream + Restic backup status and restore operations
// Biblical: Ruth 4:7 — "to confirm all things"
// Authors: Nova Dawn
// Version: 1.0.0
// Created: 2026-02-12
//
// ═══════════════════════════════════════════════════════════════════════════

package ops

// ═══════════════════════════════════════════════════════════════════════════
// SETUP
// ═══════════════════════════════════════════════════════════════════════════

import (
	"context"
	"fmt"
	"strings"

	"cws.studio/server/internal/config"
)

// ═══════════════════════════════════════════════════════════════════════════
// BODY
// ═══════════════════════════════════════════════════════════════════════════

// BackupDBInfo represents a Litestream-replicated database.
type BackupDBInfo struct {
	Name       string `json:"name"`
	Path       string `json:"path"`
	SizeBytes  int64  `json:"sizeBytes"`
	SizeHuman  string `json:"sizeHuman"`
	Replicating bool  `json:"replicating"`
}

// BackupStatus holds the overall backup system status.
type BackupStatus struct {
	LitestreamRunning bool           `json:"litestreamRunning"`
	Databases         []BackupDBInfo `json:"databases"`
	Errors            []string       `json:"errors,omitempty"`
}

// GetBackupStatus checks Litestream service and lists replicated databases.
func GetBackupStatus(ctx context.Context, host config.Host) (*BackupStatus, error) {
	status := &BackupStatus{}

	// Check if litestream is running
	result, err := RunSSH(ctx, host, "systemctl is-active litestream.service")
	if err != nil {
		return nil, err
	}
	status.LitestreamRunning = result.OK() && strings.TrimSpace(result.Stdout) == "active"

	// List databases with sizes
	output, err := RunSSHCapture(ctx, host,
		`ls -lh /home/seanje/cws/data/*.db 2>/dev/null | awk '{print $5, $NF}'`)
	if err != nil {
		status.Errors = append(status.Errors, fmt.Sprintf("list databases: %v", err))
		return status, nil
	}

	for line := range strings.SplitSeq(strings.TrimSpace(output), "\n") {
		if line == "" {
			continue
		}
		parts := strings.Fields(line)
		if len(parts) < 2 {
			continue
		}

		name := parts[1]
		// Extract just the filename
		if idx := strings.LastIndex(name, "/"); idx >= 0 {
			name = name[idx+1:]
		}

		status.Databases = append(status.Databases, BackupDBInfo{
			Name:        strings.TrimSuffix(name, ".db"),
			Path:        parts[1],
			SizeHuman:   parts[0],
			Replicating: status.LitestreamRunning,
		})
	}

	return status, nil
}

// RestoreDatabase restores a database from R2 via Litestream.
// This is a destructive operation — callers must confirm first.
func RestoreDatabase(ctx context.Context, host config.Host, dbName, destPath string) (ExecResult, error) {
	if destPath == "" {
		destPath = fmt.Sprintf("/tmp/%s-restored.db", dbName)
	}

	// Load environment and run restore
	cmd := fmt.Sprintf(
		`export $(grep -v "^#" ~/.config/litestream/env | xargs) && `+
			`litestream restore -config /etc/litestream.yml -o %s s3://cws-backups/litestream/%s.db`,
		destPath, dbName,
	)

	return RunSSH(ctx, host, cmd)
}

// VerifyBackups runs a quick integrity check on all replicated databases.
func VerifyBackups(ctx context.Context, host config.Host) (string, error) {
	return RunSSHCapture(ctx, host,
		`for db in /home/seanje/cws/data/*.db; do
			name=$(basename "$db")
			result=$(sqlite3 "$db" "PRAGMA integrity_check" 2>&1)
			if [ "$result" = "ok" ]; then
				echo "  $name: ok"
			else
				echo "  $name: FAILED ($result)"
			fi
		done`)
}

// ═══════════════════════════════════════════════════════════════════════════
// CLOSING
// ═══════════════════════════════════════════════════════════════════════════
