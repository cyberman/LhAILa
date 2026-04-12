#include <libraries/lhaarchive.h>

VOID lhaila_print_error(LONG rc);

LONG lhaila_cmd_extract(STRPTR archivePath, STRPTR destDir)
{
    struct LHAArchive *arc;
    struct LHAExtractOptions opts;
    LONG rc;

    if ((archivePath == NULL) || (destDir == NULL))
        return LHAERR_INVALID_ARGUMENT;

    arc = NULL;

    rc = LHAOpenArchive(archivePath, &arc);
    if (rc != LHAERR_OK)
    {
        lhaila_print_error(rc);
        return rc;
    }

    opts.leo_StructSize = sizeof(opts);
    opts.leo_Flags = 0UL;

    rc = LHAExtractArchive(arc, destDir, &opts);
    if (rc != LHAERR_OK)
        lhaila_print_error(rc);

    LHACloseArchive(arc);
    return rc;
}
