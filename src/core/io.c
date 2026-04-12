static LONG lha_seek_abs(struct LHAArchive *arc, ULONG offset);
static LONG lha_read_exact(struct LHAArchive *arc, APTR buf, ULONG size);
static LONG lha_skip_exact(struct LHAArchive *arc, ULONG size);
static LONG lha_tell(struct LHAArchive *arc, ULONG *outOffset);
