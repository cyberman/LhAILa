#include <exec/types.h>
#include <proto/dos.h>

#include "../../internal/lha_errors.h"

LONG lhaila_cmd_list(STRPTR archivePath);
LONG lhaila_cmd_test(STRPTR archivePath);
LONG lhaila_cmd_extract(STRPTR archivePath, STRPTR destDir);
VOID lhaila_print_error(LONG rc);

static int lhaila_ascii_tolower(int c)
{
    if ((c >= 'A') && (c <= 'Z'))
        return c + ('a' - 'A');

    return c;
}

static int lhaila_stricmp(STRPTR a, STRPTR b)
{
    int ca;
    int cb;

    if ((a == NULL) || (b == NULL))
        return 1;

    for (;;)
    {
        ca = lhaila_ascii_tolower((unsigned char)*a);
        cb = lhaila_ascii_tolower((unsigned char)*b);

        if (ca != cb)
            return ca - cb;

        if (ca == 0)
            return 0;

        a++;
        b++;
    }
}

static VOID lhaila_print_usage(VOID)
{
    PutStr("Usage:\n");
    PutStr("  lhaila list <archive>\n");
    PutStr("  lhaila test <archive>\n");
    PutStr("  lhaila extract <archive> <destdir>\n");
}

int main(int argc, char **argv)
{
    LONG rc;

    if (argc < 3)
    {
        lhaila_print_usage();
        return (int)LHAERR_INVALID_ARGUMENT;
    }

    if (argv[1] == NULL)
    {
        lhaila_print_usage();
        return (int)LHAERR_INVALID_ARGUMENT;
    }

    if (lhaila_stricmp((STRPTR)argv[1], "list") == 0)
    {
        rc = lhaila_cmd_list((STRPTR)argv[2]);
        return (int)rc;
    }

    if (lhaila_stricmp((STRPTR)argv[1], "test") == 0)
    {
        rc = lhaila_cmd_test((STRPTR)argv[2]);
        return (int)rc;
    }

    if (lhaila_stricmp((STRPTR)argv[1], "extract") == 0)
    {
        if (argc < 4)
        {
            lhaila_print_usage();
            return (int)LHAERR_INVALID_ARGUMENT;
        }

        rc = lhaila_cmd_extract((STRPTR)argv[2], (STRPTR)argv[3]);
        return (int)rc;
    }

    PutStr("lhaila: unknown command\n");
    lhaila_print_usage();

    return (int)LHAERR_INVALID_ARGUMENT;
}
