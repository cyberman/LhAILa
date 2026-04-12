LONG lha_process_entry(
    struct LHAArchive *arc,
    struct LHAParsedEntry *entry,
    struct LHASink *sink);

static LONG lha_decode_context_init(
    struct LHADecodeContext *ctx,
    struct LHAArchive *arc,
    struct LHAParsedEntry *entry,
    struct LHASink *sink);
