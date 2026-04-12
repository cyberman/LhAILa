#ifndef LHAILA_INTERNAL_LHA_TEST_H
#define LHAILA_INTERNAL_LHA_TEST_H

/*
 * LhAILa
 * Internal archive test contracts
 *
 * Private header.
 */

#ifndef LHAILA_INTERNAL_LHA_TYPES_H
#include "lha_types.h"
#endif

LONG lha_test_single_entry(
    struct LHAArchive *arc,
    struct LHAParsedEntry *entry,
    struct LHATestResult *outResult);

LONG lha_test_whole_archive(
    struct LHAArchive *arc,
    struct LHATestResult *outResult);

#endif /* LHAILA_INTERNAL_LHA_TEST_H */

