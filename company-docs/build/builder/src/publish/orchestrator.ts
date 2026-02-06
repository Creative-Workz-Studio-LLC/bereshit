/**
 * CWS Manual Builder — Build Orchestrator
 *
 * Coordinates format builds based on configuration.
 * Sequential or parallel execution, output management, event emission.
 *
 * "Whatsoever thy hand findeth to do, do it with thy might."
 *  — Ecclesiastes 9:10
 */

import { rm, readdir, stat } from 'fs/promises';
import { existsSync } from 'fs';
import { join } from 'path';
import type { BuildConfig, RuntimePaths } from '../config/types.js';
import { getEnabledFormats } from '../config/loader.js';
import { buildFormats, buildFormat } from './executor.js';
import { BuildWatcher } from './watcher.js';
import type { BuildResult, BuildEventCallback, OutputFileInfo } from './types.js';

// =============================================================================
// Builder Class
// =============================================================================

/**
 * Manual builder — orchestrates format builds.
 * All behavior is driven by config.
 */
export class ManualBuilder {
  private config: BuildConfig;
  private paths: RuntimePaths;
  private watcher: BuildWatcher | null = null;
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
   * Emit a build event.
   */
  private emit(event: Parameters<BuildEventCallback>[0]): void {
    this.onEvent(event);
  }

  /**
   * Build specified formats (sequential).
   * If no formats specified, uses enabled formats from config.
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
   * Build all formats in parallel (faster but uses more resources).
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
   * Start watching for file changes.
   * Watch patterns come from config.
   */
  startWatch(formats?: string[]): void {
    if (this.watcher) {
      return;
    }

    this.watcher = new BuildWatcher(this.config, this.paths, this, this.onEvent);
    this.watcher.start(formats);
  }

  /**
   * Stop watching for file changes.
   */
  stopWatch(): void {
    if (this.watcher) {
      this.watcher.stop();
      this.watcher = null;
    }
  }

  /**
   * Clean output directory.
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
   * Get output file info.
   */
  async getOutputInfo(): Promise<OutputFileInfo[]> {
    if (!existsSync(this.paths.outputDir)) {
      return [];
    }

    const entries = await readdir(this.paths.outputDir);
    const info: OutputFileInfo[] = [];

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

// =============================================================================
// Factory Function
// =============================================================================

/**
 * Create a builder instance.
 */
export function createBuilder(
  config: BuildConfig,
  paths: RuntimePaths,
  onEvent?: BuildEventCallback
): ManualBuilder {
  return new ManualBuilder(config, paths, onEvent);
}
