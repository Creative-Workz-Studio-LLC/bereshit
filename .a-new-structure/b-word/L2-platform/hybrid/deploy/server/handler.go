//omni:code --go -library
//omni:key B-L2-server-handler
//omni:version a-01.00

// ═══════════════════════════════════════════════════════════════════════════
// METADATA
// ═══════════════════════════════════════════════════════════════════════════
//
// Key: B-L2-server-handler
// Purpose: HTTP handlers for build configuration and dashboard preferences
// Biblical: Habakkuk 2:2 — "Write the vision, and make it plain upon tables"
// Authors: Nova Dawn
// Version: 1.1.0
// Created: 2026-02-08
//
// ═══════════════════════════════════════════════════════════════════════════

package server

// ═══════════════════════════════════════════════════════════════════════════
// SETUP
// ═══════════════════════════════════════════════════════════════════════════

import (
	"errors"
	"fmt"
	"io"
	"net/http"
	"os"

	"creativeworkzstudio.com/bereshit/L2-platform/hybrid/preferences"

	"gopkg.in/yaml.v3"
)

// ═══════════════════════════════════════════════════════════════════════════
// BODY
// ═══════════════════════════════════════════════════════════════════════════

// ───────────────────────────────────────────────────────────────────────────
// Config Handlers — read/write build.config.yaml with comment preservation
// ───────────────────────────────────────────────────────────────────────────

// handleReadConfig reads build.config.yaml and returns it as JSON.
// The YAML is fully parsed so the frontend receives structured data.
func (s *Service) handleReadConfig(w http.ResponseWriter, _ *http.Request) {
	data, err := os.ReadFile(s.paths.ConfigPath)
	if err != nil {
		if errors.Is(err, os.ErrNotExist) {
			writeError(w, http.StatusNotFound, "Config not found")
			return
		}
		writeError(w, http.StatusInternalServerError, err.Error())
		return
	}

	var parsed any
	if err := yaml.Unmarshal(data, &parsed); err != nil {
		writeError(w, http.StatusInternalServerError, fmt.Sprintf("YAML parse error: %s", err))
		return
	}

	writeJSON(w, http.StatusOK, parsed)
}

// handleUpdateConfigSection updates a single top-level section in build.config.yaml.
//
// Uses yaml.Node to preserve comments and key ordering in the YAML file.
// The incoming body (YAML or JSON) replaces only the targeted section's value
// node, leaving all other sections and their comments intact.
func (s *Service) handleUpdateConfigSection(w http.ResponseWriter, r *http.Request) {
	section := r.PathValue("section")
	if section == "" {
		writeError(w, http.StatusBadRequest, "section is required")
		return
	}

	body, err := io.ReadAll(r.Body)
	if err != nil {
		writeError(w, http.StatusBadRequest, "failed to read body")
		return
	}

	// Read existing YAML as a document node tree (preserves comments and ordering).
	yamlData, err := os.ReadFile(s.paths.ConfigPath)
	if err != nil {
		writeError(w, http.StatusInternalServerError, err.Error())
		return
	}

	var doc yaml.Node
	if err := yaml.Unmarshal(yamlData, &doc); err != nil {
		writeError(w, http.StatusInternalServerError, fmt.Sprintf("YAML parse error: %s", err))
		return
	}

	// Parse the incoming section data into a node tree.
	var sectionNode yaml.Node
	if err := yaml.Unmarshal(body, &sectionNode); err != nil {
		writeError(w, http.StatusBadRequest, fmt.Sprintf("invalid YAML/JSON: %s", err))
		return
	}

	// Walk the document's top-level mapping to find and replace the section.
	// MappingNode stores keys and values as alternating Content entries:
	// Content[0]=key1, Content[1]=val1, Content[2]=key2, Content[3]=val2, ...
	updated := false
	if doc.Kind == yaml.DocumentNode && len(doc.Content) > 0 {
		mapping := doc.Content[0]
		if mapping.Kind == yaml.MappingNode {
			for i := 0; i < len(mapping.Content)-1; i += 2 {
				if mapping.Content[i].Value == section {
					// Unwrap DocumentNode wrapper if present (yaml.Unmarshal wraps in DocumentNode).
					if sectionNode.Kind == yaml.DocumentNode && len(sectionNode.Content) > 0 {
						mapping.Content[i+1] = sectionNode.Content[0]
					} else {
						mapping.Content[i+1] = &sectionNode
					}
					updated = true
					break
				}
			}
		}
	}

	if !updated {
		writeError(w, http.StatusNotFound, fmt.Sprintf("section %q not found in config", section))
		return
	}

	output, err := yaml.Marshal(&doc)
	if err != nil {
		writeError(w, http.StatusInternalServerError, fmt.Sprintf("YAML serialize error: %s", err))
		return
	}

	if err := os.WriteFile(s.paths.ConfigPath, output, 0644); err != nil {
		writeError(w, http.StatusInternalServerError, err.Error())
		return
	}

	writeJSON(w, http.StatusOK, map[string]string{
		"status":  "updated",
		"section": section,
	})
}

// ───────────────────────────────────────────────────────────────────────────
// Preferences Handlers — XDG-compliant dashboard user preferences
// ───────────────────────────────────────────────────────────────────────────

// handleReadPreferences reads the dashboard preferences from XDG config dir.
// Returns an empty object if no preferences file exists (not an error).
func (s *Service) handleReadPreferences(w http.ResponseWriter, _ *http.Request) {
	prefs, err := preferences.Read()
	if err != nil {
		writeError(w, http.StatusInternalServerError, err.Error())
		return
	}
	writeJSON(w, http.StatusOK, prefs)
}

// handleWritePreferences writes dashboard preferences to XDG config dir.
func (s *Service) handleWritePreferences(w http.ResponseWriter, r *http.Request) {
	var prefs map[string]any
	if err := decodeJSON(r, &prefs); err != nil {
		writeError(w, http.StatusBadRequest, err.Error())
		return
	}

	if err := preferences.Write(prefs); err != nil {
		writeError(w, http.StatusInternalServerError, err.Error())
		return
	}

	writeJSON(w, http.StatusOK, map[string]string{"status": "saved"})
}

// ═══════════════════════════════════════════════════════════════════════════
// CLOSING
// ═══════════════════════════════════════════════════════════════════════════
