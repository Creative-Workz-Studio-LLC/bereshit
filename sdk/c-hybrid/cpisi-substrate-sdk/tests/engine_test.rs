// ──────────────────────────────────────────────────────────────────────────
// 1. Imports
// ──────────────────────────────────────────────────────────────────────────

use cpisi_substrate_sdk::engine::MapperEngine;
use cpisi_substrate_sdk::error::ErrorCode;

mod helpers;
use helpers::{fixture, witness_for};

// ──────────────────────────────────────────────────────────────────────────
// 4. Core Logic
// ──────────────────────────────────────────────────────────────────────────

#[test]
fn t01_001_witness_map_not_found() {
    witness_for("CWS-T01-001", "Substrate map file not found or unreadable");
    
    let mut engine = MapperEngine::new();
    let result = engine.load_from_file(&fixture("maps/does_not_exist.toml"));
    
    assert!(result.is_err());
    let err = result.unwrap_err();
    assert_eq!(err.code, ErrorCode::MapNotFound);
}

#[test]
fn t01_002_witness_map_parse_error() {
    witness_for("CWS-T01-002", "Substrate map file contains invalid TOML");
    
    let mut engine = MapperEngine::new();
    let result = engine.load_from_file(&fixture("maps/invalid_map.toml"));
    
    assert!(result.is_err());
    let err = result.unwrap_err();
    assert_eq!(err.code, ErrorCode::MapParseError);
}

#[test]
fn engine_loads_valid_fixture() {
    let mut engine = MapperEngine::new();
    let result = engine.load_from_file(&fixture("maps/valid_map.toml"));
    
    assert!(result.is_ok(), "Engine should load valid TOML fixture map");
}
