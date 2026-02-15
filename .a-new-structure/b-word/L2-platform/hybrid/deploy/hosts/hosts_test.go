//omni:code --go -test
//omni:key B-L2-hosts-test
//omni:version a-01.00

// ═══════════════════════════════════════════════════════════════════════════
// METADATA
// ═══════════════════════════════════════════════════════════════════════════
//
// Key: B-L2-hosts-test
// Purpose: Unit tests for production host definitions
// Biblical: Proverbs 14:15 — "The prudent man looketh well to his going"
// Authors: Nova Dawn
// Version: 1.0.0
// Created: 2026-02-15
//
// ═══════════════════════════════════════════════════════════════════════════

package hosts

// ═══════════════════════════════════════════════════════════════════════════
// SETUP
// ═══════════════════════════════════════════════════════════════════════════

import "testing"

// ═══════════════════════════════════════════════════════════════════════════
// BODY
// ═══════════════════════════════════════════════════════════════════════════

// ───────────────────────────────────────────────────────────────────────────
// Host Map — verify all expected hosts exist with correct data
// ───────────────────────────────────────────────────────────────────────────

func TestAllContainsOracleAndDell(t *testing.T) {
	if len(All()) != 2 {
		t.Fatalf("expected 2 hosts, got %d", len(All()))
	}

	if _, ok := All()["oracle"]; !ok {
		t.Error("missing host: oracle")
	}
	if _, ok := All()["dell"]; !ok {
		t.Error("missing host: dell")
	}
}

func TestOracleHostDetails(t *testing.T) {
	h := All()["oracle"]

	if h.Name != "oracle" {
		t.Errorf("oracle.Name = %q, want %q", h.Name, "oracle")
	}
	if h.User != "seanje" {
		t.Errorf("oracle.User = %q, want %q", h.User, "seanje")
	}
	if h.Address != "163.192.118.124" {
		t.Errorf("oracle.Address = %q, want %q", h.Address, "163.192.118.124")
	}
	if h.Arch != "arm64" {
		t.Errorf("oracle.Arch = %q, want %q", h.Arch, "arm64")
	}
	if !h.Primary {
		t.Error("oracle should be primary")
	}
}

func TestDellHostDetails(t *testing.T) {
	h := All()["dell"]

	if h.Name != "dell" {
		t.Errorf("dell.Name = %q, want %q", h.Name, "dell")
	}
	if h.Arch != "amd64" {
		t.Errorf("dell.Arch = %q, want %q", h.Arch, "amd64")
	}
	if h.Primary {
		t.Error("dell should not be primary")
	}
}

// ───────────────────────────────────────────────────────────────────────────
// DefaultHost — verify primary target is oracle
// ───────────────────────────────────────────────────────────────────────────

func TestDefaultHostIsOracle(t *testing.T) {
	h := DefaultHost()

	if h.Name != "oracle" {
		t.Errorf("DefaultHost().Name = %q, want %q", h.Name, "oracle")
	}
	if !h.Primary {
		t.Error("default host should be primary")
	}
}

// ───────────────────────────────────────────────────────────────────────────
// LookupHost — found and not-found paths
// ───────────────────────────────────────────────────────────────────────────

func TestLookupHostFound(t *testing.T) {
	h, ok := LookupHost("oracle")
	if !ok {
		t.Fatal("LookupHost(oracle) returned false")
	}
	if h.Name != "oracle" {
		t.Errorf("got name %q, want %q", h.Name, "oracle")
	}
}

func TestLookupHostNotFound(t *testing.T) {
	_, ok := LookupHost("nonexistent")
	if ok {
		t.Error("LookupHost(nonexistent) should return false")
	}
}

// ───────────────────────────────────────────────────────────────────────────
// SSHTarget — verify format
// ───────────────────────────────────────────────────────────────────────────

func TestSSHTargetFormat(t *testing.T) {
	h := All()["oracle"]
	target := h.SSHTarget()
	expected := "seanje@163.192.118.124"

	if target != expected {
		t.Errorf("SSHTarget() = %q, want %q", target, expected)
	}
}

// ───────────────────────────────────────────────────────────────────────────
// Constants — verify production paths
// ───────────────────────────────────────────────────────────────────────────

func TestProdPaths(t *testing.T) {
	if ProdDir() != "/home/seanje/cws" {
		t.Errorf("ProdDir() = %q, want /home/seanje/cws", ProdDir())
	}
	if ProdBin() != "/usr/local/bin/cws-server" {
		t.Errorf("ProdBin() = %q, want /usr/local/bin/cws-server", ProdBin())
	}
}

// ═══════════════════════════════════════════════════════════════════════════
// CLOSING
// ═══════════════════════════════════════════════════════════════════════════
