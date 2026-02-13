// ═══════════════════════════════════════════════════════════════════════════
// METADATA
// ═══════════════════════════════════════════════════════════════════════════
//
// Key: cws-ops-main
// Purpose: CWS Ops Tool — CLI for managing CWS production infrastructure
// Biblical: Nehemiah 2:17 — "Come, and let us build up the wall of Jerusalem"
// Authors: Nova Dawn
// Version: 0.1.0
// Created: 2026-02-12
//
// ═══════════════════════════════════════════════════════════════════════════

// CWS Ops is an infrastructure management tool for CreativeWorkzStudio
// production servers. It provides both a CLI and web GUI interface for:
//
//   - Checking server status, services, and resources
//   - Deploying code, config, and assets
//   - Managing systemd services
//   - Streaming server logs
//   - DNS management via Cloudflare API
//   - Backup status and restore via Litestream/Restic
//
// All destructive operations require explicit confirmation unless
// --no-confirm is passed. SSH access reuses the operator's local config.
//
// Usage:
//
//	cws-ops [flags] <command> [args...]
//	cws-ops status          Check infrastructure health
//	cws-ops ssh             Open interactive SSH session
//	cws-ops help            Show usage
package main

// ═══════════════════════════════════════════════════════════════════════════
// SETUP
// ═══════════════════════════════════════════════════════════════════════════

import (
	"context"
	"encoding/json"
	"flag"
	"fmt"
	"os"
	"os/signal"
	"strings"
	"syscall"
	"text/tabwriter"

	"cws.studio/server/internal/config"
	"cws.studio/server/internal/ops"
)

// ═══════════════════════════════════════════════════════════════════════════
// BODY
// ═══════════════════════════════════════════════════════════════════════════

const version = "0.1.0"

// Global flags parsed before subcommand dispatch.
var (
	flagTarget    string
	flagVerbose   bool
	flagJSON      bool
	flagDryRun    bool
	flagNoConfirm bool
)

func main() {
	// ── Global Flags ─────────────────────────────────────────────────
	flag.StringVar(&flagTarget, "target", "oracle", "Deploy target (oracle, dell)")
	flag.BoolVar(&flagVerbose, "verbose", false, "Verbose output")
	flag.BoolVar(&flagJSON, "json", false, "Output as JSON")
	flag.BoolVar(&flagDryRun, "dry-run", false, "Show what would happen without executing")
	flag.BoolVar(&flagNoConfirm, "no-confirm", false, "Skip confirmation prompts")
	flag.Usage = usage
	flag.Parse()

	args := flag.Args()
	if len(args) == 0 {
		usage()
		os.Exit(1)
	}

	// ── Resolve Host ─────────────────────────────────────────────────
	host, ok := config.LookupHost(flagTarget)
	if !ok {
		fatalf("unknown target: %s (known: oracle, dell)", flagTarget)
	}

	// ── Context ──────────────────────────────────────────────────────
	ctx, cancel := signal.NotifyContext(context.Background(), syscall.SIGINT, syscall.SIGTERM)
	defer cancel()

	// ── Dispatch ─────────────────────────────────────────────────────
	switch args[0] {
	case "status":
		cmdStatus(ctx, host, args[1:])
	case "deploy":
		cmdDeploy(ctx, host, args[1:])
	case "logs":
		cmdLogs(ctx, host, args[1:])
	case "services":
		cmdServices(ctx, host, args[1:])
	case "backups":
		cmdBackups(ctx, host, args[1:])
	case "health":
		cmdHealth(ctx, host, args[1:])
	case "serve":
		cmdServe(ctx, host, args[1:])
	case "ssh":
		cmdSSH(host)
	case "version":
		cmdVersion()
	case "help":
		usage()
	default:
		fatalf("unknown command: %s\nRun 'cws-ops help' for usage.", args[0])
	}
}

// ── Commands ─────────────────────────────────────────────────────────

func cmdStatus(ctx context.Context, host config.Host, args []string) {
	fs := flag.NewFlagSet("status", flag.ExitOnError)
	full := fs.Bool("full", false, "Show full status (services + resources)")
	servicesOnly := fs.Bool("services", false, "Show only service status")
	resourcesOnly := fs.Bool("resources", false, "Show only resource usage")
	_ = fs.Parse(args)

	// Default to full status
	showServices := *full || *servicesOnly || (!*servicesOnly && !*resourcesOnly)
	showResources := *full || *resourcesOnly || (!*servicesOnly && !*resourcesOnly)

	if flagJSON {
		// JSON mode: always full
		status, err := ops.GetFullStatus(ctx, host)
		if err != nil {
			fatalf("status: %v", err)
		}
		enc := json.NewEncoder(os.Stdout)
		enc.SetIndent("", "  ")
		if err := enc.Encode(status); err != nil {
			fatalf("json: %v", err)
		}
		return
	}

	fmt.Printf("CWS Infrastructure — %s (%s)\n", host.Name, host.Address)
	fmt.Println(strings.Repeat("─", 50))

	if showServices {
		fmt.Println()
		printServices(ctx, host)
	}

	if showResources {
		fmt.Println()
		printResources(ctx, host)
	}

	// Quick health
	fmt.Println()
	health, err := ops.RunSSHCapture(ctx, host,
		"curl -sf http://localhost:3847/healthz -o /dev/null && echo ok || echo fail")
	if err != nil {
		fmt.Printf("  Server:  connection error\n")
	} else if strings.TrimSpace(health) == "ok" {
		fmt.Printf("  Server:  healthy\n")
	} else {
		fmt.Printf("  Server:  unhealthy\n")
	}
	fmt.Println()
}

func printServices(ctx context.Context, host config.Host) {
	services, err := ops.GetServiceStatuses(ctx, host)
	if err != nil {
		fmt.Printf("  Error: %v\n", err)
		return
	}

	tw := tabwriter.NewWriter(os.Stdout, 0, 0, 2, ' ', 0)
	fmt.Fprintf(tw, "  SERVICE\tSTATUS\tPID\tMEMORY\tUPTIME\n")
	fmt.Fprintf(tw, "  ───────\t──────\t───\t──────\t──────\n")

	for _, svc := range services {
		status := "stopped"
		if svc.Active {
			status = "running"
		}
		pid := "—"
		if svc.PID > 0 {
			pid = fmt.Sprintf("%d", svc.PID)
		}
		uptime := svc.Uptime
		if uptime == "" {
			uptime = "—"
		}

		fmt.Fprintf(tw, "  %s\t%s\t%s\t%s\t%s\n",
			svc.Name, status, pid, svc.Memory, uptime)
	}
	tw.Flush()
}

func printResources(ctx context.Context, host config.Host) {
	resources, err := ops.GetResources(ctx, host)
	if err != nil {
		fmt.Printf("  Error: %v\n", err)
		return
	}

	fmt.Printf("  CPU:     %.0f%% (load: %.2f / %.2f / %.2f)\n",
		resources.CPUPercent, resources.LoadAvg1, resources.LoadAvg5, resources.LoadAvg15)
	fmt.Printf("  Memory:  %s / %s (%.0f%%)\n",
		humanBytes(resources.MemoryUsed), humanBytes(resources.MemoryTotal),
		percent(resources.MemoryUsed, resources.MemoryTotal))
	fmt.Printf("  Disk:    %s / %s (%.0f%%)\n",
		humanBytes(resources.DiskUsed), humanBytes(resources.DiskTotal),
		percent(resources.DiskUsed, resources.DiskTotal))
}

func cmdSSH(host config.Host) {
	fmt.Printf("Connecting to %s (%s)...\n", host.Name, host.Address)
	if err := ops.InteractiveSSH(host); err != nil {
		fatalf("ssh: %v", err)
	}
}

func cmdVersion() {
	if flagJSON {
		fmt.Printf(`{"version":"%s"}%s`, version, "\n")
	} else {
		fmt.Printf("cws-ops %s\n", version)
	}
}

// ── Deploy ───────────────────────────────────────────────────────────

func cmdDeploy(ctx context.Context, host config.Host, args []string) {
	if len(args) == 0 {
		fatalf("usage: cws-ops deploy <all|binary|dash|site|builder|fly|monitoring>")
	}

	target := ops.DeployTarget(args[0])
	if !ops.IsValidDeployTarget(args[0]) {
		fatalf("unknown deploy target: %s\nValid targets: all, binary, dash, site, builder, fly, monitoring", args[0])
	}

	desc := ops.DeployDescription[target]

	if flagDryRun {
		ops.DryRunDeploy(host, target, os.Stdout)
		return
	}

	if !ops.Confirm(fmt.Sprintf("Deploy %s to %s?\n  %s", target, host.Name, desc), flagNoConfirm) {
		fmt.Println("Cancelled.")
		return
	}

	fmt.Printf("Deploying %s to %s...\n\n", target, host.Name)
	if err := ops.Deploy(ctx, host, target, os.Stdout); err != nil {
		fatalf("deploy failed: %v", err)
	}
}

// ── Logs ─────────────────────────────────────────────────────────────

func cmdLogs(ctx context.Context, host config.Host, args []string) {
	fs := flag.NewFlagSet("logs", flag.ExitOnError)
	follow := fs.Bool("follow", false, "Stream logs in real time")
	lines := fs.Int("lines", 50, "Number of recent lines")
	service := fs.String("service", "cws-server", "Service to show logs for")
	since := fs.String("since", "", "Show logs since (e.g., 1h, 30m)")
	_ = fs.Parse(args)

	// Shortcut: -f for --follow
	for _, a := range args {
		if a == "-f" {
			*follow = true
		}
	}

	if flagJSON {
		output, err := ops.GetRecentLogs(ctx, host, *service, *lines)
		if err != nil {
			fatalf("logs: %v", err)
		}
		enc := json.NewEncoder(os.Stdout)
		enc.SetIndent("", "  ")
		_ = enc.Encode(map[string]string{"logs": output})
		return
	}

	opts := ops.LogOptions{
		Service: *service,
		Follow:  *follow,
		Lines:   *lines,
		Since:   *since,
	}

	if err := ops.StreamLogs(ctx, host, opts, os.Stdout); err != nil {
		// Context cancellation is normal for streaming
		if ctx.Err() != nil {
			fmt.Println()
			return
		}
		fatalf("logs: %v", err)
	}
}

// ── Services ─────────────────────────────────────────────────────────

func cmdServices(ctx context.Context, host config.Host, args []string) {
	if len(args) == 0 {
		// Default: list all services
		printServices(ctx, host)
		return
	}

	action := args[0]
	switch action {
	case "list":
		printServices(ctx, host)

	case "restart", "stop", "start":
		if len(args) < 2 {
			fatalf("usage: cws-ops services %s <service-name>", action)
		}
		name := args[1]

		if !ops.IsManaged(name) {
			fatalf("unknown service: %s\nManaged services: %s",
				name, strings.Join(ops.ManagedServiceNames(), ", "))
		}

		if flagDryRun {
			fmt.Printf("Dry run: would %s %s on %s\n", action, name, host.Name)
			return
		}

		// Capitalize first letter of action for prompt
		actionTitle := strings.ToUpper(action[:1]) + action[1:]
		prompt := fmt.Sprintf("%s %s on %s?", actionTitle, name, host.Name)
		if !ops.Confirm(prompt, flagNoConfirm) {
			fmt.Println("Cancelled.")
			return
		}

		result, err := ops.ManageService(ctx, host, name, ops.ServiceAction(action))
		if err != nil {
			fatalf("service %s: %v", action, err)
		}
		if !result.OK() {
			fatalf("service %s failed (exit %d): %s", action, result.ExitCode, result.Stderr)
		}

		fmt.Printf("%s %s: done\n", name, action)

		// Show updated status
		svc, err := ops.GetServiceStatus(ctx, host, name)
		if err == nil {
			status := "stopped"
			if svc.Active {
				status = "running"
			}
			fmt.Printf("  Status: %s (PID: %d, Memory: %s)\n", status, svc.PID, svc.Memory)
		}

	default:
		fatalf("unknown action: %s (use: list, restart, stop, start)", action)
	}
}

// ── Backups ──────────────────────────────────────────────────────────

func cmdBackups(ctx context.Context, host config.Host, args []string) {
	action := "status"
	if len(args) > 0 {
		action = args[0]
	}

	switch action {
	case "status":
		status, err := ops.GetBackupStatus(ctx, host)
		if err != nil {
			fatalf("backup status: %v", err)
		}

		if flagJSON {
			enc := json.NewEncoder(os.Stdout)
			enc.SetIndent("", "  ")
			_ = enc.Encode(status)
			return
		}

		lsStatus := "stopped"
		if status.LitestreamRunning {
			lsStatus = "running"
		}
		fmt.Printf("Litestream: %s\n\n", lsStatus)

		if len(status.Databases) > 0 {
			tw := tabwriter.NewWriter(os.Stdout, 0, 0, 2, ' ', 0)
			fmt.Fprintf(tw, "  DATABASE\tSIZE\tREPLICATING\n")
			fmt.Fprintf(tw, "  ────────\t────\t───────────\n")
			for _, db := range status.Databases {
				repl := "no"
				if db.Replicating {
					repl = "yes"
				}
				fmt.Fprintf(tw, "  %s\t%s\t%s\n", db.Name, db.SizeHuman, repl)
			}
			tw.Flush()
		}

	case "verify":
		fmt.Printf("Verifying database integrity on %s...\n", host.Name)
		output, err := ops.VerifyBackups(ctx, host)
		if err != nil {
			fatalf("verify: %v", err)
		}
		fmt.Println(output)

	case "restore":
		if len(args) < 2 {
			fatalf("usage: cws-ops backups restore <database-name> [--to /path/to/dest.db]")
		}
		dbName := args[1]

		var destPath string
		for i, a := range args {
			if a == "--to" && i+1 < len(args) {
				destPath = args[i+1]
			}
		}

		if flagDryRun {
			dest := destPath
			if dest == "" {
				dest = fmt.Sprintf("/tmp/%s-restored.db", dbName)
			}
			fmt.Printf("Dry run: would restore %s.db to %s on %s\n", dbName, dest, host.Name)
			return
		}

		if !ops.Confirm(fmt.Sprintf("Restore %s.db from R2 backup on %s?", dbName, host.Name), flagNoConfirm) {
			fmt.Println("Cancelled.")
			return
		}

		fmt.Printf("Restoring %s.db...\n", dbName)
		result, err := ops.RestoreDatabase(ctx, host, dbName, destPath)
		if err != nil {
			fatalf("restore: %v", err)
		}
		if !result.OK() {
			fatalf("restore failed (exit %d): %s", result.ExitCode, result.Stderr)
		}

		dest := destPath
		if dest == "" {
			dest = fmt.Sprintf("/tmp/%s-restored.db", dbName)
		}
		fmt.Printf("Restored to %s\n", dest)

	default:
		fatalf("unknown action: %s (use: status, verify, restore)", action)
	}
}

// ── Health ───────────────────────────────────────────────────────────

func cmdHealth(ctx context.Context, host config.Host, args []string) {
	fs := flag.NewFlagSet("health", flag.ExitOnError)
	lines := fs.Int("lines", 20, "Number of log lines to show")
	_ = fs.Parse(args)

	output, err := ops.RunSSHCapture(ctx, host,
		fmt.Sprintf("cat /home/seanje/cws/logs/health.log 2>/dev/null | tail -%d", *lines))
	if err != nil {
		fatalf("health: %v", err)
	}
	if output == "" {
		fmt.Println("No health events logged (all checks passing)")
		return
	}
	fmt.Println(output)
}

// ── Helpers ──────────────────────────────────────────────────────────

func usage() {
	fmt.Fprintf(os.Stderr, `CWS Ops — Infrastructure Management Tool v%s

Usage:
  cws-ops [flags] <command> [args...]

Commands:
  status [--full|--services|--resources]                Check infrastructure health
  deploy <all|binary|dash|site|builder|fly|monitoring>  Deploy to production
  logs [--follow] [--lines N] [--service NAME]          View/stream server logs
  services [list|restart|stop|start] [NAME]             Manage systemd services
  backups [status|verify|restore DB [--to PATH]]        Backup status and restore
  health [--lines N]                                    View health watchdog log
  serve [--port 4847] [--open]                          Start web GUI
  ssh                                                   Open interactive SSH session
  version                                               Show version
  help                                                  Show this help

Flags:
  --target oracle|dell   Deploy target (default: oracle)
  --verbose              Verbose output
  --json                 Output as JSON
  --dry-run              Show what would happen
  --no-confirm           Skip confirmation prompts

`, version)
}

func fatalf(format string, args ...any) {
	fmt.Fprintf(os.Stderr, "error: "+format+"\n", args...)
	os.Exit(1)
}

func humanBytes(b uint64) string {
	switch {
	case b < 1024*1024:
		return fmt.Sprintf("%.0fKB", float64(b)/1024)
	case b < 1024*1024*1024:
		return fmt.Sprintf("%.0fMB", float64(b)/(1024*1024))
	default:
		return fmt.Sprintf("%.1fGB", float64(b)/(1024*1024*1024))
	}
}

func percent(used, total uint64) float64 {
	if total == 0 {
		return 0
	}
	return float64(used) / float64(total) * 100
}

// ═══════════════════════════════════════════════════════════════════════════
// CLOSING
// ═══════════════════════════════════════════════════════════════════════════
//
// "Come, and let us build up the wall of Jerusalem,
//  that we be no more a reproach." — Nehemiah 2:17
