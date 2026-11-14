/* Pilot background: islands and coral around a sea surface that samples a
 * reflection render effect through indirect-textured scrolling layers. */

#include "types.h"
#include "avdisp.h"
#include "background.h"
#include "camera.h"
#include "gx.h"
#include "libc.h"
#include "mathutil.h"
#include "nl2ngc.h"
#include "rand.h"
#include "stage.h"

extern struct { u32 flags; } g_some_render_flag;

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

struct BgPilReflection
{
    u8 filler0[4];
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

struct BgPilLayer
{
    /*0x00*/ Vec pos;
    /*0x0C*/ Vec vel;
    /*0x18*/ Vec scale;
    /*0x24*/ s16 rotZ;
    /*0x28*/ Mtx mtx;
};  /* size = 0x58 */

struct BgPilWork
{
    /*0x000*/ s32 initialized;
    /*0x004*/ struct GMAModel *islandA;
    /*0x008*/ struct GMAModel *islandB;
    /*0x00C*/ struct GMAModel *islandC;
    /*0x010*/ struct GMAModel *sangosyo;
    u8 filler14[4];
    /*0x018*/ struct StageBgModel *sea;
    /*0x01C*/ struct GMAModel *seaModel;
    /*0x020*/ GXTexObj *waterSpecTex;
    /*0x024*/ GXTexObj *indMapATex;
    /* typed (not a filler) so the x64 layout matches bg_pil2.c and
     * rend_efc_ripple.c, which share this work area */
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
    u8 filler1EC[0x1FC - 0x1EC];
    /*0x1FC*/ struct BgPilReflection *reflection;
    /*0x200*/ struct BgPilLayer layers[3];
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

void fn_1_7E138(struct BallEnvArg *);
int fn_1_7E5BC(int, struct GMAModelEntry *);
int fn_1_7E664(int, struct StageBgModel *);

static struct BgModelSearch lbl_1_data_31A20[] =
{
    { BG_MDL_CMP_FULL, "PIL_WATER_SPEC" },
    { BG_MDL_CMP_FULL, "PIL_IND_MAP_A" },
    { BG_MDL_CMP_FULL, "PIL_SEA_MASK" },
    { BG_MDL_CMP_FULL, "PIL_VTX_SEA_BALL_A" },
    { BG_MDL_CMP_FULL, "PIL_IND_MAP_B" },
    { BG_MDL_CMP_FULL, "PIL_IND_MAP_C" },
    { BG_MDL_CMP_PREFIX, "PIL_HAMON_" },
    { BG_MDL_CMP_END },
};

static struct BgModelSearch lbl_1_data_31AAC[] =
{
    { BG_MDL_CMP_PREFIX, "PIL_SEA_" },
    { BG_MDL_CMP_FULL, "PIL_SANGOSYO_A" },
    { BG_MDL_CMP_FULL, "PIL_ISLAND_A" },
    { BG_MDL_CMP_FULL, "PIL_ISLAND_B" },
    { BG_MDL_CMP_FULL, "PIL_ISLAND_C" },
    { BG_MDL_CMP_END },
};

void bg_pil_init(void)
{
    struct BgPilWork *work = world_theme.work;
    struct BgPilLayer *layer;
    int i;
    Vec sp8;
    struct RenderEffect rendEfc;

    bg_init_base();
    if (work->initialized == 0)
    {
        work->hamonCount = 0;
        g_smth_with_bg_model_names(lbl_1_data_31A20, fn_1_7E5BC);
    }
    work->sea = NULL;
    g_init_stagedef_bg_fg_models(stagedef->bgModels, stagedef->bgModelCount, lbl_1_data_31AAC, fn_1_7E664);
    g_init_stagedef_bg_fg_models(stagedef->fgModels, stagedef->fgModelCount, lbl_1_data_31AAC, fn_1_7E664);

    memset(&rendEfc, 0, sizeof(rendEfc));
    rendEfc.cameraMask = 0xFFFF;
    rendEfc.funcs = &lbl_1_data_533F0;
    fn_1_D8130(0, 0, &rendEfc);

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
}

void bg_pil_tick(void)
{
    struct BgPilWork *work = world_theme.work;
    int i;
    struct BgPilLayer *layer;

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

void bg_pil_dest(void) {}

void bg_pil_disp(void)
{
    struct BgPilWork *work = world_theme.work;
    struct StageBgModel *sea;
    struct BgPilReflection *reflection;
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
    float len;
    float x;
    float d;
    float a;
    float b;

    g_smth_with_challenge_mode_var_and_3p();
    if (work->sea != NULL)
        work->sea->flags = work->sea->flags & ~0x10000;
    bg_disp_base();

    mtxa_from_mtxb();
    GXLoadPosMtxImm(mtxa->mtxA, 0);
    GXLoadNrmMtxImm(mtxa->mtxA, 0);
    if (work->islandA != NULL)
        avdisp_draw_model_culled_sort_never(work->islandA);
    if (work->islandB != NULL)
        avdisp_draw_model_culled_sort_never(work->islandB);
    if (work->islandC != NULL)
        avdisp_draw_model_culled_sort_never(work->islandC);
    if (work->sangosyo != NULL)
        avdisp_draw_model_culled_sort_never(work->sangosyo);

    if (work->sea != NULL && !(g_some_render_flag.flags & 4))
    {
        sea = work->sea;
        reflection = work->reflection;
        if (reflection != NULL)
        {
            C_MTXPerspective(sp54, 0.005493164f * g_current_camera->sub28.view.fov,
                g_current_camera->sub28.view.aspect, 0.1f, 20000.0f);
            mtxa_from_identity();
            mtxa->mtxA[0][2] = 0.5 - 0.5 * (g_current_camera->sub28.view.tanHalfFov *
                (g_current_camera->sub28.view.aspect * (sp54[0][0] * g_current_camera->sub28.view.unk28)));
            mtxa->mtxA[1][2] = 0.5 - 0.5 * (g_current_camera->sub28.view.tanHalfFov *
                (sp54[1][1] * g_current_camera->sub28.view.unk2C));
            d = math_tan(0.5 * g_current_camera->sub28.view.fov);
            x = 0.5f / d;
            mtxa->mtxA[0][0] = x / g_current_camera->sub28.view.aspect;
            mtxa->mtxA[1][1] = x;
            mtxa_to_mtx(sp94);

            mtxa_from_mtx(sp94);
            mtxa_to_mtx(spC4);
            mtxa_from_mtxb();
            mtxa_mult_right(reflection->mtx);
            mtxa_mult_left(spC4);
            mtxa_to_mtx(work->reflectTexMtx);

            mtxa_from_mtx(spC4);
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
            a = mathutil_vec_dot_prod(&sp8, &sp44.n);
            b = mathutil_vec_dot_prod(&sp20, &sp44.n);
            len = a * b;
#endif
            vec_set_len(len, &sp20, &sp8);
            sp14.x += sp8.x;
            sp14.y += sp8.y;
            sp14.z += sp8.z;
            sp8.x = sp14.x + sp2C.x;
            sp8.y = sp14.y + sp2C.y;
            sp8.z = sp14.z + sp2C.z;
            C_MTXLookAt(spC4, &sp14, &sp44.n, &sp8);

            mtxa_from_translate_xyz(0.5f, 0.5f, 0.0f);
            mtxa_scale_s(0.5f / (4.0f * len));
            mtxa_mult_right(spC4);
            mtxa_to_mtx(work->specTexMtx);

            mtxa_from_mtxb();
            mtxa_mult_right(reflection->mtx);
            GXLoadPosMtxImm(mtxa->mtxA, 0);
            GXLoadNrmMtxImm(mtxa->mtxA, 0);
            prevFunc = g_yet_another_unk_draw_func(fn_1_7E138);
            avdisp_set_z_mode(1, 3, 0);
            avdisp_draw_model_culled_sort_never(sea->model);
            avdisp_set_z_mode(1, 3, 1);
            g_yet_another_unk_draw_func(prevFunc);
        }
        else
        {
            mtxa_from_mtxb_translate(&sea->pos);
            mtxa_rotate_z(sea->rotZ);
            mtxa_rotate_y(sea->rotY);
            mtxa_rotate_x(sea->rotX);
            mtxa_scale(&sea->scale);
            GXLoadPosMtxImm(mtxa->mtxA, 0);
            GXLoadNrmMtxImm(mtxa->mtxA, 0);
            avdisp_draw_model_culled_sort_auto(sea->model);
        }
    }
}

void bg_pil_item_coin_coli(void) {}

void fn_1_7E138(struct BallEnvArg *arg)
{
    struct BgPilWork *work = world_theme.work;
    struct BgPilReflection *reflection = work->reflection;
    struct TevStageInfo sp40 = arg->tev;
    float sp28[2][3];
    float sp10[2][3];

    GXLoadTexObj_cached(&reflection->tex, sp40.texMapId);
    GXLoadTexObj_cached(&reflection->indTex, sp40.texMapId + 1);
    GXLoadTexObj_cached(work->waterSpecTex, sp40.texMapId + 2);
    GXLoadTexMtxImm(work->reflectTexMtx, sp40.texMtxId, 0);
    GXLoadTexMtxImm(work->mirrorTexMtx, sp40.texMtxId + 3, 0);

    GXSetTevKColorSel_cached(sp40.tevStage, 2);
    tex_coord_gen(sp40.texCoordId, 0, 0, sp40.texMtxId);
    GXSetTevSwapMode_cached(sp40.tevStage, 0, 1);
    g_GXSetTevOrder_wrapper(sp40.tevStage, sp40.texCoordId, sp40.texMapId, 0xFF);
    GXSetTevColorIn_cached(sp40.tevStage, 0xF, 8, 0xE, 0);
    GXSetTevColorOp_cached(sp40.tevStage, 0, 0, 0, 1, 0);
    GXSetTevAlphaIn_cached(sp40.tevStage, 7, 4, 0, 0);
    GXSetTevAlphaOp_cached(sp40.tevStage, 0, 0, 0, 1, 0);
    tex_coord_gen(sp40.texCoordId + 1, 0, 0, sp40.texMtxId + 3);
    GXSetIndTexOrder_cached(sp40.tevIndStage, sp40.texCoordId + 1, sp40.texMapId + 1);
    sp28[0][0] = 0.0f;
    sp28[0][1] = 0.2f;
    sp28[0][2] = 0.0f;
    sp28[1][0] = 0.2f;
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
    sp10[0][1] = -0.8f;
    sp10[0][2] = 0.0f;
    sp10[1][0] = 0.9f;
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

int fn_1_7E5BC(int index, struct GMAModelEntry *entry)
{
    struct BgPilWork *work = world_theme.work;

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
    }
    return 1;
}

int fn_1_7E664(int index, struct StageBgModel *model)
{
    struct BgPilWork *work = world_theme.work;

    switch (index)
    {
    case 0:
        work->sea = model;
        work->seaModel = model->model;
        break;
    case 1:
        work->islandA = model->model;
        model->model = NULL;
        break;
    case 2:
        work->islandB = model->model;
        model->model = NULL;
        break;
    case 3:
        work->islandC = model->model;
        model->model = NULL;
        break;
    case 4:
        work->sangosyo = model->model;
        model->model = NULL;
        break;
    }
    return 1;
}
