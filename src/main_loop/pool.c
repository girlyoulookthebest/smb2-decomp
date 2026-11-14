#include "types.h"
#include "macros.h"
#include "dip.h"
#include "event.h"
#include "pool.h"

void pool_update_idxs_of_all_pools(void);
void pool_update_idxs(struct PoolInfo_sub *pool, int eventId);
void g_do_nothing(void);

char *EVENT_STATUS_NAMES[] =
{
    "STAT_NULL",
    "STAT_INIT",
    "STAT_NORMAL",
    "STAT_DEST",
    "STAT_FREEZE",
    "STAT_INIT_FIRST",
    "STAT_GOAL_INIT",
    "STAT_GOAL",
    "STAT_RINGOUT_INIT",
    "STAT_RINGOUT",
    "STAT_MINI_MODE_0_INIT",
    "STAT_MINI_MODE_0",
};

struct PoolInfo g_poolInfo;

void pool_init(void)
{
    g_poolInfo.playerPool.capacity = ARRAY_COUNT(g_poolInfo.playerBuf);
    g_poolInfo.playerPool.statusList = g_poolInfo.playerBuf;

    g_poolInfo.itemPool.capacity = ARRAY_COUNT(g_poolInfo.itemBuf);
    g_poolInfo.itemPool.statusList = g_poolInfo.itemBuf;

    g_poolInfo.stobjPool.capacity = ARRAY_COUNT(g_poolInfo.stobjBuf);
    g_poolInfo.stobjPool.statusList = g_poolInfo.stobjBuf;

    g_poolInfo.spritePool.capacity = ARRAY_COUNT(g_poolInfo.spriteBuf);
    g_poolInfo.spritePool.statusList = g_poolInfo.spriteBuf;

    g_poolInfo.effectPool.capacity = ARRAY_COUNT(g_poolInfo.effectBuf);
    g_poolInfo.effectPool.statusList = g_poolInfo.effectBuf;

    g_poolInfo.unk50Pool.capacity = ARRAY_COUNT(g_poolInfo.unk50Buf);
    g_poolInfo.unk50Pool.statusList = g_poolInfo.unk50Buf;

    g_poolInfo.unk60Pool.capacity = ARRAY_COUNT(g_poolInfo.unk60Buf);
    g_poolInfo.unk60Pool.statusList = g_poolInfo.unk60Buf;

    g_poolInfo.cameraPool.capacity = ARRAY_COUNT(g_poolInfo.cameraBuf);
    g_poolInfo.cameraPool.statusList = g_poolInfo.cameraBuf;

    pool_clear(&g_poolInfo.playerPool);
    pool_clear(&g_poolInfo.itemPool);
    pool_clear(&g_poolInfo.stobjPool);
    pool_clear(&g_poolInfo.spritePool);
    pool_clear(&g_poolInfo.effectPool);
    pool_clear(&g_poolInfo.unk50Pool);
    pool_clear(&g_poolInfo.unk60Pool);
    pool_clear(&g_poolInfo.cameraPool);
}

void pool_tick(void)
{
    pool_update_idxs_of_all_pools();
    g_do_nothing();
}

void pool_update_idxs_of_all_pools(void)
{
    pool_update_idxs(&g_poolInfo.playerPool, -1);
    pool_update_idxs(&g_poolInfo.itemPool, EVENT_ITEM);
    pool_update_idxs(&g_poolInfo.stobjPool, EVENT_STOBJ);
    pool_update_idxs(&g_poolInfo.spritePool, EVENT_SPRITE);
    pool_update_idxs(&g_poolInfo.effectPool, EVENT_EFFECT);
    pool_update_idxs(&g_poolInfo.cameraPool, EVENT_CAMERA);
}

/* Recompute the first free slot and the used length of a pool. Pools of inactive
 * events are treated as empty. */
void pool_update_idxs(struct PoolInfo_sub *pool, int eventId)
{
    int i;
    int firstFree;
    int end;
    s8 *status;

    firstFree = -1;
    end = 0;
    status = pool->statusList;
    if (eventId == -1 || events[eventId].state != EV_STATE_INACTIVE)
    {
        for (i = 0; i < pool->capacity; i++, status++)
        {
            if (*status != 0)
                end = i + 1;
            else if (firstFree == -1)
                firstFree = i;
        }
    }
    if (firstFree == -1)
        firstFree = 0;
    pool->nextFree = firstFree;
    pool->count = end;
}

void g_do_nothing(void)
{
    if (g_some_other_flags & 0xA)
        return;
    else
        return;
}

/* Claim the first free slot at or after nextFree (wrapping around) and mark it with
 * 'status'. Returns the slot index, or -1 if the pool is full. */
int pool_alloc(struct PoolInfo_sub *pool, s8 status)
{
    int i;
    s8 *ptr;

    ptr = pool->statusList + pool->nextFree;
    for (i = pool->nextFree; i < pool->capacity; i++, ptr++)
    {
        if (*ptr == 0)
        {
            if (i + 1 > pool->count)
                pool->count = i + 1;
            pool->nextFree = i + 1;
            *ptr = status;
            return i;
        }
    }

    ptr = pool->statusList;
    for (i = 0; i < pool->nextFree; i++, ptr++)
    {
        if (*ptr == 0)
        {
            pool->nextFree = i + 1;
            *ptr = status;
            return i;
        }
    }

    return -1;
}

void pool_clear(struct PoolInfo_sub *pool)
{
    int i;
    s8 *ptr;

    ptr = pool->statusList;
    for (i = pool->capacity; i > 0; i--, ptr++)
        *ptr = 0;
    pool->nextFree = 0;
    pool->count = 0;
}
