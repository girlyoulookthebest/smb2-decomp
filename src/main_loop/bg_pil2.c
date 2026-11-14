/* Target background: a bg_pil style reflective sea with animated caustic
 * textures, lamp glows, and beams hidden by camera position. */

#include "types.h"
#include "avdisp.h"
#include "background.h"
#include "ball.h"
#include "camera.h"
#include "gx.h"
#include "libc.h"
#include "light.h"
#include "mathutil.h"
#include "mode.h"
#include "nl2ngc.h"
#include "rand.h"
#include "stage.h"
#include "stcoli.h"

extern struct { u32 flags; } g_some_render_flag;
extern u32 g_font_char_var_3;  /* global frame counter */

struct RenderEffect
{
    /*0x00*/ u8 state;
    u8 filler1[5];
    /*0x06*/ u16 cameraMask;
    /*0x08*/ u32 enableFlags;
    u8 fillerC[4];
    /*0x10*/ void *work;
    /*0x14*/ struct RenderEffectFuncs *funcs;
};

struct BgPil2Reflection
{
    /*0x00*/ s32 state;
    /*0x04*/ GXTexObj tex;
    /* typed like rend_efc_ripple.c's RenderEffectSeaReflection (which
     * allocates it) so the x64 layout matches */
    /*0x24*/ int format;
    /*0x28*/ void *image;
    /*0x2C*/ u16 xres;
    /*0x2E*/ u16 yres;
    /*0x30*/ GXTexObj indTex;
    /*0x50*/ void *indBuf;
    /*0x54*/ Mtx mtx;
};

struct BgPil2Layer
{
    /*0x00*/ Vec pos;
    /*0x0C*/ Vec vel;
    /*0x18*/ Vec scale;
    /*0x24*/ s16 rotZ;
    /*0x28*/ Mtx mtx;
};  /* size = 0x58 */

struct BgPil2Work
{
    /*0x000*/ s32 initialized;
    /*0x004*/ struct GMAModel *unk4[4];  /* bg_pil.c islands; typed for the x64 layout */
    /*0x014*/ s32 unk14;
    /*0x018*/ struct StageBgModel *sea;
    /*0x01C*/ struct GMAModel *seaModel;
    /*0x020*/ GXTexObj *waterSpecTex;
    /*0x024*/ GXTexObj *indMapATex;
    /*0x028*/ s32 oceanTexCount;
    /*0x02C*/ GXTexObj *oceanTex[32];
    /*0x0AC*/ s32 oceanBTexCount;
    /*0x0B0*/ GXTexObj *oceanBTex[32];
    /*0x130*/ struct GMAModel *seaMask;
    /*0x134*/ struct GMAModel *vtxSeaBall;
    /*0x138*/ s32 hamonCount;
    /*0x13C*/ struct GMAModel *hamon[8];
    /*0x15C*/ Mtx reflectTexMtx;
    /*0x18C*/ Mtx mirrorTexMtx;
    /*0x1BC*/ Mtx specTexMtx;
    /*0x1EC*/ float scrollX;
    /*0x1F0*/ float scrollY;
    /*0x1F4*/ float scaleA;
    /*0x1F8*/ float scaleB;
    /*0x1FC*/ struct BgPil2Reflection *reflection;
    /*0x200*/ struct BgPil2Layer layers[3];
    /*0x308*/ struct GMAModel *lampLight;
    /*0x30C*/ s32 lampCount;
    /*0x310*/ struct StageBgModel *lamps[8];
};

struct BgPil2Beam
{
    char *name;
    u32 flags;
    struct StageBgModel *model;
};

void GXSetTevKColorSel_cached(int stage, int sel);
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
void GXLoadPosMtxImm(Mtx m, int id);
void GXLoadNrmMtxImm(Mtx m, int id);
void C_MTXLookAt(Mtx m, Vec *pos, Vec *up, Vec *target);
void GXSetTevKAlphaSel_cached(int stage, int sel);
void fn_1_D8130(int, int, struct RenderEffect *);
void mtxa_from_translate_xyz(float x, float y, float z);

extern struct RenderEffectFuncs lbl_1_data_533F0;
void GXInvalidateTexAll(void);
void GXSetTevSwapModeTable_cached(int table, int r, int g, int b, int a);
void g_GXSetTevIndirect_zero_if_different(int stage);
void *GXGetTexObjData(GXTexObj *obj);
u16 GXGetTexObjWidth(GXTexObj *obj);
u16 GXGetTexObjHeight(GXTexObj *obj);
int GXGetTexObjFmt(GXTexObj *obj);
int GXGetTexObjWrapS(GXTexObj *obj);
int GXGetTexObjWrapT(GXTexObj *obj);
void GXInitTexObj(GXTexObj *obj, void *image, u16 width, u16 height, int format, int wrapS, int wrapT, u8 mipmap);

extern struct RenderEffectFuncs lbl_1_data_53400;

void fn_1_8AAA8(struct BallEnvArg *);
void fn_1_8AFE4(struct BallEnvArg *);
void fn_1_8B45C(struct BgPil2Work *);
void fn_1_8B984(struct BgPil2Work *);
void fn_1_8BCA0(struct BgPil2Work *, u32);
int fn_1_8C160(int, struct GMAModelEntry *);
int fn_1_8C380(int, struct StageBgModel *);
void fn_1_8C3E4(void);
void fn_1_8C4A0(void);


static struct BgModelSearch lbl_1_data_31FB4[] =
{
    { BG_MDL_CMP_FULL, "TAR_WATER_SPEC" },
    { BG_MDL_CMP_FULL, "TAR_IND_MAP_A" },
    { BG_MDL_CMP_FULL, "TAR_SEA_MASK" },
    { BG_MDL_CMP_FULL, "TAR_VTX_SEA_BALL_A" },
    { BG_MDL_CMP_FULL, "TAR_IND_MAP_B" },
    { BG_MDL_CMP_FULL, "TAR_IND_MAP_C" },
    { BG_MDL_CMP_PREFIX, "TAR_HAMON_" },
    { BG_MDL_CMP_PREFIX, "pen_ocean" },
    { BG_MDL_CMP_PREFIX, "pen_B_ocean" },
    { BG_MDL_CMP_FULL, "LAMP_LIGHT" },
    { BG_MDL_CMP_END },
};

static struct BgModelSearch lbl_1_data_32024[] =
{
    { BG_MDL_CMP_PREFIX, "TAR_SEA_" },
    { BG_MDL_CMP_FULL, "TAR_RAMP" },
    { BG_MDL_CMP_END },
};

void bg_pil2_init(void)
{
    struct BgPil2Work *work = world_theme.work;
    struct BgPil2Layer *layer;
    int i;
    Vec sp8;
    struct RenderEffect rendEfc;

    bg_init_base();
    if (work->initialized == 0)
    {
        work->lampLight = init_common_gma->modelEntries[94].model;
        work->hamonCount = 0;
        g_smth_with_bg_model_names(lbl_1_data_31FB4, fn_1_8C160);
        work->initialized = 1;
    }
    work->sea = NULL;
    work->lampCount = 0;
    g_init_stagedef_bg_fg_models(stagedef->bgModels, stagedef->bgModelCount, lbl_1_data_32024, fn_1_8C380);
    g_init_stagedef_bg_fg_models(stagedef->fgModels, stagedef->fgModelCount, lbl_1_data_32024, fn_1_8C380);

    memset(&rendEfc, 0, sizeof(rendEfc));
    rendEfc.cameraMask = 0xFFFF;
    if ((modeCtrl.courseFlags & 0x04000000) == 0)
        rendEfc.funcs = &lbl_1_data_533F0;
    else
        rendEfc.funcs = &lbl_1_data_53400;
    fn_1_D8130(0, 0, &rendEfc);

    work->unk14 = rand() & 1;

    layer = work->layers;
    layer->pos.x = RAND_FLOAT();
    layer->pos.y = RAND_FLOAT();
    layer->pos.z = RAND_FLOAT();
    sp8.x = 0.0013888889f * (0.8f + (0.4f * RAND_FLOAT()));
    sp8.y = 0.0023809525f * (0.8f + (0.4f * RAND_FLOAT()));
    sp8.z = 0.0f;
    mtxa_from_rotate_z(rand() & 0x7FFF);
    mtxa_tf_vec(&sp8, &layer->vel);
    layer->scale.x = 0.0033333334f * (0.95f + (0.1f * RAND_FLOAT()));
    layer->scale.y = 0.0033333334f * (0.95f + (0.1f * RAND_FLOAT()));
    layer->scale.z = 1.0f;
    layer->rotZ = (s16)(rand() & 0x7FFF);
    layer++;

    layer->pos.x = RAND_FLOAT();
    layer->pos.y = RAND_FLOAT();
    layer->pos.z = RAND_FLOAT();
    sp8.x = 0.0042735045f * (0.8f + (0.4f * RAND_FLOAT()));
    sp8.y = 0.0055555557f * (0.8f + (0.4f * RAND_FLOAT()));
    sp8.z = 0.0f;
    mtxa_from_rotate_z(rand() & 0x7FFF);
    mtxa_tf_vec(&sp8, &layer->vel);
    layer->scale.x = 0.0125f * (0.95f + (0.1f * RAND_FLOAT()));
    layer->scale.y = 0.0125f * (0.95f + (0.1f * RAND_FLOAT()));
    layer->scale.z = 1.0f;
    layer->rotZ = (s16)((int)(4096.0f * RAND_FLOAT()) + 0x1000 + (layer - 1)->rotZ);
    layer++;

    layer->pos.x = RAND_FLOAT();
    layer->pos.y = RAND_FLOAT();
    layer->pos.z = RAND_FLOAT();
    sp8.x = 0.0042735045f * (0.8f + (0.4f * RAND_FLOAT()));
    sp8.y = 0.0055555557f * (0.8f + (0.4f * RAND_FLOAT()));
    sp8.z = 0.0f;
    mtxa_from_rotate_z(rand() & 0x7FFF);
    mtxa_tf_vec(&sp8, &layer->vel);
    layer->scale.x = 0.05f * (0.95f + (0.1f * RAND_FLOAT()));
    layer->scale.y = 0.05f * (0.95f + (0.1f * RAND_FLOAT()));
    layer->scale.z = 1.0f;
    layer->rotZ = (s16)((int)(4096.0f * RAND_FLOAT()) + 0x1000 + (layer - 1)->rotZ);

    layer = work->layers;
    for (i = 3; i > 0; i--, layer++)
    {
        mtxa_from_translate(&layer->pos);
        mtxa_scale(&layer->scale);
        mtxa_rotate_z(layer->rotZ);
        mtxa_rotate_x(0x4000);
        mtxa_to_mtx(layer->mtx);
    }

    work->scrollX = 0.1f * (RAND_FLOAT() - 0.5f) / 60.0f;
    work->scrollY = 0.1f * (RAND_FLOAT() - 0.5f) / 60.0f;
    work->scaleA = 5.5f * (1.0f + (0.5f * RAND_FLOAT()));
    work->scaleB = 8.0f * (1.0f + RAND_FLOAT());
    fn_1_8C3E4();
}

void bg_pil2_tick(void)
{
    struct BgPil2Work *work = world_theme.work;
    int i;
    struct BgPil2Layer *layer;

    bg_tick_base();
    if (!is_game_paused_and_in_view_stage())
    {
        layer = work->layers;
        for (i = 3; i > 0; i--, layer++)
        {
            layer->pos.x += layer->vel.x;
            layer->pos.y += layer->vel.y;
            layer->pos.z += layer->vel.z;
            mtxa_from_translate(&layer->pos);
            mtxa_scale(&layer->scale);
            mtxa_rotate_z(layer->rotZ);
            mtxa_rotate_x(0x4000);
            mtxa_to_mtx(layer->mtx);
        }
    }
}

void bg_pil2_dest(void) {}

void bg_pil2_disp(void)
{
    struct BgPil2Work *work = world_theme.work;
    u32 mask = g_smth_with_challenge_mode_var_and_3p();
    struct BgPil2Reflection *reflection;

    if (work->sea != NULL)
        work->sea->flags = work->sea->flags & ~0x10000;
    fn_1_8C4A0();
    bg_disp_base();
    fn_1_8BCA0(work, mask);
    if (work->sea != NULL && !(g_some_render_flag.flags & 4))
    {
        reflection = work->reflection;
        if (reflection == NULL || g_something_with_camera3 != modeCtrl.currPlayer || reflection->state == 0)
        {
            fn_1_8B984(work);
        }
        else
        {
            GXInvalidateTexAll();
            fn_1_8B45C(work);
        }
    }
}

void bg_pil2_item_coin_coli(void) {}

void fn_1_8AAA8(struct BallEnvArg *arg)
{
    struct BgPil2Work *work = world_theme.work;
    struct BgPil2Reflection *reflection = work->reflection;
    struct TevStageInfo sp40 = arg->tev;
    float sp28[2][3];
    float sp10[2][3];

    GXLoadTexObj_cached(&reflection->indTex, sp40.texMapId);
    GXLoadTexObj_cached(&reflection->tex, sp40.texMapId + 1);
    GXLoadTexObj_cached(&reflection->indTex, sp40.texMapId + 2);
    GXLoadTexObj_cached(work->waterSpecTex, sp40.texMapId + 3);
    GXLoadTexMtxImm(work->reflectTexMtx, sp40.texMtxId, 0);
    GXLoadTexMtxImm(work->mirrorTexMtx, sp40.texMtxId + 3, 0);

    tex_coord_gen(sp40.texCoordId, 0, 0, sp40.texMtxId + 3);
    GXSetTevSwapMode_cached(sp40.tevStage, 0, 2);
    g_GXSetTevOrder_wrapper(sp40.tevStage, sp40.texCoordId, sp40.texMapId, 0xFF);
    GXSetTevColorIn_cached(sp40.tevStage, 0xF, 0xF, 0xF, 8);
    GXSetTevColorOp_cached(sp40.tevStage, 0, 0, 0, 1, 1);
    GXSetTevAlphaIn_cached(sp40.tevStage, 7, 4, 0, 7);
    GXSetTevAlphaOp_cached(sp40.tevStage, 0, 0, 0, 1, 1);
    g_GXSetTevIndirect_zero_if_different(sp40.tevStage);
    sp40.tevStage++;
    sp40.texCoordId++;
    sp40.texMapId++;

    tex_coord_gen(sp40.texCoordId, 0, 0, sp40.texMtxId);
    GXSetTevSwapMode_cached(sp40.tevStage, 0, 1);
    g_GXSetTevOrder_wrapper(sp40.tevStage, sp40.texCoordId, sp40.texMapId, 0xFF);
    GXSetTevColorIn_cached(sp40.tevStage, 0xF, 8, 2, 0);
    GXSetTevColorOp_cached(sp40.tevStage, 0, 0, 0, 1, 0);
    GXSetTevAlphaIn_cached(sp40.tevStage, 7, 4, 0, 0);
    GXSetTevAlphaOp_cached(sp40.tevStage, 0, 0, 0, 1, 0);
    GXSetIndTexOrder_cached(sp40.tevIndStage, sp40.texCoordId - 1, sp40.texMapId + 1);
    sp28[0][0] = 0.0f;
    sp28[0][1] = 0.22f;
    sp28[0][2] = 0.0f;
    sp28[1][0] = 0.22f;
    sp28[1][1] = 0.0f;
    sp28[1][2] = 0.0f;
    g_GXSetIndTexMtx_wrapper(sp40.unk1C, sp28, 0);
    GXSetTevIndirect_cached(sp40.tevStage, sp40.tevIndStage, 0, 7, sp40.unk1C, 0, 0, 0, 0, 0);
    sp40.unk1C++;
    sp40.tevStage++;
    sp40.texCoordId += 2;
    sp40.texMtxId += 6;
    sp40.texMapId += 2;

    GXSetTevKColorSel_cached(sp40.tevStage, 1);
    GXSetTevKAlphaSel_cached(sp40.tevStage, 0);
    GXLoadTexMtxImm(work->specTexMtx, sp40.texMtxId, 1);
    tex_coord_gen(sp40.texCoordId, 1, 0, sp40.texMtxId);
    GXSetTevSwapMode_cached(sp40.tevStage, 0, 1);
    g_GXSetTevOrder_wrapper(sp40.tevStage, sp40.texCoordId, sp40.texMapId, 0xFF);
    GXSetTevColorIn_cached(sp40.tevStage, 0xF, 8, 0xE, 0);
    GXSetTevColorOp_cached(sp40.tevStage, 0, 0, 0, 1, 0);
    GXSetTevAlphaIn_cached(sp40.tevStage, 0, 6, 4, 7);
    GXSetTevAlphaOp_cached(sp40.tevStage, 0, 0, 0, 1, 0);
    sp10[0][0] = 0.0f;
    sp10[0][1] = 0.8f;
    sp10[0][2] = 0.0f;
    sp10[1][0] = -0.9f;
    sp10[1][1] = 0.0f;
    sp10[1][2] = 0.0f;
    g_GXSetIndTexMtx_wrapper(sp40.unk1C, sp10, 0);
    GXSetTevIndirect_cached(sp40.tevStage, sp40.tevIndStage, 0, 7, sp40.unk1C, 0, 0, 0, 0, 0);
    sp40.tevIndStage++;
    sp40.unk1C += 2;
    sp40.tevStage++;
    sp40.texCoordId++;
    sp40.texMtxId += 3;
    sp40.texMapId++;

    arg->tev = sp40;
}

void fn_1_8AFE4(struct BallEnvArg *arg)
{
    struct BgPil2Work *work = world_theme.work;
    float timer = world_theme.animTimer;
    struct TevStageInfo sp28 = arg->tev;
    GXTexObj *tex;
    float sp10[2][3];

    tex = work->oceanTex[g_font_char_var_3 % work->oceanTexCount];
    if (tex != NULL)
    {
        GXLoadTexObj_cached(tex, sp28.texMapId);
        GXLoadTexObj_cached(tex, sp28.texMapId + 1);
    }
    GXLoadTexObj_cached(work->waterSpecTex, sp28.texMapId + 2);
    GXLoadTexMtxImm(work->specTexMtx, sp28.texMtxId, 1);

    mtxa_push();
    mtxa_from_translate_xyz(work->scrollX * timer, work->scrollY * timer, 1.0f);
    mtxa_push();
    mtxa_scale_s(work->scaleA);
    GXLoadTexMtxImm(mtxa->mtxA, sp28.texMtxId + 3, 1);
    mtxa_pop();
    mtxa_scale_s(work->scaleB);
    GXLoadTexMtxImm(mtxa->mtxA, sp28.texMtxId + 6, 1);
    mtxa_pop();

    GXSetTevKColorSel_cached(sp28.tevStage, 1);
    tex_coord_gen(sp28.texCoordId, 1, 0, sp28.texMtxId);
    GXSetTevSwapMode_cached(sp28.tevStage, 0, 1);
    g_GXSetTevOrder_wrapper(sp28.tevStage, sp28.texCoordId, sp28.texMapId + 2, 0xFF);
    GXSetTevColorIn_cached(sp28.tevStage, 0xF, 8, 0xE, 0);
    GXSetTevColorOp_cached(sp28.tevStage, 0, 0, 0, 1, 0);
    GXSetTevAlphaIn_cached(sp28.tevStage, 0, 6, 4, 7);
    GXSetTevAlphaOp_cached(sp28.tevStage, 0, 0, 0, 1, 0);
    tex_coord_gen(sp28.texCoordId + 1, 1, 4, sp28.texMtxId + 3);
    GXSetIndTexOrder_cached(sp28.tevIndStage, sp28.texCoordId + 1, sp28.texMapId);
    sp10[0][0] = 0.0f;
    sp10[0][1] = -0.8f;
    sp10[0][2] = 0.0f;
    sp10[1][0] = 0.05f;
    sp10[1][1] = 0.0f;
    sp10[1][2] = 0.0f;
    g_GXSetIndTexMtx_wrapper(sp28.unk1C, sp10, 0);
    GXSetTevIndirect_cached(sp28.tevStage, sp28.tevIndStage, 0, 7, sp28.unk1C, 0, 0, 0, 0, 0);
    sp28.tevIndStage++;
    sp28.tevStage++;
    sp28.texCoordId += 2;

    GXSetTevKAlphaSel_cached(sp28.tevStage, 4);
    tex_coord_gen(sp28.texCoordId, 1, 4, sp28.texMtxId + 6);
    GXSetTevSwapMode_cached(sp28.tevStage, 0, 2);
    g_GXSetTevOrder_wrapper(sp28.tevStage, sp28.texCoordId, sp28.texMapId + 1, 0xFF);
    GXSetTevColorIn_cached(sp28.tevStage, 8, 0xF, 0, 0);
    GXSetTevColorOp_cached(sp28.tevStage, 0, 0, 0, 1, 0);
    GXSetTevAlphaIn_cached(sp28.tevStage, 7, 4, 6, 0);
    GXSetTevAlphaOp_cached(sp28.tevStage, 0, 0, 0, 1, 0);
    g_GXSetTevIndirect_zero_if_different(sp28.tevStage);
    sp28.tevStage++;
    sp28.texCoordId++;
    sp28.texMapId += 3;
    sp28.texMtxId += 9;

    arg->tev = sp28;
}

void fn_1_8B45C(struct BgPil2Work *work)
{
    struct StageBgModel *sea = work->sea;
    struct BgPil2Reflection *reflection = work->reflection;
    void *prevFunc;
    Mtx spC4;
    Mtx sp94;
    Mtx44 sp54;
    struct { Vec n; float d; } sp44;
    Vec sp38;
    Vec sp2C;
    Vec sp20;
    Vec sp14;
    Vec sp8;
    float d;
    float len;
    float x;

    C_MTXPerspective(sp54, 0.005493164f * g_current_camera->sub28.view.fov,
        g_current_camera->sub28.view.aspect, 0.1f, 20000.0f);
    mtxa_from_identity();
    mtxa->mtxA[0][2] = 0.5 - 0.5 * (g_current_camera->sub28.view.tanHalfFov *
        (g_current_camera->sub28.view.aspect * (sp54[0][0] * g_current_camera->sub28.view.unk28)));
    mtxa->mtxA[1][2] = 0.5 - 0.5 * (g_current_camera->sub28.view.tanHalfFov *
        (sp54[1][1] * g_current_camera->sub28.view.unk2C));
    x = 0.5f / math_tan(0.5 * g_current_camera->sub28.view.fov);
    mtxa->mtxA[0][0] = x / g_current_camera->sub28.view.aspect;
    mtxa->mtxA[1][1] = x;
    mtxa_to_mtx(spC4);

    mtxa_from_mtx(spC4);
    mtxa_to_mtx(sp94);
    mtxa_from_mtxb();
    mtxa_mult_right(reflection->mtx);
    mtxa_mult_left(sp94);
    mtxa_to_mtx(work->reflectTexMtx);

    mtxa_from_mtx(sp94);
    mtxa_scale_xyz(-1.0f, 1.0f, 1.0f);
    mtxa_mult_right(mtxa->mtxB);
    mtxa_mult_right(reflection->mtx);
    mtxa_to_mtx(work->mirrorTexMtx);

    mtxa_from_mtx(reflection->mtx);
    mtxa_tf_point_xyz(0.0f, 0.0f, 0.0f, &sp38);
    mtxa_tf_vec_xyz(0.0f, 1.0f, 0.0f, &sp44.n);

    mtxa_from_rotate_y(0x6800);
    mtxa_rotate_x(lbl_1_bss_2F48.infLightRotX);
    mtxa_tf_vec_xyz(0.0f, 0.0f, 1.0f, &sp2C);

    d = -2.0f * mathutil_vec_dot_prod(&sp44.n, &sp2C);
    sp20.x = sp2C.x + (d * sp44.n.x);
    sp20.y = sp2C.y + (d * sp44.n.y);
    sp20.z = sp2C.z + (d * sp44.n.z);
    sp20.x = -sp20.x;
    sp20.y = -sp20.y;
    sp20.z = -sp20.z;

    mtxa_from_mtxb();
    mtxa_rigid_inv_tf_tl(&sp14);
    sp8.x = sp14.x - sp38.x;
    sp8.y = sp14.y - sp38.y;
    sp8.z = sp14.z - sp38.z;
    /* Reuse the accumulator while retaining the first dot product separately. */
#ifdef __MWERKS__
    {
        register float sum, ay1, az1, ax2, ay2, az2, carry, by1, bz1, bx2, by2, bz2;
        register Vec *pa;
        register Vec *pb;
        pb = &sp44.n;
        pa = &sp8;
        asm
        {
            lfs sum, pa->x
            lfs ax2, pb->x
            lfs ay1, pa->y
            lfs ay2, pb->y
            lfs az1, pa->z
            lfs az2, pb->z
            fmuls sum, sum, ax2
            fmadds sum, ay1, ay2, sum
            fmadds sum, az1, az2, sum
            fmr carry, sum
        }
        pb = &sp44.n;
        pa = &sp20;
        asm
        {
            lfs sum, pa->x
            lfs bx2, pb->x
            lfs by1, pa->y
            lfs by2, pb->y
            lfs bz1, pa->z
            lfs bz2, pb->z
            fmuls sum, sum, bx2
            fmadds sum, by1, by2, sum
            fmadds sum, bz1, bz2, sum
        }
        len = carry * sum;
    }
#else
    len = mathutil_vec_dot_prod(&sp8, &sp44.n) * mathutil_vec_dot_prod(&sp20, &sp44.n);
#endif
    vec_set_len(len, &sp20, &sp8);
    sp14.x += sp8.x;
    sp14.y += sp8.y;
    sp14.z += sp8.z;
    sp8.x = sp14.x + sp2C.x;
    sp8.y = sp14.y + sp2C.y;
    sp8.z = sp14.z + sp2C.z;
    C_MTXLookAt(sp94, &sp14, &sp44.n, &sp8);

    mtxa_from_translate_xyz(0.5f, 0.5f, 0.0f);
    mtxa_scale_s(0.5f / (4.0f * len));
    mtxa_mult_right(sp94);
    mtxa_to_mtx(work->specTexMtx);

    mtxa_from_mtxb();
    mtxa_mult_right(reflection->mtx);
    load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
    GXSetTevSwapModeTable_cached(2, 1, 1, 1, 2);
    GXSetZCompLoc_cached(1);
    prevFunc = g_yet_another_unk_draw_func(fn_1_8AAA8);
    avdisp_set_z_mode(1, 3, 0);
    avdisp_draw_model_culled_sort_never(sea->model);
    avdisp_set_z_mode(1, 3, 1);
    g_yet_another_unk_draw_func(prevFunc);
    GXSetZCompLoc_cached(0);
}

void fn_1_8B984(struct BgPil2Work *work)
{
    float _dummy = 0.0f;
    struct StageBgModel *sea = work->sea;
    void *prevFunc;
    Mtx sp54;
    struct { Vec n; float d; } sp44;
    Vec sp38;
    Vec sp2C;
    Vec sp20;
    Vec sp14;
    Vec sp8;
    float d;
    float len;

    sp38.x = 0.0f;
    sp38.y = 0.0f;
    sp38.z = 0.0f;
    sp44.n.x = 0.0f;
    sp44.n.y = 1.0f;
    sp44.n.z = 0.0f;

    mtxa_from_rotate_y(0x6800);
    mtxa_rotate_x(lbl_1_bss_2F48.infLightRotX);
    mtxa_tf_vec_xyz(0.0f, 0.0f, 1.0f, &sp2C);

    d = -2.0f * mathutil_vec_dot_prod(&sp44.n, &sp2C);
    sp20.x = sp2C.x + (d * sp44.n.x);
    sp20.y = sp2C.y + (d * sp44.n.y);
    sp20.z = sp2C.z + (d * sp44.n.z);
    sp20.x = -sp20.x;
    sp20.y = -sp20.y;
    sp20.z = -sp20.z;

    mtxa_from_mtxb();
    mtxa_rigid_inv_tf_tl(&sp14);
    sp8.x = sp14.x - sp38.x;
    sp8.y = sp14.y - sp38.y;
    sp8.z = sp14.z - sp38.z;
    len = mathutil_vec_dot_prod(&sp8, &sp44.n) * mathutil_vec_dot_prod(&sp20, &sp44.n);
    vec_set_len(len, &sp20, &sp8);
    sp14.x += sp8.x;
    sp14.y += sp8.y;
    sp14.z += sp8.z;
    sp8.x = sp14.x + sp2C.x;
    sp8.y = sp14.y + sp2C.y;
    sp8.z = sp14.z + sp2C.z;
    C_MTXLookAt(sp54, &sp14, &sp44.n, &sp8);

    mtxa_from_translate_xyz(0.5f, 0.5f, 0.0f);
    mtxa_scale_s(0.5f / (6.0f * len));
    mtxa_mult_right(sp54);
    mtxa_to_mtx(work->specTexMtx);

    mtxa_from_mtxb_translate(&sea->pos);
    load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
    GXSetTevSwapModeTable_cached(2, 1, 1, 1, 2);
    GXSetZCompLoc_cached(1);
    prevFunc = g_yet_another_unk_draw_func(fn_1_8AFE4);
    avdisp_set_z_mode(1, 3, 0);
    avdisp_draw_model_culled_sort_never(sea->model);
    avdisp_set_z_mode(1, 3, 1);
    g_yet_another_unk_draw_func(prevFunc);
    GXSetZCompLoc_cached(0);
}

/* Lamp glow: a flare billboard towards the camera, then a fading chain of
 * sprites running out of each side of the lamp. */
void fn_1_8BCA0(struct BgPil2Work *work, u32 mask)
{
    float _dummy = 0.0f;  /* force rodata base into r31 */
    int j;
    struct GMAModel *light;
    struct StageBgModel *bgModel;
    struct GMAModel *model;
    int i;
    struct StageBgModel **lamp;
    float lightRadius;
    float radius;
    float a;
    float b;
    float c;
    float tanHalfFov;
    Vec sp8;

    light = work->lampLight;
    lightRadius = light->boundSphereRadius;
    tanHalfFov = g_current_camera->sub28.view.tanHalfFov;
    lamp = work->lamps;
    for (i = work->lampCount; i > 0; i--, lamp++)
    {
        bgModel = *lamp;
        if ((bgModel->flags & mask) != 0)
        {
            model = bgModel->model;
            radius = model->boundSphereRadius;
            mtxa_from_mtxb_translate(&bgModel->pos);
            mtxa_rotate_z(bgModel->rotZ);
            mtxa_rotate_y(bgModel->rotY);
            mtxa_rotate_x(bgModel->rotX);
            mtxa_scale(&bgModel->scale);
            mtxa_rotate_y(-0x2000);
            mtxa_translate(&model->boundSphereCenter);
            avdisp_set_z_mode(1, 3, 0);

            mtxa_push();
            mtxa_tf_vec_xyz(0.0f, 0.0f, -1.0f, &sp8);
            c = a = 0.25 + __fabs(sp8.z);
            mtxa_sq_from_identity();
            mathutil_mtxA_get_translate(&sp8);
            if (sp8.z < -1.8f)
            {
                b = (1.5f + sp8.z) / sp8.z;
                sp8.x *= b;
                sp8.y *= b;
                sp8.z += 1.5f;
                mathutil_mtxA_set_translate(&sp8);
                mtxa_scale_s(b);
                b = -(0.25f * (sp8.z * tanHalfFov));
                b = 1.0f + 0.25f * (b - 1.0f);
                c /= 0.2f * b;
                mtxa_scale_s(b);
                mtxa_scale_s(1.0f / lightRadius);
                mtxa_rotate_z(16384.0f * (sp8.x + sp8.y) / sp8.z);
                b = 1.0f + a;
                mtxa_scale_xyz(1.0f + 1.5f * (b - 1.0f), b, b);
                {
                    float red;
                    float green;
                    float blue;

                    if (c < 1.0f)
                    {
                        red = c;
                        green = c;
                        blue = c;
                    }
                    else
                    {
                        red = 1.0f;
                        green = 1.0f;
                        blue = 1.0f;
                    }
                    set_post_mult_color(red, green, blue, 1.0f);
                }
                avdisp_draw_model_culled_sort_always(light);
            }
            mtxa_pop();

            mtxa_push();
            mtxa_tf_vec_xyz(0.0f, 0.0f, 1.0f, &sp8);
            a = 0.5f * (1.0f + sp8.z);
            mtxa_translate_xyz(0.0f, 0.0f, radius);
            for (j = 8; j > 0; j--)
            {
                b = (j - 1) / 7.0f;
                c = a * (0.8f + (-0.6f * b));
                set_post_mult_color(c, c, c, 1.0f);
                mtxa_push();
                mtxa_translate_xyz(0.0f, 0.0f, 4.0f * b);
                mtxa_sq_from_identity();
                b = 1.5f + 1.5f * b;
                mtxa_scale_s(b / lightRadius);
                avdisp_draw_model_culled_sort_always(light);
                mtxa_pop();
            }
            mtxa_pop();

            mtxa_push();
            mtxa_tf_vec_xyz(0.0f, 0.0f, -1.0f, &sp8);
            a = 0.5f * (1.0f + sp8.z);
            mtxa_translate_xyz(0.0f, 0.0f, -radius);
            for (j = 8; j > 0; j--)
            {
                b = (j - 1) / 7.0f;
                c = a * (0.8f + (-0.6f * b));
                set_post_mult_color(c, c, c, 1.0f);
                mtxa_push();
                mtxa_translate_xyz(0.0f, 0.0f, -(4.0f * b));
                mtxa_sq_from_identity();
                b = 1.5f + 1.5f * b;
                mtxa_scale_s(b / lightRadius);
                avdisp_draw_model_culled_sort_always(light);
                mtxa_pop();
            }
            mtxa_pop();

            fade_color_base_default();
            avdisp_set_z_mode(1, 3, 1);
        }
    }
}

int fn_1_8C160(int index, struct GMAModelEntry *entry)
{
    struct BgPil2Work *work = world_theme.work;
    u8 *p;
    GXTexObj *tex;
    int n;

    switch (index)
    {
    case 0:
        work->waterSpecTex = entry->model->texObjs;
        break;
    case 1:
        work->indMapATex = entry->model->texObjs;
        break;
    case 2:
        work->seaMask = entry->model;
        break;
    case 3:
        work->vtxSeaBall = entry->model;
        break;
    case 4:
    case 5:
    case 6:
        if (work->hamonCount < 8)
        {
            work->hamon[work->hamonCount] = entry->model;
            work->hamonCount++;
        }
        break;
    case 7:
        p = &((u8 *)entry->name)[strlen(entry->name) - 1];
        while (*p >= '0' && *p <= '9')
            p--;
        n = atoi(p + 1) - 1;
        if (n < 32)
        {
            GXTexObj *oceanTex = entry->model->texObjs;
            work->oceanTex[n] = oceanTex;
            if (work->oceanTexCount < n + 1)
                work->oceanTexCount = n;
        }
        break;
    case 8:
        p = &((u8 *)entry->name)[strlen(entry->name) - 1];
        while (*p >= '0' && *p <= '9')
            p--;
        n = atoi(p + 1) - 1;
        if (n < 32)
        {
            tex = entry->model->texObjs;
            work->oceanBTex[n] = tex;
            if (work->oceanBTexCount < n + 1)
                work->oceanBTexCount = n;
            GXInitTexObj(tex, GXGetTexObjData(tex), GXGetTexObjWidth(tex), GXGetTexObjHeight(tex),
                GXGetTexObjFmt(tex), GXGetTexObjWrapS(tex), GXGetTexObjWrapT(tex), 1);
        }
        break;
    case 9:
        work->lampLight = entry->model;
        break;
    }
    return 1;
}

int fn_1_8C380(int index, struct StageBgModel *model)
{
    struct BgPil2Work *work = world_theme.work;

    switch (index)
    {
    case 0:
        work->sea = model;
        work->seaModel = model->model;
        break;
    case 1:
        if (work->lampCount < 8)
        {
            work->lamps[work->lampCount] = model;
            work->lampCount++;
        }
        break;
    }
    return 1;
}

static struct BgPil2Beam lbl_1_data_321A4[] =
{
    { "TAR_TEKKOTU01_L", 0 },
    { "TAR_TEKKOTU01_R", 0 },
    { "TAR_TEKKOTU01_B", 1 },
    { "TAR_TEKKOTU02_L", 0 },
    { "TAR_TEKKOTU02_R", 0 },
    { "TAR_TEKKOTU02_B", 0 },
    { "TAR_TEKKOTU03_L", 0 },
    { "TAR_TEKKOTU03_R", 0 },
    { "TAR_TEKKOTU03_F", 0 },
    { "TAR_JUMPDAI_A", 0 },
    { "TAR_JUMPDAI_B", 0 },
    { "TAR_JUMPDAI_C", 0 },
    { "TAR_JUMPDAI_D", 0 },
    { "TAR_JUMPDAI_E", 0 },
    { "TAR_JUMPDAI_F", 0 },
    { "TAR_JUMPDAI_G", 0 },
    { "TAR_JUMPDAI_H", 0 },
    { "TAR_JUMPDAI_I", 0 },
    { "TAR_JUMPDAI_J", 0 },
    { "TAR_JUMPDAI_K", 0 },
};

void fn_1_8C3E4(void)
{
    struct BgPil2Beam *beam = lbl_1_data_321A4;
    int i;
    struct StageBgModel *model;
    int j;

    for (i = 0; i < sizeof(lbl_1_data_321A4) / sizeof(lbl_1_data_321A4[0]); i++, beam++)
    {
        beam->model = NULL;
        model = stagedef->fgModels;
        for (j = 0; j < stagedef->fgModelCount; j++, model++)
        {
            if (strcmp(model->name, beam->name) == 0)
            {
                beam->model = model;
                break;
            }
        }
    }
}

/* Hides the beams and jump ramps the camera would otherwise be inside. */
void fn_1_8C4A0(void)
{
    struct Ball *ball = &balls[g_current_camera->idx];
    struct RaycastHit hit;

    raycast_stage_down(&g_current_camera->eye, &hit, NULL);
    {
        int mode = 0;
        struct BgPil2Beam *beam;
        int i;

        if (-8.70075 <= g_current_camera->eye.x && g_current_camera->eye.x <= 8.70075)
        {
            if (280.04651 <= g_current_camera->eye.z && g_current_camera->eye.z <= 400.22656 && (hit.flags & 1) != 0
                && g_current_camera->eye.y < 3.0 + hit.pos.y)
                mode = 2;
            else if (g_current_camera->eye.z > 400.22656 && g_current_camera->eye.y > 144.98485
                && (ball->unk17C.flags & 1) != 0 && ball->pos.y < 1.0 + ball->unk17C.pos.y)
                mode = 1;
        }

        beam = lbl_1_data_321A4;
        for (i = 0; i < sizeof(lbl_1_data_321A4) / sizeof(lbl_1_data_321A4[0]); i++, beam++)
        {
            struct StageBgModel *model = beam->model;

            if (model != NULL)
            {
                switch (mode)
                {
                case 0:
                    model->flags |= 0x10000;
                    break;
                case 1:
                    if ((beam->flags & 1) != 0)
                        model->flags |= 0x10000;
                    else
                        model->flags &= ~0x10000;
                    break;
                case 2:
                    model->flags &= ~0x10000;
                    break;
                }
            }
        }
    }
}
