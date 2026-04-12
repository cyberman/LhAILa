LONG lha_core_next_entry(struct LHAArchive *arc, struct LHAParsedEntry *outEntry);
static LONG lha_detect_header_level(struct LHAArchive *arc, ULONG *outLevel);
static VOID lha_clear_parsed_entry(struct LHAParsedEntry *entry);
