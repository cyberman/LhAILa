#ifndef LHAILA_INTERNAL_LHA_ORCHESTRATION_H
#define LHAILA_INTERNAL_LHA_ORCHESTRATION_H

/*
 * LhAILa
 * Internal orchestration contracts
 *
 * Private header.
 */

#ifndef LHAILA_INTERNAL_LHA_TYPES_H
#include "lha_types.h"
#endif

LONG lha_process_entry(
    struct LHAArchive *arc,
    struct LHAParsedEntry *entry,
    struct LHASink *sink);

#endif /* LHAILA_INTERNAL_LHA_ORCHESTRATION_H */

