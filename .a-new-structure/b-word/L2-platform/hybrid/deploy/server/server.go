//omni:code --go -library
//omni:key B-L2-server-config-server
//omni:version a-01.00

// ═══════════════════════════════════════════════════════════════════════════
// METADATA
// ═══════════════════════════════════════════════════════════════════════════
//
// Key: B-L2-server-config-server
// Purpose: Server configuration — flags, environment, feature toggles
// Biblical: Proverbs 24:27 — "Prepare thy work without, and make it fit"
// Authors: Nova Dawn
// Version: 1.1.0
// Created: 2026-02-08
//
// ═══════════════════════════════════════════════════════════════════════════

// Package config provides server configuration, path resolution, and user
// preferences for the CWS server. Configuration flows through three layers:
//
//  1. Defaults — hardcoded sensible values (port 3847, CPI-SI enabled)
//  2. Environment — CWS_SERVER_PORT, CWS_BUILDER_DIR override defaults
//  3. Flags — command-line flags override everything
//
// This layered approach means the server runs correctly with zero configuration
// in development, while production can tune via environment or flags.
package server

// ═══════════════════════════════════════════════════════════════════════════
// SETUP
// ═══════════════════════════════════════════════════════════════════════════

import (
	"os"
	"strconv"

	"creativeworkzstudio.com/bereshit/L2-platform/hybrid/registry"
)

// ────────────────────────────────────────────────────────────────────────────
// S.1 Config — database-backed server settings with hardcoded fallback
// ────────────────────────────────────────────────────────────────────────────

// Defaults holds database-sourced default values. These are the "first layer"
// that environment variables and flags can override.
type Defaults struct {
	Port       int    // default port (from server_config or 3847)
	EnvPort    string // env var name for port override
	EnvBuilder string // env var name for builder dir override
}

// DefaultDefaults returns hardcoded server defaults matching hosts.toml server section.
func DefaultDefaults() Defaults {
	return Defaults{
		Port:       3847,
		EnvPort:    "CWS_SERVER_PORT",
		EnvBuilder: "CWS_BUILDER_DIR",
	}
}

// package-level defaults
var defaults = DefaultDefaults()

// InitDefaults sets the package defaults from database. Pass nil to keep hardcoded.
func InitDefaults(d *Defaults) {
	if d != nil {
		defaults = *d
	}
}

// ═══════════════════════════════════════════════════════════════════════════
// BODY
// ═══════════════════════════════════════════════════════════════════════════

// ServerConfig holds the resolved server configuration.
// All fields can be set via flags (see main.go) or environment variables.
type ServerConfig struct {
	Port        int    // HTTP listen port (default: from DB or 3847, env: CWS_SERVER_PORT)
	BuilderDir  string // Path to company-docs/ project root (env: CWS_BUILDER_DIR)
	EnableCPISI bool   // Enable CPI-SI state machine + journal service
	DevMode     bool   // Development mode: debug logging, filesystem serving
}

// DefaultConfig returns a ServerConfig with sensible defaults, merged with
// environment variables. Port 3847 was chosen to avoid conflicts with common
// dev ports (3000, 5173, 8080). CPI-SI is enabled by default; builder service
// requires a valid project directory to activate.
func DefaultConfig() *ServerConfig {
	port := defaults.Port
	if envPort := os.Getenv(defaults.EnvPort); envPort != "" {
		if p, err := strconv.Atoi(envPort); err == nil {
			port = p
		}
	}

	return &ServerConfig{
		Port:        port,
		BuilderDir:  os.Getenv(defaults.EnvBuilder),
		EnableCPISI: true,
		DevMode:     false,
	}
}

// ────────────────────────────────────────────────────────────────────────────
// B.3 Registry Integration
// ────────────────────────────────────────────────────────────────────────────

// DefaultsFromRegistry builds a Defaults struct from the L2 platform database
// via the cross-layer registry. Server config keys come from server_config.
func DefaultsFromRegistry(r *registry.Registry) Defaults {
	d := DefaultDefaults()

	if portCfg, err := r.Platform.ServerConfigByKey("port"); err == nil {
		if p, err := strconv.Atoi(portCfg.Value); err == nil {
			d.Port = p
		}
	}

	if envPort, err := r.Platform.ServerConfigByKey("env_port"); err == nil && envPort.Value != "" {
		d.EnvPort = envPort.Value
	}

	if envBuilder, err := r.Platform.ServerConfigByKey("env_builder"); err == nil && envBuilder.Value != "" {
		d.EnvBuilder = envBuilder.Value
	}

	return d
}

// ═══════════════════════════════════════════════════════════════════════════
// CLOSING
// ═══════════════════════════════════════════════════════════════════════════
