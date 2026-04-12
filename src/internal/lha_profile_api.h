#ifndef LHAILA_INTERNAL_LHA_PROFILE_H
#define LHAILA_INTERNAL_LHA_PROFILE_H

/*
 * LhAILa
 * Internal classic profile definitions
 *
 * Private header.
 */

#ifndef EXEC_TYPES_H
#include <exec/types.h>
#endif

/*
 * Header levels
 */
#define LHA_HEADER_LEVEL_0  0UL
#define LHA_HEADER_LEVEL_1  1UL
#define LHA_HEADER_LEVEL_2  2UL

/*
 * Internal method ids.
 * We store the 5-byte ASCII tag in a compact symbolic form here.
 *
 * The exact numeric values are internal only.
 */
#define LHA_METH_LH0  0x0001
#define LHA_METH_LH1  0x0002
#define LHA_METH_LH2  0x0003
#define LHA_METH_LH3  0x0004
#define LHA_METH_LH4  0x0005
#define LHA_METH_LH5  0x0006
#define LHA_METH_LH6  0x0007
#define LHA_METH_LH7  0x0008
#define LHA_METH_LZS  0x0009
#define LHA_METH_LZ4  0x000A
#define LHA_METH_LZ5  0x000B
#define LHA_METH_UNKNOWN 0xFFFF

/*
 * Profile status
 */
#define LHAPROFILE_MATCH          1UL
#define LHAPROFILE_KNOWN_OUTSIDE  2UL
#define LHAPROFILE_INVALID        3UL

/*
 * Profile reasons
 */
#define LHAPROFILE_REASON_NONE                 0UL
#define LHAPROFILE_REASON_BAD_HEADER_LEVEL     1UL
#define LHAPROFILE_REASON_UNSUPPORTED_METHOD   2UL
#define LHAPROFILE_REASON_BAD_ENTRY_TYPE       3UL
#define LHAPROFILE_REASON_CORRUPT_HEADER       4UL

/*
 * Classic Profile 0.1:
 * supported header levels
 */
#define LHA_PROFILE_HEADER_ALLOWED(level) \
    (((level) == LHA_HEADER_LEVEL_0) || ((level) == LHA_HEADER_LEVEL_1))

/*
 * Classic Profile 0.1:
 * supported methods
 */
#define LHA_PROFILE_METHOD_ALLOWED(method_id)                     \
    (                                                             \
        ((method_id) == LHA_METH_LH0) ||                          \
        ((method_id) == LHA_METH_LH1) ||                          \
        ((method_id) == LHA_METH_LH4) ||                          \
        ((method_id) == LHA_METH_LH5)                             \
    )

/*
 * Related but outside-profile methods.
 * Useful for clean "known outside" classification.
 */
#define LHA_PROFILE_METHOD_KNOWN_OUTSIDE(method_id)               \
    (                                                             \
        ((method_id) == LHA_METH_LH2) ||                          \
        ((method_id) == LHA_METH_LH3) ||                          \
        ((method_id) == LHA_METH_LH6) ||                          \
        ((method_id) == LHA_METH_LH7) ||                          \
        ((method_id) == LHA_METH_LZS) ||                          \
        ((method_id) == LHA_METH_LZ4) ||                          \
        ((method_id) == LHA_METH_LZ5)                             \
    )

#endif /* LHAILA_INTERNAL_LHA_PROFILE_H */

