#ifndef LHAILA_INTERNAL_LHA_ARCHIVE_CORE_H
#define LHAILA_INTERNAL_LHA_ARCHIVE_CORE_H

#ifndef LHAILA_INTERNAL_LHA_TYPES_H
#include "lha_types.h"
#endif

LONG lha_core_open_archive(STRPTR path, struct LHAArchive **outArc);
VOID lha_core_close_archive(struct LHAArchive *arc);
LONG lha_core_rewind_archive(struct LHAArchive *arc);

#endif /* LHAILA_INTERNAL_LHA_ARCHIVE_CORE_H */

