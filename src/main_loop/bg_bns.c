/* Bonus stage background: pulsing star points, shooting stars, lightmap env callback. */

#include "types.h"
#include "avdisp.h"
#include "background.h"
#include "ball.h"
#include "camera.h"
#include "effect.h"
#include "gx.h"
#include "libc.h"
#include "light.h"
#include "mathutil.h"
#include "mode.h"
#include "nl2ngc.h"
#include "rand.h"
#include "stage.h"
#include "bg_work.h"

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

float fn_1_815C(Vec *);
extern struct
{
    /*0x00*/ u32 flags;
} g_some_render_flag;

static struct BgModelSearch lbl_1_data_317B0[] =
{
    { BG_MDL_CMP_FULL,   "BNS_SHOTSTAR" },
    { BG_MDL_CMP_PREFIX, "STARLIGHT_" },
    { BG_MDL_CMP_FULL,   "BNS_LIGHTMAP" },
    { BG_MDL_CMP_FULL,   "BNS_LIGHTMAP_A" },
    { BG_MDL_CMP_PREFIX, "STARPOINT" },
    { BG_MDL_CMP_END,    NULL },
};

static struct BgModelSearch lbl_1_data_317EC[] =
{
    { BG_MDL_CMP_FULL, "BNS_MAIN" },
    { BG_MDL_CMP_END,  NULL },
};

void ray_to_euler_xy(Vec *, Vec *, s16 *, s16 *);
void fn_1_7AD1C(void);
void fn_1_7AD90(struct BallEnvArg *a);
int fn_1_7B0EC(int, struct GMAModelEntry *);
int fn_1_7B1A8(int, struct StageBgModel *);

void bg_bns_init(void)
{
    struct BgBnsWork *work = (void *)world_theme.work;
    int i;
    struct BgBnsStarpoint *starpoint;

    bg_init_base();
    world_theme.ballEnvFunc = fn_1_7AD90;
    world_theme.unk98 = fn_1_7AD1C;

    // find models
    if (!work->initialized)
    {
        work->starpointCount = 0;
        g_smth_with_bg_model_names(lbl_1_data_317B0, fn_1_7B0EC);
        work->initialized = TRUE;
    }
    g_init_stagedef_bg_fg_models(
        stagedef->bgModels,
        stagedef->bgModelCount,
        lbl_1_data_317EC,
        fn_1_7B1A8);
    g_init_stagedef_bg_fg_models(
        stagedef->fgModels,
        stagedef->fgModelCount,
        lbl_1_data_317EC,
        fn_1_7B1A8);

    starpoint = work->starpoints;
    for (i = work->starpointCount; i > 0; i--, starpoint++)
    {
        starpoint->pulse = rand() & 0x7FFF;
        starpoint->pulseSpeed = ((RAND_FLOAT() * 0.5f + 1.0f) * 65536.0f) / 180.0f;
    }
}

void bg_bns_tick(void)
{
    struct BgBnsWork *work = world_theme.work;
    int i;
    struct BgBnsStarpoint *starpoint;
    Vec sp8;

    bg_tick_base();
    if (is_game_paused_and_in_view_stage())
        return;

    starpoint = work->starpoints;
    for (i = work->starpointCount; i > 0; i--, starpoint++)
    {
        float intensity;

        // Pulse the star's glow (a slightly off-white color)
        starpoint->pulse += starpoint->pulseSpeed;
        intensity = (math_sin(starpoint->pulse) + 1.0f) * 0.25f + 0.5f;
        starpoint->red = intensity * 1.1f;
        starpoint->green = intensity * 1.05f;
        starpoint->blue = intensity;
        if (starpoint->red > 1.0f)
            starpoint->red = 1.0f;
        if (starpoint->green > 1.0f)
            starpoint->green = 1.0f;
    }

    mtxa_from_identity();
    sp8.x = (2.0f / 3.0f);
    sp8.y = (2.0f / 3.0f);
    sp8.z = (2.0f / 3.0f);
    mtxa_scale(&sp8);
    mtxa_rotate_y(0x1000);
    mtxa_rotate_x(0x4000);
    mtxa_to_mtx(work->unk71C);

    mtxa_from_identity();
    mtxa_rotate_y(0x1000);
    mtxa_rotate_x(0x4000);
    mtxa_to_mtx(work->unk74C);

    mtxa_from_identity();
    mtxa->mtxA[0][0] = 0.0f;
    mtxa->mtxA[1][1] = 0.0f;
    mtxa->mtxA[1][2] = -0.5f;
    mtxa->mtxA[1][3] = 0.5f;
    mtxa->mtxA[2][2] = 0.0f;
    mtxa->mtxA[2][3] = 1.0f;
    mtxa_to_mtx(work->unk77C);
}

void bg_bns_dest(void) {}

void bg_bns_disp(void)
{
    float _pool_base = 0.0f;
    struct BgBnsWork *work = (void *)world_theme.work;
    struct StageBgModel *mainObj;
    struct GMAModel *starlightModel;
    struct BgBnsStarpoint *starpoint;
    int i;
    Vec starPos;
    Vec bgScale;  // scale of the entire background

    bg_disp_base();
    mainObj = work->mainObj;
    bgScale = mainObj->scale;
    mtxa_from_mtx(g_ord_tbl_stuff->matrices[0]);
    mtxa_translate(&mainObj->pos);
    mtxa_rotate_z(mainObj->rotZ);
    mtxa_rotate_y(mainObj->rotY);
    mtxa_rotate_x(mainObj->rotX);
    mtxa_scale(&bgScale);

    avdisp_set_z_mode(1, 3, 0);
    starlightModel = work->starlightModel;
    for (starpoint = work->starpoints, i = work->starpointCount; i > 0; i--, starpoint++)
    {
        float pulse = (starpoint->red + starpoint->green + starpoint->blue) * 0.75f;

        if (g_some_render_flag.flags & (1 << 2))
        {
            starPos.x = starpoint->u_pos.x * bgScale.x;
            starPos.y = starpoint->u_pos.y * bgScale.y;
            starPos.z = starpoint->u_pos.z * bgScale.z;
            if (fn_1_815C(&starPos) < -(starlightModel->boundSphereRadius * pulse))
                continue;
        }

        // Draw star light billboard
        mtxa_push();
        mtxa_translate(&starpoint->u_pos);
        mtxa_sq_from_identity();
        mathutil_mtxA_get_translate(&starPos);
        if (starPos.z < -30.0f)
        {
            float f3 = (26.0f + starPos.z) / starPos.z;

            starPos.x *= f3;
            starPos.y *= f3;
            starPos.z *= f3;
            mathutil_mtxA_set_translate(&starPos);
            pulse *= f3;
            mtxa_scale_s(pulse);
            set_post_mult_color(starpoint->red, starpoint->green, starpoint->blue, 1.0f);
            avdisp_draw_model_culled_sort_auto(starlightModel);
            fade_color_base_default();
        }
        mtxa_pop();
    }
    avdisp_set_z_mode(1, 3, 1);
}

void bg_bns_item_coin_coli(int a)
{
    struct Effect star;
    Vec spC;
    float f31;

    switch (a)
    {
    case 0:
    case 1:
        // spawn shooting star
        memset(&star, 0, sizeof(star));
        star.type = 0xB;
        star.playerId = current_ball->playerId;
        mtxa_from_mtx(cameras[star.playerId].unk1AC);
        spC.z = -120.0f + RAND_FLOAT() * -225.0f;
        spC.x = spC.z * -(8.0f / 3.0f) * g_current_camera->sub28.view.tanHalfFov * (RAND_FLOAT() - 0.5f);
        spC.y = spC.z * -1.1f * g_current_camera->sub28.view.tanHalfFov;
        mtxa_rigid_inv_tf_point(&spC, &star.pos);
        f31 = -spC.z * (1.0f / 300.0f);
        star.vel.x = (1.0f + RAND_FLOAT()) * f31;
        star.vel.y = (-3.0f + RAND_FLOAT() * -1.0f) * f31;
        star.vel.z = (1.0f + RAND_FLOAT()) * f31;
        spC.x = 0.0f;
        spC.y = 0.0f;
        spC.z = 0.0f;
        mtxa_rigid_inv_tf_point(&spC, &spC);
        ray_to_euler_xy(&spC, &star.pos, &star.rotX, &star.rotY);
        star.rotZ = rand() & 0x7FFF;
        spawn_effect(&star);
        break;
    }
}

void fn_1_7AD1C(void)
{
    struct BgBnsWork *work = (void *)world_theme.work;
    Mtx sp8;

    mtxa_from_mtx(g_ord_tbl_stuff->matrices[0]);
    mtxa_rigid_invert();
    mtxa_to_mtx(sp8);
    mtxa_mult_left(work->unk71C);
    mtxa_to_mtx(work->unk7AC);
    mtxa_from_mtx(sp8);
    mtxa_mult_left(work->unk74C);
    mtxa_to_mtx(work->unk7DC);
}

void fn_1_7AD90(struct BallEnvArg *a)
{
    struct BgBnsWork *work = (void *)world_theme.work;
    struct TevStageInfo spC = a->tev;

    GXSetBlendMode_cached(1, 1, 1, 0);
    fn_1_E2F1C();
    GXLoadTexObj_cached(work->lightmapTexObjs, spC.texMapId);
    mtxa_push();
    mtxa_mult_left(work->unk7AC);
    GXLoadTexMtxImm(mtxa->mtxA, spC.texMtxId, 0);
    mtxa_pop();
    tex_coord_gen(spC.texCoordId, 1, 0, spC.texMtxId);
    g_GXSetTevIndirect_zero_if_different(spC.tevStage);
    g_GXSetTevOrder_wrapper(spC.tevStage, spC.texCoordId, spC.texMapId, 0xFF);
    GXSetTevColorIn_cached(spC.tevStage, 0xF, 0xF, 0xF, 8);
    GXSetTevColorOp_cached(spC.tevStage, 0, 0, 0, 1, 2);
    GXSetTevAlphaIn_cached(spC.tevStage, 7, 7, 7, 6);
    GXSetTevAlphaOp_cached(spC.tevStage, 0, 0, 0, 1, 0);
    spC.tevStage++;
    spC.texCoordId++;
    spC.texMtxId += 3;
    spC.texMapId++;
    mtxa_push();
    mtxa_mult_left(work->unk7DC);
    mathutil_mtxA_set_translate_xyz(0.0f, 0.0f, 0.0f);
    GXLoadTexMtxImm(mtxa->mtxA, spC.texMtxId, 0);
    mtxa_pop();
    GXLoadTexMtxImm(work->unk77C, spC.postTexMtxId, 0);
    GXLoadTexObj_cached(work->lightmapATexObjs, spC.texMapId);
    GXSetTexCoordGen2_cached(spC.texCoordId, 0, 1, spC.texMtxId, 1, spC.postTexMtxId);
    g_GXSetTevIndirect_zero_if_different(spC.tevStage);
    g_GXSetTevOrder_wrapper(spC.tevStage, spC.texCoordId, spC.texMapId, 0xFF);
    GXSetTevColorIn_cached(spC.tevStage, 0xF, 8, 4, 0xF);
    GXSetTevColorOp_cached(spC.tevStage, 0, 0, 0, 1, 0);
    GXSetTevAlphaIn_cached(spC.tevStage, 7, 7, 7, 0);
    GXSetTevAlphaOp_cached(spC.tevStage, 0, 0, 0, 1, 0);
    spC.tevStage++;
    spC.texCoordId++;
    spC.texMtxId += 3;
    spC.postTexMtxId += 3;
    spC.texMapId++;
    a->tev = spC;
}

int fn_1_7B0EC(int index, struct GMAModelEntry *entry)
{
    struct BgBnsWork *work = world_theme.work;

    switch (index)
    {
    case 0:  // BNS_SHOTSTAR
        work->shotstarModel = entry->model;
        break;
    case 1:  // STARLIGHT_
        work->starlightModel = entry->model;
        break;
    case 2:  // BNS_LIGHTMAP
        work->lightmapTexObjs = entry->model->texObjs;
        break;
    case 3:  // BNS_LIGHTMAP_A
        work->lightmapATexObjs = entry->model->texObjs;
        break;
    case 4:  // STARPOINT
        if (work->starpointCount < 64)
        {
            struct BgBnsStarpoint *starpoint = &work->starpoints[work->starpointCount];

            starpoint->u_pos = entry->model->boundSphereCenter;
            work->starpointCount++;
        }
        break;
    }
    return 1;
}

int fn_1_7B1A8(int index, struct StageBgModel *bgObj)
{
    struct BgBnsWork *work = world_theme.work;

    switch (index)
    {
    case 0:  // BNS_MAIN
        work->mainObj = bgObj;
        break;
    }
    return 1;
}
