#include "../internal/lha_types.h"
#include "../internal/lha_errors.h"
#include "../internal/lha_profile.h"

static BOOL lha_profile_header_allowed(ULONG header_level)
{
    return (BOOL)LHA_PROFILE_HEADER_ALLOWED(header_level);
}

static BOOL lha_profile_method_allowed(UWORD method_id)
{
    return (BOOL)LHA_PROFILE_METHOD_ALLOWED(method_id);
}

static BOOL lha_profile_method_known_outside(UWORD method_id)
{
    return (BOOL)LHA_PROFILE_METHOD_KNOWN_OUTSIDE(method_id);
}

static BOOL lha_profile_entry_type_allowed(ULONG entry_type)
{
    if (entry_type == LHA_ENTRY_TYPE_FILE)
        return TRUE;

    if (entry_type == LHA_ENTRY_TYPE_DIRECTORY)
        return TRUE;

    return FALSE;
}

LONG lha_profile_check_entry(
    struct LHAParsedEntry *entry,
    ULONG *outProfileStatus,
    ULONG *outProfileReason)
{
    if ((entry == NULL) || (outProfileStatus == NULL) || (outProfileReason == NULL))
        return LHAERR_INVALID_ARGUMENT;

    *outProfileStatus = LHAPROFILE_INVALID;
    *outProfileReason = LHAPROFILE_REASON_NONE;

    if (!lha_profile_entry_type_allowed(entry->lpe_Type))
    {
        *outProfileStatus = LHAPROFILE_KNOWN_OUTSIDE;
        *outProfileReason = LHAPROFILE_REASON_BAD_ENTRY_TYPE;
        return LHAERR_OK;
    }

    if (!lha_profile_header_allowed(entry->lpe_HeaderLevel))
    {
        if (entry->lpe_HeaderLevel == LHA_HEADER_LEVEL_2)
        {
            *outProfileStatus = LHAPROFILE_KNOWN_OUTSIDE;
            *outProfileReason = LHAPROFILE_REASON_BAD_HEADER_LEVEL;
            return LHAERR_OK;
        }

        *outProfileStatus = LHAPROFILE_INVALID;
        *outProfileReason = LHAPROFILE_REASON_CORRUPT_HEADER;
        return LHAERR_OK;
    }

    if (lha_profile_method_allowed(entry->lpe_MethodID))
    {
        *outProfileStatus = LHAPROFILE_MATCH;
        *outProfileReason = LHAPROFILE_REASON_NONE;
        return LHAERR_OK;
    }

    if (lha_profile_method_known_outside(entry->lpe_MethodID))
    {
        *outProfileStatus = LHAPROFILE_KNOWN_OUTSIDE;
        *outProfileReason = LHAPROFILE_REASON_UNSUPPORTED_METHOD;
        return LHAERR_OK;
    }

    *outProfileStatus = LHAPROFILE_INVALID;
    *outProfileReason = LHAPROFILE_REASON_CORRUPT_HEADER;

    return LHAERR_OK;
}
