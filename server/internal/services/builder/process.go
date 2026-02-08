// ═══════════════════════════════════════════════════════════════════════════
// METADATA
// ═══════════════════════════════════════════════════════════════════════════
//
// Key: cws-server-builder-process
// Purpose: CLI process spawning, output streaming, and cancellation
// Biblical: Isaiah 28:16 — "He that believeth shall not make haste"
// Authors: Nova Dawn
// Version: 1.1.0
// Created: 2026-02-08
//
// ═══════════════════════════════════════════════════════════════════════════

package builder

// ═══════════════════════════════════════════════════════════════════════════
// SETUP
// ═══════════════════════════════════════════════════════════════════════════

import (
	"bufio"
	"fmt"
	"log/slog"
	"math/rand"
	"os/exec"
	"regexp"
	"strconv"
	"sync"
	"syscall"
	"time"

	"cws.studio/server/internal/ws"
)

// ═══════════════════════════════════════════════════════════════════════════
// BODY
// ═══════════════════════════════════════════════════════════════════════════

// ───────────────────────────────────────────────────────────────────────────
// Types — WebSocket message formats and process tracking
// ───────────────────────────────────────────────────────────────────────────

// ansiRegex matches ANSI escape sequences (colors, cursor movement, etc.).
// Stripped from process output before broadcasting — raw ANSI in JSON payloads
// causes rendering issues in browser consoles.
var ansiRegex = regexp.MustCompile(`\x1B\[[0-9;]*[a-zA-Z]`)

func stripAnsi(s string) string {
	return ansiRegex.ReplaceAllString(s, "")
}

// OutputMessage is broadcast via WebSocket during process execution.
// Each line of stdout/stderr becomes one message.
type OutputMessage struct {
	Type      string `json:"type"`      // Always "output"
	ProcessID string `json:"processId"` // Unique process identifier
	Stream    string `json:"stream"`    // "stdout" or "stderr"
	Line      string `json:"line"`      // Single line of output (ANSI stripped)
	Timestamp string `json:"timestamp"` // RFC3339Nano timestamp
}

// ExitMessage is broadcast via WebSocket when a process completes.
type ExitMessage struct {
	Type      string `json:"type"`      // Always "exit"
	ProcessID string `json:"processId"` // Unique process identifier
	ExitCode  int    `json:"exitCode"`  // Process exit code (0 = success)
	Success   bool   `json:"success"`   // Convenience: exitCode == 0
}

// ProcessInfo tracks a running child process.
type ProcessInfo struct {
	ID      string    // Unique process identifier
	Cmd     *exec.Cmd // The OS process handle
	Started time.Time // When the process was spawned
}

// ProcessManager handles spawning, tracking, and cancelling CLI processes.
// It uses a sync.Map for thread-safe concurrent access from HTTP handlers
// and the process completion goroutines.
type ProcessManager struct {
	processes sync.Map // map[string]*ProcessInfo
	hub       *ws.Hub  // WebSocket hub for broadcasting output
}

// NewProcessManager creates a new process manager that broadcasts to the given hub.
func NewProcessManager(hub *ws.Hub) *ProcessManager {
	return &ProcessManager{hub: hub}
}

// generateID creates a unique process identifier.
// Format: base36(milliseconds) + base36(random) — matches Express server format
// for frontend compatibility.
func generateID() string {
	now := strconv.FormatInt(time.Now().UnixMilli(), 36)
	suffix := strconv.FormatInt(rand.Int63n(1_000_000), 36)
	return now + suffix
}

// ───────────────────────────────────────────────────────────────────────────
// Process Spawning — shared lifecycle for CLI and Make processes
// ───────────────────────────────────────────────────────────────────────────

// spawnProcess starts a prepared command, pipes stdout/stderr to WebSocket,
// and broadcasts an ExitMessage when the process completes.
//
// The command must have Dir and SysProcAttr already set. Returns the processId
// immediately; output streams asynchronously via goroutines.
//
// Process group isolation: Commands run with Setpgid=true so the entire
// process group (including any child processes) can be killed with a single
// signal using the negative PID convention: kill(-pgid, SIGTERM).
func (pm *ProcessManager) spawnProcess(cmd *exec.Cmd) string {
	processID := generateID()

	stdout, err := cmd.StdoutPipe()
	if err != nil {
		slog.Error("failed to create stdout pipe", "error", err)
		return processID
	}
	stderr, err := cmd.StderrPipe()
	if err != nil {
		slog.Error("failed to create stderr pipe", "error", err)
		return processID
	}

	if err := cmd.Start(); err != nil {
		slog.Error("failed to start process", "processId", processID, "error", err)
		pm.hub.Broadcast(ExitMessage{
			Type:      "exit",
			ProcessID: processID,
			ExitCode:  -1,
			Success:   false,
		})
		return processID
	}

	pm.processes.Store(processID, &ProcessInfo{
		ID:      processID,
		Cmd:     cmd,
		Started: time.Now(),
	})

	// Stream stdout and stderr concurrently via WebSocket.
	go pm.streamPipe(processID, "stdout", bufio.NewScanner(stdout))
	go pm.streamPipe(processID, "stderr", bufio.NewScanner(stderr))

	// Wait for completion in background, then broadcast exit status.
	go func() {
		err := cmd.Wait()
		pm.processes.Delete(processID)

		exitCode := 0
		if err != nil {
			if exitErr, ok := err.(*exec.ExitError); ok {
				exitCode = exitErr.ExitCode()
			} else {
				exitCode = -1
			}
		}

		pm.hub.Broadcast(ExitMessage{
			Type:      "exit",
			ProcessID: processID,
			ExitCode:  exitCode,
			Success:   exitCode == 0,
		})

		slog.Info("process completed", "processId", processID, "exitCode", exitCode)
	}()

	return processID
}

// RunCLI spawns a builder CLI command (node cws-build <command> <args...>)
// and streams output via WebSocket. Returns the processId immediately.
func (pm *ProcessManager) RunCLI(command string, args []string, cliPath, cwd string) string {
	cmdArgs := append([]string{cliPath, command}, args...)
	cmd := exec.Command("node", cmdArgs...)
	cmd.Dir = cwd
	cmd.SysProcAttr = &syscall.SysProcAttr{Setpgid: true}
	return pm.spawnProcess(cmd)
}

// RunMake spawns a Makefile target and streams output via WebSocket.
// Returns the processId immediately.
func (pm *ProcessManager) RunMake(target string, vars map[string]string, cwd string) string {
	makeArgs := []string{"-C", cwd, target}
	for k, v := range vars {
		makeArgs = append(makeArgs, fmt.Sprintf("%s=%s", k, v))
	}
	cmd := exec.Command("make", makeArgs...)
	cmd.Dir = cwd
	cmd.SysProcAttr = &syscall.SysProcAttr{Setpgid: true}
	return pm.spawnProcess(cmd)
}

// ───────────────────────────────────────────────────────────────────────────
// Output Streaming — pipe process output to WebSocket hub
// ───────────────────────────────────────────────────────────────────────────

// streamPipe reads lines from a scanner and broadcasts each as an OutputMessage.
// ANSI escape sequences are stripped before broadcasting — raw escape codes
// in JSON cause rendering issues in browser developer consoles.
// Buffer is set to 1MB to handle long output lines (e.g., compiler errors).
func (pm *ProcessManager) streamPipe(processID, stream string, scanner *bufio.Scanner) {
	scanner.Buffer(make([]byte, 64*1024), 1024*1024)

	for scanner.Scan() {
		line := stripAnsi(scanner.Text())
		if line == "" {
			continue
		}
		pm.hub.Broadcast(OutputMessage{
			Type:      "output",
			ProcessID: processID,
			Stream:    stream,
			Line:      line,
			Timestamp: time.Now().UTC().Format(time.RFC3339Nano),
		})
	}
}

// ───────────────────────────────────────────────────────────────────────────
// Process Management — cancel and query running processes
// ───────────────────────────────────────────────────────────────────────────

// Cancel sends SIGTERM to a running process and its entire process group.
// Uses negative PID (-pgid) to also terminate any child processes.
func (pm *ProcessManager) Cancel(processID string) error {
	val, ok := pm.processes.Load(processID)
	if !ok {
		return fmt.Errorf("process %s not found", processID)
	}

	info := val.(*ProcessInfo)
	if info.Cmd.Process != nil {
		syscall.Kill(-info.Cmd.Process.Pid, syscall.SIGTERM)
	}
	pm.processes.Delete(processID)
	return nil
}

// Active returns the list of currently running process IDs.
func (pm *ProcessManager) Active() []string {
	var ids []string
	pm.processes.Range(func(key, _ any) bool {
		ids = append(ids, key.(string))
		return true
	})
	return ids
}

// CancelAll sends SIGTERM to all running processes. Used during graceful shutdown.
func (pm *ProcessManager) CancelAll() {
	pm.processes.Range(func(key, value any) bool {
		info := value.(*ProcessInfo)
		if info.Cmd.Process != nil {
			syscall.Kill(-info.Cmd.Process.Pid, syscall.SIGTERM)
		}
		pm.processes.Delete(key)
		return true
	})
}

// ═══════════════════════════════════════════════════════════════════════════
// CLOSING
// ═══════════════════════════════════════════════════════════════════════════
