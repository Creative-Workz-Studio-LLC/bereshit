// #!omni code --go -library
// #!omni meta.key = B-L1-omnicode-hybrid-pragma
// #!omni meta.from = bereshit/word/omni/syntax.omni
// #!omni meta.at = a-01.00
//omni:code --go -library
//omni:key B-L1-omnicode-hybrid-pragma
//omni:version a-01.00
// ============================================================================
// METADATA
// ============================================================================
//
// OmniCode Pragma Reader — The First Thing Read in Any OmniCode File
//
// Key:     B-L1-omnicode-hybrid-pragma
// Purpose: Parse #!omni pragma lines from any file format
// Biblical: Genesis 1:3 - "And God said, Let there be light"
//           The pragma is the first word spoken — identity before content.
// Authors: Nova Dawn
// Version: a-01.00
// Created: 2026-02-13
//
// Pragma lines declare what a file IS, where it comes FROM, and where it's AT.
// This reader strips format-specific comment prefixes (// # <!-- -->) and
// extracts the structured identity information.
//
// Stands on: word/omni/syntax.omni (canonical spec, lines 150-249)
//
// ============================================================================
// END METADATA
// ============================================================================

package pragma

// ============================================================================
// SETUP
// ============================================================================

import (
	"bufio"
	"fmt"
	"os"
	"path/filepath"
	"strings"
	"sync"

	"creativeworkzstudio.com/bereshit/L0-universal/spiral/identity"
)

// --- Constants ---

const (
	// PragmaPrefix is the universal OmniCode pragma marker.
	PragmaPrefix = "#!omni"

	// MaxPragmaLines limits how far into a file we scan for pragmas.
	// Pragmas must appear at the top — this prevents scanning huge files.
	MaxPragmaLines = 20

	// MaxLineLength bounds the maximum length of a single line during
	// pragma scanning (100KB). Prevents DoS via files with extremely
	// long lines. Go's bufio.Scanner defaults to 64KB; we set explicitly.
	MaxLineLength = 100 * 1024
)

// --- Types ---

// Type represents what a file IS.
type Type string

const (
	TypeUniversal     Type = "universal"
	TypeTemplate      Type = "template"
	TypeCode          Type = "code"
	TypeDocumentation Type = "documentation"
	TypeInterface     Type = "interface"
	TypeData          Type = "data"
	TypeFolder        Type = "folder"
	TypeSpec          Type = "spec" // Extension: specification files
)

// Format represents the target language/format.
type Format string

const (
	FormatOmni Format = "omni" // Pure OmniCode (default)
	FormatGo   Format = "go"
	FormatC    Format = "c"
	FormatRust Format = "rs"
	FormatAdoc Format = "adoc"
	FormatMD   Format = "md"
	FormatJSON Format = "json"
	FormatYAML Format = "yaml"
	FormatTOML   Format = "toml"
	FormatTxt    Format = "txt"
	FormatBinary Format = "binary" // Compiled/encoded output (.gen)
	FormatText   Format = "text"   // Plain text decoding output (.rev)
	FormatJSONC  Format = "jsonc"  // JSONC with comments (.witness, .scroll)
)

// Modifier refines the subtype.
type Modifier string

const (
	ModNone       Modifier = ""
	ModLibrary    Modifier = "library"
	ModExecutable Modifier = "executable"
	ModTest       Modifier = "test"
	ModDemo       Modifier = "demo"
)

// Pragma holds the parsed identity of an OmniCode file.
type Pragma struct {
	// Line 1: What this file IS
	FileType Type     // universal, template, code, data, etc.
	Format   Format   // omni, go, c, toml, json, etc.
	Modifier Modifier // library, executable, test, demo

	// Lines 2-4: Meta fields
	Key  string // meta.key — unique identifier
	From string // meta.from — derivation source
	At   string // meta.at — version state

	// Raw pragma lines (for round-tripping)
	RawLines []string
}

// --- File Extension Constants ---

const (
	ExtOmni     = ".omni" // OmniCode native files
	ExtOmniFold = ".ofd"  // OmniFold folder metadata
)

// --- Pragma Cache ---
//
// Parse() caches results by filepath, avoiding repeated disk I/O for the
// same file. Protected by sync.RWMutex for concurrent access.
// Matches L0 config loader caching pattern (loader.go:424-429).

var (
	pragmaMu    sync.RWMutex
	pragmaCache = make(map[string]*Pragma)
)

// ────────────────────────────────────────────────────────────────
// Identity — OmniCode Self-Registration
// ────────────────────────────────────────────────────────────────

// _pragma — OmniCode file identity (P1-P5). Runtime-queryable.
var _pragma = map[string]string{
	// P1: Core Identity
	"P1.key":    "B-L1-omnicode-hybrid-pragma",
	"P1.type":   "code",
	"P1.format": "go",
	"P1.style":  "library",
	// P2: Structure
	"P2.blocks": "4-block",
	// P3: Derivation
	"P3.from":    "bereshit/word/omni/syntax.omni",
	"P3.derives": "B-word-omni-syntax",
	// P4: Version
	"P4.at": "a-01.00",
	// P5: Summary
	"P5.title":   "OmniCode Pragma Reader",
	"P5.summary": "Parse #!omni pragma lines from any file format — identity before content",
}

// _metadata — OmniCode file metadata (M1-M10). Runtime-queryable.
var _metadata = map[string]string{
	// ── M1: Core Identity ─────────────────────────────────────────
	"M1.key":            "B-L1-omnicode-hybrid-pragma",
	"M1.component_type": "Hybrid",
	"M1.architect":      "Seanje Lenox-Wise",
	"M1.implementation": "Nova Dawn",
	"M1.created":        "2026-02-13",

	// ── M2: Version History ───────────────────────────────────────
	"M2.a-01.00": "2026-02-13 — Initial creation: pragma reader, extension registry, format dispatch",

	// ── M3: Interface ─────────────────────────────────────────────
	"M3.requires.stdlib":   "bufio, fmt, os, path/filepath, strings",
	"M3.requires.external": "github.com/BurntSushi/toml (via L0 config/util)",
	"M3.requires.internal": "L0-universal/hybrid/config/util (format loaders), L0-universal/spiral/identity (registration)",
	"M3.used_by":           "L2-platform/substrates, L3-cpisi/orchestration, L5-applications",
	"M3.import":            "creativeworkzstudio.com/bereshit/L1-omnicode/hybrid/pragma",
	"M3.pattern":           "pragma.Parse(path) → *Pragma; pragma.LoadConfig[T](path) → (*Pragma, *T, error)",

	// ── M4: Public API ────────────────────────────────────────────
	"M4.reader":        "Parse, ParseReader, ParseString",
	"M4.extensions":    "LookupExt, IsRegisteredExt, ExtsByCategory, ExtsByPhase, ExtsByState, AllExts",
	"M4.dispatch":      "LoadConfig, LoadConfigMap, LoadFolder, IsOmniCodeFile",
	"M4.format_bridge": "InternalFormatForExt, BlockPatternForExt, DefaultTypeForExt",
	"M4.identity":      "Pragma, Metadata (accessors)",

	// ── M5: Operational ───────────────────────────────────────────
	"M5.blocking": "file I/O for Parse() and LoadConfig() — otherwise in-memory",
	"M5.health":   "Provider | granted: registered | deferred: init pending | denied: n/a",

	// ── M6: Classification ────────────────────────────────────────
	"M6.tags":     "pragma, identity, parser, omnicode, format-dispatch, extensions",
	"M6.category": "Hybrid",
	"M6.domain":   "pragma",
	"M6.layer":    "L1-omnicode",
	"M6.paradigm": "CPI-SI",

	// ── M7: Intent ────────────────────────────────────────────────
	"M7.purpose":    "The first thing read in any OmniCode file — identity before content",
	"M7.philosophy": "Identity declares existence. Pragma precedes processing. Genesis 1:3 before Genesis 1:4.",
	"M7.provides":   "Pragma parsing, 15-extension registry, format dispatch to L0 loaders, identity registration",

	// ── M8: Grounding ─────────────────────────────────────────────
	"M8.scripture": "Genesis 1:3 — And God said, Let there be light",
	"M8.principle": "The pragma is the first word spoken — identity before content",
	"M8.anchor":    "Genesis 1:1 — In the beginning God created the heaven and the earth",

	// ── M9: Dependencies ──────────────────────────────────────────
	"M9.needs.stdlib":   "bufio, fmt, os, path/filepath, strings",
	"M9.needs.external": "github.com/BurntSushi/toml (transitive via L0)",
	"M9.needs.internal": "L0-universal/hybrid/config/util, L0-universal/spiral/identity",
	"M9.used_by":        "L2-platform, L3-cpisi, L5-applications — any code that reads OmniCode files",
	"M9.layer_deps":     "L0 only — no lateral or upward dependencies",

	// ── M10: Roadmap ──────────────────────────────────────────────
	"M10.current":     "a-01.00 — Core pragma reader, extension registry, format dispatch",
	"M10.planned":     "Native OmniCode parser (FormatOmni dispatch), block validator, derivation checker",
	"M10.limitations": "Native OmniCode format not yet parseable, block validation not yet implemented",
}

func init() {
	identity.Register(_pragma["P1.key"], _pragma, _metadata)
}

// PragmaIdentity returns this package's own OmniCode pragma identity (P1-P5).
func PragmaIdentity() map[string]string { return _pragma }

// MetadataIdentity returns this package's own OmniCode metadata (M1-M10).
func MetadataIdentity() map[string]string { return _metadata }

// ============================================================================
// END SETUP
// ============================================================================

// ============================================================================
// BODY
// ============================================================================

// ────────────────────────────────────────────────────────────────
// Helpers — Comment Prefix Stripping
// ────────────────────────────────────────────────────────────────

// stripCommentPrefix removes format-specific comment markers to reveal
// the bare #!omni pragma line underneath.
//
// Supported prefixes (from syntax.omni PRAGMA PLACEMENT):
//   .omni files: bare (no prefix)
//   .go/.c/.rs:  // prefix
//   .toml/.yaml: # prefix
//   .adoc:       //// block (we handle // prefix)
//   .md:         <!-- --> wrapping
func stripCommentPrefix(line string) string {
	trimmed := strings.TrimSpace(line)

	// Already bare pragma
	if strings.HasPrefix(trimmed, PragmaPrefix) {
		return trimmed
	}

	// // #!omni ... (Go, C, Rust, AsciiDoc)
	if rest, ok := strings.CutPrefix(trimmed, "//"); ok {
		inner := strings.TrimSpace(rest)
		if strings.HasPrefix(inner, PragmaPrefix) {
			return inner
		}
	}

	// # #!omni ... (TOML, YAML, shell)
	// But NOT #!omni itself (already handled above)
	if strings.HasPrefix(trimmed, "#") && !strings.HasPrefix(trimmed, "#!") {
		if rest, ok := strings.CutPrefix(trimmed, "#"); ok {
			inner := strings.TrimSpace(rest)
			if strings.HasPrefix(inner, PragmaPrefix) {
				return inner
			}
		}
	}

	// <!-- #!omni ... --> (Markdown, HTML)
	if rest, ok := strings.CutPrefix(trimmed, "<!--"); ok {
		inner := strings.TrimSuffix(rest, "-->")
		inner = strings.TrimSpace(inner)
		if strings.HasPrefix(inner, PragmaPrefix) {
			return inner
		}
	}

	return "" // Not a pragma line
}

// ────────────────────────────────────────────────────────────────
// Parsing
// ────────────────────────────────────────────────────────────────

// parseLine1 parses: #!omni [type] --[format] -[modifier]
func parseLine1(line string) (Type, Format, Modifier) {
	// Remove the #!omni prefix
	after, _ := strings.CutPrefix(line, PragmaPrefix)
	parts := strings.Fields(strings.TrimSpace(after))

	var fileType Type
	var format Format
	var modifier Modifier

	for _, part := range parts {
		if fmtStr, ok := strings.CutPrefix(part, "--"); ok {
			// Format: --go, --toml, --json, etc.
			format = Format(fmtStr)
		} else if modStr, ok := strings.CutPrefix(part, "-"); ok {
			// Modifier: -library, -executable, etc.
			modifier = Modifier(modStr)
		} else {
			// Type: code, data, documentation, etc.
			fileType = Type(part)
		}
	}

	// Default format is omni
	if format == "" {
		format = FormatOmni
	}

	return fileType, format, modifier
}

// parseMetaLine parses: #!omni meta.[field] = [value]
func parseMetaLine(line string) (field, value string) {
	after, _ := strings.CutPrefix(line, PragmaPrefix)
	rest := strings.TrimSpace(after)

	// Expect: meta.[field] = [value]
	metaRest, ok := strings.CutPrefix(rest, "meta.")
	if !ok {
		return "", ""
	}

	parts := strings.SplitN(metaRest, "=", 2)
	if len(parts) != 2 {
		return "", ""
	}

	field = strings.TrimSpace(parts[0])
	value = strings.TrimSpace(parts[1])
	return field, value
}

// ────────────────────────────────────────────────────────────────
// Public API
// ────────────────────────────────────────────────────────────────

// Parse reads pragma lines from a file and returns the parsed identity.
// Scans up to MaxPragmaLines from the beginning of the file.
//
// Results are cached by filepath — subsequent calls for the same path
// return the cached result without disk I/O. Use InvalidatePragmaCache()
// or InvalidatePragmaCacheEntry() after file modifications.
func Parse(path string) (*Pragma, error) {
	// Normalize path for consistent cache keys — prevents bypass via
	// "./file" vs "file" vs "/abs/file" producing separate cache entries.
	path = filepath.Clean(path)

	// Check cache first (read lock — concurrent readers allowed)
	pragmaMu.RLock()
	if cached, ok := pragmaCache[path]; ok {
		pragmaMu.RUnlock()
		return cached, nil
	}
	pragmaMu.RUnlock()

	// Cache miss — read from disk
	f, err := os.Open(path)
	if err != nil {
		return nil, fmt.Errorf("pragma: open %s: %w", path, err)
	}
	defer f.Close()

	p, err := ParseReader(bufio.NewReader(f))
	if err != nil {
		return nil, err
	}

	// Store in cache (write lock — exclusive access)
	pragmaMu.Lock()
	pragmaCache[path] = p
	pragmaMu.Unlock()

	return p, nil
}

// InvalidatePragmaCache clears the entire pragma cache.
// Call after bulk file modifications or during testing.
func InvalidatePragmaCache() {
	pragmaMu.Lock()
	pragmaCache = make(map[string]*Pragma)
	pragmaMu.Unlock()
}

// InvalidatePragmaCacheEntry removes a single entry from the pragma cache.
// Call after modifying a specific file to force re-parsing on next access.
func InvalidatePragmaCacheEntry(path string) {
	path = filepath.Clean(path)
	pragmaMu.Lock()
	delete(pragmaCache, path)
	pragmaMu.Unlock()
}

// ParseReader reads pragma lines from a reader.
func ParseReader(r *bufio.Reader) (*Pragma, error) {
	p := &Pragma{}
	lineNum := 0
	foundPragma := false

	scanner := bufio.NewScanner(r)
	scanner.Buffer(make([]byte, 0, 4096), MaxLineLength)
	for scanner.Scan() {
		lineNum++
		if lineNum > MaxPragmaLines {
			break
		}

		raw := scanner.Text()
		stripped := stripCommentPrefix(raw)
		if stripped == "" {
			// Non-pragma line — if we already found pragmas, stop scanning.
			// Pragmas must be contiguous at the top.
			if foundPragma {
				break
			}
			continue
		}

		foundPragma = true
		p.RawLines = append(p.RawLines, stripped)

		// First pragma line = type declaration
		if len(p.RawLines) == 1 {
			p.FileType, p.Format, p.Modifier = parseLine1(stripped)
			continue
		}

		// Subsequent lines = meta fields
		field, value := parseMetaLine(stripped)
		switch field {
		case "key":
			p.Key = value
		case "from":
			p.From = value
		case "at":
			p.At = value
		}
	}

	if err := scanner.Err(); err != nil {
		return nil, fmt.Errorf("pragma: scan: %w", err)
	}

	if !foundPragma {
		return nil, fmt.Errorf("pragma: no #!omni pragma found in first %d lines", MaxPragmaLines)
	}

	return p, nil
}

// ParseString parses pragma from a string (convenience for testing).
func ParseString(content string) (*Pragma, error) {
	return ParseReader(bufio.NewReader(strings.NewReader(content)))
}

// ────────────────────────────────────────────────────────────────
// Pragma Methods
// ────────────────────────────────────────────────────────────────

// IsConfig returns true if this file holds configuration data.
// Config files are data-type files with TOML, JSON, JSONC, or YAML format.
func (p *Pragma) IsConfig() bool {
	if p.FileType != TypeData && p.FileType != TypeFolder {
		return false
	}
	switch p.Format {
	case FormatTOML, FormatJSON, FormatJSONC, FormatYAML:
		return true
	default:
		return false
	}
}

// IsNativeOmni returns true if this file is pure OmniCode (not wrapping another format).
func (p *Pragma) IsNativeOmni() bool {
	return p.Format == FormatOmni
}

// IsFolder returns true if this is an OmniFold folder metadata file.
func (p *Pragma) IsFolder() bool {
	return p.FileType == TypeFolder
}

// BlockPattern returns the expected block structure for this file type.
// Returns "3-block", "4-block", or "5-block" based on the type.
func (p *Pragma) BlockPattern() string {
	switch p.FileType {
	case TypeData, TypeInterface, TypeFolder:
		return "3-block"
	case TypeCode, TypeTemplate, TypeUniversal:
		return "4-block"
	case TypeDocumentation:
		return "5-block"
	default:
		return "unknown"
	}
}

// String returns a human-readable representation.
func (p *Pragma) String() string {
	s := fmt.Sprintf("#!omni %s", p.FileType)
	if p.Format != FormatOmni {
		s += fmt.Sprintf(" --%s", p.Format)
	}
	if p.Modifier != ModNone {
		s += fmt.Sprintf(" -%s", p.Modifier)
	}
	if p.Key != "" {
		s += fmt.Sprintf(" [key=%s]", p.Key)
	}
	if p.At != "" {
		s += fmt.Sprintf(" [at=%s]", p.At)
	}
	return s
}

// ============================================================================
// END BODY
// ============================================================================

// ============================================================================
// CLOSING
// ============================================================================
//
// The pragma reader is the first thing that touches an OmniCode file.
// Before content is parsed, before format is dispatched, before blocks are
// validated — the pragma declares WHAT this file IS.
//
// Like Genesis 1:3 ("Let there be light") precedes all other creation,
// the pragma precedes all other processing. Identity first. Always.
//
// Next in the chain:
//   - L1 format dispatcher (reads pragma → routes to L0 parser)
//   - L1 block validator (reads pragma → validates block structure)
//   - L1 derivation checker (reads meta.from → validates lineage)
//
// ============================================================================
// END CLOSING
// ============================================================================
