LONG lha_extract_entry_to_dest(
    struct LHAArchive *arc,
    struct LHAParsedEntry *entry,
    STRPTR destDir,
    struct LHAExtractOptions *opts);

LONG lha_extract_archive_to_dest(
    struct LHAArchive *arc,
    STRPTR destDir,
    struct LHAExtractOptions *opts);
