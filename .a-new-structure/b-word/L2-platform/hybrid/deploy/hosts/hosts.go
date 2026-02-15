//omni:code --go -library
//omni:key B-L2-hosts
//omni:version a-01.00

// ═══════════════════════════════════════════════════════════════════════════
// METADATA
// ═══════════════════════════════════════════════════════════════════════════
//
// Key: B-L2-hosts
// Purpose: Production host definitions — shared between server and ops tool
// Biblical: Psalm 127:1 — "Except the LORD build the house, they labour in vain"
// Authors: Nova Dawn
// Version: 1.0.0
// Created: 2026-02-12
//
// ═══════════════════════════════════════════════════════════════════════════

package hosts

// ═══════════════════════════════════════════════════════════════════════════
// SETUP
// ═══════════════════════════════════════════════════════════════════════════

import (
	"creativeworkzstudio.com/bereshit/L2-platform/hybrid/registry"
)

// ────────────────────────────────────────────────────────────────────────────
// S.1 Config — database-backed host definitions with hardcoded fallback
// ────────────────────────────────────────────────────────────────────────────

// Host represents a deployment target with SSH connection details and
// architecture information for cross-compilation.
type Host struct {
	Name    string // Human-readable name (oracle, dell)
	User    string // SSH username
	Address string // IP address or hostname
	Arch    string // GOARCH value (arm64, amd64)
	Primary bool   // Whether this is the primary production target
}

// Config holds deployment host definitions and production paths.
type Config struct {
	Hosts   map[string]Host
	ProdDir string
	ProdBin string
}

// DefaultConfig returns hardcoded host definitions matching hosts.toml.
func DefaultConfig() Config {
	return Config{
		Hosts: map[string]Host{
			"oracle": {Name: "oracle", User: "seanje", Address: "163.192.118.124", Arch: "arm64", Primary: true},
			"dell":   {Name: "dell", User: "seanje", Address: "192.168.1.99", Arch: "amd64", Primary: false},
		},
		ProdDir: "/home/seanje/cws",
		ProdBin: "/usr/local/bin/cws-server",
	}
}

// package-level config
var cfg = DefaultConfig()

// Init sets the package configuration. Pass nil to keep defaults.
func Init(c *Config) {
	if c != nil {
		cfg = *c
	}
}

// ═══════════════════════════════════════════════════════════════════════════
// BODY
// ═══════════════════════════════════════════════════════════════════════════

// ProdDir returns the remote directory where CWS is deployed.
func ProdDir() string { return cfg.ProdDir }

// ProdBin returns the remote path to the cws-server binary.
func ProdBin() string { return cfg.ProdBin }

// All returns all known deployment hosts.
func All() map[string]Host { return cfg.Hosts }

// DefaultHost returns the primary deployment target.
func DefaultHost() Host {
	for _, h := range cfg.Hosts {
		if h.Primary {
			return h
		}
	}
	// Fallback — first host if no primary
	for _, h := range cfg.Hosts {
		return h
	}
	return Host{}
}

// LookupHost returns a host by name or false if not found.
func LookupHost(name string) (Host, bool) {
	h, ok := cfg.Hosts[name]
	return h, ok
}

// SSHTarget returns the user@address string for SSH commands.
func (h Host) SSHTarget() string {
	return h.User + "@" + h.Address
}

// ────────────────────────────────────────────────────────────────────────────
// B.3 Registry Integration
// ────────────────────────────────────────────────────────────────────────────

// ConfigFromRegistry builds a Config from the L2 platform database
// via the cross-layer registry. Host definitions come from deployment_hosts.
func ConfigFromRegistry(r *registry.Registry) Config {
	c := DefaultConfig()

	if dbHosts, err := r.Platform.AllHosts(); err == nil && len(dbHosts) > 0 {
		hosts := make(map[string]Host, len(dbHosts))
		var prodDir, prodBin string
		for _, h := range dbHosts {
			hosts[h.Name] = Host{
				Name:    h.Name,
				User:    h.Username,
				Address: h.Address,
				Arch:    h.Arch,
				Primary: h.IsPrimary,
			}
			// Use prod paths from the first host that has them
			if prodDir == "" && h.ProdDir != "" {
				prodDir = h.ProdDir
			}
			if prodBin == "" && h.ProdBin != "" {
				prodBin = h.ProdBin
			}
		}
		c.Hosts = hosts
		if prodDir != "" {
			c.ProdDir = prodDir
		}
		if prodBin != "" {
			c.ProdBin = prodBin
		}
	}

	return c
}

// ═══════════════════════════════════════════════════════════════════════════
// CLOSING
// ═══════════════════════════════════════════════════════════════════════════
