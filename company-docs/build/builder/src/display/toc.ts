/**
 * CWS Manual Builder — TOC & Page Display
 *
 * Formats TOC parse results, page status, fill results,
 * and check results for terminal output.
 */

import chalk from 'chalk';
import { sectionTypeFlag } from '../errors.js';
import type { PageParseResult, FilePageResult, TocPipelineResult } from '../validate/pages.js';

// =============================================================================
// TOC Parse Display
// =============================================================================

/**
 * Display TOC parse results with colored output.
 */
export function displayTocResults(result: PageParseResult): void {
  const isMultiFile = result.files.filter(f => f.totalPages > 0).length > 1;

  console.log();
  console.log(chalk.cyan('TOC & Page Structure'));
  console.log(chalk.cyan('===================='));

  // Collect files with § section data for display AFTER all page status
  const filesWithSections: typeof result.files = [];

  // --- Pass 1: Page status for all files (in book order) ---
  for (const fileResult of result.files) {
    const flag = sectionTypeFlag(fileResult.sectionType);

    // Show files without page data as placeholders (so sequence is visible)
    if (fileResult.pages.length === 0 && fileResult.sections.length === 0) {
      if (isMultiFile) {
        console.log(chalk.gray(`\n--- [${flag}] ${fileResult.filename} — (no page markers) ---`));
      }
      continue;
    }

    const bookRange = isMultiFile && fileResult.inBookStart > 0
      ? chalk.gray(`, book p.${fileResult.inBookStart}–${fileResult.inBookEnd}`)
      : '';
    console.log(chalk.gray(`\n--- [${flag}] ${fileResult.filename} (${fileResult.totalPages} pages${bookRange}) ---\n`));

    // Page status
    if (fileResult.pages.length > 0) {
      for (const page of fileResult.pages) {
        const lockIcon = page.contentFull
          ? chalk.yellow('LOCKED')
          : chalk.green('EDIT  ');
        const title = page.title
          ? chalk.gray(` — ${page.title}`)
          : '';
        const bookNum = isMultiFile && page.inBookNumber > 0
          ? chalk.gray(`  [book p.${page.inBookNumber}]`)
          : '';
        console.log(`    ${lockIcon}  p.${page.number}${title}${bookNum}`);
      }
      console.log();
      console.log(
        chalk.gray('  ') +
        `Total: ${fileResult.totalPages} pages` +
        `  Content-full: ${fileResult.contentFullCount}` +
        `  Editable: ${fileResult.editableCount}`
      );
    }

    // § Section entries — collect for later in multi-file, show inline for single-file
    if (fileResult.sections.length > 0) {
      if (isMultiFile) {
        // Compact count inline, full detail deferred
        console.log(
          chalk.gray('  ') +
          `Articles: ${fileResult.articles.length}` +
          `  Sections: ${fileResult.sections.length}` +
          `  Placeholders: ${fileResult.placeholderCount}`
        );
        filesWithSections.push(fileResult);
      } else {
        // Single-file: full detail inline
        displaySectionDetails(fileResult);
      }
    } else if (fileResult.placeholderCount > 0) {
      console.log(chalk.gray(`  Page placeholders (p. __): ${fileResult.placeholderCount}`));
    }
  }

  // --- Pass 2: § Navigation index (multi-file only, after all page status) ---
  if (filesWithSections.length > 0) {
    console.log(chalk.cyan('\n\nNavigation Index'));
    console.log(chalk.cyan('================'));
    console.log(chalk.gray('§ section entries parsed from TOC — these reference content pages, not frontmatter.\n'));

    for (const fileResult of filesWithSections) {
      const flag = sectionTypeFlag(fileResult.sectionType);
      console.log(chalk.gray(`--- [${flag}] ${fileResult.filename} ---`));
      displaySectionDetails(fileResult);
    }
  }

  // Validation issues
  if (result.issues.length > 0) {
    console.log(chalk.gray('\n--- Issues ---\n'));
    for (const issue of result.issues) {
      const prefix = issue.severity === 'error' ? chalk.red('  ERROR')
        : issue.severity === 'warn' ? chalk.yellow('  WARN ')
        : chalk.gray('  INFO ');
      const loc = issue.line > 0 ? `:${issue.line}` : '';
      console.log(`${prefix}  ${issue.file}${loc}: ${issue.message}`);
    }
  }

  // Summary
  const t = result.totals;
  console.log(chalk.gray('\n--- Summary ---\n'));
  console.log(
    `  Files: ${t.files}  ` +
    `Pages: ${t.pages}  ` +
    `Content-full: ${t.contentFull}  ` +
    `Editable: ${t.editable}`
  );
  if (t.sections > 0) {
    console.log(
      `  Articles: ${t.articles}  ` +
      `Sections: ${t.sections}  ` +
      `Placeholders: ${t.placeholders}`
    );
  }

  if (result.issues.length > 0) {
    const errors = result.issues.filter(i => i.severity === 'error').length;
    const warns = result.issues.filter(i => i.severity === 'warn').length;
    console.log(
      `  Issues: ${errors} errors, ${warns} warnings, ` +
      `${result.issues.length - errors - warns} info`
    );
  }

  console.log();
}

// =============================================================================
// Section Detail Display (helper)
// =============================================================================

/**
 * Display full § section detail for a single file.
 * Used by displayTocResults in both single-file and deferred multi-file modes.
 */
function displaySectionDetails(fileResult: FilePageResult): void {
  console.log();
  console.log(chalk.white('  Sections:'));

  let lastArticle = 0;
  for (const section of fileResult.sections) {
    if (section.article !== lastArticle) {
      lastArticle = section.article;
      const art = fileResult.articles.find(a => a.number === section.article);
      if (art) {
        console.log(chalk.yellow(`\n    Article ${art.number}: ${art.title}`));
        if (art.subtitle) {
          console.log(chalk.gray(`    ${art.subtitle}`));
        }
      }
    }

    const placeholder = section.hasPlaceholder
      ? chalk.gray(' [p. __]')
      : '';
    const desc = section.description
      ? chalk.gray(` — ${section.description}`)
      : '';
    console.log(`      ${section.ref} — ${section.title}${desc}${placeholder}`);
  }

  if (fileResult.placeholderCount > 0) {
    console.log();
    console.log(chalk.gray(`  Page placeholders (p. __): ${fileResult.placeholderCount}`));
  }
}

// =============================================================================
// TOC Fill Display
// =============================================================================

/**
 * Display TOC fill results with colored output.
 */
export function displayTocFillResult(result: TocPipelineResult, dryRun?: boolean): void {
  console.log();
  console.log(chalk.cyan('TOC Fill'));
  console.log(chalk.cyan('========'));

  if (dryRun) {
    console.log(chalk.gray('  (dry run — no changes written)\n'));
  }

  // Page map entries
  if (result.pageMap.size > 0) {
    console.log(chalk.white('  Computed page numbers:'));
    for (const [ref, page] of result.pageMap) {
      console.log(chalk.green(`    ${ref} → p. ${page}`));
    }
    console.log();
  }

  // Missing markers
  if (result.missingMarkers.length > 0) {
    console.log(chalk.yellow('  Content files missing page markers:'));
    for (const path of result.missingMarkers) {
      console.log(chalk.yellow(`    ${path}`));
    }
    console.log(chalk.gray('  (add // end p.N markers to these files)\n'));
  }

  // Issues
  for (const issue of result.issues) {
    const prefix = issue.severity === 'error' ? chalk.red('  ERROR')
      : issue.severity === 'warn' ? chalk.yellow('  WARN ')
      : chalk.gray('  INFO ');
    const loc = issue.line > 0 ? `:${issue.line}` : '';
    console.log(`${prefix}  ${issue.file}${loc}: ${issue.message}`);
  }

  // Summary
  console.log(chalk.gray('\n--- Summary ---\n'));
  console.log(
    `  Filled: ${result.matched}  ` +
    `Unresolved: ${result.unresolved}  ` +
    `Missing markers: ${result.missingMarkers.length}`
  );
  if (result.written) {
    console.log(chalk.green('  Changes written to B-TABLE-OF-CONTENTS.adoc'));
  } else if (result.matched > 0 && dryRun) {
    console.log(chalk.gray('  Run without --dry-run to write changes.'));
  }
  console.log();
}

// =============================================================================
// TOC Check Display
// =============================================================================

/**
 * Display TOC check results with colored output.
 */
export function displayTocCheckResult(result: TocPipelineResult): void {
  console.log();
  console.log(chalk.cyan('TOC Check'));
  console.log(chalk.cyan('========='));

  // Page map
  if (result.pageMap.size > 0) {
    console.log(chalk.white('\n  Computed page numbers:'));
    for (const [ref, page] of result.pageMap) {
      console.log(chalk.gray(`    ${ref} → p. ${page}`));
    }
  }

  // Issues (stale page numbers, verification failures)
  if (result.issues.length > 0) {
    console.log();
    for (const issue of result.issues) {
      const prefix = issue.severity === 'error' ? chalk.red('  ERROR')
        : issue.severity === 'warn' ? chalk.yellow('  WARN ')
        : chalk.gray('  INFO ');
      const loc = issue.line > 0 ? `:${issue.line}` : '';
      console.log(`${prefix}  ${issue.file}${loc}: ${issue.message}`);
    }
  }

  // Summary
  console.log(chalk.gray('\n--- Summary ---\n'));
  console.log(
    `  Verified: ${result.matched}  ` +
    `Stale: ${result.stale}  ` +
    `Unfilled: ${result.unresolved}  ` +
    `Missing markers: ${result.missingMarkers.length}`
  );

  if (result.stale > 0) {
    console.log(chalk.yellow(`\n  ${result.stale} page number(s) have shifted — run cws-build toc --fill to update.`));
  } else if (result.matched > 0 && result.stale === 0) {
    console.log(chalk.green('  All page numbers are current.'));
  }
  console.log();
}
