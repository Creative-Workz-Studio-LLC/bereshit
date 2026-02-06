/**
 * CWS Manual Builder — Spine Module
 *
 * Re-exports all spine discovery components.
 */

// Types
export type {
  SpineScope,
  SpineTree,
  SpineScopeNode,
  SpinePart,
  SpineArticle,
  SpineSection,
  InventoryReport,
  InventorySummary,
  PartInventory,
} from './types.js';

// Constants
export { SCOPE_NAMES, SCOPE_DIRS } from './types.js';

// Scanner
export {
  scanSpine,
  getPartIds,
  getPart,
  getScopeDir,
  scopeFromDir,
  resolveBookDir,
} from './scanner.js';

// Inventory
export {
  buildInventory,
  formatInventoryTree,
} from './inventory.js';
