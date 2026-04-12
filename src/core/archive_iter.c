#include "../internal/lha_types.h"
#include "../internal/lha_errors.h"
#include "../internal/lha_io.h"
#include "../internal/lha_parse.h"

static VOID lha_clear_parsed_entry(struct LHAParsedEntry *entry)
{
    if (entry == NULL)
        return;

    entry->lpe_HeaderLevel = 0UL;
    entry->lpe_MethodID = 0U;
    entry->lpe_Type = 0UL;

    entry->lpe_HeaderOffset = 0UL;
    entry->lpe_DataOffset = 0UL;

    entry->lpe_PackedSize = 0UL;
    entry->lpe_UnpackedSize = 0UL;

    entry->lpe_CRC = 0U;

    entry->lpe_TimestampDays = 0UL;
    entry->lpe_TimestampMins = 0UL;
    entry->lpe_TimestampTicks = 0UL;

    entry->lpe_Path = NULL;
    entry->lpe_Flags = 0UL;
}

/*
 * Minimal 0.1 header level detection scaffold.
 */
static LONG lha_detect_header_level(struct LHAArchive *arc, ULONG *outLevel)
{
    UBYTE probe;
    ULONG pos_before;
    LONG rc;

    if ((arc == NULL) || (outLevel == NULL))
        return LHAERR_INVALID_ARGUMENT;

    rc = lha_tell(arc, &pos_before);
    if (rc != LHAERR_OK)
        return rc;

    rc = lha_read_exact(arc, &probe, 1UL);
    if (rc != LHAERR_OK)
        return rc;

    rc = lha_seek_abs(arc, pos_before);
    if (rc != LHAERR_OK)
        return rc;

    if (probe == 0U)
    {
        *outLevel = 0xFFFFFFFFUL;
        return LHAERR_END_OF_ARCHIVE;
    }

    *outLevel = 0UL;
    return LHAERR_OK;
}

LONG lha_core_next_entry(struct LHAArchive *arc, struct LHAParsedEntry *outEntry)
{
    ULONG header_level;
    ULONG entry_offset;
    LONG rc;

    if ((arc == NULL) || (outEntry == NULL))
        return LHAERR_INVALID_ARGUMENT;

    if (arc->lhaa_File == ZERO)
        return LHAERR_INVALID_ARGUMENT;

    lha_clear_parsed_entry(outEntry);

    rc = lha_tell(arc, &entry_offset);
    if (rc != LHAERR_OK)
        return rc;

    rc = lha_detect_header_level(arc, &header_level);
    if (rc == LHAERR_END_OF_ARCHIVE)
    {
        arc->lhaa_HaveCurrentEntry = FALSE;
        arc->lhaa_LastError = LHAERR_END_OF_ARCHIVE;
        return LHAERR_END_OF_ARCHIVE;
    }
    if (rc != LHAERR_OK)
    {
        arc->lhaa_HaveCurrentEntry = FALSE;
        arc->lhaa_LastError = rc;
        return rc;
    }

    switch (header_level)
    {
        case 0UL:
            rc = lha_parse_level0(arc, outEntry);
            break;

        case 1UL:
            rc = lha_parse_level1(arc, outEntry);
            break;

        default:
            rc = LHAERR_BAD_ARCHIVE;
            break;
    }

    if (rc != LHAERR_OK)
    {
        arc->lhaa_HaveCurrentEntry = FALSE;
        arc->lhaa_LastError = rc;
        return rc;
    }

    outEntry->lpe_HeaderOffset = entry_offset;

    arc->lhaa_CurrentEntry = *outEntry;
    arc->lhaa_HaveCurrentEntry = TRUE;
    arc->lhaa_LastError = LHAERR_OK;

    return LHAERR_OK;
}
