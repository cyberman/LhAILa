#ifndef LHAILA_INTERNAL_LHA_IO_H
#define LHAILA_INTERNAL_LHA_IO_H

/*
 * LhAILa
 * Internal archive I/O helpers
 *
 * Private header.
 */

#ifndef EXEC_TYPES_H
#include <exec/types.h>
#endif

#ifndef LHAILA_INTERNAL_LHA_TYPES_H
#include "lha_types.h"
#endif

LONG lha_seek_abs(struct LHAArchive *arc, ULONG offset);
LONG lha_read_exact(struct LHAArchive *arc, APTR buf, ULONG size);
LONG lha_skip_exact(struct LHAArchive *arc, ULONG size);
LONG lha_tell(struct LHAArchive *arc, ULONG *outOffset);

#endif /* LHAILA_INTERNAL_LHA_IO_H */

