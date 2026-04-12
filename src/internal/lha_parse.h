#ifndef LHAILA_INTERNAL_LHA_PARSE_H
#define LHAILA_INTERNAL_LHA_PARSE_H

/*
 * LhAILa
 * Internal parser contracts
 *
 * Private header.
 */

#ifndef EXEC_TYPES_H
#include <exec/types.h>
#endif

#ifndef LHAILA_INTERNAL_LHA_TYPES_H
#include "lha_types.h"
#endif

LONG lha_parse_level0(struct LHAArchive *arc, struct LHAParsedEntry *outEntry);
LONG lha_parse_level1(struct LHAArchive *arc, struct LHAParsedEntry *outEntry);

LONG lha_core_next_entry(struct LHAArchive *arc, struct LHAParsedEntry *outEntry);

#endif /* LHAILA_INTERNAL_LHA_PARSE_H */

