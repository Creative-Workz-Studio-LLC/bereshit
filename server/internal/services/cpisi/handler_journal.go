// ═══════════════════════════════════════════════════════════════════════════
// METADATA
// ═══════════════════════════════════════════════════════════════════════════
//
// Key: cws-server-cpisi-handler-journal
// Purpose: HTTP handlers for journal CRUD, search, and import
// Biblical: Psalm 102:18 — "This shall be written for the generation to come"
// Authors: Nova Dawn
// Version: 1.0.0
// Created: 2026-02-08
//
// ═══════════════════════════════════════════════════════════════════════════

package cpisi

// ═══════════════════════════════════════════════════════════════════════════
// SETUP
// ═══════════════════════════════════════════════════════════════════════════

import (
	"encoding/json"
	"fmt"
	"io"
	"log/slog"
	"net/http"
	"strings"
	"time"

	"cws.studio/server/internal/server"
)

// ═══════════════════════════════════════════════════════════════════════════
// BODY
// ═══════════════════════════════════════════════════════════════════════════

// ───────────────────────────────────────────────────────────────────────────
// Journal CRUD — create, read, update, delete journal entries
// ───────────────────────────────────────────────────────────────────────────

// handleCreateJournal creates a new journal entry.
// Requires title, content, and type (bible-study|personal|instance|universal).
func (s *Service) handleCreateJournal(w http.ResponseWriter, r *http.Request) {
	var entry JournalEntry
	if err := server.DecodeJSON(r, &entry); err != nil {
		server.WriteError(w, http.StatusBadRequest, err.Error())
		return
	}

	if entry.Title == "" || entry.Content == "" || entry.Type == "" {
		server.WriteError(w, http.StatusBadRequest, "title, content, and type are required")
		return
	}

	created, err := s.journals.Create(&entry)
	if err != nil {
		server.WriteError(w, http.StatusInternalServerError, err.Error())
		return
	}

	slog.Info("journal created", "id", created.ID, "type", created.Type, "title", created.Title)
	server.WriteJSON(w, http.StatusCreated, created)
}

// handleGetJournal returns a single journal entry by ID.
func (s *Service) handleGetJournal(w http.ResponseWriter, r *http.Request) {
	id := r.PathValue("id")
	entry, err := s.journals.Get(id)
	if err != nil {
		if strings.Contains(err.Error(), "not found") {
			server.WriteError(w, http.StatusNotFound, err.Error())
		} else {
			server.WriteError(w, http.StatusInternalServerError, err.Error())
		}
		return
	}
	server.WriteJSON(w, http.StatusOK, entry)
}

// handleListJournals returns filtered journal entries with pagination.
// Query params: type, tag, from, to, session, limit, offset.
func (s *Service) handleListJournals(w http.ResponseWriter, r *http.Request) {
	q := JournalQuery{
		Type:      r.URL.Query().Get("type"),
		Tag:       r.URL.Query().Get("tag"),
		DateFrom:  r.URL.Query().Get("from"),
		DateTo:    r.URL.Query().Get("to"),
		SessionID: r.URL.Query().Get("session"),
		Limit:     server.QueryInt(r, "limit", 50),
		Offset:    server.QueryInt(r, "offset", 0),
	}

	entries, err := s.journals.List(q)
	if err != nil {
		server.WriteError(w, http.StatusInternalServerError, err.Error())
		return
	}

	server.WriteJSON(w, http.StatusOK, map[string]any{
		"entries": entries,
		"count":   len(entries),
		"query":   q,
	})
}

// handleUpdateJournal updates an existing journal entry by ID.
// The ID from the URL path takes precedence over any ID in the body.
func (s *Service) handleUpdateJournal(w http.ResponseWriter, r *http.Request) {
	id := r.PathValue("id")

	var entry JournalEntry
	if err := server.DecodeJSON(r, &entry); err != nil {
		server.WriteError(w, http.StatusBadRequest, err.Error())
		return
	}
	entry.ID = id

	updated, err := s.journals.Update(&entry)
	if err != nil {
		if strings.Contains(err.Error(), "not found") {
			server.WriteError(w, http.StatusNotFound, err.Error())
		} else {
			server.WriteError(w, http.StatusInternalServerError, err.Error())
		}
		return
	}

	slog.Info("journal updated", "id", updated.ID)
	server.WriteJSON(w, http.StatusOK, updated)
}

// handleDeleteJournal removes a journal entry by ID.
// The FTS5 index is automatically updated via database triggers.
func (s *Service) handleDeleteJournal(w http.ResponseWriter, r *http.Request) {
	id := r.PathValue("id")
	if err := s.journals.Delete(id); err != nil {
		if strings.Contains(err.Error(), "not found") {
			server.WriteError(w, http.StatusNotFound, err.Error())
		} else {
			server.WriteError(w, http.StatusInternalServerError, err.Error())
		}
		return
	}

	slog.Info("journal deleted", "id", id)
	server.WriteJSON(w, http.StatusOK, map[string]string{"status": "deleted", "id": id})
}

// ───────────────────────────────────────────────────────────────────────────
// Search & Stats — FTS5 full-text search and aggregation
// ───────────────────────────────────────────────────────────────────────────

// handleSearchJournals performs FTS5 full-text search across journal entries.
// Searches title and content fields, ranked by relevance.
func (s *Service) handleSearchJournals(w http.ResponseWriter, r *http.Request) {
	query := r.URL.Query().Get("q")
	if query == "" {
		server.WriteError(w, http.StatusBadRequest, "query parameter 'q' is required")
		return
	}

	limit := server.QueryInt(r, "limit", 20)
	entries, err := s.journals.Search(query, limit)
	if err != nil {
		server.WriteError(w, http.StatusInternalServerError, err.Error())
		return
	}

	server.WriteJSON(w, http.StatusOK, map[string]any{
		"entries": entries,
		"count":   len(entries),
		"query":   query,
	})
}

// handleJournalStats returns entry counts grouped by journal type.
func (s *Service) handleJournalStats(w http.ResponseWriter, _ *http.Request) {
	stats, err := s.journals.Stats()
	if err != nil {
		server.WriteError(w, http.StatusInternalServerError, err.Error())
		return
	}
	server.WriteJSON(w, http.StatusOK, stats)
}

// ───────────────────────────────────────────────────────────────────────────
// Import — bulk import existing markdown journals
// ───────────────────────────────────────────────────────────────────────────

// importEntry is the JSON format for bulk importing existing markdown journals.
type importEntry struct {
	Type      string `json:"type"`      // Journal type (bible-study, personal, etc.)
	Title     string `json:"title"`     // Entry title
	Content   string `json:"content"`   // Full markdown content
	Tags      string `json:"tags"`      // Comma-separated tags
	Filename  string `json:"filename"`  // Original filename (for error reporting)
	CreatedAt string `json:"createdAt"` // Date string in YYYY-MM-DD format
}

// handleImportJournals accepts a JSON array of importEntry objects and creates
// journal entries for each. Reports created/failed counts and any errors.
// Date parsing uses YYYY-MM-DD format (extracted from filename pattern).
func (s *Service) handleImportJournals(w http.ResponseWriter, r *http.Request) {
	body, err := io.ReadAll(r.Body)
	if err != nil {
		server.WriteError(w, http.StatusBadRequest, "failed to read body")
		return
	}
	defer r.Body.Close()

	var imports []importEntry
	if err := json.Unmarshal(body, &imports); err != nil {
		server.WriteError(w, http.StatusBadRequest, fmt.Sprintf("invalid JSON: %s", err))
		return
	}

	var created, failed int
	var errors []string

	for _, imp := range imports {
		entry := &JournalEntry{
			Type:    imp.Type,
			Title:   imp.Title,
			Content: imp.Content,
			Tags:    splitTags(imp.Tags),
		}

		// Parse date from filename pattern: YYYY-MM-DD_title.md
		if imp.CreatedAt != "" {
			if t, err := time.Parse("2006-01-02", imp.CreatedAt); err == nil {
				entry.CreatedAt = t
			}
		}

		if _, err := s.journals.Create(entry); err != nil {
			failed++
			errors = append(errors, fmt.Sprintf("%s: %s", imp.Filename, err))
		} else {
			created++
		}
	}

	slog.Info("journal import complete", "created", created, "failed", failed)
	server.WriteJSON(w, http.StatusOK, map[string]any{
		"created": created,
		"failed":  failed,
		"errors":  errors,
	})
}

// ═══════════════════════════════════════════════════════════════════════════
// CLOSING
// ═══════════════════════════════════════════════════════════════════════════
