#include "../internal/lha_types.h"
#include "../internal/lha_errors.h"
#include "../internal/lha_parse.h"
#include "../internal/lha_orchestration.h"
#include "../internal/lha_extract.h"

LONG lha_sanitize_entry_path(
    struct LHAArchive *arc,
    STRPTR entryPath,
    STRPTR destDir,
    STRPTR *outFullPath);

LONG lha_ensure_parent_dirs(STRPTR fullPath);

LONG lha_file_sink_begin(struct LHASink *sink, struct LHAParsedEntry *entry);
LONG lha_file_sink_write(struct LHASink *sink, CONST_APTR data, ULONG size);
LONG lha_file_sink_end(struct LHASink *sink);
VOID lha_file_sink_abort(struct LHASink *sink);
LONG lha_ensure_directory_path(STRPTR dirPath);

struct LHAFileSinkState
{
    BPTR   lhfs_File;
    STRPTR lhfs_FullPath;
    ULONG  lhfs_BytesWritten;
    UWORD  lhfs_RunningCRC;
    BOOL   lhfs_Completed;
};

static LONG lha_build_file_sink(
    struct LHASink *outSink,
    struct LHAFileSinkState *outState,
    STRPTR fullPath)
{
    if ((outSink == NULL) || (outState == NULL) || (fullPath == NULL))
        return LHAERR_INVALID_ARGUMENT;

    outState->lhfs_File = ZERO;
    outState->lhfs_FullPath = fullPath;
    outState->lhfs_BytesWritten = 0UL;
    outState->lhfs_RunningCRC = 0U;
    outState->lhfs_Completed = FALSE;

    outSink->lhs_UserData = (APTR)outState;
    outSink->lhs_Begin = lha_file_sink_begin;
    outSink->lhs_Write = lha_file_sink_write;
    outSink->lhs_End = lha_file_sink_end;
    outSink->lhs_Abort = lha_file_sink_abort;
    outSink->lhs_Flags = 0UL;

    return LHAERR_OK;
}

LONG lha_extract_entry_to_dest(
    struct LHAArchive *arc,
    struct LHAParsedEntry *entry,
    STRPTR destDir,
    struct LHAExtractOptions *opts)
{
    struct LHASink sink;
    struct LHAFileSinkState sink_state;
    STRPTR full_path;
    LONG rc;

    (void)opts;

    if ((arc == NULL) || (entry == NULL) || (destDir == NULL))
        return LHAERR_INVALID_ARGUMENT;

    if (entry->lpe_Type == LHA_ENTRY_TYPE_DIRECTORY)
    {
        rc = lha_sanitize_entry_path(arc, entry->lpe_Path, destDir, &full_path);
        if (rc != LHAERR_OK)
            return rc;

        rc = lha_ensure_parent_dirs(full_path);
        if (rc != LHAERR_OK)
            return rc;

        return lha_ensure_directory_path(full_path);
    }

    rc = lha_sanitize_entry_path(arc, entry->lpe_Path, destDir, &full_path);
    if (rc != LHAERR_OK)
        return rc;

    rc = lha_ensure_parent_dirs(full_path);
    if (rc != LHAERR_OK)
        return rc;

    rc = lha_build_file_sink(&sink, &sink_state, full_path);
    if (rc != LHAERR_OK)
        return rc;

    return lha_process_entry(arc, entry, &sink);
}

LONG lha_extract_archive_to_dest(
    struct LHAArchive *arc,
    STRPTR destDir,
    struct LHAExtractOptions *opts)
{
    struct LHAParsedEntry entry;
    LONG rc;

    if ((arc == NULL) || (destDir == NULL))
        return LHAERR_INVALID_ARGUMENT;

    rc = lha_core_rewind_archive(arc);
    if (rc != LHAERR_OK)
        return rc;

    for (;;)
    {
        rc = lha_core_next_entry(arc, &entry);
        if (rc == LHAERR_END_OF_ARCHIVE)
            return LHAERR_OK;

        if (rc != LHAERR_OK)
            return rc;

        rc = lha_extract_entry_to_dest(arc, &entry, destDir, opts);
        if (rc != LHAERR_OK)
            return rc;
    }
}
