// #!omni code --go -library
// #!omni meta.key = B-L1-omnicode-hybrid-pragma-dispatch
// #!omni meta.from = B-L1-omnicode-hybrid-pragma
// #!omni meta.at = a-01.00
//omni:code --go -library
//omni:key B-L1-omnicode-hybrid-pragma-dispatch
//omni:version a-01.00
// ============================================================================
// METADATA
// ============================================================================
//
// OmniCode Format Dispatcher — Route Files to the Right Parser
//
// Key:     B-L1-omnicode-hybrid-pragma-dispatch
// Purpose: Read pragma, dispatch to L0 config/util parser (TOML, JSONC, JSON)
// Biblical: Genesis 1:4 - "And God divided the light from the darkness"
//           Separation enables identity — each format gets its proper handler.
// Authors: Nova Dawn
// Version: a-01.00
// Created: 2026-02-13
//
// The dispatcher bridges L1 (OmniCode identity) and L0 (format parsing).
// An .omni file with --toml format gets its pragma read by L1, then its
// content parsed by L0's TOML loader. An .omni file with --json gets L0's
// JSONC parser. An .omni file with --omni gets the native parser (future).
//
// ============================================================================
// END METADATA
// ============================================================================

package pragma

// ============================================================================
// SETUP
// ============================================================================

import (
	"fmt"
	"path/filepath"
	"sync"

	// L0 shared config utilities — format parsers, structured errors
	"creativeworkzstudio.com/bereshit/L0-universal/hybrid/config/util"
)

// --- File Extension Registry ---

// KnownExtensions maps file extensions to their OmniCode significance.
var KnownExtensions = map[string]string{
	".omni": "OmniCode native file",
	".ofd":  "OmniFold folder metadata",
	".toml": "TOML config (may contain pragma)",
	".json": "JSON data (may contain pragma in adjacent .omni)",
	".jsonc": "JSONC config (may contain pragma in adjacent .omni)",
}

// --- Dispatch Cache ---
//
// Caches LoadConfigMap results by filepath. Generic LoadConfig[T] can't be
// easily cached (type erasure), but the map variant is the common hot path
// for OmniFold traversal and tools that inspect arbitrary files.

type configMapEntry struct {
	pragma *Pragma
	data   map[string]any
}

var (
	dispatchMu    sync.RWMutex
	dispatchCache = make(map[string]*configMapEntry)
)

// ============================================================================
// END SETUP
// ============================================================================

// ============================================================================
// BODY
// ============================================================================

// ────────────────────────────────────────────────────────────────
// Helpers
// ────────────────────────────────────────────────────────────────

// IsOmniCodeFile returns true if the file extension indicates an OmniCode file.
func IsOmniCodeFile(path string) bool {
	ext := filepath.Ext(path)
	return ext == ExtOmni || ext == ExtOmniFold
}

// ────────────────────────────────────────────────────────────────
// Format Dispatch
// ────────────────────────────────────────────────────────────────

// LoadConfig reads an OmniCode-wrapped config file.
// Parses the pragma to determine format, then dispatches to the right L0 parser.
//
// Supports:
//   - .omni files with --toml format → L0 TOML loader
//   - .omni files with --json format → L0 JSONC loader
//   - .ofd files (folder metadata) → L0 JSONC or TOML loader based on pragma
//
// For pure OmniCode (--omni), returns an error — native parser not yet implemented.
//
// Uses L0 structured error types (util.LoadError) for consistent error handling
// across the L0-L1 boundary.
func LoadConfig[T any](path string) (*Pragma, *T, error) {
	// Step 1: Read pragma (L1 — identity, cached by Parse())
	p, err := Parse(path)
	if err != nil {
		return nil, nil, util.NewLoadError(path, "pragma", err)
	}

	// Step 2: Dispatch to L0 parser based on format
	var result *T

	switch p.Format {
	case FormatTOML:
		result, err = loadTOMLContent[T](path)
	case FormatJSON, FormatJSONC:
		result, err = util.LoadJSONCFile[T](path)
	case FormatOmni:
		return p, nil, util.NewLoadError(path, "dispatch",
			fmt.Errorf("native OmniCode parser not yet implemented"))
	default:
		return p, nil, util.NewLoadError(path, "dispatch",
			fmt.Errorf("unsupported config format %q", p.Format))
	}

	if err != nil {
		return p, nil, util.NewLoadError(path, "parse", err)
	}

	return p, result, nil
}

// LoadConfigMap loads an OmniCode config file into a generic map.
// Useful when the exact structure isn't known at compile time.
//
// Results are cached by filepath — subsequent calls for the same path
// return the cached result without disk I/O. Use InvalidateDispatchCache()
// after file modifications.
func LoadConfigMap(path string) (*Pragma, map[string]any, error) {
	// Normalize path for consistent cache keys
	path = filepath.Clean(path)

	// Check cache first (read lock — concurrent readers allowed)
	dispatchMu.RLock()
	if cached, ok := dispatchCache[path]; ok {
		dispatchMu.RUnlock()
		return cached.pragma, cached.data, nil
	}
	dispatchMu.RUnlock()

	// Cache miss — load via generic path
	p, result, err := LoadConfig[map[string]any](path)
	if err != nil {
		return p, nil, err
	}

	var data map[string]any
	if result != nil {
		data = *result
	}

	// Store in cache (write lock — exclusive access)
	dispatchMu.Lock()
	dispatchCache[path] = &configMapEntry{pragma: p, data: data}
	dispatchMu.Unlock()

	return p, data, nil
}

// ────────────────────────────────────────────────────────────────
// Format-Specific Loaders
// ────────────────────────────────────────────────────────────────

// loadTOMLContent loads TOML content from an OmniCode-wrapped file.
// Skips pragma lines (they're OmniCode, not TOML) and parses the rest.
//
// NOTE: For files where pragma is in TOML comments (# #!omni ...),
// the TOML parser naturally ignores comments. For bare pragma files (.omni),
// we need to strip pragma lines before parsing — this is the OmniCode-native
// case where pragma IS syntax, not comments.
//
// TODO(L1): Handle bare .omni files where pragma lines aren't comments.
// Current implementation works for .toml files with pragma in # comments.
func loadTOMLContent[T any](path string) (*T, error) {
	// For now, the TOML parser handles # #!omni lines as comments.
	// This works for .toml files. For .omni files with --toml format,
	// we'll need to strip pragma lines first (future L1 work).
	return util.LoadTOMLFile[T](path)
}

// ────────────────────────────────────────────────────────────────
// OmniFold (.ofd) Support
// ────────────────────────────────────────────────────────────────

// LoadFolder reads an OmniFold (.ofd) folder metadata file.
// Returns the pragma and raw content as a map.
func LoadFolder(path string) (*Pragma, map[string]any, error) {
	if ext := filepath.Ext(path); ext != ExtOmniFold && ext != ExtOmni {
		return nil, nil, util.NewLoadError(path, "dispatch",
			fmt.Errorf("expected %s or %s extension, got %s", ExtOmniFold, ExtOmni, ext))
	}

	p, data, err := LoadConfigMap(path)
	if err != nil {
		return nil, nil, err // Already a LoadError from LoadConfig
	}

	if p.FileType != TypeFolder {
		return p, data, util.NewLoadError(path, "validate",
			fmt.Errorf("expected folder type, got %s", p.FileType))
	}

	return p, data, nil
}

// ────────────────────────────────────────────────────────────────
// Cache Invalidation
// ────────────────────────────────────────────────────────────────

// InvalidateDispatchCache clears the entire dispatch cache.
// Call after bulk file modifications or during testing.
// Also clears the pragma cache since dispatch depends on it.
func InvalidateDispatchCache() {
	dispatchMu.Lock()
	dispatchCache = make(map[string]*configMapEntry)
	dispatchMu.Unlock()

	// Pragma cache is upstream — clear it too
	InvalidatePragmaCache()
}

// InvalidateDispatchCacheEntry removes a single entry from the dispatch cache.
// Also clears the corresponding pragma cache entry.
func InvalidateDispatchCacheEntry(path string) {
	path = filepath.Clean(path)
	dispatchMu.Lock()
	delete(dispatchCache, path)
	dispatchMu.Unlock()

	InvalidatePragmaCacheEntry(path)
}

// ============================================================================
// END BODY
// ============================================================================

// ============================================================================
// CLOSING
// ============================================================================
//
// The dispatch chain:
//
//   File (.omni/.ofd)
//     ↓ Parse pragma (L1)
//   Pragma { type, format, key, from, at }
//     ↓ Dispatch by format
//   L0 parser (TOML / JSONC / future: native OmniCode)
//     ↓ Returns typed data
//   Caller gets Pragma + parsed config
//
// This wires OmniCode identity into the config loading chain.
// The pragma tells us WHAT the file is. The L0 parsers know HOW to read
// the underlying format. L1 connects the two.
//
// Future: When the native OmniCode parser exists (L1 frontend/lexer),
// FormatOmni dispatch will route there instead of returning an error.
//
// ============================================================================
// END CLOSING
// ============================================================================
