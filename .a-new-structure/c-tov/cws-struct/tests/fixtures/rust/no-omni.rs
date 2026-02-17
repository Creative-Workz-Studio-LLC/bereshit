// A plain Rust file with no //omni: directives and no block markers.
// The linter should report info only — "not a 4-block file".

fn hello() {
    println!("No structural markers here.");
}
