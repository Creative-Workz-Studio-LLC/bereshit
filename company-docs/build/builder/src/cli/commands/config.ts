/**
 * CWS Manual Builder — Config Command
 *
 * Command: config — inspect, validate, or generate derived config files
 */

import chalk from 'chalk';
import type { Command } from 'commander';
import type { BuildConfig, RuntimePaths } from '../../config/types.js';
import { delegateToMake, exitWithError } from '../utils.js';

// =============================================================================
// Command Registration
// =============================================================================

export function registerConfigCommand(
  program: Command,
  config: BuildConfig,
  paths: RuntimePaths,
): void {
  program
    .command('config')
    .description('Inspect, validate, or generate derived config files from build.config.yaml')
    .option('--show', 'Display the full resolved configuration')
    .option('--validate', 'Validate config structure and report issues')
    .option('--make', 'Regenerate config.mk from build.config.yaml')
    .option('--types', 'Generate TypeScript types from build.config.yaml')
    .option('--json', 'Output as JSON (with --show)')
    .option('-o, --output <file>', 'Output file path (with --types)')
    .action(async (opts: { show?: boolean; validate?: boolean; make?: boolean; types?: boolean; json?: boolean; output?: string }) => {
      // Default to --show if no flags given
      if (!opts.show && !opts.validate && !opts.make && !opts.types) {
        opts.show = true;
      }

      if (opts.validate) {
        console.log();
        console.log(chalk.cyan('Config Validation'));
        console.log(chalk.cyan('=================\n'));

        // Required sections
        const requiredSections: Array<keyof typeof config> = [
          'document', 'attributes', 'formats', 'watch', 'display',
        ];
        const optionalSections: Array<keyof typeof config> = [
          'assets', 'scripture', 'paths', 'scopes', 'brand',
          'typography', 'page_layout',
          'legal_language', 'tone_and_voice', 'industry_specific',
          'document_types', 'asciidoc_publishing', 'company_conventions',
        ];

        let issues = 0;

        for (const section of requiredSections) {
          if (section in config) {
            console.log(chalk.green(`  PASS  §${section} — present`));
          } else {
            console.log(chalk.red(`  FAIL  §${section} — MISSING (required)`));
            issues++;
          }
        }

        for (const section of optionalSections) {
          if (section in config) {
            console.log(chalk.green(`  OK    §${section} — present`));
          } else {
            console.log(chalk.gray(`  SKIP  §${section} — not defined (optional)`));
          }
        }

        // Format count
        const formatCount = Object.keys(config.formats).length;
        const enabledCount = Object.values(config.formats).filter(f => f.enabled).length;
        console.log(chalk.gray(`\n  Formats: ${formatCount} defined, ${enabledCount} enabled`));

        // Scope count
        if (config.scopes) {
          const scopeCount = Object.keys(config.scopes).length;
          console.log(chalk.gray(`  Scopes: ${scopeCount} defined (${Object.keys(config.scopes).join(', ')})`));
        }

        console.log(chalk.gray(`  Version: ${config.version}\n`));

        if (issues > 0) {
          exitWithError('E20', `Config validation: ${issues} required section(s) missing`);
        } else {
          console.log(chalk.green('  Config is valid.\n'));
        }
      }

      if (opts.make) {
        console.log(chalk.cyan('Regenerating config.mk...\n'));
        await delegateToMake('config', paths.sourceDir);
      }

      if (opts.types) {
        const { generateTypesFromYaml } = await import('../../config/gen-types.js');
        const source = generateTypesFromYaml(paths.configFile);

        if (opts.output) {
          const { writeFileSync } = await import('fs');
          writeFileSync(opts.output, source, 'utf-8');
          console.log(chalk.green(`  Generated: ${opts.output}`));
        } else {
          // Default output: src/config/generated-types.ts
          const { resolve: resolvePath } = await import('path');
          const defaultOutput = resolvePath(paths.sourceDir, 'builder/src/config/generated-types.ts');
          const { writeFileSync } = await import('fs');
          writeFileSync(defaultOutput, source, 'utf-8');
          console.log(chalk.green(`  Generated: ${defaultOutput}`));
        }
      }

      if (opts.show) {
        if (opts.json) {
          console.log(JSON.stringify(config, null, 2));
        } else {
          console.log();
          console.log(chalk.cyan('Build Configuration'));
          console.log(chalk.cyan('===================\n'));
          console.log(chalk.gray(`  Config file: ${paths.configFile}`));
          console.log(chalk.gray(`  Version:     ${config.version}`));
          console.log(chalk.gray(`  Output dir:  ${paths.outputDir}`));
          console.log(chalk.gray(`  Master doc:  ${paths.masterDocument}`));
          console.log(chalk.gray(`  Book dir:    ${paths.bookDir}`));

          // Formats
          console.log(chalk.white('\n  Formats:'));
          for (const [name, fmt] of Object.entries(config.formats)) {
            const status = fmt.enabled ? chalk.green('enabled') : chalk.gray('disabled');
            console.log(`    ${name}: ${status} (${fmt.command})`);
          }

          // Scopes
          if (config.scopes) {
            console.log(chalk.white('\n  Scopes:'));
            for (const [key, scope] of Object.entries(config.scopes)) {
              console.log(`    ${key}: ${scope.name} → ${scope.output}`);
            }
          }

          // Brand
          if (config.brand?.colors) {
            console.log(chalk.white('\n  Brand Colors:'));
            for (const [name, hex] of Object.entries(config.brand.colors)) {
              console.log(`    ${name}: ${hex}`);
            }
          }

          console.log();
        }
      }
    });
}
