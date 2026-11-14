/* Effects: particles, banana pickups, bubbles, meteors and friends. */

#include "types.h"
#include "camera.h"
#include "dip.h"
#include "effect.h"
#include "libc.h"
#include "light.h"
#include "mode.h"
#include "pool.h"
#include "macros.h"
#include "rand.h"
#include "avdisp.h"
#include "ball.h"
#include "frustum.h"
#include "gx.h"
#include "mathutil.h"
#include "nl2ngc.h"
#include "stage.h"
#include "world.h"
#include "bg_work.h"
#include "pc_hfr.h"

struct PointWithColor
{
    Vec pos;
    GXColor color;
};

struct EffectOrdNode
{
    struct OrdTblNode node;
    struct Effect *effect;
};

extern u32 monkey_flags;
extern struct
{
    /*0x00*/ u32 flags;
} g_some_render_flag;
extern struct
{
    u8 filler0[0x10];
    u32 unk10;
} perfInfo;

float fn_1_80D4(Vec *pos);
float fn_1_815C(Vec *pos);
int fn_1_A74E4(Vec *a, Vec *b, float e, Vec *c, Vec *d, float f);
/* Per-background work area, stored 0x9C past the world_theme global
 * (BackgroundInfo.work; the offset is 32-bit only). */
#ifdef __MWERKS__
#define BG_WORK(type) ((type)*(void **)((u8 *)&world_theme + 0x9C))
#else
#define BG_WORK(type) ((type)pc_bg_work())  /* effect.c sees world_theme as s16 */
#endif
void g_smth_with_gpu_and_locked_cache_mtx(void);
void vec_to_euler_xy(Vec *v, s16 *x, s16 *y);
void *ord_tbl_alloc_node(u32 size);
struct OrdTblNode *fn_1_CCC18(float z);
void fn_1_CD418(struct OrdTblNode *list, struct OrdTblNode *node);
void fn_1_E2FD0(int width);
void fn_1_E2FE0(int a, int b, int c, int d);
void fn_1_E349C(struct PointWithColor *a, struct PointWithColor *b);
static void fn_1_6A064(struct OrdTblNode *node_);
static void fn_1_6B3E4(struct OrdTblNode *node_);
void effect_default_init(struct Effect *effect);
void effect_default_tick(struct Effect *effect);
void effect_default_disp(struct Effect *effect);
void effect_default_dest(struct Effect *effect);

void effect_banana_drop_dest(struct Effect *effect);
void effect_banana_drop_disp(struct Effect *effect);
void effect_banana_drop_init(struct Effect *effect);
void effect_banana_drop_tick(struct Effect *effect);
void effect_bgbtmfire_ptcl_dest(struct Effect *effect);
void effect_bgbtmfire_ptcl_disp(struct Effect *effect);
void effect_bgbtmfire_ptcl_init(struct Effect *effect);
void effect_bgbtmfire_ptcl_tick(struct Effect *effect);
void effect_bgend_water_dest(struct Effect *effect);
void effect_bgend_water_disp(struct Effect *effect);
void effect_bgend_water_init(struct Effect *effect);
void effect_bgend_water_tick(struct Effect *effect);
void effect_bgmst_gen_cloud_dest(struct Effect *effect);
void effect_bgmst_gen_cloud_disp(struct Effect *effect);
void effect_bgmst_gen_cloud_init(struct Effect *effect);
void effect_bgmst_gen_cloud_tick(struct Effect *effect);
void effect_bgmst_water_dest(struct Effect *effect);
void effect_bgmst_water_disp(struct Effect *effect);
void effect_bgmst_water_init(struct Effect *effect);
void effect_bgmst_water_tick(struct Effect *effect);
void effect_bgstm_rainripple_dest(struct Effect *effect);
void effect_bgstm_rainripple_disp(struct Effect *effect);
void effect_bgstm_rainripple_init(struct Effect *effect);
void effect_bgstm_rainripple_tick(struct Effect *effect);
void effect_bgwat_bubble_base_dest(struct Effect *effect);
void effect_bgwat_bubble_base_disp(struct Effect *effect);
void effect_bgwat_bubble_base_init(struct Effect *effect);
void effect_bgwat_bubble_base_tick(struct Effect *effect);
void effect_bgwat_bubble_dest(struct Effect *effect);
void effect_bgwat_bubble_disp(struct Effect *effect);
void effect_bgwat_bubble_init(struct Effect *effect);
void effect_bgwat_bubble_tick(struct Effect *effect);
void effect_bns_stg_star_dest(struct Effect *effect);
void effect_bns_stg_star_disp(struct Effect *effect);
void effect_bns_stg_star_init(struct Effect *effect);
void effect_bns_stg_star_tail_dest(struct Effect *effect);
void effect_bns_stg_star_tail_disp(struct Effect *effect);
void effect_bns_stg_star_tail_init(struct Effect *effect);
void effect_bns_stg_star_tail_tick(struct Effect *effect);
void effect_bns_stg_star_tick(struct Effect *effect);
void effect_coliflash_dest(struct Effect *effect);
void effect_coliflash_disp(struct Effect *effect);
void effect_coliflash_init(struct Effect *effect);
void effect_coliflash_tick(struct Effect *effect);
void effect_colistar_particle_dest(struct Effect *effect);
void effect_colistar_particle_disp(struct Effect *effect);
void effect_colistar_particle_init(struct Effect *effect);
void effect_colistar_particle_tick(struct Effect *effect);
void effect_commendfrag_dest(struct Effect *effect);
void effect_commendfrag_disp(struct Effect *effect);
void effect_commendfrag_init(struct Effect *effect);
void effect_commendfrag_tick(struct Effect *effect);
void effect_ending_ballfrag_dest(struct Effect *effect);
void effect_ending_ballfrag_disp(struct Effect *effect);
void effect_ending_ballfrag_init(struct Effect *effect);
void effect_ending_ballfrag_tick(struct Effect *effect);
void effect_get_banana_dest(struct Effect *effect);
void effect_get_banana_disp(struct Effect *effect);
void effect_get_banana_init(struct Effect *effect);
void effect_get_banana_tick(struct Effect *effect);
void effect_get_nameent_code_dest(struct Effect *effect);
void effect_get_nameent_code_disp(struct Effect *effect);
void effect_get_nameent_code_init(struct Effect *effect);
void effect_get_nameent_code_tick(struct Effect *effect);
void effect_holding_banana_dest(struct Effect *effect);
void effect_holding_banana_disp(struct Effect *effect);
void effect_holding_banana_init(struct Effect *effect);
void effect_holding_banana_tick(struct Effect *effect);
void effect_levitate_dest(struct Effect *effect);
void effect_levitate_disp(struct Effect *effect);
void effect_levitate_init(struct Effect *effect);
void effect_levitate_tick(struct Effect *effect);
void effect_meteo_dest(struct Effect *effect);
void effect_meteo_disp(struct Effect *effect);
void effect_meteo_fix_dest(struct Effect *effect);
void effect_meteo_fix_disp(struct Effect *effect);
void effect_meteo_fix_init(struct Effect *effect);
void effect_meteo_fix_tick(struct Effect *effect);
void effect_meteo_init(struct Effect *effect);
void effect_meteo_tick(struct Effect *effect);
void effect_nameent_code_dest(struct Effect *effect);
void effect_nameent_code_disp(struct Effect *effect);
void effect_nameent_code_init(struct Effect *effect);
void effect_nameent_code_tick(struct Effect *effect);
void effect_paperfrag_dest(struct Effect *effect);
void effect_paperfrag_disp(struct Effect *effect);
void effect_paperfrag_init(struct Effect *effect);
void effect_paperfrag_tick(struct Effect *effect);
void effect_particle_dest_coli(struct Effect *effect);
void effect_particle_disp_coli(struct Effect *effect);
void effect_particle_init_coli(struct Effect *effect);
void effect_particle_tick_coli(struct Effect *effect);
void effect_piyo_dest(struct Effect *effect);
void effect_piyo_disp(struct Effect *effect);
void effect_piyo_init(struct Effect *effect);
void effect_piyo_tick(struct Effect *effect);

/* unreferenced by name in this file, but it is the anchor every other file-local static is addressed from */
#pragma force_active on
static u32 lbl_1_data_2F438[] =
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
#pragma force_active reset

char *effectNames[] =
{
    "ET_PAPERFRAG",
    "ET_GET_BANANA",
    "ET_COLI_PARTICLE",
    "ET_HOLDING_BANANA",
    "ET_LEVITATE",
    "ET_COLISTAR_PARTICLE",
    "ET_BGWAT_BUBBLE_BASE",
    "ET_BGWAT_BUBBLE",
    "ET_METEO",
    "ET_METEO_FIX",
    "ET_COLIFLASH",
    "ET_BNS_STG_STAR",
    "ET_BNS_STG_STAR_TAIL",
    "ET_BGMST_GEN_CLOUD",
    "ET_BGSTM_RAINRIPPLE",
    "ET_NAMEENT_CODE",
    "ET_GET_NAMEENT_CODE",
    "ET_BGMST_WATER",
    "ET_COMMENDFRAG",
    "ET_ENDING_BALLFRAG",
    "ET_BANANA_DROP",
    "ET_BGEND_WATER",
    "ET_BGBTMFIRE_PTCL",
    "ET_PIYO",
    "ET_DUMMY1",
    "ET_DUMMY2",
    "ET_DUMMY3",
    "ET_DUMMY4",
    "ET_DUMMY5",
    "ET_DUMMY6",
    "ET_DUMMY7",
    "ET_DUMMY8",
    "ET_DUMMY9",
    "ET_DUMMYA",
    "ET_DUMMYB",
    "ET_DUMMYC",
    "ET_DUMMYD",
    "ET_DUMMYE",
    "ET_DUMMYF",
    "ET_TMP001",
    "ET_TMP002",
    "ET_TMP003",
    "ET_TMP004",
    "ET_TMP005",
    "ET_TMP006",
    "ET_TMP007",
    "ET_TMP008",
    "ET_TMP009",
    "ET_TMP010",
    "ET_TMP011",
    "ET_TMP012",
    "ET_TMP013",
    "ET_TMP014",
    "ET_TMP015",
    "ET_TMP016",
    "ET_TMP017",
    "ET_TMP018",
    "ET_TMP019",
    "ET_TMP020",
    "ET_TMP021",
    "ET_TMP022",
    "ET_TMP023",
    "ET_TMP024",
    "ET_TMP025",
    "ET_TMP026",
    "ET_TMP027",
    "ET_TMP028",
    "ET_TMP029",
    "ET_TMP030",
    "ET_TMP031",
    "ET_TMP032",
};

void (*effect_init_funcs[])(struct Effect *) =
{
    effect_paperfrag_init,
    effect_get_banana_init,
    effect_particle_init_coli,
    effect_holding_banana_init,
    effect_levitate_init,
    effect_colistar_particle_init,
    effect_bgwat_bubble_base_init,
    effect_bgwat_bubble_init,
    effect_meteo_init,
    effect_meteo_fix_init,
    effect_coliflash_init,
    effect_bns_stg_star_init,
    effect_bns_stg_star_tail_init,
    effect_bgmst_gen_cloud_init,
    effect_bgstm_rainripple_init,
    effect_nameent_code_init,
    effect_get_nameent_code_init,
    effect_bgmst_water_init,
    effect_commendfrag_init,
    effect_ending_ballfrag_init,
    effect_banana_drop_init,
    effect_bgend_water_init,
    effect_bgbtmfire_ptcl_init,
    effect_piyo_init,
    effect_default_init,
    effect_default_init,
    effect_default_init,
    effect_default_init,
    effect_default_init,
    effect_default_init,
    effect_default_init,
    effect_default_init,
    effect_default_init,
    effect_default_init,
    effect_default_init,
    effect_default_init,
    effect_default_init,
    effect_default_init,
    effect_default_init,
    effect_default_init,
    effect_default_init,
    effect_default_init,
    effect_default_init,
    effect_default_init,
    effect_default_init,
    effect_default_init,
    effect_default_init,
    effect_default_init,
    effect_default_init,
    effect_default_init,
    effect_default_init,
    effect_default_init,
    effect_default_init,
    effect_default_init,
    effect_default_init,
    effect_default_init,
    effect_default_init,
    effect_default_init,
    effect_default_init,
    effect_default_init,
    effect_default_init,
    effect_default_init,
    effect_default_init,
    effect_default_init,
    effect_default_init,
    effect_default_init,
    effect_default_init,
    effect_default_init,
    effect_default_init,
    effect_default_init,
    effect_default_init,
    NULL,
};

void (*effect_tick_funcs[])(struct Effect *) =
{
    effect_paperfrag_tick,
    effect_get_banana_tick,
    effect_particle_tick_coli,
    effect_holding_banana_tick,
    effect_levitate_tick,
    effect_colistar_particle_tick,
    effect_bgwat_bubble_base_tick,
    effect_bgwat_bubble_tick,
    effect_meteo_tick,
    effect_meteo_fix_tick,
    effect_coliflash_tick,
    effect_bns_stg_star_tick,
    effect_bns_stg_star_tail_tick,
    effect_bgmst_gen_cloud_tick,
    effect_bgstm_rainripple_tick,
    effect_nameent_code_tick,
    effect_get_nameent_code_tick,
    effect_bgmst_water_tick,
    effect_commendfrag_tick,
    effect_ending_ballfrag_tick,
    effect_banana_drop_tick,
    effect_bgend_water_tick,
    effect_bgbtmfire_ptcl_tick,
    effect_piyo_tick,
    effect_default_tick,
    effect_default_tick,
    effect_default_tick,
    effect_default_tick,
    effect_default_tick,
    effect_default_tick,
    effect_default_tick,
    effect_default_tick,
    effect_default_tick,
    effect_default_tick,
    effect_default_tick,
    effect_default_tick,
    effect_default_tick,
    effect_default_tick,
    effect_default_tick,
    effect_default_tick,
    effect_default_tick,
    effect_default_tick,
    effect_default_tick,
    effect_default_tick,
    effect_default_tick,
    effect_default_tick,
    effect_default_tick,
    effect_default_tick,
    effect_default_tick,
    effect_default_tick,
    effect_default_tick,
    effect_default_tick,
    effect_default_tick,
    effect_default_tick,
    effect_default_tick,
    effect_default_tick,
    effect_default_tick,
    effect_default_tick,
    effect_default_tick,
    effect_default_tick,
    effect_default_tick,
    effect_default_tick,
    effect_default_tick,
    effect_default_tick,
    effect_default_tick,
    effect_default_tick,
    effect_default_tick,
    effect_default_tick,
    effect_default_tick,
    effect_default_tick,
    effect_default_tick,
    NULL,
};

void (*effect_disp_funcs[])(struct Effect *) =
{
    effect_paperfrag_disp,
    effect_get_banana_disp,
    effect_particle_disp_coli,
    effect_holding_banana_disp,
    effect_levitate_disp,
    effect_colistar_particle_disp,
    effect_bgwat_bubble_base_disp,
    effect_bgwat_bubble_disp,
    effect_meteo_disp,
    effect_meteo_fix_disp,
    effect_coliflash_disp,
    effect_bns_stg_star_disp,
    effect_bns_stg_star_tail_disp,
    effect_bgmst_gen_cloud_disp,
    effect_bgstm_rainripple_disp,
    effect_nameent_code_disp,
    effect_get_nameent_code_disp,
    effect_bgmst_water_disp,
    effect_commendfrag_disp,
    effect_ending_ballfrag_disp,
    effect_banana_drop_disp,
    effect_bgend_water_disp,
    effect_bgbtmfire_ptcl_disp,
    effect_piyo_disp,
    effect_default_disp,
    effect_default_disp,
    effect_default_disp,
    effect_default_disp,
    effect_default_disp,
    effect_default_disp,
    effect_default_disp,
    effect_default_disp,
    effect_default_disp,
    effect_default_disp,
    effect_default_disp,
    effect_default_disp,
    effect_default_disp,
    effect_default_disp,
    effect_default_disp,
    effect_default_disp,
    effect_default_disp,
    effect_default_disp,
    effect_default_disp,
    effect_default_disp,
    effect_default_disp,
    effect_default_disp,
    effect_default_disp,
    effect_default_disp,
    effect_default_disp,
    effect_default_disp,
    effect_default_disp,
    effect_default_disp,
    effect_default_disp,
    effect_default_disp,
    effect_default_disp,
    effect_default_disp,
    effect_default_disp,
    effect_default_disp,
    effect_default_disp,
    effect_default_disp,
    effect_default_disp,
    effect_default_disp,
    effect_default_disp,
    effect_default_disp,
    effect_default_disp,
    effect_default_disp,
    effect_default_disp,
    effect_default_disp,
    effect_default_disp,
    effect_default_disp,
    effect_default_disp,
    NULL,
};

void (*effect_dest_funcs[])(struct Effect *) =
{
    effect_paperfrag_dest,
    effect_get_banana_dest,
    effect_particle_dest_coli,
    effect_holding_banana_dest,
    effect_levitate_dest,
    effect_colistar_particle_dest,
    effect_bgwat_bubble_base_dest,
    effect_bgwat_bubble_dest,
    effect_meteo_dest,
    effect_meteo_fix_dest,
    effect_coliflash_dest,
    effect_bns_stg_star_dest,
    effect_bns_stg_star_tail_dest,
    effect_bgmst_gen_cloud_dest,
    effect_bgstm_rainripple_dest,
    effect_nameent_code_dest,
    effect_get_nameent_code_dest,
    effect_bgmst_water_dest,
    effect_commendfrag_dest,
    effect_ending_ballfrag_dest,
    effect_banana_drop_dest,
    effect_bgend_water_dest,
    effect_bgbtmfire_ptcl_dest,
    effect_piyo_dest,
    effect_default_dest,
    effect_default_dest,
    effect_default_dest,
    effect_default_dest,
    effect_default_dest,
    effect_default_dest,
    effect_default_dest,
    effect_default_dest,
    effect_default_dest,
    effect_default_dest,
    effect_default_dest,
    effect_default_dest,
    effect_default_dest,
    effect_default_dest,
    effect_default_dest,
    effect_default_dest,
    effect_default_dest,
    effect_default_dest,
    effect_default_dest,
    effect_default_dest,
    effect_default_dest,
    effect_default_dest,
    effect_default_dest,
    effect_default_dest,
    effect_default_dest,
    effect_default_dest,
    effect_default_dest,
    effect_default_dest,
    effect_default_dest,
    effect_default_dest,
    effect_default_dest,
    effect_default_dest,
    effect_default_dest,
    effect_default_dest,
    effect_default_dest,
    effect_default_dest,
    effect_default_dest,
    effect_default_dest,
    effect_default_dest,
    effect_default_dest,
    effect_default_dest,
    effect_default_dest,
    effect_default_dest,
    effect_default_dest,
    effect_default_dest,
    effect_default_dest,
    effect_default_dest,
    NULL,
};

s16 next_effect_id;
struct Effect effects[MAX_EFFECTS];

void event_effect_init(void)
{
    int i;
    struct Effect *effect;

    next_effect_id = 0;
    memset(effects, 0, sizeof(effects));
    effect = effects;
    for (i = 0; i < MAX_EFFECTS; i++, effect++)
    {
        effect->poolIndex = i;
        effect->uid = -1;
    }
    pool_clear(&g_poolInfo.effectPool);
}

void event_effect_tick(void)
{
    int i;
    struct Effect *effect;
    s8 *poolStatus;

    if (g_some_other_flags & 0xA)
        return;
    poolStatus = g_poolInfo.effectPool.statusList;
    effect = effects;
    for (i = g_poolInfo.effectPool.count; i > 0; i--, poolStatus++, effect++)
    {
        if (*poolStatus != 0)
        {
            if (effect->timer > 0)
                effect->timer--;
            if (effect->timer == 0 || *poolStatus == 3)
            {
                effect_dest_funcs[effect->type](effect);
                effect->uid = -1;
                *poolStatus = 0;
            }
            else
            {
                effect->prevPos = effect->pos;
                effect_tick_funcs[effect->type](effect);
            }
        }
    }
}

void event_effect_dest(void)
{
    int i;
    struct Effect *effect;
    s8 *poolStatus;

    poolStatus = g_poolInfo.effectPool.statusList;
    effect = effects;
    for (i = g_poolInfo.effectPool.count; i > 0; i--, poolStatus++, effect++)
    {
        if (*poolStatus != 0)
        {
            effect_dest_funcs[effect->type](effect);
            effect->uid = -1;
            *poolStatus = 0;
        }
    }
}

void g_draw_effects(void)
{
    int i;
    struct Effect *effect;
    s8 *poolStatus;
    u16 cameras;

    if (!(monkey_flags & 0x10))
        return;
    if (dip_switches & 0x80000000)
        return;
    perf_init_timer(8);
    switch (modeCtrl.gameType)
    {
    case 4:
    case 8:
    case 9:
        cameras = 0xFFFF;
        break;
    default:
        cameras = 1 << g_something_with_camera3;
        break;
    }
    poolStatus = g_poolInfo.effectPool.statusList;
    effect = effects;
    if (!(g_current_camera->flags & 1))
    {
        for (i = g_poolInfo.effectPool.count; i > 0; i--, poolStatus++, effect++)
        {
            if (*poolStatus != 0 && (effect->cameras & cameras) && !(effect->flags & 1))
            {
                HFR_TAG(HFR_K_EFFECT, (u16)effect->uid);
                effect_disp_funcs[effect->type](effect);
                HFR_UNTAG();
            }
        }
    }
    else
    {
        for (i = g_poolInfo.effectPool.count; i > 0; i--, poolStatus++, effect++)
        {
            if (*poolStatus != 0 && (effect->cameras & cameras) && !(effect->flags & 1))
            {
                if (effect->flags & 0x10)
                {
                    effect_dest_funcs[effect->type](effect);
                    effect->uid = -1;
                    *poolStatus = 0;
                }
                else
                {
                    if (effect->flags & 0xE)
                        effect->prevPos = effect->pos;
                    HFR_TAG(HFR_K_EFFECT, (u16)effect->uid);
                    effect_disp_funcs[effect->type](effect);
                    HFR_UNTAG();
                }
            }
        }
    }
    perfInfo.unk10 += perf_stop_timer(8);
}

int spawn_effect(struct Effect *effect)
{
    int index;
    struct Effect *newEffect;

    index = pool_alloc(&g_poolInfo.effectPool, 1);
    if (index < 0)
        return -1;
    newEffect = &effects[index];
    memcpy_handler(newEffect, effect, sizeof(*newEffect));
    newEffect->poolIndex = index;
    effect_init_funcs[newEffect->type](newEffect);
    if (newEffect->timer == 0)
        newEffect->timer = -1;
    if (newEffect->cameras == 0)
        newEffect->cameras = 0x1F;
    newEffect->uid = next_effect_id;
    next_effect_id++;
    if (next_effect_id < 0)
        next_effect_id = 0;
    return newEffect->uid;
}

void fn_1_6869C(int type)
{
    int i;
    struct Effect *effect;
    s8 *poolStatus;

    poolStatus = g_poolInfo.effectPool.statusList;
    effect = effects;
    for (i = g_poolInfo.effectPool.count; i > 0; i--, poolStatus++, effect++)
    {
        if (*poolStatus != 0 && effect->type == type)
        {
            *poolStatus = 3;
            effect->flags |= 1;
        }
    }
}

void fn_1_6870C(int id)
{
    struct Effect *effect = fn_1_6876C(id);

    if (effect != NULL)
    {
        s8 *poolStatus = &g_poolInfo.effectPool.statusList[effect->poolIndex];

        if (*poolStatus != 0)
        {
            *poolStatus = 3;
            effect->flags |= 1;
        }
    }
}

struct Effect *fn_1_6876C(int id)
{
    int i;
    struct Effect *effect;
    s8 *poolStatus;

    poolStatus = g_poolInfo.effectPool.statusList;
    effect = effects;
    for (i = g_poolInfo.effectPool.count; i > 0; i--, poolStatus++, effect++)
    {
        if (*poolStatus != 0 && effect->uid == id)
            break;
    }
    if (i == 0)
        return NULL;
    else
        return effect;
}

static struct EffectFuncs s_defaultEffectFuncs =
{
    effect_default_init,
    effect_default_tick,
    effect_default_disp,
    effect_default_dest,
};

static u16 s_paperPieceModels[] = { 0x46, 0x47, 0x48, 0x49, 0x4A };
static u16 s_commendPieceModels[] = { 0x27, 0x28, 0x29, 0x2A, 0x2B };

void overwrite_effect_funcs(int type, struct EffectFuncs *newFuncs)
{
    struct EffectFuncs funcs;

    if (newFuncs == NULL)
        funcs = s_defaultEffectFuncs;
    else
    {
        funcs = *newFuncs;
        if (funcs.init == NULL)
            funcs.init = s_defaultEffectFuncs.init;
        if (funcs.tick == NULL)
            funcs.tick = s_defaultEffectFuncs.tick;
        if (funcs.disp == NULL)
            funcs.disp = s_defaultEffectFuncs.disp;
        if (funcs.dest == NULL)
            funcs.dest = s_defaultEffectFuncs.dest;
    }
    effect_init_funcs[type] = funcs.init;
    effect_tick_funcs[type] = funcs.tick;
    effect_disp_funcs[type] = funcs.disp;
    effect_dest_funcs[type] = funcs.dest;
}

void effect_default_init(struct Effect *effect) {}

void effect_default_tick(struct Effect *effect) {}

void effect_default_disp(struct Effect *effect) {}

void effect_default_dest(struct Effect *effect) {}

void effect_paperfrag_init(struct Effect *effect)
{
    if (effect->timer == 0)
        effect->timer = 210.0 + (60.0f * RAND_FLOAT());
    if (effect->model == NULL)
    {
        u32 modelIndex = s_paperPieceModels[(u32)(rand() & 0x7FFF) % ARRAY_COUNT(s_paperPieceModels)];
        effect->model = init_common_gma->modelEntries[modelIndex].model;
    }
    effect->state = 0;
    if (effect->scale.x == 0.0f)
        effect->scale.x = 0.5 + (0.5 * RAND_FLOAT());
}

void effect_paperfrag_tick(struct Effect *effect)
{
    Vec sp58;
    struct RaycastHit stageHit;
    Vec sp30;
    Vec sp24;
    Vec sp18;
    Vec spC;
    float temp_f2;
    float temp_f5;
    float temp_f7;
    float temp_f8;
    float temp_f6;
    float temp_f3;
    float temp_f4;
    s16 temp_r29;
    s16 temp_r31;
    struct World *world = current_world_info;

    effect->vel.x += 0.004 * world->unk10.x;
    effect->vel.y += 0.004 * world->unk10.y;
    effect->vel.z += 0.004 * world->unk10.z;
    mtxa_from_rotate_y(effect->rotY);
    mtxa_rotate_x(effect->rotX);
    mtxa_rotate_z(effect->rotZ);
    sp58.x = 0.0f;
    sp58.y = 1.0f;
    sp58.z = 0.0f;
    mtxa_tf_vec(&sp58, &sp58);
    temp_f5 = -0.7 * mathutil_vec_dot_prod(&sp58, &effect->vel);
    effect->vel.x += sp58.x * temp_f5;
    effect->vel.y += sp58.y * temp_f5;
    effect->vel.z += sp58.z * temp_f5;
    effect->vel.x *= 0.95;
    effect->vel.y *= 0.95;
    effect->vel.z *= 0.95;
    effect->pos.x += effect->vel.x;
    effect->pos.y += effect->vel.y;
    effect->pos.z += effect->vel.z;
    temp_f6 = 2560.0f / effect->scale.x;
    effect->unk52 += (s16)(temp_f6 * (effect->vel.x + effect->vel.y));
    effect->unk54 += (s16)(temp_f6 * (effect->vel.y + effect->vel.z));
    effect->unk56 += (s16)(temp_f6 * (effect->vel.z + effect->vel.x));
    effect->unk52 -= effect->unk52 >> 5;
    effect->unk54 -= effect->unk54 >> 5;
    effect->unk56 -= effect->unk56 >> 5;
    effect->rotX += effect->unk52;
    effect->rotY += effect->unk54;
    effect->rotZ += effect->unk56;

    if (!(effect->timer & 0xF))
    {
        effect->state = 0;
        if (raycast_stage_down(&effect->pos, &stageHit, &effect->unk80)
         && stageHit.pos.y > effect->pos.y - 1.0)
        {
            effect->state = 1;
            effect->unk64.x = stageHit.pos.x;
            effect->unk64.y = stageHit.pos.y;
            effect->unk64.z = stageHit.pos.z;
            effect->unk70.x = stageHit.normal.x;
            effect->unk70.y = stageHit.normal.y;
            effect->unk70.z = stageHit.normal.z;
        }
    }

    switch (effect->state)
    {
    case 0:
        return;
    }

    sp24 = effect->unk70;
    sp30.x = 0.0f;
    sp30.y = 1.0f;
    sp30.z = 0.0f;
    mtxa_from_rotate_y(effect->rotY);
    mtxa_rotate_x(effect->rotX);
    mtxa_rotate_z(effect->rotZ);
    mtxa_tf_vec(&sp30, &sp30);

    temp_f3 = (sp30.x * sp24.x) + (sp30.y * sp24.y) + (sp30.z * sp24.z);
    temp_f7 = (1.1920929e-7f * effect->poolIndex) + (0.1 * (1.0 - fabs(temp_f3)));
    sp18.x = effect->pos.x - effect->unk64.x;
    sp18.y = effect->pos.y - effect->unk64.y;
    sp18.z = effect->pos.z - effect->unk64.z;
    temp_f4 = (sp24.x * sp18.x) + (sp24.y * sp18.y) + (sp24.z * sp18.z);
    if (temp_f4 < temp_f7 + 0.05)
    {
        temp_f2 = temp_f7 + 0.05 - temp_f4;
        effect->pos.x += temp_f2 * sp24.x;
        effect->pos.y += temp_f2 * sp24.y;
        effect->pos.z += temp_f2 * sp24.z;
        temp_f2 = (effect->vel.x * sp24.x) + (effect->vel.y * sp24.y) + (effect->vel.z * sp24.z);
        if (temp_f2 < 0.0)
        {
            temp_f2 = -temp_f2;
            effect->vel.x += temp_f2 * sp24.x;
            effect->vel.y += temp_f2 * sp24.y;
            effect->vel.z += temp_f2 * sp24.z;
            temp_f8 = fabs(temp_f3) * (1.0 / 8.0);
            effect->vel.x = effect->vel.x + (temp_f8 * (effect->unk80.x - effect->vel.x));
            effect->vel.y = effect->vel.y + (temp_f8 * (effect->unk80.y - effect->vel.y));
            effect->vel.z = effect->vel.z + (temp_f8 * (effect->unk80.z - effect->vel.z));
            temp_f2 = temp_f2 / 2.0;
            effect->vel.x += temp_f2 * sp24.x;
            effect->vel.y += temp_f2 * sp24.y;
            effect->vel.z += temp_f2 * sp24.z;
        }
        spC = sp24;
        if (temp_f3 < 0.0)
        {
            spC.x = -spC.x;
            spC.y = -spC.y;
            spC.z = -spC.z;
        }
        sp30.x = (0.05 * spC.x) + (0.95 * sp30.x);
        sp30.y = (0.05 * spC.y) + (0.95 * sp30.y);
        sp30.z = (0.05 * spC.z) + (0.95 * sp30.z);
        vec_normalize_len(&sp30);
        temp_r29 = effect->rotX;
        temp_r31 = effect->rotZ;
        mtxa_from_rotate_y(-effect->rotY);
        mtxa_rotate_x(0);
        mtxa_tf_vec(&sp30, &sp30);
        effect->rotX = math_atan2(sp30.z, sp30.y);
        effect->rotZ = -math_atan2(sp30.x, math_sqrt(mathutil_sum_of_sq_2(sp30.z, sp30.y)));
        effect->unk52 >>= 2;
        effect->unk52 += (effect->rotX - temp_r29) >> 2;
        effect->unk54 -= effect->unk54 >> 4;
        effect->unk56 >>= 2;
        effect->unk56 += (effect->rotZ - temp_r31) >> 2;
    }
}

void effect_paperfrag_disp(struct Effect *effect)
{
    float temp_f30;
    float var_f31;
    float new_var;

    if (g_some_render_flag.flags & 4)
    {
        temp_f30 = 0.1 * effect->scale.x;
        var_f31 = temp_f30 + fn_1_80D4(&effect->pos);
        if (var_f31 <= 0.0f)
            return;
        if (var_f31 > temp_f30)
            var_f31 = 1.0f;
        else
            var_f31 *= (1.0f / temp_f30);
    }
    else
        var_f31 = 1.0f;

    mtxa_from_mtxb_translate(&effect->pos);
    mtxa_rotate_y(effect->rotY);
    mtxa_rotate_x(effect->rotX);
    mtxa_rotate_z(effect->rotZ);

    new_var = g_current_camera->sub28.view.invTanHalfFov * ((120.0f * g_current_camera->sub28.viewportH) * (0.11f * effect->scale.x));
    var_f31 *= 2.0f * (new_var + mtxa->mtxA[2][3]);
    if (var_f31 < 0.0)
        return;
    if (var_f31 > 1.0)
        var_f31 = 1.0f;
    mtxa_scale_s(effect->scale.x);
    avdisp_set_scale_factor(effect->scale.x);
    if (effect->timer < 12)
        var_f31 *= 0.083333336f * effect->timer;
    if (var_f31 < 1.0)
    {
        avdisp_set_alpha(var_f31);
        avdisp_draw_model_culled_sort_always(effect->model);
    }
    else
    {
        load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
        avdisp_draw_model_culled_sort_auto(effect->model);
    }
}

void effect_paperfrag_dest(struct Effect *effect) {}

void effect_get_banana_init(struct Effect *effect)
{
    effect->timer = 60.0 * (0.25 + (0.125 * RAND_FLOAT()));
    effect->cameras = 1 << effect->playerId;
    mtxa_from_mtx(cameras[effect->playerId].unk14C);
    mtxa_tf_point(&effect->pos, &effect->pos);
    mtxa_tf_vec(&effect->vel, &effect->vel);
    mtxa_rotate_y(effect->rotY);
    mtxa_rotate_x(effect->rotX);
    mtxa_rotate_z(effect->rotZ);
    mtxa_to_euler_yxz(&effect->rotY, &effect->rotX, &effect->rotZ);
}

void effect_get_banana_tick(struct Effect *effect)
{
    float temp_f0;
    float temp_f7;
    float new_var;
    Vec v;
    struct Camera *camera = &cameras[effect->playerId];
    float radius = effect->model->boundSphereRadius;

    v.z = -2.0f;
    temp_f7 = 2.0f * camera->sub28.view.tanHalfFov;
    v.x = (1.0583333333333333 - camera->sub28.view.unk28) * temp_f7;
    v.y = (0.8416666666666667 - camera->sub28.view.unk2C) * temp_f7;
    new_var = (0.05333333333333334 * temp_f7) / radius;

    temp_f0 = 1.0 / ((float) effect->timer);
    effect->vel.x = temp_f0 * (v.x - effect->pos.x);
    effect->vel.y = temp_f0 * (v.y - effect->pos.y);
    effect->vel.z = temp_f0 * (v.z - effect->pos.z);

    effect->pos.x += effect->vel.x;
    effect->pos.y += effect->vel.y;
    effect->pos.z += effect->vel.z;

    effect->scale.x += temp_f0 * (new_var - effect->scale.x);
    effect->scale.y = effect->scale.x;
    effect->scale.z = effect->scale.x;

    effect->unk54 -= effect->unk54 >> 7;
    effect->rotX += effect->unk52;
    effect->rotY += effect->unk54;
    effect->rotZ += effect->unk56;
}

void effect_get_banana_disp(struct Effect *effect)
{
    if (!(g_some_render_flag.flags & 4))
    {
        struct GMAModel *model;

        mtxa_from_translate(&effect->pos);
        mtxa_rotate_y(effect->rotY);
        mtxa_rotate_x(effect->rotX);
        mtxa_rotate_z(effect->rotZ);
        mtxa_scale_xyz(effect->scale.x, effect->scale.y, effect->scale.z);
        model = effect->model;
        if (g_is_sphere_visible(&model->boundSphereCenter, model->boundSphereRadius * effect->scale.x))
        {
            if (g_some_render_flag.flags & 0x100)
                avdisp_draw_model_unculled_sort_always(model);
            else
            {
                g_smth_with_gpu_and_locked_cache_mtx();
                avdisp_draw_model_unculled_sort_never(model);
            }
        }
    }
}

void effect_get_banana_dest(struct Effect *effect) {}

void effect_particle_init_coli(struct Effect *effect)
{
    effect->flags |= 2;
    effect->state = 0;
    effect->timer = 15.0 + (45.0 * RAND_FLOAT());
    effect->u_otherTimer = 0;
    effect->scale.x = 1.0f;
    if (effect->unk18 == 0.0f && effect->unk1C == 0.0f && effect->unk20 == 0.0f)
    {
        effect->unk18 = 0.9f;
        effect->unk1C = 0.8f;
        effect->unk20 = 0.4f;
    }
    effect->cameras = 1 << effect->playerId;
    effect->u_otherTimer = 0;
}

void effect_particle_tick_coli(struct Effect *effect)
{
    struct RaycastHit stageHit;
    Vec sp18;
    Vec spC;
    float temp_f31;
    struct World *world = current_world_info;

    effect->vel.x += 0.008f * world->unk10.x;
    effect->vel.y += 0.008f * world->unk10.y;
    effect->vel.z += 0.008f * world->unk10.z;
    effect->vel.x *= 0.992f;
    effect->vel.y *= 0.992f;
    effect->vel.z *= 0.992f;
    effect->pos.x += effect->vel.x;
    effect->pos.y += effect->vel.y;
    effect->pos.z += effect->vel.z;
    if (!(effect->timer & 0xF))
    {
        effect->state = 0;
        if (raycast_stage_down(&effect->pos, &stageHit, &effect->unk80)
         && stageHit.pos.y > effect->pos.y - 0.5)
        {
            effect->state = 1;
            effect->unk64.x = stageHit.pos.x;
            effect->unk64.y = stageHit.pos.y;
            effect->unk64.z = stageHit.pos.z;
            effect->unk70.x = stageHit.normal.x;
            effect->unk70.y = stageHit.normal.y;
            effect->unk70.z = stageHit.normal.z;
        }
    }
    if (effect->timer < 24)
    {
        effect->unk18 *= 0.96;
        effect->unk1C *= 0.87;
        effect->unk20 *= 0.86;
        effect->scale.x = (float)effect->timer / 24.0;
    }

    switch (effect->state)
    {
    case 0:
        if (effect->colorFactor > 0.0)
        {
            effect->colorFactor -= 0.125;
            if (effect->colorFactor < 0.0)
                effect->colorFactor = 0.0f;
        }
        return;
    case 1:
        sp18 = effect->unk70;
        spC.x = effect->pos.x - effect->unk64.x;
        spC.y = effect->pos.y - effect->unk64.y;
        spC.z = effect->pos.z - effect->unk64.z;
        temp_f31 = mathutil_vec_dot_prod(&sp18, &spC);
        effect->colorFactor += 0.25 * (temp_f31 - effect->colorFactor);
        effect->unk8C.x = effect->pos.x - ((temp_f31 - 0.02) * sp18.x);
        effect->unk8C.y = effect->pos.y - ((temp_f31 - 0.02) * sp18.y);
        effect->unk8C.z = effect->pos.z - ((temp_f31 - 0.02) * sp18.z);
        vec_to_euler_xy(&sp18, &effect->unkA4, &effect->unkA6);
        effect->unkA4 += 0x8000;
        if (temp_f31 < 0.025)
        {
            temp_f31 = 0.025 - temp_f31;
            effect->pos.x += temp_f31 * sp18.x;
            effect->pos.y += temp_f31 * sp18.y;
            effect->pos.z += temp_f31 * sp18.z;
            temp_f31 = mathutil_vec_dot_prod(&effect->vel, &sp18);
            if (temp_f31 < 0.0)
            {
                    temp_f31 *= -1.0;
                effect->vel.x += temp_f31 * sp18.x;
                effect->vel.y += temp_f31 * sp18.y;
                effect->vel.z += temp_f31 * sp18.z;
                effect->vel.x += 0.03 * (effect->unk80.x - effect->vel.x);
                effect->vel.y += 0.03 * (effect->unk80.y - effect->vel.y);
                effect->vel.z += 0.03 * (effect->unk80.z - effect->vel.z);
                temp_f31 *= 0.9;
                effect->vel.x += temp_f31 * sp18.x;
                effect->vel.y += temp_f31 * sp18.y;
                effect->vel.z += temp_f31 * sp18.z;
            }
        }
        break;
    }
}

void effect_particle_disp_coli(struct Effect *effect)
{
    float temp_f31;
    float temp_f30;
    float var_f1;
    struct PointWithColor sp28;
    struct PointWithColor sp18;
    Vec sp38;
    Vec spC;

    if ((g_some_render_flag.flags & 4) && fn_1_80D4(&effect->pos) < 0.0f)
        return;
    mtxa_from_mtx(g_ord_tbl_stuff->matrices[3]);
    mtxa_tf_point(&effect->prevPos, &sp38);
    mtxa_from_mtxb();
    if (effect->u_otherTimer == 0)
    {
        effect->u_otherTimer = 1;
        return;
    }
    mtxa_tf_point(&effect->pos, &sp18.pos);
    sp28.pos.x = (1.5 * sp38.x) - (0.5 * sp18.pos.x);
    sp28.pos.y = (1.5 * sp38.y) - (0.5 * sp18.pos.y);
    sp28.pos.z = (1.5 * sp38.z) - (0.5 * sp18.pos.z);
    mtxa_from_identity();
    temp_f31 = 0.5f * mathutil_vec_distance(&sp18.pos, &sp28.pos);
    spC.x = 0.5f * (sp18.pos.x + sp28.pos.y);
    spC.y = 0.5f * (sp18.pos.y + sp28.pos.y);
    spC.z = 0.5f * (sp18.pos.z + sp28.pos.z);
    if (g_is_sphere_visible(&spC, temp_f31))
    {
        sp18.color.r = 255.0f * effect->unk18;
        sp18.color.g = 255.0f * effect->unk1C;
        sp18.color.b = 255.0f * effect->unk20;
        sp18.color.a = 255;
        sp28.color.r = 96.0f * effect->unk18;
        sp28.color.g = 88.0f * effect->unk1C;
        sp28.color.b = 8.0f * effect->unk20;
        sp28.color.a = 255;
        if (sp18.pos.z < -3.0)
            var_f1 = 1.5f;
        else if (sp18.pos.z > 0.0)
            var_f1 = 4.5f;
        else
            var_f1 = 1.5 + (3.0 + sp18.pos.z);
        fn_1_E2FD0(6.0f * var_f1);
        fn_1_E2FE0(1, 1, 1, 0);
        fn_1_E349C(&sp18, &sp28);
    }
    if (effect->colorFactor > 0.0 && effect->colorFactor < 0.5)
    {
        struct GMAModel *model;
        struct OrdTblNode *entry;
        struct EffectOrdNode *node;

        temp_f30 = (0.25 - (0.5 * effect->colorFactor)) * effect->scale.x;
        model = init_common_gma->modelEntries[94].model;
        mtxa_from_mtxb_translate(&effect->unk8C);
        if (g_is_sphere_visible(&model->boundSphereCenter, model->boundSphereRadius * temp_f30))
        {
            entry = fn_1_CCC18(mtxa->mtxA[2][3]);
            node = ord_tbl_alloc_node(sizeof(*node));
            if (node != NULL)
            {
                node->node.drawFunc = fn_1_6A064;
                node->effect = effect;
                fn_1_CD418(entry, &node->node);
            }
        }
    }
}

static void fn_1_6A064(struct OrdTblNode *node_)
{
    struct EffectOrdNode *node = (struct EffectOrdNode *)node_;
    struct Effect *effect = node->effect;
    float temp_f30;
    float temp_f31;

    temp_f31 = 0.5 / (1.0 + (2.0 * effect->colorFactor));
    temp_f30 = (0.25 - (0.5 * effect->colorFactor)) * effect->scale.x;

    mtxa_from_mtxb_translate(&effect->unk8C);
    mtxa_rotate_y(effect->unkA6);
    mtxa_rotate_x(effect->unkA4);
    mtxa_scale_s(temp_f30);
    load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
    avdisp_set_z_mode(1, 3, 0);
    set_post_mult_color(effect->unk18 * temp_f31, effect->unk1C * temp_f31, effect->unk20 * temp_f31, 1.0f);
    avdisp_draw_model_unculled_sort_never(init_common_gma->modelEntries[94].model);
    avdisp_set_z_mode(1, 3, 1);
    fade_color_base_default();
}

void effect_particle_dest_coli(struct Effect *effect) {}

void effect_holding_banana_init(struct Effect *effect)
{
    struct Ball *ball = current_ball;

    effect->u_otherTimer = 30;
    effect->unk8C.x = effect->pos.x - ball->pos.x;
    effect->unk8C.y = effect->pos.y - ball->pos.y;
    effect->unk8C.z = effect->pos.z - ball->pos.z;
    effect->unk98.x = 0.5 * effect->scale.x;
    effect->unk98.y = 0.5 * effect->scale.y;
    effect->unk98.z = 0.5 * effect->scale.z;
    effect->unkA6 = 0x1000;
}

void effect_holding_banana_tick(struct Effect *effect)
{
    struct Ball *ball = &balls[effect->playerId];
    float q;
    float temp;

    effect->u_otherTimer--;
    if (effect->u_otherTimer == 0)
    {
        struct Effect newEffect;

        memcpy_handler(&newEffect, effect, sizeof(newEffect));
        newEffect.type = 1;
        newEffect.timer = 0;
        newEffect.playerId = effect->playerId;
        newEffect.cameras = effect->cameras;
        spawn_effect(&newEffect);
        effect->timer = 1;
    }
    else
    {
        q = (float)(30.0 - effect->u_otherTimer) / 30.0f;
        temp = 0.75 + 0.5 * q;
        effect->unk8C.x *= 0.8;
        effect->unk8C.y += 0.2 * (temp - effect->unk8C.y);
        effect->unk8C.z *= 0.8;
        effect->pos.x = ball->pos.x + effect->unk8C.x;
        effect->pos.y = ball->pos.y + effect->unk8C.y;
        effect->pos.z = ball->pos.z + effect->unk8C.z;
        effect->unk54 += (effect->unkA6 - effect->unk54) >> 3;
        effect->rotY += effect->unk54;
        effect->scale.x += 0.2 * (effect->unk98.x - effect->scale.x);
        effect->scale.y += 0.2 * (effect->unk98.y - effect->scale.y);
        effect->scale.z += 0.2 * (effect->unk98.z - effect->scale.z);
    }
}

void effect_holding_banana_disp(struct Effect *effect)
{
    struct GMAModel *model;

    mtxa_from_mtxb();
    mtxa_translate(&effect->pos);
    mtxa_rotate_y(effect->rotY);
    mtxa_rotate_x(effect->rotX);
    mtxa_rotate_z(effect->rotZ);
    mtxa_scale_xyz(effect->scale.x, effect->scale.y, effect->scale.z);
    model = effect->model;
    if (g_is_sphere_visible(&model->boundSphereCenter, model->boundSphereRadius * effect->scale.x))
    {
        if (g_some_render_flag.flags & 0x100)
            avdisp_draw_model_unculled_sort_always(model);
        else
        {
            g_smth_with_gpu_and_locked_cache_mtx();
            avdisp_draw_model_unculled_sort_never(model);
        }
    }
}

void effect_holding_banana_dest(struct Effect *effect) {}

void effect_levitate_init(struct Effect *effect)
{
    struct RaycastHit stageHit;
    float a;
    float b;

    a = 1.0f;
    b = 1.0f;
    if (sub_mode == 8)
    {
        a = 8.0f;
        b = 3.0f;
    }
    effect->timer = (rand() & 0x7FFF) % 15 + 60;
    effect->u_otherTimer = (rand() & 3) + 10;
    effect->pos.x += a * (0.75 * (RAND_FLOAT() - 0.5));
    effect->pos.y += a * (0.75 * (RAND_FLOAT() - 0.5));
    effect->pos.z += a * (0.75 * (RAND_FLOAT() - 0.5));
    if (raycast_stage_down(&effect->pos, &stageHit, NULL))
        effect->unk64.y = stageHit.pos.y;
    else
        effect->unk64.y = effect->pos.y - 100.0;
    effect->vel.y = 0.0040833335f;
    effect->cameras = 1 << effect->playerId;
    if (modeCtrl.gameType == 1)
        effect->scale.x = 0.45f;
    else
        effect->scale.x = 0.1875 * b;
}

void effect_levitate_tick(struct Effect *effect)
{
    effect->vel.y -= 0.0008166666666666666;
    effect->pos.y += effect->vel.y;
    if (effect->pos.y < effect->unk64.y + (0.5 * effect->scale.x))
    {
        effect->pos.y = effect->unk64.y + (0.5 * effect->scale.x);
        if (effect->scale.x > 0.05)
            effect->scale.x -= 0.05;
        else
            g_poolInfo.effectPool.statusList[effect->poolIndex] = 3;
    }
}

void effect_levitate_disp(struct Effect *effect)
{
    Vec spC;
    float t;
    float scale;
    float alpha;

    mtxa_from_mtx(g_ord_tbl_stuff->matrices[0]);
    mtxa_translate(&effect->pos);
    mathutil_mtxA_get_translate(&spC);
    mtxa_sq_from_identity();
    t = (float)(effect->timer % effect->u_otherTimer) / (float)effect->u_otherTimer;
    if (!(spC.z >= -1.1920929e-7f))
    {
        scale = effect->scale.x * (0.25 + 0.75 * t) * math_sqrt((-spC.z * math_tan(g_current_camera->sub28.view.fov / 2)) / 0.577350269);
        mtxa_scale_s(scale);
        avdisp_set_scale_factor(scale);
        if (effect->timer >= 56)
        {
            alpha = 60 - effect->timer;
            alpha *= 0.25;
            avdisp_set_alpha(alpha);
        }
        load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
        avdisp_set_z_mode(1, 3, 0);
        avdisp_set_alpha(t);
        avdisp_draw_model_culled_sort_always(init_common_gma->modelEntries[38].model);
        avdisp_set_z_mode(1, 3, 1);
    }
}

void effect_levitate_dest(struct Effect *effect) {}

void effect_colistar_particle_init(struct Effect *effect)
{
    effect->flags |= 2;
    effect->state = 0;
    effect->timer = 15.0 + (45.0 * RAND_FLOAT());
    effect->u_otherTimer = 0;
    effect->cameras = 1 << effect->playerId;
    effect->model = init_common_gma->modelEntries[37].model;
    effect->rotX = 0;
    effect->colorFactor = 0.0f;
    effect->prevPos = effect->pos;
    effect->scale.x = 0.0f;
    if (effect->scale.y == 0.0)
        effect->scale.y = 0.015f;
    else
        effect->scale.y *= 0.015f;
    effect->rotX = 0;
    effect->rotY = rand() & 0x7FFF;
    effect->rotZ = rand() & 0x7FFF;
    effect->unk52 = 0;
    effect->unk54 = (rand() & 0xFFF) + 0x1000;
    effect->unk56 = 0;
}

void effect_colistar_particle_tick(struct Effect *effect)
{
    struct RaycastHit stageHit;
    Vec sp18;
    Vec spC;
    float temp_f31;
    struct World *world = &current_world_info[effect->playerId];

    effect->vel.x += 0.004f * world->unk10.x;
    effect->vel.y += 0.004f * world->unk10.y;
    effect->vel.z += 0.004f * world->unk10.z;
    effect->vel.x *= 0.985f;
    effect->vel.y *= 0.985f;
    effect->vel.z *= 0.985f;
    effect->pos.x += effect->vel.x;
    effect->pos.y += effect->vel.y;
    effect->pos.z += effect->vel.z;
    effect->unk52 -= effect->unk52 >> 2;
    effect->unk54 -= effect->unk54 >> 5;
    effect->unk56 -= effect->unk56 >> 3;
    effect->rotX += effect->unk52;
    effect->rotY += effect->unk54;
    effect->rotZ += effect->unk56;
    if (!(effect->timer & 7))
    {
        effect->state = 0;
        if (raycast_stage_down(&effect->pos, &stageHit, &effect->unk80)
         && stageHit.pos.y > effect->pos.y - 0.6)
        {
            effect->state = 1;
            effect->unk64.x = stageHit.pos.x;
            effect->unk64.y = stageHit.pos.y;
            effect->unk64.z = stageHit.pos.z;
            effect->unk70.x = stageHit.normal.x;
            effect->unk70.y = stageHit.normal.y;
            effect->unk70.z = stageHit.normal.z;
        }
    }

    if (effect->timer < 24)
    {
        float temp_f2 = (float)effect->timer;
        effect->scale.x *= temp_f2 / (1.0f + temp_f2);
    }
    else
        effect->scale.x += 0.3312f * (effect->scale.y - effect->scale.x);

    switch (effect->state)
    {
    case 0:
        if (effect->colorFactor > 0.0)
        {
            effect->colorFactor -= 0.0625;
            if (effect->colorFactor < 0.0)
                effect->colorFactor = 0.0f;
        }
        break;
    case 1:
        sp18 = effect->unk70;
        spC.x = effect->pos.x - effect->unk64.x;
        spC.y = effect->pos.y - effect->unk64.y;
        spC.z = effect->pos.z - effect->unk64.z;
        temp_f31 = mathutil_vec_dot_prod(&sp18, &spC);
        effect->colorFactor += 0.25 * (temp_f31 - effect->colorFactor);
        effect->unk8C.x = effect->pos.x - ((temp_f31 - 0.02) * sp18.x);
        effect->unk8C.y = effect->pos.y - ((temp_f31 - 0.02) * sp18.y);
        effect->unk8C.z = effect->pos.z - ((temp_f31 - 0.02) * sp18.z);
        vec_to_euler_xy(&sp18, &effect->unkA4, &effect->unkA6);
        effect->unkA4 += 0x8000;
        if (temp_f31 < 8.090365f * effect->scale.x)
        {
            temp_f31 = 8.090365f * effect->scale.x - temp_f31;
            effect->pos.x += temp_f31 * sp18.x;
            effect->pos.y += temp_f31 * sp18.y;
            effect->pos.z += temp_f31 * sp18.z;
            temp_f31 = mathutil_vec_dot_prod(&effect->vel, &sp18);
            if (temp_f31 < 0.0)
            {
                temp_f31 *= -1.0;
                effect->vel.x += temp_f31 * sp18.x;
                effect->vel.y += temp_f31 * sp18.y;
                effect->vel.z += temp_f31 * sp18.z;
                effect->vel.x += 0.029999971f * (effect->unk80.x - effect->vel.x);
                effect->vel.y += 0.029999971f * (effect->unk80.y - effect->vel.y);
                effect->vel.z += 0.029999971f * (effect->unk80.z - effect->vel.z);
                effect->unk52 += 131072.0f * (temp_f31 * (RAND_FLOAT() - 0.5f));
                effect->unk54 += 524288.0f * (temp_f31 * (RAND_FLOAT() - 0.5f));
                effect->unk56 += 131072.0f * (temp_f31 * (RAND_FLOAT() - 0.5f));
                effect->vel.x += temp_f31 * sp18.x;
                effect->vel.y += temp_f31 * sp18.y;
                effect->vel.z += temp_f31 * sp18.z;
            }
        }
        break;
    }
}

void effect_colistar_particle_disp(struct Effect *effect)
{
    struct GMAModel *model = effect->model;
    struct OrdTblNode *entry;
    struct EffectOrdNode *node;

    if ((g_some_render_flag.flags & 4)
     && fn_1_80D4(&effect->pos) < -(model->boundSphereRadius * effect->scale.x))
        return;

    mtxa_from_mtxb_translate(&effect->pos);
    mtxa_rotate_y(effect->rotY);
    mtxa_rotate_x(effect->rotX);
    mtxa_rotate_z(effect->rotZ);

    mtxa_push();
    mtxa_scale_s(effect->scale.x);
    if (g_is_sphere_visible_scaled(&model->boundSphereCenter, model->boundSphereRadius, effect->scale.x))
    {
        load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
        avdisp_draw_model_unculled_sort_never(model);
    }
    mtxa_pop();

    entry = fn_1_CCC18(mtxa->mtxA[2][3]);
    node = ord_tbl_alloc_node(sizeof(*node));
    if (node != NULL)
    {
        node->node.drawFunc = fn_1_6B3E4;
        node->effect = effect;
        fn_1_CD418(entry, &node->node);
    }
}

static void fn_1_6B3E4(struct OrdTblNode *node_)
{
    struct EffectOrdNode *node = (struct EffectOrdNode *)node_;
    struct Effect *effect;
    Vec sp30;
    Vec sp24;
    Vec sp18;
    Vec spC;
    float temp_f31;
    float temp_f30;
    float scale;
    float temp_f10;
    float temp_f1;
    struct GMAModel *model;
    s16 temp_r29;
    s16 var_r28;

    effect = node->effect;
    mtxa_from_mtxb_translate(&effect->pos);
    mtxa_rotate_y(effect->rotY);
    mtxa_rotate_x(effect->rotX);
    mtxa_rotate_z(effect->rotZ);

    mathutil_mtxA_get_translate(&sp30);
    mtxa_from_mtx(g_ord_tbl_stuff->matrices[3]);
    mtxa_tf_point(&effect->prevPos, &sp18);

    if (sp30.z < -0.1f && sp18.z < -0.1f
     && effect->scale.x > (effect->scale.y * 0.1f))
    {
        sp24 = sp30;

        temp_r29 = 24576.0f * ((sp30.x + sp30.y) / sp30.z);
        scale = 0.6f * (0.5f + (effect->scale.x * (0.5f / effect->scale.y)));
        if (scale > 1.0f)
            scale = 1.0f;

        model = init_common_gma->modelEntries[36].model;
        temp_f10 = (0.1f + sp30.z) / sp30.z;
        temp_f30 = 0.4f * effect->scale.x * temp_f10;
        temp_f31 = temp_f30 * (model->boundSphereRadius);

        temp_f10 = (0.1f + sp30.z) / sp24.z;
        sp24.x *= temp_f10;
        sp24.y *= temp_f10;
        sp24.z = 0.1f + sp30.z;

        temp_f10 = (0.1f + sp18.z) / sp18.z;
        sp18.x *= temp_f10;
        sp18.y *= temp_f10;
        sp18.z = 0.1f + sp18.z;

        sp30.x = sp24.x - sp18.x;
        sp30.y = sp24.y - sp18.y;
        sp30.z = 0.0f;

        if (sp30.x == 0.0f && sp30.y == 0.0f)
        {
            var_r28 = 0;
            spC.x = temp_f30;
            spC.y = temp_f30;
            spC.z = temp_f30;
        }
        else
        {
            vec_set_len(temp_f31, &sp30, &sp30);
            sp24.x += sp30.x;
            sp24.y += sp30.y;
            sp18.x -= sp30.x;
            sp18.y -= sp30.y;
            var_r28 = math_atan2(sp30.y, sp30.x);
            temp_f1 = (0.5f * mathutil_vec_distance(&sp24, &sp18)) / temp_f31;
            spC.x = temp_f30 * temp_f1;
            spC.y = temp_f30 / temp_f1;
            spC.z = temp_f30;
        }

        mtxa_from_translate_xyz(0.5f * (sp24.x + sp18.x), 0.5f * (sp24.y + sp18.y), sp24.z);
        mtxa_rotate_z(var_r28);
        mtxa_scale(&spC);
        mtxa_rotate_z(temp_r29 - var_r28);
        avdisp_set_scale_factor(spC.x);
        load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
        avdisp_set_z_mode(1, 3, 0);
        set_post_mult_color(scale, scale, scale, 1.0f);
        avdisp_draw_model_culled_sort_never(model);
        fade_color_base_default();
        avdisp_set_z_mode(1, 3, 1);
    }

    if (effect->colorFactor > 0.0f && effect->colorFactor < 0.6f)
    {
        temp_f30 = effect->scale.x * (1.0f / effect->scale.y);
        scale = (0.6f - effect->colorFactor) * temp_f30;
        mtxa_from_mtxb_translate(&effect->unk8C);
        mtxa_rotate_y(effect->unkA6);
        mtxa_rotate_x(effect->unkA4);
        mtxa_scale_s(scale);
        temp_f30 *= 1.0f - (0.083333336f * effect->colorFactor);
        if (temp_f30 > 1.0f)
            set_post_mult_color(0.71999997f, 0.63f, 0.54f, 1.0f);
        else
            set_post_mult_color(0.71999997f * temp_f30, 0.63f * temp_f30, 0.54f * temp_f30, 1.0f);
        avdisp_set_z_mode(1, 3, 0);
        load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
        avdisp_set_scale_factor(scale);
        avdisp_draw_model_culled_sort_never(init_common_gma->modelEntries[94].model);
        avdisp_set_z_mode(1, 3, 1);
        fade_color_base_default();
    }
}

void effect_colistar_particle_dest(struct Effect *effect) {}

void effect_bgwat_bubble_base_init(struct Effect *effect)
{
    struct StageBgModel *bgObj;
    struct GMAModel *model;

    effect->flags |= 1;
    effect->u_otherTimer = 0;
    bgObj = (struct StageBgModel *)effect->model;
    model = bgObj->model;
    mtxa_from_translate(&bgObj->pos);
    mtxa_rotate_z(bgObj->rotZ);
    mtxa_rotate_y(bgObj->rotY);
    mtxa_rotate_x(bgObj->rotX);
    mtxa_tf_point(&model->boundSphereCenter, &effect->pos);
}

void effect_bgwat_bubble_base_tick(struct Effect *effect)
{
    struct StageBgModel *bgObj = (struct StageBgModel *)effect->model;
    struct GMAModel *model = bgObj->model;
    struct Effect bubble;
    Vec spC;

    mtxa_from_translate(&bgObj->pos);
    mtxa_rotate_z(bgObj->rotZ);
    mtxa_rotate_y(bgObj->rotY);
    mtxa_rotate_x(bgObj->rotX);
    mtxa_scale(&bgObj->scale);
    mtxa_tf_point(&model->boundSphereCenter, &effect->pos);
    effect->vel.x = effect->pos.x - effect->prevPos.x;
    effect->vel.y = effect->pos.y - effect->prevPos.y;
    effect->vel.z = effect->pos.z - effect->prevPos.z;
    effect->u_otherTimer++;
    if (!(effect->u_otherTimer & 0xF))
    {
        float radius = model->boundSphereRadius;

        memset(&bubble, 0, sizeof(bubble));
        bubble.type = 7;
        bubble.model = BG_WORK(struct BgWatWork *)->bubbleModel;
        spC.x = RAND_FLOAT() - 0.5f;
        spC.y = RAND_FLOAT() - 0.5f;
        spC.z = 0.0f;
        vec_set_len(radius * RAND_FLOAT(), &spC, &spC);
        mtxa_tf_vec(&spC, &spC);
        bubble.pos = effect->pos;
        bubble.pos.x += spC.x;
        bubble.pos.y += spC.y;
        bubble.pos.z += spC.z;
        bubble.vel.x = -2.5f * (effect->vel.x * (1.0f + RAND_FLOAT()));
        bubble.vel.y = -2.5f * (effect->vel.y * (1.0f + RAND_FLOAT()));
        bubble.vel.z = -2.5f * (effect->vel.z * (1.0f + RAND_FLOAT()));
        bubble.scale.x = radius * bgObj->scale.x * (1.0f + RAND_FLOAT());
        bubble.rotX = rand() & 0x7FFF;
        bubble.rotY = rand() & 0x7FFF;
        bubble.rotZ = rand() & 0x7FFF;
        bubble.timer = 60.0 * (1.0 + (2.0 * RAND_FLOAT()));
        spawn_effect(&bubble);
    }
}

void effect_bgwat_bubble_base_disp(struct Effect *effect) {}

void effect_bgwat_bubble_base_dest(struct Effect *effect) {}

void effect_bgwat_bubble_init(struct Effect *effect)
{
    effect->colorFactor = 0.0f;
    effect->scale.z = effect->scale.x;
    effect->scale.x = 0.0f;
}

void effect_bgwat_bubble_tick(struct Effect *effect)
{
    Vec spC;
    float temp_f2;
    float temp_f8;

    effect->vel.y += 0.012;
    mtxa_from_rotate_y(effect->rotY);
    mtxa_rotate_x(effect->rotX);
    mtxa_rotate_z(effect->rotZ);
    spC.x = 0.0f;
    spC.y = 1.0f;
    spC.z = 0.0f;
    mtxa_tf_vec(&spC, &spC);
    temp_f2 = -0.15 * mathutil_vec_dot_prod(&spC, &effect->vel);
    effect->vel.x += temp_f2 * spC.x;
    effect->vel.y += temp_f2 * spC.y;
    effect->vel.z += temp_f2 * spC.z;
    effect->vel.x *= 0.98;
    effect->vel.y *= 0.98;
    effect->vel.z *= 0.98;
    effect->pos.x += effect->vel.x;
    effect->pos.y += effect->vel.y;
    effect->pos.z += effect->vel.z;
    temp_f8 = 1.0 / (0.25 * effect->scale.x);
    effect->rotX += 16384.0f * (effect->vel.x * temp_f8);
    effect->rotY += 16384.0f * (effect->vel.y * temp_f8) + (1024.0f * (temp_f8 * (effect->vel.x + effect->vel.z)));
    effect->rotZ += 16384.0f * (temp_f8 * (effect->vel.z + effect->vel.y));
    if (effect->timer < 15.0)
    {
        effect->colorFactor *= 0.9;
        effect->scale.x *= 0.99f;
    }
    else
    {
        effect->colorFactor += 0.25 * (1.0 - effect->colorFactor);
        effect->scale.x += 0.1f * (effect->scale.z - effect->scale.x);
    }
}

void effect_bgwat_bubble_disp(struct Effect *effect)
{
    struct GMAModel *model;

    mtxa_from_mtx(g_ord_tbl_stuff->matrices[0]);
    mtxa_translate(&effect->pos);
    mtxa_sq_from_identity();
    mtxa_rotate_z((s16)((2048.0f * math_sin(effect->rotZ)) - 1024.0f));
    mtxa_scale_xyz(effect->scale.x, effect->scale.x, 1.0f);
    model = effect->model;
    if (g_is_sphere_visible_scaled(&model->boundSphereCenter, model->boundSphereRadius, effect->scale.x))
    {
        load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
        avdisp_set_z_mode(1, 3, 0);
        avdisp_set_scale_factor(effect->scale.x);
        if (effect->colorFactor < 1.0f)
        {
            avdisp_set_alpha(effect->colorFactor);
            avdisp_draw_model_unculled_sort_always(model);
        }
        else
            avdisp_draw_model_unculled_sort_auto(model);
        avdisp_set_z_mode(1, 3, 1);
    }
}

void effect_bgwat_bubble_dest(struct Effect *effect) {}

void effect_meteo_init(struct Effect *effect)
{
    effect->state = 0;
    effect->u_otherTimer = 0x12C;
    effect->unk8C.x = effect->scale.x * effect->scale.x;
}

void effect_meteo_tick(struct Effect *effect)
{
    float temp_f31 = 9.5f * effect->scale.x;
    float temp_f1 = *(float *)&BG_WORK(struct BgBnsWork *)->starpointCount;
    float temp_f0_2 = mathutil_vec_sq_len(&effect->pos);
    float temp_f31_2;
    float temp_f30;
    Vec sp24;
    Vec sp18;
    Vec spC;

    switch (effect->state)
    {
    case 0:
        effect->u_otherTimer -= 1;
        if (effect->u_otherTimer < 0)
            effect->state = 2;
        if (temp_f0_2 < 90000.0f * effect->unk8C.x)
            effect->state = 1;
        if (effect->colorFactor < 1.0f)
            effect->colorFactor += 0.011111111f;
        break;
    case 1:
        if (temp_f0_2 > 90000.0f * effect->unk8C.x)
            effect->state = 2;
        if (effect->colorFactor < 1.0f)
            effect->colorFactor += 0.011111111f;
        break;
    case 2:
        if (temp_f0_2 < 90000.0f * effect->unk8C.x)
            effect->state = 1;
        effect->colorFactor -= 0.011111111f;
        if (effect->colorFactor < 0.0f)
        {
            effect->colorFactor = 0.0f;
            g_poolInfo.effectPool.statusList[effect->poolIndex] = 3;
        }
        break;
    }
    effect->vel.x += 0.005f * (effect->unk98.x - effect->vel.x);
    effect->vel.y += 0.005f * (effect->unk98.y - effect->vel.y);
    effect->vel.z += 0.005f * (effect->unk98.z - effect->vel.z);
    effect->pos.x += effect->vel.x;
    effect->pos.y += effect->vel.y;
    effect->pos.z += effect->vel.z;
    effect->unk52 -= effect->unk52 >> 6;
    effect->unk54 -= effect->unk54 >> 6;
    effect->unk56 -= effect->unk56 >> 6;
    effect->rotX += effect->unkA4 + effect->unk52;
    effect->rotY += effect->unkA6 + effect->unk54;
    effect->rotZ += effect->unkA8 + effect->unk56;
    if (effect->pos.y < temp_f31 + 10.0f)
    {
        if (temp_f0_2 < (20.0f + temp_f31 + temp_f1) * (20.0f + temp_f31 + temp_f1))
        {
            sp24 = effect->prevPos;
            vec_set_len(temp_f1, &sp24, &sp24);
            if (sp24.y > -10.0f)
                sp24.y = -10.0f;
            if (fn_1_A74E4(&effect->prevPos, &effect->pos, temp_f31, &sp24, &sp24, 20.0f))
            {
                sp18.x = effect->pos.x - sp24.x;
                sp18.y = effect->pos.y - sp24.y;
                sp18.z = effect->pos.z - sp24.z;
                vec_normalize_len(&sp18);
                spC = effect->vel;
                temp_f31_2 = mathutil_vec_dot_prod(&spC, &sp18);
                if (temp_f31_2 < 0.0f)
                {
                    temp_f30 = (4096.0f * temp_f31_2) / effect->scale.x;
                    effect->unk52 += (s16)(temp_f30 * (RAND_FLOAT() - 0.5f));
                    effect->unk54 += (s16)(temp_f30 * (RAND_FLOAT() - 0.5f));
                    effect->unk56 += (s16)(temp_f30 * (RAND_FLOAT() - 0.5f));
                    temp_f31_2 *= -2.0f;
                    spC.x += temp_f31_2 * sp18.x;
                    spC.y += temp_f31_2 * sp18.y;
                    spC.z += temp_f31_2 * sp18.z;
                    effect->vel = spC;
                }
                effect->pos.x += effect->vel.x;
                effect->pos.y += effect->vel.y;
                effect->pos.z += effect->vel.z;
            }
        }
    }
}

void effect_meteo_disp(struct Effect *effect)
{
    struct GMAModel *temp_r31 = effect->model;

    if ((g_some_render_flag.flags & 4) && fn_1_815C(&effect->pos) < -(temp_r31->boundSphereRadius * effect->scale.x))
        return;
    mtxa_from_mtx(g_ord_tbl_stuff->matrices[0]);
    mtxa_translate(&effect->pos);
    mtxa_rotate_y(effect->rotY);
    mtxa_rotate_x(effect->rotX);
    mtxa_rotate_z(effect->rotZ);
    mtxa_scale_s(effect->scale.x);
    if (g_is_sphere_visible_scaled(&temp_r31->boundSphereCenter, temp_r31->boundSphereRadius, effect->scale.x))
    {
        avdisp_set_scale_factor(effect->scale.x);
        if (effect->colorFactor < 1.0f)
        {
            avdisp_set_alpha(effect->colorFactor);
            avdisp_draw_model_unculled_sort_always(temp_r31);
        }
        else
        {
            load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
            avdisp_draw_model_unculled_sort_auto(temp_r31);
        }
    }
}

void effect_meteo_dest(struct Effect *effect) {}

void effect_meteo_fix_init(struct Effect *effect) {}

void effect_meteo_fix_tick(struct Effect *effect)
{
    effect->unk52 -= effect->unk52 >> 6;
    effect->unk54 -= effect->unk54 >> 6;
    effect->unk56 -= effect->unk56 >> 6;
    effect->rotX += effect->unkA4 + effect->unk52;
    effect->rotY += effect->unkA6 + effect->unk54;
    effect->rotZ += effect->unkA8 + effect->unk56;
}

void effect_meteo_fix_disp(struct Effect *effect)
{
    struct GMAModel *temp_r31;

    mtxa_from_mtx(g_ord_tbl_stuff->matrices[0]);
    mtxa_translate(&effect->pos);
    mtxa_rotate_y(effect->rotY);
    mtxa_rotate_x(effect->rotX);
    mtxa_rotate_z(effect->rotZ);
    mtxa_scale_s(effect->scale.x);
    temp_r31 = effect->model;
    if (g_is_sphere_visible_scaled(&temp_r31->boundSphereCenter, temp_r31->boundSphereRadius, effect->scale.x))
    {
        avdisp_set_scale_factor(effect->scale.x);
        load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
        avdisp_draw_model_unculled_sort_auto(temp_r31);
    }
}

void effect_meteo_fix_dest(struct Effect *effect) {}

void effect_coliflash_init(struct Effect *effect)
{
    float temp_f1;

    effect->timer = 12;
    temp_f1 = math_sqrt((float)fabs(10.0f * effect->colorFactor));
    effect->scale.x = 0.25f * temp_f1;
    effect->scale.y = temp_f1;
    effect->unk18 = 1.0f;
    effect->cameras = 1 << effect->playerId;
    switch (world_theme)
    {
    case 5:
        effect->model = init_common_gma->modelEntries[96].model;
        break;
    default:
        effect->model = init_common_gma->modelEntries[95].model;
        break;
    }
}

void effect_coliflash_tick(struct Effect *effect)
{
    vec_to_euler_xy(&effect->unk8C, &effect->rotX, &effect->rotY);
    effect->rotX += 0x8000;
    effect->scale.x += 0.1f * (effect->scale.y - effect->scale.x);
    if (effect->timer < 7.5f)
    {
        float temp_f2 = effect->timer;
        temp_f2 = (temp_f2 - 1.0f) / temp_f2;
        effect->unk18 *= temp_f2;
    }
}

void effect_coliflash_disp(struct Effect *effect)
{
    Vec sp18;
    Vec spC;
    float temp_f31;
    float temp_f3;
    float temp_f30;
    float var_f29;
    float temp_f28;

    mtxa_from_mtxb_translate(&effect->pos);
    mathutil_mtxA_get_translate(&spC);
    temp_f31 = mathutil_vec_len(&spC);
    if (spC.z > -0.1f)
        return;
    var_f29 = effect->unk18;
    if (temp_f31 > 16.0f)
    {
        if (temp_f31 > 24.0f)
            return;
        var_f29 *= 0.125f * (24.0f - temp_f31);
        if (var_f29 < 0.003921569f)
            return;
    }
    mtxa_tf_vec(&effect->unk8C, &sp18);
    temp_f28 = vec_dot_normalized_safe(&sp18, &spC);
    if (temp_f28 > 0.0f)
        return;
    avdisp_set_z_mode(1, 6, 0);
    if (var_f29 != 1.0f)
        set_post_mult_color(var_f29, var_f29, var_f29, 1.0f);
    temp_f30 = 2.0f * effect->scale.x;
    temp_f3 = (temp_f31 + temp_f30) / temp_f31;
    sp18.x = spC.x * temp_f3;
    sp18.y = spC.y * temp_f3;
    sp18.z = spC.z * temp_f3;
    temp_f30 = temp_f30 * temp_f3;
    mathutil_mtxA_set_translate(&sp18);
    mtxa_rotate_y(effect->rotY);
    mtxa_rotate_x(effect->rotX);
    mtxa_scale_s(temp_f30);
    avdisp_set_scale_factor(temp_f30);
    avdisp_draw_model_culled_sort_always(effect->model);
    temp_f30 *= 0.6f;
    mtxa_sq_from_identity();
    mtxa_scale_s(temp_f30);
    if (temp_f28 > -0.2f)
    {
        var_f29 *= -5.0f * temp_f28;
        set_post_mult_color(var_f29, var_f29, var_f29, 1.0f);
    }
    avdisp_set_scale_factor(temp_f30);
    avdisp_draw_model_culled_sort_always(effect->model);
    avdisp_set_z_mode(1, 3, 1);
    if (var_f29 != 1.0f)
        fade_color_base_default();

}

void effect_coliflash_dest(struct Effect *effect) {}

void effect_bns_stg_star_init(struct Effect *effect)
{
    effect->timer = 60.0 * (0.75 + (0.5 * RAND_FLOAT()));
    effect->cameras = 1 << effect->playerId;
    effect->colorFactor = 1.0f;
}

void effect_bns_stg_star_tick(struct Effect *effect)
{
    Vec rnd;
    struct Effect starTail;

    effect->vel.y += -0.016;
    effect->pos.x += effect->vel.x;
    effect->pos.y += effect->vel.y;
    effect->pos.z += effect->vel.z;
    effect->rotZ += 0x400;
    if (effect->colorFactor > 0.5 && !(effect->timer & 1))
    {
        memset(&starTail, 0, sizeof(starTail));
        starTail.type = 12;
        starTail.pos = effect->pos;
        starTail.playerId = effect->playerId;
        rnd.x = 2.0 * (RAND_FLOAT() - 0.5);
        rnd.y = 2.0 * (RAND_FLOAT() - 0.5);
        rnd.z = 2.0 * (RAND_FLOAT() - 0.5);
        starTail.vel.x = (0.25 * effect->vel.x) + rnd.x;
        starTail.vel.y = (0.25 * effect->vel.y) + rnd.y;
        starTail.vel.z = (0.25 * effect->vel.z) + rnd.z;
        starTail.scale.x = effect->colorFactor * (0.25 + (0.25 * RAND_FLOAT()));
        spawn_effect(&starTail);
    }
    if (effect->timer < 15.0)
        effect->colorFactor = (float)effect->timer / 15.0;
}

void effect_bns_stg_star_disp(struct Effect *effect)
{
    struct GMAModel *dummy1;
    struct GMAModel *starlightModel;
    struct GMAModel *dummy2;
    struct GMAModel *shotstarModel;
    Vec pos;
    float temp_f1;
    float temp_f1_2;
    float scale;

    shotstarModel = BG_WORK(struct BgBnsWork *)->shotstarModel;
    starlightModel = BG_WORK(struct BgBnsWork *)->starlightModel;

    if ((g_some_render_flag.flags & 4) && fn_1_815C(&effect->pos) < -shotstarModel->boundSphereRadius)
        return;

    mtxa_from_mtx(g_ord_tbl_stuff->matrices[0]);
    mtxa_translate(&effect->pos);
    mathutil_mtxA_get_translate(&pos);
    if (pos.z > -0.1f)
        return;
    mtxa_rotate_y(effect->rotY);
    mtxa_rotate_x(effect->rotX);
    mtxa_rotate_z(effect->rotZ);
    avdisp_set_alpha(effect->colorFactor);
    avdisp_draw_model_culled_sort_always(shotstarModel);
    if (pos.z < -30.0f)
    {
        temp_f1 = (26.0 + pos.z) / pos.z;
        pos.x *= temp_f1;
        pos.y *= temp_f1;
        pos.z *= temp_f1;
        mtxa_from_translate(&pos);
        scale = 0.5 + (0.5 * effect->colorFactor);
        mtxa_scale_xyz(scale, scale, scale);
        avdisp_set_scale_factor(scale);
        temp_f1_2 = 0.75f * effect->colorFactor;
        set_post_mult_color(temp_f1_2, temp_f1_2, temp_f1_2, 1.0f);
        GXLoadPosMtxImm(mtxa->mtxA, 0);
        avdisp_draw_model_unculled_sort_auto(starlightModel);
        fade_color_base_default();
    }
}

void effect_bns_stg_star_dest(struct Effect *effect) {}

void effect_bns_stg_star_tail_init(struct Effect *effect)
{
    effect->timer = 60.0 * (0.5 + (0.25 * RAND_FLOAT()));
    effect->cameras = 1 << effect->playerId;
    effect->colorFactor = 1.0f;
}

void effect_bns_stg_star_tail_tick(struct Effect *effect)
{
    effect->vel.x *= 0.99f;
    effect->vel.y *= 0.99f;
    effect->vel.z *= 0.99f;
    effect->pos.x += effect->vel.x;
    effect->pos.y += effect->vel.y;
    effect->pos.z += effect->vel.z;
    if (effect->timer < 15.0)
        effect->colorFactor = (float)effect->timer / 15.0;
}

void effect_bns_stg_star_tail_disp(struct Effect *effect)
{
    Vec spC;
    struct GMAModel *model = init_common_gma->modelEntries[38].model;
    float scale = 52.68 * (effect->scale.x * effect->colorFactor);

    if ((g_some_render_flag.flags & 4) && fn_1_815C(&model->boundSphereCenter) < -(model->boundSphereRadius * scale))
        return;
    mtxa_from_mtx(g_ord_tbl_stuff->matrices[0]);
    mtxa_translate(&effect->pos);
    mtxa_sq_from_identity();
    mtxa_scale_xyz(scale, scale, scale);
    nl2ngc_set_scale(scale);
    mathutil_mtxA_get_translate(&spC);
    mtxa_rotate_z((s16)((325.0 * spC.x) + (655.0 * spC.y)));
    load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
    avdisp_draw_model_culled_sort_auto(model);
}

void effect_bns_stg_star_tail_dest(struct Effect *effect) {}


extern struct GMA *minigameGma;
struct Ape *get_ape_by_idx(int idx);
void *g_get_ape_anim_smth(struct Ape *ape, int bone);
extern u32 g_font_char_var_3;

struct Struct8009DBB0
{
    int unk0;
    Vec unk4;
    Vec unk10;
};

int fn_1_E67D0(Vec *, struct Struct8009DBB0 *, Vec *);
void fn_1_E678C(Mtx);
u32 g_smth_with_challenge_mode_var_and_3p(void);
void *g_yet_another_unk_draw_func(void *);

static inline float dist_xz_sq(register float a, register float b, register float c, register float d)
{
#ifdef __MWERKS__
    asm
    {
        fsubs a, a, c
        fsubs b, b, d
        fmuls a, a, a
        fmadds a, b, b, a
    }
#else
    a -= c;
    b -= d;
    a = a * a + b * b;
#endif
    return a;
}

void effect_bgmst_gen_cloud_init(struct Effect *effect)
{
    effect->timer = 120.0f * (1.0f + (0.5f * RAND_FLOAT()));
    effect->colorFactor = 0.0f;
    effect->state = 0;
}

void effect_bgmst_gen_cloud_tick(struct Effect *effect)
{
    float temp_f31 = 0.5f * (effect->model->boundSphereRadius * effect->scale.x);
    float var_f2;

    switch (effect->state)
    {
    case 0:
        if (dist_xz_sq(effect->unk64.x, effect->unk64.z, effect->pos.x, effect->pos.z) >
            (effect->unk8C.x + temp_f31) * (effect->unk8C.x + temp_f31))
            effect->state = 1;
        effect->vel.x *= 0.996f;
        effect->vel.y *= 0.996f;
        effect->vel.z *= 0.996f;
        break;
    case 1:
        effect->scale.x *= 1.005;
        effect->vel.x *= 0.98f;
        effect->vel.y *= 0.99f;
        effect->vel.z *= 0.98f;
        break;
    }
    effect->vel.y += -0.08f;
    effect->pos.x += effect->vel.x;
    effect->pos.y += effect->vel.y;
    effect->pos.z += effect->vel.z;
    if (effect->state == 0)
    {
        if (effect->pos.y < effect->unk64.y)
        {
            effect->pos.y = effect->unk64.y;
            if (effect->vel.y < 0.0f)
                effect->vel.y *= -0.5f;
        }
    }
    var_f2 = (384.0f + (768.0f * mathutil_vec_len(&effect->vel))) / temp_f31;
    if (effect->poolIndex & 1)
        var_f2 *= -1.0f;
    effect->unk56 += 0.1f * (var_f2 - effect->unk56);
    effect->rotZ += effect->unk56;
    if (effect->timer < 30.0f)
    {
        effect->colorFactor = effect->timer / 30.0f;
        effect->scale.x *= 1.03f;
    }
    else
        effect->colorFactor += 0.15f * (1.0f - effect->colorFactor);
}

void effect_bgmst_gen_cloud_disp(struct Effect *effect)
{
    struct GMAModel *model;
    float scale;

    if (g_some_render_flag.flags & 4)
        return;
    if (effect->colorFactor <= 0.0f)
        return;

    model = effect->model;
    scale = effect->scale.x;
    mtxa_from_mtx(g_ord_tbl_stuff->matrices[0]);
    mtxa_translate(&effect->pos);
    mtxa_sq_from_identity();
    mtxa_rotate_z(effect->rotZ);
    mtxa_scale_s(scale);
    GXLoadPosMtxImm(mtxa->mtxA, 0);
    avdisp_set_z_mode(1, 3, 0);
    avdisp_set_scale_factor(scale);
    if (effect->colorFactor < 1.0f)
    {
        avdisp_set_alpha(effect->colorFactor);
        avdisp_draw_model_culled_sort_always(model);
    }
    else
        avdisp_draw_model_culled_sort_auto(model);
    avdisp_set_z_mode(1, 3, 1);
}

void effect_bgmst_gen_cloud_dest(struct Effect *effect) {}

void effect_bgstm_rainripple_init(struct Effect *effect)
{
    float temp_f3;
    float temp_f5;

    effect->timer = 15.0f + (30.0f * RAND_FLOAT());
    effect->colorFactor = 1.0f;
    effect->scale.x = 0.0f;
    effect->scale.y = 0.016666668f * (1.0f + RAND_FLOAT());
    effect->vel.x = effect->unk80.x;
    effect->vel.y = effect->unk80.y;
    effect->vel.z = effect->unk80.z;
    temp_f5 = -(-0.008 * effect->unk70.y);
    effect->unk98.x = temp_f5 * effect->unk70.x;
    effect->unk98.y = temp_f5 * effect->unk70.y + -0.008;
    effect->unk98.z = temp_f5 * effect->unk70.z;
    temp_f5 = effect->unk70.x * effect->unk80.x + effect->unk70.y * effect->unk80.y + effect->unk70.z * effect->unk80.z;
    temp_f5 = fabs(temp_f5);
    temp_f3 = 1.0f - (5.0f * temp_f5);
    effect->timer *= temp_f3;
    temp_f3 = 1.0f / (1.0f + (3.0f * mathutil_vec_len(&effect->unk80)));
    effect->timer *= temp_f3;
    effect->scale.y *= temp_f3;
    if (effect->timer < 6.0f)
        effect->timer = 6;
}

void effect_bgstm_rainripple_tick(struct Effect *effect)
{
    effect->vel.x += effect->unk98.x;
    effect->vel.y += effect->unk98.y;
    effect->vel.z += effect->unk98.z;
    effect->vel.x += 0.1f * (effect->unk80.x - effect->vel.x);
    effect->vel.y += 0.1f * (effect->unk80.y - effect->vel.y);
    effect->vel.z += 0.1f * (effect->unk80.z - effect->vel.z);
    effect->pos.x += effect->vel.x;
    effect->pos.y += effect->vel.y;
    effect->pos.z += effect->vel.z;
    effect->scale.y *= 0.96f;
    effect->scale.x += effect->scale.y;
    if (effect->timer < 15)
        effect->colorFactor *= (float)effect->timer / (float)(effect->timer + 1);
}

void effect_bgstm_rainripple_disp(struct Effect *effect)
{
    Vec pos;
    float var_f31;
    float temp_f30;

    if (!(g_some_render_flag.flags & 4))
    {
        mtxa_from_mtxb_translate(&effect->pos);
        mathutil_mtxA_get_translate(&pos);
        var_f31 = 0.5 * (6.0f + pos.z);
        if (var_f31 < 0.0f)
            return;
        if (var_f31 > 1.0f)
            var_f31 = 1.0f;
        var_f31 *= effect->colorFactor;
        temp_f30 = (0.1f + pos.z) / pos.z;
        pos.x *= temp_f30;
        pos.y *= temp_f30;
        pos.z *= temp_f30;
        mathutil_mtxA_set_translate(&pos);
        mtxa_rotate_y(effect->rotY);
        mtxa_rotate_x(effect->rotX);
        mtxa_rotate_z(effect->rotZ);
        mtxa_rotate_x(-0x4000);
        mtxa_scale_s(effect->scale.x * temp_f30);
        avdisp_set_scale_factor(effect->scale.x * temp_f30);
        avdisp_set_z_mode(1, 3, 0);
        avdisp_set_alpha(var_f31);
        avdisp_draw_model_culled_sort_auto(effect->model);
        avdisp_set_z_mode(1, 3, 1);
    }
}

void effect_bgstm_rainripple_dest(struct Effect *effect) {}

void effect_bgmst_water_init(struct Effect *effect) {}

void effect_bgmst_water_tick(struct Effect *effect) {}

void effect_bgmst_water_disp(struct Effect *effect)
{
    struct StageBgModel *bgObj = (struct StageBgModel *)effect->model;
    struct BgMstWork *bgWork = BG_WORK(struct BgMstWork *);
    void *func;

    if (bgObj->flags & g_smth_with_challenge_mode_var_and_3p())
    {
        mtxa_from_mtx(g_ord_tbl_stuff->matrices[0]);
        mtxa_translate(&bgObj->pos);
        mtxa_rotate_z(bgObj->rotZ);
        mtxa_rotate_y(bgObj->rotY);
        mtxa_rotate_x(bgObj->rotX);
        mtxa_scale(&bgObj->scale);
        GXLoadNrmMtxImm(mtxa->mtxA, 0);
        mtxa_to_mtx(bgWork->specMtx);
        mtxa_translate_xyz(0.0f, -20.0f, 0.0f);
        mtxa_scale_xyz(1.0f, 0.0f, 1.0f);
        GXLoadPosMtxImm(mtxa->mtxA, 0);
        func = g_yet_another_unk_draw_func(bgWork->waterEnvFunc);
        avdisp_draw_model_culled_sort_never(bgWork->waterModel);
        g_yet_another_unk_draw_func(func);
    }
}

void effect_bgmst_water_dest(struct Effect *effect) {}

void effect_commendfrag_init(struct Effect *effect)
{
    if (effect->timer == 0)
        effect->timer = 210.0 + (60.0f * RAND_FLOAT());
    if (effect->model == NULL)
    {
        if (modeCtrl.gameType == 3)
        {
            u32 modelIndex = s_commendPieceModels[(u32)(rand() & 0x7FFF) % ARRAY_COUNT(s_commendPieceModels)];
            effect->model = minigameGma->modelEntries[modelIndex].model;
        }
        else
        {
            u32 modelIndex = s_paperPieceModels[(u32)(rand() & 0x7FFF) % ARRAY_COUNT(s_paperPieceModels)];
            effect->model = init_common_gma->modelEntries[modelIndex].model;
        }
    }
    effect->state = 0;
    if (effect->scale.x == 0.0f)
        effect->scale.x = 0.2 + (0.3 * RAND_FLOAT());
}

void effect_commendfrag_tick(struct Effect *effect)
{
    Vec sp58;
    struct Struct8009DBB0 sp3C;
    Vec sp30;
    Vec sp24;
    Vec sp18;
    Vec spC;
    float dummy_fpr1;
    float dummy_fpr2;
    float dummy_fpr3;
    float dummy_fpr4;
    float dummy_fpr5;
    float dummy_fpr6;
    float dummy_fpr7;
    float dummy_fpr8;
    float dummy_fpr9;
    float dummy_fpr10;
    float dummy_fpr11;
    float dummy_fpr12;
    float dummy_fpr13;
    float dummy_fpr14;
    float dummy_fpr15;
    float dummy_fpr16;
    float dummy_fpr17;
    float dummy_fpr18;
    float dummy_fpr19;
    float dummy_fpr20;
    float tA;
    float tB;
    float temp_f2;
    float temp_f3;
    float temp_f2_3;
    float temp_f4;
    float tC;
    s16 temp_r31;
    s16 temp_r29;

    effect->vel.y += -0.004;
    mtxa_from_rotate_y(effect->rotY);
    mtxa_rotate_x(effect->rotX);
    mtxa_rotate_z(effect->rotZ);
    sp58.x = 0.0f;
    sp58.y = 1.0f;
    sp58.z = 0.0f;
    mtxa_tf_vec(&sp58, &sp58);
    tA = -0.7 * mathutil_vec_dot_prod(&sp58, &effect->vel);
    effect->vel.x += sp58.x * tA;
    effect->vel.y += sp58.y * tA;
    effect->vel.z += sp58.z * tA;
    effect->vel.x *= 0.95;
    effect->vel.y *= 0.95;
    effect->vel.z *= 0.95;
    effect->pos.x += effect->vel.x;
    effect->pos.y += effect->vel.y;
    effect->pos.z += effect->vel.z;
    tB = 2560.0f / effect->scale.x;
    effect->unk52 += (s16)(tB * (effect->vel.x + effect->vel.y));
    effect->unk54 += (s16)(tB * (effect->vel.y + effect->vel.z));
    effect->unk56 += (s16)(tB * (effect->vel.z + effect->vel.x));
    effect->unk52 -= effect->unk52 >> 5;
    effect->unk54 -= effect->unk54 >> 5;
    effect->unk56 -= effect->unk56 >> 5;
    effect->rotX += effect->unk52;
    effect->rotY += effect->unk54;
    effect->rotZ += effect->unk56;

    if (!(effect->timer & 0xF))
    {
        effect->state = 0;
        if (fn_1_E67D0(&effect->pos, &sp3C, &effect->unk80) != 0
         && sp3C.unk4.y > effect->pos.y - 1.0)
        {
            effect->state = 1;
            effect->unk64.x = sp3C.unk4.x;
            effect->unk64.y = sp3C.unk4.y;
            effect->unk64.z = sp3C.unk4.z;
            effect->unk70.x = sp3C.unk10.x;
            effect->unk70.y = sp3C.unk10.y;
            effect->unk70.z = sp3C.unk10.z;
        }
    }

    switch (effect->state)
    {
    case 0:
        return;
    }

    sp24 = effect->unk70;
    sp30.x = 0.0f;
    sp30.y = 1.0f;
    sp30.z = 0.0f;
    mtxa_from_rotate_y(effect->rotY);
    mtxa_rotate_x(effect->rotX);
    mtxa_rotate_z(effect->rotZ);
    mtxa_tf_vec(&sp30, &sp30);

    temp_f3 = (sp30.x * sp24.x) + (sp30.y * sp24.y) + (sp30.z * sp24.z);
    temp_f2_3 = (1.1920929e-7f * effect->poolIndex) + (0.1 * (1.0 - fabs(temp_f3)));
    sp18.x = effect->pos.x - effect->unk64.x;
    sp18.y = effect->pos.y - effect->unk64.y;
    sp18.z = effect->pos.z - effect->unk64.z;
    temp_f4 = (sp24.x * sp18.x) + (sp24.y * sp18.y) + (sp24.z * sp18.z);
    if (temp_f4 < temp_f2_3 + 0.01)
    {
        temp_f2 = temp_f2_3 + 0.01 - temp_f4;
        effect->pos.x += temp_f2 * sp24.x;
        effect->pos.y += temp_f2 * sp24.y;
        effect->pos.z += temp_f2 * sp24.z;

        temp_f2 = (effect->vel.x * sp24.x) + (effect->vel.y * sp24.y) + (effect->vel.z * sp24.z);
        if (temp_f2 < 0.0)
        {
            temp_f2 = -temp_f2;
            effect->vel.x += temp_f2 * sp24.x;
            effect->vel.y += temp_f2 * sp24.y;
            effect->vel.z += temp_f2 * sp24.z;
            temp_f4 = fabs(temp_f3) * (1.0 / 8.0);
            effect->vel.x += (temp_f4 * (effect->unk80.x - effect->vel.x));
            effect->vel.y += (temp_f4 * (effect->unk80.y - effect->vel.y));
            effect->vel.z += (temp_f4 * (effect->unk80.z - effect->vel.z));
            temp_f2 = temp_f2 / 2.0;
            effect->vel.x += temp_f2 * sp24.x;
            effect->vel.y += temp_f2 * sp24.y;
            effect->vel.z += temp_f2 * sp24.z;
        }
        spC = sp24;
        if (temp_f3 < 0.0)
        {
            spC.x = -spC.x;
            spC.y = -spC.y;
            spC.z = -spC.z;
        }
        sp30.x = (0.05 * spC.x) + (0.95 * sp30.x);
        sp30.y = (0.05 * spC.y) + (0.95 * sp30.y);
        sp30.z = (0.05 * spC.z) + (0.95 * sp30.z);
        vec_normalize_len(&sp30);
        temp_r29 = effect->rotX;
        temp_r31 = effect->rotZ;
        mtxa_from_rotate_y(-effect->rotY);
        mtxa_rotate_x(0);
        mtxa_tf_vec(&sp30, &sp30);
        effect->rotX = math_atan2(sp30.z, sp30.y);
        effect->rotZ = -math_atan2(sp30.x, math_sqrt(mathutil_sum_of_sq_2(sp30.z, sp30.y)));
        effect->unk52 >>= 2;
        effect->unk52 += (effect->rotX - temp_r29) >> 2;
        effect->unk54 -= effect->unk54 >> 4;
        effect->unk56 >>= 2;
        effect->unk56 += (effect->rotZ - temp_r31) >> 2;
    }
}

void effect_commendfrag_disp(struct Effect *effect)
{
    Mtx sp10;
    float temp_f30;
    float var_f31;
    float new_var;

    if (g_some_render_flag.flags & 4)
    {
        temp_f30 = 0.1 * effect->scale.x;
        var_f31 = temp_f30 + fn_1_80D4(&effect->pos);
        if (var_f31 <= 0.0f)
            return;
        if (var_f31 > temp_f30)
            var_f31 = 1.0f;
        else
            var_f31 *= (1.0f / temp_f30);
    }
    else
        var_f31 = 1.0f;

    mtxa_from_mtxb();
    fn_1_E678C(sp10);
    mtxa_mult_right(sp10);
    mtxa_translate(&effect->pos);
    mtxa_rotate_y(effect->rotY);
    mtxa_rotate_x(effect->rotX);
    mtxa_rotate_z(effect->rotZ);
    new_var = g_current_camera->sub28.view.invTanHalfFov * ((120.0f * g_current_camera->sub28.viewportH) * (0.11f * effect->scale.x));
    var_f31 *= 2.0f * (new_var + mtxa->mtxA[2][3]);
    if (var_f31 < 0.0)
        return;
    if (var_f31 > 1.0)
        var_f31 = 1.0f;
    mtxa_scale_s(effect->scale.x);
    avdisp_set_scale_factor(effect->scale.x);
    if (effect->timer < 12)
        var_f31 *= 0.083333336f * effect->timer;
    if (var_f31 < 1.0)
    {
        avdisp_set_alpha(var_f31);
        avdisp_draw_model_culled_sort_always(effect->model);
    }
    else
    {
        load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
        avdisp_draw_model_culled_sort_auto(effect->model);
    }
}

void effect_commendfrag_dest(struct Effect *effect) {}

void effect_banana_drop_init(struct Effect *effect)
{
    effect->flags |= 2;
    effect->timer = 80;
    effect->cameras = 1 << effect->playerId;
    effect->pos.x = 0.0f;
    effect->pos.y = -0.2f;
    effect->pos.z = 0.0f;
    effect->vel.x = 0.0f;
    effect->vel.y = 0.15f;
    effect->vel.z = -0.05f;
    effect->rotX = 0;
    effect->rotY = 0;
    effect->rotZ = 0;
    effect->unk52 = 0;
    effect->unk54 = 0;
    effect->unk56 = 0;
    effect->state = 0;
}

void effect_banana_drop_tick(struct Effect *effect)
{
    Vec spC;

    effect->pos.x += effect->vel.x;
    effect->pos.y += effect->vel.y;
    effect->pos.z += effect->vel.z;
    effect->vel.y -= 0.0098;
    mtxa_from_translate((Vec *)effect->model);
    mtxa_rotate_y(effect->unkA6 + (effect->u_otherTimer * 0x1999));
    mtxa_tf_point(&effect->pos, &spC);
    if (spC.x < -10.0 || spC.x > 10.0)
        effect->state |= 2;
    if (!(effect->state & 1) && (effect->pos.y < -0.25))
    {
        if (effect->state & 2)
            effect->state |= 1;
        else
        {
            effect->pos.y = -0.25f;
            effect->vel.y *= -0.6;
            effect->unk52 = (rand() & 0x7FFF) >> 4;
            effect->unk54 = (rand() & 0x7FFF) >> 5;
            effect->unk56 = (rand() & 0x7FFF) >> 5;
        }
    }
    effect->rotX += effect->unk52;
    effect->rotY += effect->unk54;
    effect->rotZ += effect->unk56;
}

void effect_banana_drop_disp(struct Effect *effect)
{
    if (effect->timer >= 30 || !(g_font_char_var_3 & 2))
    {
        mtxa_from_mtxb_translate((Vec *)effect->model);
        mtxa_rotate_y(effect->unkA6 + (effect->u_otherTimer * 0x1999));
        mtxa_translate(&effect->pos);
        mtxa_sq_from_identity();
        mtxa_rotate_y(effect->rotY);
        mtxa_rotate_x(effect->rotX);
        mtxa_rotate_z(effect->rotZ);
        mtxa_scale_s(0.65f);
        load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
        avdisp_draw_model_culled_sort_auto(init_common_gma->modelEntries[65].model);
        if (!(effect->state & 3))
        {
            avdisp_set_z_mode(1, 3, 0);
            set_post_mult_color(0.3f, 0.3f, 0.3f, 0.3f);
            mtxa_from_mtxb_translate((Vec *)effect->model);
            mtxa_rotate_y(effect->unkA6 + (effect->u_otherTimer * 0x1999));
            mtxa_translate_xyz(effect->pos.x, -0.45f, effect->pos.z);
            mtxa_rotate_x(0x4000);
            mtxa_scale_s(0.25f);
            load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
            avdisp_draw_model_culled_sort_never(init_common_gma->modelEntries[103].model);
            set_post_mult_color(1.0f, 1.0f, 1.0f, 1.0f);
            avdisp_set_z_mode(1, 3, 1);
        }
    }
}

void effect_banana_drop_dest(struct Effect *effect) {}

void effect_bgend_water_init(struct Effect *effect) {}

void effect_bgend_water_tick(struct Effect *effect) {}

void effect_bgend_water_disp(struct Effect *effect)
{
    struct StageBgModel *bgObj = (struct StageBgModel *)effect->model;
    struct BgEndWork *bgWork = BG_WORK(struct BgEndWork *);
    void *func;

    if (bgObj->flags & g_smth_with_challenge_mode_var_and_3p())
    {
        mtxa_from_mtx(g_ord_tbl_stuff->matrices[0]);
        mtxa_translate(&bgObj->pos);
        mtxa_rotate_z(bgObj->rotZ);
        mtxa_rotate_y(bgObj->rotY);
        mtxa_rotate_x(bgObj->rotX);
        mtxa_scale(&bgObj->scale);
        func = g_yet_another_unk_draw_func(bgWork->unk18);
        mtxa_push();
        GXLoadNrmMtxImm(mtxa->mtxA, 0);
        mtxa_to_mtx(bgWork->unk64);
        mtxa_translate_xyz(0.0f, -20.0f, 0.0f);
        mtxa_scale_xyz(1.0f, 0.0f, 1.0f);
        GXLoadPosMtxImm(mtxa->mtxA, 0);
        avdisp_draw_model_culled_sort_never(bgWork->waterModel);
        mtxa_pop();
        g_yet_another_unk_draw_func(func);
    }
}

void effect_bgend_water_dest(struct Effect *effect) {}

void effect_bgbtmfire_ptcl_init(struct Effect *effect)
{
    effect->scale.y = effect->scale.x;
    effect->scale.x = 0.0f;
}

void effect_bgbtmfire_ptcl_tick(struct Effect *effect)
{
    float len;

    effect->vel.y += 0.024f;
    effect->vel.x *= 0.99f;
    effect->vel.y *= 0.99f;
    effect->vel.z *= 0.99f;
    len = mathutil_vec_len(&effect->vel);
    effect->pos.x += effect->vel.x + len * (RAND_FLOAT() - 0.5f);
    effect->pos.y += effect->vel.y + len * (RAND_FLOAT() - 0.5f);
    effect->pos.z += effect->vel.z + len * (RAND_FLOAT() - 0.5f);
    effect->unk56 = 0.98f * effect->unk56;
    effect->rotZ += effect->unk56;
    effect->scale.x += 0.1f * (effect->scale.y - effect->scale.x);
    effect->colorFactor = 0.1f + 0.9f * RAND_FLOAT();
    if (effect->timer < 15.0f)
    {
        len = (float)effect->timer / (float)(effect->timer + 1);
        effect->unk18 *= len;
        len *= len;
        effect->unk1C *= len;
        effect->unk20 *= len;
    }
}

void effect_bgbtmfire_ptcl_disp(struct Effect *effect)
{
    struct GMAModel *model = effect->model;
    float factor = effect->colorFactor;

    mtxa_from_mtx(g_ord_tbl_stuff->matrices[0]);
    mtxa_translate(&effect->pos);
    mtxa_sq_from_identity();
    mtxa_rotate_z(effect->rotZ);
    mtxa_scale_s(effect->scale.x * (0.8f + 0.2f * factor));
    if (g_is_sphere_visible_scaled(&model->boundSphereCenter, model->boundSphereRadius, effect->scale.x))
    {
        set_post_mult_color(effect->unk18 * factor, effect->unk1C * factor, effect->unk20 * factor, 1.0f);
        avdisp_set_z_mode(1, 3, 0);
        avdisp_draw_model_unculled_sort_always(model);
        avdisp_set_z_mode(1, 3, 1);
        fade_color_base_default();
    }
}

void effect_bgbtmfire_ptcl_dest(struct Effect *effect) {}

void effect_piyo_init(struct Effect *effect)
{
    effect->timer = 60;
}

void effect_piyo_tick(struct Effect *effect)
{
    struct Ape *ape;

    effect->timer = 60;
    ape = get_ape_by_idx(effect->playerId);
    if (ape == NULL || ape->unk88 != effect->u_otherTimer)
        g_poolInfo.effectPool.statusList[effect->poolIndex] = 3;
}

void effect_piyo_disp(struct Effect *effect)
{
    Vec sp8;
    Mtx sp14;
    struct Ape *ape;

    ape = get_ape_by_idx(effect->playerId);
    if (ape == NULL || ape->unk88 != effect->u_otherTimer)
        return;
    mtxa_from_quat(&ape->rot);
    mtxa_to_mtx(sp14);
    mtxa_from_translate(&ape->pos);
    mtxa_mult_right(sp14);
    mtxa_scale_s(ape->scale);
    mtxa_mult_right(g_get_ape_anim_smth(ape, 3));
    mtxa_tf_point_xyz(0.0f, 0.0f, 0.15f, &sp8);
    mtxa_from_mtxb_translate(&sp8);
    mtxa_rotate_y(g_font_char_var_3 << 10);
    mtxa_push();
    mtxa_translate_xyz(0.0f, 0.35f, 0.2f);
    mtxa_sq_from_identity();
    mtxa_scale_s(0.005f);
    load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
    avdisp_draw_model_unculled_sort_auto(init_common_gma->modelEntries[37].model);
    mtxa_pop();
    mtxa_translate_xyz(0.0f, 0.35f, -0.2f);
    mtxa_sq_from_identity();
    mtxa_scale_s(0.005f);
    load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
    avdisp_draw_model_unculled_sort_auto(init_common_gma->modelEntries[37].model);
}

void effect_piyo_dest(struct Effect *effect) {}
