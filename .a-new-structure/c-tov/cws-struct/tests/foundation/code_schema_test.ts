// ============================================================================
// METADATA
// ============================================================================
//
// file:    tests/foundation/code_schema_test.ts
// key:     B-tov-cws-struct-tests-foundation-code-schema
// title:   CWS Struct — Code Schema Loader Tests
// type:    Code (Test)
// version: a-01.00
// created: 2026-02-19
// authors: Nova Dawn (CPI-SI)
// purpose: Tests for loadCodeRules METADATA block extraction.
//          Verifies I/C field requirements, doc comments, and subtypes
//          are correctly loaded from Go and Rust schemas.
//
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================

import { assertEquals, assert, assertGreater } from "jsr:@std/assert";
import { loadCodeRules, clearCodeCache } from "../../lib/foundation/mod.ts";
// Types used implicitly via loadCodeRules return type

// ============================================================================
// BODY
// ============================================================================

// ---------------------------------------------------------------------------
// Go schema — pragma field requirements (I1-I4)
// ---------------------------------------------------------------------------

Deno.test("code-schema/go: loads pragma field requirements (I1-I4)", async () => {
  clearCodeCache("go");
  const rules = await loadCodeRules("go");

  assert(rules.pragmaFieldRequirements !== undefined, "Should have pragmaFieldRequirements");

  // I1 — WHO: unique identity
  const i1 = rules.pragmaFieldRequirements["I1"];
  assert(i1 !== undefined, "Should have I1");
  assertEquals(i1.required, ["key", "format", "from", "at"]);
  assertEquals(i1.purpose, "WHO — unique identity");

  // I2 — WHAT: classification
  const i2 = rules.pragmaFieldRequirements["I2"];
  assert(i2 !== undefined, "Should have I2");
  assertEquals(i2.required, ["type", "structure"]);
  assertEquals(i2.defined, ["subtype", "role"]);

  // I3 — THIS: file-specific
  const i3 = rules.pragmaFieldRequirements["I3"];
  assert(i3 !== undefined, "Should have I3");
  assertEquals(i3.required, ["file", "title"]);
  assert(i3.defined.includes("component"), "I3.defined should include component");

  // I4 — WHERE: structural context
  const i4 = rules.pragmaFieldRequirements["I4"];
  assert(i4 !== undefined, "Should have I4");
  assertEquals(i4.required, []); // I4 has no required fields — all are defined-only
  assert(i4.defined.includes("layer"), "I4.defined should include layer");
});

// ---------------------------------------------------------------------------
// Go schema — metadata field requirements (C1-C7)
// ---------------------------------------------------------------------------

Deno.test("code-schema/go: loads metadata field requirements (C1-C7)", async () => {
  const rules = await loadCodeRules("go");

  assert(rules.metadataFieldRequirements !== undefined, "Should have metadataFieldRequirements");

  // C1 — WHEN: lifecycle
  const c1 = rules.metadataFieldRequirements["C1"];
  assert(c1 !== undefined, "Should have C1");
  assertEquals(c1.required, ["version", "status"]);
  assertEquals(c1.purpose, "WHEN — lifecycle");

  // C2 — WHO MADE: credit
  const c2 = rules.metadataFieldRequirements["C2"];
  assert(c2 !== undefined, "Should have C2");
  assertEquals(c2.required, ["organization"]);

  // C3 — WHY: biblical foundation
  const c3 = rules.metadataFieldRequirements["C3"];
  assert(c3 !== undefined, "Should have C3");
  assertEquals(c3.required, ["scripture"]);

  // C4 — NEEDS: dependency graph
  const c4 = rules.metadataFieldRequirements["C4"];
  assert(c4 !== undefined, "Should have C4");
  assertEquals(c4.required, ["requires", "consumers"]);

  // C5 — GOAL: design intent (all defined, no required)
  const c5 = rules.metadataFieldRequirements["C5"];
  assert(c5 !== undefined, "Should have C5");
  assertEquals(c5.required, []); // No required fields
  assert(c5.defined.includes("purpose"), "C5.defined should include purpose");

  // C6 — NEXT: trajectory (all defined)
  const c6 = rules.metadataFieldRequirements["C6"];
  assert(c6 !== undefined, "Should have C6");

  // C7 — TAGS: discovery (all defined)
  const c7 = rules.metadataFieldRequirements["C7"];
  assert(c7 !== undefined, "Should have C7");
  assert(c7.defined.includes("tags"), "C7.defined should include tags");
});

// ---------------------------------------------------------------------------
// Rust schema — I/C field requirements match Go (format-agnostic standard)
// ---------------------------------------------------------------------------

Deno.test("code-schema/rust: I/C field requirements match Go schema", async () => {
  clearCodeCache("rust");
  const goRules = await loadCodeRules("go");
  const rustRules = await loadCodeRules("rust");

  // Pragma I1-I4 should be identical
  const goI1 = goRules.pragmaFieldRequirements["I1"];
  const rustI1 = rustRules.pragmaFieldRequirements["I1"];
  assertEquals(rustI1?.required, goI1?.required, "I1.required should match across formats");

  const goI2 = goRules.pragmaFieldRequirements["I2"];
  const rustI2 = rustRules.pragmaFieldRequirements["I2"];
  assertEquals(rustI2?.required, goI2?.required, "I2.required should match");
  assertEquals(rustI2?.defined, goI2?.defined, "I2.defined should match");

  // Metadata C1-C7 should be identical
  const goC1 = goRules.metadataFieldRequirements["C1"];
  const rustC1 = rustRules.metadataFieldRequirements["C1"];
  assertEquals(rustC1?.required, goC1?.required, "C1.required should match");

  const goC3 = goRules.metadataFieldRequirements["C3"];
  const rustC3 = rustRules.metadataFieldRequirements["C3"];
  assertEquals(rustC3?.required, goC3?.required, "C3.required should match");

  // Same number of sections
  const goPragmaKeys = Object.keys(goRules.pragmaFieldRequirements).sort();
  const rustPragmaKeys = Object.keys(rustRules.pragmaFieldRequirements).sort();
  assertEquals(rustPragmaKeys, goPragmaKeys, "Same pragma section keys");

  const goMetaKeys = Object.keys(goRules.metadataFieldRequirements).sort();
  const rustMetaKeys = Object.keys(rustRules.metadataFieldRequirements).sort();
  assertEquals(rustMetaKeys, goMetaKeys, "Same metadata section keys");
});

// ---------------------------------------------------------------------------
// Go schema — doc comment expectations
// ---------------------------------------------------------------------------

Deno.test("code-schema/go: loads doc comment expectations", async () => {
  const rules = await loadCodeRules("go");

  assert(rules.docCommentExpectations !== undefined, "Should have docCommentExpectations");
  assertGreater(
    Object.keys(rules.docCommentExpectations).length,
    0,
    "Should have at least one doc comment entry",
  );

  // Go has package_doc, pragma_doc, metadata_doc
  const packageDoc = rules.docCommentExpectations["package_doc"];
  assert(packageDoc !== undefined, "Should have package_doc");
  assertEquals(packageDoc.severity, "warn");
  assert(
    packageDoc.purpose.length > 0,
    "package_doc should have a purpose",
  );

  const pragmaDoc = rules.docCommentExpectations["pragma_doc"];
  assert(pragmaDoc !== undefined, "Should have pragma_doc");
  assertEquals(pragmaDoc.severity, "warn");
});

// ---------------------------------------------------------------------------
// Rust schema — doc comment expectations (different structure)
// ---------------------------------------------------------------------------

Deno.test("code-schema/rust: loads doc comment expectations", async () => {
  const rules = await loadCodeRules("rust");

  assert(rules.docCommentExpectations !== undefined, "Should have docCommentExpectations");

  // Rust has a flat doc_comments structure — no named sub-entries
  // The extractor should still produce entries if the schema has object-typed values
  // (Rust's doc_comments has only string values like "purpose", "location",
  //  "crate_root_severity" — no sub-objects to iterate)
  // This is expected: Rust's doc comments are described at the top level,
  // not as named entries like Go's.
});

// ---------------------------------------------------------------------------
// Go schema — subtype definitions
// ---------------------------------------------------------------------------

Deno.test("code-schema/go: loads subtype definitions (library, executable, demo-test)", async () => {
  const rules = await loadCodeRules("go");

  assert(rules.subtypeDefinitions !== undefined, "Should have subtypeDefinitions");
  assertEquals(
    Object.keys(rules.subtypeDefinitions).sort(),
    ["demo-test", "executable", "library"],
    "Should have all 3 subtypes with normalized names",
  );

  // Library
  const lib = rules.subtypeDefinitions["library"];
  assert(lib !== undefined, "Should have library subtype");
  assertEquals(lib.subtypeValue, "library");
  assertEquals(lib.pragmaDirective, "//omni:code --go -library");
  assertEquals(lib.hasPublicApi, true);

  // Executable
  const exe = rules.subtypeDefinitions["executable"];
  assert(exe !== undefined, "Should have executable subtype");
  assertEquals(exe.subtypeValue, "executable");
  assertEquals(exe.hasPublicApi, false);
  assertEquals(exe.filePattern, "main.go");

  // Demo-test (normalized from demo_test)
  const dt = rules.subtypeDefinitions["demo-test"];
  assert(dt !== undefined, "Should have demo-test subtype");
  assertEquals(dt.subtypeValue, "demo-test");
  assert(dt.filePattern.includes("_test.go"), "demo-test should match test files");
});

// ---------------------------------------------------------------------------
// Rust schema — subtype definitions (with has_tests_block)
// ---------------------------------------------------------------------------

Deno.test("code-schema/rust: loads subtype definitions with Rust-specific fields", async () => {
  clearCodeCache("rust");
  const rules = await loadCodeRules("rust");

  assertEquals(
    Object.keys(rules.subtypeDefinitions).sort(),
    ["bare-bone", "demo-test", "executable", "library", "module"],
    "Rust should have 5 subtypes (library, executable, module, demo-test, bare-bone)",
  );

  // Library — Rust has has_tests_block
  const lib = rules.subtypeDefinitions["library"];
  assert(lib !== undefined, "Should have library");
  assertEquals(lib.pragmaDirective, "//omni:code --rust -library");
  assertEquals(lib.hasTestsBlock, true, "Rust library has tests block");
  assertEquals(lib.filePattern, "lib.rs");

  // Executable — no tests block
  const exe = rules.subtypeDefinitions["executable"];
  assert(exe !== undefined, "Should have executable");
  assertEquals(exe.filePattern, "main.rs");

  // Module — internal crate file
  const mod_ = rules.subtypeDefinitions["module"];
  assert(mod_ !== undefined, "Should have module");
  assertEquals(mod_.pragmaDirective, "//omni:code --rust -module");
  assertEquals(mod_.hasTestsBlock, true, "Rust module has tests block");

  // Demo-test — has tests block
  const dt = rules.subtypeDefinitions["demo-test"];
  assert(dt !== undefined, "Should have demo-test");

  // Bare-bone — foundation template
  const bb = rules.subtypeDefinitions["bare-bone"];
  assert(bb !== undefined, "Should have bare-bone");
  assertEquals(bb.purpose.includes("Foundation"), true, "Bare-bone purpose mentions foundation");
  assertEquals(dt.hasTestsBlock, true, "Rust demo-test has tests block");
});

// ---------------------------------------------------------------------------
// Caching — METADATA fields survive cache
// ---------------------------------------------------------------------------

Deno.test("code-schema/cache: METADATA fields persist across cached calls", async () => {
  clearCodeCache("go");

  // First call — loads from disk
  const first = await loadCodeRules("go");
  const i1First = first.pragmaFieldRequirements["I1"];

  // Second call — from cache
  const second = await loadCodeRules("go");
  const i1Second = second.pragmaFieldRequirements["I1"];

  assertEquals(i1Second?.required, i1First?.required, "Cached I1.required should match");
  assertEquals(
    Object.keys(second.subtypeDefinitions).length,
    Object.keys(first.subtypeDefinitions).length,
    "Cached subtypes count should match",
  );
});

// ---------------------------------------------------------------------------
// Completeness — all expected sections loaded
// ---------------------------------------------------------------------------

Deno.test("code-schema/completeness: Go has 4 pragma + 7 metadata sections", async () => {
  const rules = await loadCodeRules("go");

  assertEquals(
    Object.keys(rules.pragmaFieldRequirements).sort(),
    ["I1", "I2", "I3", "I4"],
    "Should have exactly I1-I4",
  );

  assertEquals(
    Object.keys(rules.metadataFieldRequirements).sort(),
    ["C1", "C2", "C3", "C4", "C5", "C6", "C7"],
    "Should have exactly C1-C7",
  );
});

// ---------------------------------------------------------------------------
// Go schema — content kind placement maps (schema-driven)
// ---------------------------------------------------------------------------

Deno.test("code-schema/go: loads content kind placement maps from schema", async () => {
  clearCodeCache("go");
  const rules = await loadCodeRules("go");

  const pm = rules.placementMaps;
  assert(pm !== undefined, "Should have placementMaps");

  // Block placement — Go content kinds to "SETUP" or "BODY"
  assertEquals(pm.blockPlacement["import_decl"], "SETUP");
  assertEquals(pm.blockPlacement["const_decl"], "SETUP");
  assertEquals(pm.blockPlacement["var_decl"], "SETUP");
  assertEquals(pm.blockPlacement["type_decl"], "SETUP");
  assertEquals(pm.blockPlacement["func_decl"], "BODY");
  assertEquals(pm.blockPlacement["method_decl"], "BODY");
  assertEquals(pm.blockPlacement["init_func"], "BODY");

  // Missing kinds should be undefined (can appear anywhere)
  assertEquals(pm.blockPlacement["comment"], undefined, "comment has no placement");
  assertEquals(pm.blockPlacement["blank"], undefined, "blank has no placement");
  assertEquals(pm.blockPlacement["package_decl"], undefined, "package_decl has no placement");
});

Deno.test("code-schema/go: loads subsection placement maps from schema", async () => {
  const rules = await loadCodeRules("go");
  const pm = rules.placementMaps;

  // Subsection placement — Go content kinds to subsection tags
  assertEquals(pm.subsectionPlacement["import_decl"], "Imports");
  assertEquals(pm.subsectionPlacement["const_decl"], "Constants");
  assertEquals(pm.subsectionPlacement["var_decl"], "Variables");
  assertEquals(pm.subsectionPlacement["type_decl"], "CoreTypes");
  assertEquals(pm.subsectionPlacement["method_decl"], "TypeMethods");
});

Deno.test("code-schema/go: loads metadata forbidden kinds from schema", async () => {
  const rules = await loadCodeRules("go");
  const pm = rules.placementMaps;

  // Metadata forbidden — these should never appear in METADATA
  assert(pm.metadataForbidden.includes("import_decl"), "import_decl should be forbidden");
  assert(pm.metadataForbidden.includes("func_decl"), "func_decl should be forbidden");
  assert(pm.metadataForbidden.includes("init_func"), "init_func should be forbidden");
  assertEquals(pm.metadataForbidden.length, 7, "Go has 7 forbidden kinds");
});

// ---------------------------------------------------------------------------
// Rust schema — content kind placement maps (schema-driven)
// ---------------------------------------------------------------------------

Deno.test("code-schema/rust: loads content kind placement maps from schema", async () => {
  clearCodeCache("rust");
  const rules = await loadCodeRules("rust");

  const pm = rules.placementMaps;
  assert(pm !== undefined, "Should have placementMaps");

  // Block placement — Rust content kinds
  assertEquals(pm.blockPlacement["use_decl"], "SETUP");
  assertEquals(pm.blockPlacement["reexport_decl"], "SETUP");
  assertEquals(pm.blockPlacement["mod_decl"], "SETUP");
  assertEquals(pm.blockPlacement["struct_decl"], "SETUP");
  assertEquals(pm.blockPlacement["enum_decl"], "SETUP");
  assertEquals(pm.blockPlacement["trait_decl"], "SETUP");
  assertEquals(pm.blockPlacement["macro_decl"], "SETUP");
  assertEquals(pm.blockPlacement["fn_decl"], "BODY");
  assertEquals(pm.blockPlacement["impl_block"], "BODY");

  // Missing kinds
  assertEquals(pm.blockPlacement["comment"], undefined, "comment has no placement");
  assertEquals(pm.blockPlacement["attr"], undefined, "attr has no placement");
});

Deno.test("code-schema/rust: loads subsection placement maps from schema", async () => {
  const rules = await loadCodeRules("rust");
  const pm = rules.placementMaps;

  // Subsection placement — Rust-specific mappings
  assertEquals(pm.subsectionPlacement["use_decl"], "Imports");
  assertEquals(pm.subsectionPlacement["reexport_decl"], "Modules");
  assertEquals(pm.subsectionPlacement["mod_decl"], "Modules");
  assertEquals(pm.subsectionPlacement["const_decl"], "Constants");
  assertEquals(pm.subsectionPlacement["static_decl"], "Statics");
  assertEquals(pm.subsectionPlacement["type_alias"], "TypeAliases");
  assertEquals(pm.subsectionPlacement["struct_decl"], "CoreTypes");
  assertEquals(pm.subsectionPlacement["enum_decl"], "CoreTypes");
  assertEquals(pm.subsectionPlacement["trait_decl"], "TraitDefs");
  assertEquals(pm.subsectionPlacement["macro_decl"], "Macros");
});

Deno.test("code-schema/rust: loads metadata forbidden kinds from schema", async () => {
  const rules = await loadCodeRules("rust");
  const pm = rules.placementMaps;

  assert(pm.metadataForbidden.includes("use_decl"), "use_decl should be forbidden");
  assert(pm.metadataForbidden.includes("fn_decl"), "fn_decl should be forbidden");
  assert(pm.metadataForbidden.includes("impl_block"), "impl_block should be forbidden");
  assertEquals(pm.metadataForbidden.length, 12, "Rust has 12 forbidden kinds");
});

// ---------------------------------------------------------------------------
// Placement maps — schema produces same values as previously hardcoded
// ---------------------------------------------------------------------------

Deno.test("code-schema/placement: Go placement maps have expected entry counts", async () => {
  const rules = await loadCodeRules("go");
  const pm = rules.placementMaps;

  assertEquals(
    Object.keys(pm.blockPlacement).length,
    7,
    "Go block placement: 4 SETUP + 3 BODY kinds",
  );
  assertEquals(
    Object.keys(pm.subsectionPlacement).length,
    5,
    "Go subsection placement: 5 SETUP subsection mappings",
  );
});

Deno.test("code-schema/placement: Rust placement maps have expected entry counts", async () => {
  const rules = await loadCodeRules("rust");
  const pm = rules.placementMaps;

  assertEquals(
    Object.keys(pm.blockPlacement).length,
    12,
    "Rust block placement: 10 SETUP + 2 BODY kinds",
  );
  assertEquals(
    Object.keys(pm.subsectionPlacement).length,
    10,
    "Rust subsection placement: 10 SETUP subsection mappings",
  );
});

// ---------------------------------------------------------------------------
// Schema structural equivalence — SETUP/BODY consistency across Go and Rust
// ---------------------------------------------------------------------------

Deno.test("code-schema/equivalence: Go subtypes are subset of Rust emphasis keys", async () => {
  const goRules = await loadCodeRules("go");
  const rustRules = await loadCodeRules("rust");

  // Go has 3 subtypes (library, executable, demo-test); Rust has 5 (+ module, bare-bone).
  // Every Go subtype should exist in Rust emphasis (shared foundation).
  const goSetupKeys = Object.keys(goRules.subtypeEmphasis.setup).sort();
  const rustSetupKeys = Object.keys(rustRules.subtypeEmphasis.setup).sort();
  for (const k of goSetupKeys) {
    assert(rustSetupKeys.includes(k), `Rust SETUP emphasis should include Go subtype "${k}"`);
  }

  const goBodyKeys = Object.keys(goRules.subtypeEmphasis.body).sort();
  const rustBodyKeys = Object.keys(rustRules.subtypeEmphasis.body).sort();
  for (const k of goBodyKeys) {
    assert(rustBodyKeys.includes(k), `Rust BODY emphasis should include Go subtype "${k}"`);
  }

  // Rust should have additional subtypes beyond Go
  assertGreater(rustSetupKeys.length, goSetupKeys.length, "Rust has more SETUP subtypes than Go");
  assertGreater(rustBodyKeys.length, goBodyKeys.length, "Rust has more BODY subtypes than Go");
});

Deno.test("code-schema/equivalence: emphasis subtypes have heavy and light arrays", async () => {
  const goRules = await loadCodeRules("go");
  const rustRules = await loadCodeRules("rust");

  for (const subtype of ["library", "executable", "demo-test"]) {
    const goSetup = goRules.subtypeEmphasis.setup[subtype];
    assert(goSetup !== undefined, `Go SETUP emphasis missing ${subtype}`);
    assertGreater(goSetup.heavy.length, 0, `Go ${subtype} SETUP should have heavy subs`);
    assertGreater(goSetup.light.length, 0, `Go ${subtype} SETUP should have light subs`);

    const rustSetup = rustRules.subtypeEmphasis.setup[subtype];
    assert(rustSetup !== undefined, `Rust SETUP emphasis missing ${subtype}`);
    assertGreater(rustSetup.heavy.length, 0, `Rust ${subtype} SETUP should have heavy subs`);
    assertGreater(rustSetup.light.length, 0, `Rust ${subtype} SETUP should have light subs`);
  }
});

Deno.test("code-schema/equivalence: Go and Rust setupData have requiredIn fields", async () => {
  const goRules = await loadCodeRules("go");
  const rustRules = await loadCodeRules("rust");

  // Both should have at least one subsection with requiredIn = ["library"]
  const goHasRequired = goRules.setupData.some((s) => s.requiredIn.includes("library"));
  const rustHasRequired = rustRules.setupData.some((s) => s.requiredIn.includes("library"));
  assert(goHasRequired, "Go setupData should have library-required subsections");
  assert(rustHasRequired, "Rust setupData should have library-required subsections");

  // S1 Imports should be required in all subtypes for both
  const goImports = goRules.setupData.find((s) => s.tag === "Imports");
  const rustImports = rustRules.setupData.find((s) => s.tag === "Imports");
  assert(goImports !== undefined, "Go should have Imports subsection");
  assert(rustImports !== undefined, "Rust should have Imports subsection");
  assertEquals(goImports!.requiredIn.sort(), ["demo-test", "executable", "library"], "Go Imports required in all");
  assertEquals(rustImports!.requiredIn.sort(), ["demo-test", "executable", "library"], "Rust Imports required in all");
});

Deno.test("code-schema/equivalence: Go bodyData subtypes are subset of Rust", async () => {
  const goRules = await loadCodeRules("go");
  const rustRules = await loadCodeRules("rust");

  const goBodyKeys = Object.keys(goRules.bodyData).sort();
  const rustBodyKeys = Object.keys(rustRules.bodyData).sort();

  // Go subtypes should all exist in Rust (shared foundation)
  for (const k of goBodyKeys) {
    assert(rustBodyKeys.includes(k), `Rust bodyData should include Go subtype "${k}"`);
  }

  // Rust should have additional subtypes (module, bare-bone)
  assertGreater(rustBodyKeys.length, goBodyKeys.length, "Rust has more body subtypes than Go");

  // Each subtype should have subsections
  for (const key of goBodyKeys) {
    assertGreater(goRules.bodyData[key]!.subsections.length, 0, `Go ${key} should have BODY subsections`);
  }
  for (const key of rustBodyKeys) {
    assertGreater(rustRules.bodyData[key]!.subsections.length, 0, `Rust ${key} should have BODY subsections`);
  }
});

// ---------------------------------------------------------------------------
// Fill content — Go schema
// ---------------------------------------------------------------------------

Deno.test("code-schema/go: fill content loads all sections", async () => {
  clearCodeCache("go");
  const rules = await loadCodeRules("go");

  assert(rules.fillContent !== undefined, "Go schema should have fillContent");
  const fc = rules.fillContent!;

  // Defaults
  assertEquals(fc.defaults.version, "a-01.00");
  assertEquals(fc.defaults.organization, "CreativeWorkzStudio LLC");
  assertEquals(fc.defaults.scripture, "Genesis 1:1");
  assertEquals(fc.defaults.structure, "4-block");

  // Directives
  assertEquals(fc.directives.length, 3, "Should have 3 directives");
  assert(fc.directives[0]!.includes("//omni:key"), "First directive is key");
  assert(fc.directives[1]!.includes("//omni:code"), "Second directive is code");
  assert(fc.directives[2]!.includes("//omni:version"), "Third directive is version");

  // Go-specific: package declaration
  assert(fc.packageDeclaration !== undefined, "Go should have packageDeclaration");
  assert(fc.packageDeclaration!.includes("package"), "Should reference package");

  // Go-specific: no module doc
  assertEquals(fc.moduleDoc, undefined, "Go should not have moduleDoc");
});

Deno.test("code-schema/go: fill content identity syntax", async () => {
  clearCodeCache("go");
  const rules = await loadCodeRules("go");
  const fc = rules.fillContent!;

  // Go uses [][2]string syntax
  assert(fc.identitySyntax.pragma.declaration.includes("[][2]string"), "Go Pragma uses [][2]string");
  assert(fc.identitySyntax.metadata.declaration.includes("[][2]string"), "Go Metadata uses [][2]string");
  assert(fc.identitySyntax.pragma.close === "}", "Go Pragma closes with }");
});

Deno.test("code-schema/go: fill content pragma/metadata entries", async () => {
  clearCodeCache("go");
  const rules = await loadCodeRules("go");
  const fc = rules.fillContent!;

  // Pragma entries — should have I1, I2, I3 fields
  assertGreater(fc.pragmaEntries.length, 0, "Should have pragma entries");
  const pragmaKeys = fc.pragmaEntries.map(([k]) => k);
  assert(pragmaKeys.includes("I1.key"), "Should have I1.key");
  assert(pragmaKeys.includes("I1.format"), "Should have I1.format");
  assert(pragmaKeys.includes("I2.type"), "Should have I2.type");
  assert(pragmaKeys.includes("I3.file"), "Should have I3.file");

  // Check I1.format is "go" (not a placeholder)
  const formatEntry = fc.pragmaEntries.find(([k]) => k === "I1.format");
  assertEquals(formatEntry![1], "go", "I1.format should be 'go'");

  // Metadata entries — should have C1-C4 fields
  assertGreater(fc.metadataEntries.length, 0, "Should have metadata entries");
  const metaKeys = fc.metadataEntries.map(([k]) => k);
  assert(metaKeys.includes("C1.version"), "Should have C1.version");
  assert(metaKeys.includes("C2.organization"), "Should have C2.organization");
  assert(metaKeys.includes("C3.scripture"), "Should have C3.scripture");
});

Deno.test("code-schema/go: fill content closing defaults", async () => {
  clearCodeCache("go");
  const rules = await loadCodeRules("go");
  const fc = rules.fillContent!;

  assert(fc.closingDefaults["X1"] !== undefined, "Should have X1 defaults");
  assert(fc.closingDefaults["X5"] !== undefined, "Should have X5 defaults");

  const x1 = fc.closingDefaults["X1"]!;
  assert(x1["never"] !== undefined, "X1 should have 'never' default");
  assert(x1["careful"] !== undefined, "X1 should have 'careful' default");
  assert(x1["safe"] !== undefined, "X1 should have 'safe' default");

  const x5 = fc.closingDefaults["X5"]!;
  assert(x5["note"] !== undefined, "X5 should have 'note' default");
  assert(x5["scripture"] !== undefined, "X5 should have 'scripture' default");
});

// ---------------------------------------------------------------------------
// Fill content — Rust schema
// ---------------------------------------------------------------------------

Deno.test("code-schema/rust: fill content loads all sections", async () => {
  clearCodeCache("rust");
  const rules = await loadCodeRules("rust");

  assert(rules.fillContent !== undefined, "Rust schema should have fillContent");
  const fc = rules.fillContent!;

  // Defaults
  assertEquals(fc.defaults.version, "a-01.00");
  assertEquals(fc.defaults.organization, "CreativeWorkzStudio LLC");

  // Directives
  assertEquals(fc.directives.length, 3, "Should have 3 directives");
  assert(fc.directives[1]!.includes("--rust"), "Second directive should reference rust");

  // Rust-specific: module doc, no package declaration
  assert(fc.moduleDoc !== undefined, "Rust should have moduleDoc");
  assert(fc.moduleDoc!.includes("//!"), "Rust moduleDoc should use //! syntax");
  assertEquals(fc.packageDeclaration, undefined, "Rust should not have packageDeclaration");
});

Deno.test("code-schema/rust: fill content identity syntax", async () => {
  clearCodeCache("rust");
  const rules = await loadCodeRules("rust");
  const fc = rules.fillContent!;

  // Rust uses &[(&str, &str)] syntax
  assert(fc.identitySyntax.pragma.declaration.includes("PRAGMA"), "Rust Pragma uses PRAGMA name");
  assert(fc.identitySyntax.metadata.declaration.includes("METADATA"), "Rust Metadata uses METADATA name");
  assert(fc.identitySyntax.pragma.close === "];", "Rust Pragma closes with ];");

  // Check I1.format is "rust"
  const formatEntry = fc.pragmaEntries.find(([k]) => k === "I1.format");
  assertEquals(formatEntry![1], "rust", "I1.format should be 'rust'");
});

// ---------------------------------------------------------------------------
// Fill content — Go/Rust parity
// ---------------------------------------------------------------------------

Deno.test("code-schema/parity: Go and Rust fill content have same placeholder keys", async () => {
  clearCodeCache("go");
  clearCodeCache("rust");
  const goRules = await loadCodeRules("go");
  const rustRules = await loadCodeRules("rust");
  const goFc = goRules.fillContent!;
  const rustFc = rustRules.fillContent!;

  // Same number of directives
  assertEquals(goFc.directives.length, rustFc.directives.length,
    "Go and Rust should have same number of directives");

  // Same pragma field keys (except format value)
  const goPragmaKeys = goFc.pragmaEntries.map(([k]) => k);
  const rustPragmaKeys = rustFc.pragmaEntries.map(([k]) => k);
  assertEquals(goPragmaKeys, rustPragmaKeys, "Go and Rust pragma entry keys should match");

  // Same metadata field keys
  const goMetaKeys = goFc.metadataEntries.map(([k]) => k);
  const rustMetaKeys = rustFc.metadataEntries.map(([k]) => k);
  assertEquals(goMetaKeys, rustMetaKeys, "Go and Rust metadata entry keys should match");

  // Same closing default zones
  const goClosingTags = Object.keys(goFc.closingDefaults).sort();
  const rustClosingTags = Object.keys(rustFc.closingDefaults).sort();
  assertEquals(goClosingTags, rustClosingTags, "Go and Rust closing default tags should match");
});

// ---------------------------------------------------------------------------
// Fill content — identity groups
// ---------------------------------------------------------------------------

Deno.test("code-schema/rust: fill content has identity groups", async () => {
  clearCodeCache("rust");
  const rules = await loadCodeRules("rust");
  const fc = rules.fillContent!;

  assert(fc.identityGroups !== undefined, "Rust should have identityGroups");
  const ig = fc.identityGroups!;

  // Pragma groups: I1-I4
  assertEquals(ig.pragma.length, 4, "Should have 4 pragma groups (I1-I4)");
  assertEquals(ig.pragma[0]!.range, "I1");
  assertEquals(ig.pragma[0]!.label, "Core");
  assert(ig.pragma[0]!.docstring !== undefined, "I1 should have a docstring");

  // Metadata groups: C1-C7
  assertEquals(ig.metadata.length, 7, "Should have 7 metadata groups (C1-C7)");
  assertEquals(ig.metadata[0]!.range, "C1");
  assertEquals(ig.metadata[0]!.label, "State");

  // Section headers
  assert(ig.sectionHeaders.pragma.includes("I1-I4"), "Pragma header should reference I1-I4");
  assert(ig.sectionHeaders.metadata.includes("C1-C7"), "Metadata header should reference C1-C7");
});

Deno.test("code-schema/parity: Go and Rust identity groups match", async () => {
  clearCodeCache("go");
  clearCodeCache("rust");
  const goRules = await loadCodeRules("go");
  const rustRules = await loadCodeRules("rust");
  const goIg = goRules.fillContent!.identityGroups!;
  const rustIg = rustRules.fillContent!.identityGroups!;

  // Same group ranges
  const goPragmaRanges = goIg.pragma.map(g => g.range);
  const rustPragmaRanges = rustIg.pragma.map(g => g.range);
  assertEquals(goPragmaRanges, rustPragmaRanges, "Pragma group ranges should match");

  const goMetaRanges = goIg.metadata.map(g => g.range);
  const rustMetaRanges = rustIg.metadata.map(g => g.range);
  assertEquals(goMetaRanges, rustMetaRanges, "Metadata group ranges should match");
});

// ---------------------------------------------------------------------------
// Fill content — transformer modes
// ---------------------------------------------------------------------------

Deno.test("code-schema/rust: fill content has transformer modes", async () => {
  clearCodeCache("rust");
  const rules = await loadCodeRules("rust");
  const fc = rules.fillContent!;

  assert(fc.transformerModes !== undefined, "Rust should have transformerModes");
  const tm = fc.transformerModes!;

  // Strict mode — full production-grade
  assertEquals(tm.strict.linterTarget, "0E 0W 0I");
  assertEquals(tm.strict.identityFormatting, true);
  assertEquals(tm.strict.sectionHeaders, true);
  assertEquals(tm.strict.groupComments, true);
  assertEquals(tm.strict.columnAlignment, true);

  // Balance mode — correct but not polished
  assertEquals(tm.balance.linterTarget, "0E 0W");
  assertEquals(tm.balance.identityFormatting, true);
  assertEquals(tm.balance.sectionHeaders, false);

  // Growth mode — minimal scaffolding
  assertEquals(tm.growth.linterTarget, "0E");
  assertEquals(tm.growth.identityFormatting, false);
  assertEquals(tm.growth.blockOverviews, false);
});

Deno.test("code-schema/rust: fill content extra defaults captured", async () => {
  clearCodeCache("rust");
  const rules = await loadCodeRules("rust");
  const fc = rules.fillContent!;

  // Extra defaults from schema enrichment
  assertEquals(fc.defaults["architect"], "Seanje Lenox-Wise");
  assertEquals(fc.defaults["implementation"], "Nova Dawn (CPI-SI)");
  assertEquals(fc.defaults["domain"], "bereshit");
  assertEquals(fc.defaults["paradigm"], "CPI-SI");
});

// ============================================================================
// CLOSING
// ============================================================================
//
// These tests verify METADATA extraction, content kind placement maps,
// CLOSING containerization, and fill content enrichment from Go and Rust
// schemas. The I/C standard is format-agnostic — both schemas define the
// same sections. What differs is the carrier syntax and format-specific
// features.
//
// "Prove all things; hold fast that which is good." — 1 Thessalonians 5:21
// ============================================================================
