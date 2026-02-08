// ═══════════════════════════════════════════════════════════════════════════
// METADATA
// ═══════════════════════════════════════════════════════════════════════════
//
// Key: cws-server-config-server
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
package config

// ═══════════════════════════════════════════════════════════════════════════
// SETUP
// ═══════════════════════════════════════════════════════════════════════════

import (
	"os"
	"strconv"
)

// ═══════════════════════════════════════════════════════════════════════════
// BODY
// ═══════════════════════════════════════════════════════════════════════════

// ServerConfig holds the resolved server configuration.
// All fields can be set via flags (see main.go) or environment variables.
type ServerConfig struct {
	Port        int    // HTTP listen port (default: 3847, env: CWS_SERVER_PORT)
	BuilderDir  string // Path to company-docs/ project root (env: CWS_BUILDER_DIR)
	EnableCPISI bool   // Enable CPI-SI state machine + journal service
	DevMode     bool   // Development mode: debug logging, filesystem serving
}

// DefaultConfig returns a ServerConfig with sensible defaults, merged with
// environment variables. Port 3847 was chosen to avoid conflicts with common
// dev ports (3000, 5173, 8080). CPI-SI is enabled by default; builder service
// requires a valid project directory to activate.
func DefaultConfig() *ServerConfig {
	port := 3847
	if envPort := os.Getenv("CWS_SERVER_PORT"); envPort != "" {
		if p, err := strconv.Atoi(envPort); err == nil {
			port = p
		}
	}

	return &ServerConfig{
		Port:        port,
		BuilderDir:  os.Getenv("CWS_BUILDER_DIR"),
		EnableCPISI: true,
		DevMode:     false,
	}
}

// ═══════════════════════════════════════════════════════════════════════════
// CLOSING
// ═══════════════════════════════════════════════════════════════════════════
