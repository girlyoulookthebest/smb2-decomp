/* Water background: caustics, lightmap env callbacks and the bubble effect hook. */

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

static struct BgModelSearch lbl_1_data_3159C[] =
{
    { BG_MDL_CMP_FULL,   "WAT_SUIMEN_MAT_ONLY" },
    { BG_MDL_CMP_FULL,   "WAT_SANSYO_TEX_WATER" },
    { BG_MDL_CMP_FULL,   "WAT_SUIMEN_TEST_LOW_CONT" },
    { BG_MDL_CMP_PREFIX, "WAT_BUBBLE_" },
    { BG_MDL_CMP_FULL,   "WAT_LIGHTMAP" },
    { BG_MDL_CMP_FULL,   "WAT_LIGHTMAP_STAGE" },
    { BG_MDL_CMP_FULL,   "WAT_LIGHTMAP_GRAD" },
    { BG_MDL_CMP_END,    NULL },
};

static struct BgModelSearch lbl_1_data_31638[] =
{
    { BG_MDL_CMP_PREFIX, "WAT_SUB_SUKRYU" },
    { BG_MDL_CMP_FULL,   "WAT_SUIMEN" },
    { BG_MDL_CMP_PREFIX, "WAT_LIGHTCHAIN" },
    { BG_MDL_CMP_PREFIX, "WAT_LONG_CHAIN" },
    { BG_MDL_CMP_PREFIX, "WAT_PLANT" },
    { BG_MDL_CMP_PREFIX, "WAT_UKIWA" },
    { BG_MDL_CMP_PREFIX, "WAT_SUB" },
    { BG_MDL_CMP_END,    NULL },
};

void fn_1_77658(void);
void fn_1_776E4(struct BallEnvArg *);
void fn_1_77964(struct BallEnvArg *);
void fn_1_77BE4(struct BallEnvArg *);
void fn_1_77FC8(struct BallEnvArg *);
void fn_1_78424(struct BallEnvArg *);
#ifdef __MWERKS__
int fn_1_78824(int, struct GMAModelEntry *);
#else
static int fn_1_78824(int, struct GMAModelEntry *);
#endif
int g_set_bgwat_bubble_effect(int, struct StageBgModel *);

struct TevStageInfo lbl_1_bss_87F80;

void bg_wat_init(void)
{
    struct BgWatWork *work = world_theme.work;

    bg_init_base();
    if (modeCtrl.unk30 < 2)
    {
        world_theme.unk8C = fn_1_776E4;
        world_theme.unk90 = fn_1_77964;
    }
    world_theme.ballEnvFunc = fn_1_77FC8;
    if (work->unk0 == 0)
    {
        debug_textdraw_set_pos(4, 4);
        g_smth_with_bg_model_names(lbl_1_data_3159C, fn_1_78824);
        work->unk0 = 1;
    }
    fn_1_7385C();
    work->waterSurface = NULL;
    g_init_stagedef_bg_fg_models(
        stagedef->bgModels,
        stagedef->bgModelCount,
        lbl_1_data_31638,
        g_set_bgwat_bubble_effect);
    g_init_stagedef_bg_fg_models(
        stagedef->fgModels,
        stagedef->fgModelCount,
        lbl_1_data_31638,
        g_set_bgwat_bubble_effect);
    world_theme.unk14.x = RAND_FLOAT() - 0.5f;
    world_theme.unk14.y = RAND_FLOAT() - 0.5f;
    world_theme.unk14.z = RAND_FLOAT() - 0.5f;
    world_theme.unk20.x = RAND_FLOAT() - 0.5f;
    world_theme.unk20.y = 0.0f;
    world_theme.unk20.z = RAND_FLOAT() - 0.5f;
    vec_set_len((0.9f + (0.2f * RAND_FLOAT())) / 30.0f, &world_theme.unk20, &world_theme.unk20);
    world_theme.unk2C.x = RAND_FLOAT() - 0.5f;
    world_theme.unk2C.y = RAND_FLOAT() - 0.5f;
    world_theme.unk2C.z = RAND_FLOAT() - 0.5f;
    world_theme.unk38.x = RAND_FLOAT() - 0.5f;
    world_theme.unk38.y = 0.0f;
    world_theme.unk38.z = RAND_FLOAT() - 0.5f;
    vec_set_len((0.9f + (0.2f * RAND_FLOAT())) / 30.0f, &world_theme.unk38, &world_theme.unk38);
    work->unk10 = rand() & 0x7FFF;
    work->unk12 = rand() & 0x7FFF;
    work->unk14 = rand() & 0x7FFF;
    work->unk16 = rand() & 0x7FFF;
    work->unk18 = rand() & 0x7FFF;
    work->unk1A = rand() & 0x7FFF;
    work->unk1E = 1;
    work->unk1C = 60;
    work->unk20 = 0.5f;
    world_theme.unk98 = fn_1_77658;
}

void bg_wat_tick(void)
{
    Vec scale;
    struct BgWatWork *work;

    if (is_game_paused_and_in_view_stage())
        return;
    bg_tick_base();
    work = world_theme.work;
    world_theme.unk14.x += world_theme.unk20.x;
    world_theme.unk14.y += world_theme.unk20.y;
    world_theme.unk14.z += world_theme.unk20.z;
    world_theme.unk2C.x += world_theme.unk38.x;
    world_theme.unk2C.y += world_theme.unk38.y;
    world_theme.unk2C.z += world_theme.unk38.z;
    work->unk10 += 0x20;
    work->unk12 += 0x3F;
    work->unk14 += 0x5F;
    work->unk16 += 0x17;
    work->unk18 += 0x20;
    work->unk1A += 0x61;
    work->unk1C--;
    if (work->unk1E != 0)
        work->unk20 += 0.3f * (0.5f - work->unk20);
    else
        work->unk20 += 0.1f * (1.0f - work->unk20);
    mtxa_from_identity();
    scale.x = 0.125 * (0.9f + 0.2f * math_sin(work->unk10));
    scale.y = 0.125 * (0.9f + 0.2f * math_sin(work->unk12));
    scale.z = 0.125 * (0.9f + 0.2f * math_sin(work->unk14));
    mtxa_scale(&scale);
    mtxa_rotate_y(0x1000);
    mtxa_rotate_x(0x3800);
    mtxa_translate(&world_theme.unk14);
    mtxa_to_mtx(work->unk3C);
    mtxa_from_identity();
    scale.x = 0.078125f * (0.9f + 0.2f * math_sin(work->unk16));
    scale.y = 0.078125f * (0.9f + 0.2f * math_sin(work->unk18));
    scale.z = 0.078125f * (0.9f + 0.2f * math_sin(work->unk1A));
    mtxa_scale(&scale);
    mtxa_rotate_y(0x1000);
    mtxa_rotate_x(0x3800);
    mtxa_rotate_z(0x1000);
    mtxa_translate(&world_theme.unk2C);
    mtxa_to_mtx(work->unk6C);
    mtxa_from_identity();
    mtxa_rotate_y(0x1000);
    mtxa_rotate_x(0x3800);
    mtxa_to_mtx(work->unk9C);
    mtxa_from_identity();
    mtxa->mtxA[0][0] = 0.0f;
    mtxa->mtxA[0][2] = -0.5f;
    mtxa->mtxA[0][3] = work->unk20;
    mtxa->mtxA[1][1] = 0.0f;
    mtxa->mtxA[2][2] = 0.0f;
    mtxa->mtxA[2][3] = 1.0f;
    mtxa_to_mtx(work->unkCC);
}

void bg_wat_dest(void) {}

void bg_wat_disp(void)
{
    struct BgWatWork *work = world_theme.work;
    struct StageBgModel *temp_r31;

    mtxa_from_mtx(g_ord_tbl_stuff->matrices[0]);
    temp_r31 = work->waterSurface;
    if (temp_r31 != NULL)
    {
        temp_r31->flags &= 0xFFFEFFFF;
        mtxa_translate(&temp_r31->pos);
        mtxa_rotate_z(temp_r31->rotZ);
        mtxa_rotate_y(temp_r31->rotY);
        mtxa_rotate_x(temp_r31->rotX);
        mtxa_scale(&temp_r31->scale);
    }
    GXLoadPosMtxImm(mtxa->mtxA, 0);
    GXLoadNrmMtxImm(mtxa->mtxA, 0);
    g_yet_another_unk_draw_func(fn_1_78424);
    avdisp_draw_model_culled_sort_never(work->waterSurfaceMat);
    g_yet_another_unk_draw_func(NULL);
    bg_disp_base();
}

void bg_wat_item_coin_coli(void) {}

void fn_1_77658(void)
{
    struct BgWatWork *work = world_theme.work;
    Mtx sp8;

    mtxa_from_mtx(g_ord_tbl_stuff->matrices[0]);
    mtxa_rigid_invert();
    mtxa_to_mtx(sp8);
    mtxa_mult_left(work->unk3C);
    mtxa_to_mtx(work->unkFC);
    mtxa_from_mtx(sp8);
    mtxa_mult_left(work->unk6C);
    mtxa_to_mtx(work->unk12C);
    mtxa_from_mtx(sp8);
    mtxa_mult_left(work->unk9C);
    mtxa_to_mtx(work->unk15C);
}

void fn_1_776E4(struct BallEnvArg *arg0)
{
    struct TevStageInfo sp24 = arg0->tev;
    float spC[2][3];

    if (arg0->unk0 == 0
     && sp24.tevStage == lbl_1_bss_87F80.tevStage
     && sp24.tevIndStage == lbl_1_bss_87F80.tevIndStage
     && sp24.texMtxId == lbl_1_bss_87F80.texMtxId
     && sp24.postTexMtxId == lbl_1_bss_87F80.postTexMtxId
     && sp24.unk1C == lbl_1_bss_87F80.unk1C
     && sp24.texMapId == lbl_1_bss_87F80.texMapId)
    {
        arg0->tev.tevStage += 2;
        arg0->tev.tevIndStage += 1;
        arg0->tev.texCoordId += 3;
        arg0->tev.texMtxId += 9;
        arg0->tev.postTexMtxId += 3;
        arg0->tev.unk1C += 1;
        arg0->tev.texMapId += 2;
        return;
    }
    lbl_1_bss_87F80 = sp24;
    GXLoadTexObj_cached(((struct BgWatWork *)world_theme.work)->lightmapStageTex, arg0->tev.texMapId);
    GXLoadTexObj_cached(((struct BgWatWork *)world_theme.work)->lightmapStageTex, arg0->tev.texMapId + 1);
    spC[0][0] = 0.8f;
    spC[0][1] = 0.0f;
    spC[0][2] = 0.0f;
    spC[1][0] = 0.0f;
    spC[1][1] = 0.8f;
    spC[1][2] = 0.0f;
    g_GXSetIndTexMtx_wrapper(arg0->tev.unk1C, spC, 0);
    fn_1_77BE4(arg0);
}

void fn_1_77964(struct BallEnvArg *arg0)
{
    struct TevStageInfo sp24 = arg0->tev;
    float spC[2][3];

    if (arg0->unk0 == 0
     && sp24.tevStage == lbl_1_bss_87F80.tevStage
     && sp24.tevIndStage == lbl_1_bss_87F80.tevIndStage
     && sp24.texMtxId == lbl_1_bss_87F80.texMtxId
     && sp24.postTexMtxId == lbl_1_bss_87F80.postTexMtxId
     && sp24.unk1C == lbl_1_bss_87F80.unk1C
     && sp24.texMapId == lbl_1_bss_87F80.texMapId)
    {
        arg0->tev.tevStage += 2;
        arg0->tev.tevIndStage += 1;
        arg0->tev.texCoordId += 3;
        arg0->tev.texMtxId += 9;
        arg0->tev.postTexMtxId += 3;
        arg0->tev.unk1C += 1;
        arg0->tev.texMapId += 2;
        return;
    }
    lbl_1_bss_87F80 = sp24;
    GXLoadTexObj_cached(((struct BgWatWork *)world_theme.work)->lightmapTex, arg0->tev.texMapId);
    GXLoadTexObj_cached(((struct BgWatWork *)world_theme.work)->lightmapTex, arg0->tev.texMapId + 1);
    spC[0][0] = 0.6f;
    spC[0][1] = 0.0f;
    spC[0][2] = 0.0f;
    spC[1][0] = 0.0f;
    spC[1][1] = 0.6f;
    spC[1][2] = 0.0f;
    g_GXSetIndTexMtx_wrapper(arg0->tev.unk1C, spC, 0);
    fn_1_77BE4(arg0);
}

void fn_1_77BE4(struct BallEnvArg *arg0)
{
    struct BgWatWork *work = world_theme.work;
    struct TevStageInfo sp2C = arg0->tev;

    mtxa_push();
    mtxa_mult_left(work->unkFC);
    GXLoadTexMtxImm(mtxa->mtxA, sp2C.texMtxId, 0);
    mtxa_pop();
    mtxa_push();
    mtxa_mult_left(work->unk12C);
    GXLoadTexMtxImm(mtxa->mtxA, sp2C.texMtxId + 3, 0);
    mtxa_pop();
    tex_coord_gen(sp2C.texCoordId, 1, 0, sp2C.texMtxId);
    tex_coord_gen(sp2C.texCoordId + 1, 1, 0, sp2C.texMtxId + 3);
    GXSetIndTexOrder_cached(sp2C.tevIndStage, sp2C.texCoordId + 1, sp2C.texMapId + 1);
    GXSetTevIndirect_cached(sp2C.tevStage, sp2C.tevIndStage, 0, 0, sp2C.unk1C, 0, 0, 0, 0, 0);
    g_GXSetTevOrder_wrapper(sp2C.tevStage, sp2C.texCoordId, sp2C.texMapId, 0xFF);
    GXSetTevColorIn_cached(sp2C.tevStage, 0xF, 0xF, 0xF, 8);
    GXSetTevColorOp_cached(sp2C.tevStage, 0, 0, 0, 1, 2);
    GXSetTevAlphaIn_cached(sp2C.tevStage, 7, 7, 7, 0);
    GXSetTevAlphaOp_cached(sp2C.tevStage, 0, 0, 0, 1, 0);
    mtxa_push();
    mtxa_mult_left(work->unk15C);
    mathutil_mtxA_set_translate_xyz(0.0f, 0.0f, 0.0f);
    GXLoadTexMtxImm(mtxa->mtxA, sp2C.texMtxId + 6, 0);
    mtxa_pop();
    GXLoadTexMtxImm(work->unkCC, sp2C.postTexMtxId, 0);
    GXLoadTexObj_cached(work->lightmapGradTex, sp2C.texMapId + 2);
    GXSetTexCoordGen2_cached(sp2C.texCoordId + 2, 0, 1, sp2C.texMtxId + 6, 1, sp2C.postTexMtxId);
    g_GXSetTevIndirect_zero_if_different(sp2C.tevStage + 1);
    g_GXSetTevOrder_wrapper(sp2C.tevStage + 1, sp2C.texCoordId + 2, sp2C.texMapId + 2, 0xFF);
    GXSetTevColorIn_cached(sp2C.tevStage + 1, 0xF, 8, 4, 0);
    GXSetTevColorOp_cached(sp2C.tevStage + 1, 0, 0, 0, 1, 0);
    GXSetTevAlphaIn_cached(sp2C.tevStage + 1, 7, 7, 7, 0);
    GXSetTevAlphaOp_cached(sp2C.tevStage + 1, 0, 0, 0, 1, 0);
    sp2C.tevStage += 2;
    sp2C.tevIndStage += 1;
    sp2C.texCoordId += 3;
    sp2C.texMtxId += 9;
    sp2C.postTexMtxId += 3;
    sp2C.unk1C += 1;
    sp2C.texMapId += 3;
    arg0->tev = sp2C;
}

void fn_1_77FC8(struct BallEnvArg *arg0)
{
    struct BgWatWork *work = world_theme.work;
    struct TevStageInfo sp2C = arg0->tev;
    float sp14[2][3];

    GXSetBlendMode_cached(1, 1, 1, 0);
    GXLoadTexObj_cached(work->lightmapStageTex, sp2C.texMapId);
    GXLoadTexObj_cached(work->lightmapStageTex, sp2C.texMapId + 1);
    sp14[0][0] = 0.6f;
    sp14[0][1] = 0.0f;
    sp14[0][2] = 0.0f;
    sp14[1][0] = 0.0f;
    sp14[1][1] = 0.6f;
    sp14[1][2] = 0.0f;
    g_GXSetIndTexMtx_wrapper(sp2C.unk1C, sp14, 0);
    mtxa_push();
    mtxa_mult_left(work->unkFC);
    mtxa_scale_s(3.0f);
    GXLoadTexMtxImm(mtxa->mtxA, sp2C.texMtxId, 0);
    mtxa_pop();
    mtxa_push();
    mtxa_mult_left(work->unk12C);
    mtxa_scale_s(3.0f);
    GXLoadTexMtxImm(mtxa->mtxA, sp2C.texMtxId + 3, 0);
    mtxa_pop();
    tex_coord_gen(sp2C.texCoordId, 1, 0, sp2C.texMtxId);
    tex_coord_gen(sp2C.texCoordId + 1, 1, 0, sp2C.texMtxId + 3);
    GXSetIndTexOrder_cached(sp2C.tevIndStage, sp2C.texCoordId + 1, sp2C.texMapId + 1);
    GXSetTevIndirect_cached(sp2C.tevStage, sp2C.tevIndStage, 0, 0, sp2C.unk1C, 0, 0, 0, 0, 0);
    g_GXSetTevOrder_wrapper(sp2C.tevStage, sp2C.texCoordId, sp2C.texMapId, 0xFF);
    GXSetTevColorIn_cached(sp2C.tevStage, 0xF, 0xF, 0xF, 8);
    GXSetTevColorOp_cached(sp2C.tevStage, 0, 0, 0, 1, 2);
    GXSetTevAlphaIn_cached(sp2C.tevStage, 7, 7, 7, 6);
    GXSetTevAlphaOp_cached(sp2C.tevStage, 0, 0, 0, 1, 0);
    mtxa_push();
    mtxa_mult_left(work->unk15C);
    mathutil_mtxA_set_translate_xyz(0.0f, 0.0f, 0.0f);
    GXLoadTexMtxImm(mtxa->mtxA, sp2C.texMtxId + 6, 0);
    mtxa_pop();
    GXLoadTexMtxImm(work->unkCC, sp2C.postTexMtxId, 0);
    GXLoadTexObj_cached(work->lightmapGradTex, sp2C.texMapId + 2);
    GXSetTexCoordGen2_cached(sp2C.texCoordId + 2, 0, 1, sp2C.texMtxId + 6, 1, sp2C.postTexMtxId);
    g_GXSetTevIndirect_zero_if_different(sp2C.tevStage + 1);
    g_GXSetTevOrder_wrapper(sp2C.tevStage + 1, sp2C.texCoordId + 2, sp2C.texMapId + 2, 0xFF);
    GXSetTevColorIn_cached(sp2C.tevStage + 1, 0xF, 8, 4, 0xF);
    GXSetTevColorOp_cached(sp2C.tevStage + 1, 0, 0, 0, 1, 0);
    GXSetTevAlphaIn_cached(sp2C.tevStage + 1, 7, 7, 7, 0);
    GXSetTevAlphaOp_cached(sp2C.tevStage + 1, 0, 0, 0, 1, 0);
    sp2C.tevStage += 2;
    sp2C.tevIndStage += 1;
    sp2C.texCoordId += 3;
    sp2C.texMtxId += 9;
    sp2C.postTexMtxId += 3;
    sp2C.unk1C += 1;
    sp2C.texMapId += 3;
    arg0->tev = sp2C;
}

void fn_1_78424(struct BallEnvArg *arg0)
{
    struct BgWatWork *work = world_theme.work;
    struct TevStageInfo sp2C = arg0->tev;
    float sp14[2][3];

    GXLoadTexObj_cached(work->causticTex, sp2C.texMapId);
    GXLoadTexObj_cached(work->waterSurfaceTestTex, sp2C.texMapId + 1);
    mtxa_push();
    mtxa_from_identity();
    mtxa_rotate_x(0x4000);
    mtxa_rotate_y(world_theme.unkA4);
    mtxa_translate_xyz(
        world_theme.animTimer / 2160.0f,
        world_theme.animTimer / 2160.0f,
        world_theme.animTimer / -5280.0f);
    mtxa_scale_s(1.0f / (214.0f + (20.0f * math_sin(world_theme.unkA4 << 6))));
    GXLoadTexMtxImm(mtxa->mtxA, sp2C.texMtxId, 0);
    mtxa_from_identity();
    mtxa_rotate_x(0x4000);
    mtxa_rotate_y(0x400);
    mtxa_translate_xyz(
        world_theme.animTimer / 360.0f,
        world_theme.animTimer / 360.0f,
        world_theme.animTimer / 360.0f);
    mtxa_scale_s(0.005f * (1.0f + (0.25f * math_sin(world_theme.unkA4 << 5))));
    GXLoadTexMtxImm(mtxa->mtxA, sp2C.texMtxId + 3, 0);
    mtxa_pop();
    sp14[0][0] = 0.6f;
    sp14[0][1] = -0.01f;
    sp14[0][2] = 0.0f;
    sp14[1][0] = 0.0f;
    sp14[1][1] = 0.6f;
    sp14[1][2] = 0.2f;
    g_GXSetIndTexMtx_wrapper(arg0->tev.unk1C, sp14, 1);
    tex_coord_gen(sp2C.texCoordId, 1, 0, sp2C.texMtxId);
    // NONMATCHING: need it to reload sp2C.texCoordId here instead of using r29
    tex_coord_gen(sp2C.texCoordId + 1, 1, 0, sp2C.texMtxId + 3);
    GXSetIndTexOrder_cached(sp2C.tevIndStage, sp2C.texCoordId + 1, sp2C.texMapId + 1);
    GXSetTevIndirect_cached(sp2C.tevStage, sp2C.tevIndStage, 0, 0, sp2C.unk1C, 0, 0, 0, 0, 0);
    g_GXSetTevOrder_wrapper(sp2C.tevStage, sp2C.texCoordId, sp2C.texMapId, 0xFF);
    GXSetTevColorIn_cached(sp2C.tevStage, 0xF, 8, 0, 0xF);
    GXSetTevColorOp_cached(sp2C.tevStage, 0, 0, 0, 1, 0);
    GXSetTevAlphaIn_cached(sp2C.tevStage, 7, 7, 7, 0);
    GXSetTevAlphaOp_cached(sp2C.tevStage, 0, 0, 0, 1, 0);
    sp2C.tevStage += 1;
    sp2C.tevIndStage += 1;
    sp2C.texCoordId += 2;
    sp2C.texMtxId += 6;
    sp2C.texMapId += 2;
    sp2C.unk1C += 1;
    arg0->tev = sp2C;
}

static int fn_1_78824(int index, struct GMAModelEntry *entry)
{
    struct BgWatWork *work = world_theme.work;

    switch (index)
    {
    case 0:  // WAT_SUIMEN_MAT_ONLY
        work->waterSurfaceMat = entry->model;
        break;
    case 1:  // WAT_SANSYO_TEX_WATER
        work->causticTex = &entry->model->texObjs[0];
        break;
    case 2:  // WAT_SUIMEN_TEST_LOW_CONT
        work->waterSurfaceTestTex = &entry->model->texObjs[0];
        break;
    case 3:  // WAT_BUBBLE_
        work->bubbleModel = entry->model;
        break;
    case 4:  // WAT_LIGHTMAP
        work->lightmapTex = &entry->model->texObjs[0];
        break;
    case 5:  // WAT_LIGHTMAP_STAGE
        work->lightmapStageTex = &entry->model->texObjs[0];
        break;
    case 6:  // WAT_LIGHTMAP_GRAD
        work->lightmapGradTex = &entry->model->texObjs[0];
        break;
    }
    return 1;
}

int g_set_bgwat_bubble_effect(int index, struct StageBgModel *bgObj)
{
    struct BgWatWork *work = world_theme.work;
    struct Effect effect;

    switch (index)
    {
    case 0:  // WAT_SUB_SUKRYU
        // submarine propeller
        memset(&effect, 0, sizeof(effect));
        effect.type = 6;
        effect.model = (void *)bgObj;
        spawn_effect(&effect);
        break;
    case 1:  // WAT_SUIMEN
        work->waterSurface = bgObj;
        break;
    default:
        bgObj->flags |= 0x1000000;
        break;
    }
    return 1;
}
