// ═══════════════════════════════════════════════════════════════════════════
// METADATA
// ═══════════════════════════════════════════════════════════════════════════
//
// Key: cws-ops-services
// Purpose: systemd service management — start, stop, restart
// Biblical: Ecclesiastes 3:1 — "To every thing there is a season"
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
	"slices"

	"cws.studio/server/internal/config"
)

// ═══════════════════════════════════════════════════════════════════════════
// BODY
// ═══════════════════════════════════════════════════════════════════════════

// ServiceAction represents a systemd action on a service.
type ServiceAction string

const (
	ServiceRestart ServiceAction = "restart"
	ServiceStop    ServiceAction = "stop"
	ServiceStart   ServiceAction = "start"
)

// ManagedServiceNames returns the list of services we track.
func ManagedServiceNames() []string {
	return append([]string{}, managedServices...)
}

// IsManaged checks if a service name is in our managed list.
func IsManaged(name string) bool {
	return slices.Contains(managedServices, name)
}

// ManageService performs a systemd action (start/stop/restart) on a named
// service via SSH. Returns the ExecResult for caller inspection.
func ManageService(ctx context.Context, host config.Host, name string, action ServiceAction) (ExecResult, error) {
	if !IsManaged(name) {
		return ExecResult{}, fmt.Errorf("unknown service: %s", name)
	}

	cmd := fmt.Sprintf("sudo systemctl %s %s.service", action, name)
	return RunSSH(ctx, host, cmd)
}

// GetServiceStatus returns the status of a single named service.
func GetServiceStatus(ctx context.Context, host config.Host, name string) (*ServiceStatus, error) {
	if !IsManaged(name) {
		return nil, fmt.Errorf("unknown service: %s", name)
	}

	output, err := RunSSHCapture(ctx, host, fmt.Sprintf(
		"systemctl show %s.service --property=Id,ActiveState,SubState,MainPID,MemoryCurrent,UnitFileState,ActiveEnterTimestamp --no-pager",
		name,
	))
	if err != nil {
		return nil, err
	}

	statuses := parseServiceStatuses(output)
	if len(statuses) == 0 {
		return nil, fmt.Errorf("no status returned for %s", name)
	}

	return &statuses[0], nil
}

// ═══════════════════════════════════════════════════════════════════════════
// CLOSING
// ═══════════════════════════════════════════════════════════════════════════
