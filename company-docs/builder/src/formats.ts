/**
 * CWS Manual Builder - Format Handlers
 *
 * Handles conversion to different output formats using Asciidoctor.
 * Each format has specific command-line arguments and requirements.
 */

import { spawn } from 'child_process';
import { existsSync } from 'fs';
import { mkdir } from 'fs/promises';
import { join, dirname } from 'path';
import type { BuildConfig, OutputFormat } from './config.js';

/**
 * Build result for a single format
 */
export interface BuildResult {
  format: OutputFormat;
  success: boolean;
  outputPath: string;
  duration: number;
  error?: string;
}

/**
 * Execute a shell command and return a promise
 */
function executeCommand(
  command: string,
  args: string[],
  cwd: string
): Promise<{ stdout: string; stderr: string; code: number }> {
  return new Promise((resolve) => {
    const proc = spawn(command, args, { cwd, shell: true });
    let stdout = '';
    let stderr = '';

    proc.stdout.on('data', (data) => {
      stdout += data.toString();
    });

    proc.stderr.on('data', (data) => {
      stderr += data.toString();
    });

    proc.on('close', (code) => {
      resolve({ stdout, stderr, code: code ?? 1 });
    });

    proc.on('error', (err) => {
      resolve({ stdout, stderr: err.message, code: 1 });
    });
  });
}

/**
 * Build attribute arguments for Asciidoctor
 */
function buildAttributeArgs(attributes: Record<string, string>): string[] {
  const args: string[] = [];
  for (const [key, value] of Object.entries(attributes)) {
    if (value === '') {
      args.push('-a', key);
    } else {
      args.push('-a', `${key}=${value}`);
    }
  }
  return args;
}

/**
 * Format handler interface
 */
interface FormatHandler {
  command: string;
  checkCommand: string;
  buildArgs: (config: BuildConfig, outputPath: string) => string[];
  extension: string;
  installHint: string;
}

/**
 * Format handlers for each output type
 */
const formatHandlers: Record<OutputFormat, FormatHandler> = {
  html: {
    command: 'asciidoctor',
    checkCommand: 'asciidoctor --version',
    extension: '.html',
    installHint: 'gem install asciidoctor',
    buildArgs: (config, outputPath) => {
      const args = [
        '-b', 'html5',
        '-d', 'book',
        '-o', outputPath,
        ...buildAttributeArgs(config.attributes),
      ];

      const opts = config.formatOptions.html;
      if (opts.standalone) args.push('-s');
      if (opts.toc) {
        args.push('-a', 'toc=left');
        args.push('-a', `toc-position=${opts.tocPosition}`);
      }
      if (opts.embedImages) {
        args.push('-a', 'data-uri');
      }
      if (opts.stylesheet) {
        args.push('-a', `stylesheet=${opts.stylesheet}`);
      }

      args.push(join(config.sourceDir, config.masterDocument));
      return args;
    },
  },

  pdf: {
    command: 'asciidoctor-pdf',
    checkCommand: 'asciidoctor-pdf --version',
    extension: '.pdf',
    installHint: 'gem install asciidoctor-pdf',
    buildArgs: (config, outputPath) => {
      const args = [
        '-d', 'book',
        '-o', outputPath,
        ...buildAttributeArgs(config.attributes),
      ];

      const opts = config.formatOptions.pdf;
      if (opts.theme) {
        args.push('-a', `pdf-theme=${opts.theme}`);
      }
      if (opts.fontsDir) {
        args.push('-a', `pdf-fontsdir=${opts.fontsDir}`);
      }
      args.push('-a', `pdf-page-size=${opts.paperSize}`);

      args.push(join(config.sourceDir, config.masterDocument));
      return args;
    },
  },

  epub: {
    command: 'asciidoctor-epub3',
    checkCommand: 'asciidoctor-epub3 --version',
    extension: '.epub',
    installHint: 'gem install asciidoctor-epub3',
    buildArgs: (config, outputPath) => {
      const args = [
        '-d', 'book',
        '-o', outputPath,
        ...buildAttributeArgs(config.attributes),
      ];

      const opts = config.formatOptions.epub;
      if (opts.coverImage) {
        args.push('-a', `front-cover-image=${opts.coverImage}`);
      }
      if (opts.stylesheet) {
        args.push('-a', `epub3-stylesdir=${dirname(opts.stylesheet)}`);
      }

      args.push(join(config.sourceDir, config.masterDocument));
      return args;
    },
  },

  docbook: {
    command: 'asciidoctor',
    checkCommand: 'asciidoctor --version',
    extension: '.xml',
    installHint: 'gem install asciidoctor',
    buildArgs: (config, outputPath) => {
      const args = [
        '-b', 'docbook5',
        '-d', 'book',
        '-o', outputPath,
        ...buildAttributeArgs(config.attributes),
      ];

      args.push(join(config.sourceDir, config.masterDocument));
      return args;
    },
  },
};

/**
 * Check if a format's required tool is installed
 */
export async function checkToolInstalled(format: OutputFormat): Promise<boolean> {
  const handler = formatHandlers[format];
  const result = await executeCommand(handler.checkCommand, [], process.cwd());
  return result.code === 0;
}

/**
 * Get installation hint for a format
 */
export function getInstallHint(format: OutputFormat): string {
  return formatHandlers[format].installHint;
}

/**
 * Build a single format
 */
export async function buildFormat(
  format: OutputFormat,
  config: BuildConfig
): Promise<BuildResult> {
  const startTime = Date.now();
  const handler = formatHandlers[format];
  const outputPath = join(
    config.outputDir,
    `${config.outputName}${handler.extension}`
  );

  // Ensure output directory exists
  if (!existsSync(config.outputDir)) {
    await mkdir(config.outputDir, { recursive: true });
  }

  // Check if tool is installed
  const isInstalled = await checkToolInstalled(format);
  if (!isInstalled) {
    return {
      format,
      success: false,
      outputPath,
      duration: Date.now() - startTime,
      error: `${handler.command} not found. Install with: ${handler.installHint}`,
    };
  }

  // Build the document
  const args = handler.buildArgs(config, outputPath);
  const result = await executeCommand(handler.command, args, config.sourceDir);

  const duration = Date.now() - startTime;

  if (result.code !== 0) {
    return {
      format,
      success: false,
      outputPath,
      duration,
      error: result.stderr || 'Unknown error during build',
    };
  }

  // Verify output was created
  if (!existsSync(outputPath)) {
    return {
      format,
      success: false,
      outputPath,
      duration,
      error: 'Build completed but output file not found',
    };
  }

  return {
    format,
    success: true,
    outputPath,
    duration,
  };
}

/**
 * Build multiple formats in parallel
 */
export async function buildFormats(
  formats: OutputFormat[],
  config: BuildConfig
): Promise<BuildResult[]> {
  const promises = formats.map((format) => buildFormat(format, config));
  return Promise.all(promises);
}

/**
 * Get all supported formats
 */
export function getSupportedFormats(): OutputFormat[] {
  return Object.keys(formatHandlers) as OutputFormat[];
}
