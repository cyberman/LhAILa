#include <exec/types.h>
#include <exec/memory.h>

#include <proto/dos.h>

#include "../internal/lha_types.h"
#include "../internal/lha_errors.h"
#include "../internal/lha_crc.h"
#include "../internal/lha_sink_state.h"

struct LHAFileSinkState
{
    struct LHASinkStateCommon lhfs_Common;

    BPTR   lhfs_File;
    STRPTR lhfs_FullPath;
    BOOL   lhfs_Completed;
};

LONG lha_file_sink_begin(struct LHASink *sink, struct LHAParsedEntry *entry)
{
    struct LHAFileSinkState *state;

    if ((sink == NULL) || (entry == NULL))
        return LHAERR_INVALID_ARGUMENT;

    state = (struct LHAFileSinkState *)sink->lhs_UserData;
    if (state == NULL)
        return LHAERR_INVALID_ARGUMENT;

    if (state->lhfs_FullPath == NULL)
        return LHAERR_INVALID_ARGUMENT;

    state->lhfs_File = Open(state->lhfs_FullPath, MODE_NEWFILE);
    if (state->lhfs_File == (BPTR)0)
        return LHAERR_WRITE_FAILED;

    state->lhfs_Common.lhsc_BytesSeen = 0UL;
    state->lhfs_Completed = FALSE;
    lha_crc_init(&state->lhfs_Common.lhsc_RunningCRC);

    (void)entry;
    return LHAERR_OK;
}

LONG lha_file_sink_write(struct LHASink *sink, CONST_APTR data, ULONG size)
{
    struct LHAFileSinkState *state;
    LONG written;

    if ((sink == NULL) || (data == NULL))
        return LHAERR_INVALID_ARGUMENT;

    state = (struct LHAFileSinkState *)sink->lhs_UserData;
    if (state == NULL)
        return LHAERR_INVALID_ARGUMENT;

    if (state->lhfs_File == (BPTR)0)
        return LHAERR_INTERNAL;

    if (size == 0UL)
        return LHAERR_OK;

    written = Write(state->lhfs_File, (APTR)data, (LONG)size);
    if (written < 0L)
        return LHAERR_WRITE_FAILED;

    if ((ULONG)written != size)
        return LHAERR_WRITE_FAILED;

    state->lhfs_Common.lhsc_BytesSeen += size;
    lha_crc_update(&state->lhfs_Common.lhsc_RunningCRC, data, size);

    return LHAERR_OK;
}

LONG lha_file_sink_end(struct LHASink *sink)
{
    struct LHAFileSinkState *state;

    if (sink == NULL)
        return LHAERR_INVALID_ARGUMENT;

    state = (struct LHAFileSinkState *)sink->lhs_UserData;
    if (state == NULL)
        return LHAERR_INVALID_ARGUMENT;

    if (state->lhfs_File != (BPTR)0)
    {
        Close(state->lhfs_File);
        state->lhfs_File = (BPTR)0;
    }

    state->lhfs_Completed = TRUE;
    return LHAERR_OK;
}

VOID lha_file_sink_abort(struct LHASink *sink)
{
    struct LHAFileSinkState *state;

    if (sink == NULL)
        return;

    state = (struct LHAFileSinkState *)sink->lhs_UserData;
    if (state == NULL)
        return;

    if (state->lhfs_File != (BPTR)0)
    {
        Close(state->lhfs_File);
        state->lhfs_File = (BPTR)0;
    }

    if ((state->lhfs_FullPath != NULL) && !state->lhfs_Completed)
        DeleteFile(state->lhfs_FullPath);

    state->lhfs_Common.lhsc_BytesSeen = 0UL;
    state->lhfs_Common.lhsc_RunningCRC = 0U;
}
