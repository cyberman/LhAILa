#include "../internal/lha_types.h"
#include "../internal/lha_errors.h"
#include "../internal/lha_archive_core.h"

/*
 * Public-facing lifecycle entry points for the initial library path.
 *
 * 0.1 note:
 * We currently expose internal archive handles directly.
 * A stricter public/private split can be tightened later once the
 * outer header/API layer is finalized.
 */

LONG LHAOpenArchive(STRPTR path, struct LHAArchive **outArchive)
{
    if ((path == NULL) || (outArchive == NULL))
        return LHAERR_INVALID_ARGUMENT;

    return lha_core_open_archive(path, outArchive);
}

VOID LHACloseArchive(struct LHAArchive *archive)
{
    lha_core_close_archive(archive);
}

LONG LHARewind(struct LHAArchive *archive)
{
    if (archive == NULL)
        return LHAERR_INVALID_ARGUMENT;

    return lha_core_rewind_archive(archive);
}
