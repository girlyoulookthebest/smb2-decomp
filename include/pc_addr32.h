#ifndef PC_ADDR32_H
#define PC_ADDR32_H

/* Address words in u32 data tables (PC port support; see
 * pc_port/docs/x64.md).
 *
 * On GameCube and the 32-bit PC build PC_A32(x) is just (u32)(x).  On the
 * 64-bit PC build a pointer->u32 cast is not a constant initializer, so the
 * word is left 0 and a generated fixup table stores the (below-4GiB)
 * address at startup. */
#include "types.h"

#if defined(__MWERKS__) || !defined(__SIZEOF_POINTER__) || __SIZEOF_POINTER__ == 4
#define PC_A32(x) ((u32)(x))
#define PC_ADDR32_FIXUPS 0
#else
#define PC_A32(x) 0u
#define PC_ADDR32_FIXUPS 1

struct PcAddr32Fixup
{
    u32 *slot;
    const void *target;
};

/* Stores each target's address into its slot; aborts if one does not fit
 * in 32 bits.  Defined in pc_port/src/game/shim/gc_mem.c. */
void pc_apply_addr32_fixups(const struct PcAddr32Fixup *f, int n, const char *what);
#endif

#endif
