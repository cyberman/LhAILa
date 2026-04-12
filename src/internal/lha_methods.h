#ifndef LHAILA_INTERNAL_LHA_METHODS_H
#define LHAILA_INTERNAL_LHA_METHODS_H

/*
 * LhAILa
 * Internal method registry contract
 *
 * Private header.
 */

#ifndef LHAILA_INTERNAL_LHA_TYPES_H
#include "lha_types.h"
#endif

const struct LHAMethodOps *lha_find_method_ops(UWORD methodId);

LONG lha_decode_lh0(struct LHADecodeContext *ctx);
LONG lha_decode_lh1(struct LHADecodeContext *ctx);
LONG lha_decode_lh4(struct LHADecodeContext *ctx);
LONG lha_decode_lh5(struct LHADecodeContext *ctx);

#endif /* LHAILA_INTERNAL_LHA_METHODS_H */

