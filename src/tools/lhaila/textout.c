#include <exec/types.h>
#include <proto/dos.h>

#include <libraries/lhaarchive.h>
#include "../../internal/lha_errors.h"

static STRPTR lhaila_error_to_string(LONG rc)
{
    switch (rc)
    {
        case LHAERR_OK:
            return "ok";

        case LHAERR_NO_MEMORY:
            return "out of memory";

        case LHAERR_INVALID_ARGUMENT:
            return "invalid argument";

        case LHAERR_OPEN_FAILED:
            return "failed to open archive";

        case LHAERR_READ_FAILED:
            return "read failed";

        case LHAERR_BAD_ARCHIVE:
            return "bad or unsupported archive structure";

        case LHAERR_UNSUPPORTED_METHOD:
            return "unsupported method";

        case LHAERR_CRC_MISMATCH:
            return "crc mismatch";

        case LHAERR_BAD_PATH:
            return "bad path";

        case LHAERR_CREATE_DIR_FAILED:
            return "failed to create directory";

        case LHAERR_WRITE_FAILED:
            return "write failed";

        case LHAERR_ENTRY_NOT_FOUND:
            return "entry not found";

        case LHAERR_END_OF_ARCHIVE:
            return "end of archive";

        case LHAERR_INTERNAL:
            return "internal error";

        case LHAERR_KNOWN_OUTSIDE:
            return "known outside current profile";

        default:
            return "unknown error";
    }
}

VOID lhaila_print_error(LONG rc)
{
    PutStr("lhaila: ");
    PutStr(lhaila_error_to_string(rc));
    PutStr("\n");
}

VOID lhaila_print_test_result(struct LHATestResult *res)
{
    if (res == NULL)
        return;

    PutStr("Test result:\n");

    PutStr("  entries seen: ");
    VPrintf("%ld\n", (LONG *)&res->ltr_EntriesSeen);

    PutStr("  entries tested: ");
    VPrintf("%ld\n", (LONG *)&res->ltr_EntriesTested);

    PutStr("  entries failed: ");
    VPrintf("%ld\n", (LONG *)&res->ltr_EntriesFailed);

    PutStr("  first error: ");
    VPrintf("%ld\n", &res->ltr_FirstError);

    if (res->ltr_FirstErrorPath != NULL)
    {
        PutStr("  first error path: ");
        PutStr(res->ltr_FirstErrorPath);
        PutStr("\n");
    }
}

VOID lhaila_print_entry_info(struct LHAEntryInfo *info)
{
    STRPTR type_str;

    if (info == NULL)
        return;

    switch (info->lei_Type)
    {
        case LHAENTRYTYPE_FILE:
            type_str = "file";
            break;

        case LHAENTRYTYPE_DIRECTORY:
            type_str = "dir";
            break;

        default:
            type_str = "other";
            break;
    }

    PutStr(type_str);
    PutStr("  ");

    VPrintf("%ld", (LONG *)&info->lei_PackedSize);
    PutStr("  ");

    VPrintf("%ld", (LONG *)&info->lei_UnpackedSize);
    PutStr("  ");

    if (info->lei_Path != NULL)
        PutStr(info->lei_Path);
    else
        PutStr("<no-path>");

    PutStr("\n");
}

