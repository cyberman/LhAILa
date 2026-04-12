#include "../internal/lha_types.h"
#include "../internal/lha_errors.h"
#include "../internal/lha_sinks.h"
#include "../internal/lha_crc.h"
#include "../internal/lha_sink_state.h"

struct LHATestSinkState
{
    struct LHASinkStateCommon lhts_Common;
};

LONG lha_test_sink_begin(struct LHASink *sink, struct LHAParsedEntry *entry)
{
    struct LHATestSinkState *state;

    if ((sink == NULL) || (entry == NULL))
        return LHAERR_INVALID_ARGUMENT;

    state = (struct LHATestSinkState *)sink->lhs_UserData;
    if (state == NULL)
        return LHAERR_INVALID_ARGUMENT;

    state->lhts_Common.lhsc_BytesSeen = 0UL;
    lha_crc_init(&state->lhts_Common.lhsc_RunningCRC);

    (void)entry;
    return LHAERR_OK;
}

LONG lha_test_sink_write(struct LHASink *sink, CONST_APTR data, ULONG size)
{
    struct LHATestSinkState *state;

    if ((sink == NULL) || (data == NULL))
        return LHAERR_INVALID_ARGUMENT;

    state = (struct LHATestSinkState *)sink->lhs_UserData;
    if (state == NULL)
        return LHAERR_INVALID_ARGUMENT;

    state->lhts_Common.lhsc_BytesSeen += size;
    lha_crc_update(&state->lhts_Common.lhsc_RunningCRC, data, size);

    return LHAERR_OK;
}

LONG lha_test_sink_end(struct LHASink *sink)
{
    if (sink == NULL)
        return LHAERR_INVALID_ARGUMENT;

    return LHAERR_OK;
}

VOID lha_test_sink_abort(struct LHASink *sink)
{
    struct LHATestSinkState *state;

    if (sink == NULL)
        return;

    state = (struct LHATestSinkState *)sink->lhs_UserData;
    if (state != NULL)
    {
        state->lhts_Common.lhsc_BytesSeen = 0UL;
        state->lhts_Common.lhsc_RunningCRC = 0U;
    }
}
