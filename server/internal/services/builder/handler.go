// ═══════════════════════════════════════════════════════════════════════════
// METADATA
// ═══════════════════════════════════════════════════════════════════════════
//
// Key: cws-server-builder-handler
// Purpose: HTTP handlers for the builder dashboard service
// Authors: Nova Dawn
// Version: 1.0.0
// Created: 2026-02-08
//
// ═══════════════════════════════════════════════════════════════════════════

package builder

// ═══════════════════════════════════════════════════════════════════════════
// SETUP
// ═══════════════════════════════════════════════════════════════════════════

import (
	"encoding/json"
	"errors"
	"fmt"
	"io"
	"log/slog"
	"net/http"
	"os"

	"cws.studio/server/internal/config"

	"gopkg.in/yaml.v3"
)

// ═══════════════════════════════════════════════════════════════════════════
// BODY
// ═══════════════════════════════════════════════════════════════════════════

// Service is the builder dashboard HTTP service.
type Service struct {
	pm    *ProcessManager
	paths *config.BuilderPaths
}

// NewService creates a new builder dashboard service.
func NewService(pm *ProcessManager, paths *config.BuilderPaths) *Service {
	return &Service{pm: pm, paths: paths}
}

// RegisterRoutes registers all builder dashboard routes on the given mux.
func (s *Service) RegisterRoutes(mux *http.ServeMux) {
	// Command execution
	mux.HandleFunc("POST /api/builder/commands/run", s.handleRunCommand)
	mux.HandleFunc("POST /api/builder/commands/make", s.handleRunMake)
	mux.HandleFunc("POST /api/builder/commands/cancel", s.handleCancelProcess)
	mux.HandleFunc("GET /api/builder/commands/active", s.handleActiveProcesses)

	// Configuration
	mux.HandleFunc("GET /api/builder/config/preferences", s.handleReadPreferences)
	mux.HandleFunc("PUT /api/builder/config/preferences", s.handleWritePreferences)
	mux.HandleFunc("GET /api/builder/config", s.handleReadConfig)
	mux.HandleFunc("PATCH /api/builder/config/{section}", s.handleUpdateConfigSection)

	slog.Info("builder service routes registered")
}

// ───────────────────────────────────────────────────────────────────────────
// Command Handlers
// ───────────────────────────────────────────────────────────────────────────

type runCommandRequest struct {
	Command string   `json:"command"`
	Args    []string `json:"args"`
}

func (s *Service) handleRunCommand(w http.ResponseWriter, r *http.Request) {
	var req runCommandRequest
	if err := decodeJSON(r, &req); err != nil {
		writeError(w, http.StatusBadRequest, err.Error())
		return
	}

	processID := s.pm.RunCLI(req.Command, req.Args, s.paths.BuilderCLI, s.paths.ProjectDir)
	slog.Info("started CLI process", "command", req.Command, "processId", processID)
	writeJSON(w, http.StatusOK, map[string]string{
		"processId": processID,
		"status":    "started",
	})
}

type runMakeRequest struct {
	Target string            `json:"target"`
	Vars   map[string]string `json:"vars"`
}

func (s *Service) handleRunMake(w http.ResponseWriter, r *http.Request) {
	var req runMakeRequest
	if err := decodeJSON(r, &req); err != nil {
		writeError(w, http.StatusBadRequest, err.Error())
		return
	}

	processID := s.pm.RunMake(req.Target, req.Vars, s.paths.ProjectDir)
	slog.Info("started Make process", "target", req.Target, "processId", processID)
	writeJSON(w, http.StatusOK, map[string]string{
		"processId": processID,
		"status":    "started",
	})
}

type cancelRequest struct {
	ProcessID string `json:"processId"`
}

func (s *Service) handleCancelProcess(w http.ResponseWriter, r *http.Request) {
	var req cancelRequest
	if err := decodeJSON(r, &req); err != nil {
		writeError(w, http.StatusBadRequest, err.Error())
		return
	}

	if err := s.pm.Cancel(req.ProcessID); err != nil {
		writeError(w, http.StatusNotFound, err.Error())
		return
	}

	slog.Info("cancelled process", "processId", req.ProcessID)
	writeJSON(w, http.StatusOK, map[string]string{"status": "cancelled"})
}

func (s *Service) handleActiveProcesses(w http.ResponseWriter, _ *http.Request) {
	active := s.pm.Active()
	if active == nil {
		active = []string{}
	}
	writeJSON(w, http.StatusOK, map[string]any{
		"active": active,
		"count":  len(active),
	})
}

// ───────────────────────────────────────────────────────────────────────────
// Config Handlers
// ───────────────────────────────────────────────────────────────────────────

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

	// Read existing YAML as document (preserves comments/ordering)
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

	// Parse the incoming section data
	var sectionNode yaml.Node
	if err := yaml.Unmarshal(body, &sectionNode); err != nil {
		writeError(w, http.StatusBadRequest, fmt.Sprintf("invalid YAML/JSON: %s", err))
		return
	}

	// Find and replace the section in the document
	updated := false
	if doc.Kind == yaml.DocumentNode && len(doc.Content) > 0 {
		mapping := doc.Content[0]
		if mapping.Kind == yaml.MappingNode {
			for i := 0; i < len(mapping.Content)-1; i += 2 {
				if mapping.Content[i].Value == section {
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

func (s *Service) handleReadPreferences(w http.ResponseWriter, _ *http.Request) {
	prefs, err := config.ReadPreferences()
	if err != nil {
		writeError(w, http.StatusInternalServerError, err.Error())
		return
	}
	writeJSON(w, http.StatusOK, prefs)
}

func (s *Service) handleWritePreferences(w http.ResponseWriter, r *http.Request) {
	var prefs map[string]any
	if err := decodeJSON(r, &prefs); err != nil {
		writeError(w, http.StatusBadRequest, err.Error())
		return
	}

	if err := config.WritePreferences(prefs); err != nil {
		writeError(w, http.StatusInternalServerError, err.Error())
		return
	}

	writeJSON(w, http.StatusOK, map[string]string{"status": "saved"})
}

// ───────────────────────────────────────────────────────────────────────────
// Helpers
// ───────────────────────────────────────────────────────────────────────────

func decodeJSON(r *http.Request, v any) error {
	if r.Body == nil {
		return fmt.Errorf("request body is empty")
	}
	defer r.Body.Close()
	dec := json.NewDecoder(r.Body)
	return dec.Decode(v)
}

func writeJSON(w http.ResponseWriter, status int, v any) {
	w.Header().Set("Content-Type", "application/json")
	w.WriteHeader(status)
	json.NewEncoder(w).Encode(v)
}

func writeError(w http.ResponseWriter, status int, msg string) {
	w.Header().Set("Content-Type", "application/json")
	w.WriteHeader(status)
	json.NewEncoder(w).Encode(map[string]string{"error": msg})
}

// ═══════════════════════════════════════════════════════════════════════════
// CLOSING
// ═══════════════════════════════════════════════════════════════════════════
