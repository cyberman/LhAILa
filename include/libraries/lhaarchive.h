#ifndef LIBRARIES_LHAARCHIVE_H
#define LIBRARIES_LHAARCHIVE_H

/*
 * LhAILa
 * Public library header
 *
 * Initial 0.1 surface.
 */

#ifndef EXEC_TYPES_H
#include <exec/types.h>
#endif

struct LHAArchive;
struct LHAEntry;

struct LHAExtractOptions
{
    ULONG leo_StructSize;
    ULONG leo_Flags;
};

struct LHAEntryInfo
{
    ULONG  lei_StructSize;

    STRPTR lei_Path;
    ULONG  lei_Type;

    ULONG  lei_PackedSize;
    ULONG  lei_UnpackedSize;

    ULONG  lei_TimestampDays;
    ULONG  lei_TimestampMins;
    ULONG  lei_TimestampTicks;

    UWORD  lei_CRC;
    UWORD  lei_MethodID;

    ULONG  lei_Flags;
};

struct LHATestResult
{
    ULONG  ltr_StructSize;

    ULONG  ltr_EntriesSeen;
    ULONG  ltr_EntriesTested;
    ULONG  ltr_EntriesFailed;

    LONG   ltr_FirstError;
    STRPTR ltr_FirstErrorPath;
};

#define LHAERR_OK                   0L
#define LHAERR_NO_MEMORY           10L
#define LHAERR_INVALID_ARGUMENT    11L
#define LHAERR_OPEN_FAILED         12L
#define LHAERR_READ_FAILED         13L
#define LHAERR_BAD_ARCHIVE         14L
#define LHAERR_UNSUPPORTED_METHOD  15L
#define LHAERR_CRC_MISMATCH        16L
#define LHAERR_BAD_PATH            17L
#define LHAERR_CREATE_DIR_FAILED   18L
#define LHAERR_WRITE_FAILED        19L
#define LHAERR_ENTRY_NOT_FOUND     20L
#define LHAERR_END_OF_ARCHIVE      21L
#define LHAERR_INTERNAL            22L

#define LHAENTRYTYPE_FILE       1UL
#define LHAENTRYTYPE_DIRECTORY  2UL
#define LHAENTRYTYPE_OTHER      3UL

#define LHAENTRYF_HAS_CRC          (1UL << 0)
#define LHAENTRYF_METHOD_KNOWN     (1UL << 1)
#define LHAENTRYF_PATH_PRESENT     (1UL << 2)
#define LHAENTRYF_PARTIAL_METADATA (1UL << 3)

LONG LHAOpenArchive(STRPTR path, struct LHAArchive **outArchive);
VOID LHACloseArchive(struct LHAArchive *archive);
LONG LHARewind(struct LHAArchive *archive);

LONG LHANextEntry(struct LHAArchive *archive, struct LHAEntry **outEntry);
LONG LHAGetEntryInfo(struct LHAEntry *entry, struct LHAEntryInfo *outInfo);

LONG LHATestArchive(struct LHAArchive *archive, struct LHATestResult *outResult);
LONG LHATestEntry(struct LHAEntry *entry, struct LHATestResult *outResult);

#define LHAEXTRACTF_OVERWRITE   (1UL << 0)
#define LHAEXTRACTF_SKIP_EXIST  (1UL << 1)
#define LHAEXTRACTF_QUIET       (1UL << 2)

LONG LHAExtractArchive(
    struct LHAArchive *archive,
    STRPTR destDir,
    struct LHAExtractOptions *opts);

LONG LHAExtractEntry(
    struct LHAEntry *entry,
    STRPTR destDir,
    struct LHAExtractOptions *opts);

#endif /* LIBRARIES_LHAARCHIVE_H */
