VOID lha_crc_init(UWORD *crc);
VOID lha_crc_update(UWORD *crc, CONST_APTR data, ULONG size);
LONG lha_crc_verify(UWORD expected, UWORD actual);
