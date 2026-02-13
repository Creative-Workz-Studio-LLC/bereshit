// ═══════════════════════════════════════════════════════════════════════════
// METADATA
// ═══════════════════════════════════════════════════════════════════════════
//
// Key: cws-ops-ssh
// Purpose: SSH command execution via os/exec — reuses operator's SSH config
// Biblical: Isaiah 22:22 — "the key of the house of David will I lay upon
//           his shoulder; so he shall open, and none shall shut"
// Authors: Nova Dawn
// Version: 1.0.0
// Created: 2026-02-12
//
// ═══════════════════════════════════════════════════════════════════════════

package ops

// ═══════════════════════════════════════════════════════════════════════════
// SETUP
// ═══════════════════════════════════════════════════════════════════════════

import (
	"bytes"
	"context"
	"fmt"
	"io"
	"os"
	"os/exec"
	"strings"
	"time"

	"cws.studio/server/internal/config"
)

// ═══════════════════════════════════════════════════════════════════════════
// BODY
// ═══════════════════════════════════════════════════════════════════════════

// RunSSH executes a command on the remote host via SSH and returns the
// captured output. The context controls timeout/cancellation.
//
// Uses exec-based SSH so the operator's ~/.ssh/config, keys, and agent
// are inherited automatically — no key management needed.
func RunSSH(ctx context.Context, host config.Host, command string) (ExecResult, error) {
	start := time.Now()

	cmd := exec.CommandContext(ctx, "ssh",
		"-o", "ConnectTimeout=10",
		"-o", "BatchMode=yes",
		host.SSHTarget(),
		command,
	)

	var stdout, stderr bytes.Buffer
	cmd.Stdout = &stdout
	cmd.Stderr = &stderr

	err := cmd.Run()

	result := ExecResult{
		Stdout:   strings.TrimSpace(stdout.String()),
		Stderr:   strings.TrimSpace(stderr.String()),
		Duration: time.Since(start),
	}

	if err != nil {
		if exitErr, ok := err.(*exec.ExitError); ok {
			result.ExitCode = exitErr.ExitCode()
			return result, nil // Non-zero exit is not an error — caller decides
		}
		return result, fmt.Errorf("ssh exec failed: %w", err)
	}

	return result, nil
}

// RunSSHCapture is a convenience wrapper that returns stdout directly.
// Returns an error if the command exits non-zero.
func RunSSHCapture(ctx context.Context, host config.Host, command string) (string, error) {
	result, err := RunSSH(ctx, host, command)
	if err != nil {
		return "", err
	}
	if !result.OK() {
		return result.Stdout, fmt.Errorf("command exited %d: %s", result.ExitCode, result.Stderr)
	}
	return result.Stdout, nil
}

// StreamSSH executes a command on the remote host and streams output to
// the provided writer in real time. Used for log tailing and deploy output.
// Returns when the command completes or context is cancelled.
func StreamSSH(ctx context.Context, host config.Host, command string, w io.Writer) error {
	cmd := exec.CommandContext(ctx, "ssh",
		"-o", "ConnectTimeout=10",
		"-o", "BatchMode=yes",
		"-t", "-t", // Force PTY for streaming
		host.SSHTarget(),
		command,
	)

	cmd.Stdout = w
	cmd.Stderr = w

	return cmd.Run()
}

// InteractiveSSH opens an interactive SSH session to the host, connecting
// the user's terminal directly. Used by the "ssh" subcommand.
func InteractiveSSH(host config.Host) error {
	cmd := exec.Command("ssh", host.SSHTarget())
	cmd.Stdin = os.Stdin
	cmd.Stdout = os.Stdout
	cmd.Stderr = os.Stderr
	return cmd.Run()
}

// ═══════════════════════════════════════════════════════════════════════════
// CLOSING
// ═══════════════════════════════════════════════════════════════════════════
