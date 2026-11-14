/* Gear background: river surface rendered with a mirrored off-screen reflection
 * texture, two scrolling ripple layers and a sun glint projected from the sun model. */

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

struct BgGearRiver
{
    /*0x000*/ struct StageBgModel *model;
    /*0x004*/ Vec pos1;
    /*0x010*/ Vec vel1;
    /*0x01C*/ Vec scale1;
    /*0x028*/ Vec pos2;
    /*0x034*/ Vec vel2;
    /*0x040*/ Vec scale2;
    /*0x04C*/ Mtx mtx;
    /*0x07C*/ Mtx projTexMtx;
    /*0x0AC*/ Mtx sunTexMtx;
    /*0x0DC*/ s32 texReady;
    /*0x0E0*/ GXTexObj reflTexObj;
    /*0x100*/ void *reflTexData;
    /*0x104*/ GXTexObj testTexObj;
    /*0x124*/ GXTexObj *texObjs;
};  /* size = 0x128 */

struct BgGearSun
{
    /*0x00*/ struct StageBgModel *model;
    /*0x04*/ Vec pos;
    /*0x10*/ float radius;
};  /* size = 0x14 */

struct BgGearWork
{
    /*0x000*/ s32 initialized;
    /*0x004*/ struct BgGearRiver river;
    /*0x12C*/ Vec scale;
    /*0x138*/ struct BgGearSun sun;
    /*0x14C*/ s32 count;
    /*0x150*/ struct StageBgModel *models[16];
};  /* size = 0x190 */

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
extern void *stage_heap;

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
int GXGetTexObjFmt(GXTexObj *obj);
u16 GXGetTexObjHeight(GXTexObj *obj);
u16 GXGetTexObjWidth(GXTexObj *obj);
void *GXGetTexObjData(GXTexObj *obj);
void ord_tbl_draw_nodes(void);
void fn_1_D8130(int, int, struct RenderEffect *);
void g_smth_with_bg_model_names(struct BgModelSearch *search, int (*callback)(int, struct GMAModelEntry *));
void GXLoadTexObj_cached(GXTexObj *obj, int map);
void GXLoadTexMtxImm(Mtx m, int id, int type);
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
void g_GXSetIndTexMtx_wrapper(int id, float mtx[2][3], int scale);
void g_GXSetTevOrder_wrapper(int stage, int coord, int map, int color);
void g_GXSetTevIndirect_zero_if_different(int stage);
void mtxa_rigid_inv_tf_vec_xyz(float x, float y, float z, Vec *dst);
void *g_yet_another_unk_draw_func(void *func);
void C_MTXLookAt(Mtx m, Vec *eye, Vec *up, Vec *target);

void fn_1_9DB08(struct BgGearWork *work, int mask);
void g_something_with_stage_heap_and_gear_theme(struct BgGearWork *work);
void fn_1_9DF74(struct BgGearWork *work);
void fn_1_9DFD4(struct BgGearWork *work, int mask);
void fn_1_9E048(struct BgGearWork *work, int mask);
void fn_1_9E440(struct BallEnvArg *arg);
void fn_1_9E9C0(struct RenderEffect *);
void fn_1_9E9D4(struct RenderEffect *);
void fn_1_9E9D8(struct RenderEffect *);
void fn_1_9E9DC(int, struct RenderEffect *);
void fn_1_9EA00(struct RenderEffect *);
int fn_1_9F214(int, struct GMAModelEntry *);
int fn_1_9F2B4(int, struct StageBgModel *);

/* Unreferenced table (kept for data layout) */
static u32 lbl_1_data_32F28[0x5C] =
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
    { BG_MDL_CMP_FULL, "GEAR_RIVER_TEST" },
    { BG_MDL_CMP_END, NULL },
};

static struct BgModelSearch s_bgModelSearch[] =
{
    { BG_MDL_CMP_FULL, "GEAR_RIVER" },
    { BG_MDL_CMP_FULL, "GEAR_SUN" },
    { BG_MDL_CMP_FULL, "GEAR_SKY" },
    { BG_MDL_CMP_PREFIX, "GEAR_GROUND" },
    { BG_MDL_CMP_PREFIX, "GEAR_CATSLE" },
    { BG_MDL_CMP_FULL, "GEAR_SKY_UP" },
    { BG_MDL_CMP_PREFIX, "GEAR_BRIDGE" },
    { BG_MDL_CMP_END, NULL },
};

static struct RenderEffectFuncs s_renderEffectFuncs =
{
    fn_1_9E9C0,
    fn_1_9E9D4,
    fn_1_9E9D8,
    fn_1_9E9DC,
};

void bg_gear_init(void)
{
    struct BgGearWork *work = world_theme.work;
    struct RenderEffect rendEfc;

    bg_init_base();
    if (!work->initialized)
    {
        g_smth_with_bg_model_names(s_modelSearch, fn_1_9F214);
        work->river.texObjs = init_common_gma->modelEntries[0x5F].model->texObjs;
        work->initialized = TRUE;
    }
    work->count = 0;
    g_init_stagedef_bg_fg_models(
        stagedef->bgModels,
        stagedef->bgModelCount,
        s_bgModelSearch,
        fn_1_9F2B4);
    g_init_stagedef_bg_fg_models(
        stagedef->fgModels,
        stagedef->fgModelCount,
        s_bgModelSearch,
        fn_1_9F2B4);
    g_something_with_stage_heap_and_gear_theme(work);

    memset(&rendEfc, 0, sizeof(rendEfc));
    rendEfc.cameraMask = 0xFFFF;
    rendEfc.funcs = &s_renderEffectFuncs;
    fn_1_D8130(0, 0, &rendEfc);
}

void bg_gear_author_tick(void)
{
    struct BgGearWork *work = world_theme.work;
    int mask;

    mask = g_smth_with_challenge_mode_var_and_3p();
    bg_tick_base();
    if (!is_game_paused_and_in_view_stage())
    {
        fn_1_9DB08(work, mask);
        fn_1_9DFD4(work, mask);
    }
}

void bg_gear_author_dest(void)
{
    struct BgGearWork *work = world_theme.work;

    fn_1_9DF74(work);
}

void bg_gear_author_disp(void)
{
    struct BgGearWork *work = world_theme.work;
    int mask;

    mask = g_smth_with_challenge_mode_var_and_3p();
    fn_1_9E048(work, mask);
    bg_disp_base();
}

void bg_gear_item_coin_coli(void) {}

void fn_1_9DB08(struct BgGearWork *work, int mask)
{
    struct BgGearSun *sun = &work->sun;
    struct StageBgModel *bg = sun->model;
    struct GMAModel *model;

    if (bg == NULL || !(bg->flags & mask))
    {
        sun->pos.x = -5740.0f;
        sun->pos.y = 740.0f;
        sun->pos.z = 15200.0f;
        sun->radius = 1000.0f;
        mtxa_from_identity();
        mtxa_scale(&work->scale);
        mtxa_tf_point(&sun->pos, &sun->pos);
        sun->radius *= work->scale.x;
    }
    else
    {
        model = bg->model;
        mtxa_from_translate(&bg->pos);
        mtxa_rotate_z(bg->rotZ);
        mtxa_rotate_y(bg->rotY);
        mtxa_rotate_x(bg->rotX);
        mtxa_scale(&bg->scale);
        mtxa_tf_point(&model->boundSphereCenter, &sun->pos);
        sun->radius = model->boundSphereRadius * bg->scale.x;
    }
}

void g_something_with_stage_heap_and_gear_theme(struct BgGearWork *work)
{
    struct BgGearRiver *river = &work->river;

    if (river->model != NULL)
    {
        river->texReady = 0;
        river->reflTexData = alloc_from_heap_or_panic(stage_heap, GXGetTexBufferSize(128, 128, 4, 0, 0), "bg_gear.c", 295);
        river->pos1.x = RAND_FLOAT();
        river->pos1.y = RAND_FLOAT();
        river->pos1.z = 0.0f;
        river->vel1.x = 0.0027777778f * (0.8f + 0.2f * RAND_FLOAT());
        river->vel1.y = 0.0027777778f * (RAND_FLOAT() - 0.5f);
        river->vel1.z = 0.0f;
        river->scale1.x = 2.0f * (0.8f + 0.4f * RAND_FLOAT());
        river->scale1.y = 2.0f * (0.8f + 0.4f * RAND_FLOAT());
        river->scale1.z = 1.0f;
        river->pos2.x = RAND_FLOAT();
        river->pos2.y = RAND_FLOAT();
        river->pos2.z = 0.0f;
        river->vel2.x = 0.0055555557f * (0.8f + 0.4f * RAND_FLOAT());
        river->vel2.y = 0.011111111f * (RAND_FLOAT() - 0.5f);
        river->vel2.z = 0.0f;
        river->scale2.x = 2.0f * (0.8f + 0.4f * RAND_FLOAT());
        river->scale2.y = 2.0f * (0.8f + 0.4f * RAND_FLOAT());
        river->scale2.z = 1.0f;
    }
}

void fn_1_9DF74(struct BgGearWork *work)
{
    struct BgGearRiver *river = &work->river;

    if (river->model != NULL && river->reflTexData != NULL)
    {
        gp_wait_wrapper();
        OSFreeToHeap(stage_heap, river->reflTexData);
        river->reflTexData = NULL;
    }
}

void fn_1_9DFD4(struct BgGearWork *work, int mask)
{
    struct BgGearRiver *river = &work->river;

    if (river->model != NULL)
    {
        river->pos1.x += river->vel1.x;
        river->pos1.y += river->vel1.y;
        river->pos1.z += river->vel1.z;
        river->pos2.x += river->vel2.x;
        river->pos2.y += river->vel2.y;
        river->pos2.z += river->vel2.z;
    }
}

void fn_1_9E048(struct BgGearWork *work, int mask)
{
    struct BgGearRiver *river = &work->river;
    struct StageBgModel *bg = river->model;
    void *prevFunc;
    float s;
    Mtx viewModel;
    Mtx modelMtx;
    Mtx lookAt;
    float proj[7];
    Vec origin;
    Vec normal;
    Vec sun;
    Vec eye;
    Vec camDir;
    Vec d;

    if (bg != NULL && (bg->flags & mask) && river->texReady)
    {
        bg->flags &= ~(1 << 16);
        mtxa_from_translate(&bg->pos);
        mtxa_rotate_z(bg->rotZ);
        mtxa_rotate_y(bg->rotY);
        mtxa_rotate_x(bg->rotX);
        mtxa_scale(&bg->scale);
        mtxa_to_mtx(modelMtx);
        mtxa_mult_left(g_ord_tbl_stuff->matrices[0]);
        GXGetProjectionv(proj);
        mtxa_to_mtx(viewModel);

        mtxa_push();
        mtxa_from_identity();
        mtxa->mtxA[0][0] = 0.5f * proj[1];
        mtxa->mtxA[0][2] = 0.5f + 0.5f * proj[2];
        mtxa->mtxA[1][1] = 0.5f * proj[3];
        mtxa->mtxA[1][2] = 0.5f + 0.5f * proj[4];
        mtxa_mult_right(viewModel);
        mtxa_to_mtx(river->projTexMtx);
        mtxa_pop();

        mtxa_push();
        mtxa_to_mtx(viewModel);
        mtxa_from_mtx(g_ord_tbl_stuff->matrices[0]);
        mtxa_rigid_inv_tf_tl(&eye);
        mtxa_rigid_inv_tf_vec_xyz(0.0f, 0.0f, -1.0f, &camDir);
        mtxa_from_mtx(river->mtx);
        mathutil_mtxA_get_translate(&origin);
        mtxa_tf_vec_xyz(0.0f, 1.0f, 0.0f, &normal);
        sun = work->sun.pos;
        d.x = sun.x - origin.x;
        d.y = sun.y - origin.y;
        d.z = sun.z - origin.z;
        s = -2.0f * mathutil_vec_dot_prod(&d, &normal);
        d.x += s * normal.x;
        d.y += s * normal.y;
        d.z += s * normal.z;
        sun.x = origin.x + d.x;
        sun.y = origin.y + d.y;
        sun.z = origin.z + d.z;
        s = 0.5f * (mathutil_vec_distance(&eye, &sun) / work->sun.radius);
        C_MTXLookAt(lookAt, &eye, &normal, &sun);
        mtxa_from_identity();
        mtxa->mtxA[0][0] = s;
        mtxa->mtxA[0][2] = 1.5f;
        mtxa->mtxA[1][1] = 0.25f * s;
        mtxa->mtxA[1][2] = 0.0f;
        mtxa->mtxA[2][2] = -1.0f;
        mtxa_mult_right(lookAt);
        mtxa_mult_right(modelMtx);
        mtxa_to_mtx(river->sunTexMtx);
        mtxa_pop();

        prevFunc = g_yet_another_unk_draw_func(fn_1_9E440);
        load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
        avdisp_draw_model_culled_sort_auto(bg->model);
        g_yet_another_unk_draw_func(prevFunc);
    }
}

void fn_1_9E440(struct BallEnvArg *arg)
{
    struct BgGearRiver *river = &((struct BgGearWork *)world_theme.work)->river;
    struct TevStageInfo tev = arg->tev;
    float indMtx[2][3];
    int coord;

    GXLoadTexObj_cached(&river->reflTexObj, tev.texMapId);
    GXLoadTexObj_cached(&river->testTexObj, tev.texMapId + 1);
    GXLoadTexObj_cached(&river->testTexObj, tev.texMapId + 2);
    GXLoadTexObj_cached(river->texObjs, tev.texMapId + 3);
    GXLoadTexMtxImm(river->projTexMtx, tev.texMtxId, 0);
    mtxa_push();
    mtxa_from_translate(&river->pos1);
    mtxa_scale(&river->scale1);
    GXLoadTexMtxImm(mtxa->mtxA, tev.texMtxId + 3, 1);
    mtxa_pop();
    GXLoadTexMtxImm(river->sunTexMtx, tev.texMtxId + 6, 0);
    mtxa_push();
    mtxa_from_translate(&river->pos2);
    mtxa_scale(&river->scale2);
    GXLoadTexMtxImm(mtxa->mtxA, tev.texMtxId + 9, 1);
    mtxa_pop();

    indMtx[0][0] = 0.0f;
    indMtx[0][1] = 0.05f;
    indMtx[0][2] = 0.0f;
    indMtx[1][0] = 0.0f;
    indMtx[1][1] = 0.0f;
    indMtx[1][2] = 0.01f;
    g_GXSetIndTexMtx_wrapper(tev.unk1C, indMtx, 0);
    indMtx[0][1] = 0.9f;
    indMtx[1][2] = 0.2f;
    g_GXSetIndTexMtx_wrapper(tev.unk1C + 1, indMtx, 0);

    coord = tev.texCoordId;
    tex_coord_gen(tev.texCoordId, 1, 4, tev.texMtxId + 3);
    GXSetTevSwapMode_cached(tev.tevStage, 0, 2);
    g_GXSetTevOrder_wrapper(tev.tevStage, tev.texCoordId, tev.texMapId + 2, 0xFF);
    GXSetTevColorIn_cached(tev.tevStage, 0xC, 0xF, 0xF, 8);
    GXSetTevColorOp_cached(tev.tevStage, 0, 0, 3, 1, 1);
    GXSetTevAlphaIn_cached(tev.tevStage, 7, 7, 7, 0);
    GXSetTevAlphaOp_cached(tev.tevStage, 0, 0, 0, 1, 1);
    g_GXSetTevIndirect_zero_if_different(tev.tevStage);
    tev.tevStage++;
    tev.texCoordId++;

    tex_coord_gen(tev.texCoordId, 0, 0, tev.texMtxId);
    GXSetTevSwapMode_cached(tev.tevStage, 0, 2);
    g_GXSetTevOrder_wrapper(tev.tevStage, tev.texCoordId, tev.texMapId, 0xFF);
    GXSetTevColorIn_cached(tev.tevStage, 0xF, 8, 2, 0);
    GXSetTevColorOp_cached(tev.tevStage, 0, 0, 0, 1, 0);
    GXSetTevAlphaIn_cached(tev.tevStage, 7, 7, 7, 0);
    GXSetTevAlphaOp_cached(tev.tevStage, 0, 0, 0, 1, 0);
    GXSetIndTexOrder_cached(tev.tevIndStage, coord, tev.texMapId + 1);
    GXSetTevIndirect_cached(tev.tevStage, tev.tevIndStage, 0, 7, tev.unk1C, 0, 0, 0, 0, 0);
    tev.tevStage++;
    tev.texCoordId++;
    tev.tevIndStage++;

    tex_coord_gen(tev.texCoordId, 0, 0, tev.texMtxId + 6);
    GXSetTevSwapMode_cached(tev.tevStage, 0, 2);
    g_GXSetTevOrder_wrapper(tev.tevStage, tev.texCoordId, tev.texMapId + 3, 0xFF);
    GXSetTevColorIn_cached(tev.tevStage, 8, 0xF, 0xF, 0);
    GXSetTevColorOp_cached(tev.tevStage, 0, 0, 0, 1, 0);
    GXSetTevAlphaIn_cached(tev.tevStage, 7, 7, 7, 0);
    GXSetTevAlphaOp_cached(tev.tevStage, 0, 0, 0, 1, 0);
    tex_coord_gen(tev.texCoordId + 1, 1, 4, tev.texMtxId + 9);
    GXSetIndTexOrder_cached(tev.tevIndStage, tev.texCoordId + 1, tev.texMapId + 1);
    GXSetTevIndirect_cached(tev.tevStage, tev.tevIndStage, 0, 0, tev.unk1C + 1, 0, 0, 0, 0, 0);
    tev.tevStage++;
    tev.texCoordId += 2;
    tev.tevIndStage++;
    tev.texMapId += 4;
    tev.texMtxId += 12;
    tev.unk1C += 2;

    arg->tev = tev;
}

void fn_1_9E9C0(struct RenderEffect *rendEfc)
{
    rendEfc->enableFlags = 4;
    rendEfc->work = NULL;
}

void fn_1_9E9D4(struct RenderEffect *rendEfc) {}

void fn_1_9E9D8(struct RenderEffect *rendEfc) {}

void fn_1_9E9DC(int mask, struct RenderEffect *rendEfc)
{
    fn_1_9EA00(rendEfc);
}

void fn_1_9EA00(struct RenderEffect *rendEfc)
{
    struct Camera *cam;
    struct BgGearWork *work = world_theme.work;
    struct BgGearRiver *river = &work->river;
    struct StageBgModel *bg;
    struct StageBgModel **models;
    int mask;
    struct StageBgModel *model;
    int i;
    struct GMAModel *gma;
    int width;
    int height;
    int halfWidth;
    int halfHeight;
    struct Camera saved;
    Mtx mtx;
    Mtx mirror;
    Mtx reflMtx;
    Mtx viewMtx;
    Mtx44 proj;
    Vec tl;

    cam = g_current_camera;
    saved = *cam;
    bg = river->model;
    if (bg == NULL)
        return;
    if (!(bg->flags & g_smth_with_challenge_mode_var_and_3p()))
    {
        river->texReady = 0;
        return;
    }
    width = 256;
    height = 256;
    halfWidth = width >> 1;
    halfHeight = height >> 1;
    push_light_group();

    gma = bg->model;
    mtxa_from_translate(&bg->pos);
    mtxa_rotate_z(bg->rotZ);
    mtxa_rotate_y(bg->rotY);
    mtxa_rotate_x(bg->rotX);
    mtxa_scale(&bg->scale);
    mtxa_translate(&gma->boundSphereCenter);
    mtxa_to_mtx(mtx);
    mtxa_to_mtx(river->mtx);
    mtxa_mult_left(mtxa->mtxB);
    mtxa_rigid_inv_tf_tl(&tl);
    if (tl.y < 0.0f)
    {
        mtxa_from_mtx(mtx);
        mtxa_rotate_z(-0x8000);
        mtxa_to_mtx(mtx);
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
    mtxa_mult_right(mtx);
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
    mtxa_from_mtx(mtx);
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
    mtxa_from_mtx(g_ord_tbl_stuff->matrices[0]);
    g_set_active_light_group(world_theme.unkAA);
    models = work->models;
    for (i = work->count; i > 0; i--, models++)
    {
        model = *models;
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
    ord_tbl_draw_nodes();
    g_some_render_flag.flags &= ~4;

    GXSetZMode_cached(1, 3, 1);
    GXSetTexCopySrc_cached(0, 0, width, height);
    GXSetTexCopyDst_cached(halfWidth, halfHeight, 4, 1);
    GXCopyTex(river->reflTexData, 1);
    GXInitTexObj(&river->reflTexObj, river->reflTexData, halfWidth, halfHeight, 4, 2, 2, 0);
    river->texReady = 1;
    GXInvalidateTexAll();

    *cam = saved;
    camera_apply_viewport(modeCtrl.currPlayer);
    pop_light_group();
}

int fn_1_9F214(int index, struct GMAModelEntry *entry)
{
    struct BgGearWork *work = world_theme.work;
    GXTexObj *tex = entry->model->texObjs;

    switch (index)
    {
    case 0:
        GXInitTexObj(&work->river.testTexObj, GXGetTexObjData(tex), GXGetTexObjWidth(tex), GXGetTexObjHeight(tex),
            GXGetTexObjFmt(tex), 2, 2, 0);
        break;
    }
    return 1;
}

int fn_1_9F2B4(int index, struct StageBgModel *model)
{
    struct BgGearWork *work = world_theme.work;

    switch (index)
    {
    case 0:
        work->river.model = model;
        break;
    case 1:
        work->sun.model = model;
        break;
    case 2:
        work->scale = model->scale;
        /* fallthrough */
    case 3:
    case 4:
    case 5:
    case 6:
        if (work->count < 16)
        {
            work->models[work->count] = model;
            work->count++;
        }
        break;
    }
    return 1;
}
