#ifndef FILE_H
#define FILE_H

#include "types.h"

/* A file held in the ARAM file cache. */
struct FileCacheEntry
{
    /*0x00*/ s32 valid;
    /*0x04*/ s32 entryNum;
    /*0x08*/ u32 aramAddr;
    /*0x0C*/ u32 size;
}; /* size = 0x10 */

struct DiscFileInfo
{
    /*0x00*/ u8 filler0[0x34];
    /*0x34*/ u32 length;
    /*0x38*/ u32 unk38;
}; /* size = 0x3C, DVDFileInfo */

/* An open file (DVD or file cache). */
struct File
{
    /*0x00*/ u32 isCached;
    /*0x04*/ struct DiscFileInfo fileInfo;
    /*0x40*/ struct FileCacheEntry cache;
}; /* size = 0x50 */

BOOL file_open(char *filename, struct File *file);
BOOL file_close(struct File *file);
u32 file_size(struct File *file);
s32 file_read(struct File *file, void *dest, u32 size, u32 offset);

/* Round up to a multiple of 32 bytes (the DVD and cache granularity). */
#define OSRoundUp32B(x) (((u32)(x) + 31) & ~31)

#ifdef __MWERKS__
#define LWBRX(ptr, offset) __lwbrx((ptr), (offset))
#else
static inline u32 LWBRX(void *ptr, u32 offset)
{
    u8 *p = (u8 *)ptr + offset;
    return p[0] | (p[1] << 8) | (p[2] << 16) | ((u32)p[3] << 24);
}
#endif

#endif
