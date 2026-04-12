LONG lha_sanitize_entry_path(
    struct LHAArchive *arc,
    STRPTR entryPath,
    STRPTR destDir,
    STRPTR *outFullPath);

LONG lha_ensure_parent_dirs(STRPTR fullPath);

