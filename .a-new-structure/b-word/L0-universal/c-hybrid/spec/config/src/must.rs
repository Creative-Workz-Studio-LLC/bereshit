// #!omni code --rust -module
//omni:key B-L0-hybrid-config-must
//omni:code --rust -module
//omni:version b-03.00
//! Generic panic-on-error wrappers — `MustExt` for config loading.
//!
//! Ported from Go `must.go`. Replaces 7 identical MustLoad* wrappers.
//!
//! Joshua 1:9 — "Be strong and of a good courage."
//! Must is courage in code. If the config isn't there at startup,
//! don't limp along — halt immediately and name the problem.
//!
//! # Usage
//!
//! ```
//! use bereshit_l0_config::MustExt;
//!
//! let value: i32 = Ok::<i32, String>(42).must("should have a value");
//! assert_eq!(value, 42);
//! ```

use std::fmt::Display;

// ────────────────────────────────────────────────────────────────
// (T, error) Pattern → Result<T, E>
// ────────────────────────────────────────────────────────────────

/// Extension trait on `Result<T, E>` for fatal config assertions.
///
/// Use for config loading in `main()` or initialization where failure
/// is unrecoverable.
///
/// ```
/// use bereshit_l0_config::MustExt;
///
/// let schemas: Vec<String> = Ok::<Vec<String>, String>(vec!["a".into()]).must("load schemas");
/// assert_eq!(schemas.len(), 1);
/// ```
pub trait MustExt<T> {
    /// Unwrap the value, panicking with a structured message if Err.
    fn must(self, context: &str) -> T;
}

impl<T, E: Display> MustExt<T> for Result<T, E> {
    fn must(self, context: &str) -> T {
        match self {
            Ok(val) => val,
            Err(e) => panic!("config: must: {context}: {e}"),
        }
    }
}

// ────────────────────────────────────────────────────────────────
// (T, bool) Pattern → Option<T>
// ────────────────────────────────────────────────────────────────

/// Extension trait on `Option<T>` for fatal assertions on lookups.
///
/// Use for map lookups or conversions where absence is fatal.
///
/// ```
/// use bereshit_l0_config::MustValExt;
///
/// let val: i32 = Some(42).must_val("system not found");
/// assert_eq!(val, 42);
/// ```
pub trait MustValExt<T> {
    /// Unwrap the value, panicking with `msg` if None.
    fn must_val(self, msg: &str) -> T;
}

impl<T> MustValExt<T> for Option<T> {
    fn must_val(self, msg: &str) -> T {
        match self {
            Some(val) => val,
            None => panic!("config: must: {msg}"),
        }
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_must_ext_success() {
        let result: Result<i32, String> = Ok(42);
        assert_eq!(result.must("should not panic"), 42);
    }

    #[test]
    #[should_panic(expected = "config: must: boom")]
    fn test_must_ext_panic() {
        let result: Result<i32, String> = Err("kaboom".into());
        let _ = result.must("boom");
    }

    #[test]
    fn test_must_val_ext_success() {
        assert_eq!(Some(42).must_val("should not panic"), 42);
    }

    #[test]
    #[should_panic(expected = "config: must: not found")]
    fn test_must_val_ext_panic() {
        let _: i32 = None.must_val("not found");
    }
}
