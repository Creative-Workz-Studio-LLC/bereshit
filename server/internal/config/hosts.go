// ═══════════════════════════════════════════════════════════════════════════
// METADATA
// ═══════════════════════════════════════════════════════════════════════════
//
// Key: cws-config-hosts
// Purpose: Production host definitions — shared between server and ops tool
// Biblical: Psalm 127:1 — "Except the LORD build the house, they labour in vain"
// Authors: Nova Dawn
// Version: 1.0.0
// Created: 2026-02-12
//
// ═══════════════════════════════════════════════════════════════════════════

package config

// ═══════════════════════════════════════════════════════════════════════════
// SETUP
// ═══════════════════════════════════════════════════════════════════════════

// ═══════════════════════════════════════════════════════════════════════════
// BODY
// ═══════════════════════════════════════════════════════════════════════════

// Host represents a deployment target with SSH connection details and
// architecture information for cross-compilation.
type Host struct {
	Name    string // Human-readable name (oracle, dell)
	User    string // SSH username
	Address string // IP address or hostname
	Arch    string // GOARCH value (arm64, amd64)
	Primary bool   // Whether this is the primary production target
}

// ProdDir is the remote directory where CWS is deployed on all hosts.
const ProdDir = "/home/seanje/cws"

// ProdBin is the remote path to the cws-server binary.
const ProdBin = "/usr/local/bin/cws-server"

// Hosts defines all known deployment targets. The map key matches the
// PROD_TARGET variable in the Makefile so either system can reference
// hosts consistently.
var Hosts = map[string]Host{
	"oracle": {
		Name:    "oracle",
		User:    "seanje",
		Address: "163.192.118.124",
		Arch:    "arm64",
		Primary: true,
	},
	"dell": {
		Name:    "dell",
		User:    "seanje",
		Address: "192.168.1.99",
		Arch:    "amd64",
		Primary: false,
	},
}

// DefaultHost returns the primary deployment target.
func DefaultHost() Host {
	return Hosts["oracle"]
}

// LookupHost returns a host by name or false if not found.
func LookupHost(name string) (Host, bool) {
	h, ok := Hosts[name]
	return h, ok
}

// SSHTarget returns the user@address string for SSH commands.
func (h Host) SSHTarget() string {
	return h.User + "@" + h.Address
}

// ═══════════════════════════════════════════════════════════════════════════
// CLOSING
// ═══════════════════════════════════════════════════════════════════════════
