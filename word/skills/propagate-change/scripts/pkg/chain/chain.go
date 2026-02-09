// ═══════════════════════════════════════════════════════════════════════════
// METADATA
// ═══════════════════════════════════════════════════════════════════════════
//
// Key: propagate-change-pkg-chain
// Purpose: Chain tracing logic for propagate-change skill
// Biblical: Galatians 5:9 - "A little leaven leaveneth the whole lump"
// Authors: Nova Dawn
// Version: 2.0.0
// Created: 2025-12-09
//
// ═══════════════════════════════════════════════════════════════════════════

package chain

// ═══════════════════════════════════════════════════════════════════════════
// SETUP
// ═══════════════════════════════════════════════════════════════════════════

import (
	"bufio"
	"os"
	"path/filepath"
	"regexp"
	"strings"

	"cws.studio/skills/propagate-change/scripts/pkg/config"
)

// Link represents one file in the derives_from chain
type Link struct {
	Path        string
	DerivesFrom string
	Level       int
	Type        string // "syntax-spec", "universal", "specialized", "format", "document"
}

// Result holds the result of a chain operation
type Result struct {
	File        string
	Chain       []Link
	Descendants []string
	Level       string
	Message     string
}

// ═══════════════════════════════════════════════════════════════════════════
// BODY
// ═══════════════════════════════════════════════════════════════════════════

// --- Helpers ---

// buildDerivesRegex builds regex from config patterns
func buildDerivesRegex(cfg *config.Config) *regexp.Regexp {
	patterns := make([]string, len(cfg.Patterns.DerivesFrom))
	for i, p := range cfg.Patterns.DerivesFrom {
		patterns[i] = regexp.QuoteMeta(p)
	}
	pattern := "(?:" + strings.Join(patterns, "|") + ")\\s*[\"']?([^\\s\"']+)"
	return regexp.MustCompile(pattern)
}

// extractDerivesFrom finds the derives_from reference in a file
func extractDerivesFrom(filename string, derivesRegex *regexp.Regexp) string {
	file, err := os.Open(filename)
	if err != nil {
		return ""
	}
	defer file.Close()

	scanner := bufio.NewScanner(file)
	for scanner.Scan() {
		if matches := derivesRegex.FindStringSubmatch(scanner.Text()); matches != nil {
			return matches[1]
		}
	}

	return ""
}

// determineLevel determines the hierarchy level of a file
func determineLevel(path string, cfg *config.Config) string {
	// Check if it's in omni seed (specialized template)
	if strings.Contains(path, cfg.Locations.OmniSeed) {
		if strings.HasSuffix(path, "universal.omni") {
			return "universal"
		}
		return "specialized"
	}

	// Check if it's in format seed (format template)
	if strings.Contains(path, cfg.Locations.FormatSeed) {
		return "format"
	}

	// Default to document
	return "document"
}

// --- Core Operations ---

// TraceUp follows derives_from links up to root
func TraceUp(startFile string, cfg *config.Config) Result {
	derivesRegex := buildDerivesRegex(cfg)
	var chain []Link
	seen := make(map[string]bool)

	currentFile := startFile
	level := 0

	for currentFile != "" {
		// Check if file exists
		if _, err := os.Stat(currentFile); os.IsNotExist(err) {
			// Check if it's a root marker
			if cfg.IsRootMarker(strings.ToLower(currentFile)) {
				chain = append(chain, Link{
					Path:        "(original - root of chain)",
					DerivesFrom: "",
					Level:       level,
					Type:        "syntax-spec",
				})
				break
			}
			break
		}

		// Prevent infinite loops
		if seen[currentFile] {
			chain = append(chain, Link{
				Path:        currentFile + " (CYCLE DETECTED)",
				DerivesFrom: "",
				Level:       level,
				Type:        "error",
			})
			break
		}
		seen[currentFile] = true

		derivesFrom := extractDerivesFrom(currentFile, derivesRegex)
		fileType := determineLevel(currentFile, cfg)

		chain = append(chain, Link{
			Path:        currentFile,
			DerivesFrom: derivesFrom,
			Level:       level,
			Type:        fileType,
		})

		if derivesFrom == "" || cfg.IsRootMarker(strings.ToLower(derivesFrom)) {
			break
		}

		// Resolve next file path
		if !filepath.IsAbs(derivesFrom) {
			dir := filepath.Dir(currentFile)
			derivesFrom = filepath.Join(dir, derivesFrom)
		}

		currentFile = derivesFrom
		level++
	}

	// Determine the level of the starting file
	startLevel := "document"
	if len(chain) > 0 {
		startLevel = chain[0].Type
	}

	return Result{
		File:    startFile,
		Chain:   chain,
		Level:   startLevel,
		Message: "",
	}
}

// FindDescendants searches for files that derive from the template
func FindDescendants(templateName string, searchDir string, cfg *config.Config) Result {
	derivesRegex := buildDerivesRegex(cfg)
	var descendants []string

	err := filepath.Walk(searchDir, func(path string, info os.FileInfo, err error) error {
		if err != nil {
			return nil // Continue walking
		}

		// Skip directories
		if info.IsDir() {
			return nil
		}

		// Only check relevant file types
		ext := strings.ToLower(filepath.Ext(path))
		validExt := false
		for _, pattern := range cfg.Locations.FilePatterns {
			if strings.HasSuffix(pattern, ext) || pattern == "*"+ext {
				validExt = true
				break
			}
		}
		if !validExt {
			return nil
		}

		// Check if file contains derives_from with template name
		file, err := os.Open(path)
		if err != nil {
			return nil
		}
		defer file.Close()

		scanner := bufio.NewScanner(file)
		for scanner.Scan() {
			line := scanner.Text()
			if derivesRegex.MatchString(line) && strings.Contains(line, templateName) {
				descendants = append(descendants, path)
				break
			}
		}

		return nil
	})

	message := ""
	if err != nil {
		message = "Warning: error walking directory: " + err.Error()
	}

	return Result{
		File:        templateName,
		Descendants: descendants,
		Message:     message,
	}
}

// GenerateReport creates a propagation report for a changed file
func GenerateReport(file string, cfg *config.Config) Result {
	// Trace the chain up
	chainResult := TraceUp(file, cfg)

	// Find descendants
	// Use the first search path as root
	searchDir := "."
	if len(cfg.Locations.SearchPaths) > 0 {
		searchDir = cfg.Locations.SearchPaths[0]
	}

	descendantResult := FindDescendants(filepath.Base(file), searchDir, cfg)

	return Result{
		File:        file,
		Chain:       chainResult.Chain,
		Level:       chainResult.Level,
		Descendants: descendantResult.Descendants,
		Message:     chainResult.Message + descendantResult.Message,
	}
}

// ═══════════════════════════════════════════════════════════════════════════
// CLOSING
// ═══════════════════════════════════════════════════════════════════════════
