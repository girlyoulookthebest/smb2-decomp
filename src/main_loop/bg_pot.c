/* Boiling Pot background: sun lens flare, falling leaves, boiling pots with steam, and the fire. */

#include "types.h"
#include "avdisp.h"
#include "ball.h"
#include "background.h"
#include "camera.h"
#include "effect.h"
#include "frustum.h"
#include "gx.h"
#include "light.h"
#include "mathutil.h"
#include "pc_hfr.h"
#include "mode.h"
#include "nl2ngc.h"
#include "pool.h"
#include "rand.h"
#include "stage.h"
#include "stcoli.h"


/* Sun occlusion test through the Z buffer (GXPeekZ queries). */
struct BgPotPeekEntry
{
    u32 unk0;
    u32 z;
};

struct BgPotPeekQuery
{
    /*0x00*/ struct BgPotPeekQuery *next;
    /*0x04*/ s32 count;
    /*0x08*/ struct BgPotPeekEntry *entries;
    /*0x0C*/ struct BgPotPeekEntry data[20];
};  /* size = 0xAC */

struct BgPotFlare
{
    /*0x000*/ struct StageBgModel *sun;
    /*0x004*/ Vec offsets[20];
    /*0x0F4*/ float alpha[20];
    /*0x144*/ Vec screenPts[20];
    /*0x234*/ struct BgPotPeekQuery peek[4];
};  /* size = 0x4E4 */

struct BgPotLeaf
{
    /*0x00*/ Vec pos;
    /*0x0C*/ Vec vel;
    /*0x18*/ s16 rotX;
    /*0x1A*/ s16 rotY;
    /*0x1C*/ s16 rotZ;
    /*0x1E*/ s16 rotVelX;
    /*0x20*/ s16 rotVelY;
    /*0x22*/ s16 rotVelZ;
    /*0x24*/ float swayX;
    /*0x28*/ float swayY;
    /*0x2C*/ float swayZ;
};  /* size = 0x30 */

struct BgPotLeaves
{
    /*0x00*/ struct StageBgModel *model;
    /*0x04*/ s32 count;
    /*0x08*/ struct BgPotLeaf *leaves;
};

struct BgPotBubble
{
    /*0x00*/ s32 timer;
    /*0x04*/ Vec pos;
    /*0x10*/ Vec vel;
    /*0x1C*/ s16 rot;
    /*0x1E*/ s16 rotVel;
    /*0x20*/ float size;
    /*0x24*/ float unk24;
    /*0x28*/ float unk28;
};  // size = 0x2C

struct BgPotVentCloud
{
    /*0x00*/ float unk0;
    /*0x04*/ float unk4;
    /*0x08*/ float unk8;
    /*0x0C*/ float unkC;
    /*0x10*/ float unk10;
    /*0x14*/ s16 unk14;
    /*0x16*/ s16 unk16;
};  // size = 0x18

struct BgPotVent
{
    /*0x00*/ struct StageBgModel *model;
    /*0x04*/ float unk4;
    /*0x08*/ float unk8;
    /*0x0C*/ struct BgPotVentCloud clouds[4];
};  // size = 0x6C

struct BgPotCloudLayer
{
    /*0x00*/ Vec pos;
    /*0x0C*/ Vec vel;
    /*0x18*/ float scaleX;
    /*0x1C*/ float scaleY;
    /*0x20*/ float unk20;
    /*0x24*/ float unk24;
    /*0x28*/ float unk28;
    /*0x2C*/ s16 rot[3];
    /*0x32*/ s16 rotVel[3];
    /*0x38*/ float unk38;
    /*0x3C*/ float unk3C;
    /*0x40*/ float unk40;
};  // size = 0x44

struct BgPotCloudTex
{
    /*0x000*/ s32 rendered;
    /*0x004*/ GXTexObj tex;
    /*0x024*/ void *texData;
    /*0x028*/ struct TevStageInfo tev;
    /*0x054*/ Mtx mtx;
    /*0x084*/ s16 layerCount;
    /*0x086*/ u8 filler86[2];
    /*0x088*/ GXTexObj *layerTex[4];
    /*0x098*/ struct BgPotCloudLayer layers[4];
};  // size = 0x1A8

struct BgPotPot
{
    /*0x000*/ struct StageBgModel *model;
    /*0x004*/ float phase;
    /*0x008*/ float phaseVel;
    /*0x00C*/ float unkC;
    /*0x010*/ float unk10;
    /*0x014*/ float unk14;
    /*0x018*/ float unk18;
    /*0x01C*/ float unk1C;
    /*0x020*/ float unk20;
    /*0x024*/ float unk24;
    /*0x028*/ float unk28;
    /*0x02C*/ float unk2C;
    /*0x030*/ s16 unk30;
    /*0x032*/ s16 unk32;
    /*0x034*/ struct BgPotBubble bubbles[16];
};  /* size = 0x2F4 */

struct BgPotHaze
{
    /*0x00*/ Vec pos;
    /*0x0C*/ Vec vel;
    /*0x18*/ float unk18;
    /*0x1C*/ float unk1C;
    /*0x20*/ float unk20;
    /*0x24*/ float unk24;
    /*0x28*/ float unk28;
    /*0x2C*/ float unk2C;
    /*0x30*/ float unk30;
    /*0x34*/ float unk34;
    /*0x38*/ float unk38;
    /*0x3C*/ float r;
    /*0x40*/ float g;
    /*0x44*/ float a;
    /*0x48*/ float b;
};  /* size = 0x4C */

struct BgPotWork
{
    /*0x000*/ s32 initialized;
    /*0x004*/ s32 variant;
    /*0x008*/ s16 windOn;
    /*0x00A*/ s16 windTimer;
    /*0x00C*/ Vec windPos;
    /*0x018*/ Vec windVel;
    /*0x024*/ Vec windTarget;
    /*0x030*/ Vec windDir;
    /*0x03C*/ struct GMAModel *flareModel;
    /*0x040*/ struct BgPotFlare flare;
    /*0x524*/ s32 glowTexCount;
    /*0x528*/ GXTexObj *glowTex[2];
    /*0x530*/ GXTexObj *hazeTex;
    /*0x534*/ struct BgPotHaze haze[4];
    /*0x664*/ struct BgPotLeaves *leaves;
    /*0x668*/ struct BgPotCloudTex cloudTex[1];
    /*0x810*/ struct GMAModel *steamBaseModel;
    /*0x814*/ struct GMAModel *steamModel;
    /*0x818*/ s32 potCount;
    /*0x81C*/ struct BgPotPot pots[2];
    /*0xE04*/ struct GMAModel *unkE04;
    /*0xE08*/ s32 ventCount;
    /*0xE0C*/ struct BgPotVent vents[8];
};

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
    void (*disp)(u32, struct RenderEffect *);
};

struct BgPotOrdNode
{
    struct OrdTblNode node;
    struct BgPotWork *work;
    void *data;
    Mtx mtx;
};

/* Sun lens flare node: only the work pointer follows the list node.  The
 * original allocates a literal 0xC (sizeof on GC); on x64 the node is 24
 * bytes, hence sizeof(*node). */
struct BgPotSunOrdNode
{
    struct OrdTblNode node;
    struct BgPotWork *work;
};

struct BgPotGlowOrdNode
{
    struct OrdTblNode node;
    struct BgPotWork *work;
    struct BgPotPot *pot;
    float t;
};

struct BgPotVentOrdNode
{
    struct OrdTblNode node;
    Mtx mtx;
    struct BgPotVent *vent;
};  // size = 0x3C

struct BgPotHazeOrdNode
{
    struct OrdTblNode node;
    struct BgPotWork *work;
    struct BgPotHaze *haze;
};

struct BgPotLeavesOrdNode
{
    struct OrdTblNode node;
    int lightGroup;
    struct BgPotLeaves *leaves;
};

extern struct { u32 flags; } g_some_render_flag;
extern void *g_some_buffer_ptr;
extern s32 lbl_1_bss_6F00;
extern void *bg_heap;

void *OSFreeToHeap(void *heap, void *ptr);
void *alloc_from_heap_or_panic(void *heap, u32 size, char *file, int line);
void GXGetViewportv(float *vp);
void GXGetScissor(u32 *left, u32 *top, u32 *wd, u32 *ht);
u32 GXGetTexBufferSize(u16 width, u16 height, u32 format, u8 mipmap, u8 maxLod);
void GXInitTexObj(GXTexObj *obj, void *image, u16 width, u16 height, int format, int wrapS, int wrapT, u8 mipmap);
void GXSetTexCopySrc_cached(u16 left, u16 top, u16 width, u16 height);
void GXSetTexCopyDst_cached(u16 width, u16 height, int format, u8 mipmap);
void GXCopyTex(void *dest, int clear);
void GXInvalidateTexAll(void);
void gp_wait_wrapper(void);
extern void *stage_heap;
void GXGetProjectionv(float *proj);
void g_smth_with_gpu_and_locked_cache_mtx(void);
struct OrdTblNode *ord_tbl_get_entry_for_pos(Vec *pos);
struct OrdTblNode *fn_1_CCD5C(int);
void *ord_tbl_alloc_node(u32 size);
void fn_1_CD418(struct OrdTblNode *list, struct OrdTblNode *node);
void draw_poly(struct GMAModel *model, Mtx **mtxs);
void pointer_range_advance(struct GMAModel *model, void *mtxs);
void fn_1_D8130(int, int, struct RenderEffect *);
void g_smth_with_bg_model_names(struct BgModelSearch *search, int (*callback)(int, struct GMAModelEntry *));
u32 fn_1_118164(float z);
void g_stores_doubles2(float, float, float, float);
void GXLoadTexObj_cached(GXTexObj *obj, int map);
void GXLoadTexMtxImm(Mtx m, int id, int type);
void GXLoadPosMtxImm(Mtx m, int id);
void GXSetNumChans_cached(u8 n);
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
void g_GXSetIndTexMtx_wrapper(int id, float mtx[2][3], int scale);
void g_GXSetTevOrder_wrapper(int stage, int coord, int map, int color);
void g_GXSetTevIndirect_zero_if_different(int stage);
void g_something_with_GX_vtx_desc(u32 attrs);
void g_something_with_raster_state(int);
void opti_GXSetChanCtrl(int, int, int, int, int, int, int);
void mtxa_from_translate_xyz(float, float, float);
void mtxa_rigid_inv_tf_vec_xyz(float, float, float, Vec *);
void fn_1_117E94(int count, Vec *pts, struct BgPotPeekEntry *entries);
void fn_1_1181E8(struct BgPotPeekQuery *query);
void fn_1_118210(struct BgPotPeekQuery *query);

void fn_1_8FAC8(struct BgPotWork *);
void fn_1_8FC14(struct BgPotWork *);
void fn_1_8FC68(struct BgPotWork *);
void fn_1_8FC6C(struct BgPotWork *, u32);
void fn_1_8FD0C(struct OrdTblNode *);
void fn_1_90294(struct BgPotWork *);
void fn_1_90638(struct BgPotWork *);
void fn_1_90694(struct BgPotWork *);
void fn_1_909BC(struct BgPotWork *, u32);
void fn_1_90A90(struct OrdTblNode *);
void fn_1_90BB4(struct BgPotWork *);
void fn_1_90E04(struct BgPotWork *);
void fn_1_91018(struct BgPotWork *, u32);
void fn_1_9124C(struct OrdTblNode *);
void fn_1_91410(struct BgPotWork *);
void fn_1_91714(struct BgPotWork *);
void fn_1_91CCC(struct OrdTblNode *);
void fn_1_91BC8(struct BgPotWork *, u32);
void fn_1_92244(struct BgPotPot *);
void fn_1_92A70(struct OrdTblNode *);
void fn_1_92D0C(struct BgPotWork *);
void fn_1_92E70(void);
void fn_1_924A4(struct BgPotWork *, struct BgPotPot *);
void fn_1_931BC(struct BgPotWork *);
void fn_1_9333C(struct BgPotWork *);
void fn_1_93778(struct BgPotWork *, u32);
void fn_1_9387C(struct OrdTblNode *);
void fn_1_939E0(struct RenderEffect *);
void fn_1_939A0(struct RenderEffect *);
void fn_1_939B4(struct RenderEffect *);
void fn_1_939B8(struct RenderEffect *);
void fn_1_939BC(u32, struct RenderEffect *);
void g_something_with_stage_heap_and_pot_theme(struct BgPotWork *);
void fn_1_94324(struct BgPotWork *);
void fn_1_943A0(struct BgPotWork *);
void fn_1_94608(void);
void fn_1_946A4(struct BallEnvArg *);
int fn_1_949AC(int, struct GMAModelEntry *);
int fn_1_94A88(int, struct StageBgModel *);

static inline void tex_coord_gen(int dst, int func, int src, int mtx)
{
    GXSetTexCoordGen2_cached(dst, func, src, mtx, 0, 0x7D);
}

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

/* Unreferenced table (kept for data layout) */
static u32 lbl_1_data_325E0[0x5C] =
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
    { BG_MDL_CMP_PREFIX, "POD_RENZ_FREA_" },
    { BG_MDL_CMP_FULL, "POD_POD_AWA_A" },
    { BG_MDL_CMP_FULL, "POD_POD_SUIMEN_A" },
    { BG_MDL_CMP_FULL, "POD_YUGE_A" },
    { BG_MDL_CMP_FULL, "POD_FIRE_POWDER_A" },
    { BG_MDL_CMP_PREFIX, "POD_SHADOWMAKE_" },
    { BG_MDL_CMP_PREFIX, "POD_SUIJOKI_" },
    { BG_MDL_CMP_END },
};

static struct BgModelSearch s_bgModelSearch[] =
{
    { BG_MDL_CMP_PREFIX, "POD_SUN_" },
    { BG_MDL_CMP_PREFIX, "POD_LEAVES_" },
    { BG_MDL_CMP_FULL, "POD_POD_A" },
    { BG_MDL_CMP_PREFIX, "POD_FIRE_" },
    { BG_MDL_CMP_FULL, "POD_ROCK_A" },
    { BG_MDL_CMP_FULL, "POD_KAMADO_A" },
    { BG_MDL_CMP_FULL, "POD_TABLE_A" },
    { BG_MDL_CMP_PREFIX, "POD_TENT_" },
    { BG_MDL_CMP_PREFIX, "POD_GLASS_" },
    { BG_MDL_CMP_PREFIX, "POD_MAKI_" },
    { BG_MDL_CMP_PREFIX, "POD_TUBO_" },
    { BG_MDL_CMP_PREFIX, "POD_TREE_" },
    { BG_MDL_CMP_END },
};

static struct RenderEffectFuncs s_rendEfcFuncs =
{
    fn_1_939A0,
    fn_1_939B4,
    fn_1_939B8,
    fn_1_939BC,
};

void bg_pot_init(void)
{
    struct BgPotWork *work = world_theme.work;
    struct RenderEffect rendEfc;

    bg_init_base();
    work->variant = 0;
    switch (current_stage_id)
    {
    case 0x27:
    case 0xF2:
    case 0xF3:
    case 0xF7:
    case 0xF9:
    case 0x10B:
    case 0x10C:
        work->variant = 2;
        break;
    default:
        work->variant = 3;
        break;
    }
    work->leaves = alloc_from_heap_or_panic(bg_heap, sizeof(struct BgPotLeaves), "bg_pot.c", 0xF5);
    memset(work->leaves, 0, sizeof(struct BgPotLeaves));
    if (work->initialized == 0)
    {
        g_smth_with_bg_model_names(s_modelSearch, fn_1_949AC);
        work->initialized = 1;
    }
    work->flare.sun = NULL;
    work->potCount = 0;
    work->ventCount = 0;
    g_init_stagedef_bg_fg_models(stagedef->bgModels, stagedef->bgModelCount, s_bgModelSearch, fn_1_94A88);
    g_init_stagedef_bg_fg_models(stagedef->fgModels, stagedef->fgModelCount, s_bgModelSearch, fn_1_94A88);
    work->windOn = rand() & 1;
    work->windTimer = 240.0f * RAND_FLOAT();
    work->windPos.x = 2.0f * (RAND_FLOAT() - 0.5f);
    work->windPos.y = 0.1f * (RAND_FLOAT() - 0.5f);
    work->windPos.z = 2.0f * (RAND_FLOAT() - 0.5f);
    work->windVel.x = 0.0f;
    work->windVel.y = 0.0f;
    work->windVel.z = 0.0f;
    work->windTarget.x = 0.0f;
    work->windTarget.y = 0.0f;
    work->windTarget.z = 0.0f;
    mtxa_from_rotate_y(rand() & 0x7FFF);
    mtxa_rotate_x((rand() & 0x1FF) - 0x100);
    mtxa_tf_vec_xyz(0.0f, 0.0f, -1.0f, &work->windDir);
    fn_1_8FAC8(work);
    fn_1_90294(work);
    fn_1_90BB4(work);
    fn_1_91410(work);
    fn_1_931BC(work);
    g_something_with_stage_heap_and_pot_theme(work);

    memset(&rendEfc, 0, sizeof(rendEfc));
    rendEfc.cameraMask = 0xFFFF;
    rendEfc.funcs = &s_rendEfcFuncs;
    fn_1_D8130(0, 0, &rendEfc);
    world_theme.unk98 = fn_1_94608;
    world_theme.unk90 = fn_1_946A4;
}

void bg_pot_tick(void)
{
    struct BgPotWork *work = world_theme.work;

    g_smth_with_challenge_mode_var_and_3p();
    bg_tick_base();
    if (is_game_paused_and_in_view_stage())
        return;
    {
        work->windTimer--;
        if (work->windTimer < 0)
        {
            work->windTimer = 240.0f * RAND_FLOAT();
            work->windOn ^= 1;
            if (work->windOn != 0)
            {
                float strength = 0.25f + 0.75f * RAND_FLOAT();

                work->windTarget.x = strength * (work->windDir.x + (0.2f * RAND_FLOAT() - 0.1f));
                work->windTarget.y = strength * (work->windDir.y + (0.2f * RAND_FLOAT() - 0.1f));
                work->windTarget.z = strength * (work->windDir.z + (0.2f * RAND_FLOAT() - 0.1f));
            }
            else
            {
                work->windTarget.x = 0.0f;
                work->windTarget.y = 0.0f;
                work->windTarget.z = 0.0f;
            }
        }
        work->windVel.x += 0.01f * (0.1f * (work->windTarget.x - work->windPos.x) - work->windVel.x);
        work->windVel.y += 0.01f * (0.1f * (work->windTarget.y - work->windPos.y) - work->windVel.y);
        work->windVel.z += 0.01f * (0.1f * (work->windTarget.z - work->windPos.z) - work->windVel.z);
        work->windPos.x += work->windVel.x;
        work->windPos.y += work->windVel.y;
        work->windPos.z += work->windVel.z;
        fn_1_8FC68(work);
        fn_1_90694(work);
        fn_1_90E04(work);
        fn_1_91714(work);
        fn_1_9333C(work);
        fn_1_943A0(work);
    }
}

void bg_pot_dest(void)
{
    struct BgPotWork *work = world_theme.work;

    fn_1_94324(work);
    fn_1_90638(work);
    fn_1_8FC14(work);
    if (work->leaves != NULL)
    {
        OSFreeToHeap(bg_heap, work->leaves);
        work->leaves = NULL;
    }
}

void bg_pot_disp(void)
{
    struct BgPotWork *work = world_theme.work;
    u32 mask = g_smth_with_challenge_mode_var_and_3p();

    fn_1_8FC6C(work, mask);
    fn_1_909BC(work, mask);
    fn_1_91018(work, mask);
    fn_1_91BC8(work, mask);
    fn_1_93778(work, mask);
    bg_disp_base();
}

void bg_pot_item_coin_coli(int arg0) {}

void bg_pot_fgt_init(void)
{
    bg_pot_init();
}

void bg_pot_fgt_tick(void)
{
    bg_pot_tick();
}

void bg_pot_fgt_dest(void)
{
    struct BgPotWork *work = world_theme.work;

    fn_1_94324(work);
    fn_1_90638(work);
    fn_1_8FC14(work);
    if (work->leaves != NULL)
    {
        OSFreeToHeap(bg_heap, work->leaves);
        work->leaves = NULL;
    }
}

void bg_pot_fgt_disp(void)
{
    u32 mask;
    struct BgPotWork *work;

    if (lbl_1_bss_6F00 == 0)
    {
        g_set_light_color_maybe(world_theme.unkAA, 0.4f, 0.4f, 0.4f);
        g_set_light_color_maybe(world_theme.unkA8, 0.4f, 0.4f, 0.4f);
    }
    work = world_theme.work;
    mask = g_smth_with_challenge_mode_var_and_3p();
    fn_1_8FC6C(work, mask);
    fn_1_909BC(work, mask);
    fn_1_91018(work, mask);
    fn_1_91BC8(work, mask);
    fn_1_93778(work, mask);
    bg_disp_base();
}

void bg_pot_fgt_item_coin_coli(int arg0) {}

/* Sun lens flare: init */
void fn_1_8FAC8(struct BgPotWork *work)
{
    struct BgPotFlare *flare = &work->flare;
    int i;
    struct GMAModel *model;
    Vec *offset;
    float t;

    if (flare->sun == NULL)
        return;
    for (i = 0; i < 4; i++)
    {
        flare->peek[i].count = 20;
        flare->peek[i].entries = flare->peek[i].data;
    }
    memset(flare->alpha, 0, sizeof(flare->alpha));
    model = flare->sun->model;
    offset = flare->offsets;
    for (i = 20; i > 0; i--, offset++)
    {
        mtxa_from_rotate_z(65536.0f * ((4.0f * (float)i) / 20.0f));
        t = (float)i / 20.0f;
        mtxa_tf_vec_xyz(0.3f * model->boundSphereRadius * t, 0.0f, 0.0f, offset);
    }
}

/* Sun lens flare: dest */
void fn_1_8FC14(struct BgPotWork *work)
{
    struct BgPotFlare *flare = &work->flare;
    int i;
    struct BgPotPeekQuery *query = flare->peek;

    for (i = 4; i > 0; i--, query++)
        fn_1_118210(query);
}

/* Sun lens flare: tick */
void fn_1_8FC68(struct BgPotWork *work) {}

/* Sun lens flare: disp */
void fn_1_8FC6C(struct BgPotWork *work, u32 mask)
{
    struct OrdTblNode *list;
    struct BgPotSunOrdNode *node;

    if (work->flare.sun != NULL && (work->flare.sun->flags & mask) && !(g_some_render_flag.flags & 0x84))
    {
        list = fn_1_CCD5C(0);
        node = ord_tbl_alloc_node(sizeof(*node));
        if (node != NULL)
        {
            node->node.drawFunc = fn_1_8FD0C;
            node->work = work;
            fn_1_CD418(list, &node->node);
        }
    }
}

/* The original reuses the alpha pointer variable for the sun model (needed for regalloc). */
#define SUN_MODEL ((struct GMAModel *)alpha)
void fn_1_8FD0C(struct OrdTblNode *arg)
{
    struct BgPotWork *work;
    struct BgPotFlare *flare;
    struct StageBgModel *sunBg;
    struct BgPotPeekQuery *query;
    struct GMAModel *model;
    struct BgPotPeekEntry *entry;
    float *alpha; /* holds the sun model pointer (SUN_MODEL) until the first loop */
    int i;
    Vec *pt;
    Vec *offset;
    u32 depth;
    int angle;
    float screenX;
    float screenY;
    float scale;
    float invW;
    float sum;
    float brightness;
    float fade;
    float b;
    Vec pos;
    float proj[7];
    Mtx mtx;

    work = ((struct BgPotSunOrdNode *)arg)->work;
    flare = &work->flare;
    sunBg = flare->sun;
    alpha = (float *)sunBg->model;
    mtxa_from_mtx(g_ord_tbl_stuff->matrices[0]);
    mtxa_translate(&sunBg->pos);
    mtxa_rotate_z(sunBg->rotZ);
    mtxa_rotate_y(sunBg->rotY);
    mtxa_rotate_x(sunBg->rotX);
    mtxa_scale(&sunBg->scale);
    mtxa_translate(&SUN_MODEL->boundSphereCenter);
    GXGetProjectionv(proj);
    invW = 1.0f / proj[3];
    mathutil_mtxA_get_translate(&pos);
    if (!(pos.z >= -0.1f))
    {
        b = 1.0f / pos.z;
        screenX = proj[2] + b * (pos.x * proj[1]);
        screenY = proj[4] + b * (pos.y * proj[3]);
        query = &flare->peek[g_something_with_camera3];
        depth = fn_1_118164(pos.z + SUN_MODEL->boundSphereRadius);
        entry = query->entries;
        alpha = flare->alpha;
        for (i = 20; i > 0; i--, alpha++, entry++)
        {
            if (entry->z > depth)
                *alpha += 0.4f * (1.0f - *alpha);
            else
                *alpha += 0.5f * (-0.1f - *alpha);
        }
        pt = flare->screenPts;
        offset = flare->offsets;
        for (i = 20; i > 0; i--, pt++, offset++)
        {
            *pt = pos;
            pt->x += offset->x;
            pt->y += offset->y;
        }
        fn_1_117E94(20, flare->screenPts, query->entries);
        fn_1_1181E8(query);
        mtxa_sq_from_identity();
        mtxa_to_mtx(mtx);
        scale = -1.1f / pos.z;
        mtxa_from_identity();
        mtxa_scale_s(scale);
        mtxa_mult_right(mtx);
        mathutil_mtxA_get_translate(&pos);
        sum = 0.0f;
        alpha = flare->alpha;
        mtxa_push();
        for (i = 20; i > 0; i--, alpha++)
        {
            if (*alpha < 0.0f)
                continue;
            sum += *alpha;
        }
        sum *= 0.1f;
        sum -= 0.8f;
        brightness = sum * (-pos.z / mathutil_vec_len(&pos));
        fade = brightness;
        if (fade > 1.0f)
            fade = 1.0f;
        else if (fade < 0.0f)
            fade = 0.0f;
        fade = 1.0f - 0.7f * fade;
        avdisp_set_z_mode(1, 3, 0);
        angle = 24576.0f + 8192.0f * (0.7f * screenX + screenY);
        model = work->flareModel;
        offset = flare->offsets;
        alpha = flare->alpha;
        mtxa_push();
        for (i = 20; i > 0; i--, alpha++, offset++)
        {
            sum = *alpha;
            if (sum <= 0.0f)
                continue;
            mtxa_push();
            mtxa_translate(offset);
            mtxa_rotate_z(angle + (i << 16) / 20);
            b = fade * (0.4f * sum);
            set_post_mult_color(b, b, b, 1.0f);
            mtxa_scale_s(invW * (0.2f * (1.0f + sum) * -pos.z) / (model->boundSphereRadius * scale));
            g_smth_with_gpu_and_locked_cache_mtx();
            avdisp_draw_model_unculled_sort_never(model);
            mtxa_pop();
        }
        mtxa_pop();
        fade_color_base_default();
        avdisp_set_z_mode(1, 3, 1);
        sum = brightness;
        angle = 24576.0f + 4096.0f * (screenX + 0.5f * screenY);
        model = work->flareModel;
        if (sum > 0.0f)
        {
            avdisp_set_z_mode(1, 3, 0);
            mtxa_rotate_z(angle);
            b = sum;
            if (b > 1.0f)
                b = 1.0f;
            set_post_mult_color(b, b, b, 1.0f);
            mtxa_scale_s((4.0f * -pos.z * invW) / (model->boundSphereRadius * scale));
            g_smth_with_gpu_and_locked_cache_mtx();
            avdisp_draw_model_unculled_sort_never(model);
            set_post_mult_color(1.0f, 1.0f, 1.0f, 1.0f);
            avdisp_set_z_mode(1, 3, 1);
        }
        mtxa_pop();
    }
}

#undef SUN_MODEL

/* Leaves: init */
void fn_1_90294(struct BgPotWork *work)
{
    struct BgPotLeaves *leaves = work->leaves;
    struct StageBgModel *bgModel;
    struct BgPotLeaf *leaf;
    int n;

    bgModel = leaves->model;
    if (bgModel == NULL)
        return;
    n = bgModel->model->mtxCount;
    if (n == 0)
    {
        leaves->leaves = NULL;
        leaves->count = 0;
        return;
    }
    leaves->count = n;
    leaves->leaves = alloc_from_heap_or_panic(bg_heap, n * sizeof(struct BgPotLeaf), "bg_pot.c", 0x331);
    leaf = leaves->leaves;
    for (; n > 0; n--, leaf++)
    {
        leaf->pos.x = RAND_FLOAT() - 0.5f;
        leaf->pos.y = RAND_FLOAT() - 0.5f;
        leaf->pos.z = RAND_FLOAT() - 0.5f;
        leaf->vel.x = RAND_FLOAT() - 0.5f;
        leaf->vel.y = RAND_FLOAT() - 0.5f;
        leaf->vel.z = RAND_FLOAT() - 0.5f;
        leaf->rotX = rand() & 0x7FFF;
        leaf->rotY = rand() & 0x7FFF;
        leaf->rotZ = rand() & 0x7FFF;
        leaf->rotVelX = 1638.4f * (0.5f + RAND_FLOAT());
        leaf->rotVelY = 1638.4f * (0.5f + RAND_FLOAT());
        leaf->rotVelZ = 1638.4f * (0.5f + RAND_FLOAT());
        leaf->swayX = 0.02f * (1.0f + RAND_FLOAT());
        leaf->swayY = 0.1f * (1.0f + RAND_FLOAT());
        leaf->swayZ = 0.1f * (1.0f + RAND_FLOAT());
    }
}

/* Leaves: dest */
void fn_1_90638(struct BgPotWork *work)
{
    struct BgPotLeaves *leaves = work->leaves;

    if (leaves->model != NULL && leaves->leaves != NULL)
    {
        OSFreeToHeap(bg_heap, leaves->leaves);
        leaves->leaves = NULL;
    }
}

/* Leaves: tick */
void fn_1_90694(struct BgPotWork *work)
{
    struct BgPotLeaves *leaves;
    struct StageBgModel *bgModel;
    int n;
    struct BgPotLeaf *leaf;
    float windLen;
    float dx;
    float dy;
    float dz;
    Vec wind;

    leaves = work->leaves;
    if (leaves->model == NULL)
        return;
    wind = work->windPos;
    bgModel = leaves->model;
    mtxa_from_translate(&bgModel->pos);
    mtxa_rotate_z(bgModel->rotZ);
    mtxa_rotate_y(bgModel->rotY);
    mtxa_rotate_x(bgModel->rotX);
    mtxa_rigid_inv_tf_vec(&wind, &wind);
    windLen = mathutil_vec_len(&wind);
    leaf = leaves->leaves;
    for (n = leaves->count; n > 0; n--, leaf++)
    {
        leaf->vel.x += 0.02f * (wind.x - leaf->vel.x);
        leaf->vel.y += 0.02f * (wind.y - leaf->vel.y);
        leaf->vel.z += 0.02f * (wind.z - leaf->vel.z);
        leaf->rotX += (s16)((float)leaf->rotVelX * windLen);
        leaf->rotY += (s16)((float)leaf->rotVelY * windLen);
        leaf->rotZ += (s16)((float)leaf->rotVelZ * windLen);
        dx = windLen * leaf->swayX * math_sin(leaf->rotX);
        dy = windLen * leaf->swayY * math_sin(leaf->rotY);
        dz = windLen * leaf->swayZ * math_sin(leaf->rotZ);
        leaf->vel.x += dx;
        leaf->vel.y += dy;
        leaf->vel.z += dz;
        leaf->vel.x -= 0.02f * leaf->pos.x;
        leaf->vel.y -= 0.02f * leaf->pos.y;
        leaf->vel.z -= 0.02f * leaf->pos.z;
        leaf->pos.x += leaf->vel.x;
        leaf->pos.y += leaf->vel.y;
        leaf->pos.z += leaf->vel.z;
    }
}

/* Leaves: disp */
void fn_1_909BC(struct BgPotWork *work, u32 mask)
{
    struct BgPotLeaves *leaves = work->leaves;
    struct StageBgModel *bgModel = leaves->model;
    struct GMAModel *model;
    struct OrdTblNode *entry;
    struct BgPotLeavesOrdNode *node;

    if (bgModel == NULL)
        return;
    model = bgModel->model;
    if (!(bgModel->flags & mask))
        return;
    mtxa_from_mtx(g_ord_tbl_stuff->matrices[0]);
    mtxa_translate(&bgModel->pos);
    mtxa_rotate_z(bgModel->rotZ);
    mtxa_rotate_y(bgModel->rotY);
    mtxa_rotate_x(bgModel->rotX);
    mtxa_scale(&bgModel->scale);
    entry = ord_tbl_get_entry_for_pos(&model->boundSphereCenter);
    node = ord_tbl_alloc_node(sizeof(*node));
    if (node != NULL)
    {
        node->node.drawFunc = fn_1_90A90;
        node->lightGroup = get_active_light_group();
        node->leaves = leaves;
    }
    fn_1_CD418(entry, &node->node);
}

void fn_1_90A90(struct OrdTblNode *arg)
{
    int n;
    struct BgPotLeaves *leaves;
    struct BgPotLeaf *leaf;
    struct StageBgModel *bgModel;
    Mtx *mtxs;
    Mtx *mtx;
    struct GMAModel *model;
    Mtx **ptrs;
    Mtx **buf;
    int count;

    leaves = ((struct BgPotLeavesOrdNode *)arg)->leaves;
    bgModel = leaves->model;
    model = bgModel->model;
    load_light_group_cached(((struct BgPotLeavesOrdNode *)arg)->lightGroup);
    count = leaves->count;
    buf = g_some_buffer_ptr;
    mtxs = (Mtx *)(buf + count);
    ptrs = buf;
    mtx = mtxs;
    for (n = count; n > 0; n--, ptrs++, mtx++)
        *ptrs = mtx;
    draw_poly(model, buf);
    mtx = mtxs;
    leaf = leaves->leaves;
    for (n = leaves->count; n > 0; n--, leaf++, mtx++)
    {
        mtxa_from_mtx(*mtx);
        mtxa_translate(&leaf->pos);
        mtxa_to_mtx(*mtx);
    }
    pointer_range_advance(model, mtxs);
    mtxa_from_mtx(g_ord_tbl_stuff->matrices[0]);
    mtxa_translate(&bgModel->pos);
    mtxa_rotate_z(bgModel->rotZ);
    mtxa_rotate_y(bgModel->rotY);
    mtxa_rotate_x(bgModel->rotX);
    mtxa_scale(&bgModel->scale);
    load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
    avdisp_draw_model_unculled_sort_never(model);
}

/* Pots: init */
void fn_1_90BB4(struct BgPotWork *work)
{
    int n;
    struct BgPotPot *pot = work->pots;

    for (n = work->potCount; n > 0; n--, pot++)
    {
        pot->phase = RAND_FLOAT();
        pot->phaseVel = (0.5f + 0.5f * RAND_FLOAT()) / 60.0f;
        pot->unkC = 1.0f;
        pot->unk10 = 1.0f;
        pot->unk14 = 1.0f;
        pot->unk18 = 1.0f;
        pot->unk1C = 0.0f;
        pot->unk28 = 0.1f + 0.5f * RAND_FLOAT();
        pot->unk24 = pot->unk28 + (0.5f * (1.0f - pot->unk28)) * (0.5f + 0.5f * RAND_FLOAT());
        pot->unk20 = pot->unk24 + (0.5f * (1.0f - pot->unk24)) * (0.5f + 0.5f * RAND_FLOAT());
        pot->unk2C = (0.2f * (3.0f - (pot->unk28 + (pot->unk20 + pot->unk24)))) * (0.25f + 0.75f * RAND_FLOAT());
        fn_1_92244(pot);
    }
}

/* Pots: tick */
void fn_1_90E04(struct BgPotWork *work)
{
    int n;
    struct BgPotPot *pot;
    float timeFrac;
    float rate;

    if (mode_info.timerMax == 0)
    {
        timeFrac = 0.0f;
        rate = 0.0f;
    }
    else
    {
        timeFrac = 1.0f - (float)mode_info.timerCurr / (float)mode_info.timerMax;
        if (mode_info.timerCurr < mode_info.timerMax)
            rate = 1.0f / (float)mode_info.timerMax;
        else
            rate = 0.0f;
    }
    pot = work->pots;
    for (n = work->potCount; n > 0; n--, pot++)
    {
        pot->phase += pot->phaseVel;
        pot->unkC += 0.2f * ((1.0f - 0.7f * timeFrac) - pot->unkC);
        pot->unk10 += rate * (pot->unk20 - pot->unk10);
        pot->unk14 += rate * (pot->unk24 - pot->unk14);
        pot->unk18 += rate * (pot->unk28 - pot->unk18);
        pot->unk1C += rate * (pot->unk2C - pot->unk1C);
        fn_1_924A4(work, pot);
    }
}

/* Pots: disp (enqueue steam + glow) */
void fn_1_91018(struct BgPotWork *work, u32 mask)
{
    int n;
    struct BgPotPot *pot;
    struct GMAModel *model;
    int glow;
    struct StageBgModel *bg;
    struct OrdTblNode *entry;
    struct BgPotOrdNode *node;
    struct BgPotGlowOrdNode *glowNode;
    int variant;
    u32 fl;
    float t;
    Vec pos;

    fl = g_some_render_flag.flags;
    if (fl & 0x84)
        return;
    variant = work->variant;
    if (fl & 1)
        glow = variant & 2;
    else
        glow = variant & 1;
    if (fl & 2)
        glow = 0;
    if (fl & 0x800)
        glow = 0;
    model = work->steamModel;
    pot = work->pots;
    for (n = work->potCount; n > 0; n--, pot++)
    {
        bg = pot->model;
        if (bg->flags & mask)
        {
            mtxa_from_mtx(g_ord_tbl_stuff->matrices[0]);
            mtxa_translate(&bg->pos);
            mtxa_rotate_z(bg->rotZ);
            mtxa_rotate_y(bg->rotY);
            mtxa_rotate_x(bg->rotX);
            mtxa_scale(&bg->scale);
            if (g_is_sphere_visible_scaled(&model->boundSphereCenter, model->boundSphereRadius, bg->scale.x))
            {
                entry = ord_tbl_get_entry_for_pos(&model->boundSphereCenter);
                node = ord_tbl_alloc_node(sizeof(*node));
                if (node != NULL)
                {
                    node->node.drawFunc = fn_1_9124C;
                    node->work = work;
                    node->data = pot;
                    mtxa_to_mtx(node->mtx);
                    fn_1_CD418(entry, &node->node);
                }
            }
            if (glow)
            {
                mtxa_push();
                mtxa_translate(&model->boundSphereCenter);
                mtxa_rigid_inv_tf_tl(&pos);
                mtxa_pop();
                t = (1.0f / model->boundSphereRadius) * math_sqrt(mathutil_sum_of_sq_2(pos.x, pos.z));
                if (t < 1.0f)
                    t = 0.0f;
                else if (t < 1.25f)
                    t = 4.0f * (t - 1.0f);
                else
                    t = 1.0f;
                if (t > 0.003921569f)
                {
                    entry = fn_1_CCD5C(0);
                    glowNode = ord_tbl_alloc_node(sizeof(*glowNode));
                    if (glowNode != NULL)
                    {
                        glowNode->node.drawFunc = fn_1_92A70;
                        glowNode->work = work;
                        glowNode->pot = pot;
                        glowNode->t = t;
                        fn_1_CD418(entry, &glowNode->node);
                    }
                }
            }
        }
    }
}

/* Pots: draw steam */
void fn_1_9124C(struct OrdTblNode *arg)
{
    struct BgPotOrdNode *node = (struct BgPotOrdNode *)arg;
    struct BgPotPot *pot;
    struct GMAModel *model;
    struct GMAModel *baseModel;

    baseModel = node->work->steamBaseModel;
    model = node->work->steamModel;
    pot = node->data;
    mtxa_from_mtx(node->mtx);
    GXSetZCompLoc_cached(1);
    g_something_with_texture_scroll_2(1);
    avdisp_set_z_mode(1, 3, 0);
    mtxa_push();
    mtxa_from_translate_xyz(0.0f, pot->phase, 0.0f);
    mtxa_scale_xyz(1.0f, pot->unkC, 1.0f);
    fn_1_10A4CC(0, mtxa->mtxA);
    mtxa_pop();
    load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
    avdisp_draw_model_unculled_sort_never(baseModel);
    avdisp_set_z_mode(1, 3, 1);
    g_something_with_texture_scroll_2(0);
    avdisp_set_z_mode(1, 3, 0);
    set_post_mult_color(pot->unk10, pot->unk14, pot->unk18, 1.0f);
    g_stores_doubles2(0.0f, 0.0f, 0.0f, pot->unk1C);
    load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
    avdisp_draw_model_unculled_sort_never(model);
    g_stores_doubles2(0.0f, 0.0f, 0.0f, 0.0f);
    fade_color_base_default();
    avdisp_set_z_mode(1, 3, 1);
    GXSetZCompLoc_cached(0);
}

/* Haze: init */
void fn_1_91410(struct BgPotWork *work)
{
    int n;
    struct BgPotHaze *haze;
    struct BgPotHaze *hazes;

    hazes = work->haze;
    haze = hazes;
    for (n = 4; n > 0; n--, haze++)
    {
        haze->pos.x = RAND_FLOAT();
        haze->pos.y = RAND_FLOAT();
        haze->pos.z = RAND_FLOAT();
        haze->vel.x = 0.0f;
        haze->vel.y = 0.025f;
        haze->vel.z = 0.0f;
        haze->unk18 = 0.2f + 0.2f * RAND_FLOAT();
        haze->unk1C = 0.2f + 0.2f * RAND_FLOAT();
        if (rand() & 1)
            haze->unk18 *= -1.0f;
        if (rand() & 1)
            haze->unk1C *= -1.0f;
        haze->unk24 = -(0.4f + 0.25f * RAND_FLOAT());
        haze->unk28 = -(0.4f + 0.25f * RAND_FLOAT());
        haze->unk30 = 1.0f + 0.2f * RAND_FLOAT();
        haze->unk34 = 1.0f + 0.2f * RAND_FLOAT();
        if (!(modeCtrl.courseFlags & 0x04000000))
        {
            haze->r = 255.0f;
            haze->g = 255.0f;
            haze->a = 255.0f;
            haze->b = 255.0f;
        }
        else
        {
            haze->r = 0.0f;
            haze->g = 0.0f;
            haze->a = 0.0f;
            haze->b = 0.0f;
        }
    }
}

/* Haze: tick */
void fn_1_91714(struct BgPotWork *work)
{
    int n;
    int potN;
    struct BgPotHaze *haze;
    struct BgPotPot *pot;
    s8 *status;
    struct Camera *camera;
    struct StageBgModel *bg;
    struct GMAModel *model;
    float heightFac;
    float vis;
    float len;
    struct RaycastHit hit;
    Vec camPos;
    Vec move;
    Vec tmp;
    Vec wind;
    struct BgPotHaze *hazes;

    hazes = work->haze;
    status = g_poolInfo.cameraPool.statusList;
    camera = cameras;
    haze = hazes;
    for (n = 4; n > 0; n--, haze++, status++, camera++)
    {
        if (*status == 0)
            continue;
        mtxa_from_mtx(camera->unk14C);
        mtxa_rigid_inv_tf_tl(&camPos);
        mtxa_tf_vec(&work->windPos, &wind);
        wind.x *= 0.0020833334f;
        wind.y *= 0.0020833334f;
        wind.z *= 0.0020833334f;
        mtxa_push();
        raycast_stage_down(&camPos, &hit, NULL);
        mtxa_pop();
        vis = -1.0f;
        model = work->steamModel;
        mtxa_push();
        pot = work->pots;
        for (potN = work->potCount; potN > 0; potN--, pot++)
        {
            bg = pot->model;
            mtxa_from_translate(&bg->pos);
            mtxa_rotate_z(bg->rotZ);
            mtxa_rotate_y(bg->rotY);
            mtxa_rotate_x(bg->rotX);
            mtxa_scale(&bg->scale);
            mtxa_tf_point(&model->boundSphereCenter, &tmp);
            len = mathutil_distance_2(tmp.x, tmp.z, camPos.x, camPos.z);
            len *= 1.0f / (model->boundSphereRadius * bg->scale.x);
            if (vis < 0.0f || vis > len)
                vis = len;
        }
        mtxa_pop();
        if (vis < 0.0f)
            vis = 0.0f;
        else if (vis < 1.0f)
            vis = 1.0f;
        else if (vis < 1.5f)
            vis = 1.0f - 2.0f * (vis - 1.0f);
        else
            vis = 0.0f;
        if (!(hit.flags & 1))
        {
            heightFac = 1.0f;
            haze->vel.y += 0.08f * (0.025f - haze->vel.y);
        }
        else
        {
            heightFac = 0.05f * (camPos.y - hit.pos.y);
            if (heightFac > 1.0f)
                heightFac = 1.0f;
            len = 0.025f * heightFac + (0.1f * 0.025f) * (1.0f - heightFac);
            haze->vel.y += 0.08f * (len - haze->vel.y);
        }
        mtxa_tf_vec(&haze->vel, &tmp);
        len = mathutil_vec_len(&tmp);
        tmp.z = 0.0f;
        if (0.0f == tmp.x && 0.0f == tmp.y)
            tmp.y = len;
        else
            vec_set_len(len, &tmp, &move);
        move.x += wind.x;
        move.y += wind.y;
        mtxa_push();
        mtxa_from_mtx(camera->unk17C);
        mtxa_rigid_inv_tf_vec_xyz(0.0f, 0.0f, -1.0f, &tmp);
        mtxa_pop();
        mtxa_tf_vec(&tmp, &tmp);
        move.x += tmp.x;
        move.y += tmp.y;
        haze->pos.x += move.x;
        haze->pos.y += move.y;
        if (vis > 0.0f)
        {
            len = (160.0f + 95.0f * heightFac) * vis;
            haze->r += 0.05f * (len - haze->r);
            haze->g += 0.04f * (len - haze->g);
            len = (96.0f * heightFac) * vis;
            haze->a += 0.02f * (len - haze->a);
            haze->b += 0.03f * (len - haze->b);
        }
        else
        {
            haze->r += 0.05f * -haze->r;
            haze->g += 0.04f * -haze->g;
            haze->a += 0.02f * -haze->a;
            haze->b += 0.03f * -haze->b;
        }
    }
}

/* Haze: disp (enqueue) */
void fn_1_91BC8(struct BgPotWork *work, u32 mask)
{
    struct BgPotHaze *hazes = work->haze;
    struct BgPotHaze *haze = &hazes[g_something_with_camera3];
    struct OrdTblNode *list;
    struct BgPotHazeOrdNode *node;

    if (g_some_render_flag.flags & 0x884)
        return;
    list = fn_1_CCD5C(0);
    if (haze->r > 1.0f || haze->g > 1.0f || haze->a > 1.0f || haze->b > 1.0f)
    {
        node = ord_tbl_alloc_node(sizeof(*node));
        if (node != NULL)
        {
            node->node.drawFunc = fn_1_91CCC;
            node->work = work;
            node->haze = haze;
            fn_1_CD418(list, &node->node);
        }
    }
}

static Vec lbl_1_data_3291C[4] = { { -1, 1, 0 }, { 1, 1, 0 }, { 1, -1, 0 }, { -1, -1, 0 } };

/* Haze: draw fullscreen quad */
void fn_1_91CCC(struct OrdTblNode *arg)
{
    struct BgPotHaze *haze;
    struct BgPotWork *work;
    float x0;
    float x1;
    float y0;
    float y1;
    float invW;
    float invH;
    float indMtx[2][3];
    float proj[7];
    GXColor color;

    work = ((struct BgPotHazeOrdNode *)arg)->work;
    haze = ((struct BgPotHazeOrdNode *)arg)->haze;
    GXLoadTexObj_cached(work->hazeTex, 0);
    GXLoadTexObj_cached(work->hazeTex, 1);
    GXSetBlendMode_cached(1, 4, 1, 0);
    GXSetZMode_cached(0, 7, 0);
    g_something_with_raster_state(0);
    GXSetCullMode_cached(0);
    mtxa_from_translate(&haze->pos);
    GXLoadTexMtxImm(mtxa->mtxA, 0x1E, 1);
    mtxa_from_translate_xyz(haze->unk24 * (haze->pos.x + 0.2f * haze->pos.y), haze->unk28 * (haze->pos.y + 0.2f * haze->pos.x), 0.0f);
    mtxa_scale_xyz(haze->unk30, haze->unk34, 1.0f);
    GXLoadTexMtxImm(mtxa->mtxA, 0x21, 1);
    opti_GXSetChanCtrl(4, 0, 1, 1, 0, 2, 1);
    indMtx[0][0] = 0.0f;
    indMtx[0][1] = haze->unk18;
    indMtx[0][2] = 0.0f;
    indMtx[1][0] = 0.0f;
    indMtx[1][1] = 0.0f;
    indMtx[1][2] = haze->unk1C;
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
    f32_to_u8(&haze->r, &color.r);
    f32_to_u8(&haze->g, &color.g);
    f32_to_u8(&haze->b, &color.b);
    f32_to_u8(&haze->a, &color.a);
    invW = 1.0f / proj[1];
    invH = 1.0f / proj[3];
    x0 = -100.0f * (-1.0f - proj[2]) * invW;
    x1 = -100.0f * (1.0f - proj[2]) * invW;
    y0 = -100.0f * (-1.0f - proj[4]) * invH;
    y1 = -100.0f * (1.0f - proj[4]) * invH;
    GXBegin(0x80, 0, 4);
    GXPosition3f32(x0, y0, -100.0f);
    GXColor4u8(0xFF, 0xFF, 0xFF, (u8)color.r);
    GXTexCoord2f32(0.0f, 0.0f);
    GXPosition3f32(x1, y0, -100.0f);
    GXColor4u8(0xFF, 0xFF, 0xFF, (u8)color.g);
    GXTexCoord2f32(1.0f, 0.0f);
    GXPosition3f32(x1, y1, -100.0f);
    GXColor4u8(0xFF, 0xFF, 0xFF, (u8)color.b);
    GXTexCoord2f32(1.0f, 1.0f);
    GXPosition3f32(x0, y1, -100.0f);
    GXColor4u8(0xFF, 0xFF, 0xFF, (u8)color.a);
    GXTexCoord2f32(0.0f, 1.0f);
}

/* ---- stubs ---- */
void fn_1_92244(struct BgPotPot *pot)
{
    struct StageBgModel *model;
    int i;
    struct BgPotBubble *bubble;
    float t;

    model = pot->model;
    mtxa_from_translate(&model->pos);
    mtxa_rotate_z(model->rotZ);
    mtxa_rotate_y(model->rotY);
    mtxa_rotate_x(model->rotX);
    mtxa_scale(&model->scale);
    mtxa_translate(&model->model->boundSphereCenter);
    pot->unk30 = 0;
    pot->unk32 = 0;
    bubble = pot->bubbles;
    for (i = 16; i > 0; i--, bubble++)
    {
        t = 0.0625f * i;
        bubble->timer = 90.0f + 180.0f * (1.0f - t);
        bubble->size = 179.2f;
        bubble->unk24 = bubble->size * (0.2f + 0.8f * RAND_FLOAT());
        mtxa_tf_point_xyz(0.0f, 512.0f * t, 0.0f, &bubble->pos);
        bubble->vel.x = RAND_FLOAT() - 0.5f;
        bubble->vel.y = RAND_FLOAT() - 0.5f;
        bubble->vel.z = RAND_FLOAT() - 0.5f;
        bubble->rot = (s16)(rand() & 0x7FFF);
        bubble->rotVel = 128.0f * (RAND_FLOAT() - 0.5f);
        bubble->unk28 = 1.0f;
    }
}
void fn_1_924A4(struct BgPotWork *work, struct BgPotPot *pot)
{
    struct StageBgModel *model;
    struct GMAModel *steamModel;
    int i;
    struct BgPotBubble *bubble;
    float radius;
    float t;
    Vec up;
    Vec wind;

    model = pot->model;
    steamModel = work->steamModel;
    radius = steamModel->boundSphereRadius;
    mtxa_from_translate(&model->pos);
    mtxa_rotate_z(model->rotZ);
    mtxa_rotate_y(model->rotY);
    mtxa_rotate_x(model->rotX);
    mtxa_tf_vec(&work->windPos, &wind);
    mtxa_scale(&model->scale);
    mtxa_translate(&steamModel->boundSphereCenter);
    mtxa_tf_vec_xyz(0.0f, 1.0f, 0.0f, &up);
    vec_set_len(0.016f, &up, &up);
    bubble = pot->bubbles;
    for (i = 16; i > 0; i--, bubble++)
    {
        if (bubble->timer == 0)
        {
            bubble->timer = 180.0f * (1.0f + RAND_FLOAT());
            bubble->size = 1.4f * radius;
            bubble->unk24 = bubble->size * (0.2f + 0.8f * RAND_FLOAT());
            mtxa_push();
            mtxa_rotate_y((s16)(rand() & 0x7FFF));
            mtxa_tf_point_xyz(radius * (0.5f * (RAND_FLOAT() - 0.5f)), 0.0f, 0.0f, &bubble->pos);
            mtxa_pop();
            bubble->vel.x = RAND_FLOAT() - 0.5f;
            bubble->vel.y = RAND_FLOAT() - 0.5f;
            bubble->vel.z = RAND_FLOAT() - 0.5f;
            bubble->vel.x += up.x * (5.0f + 5.0f * RAND_FLOAT());
            bubble->vel.y += up.y * (5.0f + 5.0f * RAND_FLOAT());
            bubble->vel.z += up.z * (5.0f + 5.0f * RAND_FLOAT());
            bubble->rot = (s16)(rand() & 0x7FFF);
            bubble->rotVel = 512.0f * (RAND_FLOAT() - 0.5f);
            bubble->unk28 = 0.0f;
        }
        bubble->vel.x += up.x;
        bubble->vel.y += up.y;
        bubble->vel.z += up.z;
        bubble->vel.x += 0.005f * (wind.x - bubble->vel.x);
        bubble->vel.y += 0.005f * (wind.y - bubble->vel.y);
        bubble->vel.z += 0.005f * (wind.z - bubble->vel.z);
        bubble->pos.x += bubble->vel.x;
        bubble->pos.y += bubble->vel.y;
        bubble->pos.z += bubble->vel.z;
        bubble->rotVel = 0.998f * bubble->rotVel;
        bubble->rot += bubble->rotVel;
        if ((float)bubble->timer > 24.0f)
            bubble->size += 0.01f * (bubble->unk24 - bubble->size);
        else
            bubble->size = math_sqrt(1.03f * (bubble->size * bubble->size));
        if ((float)bubble->timer > 30.0f)
            bubble->unk28 += 0.05f * (1.0f - bubble->unk28);
        else
        {
            t = 1.0f - 1.0f / (float)(bubble->timer + 1);
            bubble->unk28 *= t;
        }
        bubble->timer--;
    }
}
void fn_1_92A70(struct OrdTblNode *arg)
{
    struct BgPotGlowOrdNode *node = (struct BgPotGlowOrdNode *)arg;
    struct BgPotWork *work = node->work;
    struct BgPotPot *pot = node->pot;
    int i;
    struct BgPotBubble *bubble;
    float t = 0.25f * node->t;
    float size;
    Vec v3;
    Vec v2;
    Vec v1;
    Vec v0;
    GXColor color;

    fn_1_92D0C(work);
    mtxa_from_mtx(g_ord_tbl_stuff->matrices[0]);
    bubble = pot->bubbles;
    for (i = 16; i > 0; i--, bubble++)
    {
        if (bubble->timer == 0)
            continue;
        if (bubble->unk28 <= 0.0f)
            continue;
        size = bubble->size;
        if (!g_is_sphere_visible(&bubble->pos, size))
            continue;
        mtxa_push();
        mtxa_translate(&bubble->pos);
        mtxa_sq_from_identity();
        mtxa_scale_s(size);
        mtxa_rotate_z(bubble->rot);
        mtxa_tf_point(&lbl_1_data_3291C[0], &v0);
        mtxa_tf_point(&lbl_1_data_3291C[1], &v1);
        mtxa_tf_point(&lbl_1_data_3291C[2], &v2);
        mtxa_tf_point(&lbl_1_data_3291C[3], &v3);
        mtxa_pop();
        /* PC HFR: anonymous identical quads, paired per bubble */
        HFR_TAG(HFR_K_BG, 0x504F0000 | (u32)(bubble - pot->bubbles));
        color.r = 255.0f * (bubble->unk28 * t);
        color.g = color.r;
        color.b = color.r;
        color.a = 0xFF;
        GXSetTevKColor_cached(0, color);
        fn_1_92E70();
        GXBegin(0x80, 0, 4);
        GXPosition3f32(v0.x, v0.y, v0.z);
        GXTexCoord2f32(0.0f, 0.0f);
        GXPosition3f32(v1.x, v1.y, v1.z);
        GXTexCoord2f32(1.0f, 0.0f);
        GXPosition3f32(v2.x, v2.y, v2.z);
        GXTexCoord2f32(1.0f, 1.0f);
        GXPosition3f32(v3.x, v3.y, v3.z);
        GXTexCoord2f32(0.0f, 1.0f);
        HFR_UNTAG();
    }
}

void fn_1_92D0C(struct BgPotWork *work)
{
    GXLoadTexObj_cached(work->glowTex[0], 0);
    GXSetBlendMode_cached(1, 1, 1, 0);
    GXSetZMode_cached(1, 3, 0);
    g_something_with_raster_state(0);
    GXSetCullMode_cached(0);
    g_GXSetTevIndirect_zero_if_different(0);
    tex_coord_gen(0, 1, 4, 0x3C);
    g_GXSetTevOrder_wrapper(0, 0, 0, 0xFF);
    GXSetTevKColorSel_cached(0, 0xC);
    GXSetTevKAlphaSel_cached(0, 0x1C);
    GXSetTevSwapMode_cached(0, 0, 1);
    GXSetTevColorIn_cached(0, 0xF, 0xE, 8, 0xF);
    GXSetTevColorOp_cached(0, 0, 0, 0, 1, 0);
    GXSetTevAlphaIn_cached(0, 7, 4, 6, 7);
    GXSetTevAlphaOp_cached(0, 0, 0, 0, 1, 0);
    GXSetNumTevStages_cached(1);
    GXSetNumTexGens_cached(1);
    GXSetNumIndStages_cached(0);
    GXSetNumChans_cached(0);
    g_something_with_GX_vtx_desc(0x2200);
    mtxa_from_identity();
    GXLoadPosMtxImm(mtxa->mtxA, 0);
}

void fn_1_92E70(void)
{
    GXBegin(0x90, 0, 8);
    GXPosition3f32(0.0f, 0.0f, -10.0f);
    GXTexCoord2f32(0.0f, 0.0f);
    GXPosition3f32(0.0f, 0.0f, -10.0f);
    GXTexCoord2f32(0.0f, 0.0f);
    GXPosition3f32(0.0f, 0.0f, -10.0f);
    GXTexCoord2f32(0.0f, 0.0f);
    GXPosition3f32(0.0f, 0.0f, -10.0f);
    GXTexCoord2f32(0.0f, 0.0f);
    GXPosition3f32(0.0f, 0.0f, -10.0f);
    GXTexCoord2f32(0.0f, 0.0f);
    GXPosition3f32(0.0f, 0.0f, -10.0f);
    GXTexCoord2f32(0.0f, 0.0f);
    GXPosition3f32(0.0f, 0.0f, -10.0f);
    GXTexCoord2f32(0.0f, 0.0f);
    GXPosition3f32(0.0f, 0.0f, -10.0f);
    GXTexCoord2f32(0.0f, 0.0f);
}
void fn_1_931BC(struct BgPotWork *work)
{
    int n;
    int j;
    struct BgPotVent *vent;
    struct BgPotVentCloud *cloud;

    vent = work->vents;
    n = work->ventCount;
    for (; n > 0; n--, vent++)
    {
        vent->unk8 = RAND_FLOAT();
        cloud = vent->clouds;
        for (j = 4; j > 0; j--, cloud++)
        {
            cloud->unkC = (float)j / 4.0f;
            cloud->unk10 = 0.055555556f * (1.0f + 0.0f * RAND_FLOAT());
            cloud->unk0 = 1.0f + 0.5f * RAND_FLOAT();
            cloud->unk4 = 0.0f;
            cloud->unk8 = 0.75f + 0.5f * RAND_FLOAT();
            cloud->unk14 = (s16)(rand() & 0x7FFF);
            cloud->unk16 = (s16)((rand() & 0xFFF) - 0x800);
        }
    }
}
void fn_1_9333C(struct BgPotWork *work)
{
    int n;
    int j;
    struct BgPotVent *vent;
    struct BgPotVentCloud *cloud;
    struct Effect effect;

    memset(&effect, 0, sizeof(effect));
    vent = work->vents;
    n = work->ventCount;
    for (; n > 0; n--, vent++)
    {
        vent->unk4 = 0.95f + 0.1f * RAND_FLOAT();
        cloud = vent->clouds;
        for (j = 4; j > 0; j--, cloud++)
        {
            cloud->unkC += cloud->unk10;
            cloud->unk4 += 0.5f * (cloud->unk0 - cloud->unk4);
            if (cloud->unkC > 1.0f)
            {
                while (cloud->unkC > 1.0f)
                    cloud->unkC -= 1.0f;
                cloud->unk0 = 1.0f + 0.5f * RAND_FLOAT();
                cloud->unk4 = 0.0f;
                cloud->unk8 = 0.75f + 0.5f * RAND_FLOAT();
                cloud->unk14 = (s16)(rand() & 0x7FFF);
                cloud->unk16 = 3072.0f * (RAND_FLOAT() - 0.5f);
            }
        }
        vent->unk8 += 1.0 / 30.0;
        if (vent->unk8 > 1.0f)
        {
            effect.type = 0x16;
            effect.pos = vent->model->pos;
            effect.unk18 = 0.85f;
            effect.unk1C = 0.48f;
            effect.unk20 = 0.3f;
            effect.model = work->unkE04;
            while (vent->unk8 > 1.0f)
            {
                effect.model = work->unkE04;
                effect.timer = 120.0f * (0.25f + RAND_FLOAT());
                effect.scale.x = 0.6f * vent->model->scale.x * (1.0f + RAND_FLOAT());
                effect.vel.x = vent->model->scale.x * (RAND_FLOAT() - 0.5f);
                effect.vel.y = vent->model->scale.y * (0.5f + RAND_FLOAT());
                effect.vel.z = vent->model->scale.z * (RAND_FLOAT() - 0.5f);
                effect.rotZ = (s16)(rand() & 0x7FFF);
                effect.unk56 = 2048.0f * (RAND_FLOAT() - 0.5f);
                spawn_effect(&effect);
                vent->unk8 -= 1.0f;
            }
        }
    }
}
void fn_1_93778(struct BgPotWork *work, u32 mask)
{
    int n;
    struct BgPotVent *vent;
    struct StageBgModel *model;
    struct GMAModel *gma;
    struct OrdTblNode *entry;
    struct BgPotVentOrdNode *node;

    vent = work->vents;
    n = work->ventCount;
    for (; n > 0; n--, vent++)
    {
        model = vent->model;
        if (model->flags & mask)
        {
            model->flags &= ~0x10000;
            mtxa_from_mtx(g_ord_tbl_stuff->matrices[0]);
            mtxa_translate(&model->pos);
            mtxa_rotate_z(model->rotZ);
            mtxa_rotate_y(model->rotY);
            mtxa_rotate_x(model->rotX);
            mtxa_scale(&model->scale);
            gma = model->model;
            if (g_is_sphere_visible_scaled(&gma->boundSphereCenter, gma->boundSphereRadius, model->scale.x))
            {
                entry = ord_tbl_get_entry_for_pos(&gma->boundSphereCenter);
                node = ord_tbl_alloc_node(sizeof(*node));
                if (node != NULL)
                {
                    node->node.drawFunc = fn_1_9387C;
                    mtxa_to_mtx(node->mtx);
                    node->vent = vent;
                    fn_1_CD418(entry, &node->node);
                }
            }
        }
    }
}

void fn_1_9387C(struct OrdTblNode *arg)
{
    struct BgPotVentOrdNode *node = (struct BgPotVentOrdNode *)arg;
    int i;
    struct BgPotVent *vent;
    struct BgPotVentCloud *cloud;
    struct GMAModel *model;
    float t;

    vent = node->vent;
    mtxa_from_mtx(node->mtx);
    avdisp_set_z_mode(1, 3, 0);
    model = vent->model->model;
    cloud = vent->clouds;
    for (i = 4; i > 0; i--, cloud++)
    {
        t = cloud->unkC;
        if (t <= 0.0f)
            continue;
        mtxa_push();
        mtxa_rotate_z(cloud->unk16);
        mtxa_rotate_y(cloud->unk14);
        mtxa_scale_xyz(cloud->unk4 * (1.0f - t), cloud->unk8 * t, 1.0f);
        if (t > 0.5f)
            avdisp_set_alpha(1.0f - 2.0f * (t - 0.5f));
        g_smth_with_gpu_and_locked_cache_mtx();
        avdisp_draw_model_unculled_sort_never(model);
        mtxa_pop();
    }
    avdisp_set_z_mode(1, 3, 1);
}
void fn_1_939A0(struct RenderEffect *rendEfc)
{
    rendEfc->enableFlags = 1;
    rendEfc->work = NULL;
}

void fn_1_939B4(struct RenderEffect *rendEfc) {}

void fn_1_939B8(struct RenderEffect *rendEfc) {}

void fn_1_939BC(u32 a, struct RenderEffect *rendEfc)
{
    fn_1_939E0(rendEfc);
}

void fn_1_939E0(struct RenderEffect *rendEfc)
{
    struct BgPotCloudTex *ct;
    int w2;
    int h2;
    int w;
    int h;
    int stage;
    int texGen;
    int texMtx;
    int texMap;
    int n;
    GXTexObj **texp;
    struct BgPotCloudLayer *layer;
    float x0;
    float x1;
    float y0;
    float y1;
    float invW;
    float invH;
    float vp[6];
    float proj[7];
    u32 scLeft;
    u32 scTop;
    u32 scWidth;
    u32 scHeight;

    ct = ((struct BgPotWork *)world_theme.work)->cloudTex;
    w = 0x80;
    h = 0x80;
    w2 = w << 1;
    h2 = h << 1;
    GXGetViewportv(vp);
    GXGetScissor(&scLeft, &scTop, &scWidth, &scHeight);
    GXSetViewport(0.0f, 0.0f, w2, h2, 0.0f, 1.0f);
    GXSetScissor_cached(0, 0, w2, h2);
    GXLoadTexObj_cached(ct->layerTex[0], 0);
    GXSetBlendMode_cached(0, 1, 0, 0);
    GXSetZMode_cached(0, 7, 0);
    g_something_with_raster_state(0);
    GXSetCullMode_cached(0);
    stage = 0;
    texGen = 0;
    texMtx = 0x1E;
    texMap = 0;
    texp = ct->layerTex;
    layer = ct->layers;
    n = ct->layerCount;
    for (; n > 0; n--, texp++, layer++)
    {
        GXLoadTexObj_cached(*texp, texMap);
        mtxa_from_translate_xyz(layer->pos.x + layer->vel.x, layer->pos.y + layer->vel.y, layer->pos.z + layer->vel.z);
        mtxa_scale_xyz(layer->scaleX, layer->scaleY, 1.0f);
        GXLoadTexMtxImm(mtxa->mtxA, texMtx, 1);
        g_GXSetTevIndirect_zero_if_different(stage);
        tex_coord_gen(texGen, 1, 4, texMtx);
        g_GXSetTevOrder_wrapper(stage, texGen, texMap, 0xFF);
        if (stage == 0)
        {
            GXSetTevColorIn_cached(stage, 0xF, 0xF, 0xF, 8);
            GXSetTevColorOp_cached(stage, 0, 0, 0, 1, 0);
        }
        else
        {
            GXSetTevColorIn_cached(stage, 0xF, 8, 0, 0xF);
            GXSetTevColorOp_cached(stage, 0, 0, 0, 1, 0);
        }
        GXSetTevAlphaIn_cached(stage, 7, 7, 7, 4);
        GXSetTevAlphaOp_cached(stage, 0, 0, 0, 1, 0);
        stage++;
        texGen++;
        texMtx += 3;
        texMap++;
    }
    GXSetNumTevStages_cached((u8)stage);
    GXSetNumTexGens_cached((u8)texGen);
    GXSetNumIndStages_cached(0);
    GXSetNumChans_cached(0);
    g_something_with_GX_vtx_desc(0x2200);
    mtxa_from_identity();
    GXLoadPosMtxImm(mtxa->mtxA, 0);
    GXGetProjectionv(proj);
    invW = 1.0f / proj[1];
    invH = 1.0f / proj[3];
    x0 = -100.0f * (-1.0f - proj[2]) * invW;
    x1 = -100.0f * (1.0f - proj[2]) * invW;
    y0 = -100.0f * (-1.0f - proj[4]) * invH;
    y1 = -100.0f * (1.0f - proj[4]) * invH;
    GXBegin(0x80, 0, 4);
    GXPosition3f32(x0, y0, -100.0f);
    GXTexCoord2f32(-0.5f, -0.5f);
    GXPosition3f32(x1, y0, -100.0f);
    GXTexCoord2f32(0.5f, -0.5f);
    GXPosition3f32(x1, y1, -100.0f);
    GXTexCoord2f32(0.5f, 0.5f);
    GXPosition3f32(x0, y1, -100.0f);
    GXTexCoord2f32(-0.5f, 0.5f);
    GXSetTexCopySrc_cached(0, 0, w2, h2);
    GXSetTexCopyDst_cached(w, h, 4, 1);
    GXCopyTex(ct->texData, 1);
    ct->rendered = 1;
    GXInvalidateTexAll();
    GXSetViewport(vp[0], vp[1], vp[2], vp[3], vp[4], vp[5]);
    GXSetScissor_cached(scLeft, scTop, scWidth, scHeight);
}
void g_something_with_stage_heap_and_pot_theme(struct BgPotWork *work)
{
    int n;
    int j;
    struct BgPotCloudTex *ct;
    struct BgPotCloudLayer *layer;

    ct = work->cloudTex;
    for (n = 1; n > 0; n--, ct++)
    {
        ct->rendered = 0;
        ct->texData = alloc_from_heap_or_panic(stage_heap, GXGetTexBufferSize(0x80, 0x80, 4, 0, 0), "bg_pot.c", 0xA2D);
        GXInitTexObj(&ct->tex, ct->texData, 0x80, 0x80, 4, 1, 1, 0);
        layer = ct->layers;
        j = ct->layerCount;
        for (; j > 0; j--, layer++)
        {
            layer->pos.x = RAND_FLOAT() - 0.5f;
            layer->pos.y = RAND_FLOAT() - 0.5f;
            layer->vel.x = 0.2f * (RAND_FLOAT() - 0.5f);
            layer->vel.y = 0.2f * (RAND_FLOAT() - 0.5f);
            layer->scaleX = 1.0f;
            layer->scaleY = 1.0f;
            if (rand() & 1)
                layer->scaleX = -1.0f;
            if (rand() & 1)
                layer->scaleY = -1.0f;
            layer->unk20 = 0.2f * RAND_FLOAT();
            layer->unk24 = 0.2f * (RAND_FLOAT() - 0.5f);
            layer->unk28 = 0.01f * (1.0f + 4.0f * RAND_FLOAT());
            layer->rot[0] = (s16)(rand() & 0x7FFF);
            layer->rot[1] = (s16)(rand() & 0x7FFF);
            layer->rot[2] = (s16)(rand() & 0x7FFF);
            layer->rotVel[0] = 2184.5334f * (RAND_FLOAT() - 0.5f);
            layer->rotVel[1] = 2184.5334f * (RAND_FLOAT() - 0.5f);
            layer->rotVel[2] = 2184.5334f * (RAND_FLOAT() - 0.5f);
            layer->unk38 = 0.001f * (1.0f + RAND_FLOAT());
            layer->unk3C = 0.001f * (1.0f + RAND_FLOAT());
            layer->unk40 = 0.001f * (1.0f + RAND_FLOAT());
        }
    }
}
void fn_1_94324(struct BgPotWork *work)
{
    int n;
    struct BgPotCloudTex *ct;

    ct = work->cloudTex;
    for (n = 1; n > 0; n--, ct++)
    {
        ct->rendered = 0;
        if (ct->texData != NULL)
        {
            gp_wait_wrapper();
            OSFreeToHeap(stage_heap, ct->texData);
            ct->texData = NULL;
        }
    }
}
void fn_1_943A0(struct BgPotWork *work)
{
    int n;
    int j;
    struct BgPotCloudTex *ct;
    struct BgPotCloudLayer *layer;
    float windLen;
    float speed;
    float dx;
    float dy;
    float dz;
    Vec wind;

    wind = work->windPos;
    windLen = mathutil_vec_len(&wind);
    ct = work->cloudTex;
    for (n = 1; n > 0; n--, ct++)
    {
        layer = ct->layers;
        j = ct->layerCount;
        for (; j > 0; j--, layer++)
        {
            layer->unk24 += layer->unk28 * (0.05f * (windLen - layer->unk20) - layer->unk24);
            layer->unk20 += layer->unk24;
            speed = layer->unk20;
            layer->rot[0] += (s16)((float)layer->rotVel[0] * speed);
            layer->rot[1] += (s16)((float)layer->rotVel[1] * speed);
            layer->rot[2] += (s16)((float)layer->rotVel[2] * speed);
            dx = speed * layer->unk38 * math_sin(layer->rot[0]);
            dy = speed * layer->unk3C * math_sin(layer->rot[1]);
            dz = speed * layer->unk40 * math_sin(layer->rot[2]);
            layer->vel.x += dx;
            layer->vel.y += dy;
            layer->vel.z += dz;
        }
    }
}
void fn_1_94608(void)
{
    struct BgPotWork *work = world_theme.work;
    int n;
    struct BgPotCloudTex *ct;
    Mtx invView;

    mtxa_from_mtx(g_ord_tbl_stuff->matrices[0]);
    mtxa_rigid_invert();
    mtxa_to_mtx(invView);
    ct = work->cloudTex;
    for (n = 1; n > 0; n--, ct++)
    {
        mtxa_from_identity();
        mtxa_rotate_x(0x3800);
        mtxa_scale_s(0.0015f);
        mtxa_mult_right(invView);
        mtxa_to_mtx(ct->mtx);
    }
}
void fn_1_946A4(struct BallEnvArg *arg)
{
    struct BgPotCloudTex *ct = ((struct BgPotWork *)world_theme.work)->cloudTex;
    struct TevStageInfo tev;

    if (ct->rendered == 0)
        return;
    tev = arg->tev;
    if (arg->unk0 == 0
     && tev.tevStage == ct->tev.tevStage
     && tev.texMtxId == ct->tev.texMtxId
     && tev.texMapId == ct->tev.texMapId)
    {
        tev.tevStage += 1;
        tev.texCoordId += 1;
        tev.texMtxId += 3;
        arg->tev = tev;
        return;
    }
    ct->tev = tev;
    GXLoadTexObj_cached(&ct->tex, arg->tev.texMapId);
    mtxa_push();
    mtxa_mult_left(ct->mtx);
    GXLoadTexMtxImm(mtxa->mtxA, tev.texMtxId, 0);
    mtxa_pop();
    tex_coord_gen(tev.texCoordId, 1, 0, tev.texMtxId);
    g_GXSetTevOrder_wrapper(tev.tevStage, tev.texCoordId, tev.texMapId, 0xFF);
    g_GXSetTevIndirect_zero_if_different(tev.tevStage);
    GXSetTevColorIn_cached(tev.tevStage, 0xF, 8, 0, 0xF);
    GXSetTevColorOp_cached(tev.tevStage, 0, 0, 0, 1, 2);
    GXSetTevAlphaIn_cached(tev.tevStage, 7, 7, 7, 0);
    GXSetTevAlphaOp_cached(tev.tevStage, 0, 0, 0, 1, 0);
    tev.tevStage += 1;
    tev.texCoordId += 1;
    tev.texMtxId += 3;
    arg->tev = tev;
}
int fn_1_949AC(int id, struct GMAModelEntry *entry)
{
    struct BgPotWork *work = world_theme.work;
    struct BgPotCloudTex *ct;

    switch (id)
    {
    case 0:
        work->flareModel = entry->model;
        break;
    case 1:
        work->steamBaseModel = entry->model;
        break;
    case 2:
        work->steamModel = entry->model;
        break;
    case 3:
        work->hazeTex = entry->model->texObjs;
        break;
    case 4:
        work->unkE04 = entry->model;
        break;
    case 5:
        ct = work->cloudTex;
        if (ct->layerCount < 4)
        {
            ct->layerTex[ct->layerCount] = entry->model->texObjs;
            ct->layerCount++;
        }
        break;
    case 6:
        if (work->glowTexCount < 2)
        {
            work->glowTex[work->glowTexCount] = entry->model->texObjs;
            work->glowTexCount++;
        }
        break;
    }
    return 1;
}
int fn_1_94A88(int id, struct StageBgModel *model)
{
    struct BgPotWork *work = world_theme.work;

    switch (id)
    {
    case 0:
        work->flare.sun = model;
        break;
    case 1:
        if (model->model->flags & 8)
            work->leaves->model = model;
        break;
    case 2:
        if (work->potCount < 2)
        {
            work->pots[work->potCount].model = model;
            work->potCount++;
        }
        break;
    case 3:
        if (work->ventCount < 8)
        {
            work->vents[work->ventCount].model = model;
            work->ventCount++;
        }
        break;
    case 4:
    case 5:
    case 6:
    case 7:
    case 8:
    case 9:
    case 10:
    case 11:
        model->flags |= 0x1000000;
        break;
    }
    return 1;
}
