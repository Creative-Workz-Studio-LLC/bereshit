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

	"cws.studio/pkg/util/fs/paths"
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
//
// On production servers, runtime state files don't exist (no Claude Code hooks
// writing state). The watcher gracefully skips missing directories — the
// DashboardService still provides full CRUD and search without real-time updates.
func (fw *FileWatcher) WatchRuntimeFiles(ctx context.Context) error {
	watching := 0

	// Watch the runtime state directory (may not exist on production)
	runtimeDir := filepath.Dir(paths.StateMachineRuntimeState())
	if err := fw.watcher.Add(runtimeDir); err != nil {
		// Non-fatal — runtime dir doesn't exist on production servers
		_ = err
	} else {
		watching++
	}

	// Watch session log directory
	logsDir := paths.LogsSessionDir()
	if err := fw.watcher.Add(logsDir); err != nil {
		// Non-fatal — logs dir might not exist yet
		_ = err
	} else {
		watching++
	}

	// Start the watch loop even with zero watchers — it handles the
	// context cancellation cleanup. New directories can be added later.
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
