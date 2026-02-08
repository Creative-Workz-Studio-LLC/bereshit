// ═══════════════════════════════════════════════════════════════════════════
// METADATA
// ═══════════════════════════════════════════════════════════════════════════
//
// Key: cws-server-builder-process
// Purpose: CLI process spawning, output streaming, and cancellation
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

var ansiRegex = regexp.MustCompile(`\x1B\[[0-9;]*[a-zA-Z]`)

func stripAnsi(s string) string {
	return ansiRegex.ReplaceAllString(s, "")
}

// OutputMessage is broadcast via WebSocket during process execution.
type OutputMessage struct {
	Type      string `json:"type"`
	ProcessID string `json:"processId"`
	Stream    string `json:"stream"`
	Line      string `json:"line"`
	Timestamp string `json:"timestamp"`
}

// ExitMessage is broadcast via WebSocket when a process completes.
type ExitMessage struct {
	Type      string `json:"type"`
	ProcessID string `json:"processId"`
	ExitCode  int    `json:"exitCode"`
	Success   bool   `json:"success"`
}

// ProcessInfo tracks a running child process.
type ProcessInfo struct {
	ID      string
	Cmd     *exec.Cmd
	Started time.Time
}

// ProcessManager handles spawning, tracking, and cancelling CLI processes.
type ProcessManager struct {
	processes sync.Map // map[string]*ProcessInfo
	hub       *ws.Hub
}

// NewProcessManager creates a new process manager that broadcasts to the given hub.
func NewProcessManager(hub *ws.Hub) *ProcessManager {
	return &ProcessManager{hub: hub}
}

// generateID creates a unique process identifier (matches Express format).
func generateID() string {
	now := strconv.FormatInt(time.Now().UnixMilli(), 36)
	suffix := strconv.FormatInt(rand.Int63n(1_000_000), 36)
	return now + suffix
}

// RunCLI spawns a builder CLI command and streams output via WebSocket.
// Returns the processId immediately; output streams asynchronously.
func (pm *ProcessManager) RunCLI(command string, args []string, cliPath, cwd string) string {
	processID := generateID()

	cmdArgs := append([]string{cliPath, command}, args...)
	cmd := exec.Command("node", cmdArgs...)
	cmd.Dir = cwd
	cmd.SysProcAttr = &syscall.SysProcAttr{Setpgid: true}

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
		slog.Error("failed to start process", "command", command, "error", err)
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

	// Stream stdout
	go pm.streamPipe(processID, "stdout", bufio.NewScanner(stdout))

	// Stream stderr
	go pm.streamPipe(processID, "stderr", bufio.NewScanner(stderr))

	// Wait for completion
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

// RunMake spawns a Makefile target and streams output via WebSocket.
func (pm *ProcessManager) RunMake(target string, vars map[string]string, cwd string) string {
	processID := generateID()

	makeArgs := []string{"-C", cwd, target}
	for k, v := range vars {
		makeArgs = append(makeArgs, fmt.Sprintf("%s=%s", k, v))
	}

	cmd := exec.Command("make", makeArgs...)
	cmd.Dir = cwd
	cmd.SysProcAttr = &syscall.SysProcAttr{Setpgid: true}

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
		slog.Error("failed to start make", "target", target, "error", err)
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

	go pm.streamPipe(processID, "stdout", bufio.NewScanner(stdout))
	go pm.streamPipe(processID, "stderr", bufio.NewScanner(stderr))

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
	}()

	return processID
}

// streamPipe reads lines from a scanner and broadcasts each as an OutputMessage.
func (pm *ProcessManager) streamPipe(processID, stream string, scanner *bufio.Scanner) {
	// Increase buffer for long lines (1MB)
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

// Cancel sends SIGTERM to a running process.
func (pm *ProcessManager) Cancel(processID string) error {
	val, ok := pm.processes.Load(processID)
	if !ok {
		return fmt.Errorf("process %s not found", processID)
	}

	info := val.(*ProcessInfo)
	// Kill the process group to also kill children
	if info.Cmd.Process != nil {
		syscall.Kill(-info.Cmd.Process.Pid, syscall.SIGTERM)
	}
	pm.processes.Delete(processID)
	return nil
}

// Active returns the list of active process IDs.
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
