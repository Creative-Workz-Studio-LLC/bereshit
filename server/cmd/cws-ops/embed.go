// ═══════════════════════════════════════════════════════════════════════════
// METADATA
// ═══════════════════════════════════════════════════════════════════════════
//
// Key: cws-ops-embed
// Purpose: Embed compiled frontend assets into the binary
// Authors: Nova Dawn
// Version: 1.0.0
// Created: 2026-02-12
//
// ═══════════════════════════════════════════════════════════════════════════

package main

import "embed"

// StaticFS holds the compiled frontend assets (Vite build output).
// The static/ directory is populated by `npm run build` in ops-frontend/.
// If the directory is empty, the serve command will note that the frontend
// needs to be built first.
//
//go:embed all:static
var StaticFS embed.FS
