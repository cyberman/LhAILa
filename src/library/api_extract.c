#include "../internal/lha_types.h"
#include "../internal/lha_errors.h"
#include "../internal/lha_extract.h"

LONG LHAExtractArchive(
    struct LHAArchive *archive,
    STRPTR destDir,
    struct LHAExtractOptions *opts)
{
    if ((archive == NULL) || (destDir == NULL))
        return LHAERR_INVALID_ARGUMENT;

    return lha_extract_archive_to_dest(archive, destDir, opts);
}

LONG LHAExtractEntry(
    struct LHAEntry *entry,
    STRPTR destDir,
    struct LHAExtractOptions *opts)
{
    (void)entry;
    (void)destDir;
    (void)opts;

    /*
     * Same honest limitation as LHATestEntry():
     * no stable public entry wrapper yet.
     */
    return LHAERR_INTERNAL;
}
