/**
 * CWS Manual Builder — Spine & Inventory Commands
 *
 * Commands: inventory, spine
 */

import type { Command } from 'commander';
import type { BuildConfig, RuntimePaths } from '../../config/types.js';
import { scanSpine } from '../../spine/scanner.js';
import { buildInventory, formatInventoryTree } from '../../spine/inventory.js';
import { displaySpineTree } from '../../display/spine.js';

// =============================================================================
// Command Registration
// =============================================================================

export function registerSpineCommands(
  program: Command,
  _config: BuildConfig,
  paths: RuntimePaths,
): void {
  // cws-build inventory
  program
    .command('inventory')
    .description('Show book parts list and file inventory')
    .action(async () => {
      const tree = await scanSpine(paths.bookDir);
      const report = buildInventory(tree);
      console.log();
      console.log(formatInventoryTree(report));
      console.log();
    });

  // cws-build spine
  program
    .command('spine')
    .description('Show book structure tree')
    .action(async () => {
      const tree = await scanSpine(paths.bookDir);
      displaySpineTree(tree.bookDir, tree.scopes);
    });
}
