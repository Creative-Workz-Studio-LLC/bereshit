/**
 * CWS Manual Builder — Witness Module
 *
 * Unified structured observation of system state.
 * The system bears witness to its own health — observation layer
 * of the CPI-SI feedback loop.
 *
 * "Surely the Lord GOD will do nothing, but he revealeth
 *  his secret unto his servants the prophets." — Amos 3:7
 */

// Types and schema
export {
  WITNESS_SCHEMA_VERSION,
  ALL_DOMAINS,
} from './types.js';

export type {
  SystemHealth,
  DomainHealth,
  ActiveHealth,
  DomainName,
  WitnessMetadata,
  DomainSummary,
  DomainResult,
  StructuralDomainResult,
  EditorialDomainResult,
  ThemeDomainResult,
  ThemeFileResult,
  BibleDomainResult,
  TocDomainResult,
  SkippedDomainResult,
  Recommendation,
  WitnessReport,
} from './types.js';

// Collector
export { WitnessCollector } from './collector.js';

// Recommendations
export { generateRecommendations } from './recommendations.js';

// Serialization
export { serializeWitnessReport, writeWitnessReport } from './serializer.js';

// Runner
export { runWitness } from './runner.js';
export type { WitnessOptions } from './runner.js';
