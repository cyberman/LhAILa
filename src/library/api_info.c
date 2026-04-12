#include "../internal/lha_types.h"
#include "../internal/lha_errors.h"
#include "../../include/libraries/lhaarchive.h"

LONG LHAGetEntryInfo(struct LHAEntry *entry, struct LHAEntryInfo *outInfo)
{
    struct LHAParsedEntry *parsed;

    if ((entry == NULL) || (outInfo == NULL))
        return LHAERR_INVALID_ARGUMENT;

    if (outInfo->lei_StructSize != sizeof(struct LHAEntryInfo))
        return LHAERR_INVALID_ARGUMENT;

    parsed = (struct LHAParsedEntry *)entry;

    outInfo->lei_Path = parsed->lpe_Path;
    outInfo->lei_Type = parsed->lpe_Type;

    outInfo->lei_PackedSize = parsed->lpe_PackedSize;
    outInfo->lei_UnpackedSize = parsed->lpe_UnpackedSize;

    outInfo->lei_TimestampDays = parsed->lpe_TimestampDays;
    outInfo->lei_TimestampMins = parsed->lpe_TimestampMins;
    outInfo->lei_TimestampTicks = parsed->lpe_TimestampTicks;

    outInfo->lei_CRC = parsed->lpe_CRC;
    outInfo->lei_MethodID = parsed->lpe_MethodID;

    outInfo->lei_Flags = parsed->lpe_Flags;

    return LHAERR_OK;
}
