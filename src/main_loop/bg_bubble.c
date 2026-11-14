/* Bubble background: rising soap bubbles with refraction, wind-blown laundry cloth
 * simulation, ripples on the spa surface and a screen-space soap film overlay. */

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

struct BgBubbleBubble
{
    /*0x00*/ s32 life;
    /*0x04*/ Vec pos;
    /*0x10*/ Vec vel;
    /*0x1C*/ float scale;
    /*0x20*/ float radius;
    /*0x24*/ float radiusY;
    /*0x28*/ float radiusVel;
    /*0x2C*/ float radiusX;
    /*0x30*/ float rate;
    /*0x34*/ s16 ang;
    /*0x36*/ s16 angVel;
};  /* size = 0x38 */

struct BgBubbleClothBone
{
    /*0x00*/ Vec pos;
    /*0x0C*/ Vec vel;
    /*0x18*/ Vec dir;
    /*0x24*/ Vec normal;
    /*0x30*/ float rate;
    /*0x34*/ Mtx mtx;
    /*0x64*/ float len;
    /*0x68*/ struct BgBubbleClothBone *parent;
    /*0x6C*/ struct BgBubbleClothBone *child;
};  /* size = 0x70 */

struct BgBubbleCloth
{
    /*0x00*/ struct StageBgModel *model;
    /*0x04*/ s32 boneCount;
    /*0x08*/ struct BgBubbleClothBone *bones;
};  /* size = 0x0C */

struct BgBubbleRipple
{
    /*0x00*/ s16 life;
    /*0x02*/ s16 rotZ;
    /*0x04*/ Vec pos;
    /*0x10*/ Vec vel;
    /*0x1C*/ float radius;
    /*0x20*/ float radiusVel;
    /*0x24*/ float alpha;
    /*0x28*/ struct GMAModel *model;
};  /* size = 0x2C */

struct BgBubbleTracker
{
    /*0x00*/ s32 inWater;
    /*0x04*/ s32 counter;
    /*0x08*/ Vec pos;
};  /* size = 0x14 */

struct BgBubbleFilm
{
    /*0x00*/ float pos[3];
    /*0x0C*/ Vec vel;
    /*0x18*/ float sinA;
    /*0x1C*/ float sinB;
    /*0x20*/ float pad20;
    /*0x24*/ float ampA;
    /*0x28*/ float ampB;
    /*0x2C*/ float pad2C;
    /*0x30*/ float scaleA;
    /*0x34*/ float scaleB;
    /*0x38*/ float pad38;
    /*0x3C*/ float r;
    /*0x40*/ float g;
    /*0x44*/ float b;
    /*0x48*/ float a;
};  /* size = 0x4C */

struct BgBubbleWind
{
    /*0x00*/ S16Vec phase;
    /*0x06*/ S16Vec speed;
    /*0x0C*/ Vec amp;
    /*0x18*/ Mtx mtx;
    /*0x48*/ s16 rot;
    /*0x4A*/ s16 rotSpeed;
    /*0x4C*/ Vec dir;
};  /* size = 0x58 */

struct BgBubbleAwawa
{
    /*0x00*/ struct StageBgModel *model;
    u8 filler4[0x18];
    /*0x1C*/ GXTexObj *texObjs;
    /*0x20*/ GXTexObj *texObjs2;
};  /* size = 0x24 */

struct BgBubbleSpa
{
    /*0x000*/ struct StageBgModel *model;
    /*0x004*/ s32 texReady;
    /*0x008*/ GXTexObj rippleTexObj;
    /*0x028*/ void *rippleTexData;
    /*0x02C*/ GXTexObj spaTexObj;
    /*0x04C*/ Mtx spaTexMtx;
    /*0x07C*/ s32 ringIdx;
    /*0x080*/ struct BgBubbleRipple ripples[32];
    /*0x600*/ struct BgBubbleTracker trackers[16];
};  /* size = 0x740 */

struct BgBubbleWork
{
    /*0x0000*/ s32 initialized;
    u8 filler4[0x2C];
    /*0x0030*/ struct BgBubbleWind wind;
    /*0x0088*/ struct StageBgModel *wash;
    /*0x008C*/ struct StageBgModel *washTs;
    u8 filler90[0x30];
    /*0x00C0*/ s32 clothCount;
    /*0x00C4*/ struct BgBubbleCloth cloths[16];
    /*0x0184*/ GXTexObj texObj;
    /*0x01A4*/ Mtx texMtx;
    /*0x01D4*/ float indMtx[2][3];
    /*0x01EC*/ struct GMAModel *bubbleModel;
    /*0x01F0*/ struct BgBubbleBubble bubbles[64];
    /*0x0FF0*/ struct BgBubbleAwawa awawa;
    /*0x1014*/ struct BgBubbleSpa spa;
    /*0x1754*/ GXTexObj *filmTexObjs;
    /*0x1758*/ struct BgBubbleFilm film[4];
};  /* size = 0x1888 */

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

struct BgBubbleOrdNode
{
    /*0x00*/ struct OrdTblNode node;
    /*0x08*/ struct BgBubbleWork *work;
};  /* size = 0x0C */

struct BgBubbleClothOrdNode
{
    /*0x00*/ struct OrdTblNode node;
    /*0x08*/ struct BgBubbleCloth *cloth;
    /*0x0C*/ Mtx mtx;
    /*0x3C*/ s32 lightGroup;
};  /* size = 0x40 */

struct BgBubbleTevArg
{
    /* BallEnvArg (avdisp.h) layout: tev follows an int and two pointers */
    /*0x00*/ s32 unk0;
    /*0x04*/ void *shape;
    /*0x08*/ void *layers;
    /*0x0C*/ struct TevStageInfo tev;
};

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
extern void *lbl_801988E0;
extern u32 g_font_char_var_3;  /* global frame counter */
extern void *bg_heap;
extern struct GMA *efcmdlGma;

void *OSFreeToHeap(void *heap, void *ptr);
void *alloc_from_heap_or_panic(void *heap, u32 size, char *file, int line);
u32 GXGetTexBufferSize(u16 width, u16 height, u32 format, u8 mipmap, u8 maxLod);
void GXInitTexObj(GXTexObj *obj, void *image, u16 width, u16 height, int format, int wrapS, int wrapT, u8 mipmap);
void GXSetTexCopySrc_cached(u16 left, u16 top, u16 width, u16 height);
void GXSetTexCopyDst_cached(u16 width, u16 height, int format, u8 mipmap);
void GXCopyTex(void *dest, int clear);
void GXInvalidateTexAll(void);
void GXGetProjectionv(float *proj);
void GXGetViewportv(float *viewport);
struct OrdTblNode *ord_tbl_get_entry_for_pos(Vec *pos);
void *ord_tbl_alloc_node(u32 size);
void fn_1_CD418(struct OrdTblNode *list, struct OrdTblNode *node);
struct OrdTblNode *fn_1_CCD5C(int);
void draw_poly(struct GMAModel *model, Mtx **mtxs);
void pointer_range_advance(struct GMAModel *model, void *mtxs);
s16 *fn_80069964(struct GMAModel *model);
void fn_1_D8130(int, int, struct RenderEffect *);
void fn_1_D8264(int);
void g_smth_with_bg_model_names(struct BgModelSearch *search, int (*callback)(int, struct GMAModelEntry *));
void GXLoadTexObj_cached(GXTexObj *obj, int map);
void GXLoadTexMtxImm(Mtx m, int id, int type);
void GXLoadPosMtxImm(Mtx m, int id);
void GXSetNumTexGens_cached(u8 n);
void GXSetNumTevStages_cached(u8 n);
void GXSetNumIndStages_cached(u8 n);
void GXSetNumChans_cached(u8 n);
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
void g_something_with_GX_vtx_desc(u32 attrs);
void opti_GXSetChanCtrl(int chan, int enable, int ambSrc, int matSrc, int lightMask, int diffFn, int attnFn);
void g_smth_with_bg_color_drawing(GXColor *color);
void mtxa_from_translate_xyz(float, float, float);
void mtxa_rigid_inv_tf_vec_xyz(float x, float y, float z, Vec *dst);
void vec_to_euler_xy(Vec *v, s16 *x, s16 *y);
void *g_yet_another_unk_draw_func(void *func);
struct Ape *get_ape_by_idx(int idx);
void *g_get_ape_anim_smth(struct Ape *ape, int bone);
void fn_1_10A2C0(struct GMAModel *model);
void fn_1_10A300(struct GMAModel *model);
float fn_1_15F720(void);
int fn_1_15F7E4(void);
int fn_1_15F84C(void);

/* quantized store through the U8 graphics quantization register */
static inline void f32_to_u8(register float *in, register u8 *out)
{
#ifdef __MWERKS__
    asm
    {
        lfs f1, 0(in)
        psq_st f1, 0(out), 1, 2
    }
#else
    *out = pc_psq_st_u8(*in);
#endif
}

void fn_1_99198(struct BgBubbleWork *work);
void fn_1_995E0(struct BgBubbleWork *work);
void fn_1_99628(struct BgBubbleWork *work);
void fn_1_99654(struct BgBubbleWork *work, int mask);
void fn_1_99F2C(struct BgBubbleWork *work, int mask);
void fn_1_99FCC(struct BgBubbleOrdNode *node);
void fn_1_9A380(struct BgBubbleTevArg *arg);
void fn_1_9A600(struct BgBubbleWork *work);
void fn_1_9A650(struct BgBubbleWork *work);
void fn_1_9A6A0(struct BgBubbleWork *work, int mask);
void fn_1_9A6F0(struct BgBubbleWork *work, int mask);
void fn_1_9A854(struct BgBubbleClothOrdNode *node);
void fn_1_9A8CC(struct BgBubbleCloth *cloth);
void fn_1_9ABE8(struct BgBubbleCloth *cloth);
void fn_1_9AC40(struct BgBubbleCloth *cloth);
void fn_1_9B470(struct BgBubbleCloth *cloth);
void fn_1_9B564(struct BgBubbleWork *work);
void fn_1_9B5A0(struct BgBubbleWork *work, int mask);
void fn_1_9B5A4(struct BgBubbleWork *work, int mask);
void fn_1_9B5A8(struct BgBubbleWork *work, int mask);
void fn_1_9BC6C(struct BgBubbleWork *work, int mask);
void fn_1_9C03C(struct BgBubbleWork *work, int mask);
void fn_1_9C2CC(struct BgBubbleTevArg *arg);
void fn_1_9C658(struct BgBubbleTevArg *arg);
void fn_1_9C9B8(struct RenderEffect *);
void fn_1_9CA5C(struct RenderEffect *);
void fn_1_9CAB8(struct RenderEffect *);
void fn_1_9CB18(int, struct RenderEffect *);
void fn_1_9CBA4(struct BgBubbleWork *work);
void fn_1_9CE88(struct BgBubbleWork *work, int mask);
void fn_1_9D25C(struct BgBubbleWork *work, int mask);
void fn_1_9D2D4(struct BgBubbleWork *work, struct BgBubbleFilm *film);
int fn_1_9D854(int, struct GMAModelEntry *);
int fn_1_9D87C(int, struct StageBgModel *);

/* Unreferenced table (kept for data layout) */
static u32 lbl_1_data_32CA8[0x5C] =
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
    { BG_MDL_CMP_FULL, "BUB_BUBBLE" },
    { BG_MDL_CMP_END, NULL },
};

static struct BgModelSearch s_bgModelSearch[] =
{
    { BG_MDL_CMP_FULL, "BUB_WASH" },
    { BG_MDL_CMP_FULL, "BUB_WASH_TS" },
    { BG_MDL_CMP_FULL, "BUB_AWAWA" },
    { BG_MDL_CMP_PREFIX, "BUB_WASH_T_SHIRT" },
    { BG_MDL_CMP_PREFIX, "BUB_WASH_TOWEL" },
    { BG_MDL_CMP_PREFIX, "BUB_WASH_SKIRT" },
    { BG_MDL_CMP_PREFIX, "BUB_WASH_SHEET" },
    { BG_MDL_CMP_FULL, "BUB_SPA" },
    { BG_MDL_CMP_END, NULL },
};

static struct RenderEffectFuncs s_renderEffectFuncs =
{
    fn_1_9C9B8,
    fn_1_9CA5C,
    fn_1_9CAB8,
    fn_1_9CB18,
};

#define RAND15() (rand() & 0x7FFF)

void bg_bubble_init(void)
{
    struct BgBubbleWork *work = world_theme.work;
    struct RenderEffect rendEfc;

    bg_init_base();
    if (!work->initialized)
    {
        g_smth_with_bg_model_names(s_modelSearch, fn_1_9D854);
        work->initialized = TRUE;
    }
    work->washTs = NULL;
    work->awawa.model = NULL;
    work->clothCount = 0;
    g_init_stagedef_bg_fg_models(
        stagedef->bgModels,
        stagedef->bgModelCount,
        s_bgModelSearch,
        fn_1_9D87C);
    g_init_stagedef_bg_fg_models(
        stagedef->fgModels,
        stagedef->fgModelCount,
        s_bgModelSearch,
        fn_1_9D87C);
    if (modeCtrl.courseFlags & (1 << 26))
    {
        memset(&rendEfc, 0, sizeof(rendEfc));
        rendEfc.cameraMask = 0xFFFF;
        rendEfc.funcs = &s_renderEffectFuncs;
        fn_1_D8130(0, 0, &rendEfc);
    }
    fn_1_99198(work);
    fn_1_99628(work);
    fn_1_9A600(work);
    fn_1_9B564(work);
}

void bg_bubble_tick(void)
{
    struct BgBubbleWork *work = world_theme.work;
    int mask;

    mask = g_smth_with_challenge_mode_var_and_3p();
    bg_tick_base();
    if (!is_game_paused_and_in_view_stage())
    {
        fn_1_995E0(work);
        fn_1_99654(work, mask);
        fn_1_9A6A0(work, mask);
        fn_1_9B5A0(work, mask);
    }
}

void bg_bubble_dest(void)
{
    struct BgBubbleWork *work = world_theme.work;

    fn_1_9A650(work);
    fn_1_D8264(0);
}

void bg_bubble_disp(void)
{
    struct BgBubbleWork *work = world_theme.work;
    int mask;

    mask = g_smth_with_challenge_mode_var_and_3p();
    fn_1_99F2C(work, mask);
    fn_1_9A6F0(work, mask);
    fn_1_9B5A4(work, mask);
    bg_disp_base();
}

void bg_bubble_item_coin_coli(void) {}

/* Wind field setup: random phases, speeds, amplitudes and a random transform. */
void fn_1_99198(struct BgBubbleWork *work)
{
    struct BgBubbleWind *wind = &work->wind;
    float sz, sy, sx;

    wind->phase.x = rand() & 0x7FFF;
    wind->phase.y = rand() & 0x7FFF;
    wind->phase.z = rand() & 0x7FFF;
    wind->speed.x = 546.13336f * (0.25f + RAND_FLOAT());
    wind->speed.y = 546.13336f * (0.25f + RAND_FLOAT());
    wind->speed.z = 546.13336f * (0.25f + RAND_FLOAT());
    wind->amp.x = 2.0f * (RAND_FLOAT() - 0.5f);
    wind->amp.y = 0.5f * (RAND_FLOAT() - 0.5f);
    wind->amp.z = 2.0f * (RAND_FLOAT() - 0.5f);

    mtxa_from_identity();
    sz = 3276.8f * (0.1f + RAND_FLOAT());
    sy = 3276.8f * (0.1f + RAND_FLOAT());
    sx = 3276.8f * (0.1f + RAND_FLOAT());
    mtxa_scale_xyz(sx, sy, sz);
    mtxa_rotate_y(rand() & 0x7FFF);
    mtxa_rotate_x(rand() & 0x7FFF);
    mtxa_rotate_z(rand() & 0x7FFF);
    mtxa_to_mtx(wind->mtx);

    wind->rot = rand() & 0x7FFF;
    wind->rotSpeed = 109.22667f * (0.25f + RAND_FLOAT());

    mtxa_from_identity();
    sz = 3276.8f * (0.1f + RAND_FLOAT());
    sy = 3276.8f * (0.1f + RAND_FLOAT());
    sx = 3276.8f * (0.1f + RAND_FLOAT());
    mtxa_scale_xyz(sx, sy, sz);
    mtxa_rotate_y(rand() & 0x7FFF);
    mtxa_rotate_x(rand() & 0x7FFF);
    mtxa_tf_vec_xyz(0.0f, 0.0f, 65536.0f, &wind->dir);
}

void fn_1_995E0(struct BgBubbleWork *work)
{
    struct BgBubbleWind *wind = &work->wind;
    wind->phase.x += wind->speed.x;
    wind->phase.y += wind->speed.y;
    wind->phase.z += wind->speed.z;
    wind->rot += wind->rotSpeed;
}

void fn_1_99628(struct BgBubbleWork *work)
{
    memset(work->bubbles, 0, sizeof(work->bubbles));
}

/* Bubble spawning and simulation. */
void fn_1_99654(struct BgBubbleWork *work, int mask)
{
    float unused = 0.8f;
    struct StageBgModel *bg;
    struct GMAModel *model;
    struct BgBubbleWind *wind;
    float radius;
    float scale;
    float fill;
    float spawnRate;
    float s;
    float tx;
    float ty;
    float r;
    float tz;
    float x;
    float rate;
    Vec origin;
    Vec baseVel;
    Vec tp;
    S16Vec ang;
    if (modeCtrl.courseFlags & (1 << 26))
        fill = fn_1_15F720();
    else
    {
        switch (modeCtrl.gameType)
        {
        case 0:
        case 1:
        case 2:
        case 14:
            fill = 1.0f - (float)mode_info.timerCurr / (float)mode_info.timerMax;
            break;
        default:
            fill = 0.1f + (1.0f - 0.45f * math_sin(16384.0f + 72.81778f * world_theme.animTimer));
            break;
        }
    }
    {
        int i;
        struct BgBubbleBubble *bubble;
        int count;

    count = (int)(64.0f * fill) + 0x30;
    if (fill < 0.2f)
        spawnRate = 10.0f * fill;
    else
        spawnRate = 2.0f;

    bg = work->washTs;
    if (bg == NULL || !(bg->flags & mask))
        return;
    model = bg->model;
    mtxa_from_translate(&bg->pos);
    mtxa_rotate_z(bg->rotZ);
    mtxa_rotate_y(bg->rotY);
    mtxa_rotate_x(bg->rotX);
    mtxa_scale(&bg->scale);
    scale = bg->scale.x;
    radius = model->boundSphereRadius * scale;
    mtxa_tf_vec_xyz(
        model->boundSphereCenter.x,
        model->boundSphereCenter.y + 0.5f * radius,
        model->boundSphereCenter.z,
        &origin);
    mtxa_tf_vec_xyz(0.0f, model->boundSphereRadius * (0.025f * (1.0f + fill)), 0.0f, &baseVel);

    bubble = work->bubbles;
    for (i = 64; i > 0; i--, bubble++)
    {
        if (bubble->life == 0 && i < count)
        {
            bubble->pos = origin;
            bubble->pos.x += radius * (RAND_FLOAT() - 0.5f);
            bubble->pos.y += radius * (RAND_FLOAT() - 0.5f);
            bubble->pos.z += radius * (RAND_FLOAT() - 0.5f);
            bubble->vel = baseVel;
            bubble->vel.x += radius * (0.08f * (RAND_FLOAT() - 0.5f));
            bubble->vel.y += radius * (0.03f * (RAND_FLOAT() - 0.5f));
            bubble->vel.z += radius * (0.08f * (RAND_FLOAT() - 0.5f));
            bubble->scale = 0.0f;
            x = 0.05f + (0.45f + 0.5f * fill) * RAND_FLOAT();
            bubble->radius = 0.8f * (scale * x);
            r = bubble->radius;
            bubble->radiusY = r * (0.95f + 0.1f * RAND_FLOAT());
            bubble->radiusVel = r * (0.15f * (RAND_FLOAT() - 0.5f));
            bubble->rate = 0.014f * (1.0f + bubble->radius * RAND_FLOAT());
            bubble->ang = (s16)(16384.0f * (RAND_FLOAT() - 0.5f));
            bubble->angVel = 0;
            bubble->life = 60.0f * (spawnRate + 3.0f * RAND_FLOAT());
        }
        if (bubble->life != 0)
        {
            wind = &work->wind;
            ang.x = wind->rot + (int)mathutil_vec_dot_prod(&bubble->pos, &wind->dir);
            s = 0.8f + math_sin(ang.x);
            mtxa_from_mtx(wind->mtx);
            mtxa_tf_point(&bubble->pos, &tp);
            ang = wind->phase;
            ang.x += (s16)tp.x;
            ang.y += (s16)tp.y;
            ang.z += (s16)tp.z;
            tx = s * (wind->amp.x * (0.8f + math_sin(ang.x)));
            ty = s * (wind->amp.y * (0.8f + math_sin(ang.y)));
            tz = s * (wind->amp.z * (0.8f + math_sin(ang.z)));
            bubble->vel.y += 0.01f;
            rate = bubble->rate;
            bubble->vel.x += rate * (tx - bubble->vel.x);
            bubble->vel.y += rate * (ty - bubble->vel.y);
            bubble->vel.z += rate * (tz - bubble->vel.z);
            bubble->pos.x += bubble->vel.x;
            bubble->pos.y += bubble->vel.y;
            bubble->pos.z += bubble->vel.z;
            bubble->scale += 0.05f * (1.0f - bubble->scale);
            r = bubble->radius;
            bubble->radiusVel += 0.04f * (r - bubble->radiusY);
            bubble->radiusVel *= 0.99f;
            bubble->radiusY += bubble->radiusVel;
            bubble->radiusX = (r * r) / bubble->radiusY;
            bubble->angVel += (s16)(128.0f * math_sin(bubble->ang));
            bubble->angVel = 0.97f * (float)bubble->angVel;
            bubble->ang += bubble->angVel;
            bubble->life--;
        }
    }
    }
}

void fn_1_99F2C(struct BgBubbleWork *work, int mask)
{
    struct StageBgModel *bg = work->wash;
    struct OrdTblNode *list;
    struct BgBubbleOrdNode *node;

    if (bg == NULL || !(bg->flags & mask))
        return;
    if (g_some_render_flag.flags & 0x84)
        return;
    list = fn_1_CCD5C(0);
    node = ord_tbl_alloc_node(sizeof(*node));
    if (node != NULL)
    {
        node->node.drawFunc = (void (*)(struct OrdTblNode *))fn_1_99FCC;
        node->work = work;
        fn_1_CD418(list, &node->node);
    }
}

/* Draw bubbles: grab the frame buffer as a refraction texture, then draw each bubble
 * distorted by it. */
void fn_1_99FCC(struct BgBubbleOrdNode *node)
{
    int i;
    struct BgBubbleWork *work = node->work;
    int width;
    int height;
    void *dest;
    struct BgBubbleBubble *bubble;
    struct GMAModel *model;
    void *prevFunc;
    float proj[7];
    float vp[6];
    Mtx texMtx;
    Vec pos;
    int x, y, w, h;
    float radius;
    float depth;
    float alpha;
    float big;
    float invZ;
    float t;

    GXGetProjectionv(proj);
    mtxa_from_identity();
    mtxa->mtxA[0][0] = -0.5f * proj[1];
    mtxa->mtxA[0][2] = 0.5f + 0.5f * proj[2];
    mtxa->mtxA[1][1] = 0.5f * proj[3];
    mtxa->mtxA[1][2] = 0.5f + 0.5f * proj[4];
    mtxa_to_mtx(texMtx);

    dest = lbl_801988E0;
    GXGetViewportv(vp);
    x = vp[0];
    y = vp[1];
    w = vp[2];
    h = vp[3];
    width = w >> 1;
    height = h >> 1;
    GXSetTexCopySrc_cached(x, y, w, h);
    GXSetTexCopyDst_cached(width, height, 4, 1);
    GXCopyTex(dest, 0);
    GXInitTexObj(&work->texObj, dest, width, height, 4, 0, 0, 0);

    prevFunc = g_yet_another_unk_draw_func(fn_1_9A380);
    avdisp_set_z_mode(1, 3, 0);
    GXSetZCompLoc_cached(1);
    model = work->bubbleModel;
    radius = model->boundSphereRadius;
    bubble = work->bubbles;
    for (i = 64; i > 0; i--, bubble++)
    {
        if (bubble->life == 0)
            continue;
        mtxa_from_mtx(g_ord_tbl_stuff->matrices[0]);
        mtxa_translate(&bubble->pos);
        mathutil_mtxA_get_translate(&pos);
        depth = -pos.z - bubble->scale * radius;
        if (depth < 5.0f)
            continue;
        if (depth < 15.0f)
            alpha = (depth - 5.0f) / 10.0f;
        else
            alpha = 1.0f;
        big = bubble->scale * MAX(bubble->radiusX, bubble->radiusY);
        if (g_is_sphere_visible(&model->boundSphereCenter, radius * big))
        {
            mtxa_sq_from_identity();
            mtxa_rotate_z(bubble->ang);
            mtxa_scale_xyz(bubble->radiusX * bubble->scale, bubble->radiusY * bubble->scale, bubble->scale);
            mtxa_rotate_z(-bubble->ang);
            invZ = 1.0f / pos.z;
            t = proj[2] + invZ * (pos.x * proj[1]);
            if (t > 1.0f)
                t = 1.0f;
            else if (t < -1.0f)
                t = -1.0f;
            work->indMtx[0][1] = 0.6f * t;
            work->indMtx[1][0] = 0.6f;
            mtxa_push();
            mtxa_mult_left(texMtx);
            mtxa_to_mtx(work->texMtx);
            mtxa_pop();
            load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
            if (alpha != 1.0f)
                avdisp_set_alpha(alpha);
            avdisp_draw_model_unculled_sort_never(model);
        }
    }
    GXSetZCompLoc_cached(0);
    avdisp_set_z_mode(1, 3, 1);
    g_yet_another_unk_draw_func(prevFunc);
}

/* TEV setup for a bubble: refraction texture indirect-warped by the bubble's normal map. */
void fn_1_9A380(struct BgBubbleTevArg *arg)
{
    struct BgBubbleWork *work = world_theme.work;
    struct TevStageInfo tev = arg->tev;

    GXLoadTexObj_cached(&work->texObj, tev.texMapId);
    GXLoadTexObj_cached(&work->bubbleModel->texObjs[1], tev.texMapId + 1);
    GXLoadTexMtxImm(work->texMtx, tev.texMtxId, 0);
    g_GXSetIndTexMtx_wrapper(tev.unk1C, work->indMtx, 0);
    GXSetTevSwapMode_cached(tev.tevStage, 0, 1);
    tex_coord_gen(tev.texCoordId, 0, 0, tev.texMtxId);
    g_GXSetTevOrder_wrapper(tev.tevStage, tev.texCoordId, tev.texMapId, 0xFF);
    GXSetTevColorIn_cached(tev.tevStage, 8, 0xF, 0xF, 0);
    GXSetTevColorOp_cached(tev.tevStage, 0, 0, 3, 1, 2);
    GXSetTevAlphaIn_cached(tev.tevStage, 7, 4, 0, 0);
    GXSetTevAlphaOp_cached(tev.tevStage, 0, 0, 1, 1, 0);
    tex_coord_gen(tev.texCoordId + 1, 1, 5, 0x3C);
    GXSetIndTexOrder_cached(tev.tevIndStage, tev.texCoordId + 1, tev.texMapId + 1);
    GXSetTevIndirect_cached(tev.tevStage, tev.tevIndStage, 0, 0, tev.unk1C, 0, 0, 0, 0, 0);
    tev.tevStage += 1;
    tev.texCoordId += 2;
    tev.tevIndStage += 1;
    tev.texMtxId += 3;
    tev.unk1C += 1;
    arg->tev = tev;
}

void fn_1_9A600(struct BgBubbleWork *work)
{
    int i;
    struct BgBubbleCloth *cloth = work->cloths;

    for (i = work->clothCount; i > 0; i--, cloth++)
        fn_1_9A8CC(cloth);
}

void fn_1_9A650(struct BgBubbleWork *work)
{
    int i;
    struct BgBubbleCloth *cloth = work->cloths;

    for (i = work->clothCount; i > 0; i--, cloth++)
        fn_1_9ABE8(cloth);
}

void fn_1_9A6A0(struct BgBubbleWork *work, int mask)
{
    int i;
    struct BgBubbleCloth *cloth = work->cloths;

    for (i = work->clothCount; i > 0; i--, cloth++)
        fn_1_9AC40(cloth);
}

void fn_1_9A6F0(struct BgBubbleWork *work, int mask)
{
    int i;
    struct BgBubbleCloth *cloth;
    struct GMAModel *model;
    struct StageBgModel *bg;
    struct OrdTblNode *entry;
    struct BgBubbleClothOrdNode *node;

    mtxa_from_mtx(g_ord_tbl_stuff->matrices[0]);
    g_set_active_light_group(world_theme.unkAA);
    cloth = work->cloths;
    for (i = work->clothCount; i > 0; i--, cloth++)
    {
        bg = cloth->model;
        if (bg->flags & mask)
        {
            model = bg->model;
            mtxa_from_mtx(g_ord_tbl_stuff->matrices[0]);
            mtxa_translate(&bg->pos);
            mtxa_rotate_z(bg->rotZ);
            mtxa_rotate_y(bg->rotY);
            mtxa_rotate_x(bg->rotX);
            mtxa_scale(&bg->scale);
            if (g_is_sphere_visible(&model->boundSphereCenter, model->boundSphereRadius * bg->scale.x))
            {
                if (model->translucentShapeCount == 0)
                {
                    mtxa_push();
                    fn_1_9B470(cloth);
                    mtxa_pop();
                    load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
                    avdisp_draw_model_unculled_sort_never(model);
                }
                else
                {
                    entry = ord_tbl_get_entry_for_pos(&model->boundSphereCenter);
                    node = ord_tbl_alloc_node(sizeof(*node));
                    if (node != NULL)
                    {
                        node->node.drawFunc = (void (*)(struct OrdTblNode *))fn_1_9A854;
                        node->cloth = cloth;
                        mtxa_to_mtx(node->mtx);
                        node->lightGroup = get_active_light_group();
                        fn_1_CD418(entry, &node->node);
                    }
                }
            }
        }
    }
}

void fn_1_9A854(struct BgBubbleClothOrdNode *node)
{
    struct BgBubbleCloth *cloth = node->cloth;
    struct GMAModel *model = cloth->model->model;

    load_light_group_cached(node->lightGroup);
    fn_1_9B470(cloth);
    mtxa_from_mtx(node->mtx);
    load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
    avdisp_draw_model_unculled_sort_never(model);
}

/* Build a cloth bone chain from the model's skin matrices. */
void fn_1_9A8CC(struct BgBubbleCloth *cloth)
{
    int count;
    Mtx **mtxPtrs;
    struct BgBubbleClothBone *bone;
    Mtx *mtx;
    struct GMAModel *model;
    Mtx *mtxs;
    Mtx **ptr;
    s16 *parents;
    struct BgBubbleClothBone *parent;

    model = cloth->model->model;
    count = model->mtxCount;
    mtxPtrs = g_some_buffer_ptr;
    mtxs = (Mtx *)(mtxPtrs + count);
    cloth->boneCount = count;
    cloth->bones = alloc_from_heap_or_panic(bg_heap, count * sizeof(struct BgBubbleClothBone), "bg_bubble.c", 0x419);
    ptr = mtxPtrs;
    mtx = mtxs;
    for (; count > 0; count--, ptr++, mtx++)
        *ptr = mtx;
    draw_poly(model, mtxPtrs);

    mtx = mtxs;
    bone = cloth->bones;
    for (count = cloth->boneCount; count > 0; count--, bone++, mtx++)
    {
        mtxa_from_mtx(*mtx);
        mathutil_mtxA_get_translate(&bone->pos);
        mathutil_mtxA_set_translate_xyz(0.0f, 0.0f, 0.0f);
        mtxa_to_mtx(bone->mtx);
        bone->dir.x = 1.0f;
        bone->dir.y = 0.0f;
        bone->dir.z = 0.0f;
        bone->normal.x = 0.0f;
        bone->normal.y = 0.0f;
        bone->normal.z = 1.0f;
        bone->vel.x = RAND_FLOAT() - 0.5f;
        bone->vel.y = RAND_FLOAT() - 0.5f;
        bone->vel.z = RAND_FLOAT() - 0.5f;
        bone->rate = 0.02f * (1.0f + 0.5f * RAND_FLOAT());
        bone->len = 0.0f;
        bone->parent = NULL;
        bone->child = NULL;
    }
    bone = &cloth->bones[cloth->boneCount - 1];
    bone->rate *= 0.125f;

    parents = fn_80069964(model);
    bone = cloth->bones;
    for (count = cloth->boneCount; count > 0; count--, bone++, parents++)
    {
        if (*parents >= 0)
        {
            parent = &cloth->bones[*parents];
            bone->parent = parent;
            parent->child = bone;
            bone->len = mathutil_vec_distance(&parent->pos, &bone->pos);
            if (bone->len <= 1.1920929e-7f)
                bone->len = 0.0f;
        }
    }
}

void fn_1_9ABE8(struct BgBubbleCloth *cloth)
{
    if (cloth->bones != NULL)
    {
        OSFreeToHeap(bg_heap, cloth->bones);
        cloth->bones = NULL;
        cloth->boneCount = 0;
    }
}

/* Cloth physics: gravity, bone stiffness, wind and length constraints. */
void fn_1_9AC40(struct BgBubbleCloth *cloth)
{
    struct BgBubbleWork *work;
    struct BgBubbleWind *wf;
    int i;
    struct BgBubbleClothBone *bone;
    struct BgBubbleClothBone *parent;
    struct BgBubbleClothBone *child;
    struct StageBgModel *bg = cloth->model;
    Vec wind;
    Vec localWind;
    Vec force;
    Vec a;
    Vec d;
    Vec n;
    Vec tp;
    S16Vec ang;
    s16 rx, ry;
    float gz;
    float gy;
    float gx;
    float s;
    float f;

    work = world_theme.work;
    wf = &work->wind;
    ang.x = wf->rot + (int)mathutil_vec_dot_prod(&bg->pos, &wf->dir);
    s = 0.8f + math_sin(ang.x);
    mtxa_from_mtx(wf->mtx);
    mtxa_tf_point(&bg->pos, &tp);
    ang = wf->phase;
    ang.x += (s16)tp.x;
    ang.y += (s16)tp.y;
    ang.z += (s16)tp.z;
    wind.x = s * (wf->amp.x * (0.8f + math_sin(ang.x)));
    wind.y = s * (wf->amp.y * (0.8f + math_sin(ang.y)));
    wind.z = s * (wf->amp.z * (0.8f + math_sin(ang.z)));
    wind.x *= 1.5f;
    wind.y *= 1.5f;
    wind.z *= 1.5f;
    mtxa_from_rotate_z(bg->rotZ);
    mtxa_rotate_y(bg->rotY);
    mtxa_rotate_x(bg->rotX);
    gx = 0.0f;
    gy = -1.0f;
    gz = 0.0f;
    mtxa_rigid_inv_tf_vec(&wind, &localWind);

    bone = cloth->bones;
    for (i = cloth->boneCount; i > 0; i--, bone++)
    {
        parent = bone->parent;
        child = bone->child;
        if (parent == NULL || bone->len <= 0.0f)
            continue;
        {
            bone->vel.x += 0.032f * gx;
            bone->vel.y += 0.032f * gy;
            bone->vel.z += 0.032f * gz;
            force.x = 0.0f;
            force.y = 0.0f;
            force.z = 0.0f;
            d.x = bone->pos.x - parent->pos.x;
            d.y = bone->pos.y - parent->pos.y;
            d.z = bone->pos.z - parent->pos.z;
            vec_normalize_len(&d);
            n = parent->normal;
            f = mathutil_vec_dot_prod(&n, &d);
            force.x += -0.03f * (n.x * f);
            force.y += -0.03f * (n.y * f);
            force.z += -0.03f * (n.z * f);
            a = parent->dir;
            f = mathutil_vec_dot_prod(&a, &d);
            force.x += -0.005f * (a.x * f);
            force.y += -0.005f * (a.y * f);
            force.z += -0.005f * (a.z * f);
            bone->vel.x += force.x;
            bone->vel.y += force.y;
            bone->vel.z += force.z;
            parent->vel.x -= force.x;
            parent->vel.y -= force.y;
            parent->vel.z -= force.z;

            vec_to_euler_xy(&bone->dir, &rx, &ry);
            mtxa_from_rotate_y(ry);
            mtxa_rotate_x(rx);
            mtxa_tf_vec(&bone->vel, &force);
            mtxa_tf_vec(&localWind, &a);
            force.x += 0.005f * (a.x - force.x);
            force.y += 0.005f * (a.y - force.y);
            force.z += bone->rate * (a.z - force.z);
            mtxa_rigid_inv_tf_vec(&force, &bone->vel);
            bone->pos.x += bone->vel.x;
            bone->pos.y += bone->vel.y;
            bone->pos.z += bone->vel.z;

            d.x = bone->pos.x - parent->pos.x;
            d.y = bone->pos.y - parent->pos.y;
            d.z = bone->pos.z - parent->pos.z;
            vec_normalize_len(&d);
            f = bone->len;
            force.x = parent->pos.x + d.x * f;
            force.y = parent->pos.y + d.y * f;
            force.z = parent->pos.z + d.z * f;
            bone->vel.x += force.x - bone->pos.x;
            bone->vel.y += force.y - bone->pos.y;
            bone->vel.z += force.z - bone->pos.z;
            bone->pos = force;
            f = mathutil_vec_dot_prod(&d, &bone->vel);
            bone->vel.x -= f * d.x;
            bone->vel.y -= f * d.y;
            bone->vel.z -= f * d.z;
            parent->vel.x -= f * d.x;
            parent->vel.y -= f * d.y;
            parent->vel.z -= f * d.z;

            bone->normal.x = -0.5f * bone->vel.z;
            bone->normal.y = 0.0f;
            bone->normal.z = 1.0f;
            vec_normalize_len(&bone->normal);
            mathutil_vec_cross_prod(&bone->normal, &d, &a);
            vec_normalize_len(&a);
            a.x += 0.05f * parent->dir.x;
            a.y += 0.05f * parent->dir.y;
            a.z += 0.05f * parent->dir.z;
            if (child != NULL)
            {
                a.x += 0.05f * child->dir.x;
                a.y += 0.05f * child->dir.y;
                a.z += 0.05f * child->dir.z;
            }
            vec_normalize_len(&a);
            bone->dir.x += 0.4f * (a.x - bone->dir.x);
            bone->dir.y += 0.4f * (a.y - bone->dir.y);
            bone->dir.z += 0.4f * (a.z - bone->dir.z);
        }
    }
}

/* Write the cloth bone transforms into the model's skin matrix table. */
void fn_1_9B470(struct BgBubbleCloth *cloth)
{
    struct GMAModel *model = cloth->model->model;
    int i;
    Mtx *mtxs;
    Mtx *dest;
    struct BgBubbleClothBone *bone;
    Vec up;
    Quat q;

    up.x = 1.0f;
    up.y = 0.0f;
    up.z = 0.0f;
    bone = cloth->bones;
    mtxs = g_some_buffer_ptr;
    dest = mtxs;
    for (i = cloth->boneCount; i > 0; i--, dest++, bone++)
    {
        quat_from_dirs(&q, &up, &bone->dir);
        mtxa_from_quat(&q);
        mtxa_mult_right(bone->mtx);
        mathutil_mtxA_set_translate(&bone->pos);
        mtx_copy(mtxa->mtxA, *dest);
    }
    pointer_range_advance(model, mtxs);
}

void fn_1_9B564(struct BgBubbleWork *work)
{
    struct BgBubbleAwawa *awawa = &work->awawa;
    struct StageBgModel *bg = awawa->model;

    if (bg != NULL)
    {
        awawa->texObjs = bg->model->texObjs;
        awawa->texObjs2 = init_common_gma->modelEntries[0x5F].model->texObjs;
    }
}

void fn_1_9B5A0(struct BgBubbleWork *work, int mask) {}

void fn_1_9B5A4(struct BgBubbleWork *work, int mask) {}

/* ape bones tracked for ripple spawning, -1 terminated */
static s16 s_trackedBones[] = { 9, 5, 1, 0x18, 0x13, -1 };

/* ripple model indices in init_common_gma */
static s16 s_rippleModels[8] = { 0x12, 0x11, 0x11, 0x11, 0x13, 0x13, 0x13, 0x13 };

/* Ripples: spawn where the ape's tracked bones touch the water, plus random ones. */
void fn_1_9B5A8(struct BgBubbleWork *work, int mask)
{
    float unused = 1.0f / 600.0f;
    struct BgBubbleSpa *spa = &work->spa;
    struct BgBubbleRipple *rip;
    struct StageBgModel *bg = spa->model;
    struct BgBubbleTracker *tracker;
    struct Ape *ape;
    s16 *bones;
    struct GMAModel *model;
    Vec pt;
    Vec old;
    Mtx rotMtx;
    int idx;
    int i;
    float inv;
    if (!(bg->flags & mask))
        return;

    ape = get_ape_by_idx(0);
    if (ape != NULL)
    {
        mtxa_from_quat(&ape->rot);
        mtxa_to_mtx(rotMtx);
        mtxa_from_translate(&ape->pos);
        mtxa_scale_s(ape->scale);
        mtxa_translate(&ape->unk264);
        mtxa_mult_right(rotMtx);
        for (bones = s_trackedBones, tracker = spa->trackers; *bones >= 0; bones++, tracker++)
        {
            mtxa_push();
            mtxa_mult_right(g_get_ape_anim_smth(ape, *bones));
            mathutil_mtxA_get_translate(&pt);
            mtxa_pop();
            old = tracker->pos;
            tracker->pos = pt;
            if (fabs(pt.y - -320.0f) > 20.0)
            {
                tracker->inWater = 0;
                tracker->counter = 0;
            }
            else
            {
                if ((tracker->counter & 3) == 0)
                {
                    rip = &spa->ripples[spa->ringIdx];
                    spa->ringIdx--;
                    if (spa->ringIdx < 0)
                        spa->ringIdx = 31;
                    rip->life = 60.0f * (0.5f + RAND_FLOAT());
                    rip->pos.x = pt.x;
                    rip->pos.y = -320.0f;
                    rip->pos.z = pt.z;
                    rip->vel.x = 0.5f * (pt.x - old.x);
                    rip->vel.y = 0.0f;
                    rip->vel.z = 0.5f * (pt.z - old.z);
                    rip->radius = 50.0f * (1.0f + RAND_FLOAT());
                    rip->radiusVel = 5.0f * (1.0f + 0.5f * RAND_FLOAT());
                    rip->alpha = RAND_FLOAT();
                    rip->rotZ = RAND15() - 0x7FFF;
                    if (tracker->inWater)
                        idx = s_rippleModels[(u32)RAND15() % 8];
                    else
                        idx = 0x12;
                    rip->model = efcmdlGma->modelEntries[idx].model;
                }
                tracker->inWater = 1;
                tracker->counter++;
            }
        }
    }

    if ((g_font_char_var_3 & 0xF) == 0)
    {
        model = bg->model;
        mtxa_from_translate(&bg->pos);
        mtxa_rotate_z(bg->rotZ);
        mtxa_rotate_y(bg->rotY);
        mtxa_rotate_x(bg->rotX);
        mtxa_scale(&bg->scale);
        mtxa_translate(&model->boundSphereCenter);
        mtxa_rotate_y(RAND15() - 0x7FFF);
        mtxa_tf_point_xyz(model->boundSphereRadius * (0.5f * RAND_FLOAT()), 0.0f, 0.0f, &pt);
        rip = &spa->ripples[spa->ringIdx];
        spa->ringIdx--;
        if (spa->ringIdx < 0)
            spa->ringIdx = 31;
        rip->life = 60.0f * (1.0f + RAND_FLOAT());
        rip->pos.x = pt.x;
        rip->pos.y = -320.0f;
        rip->pos.z = pt.z;
        rip->vel.x = 0.0f;
        rip->vel.y = 0.0f;
        rip->vel.z = 0.0f;
        rip->radius = 10.0f * (1.0f + RAND_FLOAT());
        rip->radiusVel = 5.0f * (1.0f + 0.5f * RAND_FLOAT());
        rip->alpha = RAND_FLOAT();
        rip->rotZ = RAND15() - 0x7FFF;
        idx = s_rippleModels[(u32)RAND15() % 8];
        rip->model = efcmdlGma->modelEntries[idx].model;
    }

    rip = spa->ripples;
    for (i = 32; i > 0; i--, rip++)
    {
        if (rip->life != 0)
        {
            rip->vel.x *= 0.98f;
            rip->vel.z *= 0.98f;
            rip->pos.x += rip->vel.x;
            rip->pos.z += rip->vel.z;
            rip->radiusVel *= 0.998f;
            rip->radius += rip->radiusVel;
            rip->life--;
            if ((float)rip->life < 15.0f)
            {
                inv = 1.0f / (float)(rip->life + 1);
                rip->alpha *= 1.0f - inv;
            }
            else
                rip->alpha += 0.1f * (1.0f - rip->alpha);
        }
    }
}

/* Render the spa water surface and ripples into a small texture. */
void fn_1_9BC6C(struct BgBubbleWork *work, int mask)
{
    struct BgBubbleSpa *spa = &work->spa;
    float vp[6];
    GXColor color;
    int x, y, w, h;
    int w2, h2;
    struct StageBgModel *bg;
    struct GMAModel *model;
    void *prevFunc;
    int i;
    struct BgBubbleRipple *rip;
    struct GMAModel *ripModel;
    float a;

    if (!fn_1_15F7E4())
    {
        spa->texReady = 0;
        return;
    }
    bg = spa->model;
    if (bg == NULL)
        return;

    GXGetViewportv(vp);
    x = vp[0];
    y = vp[1];
    w = vp[2];
    h = vp[3];
    if (w > 256)
        w = 256;
    if (h > 256)
        h = 256;
    w2 = w >> 1;
    h2 = h >> 1;
    GXSetViewport(x, y, w, h, 0.0f, 1.0f);
    GXSetScissor_cached(x, y, h, w);
    color.r = 0;
    color.g = 0;
    color.b = 0;
    color.a = 0;
    g_smth_with_bg_color_drawing(&color);

    model = bg->model;
    mtxa_from_mtx(g_ord_tbl_stuff->matrices[0]);
    mtxa_translate(&bg->pos);
    mtxa_rotate_z(bg->rotZ);
    mtxa_rotate_y(bg->rotY);
    mtxa_rotate_x(bg->rotX);
    mtxa_scale(&bg->scale);
    if (g_is_sphere_visible(&model->boundSphereCenter, model->boundSphereRadius * bg->scale.x))
    {
        prevFunc = g_yet_another_unk_draw_func(fn_1_9C658);
        load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
        fn_1_10A300(model);
        g_yet_another_unk_draw_func(prevFunc);
    }

    avdisp_set_z_mode(1, 7, 0);
    rip = spa->ripples;
    for (i = 32; i > 0; i--, rip++)
    {
        if (rip->life == 0)
            continue;
        a = rip->alpha;
        if (a <= 0.0f)
            continue;
        ripModel = rip->model;
        mtxa_from_mtx(g_ord_tbl_stuff->matrices[0]);
        mtxa_translate(&rip->pos);
        if (g_is_sphere_visible(&ripModel->boundSphereCenter, rip->radius))
        {
            a *= 0.3f;
            mtxa_rotate_x(0x4000);
            mtxa_rotate_z(rip->rotZ);
            mtxa_scale_s(rip->radius / ripModel->boundSphereRadius);
            set_post_mult_color(a, a, a, 1.0f);
            load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
            avdisp_draw_model_unculled_sort_never(ripModel);
        }
    }
    fade_color_base_default();
    avdisp_set_z_mode(1, 3, 1);
    GXSetZMode_cached(1, 3, 1);

    GXSetTexCopySrc_cached(x, y, w, h);
    GXSetTexCopyDst_cached(w2, h2, 0x28, 1);
    GXCopyTex(spa->rippleTexData, 1);
    GXInitTexObj(&spa->rippleTexObj, spa->rippleTexData, w2, h2, 1, 0, 0, 0);
    spa->texReady = 1;
    GXSetViewport(vp[0], vp[1], vp[2], vp[3], vp[4], vp[5]);
    GXSetScissor_cached((int)vp[0], (int)vp[1], (int)vp[2], (int)vp[3]);
}

/* Draw the spa model, refracting the frame buffer through the ripple texture. */
void fn_1_9C03C(struct BgBubbleWork *work, int mask)
{
    struct BgBubbleSpa *spa = &work->spa;
    struct StageBgModel *bg = spa->model;
    struct GMAModel *model;
    void *prevFunc;
    float proj[7];
    float vp[6];
    int x, y, w, h;
    int width, height;

    if (bg == NULL)
        return;
    model = bg->model;
    mtxa_from_mtx(g_ord_tbl_stuff->matrices[0]);
    mtxa_translate(&bg->pos);
    mtxa_rotate_z(bg->rotZ);
    mtxa_rotate_y(bg->rotY);
    mtxa_rotate_x(bg->rotX);
    mtxa_scale(&bg->scale);
    if (g_is_sphere_visible(&model->boundSphereCenter, model->boundSphereRadius * bg->scale.x))
    {
        load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
        if (!spa->texReady)
            avdisp_draw_model_unculled_sort_always(model);
        else
        {
            fn_1_10A2C0(model);
            GXGetProjectionv(proj);
            mtxa_push();
            mtxa_to_mtx(spa->spaTexMtx);
            mtxa_from_identity();
            mtxa->mtxA[0][0] = -0.5f * proj[1];
            mtxa->mtxA[0][2] = 0.5f + 0.5f * proj[2];
            mtxa->mtxA[1][1] = 0.5f * proj[3];
            mtxa->mtxA[1][2] = 0.5f + 0.5f * proj[4];
            mtxa_mult_right(spa->spaTexMtx);
            mtxa_to_mtx(spa->spaTexMtx);
            mtxa_pop();

            GXGetViewportv(vp);
            x = vp[0];
            y = vp[1];
            w = vp[2];
            h = vp[3];
            width = w >> 1;
            height = h >> 1;
            GXSetTexCopySrc_cached(x, y, w, h);
            GXSetTexCopyDst_cached(width, height, 4, 1);
            GXCopyTex(lbl_801988E0, 0);
            GXInvalidateTexAll();
            GXInitTexObj(&spa->spaTexObj, lbl_801988E0, width, height, 4, 0, 0, 0);
            prevFunc = g_yet_another_unk_draw_func(fn_1_9C2CC);
            fn_1_10A300(model);
            g_yet_another_unk_draw_func(prevFunc);
        }
    }
}

/* TEV setup for the spa: screen copy warped by the ripple texture, then ripple highlights. */
void fn_1_9C2CC(struct BgBubbleTevArg *arg)
{
    struct BgBubbleWork *work = world_theme.work;
    struct BgBubbleSpa *spa = &work->spa;
    struct TevStageInfo tev = arg->tev;
    float indMtx[2][3];

    GXLoadTexObj_cached(&spa->spaTexObj, tev.texMapId);
    GXLoadTexObj_cached(&spa->rippleTexObj, tev.texMapId + 1);
    GXLoadTexObj_cached(&spa->rippleTexObj, tev.texMapId + 2);
    indMtx[0][0] = 0.6f;
    indMtx[0][1] = 0.0f;
    indMtx[0][2] = 0.0f;
    indMtx[1][0] = 0.0f;
    indMtx[1][1] = -0.4f;
    indMtx[1][2] = 0.0f;
    g_GXSetIndTexMtx_wrapper(arg->tev.unk1C, indMtx, 0);
    GXLoadTexMtxImm(spa->spaTexMtx, tev.texMtxId, 0);
    tex_coord_gen(tev.texCoordId, 0, 0, tev.texMtxId);
    g_GXSetTevOrder_wrapper(tev.tevStage, tev.texCoordId, tev.texMapId, 0xFF);
    GXSetTevSwapMode_cached(tev.tevStage, 0, 0);
    GXSetTevColorIn_cached(tev.tevStage, 8, 0xF, 0xF, 0);
    GXSetTevColorOp_cached(tev.tevStage, 0, 0, 3, 1, 0);
    GXSetTevAlphaIn_cached(tev.tevStage, 7, 7, 7, 0);
    GXSetTevAlphaOp_cached(tev.tevStage, 0, 0, 0, 1, 0);
    tex_coord_gen(tev.texCoordId + 1, 0, 0, tev.texMtxId);
    GXSetIndTexOrder_cached(tev.tevIndStage, tev.texCoordId + 1, tev.texMapId + 1);
    GXSetTevIndirect_cached(tev.tevStage, tev.tevIndStage, 0, 1, tev.unk1C, 0, 0, 0, 0, 0);
    tev.tevStage++;
    tev.tevIndStage++;
    tev.texCoordId += 2;
    g_GXSetTevOrder_wrapper(tev.tevStage, tev.texCoordId - 1, tev.texMapId + 2, 0xFF);
    GXSetTevSwapMode_cached(tev.tevStage, 0, 0);
    GXSetTevKAlphaSel_cached(tev.tevStage, 4);
    GXSetTevColorIn_cached(tev.tevStage, 0xF, 0, 8, 0);
    GXSetTevColorOp_cached(tev.tevStage, 0, 0, 0, 1, 0);
    GXSetTevAlphaIn_cached(tev.tevStage, 4, 7, 7, 0);
    GXSetTevAlphaOp_cached(tev.tevStage, 0, 0, 3, 1, 0);
    g_GXSetTevIndirect_zero_if_different(tev.tevStage);
    tev.tevStage += 1;
    tev.texMtxId += 3;
    tev.unk1C += 1;
    tev.texMapId += 3;
    arg->tev = tev;
}

/* TEV setup for the water surface: two scrolling caustic layers. */
void fn_1_9C658(struct BgBubbleTevArg *arg)
{
    struct TevStageInfo tev = arg->tev;

    GXLoadTexObj_cached(efcmdlGma->modelEntries[0x14].model->texObjs, tev.texMapId);
    mtxa_push();
    mtxa_from_translate_xyz(0.0f, 0.004166667f * world_theme.animTimer, 0.0f);
    mtxa_scale_s(2.0f);
    GXLoadTexMtxImm(mtxa->mtxA, tev.texMtxId, 1);
    mtxa_from_translate_xyz(0.008333334f * world_theme.animTimer, 0.0f, 0.0f);
    mtxa_rotate_z(0x3000);
    mtxa_scale_s(3.0f);
    GXLoadTexMtxImm(mtxa->mtxA, tev.texMtxId + 1, 1);
    mtxa_pop();

    tex_coord_gen(tev.texCoordId, 1, 4, tev.texMtxId);
    g_GXSetTevOrder_wrapper(tev.tevStage, tev.texCoordId, tev.texMapId, 0xFF);
    GXSetTevSwapMode_cached(tev.tevStage, 0, 0);
    GXSetTevKColorSel_cached(tev.tevStage, 4);
    GXSetTevColorIn_cached(tev.tevStage, 0xF, 0xE, 8, 0);
    GXSetTevColorOp_cached(tev.tevStage, 0, 0, 0, 1, 0);
    GXSetTevAlphaIn_cached(tev.tevStage, 7, 7, 7, 0);
    GXSetTevAlphaOp_cached(tev.tevStage, 0, 0, 0, 1, 0);
    g_GXSetTevIndirect_zero_if_different(tev.tevStage);
    tev.tevStage++;
    tev.texCoordId++;

    tex_coord_gen(tev.texCoordId, 1, 4, tev.texMtxId + 1);
    g_GXSetTevOrder_wrapper(tev.tevStage, tev.texCoordId, tev.texMapId, 0xFF);
    GXSetTevSwapMode_cached(tev.tevStage, 0, 0);
    GXSetTevKColorSel_cached(tev.tevStage, 4);
    GXSetTevColorIn_cached(tev.tevStage, 0xF, 0xE, 8, 0);
    GXSetTevColorOp_cached(tev.tevStage, 0, 0, 0, 1, 0);
    GXSetTevAlphaIn_cached(tev.tevStage, 7, 7, 7, 0);
    GXSetTevAlphaOp_cached(tev.tevStage, 0, 0, 0, 1, 0);
    g_GXSetTevIndirect_zero_if_different(tev.tevStage);

    tev.tevStage += 1;
    tev.texCoordId += 1;
    tev.texMtxId += 6;
    tev.texMapId += 1;
    arg->tev = tev;
}

void fn_1_9C9B8(struct RenderEffect *rendEfc)
{
    struct BgBubbleSpa *spa;

    rendEfc->enableFlags = 0x1C;
    rendEfc->work = NULL;
    spa = &((struct BgBubbleWork *)world_theme.work)->spa;
    spa->model->flags |= 0x20000;
    spa->rippleTexData = alloc_from_heap_or_panic(bg_heap, GXGetTexBufferSize(128, 128, 1, 0, 0), "bg_bubble.c", 0x66A);
    fn_1_9CBA4(world_theme.work);
}

void fn_1_9CA5C(struct RenderEffect *rendEfc)
{
    struct BgBubbleWork *work = world_theme.work;
    struct BgBubbleSpa *spa = &work->spa;

    if (spa->rippleTexData != NULL)
    {
        OSFreeToHeap(bg_heap, spa->rippleTexData);
        spa->rippleTexData = NULL;
    }
}

void fn_1_9CAB8(struct RenderEffect *rendEfc)
{
    int mask;

    if (!is_game_paused_and_in_view_stage())
    {
        mask = g_smth_with_challenge_mode_var_and_3p();
        fn_1_9B5A8(world_theme.work, mask);
        fn_1_9CE88(world_theme.work, mask);
    }
}

void fn_1_9CB18(int flag, struct RenderEffect *rendEfc)
{
    int mask;

    mask = g_smth_with_challenge_mode_var_and_3p();
    switch (flag)
    {
    case 4:
        fn_1_9BC6C(world_theme.work, mask);
        break;
    case 0x10:
        fn_1_9C03C(world_theme.work, mask);
        break;
    case 8:
        fn_1_9D25C(world_theme.work, mask);
        break;
    }
}

/* Per-camera soap film overlay state. */
void fn_1_9CBA4(struct BgBubbleWork *work)
{
    struct BgBubbleFilm *films = work->film;
    int i;
    struct BgBubbleFilm *film;

    work->filmTexObjs = efcmdlGma->modelEntries[0x14].model->texObjs;
    film = films;
    for (i = 4; i > 0; i--, film++)
    {
        film->pos[0] = RAND_FLOAT();
        film->pos[1] = RAND_FLOAT();
        film->pos[2] = RAND_FLOAT();
        film->vel.x = 0.0f;
        film->vel.y = 0.0125f;
        film->vel.z = 0.0f;
        film->sinA = 0.025f + 0.0125f * RAND_FLOAT();
        film->sinB = 0.025f + 0.0125f * RAND_FLOAT();
        if (rand() & 1)
            film->sinA *= -1.0f;
        if (rand() & 1)
            film->sinB *= -1.0f;
        film->ampA = -(0.4f + 0.25f * RAND_FLOAT());
        film->ampB = -(0.4f + 0.25f * RAND_FLOAT());
        film->scaleA = 1.0f + 0.3f * RAND_FLOAT();
        film->scaleB = 1.0f + 0.3f * RAND_FLOAT();
        film->r = 255.0f;
        film->g = 255.0f;
        film->b = 255.0f;
        film->a = 255.0f;
    }
}

void fn_1_9CE88(struct BgBubbleWork *work, int mask)
{
    int i;
    struct BgBubbleFilm *films = work->film;
    struct BgBubbleFilm *film;
    s8 *status = g_poolInfo.cameraPool.statusList;
    struct Camera *camera = cameras;
    struct StageBgModel *spa;
    struct GMAModel *model;
    Vec camPos;
    Vec b;
    Vec a;
    float one;
    float f;
    float d;
    float len;

    mtxa_from_mtx(camera->unk14C);
    mtxa_rigid_inv_tf_tl(&camPos);
    film = films;
    for (i = 4; i > 0; i--, film++, status++, camera++)
    {
        if (*status != 0)
        {
            f = -1.0f;
            spa = work->spa.model;
            model = spa->model;
            mtxa_push();
            mtxa_from_translate(&spa->pos);
            mtxa_rotate_z(spa->rotZ);
            mtxa_rotate_y(spa->rotY);
            mtxa_rotate_x(spa->rotX);
            mtxa_scale(&spa->scale);
            mtxa_tf_point(&model->boundSphereCenter, &a);
            d = mathutil_distance_2(a.x, a.z, camPos.x, camPos.z);
            d *= 1.0f / (model->boundSphereRadius * spa->scale.x);
            if (f < 0.0f || f > d)
                f = d;
            mtxa_pop();

            if (f < 0.0f)
                f = 0.0f;
            else if (f < 0.5f)
                f = 1.0f;
            else if (f < 1.0f)
                f = 1.0f - 2.0f * (f - 0.5f);
            else
                f = 0.0f;
            one = 1.0f;

            film->vel.x += 0.08f * (0.00625f - film->vel.x);
            film->vel.y += 0.08f * (0.0125f - film->vel.y);
            mtxa_tf_vec(&film->vel, &a);
            len = mathutil_vec_len(&a);
            a.z = 0.0f;
            if (a.x == 0.0f && a.y == 0.0f)
                a.y = len;
            else
                vec_set_len(len, &a, &b);
            mtxa_push();
            mtxa_from_mtx(camera->unk17C);
            mtxa_rigid_inv_tf_vec_xyz(0.0f, 0.0f, -1.0f, &a);
            mtxa_pop();
            mtxa_tf_vec(&a, &a);
            b.x += a.x;
            b.y += a.y;
            film->pos[0] += b.x;
            film->pos[1] += b.y;
            if (f > 0.0f)
            {
                d = (160.0f + 95.0f * one) * f;
                film->r += 0.05f * (d - film->r);
                film->g += 0.04f * (d - film->g);
                d = (192.0f * one) * f;
                film->b += 0.02f * (d - film->b);
                film->a += 0.03f * (d - film->a);
            }
            else
            {
                film->r += 0.05f * -film->r;
                film->g += 0.04f * -film->g;
                film->b += 0.02f * -film->b;
                film->a += 0.03f * -film->a;
            }
        }
    }
}

void fn_1_9D25C(struct BgBubbleWork *work, int mask)
{
    struct BgBubbleFilm *films = work->film;
    struct BgBubbleFilm *film = films + g_something_with_camera3;

    if (g_some_render_flag.flags & 0x84)
        return;
    if (!fn_1_15F84C())
        return;
    fn_1_9D2D4(work, film);
}

/* Draw the full-screen soap film overlay. */
void fn_1_9D2D4(struct BgBubbleWork *work, struct BgBubbleFilm *film)
{
    float x0, x1, y0, y1;
    float invW;
    float invH;
    float indMtx[2][3];
    float proj[7];
    GXColor color;

    GXLoadTexObj_cached(work->filmTexObjs, 0);
    GXLoadTexObj_cached(work->filmTexObjs, 1);
    GXSetBlendMode_cached(1, 4, 1, 0);
    GXSetZMode_cached(0, 7, 0);
    g_something_with_raster_state(0);
    GXSetCullMode_cached(0);
    mtxa_from_translate((Vec *)film->pos);
    mtxa_scale_s(1.5f);
    GXLoadTexMtxImm(mtxa->mtxA, 0x1E, 1);
    mtxa_from_translate_xyz(
        film->ampA * (film->pos[0] + 0.2f * film->pos[1]),
        film->ampB * (film->pos[1] + 0.2f * film->pos[0]),
        0.0f);
    mtxa_scale_xyz(film->scaleA, film->scaleB, 1.0f);
    GXLoadTexMtxImm(mtxa->mtxA, 0x21, 1);
    opti_GXSetChanCtrl(4, 0, 1, 1, 0, 2, 1);
    indMtx[0][0] = 0.0f;
    indMtx[0][1] = film->sinA;
    indMtx[0][2] = 0.0f;
    indMtx[1][0] = 0.0f;
    indMtx[1][1] = 0.0f;
    indMtx[1][2] = film->sinB;
    g_GXSetTevIndirect_zero_if_different(0);
    tex_coord_gen(0, 1, 4, 0x1E);
    g_GXSetTevOrder_wrapper(0, 0, 0, 4);
    GXSetTevColorIn_cached(0, 0xF, 0xF, 0xF, 8);
    GXSetTevColorOp_cached(0, 0, 0, 0, 1, 0);
    GXSetTevAlphaIn_cached(0, 7, 7, 7, 5);
    GXSetTevAlphaOp_cached(0, 0, 0, 0, 1, 0);
    tex_coord_gen(1, 1, 4, 0x21);
    GXSetIndTexOrder_cached(0, 1, 1);
    g_GXSetIndTexMtx_wrapper(1, indMtx, 0);
    GXSetTevIndirect_cached(0, 0, 0, 7, 1, 0, 0, 0, 0, 0);
    GXSetNumTevStages_cached(1);
    GXSetNumTexGens_cached(2);
    GXSetNumIndStages_cached(1);
    GXSetNumChans_cached(1);
    g_something_with_GX_vtx_desc(0x2A00);
    mtxa_from_identity();
    GXLoadPosMtxImm(mtxa->mtxA, 0);
    GXGetProjectionv(proj);
    f32_to_u8(&film->r, &color.r);
    f32_to_u8(&film->g, &color.g);
    f32_to_u8(&film->a, &color.b);
    f32_to_u8(&film->b, &color.a);
    invW = 1.0f / proj[1];
    invH = 1.0f / proj[3];
    x0 = -100.0f * (-1.0f - proj[2]) * invW;
    x1 = -100.0f * (1.0f - proj[2]) * invW;
    y0 = -100.0f * (-1.0f - proj[4]) * invH;
    y1 = -100.0f * (1.0f - proj[4]) * invH;
    GXBegin(0x80, 0, 4);
    GXPosition3f32(x0, y0, -100.0f);
    GXColor4u8(255, 255, 255, (u8)color.r);
    GXTexCoord2f32(0.0f, 0.0f);
    GXPosition3f32(x1, y0, -100.0f);
    GXColor4u8(255, 255, 255, (u8)color.g);
    GXTexCoord2f32(1.0f, 0.0f);
    GXPosition3f32(x1, y1, -100.0f);
    GXColor4u8(255, 255, 255, (u8)color.b);
    GXTexCoord2f32(1.0f, 1.0f);
    GXPosition3f32(x0, y1, -100.0f);
    GXColor4u8(255, 255, 255, (u8)color.a);
    GXTexCoord2f32(0.0f, 1.0f);
}

int fn_1_9D854(int index, struct GMAModelEntry *entry)
{
    struct BgBubbleWork *work = world_theme.work;

    switch (index)
    {
    case 0:
        work->bubbleModel = entry->model;
        break;
    }
    return 1;
}

int fn_1_9D87C(int index, struct StageBgModel *model)
{
    struct BgBubbleWork *work = world_theme.work;

    switch (index)
    {
    case 0:
        work->wash = model;
        break;
    case 1:
        work->washTs = model;
        break;
    case 2:
        work->awawa.model = model;
        break;
    case 3:
    case 4:
    case 5:
    case 6:
        if (work->clothCount < 16)
        {
            work->cloths[work->clothCount].model = model;
            work->clothCount++;
        }
        break;
    case 7:
        work->spa.model = model;
        break;
    }
    return 1;
}
