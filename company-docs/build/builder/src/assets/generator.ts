/**
 * CWS Manual Builder — Asset Generator
 *
 * Generates derived visual assets from source files:
 *   Phase 1: Mermaid diagrams — .mmd → .png + .svg
 *   Phase 2: Grayscale variants — fig-*.png → fig-*-grayscale.png
 *   Phase 3: Color palette swatch — brand colors → fig-color-palette.png
 *
 * This is the TypeScript equivalent of `make assets`. Both produce
 * identical output — the Makefile is primary for editorial workflow,
 * this module integrates with the TS builder for publishing builds.
 *
 * Specification: editorial-style-guide.adoc (CWS-GDE-001)
 *
 * "Whatsoever thy hand findeth to do, do it with thy might."
 *   — Ecclesiastes 9:10
 */

import { execFile } from 'node:child_process';
import { readdir, stat, access } from 'node:fs/promises';
import { join, basename } from 'node:path';
import { promisify } from 'node:util';

import {
  BRAND_COLORS,
  type AssetResult,
  type PhaseSummary,
  type GenerationReport,
  type GenerateOptions,
} from './types.js';

const execFileAsync = promisify(execFile);

// =============================================================================
// Helpers
// =============================================================================

/** Check if a command exists on PATH. */
async function commandExists(cmd: string): Promise<boolean> {
  try {
    await execFileAsync('which', [cmd]);
    return true;
  } catch {
    return false;
  }
}

/** Check if file exists. */
async function fileExists(path: string): Promise<boolean> {
  try {
    await access(path);
    return true;
  } catch {
    return false;
  }
}

/** Get file modification time (ms since epoch). Returns 0 if missing. */
async function getMtime(path: string): Promise<number> {
  try {
    const s = await stat(path);
    return s.mtimeMs;
  } catch {
    return 0;
  }
}

/** Check if output is newer than source (cache hit). */
async function isUpToDate(output: string, source: string): Promise<boolean> {
  const outTime = await getMtime(output);
  const srcTime = await getMtime(source);
  return outTime > 0 && srcTime > 0 && outTime > srcTime;
}

// =============================================================================
// Phase 1: Mermaid Diagram Rendering
// =============================================================================

/**
 * Render all Mermaid source files to PNG and SVG.
 * Equivalent to the Phase 1 section of `make assets`.
 */
async function renderMermaidDiagrams(opts: GenerateOptions): Promise<PhaseSummary> {
  const results: AssetResult[] = [];
  const sourcesDir = join(opts.assetsDir, 'sources', 'diagrams');
  const pngDir = join(opts.assetsDir, 'figures', 'png');
  const svgDir = join(opts.assetsDir, 'figures', 'svg');
  const puppeteerConfig = join(opts.assetsDir, 'sources', 'config', 'puppeteer-config.json');
  const scale = opts.pngScale ?? 3;

  if (!(await commandExists('mmdc'))) {
    results.push({
      path: 'mmdc',
      status: 'skipped',
      error: 'mmdc not found (install @mermaid-js/mermaid-cli)',
    });
    return { name: 'Mermaid diagrams', results };
  }

  // Find all .mmd source files
  const files = (await readdir(sourcesDir)).filter(
    (f) => f.startsWith('fig-') && f.endsWith('.mmd')
  );

  for (const file of files) {
    const name = basename(file, '.mmd');
    const source = join(sourcesDir, file);
    const pngOut = join(pngDir, `${name}.png`);
    const svgOut = join(svgDir, `${name}.svg`);

    // PNG
    if (!opts.force && (await isUpToDate(pngOut, source))) {
      results.push({ path: `${name}.png`, status: 'up-to-date' });
    } else {
      try {
        await execFileAsync('mmdc', [
          '-i', source,
          '-o', pngOut,
          '-b', 'white',
          '-p', puppeteerConfig,
          '-s', String(scale),
        ]);
        results.push({ path: `${name}.png`, status: 'generated' });
      } catch (err) {
        results.push({
          path: `${name}.png`,
          status: 'failed',
          error: (err as Error).message,
        });
      }
    }

    // SVG
    if (!opts.force && (await isUpToDate(svgOut, source))) {
      results.push({ path: `${name}.svg`, status: 'up-to-date' });
    } else {
      try {
        await execFileAsync('mmdc', [
          '-i', source,
          '-o', svgOut,
          '-b', 'transparent',
          '-p', puppeteerConfig,
        ]);
        results.push({ path: `${name}.svg`, status: 'generated' });
      } catch (err) {
        results.push({
          path: `${name}.svg`,
          status: 'failed',
          error: (err as Error).message,
        });
      }
    }
  }

  return { name: 'Mermaid diagrams', results };
}

// =============================================================================
// Phase 2: Grayscale Variants
// =============================================================================

/** Files to exclude from grayscale generation. */
const GRAYSCALE_EXCLUDE = new Set([
  'fig-color-palette.png',
  'fig-title-page-mockup.png',
]);

/**
 * Generate grayscale variants of all figure PNGs.
 * Equivalent to Phase 2 of `make assets`.
 */
async function generateGrayscaleVariants(opts: GenerateOptions): Promise<PhaseSummary> {
  const results: AssetResult[] = [];
  const pngDir = join(opts.assetsDir, 'figures', 'png');
  const grayscaleDir = join(opts.assetsDir, 'figures', 'grayscale');

  if (!(await commandExists('magick'))) {
    results.push({
      path: 'magick',
      status: 'skipped',
      error: 'ImageMagick (magick) not found',
    });
    return { name: 'Grayscale variants', results };
  }

  const files = (await readdir(pngDir)).filter(
    (f) =>
      f.startsWith('fig-') &&
      f.endsWith('.png') &&
      !f.includes('-grayscale') &&
      !GRAYSCALE_EXCLUDE.has(f)
  );

  for (const file of files) {
    const name = basename(file, '.png');
    const source = join(pngDir, file);
    const output = join(grayscaleDir, `${name}-grayscale.png`);

    if (!opts.force && (await isUpToDate(output, source))) {
      results.push({ path: `${name}-grayscale.png`, status: 'up-to-date' });
    } else {
      try {
        await execFileAsync('magick', [source, '-colorspace', 'Gray', output]);
        results.push({ path: `${name}-grayscale.png`, status: 'generated' });
      } catch (err) {
        results.push({
          path: `${name}-grayscale.png`,
          status: 'failed',
          error: (err as Error).message,
        });
      }
    }
  }

  return { name: 'Grayscale variants', results };
}

// =============================================================================
// Phase 3: Color Palette Swatch
// =============================================================================

/**
 * Generate the brand color palette swatch image.
 * Equivalent to Phase 3 of `make assets`.
 *
 * Uses ImageMagick to create a 900x220 labeled color swatch with all
 * five brand colors, their hex codes, and functional roles.
 */
async function generatePaletteSwatch(opts: GenerateOptions): Promise<PhaseSummary> {
  const results: AssetResult[] = [];
  const output = join(opts.assetsDir, 'figures', 'png', 'fig-color-palette.png');
  const themeFile = join(opts.assetsDir, 'sources', 'config', 'mermaid-config.json');

  if (!(await commandExists('magick'))) {
    results.push({
      path: 'fig-color-palette.png',
      status: 'skipped',
      error: 'ImageMagick (magick) not found',
    });
    return { name: 'Color palette swatch', results };
  }

  // Cache check against theme file (colors could change)
  if (!opts.force && (await fileExists(output)) && (await isUpToDate(output, themeFile))) {
    results.push({ path: 'fig-color-palette.png', status: 'up-to-date' });
    return { name: 'Color palette swatch', results };
  }

  // Build ImageMagick command
  // Layout: 5 color blocks (150x120) with labels and hex codes
  const blockWidth = 150;
  const blockHeight = 120;
  const startX = 30;
  const spacing = 165; // blockWidth + 15px gap
  const topY = 20;

  const args: string[] = ['-size', '900x220', 'xc:white'];

  // Composite color blocks
  for (let i = 0; i < BRAND_COLORS.length; i++) {
    const color = BRAND_COLORS[i];
    if (!color) continue;
    const x = startX + i * spacing;

    if (color.name === 'Light') {
      // Light needs a visible border
      args.push(
        '(', '-size', `${blockWidth}x${blockHeight}`, `xc:${color.hex}`,
        '-bordercolor', BRAND_COLORS[3]?.hex ?? '#2D3748', '-border', '1', ')',
        '-geometry', `+${x}+${topY - 1}`, '-composite'
      );
    } else {
      args.push(
        '(', '-size', `${blockWidth}x${blockHeight}`, `xc:${color.hex}`, ')',
        '-geometry', `+${x}+${topY}`, '-composite'
      );
    }
  }

  // Color name labels (bold, centered on blocks)
  args.push('-font', 'Noto-Sans-Bold', '-pointsize', '14');

  const labelPositions = [72, 223, 410, 585, 740];
  const labelColors = ['white', 'white', BRAND_COLORS[0]?.hex ?? '#1A365D', 'white', BRAND_COLORS[3]?.hex ?? '#2D3748'];

  for (let i = 0; i < BRAND_COLORS.length; i++) {
    const color = BRAND_COLORS[i];
    const labelX = labelPositions[i];
    const labelColor = labelColors[i];
    if (!color || labelX === undefined || !labelColor) continue;

    args.push('-fill', labelColor, '-annotate', `+${labelX}+110`, color.name);
  }

  // Hex code labels (regular)
  args.push('-font', 'Noto-Sans-Regular', '-pointsize', '12', '-fill', BRAND_COLORS[3]?.hex ?? '#2D3748');

  const hexPositions = [62, 225, 393, 558, 723];
  for (let i = 0; i < BRAND_COLORS.length; i++) {
    const color = BRAND_COLORS[i];
    const hexX = hexPositions[i];
    if (!color || hexX === undefined) continue;
    args.push('-annotate', `+${hexX}+165`, color.hex);
  }

  // Role labels (regular, gray)
  args.push('-font', 'Noto-Sans-Regular', '-pointsize', '11', '-fill', 'gray');

  const rolePositions = [56, 218, 400, 560, 710];
  for (let i = 0; i < BRAND_COLORS.length; i++) {
    const color = BRAND_COLORS[i];
    const roleX = rolePositions[i];
    if (!color || roleX === undefined) continue;
    args.push('-annotate', `+${roleX}+190`, color.role);
  }

  args.push('-depth', '8', output);

  try {
    await execFileAsync('magick', args);
    results.push({ path: 'fig-color-palette.png', status: 'generated' });
  } catch (err) {
    results.push({
      path: 'fig-color-palette.png',
      status: 'failed',
      error: (err as Error).message,
    });
  }

  return { name: 'Color palette swatch', results };
}

// =============================================================================
// Main Generator
// =============================================================================

/**
 * Generate all derived assets.
 * Runs all three phases and returns a complete report.
 */
export async function generateAssets(opts: GenerateOptions): Promise<GenerationReport> {
  const phases: PhaseSummary[] = [];

  // Phase 1: Mermaid diagrams
  phases.push(await renderMermaidDiagrams(opts));

  // Phase 2: Grayscale variants
  phases.push(await generateGrayscaleVariants(opts));

  // Phase 3: Color palette swatch
  phases.push(await generatePaletteSwatch(opts));

  // Compute totals
  const totals = { generated: 0, upToDate: 0, failed: 0, skipped: 0 };
  for (const phase of phases) {
    for (const result of phase.results) {
      switch (result.status) {
        case 'generated': totals.generated++; break;
        case 'up-to-date': totals.upToDate++; break;
        case 'failed': totals.failed++; break;
        case 'skipped': totals.skipped++; break;
      }
    }
  }

  return { phases, totals };
}
