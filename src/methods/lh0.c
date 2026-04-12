#include "../internal/lha_types.h"
#include "../internal/lha_errors.h"
#include "../internal/lha_io.h"

LONG lha_decode_lh0(struct LHADecodeContext *ctx)
{
    ULONG chunk_size;
    LONG rc;

    if (ctx == NULL)
        return LHAERR_INVALID_ARGUMENT;

    if (ctx->ldc_Archive == NULL)
        return LHAERR_INVALID_ARGUMENT;

    if (ctx->ldc_Entry == NULL)
        return LHAERR_INVALID_ARGUMENT;

    if (ctx->ldc_Sink == NULL)
        return LHAERR_INVALID_ARGUMENT;

    if (ctx->ldc_IOBuffer == NULL)
        return LHAERR_INVALID_ARGUMENT;

    if (ctx->ldc_IOBufferSize == 0UL)
        return LHAERR_INVALID_ARGUMENT;

    /*
     * lh0 = stored / no compression
     *
     * Contract:
     * - copy exactly packed bytes
     * - output size must match unpacked size
     */
    if (ctx->ldc_Entry->lpe_PackedSize != ctx->ldc_Entry->lpe_UnpackedSize)
        return LHAERR_BAD_ARCHIVE;

    ctx->ldc_SourceLeft = ctx->ldc_Entry->lpe_PackedSize;
    ctx->ldc_OutputLeft = ctx->ldc_Entry->lpe_UnpackedSize;

    while (ctx->ldc_SourceLeft != 0UL)
    {
        chunk_size = ctx->ldc_IOBufferSize;
        if (chunk_size > ctx->ldc_SourceLeft)
            chunk_size = ctx->ldc_SourceLeft;

        rc = lha_read_exact(ctx->ldc_Archive, ctx->ldc_IOBuffer, chunk_size);
        if (rc != LHAERR_OK)
        {
            ctx->ldc_LastError = rc;
            return rc;
        }

        rc = ctx->ldc_Sink->lhs_Write(ctx->ldc_Sink, ctx->ldc_IOBuffer, chunk_size);
        if (rc != LHAERR_OK)
        {
            ctx->ldc_LastError = rc;
            return rc;
        }

        ctx->ldc_SourceLeft -= chunk_size;
        ctx->ldc_OutputLeft -= chunk_size;
    }

    if (ctx->ldc_OutputLeft != 0UL)
    {
        ctx->ldc_LastError = LHAERR_BAD_ARCHIVE;
        return LHAERR_BAD_ARCHIVE;
    }

    ctx->ldc_LastError = LHAERR_OK;
    return LHAERR_OK;
}

LONG lha_decode_lh1(struct LHADecodeContext *ctx)
{
    (void)ctx;
    return LHAERR_UNSUPPORTED_METHOD;
}

LONG lha_decode_lh4(struct LHADecodeContext *ctx)
{
    (void)ctx;
    return LHAERR_UNSUPPORTED_METHOD;
}

LONG lha_decode_lh5(struct LHADecodeContext *ctx)
{
    (void)ctx;
    return LHAERR_UNSUPPORTED_METHOD;
}
