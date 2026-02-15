//omni:code --rust -library
//omni:key B-L2-hosts
//omni:version a-01.00

// ═══════════════════════════════════════════════════════════════════════════
// METADATA
// ═══════════════════════════════════════════════════════════════════════════
//
// Key: B-L2-hosts
// Purpose: Production host definitions — SSH targets, architecture info
// Biblical: Psalm 127:1 — "Except the LORD build the house, they labour in vain"
// Authors: Nova Dawn
// Version: 0.1.0
// Created: 2026-02-15
//
// ═══════════════════════════════════════════════════════════════════════════

// ═══════════════════════════════════════════════════════════════════════════
// SETUP
// ═══════════════════════════════════════════════════════════════════════════

use serde::{Deserialize, Serialize};
use std::collections::HashMap;

// ────────────────────────────────────────────────────────────────────────────
// S.1 Core Types
// ────────────────────────────────────────────────────────────────────────────

/// A deployment target with SSH connection details and architecture info
/// for cross-compilation.
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct Host {
    pub name: String,
    pub user: String,
    pub address: String,
    pub arch: String,
    pub primary: bool,
}

impl Host {
    /// Returns the `user@address` string for SSH commands.
    pub fn ssh_target(&self) -> String {
        format!("{}@{}", self.user, self.address)
    }
}

// ────────────────────────────────────────────────────────────────────────────
// S.2 Configuration
// ────────────────────────────────────────────────────────────────────────────

/// Deployment host configuration — injectable from database.
#[derive(Debug, Clone)]
pub struct Config {
    pub hosts: HashMap<String, Host>,
    pub prod_dir: String,
    pub prod_bin: String,
}

impl Default for Config {
    fn default() -> Self {
        let mut hosts = HashMap::new();
        hosts.insert("oracle".into(), Host {
            name: "oracle".into(),
            user: "seanje".into(),
            address: "163.192.118.124".into(),
            arch: "arm64".into(),
            primary: true,
        });
        hosts.insert("dell".into(), Host {
            name: "dell".into(),
            user: "seanje".into(),
            address: "192.168.1.99".into(),
            arch: "amd64".into(),
            primary: false,
        });
        Self {
            hosts,
            prod_dir: "/home/seanje/cws".into(),
            prod_bin: "/usr/local/bin/cws-server".into(),
        }
    }
}

/// Package-level configuration. Set via `init()`, falls back to defaults.
static CFG: std::sync::OnceLock<Config> = std::sync::OnceLock::new();

/// Initializes the package configuration. Call once at startup.
pub fn init(cfg: Option<Config>) {
    let _ = CFG.set(cfg.unwrap_or_default());
}

/// Returns a reference to the active configuration.
fn cfg() -> &'static Config {
    CFG.get_or_init(Config::default)
}

// ═══════════════════════════════════════════════════════════════════════════
// BODY
// ═══════════════════════════════════════════════════════════════════════════

/// Returns the remote directory where CWS is deployed.
pub fn prod_dir() -> &'static str {
    &cfg().prod_dir
}

/// Returns the remote path to the cws-server binary.
pub fn prod_bin() -> &'static str {
    &cfg().prod_bin
}

/// Returns all known deployment targets.
///
/// The keys match the `PROD_TARGET` variable in the Makefile so either
/// system can reference hosts consistently.
pub fn all() -> &'static HashMap<String, Host> {
    &cfg().hosts
}

/// Returns the primary deployment target.
pub fn default_host() -> Host {
    let hosts = all();
    for h in hosts.values() {
        if h.primary {
            return h.clone();
        }
    }
    // Fallback — first host if no primary
    hosts.values().next().cloned().unwrap_or(Host {
        name: String::new(),
        user: String::new(),
        address: String::new(),
        arch: String::new(),
        primary: false,
    })
}

/// Looks up a host by name.
pub fn lookup(name: &str) -> Option<&'static Host> {
    all().get(name)
}

// ────────────────────────────────────────────────────────────────────────────
// Registry Integration
// ────────────────────────────────────────────────────────────────────────────

/// Builds a `Config` from cross-layer database queries via the registry.
///
/// Queries L2 platform `deployment_hosts` for all known targets. Extracts
/// prod_dir and prod_bin from the first host that has them.
pub fn config_from_registry(r: &bereshit_l2_registry::Registry) -> Config {
    let mut c = Config::default();

    if let Ok(rows) = r.platform.all_hosts() {
        if !rows.is_empty() {
            let mut hosts = HashMap::new();

            for dh in &rows {
                let host = Host {
                    name: dh.name.clone(),
                    user: dh.username.clone(),
                    address: dh.address.clone(),
                    arch: dh.arch.clone(),
                    primary: dh.is_primary,
                };
                hosts.insert(dh.name.clone(), host);

                // Extract prod paths from first host that has them
                if c.prod_dir.is_empty() && !dh.prod_dir.is_empty() {
                    c.prod_dir = dh.prod_dir.clone();
                }
                if c.prod_bin.is_empty() && !dh.prod_bin.is_empty() {
                    c.prod_bin = dh.prod_bin.clone();
                }
            }

            if !hosts.is_empty() {
                c.hosts = hosts;
            }
        }
    }

    c
}

// ═══════════════════════════════════════════════════════════════════════════
// CLOSING
// ═══════════════════════════════════════════════════════════════════════════

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn all_contains_oracle_and_dell() {
        let h = all();
        assert_eq!(h.len(), 2);
        assert!(h.contains_key("oracle"));
        assert!(h.contains_key("dell"));
    }

    #[test]
    fn oracle_is_primary() {
        let h = all();
        assert!(h["oracle"].primary);
        assert!(!h["dell"].primary);
    }

    #[test]
    fn oracle_details() {
        let h = &all()["oracle"];
        assert_eq!(h.name, "oracle");
        assert_eq!(h.user, "seanje");
        assert_eq!(h.address, "163.192.118.124");
        assert_eq!(h.arch, "arm64");
    }

    #[test]
    fn dell_details() {
        let h = &all()["dell"];
        assert_eq!(h.name, "dell");
        assert_eq!(h.arch, "amd64");
    }

    #[test]
    fn ssh_target_format() {
        let host = Host {
            name: "test".into(),
            user: "user".into(),
            address: "1.2.3.4".into(),
            arch: "amd64".into(),
            primary: false,
        };
        assert_eq!(host.ssh_target(), "user@1.2.3.4");
    }

    #[test]
    fn default_host_is_oracle() {
        let host = default_host();
        assert_eq!(host.name, "oracle");
        assert!(host.primary);
    }

    #[test]
    fn lookup_found() {
        assert!(lookup("oracle").is_some());
        assert!(lookup("dell").is_some());
    }

    #[test]
    fn lookup_not_found() {
        assert!(lookup("nonexistent").is_none());
    }

    #[test]
    fn prod_paths() {
        assert_eq!(prod_dir(), "/home/seanje/cws");
        assert_eq!(prod_bin(), "/usr/local/bin/cws-server");
    }

    #[test]
    fn config_default_has_2_hosts() {
        let c = Config::default();
        assert_eq!(c.hosts.len(), 2);
    }

    #[test]
    fn oracle_ssh_target() {
        let h = lookup("oracle").unwrap();
        assert_eq!(h.ssh_target(), "seanje@163.192.118.124");
    }
}
