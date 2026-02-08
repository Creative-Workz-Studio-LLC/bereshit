/**
 * Command Runner — shared utility for running CLI/Make commands from views.
 *
 * Handles running state, output panel expansion, and error display.
 * Views call runCommand() or runMake() and the runner handles the rest.
 */

import { getAdapter } from '../../scripts/adapter.js';
import { store, STATE } from '../../scripts/state.js';

/** Run a CLI command via the adapter. */
export async function runCliCommand(command: string, args: string[] = []): Promise<void> {
  const adapter = getAdapter();
  const running = store.get<string | null>(STATE.RUNNING_PROCESS);

  if (running) {
    appendSystemMessage('A process is already running. Cancel it first.');
    return;
  }

  // Expand output panel.
  const panel = document.querySelector('.output-panel');
  panel?.classList.remove('collapsed');

  // Mark running.
  appendSystemMessage(`Running: cws-build ${command} ${args.join(' ')}`.trim());
  store.set(STATE.RUNNING_PROCESS, 'pending');

  try {
    const result = await adapter.runCommand({ command, args });
    store.set(STATE.RUNNING_PROCESS, null);
    if (!result.success) {
      appendSystemMessage(`Command failed (exit code ${result.exitCode})`);
    }
  } catch (err) {
    store.set(STATE.RUNNING_PROCESS, null);
    appendSystemMessage(`Error: ${err instanceof Error ? err.message : String(err)}`);
  }
}

/** Run a Makefile target via the adapter. */
export async function runMakeTarget(
  target: string,
  vars: Record<string, string> = {},
): Promise<void> {
  const adapter = getAdapter();
  const running = store.get<string | null>(STATE.RUNNING_PROCESS);

  if (running) {
    appendSystemMessage('A process is already running. Cancel it first.');
    return;
  }

  const panel = document.querySelector('.output-panel');
  panel?.classList.remove('collapsed');

  const varStr = Object.entries(vars).map(([k, v]) => `${k}=${v}`).join(' ');
  appendSystemMessage(`Running: make ${target} ${varStr}`.trim());
  store.set(STATE.RUNNING_PROCESS, 'pending');

  try {
    const result = await adapter.runMake({ target, vars });
    store.set(STATE.RUNNING_PROCESS, null);
    if (!result.success) {
      appendSystemMessage(`Make target failed (exit code ${result.exitCode})`);
    }
  } catch (err) {
    store.set(STATE.RUNNING_PROCESS, null);
    appendSystemMessage(`Error: ${err instanceof Error ? err.message : String(err)}`);
  }
}

/** Cancel the currently running process. */
export async function cancelRunning(): Promise<void> {
  const adapter = getAdapter();
  const running = store.get<string | null>(STATE.RUNNING_PROCESS);
  if (running) {
    try {
      await adapter.cancelProcess(running);
    } catch {
      // Best-effort cancel.
    }
    store.set(STATE.RUNNING_PROCESS, null);
    appendSystemMessage('Process cancelled.');
  }
}

/** Clear the output panel. */
export function clearOutput(): void {
  const body = document.querySelector('.output-body');
  if (body) body.innerHTML = '';
  store.set(STATE.OUTPUT_LINES, []);
}

/** Append a system message to the output panel. */
function appendSystemMessage(message: string): void {
  const body = document.querySelector('.output-body');
  if (!body) return;

  const div = document.createElement('div');
  div.style.color = '#A0AEC0';
  div.style.fontStyle = 'italic';
  div.textContent = `--- ${message} ---`;
  body.appendChild(div);
  body.scrollTop = body.scrollHeight;
}

/**
 * Run a CLI command and capture all output lines.
 * Returns the collected stdout/stderr as strings for programmatic use
 * (e.g., parsing JSON output from --json flags).
 */
export async function runCommandCapture(
  command: string,
  args: string[] = [],
): Promise<{ success: boolean; exitCode: number; stdout: string; stderr: string }> {
  const adapter = getAdapter();
  const running = store.get<string | null>(STATE.RUNNING_PROCESS);

  if (running) {
    throw new Error('A process is already running. Cancel it first.');
  }

  const stdoutLines: string[] = [];
  const stderrLines: string[] = [];

  // Subscribe to output before launching — collect lines per stream.
  const unsub = adapter.onOutput((line) => {
    if (line.stream === 'stdout') stdoutLines.push(line.line);
    else stderrLines.push(line.line);
  });

  store.set(STATE.RUNNING_PROCESS, 'pending');

  try {
    const result = await adapter.runCommand({ command, args });
    return {
      success: result.success,
      exitCode: result.exitCode,
      stdout: stdoutLines.join('\n'),
      stderr: stderrLines.join('\n'),
    };
  } finally {
    unsub();
    store.set(STATE.RUNNING_PROCESS, null);
  }
}

/**
 * Create a command card with title, description, option controls, and a Run button.
 * Returns the card element. The onRun callback fires when Run is clicked.
 */
export function createCommandCard(opts: {
  title: string;
  description: string;
  onRun: () => void;
  controls?: HTMLElement;
}): HTMLElement {
  const card = document.createElement('div');
  card.className = 'card';

  const header = document.createElement('div');
  header.className = 'command-section';

  const left = document.createElement('div');
  const h3 = document.createElement('h3');
  h3.className = 'card-header';
  h3.textContent = opts.title;
  left.appendChild(h3);

  const desc = document.createElement('p');
  desc.style.cssText = 'font-size: var(--text-sm); color: var(--color-text-muted); margin-bottom: var(--space-3);';
  desc.textContent = opts.description;
  left.appendChild(desc);

  if (opts.controls) {
    left.appendChild(opts.controls);
  }

  const actions = document.createElement('div');
  actions.className = 'command-actions';
  const runBtn = document.createElement('button');
  runBtn.className = 'btn btn-primary';
  runBtn.textContent = 'Run';
  runBtn.addEventListener('click', opts.onRun);
  actions.appendChild(runBtn);

  // Disable button while a process is running.
  store.onKey(STATE.RUNNING_PROCESS, (value) => {
    runBtn.disabled = value !== null && value !== undefined;
    runBtn.textContent = value ? 'Running...' : 'Run';
  });

  header.appendChild(left);
  header.appendChild(actions);
  card.appendChild(header);

  return card;
}
