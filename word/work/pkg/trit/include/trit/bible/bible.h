// =============================================================================
// libtrit - BIBLE Layer Aggregator (Scripture Codex)
// Key: B-libtrit-bible
// =============================================================================
//
// The Bible is the codex - the specification that everything derives from.
// Genesis 1:1 is the halt condition. This layer encodes Scripture.
//
// This layer provides:
//   - rail.enc.h: Bible Rail encoding (spare states 243-255)
//   - verse.enc.h: Verse addressing (31,102 verses)
//   - path.txt.h: Scripture path resolution
//   - text.txt.h: Scripture text encoding (243 values)
//
// =============================================================================

#ifndef LIBTRIT_BIBLE_H
#define LIBTRIT_BIBLE_H

// BIBLE layer: Scripture encoding
#include "trit/bible/rail.enc.h"   // Bible Rail encoding
#include "trit/bible/verse.enc.h"  // Verse addressing
#include "trit/bible/path.txt.h"   // Scripture path resolution
#include "trit/bible/text.txt.h"   // Scripture text encoding

#endif // LIBTRIT_BIBLE_H
