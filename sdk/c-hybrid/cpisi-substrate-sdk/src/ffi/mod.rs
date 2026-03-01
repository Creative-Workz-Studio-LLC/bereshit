//omni:key B-sdk-ffi-mod
//omni:code --rust -module
//omni:version a-01.00

//! FFI Boundary

// ============================================================================
// METADATA
// ============================================================================
//
// Key:     B-sdk-ffi-mod
// Purpose: C-bindings for Go hooks
//

pub static PRAGMA: &[(&str, &str)] = &[
    ("I1.key", "B-sdk-ffi-mod"),
    ("I1.format", "rust"),
    ("I1.from", "src/ffi/mod.rs"),
    ("I1.at", "a-01.00"),
    ("I2.type", "code"),
    ("I2.structure", "4-block"),
    ("I2.subtype", "module"),
    ("I2.role", "FFI"),
    ("I3.file", "mod.rs"),
    ("I3.title", "FFI Boundary"),
    ("I3.component", "Bridge"),
    ("I3.path", "src/ffi/mod.rs"),
    ("I3.provides", "FFI_APIS"),
    ("I3.brief", "C-bindings for Go hooks and TS Mind layer."),
    ("I4.layer", "L0-foundation"),
    ("I4.position", "boundary"),
    ("I4.pattern", "FFI"),
];

pub static METADATA: &[(&str, &str)] = &[
    ("C1.version", "a-01.00"),
    ("C1.status", "Active"),
    ("C1.created", "2026-02-28"),
    ("C1.updated", "2026-02-28"),
    ("C2.architect", "Seanje Lenox-Wise"),
    ("C2.implementation", "Nova Dawn"),
    ("C2.organization", "CreativeWorkzStudio LLC"),
    ("C2.copyright", "CreativeWorkzStudio LLC"),
    ("C3.scripture", "Genesis 1:1"),
    ("C3.principle", "Order at the boundary"),
    ("C3.anchor", "Genesis 1:1"),
    ("C4.requires.stdlib", "std::ffi, std::sync"),
    ("C4.requires.external", "serde_json"),
    ("C4.requires.internal", "engine"),
    ("C4.consumers", "Go Body, TS Mind"),
    ("C4.integration", "C-ABI"),
    ("C4.if_missing", "Cross-language communication fails"),
    ("C5.purpose", "To provide a stable C-ABI for substrate mapping and artifact generation."),
    ("C5.philosophy", "The Spirit (Rust) provides the immutable truth to all other layers."),
    ("C6.current", "a-01.00 — Integrated Sabbath generation and substrate mapping"),
    ("C6.planned", "Direct state-machine pointer sharing"),
    ("C6.limitations", "String-based JSON passing (high allocation)"),
    ("C7.tags", "ffi, rust, abi"),
    ("C7.category", "Infrastructure"),
    ("C7.domain", "bridge"),
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

use crate::engine::{MapperEngine, SessionTestimony};
use std::ffi::{CStr, CString};
use std::os::raw::c_char;
use std::sync::RwLock;
use std::sync::LazyLock;

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
// 4. Statics
// ──────────────────────────────────────────────────────────────────────────

/// Global instance of the MapperEngine, protected by a lock for FFI access.
static ENGINE: LazyLock<RwLock<MapperEngine>> = LazyLock::new(|| {
    RwLock::new(MapperEngine::new())
});

// ============================================================================
// END SETUP
// ============================================================================

// ============================================================================
// BODY
// ============================================================================

// ──────────────────────────────────────────────────────────────────────────
// 22. Public APIs (C-FFI)
// ──────────────────────────────────────────────────────────────────────────

/// Generates session artifacts (.json/.adoc) from a JSON-serialized testimony.
#[unsafe(no_mangle)]
pub extern "C" fn cpisi_sdk_generate_sabbath_artifacts(testimony_json: *const c_char) -> i32 {
    if testimony_json.is_null() { return -1; }
    let ctx_str = unsafe { CStr::from_ptr(testimony_json) }.to_str().unwrap_or("");

    let testimony: SessionTestimony = match serde_json::from_str(ctx_str) {
        Ok(t) => t,
        Err(_) => return -2,
    };

    match crate::engine::generate_artifacts(&testimony) {
        Ok(_) => 0,
        Err(_) => -3,
    }
}

/// Retrieves the last session testimony as a JSON string.
/// The caller is responsible for freeing the returned string.
#[unsafe(no_mangle)]
pub extern "C" fn cpisi_sdk_get_last_testimony() -> *mut c_char {
    match crate::engine::get_last_testimony() {
        Ok(Some(t)) => {
            let json = serde_json::to_string(&t).unwrap_or_default();
            let c_result = CString::new(json).unwrap_or_default();
            c_result.into_raw()
        },
        _ => std::ptr::null_mut(),
    }
}

/// Loads a substrate map from a TOML file.
/// Returns 0 on success, negative on error.
#[unsafe(no_mangle)]
pub extern "C" fn cpisi_sdk_load_map(path: *const c_char) -> i32 {
    if path.is_null() { return -1; }
    let c_str = unsafe { CStr::from_ptr(path) };
    let path_str = match c_str.to_str() {
        Ok(s) => s,
        Err(_) => return -2,
    };

    let mut engine = match ENGINE.write() {
        Ok(g) => g,
        Err(_) => return -3,
    };

    match engine.load_from_file(path_str) {
        Ok(_) => 0,
        Err(_) => -4,
    }
}

/// Processes an event and returns a JSON string.
/// The caller is responsible for freeing the returned string using cpisi_sdk_free_string.
#[unsafe(no_mangle)]
pub extern "C" fn cpisi_sdk_process_event(
    substrate: *const c_char,
    event: *const c_char,
    raw_json: *const c_char
) -> *mut c_char {
    if substrate.is_null() || event.is_null() || raw_json.is_null() { return std::ptr::null_mut(); }
    
    let sub_str = unsafe { CStr::from_ptr(substrate) }.to_str().unwrap_or("");
    let event_str = unsafe { CStr::from_ptr(event) }.to_str().unwrap_or("");
    let json_str = unsafe { CStr::from_ptr(raw_json) }.to_str().unwrap_or("");

    let engine = match ENGINE.read() {
        Ok(g) => g,
        Err(_) => return std::ptr::null_mut(),
    };

    match engine.process_event(sub_str, event_str, json_str) {
        Ok(result) => {
            let c_result = CString::new(result).unwrap_or_default();
            c_result.into_raw()
        },
        Err(_) => std::ptr::null_mut(),
    }
}

/// Renders a specific output variant for a substrate event.
/// Context is passed as a JSON object string.
#[unsafe(no_mangle)]
pub extern "C" fn cpisi_sdk_render_output(
    substrate: *const c_char,
    event: *const c_char,
    variant: *const c_char,
    context_json: *const c_char
) -> *mut c_char {
    if substrate.is_null() || event.is_null() || variant.is_null() || context_json.is_null() {
        return std::ptr::null_mut();
    }

    let sub_str = unsafe { CStr::from_ptr(substrate) }.to_str().unwrap_or("");
    let event_str = unsafe { CStr::from_ptr(event) }.to_str().unwrap_or("");
    let var_str = unsafe { CStr::from_ptr(variant) }.to_str().unwrap_or("");
    let ctx_str = unsafe { CStr::from_ptr(context_json) }.to_str().unwrap_or("");

    // Parse context JSON into HashMap<String, String>
    let context: std::collections::HashMap<String, String> = match serde_json::from_str(ctx_str) {
        Ok(m) => m,
        Err(_) => return std::ptr::null_mut(),
    };

    let engine = match ENGINE.read() {
        Ok(g) => g,
        Err(_) => return std::ptr::null_mut(),
    };

    match engine.render_output(sub_str, event_str, var_str, &context) {
        Ok(result) => {
            let c_result = CString::new(result).unwrap_or_default();
            c_result.into_raw()
        },
        Err(_) => std::ptr::null_mut(),
    }
}

/// Frees a string allocated by the Rust SDK.
#[unsafe(no_mangle)]
pub extern "C" fn cpisi_sdk_free_string(s: *mut c_char) {
    if s.is_null() { return; }
    unsafe {
        let _ = CString::from_raw(s);
    }
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
mod identity_tests {
    use super::*;

    #[test]
    fn test_identity_accessors() {
        assert_eq!(pragma_get("I1.key"), Some("B-sdk-ffi-mod"));
        assert_eq!(metadata_get("C2.organization"), Some("CreativeWorkzStudio LLC"));
        assert!(pragma().len() > 0);
        assert!(metadata().len() > 0);
    }

    #[test]
    fn test_identity_completeness() {
        for prefix in ["I1.", "I2.", "I3.", "I4."] {
            assert!(PRAGMA.iter().any(|(k, _)| k.starts_with(prefix)));
        }
    }
}

// ──────────────────────────────────────────────────────────────────────────
// X5: Note
// ──────────────────────────────────────────────────────────────────────────
// Note: C-bindings for Go hooks
// Scripture: "In the beginning God created the heaven and the earth." — Genesis 1:1

// ============================================================================
// END CLOSING
// ============================================================================
