#include "../internal/lha_types.h"
#include "../internal/lha_errors.h"
#include "../internal/lha_io.h"
#include "../internal/lha_parse.h"

struct LHAHeaderProbe
{
    UBYTE bytes[32];
    ULONG size;
};

#define LHA_PROBE_MIN_LEVEL0_HEADER_SIZE 22UL
#define LHA_PROBE_MIN_LEVEL1_HEADER_SIZE 25UL

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

static LONG lha_read_probe(
    struct LHAArchive *arc,
    struct LHAHeaderProbe *probe,
    ULONG want_size)
{
    ULONG pos_before;
    ULONG available;
    LONG rc;

    if ((arc == NULL) || (probe == NULL))
        return LHAERR_INVALID_ARGUMENT;

    if (want_size > sizeof(probe->bytes))
        return LHAERR_INVALID_ARGUMENT;

    rc = lha_tell(arc, &pos_before);
    if (rc != LHAERR_OK)
        return rc;

    if (pos_before >= arc->lhaa_FileSize)
        return LHAERR_END_OF_ARCHIVE;

    available = arc->lhaa_FileSize - pos_before;
    if (available < want_size)
        return LHAERR_BAD_ARCHIVE;

    rc = lha_read_exact(arc, probe->bytes, want_size);
    if (rc != LHAERR_OK)
        return rc;

    probe->size = want_size;

    rc = lha_seek_abs(arc, pos_before);
    if (rc != LHAERR_OK)
        return rc;

    return LHAERR_OK;
}

static BOOL lha_probe_method_tag_plausible(const UBYTE *tag)
{
    if (tag == NULL)
        return FALSE;

    if ((tag[0] == '-') && (tag[1] == 'l') && (tag[2] == 'h') && (tag[4] == '-'))
        return TRUE;

    if ((tag[0] == '-') && (tag[1] == 'l') && (tag[2] == 'z') && (tag[4] == '-'))
        return TRUE;

    return FALSE;
}

/*
 * Minimal 0.1 header level detection scaffold.
 * Detection only.
 * Full structural validation remains the responsibility of the level parser.
 */
static LONG lha_detect_header_level(struct LHAArchive *arc, ULONG *outLevel)
{
    struct LHAHeaderProbe probe;
    UBYTE header_size;
    UBYTE level_byte;
    LONG rc;

    if ((arc == NULL) || (outLevel == NULL))
        return LHAERR_INVALID_ARGUMENT;

    rc = lha_read_probe(arc, &probe, 24UL);
    if (rc != LHAERR_OK)
        return rc;

    header_size = probe.bytes[0];

    if (header_size == 0U)
        return LHAERR_END_OF_ARCHIVE;

    if ((ULONG)header_size < LHA_PROBE_MIN_LEVEL0_HEADER_SIZE)
        return LHAERR_BAD_ARCHIVE;

    if (!lha_probe_method_tag_plausible(&probe.bytes[2]))
        return LHAERR_BAD_ARCHIVE;

    /*
     * For the current 0.1 parser stage we read the candidate level byte
     * from the classic base-header location used by our minimal level-0/1
     * skeletons.
     *
     * This is still only detection, not full validation.
     */
    level_byte = probe.bytes[23];

    switch ((ULONG)level_byte)
    {
        case 0UL:
            *outLevel = 0UL;
            return LHAERR_OK;

        case 1UL:
            if ((ULONG)header_size < LHA_PROBE_MIN_LEVEL1_HEADER_SIZE)
                return LHAERR_BAD_ARCHIVE;

            *outLevel = 1UL;
            return LHAERR_OK;

        case 2UL:
            if ((ULONG)header_size < LHA_PROBE_MIN_LEVEL1_HEADER_SIZE)
                return LHAERR_BAD_ARCHIVE;

            *outLevel = 2UL;
            return LHAERR_OK;

        default:
            return LHAERR_BAD_ARCHIVE;
    }
}

LONG lha_core_next_entry(struct LHAArchive *arc, struct LHAParsedEntry *outEntry)
{
    ULONG header_level;
    ULONG entry_offset;
    LONG rc;

    if ((arc == NULL) || (outEntry == NULL))
        return LHAERR_INVALID_ARGUMENT;

    if (arc->lhaa_File == (BPTR)0)
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

        case 2UL:
            rc = LHAERR_KNOWN_OUTSIDE;
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
