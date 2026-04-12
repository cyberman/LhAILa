#include <exec/types.h>

#include <proto/dos.h>

#include "../internal/lha_types.h"
#include "../internal/lha_errors.h"
#include "../internal/lha_io.h"

LONG lha_seek_abs(struct LHAArchive *arc, ULONG offset)
{
    LONG seek_rc;

    if (arc == NULL)
        return LHAERR_INVALID_ARGUMENT;

    if (arc->lhaa_File == (BPTR)0)
        return LHAERR_INVALID_ARGUMENT;

    seek_rc = Seek(arc->lhaa_File, (LONG)offset, OFFSET_BEGINNING);
    if (seek_rc == -1L)
    {
        arc->lhaa_LastDOSIoErr = IoErr();
        arc->lhaa_LastError = LHAERR_READ_FAILED;
        return LHAERR_READ_FAILED;
    }

    arc->lhaa_FilePos = offset;
    arc->lhaa_LastDOSIoErr = 0L;
    arc->lhaa_LastError = LHAERR_OK;

    return LHAERR_OK;
}

LONG lha_read_exact(struct LHAArchive *arc, APTR buf, ULONG size)
{
    LONG actual;

    if ((arc == NULL) || (buf == NULL))
        return LHAERR_INVALID_ARGUMENT;

    if (arc->lhaa_File == (BPTR)0)
        return LHAERR_INVALID_ARGUMENT;

    if (size == 0UL)
        return LHAERR_OK;

    actual = Read(arc->lhaa_File, buf, (LONG)size);
    if (actual < 0L)
    {
        arc->lhaa_LastDOSIoErr = IoErr();
        arc->lhaa_LastError = LHAERR_READ_FAILED;
        return LHAERR_READ_FAILED;
    }

    if ((ULONG)actual != size)
    {
        arc->lhaa_LastDOSIoErr = 0L;
        arc->lhaa_LastError = LHAERR_READ_FAILED;
        return LHAERR_READ_FAILED;
    }

    arc->lhaa_FilePos += size;
    arc->lhaa_LastDOSIoErr = 0L;
    arc->lhaa_LastError = LHAERR_OK;

    return LHAERR_OK;
}

LONG lha_skip_exact(struct LHAArchive *arc, ULONG size)
{
    LONG seek_rc;
    ULONG new_pos;

    if (arc == NULL)
        return LHAERR_INVALID_ARGUMENT;

    if (arc->lhaa_File == (BPTR)0)
        return LHAERR_INVALID_ARGUMENT;

    if (size == 0UL)
        return LHAERR_OK;

    new_pos = arc->lhaa_FilePos + size;
    seek_rc = Seek(arc->lhaa_File, (LONG)size, OFFSET_CURRENT);
    if (seek_rc == -1L)
    {
        arc->lhaa_LastDOSIoErr = IoErr();
        arc->lhaa_LastError = LHAERR_READ_FAILED;
        return LHAERR_READ_FAILED;
    }

    arc->lhaa_FilePos = new_pos;
    arc->lhaa_LastDOSIoErr = 0L;
    arc->lhaa_LastError = LHAERR_OK;

    return LHAERR_OK;
}

LONG lha_tell(struct LHAArchive *arc, ULONG *outOffset)
{
    if ((arc == NULL) || (outOffset == NULL))
        return LHAERR_INVALID_ARGUMENT;

    *outOffset = arc->lhaa_FilePos;
    return LHAERR_OK;
}
