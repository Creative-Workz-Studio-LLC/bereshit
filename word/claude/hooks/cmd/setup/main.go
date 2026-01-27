// Setup hook entry point
// Triggered via: --init, --init-only, --maintenance CLI flags
package main

import "cws.studio/claude/hooks/session"

func main() { session.Setup() }
