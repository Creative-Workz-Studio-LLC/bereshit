// ═══════════════════════════════════════════════════════════════════════════
// METADATA
// ═══════════════════════════════════════════════════════════════════════════
//
// Key: cws-server-config-server
// Purpose: Server configuration — flags, environment, feature toggles
// Authors: Nova Dawn
// Version: 1.0.0
// Created: 2026-02-08
//
// ═══════════════════════════════════════════════════════════════════════════

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
type ServerConfig struct {
	Port       int
	BuilderDir string // Path to company-docs/ (builder dashboard project dir)
	EnableCPISI bool  // Enable CPI-SI service
	DevMode    bool   // Serve frontends from filesystem (vs embedded)
}

// DefaultConfig returns a ServerConfig with sensible defaults, merged with environment.
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
