/* Stage items (bananas, coins and so on): the item pool, per-type function tables and the
 * item shadow renderer. Item types themselves live in item_coin.c and the pilot/dogfight RELs. */
#include "types.h"
#include "course.h"
#include "dip.h"
#include "item.h"
#include "libc.h"
#include "macros.h"
#include "light.h"
#include "mathutil.h"
#include "mode.h"
#include "pool.h"
#include "shadow.h"
#include "stage.h"
#include "stcoli.h"
#include "pc_hfr.h"

extern u32 monkey_flags;
extern struct
{
    u8 filler0[0x8];
    u32 unk8;
} perfInfo;

u32 lbl_1_data_375C8[0x5C] __attribute__((aligned(8))) =
{
    0xFE000102, 0x03040506, 0x0708090A, 0x0B0CFE0D, 0x0E0FFF00, 0x00010203,
    0x04050607, 0x08090A0B, 0x0C0D0E0F, 0xFF000000, 0x31000000, 0x00000000,
    0x3800B400, 0x00010000, 0x01000000, 0x00020000, 0x10004C00, 0x00030000,
    0x01000000, 0x00040000, 0x18000000, 0x00090000, 0x40000000, 0x00000000,
    0x01000000, 0x000B0000, 0x18000000, 0x000C0000, 0x18000000, 0x00050000,
    0x40000000, 0x00000000, 0x01000000, 0x00070000, 0x18000000, 0x00080000,
    0x3800B400, 0x000D0000, 0x40000000, 0x00000000, 0x18000000, 0x00140000,
    0x40000000, 0x00000000, 0x01000000, 0x00160000, 0x18000000, 0x00170000,
    0x18000000, 0x00180000, 0x18000000, 0x000F0000, 0x40000000, 0x00000000,
    0x01000000, 0x00110000, 0x18000000, 0x00120000, 0x18000000, 0x00130000,
    0x18000000, 0x00000000, 0x18000000, 0x00010000, 0x18000000, 0x00020000,
    0x18000000, 0x00030000, 0x18000000, 0x00040000, 0x18000000, 0x00050000,
    0x18000000, 0x00060000, 0x18000000, 0x00070000, 0x18000000, 0x00080000,
    0x18000000, 0x00090000, 0x18000000, 0x000A0000, 0x18000000, 0x000B0000,
    0x18000000, 0x000C0000, 0x18000000, 0x000D0000, 0x18000000, 0x000E0000,
    0x18000000, 0x000F0000,
};

char *item_names[] =
{
    "IT_COIN",
    "IT_FGT_BANANA",
    "IT_MINI_RACE",
    "IT_FGT_POWER",
    "IT_FGT_PUNCH",
    "IT_PILOT",
    "IT_DOGFIGHT",
    "IT_TMP001",
    "IT_TMP002",
    "IT_TMP003",
    "IT_TMP004",
    "IT_TMP005",
    "IT_TMP006",
    "IT_TMP007",
    "IT_TMP008",
    "IT_TMP009",
    "IT_TMP010",
    "IT_TMP011",
    "IT_TMP012",
    "IT_TMP013",
    "IT_TMP014",
    "IT_TMP015",
    "IT_TMP016",
};

void (*item_init_funcs[])(struct Item *) =
{
    item_coin_init,
    item_default_init,
    item_default_init,
    item_default_init,
    item_default_init,
    item_pilot_init,
    item_dogfight_init,
    item_default_init,
    item_default_init,
    item_default_init,
    item_default_init,
    item_default_init,
    item_default_init,
    item_default_init,
    item_default_init,
    item_default_init,
    item_default_init,
    item_default_init,
    item_default_init,
    item_default_init,
    item_default_init,
    item_default_init,
    item_default_init,
    NULL,
};

void (*item_tick_funcs[])(struct Item *) =
{
    item_coin_tick,
    item_default_tick,
    item_default_tick,
    item_default_tick,
    item_default_tick,
    item_pilot_tick,
    item_dogfight_tick,
    item_default_tick,
    item_default_tick,
    item_default_tick,
    item_default_tick,
    item_default_tick,
    item_default_tick,
    item_default_tick,
    item_default_tick,
    item_default_tick,
    item_default_tick,
    item_default_tick,
    item_default_tick,
    item_default_tick,
    item_default_tick,
    item_default_tick,
    item_default_tick,
    NULL,
};

void (*item_disp_funcs[])(struct Item *) =
{
    item_coin_disp,
    item_default_disp,
    item_default_disp,
    item_default_disp,
    item_default_disp,
    item_pilot_disp,
    item_dogfight_disp,
    item_default_disp,
    item_default_disp,
    item_default_disp,
    item_default_disp,
    item_default_disp,
    item_default_disp,
    item_default_disp,
    item_default_disp,
    item_default_disp,
    item_default_disp,
    item_default_disp,
    item_default_disp,
    item_default_disp,
    item_default_disp,
    item_default_disp,
    item_default_disp,
    NULL,
};

void (*item_coli_funcs[])(struct Item *, struct PhysicsBall *) =
{
    item_coin_coli,
    item_default_coli,
    item_default_coli,
    item_default_coli,
    item_default_coli,
    item_pilot_coli,
    item_dogfight_coli,
    item_default_coli,
    item_default_coli,
    item_default_coli,
    item_default_coli,
    item_default_coli,
    item_default_coli,
    item_default_coli,
    item_default_coli,
    item_default_coli,
    item_default_coli,
    item_default_coli,
    item_default_coli,
    item_default_coli,
    item_default_coli,
    item_default_coli,
    item_default_coli,
    NULL,
};

void (*item_dest_funcs[])(struct Item *) =
{
    item_coin_dest,
    item_default_dest,
    item_default_dest,
    item_default_dest,
    item_default_dest,
    item_pilot_dest,
    item_dogfight_dest,
    item_default_dest,
    item_default_dest,
    item_default_dest,
    item_default_dest,
    item_default_dest,
    item_default_dest,
    item_default_dest,
    item_default_dest,
    item_default_dest,
    item_default_dest,
    item_default_dest,
    item_default_dest,
    item_default_dest,
    item_default_dest,
    item_default_dest,
    item_default_dest,
    NULL,
};

void (*item_replay_init_funcs[])(struct Item *) =
{
    item_coin_replay_init,
    item_default_replay_init,
    item_default_replay_init,
    item_default_replay_init,
    item_default_replay_init,
    item_pilot_replay_init,
    item_dogfight_replay_init,
    item_default_replay_init,
    item_default_replay_init,
    item_default_replay_init,
    item_default_replay_init,
    item_default_replay_init,
    item_default_replay_init,
    item_default_replay_init,
    item_default_replay_init,
    item_default_replay_init,
    item_default_replay_init,
    item_default_replay_init,
    item_default_replay_init,
    item_default_replay_init,
    item_default_replay_init,
    item_default_replay_init,
    item_default_replay_init,
    NULL,
};

void (*item_debug_funcs[])(struct Item *) =
{
    item_debug_coin,
    item_debug_default,
    item_debug_default,
    item_debug_default,
    item_debug_default,
    item_debug_pilot,
    item_debug_dogfight,
    item_debug_default,
    item_debug_default,
    item_debug_default,
    item_debug_default,
    item_debug_default,
    item_debug_default,
    item_debug_default,
    item_debug_default,
    item_debug_default,
    item_debug_default,
    item_debug_default,
    item_debug_default,
    item_debug_default,
    item_debug_default,
    item_debug_default,
    item_debug_default,
    NULL,
};

/* Debug display strings (unused here, kept for the item debug functions). */
char string_STATUS___s_n[] = "STATUS: %s\n";
u8 lbl_1_data_37B60[] =
{
    0x16, 0x16, 0x16, 0x16,
    0x16, 0x16, 0x16, 0x16,
    0x16, 0x16, 0x16, 0x16,
    0x16, 0x16, 0x16, 0x16,
    0x16, 0x16, 0x16, 0x16,
    0x16, 0x16, 0x16, 0x0A,
    0x00, 0x00, 0x00, 0x00,
};
char string_TYPE___s_n[]          = "TYPE: %s\n";
char string_MODEL__[]             = "MODEL: ";
char string__n[]                  = "\n";
char string_POS__X__7_3f_n[]      = "POS: X,%7.3f\n";
char string______Y__7_3f_n[]      = "     Y,%7.3f\n";
char string______Z__7_3f_n[]      = "     Z,%7.3f\n";
char string_SPD__X__7_3f_n[]      = "SPD: X,%7.3f\n";
char string_ROT__X_0x_04X_n[]     = "ROT: X,0x%04X\n";
char string______Y_0x_04X_n[]     = "     Y,0x%04X\n";
char string______Z_0x_04X_n[]     = "     Z,0x%04X\n";
char string_ROTSPD__X_0x_04X_n[]  = "ROTSPD: X,0x%04X\n";
char string_________Y_0x_04X_n[]  = "        Y,0x%04X\n";
char string_________Z_0x_04X_n[]  = "        Z,0x%04X\n";
char string_COLI_RAD___7_3f_n[]   = "COLI RAD: %7.3f\n";
char string_Captured_Time___d_n[] = "Captured Time: %d\n";
char string_Flag__0x_08X_n[]      = "Flag: 0x%08X\n";

struct ItemFuncs item_default_funcs =
{
    item_default_init,
    item_default_tick,
    item_default_disp,
    item_default_coli,
    item_default_dest,
    item_default_replay_init,
    item_debug_default,
};

/* .bss */
s16 g_next_item_id;
struct Item items[256];
void (*g_item_tick_callback)(void);

void event_item_init(void)
{
    int i;
    struct Item *item;

    g_next_item_id = 0;
    memset(items, 0, sizeof(items));
    item = items;
    for (i = 0; i < 256; i++, item++)
    {
        item->id = i;
        item->unk2 = -1;
    }
    g_item_tick_callback = NULL;
    pool_clear(&g_poolInfo.itemPool);
    switch (modeCtrl.gameType)
    {
    case GAMETYPE_MINI_FIGHT:
        if (is_bonus_stage(current_stage_id))
            init_coli_header_items(stagedef->collHeaders, stagedef->itemGroupCount);
        break;
    case GAMETYPE_MAIN_COMPETITION:
        if (is_bonus_stage(current_stage_id)
         || main_mode == MD_SEL
         || (modeCtrl.courseFlags & (1 << 12)))
            init_coli_header_items(stagedef->collHeaders, stagedef->itemGroupCount);
        break;
    default:
        init_coli_header_items(stagedef->collHeaders, stagedef->itemGroupCount);
        break;
    }
}

void event_item_tick(void)
{
    int count;
    struct Item *item;
    s8 *status;
    int onStage;
    Vec pos;
    Vec unused;

    if (g_some_other_flags & 0xA)
        return;
    if (g_item_tick_callback != NULL)
        g_item_tick_callback();
    status = g_poolInfo.itemPool.statusList;
    item = items;
    for (count = g_poolInfo.itemPool.count; count > 0; count--, status++, item++)
    {
        if (*status == 0)
            continue;
        if (item->unkC != 0)
            item->unkC--;
        if (item->unk12 > 0)
            item->unk12--;
        if (item->unk12 == 0 || *status == 3)
        {
            item_dest_funcs[item->type](item);
            *status = 0;
            continue;
        }
        item_tick_funcs[item->type](item);
        if ((item->flags & ITEM_FLAG_4)
         || ((item->flags & ITEM_FLAG_SHADOW) && !(item->flags & ITEM_FLAG_INVISIBLE)))
        {
            if (item->animGroupId != 0)
            {
                mtxa_from_mtx(itemgroups[item->animGroupId].transform);
                mtxa_tf_point(&item->pos, &pos);
            }
            else
                pos = item->pos;
            item->worldPos = pos;
            if (!(item->flags & ITEM_FLAG_NO_RAYCAST))
            {
                onStage = raycast_stage_down(&pos, &item->hit, &unused);
                if (onStage)
                    item->flags |= ITEM_FLAG_ON_STAGE;
                else
                    item->flags &= ~ITEM_FLAG_ON_STAGE;
            }
            else
            {
                item->hit.pos.x = pos.x;
                item->hit.pos.z = pos.z;
            }
        }
    }
}

void event_item_dest(void)
{
    int count;
    struct Item *item;
    s8 *status;

    status = g_poolInfo.itemPool.statusList;
    item = items;
    for (count = g_poolInfo.itemPool.count; count > 0; count--, status++, item++)
    {
        if (*status != 0)
        {
            item_dest_funcs[item->type](item);
            *status = 0;
        }
    }
    g_item_tick_callback = NULL;
}

void draw_items(void)
{
    Mtx viewFromWorld;
    int count;
    struct Item *item;
    s8 *status;
    int animGrpId;

    if (!(monkey_flags & 4))
        return;
    if (dip_switches & 0x80000000)
        return;
    perf_init_timer(8);
    animGrpId = -1;
    mtx_copy(mtxa->mtxB, viewFromWorld);
    status = g_poolInfo.itemPool.statusList;
    item = items;
    for (count = g_poolInfo.itemPool.count; count > 0; count--, status++, item++)
    {
        if (*status != 0 && !(item->flags & ITEM_FLAG_INVISIBLE))
        {
            if (animGrpId != item->animGroupId)
            {
                mtxa_from_mtx(viewFromWorld);
                mtxa_mult_right(itemgroups[item->animGroupId].transform);
                mtxa_to_mtx(mtxa->mtxB);
                animGrpId = item->animGroupId;
            }
            HFR_TAG(HFR_K_ITEM, item->id);
            item_disp_funcs[item->type](item);
            HFR_UNTAG();
        }
    }
    mtx_copy(viewFromWorld, mtxa->mtxB);
    perfInfo.unk8 += perf_stop_timer(8);
}

/* Copies src into a free pool slot and initialises it. Returns the item's spawn counter. */
int spawn_item(struct Item *src)
{
    struct Item *item;
    int idx = pool_alloc(&g_poolInfo.itemPool, 1);

    if (idx < 0)
        return -1;
    item = &items[idx];
    memcpy_handler(item, src, sizeof(struct Item));
    item->id = idx;
    item->unk5E = -1;
    item_init_funcs[item->type](item);
    item->unkC = 0;
    if (item->unk18 <= 0.0)
        item->unk18 = 1.0f;
    item->prevPos = item->pos;
    item->coliFunc = item_coli_funcs[item->type];
    item->unk2 = g_next_item_id;
    g_next_item_id++;
    if (g_next_item_id < 0)
        g_next_item_id = 0;
    if (item->flags & ITEM_FLAG_SHADOW)
    {
        item->unk64 = 0;
        item->unk88 = 0.0f;
    }
    return item->unk2;
}

void draw_item_shadows(void)
{
    int count;
    struct Item *item;
    s8 *status;
    int animGrpId = 0;
    float f2;
    struct ShadowReq req;
    struct RaycastHit hit;
    Vec pos;

    status = g_poolInfo.itemPool.statusList;
    item = items;
    for (count = g_poolInfo.itemPool.count; count > 0; count--, status++, item++)
    {
        float f1;
        if (*status == 0
         || !(item->flags & ITEM_FLAG_SHADOW)
         || (item->flags & ITEM_FLAG_INVISIBLE))
            continue;

        if (item->flags & ITEM_FLAG_ON_STAGE)
        {
            item->unk64 = 1;
            item->unk88 += 0.15f * (1.0f - item->unk88);
        }
        else
        {
            if (item->unk64 == 0)
                continue;
            item->unk88 -= 0.033333333333333333;
            if (item->unk88 <= 0.0f)
            {
                item->unk64 = 0;
                item->unk88 = 0.0f;
                continue;
            }
        }

        pos = item->worldPos;
        hit = item->hit;
        if (item->animGroupId != 0 && animGrpId != item->animGroupId)
        {
            animGrpId = item->animGroupId;
            mtxa_from_mtx(itemgroups[animGrpId].transform);
        }

        if (!(item->flags & ITEM_FLAG_ON_STAGE))
        {
            req.rot = item->unk6C;
            req.pos.x = pos.x;
            req.pos.y = pos.y + item->unk74;
            req.pos.z = pos.z;
        }
        else
        {
            vec_to_euler(&hit.normal, &req.rot);
            item->unk6C.x = req.rot.x;
            item->unk6C.y = req.rot.y;
            req.rot.z = item->unk6C.z;
            req.pos = hit.pos;
            item->unk74 = req.pos.y - pos.y;
        }
        if (modeCtrl.unk30 >= 2)
            continue;
        req.scale = item->unk7C;
        req.color = item->shadowColor;
        f1 = item->unk88;
        req.color.r *= f1;
        req.color.g *= f1;
        req.color.b *= f1;
        f2 = -0.0625f * item->unk74;
        if (f2 > 1.0f)
            continue;
        f1 = 1.0f + 3.0f * f2;
        req.scale.x *= f1;
        req.scale.y *= f1;
        req.scale.z *= f1;
        if (f2 > 0.5f)
        {
            f1 = 1.0f - 2.0f * (f2 - 0.5f);
            req.color.r *= f1;
            req.color.g *= f1;
            req.color.b *= f1;
        }
        req.model = item->shadowModel;
        req.unk20 = 0.075f;
        req.unk24 = 0.1f;
        submit_shadow_req(&req);
    }
}

/* Re-runs the replay init function of every item spawned up to the given frame. */
void item_replay_init_all(int frame)
{
    int count;
    int id;
    struct Item *item;
    s8 *status;

    if (g_some_other_flags & 0xA)
        return;
    status = g_poolInfo.itemPool.statusList;
    item = items;
    for (count = g_poolInfo.itemPool.count; count > 0; count--, status++, item++)
    {
        if (*status != 0 && item->unk5E >= 0 && item->unk5E <= frame)
        {
            id = item->id;
            item_replay_init_funcs[item->type](item);
            item->id = id;
        }
    }
}

/* Spawns the bananas of every item group's collision header. */
void init_coli_header_items(struct StageCollHeader *coliHeaders, int count)
{
    struct Item item;
    int i;
    int j;

    memset(&item, 0, sizeof(item));
    item.type = IT_COIN;
    for (i = 0; i < count; i++, coliHeaders++)
    {
        struct StageBanana *banana = coliHeaders->bananas;

        for (j = 0; j < coliHeaders->bananaCount; j++, banana++)
        {
            item.pos = banana->pos;
            item.subType = banana->type;
            item.animGroupId = i;
            item.stageBanana = banana;
            spawn_item(&item);
        }
    }
}

void item_replace_type_funcs(int itemType, struct ItemFuncs *newFuncs)
{
    struct ItemFuncs funcs;

    if (newFuncs == NULL)
        funcs = item_default_funcs;
    else
    {
        funcs = *newFuncs;
        if (funcs.init == NULL)
            funcs.init = item_default_funcs.init;
        if (funcs.tick == NULL)
            funcs.tick = item_default_funcs.tick;
        if (funcs.disp == NULL)
            funcs.disp = item_default_funcs.disp;
        if (funcs.coli == NULL)
            funcs.coli = item_default_funcs.coli;
        if (funcs.dest == NULL)
            funcs.dest = item_default_funcs.dest;
        if (funcs.replayInit == NULL)
            funcs.replayInit = item_default_funcs.replayInit;
        if (funcs.debug == NULL)
            funcs.debug = item_default_funcs.debug;
    }
    item_init_funcs[itemType] = funcs.init;
    item_tick_funcs[itemType] = funcs.tick;
    item_disp_funcs[itemType] = funcs.disp;
    item_coli_funcs[itemType] = funcs.coli;
    item_dest_funcs[itemType] = funcs.dest;
    item_replay_init_funcs[itemType] = funcs.replayInit;
    item_debug_funcs[itemType] = funcs.debug;
}

void item_default_init(struct Item *item) {}

void item_default_tick(struct Item *item) {}

void item_default_disp(struct Item *item) {}

void item_default_coli(struct Item *item, struct PhysicsBall *ball) {}

void item_default_dest(struct Item *item) {}

void item_default_replay_init(struct Item *item) {}

void item_debug_default(struct Item *item) {}
