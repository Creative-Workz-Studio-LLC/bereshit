//omni:code --rust -library
//omni:key B-L2-server
//omni:version a-01.00

// ═══════════════════════════════════════════════════════════════════════════
// METADATA
// ═══════════════════════════════════════════════════════════════════════════
//
// Key: B-L2-server
// Purpose: Server configuration — flags, environment, feature toggles
// Biblical: Proverbs 24:27 — "Prepare thy work without, and make it fit"
// Authors: Nova Dawn
// Version: 0.2.0
// Created: 2026-02-15
//
// Configuration flows through three layers:
//
//  1. Defaults — hardcoded sensible values (port 3847, CPI-SI enabled)
//  2. Environment — CWS_SERVER_PORT, CWS_BUILDER_DIR override defaults
//  3. Flags — command-line flags override everything
//
// ═══════════════════════════════════════════════════════════════════════════

// ═══════════════════════════════════════════════════════════════════════════
// SETUP
// ═══════════════════════════════════════════════════════════════════════════

use serde::{Deserialize, Serialize};
use std::env;

// ────────────────────────────────────────────────────────────────────────────
// S.1 Defaults — database-backed server settings with hardcoded fallback
// ────────────────────────────────────────────────────────────────────────────

/// Database-sourced default values. These are the "first layer" that
/// environment variables and flags can override.
#[derive(Debug, Clone)]
pub struct Defaults {
    pub port: u16,
    pub env_port: String,
    pub env_builder: String,
}

impl Default for Defaults {
    fn default() -> Self {
        Self {
            port: 3847,
            env_port: "CWS_SERVER_PORT".into(),
            env_builder: "CWS_BUILDER_DIR".into(),
        }
    }
}

/// Package-level defaults. Set via `init_defaults()`, falls back to hardcoded.
static DEFAULTS: std::sync::OnceLock<Defaults> = std::sync::OnceLock::new();

/// Initializes the package defaults from database. Pass `None` to keep hardcoded.
pub fn init_defaults(d: Option<Defaults>) {
    let _ = DEFAULTS.set(d.unwrap_or_default());
}

/// Returns a reference to the active defaults.
fn defaults() -> &'static Defaults {
    DEFAULTS.get_or_init(Defaults::default)
}

// ═══════════════════════════════════════════════════════════════════════════
// BODY
// ═══════════════════════════════════════════════════════════════════════════

/// Resolved server configuration.
///
/// All fields can be set via flags or environment variables.
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct ServerConfig {
    /// HTTP listen port (default: from DB or 3847, env: CWS_SERVER_PORT).
    pub port: u16,

    /// Path to company-docs/ project root (env: CWS_BUILDER_DIR).
    pub builder_dir: String,

    /// Enable CPI-SI state machine + journal service.
    pub enable_cpisi: bool,

    /// Development mode: debug logging, filesystem serving.
    pub dev_mode: bool,
}

impl Default for ServerConfig {
    fn default() -> Self {
        Self {
            port: 3847,
            builder_dir: String::new(),
            enable_cpisi: true,
            dev_mode: false,
        }
    }
}

/// Returns a `ServerConfig` with sensible defaults, merged with environment
/// variables.
///
/// Port 3847 was chosen to avoid conflicts with common dev ports (3000,
/// 5173, 8080). CPI-SI is enabled by default; the builder service requires
/// a valid project directory to activate.
pub fn default_config() -> ServerConfig {
    let d = defaults();

    let port = env::var(&d.env_port)
        .ok()
        .and_then(|s| s.parse().ok())
        .unwrap_or(d.port);

    let builder_dir = env::var(&d.env_builder).unwrap_or_default();

    ServerConfig {
        port,
        builder_dir,
        ..Default::default()
    }
}

// ────────────────────────────────────────────────────────────────────────────
// Registry Integration
// ────────────────────────────────────────────────────────────────────────────

/// Builds `Defaults` from cross-layer database queries via the registry.
///
/// Queries L2 platform `server_configs` for port, env_port, and env_builder
/// keys. Falls back to hardcoded defaults for any key that fails.
pub fn defaults_from_registry(r: &bereshit_l2_registry::Registry) -> Defaults {
    let mut d = Defaults::default();

    if let Ok(sc) = r.platform.server_config_by_key("port") {
        if let Ok(p) = sc.value.parse::<u16>() {
            d.port = p;
        }
    }

    if let Ok(sc) = r.platform.server_config_by_key("env_port") {
        if !sc.value.is_empty() {
            d.env_port = sc.value;
        }
    }

    if let Ok(sc) = r.platform.server_config_by_key("env_builder") {
        if !sc.value.is_empty() {
            d.env_builder = sc.value;
        }
    }

    d
}

// ═══════════════════════════════════════════════════════════════════════════
// CLOSING
// ═══════════════════════════════════════════════════════════════════════════

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn default_config_port() {
        let cfg = ServerConfig::default();
        assert_eq!(cfg.port, 3847);
        assert!(cfg.enable_cpisi);
        assert!(!cfg.dev_mode);
    }

    #[test]
    fn default_config_builder_dir_empty() {
        let cfg = ServerConfig::default();
        assert!(cfg.builder_dir.is_empty());
    }

    #[test]
    fn defaults_port() {
        let d = Defaults::default();
        assert_eq!(d.port, 3847);
    }

    #[test]
    fn defaults_env_names() {
        let d = Defaults::default();
        assert_eq!(d.env_port, "CWS_SERVER_PORT");
        assert_eq!(d.env_builder, "CWS_BUILDER_DIR");
    }

    #[test]
    fn defaults_ref_returns_hardcoded() {
        let d = defaults();
        assert_eq!(d.port, 3847);
    }

    #[test]
    fn default_config_uses_defaults() {
        let cfg = default_config();
        assert_eq!(cfg.port, 3847);
        assert!(cfg.enable_cpisi);
    }
}
