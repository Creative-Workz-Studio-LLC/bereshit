/**
 * CWS Manual Builder — Progress Display
 *
 * Config-driven event handler for build progress output.
 * All icons and labels come from build.config.yaml.
 */

import chalk from 'chalk';
import type { BuildConfig } from '../../config/types.js';
import type { BuildEvent } from '../../publish/types.js';
import { formatDuration } from './format.js';

// =============================================================================
// Event Handler Factory
// =============================================================================

/**
 * Create a build event handler that uses config for display.
 */
export function createEventHandler(config: BuildConfig): (event: BuildEvent) => void {
  return function handleEvent(event: BuildEvent): void {
    const icons = config.display.icons;

    switch (event.type) {
      case 'start':
        console.log(chalk.cyan(`\n${icons['start'] || '▶'} Starting build...`));
        console.log(chalk.gray(`  Formats: ${event.formats.join(', ')}`));
        break;

      case 'format-start': {
        const icon = icons[event.format] || '📄';
        process.stdout.write(
          chalk.white(`  ${icon} Building ${event.format.toUpperCase()}... `)
        );
        break;
      }

      case 'format-complete':
        if (event.result.success) {
          console.log(
            chalk.green(icons['success'] || '✓') +
              chalk.gray(` (${formatDuration(event.result.duration)})`)
          );
        } else {
          console.log(chalk.red(icons['failure'] || '✗'));
          console.log(chalk.red(`     Error: ${event.result.error}`));
        }
        break;

      case 'complete': {
        const successful = event.results.filter((r) => r.success).length;
        const total = event.results.length;
        console.log();

        if (successful === total) {
          console.log(chalk.green.bold(`${icons['success'] || '✓'} Build complete: ${successful}/${total} formats`));
        } else if (successful > 0) {
          console.log(chalk.yellow.bold(`⚠ Build partial: ${successful}/${total} formats`));
        } else {
          console.log(chalk.red.bold(`${icons['failure'] || '✗'} Build failed: 0/${total} formats`));
        }

        // Show output paths
        console.log(chalk.gray('\nOutput files:'));
        for (const result of event.results) {
          if (result.success) {
            const icon = icons[result.format] || '📄';
            console.log(chalk.gray(`  ${icon} ${result.outputPath}`));
          }
        }
        break;
      }

      case 'watch-start':
        console.log(chalk.cyan(`\n${icons['watch'] || '👁'}  Watching for changes...`));
        console.log(chalk.gray('   Press Ctrl+C to stop\n'));
        break;

      case 'file-change':
        console.log(chalk.yellow(`\n${icons['file_change'] || '📝'} File changed: ${event.path}`));
        break;

      case 'error':
        console.log(chalk.red(`\n${icons['failure'] || '✗'} Error: ${event.error}`));
        break;
    }
  };
}
