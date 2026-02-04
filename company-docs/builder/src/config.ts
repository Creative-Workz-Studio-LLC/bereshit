/**
 * CWS Manual Builder - Configuration
 *
 * Build configuration for the Company Identity Manual.
 * Defines output formats, paths, and Asciidoctor options.
 */

import { fileURLToPath } from 'url';
import { dirname, resolve } from 'path';

// ES Module __dirname equivalent
const __filename = fileURLToPath(import.meta.url);
const __dirname = dirname(__filename);

/**
 * Supported output formats
 */
export type OutputFormat = 'html' | 'pdf' | 'epub' | 'docbook';

/**
 * Build configuration interface
 */
export interface BuildConfig {
  /** Source directory containing AsciiDoc files */
  sourceDir: string;

  /** Output directory for generated files */
  outputDir: string;

  /** Main book file to compile */
  masterDocument: string;

  /** Output filename (without extension) */
  outputName: string;

  /** Document attributes passed to Asciidoctor */
  attributes: Record<string, string>;

  /** Format-specific options */
  formatOptions: FormatOptions;
}

/**
 * Format-specific build options
 */
export interface FormatOptions {
  html: HtmlOptions;
  pdf: PdfOptions;
  epub: EpubOptions;
  docbook: DocbookOptions;
}

export interface HtmlOptions {
  /** Generate standalone HTML file */
  standalone: boolean;
  /** Include table of contents */
  toc: boolean;
  /** TOC position: left, right, auto */
  tocPosition: 'left' | 'right' | 'auto';
  /** Embed images as data URIs */
  embedImages: boolean;
  /** Custom stylesheet path */
  stylesheet?: string;
}

export interface PdfOptions {
  /** PDF theme file */
  theme?: string;
  /** Custom fonts directory */
  fontsDir?: string;
  /** Paper size */
  paperSize: 'A4' | 'Letter';
  /** Enable compression */
  compress: boolean;
}

export interface EpubOptions {
  /** EPUB version: 2 or 3 */
  version: 2 | 3;
  /** Cover image path */
  coverImage?: string;
  /** Custom CSS */
  stylesheet?: string;
}

export interface DocbookOptions {
  /** DocBook version */
  version: '5' | '4.5';
}

/**
 * Default configuration for the CWS Manual
 */
export const defaultConfig: BuildConfig = {
  sourceDir: resolve(__dirname, '../../'),
  outputDir: resolve(__dirname, '../../output'),
  masterDocument: 'book.adoc',
  outputName: 'CWS-Company-Identity-Manual',

  attributes: {
    // Company branding
    'company-name': 'Creative Workz Studio LLC',
    'company-short': 'CWS',
    'tagline': 'Where Creative Vision Meets Kingdom Purpose',

    // Document metadata
    'doctype': 'book',
    'icons': 'font',
    'icon-set': 'fa',
    'source-highlighter': 'rouge',
    'toc': 'auto',
    'toclevels': '3',
    'sectnums': '',
    'sectnumlevels': '3',
    'chapter-label': 'Chapter',
    'appendix-caption': 'Appendix',

    // Styling
    'experimental': '',
    'reproducible': '',
    'pdf-page-size': 'A4',
  },

  formatOptions: {
    html: {
      standalone: true,
      toc: true,
      tocPosition: 'left',
      embedImages: true,
    },
    pdf: {
      paperSize: 'A4',
      compress: true,
    },
    epub: {
      version: 3,
    },
    docbook: {
      version: '5',
    },
  },
};

/**
 * Load and merge custom configuration
 */
export function loadConfig(customConfig?: Partial<BuildConfig>): BuildConfig {
  if (!customConfig) {
    return defaultConfig;
  }

  return {
    ...defaultConfig,
    ...customConfig,
    attributes: {
      ...defaultConfig.attributes,
      ...customConfig.attributes,
    },
    formatOptions: {
      html: { ...defaultConfig.formatOptions.html, ...customConfig.formatOptions?.html },
      pdf: { ...defaultConfig.formatOptions.pdf, ...customConfig.formatOptions?.pdf },
      epub: { ...defaultConfig.formatOptions.epub, ...customConfig.formatOptions?.epub },
      docbook: { ...defaultConfig.formatOptions.docbook, ...customConfig.formatOptions?.docbook },
    },
  };
}
