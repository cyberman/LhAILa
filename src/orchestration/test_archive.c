#include "../internal/lha_types.h"
#include "../internal/lha_errors.h"
#include "../internal/lha_parse.h"
#include "../internal/lha_sinks.h"
#include "../internal/lha_orchestration.h"
#include "../internal/lha_sink_state.h"
#include "../internal/lha_archive_core.h"
#include "../../include/libraries/lhaarchive.h"

/*
 * Minimal private test sink state for archive/entry test flows.
 * Must match the sink_test.c expectation for lhs_UserData.
 */
struct LHATestSinkState
{
    struct LHASinkStateCommon lhts_Common;
};

static VOID lha_test_result_init(struct LHATestResult *outResult)
{
    if (outResult == NULL)
        return;

    outResult->ltr_StructSize = sizeof(struct LHATestResult);
    outResult->ltr_EntriesSeen = 0UL;
    outResult->ltr_EntriesTested = 0UL;
    outResult->ltr_EntriesFailed = 0UL;
    outResult->ltr_FirstError = LHAERR_OK;
    outResult->ltr_FirstErrorPath = NULL;
}

static LONG lha_build_test_sink(
    struct LHASink *outSink,
    struct LHATestSinkState *outState)
{
    if ((outSink == NULL) || (outState == NULL))
        return LHAERR_INVALID_ARGUMENT;

    outState->lhts_Common.lhsc_BytesSeen = 0UL;
    outState->lhts_Common.lhsc_RunningCRC = 0U;

    outSink->lhs_UserData = (APTR)outState;
    outSink->lhs_Begin = lha_test_sink_begin;
    outSink->lhs_Write = lha_test_sink_write;
    outSink->lhs_End = lha_test_sink_end;
    outSink->lhs_Abort = lha_test_sink_abort;
    outSink->lhs_Flags = 0UL;

    return LHAERR_OK;
}

LONG lha_test_single_entry(
    struct LHAArchive *arc,
    struct LHAParsedEntry *entry,
    struct LHATestResult *outResult)
{
    struct LHASink sink;
    struct LHATestSinkState sink_state;
    LONG rc;

    if ((arc == NULL) || (entry == NULL) || (outResult == NULL))
        return LHAERR_INVALID_ARGUMENT;

    lha_test_result_init(outResult);

    outResult->ltr_EntriesSeen = 1UL;

    rc = lha_build_test_sink(&sink, &sink_state);
    if (rc != LHAERR_OK)
    {
        outResult->ltr_EntriesFailed = 1UL;
        outResult->ltr_FirstError = rc;
        outResult->ltr_FirstErrorPath = entry->lpe_Path;
        return rc;
    }

    rc = lha_process_entry(arc, entry, &sink);
    if (rc == LHAERR_KNOWN_OUTSIDE)
    {
        outResult->ltr_EntriesFailed = 1UL;
        outResult->ltr_FirstError = rc;
        outResult->ltr_FirstErrorPath = entry->lpe_Path;
        return rc;
    }

    if (rc != LHAERR_OK)
    {
        outResult->ltr_EntriesFailed = 1UL;
        outResult->ltr_FirstError = rc;
        outResult->ltr_FirstErrorPath = entry->lpe_Path;
        return rc;
    }

    outResult->ltr_EntriesTested = 1UL;
    outResult->ltr_FirstError = LHAERR_OK;
    outResult->ltr_FirstErrorPath = NULL;

    return LHAERR_OK;
}

LONG lha_test_whole_archive(struct LHAArchive *arc, struct LHATestResult *outResult)
{
    struct LHAParsedEntry entry;
    struct LHASink sink;
    struct LHATestSinkState sink_state;
    LONG rc;

    if ((arc == NULL) || (outResult == NULL))
        return LHAERR_INVALID_ARGUMENT;

    lha_test_result_init(outResult);

    rc = lha_core_rewind_archive(arc);
    if (rc != LHAERR_OK)
    {
        outResult->ltr_FirstError = rc;
        return rc;
    }

    rc = lha_build_test_sink(&sink, &sink_state);
    if (rc != LHAERR_OK)
    {
        outResult->ltr_FirstError = rc;
        return rc;
    }

    for (;;)
    {
        rc = lha_core_next_entry(arc, &entry);
        if (rc == LHAERR_END_OF_ARCHIVE)
            break;

        /*
         * Known related but outside-profile entries are not treated as structurally
         * corrupt here, even though 0.1 still returns failure for the archive test.
         */
        if (rc == LHAERR_KNOWN_OUTSIDE)
        {
            outResult->ltr_EntriesFailed++;
            outResult->ltr_FirstError = rc;
            outResult->ltr_FirstErrorPath = NULL;
            return rc;
        }

        if (rc != LHAERR_OK)
        {
            outResult->ltr_EntriesFailed++;
            outResult->ltr_FirstError = rc;
            outResult->ltr_FirstErrorPath = NULL;
            return rc;
        }

        outResult->ltr_EntriesSeen++;

        rc = lha_process_entry(arc, &entry, &sink);
        if (rc != LHAERR_OK)
        {
            outResult->ltr_EntriesFailed++;
            outResult->ltr_FirstError = rc;
            outResult->ltr_FirstErrorPath = entry.lpe_Path;
            return rc;
        }

        outResult->ltr_EntriesTested++;
    }

    outResult->ltr_FirstError = LHAERR_OK;
    outResult->ltr_FirstErrorPath = NULL;

    return LHAERR_OK;
}
