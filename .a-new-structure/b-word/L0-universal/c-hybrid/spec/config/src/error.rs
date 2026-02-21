//! Configuration error types — loading, validation, and dependency failures.
//!
//! Typed enums replace stringly-typed fields for compile-time safety:
//! - [`LoadOp`] — what operation failed (read, parse, lookup, etc.)
//! - [`DepKind`] — what dependency problem (missing, cycle)

use std::fmt;

// ────────────────────────────────────────────────────────────────
// Operation & Kind Enums
// ────────────────────────────────────────────────────────────────

/// What loading operation failed — compile-time safety over stringly-typed `op`.
///
/// Each variant maps to a distinct failure mode in the config loading pipeline.
#[derive(Debug, Clone, PartialEq, Eq)]
pub enum LoadOp {
    /// Filesystem read (fs::read_to_string).
    Read,
    /// Deserialization (toml::from_str, serde_json::from_str).
    Parse,
    /// JSONC-specific parsing (strip comments + parse).
    ParseJsonc,
    /// Manifest/watch-path system lookup.
    Lookup,
    /// File existence check (stat).
    Stat,
    /// Format detection from file extension.
    Detect,
    /// Cross-format conversion (JSON → TOML).
    Convert,
    /// Encoding conversion (bytes → UTF-8).
    Decode,
    /// Config file search across multiple paths.
    Find,
}

impl fmt::Display for LoadOp {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        match self {
            Self::Read => write!(f, "read"),
            Self::Parse => write!(f, "parse"),
            Self::ParseJsonc => write!(f, "parse_jsonc"),
            Self::Lookup => write!(f, "lookup"),
            Self::Stat => write!(f, "stat"),
            Self::Detect => write!(f, "detect"),
            Self::Convert => write!(f, "convert"),
            Self::Decode => write!(f, "decode"),
            Self::Find => write!(f, "find"),
        }
    }
}

/// What dependency problem was detected — compile-time safety over stringly-typed `kind`.
#[derive(Debug, Clone, PartialEq, Eq)]
pub enum DepKind {
    /// Dependency not found in the manifest.
    Missing,
    /// Circular dependency detected in the graph.
    Cycle,
    /// Version constraint mismatch (future use).
    VersionMismatch,
}

impl fmt::Display for DepKind {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        match self {
            Self::Missing => write!(f, "missing"),
            Self::Cycle => write!(f, "cycle"),
            Self::VersionMismatch => write!(f, "version_mismatch"),
        }
    }
}

// ────────────────────────────────────────────────────────────────
// Error Type
// ────────────────────────────────────────────────────────────────

/// Configuration error — covers loading, validation, and dependency problems.
#[derive(Debug, Clone)]
pub enum ConfigError {
    /// File-level loading failure (not found, parse error).
    Load {
        /// Path to the file that failed.
        file: String,
        /// Which operation failed.
        op: LoadOp,
        /// Underlying error description.
        source: String,
    },
    /// Field-level validation failure within a spec.
    Validation {
        /// File path where the validation failed.
        path: String,
        /// TOML section containing the invalid field.
        section: String,
        /// Specific field name that failed validation.
        field: String,
        /// What went wrong.
        message: String,
    },
    /// Dependency graph problem (missing reference, cycle).
    Dependency {
        /// Spec that has the dependency issue.
        spec: String,
        /// The dependency spec referenced.
        dep_spec: String,
        /// Classification of the dependency problem.
        kind: DepKind,
        /// Detailed description.
        message: String,
    },
    /// Root directory not set — call `set_root()` first.
    RootNotSet {
        /// Name of the root that was expected.
        name: String,
    },
}

impl fmt::Display for ConfigError {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        match self {
            Self::Load { file, op, source } => write!(f, "{file} ({op}): {source}"),
            Self::Validation {
                path,
                section,
                field,
                message,
            } => {
                if field.is_empty() {
                    write!(f, "{path} [{section}]: {message}")
                } else {
                    write!(f, "{path} [{section}].{field}: {message}")
                }
            }
            Self::Dependency {
                spec,
                dep_spec,
                kind,
                message,
            } => {
                if dep_spec.is_empty() {
                    write!(f, "{spec} ({kind}): {message}")
                } else {
                    write!(f, "{spec} -> {dep_spec} ({kind}): {message}")
                }
            }
            Self::RootNotSet { name } => {
                write!(f, "{name} root not set \u{2014} call set_root() first")
            }
        }
    }
}

impl std::error::Error for ConfigError {}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_config_error_load_display() {
        let err = ConfigError::Load {
            file: "ternary.toml".into(),
            op: LoadOp::Read,
            source: "not found".into(),
        };
        assert_eq!(err.to_string(), "ternary.toml (read): not found");
    }

    #[test]
    fn test_config_error_validation_display() {
        let err = ConfigError::Validation {
            path: "types/primitives.toml".into(),
            section: "int9".into(),
            field: "min_value".into(),
            message: "expected integer".into(),
        };
        assert_eq!(
            err.to_string(),
            "types/primitives.toml [int9].min_value: expected integer"
        );
    }

    #[test]
    fn test_config_error_validation_no_field() {
        let err = ConfigError::Validation {
            path: "math/ternary.toml".into(),
            section: "ternary".into(),
            field: String::new(),
            message: "section empty".into(),
        };
        assert_eq!(
            err.to_string(),
            "math/ternary.toml [ternary]: section empty"
        );
    }

    #[test]
    fn test_config_error_dependency_display() {
        let err = ConfigError::Dependency {
            spec: "types/comp.toml".into(),
            dep_spec: "types/prims.toml".into(),
            kind: DepKind::Missing,
            message: "dependency not in manifest".into(),
        };
        assert_eq!(
            err.to_string(),
            "types/comp.toml -> types/prims.toml (missing): dependency not in manifest"
        );
    }

    #[test]
    fn test_config_error_dependency_no_dep_spec() {
        let err = ConfigError::Dependency {
            spec: "types/comp.toml".into(),
            dep_spec: String::new(),
            kind: DepKind::Cycle,
            message: "circular".into(),
        };
        assert_eq!(err.to_string(), "types/comp.toml (cycle): circular");
    }

    #[test]
    fn test_config_error_root_not_set() {
        let err = ConfigError::RootNotSet {
            name: "bereshit".into(),
        };
        assert!(err.to_string().contains("root not set"));
    }

    #[test]
    fn test_load_op_display() {
        assert_eq!(LoadOp::Read.to_string(), "read");
        assert_eq!(LoadOp::Parse.to_string(), "parse");
        assert_eq!(LoadOp::ParseJsonc.to_string(), "parse_jsonc");
        assert_eq!(LoadOp::Lookup.to_string(), "lookup");
        assert_eq!(LoadOp::Stat.to_string(), "stat");
        assert_eq!(LoadOp::Detect.to_string(), "detect");
        assert_eq!(LoadOp::Convert.to_string(), "convert");
        assert_eq!(LoadOp::Decode.to_string(), "decode");
        assert_eq!(LoadOp::Find.to_string(), "find");
    }

    #[test]
    fn test_dep_kind_display() {
        assert_eq!(DepKind::Missing.to_string(), "missing");
        assert_eq!(DepKind::Cycle.to_string(), "cycle");
        assert_eq!(DepKind::VersionMismatch.to_string(), "version_mismatch");
    }

    #[test]
    fn test_load_op_equality() {
        assert_eq!(LoadOp::Read, LoadOp::Read);
        assert_ne!(LoadOp::Read, LoadOp::Parse);
    }

    #[test]
    fn test_dep_kind_equality() {
        assert_eq!(DepKind::Missing, DepKind::Missing);
        assert_ne!(DepKind::Missing, DepKind::Cycle);
    }
}
