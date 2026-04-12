#include <exec/types.h>

#include "../internal/lha_types.h"
#include "../internal/lha_errors.h"
#include "../internal/lha_profile.h"
#include "../internal/lha_io.h"

#define LHA_LV1_METHOD_LEN 5UL

static UWORD lha_method_from_tag_lv1(const UBYTE tag[LHA_LV1_METHOD_LEN])
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

static ULONG lha_u32le_lv1(const UBYTE *p)
{
    return ((ULONG)p[0])
         | ((ULONG)p[1] << 8)
         | ((ULONG)p[2] << 16)
         | ((ULONG)p[3] << 24);
}

static UWORD lha_u16le_lv1(const UBYTE *p)
{
    return (UWORD)(((UWORD)p[0]) | ((UWORD)p[1] << 8));
}

static LONG lha_fill_level1_timestamp(struct LHAParsedEntry *outEntry, ULONG raw_timestamp)
{
    (void)raw_timestamp;

    if (outEntry == NULL)
        return LHAERR_INVALID_ARGUMENT;

    /*
     * 0.1 skeleton:
     * keep Amiga days/mins/ticks conversion deferred.
     */
    outEntry->lpe_TimestampDays = 0UL;
    outEntry->lpe_TimestampMins = 0UL;
    outEntry->lpe_TimestampTicks = 0UL;

    return LHAERR_OK;
}

static LONG lha_parse_level1_name(
    struct LHAArchive *arc,
    UBYTE name_len,
    struct LHAParsedEntry *outEntry)
{
    ULONG need_size;
    LONG rc;

    if ((arc == NULL) || (outEntry == NULL))
        return LHAERR_INVALID_ARGUMENT;

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

static LONG lha_parse_level1_ext_headers(
    struct LHAArchive *arc,
    ULONG *ioNextSize,
    ULONG *outDataOffset)
{
    UBYTE szbuf[2];
    UWORD next_size;
    ULONG cur_pos;
    ULONG payload_size;
    LONG rc;

    if ((arc == NULL) || (ioNextSize == NULL) || (outDataOffset == NULL))
        return LHAERR_INVALID_ARGUMENT;

    next_size = (UWORD)(*ioNextSize);

    /*
     * Minimal 0.1 policy:
     * - structurally walk the ext-header chain
     * - do not interpret optional header types yet
     * - stop when next_size == 0
     *
     * Assumption for this skeleton:
     * each ext header size value includes the 2-byte "next header size" field.
     */
    while (next_size != 0U)
    {
        if (next_size < 2U)
            return LHAERR_BAD_ARCHIVE;

        payload_size = (ULONG)next_size - 2UL;
        if (payload_size != 0UL)
        {
            rc = lha_skip_exact(arc, payload_size);
            if (rc != LHAERR_OK)
                return rc;
        }

        rc = lha_tell(arc, &cur_pos);
        if (rc != LHAERR_OK)
            return rc;

        rc = lha_read_exact(arc, szbuf, 2UL);
        if (rc != LHAERR_OK)
            return rc;

        next_size = lha_u16le_lv1(szbuf);
        *ioNextSize = (ULONG)next_size;
        *outDataOffset = cur_pos + 2UL;
    }

    return LHAERR_OK;
}

LONG lha_parse_level1(struct LHAArchive *arc, struct LHAParsedEntry *outEntry)
{
    UBYTE header_size;
    UBYTE header_cksum;
    UBYTE method_tag[LHA_LV1_METHOD_LEN];
    UBYTE u32buf[4];
    UBYTE u16buf[2];
    UBYTE name_len;
    UBYTE os_byte;
    ULONG entry_start;
    ULONG raw_timestamp;
    ULONG base_header_end;
    ULONG current_pos;
    ULONG ext_next_size;
    LONG rc;

    if ((arc == NULL) || (outEntry == NULL))
        return LHAERR_INVALID_ARGUMENT;

    rc = lha_tell(arc, &entry_start);
    if (rc != LHAERR_OK)
        return rc;

    rc = lha_read_exact(arc, &header_size, 1UL);
    if (rc != LHAERR_OK)
        return rc;

    if (header_size == 0U)
        return LHAERR_END_OF_ARCHIVE;

    rc = lha_read_exact(arc, &header_cksum, 1UL);
    if (rc != LHAERR_OK)
        return rc;

    (void)header_cksum; /* checksum validation deferred */

    rc = lha_read_exact(arc, method_tag, LHA_LV1_METHOD_LEN);
    if (rc != LHAERR_OK)
        return rc;

    outEntry->lpe_MethodID = lha_method_from_tag_lv1(method_tag);
    if (outEntry->lpe_MethodID != LHA_METH_UNKNOWN)
        outEntry->lpe_Flags |= LHA_ENTRY_FLAG_METHOD_KNOWN;

    rc = lha_read_exact(arc, u32buf, 4UL);
    if (rc != LHAERR_OK)
        return rc;
    outEntry->lpe_PackedSize = lha_u32le_lv1(u32buf);

    rc = lha_read_exact(arc, u32buf, 4UL);
    if (rc != LHAERR_OK)
        return rc;
    outEntry->lpe_UnpackedSize = lha_u32le_lv1(u32buf);

    rc = lha_read_exact(arc, u32buf, 4UL);
    if (rc != LHAERR_OK)
        return rc;
    raw_timestamp = lha_u32le_lv1(u32buf);

    rc = lha_fill_level1_timestamp(outEntry, raw_timestamp);
    if (rc != LHAERR_OK)
        return rc;

    {
        UBYTE attr;

        rc = lha_read_exact(arc, &attr, 1UL);
        if (rc != LHAERR_OK)
            return rc;

        /*
         * 0.1 minimal policy:
         * default to file until richer type semantics are added.
         */
        (void)attr;
        outEntry->lpe_Type = LHA_ENTRY_TYPE_FILE;
    }

    rc = lha_read_exact(arc, &name_len, 1UL);
    if (rc != LHAERR_OK)
        return rc;

    rc = lha_parse_level1_name(arc, name_len, outEntry);
    if (rc != LHAERR_OK)
        return rc;

    rc = lha_read_exact(arc, u16buf, 2UL);
    if (rc != LHAERR_OK)
        return rc;
    outEntry->lpe_CRC = lha_u16le_lv1(u16buf);
    outEntry->lpe_Flags |= LHA_ENTRY_FLAG_HAS_CRC;

    rc = lha_read_exact(arc, &os_byte, 1UL);
    if (rc != LHAERR_OK)
        return rc;
    (void)os_byte;

    rc = lha_read_exact(arc, u16buf, 2UL);
    if (rc != LHAERR_OK)
        return rc;
    ext_next_size = (ULONG)lha_u16le_lv1(u16buf);

    outEntry->lpe_HeaderLevel = LHA_HEADER_LEVEL_1;
    outEntry->lpe_HeaderOffset = entry_start;

    /*
     * Minimal plausibility:
     * header_size is the size of the base header payload after the size byte.
     * We have read exactly:
     *   1 checksum
     *   5 method
     *   4 packed
     *   4 unpacked
     *   4 timestamp
     *   1 attr
     *   1 name_len
     *   N name
     *   2 crc
     *   1 os
     *   2 next_ext_size
     *
     * = 25 + N bytes after the initial header_size byte.
     */
    if ((ULONG)header_size < (25UL + (ULONG)name_len))
        return LHAERR_BAD_ARCHIVE;

    base_header_end = entry_start + 1UL + (ULONG)header_size;

    rc = lha_tell(arc, &current_pos);
    if (rc != LHAERR_OK)
        return rc;

    /*
     * If the base header has trailing bytes we do not yet interpret,
     * skip them before walking the ext-header chain.
     */
    if (current_pos > base_header_end)
        return LHAERR_BAD_ARCHIVE;

    if (current_pos < base_header_end)
    {
        rc = lha_skip_exact(arc, base_header_end - current_pos);
        if (rc != LHAERR_OK)
            return rc;
    }

    outEntry->lpe_DataOffset = base_header_end;

    /*
     * Walk ext headers structurally. Data begins after the full chain.
     */
    if (ext_next_size != 0UL)
    {
        rc = lha_parse_level1_ext_headers(arc, &ext_next_size, &outEntry->lpe_DataOffset);
        if (rc != LHAERR_OK)
            return rc;

        rc = lha_tell(arc, &outEntry->lpe_DataOffset);
        if (rc != LHAERR_OK)
            return rc;
    }

    return LHAERR_OK;
}
