// ═══════════════════════════════════════════════════════════════════════════
// METADATA
// ═══════════════════════════════════════════════════════════════════════════
//
// Key: cws-ops-deploy
// Purpose: Deploy operations — wraps battle-tested Makefile targets
// Biblical: Nehemiah 6:15 — "So the wall was finished"
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
	"io"
	"os"
	"os/exec"
	"path/filepath"

	"cws.studio/server/internal/config"
)

// ═══════════════════════════════════════════════════════════════════════════
// BODY
// ═══════════════════════════════════════════════════════════════════════════

// DeployTarget identifies what to deploy. Maps 1:1 to Makefile targets.
type DeployTarget string

const (
	DeployAll        DeployTarget = "all"
	DeployBinary     DeployTarget = "binary"
	DeployDash       DeployTarget = "dash"
	DeploySite       DeployTarget = "site"
	DeployBuilder    DeployTarget = "builder"
	DeployFly        DeployTarget = "fly"
	DeployMonitoring DeployTarget = "monitoring"
)

// ValidDeployTargets lists all valid deploy targets for validation.
var ValidDeployTargets = []DeployTarget{
	DeployAll, DeployBinary, DeployDash, DeploySite,
	DeployBuilder, DeployFly, DeployMonitoring,
}

// makeTarget maps DeployTarget to the Makefile target name.
var makeTarget = map[DeployTarget]string{
	DeployAll:        "deploy",
	DeployBinary:     "deploy-binary",
	DeployDash:       "deploy-dash",
	DeploySite:       "deploy-site",
	DeployBuilder:    "deploy-builder",
	DeployFly:        "deploy-fly",
	DeployMonitoring: "deploy-monitoring",
}

// DeployDescription returns a human-readable description of what the
// deploy target does. Used in confirmation prompts.
var DeployDescription = map[DeployTarget]string{
	DeployAll:        "Full production deploy (binary + frontend + config + restart)",
	DeployBinary:     "Go binary only (builds, uploads, restarts service)",
	DeployDash:       "Dashboard frontend + config + themes (no restart)",
	DeploySite:       "Website to Oracle + Cloudflare Pages",
	DeployBuilder:    "Builder CLI dist + deps + Makefile",
	DeployFly:        "Deploy to Fly.io standby",
	DeployMonitoring: "Prometheus + Grafana configs",
}

// Deploy runs a Makefile deploy target, streaming output to the writer.
// The PROD_TARGET variable is set based on the host so the Makefile
// cross-compiles for the correct architecture.
func Deploy(ctx context.Context, host config.Host, target DeployTarget, w io.Writer) error {
	mk, ok := makeTarget[target]
	if !ok {
		return fmt.Errorf("unknown deploy target: %s", target)
	}

	serverDir, err := findServerDir()
	if err != nil {
		return fmt.Errorf("cannot find server directory: %w", err)
	}

	cmd := exec.CommandContext(ctx, "make",
		"-C", serverDir,
		fmt.Sprintf("PROD_TARGET=%s", host.Name),
		mk,
	)

	cmd.Stdout = w
	cmd.Stderr = w
	cmd.Env = append(os.Environ(), fmt.Sprintf("PROD_TARGET=%s", host.Name))

	return cmd.Run()
}

// DryRunDeploy shows what a deploy would do without executing.
func DryRunDeploy(host config.Host, target DeployTarget, w io.Writer) {
	mk := makeTarget[target]
	desc := DeployDescription[target]

	fmt.Fprintf(w, "Dry run: deploy %s to %s\n", target, host.Name)
	fmt.Fprintf(w, "  Target: make -C server/ PROD_TARGET=%s %s\n", host.Name, mk)
	fmt.Fprintf(w, "  Action: %s\n", desc)
	fmt.Fprintf(w, "  Host:   %s (%s, %s)\n", host.Name, host.Address, host.Arch)
}

// IsValidDeployTarget checks if a string is a valid deploy target.
func IsValidDeployTarget(s string) bool {
	for _, t := range ValidDeployTargets {
		if string(t) == s {
			return true
		}
	}
	return false
}

// findServerDir locates the server/ directory by walking up from the
// executable path or CWD.
func findServerDir() (string, error) {
	// Try executable location first
	exe, err := os.Executable()
	if err == nil {
		dir := filepath.Dir(exe)
		// Walk up looking for Makefile with our targets
		for range 5 {
			if _, err := os.Stat(filepath.Join(dir, "Makefile")); err == nil {
				if _, err := os.Stat(filepath.Join(dir, "cmd", "cws-server")); err == nil {
					return dir, nil
				}
			}
			dir = filepath.Dir(dir)
		}
	}

	// Try CWD
	cwd, err := os.Getwd()
	if err != nil {
		return "", err
	}

	dir := cwd
	for range 10 {
		if _, err := os.Stat(filepath.Join(dir, "Makefile")); err == nil {
			if _, err := os.Stat(filepath.Join(dir, "cmd", "cws-server")); err == nil {
				return dir, nil
			}
		}
		parent := filepath.Dir(dir)
		if parent == dir {
			break
		}
		dir = parent
	}

	// Hardcoded fallback — this is a company workstation
	fallback := "/media/seanje-lenox-wise/Project/Bereshit/server"
	if _, err := os.Stat(filepath.Join(fallback, "Makefile")); err == nil {
		return fallback, nil
	}

	return "", fmt.Errorf("server directory not found (tried exe path, cwd, fallback)")
}

// ═══════════════════════════════════════════════════════════════════════════
// CLOSING
// ═══════════════════════════════════════════════════════════════════════════
