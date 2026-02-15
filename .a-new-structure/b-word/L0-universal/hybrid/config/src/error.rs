//! Configuration error types — loading, validation, and dependency failures.

use std::fmt;

/// Configuration error — covers loading, validation, and dependency problems.
#[derive(Debug, Clone)]
pub enum ConfigError {
    /// File-level loading failure (not found, parse error).
    Load {
        file: String,
        op: String,
        source: String,
    },
    /// Field-level validation failure within a spec.
    Validation {
        path: String,
        section: String,
        field: String,
        message: String,
    },
    /// Dependency graph problem (missing reference, cycle).
    Dependency {
        spec: String,
        dep_spec: String,
        kind: String,
        message: String,
    },
    /// Root directory not set — call `set_root()` first.
    RootNotSet { name: String },
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
