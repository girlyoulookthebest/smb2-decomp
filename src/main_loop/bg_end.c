/* Ending background: scrolling cloud and water env maps, and paradise fountains. */

#include "types.h"
#include "avdisp.h"
#include "background.h"
#include "effect.h"
#include "gx.h"
#include "libc.h"
#include "mathutil.h"
#include "rand.h"
#include "stage.h"
#include "bg_work.h"

void GXSetIndTexOrder_cached(int stage, int coord, int map);
void g_GXSetIndTexMtx_wrapper(int id, float mtx[2][3], int scale);
void *g_yet_another_unk_draw_func(void *func);
void g_smth_with_bg_model_names(struct BgModelSearch *search, int (*callback)(int, struct GMAModelEntry *));
void vec_set_len(float len, Vec *src, Vec *dst);
void GXSetTexCoordGen2_cached(int dst, int func, int src, int mtx, int normalize, int postMtx);
static inline void tex_coord_gen(int dst, int func, int src, int mtx)
{
    GXSetTexCoordGen2_cached(dst, func, src, mtx, 0, 0x7D);
}


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


void mtxa_from_translate_xyz(float x, float y, float z);
#ifdef __MWERKS__
void fn_1_7EA94(struct BallEnvArg *);
void fn_1_7EE08(struct BallEnvArg *);
int fn_1_7F1AC(int, struct GMAModelEntry *);
int fn_1_7F21C(int, struct StageBgModel *);
#else
static void fn_1_7EA94(struct BallEnvArg *);
static void fn_1_7EE08(struct BallEnvArg *);
static int fn_1_7F1AC(int, struct GMAModelEntry *);
static int fn_1_7F21C(int, struct StageBgModel *);
#endif

static struct BgModelSearch lbl_1_data_31B30[] =
{
    { BG_MDL_CMP_PREFIX, "END_STG_CLOUD_" },
    { BG_MDL_CMP_FULL, "END_YOUSAI_WATER_C" },
    { BG_MDL_CMP_FULL, "END_YOUSAI_WATER_SPEC" },
    { BG_MDL_CMP_PREFIX, "END_FUNSUI_WATER_" },
    { BG_MDL_CMP_END },
};

static struct BgModelSearch lbl_1_data_31B68[] =
{
    { BG_MDL_CMP_PREFIX, "END_PARADISE_" },
    { BG_MDL_CMP_END },
};

void bg_end_init(void)
{
    struct BgEndWork *work = world_theme.work;

    bg_init_base();
    if (work->initialized == 0)
    {
        g_smth_with_bg_model_names(lbl_1_data_31B30, fn_1_7F1AC);
        work->initialized = 1;
    }
    g_init_stagedef_bg_fg_models(
        stagedef->bgModels,
        stagedef->bgModelCount,
        lbl_1_data_31B68,
        fn_1_7F21C);
    g_init_stagedef_bg_fg_models(
        stagedef->fgModels,
        stagedef->fgModelCount,
        lbl_1_data_31B68,
        fn_1_7F21C);
    work->unk14 = fn_1_7EE08;
    work->unk18 = fn_1_7EA94;
    work->unk28 = 0.005555555555555556 * (0.8f + 0.4f * RAND_FLOAT());
    work->unk2C = 0.005555555555555556 * (0.8f + 0.4f * RAND_FLOAT());
    work->unk34 = 5.0f * (1.0f + RAND_FLOAT());
    work->unk38 = 5.0f * (1.0f + RAND_FLOAT());
    work->unk3C = 1.0f;
    work->unk58 = 0.002777777777777778 * (0.9f + 0.2f * RAND_FLOAT());
    work->unk5C = 0.002777777777777778 * (0.9f + 0.2f * RAND_FLOAT());
}

void bg_end_tick(void)
{
    struct BgEndWork *work = world_theme.work;
    float temp_f1;

    bg_tick_base();
    if (is_game_paused_and_in_view_stage())
        return;
    work->unk1C.x += work->unk28;
    work->unk1C.y += work->unk2C;
    temp_f1 = 1.0f + 0.005f * math_sin(world_theme.unkA4 << 8);
    work->unk40.x = work->unk34 * temp_f1;
    work->unk40.y = work->unk38 * temp_f1;
    work->unk40.z = 1.0f;
    work->unk4C.x += work->unk58;
    work->unk4C.y += work->unk5C;
}

void bg_end_dest(void) {}

void bg_end_disp(void)
{
    bg_disp_base();
}

void bg_end_item_coin_coli(void) {}

static void fn_1_7EA94(struct BallEnvArg *arg0)
{
    struct BgEndWork *work = world_theme.work;
    struct TevStageInfo sp2C = arg0->tev;
    float sp14[2][3];

    GXLoadTexObj_cached(work->waterSpecTex, sp2C.texMapId);
    GXLoadTexObj_cached(work->cloudTex, sp2C.texMapId + 1);
    sp14[0][0] = 0.0f;
    sp14[0][1] = 0.5f * math_sin(world_theme.unkA4 << 8);
    sp14[0][2] = 0.0f;
    sp14[1][0] = 0.0f;
    sp14[1][1] = 0.0f;
    sp14[1][2] = 0.5f * math_sin((world_theme.unkA4 << 8) + 0x4000);
    g_GXSetIndTexMtx_wrapper(sp2C.unk1C, sp14, 0);
    mtxa_push();
    mtxa_from_mtx(work->unk64);
    mathutil_mtxA_set_translate(&work->unk4C);
    mtxa_scale_s(12.0f);
    GXLoadTexMtxImm(mtxa->mtxA, sp2C.texMtxId, 1);
    mtxa_from_identity();
    mtxa_translate(&work->unk1C);
    mtxa_scale(&work->unk40);
    GXLoadTexMtxImm(mtxa->mtxA, sp2C.texMtxId + 3, 1);
    mtxa_pop();
    tex_coord_gen(sp2C.texCoordId, 1, 1, sp2C.texMtxId);
    tex_coord_gen(sp2C.texCoordId + 1, 1, 4, sp2C.texMtxId + 3);
    GXSetIndTexOrder_cached(sp2C.tevIndStage, sp2C.texCoordId + 1, sp2C.texMapId + 1);
    GXSetTevIndirect_cached(sp2C.tevStage, sp2C.tevIndStage, 0, 7, sp2C.unk1C, 0, 0, 0, 0, 0);
    GXSetTevSwapMode_cached(sp2C.tevStage, 0, 0);
    g_GXSetTevOrder_wrapper(sp2C.tevStage, sp2C.texCoordId, sp2C.texMapId, 0xFF);
    GXSetTevColorIn_cached(sp2C.tevStage, 8, 0xF, 0xF, 0);
    GXSetTevColorOp_cached(sp2C.tevStage, 0, 0, 0, 1, 0);
    GXSetTevAlphaIn_cached(sp2C.tevStage, 7, 7, 7, 0);
    GXSetTevAlphaOp_cached(sp2C.tevStage, 0, 0, 0, 1, 0);
    sp2C.tevStage += 1;
    sp2C.tevIndStage += 1;
    sp2C.texCoordId += 2;
    sp2C.texMtxId += 6;
    sp2C.unk1C += 1;
    sp2C.texMapId += 2;
    arg0->tev = sp2C;
}

static void fn_1_7EE08(struct BallEnvArg *arg0)
{
    struct BgEndWork *work = world_theme.work;
    struct TevStageInfo sp2C = arg0->tev;
    float sp14[2][3];
    float temp_f2;

    GXLoadTexObj_cached(work->waterSpecTex, sp2C.texMapId);
    GXLoadTexObj_cached(work->cloudTex, sp2C.texMapId + 1);
    sp14[0][0] = 0.0f;
    sp14[0][1] = 0.5f * math_sin(world_theme.unkA4 << 8);
    sp14[0][2] = 0.0f;
    sp14[1][0] = 0.0f;
    sp14[1][1] = 0.0f;
    sp14[1][2] = 0.5f * math_sin((world_theme.unkA4 << 8) + 0x4000);
    g_GXSetIndTexMtx_wrapper(sp2C.unk1C, sp14, 0);
    mtxa_push();
    mtxa_from_mtx(work->unk64);
    mathutil_mtxA_set_translate(&work->unk4C);
    mtxa_scale_s(6.0f);
    GXLoadTexMtxImm(mtxa->mtxA, sp2C.texMtxId, 1);
    temp_f2 = 0.041666668f * world_theme.unkA4;
    mtxa_from_translate_xyz(0.05f * temp_f2, temp_f2, 0.0f);
    mtxa_scale_s(7.0f);
    GXLoadTexMtxImm(mtxa->mtxA, sp2C.texMtxId + 3, 1);
    mtxa_pop();
    tex_coord_gen(sp2C.texCoordId, 1, 1, sp2C.texMtxId);
    tex_coord_gen(sp2C.texCoordId + 1, 1, 4, sp2C.texMtxId + 3);
    GXSetIndTexOrder_cached(sp2C.tevIndStage, sp2C.texCoordId + 1, sp2C.texMapId + 1);
    GXSetTevIndirect_cached(sp2C.tevStage, sp2C.tevIndStage, 0, 7, sp2C.unk1C, 0, 0, 0, 0, 0);
    GXSetTevSwapMode_cached(sp2C.tevStage, 0, 0);
    g_GXSetTevOrder_wrapper(sp2C.tevStage, sp2C.texCoordId, sp2C.texMapId, 0xFF);
    GXSetTevColorIn_cached(sp2C.tevStage, 8, 0xF, 0xF, 0);
    GXSetTevColorOp_cached(sp2C.tevStage, 0, 0, 0, 1, 0);
    GXSetTevAlphaIn_cached(sp2C.tevStage, 7, 7, 7, 0);
    GXSetTevAlphaOp_cached(sp2C.tevStage, 0, 0, 0, 1, 0);
    sp2C.tevStage += 1;
    sp2C.tevIndStage += 1;
    sp2C.texCoordId += 2;
    sp2C.texMtxId += 6;
    sp2C.unk1C += 1;
    sp2C.texMapId += 2;
    arg0->tev = sp2C;
}

static int fn_1_7F1AC(int index, struct GMAModelEntry *entry)
{
    struct BgEndWork *work = world_theme.work;

    switch (index)
    {
    case 0:  // END_STG_CLOUD_
        work->cloudTex = &entry->model->texObjs[0];
        break;
    case 1:  // END_YOUSAI_WATER_C
        work->waterModel = entry->model;
        break;
    case 2:  // END_YOUSAI_WATER_SPEC
        work->waterSpecTex = &entry->model->texObjs[0];
        break;
    case 3:  // END_FUNSUI_WATER_
        work->fountainWaterModel = entry->model;
        break;
    }
    return 1;
}

static int fn_1_7F21C(int index, struct StageBgModel *bgObj)
{
    struct Effect effect;

    switch (index)
    {
    case 0:  // END_PARADISE_
        memset(&effect, 0, sizeof(effect));
        effect.type = 0x15;
        effect.model = (void *)bgObj;
        spawn_effect(&effect);
        break;
    }
    return 1;
}
