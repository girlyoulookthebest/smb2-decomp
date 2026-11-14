/* Master background: drifting cloud layers, paradise clouds shedding mist, water env. */

#include "types.h"
#include "avdisp.h"
#include "background.h"
#include "camera.h"
#include "effect.h"
#include "gx.h"
#include "libc.h"
#include "mathutil.h"
#include "mode.h"
#include "nl2ngc.h"
#include "rand.h"
#include "stage.h"
#include "bg_work.h"

extern struct { u32 flags; } g_some_render_flag;

void *OSAllocFromHeap(void *heap, u32 size);
void OSFreeToHeap(void *heap, void *ptr);
void GXInitTexObj(GXTexObj *obj, void *image, u16 width, u16 height, int format, int wrapS, int wrapT, u8 mipmap);
void GXSetTexCopySrc_cached(u16 left, u16 top, u16 width, u16 height);
void GXSetTexCopyDst_cached(u16 width, u16 height, int format, u8 mipmap);
void GXCopyTex(void *dest, u8 clear);
void GXSetNumChans_cached(u8 n);
void GXSetNumTexGens_cached(u8 n);
void GXSetNumTevStages_cached(u8 n);
void GXSetNumIndStages_cached(u8 n);
void GXSetCullMode_cached(int mode);
void GXSetTevKColorSel_cached(int stage, int sel);
void GXSetIndTexOrder_cached(int stage, int coord, int map);
void g_GXSetIndTexMtx_wrapper(int id, float mtx[2][3], int scale);
void g_something_with_GX_vtx_desc(u32 attrs);
void *g_yet_another_unk_draw_func(void *func);
void g_smth_with_bg_model_names(struct BgModelSearch *search, int (*callback)(int, struct GMAModelEntry *));
void fn_1_E2F1C(void);
struct OrdTblNode *ord_tbl_get_entry_for_pos(Vec *pos);
void *ord_tbl_alloc_node(u32 size);
void fn_1_CD418(struct OrdTblNode *list, struct OrdTblNode *node);
void vec_set_len(float len, Vec *src, Vec *dst);
void vec_to_euler_xy(Vec *v, s16 *x, s16 *y);
extern void *stage_heap;
extern void *lbl_801988E0;
void GXSetTexCoordGen2_cached(int dst, int func, int src, int mtx, int normalize, int postMtx);
static inline void tex_coord_gen(int dst, int func, int src, int mtx)
{
    GXSetTexCoordGen2_cached(dst, func, src, mtx, 0, 0x7D);
}

void debug_textdraw_set_pos(int x, int y);
void fn_1_7385C(void);

void GXSetTevIndirect_cached(int stage, int indStage, int format, int biasSel, int mtxSel, int wrapS, int wrapT,
    int addPrev, int utcLod, int alphaSel);
void GXSetTevSwapMode_cached(int stage, int rasSel, int texSel);
void g_GXSetTevOrder_wrapper(int stage, int coord, int map, int color);
void GXLoadTexObj_cached(GXTexObj *obj, int map);
void GXLoadTexMtxImm(Mtx m, int id, int type);
void GXSetTevColorIn_cached(int stage, int a, int b, int c, int d);
void GXSetTevColorOp_cached(int stage, int op, int bias, int scale, u8 clamp, int outReg);
void GXSetTevAlphaIn_cached(int stage, int a, int b, int c, int d);
void GXSetTevAlphaOp_cached(int stage, int op, int bias, int scale, u8 clamp, int outReg);
void GXLoadPosMtxImm(Mtx m, int id);
void GXLoadNrmMtxImm(Mtx m, int id);

static struct BgModelSearch lbl_1_data_318F4[] =
{
    { BG_MDL_CMP_PREFIX, "MST_STG_CLOUD_" },
    { BG_MDL_CMP_PREFIX, "MST_MINI_CLOUD_" },
    { BG_MDL_CMP_PREFIX, "MST_SPRA_CLOUD_" },
    { BG_MDL_CMP_FULL,   "MST_YOUSAI_WATER_C" },
    { BG_MDL_CMP_FULL,   "MST_YOUSAI_WATER_SPEC" },
    { BG_MDL_CMP_END,    NULL },
};

static struct BgModelSearch lbl_1_data_3197C[] =
{
    { BG_MDL_CMP_PREFIX, "MST_BOTTOMCLOUD_" },
    { BG_MDL_CMP_PREFIX, "MST_PARADISE_CLOUD_" },
    { BG_MDL_CMP_PREFIX, "MST_CLOUD_GEN_" },
    { BG_MDL_CMP_FULL,   "MST_PARADISE_A" },
    { BG_MDL_CMP_FULL,   "MST_PARADISE_B" },
    { BG_MDL_CMP_END,    NULL },
};

void fn_1_7CDC8(struct BallEnvArg *);
int fn_1_7D13C(int, struct GMAModelEntry *);
int fn_1_7D1E0(int, struct StageBgModel *);

void bg_mst_init(void)
{
    struct BgMstWork *work = world_theme.work;
    Vec sp8;
    int i;
    struct BgMstCloud *cloud;
    int j;
    struct BgMstParadise *paradise;

    bg_init_base();
    if (work->initialized == 0)
    {
        work->miniCloudCount = 0;
        g_smth_with_bg_model_names(lbl_1_data_318F4, fn_1_7D13C);
        work->initialized = 1;
    }
    work->cloudCount = 0;
    work->paradiseCount = 0;
    g_init_stagedef_bg_fg_models(stagedef->bgModels, stagedef->bgModelCount, lbl_1_data_3197C, fn_1_7D1E0);
    g_init_stagedef_bg_fg_models(stagedef->fgModels, stagedef->fgModelCount, lbl_1_data_3197C, fn_1_7D1E0);
    if (work->cloudCount != 0)
    {
        work->cloudVelRerolled = 0;
        cloud = work->clouds;
        for (i = work->cloudCount; i > 0; i--, cloud++)
        {
            cloud->texPos.x = RAND_FLOAT();
            cloud->texPos.y = RAND_FLOAT();
            cloud->texPos.z = RAND_FLOAT();
            mtxa_from_rotate_z(8192.0f * (RAND_FLOAT() - 0.5f));
            sp8.x = 0.0011904762f * (0.9f + 0.2f * RAND_FLOAT());
            sp8.y = 0.0f;
            sp8.z = 0.0f;
            mtxa_tf_vec(&sp8, &cloud->texTargetVel);
            cloud->texVel = cloud->texTargetVel;
        }
    }
    if (work->paradiseCount != 0)
    {
        paradise = work->paradises;
        for (j = work->paradiseCount; j > 0; j--, paradise++)
        {
            paradise->spawnTimer = RAND_FLOAT();
            paradise->spawnRate = 0.1f + 0.1f * RAND_FLOAT();
            paradise->spawnRateBase = paradise->spawnRate;
            paradise->angle = rand() & 0x7FFF;
            paradise->angleVel = (int)(128.0f * RAND_FLOAT()) + 0x40;
            if (j & 1)
                paradise->angleVel = -paradise->angleVel;
            paradise->texVel.x = 0.004166667f * (1.0f + 0.2f * RAND_FLOAT());
            paradise->texVel.y = 0.0013888889f * (RAND_FLOAT() - 0.5f);
            paradise->texScale.x = 0.9f + 0.2f * RAND_FLOAT();
            paradise->texScale.y = 0.9f + 0.2f * RAND_FLOAT();
            paradise->texScale.z = 1.0f;
        }
    }
    work->waterEnvFunc = fn_1_7CDC8;
    work->waterVel.x = 0.005555555555555556 * (0.8f + 0.4f * RAND_FLOAT());
    work->waterVel.y = 0.005555555555555556 * (0.8f + 0.4f * RAND_FLOAT());
    work->waterBaseScale.x = 5.0f * (1.0f + RAND_FLOAT());
    work->waterBaseScale.y = 5.0f * (1.0f + RAND_FLOAT());
    work->waterBaseScale.z = 1.0f;
    work->specVel.x = 0.001388888888888889 * (0.9f + 0.2f * RAND_FLOAT());
    work->specVel.y = 0.001388888888888889 * (0.9f + 0.2f * RAND_FLOAT());
}

void bg_mst_tick(void)
{
    struct BgMstWork *work = world_theme.work;
    u32 mask = g_smth_with_challenge_mode_var_and_3p();
    Vec sp8;
    struct Effect effect;
    int i;
    struct BgMstCloud *cloud;
    int reroll;
    int j;
    struct BgMstParadise *paradise;
    float scale;

    bg_tick_base();
    if (is_game_paused_and_in_view_stage())
        return;
    if (work->cloudCount != 0)
    {
        if (work->cloudVelRerolled == 0 && mode_info.timerCurr < 660.0f)
        {
            reroll = 1;
            work->cloudVelRerolled = 1;
        }
        else
            reroll = 0;
        cloud = work->clouds;
        for (i = work->cloudCount; i > 0; i--, cloud++)
        {
            if (reroll != 0)
            {
                mtxa_from_rotate_z(8192.0f * (RAND_FLOAT() - 0.5f));
                sp8.x = 0.0023809525f * (0.9f + 0.2f * RAND_FLOAT());
                sp8.y = 0.0f;
                sp8.z = 0.0f;
                mtxa_tf_vec(&sp8, &cloud->texTargetVel);
            }
            cloud->texVel.x += 0.05f * (cloud->texTargetVel.x - cloud->texVel.x);
            cloud->texVel.y += 0.05f * (cloud->texTargetVel.y - cloud->texVel.y);
            cloud->texPos.x += cloud->texVel.x;
            cloud->texPos.y += cloud->texVel.y;
            mtxa_from_translate(&cloud->texPos);
            mtxa_to_mtx(cloud->texMtx);
        }
    }
    if (work->paradiseCount != 0)
    {
        memset(&effect, 0, sizeof(effect));
        effect.type = 0xD;
        effect.model = work->miniClouds[(rand() & 0x7FFF) % work->miniCloudCount];
        paradise = work->paradises;
        for (j = work->paradiseCount; j > 0; j--, paradise++)
        {
            if (paradise->model->flags & mask)
            {
                paradise->angle += paradise->angleVel;
                paradise->spawnTimer += paradise->spawnRate;
                while (paradise->spawnTimer > 1.0f)
                {
                    effect.pos = paradise->model->pos;
                    sp8.x = 0.0f;
                    sp8.y = 2.5f * (paradise->spawnRate * (0.5f + RAND_FLOAT()));
                    sp8.z = 7.5f * (paradise->spawnRate * (1.0f + RAND_FLOAT()));
                    if (rand() & 1)
                        mtxa_from_rotate_y(paradise->angle);
                    else
                        mtxa_from_rotate_y(paradise->angle + 0x8000);
                    mtxa_tf_vec(&sp8, &sp8);
                    effect.vel = sp8;
                    vec_set_len(25.0f, &sp8, &sp8);
                    effect.pos.x += sp8.x;
                    effect.pos.y += sp8.y;
                    effect.pos.z += sp8.z;
                    effect.rotZ = rand() & 0x7FFF;
                    effect.unk56 = 768.0f * (RAND_FLOAT() - 0.5f);
                    effect.scale.x = 0.25f + 0.5f * RAND_FLOAT();
                    effect.unk64 = paradise->model->pos;
                    effect.unk70.x = 0.0f;
                    effect.unk70.y = 1.0f;
                    effect.unk70.z = 0.0f;
                    effect.unk8C.x = 100.0f * paradise->model->scale.x;
                    spawn_effect(&effect);
                    paradise->spawnTimer -= 1.0f;
                }
                paradise->spawnRate += 0.05f * (paradise->spawnRate - paradise->spawnRateBase);
                paradise->texPos.x += paradise->texVel.x;
                paradise->texPos.y += paradise->texVel.y;
                mtxa_from_translate(&paradise->texPos);
                mtxa_scale(&paradise->texScale);
                mtxa_to_mtx(paradise->texMtxA);
                mtxa_scale_xyz(1.05f, -1.0f, 1.0f);
                mtxa_to_mtx(paradise->texMtxB);
            }
        }
    }
    work->waterPos.x += work->waterVel.x;
    work->waterPos.y += work->waterVel.y;
    scale = 1.0f + 0.05f * math_sin(world_theme.unkA4 << 8);
    work->waterScale.x = work->waterBaseScale.x * scale;
    work->waterScale.y = work->waterBaseScale.y * scale;
    work->waterScale.z = 1.0f;
    work->specPos.x += work->specVel.x;
    work->specPos.y += work->specVel.y;
}

void bg_mst_dest(void) {}

void bg_mst_disp(void)
{
    int i;
    struct BgMstWork *work = world_theme.work;
    struct BgMstCloud *cloud;
    struct BgMstParadise *paradise;
    struct StageBgModel *model;
    u32 mask = g_smth_with_challenge_mode_var_and_3p();
    Vec sp8;

    cloud = work->clouds;
    for (i = work->cloudCount; i > 0; i--, cloud++)
        cloud->model->flags = cloud->model->flags & ~0x10000;
    if (work->cloudCount != 0)
    {
        g_something_with_texture_scroll_2(1);
        cloud = work->clouds;
        for (i = work->cloudCount; i > 0; i--, cloud++)
        {
            model = cloud->model;
            if (model->flags & mask)
            {
                fn_1_10A4CC(0, cloud->texMtx);
                mtxa_from_mtx(g_ord_tbl_stuff->matrices[0]);
                mtxa_translate(&model->pos);
                mtxa_rotate_z(model->rotZ);
                mtxa_rotate_y(model->rotY);
                mtxa_rotate_x(model->rotX);
                mtxa_scale(&model->scale);
                GXLoadPosMtxImm(mtxa->mtxA, 0);
                GXLoadNrmMtxImm(mtxa->mtxA, 0);
                avdisp_draw_model_culled_sort_auto(model->model);
            }
        }
        g_something_with_texture_scroll_2(0);
    }
    bg_disp_base();
    if ((g_some_render_flag.flags & (1 << 2)) == 0 && work->paradiseCount != 0)
    {
        avdisp_set_z_mode(1, 3, 0);
        g_something_with_texture_scroll_2(1);
        paradise = work->paradises;
        for (i = work->paradiseCount; i > 0; i--, paradise++)
        {
            model = paradise->model;
            if (model->flags & mask)
            {
                mtxa_from_mtx(g_ord_tbl_stuff->matrices[0]);
                mtxa_translate(&model->pos);
                mtxa_rotate_z(model->rotZ);
                mtxa_rotate_y(model->rotY);
                mtxa_rotate_x(model->rotX);
                mtxa_scale(&model->scale);
                mtxa_rotate_y(paradise->angle - (paradise->angleVel << 6));
                mtxa_push();
                mtxa_translate_xyz(150.0f, 0.0f, 0.0f);
                mtxa_rigid_inv_tf_tl(&sp8);
                mtxa_rotate_y(math_atan2(sp8.x, sp8.z));
                GXLoadPosMtxImm(mtxa->mtxA, 0);
                fn_1_10A4CC(0, paradise->texMtxA);
                avdisp_draw_model_culled_sort_auto(work->sprayCloud);
                mtxa_pop();
                mtxa_translate_xyz(-150.0f, 0.0f, 0.0f);
                mtxa_rigid_inv_tf_tl(&sp8);
                mtxa_rotate_y(math_atan2(sp8.x, sp8.z));
                GXLoadPosMtxImm(mtxa->mtxA, 0);
                fn_1_10A4CC(0, paradise->texMtxB);
                avdisp_draw_model_culled_sort_auto(work->sprayCloud);
            }
        }
        avdisp_set_z_mode(1, 3, 1);
        g_something_with_texture_scroll_2(0);
    }
}

void bg_mst_item_coin_coli(int a) {}

void fn_1_7CDC8(struct BallEnvArg *arg)
{
    struct BgMstWork *work = world_theme.work;
    struct TevStageInfo sp28 = arg->tev;
    float sp10[2][3];

    GXLoadTexObj_cached(work->waterSpecTex, sp28.texMapId);
    GXLoadTexObj_cached(work->cloudTex, sp28.texMapId + 1);
    sp10[0][0] = 0.0f;
    sp10[0][1] = 0.8f * math_sin(world_theme.unkA4 << 8);
    sp10[0][2] = 0.0f;
    sp10[1][0] = 0.0f;
    sp10[1][1] = 0.0f;
    sp10[1][2] = 0.8f * math_sin((world_theme.unkA4 << 8) + 0x4000);
    g_GXSetIndTexMtx_wrapper(sp28.unk1C, sp10, 0);
    mtxa_push();
    mtxa_from_mtx(work->specMtx);
    mathutil_mtxA_set_translate(&work->specPos);
    mtxa_scale_s(6.0f);
    GXLoadTexMtxImm(mtxa->mtxA, sp28.texMtxId, 1);
    mtxa_from_identity();
    mtxa_translate(&work->waterPos);
    mtxa_scale(&work->waterScale);
    GXLoadTexMtxImm(mtxa->mtxA, sp28.texMtxId + 3, 1);
    mtxa_pop();
    tex_coord_gen(sp28.texCoordId, 1, 1, sp28.texMtxId);
    tex_coord_gen(sp28.texCoordId + 1, 1, 4, sp28.texMtxId + 3);
    GXSetIndTexOrder_cached(sp28.tevIndStage, sp28.texCoordId + 1, sp28.texMapId + 1);
    GXSetTevIndirect_cached(sp28.tevStage, sp28.tevIndStage, 0, 7, sp28.unk1C, 0, 0, 0, 0, 0);
    GXSetTevSwapMode_cached(sp28.tevStage, 0, 0);
    g_GXSetTevOrder_wrapper(sp28.tevStage, sp28.texCoordId, sp28.texMapId, 0xFF);
    GXSetTevColorIn_cached(sp28.tevStage, 8, 0xF, 0xF, 0);
    GXSetTevColorOp_cached(sp28.tevStage, 0, 0, 0, 1, 0);
    GXSetTevAlphaIn_cached(sp28.tevStage, 7, 7, 7, 0);
    GXSetTevAlphaOp_cached(sp28.tevStage, 0, 0, 0, 1, 0);
    sp28.tevStage += 1;
    sp28.tevIndStage += 1;
    sp28.texCoordId += 2;
    sp28.texMtxId += 6;
    sp28.unk1C += 1;
    sp28.texMapId += 2;
    arg->tev = sp28;
}

int fn_1_7D13C(int index, struct GMAModelEntry *entry)
{
    struct BgMstWork *work = world_theme.work;

    switch (index)
    {
    case 0:
        work->cloudTex = entry->model->texObjs;
        break;
    case 1:
        if (work->miniCloudCount < 4)
        {
            work->miniClouds[work->miniCloudCount] = entry->model;
            work->miniCloudCount++;
        }
        break;
    case 2:
        work->sprayCloud = entry->model;
        break;
    case 3:
        work->waterModel = entry->model;
        break;
    case 4:
        work->waterSpecTex = entry->model->texObjs;
        break;
    }
    return 1;
}

int fn_1_7D1E0(int index, struct StageBgModel *model)
{
    struct BgMstWork *work = world_theme.work;
    struct Effect effect;

    switch (index)
    {
    case 0:
    case 1:
        if (model->model != NULL && work->cloudCount < 8)
        {
            work->clouds[work->cloudCount].model = model;
            work->cloudCount++;
        }
        break;
    case 2:
        if (model->model != NULL && work->paradiseCount < 8)
        {
            work->paradises[work->paradiseCount].model = model;
            work->paradiseCount++;
        }
        break;
    case 3:
    case 4:
        memset(&effect, 0, sizeof(effect));
        effect.type = 0x11;
        effect.model = (void *)model;
        spawn_effect(&effect);
        break;
    }
    return 1;
}
