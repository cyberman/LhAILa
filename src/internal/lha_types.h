#ifndef LHAILA_INTERNAL_LHA_TYPES_H
#define LHAILA_INTERNAL_LHA_TYPES_H

/*
 * LhAILa
 * Internal core types
 *
 * Private header.
 * Not part of public API.
 */

#ifndef EXEC_TYPES_H
#include <exec/types.h>
#endif

#ifndef DOS_DOS_H
#include <dos/dos.h>
#endif

/*
 * Internal entry types
 */
#define LHA_ENTRY_TYPE_FILE       1UL
#define LHA_ENTRY_TYPE_DIRECTORY  2UL
#define LHA_ENTRY_TYPE_OTHER      3UL

/*
 * Internal parsed entry flags
 */
#define LHA_ENTRY_FLAG_HAS_CRC          (1UL << 0)
#define LHA_ENTRY_FLAG_METHOD_KNOWN     (1UL << 1)
#define LHA_ENTRY_FLAG_PATH_PRESENT     (1UL << 2)
#define LHA_ENTRY_FLAG_PARTIAL_METADATA (1UL << 3)

/*
 * Internal method flags
 */
#define LHA_METHOD_FLAG_SUPPORTED  (1UL << 0)
#define LHA_METHOD_FLAG_CORE       (1UL << 1)

/*
 * Forward declarations
 */
struct LHAArchive;
struct LHASink;
struct LHADecodeContext;

/*
 * Normalized internal entry representation.
 * This is the parser output consumed by profile/method/sink orchestration.
 */
struct LHAParsedEntry
{
    ULONG  lpe_HeaderLevel;
    UWORD  lpe_MethodID;
    ULONG  lpe_Type;

    ULONG  lpe_HeaderOffset;
    ULONG  lpe_DataOffset;

    ULONG  lpe_PackedSize;
    ULONG  lpe_UnpackedSize;

    UWORD  lpe_CRC;

    ULONG  lpe_TimestampDays;
    ULONG  lpe_TimestampMins;
    ULONG  lpe_TimestampTicks;

    STRPTR lpe_Path;
    ULONG  lpe_Flags;
};

/*
 * Decoder method operations.
 */
struct LHAMethodOps
{
    UWORD lmo_MethodID;
    ULONG lmo_Flags;

    LONG (*lmo_Decode)(struct LHADecodeContext *ctx);
};

/*
 * Decoded output sink.
 */
struct LHASink
{
    APTR lhs_UserData;

    LONG (*lhs_Begin)(struct LHASink *sink, struct LHAParsedEntry *entry);
    LONG (*lhs_Write)(struct LHASink *sink, CONST_APTR data, ULONG size);
    LONG (*lhs_End)(struct LHASink *sink);
    VOID (*lhs_Abort)(struct LHASink *sink);

    ULONG lhs_Flags;
};

/*
 * Short-lived decode context.
 */
struct LHADecodeContext
{
    struct LHAArchive     *ldc_Archive;
    struct LHAParsedEntry *ldc_Entry;
    struct LHASink        *ldc_Sink;

    ULONG                  ldc_SourceLeft;
    ULONG                  ldc_OutputLeft;

    UWORD                  ldc_RunningCRC;

    UBYTE                 *ldc_IOBuffer;
    ULONG                  ldc_IOBufferSize;

    UBYTE                 *ldc_MethodBuffer;
    ULONG                  ldc_MethodBufferSize;

    LONG                   ldc_LastError;
};

/*
 * Archive context.
 * Master owner of long-lived archive processing resources.
 */
struct LHAArchive
{
    BPTR                 lhaa_File;
    STRPTR               lhaa_Path;

    ULONG                lhaa_FileSize;
    ULONG                lhaa_FilePos;

    ULONG                lhaa_StateFlags;
    LONG                 lhaa_LastError;
    LONG                 lhaa_LastDOSIoErr;

    struct LHAParsedEntry lhaa_CurrentEntry;
    BOOL                  lhaa_HaveCurrentEntry;

    UBYTE               *lhaa_IOBuffer;
    ULONG                lhaa_IOBufferSize;

    UBYTE               *lhaa_MethodBuffer;
    ULONG                lhaa_MethodBufferSize;

    STRPTR               lhaa_PathScratch;
    ULONG                lhaa_PathScratchSize;

    STRPTR               lhaa_ErrorPathScratch;
    ULONG                lhaa_ErrorPathScratchSize;
};

#endif /* LHAILA_INTERNAL_LHA_TYPES_H */

