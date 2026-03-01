use std::path::PathBuf;

/// Resolve a fixture path relative to the tests/fixtures directory.
pub fn fixture(path: &str) -> String {
    let mut d = PathBuf::from(env!("CARGO_MANIFEST_DIR"));
    d.push("tests/fixtures");
    d.push(path);
    d.to_string_lossy().to_string()
}

/// Register this test as a witness for a specific error code.
/// In the Rust SDK, this prints a visible marker during test execution.
pub fn witness_for(code: &str, description: &str) {
    println!("[WITNESS] {} -> {}", code, description);
}
