#ifndef POOL_H
#define POOL_H

#include "types.h"

struct PoolInfo_sub
{
    s32 capacity;  /* maximum number of objects in the pool */
    s32 nextFree;  /* index to begin the search for free slots */
    s32 count;  /* one past the last non-empty slot */
    s8 *statusList;  /* status of each object in the pool */
};

/* SMB1's PoolInfo with three more pools (the 0x50/0x60 ones are not identified yet). */
struct PoolInfo
{
    /*0x00*/ struct PoolInfo_sub playerPool;
    /*0x10*/ struct PoolInfo_sub itemPool;
    /*0x20*/ struct PoolInfo_sub stobjPool;
    /*0x30*/ struct PoolInfo_sub spritePool;
    /*0x40*/ struct PoolInfo_sub effectPool;
    /*0x50*/ struct PoolInfo_sub unk50Pool;
    /*0x60*/ struct PoolInfo_sub unk60Pool;
    /*0x70*/ struct PoolInfo_sub cameraPool;
    /*0x080*/ s8 playerBuf[4];
    /*0x084*/ s8 itemBuf[256];
    /*0x184*/ s8 stobjBuf[144];
    /*0x214*/ s8 spriteBuf[80];
    /*0x264*/ s8 effectBuf[512];
    /*0x464*/ s8 unk50Buf[16];
    /*0x474*/ s8 unk60Buf[16];
    /*0x484*/ s8 cameraBuf[5];
}; /* size = 0x48C */

extern char *EVENT_STATUS_NAMES[];

extern struct PoolInfo g_poolInfo;

void pool_init(void);
void pool_tick(void);
int pool_alloc(struct PoolInfo_sub *pool, s8 status);
void pool_clear(struct PoolInfo_sub *pool);

#endif
