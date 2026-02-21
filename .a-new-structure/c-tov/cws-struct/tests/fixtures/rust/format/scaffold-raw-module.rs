// #!omni code --rust -module
//omni:key B-test-scaffold-raw
//omni:code --rust -module
//omni:version a-01.00
//! A raw module with pragma but no block structure.
//! Used to test Transform 0 (structural scaffold).

use std::fmt;
use std::path::Path;

/// Greet a person by name.
pub fn greet(name: &str) -> String {
    format!("Hello, {}!", name)
}

/// Display a path as a string.
pub fn path_display(p: &Path) -> String {
    p.display().to_string()
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_greet() {
        assert_eq!(greet("world"), "Hello, world!");
    }
}
