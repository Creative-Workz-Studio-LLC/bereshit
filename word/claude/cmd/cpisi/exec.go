// ============================================================================
// METADATA - CPI-SI CLI Execution Helpers
// ============================================================================
//
// Purpose: Run external commands, formatted output, filesystem helpers
// Authors: Nova Dawn
// Version: 1.0.0
// Created: 2026-02-08
//
// ============================================================================

package main

// ============================================================================
// SETUP
// ============================================================================

import (
	"fmt"
	"os"
	"os/exec"
	"path/filepath"
	"strings"
)

// ============================================================================
// BODY
// ============================================================================

// runGo runs a go command in the given directory.
func runGo(dir string, args ...string) error {
	return runCmd(dir, "go", args...)
}

// runCmd runs a command in the given directory with output forwarded.
func runCmd(dir string, name string, args ...string) error {
	cmd := exec.Command(name, args...)
	cmd.Dir = dir
	cmd.Stdout = os.Stdout
	cmd.Stderr = os.Stderr
	cmd.Env = os.Environ()

	if isVerbose() {
		fmt.Printf("  > %s %s (in %s)\n", name, strings.Join(args, " "), dir)
	}

	return cmd.Run()
}

// runCmdSilent runs a command and returns its combined output.
func runCmdSilent(dir string, name string, args ...string) (string, error) {
	cmd := exec.Command(name, args...)
	cmd.Dir = dir
	cmd.Env = os.Environ()
	out, err := cmd.CombinedOutput()
	return strings.TrimSpace(string(out)), err
}

// execBinary replaces the current process with the given binary.
func execBinary(binPath string, args []string) error {
	argv := append([]string{binPath}, args...)
	return syscallExec(binPath, argv, os.Environ())
}

// syscallExec wraps syscall.Exec for testability.
var syscallExec = defaultSyscallExec

func defaultSyscallExec(argv0 string, argv []string, envv []string) error {
	cmd := exec.Command(argv0, argv[1:]...)
	cmd.Stdin = os.Stdin
	cmd.Stdout = os.Stdout
	cmd.Stderr = os.Stderr
	cmd.Env = envv
	return cmd.Run()
}

// isVerbose checks if verbose mode is enabled.
func isVerbose() bool {
	return os.Getenv("CPISI_VERBOSE") == "1"
}

// copyFile copies a file from src to dst with the given permissions.
func copyFile(src, dst string, perm os.FileMode) error {
	data, err := os.ReadFile(src)
	if err != nil {
		return err
	}
	if err := os.MkdirAll(filepath.Dir(dst), 0755); err != nil {
		return err
	}
	return os.WriteFile(dst, data, perm)
}

// fileExists checks if a file exists.
func fileExists(path string) bool {
	_, err := os.Stat(path)
	return err == nil
}

// isExecutable checks if a file exists and is executable.
func isExecutable(path string) bool {
	info, err := os.Stat(path)
	if err != nil {
		return false
	}
	return info.Mode()&0111 != 0
}

// isSymlink checks if a path is a symbolic link.
func isSymlink(path string) bool {
	info, err := os.Lstat(path)
	if err != nil {
		return false
	}
	return info.Mode()&os.ModeSymlink != 0
}

// Output formatting helpers.

func header(msg string) {
	fmt.Printf("\n=== %s ===\n", msg)
}

func success(format string, args ...any) {
	fmt.Printf("  + "+format+"\n", args...)
}

func warn(format string, args ...any) {
	fmt.Printf("  ! "+format+"\n", args...)
}

func info(format string, args ...any) {
	fmt.Printf("  "+format+"\n", args...)
}

func fail(format string, args ...any) {
	fmt.Printf("  x "+format+"\n", args...)
}

// ============================================================================
// CLOSING
// ============================================================================
