/**
 * CWS Manual Builder — Theme Typography Validation
 *
 * Validates theme YAML against the canonical typography specification
 * defined in build.config.yaml §11 Typography.
 *
 * Eight validation domains:
 *   1. Font-sizes  — depth hierarchy + supporting sizes
 *   2. Font-family — element assignments match canonical font stack
 *   3. Text-align  — heading/part/chapter alignment (primary theme)
 *   4. Legal       — Butterick standards (serif, indent, uppercase)
 *   5. Colors      — font-color per element (WCAG compliance)
 *   6. Roles       — custom role colors match canonical values
 *   7. Styles      — font-style per element (bold, italic, normal)
 *   8. Elements    — table/list/caption/image element settings
 *
 * "Prove all things; hold fast that which is good."
 *  — 1 Thessalonians 5:21
 */

import { readFileSync } from 'fs';
import { parse as parseYaml } from 'yaml';
import type { ValidationFinding } from '../types.js';
import type {
  TypographyConfig,
  DepthLevel,
  PageLayoutConfig,
  ThemeVariantOverrides,
} from '../../config/types.js';

// eslint-disable-next-line @typescript-eslint/no-explicit-any
type ThemeData = Record<string, any>;

// =============================================================================
// Helpers
// =============================================================================

/**
 * Extract a numeric font-size from a theme element.
 * Returns undefined for variable references ($base-font-size) and missing values.
 */
function numericSize(element: ThemeData | undefined): number | undefined {
  if (!element) return undefined;
  const val = element['font-size'];
  return typeof val === 'number' ? val : undefined;
}

/**
 * Pre-process asciidoctor-pdf theme YAML for JS parsing.
 *
 * The JS `yaml` package treats unquoted `#hex` values as comments
 * (YAML spec: `#` preceded by whitespace starts a comment).
 * Asciidoctor-pdf's Ruby YAML parser handles this differently.
 *
 * This function quotes bare hex color values so they survive parsing.
 * Pattern: `key: #RRGGBB` → `key: "#RRGGBB"`
 */
function quoteHexColors(raw: string): string {
  // Match lines like `  font-color: #2D2D2D` or `  border-color: #E2E8F0  # comment`
  // Captures: (key: )(#hex)(rest of line)
  return raw.replace(/^(\s*[\w-]+:\s+)(#[0-9A-Fa-f]{3,8})\b/gm, '$1"$2"');
}

/**
 * Parse theme YAML file into ThemeData.
 * Returns null on parse failure (finding added to array).
 */
function parseTheme(themeFile: string, findings: ValidationFinding[]): ThemeData | null {
  try {
    const raw = readFileSync(themeFile, 'utf-8');
    return parseYaml(quoteHexColors(raw));
  } catch (err: unknown) {
    findings.push({
      severity: 'fail',
      layer: 'base',
      category: 'theme-typography',
      message: `Theme parse error: ${(err as Error).message}`,
      path: themeFile,
    });
    return null;
  }
}

// =============================================================================
// Theme Variant Merge
// =============================================================================

/**
 * Merge per-theme overrides into canonical typography + page layout.
 *
 * Returns fresh copies — the canonical configs are never mutated.
 * Null alignment values become empty string, which causes validators
 * to skip the check (existing `if (!check.expected) continue;` logic).
 */
export function mergeThemeOverrides(
  canonical: TypographyConfig,
  pageLayout: PageLayoutConfig,
  overrides: ThemeVariantOverrides,
): { typography: TypographyConfig; pageLayout: PageLayoutConfig } {
  // Deep-clone both configs (JSON round-trip — configs are plain data)
  const typo: TypographyConfig = JSON.parse(JSON.stringify(canonical));
  const layout: PageLayoutConfig = JSON.parse(JSON.stringify(pageLayout));

  // --- body_size ---
  if (overrides.body_size !== undefined) {
    typo.body_size = overrides.body_size;
  }

  // --- depth font sizes ---
  if (overrides.depth) {
    for (const [key, val] of Object.entries(overrides.depth)) {
      if (typo.depth[key]) {
        typo.depth[key].font_size = val.font_size;
      }
    }
  }

  // --- supporting sizes ---
  if (overrides.supporting) {
    Object.assign(typo.supporting, overrides.supporting);
  }

  // --- font_assignments ---
  if (overrides.font_assignments) {
    Object.assign(typo.font_assignments, overrides.font_assignments);
  }

  // --- alignment (null → empty string = skip check) ---
  if (overrides.alignment) {
    for (const [key, val] of Object.entries(overrides.alignment)) {
      if (key in typo.alignment) {
        (typo.alignment as unknown as Record<string, string>)[key] = val ?? '';
      }
    }
  }

  // --- page_layout ---
  if (overrides.page_layout) {
    Object.assign(layout, overrides.page_layout);
  }

  return { typography: typo, pageLayout: layout };
}

// =============================================================================
// 1. Font-Size Validation
// =============================================================================

/**
 * Validate theme font-sizes against the typography config.
 */
export function validateThemeFontSizes(themeFile: string, typography: TypographyConfig): ValidationFinding[] {
  const findings: ValidationFinding[] = [];
  const theme = parseTheme(themeFile, findings);
  if (!theme) return findings;

  // --- Depth hierarchy checks ---
  const depthChecks: Array<{
    key: string;
    depth: DepthLevel;
    actual: number | undefined;
    source: string;
  }> = [];

  // Body size (anchor)
  depthChecks.push({
    key: 'body',
    depth: {
      asciidoc: '',
      theme_key: 'base',
      label: 'Body',
      font_size: typography.body_size,
      scale_step: 0,
      font: '',
    },
    actual: numericSize(theme.base),
    source: 'base.font-size',
  });

  // Part
  if (typography.depth.part && theme.part) {
    depthChecks.push({
      key: 'part',
      depth: typography.depth.part,
      actual: numericSize(theme.part),
      source: 'part.font-size',
    });
  }

  // Chapter (maps to h1/Article in depth)
  if (typography.depth.h1 && theme.chapter) {
    depthChecks.push({
      key: 'chapter',
      depth: typography.depth.h1,
      actual: numericSize(theme.chapter),
      source: 'chapter.font-size',
    });
  }

  // Headings h1-h6
  const headingLevels = ['h1', 'h2', 'h3', 'h4', 'h5', 'h6'] as const;
  for (const level of headingLevels) {
    const depthEntry = typography.depth[level];
    if (depthEntry) {
      depthChecks.push({
        key: level,
        depth: depthEntry,
        actual: numericSize(theme.heading?.[level]),
        source: `heading.${level}.font-size`,
      });
    }
  }

  // Run depth checks
  for (const check of depthChecks) {
    if (check.actual === undefined) {
      // Variable reference ($base-font-size) or missing — skip
      continue;
    }
    if (check.actual === check.depth.font_size) {
      findings.push({
        severity: 'pass',
        layer: 'base',
        category: 'theme-typography',
        message: `${check.depth.label} (${check.source}): ${check.actual}pt — matches type scale step ${check.depth.scale_step > 0 ? '+' : ''}${check.depth.scale_step}`,
      });
    } else {
      findings.push({
        severity: 'fail',
        layer: 'base',
        category: 'theme-typography',
        message: `${check.depth.label} (${check.source}): expected ${check.depth.font_size}pt (step ${check.depth.scale_step > 0 ? '+' : ''}${check.depth.scale_step}), got ${check.actual}pt`,
        path: themeFile,
      });
    }
  }

  // --- Supporting size checks ---
  const supportingChecks = [
    {
      label: 'Code',
      expected: typography.supporting.code,
      actual: numericSize(theme.code),
      source: 'code.font-size',
    },
    {
      label: 'Footnotes',
      expected: typography.supporting.footnotes,
      actual: numericSize(theme.footnotes),
      source: 'footnotes.font-size',
    },
    {
      label: 'Title page',
      expected: typography.supporting.title_page,
      actual: numericSize(theme['title-page']?.title),
      source: 'title-page.title.font-size',
    },
    {
      label: 'Header',
      expected: typography.supporting.header_footer,
      actual: numericSize(theme.header),
      source: 'header.font-size',
    },
    {
      label: 'Footer',
      expected: typography.supporting.header_footer,
      actual: numericSize(theme.footer),
      source: 'footer.font-size',
    },
    {
      label: 'TOC',
      expected: typography.supporting.toc,
      actual: numericSize(theme.toc),
      source: 'toc.font-size',
    },
  ];

  for (const check of supportingChecks) {
    if (check.actual === undefined) {
      continue;
    }
    if (check.actual === check.expected) {
      findings.push({
        severity: 'pass',
        layer: 'base',
        category: 'theme-typography',
        message: `${check.label} (${check.source}): ${check.actual}pt`,
      });
    } else {
      findings.push({
        severity: 'fail',
        layer: 'base',
        category: 'theme-typography',
        message: `${check.label} (${check.source}): expected ${check.expected}pt, got ${check.actual}pt`,
        path: themeFile,
      });
    }
  }

  return findings;
}

// =============================================================================
// 2. Font-Family Validation
// =============================================================================

/**
 * Validate theme font-family assignments against the typography config.
 *
 * Checks each theme element's font-family against the canonical
 * font_assignments map in build.config.yaml §11.
 */
export function validateThemeFontFamilies(themeFile: string, typography: TypographyConfig): ValidationFinding[] {
  const findings: ValidationFinding[] = [];
  const theme = parseTheme(themeFile, findings);
  if (!theme) return findings;

  const assignments = typography.font_assignments;
  if (!assignments) {
    findings.push({
      severity: 'info',
      layer: 'base',
      category: 'theme-fonts',
      message: 'No font_assignments in typography config — skipping font-family validation',
    });
    return findings;
  }

  // Build check list: [label, expected font, actual font, YAML path]
  const fontChecks = [
    // Core elements
    { label: 'Base (body)', expected: assignments.base, actual: theme.base?.['font-family'], source: 'base.font-family' },
    { label: 'Heading', expected: assignments.heading, actual: theme.heading?.['font-family'], source: 'heading.font-family' },
    { label: 'Part', expected: assignments.part, actual: theme.part?.['font-family'], source: 'part.font-family' },
    { label: 'Chapter', expected: assignments.chapter, actual: theme.chapter?.['font-family'], source: 'chapter.font-family' },
    { label: 'TOC', expected: assignments.toc, actual: theme.toc?.['font-family'], source: 'toc.font-family' },
    { label: 'Code', expected: assignments.code, actual: theme.code?.['font-family'], source: 'code.font-family' },
    { label: 'Codespan', expected: assignments.codespan, actual: theme.codespan?.['font-family'], source: 'codespan.font-family' },
    // Structural elements
    { label: 'Footnotes', expected: assignments.footnotes, actual: theme.footnotes?.['font-family'], source: 'footnotes.font-family' },
    { label: 'Header', expected: assignments.header, actual: theme.header?.['font-family'], source: 'header.font-family' },
    { label: 'Footer', expected: assignments.footer, actual: theme.footer?.['font-family'], source: 'footer.font-family' },
    { label: 'Caption', expected: assignments.caption, actual: theme.caption?.['font-family'], source: 'caption.font-family' },
    // Sidebar title
    { label: 'Sidebar title', expected: assignments.sidebar_title, actual: theme.sidebar?.title?.['font-family'], source: 'sidebar.title.font-family' },
    // Admonition label
    { label: 'Admonition label', expected: assignments.admonition_label, actual: theme.admonition?.label?.['font-family'], source: 'admonition.label.font-family' },
    // Title page elements
    { label: 'Title page title', expected: assignments.title_page_title, actual: theme['title-page']?.title?.['font-family'], source: 'title-page.title.font-family' },
    { label: 'Title page subtitle', expected: assignments.title_page_subtitle, actual: theme['title-page']?.subtitle?.['font-family'], source: 'title-page.subtitle.font-family' },
    { label: 'Title page authors', expected: assignments.title_page_authors, actual: theme['title-page']?.authors?.['font-family'], source: 'title-page.authors.font-family' },
    { label: 'Title page revision', expected: assignments.title_page_revision, actual: theme['title-page']?.revision?.['font-family'], source: 'title-page.revision.font-family' },
  ];

  for (const check of fontChecks) {
    if (!check.expected) continue; // No config entry for this element
    if (!check.actual) {
      // Element doesn't set font-family — might inherit.
      // Only warn for elements that should be explicit.
      findings.push({
        severity: 'info',
        layer: 'base',
        category: 'theme-fonts',
        message: `${check.label} (${check.source}): not set — inherits from parent`,
      });
      continue;
    }
    if (check.actual === check.expected) {
      findings.push({
        severity: 'pass',
        layer: 'base',
        category: 'theme-fonts',
        message: `${check.label} (${check.source}): ${check.actual}`,
      });
    } else {
      findings.push({
        severity: 'fail',
        layer: 'base',
        category: 'theme-fonts',
        message: `${check.label} (${check.source}): expected "${check.expected}", got "${check.actual}"`,
        path: themeFile,
      });
    }
  }

  return findings;
}

// =============================================================================
// 3. Text-Align Validation
// =============================================================================

/**
 * Validate theme text-align settings against the typography config.
 *
 * Only validates the primary theme (paths.theme_file).
 * Derivative themes define their own alignment — this check validates
 * that the PRIMARY theme matches the canonical alignment spec.
 */
export function validateThemeAlignment(themeFile: string, typography: TypographyConfig): ValidationFinding[] {
  const findings: ValidationFinding[] = [];
  const theme = parseTheme(themeFile, findings);
  if (!theme) return findings;

  const alignment = typography.alignment;
  if (!alignment) {
    findings.push({
      severity: 'info',
      layer: 'base',
      category: 'theme-alignment',
      message: 'No alignment in typography config — skipping text-align validation',
    });
    return findings;
  }

  const alignChecks = [
    {
      label: 'Heading',
      expected: alignment.heading,
      actual: theme.heading?.['text-align'],
      source: 'heading.text-align',
    },
    {
      label: 'Part',
      expected: alignment.part,
      actual: theme.part?.['text-align'],
      source: 'part.text-align',
    },
    {
      label: 'Chapter',
      expected: alignment.chapter,
      actual: theme.chapter?.['text-align'],
      source: 'chapter.text-align',
    },
  ];

  for (const check of alignChecks) {
    if (!check.expected) continue;
    if (!check.actual) {
      findings.push({
        severity: 'warn',
        layer: 'base',
        category: 'theme-alignment',
        message: `${check.label} (${check.source}): not set — expected "${check.expected}"`,
        path: themeFile,
      });
      continue;
    }
    if (check.actual === check.expected) {
      findings.push({
        severity: 'pass',
        layer: 'base',
        category: 'theme-alignment',
        message: `${check.label} (${check.source}): ${check.actual}`,
      });
    } else {
      findings.push({
        severity: 'fail',
        layer: 'base',
        category: 'theme-alignment',
        message: `${check.label} (${check.source}): expected "${check.expected}", got "${check.actual}"`,
        path: themeFile,
      });
    }
  }

  return findings;
}

// =============================================================================
// 4. Legal Typography Validation (Butterick Standards)
// =============================================================================

/**
 * Validate theme against legal typography standards (Butterick).
 *
 * Checks legal/governance themes for compliance with:
 *   - Serif body text
 *   - Prose first-line indent
 *   - Uppercase heading restrictions
 *   - Letter-spacing for uppercase headings
 *   - Line height percentage
 *
 * Guide §Legal Typography (Butterick Standards)
 */
export function validateLegalTypography(themeFile: string, typography: TypographyConfig): ValidationFinding[] {
  const findings: ValidationFinding[] = [];
  const legal = typography.legal;
  if (!legal) {
    findings.push({
      severity: 'info',
      layer: 'base',
      category: 'theme-legal',
      message: 'No legal typography config — skipping Butterick validation',
    });
    return findings;
  }

  const theme = parseTheme(themeFile, findings);
  if (!theme) return findings;

  // --- Body serif check ---
  if (legal.body_serif) {
    const baseFont = theme.base?.['font-family'] as string | undefined;
    if (baseFont && baseFont.toLowerCase().includes('serif')) {
      findings.push({
        severity: 'pass',
        layer: 'base',
        category: 'theme-legal',
        message: `Butterick: body font is serif (${baseFont})`,
      });
    } else if (baseFont) {
      findings.push({
        severity: 'fail',
        layer: 'base',
        category: 'theme-legal',
        message: `Butterick: body font should be serif, got "${baseFont}"`,
        path: themeFile,
      });
    }
  }

  // --- Prose indent check ---
  if (legal.prose_indent) {
    const indent = theme.prose?.['text-indent'];
    if (indent === legal.prose_indent) {
      findings.push({
        severity: 'pass',
        layer: 'base',
        category: 'theme-legal',
        message: `Butterick: prose indent ${indent}`,
      });
    } else if (indent) {
      findings.push({
        severity: 'warn',
        layer: 'base',
        category: 'theme-legal',
        message: `Butterick: prose indent expected "${legal.prose_indent}", got "${indent}"`,
        path: themeFile,
      });
    } else {
      findings.push({
        severity: 'info',
        layer: 'base',
        category: 'theme-legal',
        message: 'Butterick: no prose indent set — acceptable for non-legal themes',
      });
    }
  }

  // --- Uppercase heading check ---
  if (legal.uppercase_heading) {
    const level = legal.uppercase_heading as string;
    const headingMap: Record<string, ThemeData | undefined> = {
      h1: theme.heading?.h1,
      h2: theme.heading?.h2,
      h3: theme.heading?.h3,
      h4: theme.heading?.h4,
      h5: theme.heading?.h5,
      h6: theme.heading?.h6,
    };

    const headingElement = headingMap[level];
    if (headingElement) {
      const transform = headingElement['text-transform'];
      if (transform === 'uppercase') {
        findings.push({
          severity: 'pass',
          layer: 'base',
          category: 'theme-legal',
          message: `Butterick: ${level} uses uppercase (restricted — other levels use bold)`,
        });
      } else {
        findings.push({
          severity: 'warn',
          layer: 'base',
          category: 'theme-legal',
          message: `Butterick: ${level} expected text-transform: uppercase, got "${transform || 'none'}"`,
          path: themeFile,
        });
      }

      // Letter-spacing for uppercase readability
      if (legal.uppercase_letter_spacing) {
        const spacing = headingElement['letter-spacing'];
        if (spacing === legal.uppercase_letter_spacing) {
          findings.push({
            severity: 'pass',
            layer: 'base',
            category: 'theme-legal',
            message: `Butterick: ${level} letter-spacing ${spacing} (uppercase readability)`,
          });
        } else {
          findings.push({
            severity: 'warn',
            layer: 'base',
            category: 'theme-legal',
            message: `Butterick: ${level} letter-spacing expected "${legal.uppercase_letter_spacing}", got "${spacing || 'none'}"`,
            path: themeFile,
          });
        }
      }
    }

    // Verify other heading levels do NOT use uppercase
    for (const [otherLevel, otherElement] of Object.entries(headingMap)) {
      if (otherLevel === level || !otherElement) continue;
      if (otherElement['text-transform'] === 'uppercase') {
        findings.push({
          severity: 'warn',
          layer: 'base',
          category: 'theme-legal',
          message: `Butterick: ${otherLevel} uses uppercase — ALL CAPS reduces readability. Only ${level} should use uppercase.`,
          path: themeFile,
        });
      }
    }
  }

  // --- Line height check ---
  if (legal.line_height_percent) {
    const bodySize = numericSize(theme.base);
    if (bodySize && typography.line_height) {
      const percent = Math.round((typography.line_height / bodySize) * 100);
      if (percent >= 120) {
        findings.push({
          severity: 'pass',
          layer: 'base',
          category: 'theme-legal',
          message: `Butterick: line height ${percent}% (${typography.line_height}pt / ${bodySize}pt) — meets minimum 120%`,
        });
      } else {
        findings.push({
          severity: 'warn',
          layer: 'base',
          category: 'theme-legal',
          message: `Butterick: line height ${percent}% — below recommended 120% minimum`,
          path: themeFile,
        });
      }
    }
  }

  return findings;
}

// =============================================================================
// 5. Color Validation (Font-Color)
// =============================================================================

/**
 * Validate theme font-color assignments against the typography config.
 *
 * Checks each theme element's font-color against the canonical
 * color assignments in build.config.yaml §11.
 *
 * Guide §WCAG Color Compliance + §Font Styling
 */
export function validateThemeColors(themeFile: string, typography: TypographyConfig): ValidationFinding[] {
  const findings: ValidationFinding[] = [];
  const colors = typography.colors;
  if (!colors) {
    findings.push({
      severity: 'info',
      layer: 'base',
      category: 'theme-colors',
      message: 'No colors in typography config — skipping font-color validation',
    });
    return findings;
  }

  const theme = parseTheme(themeFile, findings);
  if (!theme) return findings;

  // Build check list: [label, expected hex, actual hex, YAML path]
  const colorChecks = [
    // Core text elements
    { label: 'Body', expected: colors.body, actual: theme.base?.['font-color'], source: 'base.font-color' },
    { label: 'Heading', expected: colors.heading, actual: theme.heading?.['font-color'], source: 'heading.font-color' },
    { label: 'Part', expected: colors.part, actual: theme.part?.['font-color'], source: 'part.font-color' },
    { label: 'Chapter', expected: colors.chapter, actual: theme.chapter?.['font-color'], source: 'chapter.font-color' },
    // Lead & quote
    { label: 'Lead', expected: colors.lead, actual: theme.lead?.['font-color'], source: 'lead.font-color' },
    { label: 'Quote', expected: colors.quote, actual: theme.quote?.['font-color'], source: 'quote.font-color' },
    // Structural elements
    { label: 'Header', expected: colors.header, actual: theme.header?.['font-color'], source: 'header.font-color' },
    { label: 'Footer', expected: colors.footer, actual: theme.footer?.['font-color'], source: 'footer.font-color' },
    { label: 'Caption', expected: colors.caption, actual: theme.caption?.['font-color'], source: 'caption.font-color' },
    { label: 'Footnotes', expected: colors.footnotes, actual: theme.footnotes?.['font-color'], source: 'footnotes.font-color' },
    { label: 'Link', expected: colors.link, actual: theme.link?.['font-color'], source: 'link.font-color' },
    { label: 'Code', expected: colors.code, actual: theme.code?.['font-color'], source: 'code.font-color' },
    // Title page elements
    { label: 'Title page title', expected: colors.title_page_title, actual: theme['title-page']?.title?.['font-color'], source: 'title-page.title.font-color' },
    { label: 'Title page subtitle', expected: colors.title_page_subtitle, actual: theme['title-page']?.subtitle?.['font-color'], source: 'title-page.subtitle.font-color' },
    { label: 'Title page authors', expected: colors.title_page_authors, actual: theme['title-page']?.authors?.['font-color'], source: 'title-page.authors.font-color' },
    { label: 'Title page revision', expected: colors.title_page_revision, actual: theme['title-page']?.revision?.['font-color'], source: 'title-page.revision.font-color' },
  ];

  for (const check of colorChecks) {
    if (!check.expected) continue;
    if (!check.actual) {
      findings.push({
        severity: 'info',
        layer: 'base',
        category: 'theme-colors',
        message: `${check.label} (${check.source}): not set — inherits from parent`,
      });
      continue;
    }
    // Normalize hex comparison (case-insensitive)
    const actual = check.actual as string;
    if (actual.toLowerCase() === check.expected.toLowerCase()) {
      findings.push({
        severity: 'pass',
        layer: 'base',
        category: 'theme-colors',
        message: `${check.label} (${check.source}): ${actual}`,
      });
    } else {
      findings.push({
        severity: 'fail',
        layer: 'base',
        category: 'theme-colors',
        message: `${check.label} (${check.source}): expected "${check.expected}", got "${actual}"`,
        path: themeFile,
      });
    }
  }

  return findings;
}

// =============================================================================
// 6. Role Color Validation
// =============================================================================

/**
 * Validate custom role colors in the theme against the typography config.
 *
 * Checks each role's font-color against the canonical role colors
 * in build.config.yaml §11.
 *
 * Guide §Custom Roles
 */
export function validateThemeRoles(themeFile: string, typography: TypographyConfig): ValidationFinding[] {
  const findings: ValidationFinding[] = [];
  const roles = typography.roles;
  if (!roles) {
    findings.push({
      severity: 'info',
      layer: 'base',
      category: 'theme-roles',
      message: 'No roles in typography config — skipping role color validation',
    });
    return findings;
  }

  const theme = parseTheme(themeFile, findings);
  if (!theme) return findings;

  if (!theme.role) {
    findings.push({
      severity: 'warn',
      layer: 'base',
      category: 'theme-roles',
      message: 'No role section in theme — expected custom roles',
      path: themeFile,
    });
    return findings;
  }

  // Check each defined role
  const roleChecks = [
    { name: 'gold', expected: roles.gold },
    { name: 'muted', expected: roles.muted },
    { name: 'blue', expected: roles.blue },
    { name: 'green', expected: roles.green },
    { name: 'purple', expected: roles.purple },
    { name: 'red', expected: roles.red },
  ];

  for (const check of roleChecks) {
    if (!check.expected) continue;
    const roleElement = theme.role[check.name];
    if (!roleElement) {
      findings.push({
        severity: 'warn',
        layer: 'base',
        category: 'theme-roles',
        message: `Role "${check.name}": not defined in theme`,
        path: themeFile,
      });
      continue;
    }
    const actual = roleElement['font-color'] as string | undefined;
    if (!actual) {
      findings.push({
        severity: 'warn',
        layer: 'base',
        category: 'theme-roles',
        message: `Role "${check.name}": no font-color set`,
        path: themeFile,
      });
      continue;
    }
    if (actual.toLowerCase() === check.expected.toLowerCase()) {
      findings.push({
        severity: 'pass',
        layer: 'base',
        category: 'theme-roles',
        message: `Role "${check.name}": ${actual}`,
      });
    } else {
      findings.push({
        severity: 'fail',
        layer: 'base',
        category: 'theme-roles',
        message: `Role "${check.name}": expected "${check.expected}", got "${actual}"`,
        path: themeFile,
      });
    }
  }

  return findings;
}

// =============================================================================
// 7. Font-Style Validation
// =============================================================================

/**
 * Validate theme font-style assignments against the typography config.
 *
 * Checks each element's font-style (bold, italic, normal) against
 * the canonical style assignments in build.config.yaml §11.
 *
 * Guide §Font Styling + §Headings
 */
export function validateThemeStyles(themeFile: string, typography: TypographyConfig): ValidationFinding[] {
  const findings: ValidationFinding[] = [];
  const styles = typography.styles;
  if (!styles) {
    findings.push({
      severity: 'info',
      layer: 'base',
      category: 'theme-styles',
      message: 'No styles in typography config — skipping font-style validation',
    });
    return findings;
  }

  const theme = parseTheme(themeFile, findings);
  if (!theme) return findings;

  // Build check list: [label, expected style, actual style, YAML path]
  const styleChecks = [
    // Core elements
    { label: 'Base (body)', expected: styles.base, actual: theme.base?.['font-style'], source: 'base.font-style' },
    { label: 'Heading', expected: styles.heading, actual: theme.heading?.['font-style'], source: 'heading.font-style' },
    { label: 'Part', expected: styles.part, actual: theme.part?.['font-style'], source: 'part.font-style' },
    { label: 'Chapter', expected: styles.chapter, actual: theme.chapter?.['font-style'], source: 'chapter.font-style' },
    { label: 'Code', expected: styles.code, actual: theme.code?.['font-style'], source: 'code.font-style' },
    { label: 'Codespan', expected: styles.codespan, actual: theme.codespan?.['font-style'], source: 'codespan.font-style' },
    // Structural elements
    { label: 'Footnotes', expected: styles.footnotes, actual: theme.footnotes?.['font-style'], source: 'footnotes.font-style' },
    { label: 'Header', expected: styles.header, actual: theme.header?.['font-style'], source: 'header.font-style' },
    { label: 'Footer', expected: styles.footer, actual: theme.footer?.['font-style'], source: 'footer.font-style' },
    { label: 'Caption', expected: styles.caption, actual: theme.caption?.['font-style'], source: 'caption.font-style' },
    // Sidebar title
    { label: 'Sidebar title', expected: styles.sidebar_title, actual: theme.sidebar?.title?.['font-style'], source: 'sidebar.title.font-style' },
    // Admonition label
    { label: 'Admonition label', expected: styles.admonition_label, actual: theme.admonition?.label?.['font-style'], source: 'admonition.label.font-style' },
    // Title page elements
    { label: 'Title page title', expected: styles.title_page_title, actual: theme['title-page']?.title?.['font-style'], source: 'title-page.title.font-style' },
    { label: 'Title page subtitle', expected: styles.title_page_subtitle, actual: theme['title-page']?.subtitle?.['font-style'], source: 'title-page.subtitle.font-style' },
    { label: 'Title page authors', expected: styles.title_page_authors, actual: theme['title-page']?.authors?.['font-style'], source: 'title-page.authors.font-style' },
    { label: 'Title page revision', expected: styles.title_page_revision, actual: theme['title-page']?.revision?.['font-style'], source: 'title-page.revision.font-style' },
  ];

  for (const check of styleChecks) {
    if (!check.expected) continue;
    if (!check.actual) {
      // font-style not set — may inherit from parent or default
      findings.push({
        severity: 'info',
        layer: 'base',
        category: 'theme-styles',
        message: `${check.label} (${check.source}): not set — inherits from parent`,
      });
      continue;
    }
    if (check.actual === check.expected) {
      findings.push({
        severity: 'pass',
        layer: 'base',
        category: 'theme-styles',
        message: `${check.label} (${check.source}): ${check.actual}`,
      });
    } else {
      findings.push({
        severity: 'fail',
        layer: 'base',
        category: 'theme-styles',
        message: `${check.label} (${check.source}): expected "${check.expected}", got "${check.actual}"`,
        path: themeFile,
      });
    }
  }

  return findings;
}

// =============================================================================
// 8. Page Layout Validation
// =============================================================================

/**
 * Validate theme page layout measurements against the config.
 *
 * Checks page size, margins, header/footer height, vertical spacing,
 * thematic break margins, and orphan prevention settings.
 *
 * Guide §Page Layout
 */
export function validatePageLayout(themeFile: string, pageLayout: PageLayoutConfig): ValidationFinding[] {
  const findings: ValidationFinding[] = [];
  const theme = parseTheme(themeFile, findings);
  if (!theme) return findings;

  // --- Page size ---
  if (theme.page?.size) {
    if (theme.page.size === pageLayout.page_size) {
      findings.push({
        severity: 'pass', layer: 'base', category: 'page-layout',
        message: `Page size: ${theme.page.size}`,
      });
    } else {
      findings.push({
        severity: 'fail', layer: 'base', category: 'page-layout',
        message: `Page size: expected "${pageLayout.page_size}", got "${theme.page.size}"`,
        path: themeFile,
      });
    }
  }

  // --- Page layout (orientation) ---
  if (theme.page?.layout) {
    if (theme.page.layout === pageLayout.page_layout) {
      findings.push({
        severity: 'pass', layer: 'base', category: 'page-layout',
        message: `Page layout: ${theme.page.layout}`,
      });
    } else {
      findings.push({
        severity: 'fail', layer: 'base', category: 'page-layout',
        message: `Page layout: expected "${pageLayout.page_layout}", got "${theme.page.layout}"`,
        path: themeFile,
      });
    }
  }

  // --- Page margins [top, right, bottom, left] ---
  if (theme.page?.margin && Array.isArray(theme.page.margin)) {
    const m = theme.page.margin;
    const expectedMargins = [
      { label: 'Top margin', expected: pageLayout.margin_top, idx: 0 },
      { label: 'Right margin', expected: pageLayout.margin_right, idx: 1 },
      { label: 'Bottom margin', expected: pageLayout.margin_bottom, idx: 2 },
      { label: 'Left margin', expected: pageLayout.margin_left, idx: 3 },
    ];
    for (const check of expectedMargins) {
      const actual = String(m[check.idx] ?? '');
      if (actual === check.expected) {
        findings.push({
          severity: 'pass', layer: 'base', category: 'page-layout',
          message: `${check.label}: ${actual}`,
        });
      } else {
        findings.push({
          severity: 'fail', layer: 'base', category: 'page-layout',
          message: `${check.label}: expected "${check.expected}", got "${actual}"`,
          path: themeFile,
        });
      }
    }
  }

  // --- Header height ---
  if (theme.header) {
    const height = theme.header['height'];
    if (height != null) {
      if (String(height) === pageLayout.header_height) {
        findings.push({
          severity: 'pass', layer: 'base', category: 'page-layout',
          message: `Header height: ${height}`,
        });
      } else {
        findings.push({
          severity: 'fail', layer: 'base', category: 'page-layout',
          message: `Header height: expected "${pageLayout.header_height}", got "${height}"`,
          path: themeFile,
        });
      }
    }
  }

  // --- Footer height ---
  if (theme.footer) {
    const height = theme.footer['height'];
    if (height != null) {
      if (String(height) === pageLayout.footer_height) {
        findings.push({
          severity: 'pass', layer: 'base', category: 'page-layout',
          message: `Footer height: ${height}`,
        });
      } else {
        findings.push({
          severity: 'fail', layer: 'base', category: 'page-layout',
          message: `Footer height: expected "${pageLayout.footer_height}", got "${height}"`,
          path: themeFile,
        });
      }
    }
  }

  // --- Vertical spacing ---
  if (theme['vertical-spacing'] !== undefined) {
    if (theme['vertical-spacing'] === pageLayout.vertical_spacing) {
      findings.push({
        severity: 'pass', layer: 'base', category: 'page-layout',
        message: `Vertical spacing: ${theme['vertical-spacing']}pt`,
      });
    } else {
      findings.push({
        severity: 'fail', layer: 'base', category: 'page-layout',
        message: `Vertical spacing: expected ${pageLayout.vertical_spacing}pt, got ${theme['vertical-spacing']}pt`,
        path: themeFile,
      });
    }
  }

  // --- Thematic break margins ---
  const tb = theme['thematic-break'];
  if (tb) {
    if (tb['margin-top'] !== undefined) {
      if (tb['margin-top'] === pageLayout.thematic_break_margin_top) {
        findings.push({
          severity: 'pass', layer: 'base', category: 'page-layout',
          message: `Thematic break top margin: ${tb['margin-top']}pt`,
        });
      } else {
        findings.push({
          severity: 'fail', layer: 'base', category: 'page-layout',
          message: `Thematic break top margin: expected ${pageLayout.thematic_break_margin_top}pt, got ${tb['margin-top']}pt`,
          path: themeFile,
        });
      }
    }
    if (tb['margin-bottom'] !== undefined) {
      if (tb['margin-bottom'] === pageLayout.thematic_break_margin_bottom) {
        findings.push({
          severity: 'pass', layer: 'base', category: 'page-layout',
          message: `Thematic break bottom margin: ${tb['margin-bottom']}pt`,
        });
      } else {
        findings.push({
          severity: 'fail', layer: 'base', category: 'page-layout',
          message: `Thematic break bottom margin: expected ${pageLayout.thematic_break_margin_bottom}pt, got ${tb['margin-bottom']}pt`,
          path: themeFile,
        });
      }
    }
    // --- Thematic break border ---
    if (tb['border-width'] !== undefined) {
      if (tb['border-width'] === pageLayout.thematic_break_border_width) {
        findings.push({
          severity: 'pass', layer: 'base', category: 'page-layout',
          message: `Thematic break border width: ${tb['border-width']}`,
        });
      } else {
        findings.push({
          severity: 'fail', layer: 'base', category: 'page-layout',
          message: `Thematic break border width: expected ${pageLayout.thematic_break_border_width}, got ${tb['border-width']}`,
          path: themeFile,
        });
      }
    }
    if (tb['border-color']) {
      if (tb['border-color'].toLowerCase() === pageLayout.thematic_break_border_color.toLowerCase()) {
        findings.push({
          severity: 'pass', layer: 'base', category: 'page-layout',
          message: `Thematic break border color: ${tb['border-color']}`,
        });
      } else {
        findings.push({
          severity: 'fail', layer: 'base', category: 'page-layout',
          message: `Thematic break border color: expected "${pageLayout.thematic_break_border_color}", got "${tb['border-color']}"`,
          path: themeFile,
        });
      }
    }
  }

  // --- Heading orphan prevention ---
  if (theme.heading) {
    const minAfter = theme.heading['min-height-after'];
    if (minAfter != null) {
      if (String(minAfter) === pageLayout.heading_min_height_after) {
        findings.push({
          severity: 'pass', layer: 'base', category: 'page-layout',
          message: `Heading min-height-after: ${minAfter} (orphan prevention)`,
        });
      } else {
        findings.push({
          severity: 'fail', layer: 'base', category: 'page-layout',
          message: `Heading min-height-after: expected "${pageLayout.heading_min_height_after}", got "${minAfter}"`,
          path: themeFile,
        });
      }
    }
  }

  // --- Heading default spacing ---
  if (theme.heading) {
    const headingRec = theme.heading;
    const marginTop = headingRec['margin-top'];
    const marginBottom = headingRec['margin-bottom'];
    const lineHeight = headingRec['line-height'];

    if (typeof marginTop === 'number') {
      if (marginTop === pageLayout.heading_margin_top) {
        findings.push({
          severity: 'pass', layer: 'base', category: 'page-layout',
          message: `Heading margin-top: ${marginTop}pt`,
        });
      } else {
        findings.push({
          severity: 'fail', layer: 'base', category: 'page-layout',
          message: `Heading margin-top: expected ${pageLayout.heading_margin_top}pt, got ${marginTop}pt`,
          path: themeFile,
        });
      }
    }
    if (typeof marginBottom === 'number') {
      if (marginBottom === pageLayout.heading_margin_bottom) {
        findings.push({
          severity: 'pass', layer: 'base', category: 'page-layout',
          message: `Heading margin-bottom: ${marginBottom}pt`,
        });
      } else {
        findings.push({
          severity: 'fail', layer: 'base', category: 'page-layout',
          message: `Heading margin-bottom: expected ${pageLayout.heading_margin_bottom}pt, got ${marginBottom}pt`,
          path: themeFile,
        });
      }
    }
    if (typeof lineHeight === 'number') {
      if (lineHeight === pageLayout.heading_line_height) {
        findings.push({
          severity: 'pass', layer: 'base', category: 'page-layout',
          message: `Heading line-height: ${lineHeight}`,
        });
      } else {
        findings.push({
          severity: 'fail', layer: 'base', category: 'page-layout',
          message: `Heading line-height: expected ${pageLayout.heading_line_height}, got ${lineHeight}`,
          path: themeFile,
        });
      }
    }
  }

  return findings;
}

// =============================================================================
// 9. Tables, Lists & Figures Element Validation
// =============================================================================

/**
 * Validate theme table/list/caption/image element settings against the
 * canonical values in build.config.yaml §11 tables_lists_figures.
 *
 * Checks colors, styles, spacing, and alignment for structured data
 * presentation elements across all 5 themes.
 *
 * Guide §Tables, Lists, and Figures
 */
export function validateThemeElements(themeFile: string, typography: TypographyConfig): ValidationFinding[] {
  const findings: ValidationFinding[] = [];
  const config = typography.tables_lists_figures;
  if (!config) {
    findings.push({
      severity: 'info',
      layer: 'base',
      category: 'theme-elements',
      message: 'No tables_lists_figures in typography config — skipping element validation',
    });
    return findings;
  }

  const theme = parseTheme(themeFile, findings);
  if (!theme) return findings;

  // Build check list: [label, expected, actual, source path]
  const colorChecks: Array<{ label: string; expected: string; actual: string | undefined; source: string }> = [];
  const stringChecks: Array<{ label: string; expected: string; actual: string | undefined; source: string }> = [];
  const numberChecks: Array<{ label: string; expected: number; actual: number | undefined; source: string }> = [];

  // --- Table checks ---
  if (config.table) {
    // Table head
    colorChecks.push({
      label: 'Table head background',
      expected: config.table.head_background_color,
      actual: theme.table?.head?.['background-color'],
      source: 'table.head.background-color',
    });
    colorChecks.push({
      label: 'Table head font-color',
      expected: config.table.head_font_color,
      actual: theme.table?.head?.['font-color'],
      source: 'table.head.font-color',
    });
    stringChecks.push({
      label: 'Table head font-style',
      expected: config.table.head_font_style,
      actual: theme.table?.head?.['font-style'],
      source: 'table.head.font-style',
    });
    // Table body stripe
    colorChecks.push({
      label: 'Table body stripe',
      expected: config.table.body_stripe_color,
      actual: theme.table?.body?.['stripe-background-color'],
      source: 'table.body.stripe-background-color',
    });
    // Table border and grid
    colorChecks.push({
      label: 'Table border-color',
      expected: config.table.border_color,
      actual: theme.table?.['border-color'],
      source: 'table.border-color',
    });
    colorChecks.push({
      label: 'Table grid-color',
      expected: config.table.grid_color,
      actual: theme.table?.['grid-color'],
      source: 'table.grid-color',
    });
  }

  // --- List checks ---
  if (config.list) {
    colorChecks.push({
      label: 'List marker color',
      expected: config.list.marker_font_color,
      actual: theme.list?.['marker-font-color'],
      source: 'list.marker-font-color',
    });
    numberChecks.push({
      label: 'List item-spacing',
      expected: config.list.item_spacing,
      actual: theme.list?.['item-spacing'],
      source: 'list.item-spacing',
    });
  }

  // --- Description-list checks ---
  if (config.description_list) {
    stringChecks.push({
      label: 'Description-list term style',
      expected: config.description_list.term_font_style,
      actual: theme['description-list']?.['term-font-style'],
      source: 'description-list.term-font-style',
    });
    colorChecks.push({
      label: 'Description-list term color',
      expected: config.description_list.term_font_color,
      actual: theme['description-list']?.['term-font-color'],
      source: 'description-list.term-font-color',
    });
  }

  // --- Caption alignment ---
  if (config.caption) {
    stringChecks.push({
      label: 'Caption alignment',
      expected: config.caption.align,
      actual: theme.caption?.align,
      source: 'caption.align',
    });
  }

  // --- Image alignment ---
  if (config.image) {
    stringChecks.push({
      label: 'Image alignment',
      expected: config.image.align,
      actual: theme.image?.align,
      source: 'image.align',
    });
  }

  // Run color checks (case-insensitive hex comparison)
  for (const check of colorChecks) {
    if (!check.actual) {
      findings.push({
        severity: 'warn', layer: 'base', category: 'theme-elements',
        message: `${check.label} (${check.source}): not set in theme`,
        path: themeFile,
      });
      continue;
    }
    if (check.actual.toLowerCase() === check.expected.toLowerCase()) {
      findings.push({
        severity: 'pass', layer: 'base', category: 'theme-elements',
        message: `${check.label} (${check.source}): ${check.actual}`,
      });
    } else {
      findings.push({
        severity: 'fail', layer: 'base', category: 'theme-elements',
        message: `${check.label} (${check.source}): expected "${check.expected}", got "${check.actual}"`,
        path: themeFile,
      });
    }
  }

  // Run string checks (exact match)
  for (const check of stringChecks) {
    if (!check.actual) {
      findings.push({
        severity: 'warn', layer: 'base', category: 'theme-elements',
        message: `${check.label} (${check.source}): not set in theme`,
        path: themeFile,
      });
      continue;
    }
    if (check.actual === check.expected) {
      findings.push({
        severity: 'pass', layer: 'base', category: 'theme-elements',
        message: `${check.label} (${check.source}): ${check.actual}`,
      });
    } else {
      findings.push({
        severity: 'fail', layer: 'base', category: 'theme-elements',
        message: `${check.label} (${check.source}): expected "${check.expected}", got "${check.actual}"`,
        path: themeFile,
      });
    }
  }

  // Run number checks
  for (const check of numberChecks) {
    if (check.actual === undefined) {
      findings.push({
        severity: 'warn', layer: 'base', category: 'theme-elements',
        message: `${check.label} (${check.source}): not set in theme`,
        path: themeFile,
      });
      continue;
    }
    if (check.actual === check.expected) {
      findings.push({
        severity: 'pass', layer: 'base', category: 'theme-elements',
        message: `${check.label} (${check.source}): ${check.actual}`,
      });
    } else {
      findings.push({
        severity: 'fail', layer: 'base', category: 'theme-elements',
        message: `${check.label} (${check.source}): expected ${check.expected}, got ${check.actual}`,
        path: themeFile,
      });
    }
  }

  return findings;
}
