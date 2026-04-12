LONG lha_sanitize_entry_path(
    struct LHAArchive *arc,
    STRPTR entryPath,
    STRPTR destDir,
    STRPTR *outFullPath);

LONG lha_ensure_parent_dirs(STRPTR fullPath);
static BOOL lha_path_has_parent_escape(STRPTR path);
static BOOL lha_path_is_absolute_or_device_like(STRPTR path);
