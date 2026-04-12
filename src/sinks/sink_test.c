LONG lha_test_sink_begin(struct LHASink *sink, struct LHAParsedEntry *entry);
LONG lha_test_sink_write(struct LHASink *sink, CONST_APTR data, ULONG size);
LONG lha_test_sink_end(struct LHASink *sink);
VOID lha_test_sink_abort(struct LHASink *sink);
