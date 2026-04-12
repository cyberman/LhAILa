#include "../internal/lha_types.h"
#include "../internal/lha_errors.h"
#include "../internal/lha_test.h"

/*
 * Public-facing test entry points for the initial library path.
 */

LONG LHATestArchive(struct LHAArchive *archive, struct LHATestResult *outResult)
{
    if ((archive == NULL) || (outResult == NULL))
        return LHAERR_INVALID_ARGUMENT;

    return lha_test_whole_archive(archive, outResult);
}

LONG LHATestEntry(struct LHAEntry *entry, struct LHATestResult *outResult)
{
    struct LHAParsedEntry *parsed;

    if ((entry == NULL) || (outResult == NULL))
        return LHAERR_INVALID_ARGUMENT;

    /*
     * 0.1 bridge:
     * LHAEntry is not yet a distinct private/public wrapper type.
     * We temporarily treat it as an internal parsed entry view.
     */
    parsed = (struct LHAParsedEntry *)entry;

    /*
     * 0.1 limitation:
     * single-entry test currently requires access through the owning archive
     * path, which is not yet represented by a separate entry wrapper.
     *
     * Until the entry view wrapper is introduced, expose this honestly.
     */
    (void)parsed;

    return LHAERR_INTERNAL;
}
