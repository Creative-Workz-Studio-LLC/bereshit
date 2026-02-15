// ============================================================================
// METADATA - CPI-SI Dashboard Module
// ============================================================================
// Purpose: Web dashboard server for CPI-SI state visualization
// Biblical: Habakkuk 2:2 - "Write the vision, and make it plain upon tables"

module cws.studio/dashboard

go 1.25.0

require (
	cws.studio/pkg v0.0.0
	github.com/gorilla/websocket v1.5.3
)

require (
	github.com/BurntSushi/toml v1.6.0 // indirect
	github.com/dustin/go-humanize v1.0.1 // indirect
	github.com/fsnotify/fsnotify v1.9.0 // indirect
	github.com/google/uuid v1.6.0 // indirect
	github.com/mattn/go-isatty v0.0.20 // indirect
	github.com/ncruces/go-strftime v1.0.0 // indirect
	github.com/remyoudompheng/bigfft v0.0.0-20230129092748-24d4a6f8daec // indirect
	golang.org/x/exp v0.0.0-20260212183809-81e46e3db34a // indirect
	golang.org/x/sys v0.41.0 // indirect
	modernc.org/libc v1.67.7 // indirect
	modernc.org/mathutil v1.7.1 // indirect
	modernc.org/memory v1.11.0 // indirect
	modernc.org/sqlite v1.45.0 // indirect
)

replace cws.studio/pkg => ../pkg
