LONG lha_profile_check_entry(
    struct LHAParsedEntry *entry,
    ULONG *outProfileStatus,
    ULONG *outProfileReason);

static BOOL lha_profile_header_allowed(ULONG headerLevel);
static BOOL lha_profile_method_allowed(UWORD methodId);
