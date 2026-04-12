#ifndef LHAILA_INTERNAL_LHA_SINKS_H
#define LHAILA_INTERNAL_LHA_SINKS_H

/*
 * LhAILa
 * Internal sink contracts
 *
 * Private header.
 */

#ifndef LHAILA_INTERNAL_LHA_TYPES_H
#include "lha_types.h"
#endif

/*
 * Test sink helpers
 */
LONG lha_test_sink_begin(struct LHASink *sink, struct LHAParsedEntry *entry);
LONG lha_test_sink_write(struct LHASink *sink, CONST_APTR data, ULONG size);
LONG lha_test_sink_end(struct LHASink *sink);
VOID lha_test_sink_abort(struct LHASink *sink);

#endif /* LHAILA_INTERNAL_LHA_SINKS_H */

