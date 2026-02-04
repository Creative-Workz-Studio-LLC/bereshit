/**
 * CWS Manual Builder - Build Orchestrator
 *
 * Main build orchestration for the Company Identity Manual.
 * Coordinates format handlers, watches for changes, and manages output.
 */

import { watch } from 'chokidar';
import { rm, readdir, stat } from 'fs/promises';
import { existsSync } from 'fs';
import { join } from 'path';
import type { BuildConfig, OutputFormat } from './config.js';
import { buildFormats, buildFormat, getSupportedFormats, type BuildResult } from './formats.js';

/**
 * Build event types
 */
export type BuildEvent =
  | { type: 'start'; formats: OutputFormat[] }
  | { type: 'format-start'; format: OutputFormat }
  | { type: 'format-complete'; result: BuildResult }
  | { type: 'complete'; results: BuildResult[] }
  | { type: 'error'; error: string }
  | { type: 'watch-start' }
  | { type: 'file-change'; path: string };

/**
 * Build event callback
 */
export type BuildEventCallback = (event: BuildEvent) => void;

/**
 * Builder class for manual compilation
 */
export class ManualBuilder {
  private config: BuildConfig;
  private watcher: ReturnType<typeof watch> | null = null;
  private onEvent: BuildEventCallback;

  constructor(config: BuildConfig, onEvent?: BuildEventCallback) {
    this.config = config;
    this.onEvent = onEvent || (() => {});
  }

  /**
   * Emit a build event
   */
  private emit(event: BuildEvent): void {
    this.onEvent(event);
  }

  /**
   * Build specified formats
   */
  async build(formats?: OutputFormat[]): Promise<BuildResult[]> {
    const targetFormats = formats || getSupportedFormats();
    this.emit({ type: 'start', formats: targetFormats });

    const results: BuildResult[] = [];

    // Build sequentially to avoid resource conflicts
    for (const format of targetFormats) {
      this.emit({ type: 'format-start', format });
      const result = await buildFormat(format, this.config);
      results.push(result);
      this.emit({ type: 'format-complete', result });
    }

    this.emit({ type: 'complete', results });
    return results;
  }

  /**
   * Build all formats in parallel (faster but uses more resources)
   */
  async buildParallel(formats?: OutputFormat[]): Promise<BuildResult[]> {
    const targetFormats = formats || getSupportedFormats();
    this.emit({ type: 'start', formats: targetFormats });

    const results = await buildFormats(targetFormats, this.config);

    for (const result of results) {
      this.emit({ type: 'format-complete', result });
    }

    this.emit({ type: 'complete', results });
    return results;
  }

  /**
   * Start watching for file changes
   */
  startWatch(formats?: OutputFormat[]): void {
    if (this.watcher) {
      return;
    }

    const targetFormats = formats || ['html'];
    const watchPatterns = [
      join(this.config.sourceDir, '**/*.adoc'),
      join(this.config.sourceDir, '**/*.asciidoc'),
    ];

    this.emit({ type: 'watch-start' });

    this.watcher = watch(watchPatterns, {
      ignored: /(^|[\/\\])\../,
      persistent: true,
      ignoreInitial: true,
    });

    let debounceTimer: NodeJS.Timeout | null = null;

    const handleChange = (path: string) => {
      this.emit({ type: 'file-change', path });

      // Debounce builds
      if (debounceTimer) {
        clearTimeout(debounceTimer);
      }
      debounceTimer = setTimeout(() => {
        this.build(targetFormats);
      }, 500);
    };

    this.watcher.on('change', handleChange);
    this.watcher.on('add', handleChange);
    this.watcher.on('unlink', handleChange);
  }

  /**
   * Stop watching for file changes
   */
  stopWatch(): void {
    if (this.watcher) {
      this.watcher.close();
      this.watcher = null;
    }
  }

  /**
   * Clean output directory
   */
  async clean(): Promise<void> {
    if (existsSync(this.config.outputDir)) {
      const entries = await readdir(this.config.outputDir);
      for (const entry of entries) {
        const entryPath = join(this.config.outputDir, entry);
        await rm(entryPath, { recursive: true, force: true });
      }
    }
  }

  /**
   * Get output file info
   */
  async getOutputInfo(): Promise<Array<{ path: string; size: number; modified: Date }>> {
    if (!existsSync(this.config.outputDir)) {
      return [];
    }

    const entries = await readdir(this.config.outputDir);
    const info: Array<{ path: string; size: number; modified: Date }> = [];

    for (const entry of entries) {
      const entryPath = join(this.config.outputDir, entry);
      const stats = await stat(entryPath);
      if (stats.isFile()) {
        info.push({
          path: entryPath,
          size: stats.size,
          modified: stats.mtime,
        });
      }
    }

    return info;
  }
}

/**
 * Create a builder instance with default config
 */
export function createBuilder(
  config: BuildConfig,
  onEvent?: BuildEventCallback
): ManualBuilder {
  return new ManualBuilder(config, onEvent);
}

/**
 * Format file size for display
 */
export function formatFileSize(bytes: number): string {
  if (bytes < 1024) return `${bytes} B`;
  if (bytes < 1024 * 1024) return `${(bytes / 1024).toFixed(1)} KB`;
  return `${(bytes / (1024 * 1024)).toFixed(1)} MB`;
}

/**
 * Format duration for display
 */
export function formatDuration(ms: number): string {
  if (ms < 1000) return `${ms}ms`;
  return `${(ms / 1000).toFixed(1)}s`;
}
