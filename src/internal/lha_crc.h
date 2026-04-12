#ifndef LHAILA_INTERNAL_LHA_CRC_H
#define LHAILA_INTERNAL_LHA_CRC_H

/*
 * LhAILa
 * Internal CRC helpers
 *
 * Private header.
 */

#ifndef EXEC_TYPES_H
#include <exec/types.h>
#endif

VOID lha_crc_init(UWORD *crc);
VOID lha_crc_update(UWORD *crc, CONST_APTR data, ULONG size);
LONG lha_crc_verify(UWORD expected, UWORD actual);

#endif /* LHAILA_INTERNAL_LHA_CRC_H */

