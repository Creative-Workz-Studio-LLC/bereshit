/**
 * CWS Manual Builder - Build Orchestrator
 *
 * Coordinates format builds based on configuration.
 * Watch patterns, debounce timing, and format selection all come from config.
 */

import { watch } from 'chokidar';
import { rm, readdir, stat } from 'fs/promises';
import { existsSync } from 'fs';
import { join } from 'path';
import type { BuildConfig, RuntimePaths } from './config.js';
import { getEnabledFormats } from './config.js';
import { buildFormats, buildFormat, type BuildResult } from './formats.js';

// -----------------------------------------------------------------------------
// Types
// -----------------------------------------------------------------------------

/**
 * Build event types
 */
export type BuildEvent =
  | { type: 'start'; formats: string[] }
  | { type: 'format-start'; format: string }
  | { type: 'format-complete'; result: BuildResult }
  | { type: 'complete'; results: BuildResult[] }
  | { type: 'error'; error: string }
  | { type: 'watch-start' }
  | { type: 'file-change'; path: string };

/**
 * Build event callback
 */
export type BuildEventCallback = (event: BuildEvent) => void;

// -----------------------------------------------------------------------------
// Builder Class
// -----------------------------------------------------------------------------

/**
 * Builder class for manual compilation
 * All behavior is driven by config
 */
export class ManualBuilder {
  private config: BuildConfig;
  private paths: RuntimePaths;
  private watcher: ReturnType<typeof watch> | null = null;
  private onEvent: BuildEventCallback;

  constructor(
    config: BuildConfig,
    paths: RuntimePaths,
    onEvent?: BuildEventCallback
  ) {
    this.config = config;
    this.paths = paths;
    this.onEvent = onEvent || (() => {});
  }

  /**
   * Emit a build event
   */
  private emit(event: BuildEvent): void {
    this.onEvent(event);
  }

  /**
   * Build specified formats (sequential)
   * If no formats specified, uses enabled formats from config
   */
  async build(formats?: string[]): Promise<BuildResult[]> {
    const targetFormats = formats || getEnabledFormats(this.config);
    this.emit({ type: 'start', formats: targetFormats });

    const results: BuildResult[] = [];

    // Build sequentially to avoid resource conflicts
    for (const format of targetFormats) {
      this.emit({ type: 'format-start', format });
      const result = await buildFormat(format, this.config, this.paths);
      results.push(result);
      this.emit({ type: 'format-complete', result });
    }

    this.emit({ type: 'complete', results });
    return results;
  }

  /**
   * Build all formats in parallel (faster but uses more resources)
   */
  async buildParallel(formats?: string[]): Promise<BuildResult[]> {
    const targetFormats = formats || getEnabledFormats(this.config);
    this.emit({ type: 'start', formats: targetFormats });

    const results = await buildFormats(targetFormats, this.config, this.paths);

    for (const result of results) {
      this.emit({ type: 'format-complete', result });
    }

    this.emit({ type: 'complete', results });
    return results;
  }

  /**
   * Start watching for file changes
   * Watch patterns come from config
   */
  startWatch(formats?: string[]): void {
    if (this.watcher) {
      return;
    }

    // Use formats from config.watch.formats if not specified
    const targetFormats = formats || this.config.watch.formats;

    // Build watch patterns from config
    const watchPatterns = this.config.watch.patterns.map(pattern =>
      join(this.paths.sourceDir, pattern)
    );

    // Build ignore patterns from config
    const ignorePatterns = this.config.watch.ignore;

    this.emit({ type: 'watch-start' });

    this.watcher = watch(watchPatterns, {
      ignored: ignorePatterns,
      persistent: true,
      ignoreInitial: true,
    });

    let debounceTimer: NodeJS.Timeout | null = null;

    const handleChange = (path: string) => {
      this.emit({ type: 'file-change', path });

      // Debounce delay from config
      if (debounceTimer) {
        clearTimeout(debounceTimer);
      }
      debounceTimer = setTimeout(() => {
        this.build(targetFormats);
      }, this.config.watch.debounce_ms);
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
    if (existsSync(this.paths.outputDir)) {
      const entries = await readdir(this.paths.outputDir);
      for (const entry of entries) {
        const entryPath = join(this.paths.outputDir, entry);
        await rm(entryPath, { recursive: true, force: true });
      }
    }
  }

  /**
   * Get output file info
   */
  async getOutputInfo(): Promise<Array<{ path: string; size: number; modified: Date }>> {
    if (!existsSync(this.paths.outputDir)) {
      return [];
    }

    const entries = await readdir(this.paths.outputDir);
    const info: Array<{ path: string; size: number; modified: Date }> = [];

    for (const entry of entries) {
      const entryPath = join(this.paths.outputDir, entry);
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

// -----------------------------------------------------------------------------
// Factory Function
// -----------------------------------------------------------------------------

/**
 * Create a builder instance
 */
export function createBuilder(
  config: BuildConfig,
  paths: RuntimePaths,
  onEvent?: BuildEventCallback
): ManualBuilder {
  return new ManualBuilder(config, paths, onEvent);
}

// -----------------------------------------------------------------------------
// Display Utilities
// -----------------------------------------------------------------------------

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
