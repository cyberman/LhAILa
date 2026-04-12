#include "../../include/libraries/lhaarchive.h"

VOID lhaila_print_error(LONG rc);
VOID lhaila_print_entry_info(struct LHAEntryInfo *info);

LONG lhaila_cmd_list(STRPTR archivePath)
{
    struct LHAArchive *arc;
    struct LHAEntry *entry;
    struct LHAEntryInfo info;
    LONG rc;

    if (archivePath == NULL)
        return LHAERR_INVALID_ARGUMENT;

    arc = NULL;
    entry = NULL;

    rc = LHAOpenArchive(archivePath, &arc);
    if (rc != LHAERR_OK)
    {
        lhaila_print_error(rc);
        return rc;
    }

    rc = LHARewind(arc);
    if (rc != LHAERR_OK)
    {
        lhaila_print_error(rc);
        LHACloseArchive(arc);
        return rc;
    }

    for (;;)
    {
        rc = LHANextEntry(arc, &entry);
        if (rc == LHAERR_END_OF_ARCHIVE)
        {
            rc = LHAERR_OK;
            break;
        }

        if (rc != LHAERR_OK)
        {
            lhaila_print_error(rc);
            break;
        }

        info.lei_StructSize = sizeof(info);

        rc = LHAGetEntryInfo(entry, &info);
        if (rc != LHAERR_OK)
        {
            lhaila_print_error(rc);
            break;
        }

        lhaila_print_entry_info(&info);
    }

    LHACloseArchive(arc);
    return rc;
}
