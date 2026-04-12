#ifndef LHAILA_INTERNAL_LHA_ERRORS_H
#define LHAILA_INTERNAL_LHA_ERRORS_H

/*
 * LhAILa
 * Internal error definitions
 *
 * Private header.
 * Public API may mirror parts of this later, but this file is internal.
 */

/*
 * Stable internal core error family for 0.1
 */
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

/*
 * Small helpers.
 * Keep these simple and side-effect free.
 */
#define LHA_SUCCESS(rc) ((rc) == LHAERR_OK)
#define LHA_FAILURE(rc) ((rc) != LHAERR_OK)

/*
 * Optional internal detail reasons for debugging / future RC2 mapping.
 * Keep distinct from public-facing core rc values.
 */
#define LHADETAIL_NONE                    0L
#define LHADETAIL_DOS_READ_EOF           100L
#define LHADETAIL_DOS_SEEK_FAILED        101L
#define LHADETAIL_BAD_HEADER_LEVEL       102L
#define LHADETAIL_BAD_METHOD_TAG         103L
#define LHADETAIL_TRUNCATED_HEADER       104L
#define LHADETAIL_TRUNCATED_DATA         105L
#define LHADETAIL_BAD_EXT_HEADER_CHAIN   106L
#define LHADETAIL_BAD_PACKED_SIZE        107L
#define LHADETAIL_BAD_UNPACKED_SIZE      108L
#define LHADETAIL_PATH_ESCAPE            109L
#define LHADETAIL_SINK_ABORTED           110L

#endif /* LHAILA_INTERNAL_LHA_ERRORS_H */

