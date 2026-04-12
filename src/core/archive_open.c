#include <exec/types.h>
#include <exec/memory.h>

#include <proto/exec.h>
#include <proto/dos.h>

#include "../internal/lha_types.h"
#include "../internal/lha_errors.h"
#include "../internal/lha_io.h"

/*
 * Local defaults for 0.1
 */
#define LHA_DEFAULT_IO_BUFFER_SIZE       4096UL
#define LHA_DEFAULT_METHOD_BUFFER_SIZE   4096UL
#define LHA_DEFAULT_PATH_SCRATCH_SIZE    1024UL
#define LHA_DEFAULT_ERROR_PATH_SIZE       256UL

VOID lha_core_close_archive(struct LHAArchive *arc);

static VOID lha_core_clear_archive(struct LHAArchive *arc)
{
    if (arc != NULL)
    {
        arc->lhaa_File = (BPTR)0;
        arc->lhaa_Path = NULL;

        arc->lhaa_FileSize = 0UL;
        arc->lhaa_FilePos = 0UL;

        arc->lhaa_StateFlags = 0UL;
        arc->lhaa_LastError = LHAERR_OK;
        arc->lhaa_LastDOSIoErr = 0L;

        arc->lhaa_HaveCurrentEntry = FALSE;

        arc->lhaa_CurrentEntry.lpe_HeaderLevel = 0UL;
        arc->lhaa_CurrentEntry.lpe_MethodID = 0U;
        arc->lhaa_CurrentEntry.lpe_Type = 0UL;
        arc->lhaa_CurrentEntry.lpe_HeaderOffset = 0UL;
        arc->lhaa_CurrentEntry.lpe_DataOffset = 0UL;
        arc->lhaa_CurrentEntry.lpe_PackedSize = 0UL;
        arc->lhaa_CurrentEntry.lpe_UnpackedSize = 0UL;
        arc->lhaa_CurrentEntry.lpe_CRC = 0U;
        arc->lhaa_CurrentEntry.lpe_TimestampDays = 0UL;
        arc->lhaa_CurrentEntry.lpe_TimestampMins = 0UL;
        arc->lhaa_CurrentEntry.lpe_TimestampTicks = 0UL;
        arc->lhaa_CurrentEntry.lpe_Path = NULL;
        arc->lhaa_CurrentEntry.lpe_Flags = 0UL;

        arc->lhaa_IOBuffer = NULL;
        arc->lhaa_IOBufferSize = 0UL;

        arc->lhaa_MethodBuffer = NULL;
        arc->lhaa_MethodBufferSize = 0UL;

        arc->lhaa_PathScratch = NULL;
        arc->lhaa_PathScratchSize = 0UL;

        arc->lhaa_ErrorPathScratch = NULL;
        arc->lhaa_ErrorPathScratchSize = 0UL;
    }
}

static STRPTR lha_core_dup_cstr(STRPTR src)
{
    STRPTR dst;
    ULONG len;
    ULONG i;

    if (src == NULL)
        return NULL;

    len = 0UL;
    while (src[len] != '\0')
        len++;

    dst = (STRPTR)AllocVec(len + 1UL, MEMF_ANY);
    if (dst == NULL)
        return NULL;

    for (i = 0UL; i < len; i++)
        dst[i] = src[i];
    dst[len] = '\0';

    return dst;
}

static LONG lha_core_alloc_buffers(struct LHAArchive *arc)
{
    arc->lhaa_IOBuffer = (UBYTE *)AllocVec(LHA_DEFAULT_IO_BUFFER_SIZE, MEMF_ANY);
    if (arc->lhaa_IOBuffer == NULL)
        return LHAERR_NO_MEMORY;
    arc->lhaa_IOBufferSize = LHA_DEFAULT_IO_BUFFER_SIZE;

    arc->lhaa_MethodBuffer = (UBYTE *)AllocVec(LHA_DEFAULT_METHOD_BUFFER_SIZE, MEMF_ANY);
    if (arc->lhaa_MethodBuffer == NULL)
        return LHAERR_NO_MEMORY;
    arc->lhaa_MethodBufferSize = LHA_DEFAULT_METHOD_BUFFER_SIZE;

    arc->lhaa_PathScratch = (STRPTR)AllocVec(LHA_DEFAULT_PATH_SCRATCH_SIZE, MEMF_ANY);
    if (arc->lhaa_PathScratch == NULL)
        return LHAERR_NO_MEMORY;
    arc->lhaa_PathScratchSize = LHA_DEFAULT_PATH_SCRATCH_SIZE;
    arc->lhaa_PathScratch[0] = '\0';

    arc->lhaa_ErrorPathScratch = (STRPTR)AllocVec(LHA_DEFAULT_ERROR_PATH_SIZE, MEMF_ANY);
    if (arc->lhaa_ErrorPathScratch == NULL)
        return LHAERR_NO_MEMORY;
    arc->lhaa_ErrorPathScratchSize = LHA_DEFAULT_ERROR_PATH_SIZE;
    arc->lhaa_ErrorPathScratch[0] = '\0';

    return LHAERR_OK;
}

LONG lha_core_open_archive(STRPTR path, struct LHAArchive **outArc)
{
    struct LHAArchive *arc;
    LONG rc;
    LONG dos_err;
    LONG seek_res;
    LONG end_pos;

    if ((path == NULL) || (outArc == NULL))
        return LHAERR_INVALID_ARGUMENT;

    *outArc = NULL;
    arc = NULL;
    rc = LHAERR_OK;
    dos_err = 0L;
    seek_res = -1L;
    end_pos = 0L;

    arc = (struct LHAArchive *)AllocVec(sizeof(struct LHAArchive), MEMF_ANY | MEMF_CLEAR);
    if (arc == NULL)
        return LHAERR_NO_MEMORY;

    lha_core_clear_archive(arc);

    arc->lhaa_Path = lha_core_dup_cstr(path);
    if (arc->lhaa_Path == NULL)
    {
        rc = LHAERR_NO_MEMORY;
        goto done;
    }

    arc->lhaa_File = Open(path, MODE_OLDFILE);
    if (arc->lhaa_File == (BPTR)0)
    {
        arc->lhaa_LastDOSIoErr = IoErr();
        rc = LHAERR_OPEN_FAILED;
        goto done;
    }

    seek_res = Seek(arc->lhaa_File, 0L, OFFSET_END);
    if (seek_res == -1L)
    {
        arc->lhaa_LastDOSIoErr = IoErr();
        rc = LHAERR_READ_FAILED;
        goto done;
    }

    end_pos = Seek(arc->lhaa_File, 0L, OFFSET_CURRENT);
    if (end_pos == -1L)
    {
        arc->lhaa_LastDOSIoErr = IoErr();
        rc = LHAERR_READ_FAILED;
        goto done;
    }

    arc->lhaa_FileSize = (ULONG)end_pos;

    if (Seek(arc->lhaa_File, 0L, OFFSET_BEGINNING) == -1L)
    {
        arc->lhaa_LastDOSIoErr = IoErr();
        rc = LHAERR_READ_FAILED;
        goto done;
    }

    arc->lhaa_FilePos = 0UL;

    rc = lha_core_alloc_buffers(arc);
    if (rc != LHAERR_OK)
        goto done;

    *outArc = arc;
    arc = NULL;

done:
    if (arc != NULL)
    {
        dos_err = arc->lhaa_LastDOSIoErr;
        lha_core_close_archive(arc);
        arc = NULL;
        if ((rc != LHAERR_OK) && (dos_err != 0L))
        {
            /* nothing else to do in 0.1; detail is discarded after close */
        }
    }

    return rc;
}

VOID lha_core_close_archive(struct LHAArchive *arc)
{
    if (arc == NULL)
        return;

    if (arc->lhaa_File != (BPTR)0)
    {
        Close(arc->lhaa_File);
        arc->lhaa_File = (BPTR)0;
    }

    if (arc->lhaa_IOBuffer != NULL)
    {
        FreeVec(arc->lhaa_IOBuffer);
        arc->lhaa_IOBuffer = NULL;
    }
    arc->lhaa_IOBufferSize = 0UL;

    if (arc->lhaa_MethodBuffer != NULL)
    {
        FreeVec(arc->lhaa_MethodBuffer);
        arc->lhaa_MethodBuffer = NULL;
    }
    arc->lhaa_MethodBufferSize = 0UL;

    if (arc->lhaa_PathScratch != NULL)
    {
        FreeVec(arc->lhaa_PathScratch);
        arc->lhaa_PathScratch = NULL;
    }
    arc->lhaa_PathScratchSize = 0UL;

    if (arc->lhaa_ErrorPathScratch != NULL)
    {
        FreeVec(arc->lhaa_ErrorPathScratch);
        arc->lhaa_ErrorPathScratch = NULL;
    }
    arc->lhaa_ErrorPathScratchSize = 0UL;

    if (arc->lhaa_Path != NULL)
    {
        FreeVec(arc->lhaa_Path);
        arc->lhaa_Path = NULL;
    }

    FreeVec(arc);
}

LONG lha_core_rewind_archive(struct LHAArchive *arc)
{
    if (arc == NULL)
        return LHAERR_INVALID_ARGUMENT;

    if (arc->lhaa_File == (BPTR)0)
        return LHAERR_INVALID_ARGUMENT;

    if (Seek(arc->lhaa_File, 0L, OFFSET_BEGINNING) == -1L)
    {
        arc->lhaa_LastDOSIoErr = IoErr();
        arc->lhaa_LastError = LHAERR_READ_FAILED;
        return LHAERR_READ_FAILED;
    }

    arc->lhaa_FilePos = 0UL;
    arc->lhaa_HaveCurrentEntry = FALSE;

    arc->lhaa_CurrentEntry.lpe_HeaderLevel = 0UL;
    arc->lhaa_CurrentEntry.lpe_MethodID = 0U;
    arc->lhaa_CurrentEntry.lpe_Type = 0UL;
    arc->lhaa_CurrentEntry.lpe_HeaderOffset = 0UL;
    arc->lhaa_CurrentEntry.lpe_DataOffset = 0UL;
    arc->lhaa_CurrentEntry.lpe_PackedSize = 0UL;
    arc->lhaa_CurrentEntry.lpe_UnpackedSize = 0UL;
    arc->lhaa_CurrentEntry.lpe_CRC = 0U;
    arc->lhaa_CurrentEntry.lpe_TimestampDays = 0UL;
    arc->lhaa_CurrentEntry.lpe_TimestampMins = 0UL;
    arc->lhaa_CurrentEntry.lpe_TimestampTicks = 0UL;
    arc->lhaa_CurrentEntry.lpe_Path = NULL;
    arc->lhaa_CurrentEntry.lpe_Flags = 0UL;

    arc->lhaa_LastError = LHAERR_OK;
    arc->lhaa_LastDOSIoErr = 0L;

    return LHAERR_OK;
}
