// ============================================================================
// METADATA - CPI-SI Dashboard GTK4 Module
// ============================================================================
// Purpose: GTK4 native desktop UI for CPI-SI state visualization
// Biblical: Habakkuk 2:2 - "Write the vision, and make it plain upon tables"

module cws.studio/dashboard/gtk

go 1.24.0

toolchain go1.24.4

require (
	cws.studio/pkg v0.0.0
	github.com/diamondburned/gotk4/pkg v0.3.1
)

require (
	github.com/BurntSushi/toml v1.5.0 // indirect
	github.com/KarpelesLab/weak v0.1.1 // indirect
	github.com/dustin/go-humanize v1.0.1 // indirect
	github.com/fsnotify/fsnotify v1.8.0 // indirect
	github.com/google/uuid v1.6.0 // indirect
	github.com/mattn/go-isatty v0.0.20 // indirect
	github.com/ncruces/go-strftime v1.0.0 // indirect
	github.com/remyoudompheng/bigfft v0.0.0-20230129092748-24d4a6f8daec // indirect
	go4.org/unsafe/assume-no-moving-gc v0.0.0-20231121144256-b99613f794b6 // indirect
	golang.org/x/exp v0.0.0-20251023183803-a4bb9ffd2546 // indirect
	golang.org/x/sync v0.17.0 // indirect
	golang.org/x/sys v0.37.0 // indirect
	modernc.org/libc v1.67.6 // indirect
	modernc.org/mathutil v1.7.1 // indirect
	modernc.org/memory v1.11.0 // indirect
	modernc.org/sqlite v1.44.3 // indirect
)

replace cws.studio/pkg => ../../pkg
