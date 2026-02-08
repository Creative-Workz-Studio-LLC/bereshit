/**
 * Command Runner — shared utility for running CLI/Make commands from views.
 *
 * Handles running state, output panel expansion, and error display.
 * Views call runCommand() or runMake() and the runner handles the rest.
 */
import { getAdapter } from '../../scripts/adapter.js';
import { store, STATE } from '../../scripts/state.js';
import { showToast } from './toast.js';
/** Run a CLI command via the adapter. */
export async function runCliCommand(command, args = []) {
    const adapter = getAdapter();
    const running = store.get(STATE.RUNNING_PROCESS);
    if (running) {
        appendErrorMessage('A process is already running. Wait for it to finish or cancel it first.');
        showToast({ message: 'A process is already running.', type: 'warning', duration: 3000 });
        return;
    }
    // Check WebSocket connection (web mode only).
    if (adapter.getMode() === 'web' && 'isConnected' in adapter && !adapter.isConnected()) {
        appendErrorMessage('Not connected to server. Check your network connection and try again.');
        showToast({ message: 'Not connected to server.', type: 'error', duration: 5000 });
        return;
    }
    // Expand output panel.
    expandOutputPanel();
    // Mark running.
    appendSystemMessage(`Running: cws-build ${command} ${args.join(' ')}`.trim());
    store.set(STATE.RUNNING_PROCESS, 'pending');
    try {
        const result = await adapter.runCommand({ command, args });
        store.set(STATE.RUNNING_PROCESS, null);
        if (!result.success) {
            if (result.exitCode === -1) {
                appendErrorMessage('Command failed to start. The server may not have the required tools installed.');
            }
            else {
                appendErrorMessage(`Command failed (exit code ${result.exitCode}). Check output above for details.`);
            }
        }
        else {
            appendSuccessMessage('Command completed successfully.');
        }
    }
    catch (err) {
        store.set(STATE.RUNNING_PROCESS, null);
        const msg = err instanceof Error ? err.message : String(err);
        appendErrorMessage(`Error: ${msg}`);
        showToast({ message: `Command error: ${msg}`, type: 'error', duration: 5000 });
    }
}
/** Run a Makefile target via the adapter. */
export async function runMakeTarget(target, vars = {}) {
    const adapter = getAdapter();
    const running = store.get(STATE.RUNNING_PROCESS);
    if (running) {
        appendErrorMessage('A process is already running. Wait for it to finish or cancel it first.');
        showToast({ message: 'A process is already running.', type: 'warning', duration: 3000 });
        return;
    }
    // Check WebSocket connection (web mode only).
    if (adapter.getMode() === 'web' && 'isConnected' in adapter && !adapter.isConnected()) {
        appendErrorMessage('Not connected to server. Check your network connection and try again.');
        showToast({ message: 'Not connected to server.', type: 'error', duration: 5000 });
        return;
    }
    expandOutputPanel();
    const varStr = Object.entries(vars).map(([k, v]) => `${k}=${v}`).join(' ');
    appendSystemMessage(`Running: make ${target} ${varStr}`.trim());
    store.set(STATE.RUNNING_PROCESS, 'pending');
    try {
        const result = await adapter.runMake({ target, vars });
        store.set(STATE.RUNNING_PROCESS, null);
        if (!result.success) {
            if (result.exitCode === -1) {
                appendErrorMessage('Make target failed to start. Is "make" installed on the server?');
            }
            else {
                appendErrorMessage(`Make target failed (exit code ${result.exitCode}). Check output above for details.`);
            }
        }
        else {
            appendSuccessMessage(`Make target "${target}" completed successfully.`);
        }
    }
    catch (err) {
        store.set(STATE.RUNNING_PROCESS, null);
        const msg = err instanceof Error ? err.message : String(err);
        appendErrorMessage(`Error: ${msg}`);
        showToast({ message: `Make error: ${msg}`, type: 'error', duration: 5000 });
    }
}
/** Cancel the currently running process. */
export async function cancelRunning() {
    const adapter = getAdapter();
    const running = store.get(STATE.RUNNING_PROCESS);
    if (running) {
        try {
            await adapter.cancelProcess(running);
        }
        catch {
            // Best-effort cancel.
        }
        store.set(STATE.RUNNING_PROCESS, null);
        appendSystemMessage('Process cancelled.');
    }
}
/** Clear the output panel. */
export function clearOutput() {
    const body = document.querySelector('.output-body');
    if (body)
        body.innerHTML = '';
    store.set(STATE.OUTPUT_LINES, []);
}
/** Expand the output panel if collapsed. */
function expandOutputPanel() {
    const panel = document.querySelector('.output-panel');
    if (panel?.classList.contains('collapsed')) {
        panel.classList.remove('collapsed');
        const arrow = panel.querySelector('.output-toggle');
        if (arrow)
            arrow.textContent = '\u25BC';
    }
}
/** Append a system message to the output panel. */
function appendSystemMessage(message) {
    const body = document.querySelector('.output-body');
    if (!body)
        return;
    const div = document.createElement('div');
    div.style.cssText = 'color: #A0AEC0; font-style: italic; padding: 2px 0;';
    div.textContent = `--- ${message} ---`;
    body.appendChild(div);
    body.scrollTop = body.scrollHeight;
}
/** Append an error message to the output panel (red, prominent). */
function appendErrorMessage(message) {
    const body = document.querySelector('.output-body');
    if (!body)
        return;
    expandOutputPanel();
    const div = document.createElement('div');
    div.style.cssText = 'color: #E53E3E; font-weight: 600; padding: 4px 0; border-left: 3px solid #E53E3E; padding-left: 8px; margin: 4px 0;';
    div.textContent = `ERROR: ${message}`;
    body.appendChild(div);
    body.scrollTop = body.scrollHeight;
}
/** Append a success message to the output panel (green). */
function appendSuccessMessage(message) {
    const body = document.querySelector('.output-body');
    if (!body)
        return;
    const div = document.createElement('div');
    div.style.cssText = 'color: #38A169; font-weight: 500; padding: 2px 0;';
    div.textContent = `✓ ${message}`;
    body.appendChild(div);
    body.scrollTop = body.scrollHeight;
}
/**
 * Run a CLI command and capture all output lines.
 * Returns the collected stdout/stderr as strings for programmatic use
 * (e.g., parsing JSON output from --json flags).
 */
export async function runCommandCapture(command, args = []) {
    const adapter = getAdapter();
    const running = store.get(STATE.RUNNING_PROCESS);
    if (running) {
        throw new Error('A process is already running. Cancel it first.');
    }
    const stdoutLines = [];
    const stderrLines = [];
    // Subscribe to output before launching — collect lines per stream.
    const unsub = adapter.onOutput((line) => {
        if (line.stream === 'stdout')
            stdoutLines.push(line.line);
        else
            stderrLines.push(line.line);
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
    }
    finally {
        unsub();
        store.set(STATE.RUNNING_PROCESS, null);
    }
}
/**
 * Create a command card with title, description, option controls, and a Run button.
 * Returns the card element. The onRun callback fires when Run is clicked.
 */
export function createCommandCard(opts) {
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
