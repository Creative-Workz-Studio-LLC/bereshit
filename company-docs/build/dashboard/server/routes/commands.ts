/**
 * Command Routes — REST + WebSocket bridge for CLI commands.
 */

import { Router } from 'express';
import { spawn, type ChildProcess } from 'child_process';
import { resolve, dirname } from 'path';
import { fileURLToPath } from 'url';
import type { WebSocketServer, WebSocket } from 'ws';

const __filename = fileURLToPath(import.meta.url);
const __dirname = dirname(__filename);

export const commandRoutes = Router();

// Active processes for cancellation.
const activeProcesses = new Map<string, ChildProcess>();

// WebSocket server reference (set after creation).
let wss: WebSocketServer | null = null;

export function setWss(server: WebSocketServer): void {
  wss = server;
}

/** Resolve the project directory (company-docs/). */
function getProjectDir(): string {
  // server/routes/commands.ts → ../../ → dashboard/ → ../../ → company-docs/
  return resolve(__dirname, '..', '..', '..', '..');
}

/** Resolve the builder CLI path. */
function getBuilderCli(): string {
  return resolve(getProjectDir(), 'build', 'builder', 'dist', 'cli.js');
}

/** Generate a simple process ID. */
function generateId(): string {
  return Date.now().toString(36) + Math.random().toString(36).slice(2, 8);
}

/** Broadcast to all WebSocket clients. */
function broadcast(data: Record<string, unknown>): void {
  if (!wss) return;
  const msg = JSON.stringify(data);
  wss.clients.forEach((client: WebSocket) => {
    if (client.readyState === 1) { // OPEN
      client.send(msg);
    }
  });
}

/** Strip ANSI escape codes. */
function stripAnsi(s: string): string {
  return s.replace(/\x1B\[[0-9;]*[a-zA-Z]/g, '');
}

// POST /api/commands/run — Run a builder CLI command.
commandRoutes.post('/run', (req, res) => {
  const { command, args = [] } = req.body as {
    command: string;
    args: string[];
  };

  const processId = generateId();
  const cliPath = getBuilderCli();
  const cwd = getProjectDir();

  const child = spawn('node', [cliPath, command, ...args], {
    cwd,
    stdio: ['ignore', 'pipe', 'pipe'],
  });

  activeProcesses.set(processId, child);

  // Stream stdout.
  child.stdout?.on('data', (data: Buffer) => {
    const lines = data.toString().split('\n').filter(Boolean);
    for (const line of lines) {
      broadcast({
        type: 'output',
        processId,
        stream: 'stdout',
        line: stripAnsi(line),
        timestamp: new Date().toISOString(),
      });
    }
  });

  // Stream stderr.
  child.stderr?.on('data', (data: Buffer) => {
    const lines = data.toString().split('\n').filter(Boolean);
    for (const line of lines) {
      broadcast({
        type: 'output',
        processId,
        stream: 'stderr',
        line: stripAnsi(line),
        timestamp: new Date().toISOString(),
      });
    }
  });

  child.on('close', (code) => {
    activeProcesses.delete(processId);
    const exitCode = code ?? -1;
    broadcast({
      type: 'exit',
      processId,
      exitCode,
      success: exitCode === 0,
    });
  });

  res.json({ processId, status: 'started' });
});

// POST /api/commands/make — Run a Makefile target.
commandRoutes.post('/make', (req, res) => {
  const { target, vars = {} } = req.body as {
    target: string;
    vars: Record<string, string>;
  };

  const processId = generateId();
  const cwd = getProjectDir();

  const makeArgs = ['-C', cwd, target];
  for (const [key, val] of Object.entries(vars)) {
    makeArgs.push(`${key}=${val}`);
  }

  const child = spawn('make', makeArgs, {
    cwd,
    stdio: ['ignore', 'pipe', 'pipe'],
  });

  activeProcesses.set(processId, child);

  child.stdout?.on('data', (data: Buffer) => {
    for (const line of data.toString().split('\n').filter(Boolean)) {
      broadcast({
        type: 'output',
        processId,
        stream: 'stdout',
        line: stripAnsi(line),
        timestamp: new Date().toISOString(),
      });
    }
  });

  child.stderr?.on('data', (data: Buffer) => {
    for (const line of data.toString().split('\n').filter(Boolean)) {
      broadcast({
        type: 'output',
        processId,
        stream: 'stderr',
        line: stripAnsi(line),
        timestamp: new Date().toISOString(),
      });
    }
  });

  child.on('close', (code) => {
    activeProcesses.delete(processId);
    broadcast({
      type: 'exit',
      processId,
      exitCode: code ?? -1,
      success: (code ?? -1) === 0,
    });
  });

  res.json({ processId, status: 'started' });
});

// POST /api/commands/cancel — Cancel a running process.
commandRoutes.post('/cancel', (req, res) => {
  const { processId } = req.body as { processId: string };
  const child = activeProcesses.get(processId);

  if (child) {
    child.kill('SIGTERM');
    activeProcesses.delete(processId);
    res.json({ status: 'cancelled' });
  } else {
    res.status(404).json({ error: `Process ${processId} not found` });
  }
});

// GET /api/commands/active — List active processes.
commandRoutes.get('/active', (_req, res) => {
  res.json({
    active: Array.from(activeProcesses.keys()),
    count: activeProcesses.size,
  });
});
