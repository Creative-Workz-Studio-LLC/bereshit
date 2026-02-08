/**
 * CWS Manual Builder — Legal Hierarchy Validation
 *
 * Validates legal hierarchy naming conventions (superset of manual):
 *   - Part directories match Part-{N}-{name}/ pattern
 *   - Article directories match Article-{NN}-{name}/ pattern
 *   - Section files match {LETTER}-{NAME}.adoc pattern
 *   - Letters are sequential within each article (A, B, C — no gaps)
 *   - No old number-prefix files remain
 *   - Per-article file inventory with section counts and letter ranges
 *
 * Maps to Makefile's `validate TYPE=manual-l` layer.
 *
 * Governance hierarchy: Part > Article > Section > Subsection > Clause > Subclause
 */

import type { SpineTree, SpinePart, SpineArticle } from '../../spine/types.js';
import type { ValidationFinding } from '../types.js';
import { validateManual } from './manual.js';

// =============================================================================
// Constants
// =============================================================================

const PART_PATTERN = /^Part-\d+-/;
const ARTICLE_PATTERN = /^Article-\d{2,}-/;
const SECTION_PATTERN = /^[A-Z]-/;
const OLD_NUMBER_PATTERN = /^\d+-\d+-/;
const ALPHABET = 'ABCDEFGHIJKLMNOPQRSTUVWXYZ';

// =============================================================================
// Legal Hierarchy Validation
// =============================================================================

/**
 * Validate legal hierarchy naming conventions.
 * Includes all manual checks plus naming validation.
 */
export function validateManualL(tree: SpineTree): ValidationFinding[] {
  // Start with manual checks
  const findings = validateManual(tree);
  const content = tree.scopes.B;

  // Part naming
  const badParts = content.parts
    .filter(p => !PART_PATTERN.test(p.dirname))
    .map(p => p.dirname);

  if (badParts.length === 0) {
    findings.push({
      severity: 'pass',
      layer: 'type',
      category: 'legal-hierarchy',
      message: 'Part naming: Part-{N}-{name}/ pattern',
    });
  } else {
    findings.push({
      severity: 'fail',
      layer: 'type',
      category: 'legal-hierarchy',
      message: 'Part naming: non-conforming directories',
      details: badParts,
    });
  }

  // Article naming
  const articleFindings = validateArticleNaming(content.parts);
  findings.push(...articleFindings);

  // Section naming, old files, and sequencing
  const sectionFindings = validateSectionNaming(content.parts);
  findings.push(...sectionFindings);

  // File inventory
  const inventoryFindings = buildFileInventory(content.parts);
  findings.push(...inventoryFindings);

  return findings;
}

// =============================================================================
// Article Naming
// =============================================================================

/**
 * Check Article-{NN}-{name}/ naming pattern.
 */
function validateArticleNaming(parts: SpinePart[]): ValidationFinding[] {
  const findings: ValidationFinding[] = [];
  let allOk = true;

  for (const part of parts) {
    const badArticles = part.articles
      .filter(a => !ARTICLE_PATTERN.test(a.dirname))
      .map(a => a.dirname);

    if (badArticles.length > 0) {
      findings.push({
        severity: 'fail',
        layer: 'type',
        category: 'legal-hierarchy',
        message: `Article naming in ${part.dirname}:`,
        details: badArticles,
      });
      allOk = false;
    }
  }

  if (allOk) {
    findings.push({
      severity: 'pass',
      layer: 'type',
      category: 'legal-hierarchy',
      message: 'Article naming: Article-{NN}-{name}/ pattern',
    });
  }

  return findings;
}

// =============================================================================
// Section Naming
// =============================================================================

/**
 * Check section file naming, sequencing, and old number-prefix files.
 */
function validateSectionNaming(parts: SpinePart[]): ValidationFinding[] {
  const findings: ValidationFinding[] = [];
  let secOk = true;
  let oldOk = true;
  let seqOk = true;

  for (const part of parts) {
    for (const article of part.articles) {
      if (article.sections.length === 0) continue;

      // Check for old number-prefix files
      const oldFiles = article.sections
        .filter(s => OLD_NUMBER_PATTERN.test(s.filename))
        .map(s => s.filename);

      if (oldFiles.length > 0) {
        findings.push({
          severity: 'fail',
          layer: 'type',
          category: 'legal-hierarchy',
          message: `${article.dirname}: old number-prefix files`,
          details: oldFiles,
        });
        oldOk = false;
      }

      // Check section naming pattern
      const badSections = article.sections
        .filter(s => !SECTION_PATTERN.test(s.filename))
        .map(s => s.filename);

      if (badSections.length > 0) {
        findings.push({
          severity: 'fail',
          layer: 'type',
          category: 'legal-hierarchy',
          message: `${article.dirname}: non-conforming section files`,
          details: badSections,
        });
        secOk = false;
      }

      // Check sequential letters
      const seqResult = checkSequentialLetters(article);
      if (seqResult) {
        findings.push({
          severity: 'fail',
          layer: 'type',
          category: 'legal-hierarchy',
          message: seqResult,
        });
        seqOk = false;
      }
    }
  }

  if (oldOk) {
    findings.push({
      severity: 'pass',
      layer: 'type',
      category: 'legal-hierarchy',
      message: 'No old number-prefix files',
    });
  }

  if (secOk) {
    findings.push({
      severity: 'pass',
      layer: 'type',
      category: 'legal-hierarchy',
      message: 'Section naming: {LETTER}-{NAME}.adoc pattern',
    });
  }

  if (seqOk) {
    findings.push({
      severity: 'pass',
      layer: 'type',
      category: 'legal-hierarchy',
      message: 'Sequential letters verified',
    });
  }

  return findings;
}

/**
 * Check that section letters are sequential (A, B, C — no gaps).
 * Returns error message if gap found, undefined if ok.
 */
function checkSequentialLetters(article: SpineArticle): string | undefined {
  const sortedSections = [...article.sections].sort((a, b) =>
    a.filename.localeCompare(b.filename)
  );

  for (let i = 0; i < sortedSections.length; i++) {
    const section = sortedSections[i];
    if (!section) continue;

    const actual = section.filename.charAt(0);
    const expected = ALPHABET.charAt(i);

    if (actual !== expected) {
      return `${article.dirname}: expected ${expected}-*, got ${section.filename}`;
    }
  }

  return undefined;
}

// =============================================================================
// File Inventory
// =============================================================================

/**
 * Build per-article file inventory findings.
 */
function buildFileInventory(parts: SpinePart[]): ValidationFinding[] {
  const findings: ValidationFinding[] = [];

  for (const part of parts) {
    const details: string[] = [];

    for (const article of part.articles) {
      const count = article.sections.length;

      if (count === 0) {
        details.push(`Art. ${article.id}: EMPTY`);
      } else {
        const sortedSections = [...article.sections].sort((a, b) =>
          a.filename.localeCompare(b.filename)
        );
        const lastSection = sortedSections[count - 1];
        const lastLetter = lastSection?.filename.charAt(0) || '?';
        details.push(`Art. ${article.id}: ${count} sections (A through ${lastLetter})`);
      }
    }

    findings.push({
      severity: 'info',
      layer: 'type',
      category: 'file-inventory',
      message: `Part ${part.id}:`,
      details,
    });
  }

  return findings;
}
