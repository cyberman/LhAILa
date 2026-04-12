#ifndef LHAILA_INTERNAL_LHA_EXTRACT_H
#define LHAILA_INTERNAL_LHA_EXTRACT_H

/*
 * LhAILa
 * Internal extract contracts
 *
 * Private header.
 */

#ifndef LHAILA_INTERNAL_LHA_TYPES_H
#include "lha_types.h"
#endif

struct LHAExtractOptions
{
    ULONG leo_StructSize;
    ULONG leo_Flags;
};

#define LHAEXTRACTF_OVERWRITE   (1UL << 0)
#define LHAEXTRACTF_SKIP_EXIST  (1UL << 1)
#define LHAEXTRACTF_QUIET       (1UL << 2)

LONG lha_extract_entry_to_dest(
    struct LHAArchive *arc,
    struct LHAParsedEntry *entry,
    STRPTR destDir,
    struct LHAExtractOptions *opts);

LONG lha_extract_archive_to_dest(
    struct LHAArchive *arc,
    STRPTR destDir,
    struct LHAExtractOptions *opts);

#endif /* LHAILA_INTERNAL_LHA_EXTRACT_H */

