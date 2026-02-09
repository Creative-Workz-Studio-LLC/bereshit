// ═══════════════════════════════════════════════════════════════════════════
// METADATA
// ═══════════════════════════════════════════════════════════════════════════
//
// Key: create-from-template-pkg-discovery
// Purpose: Template discovery and listing logic
// Biblical: Exodus 25:40 - "Look that thou make them after their pattern"
// Authors: Nova Dawn
// Version: 1.0.0
// Created: 2025-12-09
//
// ═══════════════════════════════════════════════════════════════════════════

package discovery

// ═══════════════════════════════════════════════════════════════════════════
// SETUP
// ═══════════════════════════════════════════════════════════════════════════

import (
	"fmt"
	"os"
	"path/filepath"
	"strings"

	"cws.studio/skills/create-from-template/scripts/pkg/config"
)

// --- Types ---

// Template represents a discovered template
type Template struct {
	Name       string
	Category   string
	Language   string
	Type       string
	Path       string
	Block      string
	Extensions []string
}

// ═══════════════════════════════════════════════════════════════════════════
// BODY
// ═══════════════════════════════════════════════════════════════════════════

// --- Helpers ---

// shouldSkip checks if directory should be skipped
func shouldSkip(name string, skipDirs []string) bool {
	for _, skip := range skipDirs {
		if name == skip {
			return true
		}
	}
	return false
}

// --- Core Operations ---

// ListTemplates discovers all available templates
func ListTemplates(cfg *config.Config) ([]Template, error) {
	var templates []Template
	bereshitRoot := cfg.GetBereshitRoot()

	// Process code category
	if cat, ok := cfg.Categories["code"]; ok {
		codePath := filepath.Join(bereshitRoot, cat.Path)
		for lang, langCfg := range cat.Languages {
			langPath := filepath.Join(codePath, lang)
			files, err := discoverInDir(langPath, cfg.Discovery.SkipDirs)
			if err != nil {
				continue
			}
			for _, f := range files {
				templates = append(templates, Template{
					Name:       filepath.Base(f),
					Category:   "code",
					Language:   lang,
					Type:       inferType(f, langCfg.Types),
					Path:       f,
					Block:      cat.BlockStructure,
					Extensions: langCfg.Extensions,
				})
			}
		}
	}

	// Process data category
	if cat, ok := cfg.Categories["data"]; ok {
		dataPath := filepath.Join(bereshitRoot, cat.Path)
		for format, formatCfg := range cat.Formats {
			formatPath := filepath.Join(dataPath, format)
			files, err := discoverInDir(formatPath, cfg.Discovery.SkipDirs)
			if err != nil {
				continue
			}
			for _, f := range files {
				templates = append(templates, Template{
					Name:       filepath.Base(f),
					Category:   "data",
					Language:   format,
					Type:       inferType(f, formatCfg.Types),
					Path:       f,
					Block:      cat.BlockStructure,
					Extensions: formatCfg.Extensions,
				})
			}
		}
	}

	// Process documentation category
	if cat, ok := cfg.Categories["documentation"]; ok {
		docPath := filepath.Join(bereshitRoot, cat.Path)
		for format, formatCfg := range cat.Formats {
			formatPath := filepath.Join(docPath, format)
			files, err := discoverInDir(formatPath, cfg.Discovery.SkipDirs)
			if err != nil {
				continue
			}
			for _, f := range files {
				templates = append(templates, Template{
					Name:       filepath.Base(f),
					Category:   "documentation",
					Language:   format,
					Type:       inferType(f, formatCfg.Types),
					Path:       f,
					Block:      cat.BlockStructure,
					Extensions: formatCfg.Extensions,
				})
			}
		}
	}

	return templates, nil
}

// discoverInDir finds template files in a directory
func discoverInDir(dir string, skipDirs []string) ([]string, error) {
	var files []string

	err := filepath.Walk(dir, func(path string, info os.FileInfo, err error) error {
		if err != nil {
			return nil // Skip errors
		}

		if info.IsDir() {
			if shouldSkip(info.Name(), skipDirs) {
				return filepath.SkipDir
			}
			return nil
		}

		// Include all regular files as potential templates
		files = append(files, path)
		return nil
	})

	return files, err
}

// inferType guesses template type from filename
func inferType(path string, types []string) string {
	name := strings.ToLower(filepath.Base(path))
	for _, t := range types {
		if strings.Contains(name, t) {
			return t
		}
	}
	if len(types) > 0 {
		return types[0]
	}
	return "unknown"
}

// FindTemplate finds a specific template by category, language, and type
func FindTemplate(cfg *config.Config, category, language, templateType string) (*Template, error) {
	templates, err := ListTemplates(cfg)
	if err != nil {
		return nil, err
	}

	for _, t := range templates {
		if t.Category == category && t.Language == language {
			if templateType == "" || t.Type == templateType {
				return &t, nil
			}
		}
	}

	return nil, fmt.Errorf("template not found: %s/%s/%s", category, language, templateType)
}

// GetCategories returns available categories
func GetCategories(cfg *config.Config) []string {
	var cats []string
	for name := range cfg.Categories {
		cats = append(cats, name)
	}
	return cats
}

// GetLanguages returns languages for a category
func GetLanguages(cfg *config.Config, category string) []string {
	var langs []string
	if cat, ok := cfg.Categories[category]; ok {
		for lang := range cat.Languages {
			langs = append(langs, lang)
		}
		for format := range cat.Formats {
			langs = append(langs, format)
		}
	}
	return langs
}

// ═══════════════════════════════════════════════════════════════════════════
// CLOSING
// ═══════════════════════════════════════════════════════════════════════════
