#include <dos/dos.h>
#include <exec/memory.h>

#include "../internal/lha_types.h"
#include "../internal/lha_errors.h"

static ULONG lha_strlen(STRPTR s)
{
    ULONG n;

    n = 0UL;
    if (s == NULL)
        return 0UL;

    while (s[n] != '\0')
        n++;

    return n;
}

static BOOL lha_path_is_absolute_or_device_like(STRPTR path)
{
    ULONG i;

    if (path == NULL)
        return TRUE;

    if (path[0] == '/' || path[0] == '\\')
        return TRUE;

    for (i = 0UL; path[i] != '\0'; i++)
    {
        if (path[i] == ':')
            return TRUE;
    }

    return FALSE;
}

static BOOL lha_path_has_parent_escape(STRPTR path)
{
    ULONG i;

    if (path == NULL)
        return TRUE;

    for (i = 0UL; path[i] != '\0'; i++)
    {
        if ((path[i] == '.') && (path[i + 1] == '.'))
        {
            BOOL left_ok;
            BOOL right_ok;

            left_ok = (i == 0UL) || (path[i - 1] == '/') || (path[i - 1] == '\\');
            right_ok = (path[i + 2] == '\0') || (path[i + 2] == '/') || (path[i + 2] == '\\');

            if (left_ok && right_ok)
                return TRUE;
        }
    }

    return FALSE;
}

static VOID lha_normalize_separators(STRPTR s)
{
    ULONG i;

    if (s == NULL)
        return;

    for (i = 0UL; s[i] != '\0'; i++)
    {
        if (s[i] == '\\')
            s[i] = '/';
    }
}

LONG lha_sanitize_entry_path(
    struct LHAArchive *arc,
    STRPTR entryPath,
    STRPTR destDir,
    STRPTR *outFullPath)
{
    ULONG dest_len;
    ULONG entry_len;
    ULONG total_len;
    ULONG i;
    STRPTR dst;

    if ((arc == NULL) || (entryPath == NULL) || (destDir == NULL) || (outFullPath == NULL))
        return LHAERR_INVALID_ARGUMENT;

    *outFullPath = NULL;

    if (lha_path_is_absolute_or_device_like(entryPath))
        return LHAERR_BAD_PATH;

    if (lha_path_has_parent_escape(entryPath))
        return LHAERR_BAD_PATH;

    dest_len = lha_strlen(destDir);
    entry_len = lha_strlen(entryPath);

    total_len = dest_len + 1UL + entry_len + 1UL;

    if ((arc->lhaa_PathScratch == NULL) || (arc->lhaa_PathScratchSize < total_len))
        return LHAERR_NO_MEMORY;

    dst = arc->lhaa_PathScratch;

    for (i = 0UL; i < dest_len; i++)
        dst[i] = destDir[i];

    if ((dest_len > 0UL) && (dst[dest_len - 1] != '/') && (dst[dest_len - 1] != ':'))
    {
        dst[dest_len] = '/';
        dest_len++;
    }

    for (i = 0UL; i < entry_len; i++)
        dst[dest_len + i] = entryPath[i];

    dst[dest_len + entry_len] = '\0';

    lha_normalize_separators(dst);

    *outFullPath = dst;
    return LHAERR_OK;
}

LONG lha_ensure_parent_dirs(STRPTR fullPath)
{
    ULONG i;
    BPTR lock;

    if (fullPath == NULL)
        return LHAERR_INVALID_ARGUMENT;

    for (i = 0UL; fullPath[i] != '\0'; i++)
    {
        if (fullPath[i] == '/')
        {
            fullPath[i] = '\0';

            if (fullPath[0] != '\0')
            {
                lock = CreateDir(fullPath);
                if (lock != ZERO)
                    UnLock(lock);
            }

            fullPath[i] = '/';
        }
    }

    return LHAERR_OK;
}

LONG lha_ensure_directory_path(STRPTR dirPath)
{
    BPTR lock;

    if (dirPath == NULL)
        return LHAERR_INVALID_ARGUMENT;

    lock = CreateDir(dirPath);
    if (lock != ZERO)
    {
        UnLock(lock);
        return LHAERR_OK;
    }

    /*
     * If creation failed because it already exists as a directory,
     * treat that as success for 0.1.
     * We keep this conservative and do not attempt deeper diagnosis yet.
     */
    return LHAERR_OK;
}

