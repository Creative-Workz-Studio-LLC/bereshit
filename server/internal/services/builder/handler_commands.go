// ═══════════════════════════════════════════════════════════════════════════
// METADATA
// ═══════════════════════════════════════════════════════════════════════════
//
// Key: cws-server-builder-handler-commands
// Purpose: HTTP handlers for builder CLI command execution
// Biblical: Ecclesiastes 9:10 — "Whatsoever thy hand findeth to do, do it with thy might"
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
	"log/slog"
	"net/http"

	"cws.studio/server/internal/server"
)

// ═══════════════════════════════════════════════════════════════════════════
// BODY
// ═══════════════════════════════════════════════════════════════════════════

// ───────────────────────────────────────────────────────────────────────────
// Command Execution — spawn CLI/Make processes, stream output via WebSocket
// ───────────────────────────────────────────────────────────────────────────

// runCommandRequest is the JSON body for POST /api/builder/commands/run.
type runCommandRequest struct {
	Command string   `json:"command"` // Builder CLI command (e.g., "lint", "spine", "build")
	Args    []string `json:"args"`    // Additional arguments passed to the CLI
}

// handleRunCommand spawns a builder CLI command and returns the processId.
// Output streams asynchronously via WebSocket as OutputMessage events.
func (s *Service) handleRunCommand(w http.ResponseWriter, r *http.Request) {
	var req runCommandRequest
	if err := server.DecodeJSON(r, &req); err != nil {
		server.WriteError(w, http.StatusBadRequest, err.Error())
		return
	}

	processID := s.pm.RunCLI(req.Command, req.Args, s.paths.BuilderCLI, s.paths.ProjectDir)
	slog.Info("started CLI process", "command", req.Command, "processId", processID)
	server.WriteJSON(w, http.StatusOK, map[string]string{
		"processId": processID,
		"status":    "started",
	})
}

// runMakeRequest is the JSON body for POST /api/builder/commands/make.
type runMakeRequest struct {
	Target string            `json:"target"` // Makefile target (e.g., "book", "lint", "section")
	Vars   map[string]string `json:"vars"`   // Make variables (e.g., {"FILE": "preface.adoc"})
}

// handleRunMake spawns a Makefile target and returns the processId.
// Output streams asynchronously via WebSocket as OutputMessage events.
func (s *Service) handleRunMake(w http.ResponseWriter, r *http.Request) {
	var req runMakeRequest
	if err := server.DecodeJSON(r, &req); err != nil {
		server.WriteError(w, http.StatusBadRequest, err.Error())
		return
	}

	processID := s.pm.RunMake(req.Target, req.Vars, s.paths.ProjectDir)
	slog.Info("started Make process", "target", req.Target, "processId", processID)
	server.WriteJSON(w, http.StatusOK, map[string]string{
		"processId": processID,
		"status":    "started",
	})
}

// cancelRequest is the JSON body for POST /api/builder/commands/cancel.
type cancelRequest struct {
	ProcessID string `json:"processId"` // ID of the process to cancel
}

// handleCancelProcess sends SIGTERM to a running process.
func (s *Service) handleCancelProcess(w http.ResponseWriter, r *http.Request) {
	var req cancelRequest
	if err := server.DecodeJSON(r, &req); err != nil {
		server.WriteError(w, http.StatusBadRequest, err.Error())
		return
	}

	if err := s.pm.Cancel(req.ProcessID); err != nil {
		server.WriteError(w, http.StatusNotFound, err.Error())
		return
	}

	slog.Info("cancelled process", "processId", req.ProcessID)
	server.WriteJSON(w, http.StatusOK, map[string]string{"status": "cancelled"})
}

// handleActiveProcesses returns the list of currently running process IDs.
func (s *Service) handleActiveProcesses(w http.ResponseWriter, _ *http.Request) {
	active := s.pm.Active()
	if active == nil {
		active = []string{}
	}
	server.WriteJSON(w, http.StatusOK, map[string]any{
		"active": active,
		"count":  len(active),
	})
}

// ═══════════════════════════════════════════════════════════════════════════
// CLOSING
// ═══════════════════════════════════════════════════════════════════════════
