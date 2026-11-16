/* Lava background: scrolling lava surfaces, jelly-like fountains, fire bursts, and a
 * screen-space glow with drifting embers that wrap around the camera. */

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
#include "mode.h"
#include "nl2ngc.h"
#include "pc_hfr.h"
#include "rand.h"
#include "stage.h"
#include "stcoli.h"

/* Lava surface. The texture scrolls by velY per frame on two independent layers. */
struct BgLavaScroll
{
    /*0x00*/ struct StageBgModel *model;
    /*0x04*/ float texY;
    /*0x08*/ float velY;
    /*0x0C*/ float texY2;
    /*0x10*/ float vel2;
};  /* size = 0x14 */

struct BgLavaVertex
{
    /*0x00*/ Vec pos;
    /*0x0C*/ Vec unkC;
    /*0x18*/ u32 flags;
    /*0x1C*/ s16 angle1;
    /*0x1E*/ s16 angleVel1;
    /*0x20*/ s16 angle2;
    /*0x22*/ s16 angleVel2;
    /*0x24*/ Mtx mtx;
    /*0x54*/ struct BgLavaVertex *parent;
};  /* size = 0x58 */

/* Skinned lava fountain. Starts with the same fields as BgLavaScroll. */
struct BgLavaFountain
{
    /*0x00*/ struct BgLavaScroll scroll;
    /*0x14*/ s32 vertexCount;
    /*0x18*/ struct BgLavaVertex *vertices;
};  /* size = 0x1C */

/* Per-camera glow overlay. */
struct BgLavaGlow
{
    /*0x00*/ float brightness1;
    /*0x04*/ float brightnessVel1;
    /*0x08*/ float brightness2;
    /*0x0C*/ float brightnessVel2;
    /*0x10*/ float texX;
    /*0x14*/ float texXVel;
    /*0x18*/ Vec dir;
    /*0x24*/ float scale;
    /*0x28*/ s16 angle;
    /*0x2A*/ s16 angleVel;
};  /* size = 0x2C */

struct BgLavaEmber
{
    /*0x00*/ Vec pos;
    /*0x0C*/ Vec vel;
    /*0x18*/ float alpha;
    /*0x1C*/ float size;
    /*0x20*/ float maxSize;
    /*0x24*/ s32 life;
};  /* size = 0x28 */

struct BgLavaFlame
{
    /*0x00*/ float sizeXZ;
    /*0x04*/ float currSizeXZ;
    /*0x08*/ float sizeY;
    /*0x0C*/ float t;
    /*0x10*/ float dt;
    /*0x14*/ s16 rotY;
    /*0x16*/ s16 rotZ;
};  /* size = 0x18 */

struct BgLavaFire
{
    /*0x00*/ struct StageBgModel *model;
    /*0x04*/ float glowScale;
    /*0x08*/ float spawnTimer;
    /*0x0C*/ struct BgLavaFlame flames[4];
};  /* size = 0x6C */

struct BgLavaWork
{
    /*0x0000*/ s32 initialized;
    /*0x0004*/ s32 scrollCount;
    /*0x0008*/ struct BgLavaScroll scrolls[24];
    /*0x01E8*/ s32 unk1E8;
    /*0x01EC*/ struct BgLavaScroll *currScroll;
    /*0x01F0*/ GXTexObj *indTex;
    /*0x01F4*/ s32 fountainCount;
    /*0x01F8*/ struct BgLavaFountain fountains[2];
    /*0x0230*/ GXTexObj *glowTex;
    /*0x0234*/ struct BgLavaGlow glows[5];
    /*0x0310*/ GXTexObj *emberTex;
    /*0x0314*/ GXTexObj *powderTex;
    /*0x0318*/ Vec emberWind;
    /*0x0324*/ Vec emberAccel;
    /*0x0330*/ Vec emberAccelVel;
    /*0x033C*/ float emberDensity;
    /*0x0340*/ s32 embersEnabled;
    /*0x0344*/ struct BgLavaEmber embers[96];
    /*0x1244*/ struct GMAModel *flameModel;
    /*0x1248*/ struct GMAModel *fireGlowModel;
    /*0x124C*/ struct GMAModel *powderModel;
    /*0x1250*/ s32 fireCount;
    /*0x1254*/ struct BgLavaFire fires[16];
};

struct BgLavaWorkNode
{
    /*0x00*/ struct OrdTblNode node;
    /*0x08*/ struct BgLavaWork *work;
};

struct BgLavaFireNode
{
    /*0x00*/ struct OrdTblNode node;
    /*0x08*/ Mtx mtx;
    /*0x38*/ struct BgLavaFire *fire;
};

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
struct OrdTblNode *fn_1_CCD5C(int);
void *ord_tbl_alloc_node(u32 size);
void fn_1_CD418(struct OrdTblNode *list, struct OrdTblNode *node);
void mtxa_from_translate_xyz(float x, float y, float z);
void mtxa_rigid_inv_tf_vec_xyz(float x, float y, float z, Vec *dst);
void *alloc_from_heap_or_panic(void *heap, u32 size, char *file, int line);
void draw_poly(struct GMAModel *model, Mtx **mtxs);
s16 *fn_80069964(struct GMAModel *model);
void pointer_range_advance(struct GMAModel *model, void *mtxs);
extern void *stage_heap;
extern void *g_some_buffer_ptr;
extern struct { u32 flags; } g_some_render_flag;

void g_draw_lava_particles(struct BgLavaWork *work);
void g_something_with_stage_heap_and_lava_theme(struct BgLavaFountain *fountain);
void fn_1_7F77C(struct BgLavaWork *work);
void fn_1_7F8E4(struct BgLavaWork *work);
void fn_1_7FBBC(struct BgLavaWork *work);
void fn_1_8010C(struct BallEnvArg *arg);
void fn_1_80544(struct BgLavaWork *work);
void fn_1_80758(struct BgLavaWork *work);
void fn_1_81F88(struct BgLavaWorkNode *node);
void fn_1_81FD0(struct BgLavaWork *work);
void fn_1_82118(struct BgLavaWork *work);
void fn_1_82188(struct BgLavaWork *work);
void fn_1_82204(struct BgLavaWork *work, u32 mask);
void fn_1_82720(struct BgLavaWork *work);
void fn_1_828A0(struct BgLavaWork *work);
void fn_1_82CDC(struct BgLavaWork *work, u32 mask);
void fn_1_82DE4(struct BgLavaFireNode *node);
int fn_1_82FB8(int, struct GMAModelEntry *);
int fn_1_83154(int, struct StageBgModel *);

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

static struct BgModelSearch lbl_1_data_31BE4[] =
{
    { BG_MDL_CMP_FULL, "LAV_YOUGAN_LIGHT_A" },
    { BG_MDL_CMP_FULL, "LAV_FIRE_POWDER_A" },
    { BG_MDL_CMP_FULL, "LAV_FIRE_POWDER_B" },
    { BG_MDL_CMP_FULL, "LAV_YOUGAN_INDMAP_A" },
    { BG_MDL_CMP_PREFIX, "LAV_FIRE_A" },
    { BG_MDL_CMP_PREFIX, "LAV_FIRE_GROW_" },
    { BG_MDL_CMP_END },
};

static struct BgModelSearch lbl_1_data_31C6C[] =
{
    { BG_MDL_CMP_PREFIX, "LAV_LAVABASE_" },
    { BG_MDL_CMP_PREFIX, "LAV_LAVA_" },
    { BG_MDL_CMP_PREFIX, "LAV_LAVA_FUNSUI_" },
    { BG_MDL_CMP_PREFIX, "LAV_DRAGON_LAVA_" },
    { BG_MDL_CMP_PREFIX, "LAV_FIRE_" },
    { BG_MDL_CMP_END },
};

void bg_lava_init(void)
{
    struct BgLavaWork *work = world_theme.work;
    int i;
    struct BgLavaScroll *scroll;

    bg_init_base();
    if (work->initialized == 0)
    {
        g_smth_with_bg_model_names(lbl_1_data_31BE4, fn_1_82FB8);
        work->initialized = 1;
    }
    work->scrollCount = 0;
    work->fountainCount = 0;
    work->fireCount = 0;
    g_init_stagedef_bg_fg_models(stagedef->bgModels, stagedef->bgModelCount, lbl_1_data_31C6C, fn_1_83154);
    g_init_stagedef_bg_fg_models(stagedef->fgModels, stagedef->fgModelCount, lbl_1_data_31C6C, fn_1_83154);
    if (work->scrollCount != 0)
    {
        work->unk1E8 = 0;
        scroll = work->scrolls;
        for (i = work->scrollCount; i > 0; i--, scroll++)
        {
            scroll->texY = RAND_FLOAT();
            scroll->velY = -0.0020833334f * (0.9f + 0.2f * RAND_FLOAT());
            scroll->texY2 = RAND_FLOAT();
            scroll->vel2 = -0.004166667f * (0.9f + 0.2f * RAND_FLOAT());
        }
    }
    fn_1_81FD0(work);
    fn_1_82720(work);
    fn_1_7F77C(work);
    fn_1_80544(work);
}

void bg_lava_tick(void)
{
    struct BgLavaWork *work = world_theme.work;
    int i;
    struct BgLavaScroll *scroll;

    g_smth_with_challenge_mode_var_and_3p();
    bg_tick_base();
    if (!is_game_paused_and_in_view_stage())
    {
        if (work->scrollCount != 0)
        {
            scroll = work->scrolls;
            for (i = work->scrollCount; i > 0; i--, scroll++)
            {
                scroll->texY += scroll->velY;
                scroll->texY2 += scroll->vel2;
            }
        }
        fn_1_82188(work);
        fn_1_828A0(work);
        fn_1_7F8E4(work);
        if (work->embersEnabled != 0)
            fn_1_80758(work);
    }
}

void bg_lava_dest(void)
{
    fn_1_82118(world_theme.work);
}

void bg_lava_disp(void)
{
    int i;
    struct BgLavaWork *work = world_theme.work;
    struct BgLavaScroll *scroll;
    u32 mask;
    void *prevFunc;
    struct StageBgModel *model;
    struct GMAModel *gma;
    struct OrdTblNode *list;
    struct BgLavaWorkNode *node;

    mask = g_smth_with_challenge_mode_var_and_3p();
    scroll = work->scrolls;
    for (i = work->scrollCount; i > 0; i--, scroll++)
        scroll->model->flags = scroll->model->flags & ~0x10000;
    fn_1_82204(work, mask);
    fn_1_82CDC(work, mask);
    bg_disp_base();
    if (work->scrollCount != 0)
    {
        if (!(g_some_render_flag.flags & 0x80))
            prevFunc = g_yet_another_unk_draw_func(fn_1_8010C);
        g_something_with_texture_scroll_2(1);
        scroll = work->scrolls;
        for (i = work->scrollCount; i > 0; i--, scroll++)
        {
            work->currScroll = scroll;
            model = scroll->model;
            if ((model->flags & mask) != 0)
            {
                gma = model->model;
                mtxa_from_translate_xyz(0.0f, scroll->texY, 0.0f);
                fn_1_10A4CC(0, mtxa->mtxA);
                mtxa_from_mtx(g_ord_tbl_stuff->matrices[0]);
                mtxa_translate(&model->pos);
                mtxa_rotate_z(model->rotZ);
                mtxa_rotate_y(model->rotY);
                mtxa_rotate_x(model->rotX);
                mtxa_scale(&model->scale);
                if (g_is_sphere_visible_scaled(&gma->boundSphereCenter, gma->boundSphereRadius, model->scale.x))
                {
                    g_smth_with_gpu_and_locked_cache_mtx();
                    avdisp_draw_model_unculled_sort_never(gma);
                }
            }
        }
        g_something_with_texture_scroll_2(0);
        if (!(g_some_render_flag.flags & 0x80))
            g_yet_another_unk_draw_func(prevFunc);
    }
    list = fn_1_CCD5C(0);
    node = ord_tbl_alloc_node(sizeof(*node));
    if (node != NULL)
    {
        node->node.drawFunc = (void *)fn_1_81F88;
        node->work = work;
        fn_1_CD418(list, &node->node);
    }
}

void bg_lava_item_coin_coli(void) {}

void fn_1_7F77C(struct BgLavaWork *work)
{
    int i;
    struct BgLavaGlow *glow;
    int flag = modeCtrl.courseFlags & 0x04000000;

    glow = work->glows;
    for (i = 5; i > 0; i--, glow++)
    {
        if (!flag)
        {
            glow->brightness1 = 1.0f;
            glow->brightness2 = 1.0f;
        }
        else
        {
            glow->brightness1 = 0.375f;
            glow->brightness2 = 0.375f;
        }
        glow->brightnessVel1 = 0.0f;
        glow->brightnessVel2 = 0.0f;
        glow->texX = RAND_FLOAT();
        glow->texXVel = 0.0016666667f * RAND_FLOAT();
        mtxa_from_rotate_y(rand() & 0x7FFF);
        mtxa_rotate_x(rand() & 0x7FFF);
        mtxa_tf_vec_xyz(0.0f, 0.0f, -1.0f, &glow->dir);
        glow->scale = 1.0f;
        glow->angle = rand() & 0x7FFF;
        glow->angleVel = rand() & 0x20F;
    }
}

void fn_1_7F8E4(struct BgLavaWork *work)
{
    int i;
    struct BgLavaGlow *glow;
    struct Camera *camera;
    struct RaycastHit hit;
    Vec pos;
    Vec dir;

    camera = cameras;
    glow = work->glows;
    for (i = 0; i < 5; i++, glow++, camera++)
    {
        if (cameras[i].sub28.viewportW > 0.0f && cameras[i].sub28.viewportH > 0.0f)
        {
            float a;
            float d;
            float b;
            float dot;

            mtxa_from_mtx(camera->unk14C);
            mtxa_rigid_inv_tf_tl(&pos);
            mtxa_from_mtx(camera->unk1AC);
            mtxa_rigid_inv_tf_vec_xyz(0.0f, 0.0f, -1.0f, &dir);
            vec_normalize_len(&dir);
            raycast_stage_down(&pos, &hit, NULL);
            if ((hit.flags & 1) != 0)
            {
                d = 0.041666668f * (pos.y - hit.pos.y);
                if (d > 1.0f)
                {
                    a = 0.75f;
                    b = -0.025f;
                }
                else if (d < 0.0f)
                {
                    a = 0.0f;
                    b = 0.5f;
                }
                else
                {
                    a = 0.75f * d;
                    b = 0.5f + -0.525f * d;
                }
            }
            else
            {
                a = 0.75f;
                b = -0.025f;
            }
            d = 0.5f * (1.0f + dir.y);
            a *= 2.0f - 1.5f * d;
            b *= 1.0 - 0.75 * __fabs(dir.y);
            glow->brightnessVel1 += 0.2f * (0.3f * (a - glow->brightness1) - glow->brightnessVel1);
            glow->brightnessVel1 *= 0.995f;
            glow->brightness1 += glow->brightnessVel1;
            glow->brightnessVel2 += 0.05f * (0.05f * (b - glow->brightness2) - glow->brightnessVel2);
            glow->brightnessVel2 *= 0.995f;
            glow->brightness2 += glow->brightnessVel2;
            dot = 0.0016666667f * mathutil_vec_dot_prod(&glow->dir, &dir);
            glow->texXVel += 0.025f * (dot - glow->texXVel);
            glow->texX += glow->texXVel;
            glow->angle += glow->angleVel;
            glow->scale = 1.0f + 0.1f * math_sin(glow->angle);
        }
    }
}

void fn_1_7FBBC(struct BgLavaWork *work)
{
    struct BgLavaGlow *glow = &work->glows[g_something_with_camera3];
    float proj[7];
    Vec colorf;
    GXColor color;
    float x0;
    float y0;
    float x1;
    float y1;
    float invW;
    float invH;
    float brightness;

    GXLoadTexObj_cached(work->glowTex, 0);
    GXSetBlendMode_cached(1, 1, 1, 0);
    GXSetZMode_cached(0, 7, 0);
    g_something_with_raster_state(0);
    GXSetCullMode_cached(0);
    mtxa_from_translate_xyz(0.5f + glow->texX, 1.0f, 0.0f);
    mtxa_scale_xyz(glow->scale, 1.0f / glow->scale, 1.0f);
    mtxa_translate_xyz(-0.5f, -1.0f, 0.0f);
    GXLoadTexMtxImm(mtxa->mtxA, 0x1E, 1);
    g_GXSetTevIndirect_zero_if_different(0);
    tex_coord_gen(0, 1, 4, 0x1E);
    g_GXSetTevOrder_wrapper(0, 0, 0, 0xFF);
    GXSetTevKColorSel_cached(0, 0xC);
    GXSetTevKAlphaSel_cached(0, 0x1C);
    GXSetTevColorIn_cached(0, 0xF, 8, 0xE, 0xF);
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

    brightness = 255.0f * glow->brightness1;
    colorf.x = 1.2f * brightness;
    colorf.y = 1.15f * brightness;
    colorf.z = brightness;
    f32_to_u8(&colorf.x, &color.r);
    f32_to_u8(&colorf.y, &color.g);
    f32_to_u8(&colorf.z, &color.b);
    color.a = 0xFF;
    GXSetTevKColor_cached(0, color);

    GXGetProjectionv(proj);
    invW = 1.0f / proj[1];
    invH = 1.0f / proj[3];
    x0 = -100.0f * (-1.0f - proj[2]) * invW;
    x1 = -100.0f * (1.0f - proj[2]) * invW;
    y0 = -100.0f * (-1.0f - proj[4]) * invH;
    y1 = -100.0f * (1.0f - proj[4]) * invH;

    GXBegin(0x80, 0, 4);
    GXPosition3f32(x0, y0, -100.0f);
    GXTexCoord2f32(0.0f, 0.0f);
    GXPosition3f32(x1, y0, -100.0f);
    GXTexCoord2f32(1.0f, 0.0f);
    GXPosition3f32(x1, y1, -100.0f);
    GXTexCoord2f32(1.0f, 1.0f);
    GXPosition3f32(x0, y1, -100.0f);
    GXTexCoord2f32(0.0f, 1.0f);

    brightness = 255.0f * glow->brightness2;
    if (brightness > 0.0f)
    {
        colorf.x = 1.2f * brightness;
        colorf.y = 1.15f * brightness;
        colorf.z = brightness;
        f32_to_u8(&colorf.x, &color.r);
        f32_to_u8(&colorf.y, &color.g);
        f32_to_u8(&colorf.z, &color.b);
        color.a = 0xFF;
        GXSetTevKColor_cached(0, color);

        GXBegin(0x80, 0, 4);
        GXPosition3f32(x0, y0, -100.0f);
        GXTexCoord2f32(0.0f, 1.0f);
        GXPosition3f32(x1, y0, -100.0f);
        GXTexCoord2f32(1.0f, 1.0f);
        GXPosition3f32(x1, y1, -100.0f);
        GXTexCoord2f32(1.0f, 0.0f);
        GXPosition3f32(x0, y1, -100.0f);
        GXTexCoord2f32(0.0f, 0.0f);
    }
}

void fn_1_8010C(struct BallEnvArg *arg)
{
    struct BgLavaWork *work = world_theme.work;
    struct BgLavaScroll *scroll = work->currScroll;
    struct TevStageInfo sp28 = arg->tev;
    float sp10[2][3];

    GXLoadTexObj_cached(work->indTex, sp28.texMapId);
    mtxa_push();
    mtxa_from_translate_xyz(0.0f, scroll->texY2, 0.0f);
    GXLoadTexMtxImm(mtxa->mtxA, sp28.texMtxId, 1);
    mtxa_pop();
    sp10[0][0] = 0.0f;
    sp10[0][1] = 0.6f;
    sp10[0][2] = 0.0f;
    sp10[1][0] = 0.0f;
    sp10[1][1] = 0.0f;
    sp10[1][2] = 0.6f;
    if (sp28.tevStage > 1)
    {
        mtxa_push();
        mtxa_from_translate_xyz(0.0f, scroll->texY2, 0.0f);
        GXLoadTexMtxImm(mtxa->mtxA, sp28.texMtxId, 1);
        mtxa_pop();
        tex_coord_gen(sp28.texCoordId, 1, 4, sp28.texMtxId);
        GXSetIndTexOrder_cached(sp28.tevIndStage, sp28.texCoordId, sp28.texMapId);
        g_GXSetIndTexMtx_wrapper(sp28.unk1C, sp10, 0);
        GXSetTevIndirect_cached(0, sp28.tevIndStage, 0, 0, sp28.unk1C, 0, 0, 0, 0, 0);
        sp28.unk1C++;
        sp28.texCoordId++;
        sp28.tevIndStage++;
        sp28.texMtxId += 3;
        mtxa_push();
        mtxa_from_translate_xyz(0.2f * scroll->texY2, 1.2f * scroll->texY2, 0.0f);
        GXLoadTexMtxImm(mtxa->mtxA, sp28.texMtxId, 1);
        mtxa_pop();
        sp10[0][0] = 0.0f;
        sp10[0][1] = 0.1f;
        sp10[0][2] = 0.0f;
        sp10[1][0] = 0.0f;
        sp10[1][1] = 0.0f;
        sp10[1][2] = 0.7f;
        tex_coord_gen(sp28.texCoordId, 1, 5, sp28.texMtxId);
        GXSetIndTexOrder_cached(sp28.tevIndStage, sp28.texCoordId, sp28.texMapId);
        g_GXSetIndTexMtx_wrapper(sp28.unk1C, sp10, 0);
        GXSetTevIndirect_cached(1, sp28.tevIndStage, 0, 0, sp28.unk1C, 0, 0, 0, 0, 0);
        sp28.unk1C++;
        sp28.texCoordId++;
        sp28.tevIndStage++;
        sp28.texMtxId += 3;
    }
    else
    {
        mtxa_push();
        mtxa_from_translate_xyz(0.0f, scroll->texY2, 0.0f);
        GXLoadTexMtxImm(mtxa->mtxA, sp28.texMtxId, 1);
        mtxa_pop();
        tex_coord_gen(sp28.texCoordId, 1, 4, sp28.texMtxId);
        GXSetIndTexOrder_cached(sp28.tevIndStage, sp28.texCoordId, sp28.texMapId);
        g_GXSetIndTexMtx_wrapper(sp28.unk1C, sp10, 0);
        GXSetTevIndirect_cached(0, sp28.tevIndStage, 0, 0, sp28.unk1C, 0, 0, 0, 0, 0);
        sp28.unk1C++;
        sp28.texCoordId++;
        sp28.tevIndStage++;
        sp28.texMtxId += 3;
    }
    sp28.texMapId++;
    arg->tev = sp28;
}

void fn_1_80544(struct BgLavaWork *work)
{
    int i;
    struct BgLavaEmber *ember;

    if (modeCtrl.unk30 < 2)
        work->embersEnabled = 1;
    else
    {
        work->embersEnabled = 0;
        return;
    }
    work->emberWind.x = 0.0f;
    work->emberWind.y = 0.0f;
    work->emberWind.z = 0.0f;
    work->emberAccel.x = 0.0f;
    work->emberAccel.y = 0.0f;
    work->emberAccel.z = 0.0f;
    work->emberAccelVel.x = 0.0f;
    work->emberAccelVel.y = 0.0f;
    work->emberAccelVel.z = 0.0f;
    work->emberDensity = 0.0f;
    ember = work->embers;
    for (i = 96; i > 0; i--, ember++)
    {
        ember->pos.x = 8.0f * RAND_FLOAT();
        ember->pos.y = 8.0f * RAND_FLOAT();
        ember->pos.z = 8.0f * RAND_FLOAT();
        ember->vel.x = 0.0f;
        ember->vel.y = 0.0f;
        ember->vel.z = 0.0f;
        ember->alpha = 0.0f;
        ember->maxSize = 0.02f * (1.0f + RAND_FLOAT());
        ember->size = ember->maxSize;
        ember->life = 15.0f * RAND_FLOAT();
    }
}

void fn_1_80758(struct BgLavaWork *work)
{
    struct BgLavaEmber *ember;
    int i;
    int count;
    float f31;
    struct Camera *camera;
    struct RaycastHit hit;
    Vec wind;
    Vec probe;
    Vec target;
    Vec accel;
    Vec accelVel;

    if (is_game_paused_and_in_view_stage())
        return;
    camera = cameras;
    wind = work->emberWind;
    f31 = 0.2f;
    target.x = 0.0f;
    target.y = 0.0f;
    target.z = 0.0f;
    mtxa_from_mtx(camera->unk1AC);

    mtxa_rigid_inv_tf_point_xyz(8.0f, 0.0f, 0.0f, &probe);
    mtxa_push();
    if ((int)raycast_stage_down(&probe, &hit, NULL) == 0)
    {
        f31 += 0.25f;
        target.x += -0.1f;
        target.y += 0.05f;
    }
    mtxa_pop();
    mtxa_rigid_inv_tf_point_xyz(-8.0f, 0.0f, 0.0f, &probe);
    mtxa_push();
    if ((int)raycast_stage_down(&probe, &hit, NULL) == 0)
    {
        f31 += 0.25f;
        target.x += 0.1f;
        target.y += 0.05f;
    }
    mtxa_pop();
    mtxa_rigid_inv_tf_point_xyz(0.0f, 0.0f, 8.0f, &probe);
    mtxa_push();
    if ((int)raycast_stage_down(&probe, &hit, NULL) == 0)
    {
        f31 += 0.25f;
        target.z += -0.1f;
        target.y += 0.05f;
    }
    mtxa_pop();
    mtxa_rigid_inv_tf_point_xyz(0.0f, 0.0f, -8.0f, &probe);
    mtxa_push();
    if ((int)raycast_stage_down(&probe, &hit, NULL) == 0)
    {
        f31 += 0.25f;
        target.z += 0.1f;
        target.y += 0.05f;
    }
    mtxa_pop();

    probe.y = target.y;
    target.y = 0.0f;
    mtxa_rigid_inv_tf_vec(&target, &target);
    target.y += probe.y;
    accel = work->emberAccel;
    accelVel = work->emberAccelVel;
    accelVel.x += 0.1f * (0.25f * (target.x - accel.x) - accelVel.x);
    accelVel.y += 0.1f * (0.2f * (target.y - accel.y) - accelVel.y);
    accelVel.z += 0.1f * (0.25f * (target.z - accel.z) - accelVel.z);
    accelVel.x *= 0.995f;
    accelVel.y *= 0.995f;
    accelVel.z *= 0.995f;
    accel.x += accelVel.x;
    accel.y += accelVel.y;
    accel.z += accelVel.z;
    wind.x += accel.x;
    wind.y += accel.y;
    wind.x += accel.z;  /* sic */
    work->emberAccel = accel;
    work->emberAccelVel = accelVel;
    work->emberDensity += 0.1f * (f31 - work->emberDensity);
    count = 96.0f * work->emberDensity;

    ember = work->embers;
    for (i = 96; i > 0; i--, ember++)
    {
        if (count > i && ember->life == 0)
        {
            ember->pos.x = 8.0f * RAND_FLOAT();
            ember->pos.y = 8.0f * RAND_FLOAT();
            ember->pos.z = 8.0f * RAND_FLOAT();
            ember->vel.x = 0.5f * wind.x + 0.4f * (RAND_FLOAT() - 0.5f);
            ember->vel.y = 0.5f * wind.y + 0.2f * RAND_FLOAT();
            ember->vel.z = 0.5f * wind.z + 0.4f * (RAND_FLOAT() - 0.5f);
            ember->alpha = 0.0f;
            ember->size = 0.02f * (1.0f + RAND_FLOAT());
            ember->maxSize = 0.02f * (1.0f + RAND_FLOAT());
            ember->size = ember->maxSize;
            ember->life = 30.0f + 180.0f * RAND_FLOAT();
        }
        if (ember->life > 0)
        {
            f31 = mathutil_vec_len(&ember->vel);
            ember->size = ember->maxSize * (1.0f - 1.0f / (1.0f + 2.5f * f31) * RAND_FLOAT());
            ember->vel.x += 0.025f * (f31 * (RAND_FLOAT() - 0.5f));
            ember->vel.y += 0.025f * (f31 * (RAND_FLOAT() - 0.5f));
            ember->vel.z += 0.025f * (f31 * (RAND_FLOAT() - 0.5f));
            ember->vel.y += 0.0005f;
            ember->vel.x += 0.04f * (wind.x - ember->vel.x);
            ember->vel.y += 0.04f * (wind.y - ember->vel.y);
            ember->vel.z += 0.04f * (wind.z - ember->vel.z);
            ember->pos.x += ember->vel.x + 0.05f * (RAND_FLOAT() - 0.5f);
            ember->pos.y += ember->vel.y + 0.05f * (RAND_FLOAT() - 0.5f);
            ember->pos.z += ember->vel.z + 0.05f * (RAND_FLOAT() - 0.5f);
            while (ember->pos.x < 0.0f)
                ember->pos.x += 8.0f;
            while (ember->pos.x > 8.0f)
                ember->pos.x -= 8.0f;
            while (ember->pos.y < 0.0f)
                ember->pos.y += 8.0f;
            while (ember->pos.y > 8.0f)
                ember->pos.y -= 8.0f;
            while (ember->pos.z < 0.0f)
                ember->pos.z += 8.0f;
            while (ember->pos.z > 8.0f)
                ember->pos.z -= 8.0f;
            ember->life--;
            if ((float)ember->life < 15.0f)
            {
                f31 = (float)ember->life / (float)(ember->life + 1);
                ember->alpha *= f31;
            }
            else
                ember->alpha += 0.2f * (1.0f - ember->alpha);
            if (ember->life <= 0)
            {
                ember->alpha = 0.0f;
                ember->life = 0;
            }
        }
    }
}

/* The quad primitive is preceded by an offscreen triangle strip of degenerate vertices,
 * which forces the new TEV konst colour to latch. */
static inline void emit_degenerate_tris(void)
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

void g_draw_lava_particles(struct BgLavaWork *work)
{
    Vec quad[4];
    Mtx spD8;
    Mtx spA8;
    Mtx sp78;
    Mtx sp48;
    Vec curr;
    Vec prev;
    Vec delta;
    Vec offset;
    GXColor lastColor;
    GXColor color;
    float len;
    float alpha;
    float fade;
    float size;
    int n;
    struct BgLavaEmber *ember;
    int k;
    int j;
    int i;
    s16 angle;

    mtxa_from_mtx(g_ord_tbl_stuff->matrices[0]);
    mtxa_rigid_inv_tf_tl(&curr);
    mtxa_from_mtx(g_ord_tbl_stuff->matrices[4]);
    mtxa_rigid_inv_tf_tl(&prev);
    alpha = mathutil_vec_distance(&curr, &prev);
    fade = 1.0f - 2.0f * (alpha - 1.0f);
    if (fade < 0.1f)
        fade = 0.1f;
    else if (fade > 1.0f)
        fade = 1.0f;

    GXLoadTexObj_cached(work->emberTex, 0);
    GXSetBlendMode_cached(1, 1, 1, 0);
    GXSetZMode_cached(1, 3, 0);
    g_something_with_raster_state(0);
    GXSetCullMode_cached(0);
    g_GXSetTevIndirect_zero_if_different(0);
    tex_coord_gen(0, 1, 4, 0x3C);
    g_GXSetTevOrder_wrapper(0, 0, 0, 0xFF);
    GXSetTevKColorSel_cached(0, 0xC);
    GXSetTevKAlphaSel_cached(0, 0x1C);
    GXSetTevColorIn_cached(0, 0xF, 8, 0xE, 0xF);
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
    lastColor.r = 0xD8;
    lastColor.g = 0x7A;
    lastColor.b = 0x4C;
    lastColor.a = 0xFF;
    GXSetTevKColor_cached(0, lastColor);
    emit_degenerate_tris();

    mtxa_from_mtx(g_ord_tbl_stuff->matrices[0]);
    mtxa_rigid_inv_tf_point_xyz(0.0f, 0.0f, 0.0f, &delta);
    if (delta.x < 0.0f)
        delta.x -= 8.0f;
    if (delta.y < 0.0f)
        delta.y -= 8.0f;
    if (delta.z < 0.0f)
        delta.z -= 8.0f;
    delta.x = 8.0f * (float)(int)(0.125f * delta.x);
    delta.y = 8.0f * (float)(int)(0.125f * delta.y);
    delta.z = 8.0f * (float)(int)(0.125f * delta.z);
    mtxa_translate(&delta);
    mtxa_to_mtx(spD8);
    mtxa_from_mtx(g_ord_tbl_stuff->matrices[4]);
    mtxa_translate(&delta);
    mtxa_to_mtx(spA8);

    for (i = -1; i <= 1; i++)
    {
        offset.z = 8.0f * (float)i;
        for (j = -1; j <= 1; j++)
        {
            offset.y = 8.0f * (float)j;
            for (k = -1; k <= 1; k++)
            {
                offset.x = 8.0f * (float)k;
                mtxa_from_mtx(spD8);
                mtxa_translate(&offset);
                mtxa_to_mtx(sp78);
                mtxa_from_mtx(spA8);
                mtxa_translate(&offset);
                mtxa_to_mtx(sp48);
                ember = work->embers;
                for (n = 96; n > 0; n--, ember++)
                {
                    alpha = ember->alpha * fade;
                    if (alpha <= 0.0f)
                        continue;
                    size = ember->size;
                    mtxa_from_mtx(sp78);
                    if (g_is_sphere_visible(&ember->pos, size) == 0)
                        continue;
                    mtxa_tf_point(&ember->pos, &curr);
                    if (curr.z >= -0.1f)
                        continue;
                    len = mathutil_vec_len(&curr);
                    if (len > 15.0f)
                        continue;
                    if (len > 14.0f)
                        alpha *= 15.0f - len;
                    else if (len < 1.0f)
                        alpha *= len;
                    mtxa_from_mtx(sp48);
                    prev.x = ember->pos.x - ember->vel.x;
                    prev.y = ember->pos.y - ember->vel.y;
                    prev.z = ember->pos.z - ember->vel.z;
                    mtxa_tf_point(&prev, &prev);
                    if (prev.z >= -0.1f)
                        continue;
                    {
                        float x0 = curr.x - size;
                        float y0 = curr.y - size;
                        float x1 = curr.x + size;
                        float y1 = curr.y + size;

                        quad[0].x = x0;
                        quad[0].y = y0;
                        quad[0].z = curr.z;
                        quad[1].x = x1;
                        quad[1].y = y0;
                        quad[1].z = curr.z;
                        quad[2].x = x1;
                        quad[2].y = y1;
                        quad[2].z = curr.z;
                        quad[3].x = x0;
                        quad[3].y = y1;
                        quad[3].z = curr.z;
                    }
                    prev.x *= curr.z / prev.z;
                    prev.y *= curr.z / prev.z;
                    prev.z = curr.z;
                    delta.x = prev.x - curr.x;
                    delta.y = prev.y - curr.y;
                    if (0.0f != delta.x || 0.0f != delta.y)
                    {
                        delta.z = 0.0f;
                        len = vec_normalize_len(&delta);
                        curr.x -= size * delta.x;
                        curr.y -= size * delta.y;
                        prev.x += size * delta.x;
                        prev.y += size * delta.y;
                        len += 2.0f * size;
                        angle = math_atan2(delta.y, delta.x);
                        mtxa_from_translate(&curr);
                        mtxa_rotate_z(angle);
                        mtxa_scale_xyz(len / (2.0f * size), 1.0f, 1.0f);
                        mtxa_rotate_z(-angle);
                        mtxa_translate_neg(&curr);
                        mtxa_tf_point(&quad[0], &quad[0]);
                        mtxa_tf_point(&quad[1], &quad[1]);
                        mtxa_tf_point(&quad[2], &quad[2]);
                        mtxa_tf_point(&quad[3], &quad[3]);
                    }
                    /* PC HFR: the embers are anonymous identical quads; the tag
                     * keeps each one paired with itself, not a neighbour */
                    HFR_TAG(HFR_K_BG, 0x4C450000 | (u32)(ember - work->embers));
                    color.r = 216.75f * alpha;
                    color.g = 122.399994f * alpha;
                    color.b = 76.5f * alpha;
                    if (lastColor.r != color.r || lastColor.g != color.g || lastColor.b != color.b)
                    {
                        color.a = 0xFF;
                        lastColor = color;
                        GXSetTevKColor_cached(0, lastColor);
                        emit_degenerate_tris();
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

void fn_1_81F88(struct BgLavaWorkNode *node)
{
    fn_1_7FBBC(node->work);
    if (node->work->embersEnabled != 0)
        g_draw_lava_particles(node->work);
}

void fn_1_81FD0(struct BgLavaWork *work)
{
    int i;
    struct BgLavaFountain *fountain;

    fountain = work->fountains;
    for (i = work->fountainCount; i > 0; i--, fountain++)
    {
        fountain->scroll.texY = RAND_FLOAT();
        fountain->scroll.velY = -0.0020833334f * (0.9f + 0.2f * RAND_FLOAT());
        fountain->scroll.texY2 = RAND_FLOAT();
        fountain->scroll.vel2 = -0.004166667f * (0.9f + 0.2f * RAND_FLOAT());
        fountain->vertices = NULL;
        g_something_with_stage_heap_and_lava_theme(fountain);
    }
}

void fn_1_82118(struct BgLavaWork *work)
{
    int i;
    struct BgLavaFountain *fountain;

    fountain = work->fountains;
    for (i = work->fountainCount; i > 0; i--, fountain++)
    {
        if (fountain->vertices != NULL)
        {
            OSFreeToHeap(stage_heap, fountain->vertices);
            fountain->vertices = NULL;
        }
    }
}

void fn_1_82188(struct BgLavaWork *work)
{
    int i;
    int j;
    struct BgLavaFountain *fountain;
    struct BgLavaVertex *vtx;

    fountain = work->fountains;
    for (i = work->fountainCount; i > 0; i--, fountain++)
    {
        fountain->scroll.texY += fountain->scroll.velY;
        fountain->scroll.texY2 += fountain->scroll.vel2;
        vtx = fountain->vertices;
        for (j = fountain->vertexCount; j > 0; j--, vtx++)
        {
            vtx->angle1 += vtx->angleVel1;
            vtx->angle2 += vtx->angleVel2;
        }
    }
}

void fn_1_82204(struct BgLavaWork *work, u32 mask)
{
    int i;
    Mtx *mtxs;
    Mtx *out;
    struct BgLavaVertex *vtx;
    struct BgLavaFountain *fountain;
    struct StageBgModel *model;
    void *prevFunc;
    struct GMAModel *gma;

    prevFunc = g_yet_another_unk_draw_func(fn_1_8010C);
    g_something_with_texture_scroll_2(1);
    fountain = work->fountains;
    /* The inner vertex loop reuses this counter, so only the first fountain is ever drawn. */
    for (i = work->fountainCount; i > 0; i--, fountain++)
    {
        work->currScroll = &fountain->scroll;
        model = fountain->scroll.model;
        model->flags = model->flags & ~0x10000;
        gma = model->model;
        mtxa_from_translate_xyz(0.0f, fountain->scroll.texY, 0.0f);
        fn_1_10A4CC(0, mtxa->mtxA);
        vtx = fountain->vertices;
        mtxs = g_some_buffer_ptr;
        out = mtxs;
        for (i = fountain->vertexCount; i > 0; i--, out++, vtx++)
        {
            mtxa_from_mtx(vtx->mtx);
            mathutil_mtxA_set_translate(&vtx->pos);
            mtxa_scale_s(1.15f + 0.15f * (math_sin(vtx->angle1) + math_sin(vtx->angle2)));
            mtxa_rotate_y(768.0f * (math_sin(vtx->angle1) - math_sin(vtx->angle2)));
            mtxa_rotate_x(1280.0f * (math_sin(vtx->angle1) + math_sin(vtx->angle2)));
            mtxa_to_mtx(*out);
        }
        pointer_range_advance(gma, mtxs);
        mtxa_from_mtx(g_ord_tbl_stuff->matrices[0]);
        mtxa_translate(&model->pos);
        mtxa_rotate_z(model->rotZ);
        mtxa_rotate_y(model->rotY);
        mtxa_rotate_x(model->rotX);
        mtxa_scale(&model->scale);
        g_smth_with_gpu_and_locked_cache_mtx();
        avdisp_draw_model_culled_sort_never(gma);
    }
    g_something_with_texture_scroll_2(0);
    g_yet_another_unk_draw_func(prevFunc);
}

void g_something_with_stage_heap_and_lava_theme(struct BgLavaFountain *fountain)
{
    int i;
    Mtx *mtxs;
    Mtx *m;
    Mtx **ptrs;
    int n;
    Mtx **p;
    struct BgLavaVertex *vtx;
    s16 angle1;
    s16 angle2;
    s16 *parent;
    struct GMAModel *gma;

    gma = fountain->scroll.model->model;
    n = gma->mtxCount;
    ptrs = g_some_buffer_ptr;
    mtxs = (Mtx *)(ptrs + n);
    n = fountain->scroll.model->model->mtxCount;
    fountain->vertexCount = n;
    fountain->vertices = alloc_from_heap_or_panic(stage_heap, fountain->vertexCount * sizeof(struct BgLavaVertex),
        "bg_lava.c", 0x63A);
    p = ptrs;
    m = mtxs;
    for (i = n; i > 0; i--, p++, m++)
        *p = m;
    draw_poly(gma, ptrs);
    angle1 = rand() & 0x7FFF;
    angle2 = rand() & 0x7FFF;
    m = mtxs;
    vtx = fountain->vertices;
    for (i = fountain->vertexCount; i > 0; i--, vtx++, m++)
    {
        mtxa_from_mtx(*m);
        mathutil_mtxA_get_translate(&vtx->pos);
        mathutil_mtxA_set_translate_xyz(0.0f, 0.0f, 0.0f);
        mtxa_to_mtx(vtx->mtx);
        vtx->unkC.x = RAND_FLOAT() - 0.5f;
        vtx->unkC.y = RAND_FLOAT() - 0.5f;
        vtx->unkC.z = RAND_FLOAT() - 0.5f;
        vtx->angle1 = angle1;
        vtx->angleVel1 = 384.0f * (1.0f + RAND_FLOAT());
        vtx->angle2 = angle2;
        vtx->angleVel2 = 24.0f * (1.0f + RAND_FLOAT());
        vtx->parent = NULL;
        vtx->flags = 0;
        angle1 = angle1 + 0x10000 / fountain->vertexCount;
        angle2 = angle2 + 0x10000 / fountain->vertexCount;
    }
    parent = fn_80069964(gma);
    vtx = fountain->vertices;
    for (i = fountain->vertexCount; i > 0; i--, vtx++, parent++)
    {
        if (*parent >= 0)
            fountain->vertices[*parent].parent = vtx;
        else
            vtx->flags |= 1;
    }
}

void fn_1_82720(struct BgLavaWork *work)
{
    int i;
    int j;
    struct BgLavaFire *fire;
    struct BgLavaFlame *flame;

    fire = work->fires;
    for (i = work->fireCount; i > 0; i--, fire++)
    {
        fire->spawnTimer = RAND_FLOAT();
        flame = fire->flames;
        for (j = 4; j > 0; j--, flame++)
        {
            flame->t = (float)j / 4.0f;
            flame->dt = 0.055555556f * (1.0f + 0.0f * RAND_FLOAT());
            flame->sizeXZ = 1.0f + 0.5f * RAND_FLOAT();
            flame->currSizeXZ = 0.0f;
            flame->sizeY = 0.75f + 0.5f * RAND_FLOAT();
            flame->rotY = rand() & 0x7FFF;
            flame->rotZ = (rand() & 0xFFF) - 0x800;
        }
    }
}

void fn_1_828A0(struct BgLavaWork *work)
{
    struct Effect effect;
    int i;
    int j;
    struct BgLavaFire *fire;
    struct BgLavaFlame *flame;

    memset(&effect, 0, sizeof(effect));
    fire = work->fires;
    for (i = work->fireCount; i > 0; i--, fire++)
    {
        fire->glowScale = 0.95f + 0.1f * RAND_FLOAT();
        flame = fire->flames;
        for (j = 4; j > 0; j--, flame++)
        {
            flame->t += flame->dt;
            flame->currSizeXZ += 0.5f * (flame->sizeXZ - flame->currSizeXZ);
            if (flame->t > 1.0f)
            {
                while (flame->t > 1.0f)
                    flame->t -= 1.0f;
                flame->sizeXZ = 1.0f + 0.5f * RAND_FLOAT();
                flame->currSizeXZ = 0.0f;
                flame->sizeY = 0.75f + 0.5f * RAND_FLOAT();
                flame->rotY = rand() & 0x7FFF;
                flame->rotZ = 3072.0f * (RAND_FLOAT() - 0.5f);
            }
        }
        fire->spawnTimer += 0.03333333333333333;
        if (fire->spawnTimer > 1.0f)
        {
            effect.type = 0x16;
            effect.pos = fire->model->pos;
            effect.unk18 = 0.85f;
            effect.unk1C = 0.48f;
            effect.unk20 = 0.3f;
            effect.model = work->powderModel;
            while (fire->spawnTimer > 1.0f)
            {
                effect.model = work->powderModel;
                effect.timer = 120.0f * (0.25f + RAND_FLOAT());
                effect.scale.x = 4.0f * fire->model->scale.x * (1.0f + RAND_FLOAT());
                effect.vel.x = fire->model->scale.x * (RAND_FLOAT() - 0.5f);
                effect.vel.y = fire->model->scale.y * (0.5f + RAND_FLOAT());
                effect.vel.z = fire->model->scale.z * (RAND_FLOAT() - 0.5f);
                effect.rotZ = rand() & 0x7FFF;
                effect.unk56 = 2048.0f * (RAND_FLOAT() - 0.5f);
                spawn_effect(&effect);
                fire->spawnTimer -= 1.0f;
            }
        }
    }
}

void fn_1_82CDC(struct BgLavaWork *work, u32 mask)
{
    int i;
    struct BgLavaFire *fire;
    struct StageBgModel *model;
    struct GMAModel *gma;
    struct OrdTblNode *list;
    struct BgLavaFireNode *node;

    fire = work->fires;
    for (i = work->fireCount; i > 0; i--, fire++)
    {
        model = fire->model;
        if ((model->flags & mask) != 0)
        {
            model->flags = model->flags & ~0x10000;
            mtxa_from_mtx(g_ord_tbl_stuff->matrices[0]);
            mtxa_translate(&model->pos);
            mtxa_rotate_z(model->rotZ);
            mtxa_rotate_y(model->rotY);
            mtxa_rotate_x(model->rotX);
            mtxa_scale(&model->scale);
            gma = work->fireGlowModel;
            if (g_is_sphere_visible_scaled(&gma->boundSphereCenter, gma->boundSphereRadius, model->scale.x))
            {
                list = ord_tbl_get_entry_for_pos(&gma->boundSphereCenter);
                node = ord_tbl_alloc_node(sizeof(*node));
                if (node != NULL)
                {
                    node->node.drawFunc = (void *)fn_1_82DE4;
                    mtxa_to_mtx(node->mtx);
                    node->fire = fire;
                    fn_1_CD418(list, &node->node);
                }
            }
        }
    }
}

void fn_1_82DE4(struct BgLavaFireNode *node)
{
    int i;
    struct BgLavaWork *work = world_theme.work;
    struct BgLavaFire *fire = node->fire;
    struct BgLavaFlame *flame;
    struct GMAModel *gma;
    float t;
    Vec pos;

    mtxa_from_mtx(node->mtx);
    avdisp_set_z_mode(1, 3, 0);
    gma = work->flameModel;
    flame = fire->flames;
    for (i = 4; i > 0; i--, flame++)
    {
        t = flame->t;
        if (t <= 0.0f)
            continue;
        mtxa_push();
        mtxa_rotate_z(flame->rotZ);
        mtxa_rotate_y(flame->rotY);
        mtxa_scale_xyz(flame->currSizeXZ * (1.0f - t), flame->sizeY * t, 1.0f);
        if (t > 0.5f)
            avdisp_set_alpha(1.0f - 2.0f * (t - 0.5f));
        g_smth_with_gpu_and_locked_cache_mtx();
        avdisp_draw_model_unculled_sort_never(gma);
        mtxa_pop();
    }
    gma = work->fireGlowModel;
    mtxa_sq_from_identity();
    mtxa_scale_s(fire->glowScale);
    mathutil_mtxA_get_translate(&pos);
    t = gma->boundSphereRadius;
    if (pos.z > -(0.1f + t))
    {
        float s = (pos.z + t) / pos.z;

        pos.x *= s;
        pos.y *= s;
        pos.z += t;
        mathutil_mtxA_set_translate(&pos);
        mtxa_scale_s(s);
    }
    g_smth_with_gpu_and_locked_cache_mtx();
    avdisp_draw_model_unculled_sort_never(gma);
    avdisp_set_z_mode(1, 3, 1);
}

int fn_1_82FB8(int index, struct GMAModelEntry *entry)
{
    struct BgLavaWork *work = world_theme.work;

    switch (index)
    {
    case 0:
        work->glowTex = entry->model->texObjs;
        break;
    case 1:
        work->emberTex = entry->model->texObjs;
        GXInitTexObj(work->emberTex, GXGetTexObjData(work->emberTex), GXGetTexObjWidth(work->emberTex),
            GXGetTexObjHeight(work->emberTex), GXGetTexObjFmt(work->emberTex), GXGetTexObjWrapS(work->emberTex),
            GXGetTexObjWrapT(work->emberTex), 0);
        break;
    case 2:
        work->powderModel = entry->model;
        work->powderTex = entry->model->texObjs;
        GXInitTexObj(work->powderTex, GXGetTexObjData(work->powderTex), GXGetTexObjWidth(work->powderTex),
            GXGetTexObjHeight(work->powderTex), GXGetTexObjFmt(work->powderTex), GXGetTexObjWrapS(work->powderTex),
            GXGetTexObjWrapT(work->powderTex), 0);
        break;
    case 3:
        work->indTex = entry->model->texObjs;
        break;
    case 4:
        work->flameModel = entry->model;
        break;
    case 5:
        work->fireGlowModel = entry->model;
        break;
    }
    return 1;
}

int fn_1_83154(int index, struct StageBgModel *model)
{
    struct BgLavaWork *work = world_theme.work;

    switch (index)
    {
    case 0:
    case 1:
    case 2:
        if (model->model != NULL && work->scrollCount < 24)
        {
            work->scrolls[work->scrollCount].model = model;
            work->scrollCount++;
        }
        break;
    case 3:
        if (model->model != NULL && (model->model->flags & 8) != 0 && work->fountainCount < 2)
        {
            work->fountains[work->fountainCount].scroll.model = model;
            work->fountainCount++;
        }
        break;
    case 4:
        if (work->fireCount < 16)
        {
            work->fires[work->fireCount].model = model;
            work->fireCount++;
        }
        break;
    }
    return 1;
}
