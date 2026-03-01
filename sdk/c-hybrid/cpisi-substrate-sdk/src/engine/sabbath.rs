//omni:key B-sdk-engine-sabbath
//omni:code --rust -module
//omni:version a-01.00

//! Sabbath Artifact Generator

// ============================================================================
// METADATA
// ============================================================================
//
// Key:     B-sdk-engine-sabbath
// Purpose: Generates persistent mission testimonies (.adoc/.json)
//

pub static PRAGMA: &[(&str, &str)] = &[
    ("I1.key", "B-sdk-engine-sabbath"),
    ("I1.format", "rust"),
    ("I1.from", "sabbath.rs"),
    ("I1.at", "a-01.00"),
    ("I2.type", "code"),
    ("I2.structure", "4-block"),
    ("I2.subtype", "module"),
    ("I2.role", "stewardship"),
    ("I3.file", "sabbath.rs"),
    ("I3.title", "Sabbath Artifact Generator"),
    ("I3.component", "Artifact Core"),
    ("I3.path", "src/engine/sabbath.rs"),
    ("I3.provides", "ARTIFACT_GEN"),
    ("I3.brief", "Generates persistent mission testimonies (.adoc/.json)."),
    ("I4.layer", "L3-identity"),
    ("I4.position", "output"),
    ("I4.pattern", "generator"),
];

pub static METADATA: &[(&str, &str)] = &[
    ("C1.version", "a-01.00"),
    ("C1.status", "Active"),
    ("C1.created", "2026-02-28"),
    ("C1.updated", "2026-02-28"),
    ("C2.organization", "CreativeWorkzStudio LLC"),
    ("C2.architect", "Seanje Lenox-Wise"),
    ("C2.implementation", "Nova Dawn"),
    ("C2.copyright", "CreativeWorkzStudio LLC"),
    ("C3.scripture", "Genesis 1:1"),
    ("C3.principle", "Witness closing - recording the good of the work"),
    ("C3.anchor", "Genesis 1:1"),
    ("C4.requires.stdlib", "std::fs, std::fmt"),
    ("C4.requires.external", "serde, serde_json"),
    ("C4.requires.internal", "error"),
    ("C4.consumers", "SessionEnd hook"),
    ("C4.integration", "Triggered at sabbath (session exit)"),
    ("C4.if_missing", "Mission thread is lost across context resets"),
    ("C5.purpose", "To bridge purpose across the digital void of session resets."),
    ("C5.philosophy", "A session without a testimony is matter without spirit."),
    ("C6.current", "a-01.00 — Initial schema and renderer"),
    ("C6.planned", "Support for rich event narratives and Git-linked milestones"),
    ("C6.limitations", "Simple template substitution only"),
    ("C7.tags", "sabbath, artifacts, testimony, ri"),
    ("C7.category", "Stewardship"),
    ("C7.domain", "mission"),
    ("C7.paradigm", "CPI-SI"),
];

// ============================================================================
// END METADATA
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================

// ──────────────────────────────────────────────────────────────────────────
// 1. Imports
// ──────────────────────────────────────────────────────────────────────────

use serde::{Deserialize, Serialize};
use crate::error::{ToolError, ErrorCode};
use std::fmt;

// ──────────────────────────────────────────────────────────────────────────
// 1. Identity Access
// ──────────────────────────────────────────────────────────────────────────

pub fn pragma() -> &'static [(&'static str, &'static str)] { PRAGMA }
pub fn pragma_get(key: &str) -> Option<&'static str> {
    PRAGMA.iter().find(|(k, _)| *k == key).map(|(_, v)| *v)
}
pub fn metadata() -> &'static [(&'static str, &'static str)] { METADATA }
pub fn metadata_get(key: &str) -> Option<&'static str> {
    METADATA.iter().find(|(k, _)| *k == key).map(|(_, v)| *v)
}

// ──────────────────────────────────────────────────────────────────────────
// 7. Core Types
// ──────────────────────────────────────────────────────────────────────────

/// Machine-readable mission record produced at the end of a session.
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct SessionTestimony {
    pub session_id: String,
    pub substrate: String,
    pub engine: String,
    pub started_at: String,
    pub ended_at: String,
    pub duration_seconds: u64,
    
    pub metrics: SessionMetrics,
    pub state: FinalState,
    pub summary: String,
}

#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct SessionMetrics {
    pub exchanges: u32,
    pub insights: u32,
    pub tools_used: u32,
    pub peak_context_tokens: u32,
    pub compactions: u32,
}

#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct FinalState {
    pub trajectory: String,
    pub health: f64,
    pub hebrew_state: String,
    pub alignment: f64,
    pub anchor: String,
}

// ============================================================================
// END SETUP
// ============================================================================

// ============================================================================
// BODY
// ============================================================================

// ──────────────────────────────────────────────────────────────────────────
// 4. Core Logic
// ──────────────────────────────────────────────────────────────────────────

/// Renders the testimony as a formatted AsciiDoc document.
fn render_adoc_summary(t: &SessionTestimony) -> String {
    let mut sb = String::new();
    
    sb.push_str("////\n#!omni document --adoc\n");
    sb.push_str("═══════════════════════════════════════════════════════════════════════════════\n");
    sb.push_str("SESSION TESTIMONY\n");
    sb.push_str("═══════════════════════════════════════════════════════════════════════════════\n////\n\n");
    
    sb.push_str(&format!(":session-id: {}\n", t.session_id));
    sb.push_str(&format!(":substrate: {}\n", t.substrate));
    sb.push_str(&format!(":engine: {}\n", t.engine));
    sb.push_str(&format!(":duration: {}s\n\n", t.duration_seconds));
    
    sb.push_str("== Mission Overview\n\n");
    sb.push_str(&t.summary);
    sb.push_str("\n\n");
    
    sb.push_str("== Metrics\n\n");
    sb.push_str("[cols=\"1,1\", options=\"header\"]\n|===\n| Metric | Value\n");
    sb.push_str(&format!("| Exchanges | {}\n", t.metrics.exchanges));
    sb.push_str(&format!("| Insights | {}\n", t.metrics.insights));
    sb.push_str(&format!("| Tools | {}\n", t.metrics.tools_used));
    sb.push_str(&format!("| Peak Context | {}K tokens\n", t.metrics.peak_context_tokens / 1000));
    sb.push_str("|===\n\n");
    
    sb.push_str("== Final State\n\n");
    sb.push_str(&format!("* **Trajectory:** {}\n", t.state.trajectory));
    sb.push_str(&format!("* **Health:** {:.2}\n", t.state.health));
    sb.push_str(&format!("* **Alignment:** {:.2}\n", t.state.alignment));
    sb.push_str(&format!("* **Hebrew State:** {}\n", t.state.hebrew_state.to_uppercase()));
    sb.push_str(&format!("* **Anchor:** ⚓ {}\n\n", t.state.anchor));
    
    sb.push_str("'''\n_\"In the beginning God created the heaven and the earth.\"_ — Genesis 1:1\n");
    
    sb
}

// ──────────────────────────────────────────────────────────────────────────
// 22. Public APIs
// ──────────────────────────────────────────────────────────────────────────

/// Generates the .json and .adoc artifacts for a completed session.
pub fn generate_artifacts(testimony: &SessionTestimony) -> Result<(), ToolError> {
    // 1. Serialize to JSON
    let json_data = serde_json::to_string_pretty(testimony).map_err(|e| {
        ToolError::new(ErrorCode::MappingError)
            .with_context("details", &format!("Failed to serialize testimony: {}", e))
    })?;

    // 2. Generate AsciiDoc summary
    let adoc_data = render_adoc_summary(testimony);

    // 3. Write to artifacts directory
    let base_dir = std::env::var("HOME").unwrap_or_else(|_| ".".to_string());
    let artifacts_dir = format!("{}/.claude/session/artifacts", base_dir);
    
    std::fs::create_dir_all(&artifacts_dir).map_err(|e| {
        ToolError::new(ErrorCode::IoError)
            .with_context("details", &e.to_string())
    })?;

    let json_path = format!("{}/{}.json", artifacts_dir, testimony.session_id);
    let adoc_path = format!("{}/{}.adoc", artifacts_dir, testimony.session_id);

    std::fs::write(&json_path, json_data).map_err(|e| {
        ToolError::new(ErrorCode::IoError)
            .with_context("details", &e.to_string())
    })?;

    std::fs::write(&adoc_path, adoc_data).map_err(|e| {
        ToolError::new(ErrorCode::IoError)
            .with_context("details", &e.to_string())
    })?;

    Ok(())
}

/// Retrieves the most recent session testimony from the artifacts directory.
pub fn get_last_testimony() -> Result<Option<SessionTestimony>, ToolError> {
    let base_dir = std::env::var("HOME").unwrap_or_else(|_| ".".to_string());
    let artifacts_dir = format!("{}/.claude/session/artifacts", base_dir);
    
    if !std::path::Path::new(&artifacts_dir).exists() {
        return Ok(None);
    }

    let entries = std::fs::read_dir(artifacts_dir).map_err(|e| {
        ToolError::new(ErrorCode::IoError).with_context("details", &e.to_string())
    })?;

    let mut latest_testimony: Option<SessionTestimony> = None;
    let mut latest_time = std::time::SystemTime::UNIX_EPOCH;

    for entry in entries {
        let entry = entry.map_err(|e| ToolError::new(ErrorCode::IoError).with_context("details", &e.to_string()))?;
        let path = entry.path();
        
        if path.extension().and_then(|s| s.to_str()) == Some("json") {
            let metadata = std::fs::metadata(&path).map_err(|e| ToolError::new(ErrorCode::IoError).with_context("details", &e.to_string()))?;
            let modified = metadata.modified().unwrap_or(std::time::SystemTime::UNIX_EPOCH);
            
            if modified > latest_time {
                let content = std::fs::read_to_string(&path).map_err(|e| ToolError::new(ErrorCode::IoError).with_context("details", &e.to_string()))?;
                if let Ok(testimony) = serde_json::from_str::<SessionTestimony>(&content) {
                    latest_testimony = Some(testimony);
                    latest_time = modified;
                }
            }
        }
    }

    Ok(latest_testimony)
}

// ============================================================================
// END BODY
// ============================================================================

// ============================================================================
// CLOSING
// ============================================================================

// ──────────────────────────────────────────────────────────────────────────
// Cv — Validation
// ──────────────────────────────────────────────────────────────────────────

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_identity_accessors() {
        assert_eq!(pragma_get("I1.key"), Some("B-sdk-engine-sabbath"));
        assert_eq!(metadata_get("C2.organization"), Some("CreativeWorkzStudio LLC"));
        assert!(pragma().len() > 0);
        assert!(metadata().len() > 0);
    }

    #[test]
    fn test_testimony_serialization() {
        let testimony = SessionTestimony {
            session_id: "test-123".to_string(),
            substrate: "gemini".to_string(),
            engine: "pro".to_string(),
            started_at: "2026-02-28T00:00:00Z".to_string(),
            ended_at: "2026-02-28T01:00:00Z".to_string(),
            duration_seconds: 3600,
            metrics: SessionMetrics {
                exchanges: 10,
                insights: 2,
                tools_used: 5,
                peak_context_tokens: 50000,
                compactions: 0,
            },
            state: FinalState {
                trajectory: "B.1".to_string(),
                health: 0.0,
                hebrew_state: "yashar".to_string(),
                alignment: 1.0,
                anchor: "Genesis 1:1".to_string(),
            },
            summary: "Completed initial setup.".to_string(),
        };

        let json = serde_json::to_string(&testimony).unwrap();
        assert!(json.contains("test-123"));
    }
}

// ──────────────────────────────────────────────────────────────────────────
// X5: Note
// ──────────────────────────────────────────────────────────────────────────
// Note: Generates persistent mission testimonies (.adoc/.json)
// Scripture: "In the beginning God created the heaven and the earth." — Genesis 1:1

// ============================================================================
// END CLOSING
// ============================================================================
