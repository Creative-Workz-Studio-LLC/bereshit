/**
 * CWS Manual Builder — File Watcher
 *
 * Watches for file changes and triggers rebuilds.
 * Watch patterns, debounce timing, and format selection all come from config.
 *
 * For editorial watch, use `make watch-pdf` instead.
 */

import { watch, type FSWatcher } from 'chokidar';
import { join } from 'path';
import type { BuildConfig, RuntimePaths } from '../config/types.js';
import type { BuildEventCallback } from './types.js';
import type { ManualBuilder } from './orchestrator.js';

// =============================================================================
// Watcher
// =============================================================================

/**
 * File watcher for the build system.
 * Encapsulates chokidar watch with config-driven patterns.
 */
export class BuildWatcher {
  private config: BuildConfig;
  private paths: RuntimePaths;
  private builder: ManualBuilder;
  private watcher: FSWatcher | null = null;
  private onEvent: BuildEventCallback;

  constructor(
    config: BuildConfig,
    paths: RuntimePaths,
    builder: ManualBuilder,
    onEvent: BuildEventCallback
  ) {
    this.config = config;
    this.paths = paths;
    this.builder = builder;
    this.onEvent = onEvent;
  }

  /**
   * Start watching for file changes.
   * Watch patterns come from config.
   */
  start(formats?: string[]): void {
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

    this.onEvent({ type: 'watch-start' });

    this.watcher = watch(watchPatterns, {
      ignored: ignorePatterns,
      persistent: true,
      ignoreInitial: true,
    });

    let debounceTimer: NodeJS.Timeout | null = null;

    const handleChange = (path: string) => {
      this.onEvent({ type: 'file-change', path });

      // Debounce delay from config
      if (debounceTimer) {
        clearTimeout(debounceTimer);
      }
      debounceTimer = setTimeout(() => {
        void this.builder.build(targetFormats);
      }, this.config.watch.debounce_ms);
    };

    this.watcher.on('change', handleChange);
    this.watcher.on('add', handleChange);
    this.watcher.on('unlink', handleChange);
  }

  /**
   * Stop watching for file changes.
   */
  stop(): void {
    if (this.watcher) {
      void this.watcher.close();
      this.watcher = null;
    }
  }
}
