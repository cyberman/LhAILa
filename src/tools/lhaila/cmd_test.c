#include "../../internal/lha_types.h"
#include "../../internal/lha_errors.h"

LONG LHAOpenArchive(STRPTR path, struct LHAArchive **outArchive);
VOID LHACloseArchive(struct LHAArchive *archive);
LONG LHATestArchive(struct LHAArchive *archive, struct LHATestResult *outResult);

VOID lhaila_print_error(LONG rc);
VOID lhaila_print_test_result(struct LHATestResult *res);

LONG lhaila_cmd_test(STRPTR archivePath)
{
    struct LHAArchive *arc;
    struct LHATestResult result;
    LONG rc;

    if (archivePath == NULL)
        return LHAERR_INVALID_ARGUMENT;

    arc = NULL;

    rc = LHAOpenArchive(archivePath, &arc);
    if (rc != LHAERR_OK)
    {
        lhaila_print_error(rc);
        return rc;
    }

    result.ltr_StructSize = sizeof(result);
    result.ltr_EntriesSeen = 0UL;
    result.ltr_EntriesTested = 0UL;
    result.ltr_EntriesFailed = 0UL;
    result.ltr_FirstError = LHAERR_OK;
    result.ltr_FirstErrorPath = NULL;

    rc = LHATestArchive(arc, &result);
    lhaila_print_test_result(&result);

    if (rc != LHAERR_OK)
        lhaila_print_error(rc);

    LHACloseArchive(arc);
    return rc;
}
