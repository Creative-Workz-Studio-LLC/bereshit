/**
 * Adapter Interface — abstraction layer between frontend and backend.
 *
 * Two implementations: TauriAdapter (native IPC) and WebAdapter (HTTP + WebSocket).
 * Frontend imports only this interface — never backend-specific code.
 */

export interface CommandRequest {
  /** CLI command name (e.g., 'lint', 'build', 'witness'). */
  command: string;
  /** Command arguments (e.g., ['--file', 'path/to/file.adoc']). */
  args: string[];
}

export interface MakeRequest {
  /** Makefile target (e.g., 'section', 'book', 'lint'). */
  target: string;
  /** Make variables (e.g., { FILE: 'path.adoc', FORCE: '1' }). */
  vars?: Record<string, string>;
}

export interface CommandResult {
  processId: string;
  exitCode: number;
  success: boolean;
}

export interface OutputLine {
  processId: string;
  stream: 'stdout' | 'stderr';
  line: string;
  timestamp: string;
}

export interface FileDialogOptions {
  title?: string;
  filters?: Array<{ name: string; extensions: string[] }>;
  directory?: boolean;
  multiple?: boolean;
}

export interface Adapter {
  /** Get the current mode. */
  getMode(): 'tauri' | 'web';

  /** Run a builder CLI command. */
  runCommand(req: CommandRequest): Promise<CommandResult>;

  /** Run a Makefile target. */
  runMake(req: MakeRequest): Promise<CommandResult>;

  /** Cancel a running process. */
  cancelProcess(processId: string): Promise<void>;

  /** Read the full build.config.yaml as parsed JSON. */
  readConfig(): Promise<Record<string, unknown>>;

  /** Write a single section of build.config.yaml. */
  writeConfigSection(section: string, data: unknown): Promise<void>;

  /** Read dashboard preferences. */
  readPreferences(): Promise<Record<string, unknown>>;

  /** Write dashboard preferences. */
  writePreferences(prefs: Record<string, unknown>): Promise<void>;

  /** Open a native file dialog (or web fallback). */
  openFileDialog(opts: FileDialogOptions): Promise<string[]>;

  /** Subscribe to output lines. Returns unsubscribe function. */
  onOutput(callback: (line: OutputLine) => void): () => void;

  /** Subscribe to process exit events. Returns unsubscribe function. */
  onExit(callback: (result: CommandResult) => void): () => void;
}
