/// @file dar.h
/// @omni code --c --header
///
/// ═══════════════════════════════════════════════════════════════════════════════
/// LAYER 1: DAR — Detect-Assess-Restore Aggregator
/// ═══════════════════════════════════════════════════════════════════════════════
///
/// Single include for all DAR components:
///   #include "kernel/dar/dar.h"
///
/// Provides:
///   - Manager:  dar.mgr.h (orchestrator)
///   - Phases:   detect.phase.h, assess.phase.h, restore.phase.h
///   - Utility:  rotation.util.h
///
/// @omni:req  key        = CORNERSTONE-DAR-AGGREGATOR
/// @omni:req  at         = a-01.00
/// @omni:inh  type       = header
/// @omni:inh  subtype    = aggregator
/// @omni:ins  layer      = 1
///
/// ═══════════════════════════════════════════════════════════════════════════════

#ifndef CORNERSTONE_DAR_H
#define CORNERSTONE_DAR_H

/* ═══════════════════════════════════════════════════════════════════════════════
 * DAR COMPONENTS
 * ═══════════════════════════════════════════════════════════════════════════════ */

/* Manager (orchestrator) */
#include "witness/manager/dar.mgr.h"

/* DAR Phases */
#include "witness/phase/detect.phase.h"
#include "witness/phase/assess.phase.h"
#include "witness/phase/restore.phase.h"

/* Utilities */
#include "witness/utility/rotation.util.h"

#endif /* CORNERSTONE_DAR_H */
