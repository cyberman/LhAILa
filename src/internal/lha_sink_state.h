#ifndef LHAILA_INTERNAL_LHA_SINK_STATE_H
#define LHAILA_INTERNAL_LHA_SINK_STATE_H

/*
 * LhAILa
 * Common sink state prefix for CRC-aware sinks
 *
 * Private header.
 */

#ifndef EXEC_TYPES_H
#include <exec/types.h>
#endif

struct LHASinkStateCommon
{
    ULONG lhsc_BytesSeen;
    UWORD lhsc_RunningCRC;
};

#endif /* LHAILA_INTERNAL_LHA_SINK_STATE_H */

