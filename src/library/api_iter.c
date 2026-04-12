#include "../internal/lha_types.h"
#include "../internal/lha_errors.h"
#include "../internal/lha_parse.h"

LONG LHANextEntry(struct LHAArchive *archive, struct LHAEntry **outEntry)
{
    LONG rc;

    if ((archive == NULL) || (outEntry == NULL))
        return LHAERR_INVALID_ARGUMENT;

    *outEntry = NULL;

    rc = lha_core_next_entry(archive, &archive->lhaa_CurrentEntry);
    if (rc != LHAERR_OK)
        return rc;

    archive->lhaa_HaveCurrentEntry = TRUE;
    *outEntry = (struct LHAEntry *)&archive->lhaa_CurrentEntry;

    return LHAERR_OK;
}
