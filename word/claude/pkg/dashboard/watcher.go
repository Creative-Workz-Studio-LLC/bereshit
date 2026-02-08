// ═══════════════════════════════════════════════════════════════════════════
// METADATA
// ═══════════════════════════════════════════════════════════════════════════
//
// Key: claude-global-pkg-dashboard-watcher
// Purpose: File watcher for real-time CPI-SI state change detection
// Authors: Nova Dawn
// Version: 1.0.0
// Created: 2026-02-07
//
// ═══════════════════════════════════════════════════════════════════════════

package dashboard

// ═══════════════════════════════════════════════════════════════════════════
// SETUP
// ═══════════════════════════════════════════════════════════════════════════

import (
	"context"
	"path/filepath"
	"strings"
	"time"

	"github.com/fsnotify/fsnotify"

	"github.com/creativeworkzstudio/claude-global/pkg/util/fs/paths"
)

// ═══════════════════════════════════════════════════════════════════════════
// BODY
// ═══════════════════════════════════════════════════════════════════════════

// FileWatcher uses fsnotify to watch runtime state files and session logs.
type FileWatcher struct {
	watcher  *fsnotify.Watcher
	eventBus *EventBus
}

// NewFileWatcher creates a new file watcher connected to an event bus.
func NewFileWatcher(eventBus *EventBus) (*FileWatcher, error) {
	w, err := fsnotify.NewWatcher()
	if err != nil {
		return nil, err
	}
	return &FileWatcher{
		watcher:  w,
		eventBus: eventBus,
	}, nil
}

// WatchRuntimeFiles starts watching CPI-SI runtime files.
// Watches the runtime directory for state.jsonc, path.jsonc, history.jsonc changes,
// and the session log directory for new log entries.
func (fw *FileWatcher) WatchRuntimeFiles(ctx context.Context) error {
	// Watch the runtime state directory
	runtimeDir := filepath.Dir(paths.StateMachineRuntimeState())
	if err := fw.watcher.Add(runtimeDir); err != nil {
		return err
	}

	// Watch session log directory
	logsDir := paths.LogsSessionDir()
	if err := fw.watcher.Add(logsDir); err != nil {
		// Non-fatal — logs dir might not exist yet
		_ = err
	}

	// Start the watch loop
	go fw.watchLoop(ctx)

	return nil
}

// Close stops the file watcher.
func (fw *FileWatcher) Close() {
	fw.watcher.Close()
}

// watchLoop handles fsnotify events and publishes to the event bus.
func (fw *FileWatcher) watchLoop(ctx context.Context) {
	// Debounce: state.jsonc can be written multiple times rapidly
	var debounceTimer *time.Timer
	debounceDelay := 100 * time.Millisecond

	for {
		select {
		case <-ctx.Done():
			return

		case event, ok := <-fw.watcher.Events:
			if !ok {
				return
			}

			// Only care about writes
			if !event.Has(fsnotify.Write) && !event.Has(fsnotify.Create) {
				continue
			}

			filename := filepath.Base(event.Name)

			switch {
			case filename == "state.jsonc":
				// Debounce rapid writes
				if debounceTimer != nil {
					debounceTimer.Stop()
				}
				debounceTimer = time.AfterFunc(debounceDelay, func() {
					fw.publishStateUpdate()
				})

			case filename == "path.jsonc":
				fw.eventBus.Publish(StateChange{
					Type: "path_update",
				})

			case filename == "history.jsonc":
				fw.eventBus.Publish(StateChange{
					Type: "history_update",
				})

			case strings.HasSuffix(filename, ".jsonl"):
				// Session log file updated — tail new events
				fw.publishLogEvents(event.Name)
			}

		case _, ok := <-fw.watcher.Errors:
			if !ok {
				return
			}
			// Log errors but don't crash
		}
	}
}

// publishStateUpdate reads current state and publishes a snapshot.
func (fw *FileWatcher) publishStateUpdate() {
	snap, err := BuildStateSnapshot()
	if err != nil {
		return
	}
	fw.eventBus.Publish(StateChange{
		Type:     "state_update",
		Snapshot: snap,
	})
}

// publishLogEvents reads new lines from a session log file.
func (fw *FileWatcher) publishLogEvents(logPath string) {
	events, err := TailLogFile(logPath, 10)
	if err != nil {
		return
	}
	for _, evt := range events {
		fw.eventBus.Publish(StateChange{
			Type:  "log_event",
			Event: &evt,
		})
	}
}

// ═══════════════════════════════════════════════════════════════════════════
// CLOSING
// ═══════════════════════════════════════════════════════════════════════════
