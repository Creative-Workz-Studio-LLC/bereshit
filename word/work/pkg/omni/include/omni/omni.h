// =============================================================================
// METADATA [METADATA]
// =============================================================================
//
// 4-Block Code Structure: Identity and context for this component
//
// =============================================================================

// # M.1 Core Identity [IDENTITY]
//
// Key: B-word-work-pkg-omni-omni
// Title: OmniCode Master Aggregator Header
// Type: Header
// Component: Core
// Role: Single entry point for all OmniCode subsystems

// # M.2 Lifecycle State [STATE]
//
// Status: Active
// Version: a-01.00
// Created: 2026-01-30
// Updated: 2026-01-30

// # M.3 Attribution [ATTRIBUTION]
//
// Authors: Seanje Lenox-Wise (Architect), Nova Dawn (Author)
// Organization: CreativeWorkzStudio LLC
// Copyright: (c) 2026 CreativeWorkzStudio LLC. All rights reserved.

// # M.4 Location [LOCATION]
//
// Path: word/work/pkg/omni/include/omni/omni.h

// # M.5 Derivation [DERIVATION]
//
// Derives_from: word/seed/code/c/header.h
// Pattern_from: word/work/pkg/trit/include/trit/trit.h

// # M.6 Classification [CLASSIFICATION]
//
// Tags: omnicode, aggregator, master, header

// # M.7 Intent [INTENT]
//
// Purpose: Single #include for all OmniCode functionality
// Core Design: Layer-ordered includes matching kernel/framework/app structure
// Philosophy: "In the beginning was the Word" — John 1:1
//
// Key Features:
//   - Single entry point: #include "omni/omni.h"
//   - Proper include order (foundation → runtime → tools)
//   - Layer mapping: lang=kernel, runtime=framework, ide=app

// # M.8 Grounding [GROUNDING]
//
// Scripture: "In the beginning was the Word, and the Word was with God,
//            and the Word was God." — John 1:1
// Principle: OmniCode is the Word made code — sentence-like syntax
// Anchor: "For God is not the author of confusion" — 1 Corinthians 14:33

// =============================================================================
// END METADATA
// =============================================================================

#ifndef BERESHIT_OMNI_H
#define BERESHIT_OMNI_H

// =============================================================================
// SETUP [SETUP]
// =============================================================================

// # S.1 Include Order [INCLUDE_ORDER]
//
// Layers are included in dependency order:
//
//   LAYER 1: CORE (foundation)
//     - lang/core — chunk system, patterns, DAR integration, lib
//
//   LAYER 2: CPISI (cognitive runtime)
//     - runtime/cpisi — mind, health, CPI-SI integration
//
//   LAYER 3: LANGUAGE PIPELINE (processing)
//     - lang/frontend — lexer, parser, semantic, keywords, hebrew
//     - lang/backend — IR, codegen
//
//   LAYER 4: EXECUTION (runtime)
//     - runtime/vm — virtual machine, runtime execution
//
//   LAYER 5: TOOLS (utilities)
//     - runtime/tools — debug, profile, retrieval
//
//   LAYER 6: BIBLE (codex)
//     - runtime/bible — Scripture encoding/decoding
//
//   LAYER 7: TRANSPILER (output)
//     - transpiler — C transpiler, menu transpiler
//
// IDE layer (ide/*) is NOT included here — that's application level.
// Include ide headers directly when building OmniCode IDE.

// ---------------------------------------------------------------------------
// LAYER 1: CORE — Foundation Types and Patterns
// ---------------------------------------------------------------------------

#include "lang/core/omni_lib.h"         // Core types and utilities
#include "lang/core/omni_chunks.h"      // Chunk system
#include "lang/core/omni_patterns.h"    // Pattern matching
#include "lang/core/omni_dar.h"         // DAR witness integration

// ---------------------------------------------------------------------------
// LAYER 2: CPISI — Cognitive Runtime
// ---------------------------------------------------------------------------

#include "runtime/cpisi/omni_cpisi.h"       // CPI-SI orchestration
#include "runtime/cpisi/omni_mind.h"        // Mind state management
#include "runtime/cpisi/omni_health_file.h" // Health file persistence

// ---------------------------------------------------------------------------
// LAYER 3: LANGUAGE PIPELINE — Frontend and Backend
// ---------------------------------------------------------------------------

// Frontend (source → AST)
#include "lang/frontend/omni_keywords.h"   // Keyword definitions
#include "lang/frontend/omni_hebrew.h"     // Hebrew root mappings
#include "lang/frontend/omni_lexer.h"      // Tokenization
#include "lang/frontend/omni_parser.h"     // Parsing
#include "lang/frontend/omni_semantic.h"   // Semantic analysis

// Backend (AST → output)
#include "lang/backend/omni_ir.h"          // Intermediate representation
#include "lang/backend/omni_codegen.h"     // Code generation

// ---------------------------------------------------------------------------
// LAYER 4: EXECUTION — Virtual Machine
// ---------------------------------------------------------------------------

#include "runtime/vm/omni_runtime.h"       // Runtime environment
#include "runtime/vm/omni_vm.h"            // Virtual machine

// ---------------------------------------------------------------------------
// LAYER 5: TOOLS — Development Utilities
// ---------------------------------------------------------------------------

#include "runtime/tools/omni_debug.h"      // Debug support
#include "runtime/tools/omni_profile.h"    // Profiling
#include "runtime/tools/omni_retrieval.h"  // Targeted retrieval

// ---------------------------------------------------------------------------
// LAYER 6: BIBLE — Scripture Codex
// ---------------------------------------------------------------------------

#include "runtime/bible/omni_codex.h"      // Bible encoding/decoding

// ---------------------------------------------------------------------------
// LAYER 7: TRANSPILER — Target Output
// ---------------------------------------------------------------------------

#include "transpiler/omni_c_transpiler.h"    // OmniCode → C
#include "transpiler/omni_menu_transpiler.h" // Menu DSL → C structs

// =============================================================================
// END SETUP
// =============================================================================

// =============================================================================
// BODY [BODY]
// =============================================================================

// # B.1 Layer Mapping [LAYERS]
//
// OmniCode maps to system layers:
//
//   ┌─────────────────────────────────────────────────────────────────┐
//   │  MILLENNIUMOS LAYER        │  OMNICODE COMPONENT                │
//   ├─────────────────────────────────────────────────────────────────┤
//   │  Stage 1: Kernel           │  lang/core, runtime/cpisi          │
//   │  Stage 3: Framework        │  lang/*, runtime/vm, transpiler/   │
//   │  Stage 4: Shell            │  ide/*  (NOT in this header)       │
//   └─────────────────────────────────────────────────────────────────┘

// # B.2 Version Information [VERSION]

#define OMNI_VERSION_MAJOR 0
#define OMNI_VERSION_MINOR 1
#define OMNI_VERSION_PATCH 0
#define OMNI_VERSION_STRING "0.1.0"

// =============================================================================
// END BODY
// =============================================================================

// =============================================================================
// CLOSING [CLOSING]
// =============================================================================

// # X.1 Validation [VALIDATION]
//
// Build:
//   gcc -fsyntax-only -I../include omni.h
//
// Test:
//   Include in any source: #include "omni/omni.h"

// # X.2 Usage [USAGE]
//
// Simple:
//   #include "omni/omni.h"
//   // All OmniCode functionality available
//
// Selective:
//   #include "lang/core/omni_chunks.h"  // Just chunks
//   #include "runtime/cpisi/omni_mind.h" // Just mind

// # X.8 Closing Note [NOTE]
//
// "In the beginning was the Word, and the Word was with God,
//  and the Word was God."
//   — John 1:1
//
// OmniCode is the Word made code.

// =============================================================================
// END CLOSING
// =============================================================================

#endif // BERESHIT_OMNI_H
