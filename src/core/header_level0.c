#include <exec/types.h>
#include <proto/dos.h>

#include "../internal/lha_types.h"
#include "../internal/lha_errors.h"
#include "../internal/lha_profile.h"
#include "../internal/lha_io.h"

#define LHA_LV0_METHOD_LEN 5UL
#define LHA_LV0_BASE_FIXED_SIZE 22UL

static BOOL lha_method_tag_shape_valid(const UBYTE tag[LHA_LV0_METHOD_LEN])
{
    if (tag == NULL)
        return FALSE;

    if ((tag[0] == '-') && (tag[1] == 'l') && (tag[2] == 'h') && (tag[4] == '-'))
        return TRUE;

    if ((tag[0] == '-') && (tag[1] == 'l') && (tag[2] == 'z') && (tag[4] == '-'))
        return TRUE;

    return FALSE;
}

static UWORD lha_method_from_tag(const UBYTE tag[LHA_LV0_METHOD_LEN])
{
    if ((tag[0] == '-') && (tag[1] == 'l') && (tag[2] == 'h') && (tag[4] == '-'))
    {
        switch (tag[3])
        {
            case '0': return LHA_METH_LH0;
            case '1': return LHA_METH_LH1;
            case '2': return LHA_METH_LH2;
            case '3': return LHA_METH_LH3;
            case '4': return LHA_METH_LH4;
            case '5': return LHA_METH_LH5;
            case '6': return LHA_METH_LH6;
            case '7': return LHA_METH_LH7;
            default:  return LHA_METH_UNKNOWN;
        }
    }

    if ((tag[0] == '-') && (tag[1] == 'l') && (tag[2] == 'z') && (tag[4] == '-'))
    {
        switch (tag[3])
        {
            case 's': return LHA_METH_LZS;
            case '4': return LHA_METH_LZ4;
            case '5': return LHA_METH_LZ5;
            default:  return LHA_METH_UNKNOWN;
        }
    }

    return LHA_METH_UNKNOWN;
}

static ULONG lha_u32le(const UBYTE *p)
{
    return ((ULONG)p[0])
         | ((ULONG)p[1] << 8)
         | ((ULONG)p[2] << 16)
         | ((ULONG)p[3] << 24);
}

static UWORD lha_u16le(const UBYTE *p)
{
    return (UWORD)(((UWORD)p[0]) | ((UWORD)p[1] << 8));
}

static LONG lha_fill_level0_timestamp(struct LHAParsedEntry *outEntry, ULONG raw_timestamp)
{
    /*
     * 0.1 skeleton:
     * we keep raw DOS-style timestamp handling deferred.
     *
     * For now:
     * - store zeroed Amiga time fields
     * - parser remains structurally correct
     *
     * TODO(0.2): convert archive timestamp to Amiga days/mins/ticks.
     */
    (void)raw_timestamp;

    if (outEntry == NULL)
        return LHAERR_INVALID_ARGUMENT;

    outEntry->lpe_TimestampDays = 0UL;
    outEntry->lpe_TimestampMins = 0UL;
    outEntry->lpe_TimestampTicks = 0UL;

    return LHAERR_OK;
}

static LONG lha_parse_level0_name(
    struct LHAArchive *arc,
    UBYTE name_len,
    struct LHAParsedEntry *outEntry)
{
    ULONG need_size;
    LONG rc;

    if ((arc == NULL) || (outEntry == NULL))
        return LHAERR_INVALID_ARGUMENT;

    /*
     * 0.1 conservative policy:
     * reject empty entry names.
     */
    if (name_len == 0U)
        return LHAERR_BAD_ARCHIVE;

    need_size = (ULONG)name_len + 1UL;

    if ((arc->lhaa_PathScratch == NULL) || (arc->lhaa_PathScratchSize < need_size))
        return LHAERR_NO_MEMORY;

    rc = lha_read_exact(arc, arc->lhaa_PathScratch, (ULONG)name_len);
    if (rc != LHAERR_OK)
        return rc;

    arc->lhaa_PathScratch[name_len] = '\0';

    outEntry->lpe_Path = arc->lhaa_PathScratch;
    outEntry->lpe_Flags |= LHA_ENTRY_FLAG_PATH_PRESENT;

    return LHAERR_OK;
}

LONG lha_parse_level0(struct LHAArchive *arc, struct LHAParsedEntry *outEntry)
{
    UBYTE header_size;
    UBYTE header_cksum;
    UBYTE method_tag[LHA_LV0_METHOD_LEN];
    UBYTE u32buf[4];
    UBYTE u16buf[2];
    UBYTE name_len;
    ULONG entry_start;
    ULONG raw_timestamp;
    ULONG computed_min_size;
    ULONG header_end_pos;
    ULONG consumed_after_size;
    ULONG remaining_after_name_len;
    ULONG needed_after_name_len;
    LONG rc;

    if ((arc == NULL) || (outEntry == NULL))
        return LHAERR_INVALID_ARGUMENT;

    rc = lha_tell(arc, &entry_start);
    if (rc != LHAERR_OK)
        return rc;

    rc = lha_read_exact(arc, &header_size, 1UL);
    if (rc != LHAERR_OK)
        return rc;

    {
        LONG args[1];
        args[0] = (LONG)header_size;
        VPrintf("lv0: hsize=%ld\n", args);
    }

    /*
     * End marker candidate: zero header size.
     * Caller currently treats this as archive end via detection path,
     * but we also guard here.
     */
    if (header_size == 0U)
        return LHAERR_END_OF_ARCHIVE;

    if ((ULONG)header_size < LHA_LV0_BASE_FIXED_SIZE)
        return LHAERR_BAD_ARCHIVE;

    rc = lha_read_exact(arc, &header_cksum, 1UL);
    if (rc != LHAERR_OK)
        return rc;

    (void)header_cksum; /* checksum validation is deferred in 0.1 */

    rc = lha_read_exact(arc, method_tag, LHA_LV0_METHOD_LEN);
    if (rc != LHAERR_OK)
        return rc;

    {
        LONG args[5];
        args[0] = (LONG)method_tag[0];
        args[1] = (LONG)method_tag[1];
        args[2] = (LONG)method_tag[2];
        args[3] = (LONG)method_tag[3];
        args[4] = (LONG)method_tag[4];
        VPrintf("lv0: method=%lc%lc%lc%lc%lc\n", args);
    }

    if (!lha_method_tag_shape_valid(method_tag))
        return LHAERR_BAD_ARCHIVE;

    outEntry->lpe_MethodID = lha_method_from_tag(method_tag);
    if (outEntry->lpe_MethodID != LHA_METH_UNKNOWN)
        outEntry->lpe_Flags |= LHA_ENTRY_FLAG_METHOD_KNOWN;

    rc = lha_read_exact(arc, u32buf, 4UL);
    if (rc != LHAERR_OK)
        return rc;
    outEntry->lpe_PackedSize = lha_u32le(u32buf);

    rc = lha_read_exact(arc, u32buf, 4UL);
    if (rc != LHAERR_OK)
        return rc;
    outEntry->lpe_UnpackedSize = lha_u32le(u32buf);

    rc = lha_read_exact(arc, u32buf, 4UL);
    if (rc != LHAERR_OK)
        return rc;
    raw_timestamp = lha_u32le(u32buf);

    rc = lha_fill_level0_timestamp(outEntry, raw_timestamp);
    if (rc != LHAERR_OK)
        return rc;

    {
        UBYTE attr;

        rc = lha_read_exact(arc, &attr, 1UL);
        if (rc != LHAERR_OK)
            return rc;

        /*
         * 0.1 minimal policy:
         * - do not interpret attr yet
         * - treat entry as file by default
         */
        (void)attr;
        outEntry->lpe_Type = LHA_ENTRY_TYPE_FILE;
    }

    rc = lha_read_exact(arc, &name_len, 1UL);
    if (rc != LHAERR_OK)
        return rc;

    {
        LONG args[1];
        args[0] = (LONG)name_len;
        VPrintf("lv0: namelen=%ld\n", args);
    }

    consumed_after_size = 1UL + 5UL + 4UL + 4UL + 4UL + 1UL + 1UL;
    /* checksum + method + packed + unpacked + timestamp + attr + name_len */

    if (consumed_after_size > (ULONG)header_size)
        return LHAERR_BAD_ARCHIVE;

    remaining_after_name_len = (ULONG)header_size - consumed_after_size;
    needed_after_name_len = (ULONG)name_len + 2UL + 1UL;
    /* name + crc + level */

    if (needed_after_name_len > remaining_after_name_len)
        return LHAERR_BAD_ARCHIVE;

    rc = lha_parse_level0_name(arc, name_len, outEntry);
    if (rc != LHAERR_OK)
        return rc;

    rc = lha_read_exact(arc, u16buf, 2UL);
    if (rc != LHAERR_OK)
        return rc;
    outEntry->lpe_CRC = lha_u16le(u16buf);
    outEntry->lpe_Flags |= LHA_ENTRY_FLAG_HAS_CRC;

    /*
     * We only support level 0 in this parser.
     * Many format descriptions store the header level byte as the last
     * byte of the base header region.
     */
    {
        UBYTE level;

        rc = lha_read_exact(arc, &level, 1UL);
        if (rc != LHAERR_OK)
            return rc;

        {
            LONG args[1];
            args[0] = (LONG)level;
            VPrintf("lv0: level=%ld\n", args);
        }

        if ((ULONG)level != LHA_HEADER_LEVEL_0)
            return LHAERR_BAD_ARCHIVE;

        outEntry->lpe_HeaderLevel = LHA_HEADER_LEVEL_0;
    }

    /*
     * Minimal structural plausibility:
     * header_size is the size of the header payload following the size byte.
     *
     * Base content read after header_size:
     *   1  checksum
     *   5  method
     *   4  packed
     *   4  unpacked
     *   4  timestamp
     *   1  attr
     *   1  name_len
     *   N  name
     *   2  crc
     *   1  level
     *
     * => 23 + N bytes after header_size
     */
    computed_min_size = LHA_LV0_BASE_FIXED_SIZE + (ULONG)name_len;
    if ((ULONG)header_size < computed_min_size)
        return LHAERR_BAD_ARCHIVE;

    header_end_pos = entry_start + 1UL + (ULONG)header_size;

    if (header_end_pos <= entry_start)
        return LHAERR_BAD_ARCHIVE;

    if (header_end_pos > arc->lhaa_FileSize)
        return LHAERR_BAD_ARCHIVE;

    outEntry->lpe_HeaderOffset = entry_start;
    outEntry->lpe_DataOffset = header_end_pos;

    if (outEntry->lpe_DataOffset > arc->lhaa_FileSize)
        return LHAERR_BAD_ARCHIVE;

    if (outEntry->lpe_PackedSize > (arc->lhaa_FileSize - outEntry->lpe_DataOffset))
        return LHAERR_BAD_ARCHIVE;

    /*
     * Skip any remaining header bytes not consumed by the minimal parser.
     */
    {
        ULONG cur_pos;
        ULONG remaining;

        rc = lha_tell(arc, &cur_pos);
        if (rc != LHAERR_OK)
            return rc;

        if (cur_pos > header_end_pos)
            return LHAERR_BAD_ARCHIVE;

        remaining = header_end_pos - cur_pos;
        if (remaining != 0UL)
        {
            rc = lha_skip_exact(arc, remaining);
            if (rc != LHAERR_OK)
                return rc;
        }
    }

    return LHAERR_OK;
}
