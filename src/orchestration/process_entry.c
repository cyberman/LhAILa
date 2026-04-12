#include "../internal/lha_types.h"
#include "../internal/lha_errors.h"
#include "../internal/lha_profile_api.h"
#include "../internal/lha_methods.h"
#include "../internal/lha_crc.h"
#include "../internal/lha_sink_state.h"

static LONG lha_decode_context_init(
    struct LHADecodeContext *ctx,
    struct LHAArchive *arc,
    struct LHAParsedEntry *entry,
    struct LHASink *sink)
{
    if ((ctx == NULL) || (arc == NULL) || (entry == NULL) || (sink == NULL))
        return LHAERR_INVALID_ARGUMENT;

    ctx->ldc_Archive = arc;
    ctx->ldc_Entry = entry;
    ctx->ldc_Sink = sink;

    ctx->ldc_SourceLeft = 0UL;
    ctx->ldc_OutputLeft = 0UL;
    ctx->ldc_RunningCRC = 0U;

    ctx->ldc_IOBuffer = arc->lhaa_IOBuffer;
    ctx->ldc_IOBufferSize = arc->lhaa_IOBufferSize;

    ctx->ldc_MethodBuffer = arc->lhaa_MethodBuffer;
    ctx->ldc_MethodBufferSize = arc->lhaa_MethodBufferSize;

    ctx->ldc_LastError = LHAERR_OK;

    return LHAERR_OK;
}

static LONG lha_verify_crc(struct LHADecodeContext *ctx, struct LHAParsedEntry *entry)
{
    struct LHASinkStateCommon *common;

    if ((ctx == NULL) || (entry == NULL))
        return LHAERR_INVALID_ARGUMENT;

    if ((entry->lpe_Flags & LHA_ENTRY_FLAG_HAS_CRC) == 0UL)
        return LHAERR_OK;

    if ((ctx->ldc_Sink == NULL) || (ctx->ldc_Sink->lhs_UserData == NULL))
        return LHAERR_INTERNAL;

    common = (struct LHASinkStateCommon *)ctx->ldc_Sink->lhs_UserData;

    return lha_crc_verify(entry->lpe_CRC, common->lhsc_RunningCRC);
}

LONG lha_process_entry(
    struct LHAArchive *arc,
    struct LHAParsedEntry *entry,
    struct LHASink *sink)
{
    ULONG profile_status;
    ULONG profile_reason;
    const struct LHAMethodOps *ops;
    struct LHADecodeContext ctx;
    LONG rc;

    if ((arc == NULL) || (entry == NULL) || (sink == NULL))
        return LHAERR_INVALID_ARGUMENT;

    profile_status = LHAPROFILE_INVALID;
    profile_reason = LHAPROFILE_REASON_NONE;

    rc = lha_profile_check_entry(entry, &profile_status, &profile_reason);
    if (rc != LHAERR_OK)
        return rc;

    if (profile_status != LHAPROFILE_MATCH)
    {
        if (profile_status == LHAPROFILE_KNOWN_OUTSIDE)
        {
            switch (profile_reason)
            {
                case LHAPROFILE_REASON_UNSUPPORTED_METHOD:
                    return LHAERR_UNSUPPORTED_METHOD;

                case LHAPROFILE_REASON_BAD_HEADER_LEVEL:
                    return LHAERR_BAD_ARCHIVE;

                case LHAPROFILE_REASON_BAD_ENTRY_TYPE:
                    return LHAERR_BAD_ARCHIVE;

                default:
                    return LHAERR_BAD_ARCHIVE;
            }
        }

        return LHAERR_BAD_ARCHIVE;
    }

    ops = lha_find_method_ops(entry->lpe_MethodID);
    if (ops == NULL)
        return LHAERR_UNSUPPORTED_METHOD;

    if (ops->lmo_Decode == NULL)
        return LHAERR_UNSUPPORTED_METHOD;

    if (sink->lhs_Begin == NULL)
        return LHAERR_INTERNAL;

    if (sink->lhs_Write == NULL)
        return LHAERR_INTERNAL;

    if (sink->lhs_End == NULL)
        return LHAERR_INTERNAL;

    if (sink->lhs_Abort == NULL)
        return LHAERR_INTERNAL;

    rc = sink->lhs_Begin(sink, entry);
    if (rc != LHAERR_OK)
        return rc;

    rc = lha_decode_context_init(&ctx, arc, entry, sink);
    if (rc != LHAERR_OK)
    {
        sink->lhs_Abort(sink);
        return rc;
    }

    rc = ops->lmo_Decode(&ctx);
    if (rc != LHAERR_OK)
    {
        sink->lhs_Abort(sink);
        return rc;
    }

    rc = lha_verify_crc(&ctx, entry);
    if (rc != LHAERR_OK)
    {
        sink->lhs_Abort(sink);
        return rc;
    }

    rc = sink->lhs_End(sink);
    if (rc != LHAERR_OK)
        return rc;

    return LHAERR_OK;
}
