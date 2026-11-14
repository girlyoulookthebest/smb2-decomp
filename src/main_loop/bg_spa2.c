/* Coronation space background: a ring-shaped station with a shadow render
 * pass, dome specular render pass, and drifting cloud layers. */

#include "types.h"
#include "avdisp.h"
#include "background.h"
#include "camera.h"
#include "futsal_data.h"
#include "gx.h"
#include "libc.h"
#include "light.h"
#include "mathutil.h"
#include "mode.h"
#include "nl2ngc.h"
#include "rand.h"
#include "stage.h"
#ifndef __MWERKS__
#include "frustum.h" /* prototypes: float args must not be promoted to double on x86 */
#endif

extern struct { u32 flags; } g_some_render_flag;
extern void *main_heap;
extern struct Light s_lightPool[];

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

/* A texture rendered to at runtime. */
struct BgSpa2Layer
{
    /*0x00*/ s32 ready;
    /*0x04*/ GXTexObj tex;
    /*0x24*/ void *texBuf;
    /*0x28*/ Mtx mtx;
};  /* size = 0x58 */

struct BgSpa2Data
{
    /*0x00*/ struct BgSpa2Layer shadow;
    /*0x58*/ Mtx viewShadowMtx;
    /*0x88*/ Mtx texMtx;
    /*0xB8*/ struct TevStageInfo last;
    /*0xE4*/ struct BgSpa2Layer spec;
};  /* size = 0x13C */

struct BgSpa2Entry
{
    struct StageBgModel *model;
    s32 receiveShadow;
};

struct BgSpa2Cloud
{
    /*0x00*/ struct StageBgModel *model;
    /*0x04*/ float texX;
    /*0x08*/ float texY;
    /*0x0C*/ float velX;
    /*0x10*/ float velY;
};

struct BgSpa2Work
{
    /*0x000*/ s32 initialized;
    /*0x004*/ struct BgSpa2Data *data;
    /*0x008*/ u32 flags;
    /*0x00C*/ u32 entryCount;
    /*0x010*/ struct BgSpa2Entry entries[32];
    /*0x110*/ struct StageBgModel *arm;
    /*0x114*/ struct GMAModel *shadowModel;
    /*0x118*/ struct GMAModel *shadowBoxVtx;
    /*0x11C*/ s32 cloudCount;
    /*0x120*/ struct BgSpa2Cloud clouds[2];
    /*0x148*/ struct StageBgModel *dome;
    /*0x14C*/ u16 specCount;
    /*0x14E*/ u16 boxCount;
    /*0x150*/ struct GMAModel *specModels[4];
    /*0x160*/ struct GMAModel *boxModels[4];
};

struct BgSpa2EntryNode
{
    /*0x00*/ struct OrdTblNode node;
    /*0x08*/ struct BgSpa2Entry *entry;
    /*0x0C*/ s32 lightGroup;
    /*0x10*/ Mtx mtx;
};

struct BgSpa2CloudNode
{
    /*0x00*/ struct OrdTblNode node;
    /*0x08*/ struct BgSpa2Cloud *cloud;
    /*0x0C*/ s32 lightGroup;
    /*0x10*/ Mtx mtx;
};

struct BgSpa2Shape
{
    /*0x00*/ s32 vtxCount;
    u8 filler4[4];
    /*0x08*/ s32 vtxOffset;
};

void *OSAllocFromHeap(void *heap, u32 size);
void OSFreeToHeap(void *heap, void *ptr);
u32 GXGetTexBufferSize(u16 width, u16 height, u32 format, u8 mipmap, u8 maxLod);
void GXInitTexObj(GXTexObj *obj, void *image, u16 width, u16 height, int format, int wrapS, int wrapT, u8 mipmap);
void GXInitTexObjLOD(GXTexObj *obj, int minFilt, int magFilt, float minLod, float maxLod, float lodBias, u8 biasClamp,
    u8 edgeLod, int maxAniso);
void GXSetTexCopySrc_cached(u16 left, u16 top, u16 width, u16 height);
void GXSetTexCopyDst_cached(u16 width, u16 height, int format, u8 mipmap);
void GXCopyTex(void *dest, u8 clear);
void GXGetViewportv(float *viewport);
void GXGetProjectionv(float *proj);
void GXInvalidateTexAll(void);
void GXSetNumChans_cached(u8 n);
void GXSetNumTexGens_cached(u8 n);
void GXSetNumTevStages_cached(u8 n);
void GXSetNumIndStages_cached(u8 n);
void GXSetTexCoordGen2_cached(int dst, int func, int src, int mtx, int normalize, int postMtx);
static inline void tex_coord_gen(int dst, int func, int src, int mtx)
{
    GXSetTexCoordGen2_cached(dst, func, src, mtx, 0, 0x7D);
}
void GXLoadTexObj_cached(GXTexObj *obj, int map);
void GXLoadTexMtxImm(Mtx m, int id, int type);
void GXSetTevColorIn_cached(int stage, int a, int b, int c, int d);
void GXSetTevColorOp_cached(int stage, int op, int bias, int scale, u8 clamp, int outReg);
void GXSetTevAlphaIn_cached(int stage, int a, int b, int c, int d);
void GXSetTevAlphaOp_cached(int stage, int op, int bias, int scale, u8 clamp, int outReg);
void g_GXSetTevOrder_wrapper(int stage, int coord, int map, int color);
void g_GXSetTevIndirect_zero_if_different(int stage);
void g_something_with_raster_state(int);
void g_smth_with_bg_color_drawing(GXColor *color);
void *g_yet_another_unk_draw_func(void *func);
void *fn_1_10A69C(void *func);
void g_smth_with_bg_model_names(struct BgModelSearch *search, int (*callback)(int, struct GMAModelEntry *));
void g_math_unk13(Vec *eye, Vec *up, Vec *target);
void C_MTXOrtho(Mtx44 m, float top, float bottom, float left, float right, float near, float far);
struct BgSpa2Shape *fn_8006991C(struct GMAModel *model);
void fn_1_D8130(int, int, struct RenderEffect *);
struct OrdTblNode *fn_1_CCD5C(int);
struct OrdTblNode *ord_tbl_get_entry_for_pos(Vec *pos);
void *ord_tbl_alloc_node(u32 size);
void fn_1_CD418(struct OrdTblNode *list, struct OrdTblNode *node);
void mtxa_from_translate_xyz(float x, float y, float z);
void mtxa_from_rotate_x(s16 angle);

void fn_1_8C984(void);
void fn_1_8C9A4(struct BgSpa2Work *);
void fn_1_8C9A8(struct BgSpa2Work *);
void fn_1_8C9AC(struct BgSpa2Work *, u32);
void fn_1_8CB38(struct BgSpa2EntryNode *);
void fn_1_8CBB4(struct BgSpa2Work *);
void fn_1_8CCF8(struct BallEnvArg *);
void fn_1_8D020(struct BallEnvArg *);
void fn_1_8D3D8(void);
int fn_1_8D520(void);
void fn_1_8D528(void);
void fn_1_8D588(void);
void fn_1_8D5F4(struct RenderEffect *);
void fn_1_8D6C8(struct RenderEffect *);
void fn_1_8D758(struct RenderEffect *);
void fn_1_8D75C(u32, struct RenderEffect *);
void fn_1_8D7C4(u32, struct RenderEffect *);
int fn_1_8D868(struct RenderEffect *);
void fn_1_8D8DC(struct RenderEffect *);
void fn_1_8D938(struct RenderEffect *);
int fn_1_8DEE0(struct RenderEffect *);
void fn_1_8DF54(struct RenderEffect *);
void fn_1_8DFB0(struct RenderEffect *);
void fn_1_8E2EC(struct BgSpa2Work *);
void fn_1_8E414(struct BgSpa2Work *);
void fn_1_8E454(struct BgSpa2Work *, u32);
void fn_1_8E560(struct BgSpa2CloudNode *);
int fn_1_8E714(int, struct GMAModelEntry *);
int fn_1_8E8A8(int, struct StageBgModel *);

static struct BgModelSearch lbl_1_data_324B8[] =
{
    { BG_MDL_CMP_FULL, "SPA_CORO_SHADOW_A" },
    { BG_MDL_CMP_FULL, "SPA_CORO_SHADOW_BOX_vtx" },
    { BG_MDL_CMP_FULL, "SPA_CORO_DOME_SPEC_A" },
    { BG_MDL_CMP_FULL, "SPA_CORO_DOME_SPEC_B" },
    { BG_MDL_CMP_FULL, "SPA_CORO_DOME_SPEC_C" },
    { BG_MDL_CMP_FULL, "SPA_CORO_DOME_BOX" },
    { BG_MDL_CMP_FULL, "SPA_CORO_DOME_BOX_B" },
    { BG_MDL_CMP_FULL, "SPA_CORO_DOME_BOX_C" },
    { BG_MDL_CMP_END },
};

static struct BgModelSearch lbl_1_data_32564[] =
{
    { BG_MDL_CMP_PREFIX, "SPA_CORO_ARM_" },
    { BG_MDL_CMP_PREFIX, "SPA_CORO_CENTRE_" },
    { BG_MDL_CMP_PREFIX, "SPA_CORO_DOME_" },
    { BG_MDL_CMP_PREFIX, "SPA_CORO_RING_A" },
    { BG_MDL_CMP_PREFIX, "SPA_CORO_RING_C" },
    { BG_MDL_CMP_PREFIX, "SPA_CLOUD_STAR_" },
    { BG_MDL_CMP_END },
};

static struct RenderEffectFuncs lbl_1_data_3259C = { fn_1_8D5F4, fn_1_8D6C8, fn_1_8D758, fn_1_8D75C };
static struct RenderEffectFuncs lbl_1_data_325AC = { fn_1_8D5F4, fn_1_8D6C8, fn_1_8D758, fn_1_8D7C4 };

void bg_spa2_init(void)
{
    struct BgSpa2Work *work = world_theme.work;
    struct RenderEffect rendEfc;

    bg_init_base();
    work->flags = 0;
    if (modeCtrl.gameType == 9)
        work->flags |= 1;
    switch (current_stage_id)
    {
    case 0xFE:
    case 0x137:
    case 0x13A:
    case 0x13E:
    case 0x140:
    case 0x141:
    case 0x144:
        work->flags |= 0xC;
        break;
    }
    work->specCount = 0;
    work->boxCount = 0;
    g_smth_with_bg_model_names(lbl_1_data_324B8, fn_1_8E714);
    work->entryCount = 0;
    work->cloudCount = 0;
    g_init_stagedef_bg_fg_models(stagedef->bgModels, stagedef->bgModelCount, lbl_1_data_32564, fn_1_8E8A8);
    g_init_stagedef_bg_fg_models(stagedef->fgModels, stagedef->fgModelCount, lbl_1_data_32564, fn_1_8E8A8);
    fn_1_8C9A4(work);
    fn_1_8E2EC(work);
    work->data = NULL;
    memset(&rendEfc, 0, sizeof(rendEfc));
    rendEfc.cameraMask = 0xFFFF;
    if (modeCtrl.gameType == 9)
        rendEfc.funcs = &lbl_1_data_325AC;
    else
        rendEfc.funcs = &lbl_1_data_3259C;
    fn_1_D8130(0, 0, &rendEfc);
    world_theme.unk98 = fn_1_8C984;
}

void bg_spa2_tick(void)
{
    struct BgSpa2Work *work = world_theme.work;

    g_smth_with_challenge_mode_var_and_3p();
    bg_tick_base();
    if (!is_game_paused_and_in_view_stage())
    {
        fn_1_8C9A8(work);
        fn_1_8E414(work);
    }
}

void bg_spa2_dest(void) {}

void bg_spa2_disp(void)
{
    struct BgSpa2Work *work = world_theme.work;
    u32 mask = g_smth_with_challenge_mode_var_and_3p();

    mtxa_from_mtx(g_ord_tbl_stuff->matrices[0]);
    g_set_active_light_group(4);
    fn_1_8C9AC(work, mask);
    fn_1_8E454(work, mask);
    bg_disp_base();
}

void bg_spa2_item_coin_coli(int arg0) {}

void fn_1_8C984(void)
{
    fn_1_8D588();
}

void fn_1_8C9A4(struct BgSpa2Work *work) {}

void fn_1_8C9A8(struct BgSpa2Work *work) {}

void fn_1_8C9AC(struct BgSpa2Work *work, u32 mask)
{
    int n;
    struct BgSpa2Entry *entry;
    struct StageBgModel *bgModel;
    struct GMAModel *model;
    Mtx sp8;
    struct BgSpa2Data *data;

    if ((g_some_render_flag.flags & 0x84) != 0)
        return;
    data = work->data;
    if (data == NULL || data->shadow.ready == 0)
        return;
    g_yet_another_unk_draw_func(fn_1_8CCF8);
    entry = work->entries;
    for (n = work->entryCount; n > 0; n--, entry++)
    {
        bgModel = entry->model;
        if ((bgModel->flags & mask) != 0)
        {
            model = bgModel->model;
            bgModel->flags &= ~0x10000;
            mtxa_from_translate(&bgModel->pos);
            mtxa_rotate_z(bgModel->rotZ);
            mtxa_rotate_y(bgModel->rotY);
            mtxa_rotate_x(bgModel->rotX);
            mtxa_scale(&bgModel->scale);
            mtxa_to_mtx(sp8);
            mtxa_mult_left(g_ord_tbl_stuff->matrices[0]);
            if (g_is_sphere_visible_scaled(&model->boundSphereCenter, model->boundSphereRadius, bgModel->scale.x))
            {
                if (entry->receiveShadow == 0)
                {
                    load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
                    avdisp_draw_model_unculled_sort_auto(model);
                }
                else
                {
                    struct OrdTblNode *list = fn_1_CCD5C(0);
                    struct BgSpa2EntryNode *node = ord_tbl_alloc_node(sizeof(*node));

                    if (node != NULL)
                    {
                        node->node.drawFunc = (void *)fn_1_8CB38;
                        node->entry = entry;
                        node->lightGroup = get_active_light_group();
                        mtxa_to_mtx(node->mtx);
                        fn_1_CD418(list, &node->node);
                    }
                }
            }
        }
    }
    g_yet_another_unk_draw_func(NULL);
}

void fn_1_8CB38(struct BgSpa2EntryNode *node)
{
    struct GMAModel *model = node->entry->model->model;

    load_light_group_cached(node->lightGroup);
    g_yet_another_unk_draw_func(fn_1_8CCF8);
    mtxa_from_mtx(node->mtx);
    load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
    avdisp_draw_model_unculled_sort_never(model);
    g_yet_another_unk_draw_func(NULL);
}

void fn_1_8CBB4(struct BgSpa2Work *work)
{
    int n;
    int i;
    struct StageBgModel *dome;
    struct GMAModel **list;
    struct GMAModel *model;
    struct BgSpa2Data *data;

    if ((g_some_render_flag.flags & 0x84) != 0)
        return;
    data = work->data;
    if (data == NULL || data->shadow.ready == 0)
        return;
    GXInvalidateTexAll();
    fn_1_8D3D8();
    fn_1_10A69C(fn_1_8D520);
    dome = work->dome;
    mtxa_from_mtx(g_ord_tbl_stuff->matrices[0]);
    mtxa_translate(&dome->pos);
    mtxa_rotate_z(dome->rotZ);
    mtxa_rotate_y(dome->rotY);
    mtxa_rotate_x(dome->rotX);
    mtxa_scale(&dome->scale);
    list = work->boxModels;
    for (n = work->boxCount; n > 0; n--, list++)
    {
        model = *list;
        for (i = 0; i < 4; i++)
        {
            mtxa_push();
            if (i != 0)
                mtxa_rotate_y(i << 14);
            if (g_is_sphere_visible_scaled(&model->boundSphereCenter, model->boundSphereRadius, dome->scale.x))
            {
                fn_1_8D528();
                load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
                avdisp_draw_model_unculled_sort_never(model);
            }
            mtxa_pop();
        }
    }
    fn_1_10A69C(NULL);
}

void fn_1_8CCF8(struct BallEnvArg *arg)
{
    struct TevStageInfo sp8 = arg->tev;
    struct BgSpa2Data *data = ((struct BgSpa2Work *)world_theme.work)->data;

    if (arg->unk0 != 0)
    {
        mtxa_push();
        mtxa_mult_left(data->viewShadowMtx);
        mtxa_to_mtx(data->texMtx);
        mtxa_pop();
    }
    else if (sp8.tevStage == data->last.tevStage && sp8.texCoordId == data->last.texCoordId
     && sp8.texMapId == data->last.texMapId && sp8.texMtxId == data->last.texMtxId)
    {
        sp8.tevStage += 1;
        sp8.texCoordId += 1;
        sp8.texMapId += 1;
        sp8.texMtxId += 3;
        arg->tev = sp8;
        return;
    }
    data->last = sp8;
    GXLoadTexObj_cached(&data->shadow.tex, sp8.texMapId);
    GXLoadTexMtxImm(data->texMtx, sp8.texMtxId, 1);
    tex_coord_gen(sp8.texCoordId, 1, 0, sp8.texMtxId);
    g_GXSetTevOrder_wrapper(sp8.tevStage, sp8.texCoordId, sp8.texMapId, 0xFF);
    GXSetTevColorIn_cached(sp8.tevStage, 0, 0xF, 8, 0xF);
    GXSetTevColorOp_cached(sp8.tevStage, 0, 0, 0, 1, 0);
    GXSetTevAlphaIn_cached(sp8.tevStage, 7, 7, 7, 0);
    GXSetTevAlphaOp_cached(sp8.tevStage, 0, 0, 0, 1, 0);
    g_GXSetTevIndirect_zero_if_different(sp8.tevStage);
    sp8.tevStage += 1;
    sp8.texCoordId += 1;
    sp8.texMapId += 1;
    sp8.texMtxId += 3;
    arg->tev = sp8;
}

void fn_1_8D020(struct BallEnvArg *arg)
{
    struct TevStageInfo sp8 = arg->tev;
    struct BgSpa2Data *data = ((struct BgSpa2Work *)world_theme.work)->data;

    if (arg->unk0 != 0)
    {
        mtxa_push();
        mtxa_mult_left(data->viewShadowMtx);
        mtxa_to_mtx(data->texMtx);
        mtxa_pop();
    }
    else if (sp8.tevStage == data->last.tevStage && sp8.texCoordId == data->last.texCoordId
     && sp8.texMapId == data->last.texMapId && sp8.texMtxId == data->last.texMtxId)
    {
        sp8.tevStage += 2;
        sp8.texCoordId += 1;
        sp8.texMapId += 1;
        sp8.texMtxId += 3;
        arg->tev = sp8;
        return;
    }
    data->last = sp8;
    GXLoadTexObj_cached(&data->shadow.tex, sp8.texMapId);
    GXLoadTexMtxImm(data->texMtx, sp8.texMtxId, 1);
    tex_coord_gen(sp8.texCoordId, 1, 0, sp8.texMtxId);
    g_GXSetTevOrder_wrapper(sp8.tevStage, sp8.texCoordId, sp8.texMapId, 0xFF);
    GXSetTevColorIn_cached(sp8.tevStage, 0xF, 0xF, 0xF, 8);
    GXSetTevColorOp_cached(sp8.tevStage, 0, 0, 1, 1, 3);
    GXSetTevAlphaIn_cached(sp8.tevStage, 7, 7, 7, 0);
    GXSetTevAlphaOp_cached(sp8.tevStage, 0, 0, 0, 1, 0);
    g_GXSetTevIndirect_zero_if_different(sp8.tevStage);
    sp8.tevStage += 1;
    sp8.texCoordId += 1;
    sp8.texMapId += 1;
    sp8.texMtxId += 3;
    g_GXSetTevOrder_wrapper(sp8.tevStage, 0xFF, 0xFF, 0xFF);
    GXSetTevColorIn_cached(sp8.tevStage, 0, 0xF, 6, 0xF);
    GXSetTevColorOp_cached(sp8.tevStage, 0, 0, 0, 1, 0);
    GXSetTevAlphaIn_cached(sp8.tevStage, 7, 7, 7, 0);
    GXSetTevAlphaOp_cached(sp8.tevStage, 0, 0, 0, 1, 0);
    g_GXSetTevIndirect_zero_if_different(sp8.tevStage);
    sp8.tevStage += 1;
    arg->tev = sp8;
}

void fn_1_8D3D8(void)
{
    struct BgSpa2Layer *layer = &((struct BgSpa2Work *)world_theme.work)->data->spec;
    int stage;
    int coord;

    GXSetBlendMode_cached(1, 1, 1, 0);
    GXSetZMode_cached(1, 3, 0);
    g_something_with_raster_state(0);
    GXSetNumChans_cached(0);
    GXLoadTexObj_cached(&layer->tex, 0);
    stage = 0;
    coord = 0;
    tex_coord_gen(coord, 0, 0, 0x1E);
    g_GXSetTevOrder_wrapper(stage, coord, 0, 0xFF);
    GXSetTevColorIn_cached(stage, 0xF, 0xF, 0xF, 8);
    GXSetTevColorOp_cached(stage, 0, 0, 0, 1, 0);
    GXSetTevAlphaIn_cached(stage, 7, 7, 7, 4);
    GXSetTevAlphaOp_cached(stage, 0, 0, 0, 1, 0);
    g_GXSetTevIndirect_zero_if_different(stage);
    stage++;
    coord++;
    GXSetNumTevStages_cached(stage);
    GXSetNumTexGens_cached(coord);
    GXSetNumIndStages_cached(0);
}

int fn_1_8D520(void)
{
    return 1;
}

void fn_1_8D528(void)
{
    struct BgSpa2Layer *layer = &((struct BgSpa2Work *)world_theme.work)->data->spec;

    mtxa_push();
    mtxa_mult_left(layer->mtx);
    GXLoadTexMtxImm(mtxa->mtxA, 0x1E, 0);
    mtxa_pop();
}

void fn_1_8D588(void)
{
    struct BgSpa2Work *work = world_theme.work;
    Mtx sp8;

    if (work->data != NULL)
    {
        struct BgSpa2Data *data = work->data;

        mtxa_from_mtx(g_ord_tbl_stuff->matrices[0]);
        mtxa_rigid_invert();
        mtxa_to_mtx(sp8);
        mtxa_mult_left(data->shadow.mtx);
        mtxa_to_mtx(data->viewShadowMtx);
    }
}

void fn_1_8D5F4(struct RenderEffect *rendEfc)
{
    struct BgSpa2Data *data;
    int ok;

    rendEfc->enableFlags = 0xD;
    data = OSAllocFromHeap(main_heap, sizeof(*data));
    if (data == NULL)
    {
        rendEfc->state = 0;
        return;
    }
    memset(data, 0, sizeof(*data));
    rendEfc->work = data;
    ok = 1;
    ok &= fn_1_8D868(rendEfc);
    ok &= fn_1_8DEE0(rendEfc);
    if (ok == 0)
    {
        rendEfc->state = 3;
        return;
    }
    if (world_theme.bgId == 0x16)
        ((struct BgSpa2Work *)world_theme.work)->data = data;
}

void fn_1_8D6C8(struct RenderEffect *rendEfc)
{
    struct BgSpa2Data *data = rendEfc->work;

    if (data != NULL)
    {
        fn_1_8DF54(rendEfc);
        fn_1_8D8DC(rendEfc);
        OSFreeToHeap(main_heap, data);
        rendEfc->work = NULL;
        if (world_theme.bgId == 0x16)
            ((struct BgSpa2Work *)world_theme.work)->data = NULL;
    }
}

void fn_1_8D758(struct RenderEffect *rendEfc) {}

void fn_1_8D75C(u32 mode, struct RenderEffect *rendEfc)
{
    switch ((int)mode)
    {
    case 1:
        fn_1_8D938(rendEfc);
        break;
    case 4:
        fn_1_8DFB0(rendEfc);
        break;
    case 8:
        fn_1_8CBB4(world_theme.work);
        break;
    }
}

void fn_1_8D7C4(u32 mode, struct RenderEffect *rendEfc)
{
    struct BgSpa2Layer *layer = &((struct BgSpa2Data *)rendEfc->work)->spec;

    if ((mode == 4 || mode == 8) && g_current_camera->idx != 0)
    {
        layer->ready = 0;
        return;
    }
    switch ((int)mode)
    {
    case 1:
        fn_1_8D938(rendEfc);
        break;
    case 4:
        fn_1_8DFB0(rendEfc);
        break;
    case 8:
        fn_1_8CBB4(world_theme.work);
        break;
    }
}

int fn_1_8D868(struct RenderEffect *rendEfc)
{
    struct BgSpa2Data *data = rendEfc->work;
    struct BgSpa2Layer *layer = &data->shadow;

    layer->ready = 0;
    layer->texBuf = OSAllocFromHeap(main_heap, GXGetTexBufferSize(0x140, 0xE0, 4, 0, 0));
    return layer->texBuf != 0;
}

void fn_1_8D8DC(struct RenderEffect *rendEfc)
{
    struct BgSpa2Data *data = rendEfc->work;
    struct BgSpa2Layer *layer = &data->shadow;

    layer->ready = 0;
    if (layer->texBuf != NULL)
    {
        OSFreeToHeap(main_heap, layer->texBuf);
        layer->texBuf = NULL;
    }
}

void fn_1_8D938(struct RenderEffect *rendEfc)
{
    struct GMAModel *shadowModel;
    struct BgSpa2Layer *layer;
    struct StageBgModel *arm;
    struct BgSpa2Data *data;
    struct BgSpa2Work *work;
    struct GMAModel *boxVtx;
    Mtx sp7C;
    Mtx44 sp3C;
    Vec sp30;
    Vec sp24;
    Vec sp18;
    Vec spC;
    GXColor sp8;

    data = rendEfc->work;
    layer = &data->shadow;
    work = world_theme.work;
    arm = work->arm;
    shadowModel = work->shadowModel;
    boxVtx = work->shadowBoxVtx;
    if (arm != NULL)
    {
        int width = 640;
        int height = 448;
        int texWidth = width >> 1;
        int texHeight = height >> 1;
        struct Light *light = s_lightPool;
        float minX;
        float minY;
        float minZ;
        float maxX;
        float maxY;
        float maxZ;
        float sizeX;
        float sizeY;
        float sizeZ;
        float dist;
        float near;
        float far;
        struct BgSpa2Shape *shape;

        dist = 5.0f * (boxVtx->boundSphereRadius * arm->scale.x);
        minX = 10000.0f;
        minY = 10000.0f;
        minZ = 10000.0f;
        maxX = -10000.0f;
        maxY = -10000.0f;
        maxZ = -10000.0f;
        shape = fn_8006991C(boxVtx);
        if (shape != NULL)
        {
            Vec *vtx;
            int n;

            mtxa_from_rotate_x(-light->rotX);
            mtxa_rotate_y(-light->rotY);
            mtxa_rotate_z(arm->rotZ);
            mtxa_rotate_y(arm->rotY);
            mtxa_rotate_x(arm->rotX);
            mtxa_scale(&arm->scale);
            minX = 10000.0f;
            minY = 10000.0f;
            minZ = 10000.0f;
            maxX = -10000.0f;
            maxY = -10000.0f;
            maxZ = -10000.0f;
            vtx = (Vec *)((u8 *)shape + shape->vtxOffset);
            for (n = shape->vtxCount; n > 0; n--, vtx = (Vec *)((u8 *)vtx + 0x40))
            {
                mtxa_tf_point(vtx, &sp24);
                if (minX > sp24.x)
                    minX = sp24.x;
                if (minY > sp24.y)
                    minY = sp24.y;
                if (minZ > sp24.z)
                    minZ = sp24.z;
                if (maxX < sp24.x)
                    maxX = sp24.x;
                if (maxY < sp24.y)
                    maxY = sp24.y;
                if (maxZ < sp24.z)
                    maxZ = sp24.z;
            }
        }
        sizeX = 0.5f * (maxX - minX);
        sizeY = 0.5f * (maxY - minY);
        sizeZ = 0.5f * (maxZ - minZ);
        sp30.x = 0.5f * (minX + maxX);
        sp30.y = 0.5f * (minY + maxY);
        sp30.z = 0.5f * (minZ + maxZ);
        mtxa_from_rotate_x(-light->rotX);
        mtxa_rotate_y(-light->rotY);
        mtxa_rotate_z(arm->rotZ);
        mtxa_rotate_y(arm->rotY);
        mtxa_rotate_x(arm->rotX);
        mtxa_rigid_inv_tf_point(&sp30, &sp30);
        mtxa_from_translate(&arm->pos);
        mtxa_rotate_z(arm->rotZ);
        mtxa_rotate_y(arm->rotY);
        mtxa_rotate_x(arm->rotX);
        mtxa_tf_point(&sp30, &sp18);
        mtxa_tf_vec_xyz(1000.0f, 0.0f, 1000.0f, &spC);
        mtxa_from_rotate_y(light->rotY);
        mtxa_rotate_x(light->rotX);
        mtxa_tf_vec_xyz(0.0f, 0.0f, -dist, &sp24);
        sp24.x += sp18.x;
        sp24.y += sp18.y;
        sp24.z += sp18.z;
        g_math_unk13(&sp24, &spC, &sp18);
        mtxa_to_mtx(sp7C);
        near = dist - 1.2f * sizeZ;
        far = dist + 1.2f * sizeZ;
        C_MTXOrtho(sp3C, -sizeY, sizeY, sizeX, -sizeX, near, far);
        g_something_with_GXSetProjectionv(sp3C, 1);
        mtxa_from_identity();
        mtxa->mtxA[0][0] = -0.5f / sizeX;
        mtxa->mtxA[0][3] = 0.5f;
        mtxa->mtxA[1][1] = 0.5f / sizeY;
        mtxa->mtxA[1][3] = 0.5f;
        mtxa_mult_right(sp7C);
        mtxa_to_mtx(layer->mtx);
        GXSetViewport(0.0f, 0.0f, width, height, 0.0f, 1.0f);
        GXSetScissor_cached(0, 0, width, height);
        sp8.r = 0;
        sp8.g = 0;
        sp8.b = 0;
        sp8.a = 0;
        g_smth_with_bg_color_drawing(&sp8);
        set_post_mult_color(0.5f, 0.5f, 0.45f, 1.0f);
        mtxa_from_mtx(sp7C);
        mtxa_translate(&arm->pos);
        mtxa_rotate_z(arm->rotZ);
        mtxa_rotate_y(arm->rotY);
        mtxa_rotate_x(arm->rotX);
        mtxa_scale(&arm->scale);
        g_smth_with_gpu_and_locked_cache_mtx();
        avdisp_draw_model_unculled_sort_never(shadowModel);
        set_post_mult_color(1.0f, 1.0f, 1.0f, 1.0f);
        GXSetTexCopySrc_cached(0, 0, width, height);
        GXSetTexCopyDst_cached(texWidth, texHeight, 4, 1);
        GXCopyTex(layer->texBuf, 1);
        GXInitTexObj(&layer->tex, layer->texBuf, texWidth, texHeight, 4, 0, 0, 0);
        GXInitTexObjLOD(&layer->tex, 1, 1, 0.0f, 10.0f, 0.0f, 0, 0, 0);
        layer->ready = 1;
        camera_apply_viewport(g_something_with_camera3);
    }
}

int fn_1_8DEE0(struct RenderEffect *rendEfc)
{
    struct BgSpa2Data *data = rendEfc->work;
    struct BgSpa2Layer *layer = &data->spec;

    layer->ready = 0;
    layer->texBuf = OSAllocFromHeap(main_heap, GXGetTexBufferSize(0x80, 0x80, 1, 0, 0));
    return layer->texBuf != 0;
}

void fn_1_8DF54(struct RenderEffect *rendEfc)
{
    struct BgSpa2Data *data = rendEfc->work;
    struct BgSpa2Layer *layer = &data->spec;

    layer->ready = 0;
    if (layer->texBuf != NULL)
    {
        OSFreeToHeap(main_heap, layer->texBuf);
        layer->texBuf = NULL;
    }
}

void fn_1_8DFB0(struct RenderEffect *rendEfc)
{
    struct BgSpa2Layer *layer = &((struct BgSpa2Data *)rendEfc->work)->spec;
    struct BgSpa2Work *work = world_theme.work;
    float sp28[6];
    float spC[7];
    GXColor sp8;

    if (work->dome != NULL)
    {
        int width;
        int height;
        int texWidth;
        int texHeight;
        int n;
        int i;
        struct GMAModel **list;
        struct StageBgModel *dome;
        struct GMAModel *model;

        GXGetViewportv(sp28);
        width = 256;
        height = 256;
        texWidth = width >> 1;
        texHeight = height >> 1;
        GXSetViewport(0.0f, 0.0f, width, height, 0.0f, 1.0f);
        GXSetScissor_cached(0, 0, width, height);
        GXGetProjectionv(spC);
        mtxa_from_identity();
        mtxa->mtxA[0][0] = -0.5f * spC[1];
        mtxa->mtxA[0][2] = 0.5f + 0.5f * spC[2];
        mtxa->mtxA[1][1] = 0.5f * spC[3];
        mtxa->mtxA[1][2] = 0.5f + 0.5f * spC[4];
        mtxa_to_mtx(layer->mtx);
        sp8.r = 0;
        sp8.g = 0;
        sp8.b = 0;
        sp8.a = 0;
        g_smth_with_bg_color_drawing(&sp8);
        dome = work->dome;
        mtxa_from_mtx(g_ord_tbl_stuff->matrices[0]);
        mtxa_translate(&dome->pos);
        mtxa_rotate_z(dome->rotZ);
        mtxa_rotate_y(dome->rotY);
        mtxa_rotate_x(dome->rotX);
        mtxa_scale(&dome->scale);
        g_yet_another_unk_draw_func(fn_1_8D020);
        list = work->specModels;
        for (n = work->specCount; n > 0; n--, list++)
        {
            model = *list;
            for (i = 0; i < 4; i++)
            {
                mtxa_push();
                if (i != 0)
                    mtxa_rotate_y(i << 14);
                if (g_is_sphere_visible_scaled(&model->boundSphereCenter, model->boundSphereRadius, dome->scale.x))
                {
                    load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
                    avdisp_draw_model_unculled_sort_never(model);
                }
                mtxa_pop();
            }
        }
        g_yet_another_unk_draw_func(NULL);
        GXSetTexCopySrc_cached(0, 0, width, height);
        GXSetTexCopyDst_cached(texWidth, texHeight, 0x28, 1);
        GXCopyTex(layer->texBuf, 1);
        GXInitTexObj(&layer->tex, layer->texBuf, texWidth, texHeight, 1, 0, 0, 0);
        GXInitTexObjLOD(&layer->tex, 1, 1, 0.0f, 10.0f, 0.0f, 0, 0, 0);
        layer->ready = 1;
        GXSetViewport(sp28[0], sp28[1], sp28[2], sp28[3], sp28[4], sp28[5]);
        GXSetScissor_cached(sp28[0], sp28[1], sp28[2], sp28[3]);
    }
}

void fn_1_8E2EC(struct BgSpa2Work *work)
{
    int n;
    struct BgSpa2Cloud *cloud = work->clouds;

    for (n = work->cloudCount; n > 0; n--, cloud++)
    {
        cloud->texX = (float)rand() / 32767.0f;
        cloud->texY = (float)rand() / 32767.0f;
        cloud->velX = 0.0033333334f * ((float)rand() / 32767.0f - 0.5f);
        cloud->velY = 0.0033333334f * ((float)rand() / 32767.0f - 0.5f);
    }
}

void fn_1_8E414(struct BgSpa2Work *work)
{
    struct BgSpa2Cloud *cloud = work->clouds;
    int n;

    for (n = work->cloudCount; n > 0; n--, cloud++)
    {
        cloud->texX += cloud->velX;
        cloud->texY += cloud->velY;
    }
}

void fn_1_8E454(struct BgSpa2Work *work, u32 mask)
{
    int n;
    struct BgSpa2Cloud *cloud = work->clouds;
    struct StageBgModel *bgModel;
    struct GMAModel *model;

    for (n = work->cloudCount; n > 0; n--, cloud++)
    {
        bgModel = cloud->model;
        if ((bgModel->flags & mask) != 0)
        {
            model = bgModel->model;
            bgModel->flags &= ~0x10000;
            mtxa_from_mtx(g_ord_tbl_stuff->matrices[0]);
            mtxa_translate(&bgModel->pos);
            mtxa_rotate_z(bgModel->rotZ);
            mtxa_rotate_y(bgModel->rotY);
            mtxa_rotate_x(bgModel->rotX);
            mtxa_scale(&bgModel->scale);
            if (g_is_sphere_visible_scaled(&model->boundSphereCenter, model->boundSphereRadius, bgModel->scale.x))
            {
                struct OrdTblNode *list = ord_tbl_get_entry_for_pos(&model->boundSphereCenter);
                struct BgSpa2CloudNode *node = ord_tbl_alloc_node(sizeof(*node));

                if (node != NULL)
                {
                    node->node.drawFunc = (void *)fn_1_8E560;
                    node->cloud = cloud;
                    node->lightGroup = get_active_light_group();
                    mtxa_to_mtx(node->mtx);
                    fn_1_CD418(list, &node->node);
                }
            }
        }
    }
}

void fn_1_8E560(struct BgSpa2CloudNode *node)
{
    struct BgSpa2Cloud *cloud = node->cloud;
    struct GMAModel *model = cloud->model->model;
    int i;
    float t;

    g_something_with_texture_scroll_2(1);
    avdisp_set_z_mode(1, 3, 0);
    load_light_group_cached(node->lightGroup);
    mtxa_from_translate_xyz(cloud->texX, cloud->texY, 0.0f);
    fn_1_10A4CC(0, mtxa->mtxA);
    mtxa_from_mtx(node->mtx);
    GXSetZCompLoc_cached(1);
    if ((g_some_render_flag.flags & 0x84) == 0)
    {
        for (i = 2; i > 0; i--)
        {
            t = 1.0f - (float)(i - 1);
            set_post_mult_color(0.2f + 0.8f * t, 0.2f + 0.8f * t, 0.25f + 0.75f * t, 1.0f);
            load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
            avdisp_draw_model_unculled_sort_never(model);
            mtxa_translate_xyz(0.0f, 15.0f, 0.0f);
        }
        set_post_mult_color(1.0f, 1.0f, 1.0f, 1.0f);
    }
    else
    {
        load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
        avdisp_draw_model_unculled_sort_never(model);
    }
    GXSetZCompLoc_cached(0);
    avdisp_set_z_mode(1, 3, 1);
    g_something_with_texture_scroll_2(0);
}

int fn_1_8E714(int index, struct GMAModelEntry *entry)
{
    struct BgSpa2Work *work = world_theme.work;
    int enabled;

    switch (index)
    {
    case 0:
        work->shadowModel = entry->model;
        break;
    case 1:
        work->shadowBoxVtx = entry->model;
        break;
    case 2:
    case 3:
    case 4:
        if (work->specCount < 4)
        {
            switch (index)
            {
            case 2:
                enabled = !(work->flags & 2);
                break;
            case 3:
                enabled = !(work->flags & 4);
                break;
            case 4:
                enabled = !(work->flags & 8);
                break;
            }
            if (enabled)
            {
                work->specModels[work->specCount] = entry->model;
                work->specCount++;
            }
        }
        break;
    case 5:
    case 6:
    case 7:
        if (work->boxCount < 4)
        {
            switch (index)
            {
            case 5:
                enabled = !(work->flags & 2);
                break;
            case 6:
                enabled = !(work->flags & 4);
                break;
            case 7:
                enabled = !(work->flags & 8);
                break;
            }
            if (enabled)
            {
                work->boxModels[work->boxCount] = entry->model;
                work->boxCount++;
            }
        }
        break;
    }
    return 1;
}

int fn_1_8E8A8(int index, struct StageBgModel *bgModel)
{
    int receiveShadow = 0;
    struct BgSpa2Work *work = world_theme.work;

    switch (index)
    {
    case 0:
        work->arm = bgModel;
        break;
    case 2:
        work->dome = bgModel;
        receiveShadow = work->flags & 1;
        /* fall through */
    case 1:
    case 3:
    case 4:
        if (work->entryCount < 32)
        {
            work->entries[work->entryCount].model = bgModel;
            work->entries[work->entryCount].receiveShadow = receiveShadow;
            work->entryCount++;
        }
        break;
    case 5:
        if (work->cloudCount < 2)
        {
            work->clouds[work->cloudCount].model = bgModel;
            work->cloudCount++;
        }
        break;
    }
    return 1;
}
