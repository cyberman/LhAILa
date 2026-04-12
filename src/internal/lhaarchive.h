struct LHATestResult
{
    ULONG ltr_StructSize;

    ULONG ltr_EntriesSeen;
    ULONG ltr_EntriesTested;
    ULONG ltr_EntriesFailed;

    LONG  ltr_FirstError;
    STRPTR ltr_FirstErrorPath;
};

