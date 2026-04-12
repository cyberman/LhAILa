#include <exec/types.h>

#include "../internal/lha_errors.h"

/*
 * CRC-16 used by LHA-family archives.
 *
 * Polynomial: 0xA001
 * Initial value: 0x0000
 *
 * 0.1 policy:
 * - keep implementation small and explicit
 * - no lookup table yet
 * - correctness first
 */

static UWORD lha_crc16_update_byte(UWORD crc, UBYTE value)
{
    UWORD i;

    crc ^= (UWORD)value;

    for (i = 0U; i < 8U; i++)
    {
        if ((crc & 1U) != 0U)
            crc = (UWORD)((crc >> 1) ^ 0xA001U);
        else
            crc = (UWORD)(crc >> 1);
    }

    return crc;
}

VOID lha_crc_init(UWORD *crc)
{
    if (crc == NULL)
        return;

    *crc = 0U;
}

VOID lha_crc_update(UWORD *crc, CONST_APTR data, ULONG size)
{
    const UBYTE *p;
    ULONG i;
    UWORD local_crc;

    if ((crc == NULL) || (data == NULL))
        return;

    p = (const UBYTE *)data;
    local_crc = *crc;

    for (i = 0UL; i < size; i++)
        local_crc = lha_crc16_update_byte(local_crc, p[i]);

    *crc = local_crc;
}

LONG lha_crc_verify(UWORD expected, UWORD actual)
{
    if (expected == actual)
        return LHAERR_OK;

    return LHAERR_CRC_MISMATCH;
}
