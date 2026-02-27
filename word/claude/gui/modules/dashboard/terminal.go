// ============================================================================
// METADATA
// ============================================================================
//
//omni:key   cws-gui-modules-dashboard-terminal
//omni:code  --go -package->dashboard
//omni:version a-00.10
//
// Purpose: LogEvent -> render.LogEntry translation for the message terminal.
// Biblical: Isaiah 30:8 - "Now go, write it before them in a table"
//
// ============================================================================
// END METADATA

package dashboard

// ============================================================================
// SETUP
// ============================================================================

import (
	"cws.studio/gui/render"
	"cws.studio/pkg/dashboard"
)

// ============================================================================
// END SETUP

// ============================================================================
// BODY
// ============================================================================

// LogEventToEntry converts a dashboard LogEvent to a render LogEntry.
func LogEventToEntry(event dashboard.LogEvent) render.LogEntry {
	return render.LogEntry{
		Timestamp: event.Timestamp,
		Level:     mapLogLevel(event.Level),
		Category:  event.Component,
		Message:   event.Message,
	}
}

// mapLogLevel converts a dashboard log level string to render.LogLevel.
func mapLogLevel(level string) render.LogLevel {
	switch level {
	case "DEBUG":
		return render.LogDebug
	case "INFO":
		return render.LogInfo
	case "WARN":
		return render.LogWarn
	case "ERROR":
		return render.LogError
	case "SUCCESS":
		return render.LogSuccess
	default:
		return render.LogInfo
	}
}

// ============================================================================
// END BODY

// ============================================================================
// CLOSING
// ============================================================================
// ============================================================================
// END CLOSING
