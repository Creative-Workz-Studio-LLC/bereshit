// R-level: R10 — Format-level tests (single-format, single-concern)
// Migrated from: tests/shared/go_chunker_test.ts
// ============================================================================
// METADATA
// ============================================================================
//
// file:    tests/T00/R10/shared/go_chunker_test.ts
// key:     B-tov-cws-struct-tests-shared-go-chunker
// title:   CWS Struct — Go Chunking & Scaffold Adapter Tests
// type:    Code (Test)
// version: a-01.00
// created: 2026-02-22
// authors: Nova Dawn (CPI-SI)
// purpose: Verify the Go-specific chunking configuration and scaffold adapter.
//          Tests the classifier (classifyGoChunkLine), chunker config (goChunkerConfig),
//          block/section mapping functions, metadata extraction, and the full
//          ScaffoldAdapter interface. Exercises the singleLineIfNoDelimiter flag
//          that prevents Go single-line declarations from consuming next lines.
//
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================

import { assertEquals, assert } from "jsr:@std/assert";
import {
  classifyGoChunkLine,
  parseGoCodeChunks,
  goChunkToBlock,
  goChunkToSetupSection,
  goChunkToBodySection,
  extractGoMetadataContext,
  buildGoAdapter,
} from "@lib/handlers/go.ts";
import type { GenericChunk } from "@lib/shared/scaffold.ts";

// ---------------------------------------------------------------------------
// Test helpers
// ---------------------------------------------------------------------------

/** Find chunks by category. */
function findChunks(chunks: GenericChunk[], cat: string): GenericChunk[] {
  return chunks.filter((c) => c.category === cat);
}

/** Total line count across all chunks. */
function totalLines(chunks: GenericChunk[]): number {
  return chunks.reduce((sum, c) => sum + c.lines.length, 0);
}

// ============================================================================
// BODY
// ============================================================================

// ---------------------------------------------------------------------------
// G1: classifyGoChunkLine — line classification
// ---------------------------------------------------------------------------

Deno.test("[CWS-T00-101] go-chunker/G1: classifies blank lines", () => {
  assertEquals(classifyGoChunkLine(""), "blank");
});

Deno.test("go-chunker/G1: classifies comments", () => {
  assertEquals(classifyGoChunkLine("// hello world"), "comment");
  assertEquals(classifyGoChunkLine("//omni:key B-test"), "comment");
});

Deno.test("go-chunker/G1: classifies package declaration", () => {
  assertEquals(classifyGoChunkLine("package main"), "package_decl");
  assertEquals(classifyGoChunkLine("package mylib"), "package_decl");
});

Deno.test("go-chunker/G1: classifies single-line import vs import block", () => {
  assertEquals(classifyGoChunkLine('import "fmt"'), "import_decl");
  assertEquals(classifyGoChunkLine('import "os/exec"'), "import_decl");
  assertEquals(classifyGoChunkLine("import ("), "import_block");
  assertEquals(classifyGoChunkLine("import  ("), "import_block");
});

Deno.test("go-chunker/G1: classifies function declarations", () => {
  assertEquals(classifyGoChunkLine("func main() {"), "func_decl");
  assertEquals(classifyGoChunkLine("func doWork(x int) error {"), "func_decl");
  assertEquals(classifyGoChunkLine("func init() {"), "init_func");
});

Deno.test("go-chunker/G1: classifies method declarations", () => {
  assertEquals(classifyGoChunkLine("func (s *Server) Start() error {"), "method_decl");
  assertEquals(classifyGoChunkLine("func (r Reader) Read(p []byte) (int, error) {"), "method_decl");
});

Deno.test("go-chunker/G1: classifies type block vs alias", () => {
  assertEquals(classifyGoChunkLine("type Foo struct {"), "type_block");
  assertEquals(classifyGoChunkLine("type Bar interface {"), "type_block");
  assertEquals(classifyGoChunkLine("type ("), "type_block");
  assertEquals(classifyGoChunkLine("type MyInt int"), "type_alias");
  assertEquals(classifyGoChunkLine("type Handler func(w http.ResponseWriter, r *http.Request)"), "type_alias");
});

Deno.test("go-chunker/G1: classifies const block vs single", () => {
  assertEquals(classifyGoChunkLine("const ("), "const_block");
  assertEquals(classifyGoChunkLine("const MaxRetries = 3"), "const_decl");
  assertEquals(classifyGoChunkLine("const Version = \"1.0.0\""), "const_decl");
});

Deno.test("go-chunker/G1: classifies var block vs single", () => {
  assertEquals(classifyGoChunkLine("var ("), "var_block");
  assertEquals(classifyGoChunkLine("var logger *Logger"), "var_decl");
  assertEquals(classifyGoChunkLine("var ErrNotFound = errors.New(\"not found\")"), "var_decl");
});

Deno.test("go-chunker/G1: classifies other lines", () => {
  assertEquals(classifyGoChunkLine("x := 42"), "other");
  assertEquals(classifyGoChunkLine("return nil"), "other");
  assertEquals(classifyGoChunkLine("}"), "other");
});

// ---------------------------------------------------------------------------
// G2: parseGoCodeChunks — basic chunking
// ---------------------------------------------------------------------------

Deno.test("go-chunker/G2: empty input produces no chunks", () => {
  const chunks = parseGoCodeChunks([]);
  assertEquals(chunks.length, 0);
});

Deno.test("go-chunker/G2: single package line produces one chunk", () => {
  const chunks = parseGoCodeChunks(["package main"]);
  assertEquals(chunks.length, 1);
  assertEquals(chunks[0]!.category, "package");
});

Deno.test("go-chunker/G2: pragma directives grouped into single chunk", () => {
  const lines = [
    "// #!omni code --go -library",
    "//omni:key B-test-001",
    "//omni:version a-01.00",
  ];
  const chunks = parseGoCodeChunks(lines);
  const pragmas = findChunks(chunks, "pragma");
  assertEquals(pragmas.length, 1);
  assertEquals(pragmas[0]!.lines.length, 3);
});

Deno.test("go-chunker/G2: comments grouped together", () => {
  const lines = [
    "// First line",
    "// Second line",
    "// Third line",
  ];
  const chunks = parseGoCodeChunks(lines);
  assertEquals(chunks.length, 1);
  assertEquals(chunks[0]!.category, "comment");
  assertEquals(chunks[0]!.lines.length, 3);
});

// ---------------------------------------------------------------------------
// G3: singleLineIfNoDelimiter — Go's key config flag
// ---------------------------------------------------------------------------

Deno.test("go-chunker/G3: single-line const does NOT consume next line", () => {
  const lines = [
    "const MaxRetries = 3",
    "const Timeout = 30",
  ];
  const chunks = parseGoCodeChunks(lines);
  // Each const should be its own chunk — singleLineIfNoDelimiter prevents
  // the balanced capture from absorbing the next line.
  assertEquals(chunks.length, 2);
  assertEquals(chunks[0]!.lines.length, 1);
  assertEquals(chunks[1]!.lines.length, 1);
  assertEquals(chunks[0]!.category, "const");
  assertEquals(chunks[1]!.category, "const");
});

Deno.test("go-chunker/G3: single-line var does NOT consume next line", () => {
  const lines = [
    "var logger *Logger",
    "var config *Config",
  ];
  const chunks = parseGoCodeChunks(lines);
  assertEquals(chunks.length, 2);
  assertEquals(chunks[0]!.lines.length, 1);
  assertEquals(chunks[1]!.lines.length, 1);
});

Deno.test("go-chunker/G3: single-line type alias does NOT consume next line", () => {
  const lines = [
    "type MyInt int",
    "type Handler func()",
  ];
  const chunks = parseGoCodeChunks(lines);
  assertEquals(chunks.length, 2);
  assertEquals(chunks[0]!.category, "type_def");
  assertEquals(chunks[1]!.category, "type_def");
});

Deno.test("go-chunker/G3: const block with parens IS properly balanced", () => {
  const lines = [
    "const (",
    "\tMaxRetries = 3",
    "\tTimeout = 30",
    ")",
  ];
  const chunks = parseGoCodeChunks(lines);
  assertEquals(chunks.length, 1);
  assertEquals(chunks[0]!.category, "const");
  assertEquals(chunks[0]!.lines.length, 4);
});

Deno.test("go-chunker/G3: var block with parens IS properly balanced", () => {
  const lines = [
    "var (",
    "\tlogger *Logger",
    "\tconfig *Config",
    ")",
  ];
  const chunks = parseGoCodeChunks(lines);
  assertEquals(chunks.length, 1);
  assertEquals(chunks[0]!.category, "var");
  assertEquals(chunks[0]!.lines.length, 4);
});

// ---------------------------------------------------------------------------
// G4: Function chunking — brace-balanced capture
// ---------------------------------------------------------------------------

Deno.test("go-chunker/G4: simple function captured as single chunk", () => {
  const lines = [
    "func hello() {",
    '\tfmt.Println("hello")',
    "}",
  ];
  const chunks = parseGoCodeChunks(lines);
  assertEquals(chunks.length, 1);
  assertEquals(chunks[0]!.category, "fn_decl");
  assertEquals(chunks[0]!.lines.length, 3);
});

Deno.test("go-chunker/G4: nested braces captured correctly", () => {
  const lines = [
    "func complex() error {",
    "\tif x > 0 {",
    "\t\treturn nil",
    "\t}",
    "\treturn fmt.Errorf(\"fail\")",
    "}",
  ];
  const chunks = parseGoCodeChunks(lines);
  assertEquals(chunks.length, 1);
  assertEquals(chunks[0]!.category, "fn_decl");
  assertEquals(chunks[0]!.lines.length, 6);
});

Deno.test("go-chunker/G4: method declaration captured", () => {
  const lines = [
    "func (s *Server) Start() error {",
    "\treturn s.listen()",
    "}",
  ];
  const chunks = parseGoCodeChunks(lines);
  assertEquals(chunks.length, 1);
  assertEquals(chunks[0]!.category, "fn_decl");
  assertEquals(chunks[0]!.lines.length, 3);
});

Deno.test("go-chunker/G4: init function captured", () => {
  const lines = [
    "func init() {",
    "\tlog.SetFlags(0)",
    "}",
  ];
  const chunks = parseGoCodeChunks(lines);
  assertEquals(chunks.length, 1);
  assertEquals(chunks[0]!.category, "fn_decl");
  assertEquals(chunks[0]!.lines.length, 3);
});

Deno.test("go-chunker/G4: main function classified as main_fn", () => {
  const lines = [
    "func main() {",
    "\tfmt.Println(\"go\")",
    "}",
  ];
  const chunks = parseGoCodeChunks(lines);
  assertEquals(chunks.length, 1);
  assertEquals(chunks[0]!.category, "main_fn");
});

// ---------------------------------------------------------------------------
// G5: Type struct/interface — brace-balanced
// ---------------------------------------------------------------------------

Deno.test("go-chunker/G5: struct definition captured", () => {
  const lines = [
    "type Server struct {",
    "\tHost string",
    "\tPort int",
    "}",
  ];
  const chunks = parseGoCodeChunks(lines);
  assertEquals(chunks.length, 1);
  assertEquals(chunks[0]!.category, "type_def");
  assertEquals(chunks[0]!.lines.length, 4);
});

Deno.test("go-chunker/G5: interface definition captured", () => {
  const lines = [
    "type Reader interface {",
    "\tRead(p []byte) (int, error)",
    "}",
  ];
  const chunks = parseGoCodeChunks(lines);
  assertEquals(chunks.length, 1);
  assertEquals(chunks[0]!.category, "type_def");
  assertEquals(chunks[0]!.lines.length, 3);
});

// ---------------------------------------------------------------------------
// G6: Import grouping
// ---------------------------------------------------------------------------

Deno.test("go-chunker/G6: single-line imports grouped together", () => {
  const lines = [
    'import "fmt"',
    'import "os"',
    'import "strings"',
  ];
  const chunks = parseGoCodeChunks(lines);
  assertEquals(chunks.length, 1);
  assertEquals(chunks[0]!.category, "import");
  assertEquals(chunks[0]!.lines.length, 3);
});

Deno.test("go-chunker/G6: import block captured as single chunk", () => {
  const lines = [
    "import (",
    '\t"fmt"',
    '\t"os"',
    '\t"strings"',
    ")",
  ];
  const chunks = parseGoCodeChunks(lines);
  assertEquals(chunks.length, 1);
  assertEquals(chunks[0]!.category, "import");
  assertEquals(chunks[0]!.lines.length, 5);
});

Deno.test("go-chunker/G6: separator stops import grouping", () => {
  const lines = [
    'import "fmt"',
    "// ──────────────────────────────────────────",
    "const X = 1",
  ];
  const chunks = parseGoCodeChunks(lines);
  assert(chunks.length >= 3);
  // First chunk: import
  assertEquals(chunks[0]!.category, "import");
  assertEquals(chunks[0]!.lines.length, 1);
  // Second chunk: separator
  assertEquals(chunks[1]!.category, "separator");
});

// ---------------------------------------------------------------------------
// G7: goChunkToBlock — block mapping
// ---------------------------------------------------------------------------

Deno.test("go-chunker/G7: pragma maps to METADATA", () => {
  const chunk: GenericChunk = { category: "pragma", lines: ["// #!omni code --go"] };
  assertEquals(goChunkToBlock(chunk), "METADATA");
});

Deno.test("go-chunker/G7: package maps to METADATA", () => {
  const chunk: GenericChunk = { category: "package", lines: ["package main"] };
  assertEquals(goChunkToBlock(chunk), "METADATA");
});

Deno.test("go-chunker/G7: import maps to SETUP", () => {
  const chunk: GenericChunk = { category: "import", lines: ['import "fmt"'] };
  assertEquals(goChunkToBlock(chunk), "SETUP");
});

Deno.test("go-chunker/G7: const maps to SETUP", () => {
  const chunk: GenericChunk = { category: "const", lines: ["const X = 1"] };
  assertEquals(goChunkToBlock(chunk), "SETUP");
});

Deno.test("go-chunker/G7: var maps to SETUP", () => {
  const chunk: GenericChunk = { category: "var", lines: ["var x int"] };
  assertEquals(goChunkToBlock(chunk), "SETUP");
});

Deno.test("go-chunker/G7: type_def maps to SETUP", () => {
  const chunk: GenericChunk = { category: "type_def", lines: ["type Foo struct {"] };
  assertEquals(goChunkToBlock(chunk), "SETUP");
});

Deno.test("go-chunker/G7: fn_decl maps to BODY", () => {
  const chunk: GenericChunk = { category: "fn_decl", lines: ["func doWork() {"] };
  assertEquals(goChunkToBlock(chunk), "BODY");
});

Deno.test("go-chunker/G7: main_fn maps to CLOSING", () => {
  const chunk: GenericChunk = { category: "main_fn", lines: ["func main() {"] };
  assertEquals(goChunkToBlock(chunk), "CLOSING");
});

// ---------------------------------------------------------------------------
// G8: goChunkToSetupSection — section mapping
// ---------------------------------------------------------------------------

Deno.test("go-chunker/G8: import → Imports", () => {
  const chunk: GenericChunk = { category: "import", lines: [] };
  assertEquals(goChunkToSetupSection(chunk), "Imports");
});

Deno.test("go-chunker/G8: const → Constants", () => {
  const chunk: GenericChunk = { category: "const", lines: [] };
  assertEquals(goChunkToSetupSection(chunk), "Constants");
});

Deno.test("go-chunker/G8: var → Variables", () => {
  const chunk: GenericChunk = { category: "var", lines: [] };
  assertEquals(goChunkToSetupSection(chunk), "Variables");
});

Deno.test("go-chunker/G8: type_def → Types", () => {
  const chunk: GenericChunk = { category: "type_def", lines: [] };
  assertEquals(goChunkToSetupSection(chunk), "Types");
});

// ---------------------------------------------------------------------------
// G9: goChunkToBodySection — helpers vs core logic
// ---------------------------------------------------------------------------

Deno.test("go-chunker/G9: unexported function → Helpers", () => {
  const chunk: GenericChunk = {
    category: "fn_decl",
    lines: ["func parseConfig(path string) (*Config, error) {"],
  };
  assertEquals(goChunkToBodySection(chunk), "Helpers");
});

Deno.test("go-chunker/G9: exported function → CoreLogic", () => {
  const chunk: GenericChunk = {
    category: "fn_decl",
    lines: ["func ParseConfig(path string) (*Config, error) {"],
  };
  assertEquals(goChunkToBodySection(chunk), "CoreLogic");
});

// ---------------------------------------------------------------------------
// G10: extractGoMetadataContext — identity extraction
// ---------------------------------------------------------------------------

Deno.test("go-chunker/G10: extracts key from pragma", () => {
  const chunks: GenericChunk[] = [{
    category: "pragma",
    lines: [
      "// #!omni code --go -library",
      "//omni:key B-test-mylib",
      "//omni:version a-01.00",
    ],
  }];
  const ctx = extractGoMetadataContext(chunks, "/path/to/mylib.go", "library");
  assertEquals(ctx["key"], "B-test-mylib");
  assertEquals(ctx["version"], "a-01.00");
  assertEquals(ctx["filename"], "mylib.go");
  assertEquals(ctx["subtype"], "library");
});

Deno.test("go-chunker/G10: extracts package name", () => {
  const chunks: GenericChunk[] = [{
    category: "package",
    lines: ["package mylib"],
  }];
  const ctx = extractGoMetadataContext(chunks, "/path/to/mylib.go", undefined);
  assertEquals(ctx["package"], "mylib");
});

Deno.test("go-chunker/G10: extracts exported symbols from allLines", () => {
  const chunks: GenericChunk[] = [];
  const allLines = [
    "package mylib",
    "",
    "func PublicOne() {}",
    "func PublicTwo() {}",
    "func privateHelper() {}",
    "type Config struct {",
    "}",
  ];
  const ctx = extractGoMetadataContext(chunks, "/p/mylib.go", undefined, allLines);
  assert(ctx["provides"]!.includes("PublicOne"));
  assert(ctx["provides"]!.includes("PublicTwo"));
  assert(ctx["provides"]!.includes("Config"));
  assert(!ctx["provides"]!.includes("privateHelper"));
});

Deno.test("go-chunker/G10: derives component from filename", () => {
  const chunks: GenericChunk[] = [];
  const ctx1 = extractGoMetadataContext(chunks, "/p/main.go", undefined);
  assertEquals(ctx1["component"], "binary entry point");

  const ctx2 = extractGoMetadataContext(chunks, "/p/doc.go", undefined);
  assertEquals(ctx2["component"], "package documentation");

  const ctx3 = extractGoMetadataContext(chunks, "/p/my_handler.go", undefined);
  assertEquals(ctx3["component"], "my handler");
});

// ---------------------------------------------------------------------------
// G11: buildGoAdapter — ScaffoldAdapter interface
// ---------------------------------------------------------------------------

Deno.test("go-chunker/G11: adapter has correct format", () => {
  const adapter = buildGoAdapter();
  assertEquals(adapter.format, "go");
  assertEquals(adapter.fileExtension, ".go");
});

Deno.test("go-chunker/G11: adapter parses chunks", () => {
  const adapter = buildGoAdapter();
  const lines = [
    "package main",
    "",
    'import "fmt"',
    "",
    "func main() {",
    "}",
  ];
  const chunks = adapter.parseChunks(lines);
  assert(chunks.length > 0);
  assert(findChunks(chunks, "package").length === 1);
  assert(findChunks(chunks, "import").length === 1);
  assert(findChunks(chunks, "main_fn").length === 1);
});

Deno.test("go-chunker/G11: adapter detects test functions", () => {
  const adapter = buildGoAdapter();
  const testChunk: GenericChunk = {
    category: "fn_decl",
    lines: ["func TestSomething(t *testing.T) {", "\t// test body", "}"],
  };
  const regularChunk: GenericChunk = {
    category: "fn_decl",
    lines: ["func doWork() {", "}"],
  };
  assert(adapter.isTestChunk(testChunk));
  assert(!adapter.isTestChunk(regularChunk));
});

Deno.test("go-chunker/G11: adapter detects main function", () => {
  const adapter = buildGoAdapter();
  const mainChunk: GenericChunk = {
    category: "main_fn",
    lines: ["func main() {", "}"],
  };
  const otherChunk: GenericChunk = {
    category: "fn_decl",
    lines: ["func helper() {", "}"],
  };
  assert(adapter.isMainChunk(mainChunk));
  assert(!adapter.isMainChunk(otherChunk));
});

Deno.test("go-chunker/G11: adapter detects orphan chunks", () => {
  const adapter = buildGoAdapter();
  const sepChunk: GenericChunk = { category: "separator", lines: ["// ────────"] };
  const blankChunk: GenericChunk = { category: "blank", lines: [""] };
  const commentChunk: GenericChunk = { category: "comment", lines: ["// hi"] };
  const funcChunk: GenericChunk = { category: "fn_decl", lines: ["func foo() {"] };

  assert(adapter.isOrphanChunk(sepChunk));
  assert(adapter.isOrphanChunk(blankChunk));
  assert(adapter.isOrphanChunk(commentChunk));
  assert(!adapter.isOrphanChunk(funcChunk));
});

// ---------------------------------------------------------------------------
// G12: Line conservation — total lines in == total lines out
// ---------------------------------------------------------------------------

Deno.test("go-chunker/G12: line conservation on realistic Go file", () => {
  const lines = [
    "// #!omni code --go -library",
    "//omni:key B-test-example",
    "//omni:version a-01.00",
    "",
    "package example",
    "",
    'import "fmt"',
    'import "strings"',
    "",
    "// ════════════════════════════════════════════════════════════════════════",
    "",
    "const MaxItems = 100",
    "",
    "var counter int",
    "",
    "type Config struct {",
    "\tName string",
    "\tPort int",
    "}",
    "",
    "// helper does something useful",
    "func helper(x int) int {",
    "\treturn x + 1",
    "}",
    "",
    "// Serve starts the server",
    "func Serve(cfg *Config) error {",
    '\tfmt.Printf("serving on %d\\n", cfg.Port)',
    "\treturn nil",
    "}",
  ];
  const chunks = parseGoCodeChunks(lines);
  assertEquals(totalLines(chunks), lines.length);
});

// ---------------------------------------------------------------------------
// G13: Realistic Go file — full structure verification
// ---------------------------------------------------------------------------

Deno.test("go-chunker/G13: realistic Go file parses correctly", () => {
  const lines = [
    "// #!omni code --go -library",
    "//omni:key B-test-server",
    "//omni:version a-02.00",
    "",
    "package server",
    "",
    "import (",
    '\t"fmt"',
    '\t"net/http"',
    ")",
    "",
    "const (",
    "\tDefaultPort = 8080",
    "\tMaxConns    = 1000",
    ")",
    "",
    "var activeConns int",
    "",
    "type Server struct {",
    "\tPort int",
    "\tHost string",
    "}",
    "",
    "func newServer(port int) *Server {",
    "\treturn &Server{Port: port}",
    "}",
    "",
    "func (s *Server) Start() error {",
    '\taddr := fmt.Sprintf("%s:%d", s.Host, s.Port)',
    "\treturn http.ListenAndServe(addr, nil)",
    "}",
    "",
    "func main() {",
    "\tsrv := newServer(DefaultPort)",
    "\tsrv.Start()",
    "}",
  ];

  const chunks = parseGoCodeChunks(lines);

  // Verify categories present
  const pragmas = findChunks(chunks, "pragma");
  assert(pragmas.length >= 1, "Should have pragma chunk");

  const pkgs = findChunks(chunks, "package");
  assertEquals(pkgs.length, 1, "Should have one package chunk");

  const imports = findChunks(chunks, "import");
  assertEquals(imports.length, 1, "Should have one import chunk (block form)");
  assertEquals(imports[0]!.lines.length, 4, "Import block should have 4 lines");

  const consts = findChunks(chunks, "const");
  assertEquals(consts.length, 1, "Should have one const chunk (block form)");
  assertEquals(consts[0]!.lines.length, 4, "Const block should have 4 lines");

  const vars = findChunks(chunks, "var");
  assertEquals(vars.length, 1, "Should have one var chunk");
  assertEquals(vars[0]!.lines.length, 1, "Single-line var = 1 line");

  const types = findChunks(chunks, "type_def");
  assertEquals(types.length, 1, "Should have one type_def chunk");
  assertEquals(types[0]!.lines.length, 4, "Struct block = 4 lines");

  const fns = findChunks(chunks, "fn_decl");
  // newServer (unexported) + Start (method) = 2 fn_decl chunks
  assertEquals(fns.length, 2, "Should have 2 fn_decl chunks (newServer + Start method)");

  const mains = findChunks(chunks, "main_fn");
  assertEquals(mains.length, 1, "Should have one main_fn chunk");

  // Line conservation
  assertEquals(totalLines(chunks), lines.length);
});

// ---------------------------------------------------------------------------
// G14: Mixed single-line and block declarations
// ---------------------------------------------------------------------------

Deno.test("go-chunker/G14: mixed const styles produce separate chunks", () => {
  const lines = [
    "const SingleConst = 42",
    "",
    "const (",
    "\tBlockConst1 = 1",
    "\tBlockConst2 = 2",
    ")",
    "",
    "const AnotherSingle = \"hello\"",
  ];
  const chunks = parseGoCodeChunks(lines);
  const consts = findChunks(chunks, "const");
  assertEquals(consts.length, 3, "Should have 3 const chunks: single, block, single");
  assertEquals(consts[0]!.lines.length, 1);
  assertEquals(consts[1]!.lines.length, 4);
  assertEquals(consts[2]!.lines.length, 1);
});

Deno.test("go-chunker/G14: interleaved declarations stay separate", () => {
  const lines = [
    "const X = 1",
    "var Y int",
    "type Z int",
    "const A = 2",
  ];
  const chunks = parseGoCodeChunks(lines);
  assertEquals(chunks.length, 4);
  assertEquals(chunks[0]!.category, "const");
  assertEquals(chunks[1]!.category, "var");
  assertEquals(chunks[2]!.category, "type_def");
  assertEquals(chunks[3]!.category, "const");
});

// ============================================================================
// CLOSING
// ============================================================================
//
// Go chunker tests — verify the Go-specific chunking configuration and
// scaffold adapter. Tests the classifier, singleLineIfNoDelimiter flag,
// balanced capture, block/section mapping, metadata extraction, and the
// full ScaffoldAdapter interface.
//
// "Prove all things; hold fast that which is good." — 1 Thessalonians 5:21
// ============================================================================
