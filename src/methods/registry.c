#include "../internal/lha_types.h"
#include "../internal/lha_profile.h"
#include "../internal/lha_methods.h"

/*
 * Decoder entry points.
 * lh1/lh4/lh5 may still be stubbed initially.
 */
LONG lha_decode_lh0(struct LHADecodeContext *ctx);
LONG lha_decode_lh1(struct LHADecodeContext *ctx);
LONG lha_decode_lh4(struct LHADecodeContext *ctx);
LONG lha_decode_lh5(struct LHADecodeContext *ctx);

static const struct LHAMethodOps lha_method_table[] =
{
    { LHA_METH_LH0, LHA_METHOD_FLAG_SUPPORTED | LHA_METHOD_FLAG_CORE, lha_decode_lh0 },
    { LHA_METH_LH1, LHA_METHOD_FLAG_SUPPORTED | LHA_METHOD_FLAG_CORE, lha_decode_lh1 },
    { LHA_METH_LH4, LHA_METHOD_FLAG_SUPPORTED | LHA_METHOD_FLAG_CORE, lha_decode_lh4 },
    { LHA_METH_LH5, LHA_METHOD_FLAG_SUPPORTED | LHA_METHOD_FLAG_CORE, lha_decode_lh5 }
};

#define LHA_METHOD_TABLE_COUNT \
    (sizeof(lha_method_table) / sizeof(lha_method_table[0]))

const struct LHAMethodOps *lha_find_method_ops(UWORD methodId)
{
    ULONG i;

    for (i = 0UL; i < (ULONG)LHA_METHOD_TABLE_COUNT; i++)
    {
        if (lha_method_table[i].lmo_MethodID == methodId)
            return &lha_method_table[i];
    }

    return NULL;
}
