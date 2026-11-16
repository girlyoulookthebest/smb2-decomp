/* Second water background: an ocean surface seen from below, light columns, bubbles,
 * caustics and fish. */

#include "types.h"
#include "avdisp.h"
#include "background.h"
#include "ball.h"
#include "camera.h"
#include "effect.h"
#include "frustum.h"
#include "gx.h"
#include "libc.h"
#include "mathutil.h"
#include "pc_hfr.h"
#include "mode.h"
#include "nl2ngc.h"
#include "pool.h"
#include "rand.h"
#include "stage.h"
#include "stcoli.h"
void *OSFreeToHeap(void *heap, void *ptr);
void GXInitTexObj(GXTexObj *obj, void *image, u16 width, u16 height, int format, int wrapS, int wrapT, u8 mipmap);
u16 GXGetTexObjWidth(GXTexObj *obj);
u16 GXGetTexObjHeight(GXTexObj *obj);
void *GXGetTexObjData(GXTexObj *obj);
int GXGetTexObjFmt(GXTexObj *obj);
int GXGetTexObjWrapS(GXTexObj *obj);
int GXGetTexObjWrapT(GXTexObj *obj);
void GXGetProjectionv(float *proj);
void GXLoadTexObj_cached(GXTexObj *obj, int map);
void GXLoadTexMtxImm(Mtx m, int id, int type);
void GXLoadPosMtxImm(Mtx m, int id);
void GXSetNumChans_cached(u8 n);
void GXSetNumTexGens_cached(u8 n);
void GXSetNumTevStages_cached(u8 n);
void GXSetNumIndStages_cached(u8 n);
void GXSetCullMode_cached(int mode);
void GXSetTevKColor_cached(int id, GXColor color);
void GXSetTevKColorSel_cached(int stage, int sel);
void GXSetTevKAlphaSel_cached(int stage, int sel);
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
void *g_yet_another_unk_draw_func(void *func);
void g_smth_with_gpu_and_locked_cache_mtx(void);
void g_smth_with_bg_model_names(struct BgModelSearch *search, int (*callback)(int, struct GMAModelEntry *));
struct OrdTblNode *ord_tbl_get_entry_for_pos(Vec *pos);
struct OrdTblNode *fn_1_CCC18(float z);
void *ord_tbl_alloc_node(u32 size);
void fn_1_CD418(struct OrdTblNode *list, struct OrdTblNode *node);
void mtxa_from_translate_xyz(float x, float y, float z);
void mtxa_rigid_inv_tf_vec_xyz(float x, float y, float z, Vec *dst);
void g_set_something3(int);
void fn_1_E2F1C(void);
void GXSetTevSwapMode_cached(int stage, int ras, int tex);
void *alloc_from_heap_or_panic(void *heap, u32 size, char *file, int line);
extern void *stage_heap;
extern void *bg_heap;
extern struct { u32 flags; } g_some_render_flag;


struct BgWat2Bubble
{
    /*0x00*/ u8 state;
    /*0x02*/ s16 timer;
    /*0x04*/ Vec pos;
    /*0x10*/ Vec vel;
    /*0x1C*/ float size;
    /*0x20*/ float maxSize;
    /*0x24*/ float alpha;
    /*0x28*/ float maxAlpha;
    /*0x2C*/ float drag;
};  /* size = 0x30 */

struct BgWat2Bubbles
{
    /*0x0000*/ s32 enabled;
    /*0x0004*/ float density;
    /*0x0008*/ u32 count;
    /*0x000C*/ struct BgWat2Bubble bubbles[512];
    /*0x600C*/ u32 ambientCount;
    /*0x6010*/ struct BgWat2Bubble ambient[128];
};  /* size = 0x7810 */

struct BgWat2Poko
{
    /*0x00*/ struct StageBgModel *model;
    /*0x04*/ float spawnAccum;
    /*0x08*/ s16 angle;
    /*0x0A*/ s16 angleVel;
    /*0x0C*/ float texX;
    /*0x10*/ float vel;
    /*0x14*/ float strength;
    /*0x18*/ float texY;
};  /* size = 0x1C */

struct BgWat2Caustics
{
    /*0x000*/ float unk0;
    /*0x004*/ float amp;
    /*0x008*/ float ampVel;
    /*0x00C*/ S16Vec unkC;
    /*0x012*/ S16Vec unk12;
    /*0x018*/ float unk18;
    /*0x01C*/ float unk1C;
    /*0x020*/ S16Vec unk20;
    /*0x026*/ S16Vec unk26;
    /*0x02C*/ S16Vec unk2C;
    /*0x032*/ S16Vec unk32;
    /*0x038*/ S16Vec unk38;
    /*0x03E*/ S16Vec unk3E;
    /*0x044*/ GXTexObj *tex44;
    /*0x048*/ GXTexObj *tex48;
    /*0x04C*/ GXTexObj *tex4C;
    /*0x050*/ Mtx unk50;
    /*0x080*/ Mtx unk80;
    /*0x0B0*/ Mtx unkB0;
    /*0x0E0*/ Mtx unkE0;
    /*0x110*/ Mtx unk110;
    /*0x140*/ Mtx unk140;
    /*0x170*/ Mtx unk170;
    /*0x1A0*/ struct TevStageInfo tev;
};  /* size = 0x1CC */

/* One bone of a simulated WA2_SHIP_HO sail */
struct BgWat2SailSeg
{
    /*0x00*/ Vec pos;
    /*0x0C*/ Vec dir;
    /*0x18*/ Vec side;
    /*0x24*/ Vec vel;
    /*0x30*/ Mtx mtx;
    /*0x60*/ float len;
    /*0x64*/ struct BgWat2SailSeg *parent;
};  /* size = 0x68 */

/* One bone of a simulated WA2_ROPE */
struct BgWat2RopeSeg
{
    /*0x00*/ Vec pos;
    /*0x0C*/ Vec vel;
    /*0x18*/ Vec dir;
    /*0x24*/ Vec rest;
    /*0x30*/ Mtx mtx;
    /*0x60*/ float twist;
    /*0x64*/ float twistVel;
    /*0x68*/ u32 flags;
    /*0x6C*/ Vec *anchor;
    /*0x70*/ float unk70;
    /*0x74*/ float unk74;
    /*0x78*/ struct BgWat2RopeSeg *prev;
    /*0x7C*/ float lenPrev;
    /*0x80*/ struct BgWat2RopeSeg *next;
    /*0x84*/ float lenNext;
};  /* size = 0x88 */

struct BgWat2Cloth
{
    /*0x00*/ struct StageBgModel *model;
    /*0x04*/ int count;
    /*0x08*/ void *segs;
};  /* size = 0xC */

struct BgWat2Work
{
    /*0x000*/ s32 initialized;
    /*0x004*/ u32 unk4;
    /*0x008*/ struct GMAModel *surfaceModel;
    /*0x00C*/ GXTexObj *surfaceTex;
    /*0x010*/ GXTexObj *surfaceIndTex;
    /*0x014*/ struct StageBgModel *surface;
    /*0x018*/ s32 pokoCount;
    /*0x01C*/ struct BgWat2Poko pokos[32];
    /*0x39C*/ struct GMAModel *pokoLight;
    /*0x3A0*/ struct GMAModel *bubbleModel;
    /*0x3A4*/ GXTexObj *bubbleTex;
    /*0x3A8*/ GXTexObj *bubbleMaskTex;
    /*0x3AC*/ struct BgWat2Bubbles *bubbles;
    /*0x3B0*/ struct BgWat2Caustics caustics;
    /*0x57C*/ struct StageBgModel *ship;
    /*0x580*/ Vec current;
    /*0x58C*/ float gustMin;
    /*0x590*/ float gustMax;
    /*0x594*/ s16 gustAngle;
    /*0x596*/ s16 gustAngleVel;
    /*0x598*/ float windX;
    /*0x59C*/ float windY;
    /*0x5A0*/ s16 windAngleX;
    /*0x5A2*/ s16 windAngleY;
    /*0x5A4*/ s16 windAngleXVel;
    /*0x5A6*/ s16 windAngleYVel;
    /*0x5A8*/ s32 sailCount;
    /*0x5AC*/ struct BgWat2Cloth sails[4];
    /*0x5DC*/ s16 ropeMax;
    /*0x5DE*/ s16 ropeCount;
    /*0x5E0*/ struct BgWat2Cloth *ropes;
};

struct BgWat2BubbleNode
{
    /*0x00*/ struct OrdTblNode node;
    /*0x08*/ void *data;
};

struct BgWat2SailNode
{
    /*0x00*/ struct OrdTblNode node;
    /*0x08*/ int lightGroup;
    /*0x0C*/ Mtx mtx;
    /*0x3C*/ struct StageBgModel *model;
    /*0x40*/ struct BgWat2Cloth *sail;
};  /* size = 0x44 */

void fn_1_83518(struct BgWat2Work *work);
void fn_1_8351C(struct BgWat2Work *work);
void fn_1_83520(struct BgWat2Work *work);
void fn_1_835D0(struct BallEnvArg *arg);
void fn_1_839E0(struct BgWat2Work *work);
void fn_1_83B84(struct BgWat2Work *work);
void fn_1_84124(struct BgWat2Work *work);
void fn_1_84508(struct BgWat2Work *work);
void fn_1_84964(struct BgWat2Work *work);
void fn_1_849B4(struct BgWat2Work *work);
void fn_1_84BD8(struct BgWat2Bubbles *b);
void fn_1_85284(struct BgWat2Work *work);
void fn_1_8538C(struct BgWat2Bubbles *b);
void fn_1_853FC(struct BgWat2BubbleNode *node);
void fn_1_85B20(struct BgWat2Work *work, struct BgWat2Bubble *src);
void fn_1_85B78(struct BgWat2BubbleNode *node);
void fn_1_86158(struct BgWat2Work *work);
void fn_1_86388(void);

static inline void tex_coord_gen(int dst, int func, int src, int mtx)
{
    GXSetTexCoordGen2_cached(dst, func, src, mtx, 0, 0x7D);
}

u32 lbl_1_data_31CA8[] =
{
    0xFE000102, 0x03040506, 0x0708090A, 0x0B0CFE0D,
    0x0E0FFF00, 0x00010203, 0x04050607, 0x08090A0B,
    0x0C0D0E0F, 0xFF000000, 0x31000000, 0x00000000,
    0x3800B400, 0x00010000, 0x01000000, 0x00020000,
    0x10004C00, 0x00030000, 0x01000000, 0x00040000,
    0x18000000, 0x00090000, 0x40000000, 0x00000000,
    0x01000000, 0x000B0000, 0x18000000, 0x000C0000,
    0x18000000, 0x00050000, 0x40000000, 0x00000000,
    0x01000000, 0x00070000, 0x18000000, 0x00080000,
    0x3800B400, 0x000D0000, 0x40000000, 0x00000000,
    0x18000000, 0x00140000, 0x40000000, 0x00000000,
    0x01000000, 0x00160000, 0x18000000, 0x00170000,
    0x18000000, 0x00180000, 0x18000000, 0x000F0000,
    0x40000000, 0x00000000, 0x01000000, 0x00110000,
    0x18000000, 0x00120000, 0x18000000, 0x00130000,
    0x18000000, 0x00000000, 0x18000000, 0x00010000,
    0x18000000, 0x00020000, 0x18000000, 0x00030000,
    0x18000000, 0x00040000, 0x18000000, 0x00050000,
    0x18000000, 0x00060000, 0x18000000, 0x00070000,
    0x18000000, 0x00080000, 0x18000000, 0x00090000,
    0x18000000, 0x000A0000, 0x18000000, 0x000B0000,
    0x18000000, 0x000C0000, 0x18000000, 0x000D0000,
    0x18000000, 0x000E0000, 0x18000000, 0x000F0000,
};

static struct BgModelSearch lbl_1_data_31E6C[] =
{
    {BG_MDL_CMP_FULL, "WA2_KAIMEN"},
    {BG_MDL_CMP_FULL, "WA2_POKO_LIGHT"},
    {BG_MDL_CMP_FULL, "WA2_SNOW"},
    {BG_MDL_CMP_FULL, "WA2_LIGHTMAP_GRAD"},
    {BG_MDL_CMP_FULL, "WA2_SUIMEN_TEST_HI_CONT"},
    {BG_MDL_CMP_END, NULL},
};

static struct BgModelSearch lbl_1_data_31EDC[] =
{
    {BG_MDL_CMP_FULL, "WA2_KAIMEN"},
    {BG_MDL_CMP_FULL, "WA2_POKOPOKO"},
    {BG_MDL_CMP_PREFIX, "WA2_FISH"},
    {BG_MDL_CMP_FULL, "WA2_SHIP"},
    {BG_MDL_CMP_PREFIX, "WA2_SHIP_HO"},
    {BG_MDL_CMP_PREFIX, "WA2_ROPE"},
    {BG_MDL_CMP_END, NULL},
};

int fn_1_89E40(int, struct GMAModelEntry *);
int fn_1_89F00(int, struct StageBgModel *);
void wat2_draw_caustics(struct BgWat2Work *work);
void fn_1_8708C(void);
void fn_1_8711C(struct BallEnvArg *arg);
void fn_1_873C0(struct BallEnvArg *arg);
void fn_1_87654(struct BallEnvArg *arg);
void fn_1_87A0C(struct BallEnvArg *arg);
void fn_1_88BC8(struct BgWat2SailNode *node);
void fn_1_88D34(struct BgWat2Cloth *sail);
void fn_1_89998(struct BgWat2Cloth *rope);
void mtxa_rigid_inv_tf_vec_xyz(float x, float y, float z, Vec *dst);
s16 *fn_80069964(struct GMAModel *);
void draw_poly(struct GMAModel *, void *);
void pointer_range_advance(struct GMAModel *, void *);
extern void *g_some_buffer_ptr;
void fn_1_87E98(struct BgWat2Work *work);
void fn_1_88F94(struct BgWat2Work *work);
void fn_1_86CD8(struct BgWat2Work *work);
void fn_1_881D0(struct BgWat2Work *work);
void fn_1_89084(struct BgWat2Work *work);
void fn_1_8901C(struct BgWat2Work *work);
void fn_1_8815C(struct BgWat2Work *work);
void fn_1_897C8(struct BgWat2Work *work);
void fn_1_88AA4(struct BgWat2Work *work);

void bg_wat2_init(void)
{
    struct BgWat2Work *work = world_theme.work;

    bg_init_base();
    work->unk4 = 0;
    work->ropeMax = 12;
    work->ropes = alloc_from_heap_or_panic(bg_heap, work->ropeMax * sizeof(struct BgWat2Cloth), "bg_wat2.c", 263);
    if (work->ropes == NULL)
        work->ropeMax = 0;
    if (work->initialized == 0)
    {
        g_smth_with_bg_model_names(lbl_1_data_31E6C, fn_1_89E40);
        work->initialized = 1;
    }
    work->pokoCount = 0;
    work->sailCount = 0;
    work->ropeCount = 0;
    g_init_stagedef_bg_fg_models(stagedef->bgModels, stagedef->bgModelCount, lbl_1_data_31EDC, fn_1_89F00);
    g_init_stagedef_bg_fg_models(stagedef->fgModels, stagedef->fgModelCount, lbl_1_data_31EDC, fn_1_89F00);
    fn_1_83518(work);
    fn_1_839E0(work);
    fn_1_84508(work);
    wat2_draw_caustics(work);
    fn_1_87E98(work);
    fn_1_88F94(work);
}

void bg_wat2_tick(void)
{
    struct BgWat2Work *work = world_theme.work;

    g_smth_with_challenge_mode_var_and_3p();
    bg_tick_base();
    if (is_game_paused_and_in_view_stage() == 0)
    {
        fn_1_8351C(work);
        fn_1_83B84(work);
        if (work->bubbles->enabled != 0)
            fn_1_849B4(work);
        fn_1_86CD8(work);
        fn_1_881D0(work);
        fn_1_89084(work);
    }
}

void bg_wat2_dest(void)
{
    struct BgWat2Work *work = world_theme.work;

    fn_1_8901C(work);
    fn_1_8815C(work);
    fn_1_84964(work);
    if (work->ropes != NULL)
    {
        OSFreeToHeap(bg_heap, work->ropes);
        work->ropes = NULL;
        work->ropeMax = 0;
    }
}

void bg_wat2_disp(void)
{
    struct BgWat2Work *work = world_theme.work;

    g_smth_with_challenge_mode_var_and_3p();
    if (!(g_some_render_flag.flags & 0x80))
    {
        fn_1_83520(work);
        if (work->bubbles->enabled != 0)
            fn_1_85284(work);
        fn_1_897C8(work);
    }
    fn_1_84124(work);
    fn_1_88AA4(work);
    bg_disp_base();
}

void bg_wat2_item_coin_coli(int unused) {}

void fn_1_83518(struct BgWat2Work *work) {}

void fn_1_8351C(struct BgWat2Work *work) {}

void fn_1_83520(struct BgWat2Work *work)
{
    struct StageBgModel *model;

    mtxa_from_mtx(g_ord_tbl_stuff->matrices[0]);
    model = work->surface;
    if (model != NULL)
    {
        model->flags &= ~0x10000;
        mtxa_translate(&model->pos);
        mtxa_rotate_z(model->rotZ);
        mtxa_rotate_y(model->rotY);
        mtxa_rotate_x(model->rotX);
        mtxa_scale(&model->scale);
    }
    load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
    g_yet_another_unk_draw_func(fn_1_835D0);
    avdisp_draw_model_culled_sort_never(work->surfaceModel);
    g_yet_another_unk_draw_func(NULL);
}

void fn_1_835D0(struct BallEnvArg *arg)
{
    struct BgWat2Work *work = world_theme.work;
    struct TevStageInfo sp28 = arg->tev;
    float sp10[2][3];

    GXLoadTexObj_cached(work->surfaceTex, sp28.texMapId);
    GXLoadTexObj_cached(work->surfaceIndTex, sp28.texMapId + 1);
    mtxa_push();
    mtxa_from_identity();
    mtxa_rotate_x(0x4000);
    mtxa_rotate_y(world_theme.unkA4);
    mtxa_translate_xyz(world_theme.animTimer / 2160.0f, world_theme.animTimer / 2160.0f, world_theme.animTimer / -5280.0f);
    mtxa_scale_s(1.0f / (214.0f + 20.0f * math_sin(world_theme.unkA4 << 6)));
    GXLoadTexMtxImm(mtxa->mtxA, sp28.texMtxId, 0);
    mtxa_from_identity();
    mtxa_rotate_x(0x4000);
    mtxa_rotate_y(0x400);
    mtxa_translate_xyz(world_theme.animTimer / 360.0f, world_theme.animTimer / 360.0f, world_theme.animTimer / 360.0f);
    mtxa_scale_s(0.005f * (1.0f + 0.25f * math_sin(world_theme.unkA4 << 5)));
    GXLoadTexMtxImm(mtxa->mtxA, sp28.texMtxId + 3, 0);
    mtxa_pop();
    sp10[0][0] = 0.2f;
    sp10[0][1] = -0.01f;
    sp10[0][2] = 0.0f;
    sp10[1][0] = 0.0f;
    sp10[1][1] = 0.2f;
    sp10[1][2] = 0.1f;
    g_GXSetIndTexMtx_wrapper(sp28.unk1C, sp10, -1);
    tex_coord_gen(sp28.texCoordId, 1, 0, sp28.texMtxId);
    tex_coord_gen(sp28.texCoordId + 1, 1, 0, sp28.texMtxId + 3);
    GXSetIndTexOrder_cached(sp28.tevIndStage, sp28.texCoordId + 1, sp28.texMapId + 1);
    GXSetTevIndirect_cached(sp28.tevStage, sp28.tevIndStage, 0, 0, sp28.unk1C, 0, 0, 0, 0, 0);
    g_GXSetTevOrder_wrapper(sp28.tevStage, sp28.texCoordId, sp28.texMapId, 4);
    GXSetTevSwapMode_cached(sp28.tevStage, 0, 0);
    GXSetTevColorIn_cached(sp28.tevStage, 15, 8, 10, 15);
    GXSetTevColorOp_cached(sp28.tevStage, 0, 0, 0, 1, 0);
    GXSetTevAlphaIn_cached(sp28.tevStage, 7, 7, 7, 0);
    GXSetTevAlphaOp_cached(sp28.tevStage, 0, 0, 0, 1, 0);
    sp28.tevStage++;
    sp28.tevIndStage++;
    sp28.texCoordId += 2;
    sp28.texMtxId += 6;
    sp28.texMapId += 2;
    sp28.unk1C++;
    arg->tev = sp28;
}

void fn_1_839E0(struct BgWat2Work *work)
{
    int i;
    struct BgWat2Poko *poko = work->pokos;

    i = work->pokoCount;
    for (; i > 0; i--, poko++)
    {
        poko->spawnAccum = RAND_FLOAT();
        poko->angle = rand() & 0x7FFF;
        poko->angleVel = 68.26667f * (RAND_FLOAT() - 0.5f);
        poko->texX = RAND_FLOAT();
        poko->vel = RAND_FLOAT();
        poko->strength = 0.00066666666f * (0.75f + 0.25f * RAND_FLOAT());
        poko->texY = RAND_FLOAT();
    }
}

/* Register allocation differs from the original: work is r24 rather than r31,
 * with the rodata base and every other saved local shifted to fit. All else matches. */
void fn_1_83B84(struct BgWat2Work *work)
{
    s8 *status;
    int i;
    int n;
    struct BgWat2Poko *poko;
    struct GMAModel *gma;
    struct Camera *camera;
    struct StageBgModel *model;
    struct BgWat2Bubble bubble;
    float zr;
    float maxVis;
    float rate;
    float scale;
    float density;
    float yr;
    float t;
    Vec sp1C;
    Vec sp10;

    memset(&bubble, 0, sizeof(bubble));
    bubble.state = 1;
    scale = 1.0f / (float)modeCtrl.unk30;
    density = work->bubbles->density;
    poko = work->pokos;
    for (i = work->pokoCount; i > 0; i--, poko++)
    {
        model = poko->model;
        poko->angle += poko->angleVel;
        poko->vel += poko->strength * (1.0f + 4.0f * (model->scale.y - 1.0f));
        poko->vel *= 0.997f;
        poko->texX += 0.016666668f * poko->vel;
        poko->texY = 0.5f * math_sin(poko->angle);
        rate = density * (10.0f * (model->scale.y - 1.0f) * scale);
        if (rate > 0.0f)
        {
            gma = model->model;
            mtxa_from_translate(&model->pos);
            mtxa_rotate_z(model->rotZ);
            mtxa_rotate_y(model->rotY);
            mtxa_rotate_x(model->rotX);
            mtxa_scale(&model->scale);
            mtxa_translate(&gma->boundSphereCenter);
            mathutil_mtxA_get_translate(&sp1C);
            maxVis = -1.0f;
            status = g_poolInfo.playerPool.statusList;
            camera = cameras;
            for (n = g_poolInfo.playerPool.count; n > 0; n--, status++, camera++)
            {
                if (*status == 2 && 0.0f != camera->sub28.viewportW && 0.0f != camera->sub28.viewportH)
                {
                    mtxa_push();
                    {
                        S16Vec sp8 = {0, 0, 0};

                        sp8.x = camera->rotX;
                        sp8.y = camera->rotY;
                        sp8.z = camera->rotZ;
                        fn_1_22D64(&camera->eye, &sp8, 0.005493164f * (float)camera->sub28.view.fov,
                            camera->sub28.view.aspect, camera->sub28.view.unk28, camera->sub28.view.unk2C);
                    }
                    mtxa_pop();
                    if (fn_1_233FC(&sp1C, 2.0f * gma->boundSphereRadius))
                    {
                        mtxa_push();
                        mtxa_from_mtx(camera->unk1AC);
                        mtxa_tf_point(&sp1C, &sp10);
                        mtxa_pop();
                        if (sp10.z < -0.1f)
                            t = -(gma->boundSphereRadius * camera->sub28.view.invTanHalfFov) / sp10.z;
                        else
                            t = 1.0f;
                        if (t > maxVis)
                            maxVis = t;
                    }
                }
            }
            t = (maxVis <= 0.0f) ? 0.0f : maxVis;
            maxVis = poko->spawnAccum + rate * t;
            if (maxVis > 1.0f)
            {
                mtxa_tf_point_xyz(0.0f, 0.5f * gma->boundSphereRadius, 0.0f, &bubble.pos);
                while (maxVis > 1.0f)
                {
                    zr = 1.2f * (RAND_FLOAT() - 0.5f);
                    yr = 0.2f * rate * (1.0f + 0.5f * RAND_FLOAT());
                    mtxa_tf_vec_xyz(1.2f * (RAND_FLOAT() - 0.5f), yr, zr, &bubble.vel);
                    bubble.timer = (s16)(int)(60.0f * (2.0f + 1.25f * RAND_FLOAT()));
                    bubble.maxSize = 1.0f + 3.5f * RAND_FLOAT();
                    bubble.maxAlpha = (4.0f / bubble.size) * (0.75f + RAND_FLOAT());
                    if (bubble.maxAlpha > 1.0f)
                        bubble.maxAlpha = 1.0f;
                    fn_1_85B20(work, &bubble);
                    maxVis -= 1.0f;
                }
            }
            poko->spawnAccum = maxVis;
        }
    }
    if (work->pokoCount != 0)
        camera_apply_viewport(modeCtrl.currPlayer);
}

void fn_1_84124(struct BgWat2Work *work)
{
    int i;
    struct BgWat2Poko *poko;
    struct StageBgModel *model;
    struct GMAModel *light;
    struct GMAModel *glow;
    u32 mask;
    struct GMAModel *gma;
    float t;
    float lightRadius;
    float radius;
    float scale;
    Vec sp14;
    Vec sp8;

    mask = g_smth_with_challenge_mode_var_and_3p();
    g_something_with_texture_scroll_2(1);
    avdisp_set_z_mode(1, 3, 0);
    light = work->pokoLight;
    lightRadius = light->boundSphereRadius;
    glow = init_common_gma->modelEntries[95].model;
    poko = work->pokos;
    i = work->pokoCount;
    for (; i > 0; i--, poko++)
    {
        model = poko->model;
        gma = model->model;
        radius = gma->boundSphereRadius;
        if ((model->flags & mask) != 0)
        {
            mtxa_from_translate_xyz(poko->texX, poko->texY, 0.0f);
            fn_1_10A4CC(0, mtxa->mtxA);
            mtxa_from_mtx(g_ord_tbl_stuff->matrices[0]);
            mtxa_translate(&model->pos);
            mtxa_rotate_z(model->rotZ);
            mtxa_rotate_y(model->rotY);
            mtxa_rotate_x(model->rotX);
            mtxa_translate(&gma->boundSphereCenter);
            mtxa_push();
            mtxa_sq_from_identity();
            mathutil_mtxA_get_translate(&sp8);
            if (sp8.z < -(0.1f + radius))
            {
                t = (sp8.z + radius) / sp8.z;
                sp14.x = sp8.x * t;
                sp14.y = sp8.y * t;
                sp14.z = sp8.z + radius;
                mathutil_mtxA_set_translate(&sp14);
                mtxa_push();
                scale = t;
                mtxa_scale_s(scale);
                if (g_is_sphere_visible_scaled(&light->boundSphereCenter, lightRadius, scale))
                    avdisp_draw_model_unculled_sort_always(light);
                mtxa_pop();
                mtxa_push();
                scale = 1.0f + 0.5f * ((model->scale.x > model->scale.y ? model->scale.x : model->scale.y) - 1.0f);
                scale *= t * ((3.5f * radius) / glow->boundSphereRadius);
                mtxa_scale_s(scale);
                if (g_is_sphere_visible_scaled(&glow->boundSphereCenter, glow->boundSphereRadius, scale))
                    avdisp_draw_model_unculled_sort_always(glow);
                mtxa_pop();
            }
            mtxa_pop();
            if (model->scale.y > 1.0f)
            {
                mtxa_translate_xyz(0.0f, radius * model->scale.y, 0.0f);
                mtxa_sq_from_identity();
                mathutil_mtxA_get_translate(&sp8);
                if (sp8.z < -(0.1f + radius))
                {
                    t = (sp8.z + radius) / sp8.z;
                    sp14.x = sp8.x * t;
                    sp14.y = sp8.y * t;
                    sp14.z = sp8.z + radius;
                    mathutil_mtxA_set_translate(&sp14);
                    scale = t * ((3.0f * radius) / glow->boundSphereRadius);
                    mtxa_scale_s(scale);
                    if (g_is_sphere_visible_scaled(&glow->boundSphereCenter, glow->boundSphereRadius, scale))
                    {
                        t = 0.75f * (model->scale.y - 1.0f);
                        if (t > 1.0f)
                            t = 1.0f;
                        set_post_mult_color(t, t, t, 1.0f);
                        avdisp_draw_model_unculled_sort_always(glow);
                        fade_color_base_default();
                    }
                }
            }
        }
    }
    avdisp_set_z_mode(1, 3, 1);
    g_something_with_texture_scroll_2(0);
}

void fn_1_84508(struct BgWat2Work *work)
{
    int i;
    struct BgWat2Bubbles *b;
    struct BgWat2Bubble *p;

    work->bubbles = b = alloc_from_heap_or_panic(bg_heap, sizeof(*b), "bg_wat2.c", 0x360);
    memset(b, 0, sizeof(*b));
    if (modeCtrl.unk30 < 2)
        b->enabled = 1;
    else
    {
        b->enabled = 0;
        return;
    }
    switch (modeCtrl.gameType)
    {
    case 3:
        b->density = 1.0f / (float)(modeCtrl.unk30 + 1);
        b->count = 512 * b->density;
        b->ambientCount = 128 * b->density;
        break;
    case 9:
        b->density = 1.0f;
        b->count = 512;
        b->ambientCount = 8;
        break;
    default:
        b->density = 1.0f;
        b->count = 512;
        b->ambientCount = 128;
        break;
    }
    p = b->bubbles;
    for (i = b->count; i > 0; i--, p++)
    {
        p->state = 0;
        p->pos.x = 20.0f * (RAND_FLOAT() - 0.5f);
        p->pos.y = 100.0f;
        p->pos.z = 20.0f * (RAND_FLOAT() - 0.5f);
        p->vel.x = 0.2f * (RAND_FLOAT() - 0.5f);
        p->vel.y = 0.2f * (RAND_FLOAT() - 0.5f);
        p->vel.z = 0.2f * (RAND_FLOAT() - 0.5f);
    }
    p = b->ambient;
    for (i = b->ambientCount; i > 0; i--, p++)
    {
        p->state = 2;
        p->timer = 0;
        p->maxAlpha = 0.0f;
        p->alpha = 0.0f;
        p->maxSize = 0.0f;
        p->size = 0.0f;
        p->pos.x = 10.0f * RAND_FLOAT();
        p->pos.y = 10.0f * RAND_FLOAT();
        p->pos.z = 10.0f * RAND_FLOAT();
        p->vel.x = 0.1f * (RAND_FLOAT() - 0.5f);
        p->vel.y = 0.1f * (RAND_FLOAT() - 0.5f);
        p->vel.z = 0.1f * (RAND_FLOAT() - 0.5f);
    }
}

void fn_1_84964(struct BgWat2Work *work)
{
    if (work->bubbles != NULL)
    {
        OSFreeToHeap(bg_heap, work->bubbles);
        work->bubbles = NULL;
    }
}

void fn_1_849B4(struct BgWat2Work *work)
{
    int i;
    struct BgWat2Bubbles *b = work->bubbles;
    float cx = 0.05f * work->current.x;
    float cy = 0.05f * work->current.y;
    float cz = 0.05f * work->current.z;
    struct BgWat2Bubble *p = b->bubbles;
    float t;

    i = b->count;
    for (; i > 0; i--, p++)
    {
        if (p->state == 0)
            continue;
        switch (p->state)
        {
        case 1:
            p->vel.y += -0.008f;
            p->vel.x += 0.01f * (cx - p->vel.x);
            p->vel.y += 0.01f * (cy - p->vel.y);
            p->vel.z += 0.01f * (cz - p->vel.z);
            p->pos.x += p->vel.x;
            p->pos.y += p->vel.y;
            p->pos.z += p->vel.z;
            if (p->timer != 0 && (float)p->timer < 30.0f)
            {
                t = (float)(p->timer - 1) / (float)p->timer;
                p->alpha *= 0.9f * t;
            }
            else
            {
                p->size += 0.1f * (p->maxSize - p->size);
                p->alpha += 0.2f * (p->maxAlpha - p->alpha);
            }
            if (p->timer != 0)
            {
                p->timer--;
                if (p->timer == 0)
                    p->state = 0;
            }
            break;
        }
    }
    fn_1_84BD8(b);
}

void fn_1_84BD8(struct BgWat2Bubbles *b)
{
    int i;
    struct BgWat2Bubble *p;
    int limit;
    Vec sp14;
    Vec sp8;
    float density;
    float cx;
    float cy;
    float cz;
    float drag;
    struct BgWat2Work *work = world_theme.work;
    struct Camera *camera = cameras;

    if ((camera->flags & 1) == 0)
    {
        mtxa_from_mtx(camera->unk1AC);
        mtxa_rigid_inv_tf_point_xyz(0.0f, 0.0f, 0.0f, &sp14);
        mtxa_from_mtx(camera->unk1DC);
        mtxa_rigid_inv_tf_point_xyz(0.0f, 0.0f, 0.0f, &sp8);
        sp14.x -= sp8.x;
        sp14.y -= sp8.y;
        sp14.z -= sp8.z;
    }
    switch (modeCtrl.gameType)
    {
    case 0:
    case 1:
    case 2:
    case 14:
        density = 1.0f - (float)mode_info.timerCurr / (float)mode_info.timerMax;
        break;
    default:
        density = 0.5f * (1.0f - math_sin((int)(12.136296f * world_theme.animTimer) + 0x4000));
        break;
    }
    limit = (int)((float)(b->ambientCount + 8) * density) - 8;
    cx = 0.03f * work->current.x;
    cy = 0.03f * work->current.y;
    cz = 0.03f * work->current.z;
    p = b->ambient;
    i = b->ambientCount;
    for (; i > 0; i--, p++)
    {
        p->timer--;
        if (p->timer <= 0)
        {
            if (i > limit)
            {
                p->alpha = 0.0f;
                continue;
            }
            p->timer = 60.0f * (5.0f + 10.0f * RAND_FLOAT());
            p->maxAlpha = 1.0f;
            p->alpha = 0.0f;
            p->maxSize = 0.075f * (0.25f + 1.75f * RAND_FLOAT());
            p->size = p->maxSize;
            p->pos.x = 10.0f * RAND_FLOAT();
            p->pos.y = 10.0f * RAND_FLOAT();
            p->pos.z = 10.0f * RAND_FLOAT();
            p->vel.x = 0.02f * (RAND_FLOAT() - 0.5f);
            p->vel.y = -0.01f * RAND_FLOAT();
            p->vel.z = 0.02f * (RAND_FLOAT() - 0.5f);
            p->drag = 0.03f * RAND_FLOAT();
        }
        p->vel.y += -0.0001f;
        drag = p->drag;
        p->vel.x += drag * (cx - p->vel.x);
        p->vel.y += drag * (cy - p->vel.y);
        p->vel.z += drag * (cz - p->vel.z);
        p->pos.x += p->vel.x;
        p->pos.y += p->vel.y;
        p->pos.z += p->vel.z;
        while (p->pos.x < 0.0f)
            p->pos.x += 10.0f;
        while (p->pos.x > 10.0f)
            p->pos.x -= 10.0f;
        while (p->pos.y < 0.0f)
            p->pos.y += 10.0f;
        while (p->pos.y > 10.0f)
            p->pos.y -= 10.0f;
        while (p->pos.z < 0.0f)
            p->pos.z += 10.0f;
        while (p->pos.z > 10.0f)
            p->pos.z -= 10.0f;
        p->size += 0.15f * (p->maxSize - p->size);
        if (p->timer > 60)
            p->alpha += 0.15f * (p->maxAlpha - p->alpha);
        else
        {
            density = (float)p->timer / (float)(p->timer + 1);
            p->alpha *= density;
        }
        p->timer--;
    }
}

void fn_1_85284(struct BgWat2Work *work)
{
    int i;
    struct BgWat2Bubbles *b = work->bubbles;
    struct BgWat2Bubble *p;
    struct GMAModel *model = work->bubbleModel;
    Vec sp8;
    struct OrdTblNode *list;
    struct BgWat2BubbleNode *node;

    p = b->bubbles;
    i = b->count;
    for (; i > 0; i--, p++)
    {
        if (p->state != 0 && 0.0f != p->alpha && 0.0f != p->size)
        {
            mtxa_from_mtx(g_ord_tbl_stuff->matrices[0]);
            mtxa_translate(&p->pos);
            mtxa_sq_from_identity();
            if (g_is_sphere_visible(&model->boundSphereCenter, p->size))
            {
                mtxa_tf_point(&model->boundSphereCenter, &sp8);
                list = fn_1_CCC18(sp8.z);
                node = ord_tbl_alloc_node(sizeof(*node));
                if (node != NULL)
                {
                    node->node.drawFunc = (void *)fn_1_85B78;
                    node->data = p;
                    fn_1_CD418(list, &node->node);
                }
            }
        }
    }
    fn_1_8538C(b);
}

void fn_1_8538C(struct BgWat2Bubbles *b)
{
    struct OrdTblNode *list = fn_1_CCC18(0.0f);
    struct BgWat2BubbleNode *node = ord_tbl_alloc_node(sizeof(*node));

    if (node != NULL)
    {
        node->node.drawFunc = (void *)fn_1_853FC;
        node->data = b;
        fn_1_CD418(list, &node->node);
    }
}

void fn_1_853FC(struct BgWat2BubbleNode *node)
{
    int n;
    struct BgWat2Bubble *p;
    int x;
    int y;
    int z;
    struct BgWat2Bubbles *b = node->data;
    struct BgWat2Work *work = world_theme.work;
    s16 angle;
    float len;
    float alpha;
    float size;
    Vec quad[4];
    Mtx sp78;
    Mtx sp48;
    Vec sp3C;
    Vec sp30;
    Vec sp24;
    Vec sp18;
    GXColor sp14;
    GXColor sp10;

    fn_1_86158(work);
    sp14.r = 255;
    sp14.g = 255;
    sp14.b = 255;
    sp14.a = 255;
    GXSetTevKColor_cached(0, sp14);
    fn_1_86388();
    mtxa_from_mtx(g_ord_tbl_stuff->matrices[0]);
    mtxa_rigid_inv_tf_point_xyz(0.0f, 0.0f, 0.0f, &sp24);
    if (sp24.x < 0.0f)
        sp24.x -= 10.0f;
    if (sp24.y < 0.0f)
        sp24.y -= 10.0f;
    if (sp24.z < 0.0f)
        sp24.z -= 10.0f;
    sp24.x = 10.0f * (float)(int)(0.1f * sp24.x);
    sp24.y = 10.0f * (float)(int)(0.1f * sp24.y);
    sp24.z = 10.0f * (float)(int)(0.1f * sp24.z);
    mtxa_translate(&sp24);
    mtxa_to_mtx(sp78);
    for (z = -1; z <= 1; z++)
    {
        sp18.z = 10.0f * (float)z;
        for (y = -1; y <= 1; y++)
        {
            sp18.y = 10.0f * (float)y;
            for (x = -1; x <= 1; x++)
            {
                sp18.x = 10.0f * (float)x;
                mtxa_from_mtx(sp78);
                mtxa_translate(&sp18);
                mtxa_to_mtx(sp48);
                p = b->ambient;
                for (n = b->ambientCount; n > 0; n--, p++)
                {
                    alpha = p->alpha;
                    if (alpha <= 0.0f)
                        continue;
                    size = p->size * work->bubbleModel->boundSphereRadius;
                    mtxa_from_mtx(sp48);
                    if (!g_is_sphere_visible(&p->pos, size))
                        continue;
                    mtxa_tf_point(&p->pos, &sp3C);
                    if (sp3C.z >= -0.1f)
                        continue;
                    len = mathutil_vec_len(&sp3C);
                    if (len > 11.0f)
                        continue;
                    if (len > 10.0f)
                        alpha *= 11.0f - len;
                    else if (len < 1.0f)
                        alpha *= len;
                    sp30.x = p->pos.x - p->vel.x;
                    sp30.y = p->pos.y - p->vel.y;
                    sp30.z = p->pos.z - p->vel.z;
                    mtxa_tf_point(&sp30, &sp30);
                    if (sp30.z >= -0.1f)
                        continue;
                    {
                        float x0 = sp3C.x - size;
                        float y0 = sp3C.y - size;
                        float x1 = sp3C.x + size;
                        float y1 = sp3C.y + size;

                        quad[0].x = x0;
                        quad[0].y = y0;
                        quad[0].z = sp3C.z;
                        quad[1].x = x1;
                        quad[1].y = y0;
                        quad[1].z = sp3C.z;
                        quad[2].x = x1;
                        quad[2].y = y1;
                        quad[2].z = sp3C.z;
                        quad[3].x = x0;
                        quad[3].y = y1;
                        quad[3].z = sp3C.z;
                    }
                    sp30.x *= sp3C.z / sp30.z;
                    sp30.y *= sp3C.z / sp30.z;
                    sp30.z = sp3C.z;
                    sp24.x = sp30.x - sp3C.x;
                    sp24.y = sp30.y - sp3C.y;
                    if (0.0f != sp24.x || 0.0f != sp24.y)
                    {
                        sp24.z = 0.0f;
                        len = vec_normalize_len(&sp24);
                        sp3C.x -= size * sp24.x;
                        sp3C.y -= size * sp24.y;
                        sp30.x += size * sp24.x;
                        sp30.y += size * sp24.y;
                        len += 2.0f * size;
                        angle = math_atan2(sp24.y, sp24.x);
                        mtxa_from_translate(&sp3C);
                        mtxa_rotate_z(angle);
                        mtxa_scale_xyz(len / (2.0f * size), 1.0f, 1.0f);
                        mtxa_rotate_z(-angle);
                        mtxa_translate_neg(&sp3C);
                        mtxa_tf_point(&quad[0], &quad[0]);
                        mtxa_tf_point(&quad[1], &quad[1]);
                        mtxa_tf_point(&quad[2], &quad[2]);
                        mtxa_tf_point(&quad[3], &quad[3]);
                    }
                    /* PC HFR: anonymous identical quads; the tag keeps each
                     * particle paired with itself, not a neighbour */
                    HFR_TAG(HFR_K_BG, 0x57410000 | (u32)(p - b->ambient));
                    sp10.r = 255;
                    sp10.g = 255;
                    sp10.b = 255;
                    sp10.a = 255.0f * alpha;
                    if (sp14.a != sp10.a)
                    {
                        sp14 = sp10;
                GXSetTevKColor_cached(0, sp14);
                        fn_1_86388();
                    }
                    GXBegin(0x80, 0, 4);
                    GXPosition3f32(quad[0].x, quad[0].y, quad[0].z);
                    GXTexCoord2f32(0.0f, 0.0f);
                    GXPosition3f32(quad[1].x, quad[1].y, quad[1].z);
                    GXTexCoord2f32(1.0f, 0.0f);
                    GXPosition3f32(quad[2].x, quad[2].y, quad[2].z);
                    GXTexCoord2f32(1.0f, 1.0f);
                    GXPosition3f32(quad[3].x, quad[3].y, quad[3].z);
                    GXTexCoord2f32(0.0f, 1.0f);
                    HFR_UNTAG();
                }
            }
        }
    }
}

void fn_1_85B20(struct BgWat2Work *work, struct BgWat2Bubble *src)
{
    struct BgWat2Bubbles *b = work->bubbles;
    struct BgWat2Bubble *p = b->bubbles;
    int i = b->count;

    for (; i > 0; i--, p++)
    {
        if (p->state == 0)
            break;
    }
    if (i != 0)
        memcpy_handler(p, src, sizeof(*p));
}

void fn_1_85B78(struct BgWat2BubbleNode *node)
{
    struct BgWat2Bubble *p = node->data;
    int angle;
    struct BgWat2Work *work = world_theme.work;
    float size;
    float len;
    Vec quad[4];
    Vec sp28;
    Vec sp1C;
    Vec sp10;
    GXColor color;

    fn_1_86158(work);
    color.r = 255;
    color.g = 255;
    color.b = 255;
    color.a = 255.0f * p->alpha;
    GXSetTevKColor_cached(0, color);
    size = p->size * work->bubbleModel->boundSphereRadius;
    mtxa_from_mtx(g_ord_tbl_stuff->matrices[0]);
    mtxa_tf_point(&p->pos, &sp28);
    sp1C.x = p->pos.x - 2.0f * p->vel.x;
    sp1C.y = p->pos.y - 2.0f * p->vel.y;
    sp1C.z = p->pos.z - 2.0f * p->vel.z;
    mtxa_tf_point(&sp1C, &sp1C);
    {
        float x0 = sp28.x - size;
        float y0 = sp28.y - size;
        float x1 = sp28.x + size;
        float y1 = sp28.y + size;

        quad[0].x = x0;
        quad[0].y = y0;
        quad[0].z = sp28.z;
        quad[1].x = x1;
        quad[1].y = y0;
        quad[1].z = sp28.z;
        quad[2].x = x1;
        quad[2].y = y1;
        quad[2].z = sp28.z;
        quad[3].x = x0;
        quad[3].y = y1;
        quad[3].z = sp28.z;
    }
    sp1C.x *= sp28.z / sp1C.z;
    sp1C.y *= sp28.z / sp1C.z;
    sp1C.z = sp28.z;
    sp10.x = sp1C.x - sp28.x;
    sp10.y = sp1C.y - sp28.y;
    if (0.0f != sp10.x || 0.0f != sp10.y)
    {
        sp10.z = 0.0f;
        len = vec_normalize_len(&sp10);
        sp28.x -= size * sp10.x;
        sp28.y -= size * sp10.y;
        sp1C.x += size * sp10.x;
        sp1C.y += size * sp10.y;
        len += 2.0f * size;
        angle = math_atan2(sp10.y, sp10.x);
        mtxa_from_translate(&sp28);
        mtxa_rotate_z(angle);
        mtxa_scale_xyz(len / (2.0f * size), 1.0f, 1.0f);
        mtxa_rotate_z(-angle);
        mtxa_translate_neg(&sp28);
        mtxa_tf_point(&quad[0], &quad[0]);
        mtxa_tf_point(&quad[1], &quad[1]);
        mtxa_tf_point(&quad[2], &quad[2]);
        mtxa_tf_point(&quad[3], &quad[3]);
    }
    GXBegin(0x80, 0, 12);
    GXPosition3f32(0.0f, 0.0f, 0.0f);
    GXTexCoord2f32(0.0f, 0.0f);
    GXPosition3f32(0.0f, 0.0f, 0.0f);
    GXTexCoord2f32(0.0f, 0.0f);
    GXPosition3f32(0.0f, 0.0f, 0.0f);
    GXTexCoord2f32(0.0f, 0.0f);
    GXPosition3f32(0.0f, 0.0f, 0.0f);
    GXTexCoord2f32(0.0f, 0.0f);
    GXPosition3f32(0.0f, 0.0f, 0.0f);
    GXTexCoord2f32(0.0f, 0.0f);
    GXPosition3f32(0.0f, 0.0f, 0.0f);
    GXTexCoord2f32(0.0f, 0.0f);
    GXPosition3f32(0.0f, 0.0f, 0.0f);
    GXTexCoord2f32(0.0f, 0.0f);
    GXPosition3f32(0.0f, 0.0f, 0.0f);
    GXTexCoord2f32(0.0f, 0.0f);
    GXPosition3f32(quad[0].x, quad[0].y, quad[0].z);
    GXTexCoord2f32(0.0f, 0.0f);
    GXPosition3f32(quad[1].x, quad[1].y, quad[1].z);
    GXTexCoord2f32(1.0f, 0.0f);
    GXPosition3f32(quad[2].x, quad[2].y, quad[2].z);
    GXTexCoord2f32(1.0f, 1.0f);
    GXPosition3f32(quad[3].x, quad[3].y, quad[3].z);
    GXTexCoord2f32(0.0f, 1.0f);
}

void fn_1_86158(struct BgWat2Work *work)
{
    GXLoadTexObj_cached(work->bubbleTex, 0);
    GXLoadTexObj_cached(work->bubbleMaskTex, 1);
    GXSetBlendMode_cached(1, 4, 5, 0);
    GXSetZMode_cached(1, 3, 0);
    g_set_something3(1);
    fn_1_E2F1C();
    GXSetCullMode_cached(0);
    g_GXSetTevIndirect_zero_if_different(0);
    GXSetTexCoordGen2_cached(0, 1, 4, 60, 0, 125);
    g_GXSetTevOrder_wrapper(0, 0, 0, 255);
    GXSetTevKColorSel_cached(0, 12);
    GXSetTevColorIn_cached(0, 15, 8, 14, 15);
    GXSetTevColorOp_cached(0, 0, 0, 0, 1, 0);
    GXSetTevAlphaIn_cached(0, 7, 4, 6, 7);
    GXSetTevAlphaOp_cached(0, 0, 0, 0, 1, 0);
    GXSetNumTevStages_cached(1);
    GXSetNumTexGens_cached(1);
    GXSetNumIndStages_cached(0);
    GXSetNumChans_cached(0);
    g_GXSetTevIndirect_zero_if_different(1);
    GXSetTexCoordGen2_cached(1, 1, 4, 60, 0, 125);
    g_GXSetTevOrder_wrapper(1, 1, 1, 255);
    GXSetTevKAlphaSel_cached(1, 28);
    GXSetTevColorIn_cached(1, 15, 15, 15, 0);
    GXSetTevColorOp_cached(1, 0, 0, 0, 1, 0);
    GXSetTevAlphaIn_cached(1, 7, 4, 6, 7);
    GXSetTevAlphaOp_cached(1, 0, 0, 0, 1, 0);
    GXSetNumTevStages_cached(2);
    GXSetNumTexGens_cached(2);
    GXSetNumIndStages_cached(0);
    GXSetNumChans_cached(0);
    g_something_with_GX_vtx_desc(0x2200);
    mtxa_from_identity();
    GXLoadPosMtxImm(mtxa->mtxA, 0);
}

void fn_1_86388(void)
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

void wat2_draw_caustics(struct BgWat2Work *work)
{
    struct BgWat2Caustics *c = &work->caustics;

    c->unk0 = 1.0f;
    c->amp = 6.0f * RAND_FLOAT();
    c->ampVel = 0.0011111111f * (1.0f + RAND_FLOAT());
    c->unkC.x = rand() & 0x7FFF;
    c->unkC.y = rand() & 0x7FFF;
    c->unkC.z = rand() & 0x7FFF;
    c->unk12.x = 64.0f * (0.4f + 0.8f * RAND_FLOAT());
    c->unk12.y = 64.0f * (0.4f + 0.8f * RAND_FLOAT());
    c->unk12.z = 64.0f * (0.4f + 0.8f * RAND_FLOAT());
    c->unk18 = 6.0f * RAND_FLOAT();
    c->unk1C = 0.0011111111f * (1.0f + RAND_FLOAT());
    c->unk20.x = rand() & 0x7FFF;
    c->unk20.y = rand() & 0x7FFF;
    c->unk20.z = rand() & 0x7FFF;
    c->unk26.x = 48.0f * (0.4f + 0.8f * RAND_FLOAT());
    c->unk26.y = 48.0f * (0.4f + 0.8f * RAND_FLOAT());
    c->unk26.z = 48.0f * (0.4f + 0.8f * RAND_FLOAT());
    c->unk2C.x = rand() & 0x7FFF;
    c->unk2C.y = rand() & 0x7FFF;
    c->unk2C.z = rand() & 0x7FFF;
    c->unk32.x = 64.0f * (0.4f + 0.8f * RAND_FLOAT());
    c->unk32.y = 64.0f * (0.4f + 0.8f * RAND_FLOAT());
    c->unk32.z = 64.0f * (0.4f + 0.8f * RAND_FLOAT());
    c->unk38.x = rand() & 0x7FFF;
    c->unk38.y = rand() & 0x7FFF;
    c->unk38.z = rand() & 0x7FFF;
    c->unk3E.x = 48.0f * (0.4f + 0.8f * RAND_FLOAT());
    c->unk3E.y = 48.0f * (0.4f + 0.8f * RAND_FLOAT());
    c->unk3E.z = 48.0f * (0.4f + 0.8f * RAND_FLOAT());
    if (modeCtrl.unk30 < 2)
    {
        world_theme.unk8C = fn_1_8711C;
        world_theme.unk90 = fn_1_873C0;
    }
    world_theme.ballEnvFunc = fn_1_87A0C;
    world_theme.unk98 = fn_1_8708C;
}

void fn_1_86CD8(struct BgWat2Work *work)
{
    struct BgWat2Caustics *c = &work->caustics;
    S16Vec sp8;
    Vec sp1C;
    Vec sp10;

    c->amp += c->ampVel;
    c->unkC.x += c->unk12.x;
    c->unkC.y += c->unk12.y;
    c->unkC.z += c->unk12.z;
    c->unk18 += c->unk1C;
    c->unk20.x += c->unk26.x;
    c->unk20.y += c->unk26.y;
    c->unk20.z += c->unk26.z;
    c->unk2C.x += c->unk32.x;
    c->unk2C.y += c->unk32.y;
    c->unk2C.z += c->unk32.z;
    c->unk38.x += c->unk3E.x;
    c->unk38.y += c->unk3E.y;
    c->unk38.z += c->unk3E.z;
    mtxa_from_identity();
    sp8 = c->unk2C;
    sp1C.x = 0.1111111111111111 * (0.9f + 0.2f * math_sin(sp8.x));
    sp1C.y = 0.1111111111111111 * (0.9f + 0.2f * math_sin(sp8.y));
    sp1C.z = 0.1111111111111111 * (0.9f + 0.2f * math_sin(sp8.z));
    mtxa_scale(&sp1C);
    mtxa_rotate_y(0x800);
    mtxa_rotate_x(0x3800);
    sp8 = c->unkC;
    sp10.x = c->amp * math_sin(sp8.x);
    sp10.y = c->amp * math_sin(sp8.y);
    sp10.z = c->amp * math_sin(sp8.z);
    mtxa_translate(&sp10);
    mtxa_to_mtx(c->unk50);
    mtxa_from_identity();
    sp8 = c->unk38;
    sp1C.x = 0.078125f * (0.9f + 0.2f * math_sin(sp8.x));
    sp1C.y = 0.078125f * (0.9f + 0.2f * math_sin(sp8.y));
    sp1C.z = 0.078125f * (0.9f + 0.2f * math_sin(sp8.z));
    mtxa_scale(&sp1C);
    mtxa_rotate_y(0x800);
    mtxa_rotate_x(0x3800);
    mtxa_rotate_z(0x1000);
    sp8 = c->unk20;
    sp10.x = c->unk18 * math_sin(sp8.x);
    sp10.y = c->unk18 * math_sin(sp8.y);
    sp10.z = c->unk18 * math_sin(sp8.z);
    mtxa_translate(&sp10);
    mtxa_to_mtx(c->unk80);
    mtxa_from_identity();
    mtxa_rotate_y(0x1000);
    mtxa_rotate_x(0x3800);
    mtxa_to_mtx(c->unkB0);
    mtxa_from_identity();
    mtxa->mtxA[0][0] = 0.0f;
    mtxa->mtxA[0][2] = -0.5f;
    mtxa->mtxA[0][3] = c->unk0;
    mtxa->mtxA[1][1] = 0.0f;
    mtxa->mtxA[2][2] = 0.0f;
    mtxa->mtxA[2][3] = 1.0f;
    mtxa_to_mtx(c->unkE0);
}

void fn_1_8708C(void)
{
    struct BgWat2Caustics *c = &((struct BgWat2Work *)world_theme.work)->caustics;
    Mtx sp8;

    mtxa_from_mtx(g_ord_tbl_stuff->matrices[0]);
    mtxa_rigid_invert();
    mtxa_to_mtx(sp8);
    mtxa_mult_left(c->unk50);
    mtxa_to_mtx(c->unk110);
    mtxa_from_mtx(sp8);
    mtxa_mult_left(c->unk80);
    mtxa_to_mtx(c->unk140);
    mtxa_from_mtx(sp8);
    mtxa_mult_left(c->unkB0);
    mtxa_to_mtx(c->unk170);
}

void fn_1_8711C(struct BallEnvArg *arg)
{
    struct BgWat2Work *work = world_theme.work;
    struct BgWat2Caustics *c = &work->caustics;
    struct TevStageInfo sp20 = arg->tev;
    float sp8[2][3];

    if (arg->unk0 == 0
     && sp20.tevStage == c->tev.tevStage
     && sp20.tevIndStage == c->tev.tevIndStage
     && sp20.texMtxId == c->tev.texMtxId
     && sp20.postTexMtxId == c->tev.postTexMtxId
     && sp20.unk1C == c->tev.unk1C
     && sp20.texMapId == c->tev.texMapId)
    {
        arg->tev.tevStage += 2;
        arg->tev.tevIndStage += 1;
        arg->tev.texCoordId += 3;
        arg->tev.texMtxId += 9;
        arg->tev.postTexMtxId += 3;
        arg->tev.unk1C += 1;
        arg->tev.texMapId += 2;
        return;
    }
    c->tev = sp20;
    GXLoadTexObj_cached(c->tex48, sp20.texMapId);
    GXLoadTexObj_cached(work->surfaceIndTex, sp20.texMapId + 1);
    sp8[0][0] = 0.2f;
    sp8[0][1] = 0.0f;
    sp8[0][2] = 0.0f;
    sp8[1][0] = 0.0f;
    sp8[1][1] = 0.2f;
    sp8[1][2] = 0.0f;
    g_GXSetIndTexMtx_wrapper(sp20.unk1C, sp8, 0);
    mtxa_push();
    mtxa_mult_left(c->unk110);
    GXLoadTexMtxImm(mtxa->mtxA, sp20.texMtxId, 0);
    mtxa_pop();
    mtxa_push();
    mtxa_mult_left(c->unk140);
    GXLoadTexMtxImm(mtxa->mtxA, sp20.texMtxId + 3, 0);
    mtxa_pop();
    fn_1_87654(arg);
}

void fn_1_873C0(struct BallEnvArg *arg)
{
    struct BgWat2Work *work = world_theme.work;
    struct BgWat2Caustics *c = &work->caustics;
    struct TevStageInfo sp20 = arg->tev;
    float sp8[2][3];

    if (arg->unk0 == 0
     && sp20.tevStage == c->tev.tevStage
     && sp20.tevIndStage == c->tev.tevIndStage
     && sp20.texMtxId == c->tev.texMtxId
     && sp20.postTexMtxId == c->tev.postTexMtxId
     && sp20.unk1C == c->tev.unk1C
     && sp20.texMapId == c->tev.texMapId)
    {
        arg->tev.tevStage += 2;
        arg->tev.tevIndStage += 1;
        arg->tev.texCoordId += 3;
        arg->tev.texMtxId += 9;
        arg->tev.postTexMtxId += 3;
        arg->tev.unk1C += 1;
        arg->tev.texMapId += 2;
        return;
    }
    c->tev = sp20;
    GXLoadTexObj_cached(c->tex44, sp20.texMapId);
    GXLoadTexObj_cached(work->surfaceIndTex, sp20.texMapId + 1);
    sp8[0][0] = 0.15f;
    sp8[0][1] = 0.0f;
    sp8[0][2] = 0.0f;
    sp8[1][0] = 0.0f;
    sp8[1][1] = 0.15f;
    sp8[1][2] = 0.0f;
    g_GXSetIndTexMtx_wrapper(sp20.unk1C, sp8, 0);
    mtxa_push();
    mtxa_mult_left(c->unk110);
    mtxa_scale_s(0.33333334f);
    GXLoadTexMtxImm(mtxa->mtxA, sp20.texMtxId, 0);
    mtxa_pop();
    mtxa_push();
    mtxa_mult_left(c->unk140);
    mtxa_scale_s(0.5f);
    GXLoadTexMtxImm(mtxa->mtxA, sp20.texMtxId + 3, 0);
    mtxa_pop();
    fn_1_87654(arg);
}

void fn_1_87654(struct BallEnvArg *arg)
{
    struct BgWat2Caustics *c = &((struct BgWat2Work *)world_theme.work)->caustics;
    struct TevStageInfo sp10 = arg->tev;

    tex_coord_gen(sp10.texCoordId, 1, 0, sp10.texMtxId);
    tex_coord_gen(sp10.texCoordId + 1, 1, 0, sp10.texMtxId + 3);
    GXSetIndTexOrder_cached(sp10.tevIndStage, sp10.texCoordId + 1, sp10.texMapId + 1);
    GXSetTevIndirect_cached(sp10.tevStage, sp10.tevIndStage, 0, 0, sp10.unk1C, 0, 0, 0, 0, 0);
    g_GXSetTevOrder_wrapper(sp10.tevStage, sp10.texCoordId, sp10.texMapId, 0xFF);
    GXSetTevSwapMode_cached(sp10.tevStage, 0, 0);
    GXSetTevColorIn_cached(sp10.tevStage, 0xF, 0xF, 0xF, 8);
    GXSetTevColorOp_cached(sp10.tevStage, 0, 0, 0, 1, 2);
    GXSetTevAlphaIn_cached(sp10.tevStage, 7, 7, 7, 0);
    GXSetTevAlphaOp_cached(sp10.tevStage, 0, 0, 0, 1, 0);
    mtxa_push();
    mtxa_mult_left(c->unk170);
    mathutil_mtxA_set_translate_xyz(0.0f, 0.0f, 0.0f);
    GXLoadTexMtxImm(mtxa->mtxA, sp10.texMtxId + 6, 0);
    mtxa_pop();
    GXLoadTexMtxImm(c->unkE0, sp10.postTexMtxId, 0);
    GXLoadTexObj_cached(c->tex4C, sp10.texMapId + 2);
    GXSetTexCoordGen2_cached(sp10.texCoordId + 2, 0, 1, sp10.texMtxId + 6, 1, sp10.postTexMtxId);
    g_GXSetTevIndirect_zero_if_different(sp10.tevStage + 1);
    g_GXSetTevOrder_wrapper(sp10.tevStage + 1, sp10.texCoordId + 2, sp10.texMapId + 2, 0xFF);
    GXSetTevSwapMode_cached(sp10.tevStage + 1, 0, 0);
    GXSetTevColorIn_cached(sp10.tevStage + 1, 0xF, 8, 4, 0);
    GXSetTevColorOp_cached(sp10.tevStage + 1, 0, 0, 0, 1, 0);
    GXSetTevAlphaIn_cached(sp10.tevStage + 1, 7, 7, 7, 0);
    GXSetTevAlphaOp_cached(sp10.tevStage + 1, 0, 0, 0, 1, 0);
    sp10.tevStage += 2;
    sp10.tevIndStage += 1;
    sp10.texCoordId += 3;
    sp10.texMtxId += 9;
    sp10.postTexMtxId += 3;
    sp10.unk1C += 1;
    sp10.texMapId += 3;
    arg->tev = sp10;
}

void fn_1_87A0C(struct BallEnvArg *arg)
{
    struct BgWat2Work *work = world_theme.work;
    struct BgWat2Caustics *c = &work->caustics;
    struct TevStageInfo sp28 = arg->tev;
    float sp10[2][3];

    GXSetBlendMode_cached(1, 1, 1, 0);
    GXLoadTexObj_cached(c->tex48, sp28.texMapId);
    GXLoadTexObj_cached(work->surfaceIndTex, sp28.texMapId + 1);
    sp10[0][0] = 0.2f;
    sp10[0][1] = 0.0f;
    sp10[0][2] = 0.0f;
    sp10[1][0] = 0.0f;
    sp10[1][1] = 0.2f;
    sp10[1][2] = 0.0f;
    g_GXSetIndTexMtx_wrapper(sp28.unk1C, sp10, 0);
    mtxa_push();
    mtxa_mult_left(c->unk110);
    mtxa_scale_s(3.0f);
    GXLoadTexMtxImm(mtxa->mtxA, sp28.texMtxId, 0);
    mtxa_pop();
    mtxa_push();
    mtxa_mult_left(c->unk140);
    mtxa_scale_s(3.0f);
    GXLoadTexMtxImm(mtxa->mtxA, sp28.texMtxId + 3, 0);
    mtxa_pop();
    tex_coord_gen(sp28.texCoordId, 1, 0, sp28.texMtxId);
    tex_coord_gen(sp28.texCoordId + 1, 1, 0, sp28.texMtxId + 3);
    GXSetIndTexOrder_cached(sp28.tevIndStage, sp28.texCoordId + 1, sp28.texMapId + 1);
    GXSetTevIndirect_cached(sp28.tevStage, sp28.tevIndStage, 0, 0, sp28.unk1C, 0, 0, 0, 0, 0);
    g_GXSetTevOrder_wrapper(sp28.tevStage, sp28.texCoordId, sp28.texMapId, 0xFF);
    GXSetTevSwapMode_cached(sp28.tevStage, 0, 0);
    GXSetTevColorIn_cached(sp28.tevStage, 0xF, 0xF, 0xF, 8);
    GXSetTevColorOp_cached(sp28.tevStage, 0, 0, 0, 1, 2);
    GXSetTevAlphaIn_cached(sp28.tevStage, 7, 7, 7, 6);
    GXSetTevAlphaOp_cached(sp28.tevStage, 0, 0, 0, 1, 0);
    mtxa_push();
    mtxa_mult_left(c->unk170);
    mathutil_mtxA_set_translate_xyz(0.0f, 0.0f, 0.0f);
    GXLoadTexMtxImm(mtxa->mtxA, sp28.texMtxId + 6, 0);
    mtxa_pop();
    GXLoadTexMtxImm(c->unkE0, sp28.postTexMtxId, 0);
    GXLoadTexObj_cached(c->tex4C, sp28.texMapId + 2);
    GXSetTexCoordGen2_cached(sp28.texCoordId + 2, 0, 1, sp28.texMtxId + 6, 1, sp28.postTexMtxId);
    g_GXSetTevIndirect_zero_if_different(sp28.tevStage + 1);
    g_GXSetTevOrder_wrapper(sp28.tevStage + 1, sp28.texCoordId + 2, sp28.texMapId + 2, 0xFF);
    GXSetTevSwapMode_cached(sp28.tevStage + 1, 0, 0);
    GXSetTevColorIn_cached(sp28.tevStage + 1, 0xF, 8, 4, 0xF);
    GXSetTevColorOp_cached(sp28.tevStage + 1, 0, 0, 0, 1, 0);
    GXSetTevAlphaIn_cached(sp28.tevStage + 1, 7, 7, 7, 0);
    GXSetTevAlphaOp_cached(sp28.tevStage + 1, 0, 0, 0, 1, 0);
    sp28.tevStage += 2;
    sp28.tevIndStage += 1;
    sp28.texCoordId += 3;
    sp28.texMtxId += 9;
    sp28.postTexMtxId += 3;
    sp28.unk1C += 1;
    sp28.texMapId += 3;
    arg->tev = sp28;
}

void fn_1_87E98(struct BgWat2Work *work)
{
    int i;
    struct BgWat2Cloth *sail;

    if (work->sailCount == 0)
        return;
    work->gustMax = 1.0f + 0.5f * RAND_FLOAT();
    work->gustMin = 0.2f * (work->gustMax * RAND_FLOAT()) - 0.05f * RAND_FLOAT();
    work->gustAngle = 0;
    work->gustAngleVel = 136.53334f * (0.5f + RAND_FLOAT());
    work->windX = RAND_FLOAT() - 0.5f;
    work->windY = RAND_FLOAT() - 0.5f;
    work->windAngleX = rand() & 0x7FFF;
    work->windAngleY = rand() & 0x7FFF;
    work->windAngleXVel = 546.13336f * (RAND_FLOAT() - 0.5f);
    work->windAngleYVel = 546.13336f * (RAND_FLOAT() - 0.5f);
    sail = work->sails;
    i = work->sailCount;
    for (; i > 0; i--, sail++)
    {
        sail->count = sail->model->model->mtxCount;
        sail->segs = alloc_from_heap_or_panic(bg_heap, sail->count * sizeof(struct BgWat2SailSeg), "bg_wat2.c", 0x8E2);
        fn_1_88D34(sail);
    }
}

void fn_1_8815C(struct BgWat2Work *work)
{
    int i;
    struct BgWat2Cloth *sail;

    if (work->sailCount == 0)
        return;
    sail = work->sails;
    i = work->sailCount;
    for (; i > 0; i--, sail++)
    {
        if (sail->segs != NULL)
            OSFreeToHeap(bg_heap, sail->segs);
    }
}

void fn_1_881D0(struct BgWat2Work *work)
{
    int i;
    int n;
    struct BgWat2Cloth *sail;
    struct BgWat2SailSeg *seg;
    struct BgWat2SailSeg *parent;
    struct StageBgModel *model;
    float t;
    float gz;
    float gy;
    float gx;
    float dot;
    Vec sp48;
    Vec sp3C;
    Vec sp30;
    Vec sp24;
    Vec sp18;
    Vec spC;
    s16 spA;
    s16 sp8;

    if (work->sailCount == 0)
        return;
    if ((work->unk4 & 1) == 0 && mode_info.timerCurr < 660)
    {
        work->unk4 |= 1;
        work->gustMax *= 1.5f + RAND_FLOAT();
        work->gustMin *= 1.5f + RAND_FLOAT();
        work->gustAngleVel *= 1.5f + RAND_FLOAT();
    }
    work->gustAngle += work->gustAngleVel;
    work->windAngleX += work->windAngleXVel;
    work->windAngleY += work->windAngleYVel;
    dot = 0.5f * (1.0f + math_sin(work->gustAngle));
    gy = work->gustMax * dot + work->gustMin * (1.0f - dot);
    model = work->ship;
    mtxa_from_rotate_z(model->rotZ);
    mtxa_rotate_y(model->rotY);
    mtxa_rotate_x(model->rotX);
    gx = gy * work->windY * math_sin(work->windAngleY);
    mtxa_tf_vec_xyz(gy * work->windX * math_sin(work->windAngleX), gx, gy, &work->current);
    sp18 = work->current;
    sail = work->sails;
    i = work->sailCount;
    for (; i > 0; i--, sail++)
    {
        model = sail->model;
        mtxa_from_rotate_z(model->rotZ);
        mtxa_rotate_y(model->rotY);
        mtxa_rotate_x(model->rotX);
        gx = 0.0f;
        gy = -1.0f;
        gz = 0.0f;
        mtxa_rigid_inv_tf_vec(&sp18, &spC);
        seg = sail->segs;
        n = sail->count;
        for (; n > 0; n--, seg++)
        {
            parent = seg->parent;
            if (parent == NULL)
                continue;
            seg->vel.x += 0.016f * gx;
            seg->vel.y += 0.016f * gy;
            seg->vel.z += 0.016f * gz;
            if (seg->len > 0.0f)
            {
                sp3C.x = 0.0f;
                sp3C.y = 0.0f;
                sp3C.z = 0.0f;
                sp48.x = seg->pos.x - parent->pos.x;
                sp48.y = seg->pos.y - parent->pos.y;
                sp48.z = seg->pos.z - parent->pos.z;
                vec_normalize_len(&sp48);
                sp24 = parent->side;
                dot = mathutil_vec_dot_prod(&sp24, &sp48);
                sp3C.x += -0.03f * (sp24.x * dot);
                sp3C.y += -0.03f * (sp24.y * dot);
                sp3C.z += -0.03f * (sp24.z * dot);
                sp30 = parent->dir;
                dot = mathutil_vec_dot_prod(&sp30, &sp48);
                sp3C.x += -0.01f * (sp30.x * dot);
                sp3C.y += -0.01f * (sp30.y * dot);
                sp3C.z += -0.01f * (sp30.z * dot);
                dot = 1.0f / seg->len;
                sp3C.x *= dot;
                sp3C.y *= dot;
                sp3C.z *= dot;
                seg->vel.x += sp3C.x;
                seg->vel.y += sp3C.y;
                seg->vel.z += sp3C.z;
                parent->vel.x -= sp3C.x;
                parent->vel.y -= sp3C.y;
                parent->vel.z -= sp3C.z;
            }
            vec_to_euler_xy(&seg->dir, &spA, &sp8);
            mtxa_from_rotate_y(sp8);
            mtxa_rotate_x(spA);
            mtxa_tf_vec(&seg->vel, &sp3C);
            mtxa_tf_vec(&spC, &sp30);
            sp3C.x += 0.001f * (sp30.x - sp3C.x);
            sp3C.y += 0.001f * (sp30.y - sp3C.y);
            sp3C.z += 0.06f * (sp30.z - sp3C.z);
            mtxa_rigid_inv_tf_vec(&sp3C, &seg->vel);
            seg->pos.x += seg->vel.x;
            seg->pos.y += seg->vel.y;
            seg->pos.z += seg->vel.z;
            if (seg->pos.z < 0.0f)
            {
                seg->pos.z = 0.0f;
                if (seg->vel.z < 0.0f)
                    seg->vel.z *= -0.8f;
            }
            sp48.x = seg->pos.x - parent->pos.x;
            sp48.y = seg->pos.y - parent->pos.y;
            sp48.z = seg->pos.z - parent->pos.z;
            vec_normalize_len(&sp48);
            dot = seg->len;
            sp3C.x = parent->pos.x + sp48.x * dot;
            sp3C.y = parent->pos.y + sp48.y * dot;
            sp3C.z = parent->pos.z + sp48.z * dot;
            seg->vel.x += sp3C.x - seg->pos.x;
            seg->vel.y += sp3C.y - seg->pos.y;
            seg->vel.z += sp3C.z - seg->pos.z;
            seg->pos = sp3C;
            dot = mathutil_vec_dot_prod(&sp48, &seg->vel);
            seg->vel.x -= dot * sp48.x;
            seg->vel.y -= dot * sp48.y;
            seg->vel.z -= dot * sp48.z;
            parent->vel.x -= dot * sp48.x;
            parent->vel.y -= dot * sp48.y;
            parent->vel.z -= dot * sp48.z;
            seg->side.x = 1.0f;
            seg->side.y = 0.0f;
            seg->side.z = -0.3f * seg->vel.x;
            vec_normalize_len(&seg->side);
            mathutil_vec_cross_prod(&seg->side, &sp48, &sp30);
            vec_normalize_len(&sp30);
            sp30.x += 0.05f * parent->dir.x;
            sp30.y += 0.05f * parent->dir.y;
            sp30.z += 0.05f * parent->dir.z;
            vec_normalize_len(&sp30);
            seg->dir.x += 0.4f * (sp30.x - seg->dir.x);
            seg->dir.y += 0.4f * (sp30.y - seg->dir.y);
            seg->dir.z += 0.4f * (sp30.z - seg->dir.z);
        }
    }
}

void fn_1_88AA4(struct BgWat2Work *work)
{
    u32 mask;
    int i;
    struct BgWat2Cloth *sail;
    struct StageBgModel *model;
    struct OrdTblNode *list;
    struct BgWat2SailNode *node;
    struct GMAModel *gma;
    void *draw;

    if (work->sailCount == 0)
        return;
    mask = g_smth_with_challenge_mode_var_and_3p();
    draw = fn_1_88BC8;
    sail = work->sails;
    i = work->sailCount;
    for (; i > 0; i--, sail++)
    {
        model = sail->model;
        gma = model->model;
        if (model->flags & mask)
        {
            model->flags &= ~0x10000;
            mtxa_from_mtx(g_ord_tbl_stuff->matrices[0]);
            mtxa_translate(&model->pos);
            mtxa_rotate_z(model->rotZ);
            mtxa_rotate_y(model->rotY);
            mtxa_rotate_x(model->rotX);
            mtxa_scale(&model->scale);
            if (g_is_sphere_visible(&gma->boundSphereCenter, gma->boundSphereRadius))
            {
                list = ord_tbl_get_entry_for_pos(&gma->boundSphereCenter);
                node = ord_tbl_alloc_node(sizeof(*node));
                if (node != NULL)
                {
                    node->node.drawFunc = draw;
                    node->lightGroup = get_active_light_group();
                    mtxa_to_mtx(node->mtx);
                    node->model = model;
                    node->sail = sail;
                    fn_1_CD418(list, &node->node);
                }
            }
        }
    }
}

void fn_1_88BC8(struct BgWat2SailNode *node)
{
    struct GMAModel *gma;
    struct BgWat2Cloth *sail;
    Mtx *mtxs;
    Mtx *mtx;
    struct BgWat2SailSeg *seg;
    int i;
    Vec sp18;
    Quat sp8;

    sail = node->sail;
    gma = node->model->model;
    load_light_group_cached(node->lightGroup);
    sp18.x = 0.0f;
    sp18.y = 0.0f;
    sp18.z = -1.0f;
    seg = sail->segs;
    mtxs = g_some_buffer_ptr;
    mtx = mtxs;
    i = sail->count;
    for (; i > 0; i--, mtx++, seg++)
    {
        quat_from_dirs(&sp8, &sp18, &seg->dir);
        mtxa_from_quat(&sp8);
        mtxa_mult_right(seg->mtx);
        mathutil_mtxA_set_translate(&seg->pos);
        mtxa_to_mtx(*mtx);
    }
    pointer_range_advance(gma, mtxs);
    mtxa_from_mtx(node->mtx);
    load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
    if (world_theme.unk90 == NULL || (g_some_render_flag.flags & 0x80))
    {
        avdisp_draw_model_unculled_sort_never(gma);
    }
    else
    {
        g_yet_another_unk_draw_func(world_theme.unk90);
        avdisp_draw_model_unculled_sort_never(gma);
        g_yet_another_unk_draw_func(NULL);
    }
}

void fn_1_88D34(struct BgWat2Cloth *sail)
{
    int i;
    struct BgWat2SailSeg *seg;
    Mtx *mtx;
    Mtx *mtxs;
    s16 *parents;
    struct GMAModel *gma;
    Mtx **ptrs;
    Mtx **p;
    int n;
    struct BgWat2SailSeg *parent;

    n = sail->count;
    gma = sail->model->model;
    ptrs = g_some_buffer_ptr;
    mtxs = (Mtx *)(ptrs + n);
    p = ptrs;
    mtx = mtxs;
    i = n;
    for (; i > 0; i--, p++, mtx++)
        *p = mtx;
    draw_poly(gma, ptrs);
    mtx = mtxs;
    seg = sail->segs;
    i = sail->count;
    for (; i > 0; i--, seg++, mtx++)
    {
        mtxa_from_mtx(*mtx);
        mathutil_mtxA_get_translate(&seg->pos);
        mathutil_mtxA_set_translate_xyz(0.0f, 0.0f, 0.0f);
        mtxa_to_mtx(seg->mtx);
        seg->dir.x = 0.0f;
        seg->dir.y = 0.0f;
        seg->dir.z = -1.0f;
        seg->side.x = 1.0f;
        seg->side.y = 0.0f;
        seg->side.z = 0.0f;
        seg->vel.x = RAND_FLOAT() - 0.5f;
        seg->vel.y = RAND_FLOAT() - 0.5f;
        seg->vel.z = RAND_FLOAT() - 0.5f;
        seg->parent = NULL;
    }
    parents = fn_80069964(gma);
    seg = sail->segs;
    i = sail->count;
    for (; i > 0; i--, seg++, parents++)
    {
        if (*parents >= 0)
        {
            parent = &((struct BgWat2SailSeg *)sail->segs)[*parents];
            seg->parent = parent;
            seg->len = mathutil_vec_distance(&parent->pos, &seg->pos);
        }
    }
}

void fn_1_88F94(struct BgWat2Work *work)
{
    int i;
    struct BgWat2Cloth *rope;

    rope = work->ropes;
    i = work->ropeCount;
    for (; i > 0; i--, rope++)
    {
        rope->count = rope->model->model->mtxCount;
        rope->segs = alloc_from_heap_or_panic(bg_heap, rope->count * sizeof(struct BgWat2RopeSeg), "bg_wat2.c", 0xAFB);
        fn_1_89998(rope);
    }
}

void fn_1_8901C(struct BgWat2Work *work)
{
    int i;
    struct BgWat2Cloth *rope;

    rope = work->ropes;
    i = work->ropeCount;
    for (; i > 0; i--, rope++)
    {
        if (rope->segs != NULL)
            OSFreeToHeap(bg_heap, rope->segs);
    }
}

void fn_1_89084(struct BgWat2Work *work)
{
    int i;
    int n;
    struct BgWat2Cloth *rope;
    struct BgWat2RopeSeg *seg;
    struct BgWat2RopeSeg *prev;
    struct BgWat2RopeSeg *next;
    struct StageBgModel *model;
    float distNext;
    float distPrev;
    float lenPrev;
    float lenNext;
    Vec sp5C;
    Vec sp50;
    Vec sp44;
    Vec sp38;
    Vec sp2C;
    Vec sp20;
    Vec sp14;
    Vec sp8;
    float t;

    sp38 = work->current;
    rope = work->ropes;
    i = work->ropeCount;
    for (; i > 0; i--, rope++)
    {
        model = rope->model;
        mtxa_from_rotate_z(model->rotZ);
        mtxa_rotate_y(model->rotY);
        mtxa_rotate_x(model->rotX);
        mtxa_rigid_inv_tf_vec_xyz(0.0f, -1.0f, 0.0f, &sp20);
        mtxa_rigid_inv_tf_vec(&sp38, &sp2C);
        seg = rope->segs;
        n = rope->count;
        for (; n > 0; n--, seg++)
        {
            prev = seg->prev;
            next = seg->next;
            lenPrev = seg->lenPrev;
            lenNext = seg->lenNext;
            if (prev != NULL)
            {
                sp50.x = prev->pos.x - seg->pos.x;
                sp50.y = prev->pos.y - seg->pos.y;
                sp50.z = prev->pos.z - seg->pos.z;
                distPrev = vec_normalize_len(&sp50);
            }
            if (next != NULL)
            {
                sp44.x = next->pos.x - seg->pos.x;
                sp44.y = next->pos.y - seg->pos.y;
                sp44.z = next->pos.z - seg->pos.z;
                distNext = vec_normalize_len(&sp44);
            }
            if (!(seg->flags & 1))
            {
                if (prev != NULL)
                {
                    t = 0.08f * (distPrev - lenPrev);
                    sp14.x = sp50.x * t;
                    sp14.y = sp50.y * t;
                    sp14.z = sp50.z * t;
                    seg->vel.x += sp14.x;
                    seg->vel.y += sp14.y;
                    seg->vel.z += sp14.z;
                }
                if (next != NULL)
                {
                    t = 0.12f * (distNext - lenNext);
                    sp14.x = sp44.x * t;
                    sp14.y = sp44.y * t;
                    sp14.z = sp44.z * t;
                    seg->vel.x += sp14.x;
                    seg->vel.y += sp14.y;
                    seg->vel.z += sp14.z;
                }
                seg->vel.x += 0.004f * sp20.x;
                seg->vel.y += 0.004f * sp20.y;
                seg->vel.z += 0.004f * sp20.z;
                t = 1.0 - fabs(vec_dot_normalized_safe(&seg->dir, &sp2C));
                t = seg->unk70 + seg->unk74 * t;
                seg->vel.x += t * (sp2C.x - seg->vel.x);
                seg->vel.y += t * (sp2C.y - seg->vel.y);
                seg->vel.z += t * (sp2C.z - seg->vel.z);
                seg->pos.x += seg->vel.x;
                seg->pos.y += seg->vel.y;
                seg->pos.z += seg->vel.z;
                if (prev != NULL)
                {
                    lenNext = 0.98f * lenPrev;
                    lenPrev = 1.02f * lenPrev;
                    if (prev->flags & 1)
                        sp5C = *prev->anchor;
                    else
                        sp5C = prev->pos;
                    sp8 = sp5C;
                    sp14.x = seg->pos.x - sp8.x;
                    sp14.y = seg->pos.y - sp8.y;
                    sp14.z = seg->pos.z - sp8.z;
                    t = vec_normalize_len(&sp14);
                    if (t > lenPrev)
                    {
                        seg->pos.x = sp8.x + lenPrev * sp14.x;
                        seg->pos.y = sp8.y + lenPrev * sp14.y;
                        seg->pos.z = sp8.z + lenPrev * sp14.z;
                        t = mathutil_vec_dot_prod(&sp14, &seg->vel);
                        if (t > 0.0f)
                        {
                            t *= -1.5f;
                            seg->vel.x += t * sp14.x;
                            seg->vel.y += t * sp14.y;
                            seg->vel.z += t * sp14.z;
                        }
                    }
                    else if (t < lenNext)
                    {
                        seg->pos.x = sp8.x + lenNext * sp14.x;
                        seg->pos.y = sp8.y + lenNext * sp14.y;
                        seg->pos.z = sp8.z + lenNext * sp14.z;
                        t = mathutil_vec_dot_prod(&sp14, &seg->vel);
                        if (t < 0.0f)
                        {
                            t *= -1.5f;
                            seg->vel.x += t * sp14.x;
                            seg->vel.y += t * sp14.y;
                            seg->vel.z += t * sp14.z;
                        }
                    }
                }
            }
            if (!(seg->flags & 2))
            {
                sp14.x = 0.0f;
                sp14.y = 0.0f;
                sp14.z = 0.0f;
                if (next != NULL)
                {
                    sp14.x += sp44.x;
                    sp14.y += sp44.y;
                    sp14.z += sp44.z;
                }
                if (0.0f != sp14.x || 0.0f != sp14.y || 0.0f != sp14.z)
                {
                    vec_normalize_len(&sp14);
                    seg->dir.x += 0.5f * (sp14.x - seg->dir.x);
                    seg->dir.y += 0.5f * (sp14.y - seg->dir.y);
                    seg->dir.z += 0.5f * (sp14.z - seg->dir.z);
                    vec_normalize_len(&seg->dir);
                }
                t = 24576.0f * (seg->vel.x + 0.5f * seg->vel.y);
                if (prev != NULL)
                    t += 0.5f * (prev->twist - t);
                if (next != NULL)
                    t += 0.5f * (next->twist - t);
                seg->twistVel += 0.1f * (0.25f * (t - seg->twist) - seg->twistVel);
                seg->twistVel *= 0.98f;
                seg->twist += seg->twistVel;
            }
        }
    }
}

void fn_1_897C8(struct BgWat2Work *work)
{
    int i;
    struct BgWat2Cloth *rope;
    struct GMAModel *gma;
    u32 mask;
    struct StageBgModel *model;
    int n;
    struct BgWat2RopeSeg *seg;
    Mtx *mtxs;
    Mtx *mtx;
    Quat sp8;

    mask = g_smth_with_challenge_mode_var_and_3p();
    rope = work->ropes;
    i = work->ropeCount;
    for (; i > 0; i--, rope++)
    {
        model = rope->model;
        gma = model->model;
        if (model->flags & mask)
        {
            model->flags &= ~0x10000;
            mtxa_from_mtx(g_ord_tbl_stuff->matrices[0]);
            mtxa_translate(&model->pos);
            mtxa_rotate_z(model->rotZ);
            mtxa_rotate_y(model->rotY);
            mtxa_rotate_x(model->rotX);
            mtxa_scale(&model->scale);
            if (g_is_sphere_visible(&gma->boundSphereCenter, gma->boundSphereRadius))
            {
                mtxa_push();
                seg = rope->segs;
                mtxs = g_some_buffer_ptr;
                mtx = mtxs;
                n = rope->count;
                for (; n > 0; n--, mtx++, seg++)
                {
                    if (seg->flags & 2)
                    {
                        mtxa_from_mtx(seg->mtx);
                    }
                    else
                    {
                        quat_from_dirs(&sp8, &seg->rest, &seg->dir);
                        mtxa_from_quat(&sp8);
                        mtxa_mult_right(seg->mtx);
                        mtxa_rotate_x(seg->twist);
                    }
                    if (seg->flags & 1)
                        mathutil_mtxA_set_translate(seg->anchor);
                    else
                        mathutil_mtxA_set_translate(&seg->pos);
                    mtxa_to_mtx(*mtx);
                }
                pointer_range_advance(gma, mtxs);
                mtxa_pop();
                load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
                avdisp_draw_model_unculled_sort_never(gma);
            }
        }
    }
}

void fn_1_89998(struct BgWat2Cloth *rope)
{
    int i;
    struct BgWat2RopeSeg *seg;
    Mtx *mtx;
    Mtx *mtxs;
    s16 *parents;
    struct BgWat2RopeSeg *other;
    struct GMAModel *gma;
    Mtx **ptrs;
    Mtx **p;
    int n;
    struct BgWat2RopeSeg *next;
    Vec sp8;

    n = rope->count;
    gma = rope->model->model;
    ptrs = g_some_buffer_ptr;
    mtxs = (Mtx *)(ptrs + n);
    p = ptrs;
    mtx = mtxs;
    i = n;
    for (; i > 0; i--, p++, mtx++)
        *p = mtx;
    draw_poly(gma, ptrs);
    mtx = mtxs;
    seg = rope->segs;
    i = rope->count;
    for (; i > 0; i--, seg++, mtx++)
    {
        mtxa_from_mtx(*mtx);
        mathutil_mtxA_get_translate(&seg->pos);
        mathutil_mtxA_set_translate_xyz(0.0f, 0.0f, 0.0f);
        mtxa_to_mtx(seg->mtx);
        seg->vel.x = RAND_FLOAT() - 0.5f;
        seg->vel.y = RAND_FLOAT() - 0.5f;
        seg->vel.z = RAND_FLOAT() - 0.5f;
        seg->rest.x = 0.0f;
        seg->rest.y = 0.0f;
        seg->rest.z = -1.0f;
        seg->dir = seg->rest;
        seg->twist = 0.0f;
        seg->twistVel = 0.0f;
        seg->flags = 0;
        seg->unk70 = 0.01f * (0.5f + RAND_FLOAT());
        seg->unk74 = 0.075f * (0.5f + RAND_FLOAT());
        seg->prev = NULL;
        seg->lenPrev = 0.0f;
        seg->next = NULL;
        seg->lenNext = 0.0f;
    }
    parents = fn_80069964(gma);
    seg = rope->segs;
    i = rope->count;
    for (; i > 0; i--, seg++, parents++)
    {
        if (*parents >= 0)
        {
            other = &((struct BgWat2RopeSeg *)rope->segs)[*parents];
            other->next = seg;
            seg->prev = other;
            sp8.x = seg->pos.x - other->pos.x;
            sp8.y = seg->pos.y - other->pos.y;
            sp8.z = seg->pos.z - other->pos.z;
            seg->lenPrev = mathutil_vec_len(&sp8);
            other->lenNext = seg->lenPrev;
        }
    }
    seg = rope->segs;
    i = rope->count;
    for (; i > 0; i--, seg++)
    {
        if (0.0f == seg->lenPrev || 0.0f == seg->lenNext)
        {
            seg->flags |= 1;
            seg->anchor = &seg->pos;
        }
        if (0.0f == seg->lenNext)
        {
            seg->flags |= 2;
        }
        else
        {
            next = seg->next;
            sp8.x = 0.0f;
            sp8.y = 0.0f;
            sp8.z = 0.0f;
            sp8.x += next->pos.x - seg->pos.x;
            sp8.y += next->pos.y - seg->pos.y;
            sp8.z += next->pos.z - seg->pos.z;
            vec_normalize_len(&sp8);
            seg->rest = sp8;
            seg->dir = seg->rest;
        }
    }
    seg = rope->segs;
    i = rope->count;
    for (; i > 0; i--, seg++)
    {
        if (0.0f != seg->lenNext && (seg->flags & 1))
        {
            for (other = seg; other != NULL; other = other->prev)
            {
                if (other->prev == NULL || (!seg->flags & 1))
                    seg->anchor = &seg->pos;
            }
        }
    }
}

int fn_1_89E40(int index, struct GMAModelEntry *entry)
{
    struct BgWat2Work *work = world_theme.work;

    switch (index)
    {
    case 0:
        work->surfaceModel = entry->model;
        work->surfaceTex = entry->model->texObjs;
        work->caustics.tex44 = entry->model->texObjs;
        work->caustics.tex48 = entry->model->texObjs;
        break;
    case 1:
        work->pokoLight = entry->model;
        break;
    case 2:
        work->bubbleModel = entry->model;
        work->bubbleTex = entry->model->texObjs;
        work->bubbleMaskTex = entry->model->texObjs + 1;
        break;
    case 3:
        work->caustics.tex4C = entry->model->texObjs;
        break;
    case 4:
        work->surfaceIndTex = entry->model->texObjs;
        break;
    }
    return 1;
}

int fn_1_89F00(int index, struct StageBgModel *model)
{
    struct BgWat2Work *work = world_theme.work;
    struct GMAModel *gma;

    switch (index)
    {
    case 0:
        work->surface = model;
        break;
    case 1:
        if (model->model != NULL && work->pokoCount < 32)
        {
            work->pokos[work->pokoCount].model = model;
            work->pokoCount++;
        }
        break;
    case 3:
        work->ship = model;
        break;
    case 4:
        model->flags |= 0x1000000;
        gma = model->model;
        if (gma != NULL && (gma->flags & 8) && work->sailCount < 4)
        {
            work->sails[work->sailCount].model = model;
            work->sailCount++;
        }
        break;
    case 5:
        gma = model->model;
        if (gma != NULL && (gma->flags & 8) && work->ropeCount < work->ropeMax)
        {
            work->ropes[work->ropeCount].model = model;
            work->ropeCount++;
        }
        break;
    }
    return 1;
}
