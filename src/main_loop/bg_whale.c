/* Whale background: bone-animated whale body, water surface reflection/refraction
 * textures rendered off-screen, depth-driven lighting and the ball's water environment map. */

#include "types.h"
#include "avdisp.h"
#include "ball.h"
#include "background.h"
#include "camera.h"
#include "dip.h"
#include "effect.h"
#include "frustum.h"
#include "gx.h"
#include "libc.h"
#include "light.h"
#include "macros.h"
#include "mathutil.h"
#include "mode.h"
#include "nl2ngc.h"
#include "pool.h"
#include "rand.h"
#include "stage.h"
#include "stcoli.h"

struct BgWhaleBone
{
    /*0x00*/ u32 flags;  /* bit 0: no child bone (leaf) */
    /*0x04*/ struct BgWhaleBone *child;
    /*0x08*/ Vec pos;
    /*0x14*/ Vec vel;
    /*0x20*/ Vec dir;
    /*0x2C*/ Vec dirVel;
    /*0x38*/ float scale;
    /*0x3C*/ s16 rotX;
    /*0x40*/ Mtx mtx;
    /*0x70*/ Vec amp;
    /*0x7C*/ s16 phase[3];
    /*0x82*/ s16 speed[3];
    /*0x88*/ float rotAmp;
    /*0x8C*/ s16 rotPhase;
    /*0x8E*/ s16 rotSpeed;
    /*0x90*/ float scaleAmp;
    /*0x94*/ s16 scalePhase;
    /*0x96*/ s16 scaleSpeed;
    /*0x98*/ Vec basePos;
    /*0xA4*/ Vec childDir;
    /*0xB0*/ float len;
    /*0xB4*/ Mtx base;
};  /* size = 0xE4 */

struct BgWhaleScroll
{
    /*0x00*/ Vec pos;
    /*0x0C*/ Vec vel;
    /*0x18*/ Vec accel;
    /*0x24*/ Vec velHi;
    /*0x30*/ Vec velLo;
    /*0x3C*/ float scale;
};  /* size = 0x40 */

struct BgWhaleWave
{
    /*0x00*/ struct StageBgModel *model;
    /*0x04*/ Vec pos;
    /*0x10*/ float radius;
    /*0x14*/ Mtx mtx;
    /*0x44*/ Mtx invMtx;
};  /* size = 0x74 */

struct BgWhaleBody
{
    /*0x000*/ struct StageBgModel *body;
    /*0x004*/ struct GMAModel *model;
    /*0x008*/ s32 boneCount;
    /*0x00C*/ struct BgWhaleBone *bones;
    /*0x010*/ struct BgWhaleScroll scrollA;
    /*0x050*/ struct BgWhaleScroll scrollB;
    /*0x090*/ float unk90;
    /*0x094*/ s32 waveCount;
    /*0x098*/ struct BgWhaleWave waves[4];
    /*0x268*/ s32 texReady;
    /*0x26C*/ GXTexObj reflTexObj;
    /*0x28C*/ void *reflTexData;
    /*0x290*/ GXTexObj refrTexObj;
    /*0x2B0*/ void *refrTexData;
    /*0x2B4*/ GXTexObj *wavePatCTexObjs;
    /*0x2B8*/ GXTexObj *waveSpecATexObjs;
    /*0x2BC*/ Mtx mtx;
    /*0x2EC*/ s32 extraCount;
    /*0x2F0*/ struct StageBgModel *extra[12];
};  /* size = 0x320 */

struct BgWhaleLight
{
    /*0x00*/ struct StageBgModel *model;
    /*0x04*/ float t;
    /*0x08*/ Vec ambientA;
    /*0x14*/ Vec ambientB;
    /*0x20*/ Vec lightA;
    /*0x2C*/ Vec lightB;
    /*0x38*/ Vec ambient;
    /*0x44*/ Vec light;
};  /* size = 0x50 */

struct BgWhaleEnv
{
    /*0x000*/ Vec pos;
    /*0x00C*/ Vec vel;
    /*0x018*/ Vec velTarget;
    /*0x024*/ Vec accel;
    /*0x030*/ Vec scale;
    /*0x03C*/ Vec pos2;
    /*0x048*/ Vec vel2;
    /*0x054*/ Vec velTarget2;
    /*0x060*/ Vec accel2;
    /*0x06C*/ Vec scale2;
    /*0x078*/ GXTexObj *lightmapATexObjs;
    /*0x07C*/ GXTexObj *wavePatBTexObjs;
    /*0x080*/ Mtx mtx;
    /*0x0B0*/ Mtx mtx2;
    /*0x0E0*/ Mtx texMtx;
    /*0x110*/ Mtx texMtx2;
    /*0x140*/ struct TevStageInfo tev;
};  /* size = 0x16C */

struct BgWhaleWork
{
    /*0x000*/ s32 initialized;
    /*0x004*/ struct BgWhaleBody whale;
    /*0x324*/ struct BgWhaleLight light;
    /*0x374*/ GXTexObj *inLightATexObjs;
    /*0x378*/ struct BgWhaleEnv env;
};  /* size = 0x4E4 */

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

struct RenderEffectFuncs
{
    void (*init)(struct RenderEffect *);
    void (*dest)(struct RenderEffect *);
    void (*tick)(struct RenderEffect *);
    void (*disp)(int, struct RenderEffect *);
};

struct BgWhaleOrdNode
{
    /*0x00*/ struct OrdTblNode node;
    /*0x08*/ struct BgWhaleBody *whale;
    /*0x0C*/ Mtx mtx;
};  /* size = 0x3C */

extern struct
{
    /*0x00*/ u32 flags;
    /*0x04*/ Vec unk4;
    /*0x10*/ Vec unk10;
    /*0x1C*/ u32 unk1C;
    /*0x20*/ Vec unk20;
    /*0x2C*/ Vec unk2C;
    /*0x38*/ u32 unk38;
    /*0x3C*/ Vec unk3C;
    /*0x48*/ Vec unk48;
} g_some_render_flag;
extern void *g_some_buffer_ptr;
extern void *stage_heap;
extern struct
{
    /*0x00*/ u8 type;
    /*0x04*/ float unk4;
    /*0x08*/ float unk8;
} lbl_1_bss_CF22C;

void *OSFreeToHeap(void *heap, void *ptr);
void *alloc_from_heap_or_panic(void *heap, u32 size, char *file, int line);
u32 GXGetTexBufferSize(u16 width, u16 height, u32 format, u8 mipmap, u8 maxLod);
void GXInitTexObj(GXTexObj *obj, void *image, u16 width, u16 height, int format, int wrapS, int wrapT, u8 mipmap);
void GXSetTexCopySrc_cached(u16 left, u16 top, u16 width, u16 height);
void GXSetTexCopyDst_cached(u16 width, u16 height, int format, u8 mipmap);
void GXCopyTex(void *dest, int clear);
void GXInvalidateTexAll(void);
void gp_wait_wrapper(void);
void GXGetProjectionv(float *proj);
int GXGetTexObjWrapT(GXTexObj *obj);
int GXGetTexObjWrapS(GXTexObj *obj);
int GXGetTexObjFmt(GXTexObj *obj);
u16 GXGetTexObjHeight(GXTexObj *obj);
u16 GXGetTexObjWidth(GXTexObj *obj);
void *GXGetTexObjData(GXTexObj *obj);
struct OrdTblNode *ord_tbl_get_entry_for_pos(Vec *pos);
void *ord_tbl_alloc_node(u32 size);
void fn_1_CD418(struct OrdTblNode *list, struct OrdTblNode *node);
void ord_tbl_draw_nodes(void);
void draw_poly(struct GMAModel *model, Mtx **mtxs);
void pointer_range_advance(struct GMAModel *model, void *mtxs);
s16 *fn_80069964(struct GMAModel *model);
void fn_1_D8130(int, int, struct RenderEffect *);
void g_smth_with_bg_model_names(struct BgModelSearch *search, int (*callback)(int, struct GMAModelEntry *));
void GXLoadTexObj_cached(GXTexObj *obj, int map);
void GXLoadTexMtxImm(Mtx m, int id, int type);
void GXSetNumTexGens_cached(u8 n);
void GXSetNumTevStages_cached(u8 n);
void GXSetNumIndStages_cached(u8 n);
void GXSetCullMode_cached(int mode);
void GXSetIndTexOrder_cached(int stage, int coord, int map);
void GXSetTevIndirect_cached(int stage, int indStage, int format, int biasSel, int mtxSel, int wrapS, int wrapT,
    int addPrev, int utcLod, int alphaSel);
void GXSetTevColorIn_cached(int stage, int a, int b, int c, int d);
void GXSetTevColorOp_cached(int stage, int op, int bias, int scale, u8 clamp, int outReg);
void GXSetTevAlphaIn_cached(int stage, int a, int b, int c, int d);
void GXSetTevAlphaOp_cached(int stage, int op, int bias, int scale, u8 clamp, int outReg);
void GXSetTexCoordGen2_cached(int dst, int func, int src, int mtx, int normalize, int postMtx);
static inline void tex_coord_gen(int dst, int func, int src, int mtx)
{
    GXSetTexCoordGen2_cached(dst, func, src, mtx, 0, 0x7D);
}
void GXSetTevSwapMode_cached(int stage, int rasSel, int texSel);
void GXSetTevKColorSel_cached(int stage, int sel);
void GXSetTevKAlphaSel_cached(int stage, int sel);
void g_GXSetIndTexMtx_wrapper(int id, float mtx[2][3], int scale);
void g_GXSetTevOrder_wrapper(int stage, int coord, int map, int color);
void g_GXSetTevIndirect_zero_if_different(int stage);
void g_something_with_raster_state(int);
void mtxa_from_translate_xyz(float, float, float);
void *fn_1_10A69C(void *func);
#ifdef __MWERKS__
void fn_1_4E6FC(GXColor color);
#else
/* pc_port: MWCC passes structs by invisible reference, so this by-value
 * declaration and ball_draw.c's GXColor* definition agree on GC; on x86
 * they do not, so route the call through the address. */
void fn_1_4E6FC(GXColor *color);
#define fn_1_4E6FC(c) fn_1_4E6FC(&(c))
#endif
void g_set_something3(int);
u32 get_max_gx_fifo_use_size(void);
void fn_1_E2D20(void);
void fn_1_E2D64(u8, float, float, int, int, int);
void fn_1_E2E18(void);

void fn_1_9512C(struct BgWhaleWork *work);
void fn_1_95238(struct BgWhaleWork *work, int mask);
void g_something_with_stage_heap_and_whale_theme(struct BgWhaleWork *work);
void fn_1_959C4(struct BgWhaleWork *work);
void fn_1_95A60(struct BgWhaleWork *work, int mask);
void fn_1_95CD4(struct BgWhaleBody *whale, int mask);
void fn_1_95EBC(struct BgWhaleWork *work, int mask);
void fn_1_95FFC(struct BgWhaleOrdNode *node);
void g_something_with_stage_heap_and_whale_theme2(struct BgWhaleBody *whale);
void fn_1_966EC(struct BgWhaleBody *whale);
void fn_1_9673C(struct BgWhaleBody *whale, int mask);
void fn_1_96CA4(struct BgWhaleBody *whale);
void fn_1_96D1C(struct BgWhaleBody *whale);
void fn_1_97380(struct RenderEffect *);
void fn_1_97394(struct RenderEffect *);
void fn_1_97398(struct RenderEffect *);
void fn_1_9739C(int, struct RenderEffect *);
void fn_1_97418(int, struct RenderEffect *);
void fn_1_974DC(struct RenderEffect *);
int fn_1_97A80(void);
void fn_1_97A88(struct RenderEffect *);
void fn_1_98290(struct BgWhaleWork *work);
void fn_1_98698(struct BgWhaleWork *work);
void fn_1_9884C(struct BallEnvArg *arg);
void fn_1_98CD4(void);
void fn_1_98D4C(struct BgWhaleWork *work);
void fn_1_98D50(struct BgWhaleWork *work, int mask);
void fn_1_98D54(struct BgWhaleWork *work, int mask);
int fn_1_98D58(int, struct GMAModelEntry *);
int fn_1_98E68(int, struct StageBgModel *);

/* Unreferenced table (kept for data layout) */
static u32 lbl_1_data_32998[0x5C] =
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

static struct BgModelSearch s_modelSearch[] =
{
    { BG_MDL_CMP_FULL, "WHA_WAVE_A_skin" },
    { BG_MDL_CMP_FULL, "WHA_WAVE_PAT_C" },
    { BG_MDL_CMP_FULL, "WHA_WAVE_SPEC_A" },
    { BG_MDL_CMP_FULL, "WHA_IN_LIGHT_A" },
    { BG_MDL_CMP_FULL, "WHA_LIGHTMAP_A" },
    { BG_MDL_CMP_FULL, "WHA_WAVE_PAT_B" },
    { BG_MDL_CMP_END, NULL },
};

static struct BgModelSearch s_bgModelSearch[] =
{
    { BG_MDL_CMP_FULL, "WHA_WAVE_A_skin" },
    { BG_MDL_CMP_PREFIX, "WAVE_BALL_" },
    { BG_MDL_CMP_FULL, "WHA_MOUTH_A" },
    { BG_MDL_CMP_SUFFIX, "_ltmp" },
    { BG_MDL_CMP_PREFIX, "WHA_BODY_" },
    { BG_MDL_CMP_PREFIX, "WHA_BONE_A" },
    { BG_MDL_CMP_PREFIX, "WHA_HARABONE_A" },
    { BG_MDL_CMP_PREFIX, "WHA_SEBONE_A" },
    { BG_MDL_CMP_PREFIX, "WHA_SKY_" },
    { BG_MDL_CMP_PREFIX, "WHA_MOUTH_" },
    { BG_MDL_CMP_FULL, "WHA_CAP_A" },
    { BG_MDL_CMP_END, NULL },
};

static struct RenderEffectFuncs s_renderEffectFuncs =
{
    fn_1_97380,
    fn_1_97394,
    fn_1_97398,
    fn_1_9739C,
};

static struct RenderEffectFuncs s_renderEffectFuncsCourse =
{
    fn_1_97380,
    fn_1_97394,
    fn_1_97398,
    fn_1_97418,
};

void bg_whale_init(void)
{
    struct BgWhaleWork *work = world_theme.work;
    struct RenderEffect rendEfc;
    GXColor color;

    bg_init_base();
    if (!work->initialized)
    {
        g_smth_with_bg_model_names(s_modelSearch, fn_1_98D58);
        work->initialized = TRUE;
    }
    work->whale.waveCount = 0;
    work->whale.extraCount = 0;
    g_init_stagedef_bg_fg_models(
        stagedef->bgModels,
        stagedef->bgModelCount,
        s_bgModelSearch,
        fn_1_98E68);
    g_init_stagedef_bg_fg_models(
        stagedef->fgModels,
        stagedef->fgModelCount,
        s_bgModelSearch,
        fn_1_98E68);
    fn_1_9512C(work);
    g_something_with_stage_heap_and_whale_theme(work);
    fn_1_98D4C(work);
    fn_1_98290(work);

    memset(&rendEfc, 0, sizeof(rendEfc));
    rendEfc.cameraMask = 0xFFFF;
    if (!(modeCtrl.courseFlags & (1 << 26)))
        rendEfc.funcs = &s_renderEffectFuncs;
    else
        rendEfc.funcs = &s_renderEffectFuncsCourse;
    fn_1_D8130(0, 0, &rendEfc);

    color.a = 0xFF;
    color.r = 0x5B;
    color.g = 0x9C;
    color.b = 0xC1;
    fn_1_4E6FC(color);
}

void bg_whale_tick(void)
{
    struct BgWhaleWork *work = world_theme.work;
    int mask;

    mask = g_smth_with_challenge_mode_var_and_3p();
    bg_tick_base();
    if (!is_game_paused_and_in_view_stage())
    {
        fn_1_95238(work, mask);
        fn_1_95A60(work, mask);
        fn_1_98D50(work, mask);
        fn_1_98698(work);
    }
}

void bg_whale_dest(void)
{
    struct BgWhaleWork *work = world_theme.work;
    GXColor color;

    fn_1_959C4(work);
    color.a = 0xFF;
    color.r = 0xFF;
    color.g = 0xFF;
    color.b = 0xFF;
    fn_1_4E6FC(color);
}

void bg_whale_disp(void)
{
    struct BgWhaleWork *work = world_theme.work;
    int mask;

    mask = g_smth_with_challenge_mode_var_and_3p();
    fn_1_95EBC(work, mask);
    fn_1_98D54(work, mask);
    bg_disp_base();
}

void bg_whale_item_coin_coli(void) {}

void fn_1_9512C(struct BgWhaleWork *work)
{
    struct BgWhaleLight *light = &work->light;

    light->t = 0.0f;
    if (!(modeCtrl.courseFlags & (1 << 26)))
    {
        light->ambientA.x = 0.4f;
        light->ambientA.y = 0.4f;
        light->ambientA.z = 0.4f;
        light->ambientB.x = 0.25f;
        light->ambientB.y = 0.3f;
        light->ambientB.z = 0.4f;
        light->ambient.x = 0.0625f;
        light->ambient.y = 0.075f;
        light->ambient.z = 0.1f;
    }
    else
    {
        light->ambientA.x = 0.4f;
        light->ambientA.y = 0.4f;
        light->ambientA.z = 0.4f;
        light->ambientB.x = 0.3f;
        light->ambientB.y = 0.4f;
        light->ambientB.z = 0.45f;
        light->ambient.x = 0.3f;
        light->ambient.y = 0.4f;
        light->ambient.z = 0.45f;
    }
    light->lightA.x = 1.0f;
    light->lightA.y = 1.0f;
    light->lightA.z = 1.0f;
    light->lightB.x = 0.55f;
    light->lightB.y = 0.7f;
    light->lightB.z = 0.8f;
    light->light.x = 0.55f;
    light->light.y = 0.7f;
    light->light.z = 0.8f;
}

void fn_1_95238(struct BgWhaleWork *work, int mask)
{
    struct BgWhaleLight *light = &work->light;
    struct StageBgModel *model;
    struct Light *poolLight;
    float t;
    float inv;
    Vec a;
    Vec b;
    float x;
    float y;
    float z;
    float hi;
    float lo;

    model = light->model;
    if (model == NULL || !(model->flags & mask))
        return;

    hi = -50.0f * model->scale.y;
    lo = -450.0f * model->scale.y;
    t = (model->pos.y - lo) / (hi - lo);
    if (t < 0.0f)
        t = 0.0f;
    else if (t > 1.0f)
        t = 1.0f;
    inv = 1.0f - t;
    light->t = t;

    a = light->ambientA;
    b = light->ambientB;
    x = a.x * t + b.x * inv;
    y = a.y * t + b.y * inv;
    z = a.z * t + b.z * inv;
    light->ambient.x += 0.01f * (x - light->ambient.x);
    light->ambient.y += 0.01f * (y - light->ambient.y);
    light->ambient.z += 0.01f * (z - light->ambient.z);
    set_bg_ambient(light->ambient.x, light->ambient.y, light->ambient.z);

    poolLight = alloc_pool_light(1, 0);
    if (poolLight != NULL)
    {
        a = light->lightA;
        b = light->lightB;
        x = a.x * t + b.x * inv;
        y = a.y * t + b.y * inv;
        z = a.z * t + b.z * inv;
        if (x > light->light.x)
            light->light.x += 0.007f * (x - light->light.x);
        else
            light->light.x += 0.015f * (x - light->light.x);
        if (x > light->light.y)
            light->light.y += 0.007f * (y - light->light.y);
        else
            light->light.y += 0.015f * (y - light->light.y);
        if (x > light->light.z)
            light->light.z += 0.008f * (z - light->light.z);
        else
            light->light.z += 0.012f * (z - light->light.z);
        poolLight->red = light->light.x;
        poolLight->green = light->light.y;
        poolLight->blue = light->light.z;
    }
}

void g_something_with_stage_heap_and_whale_theme(struct BgWhaleWork *work)
{
    struct BgWhaleBody *whale = &work->whale;

    whale->scrollA.pos.x = RAND_FLOAT();
    whale->scrollA.pos.y = RAND_FLOAT();
    whale->scrollA.pos.z = 0.0f;
    whale->scrollA.accel.x = 0.0f;
    whale->scrollA.accel.y = 0.0f;
    whale->scrollA.velHi.x = -0.00033333333f * (RAND_FLOAT() - 0.5f);
    whale->scrollA.velHi.y = -0.0016666667f * (1.0f + 0.5f * RAND_FLOAT());
    whale->scrollA.velLo.x = -0.00016666666f * (RAND_FLOAT() - 0.5f);
    whale->scrollA.velLo.y = whale->scrollA.velHi.y * (0.25f + 0.25f * RAND_FLOAT());
    whale->scrollA.vel.x = whale->scrollA.velLo.x;
    whale->scrollA.vel.y = whale->scrollA.velLo.y;
    whale->scrollA.scale = 0.2f + 0.3f * RAND_FLOAT();

    whale->scrollB.pos.x = RAND_FLOAT();
    whale->scrollB.pos.y = RAND_FLOAT();
    whale->scrollB.pos.z = 0.0f;
    whale->scrollB.accel.x = 0.0f;
    whale->scrollB.accel.y = 0.0f;
    whale->scrollB.velHi.x = -whale->scrollA.velHi.x * (15.0f * RAND_FLOAT());
    whale->scrollB.velHi.y = -0.004166667f * (1.0f + 0.5f * RAND_FLOAT());
    whale->scrollB.velLo.x = -whale->scrollA.velLo.x * (8.0f * RAND_FLOAT());
    whale->scrollB.velLo.y = whale->scrollB.velHi.y * -(0.25f + 0.25f * RAND_FLOAT());
    whale->scrollB.vel.x = whale->scrollB.velLo.x;
    whale->scrollB.vel.y = whale->scrollB.velLo.y;
    whale->scrollB.scale = 0.8f + RAND_FLOAT();

    whale->unk90 = 0.25f * RAND_FLOAT();

    if (whale->body != NULL)
        g_something_with_stage_heap_and_whale_theme2(whale);

    whale->texReady = 0;
    whale->reflTexData = alloc_from_heap_or_panic(stage_heap, GXGetTexBufferSize(256, 256, 3, 0, 0), "bg_whale.c", 603);
    whale->refrTexData = alloc_from_heap_or_panic(stage_heap, GXGetTexBufferSize(512, 256, 4, 0, 0), "bg_whale.c", 608);
}

void fn_1_959C4(struct BgWhaleWork *work)
{
    struct BgWhaleBody *whale = &work->whale;

    if (whale->refrTexData != NULL || whale->reflTexData != NULL)
        gp_wait_wrapper();
    if (whale->refrTexData != NULL)
    {
        OSFreeToHeap(stage_heap, whale->refrTexData);
        whale->refrTexData = NULL;
    }
    if (whale->reflTexData != NULL)
    {
        OSFreeToHeap(stage_heap, whale->reflTexData);
        whale->reflTexData = NULL;
    }
    fn_1_966EC(whale);
}

void fn_1_95A60(struct BgWhaleWork *work, int mask)
{
    struct BgWhaleBody *whale = &work->whale;
    struct StageBgModel *body;
    float t;
    float velX;
    float velY;

    body = whale->body;
    if (body == NULL || !(body->flags & mask))
        return;

    t = work->light.t;
    velX = whale->scrollA.velLo.x + t * (whale->scrollA.velHi.x - whale->scrollA.velLo.x);
    velY = whale->scrollA.velLo.y + t * (whale->scrollA.velHi.y - whale->scrollA.velLo.y);
    whale->scrollA.accel.x += 0.05f * (0.05f * (velX - whale->scrollA.vel.x) - whale->scrollA.accel.x);
    whale->scrollA.accel.y += 0.1f * (0.1f * (velY - whale->scrollA.vel.y) - whale->scrollA.accel.y);
    whale->scrollA.vel.x += whale->scrollA.accel.x;
    whale->scrollA.vel.y += whale->scrollA.accel.y;
    whale->scrollA.vel.x *= 0.99f;
    whale->scrollA.vel.y *= 0.99f;
    whale->scrollA.pos.x += whale->scrollA.vel.x;
    whale->scrollA.pos.y += whale->scrollA.vel.y;

    velX = whale->scrollB.velLo.x + t * (whale->scrollB.velHi.x - whale->scrollB.velLo.x);
    velY = whale->scrollB.velLo.y + t * (whale->scrollB.velHi.y - whale->scrollB.velLo.y);
    whale->scrollB.accel.x += 0.025f * (0.025f * (velX - whale->scrollB.vel.x) - whale->scrollB.accel.x);
    whale->scrollB.accel.y += 0.05f * (0.05f * (velY - whale->scrollB.vel.y) - whale->scrollB.accel.y);
    whale->scrollB.vel.x += whale->scrollB.accel.x;
    whale->scrollB.vel.y += whale->scrollB.accel.y;
    whale->scrollB.vel.x *= 0.99f;
    whale->scrollB.vel.y *= 0.99f;
    whale->scrollB.pos.x += whale->scrollB.vel.x;
    whale->scrollB.pos.y += whale->scrollB.vel.y;

    velX = 0.4f + -0.15f * t;
    whale->unk90 += 0.05f * (velX - whale->unk90);

    fn_1_95CD4(whale, mask);
    fn_1_9673C(whale, mask);
}

void fn_1_95CD4(struct BgWhaleBody *whale, int mask)
{
    int i;
    struct StageBgModel *model;
    struct StageBgModel *body;
    struct BgWhaleWave *wave;
    float invScale;
    Mtx bodyInv;

    body = whale->body;
    mtxa_from_translate(&body->pos);
    mtxa_rotate_z(body->rotZ);
    mtxa_rotate_y(body->rotY);
    mtxa_rotate_x(body->rotX);
    mtxa_scale(&body->scale);
    mtxa_invert();
    mtxa_to_mtx(bodyInv);
    invScale = 1.0f / MAX(MAX(body->scale.x, body->scale.y), body->scale.z);

    wave = whale->waves;
    for (i = whale->waveCount; i > 0; i--, wave++)
    {
        model = wave->model;
        if (model->flags & mask)
        {
            mtxa_from_mtx(bodyInv);
            mtxa_translate(&model->pos);
            mtxa_rotate_z(model->rotZ);
            mtxa_rotate_y(model->rotY);
            mtxa_rotate_x(model->rotX);
            mtxa_scale(&model->scale);
            mtxa_scale_s(200.0f);
            mtxa_to_mtx(wave->mtx);
            mtxa_invert();
            mtxa_to_mtx(wave->invMtx);
            wave->pos = model->pos;
            wave->radius = invScale * (200.0f * MAX(MAX(model->scale.x, model->scale.y), model->scale.z));
        }
    }
}

void fn_1_95EBC(struct BgWhaleWork *work, int mask)
{
    struct BgWhaleBody *whale = &work->whale;
    struct GMAModel *model;
    struct StageBgModel *body;
    struct OrdTblNode *list;
    struct BgWhaleOrdNode *node;
    int i;
    struct BgWhaleWave *wave;
    float unused = -200.0f;  // dead literal needed for rodata pool layout

    body = whale->body;
    if (body != NULL && (body->flags & mask))
    {
        model = whale->model;
        body->flags &= ~0x10000;
        if (!(g_some_render_flag.flags & 0x84))
        {
            mtxa_from_mtx(g_ord_tbl_stuff->matrices[0]);
            mtxa_translate(&body->pos);
            mtxa_rotate_z(body->rotZ);
            mtxa_rotate_y(body->rotY);
            mtxa_rotate_x(body->rotX);
            mtxa_scale(&body->scale);
            if (g_is_sphere_visible(&model->boundSphereCenter, model->boundSphereRadius * body->scale.x))
            {
                list = ord_tbl_get_entry_for_pos(&model->boundSphereCenter);
                node = ord_tbl_alloc_node(sizeof(*node));
                if (node != NULL)
                {
                    node->node.drawFunc = (void (*)(struct OrdTblNode *))fn_1_95FFC;
                    node->whale = whale;
                    mtxa_to_mtx(node->mtx);
                    fn_1_CD418(list, &node->node);
                }
            }
        }
    }

    wave = whale->waves;
    for (i = whale->waveCount; i > 0; i--, wave++)
    {
        body = wave->model;
        body->flags &= ~0x10000;
    }
}

void fn_1_95FFC(struct BgWhaleOrdNode *node)
{
    struct BgWhaleBody *whale = node->whale;
    struct GMAModel *model = whale->model;
    void *prev;

    fn_1_96CA4(whale);
    GXSetZCompLoc_cached(1);
    mtxa_from_mtx(node->mtx);
    if (whale->texReady)
    {
        load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
        fn_1_96D1C(whale);
        prev = fn_1_10A69C(fn_1_97A80);
        avdisp_draw_model_unculled_sort_never(model);
        fn_1_10A69C(prev);
    }
    else
    {
        load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
        avdisp_draw_model_unculled_sort_never(model);
    }
    GXSetZCompLoc_cached(0);
}

void g_something_with_stage_heap_and_whale_theme2(struct BgWhaleBody *whale)
{
    struct GMAModel *model;
    int i;
    struct BgWhaleBone *bone;
    s16 *parents;
    struct BgWhaleBone *parent;
    Mtx *mtx;
    Mtx **mtxPtrs;
    int count;
    float radius;
    float zNorm;
    float phaseScale;
    Vec d;

    model = whale->model;
    radius = model->boundSphereRadius;
    count = model->mtxCount;
    whale->boneCount = count;
    if (count == 0)
        return;

    mtxPtrs = g_some_buffer_ptr;
    // the original reuses the bone pointer to hold the matrix base (register allocation depends on it)
    bone = (struct BgWhaleBone *)(mtxPtrs + count);
    {
        Mtx **ptr = mtxPtrs;

        mtx = (Mtx *)bone;
        for (i = count; i > 0; i--, ptr++, mtx++)
            *ptr = mtx;
    }
    draw_poly(model, mtxPtrs);

    whale->bones = alloc_from_heap_or_panic(stage_heap, whale->boneCount * sizeof(struct BgWhaleBone), "bg_whale.c", 1054);
    memset(whale->bones, 0, whale->boneCount * sizeof(struct BgWhaleBone));
    phaseScale = 32768.0f * (1.0f + 2.0f * RAND_FLOAT());

    mtx = (Mtx *)bone;
    bone = whale->bones;
    for (i = whale->boneCount; i > 0; i--, bone++, mtx++)
    {
        bone->flags |= 1;
        mtxa_from_mtx(*mtx);
        mathutil_mtxA_get_translate(&bone->basePos);
        mathutil_mtxA_set_translate_xyz(0.0f, 0.0f, 0.0f);
        mtxa_to_mtx(bone->base);
        bone->child = NULL;
        bone->pos = bone->basePos;
        bone->vel.x = 0.0f;
        bone->vel.y = 0.0f;
        bone->vel.z = 0.0f;
        bone->dir.x = 0.0f;
        bone->dir.y = 1.0f;
        bone->dir.z = 0.0f;
        bone->dirVel.x = 0.0f;
        bone->dirVel.y = 0.0f;
        bone->dirVel.z = 0.0f;
        bone->rotX = 0;
        zNorm = bone->basePos.z / radius;
        bone->amp.x = 20.0f * (1.0f + RAND_FLOAT());
        bone->amp.y = 20.0f * (1.0f + RAND_FLOAT());
        bone->amp.z = 20.0f * (1.0f + RAND_FLOAT());
        bone->phase[0] = (s16)(rand() & 0x7FFF);
        bone->phase[1] = (s16)(zNorm * phaseScale);
        bone->phase[2] = (s16)(rand() & 0x7FFF);
        bone->speed[0] = (s16)(218.45334f * (0.8f + 0.2f * RAND_FLOAT()));
        bone->speed[1] = (s16)(218.45334f * (0.8f + 0.2f * RAND_FLOAT()));
        bone->speed[2] = (s16)(218.45334f * (0.8f + 0.2f * RAND_FLOAT()));
        bone->rotAmp = 4096.0f * (RAND_FLOAT() - 0.5f);
        bone->rotPhase = (s16)(rand() & 0x7FFF);
        bone->rotSpeed = (s16)(218.45334f * (0.8f + 0.2f * RAND_FLOAT()));
        bone->scaleAmp = 0.4f * (RAND_FLOAT() - 0.5f);
        bone->scalePhase = (s16)(rand() & 0x7FFF);
        bone->scaleSpeed = (s16)(218.45334f * (0.8f + 0.2f * RAND_FLOAT()));
    }

    parents = fn_80069964(model);
    bone = whale->bones;
    for (i = whale->boneCount; i > 0; i--, bone++, parents++)
    {
        if (*parents >= 0)
        {
            parent = &whale->bones[*parents];
            parent->child = bone;
            d.x = bone->pos.x - parent->pos.x;
            d.y = bone->pos.y - parent->pos.y;
            d.z = bone->pos.z - parent->pos.z;
            parent->len = mathutil_vec_len(&d);
            if (parent->len > 0.0f)
            {
                vec_normalize_len(&d);
                parent->childDir = d;
                parent->flags &= ~1;
            }
        }
    }
}

void fn_1_966EC(struct BgWhaleBody *whale)
{
    if (whale->bones != NULL)
    {
        OSFreeToHeap(stage_heap, whale->bones);
        whale->bones = NULL;
    }
}

void fn_1_9673C(struct BgWhaleBody *whale, int mask)
{
    int i;
    struct BgWhaleBone *bone;
    struct BgWhaleBone *child;
    struct BgWhaleWave *wave;
    int j;
    float len;
    float dot;
    Vec d;
    Vec up;
    Vec vecB;
    Quat q1;
    Quat q2;
    Quat q3;
    Vec target;
    Vec tmp2;
    Vec tmp;

    up.x = 0.0f;
    up.y = 1.0f;
    up.z = 0.0f;

    bone = whale->bones;
    for (i = whale->boneCount; i > 0; i--, bone++)
    {
        if (bone->flags & 1)
            continue;
        bone->phase[0] += bone->speed[0];
        bone->phase[1] += bone->speed[1];
        bone->phase[2] += bone->speed[2];
        bone->rotPhase += bone->rotSpeed;
        bone->scalePhase += bone->scaleSpeed;
        target.x = bone->basePos.x + bone->amp.x * math_sin(bone->phase[0]);
        target.y = bone->basePos.y + bone->amp.y * math_sin(bone->phase[1]);
        target.z = bone->basePos.z + bone->amp.z * math_sin(bone->phase[2]);

        vecB = up;
        wave = whale->waves;
        for (j = whale->waveCount; j > 0; j--, wave++)
        {
            if (!(wave->model->flags & mask))
                continue;
            if (mathutil_vec_distance(&target, &wave->pos) > wave->radius)
                continue;
            mtxa_from_mtx(wave->invMtx);
            mtxa_tf_point(&target, &tmp);
            if (vec_normalize_len(&tmp) < 1.0f)
            {
                tmp2 = tmp;
                mtxa_from_mtx(wave->mtx);
                mtxa_tf_point(&tmp, &target);
                mtxa_tf_vec(&tmp2, &vecB);
                vec_normalize_len(&vecB);
            }
        }

        bone->vel.x += 0.02f * (0.18f * (target.x - bone->pos.x) - bone->vel.x);
        bone->vel.y += 0.02f * (0.1f * (target.y - bone->pos.y) - bone->vel.y);
        bone->vel.z += 0.02f * (0.18f * (target.z - bone->pos.z) - bone->vel.z);
        bone->pos.x += bone->vel.x;
        bone->pos.y += bone->vel.y;
        bone->pos.z += bone->vel.z;
        bone->scale = 1.0f + bone->scaleAmp * math_sin(bone->scalePhase);
        bone->rotX = (s16)(bone->rotAmp * math_sin(bone->rotPhase));
        mtxa_from_rotate_x(bone->rotX);
        mtxa_tf_vec(&vecB, &target);
        bone->dirVel.y += 0.05f * (0.05f * (target.y - bone->dir.y) - bone->dirVel.y);
        bone->dirVel.z += 0.05f * (0.05f * (target.z - bone->dir.z) - bone->dirVel.z);
        bone->dir.y += bone->dirVel.y;
        bone->dir.z += bone->dirVel.z;
        vec_normalize_len(&bone->dir);
    }

    bone = whale->bones;
    for (i = whale->boneCount; i > 0; i--, bone++)
    {
        child = bone->child;
        if (bone->flags & 1)
        {
            mtxa_from_identity();
            mtxa_mult_right(bone->base);
            mathutil_mtxA_set_translate(&bone->pos);
        }
        else
        {
            d.x = child->pos.x - bone->pos.x;
            d.y = child->pos.y - bone->pos.y;
            d.z = child->pos.z - bone->pos.z;
            len = vec_normalize_len(&d);
            quat_from_dirs(&q1, &bone->childDir, &d);
            vecB = bone->dir;
            dot = -mathutil_vec_dot_prod(&d, &vecB);
            vecB.x += dot * d.x;
            vecB.y += dot * d.y;
            vecB.z += dot * d.z;
            quat_from_dirs(&q2, &up, &vecB);
            quat_mult(&q3, &q2, &q1);
            mtxa_from_quat(&q3);
            mtxa_mult_right(bone->base);
            mtxa_scale_xyz(len / bone->len, 1.0f, 1.0f);
            mtxa_scale_s(bone->scale);
            mathutil_mtxA_set_translate(&bone->pos);
        }
        mtxa_to_mtx(bone->mtx);
    }
}

void fn_1_96CA4(struct BgWhaleBody *whale)
{
    int i;
    Mtx *buf;
    Mtx *mtxs;
    struct BgWhaleBone *bone;
    struct GMAModel *model;

    model = whale->model;
    bone = whale->bones;
    buf = g_some_buffer_ptr;
    mtxs = buf;
    for (i = whale->boneCount; i > 0; i--, mtxs++, bone++)
        mtx_copy(bone->mtx, *mtxs);
    pointer_range_advance(model, buf);
}

void fn_1_96D1C(struct BgWhaleBody *whale)
{
    int stage;
    int coord;
    int coord2;
    int indStage;
    Mtx save;
    Mtx view;
    float proj[7];
    float indMtx[2][3];

    GXGetProjectionv(proj);
    mtxa_to_mtx(save);
    mtxa_push();
    mtxa_from_identity();
    mtxa->mtxA[0][0] = -0.5f * proj[1];
    mtxa->mtxA[0][2] = 0.5f + 0.5f * proj[2];
    mtxa->mtxA[1][1] = 0.5f * proj[3];
    mtxa->mtxA[1][2] = 0.5f + 0.5f * proj[4];
    mtxa_mult_right(save);
    GXLoadTexMtxImm(mtxa->mtxA, 0x1E, 0);
    mtxa_from_identity();
    mtxa->mtxA[0][0] = 0.5f * proj[1];
    mtxa->mtxA[0][2] = 0.5f + 0.5f * proj[2];
    mtxa->mtxA[1][1] = 0.5f * proj[3];
    mtxa->mtxA[1][2] = 0.5f + 0.5f * proj[4];
    mtxa_mult_right(save);
    GXLoadTexMtxImm(mtxa->mtxA, 0x21, 0);
    mtxa_pop();

    mtxa_push();
    mtxa_to_mtx(view);
    mtxa_from_mtx(g_ord_tbl_stuff->matrices[0]);
    mtxa_invert();
    mtxa_mult_right(view);
    mathutil_mtxA_set_translate_xyz(0.0f, 0.0f, 0.0f);
    mtxa_to_mtx(view);
    mtxa_from_identity();
    mtxa->mtxA[0][0] = 8.0f;
    mtxa->mtxA[0][3] = 0.5f;
    mtxa->mtxA[1][1] = 0.0f;
    mtxa->mtxA[1][2] = 2.0f;
    mtxa->mtxA[1][3] = 0.5f + whale->unk90;
    mtxa->mtxA[2][2] = 0.0f;
    mtxa->mtxA[2][3] = 1.0f;
    mtxa_mult_right(view);
    GXLoadTexMtxImm(mtxa->mtxA, 0x40, 0);
    mtxa_pop();

    indMtx[0][0] = 0.0f;
    indMtx[0][1] = -0.6f;
    indMtx[0][2] = 0.0f;
    indMtx[1][0] = -0.2f;
    indMtx[1][1] = 0.0f;
    indMtx[1][2] = 0.0f;
    g_GXSetIndTexMtx_wrapper(1, indMtx, 0);
    indMtx[0][0] = 0.0f;
    indMtx[0][1] = 0.8f;
    indMtx[0][2] = 0.0f;
    indMtx[1][0] = -0.2f;
    indMtx[1][1] = 0.0f;
    indMtx[1][2] = 0.0f;
    g_GXSetIndTexMtx_wrapper(2, indMtx, 2);

    GXLoadTexObj_cached(&whale->reflTexObj, 0);
    GXLoadTexObj_cached(&whale->refrTexObj, 1);
    GXLoadTexObj_cached(whale->waveSpecATexObjs, 2);
    GXLoadTexObj_cached(&whale->reflTexObj, 3);

    stage = 0;
    coord = 0;
    indStage = 0;

    GXSetTevKAlphaSel_cached(stage, 4);
    GXSetTexCoordGen2_cached(coord, 0, 0, 0x21, 0, 0x7D);
    g_GXSetTevOrder_wrapper(stage, coord, 1, 0xFF);
    GXSetTevSwapMode_cached(stage, 0, 1);
    GXSetTevColorIn_cached(stage, 0xF, 0xF, 0xF, 8);
    GXSetTevColorOp_cached(stage, 0, 0, 0, 1, 0);
    GXSetTevAlphaIn_cached(stage, 7, 4, 6, 6);
    GXSetTevAlphaOp_cached(stage, 0, 0, 0, 1, 0);
    coord2 = coord + 1;
    GXSetTexCoordGen2_cached(coord2, 0, 0, 0x1E, 0, 0x7D);
    GXSetIndTexOrder_cached(indStage, coord2, 0);
    GXSetTevIndirect_cached(stage, indStage, 0, 7, 1, 0, 0, 0, 0, 0);
    stage++;
    coord += 2;
    indStage++;

    g_GXSetTevOrder_wrapper(stage, coord2, 3, 0xFF);
    GXSetTevColorIn_cached(stage, 0xF, 8, 0, 0);
    GXSetTevColorOp_cached(stage, 0, 0, 0, 1, 0);
    GXSetTevAlphaIn_cached(stage, 4, 7, 7, 0);
    GXSetTevAlphaOp_cached(stage, 0, 0, 0, 1, 0);
    g_GXSetTevIndirect_zero_if_different(stage);
    stage++;

    GXSetTevKColorSel_cached(stage, 4);
    GXSetTevKAlphaSel_cached(stage, 4);
    GXSetTexCoordGen2_cached(coord, 0, 1, 0x3C, 1, 0x40);
    g_GXSetTevOrder_wrapper(stage, coord, 2, 0xFF);
    GXSetTevColorIn_cached(stage, 0xF, 8, 0xE, 0);
    GXSetTevColorOp_cached(stage, 0, 0, 0, 1, 0);
    GXSetTevAlphaIn_cached(stage, 7, 6, 4, 0);
    GXSetTevAlphaOp_cached(stage, 0, 0, 0, 1, 0);
    GXSetIndTexOrder_cached(indStage, coord2, 0);
    GXSetTevIndirect_cached(stage, indStage, 0, 7, 2, 0, 0, 0, 0, 0);
    stage++;
    coord++;
    indStage++;

    GXSetNumTevStages_cached(stage);
    GXSetNumTexGens_cached(coord);
    GXSetNumIndStages_cached(indStage);
    GXSetBlendMode_cached(1, 4, 1, 0);
    GXSetZMode_cached(1, 3, 1);
    g_something_with_raster_state(0);
    GXSetCullMode_cached(2);
}

void fn_1_97380(struct RenderEffect *rendEfc)
{
    rendEfc->enableFlags = 4;
    rendEfc->work = NULL;
}

void fn_1_97394(struct RenderEffect *rendEfc) {}

void fn_1_97398(struct RenderEffect *rendEfc) {}

void fn_1_9739C(int mask, struct RenderEffect *rendEfc)
{
    struct StageBgModel *body;
    struct BgWhaleWork *work;

    g_smth_with_challenge_mode_var_and_3p();
    work = world_theme.work;
    body = work->whale.body;
    if (body == NULL || !(body->flags & mask))
        work->whale.texReady = 0;
    else
    {
        fn_1_974DC(rendEfc);
        fn_1_97A88(rendEfc);
    }
}

void fn_1_97418(int mask, struct RenderEffect *rendEfc)
{
    struct BgWhaleWork *work;
    struct StageBgModel *body;

    if ((dip_switches & 1) && get_max_gx_fifo_use_size() > 0xD9999)
    {
        struct BgWhaleBody *whale = &((struct BgWhaleWork *)world_theme.work)->whale;

        whale->texReady = 0;
    }
    else
    {
        g_smth_with_challenge_mode_var_and_3p();
        work = world_theme.work;
        body = work->whale.body;
        if (body == NULL || !(body->flags & mask))
            work->whale.texReady = 0;
        else
        {
            fn_1_974DC(rendEfc);
            fn_1_97A88(rendEfc);
        }
    }
}

void fn_1_974DC(struct RenderEffect *rendEfc)
{
    struct BgWhaleWork *work = world_theme.work;
    void *prev;
    int height;
    struct BgWhaleBody *whale = &work->whale;
    int width;
    struct GMAModel *model;
    struct GMAModel *gma;
    struct BgWhaleBone *bone;
    Mtx *mtxs;
    Mtx *buf;
    int i;
    struct StageBgModel *body;

    g_set_something3(0);
    width = 256;
    height = 256;
    GXSetViewport(0.0f, 0.0f, (float)width, (float)height, 0.0f, 1.0f);
    GXSetScissor_cached(0, 0, width, height);
    GXLoadTexObj_cached(init_common_gma->modelEntries[97].model->texObjs, 0);
    GXLoadTexObj_cached(whale->wavePatCTexObjs, 1);

    {
    int stage;
    int coord;
    Mtx m;

    stage = 0;
    coord = 0;
    GXSetTevKAlphaSel_cached(stage, 0);
    GXSetTexCoordGen2_cached(coord, 0, 1, 0x3C, 1, 0x40);
    mtxa_from_translate_xyz(-0.25f, 0.0f, 1.0f);
    mtxa_scale_xyz(0.75f, 0.75f, 1.0f);
    mtxa_to_mtx(m);
    mtxa_from_mtxb();
    mathutil_mtxA_set_translate_xyz(0.0f, 0.0f, 0.0f);
    mtxa_mult_left(m);
    mtxa_scale_xyz(1.0f, 0.0f, 1.0f);
    GXLoadTexMtxImm(mtxa->mtxA, 0x40, 0);
    g_GXSetTevOrder_wrapper(stage, coord, 0, 0xFF);
    GXSetTevSwapMode_cached(stage, 0, 0);
    g_GXSetTevIndirect_zero_if_different(stage);
    GXSetTevColorIn_cached(stage, 0xF, 0xF, 0xF, 8);
    GXSetTevColorOp_cached(stage, 0, 0, 0, 1, 0);
    GXSetTevAlphaIn_cached(stage, 7, 7, 7, 6);
    GXSetTevAlphaOp_cached(stage, 0, 0, 0, 1, 0);
    stage++;
    coord++;

    GXSetTexCoordGen2_cached(coord, 1, 4, 0x1E, 0, 0x7D);
    mtxa_from_identity();
    mtxa_scale_s(whale->scrollA.scale);
    mtxa_translate(&whale->scrollA.pos);
    GXLoadTexMtxImm(mtxa->mtxA, 0x1E, 1);
    g_GXSetTevOrder_wrapper(stage, coord, 1, 0xFF);
    GXSetTevSwapMode_cached(stage, 0, 0);
    g_GXSetTevIndirect_zero_if_different(stage);
    GXSetTevColorIn_cached(stage, 0xF, 8, 0, 0);
    GXSetTevColorOp_cached(stage, 0, 0, 3, 1, 0);
    GXSetTevAlphaIn_cached(stage, 7, 7, 7, 0);
    GXSetTevAlphaOp_cached(stage, 0, 0, 0, 1, 0);
    stage++;
    coord++;

    GXSetTexCoordGen2_cached(coord, 1, 4, 0x21, 0, 0x7D);
    mtxa_from_identity();
    mtxa_scale_s(whale->scrollB.scale);
    mtxa_translate(&whale->scrollB.pos);
    GXLoadTexMtxImm(mtxa->mtxA, 0x21, 1);
    g_GXSetTevOrder_wrapper(stage, coord, 1, 0xFF);
    GXSetTevSwapMode_cached(stage, 0, 0);
    g_GXSetTevIndirect_zero_if_different(stage);
    GXSetTevColorIn_cached(stage, 0xF, 8, 0, 0);
    GXSetTevColorOp_cached(stage, 0, 0, 3, 1, 0);
    GXSetTevAlphaIn_cached(stage, 7, 7, 7, 0);
    GXSetTevAlphaOp_cached(stage, 0, 0, 0, 1, 0);
    stage++;
    coord++;

    GXSetNumTevStages_cached(stage);
    GXSetNumTexGens_cached(coord);
    }
    GXSetNumIndStages_cached(0);
    GXSetBlendMode_cached(0, 1, 0, 0);
    GXSetZMode_cached(1, 3, 1);
    GXSetCullMode_cached(2);

    prev = fn_1_10A69C(fn_1_97A80);
    model = whale->model;
    body = whale->body;
    if (body != NULL)
    {
        mtxa_from_mtx(g_ord_tbl_stuff->matrices[0]);
        mtxa_translate(&body->pos);
        mtxa_rotate_z(body->rotZ);
        mtxa_rotate_y(body->rotY);
        mtxa_rotate_x(body->rotX);
        mtxa_scale(&body->scale);
        if (g_is_sphere_visible(&model->boundSphereCenter, model->boundSphereRadius * body->scale.x))
        {
            mtxa_push();
            gma = whale->model;
            bone = whale->bones;
            buf = g_some_buffer_ptr;
            mtxs = buf;
            for (i = whale->boneCount; i > 0; i--, mtxs++, bone++)
                mtx_copy(bone->mtx, *mtxs);
            pointer_range_advance(gma, buf);
            mtxa_pop();
            load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
            avdisp_draw_model_unculled_sort_never(model);
        }
    }
    fn_1_10A69C(prev);

    GXSetZMode_cached(1, 3, 1);
    GXSetTexCopySrc_cached(0, 0, width, height);
    GXSetTexCopyDst_cached(width, height, 0x2C, 0);
    GXCopyTex(whale->reflTexData, 1);
    GXInitTexObj(&whale->reflTexObj, whale->reflTexData, width, height, 3, 0, 0, 0);
    GXInvalidateTexAll();
    whale->texReady = 1;
    fn_1_E2D20();
    camera_apply_viewport(modeCtrl.currPlayer);
}

int fn_1_97A80(void)
{
    return 1;
}

void fn_1_97A88(struct RenderEffect *rendEfc)
{
    struct Camera *cam;
    struct BgWhaleBody *whale = &((struct BgWhaleWork *)world_theme.work)->whale;
    struct StageBgModel *body;
    struct StageBgModel **extra;
    int mask;
    int i;
    struct GMAModel *gma;
    struct StageBgModel *model;
    int width;
    int height;
    int visMask;
    struct Camera saved;
    Mtx mirror;
    Mtx reflMtx;
    Mtx viewMtx;
    Mtx44 proj;
    Vec tl;

    cam = g_current_camera;
    saved = *cam;
    width = 512;
    height = 256;
    push_light_group();
    visMask = g_smth_with_challenge_mode_var_and_3p();
    body = whale->body;
    if (body == NULL || !(body->flags & visMask))
        return;

    gma = whale->model;
    mtxa_from_translate(&body->pos);
    mtxa_rotate_z(body->rotZ);
    mtxa_rotate_y(body->rotY);
    mtxa_rotate_x(body->rotX);
    mtxa_scale(&body->scale);
    mtxa_translate(&gma->boundSphereCenter);
    mtxa_to_mtx(whale->mtx);
    mtxa_mult_left(mtxa->mtxB);
    mtxa_rigid_inv_tf_tl(&tl);
    if (tl.y < 0.0f)
    {
        mtxa_from_mtx(whale->mtx);
        mtxa_rotate_z(-0x8000);
        mtxa_to_mtx(whale->mtx);
    }

    {
    Vec right = {1.0f, 0.0f, 0.0f};
    Vec upv = {0.0f, 1.0f, 0.0f};
    Vec fwd = {0.0f, 0.0f, 1.0f};
    Vec pos = {0.0f, 0.0f, 0.0f};

    mtxa_from_translate(&cam->eye);
    mtxa_rotate_y(cam->rotY);
    mtxa_rotate_x(cam->rotX);
    mtxa_rotate_z(cam->rotZ);
    mtxa_mult_right(g_ord_tbl_stuff->matrices[0]);
    mtxa_to_mtx(viewMtx);
    mtxa_mult_right(whale->mtx);
    mtxa_rigid_inv_tf_vec(&right, &right);
    mtxa_rigid_inv_tf_vec(&upv, &upv);
    mtxa_rigid_inv_tf_vec(&fwd, &fwd);
    mtxa_rigid_inv_tf_point(&pos, &pos);
    right.y = -right.y;
    upv.y = -upv.y;
    fwd.y = -fwd.y;
    pos.y = -pos.y;
    mtxa_tf_vec(&right, &right);
    mtxa_tf_vec(&upv, &upv);
    mtxa_tf_vec(&fwd, &fwd);
    mtxa_tf_point(&pos, &pos);
    mtxa->mtxA[0][0] = right.x;
    mtxa->mtxA[1][0] = right.y;
    mtxa->mtxA[2][0] = right.z;
    mtxa->mtxA[0][1] = upv.x;
    mtxa->mtxA[1][1] = upv.y;
    mtxa->mtxA[2][1] = upv.z;
    mtxa->mtxA[0][2] = fwd.x;
    mtxa->mtxA[1][2] = fwd.y;
    mtxa->mtxA[2][2] = fwd.z;
    mtxa->mtxA[0][3] = pos.x;
    mtxa->mtxA[1][3] = pos.y;
    mtxa->mtxA[2][3] = pos.z;
    }
    mtxa_invert();
    mtxa_to_mtx(reflMtx);

    mtxa_from_identity();
    mtxa_scale_xyz(-1.0f, 1.0f, 1.0f);
    mtxa_to_mtx(mirror);

    mtxa_from_identity();
    mtxa_rotate_z(-cam->rotZ);
    mtxa_rotate_x(-cam->rotX);
    mtxa_rotate_y(-cam->rotY);
    mtxa_translate_neg(&cam->eye);
    mtxa_mult_right(reflMtx);
    mtxa_mult_right(viewMtx);
    mtxa_mult_left(mirror);
    mtxa_to_mtx(cam->unk14C);
    mtxa_to_mtx(cam->unk17C);

    mtxa_from_identity();
    mtxa_rotate_z(-cam->rotZ);
    mtxa_rotate_x(-cam->rotX);
    mtxa_rotate_y(-cam->rotY);
    mtxa_translate_neg(&cam->eye);
    mtxa_mult_right(reflMtx);
    mtxa_translate(&cam->eye);
    mtxa_rotate_y(cam->rotY);
    mtxa_rotate_x(cam->rotX);
    mtxa_rotate_z(cam->rotZ);
    mtxa_mult_right(g_ord_tbl_stuff->matrices[0]);
    mtxa_mult_left(mirror);
    mtxa_to_mtx(cam->unk1AC);
    mtxa_to_mtx(cam->unk1DC);

    camera_apply_viewport(g_something_with_camera3);
    C_MTXPerspective(proj, 0.005493164f * cam->sub28.view.fov, cam->sub28.view.aspect, 0.1f, 20000.0f);
    proj[0][2] -= cam->sub28.view.tanHalfFov * (cam->sub28.view.aspect * (proj[0][0] * cam->sub28.view.unk28));
    proj[1][2] -= cam->sub28.view.tanHalfFov * (proj[1][1] * cam->sub28.view.unk2C);
    g_something_with_GXSetProjectionv(proj, 0);
    GXSetViewport(0.0f, 0.0f, (float)width, (float)height, 0.0f, 1.0f);
    GXSetScissor_cached(0, 0, width, height);
    mtxa_from_mtxb();
    g_set_active_light_group(5);
    g_some_render_flag.flags |= 4;
    mtxa_from_mtx(whale->mtx);
    mathutil_mtxA_get_translate(&g_some_render_flag.unk4);
    mtxa_tf_vec_xyz(0.0f, 1.0f, 0.0f, &g_some_render_flag.unk10);
    mtxa_from_mtxb();
    mtxa_tf_point(&g_some_render_flag.unk4, &g_some_render_flag.unk20);
    mtxa_tf_vec(&g_some_render_flag.unk10, &g_some_render_flag.unk2C);
    mtxa_from_mtx(g_ord_tbl_stuff->matrices[0]);
    mtxa_rigid_invert();
    mtxa_mult_right(mtxa->mtxB);
    mtxa_tf_point(&g_some_render_flag.unk4, &g_some_render_flag.unk3C);
    mtxa_tf_vec(&g_some_render_flag.unk10, &g_some_render_flag.unk48);

    mask = g_smth_with_challenge_mode_var_and_3p();
    fn_1_E2D64(lbl_1_bss_CF22C.type, lbl_1_bss_CF22C.unk4, lbl_1_bss_CF22C.unk8, 0, 0, 0);
    extra = whale->extra;
    for (i = whale->extraCount; i > 0; i--, extra++)
    {
        model = *extra;
        if (model->flags & mask)
        {
            mtxa_from_mtx(g_ord_tbl_stuff->matrices[0]);
            mtxa_translate(&model->pos);
            mtxa_rotate_z(model->rotZ);
            mtxa_rotate_y(model->rotY);
            mtxa_rotate_x(model->rotX);
            mtxa_scale(&model->scale);
            load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
            avdisp_draw_model_unculled_sort_auto(model->model);
        }
    }
    fn_1_E2E18();
    ord_tbl_draw_nodes();
    g_some_render_flag.flags &= ~4;

    GXSetZMode_cached(1, 3, 1);
    GXSetTexCopySrc_cached(0, 0, width, height);
    GXSetTexCopyDst_cached(width, height, 4, 0);
    GXCopyTex(whale->refrTexData, 1);
    GXInitTexObj(&whale->refrTexObj, whale->refrTexData, width, height, 4, 2, 2, 0);

    *cam = saved;
    camera_apply_viewport(modeCtrl.currPlayer);
    pop_light_group();
}

void fn_1_98290(struct BgWhaleWork *work)
{
    struct BgWhaleEnv *env = &work->env;

    env->pos.x = 0.0f;
    env->pos.y = 0.0f;
    env->pos.z = 0.0f;
    env->velTarget.x = 0.33333334f * (1.0f + RAND_FLOAT());
    env->velTarget.z = 0.33333334f * (1.0f + RAND_FLOAT());
    env->vel.x = env->velTarget.x;
    env->vel.z = env->velTarget.z;
    env->accel.x = 0.008333334f * (1.0f + RAND_FLOAT());
    env->accel.z = 0.008333334f * (1.0f + RAND_FLOAT());
    if (rand() & 1)
        env->accel.x = -env->accel.x;
    if (rand() & 1)
        env->accel.y = -env->accel.y;
    env->scale.x = 0.00390625f * (0.6f + 0.4f * RAND_FLOAT());
    env->scale.y = 0.00390625f * (0.6f + 0.4f * RAND_FLOAT());
    env->scale.z = 1.0f;

    env->pos2.x = 0.0f;
    env->pos2.y = 0.0f;
    env->pos2.z = 0.0f;
    env->velTarget2.x = env->velTarget.x * (-2.0f * (0.5f + RAND_FLOAT()));
    env->velTarget2.z = env->velTarget.z * (-2.0f * (0.5f + RAND_FLOAT()));
    env->vel2.x = env->velTarget.x;
    env->vel2.z = env->velTarget.z;
    env->accel2.x = env->accel.x * (-2.0f * (0.5f + RAND_FLOAT()));
    env->accel2.z = env->accel.z * (-2.0f * (0.5f + RAND_FLOAT()));
    env->scale2.x = 0.0026041667f * (0.6f + 0.4f * RAND_FLOAT());
    env->scale2.y = 0.0026041667f * (0.6f + 0.4f * RAND_FLOAT());
    env->scale2.z = 1.0f;

    world_theme.unk90 = fn_1_9884C;
    world_theme.unk98 = fn_1_98CD4;
}

void fn_1_98698(struct BgWhaleWork *work)
{
    struct BgWhaleEnv *env = &work->env;
    float t;

    t = work->light.t;
    env->vel.x += env->accel.x * t;
    env->vel.z += env->accel.z * t;
    env->vel.x += 0.006f * (env->velTarget.x - env->vel.x);
    env->vel.z += 0.006f * (env->velTarget.z - env->vel.z);
    env->pos.x += env->vel.x;
    env->pos.z += env->vel.z;

    env->vel2.x += env->accel2.x * t;
    env->vel2.z += env->accel2.z * t;
    env->vel2.x += 0.005f * (env->velTarget2.x - env->vel2.x);
    env->vel2.z += 0.005f * (env->velTarget2.z - env->vel2.z);
    env->pos2.x += env->vel2.x;
    env->pos2.z += env->vel2.z;

    mtxa_from_identity();
    mtxa_scale(&env->scale);
    mtxa_rotate_y(0);
    mtxa_rotate_x(0x4100);
    mtxa_translate(&env->pos);
    mtxa_to_mtx(env->mtx);

    mtxa_from_identity();
    mtxa_scale(&env->scale2);
    mtxa_rotate_y(0);
    mtxa_rotate_x(0x3F00);
    mtxa_translate(&env->pos2);
    mtxa_to_mtx(env->mtx2);
}

void fn_1_9884C(struct BallEnvArg *arg)
{
    struct BgWhaleWork *work = world_theme.work;
    struct BgWhaleEnv *env = &work->env;
    struct TevStageInfo tev = arg->tev;
    float indMtx[2][3];

    if (arg->unk0 == 0
     && tev.tevStage == env->tev.tevStage
     && tev.tevIndStage == env->tev.tevIndStage
     && tev.texMtxId == env->tev.texMtxId
     && tev.postTexMtxId == env->tev.postTexMtxId
     && tev.unk1C == env->tev.unk1C
     && tev.texMapId == env->tev.texMapId)
    {
        tev.tevStage += 1;
        tev.tevIndStage += 1;
        tev.texCoordId += 2;
        tev.texMtxId += 6;
        tev.unk1C += 1;
        tev.texMapId += 2;
        arg->tev = tev;
        return;
    }

    env->tev = tev;
    GXLoadTexObj_cached(env->lightmapATexObjs, tev.texMapId);
    GXLoadTexObj_cached(env->wavePatBTexObjs, tev.texMapId + 1);
    indMtx[0][0] = 0.4f;
    indMtx[0][1] = 0.0f;
    indMtx[0][2] = 0.0f;
    indMtx[1][0] = 0.0f;
    indMtx[1][1] = 0.4f;
    indMtx[1][2] = 0.0f;
    g_GXSetIndTexMtx_wrapper(arg->tev.unk1C, indMtx, 0);
    mtxa_push();
    mtxa_mult_left(env->texMtx);
    GXLoadTexMtxImm(mtxa->mtxA, tev.texMtxId, 1);
    mtxa_pop();
    mtxa_push();
    mtxa_mult_left(env->texMtx2);
    GXLoadTexMtxImm(mtxa->mtxA, tev.texMtxId + 3, 1);
    mtxa_pop();
    tex_coord_gen(tev.texCoordId, 1, 0, tev.texMtxId);
    tex_coord_gen(tev.texCoordId + 1, 1, 0, tev.texMtxId + 3);
    GXSetIndTexOrder_cached(tev.tevIndStage, tev.texCoordId + 1, tev.texMapId + 1);
    GXSetTevIndirect_cached(tev.tevStage, tev.tevIndStage, 0, 0, tev.unk1C, 0, 0, 0, 0, 0);
    g_GXSetTevOrder_wrapper(tev.tevStage, tev.texCoordId, tev.texMapId, 0xFF);
    GXSetTevColorIn_cached(tev.tevStage, 0xF, 0xF, 0xF, 8);
    GXSetTevColorOp_cached(tev.tevStage, 0, 0, 0, 1, 2);
    GXSetTevAlphaIn_cached(tev.tevStage, 7, 7, 7, 0);
    GXSetTevAlphaOp_cached(tev.tevStage, 0, 0, 0, 1, 0);
    tev.tevStage += 1;
    tev.tevIndStage += 1;
    tev.texCoordId += 2;
    tev.texMtxId += 6;
    tev.unk1C += 1;
    tev.texMapId += 2;
    arg->tev = tev;
}

void fn_1_98CD4(void)
{
    struct BgWhaleWork *work = world_theme.work;
    struct BgWhaleEnv *env = &work->env;
    Mtx viewInv;

    mtxa_from_mtx(g_ord_tbl_stuff->matrices[0]);
    mtxa_rigid_invert();
    mtxa_to_mtx(viewInv);
    mtxa_mult_left(env->mtx);
    mtxa_to_mtx(env->texMtx);
    mtxa_from_mtx(viewInv);
    mtxa_mult_left(env->mtx2);
    mtxa_to_mtx(env->texMtx2);
}

void fn_1_98D4C(struct BgWhaleWork *work) {}

void fn_1_98D50(struct BgWhaleWork *work, int mask) {}

void fn_1_98D54(struct BgWhaleWork *work, int mask) {}

int fn_1_98D58(int index, struct GMAModelEntry *entry)
{
    struct BgWhaleWork *work = world_theme.work;
    GXTexObj *tex = entry->model->texObjs;

    switch (index)
    {
    case 0:
        work->whale.model = entry->model;
        break;
    case 1:
        work->whale.wavePatCTexObjs = tex;
        GXInitTexObj(tex, GXGetTexObjData(tex), GXGetTexObjWidth(tex), GXGetTexObjHeight(tex),
            GXGetTexObjFmt(tex), GXGetTexObjWrapS(tex), GXGetTexObjWrapT(tex), 0);
        break;
    case 2:
        work->whale.waveSpecATexObjs = tex;
        break;
    case 3:
        work->inLightATexObjs = tex;
        break;
    case 4:
        work->env.lightmapATexObjs = tex;
        break;
    case 5:
        work->env.wavePatBTexObjs = tex;
        break;
    }
    return 1;
}

int fn_1_98E68(int index, struct StageBgModel *model)
{
    struct BgWhaleWork *work = world_theme.work;

    switch (index)
    {
    case 0:
        work->whale.body = model;
        break;
    case 1:
        if (work->whale.waveCount < 4)
        {
            work->whale.waves[work->whale.waveCount].model = model;
            work->whale.waveCount++;
        }
        break;
    case 2:
        work->light.model = model;
        break;
    case 3:
        model->flags |= (1 << 24);
        break;
    case 4:
    case 5:
    case 6:
    case 7:
    case 8:
    case 9:
    case 10:
        if (work->whale.extraCount < 12 && !(model->flags & (1 << 24)))
        {
            work->whale.extra[work->whale.extraCount] = model;
            work->whale.extraCount++;
        }
        break;
    }
    return 1;
}
