/* Sunset background: scrolling cloud layers. */

#include "types.h"
#include "avdisp.h"
#include "background.h"
#include "camera.h"
#include "gx.h"
#include "libc.h"
#include "mathutil.h"
#include "mode.h"
#include "nl2ngc.h"
#include "rand.h"
#include "stage.h"

struct BgSunLayer
{
    /*0x00*/ struct StageBgModel *bgObj;
    /*0x04*/ Vec texTranslation;
    /*0x10*/ Vec currTexVel;
    /*0x1C*/ Vec desiredTexVel;
    /*0x28*/ Mtx texMtx;
};

struct BgSunWork
{
    u8 filler0[4];
    /*0x04*/ s32 layersCount;
    /*0x08*/ struct BgSunLayer layers[4];
    /*0x168*/ s32 mode;
};

void *OSAllocFromHeap(void *heap, u32 size);
void OSFreeToHeap(void *heap, void *ptr);
void GXInitTexObj(GXTexObj *obj, void *image, u16 width, u16 height, int format, int wrapS, int wrapT, u8 mipmap);
void GXSetTexCopySrc_cached(u16 left, u16 top, u16 width, u16 height);
void GXSetTexCopyDst_cached(u16 width, u16 height, int format, u8 mipmap);
void GXCopyTex(void *dest, u8 clear);
void GXSetNumChans_cached(u8 n);
void GXSetNumTexGens_cached(u8 n);
void GXSetNumTevStages_cached(u8 n);
void GXSetNumIndStages_cached(u8 n);
void GXSetCullMode_cached(int mode);
void GXSetTevKColorSel_cached(int stage, int sel);
void GXSetIndTexOrder_cached(int stage, int coord, int map);
void g_GXSetIndTexMtx_wrapper(int id, float mtx[2][3], int scale);
void g_something_with_GX_vtx_desc(u32 attrs);
void *g_yet_another_unk_draw_func(void *func);
void g_smth_with_bg_model_names(struct BgModelSearch *search, int (*callback)(int, struct GMAModelEntry *));
void fn_1_E2F1C(void);
struct OrdTblNode *ord_tbl_get_entry_for_pos(Vec *pos);
void *ord_tbl_alloc_node(u32 size);
void fn_1_CD418(struct OrdTblNode *list, struct OrdTblNode *node);
void vec_set_len(float len, Vec *src, Vec *dst);
void vec_to_euler_xy(Vec *v, s16 *x, s16 *y);
extern void *stage_heap;
extern void *lbl_801988E0;
void GXSetTexCoordGen2_cached(int dst, int func, int src, int mtx, int normalize, int postMtx);
static inline void tex_coord_gen(int dst, int func, int src, int mtx)
{
    GXSetTexCoordGen2_cached(dst, func, src, mtx, 0, 0x7D);
}

void debug_textdraw_set_pos(int x, int y);
void fn_1_7385C(void);

static struct BgModelSearch lbl_1_data_31750[] =
{
    { BG_MDL_CMP_FULL,   "SUN_GROUND" },
    { BG_MDL_CMP_PREFIX, "SUN_CLOUD_" },
    { BG_MDL_CMP_END,    NULL },
};

int fn_1_7A4A0(int, struct StageBgModel *);

void bg_sun_init(void)
{
    struct BgSunWork *work = world_theme.work;
    struct BgSunLayer *layer;
    int i;
    Vec sp8;

    bg_init_base();

    // find models
    work->layersCount = 0;
    g_init_stagedef_bg_fg_models(
        stagedef->bgModels,
        stagedef->bgModelCount,
        lbl_1_data_31750,
        fn_1_7A4A0);
    g_init_stagedef_bg_fg_models(
        stagedef->fgModels,
        stagedef->fgModelCount,
        lbl_1_data_31750,
        fn_1_7A4A0);
    if (work->layersCount == 0)
        return;

    work->mode = 0;

    layer = work->layers;
    for (i = work->layersCount; i > 0; i--, layer++)
    {
        layer->texTranslation.x = RAND_FLOAT();
        layer->texTranslation.y = RAND_FLOAT();
        layer->texTranslation.z = RAND_FLOAT();
        mtxa_from_rotate_z(rand() & 0x7FFF);
        sp8.x = 0.0f;
        sp8.y = (RAND_FLOAT() * 0.2f + 0.9f) * 0.0015151514671742916f;
        sp8.z = 0.0f;
        mtxa_tf_vec(&sp8, &layer->desiredTexVel);
        layer->currTexVel = layer->desiredTexVel;
    }
}

void bg_sun_tick(void)
{
    struct BgSunWork *work = world_theme.work;
    int i;
    struct BgSunLayer *layer;
    int speedUpTexVel;
    Vec newTexVel;

    bg_tick_base();
    if (is_game_paused_and_in_view_stage())
        return;
    if (work->layersCount == 0)
        return;

    // Speed up texture scroll vel 11s before time over
    if (work->mode == 0 && mode_info.timerCurr < 660.0f)
    {
        speedUpTexVel = 1;
        work->mode = 1;
    }
    else
        speedUpTexVel = 0;

    layer = work->layers;
    for (i = work->layersCount; i > 0; i--, layer++)
    {
        if (speedUpTexVel)
        {
            mtxa_from_rotate_z(rand() & 0x7FFF);
            newTexVel.x = 0.0f;
            newTexVel.y = (RAND_FLOAT() * 0.2f + 0.9f) * 0.0030303029343485832f;
            newTexVel.z = 0.0f;
            mtxa_tf_vec(&newTexVel, &layer->desiredTexVel);
        }
        layer->currTexVel.x += (layer->desiredTexVel.x - layer->currTexVel.x) * 0.05f;
        layer->currTexVel.y += (layer->desiredTexVel.y - layer->currTexVel.y) * 0.05f;
        layer->texTranslation.x += layer->currTexVel.x;
        layer->texTranslation.y += layer->currTexVel.y;
        mtxa_from_translate(&layer->texTranslation);
        mtxa_to_mtx(layer->texMtx);
    }
}

void bg_sun_dest(void) {}

void bg_sun_disp(void)
{
    int i;
    struct BgSunWork *work = world_theme.work;
    struct BgSunLayer *layer;
    u32 r28;
    struct StageBgModel *bgObj;

    r28 = g_smth_with_challenge_mode_var_and_3p();

    layer = work->layers;
    for (i = work->layersCount; i > 0; i--, layer++)
        layer->bgObj->flags &= ~0x10000;
    // draw cloud layers
    if (work->layersCount != 0)
    {
        g_something_with_texture_scroll_2(1);
        layer = work->layers;
        for (i = work->layersCount; i > 0; i--, layer++)
        {
            bgObj = layer->bgObj;
            if (bgObj->flags & r28)
            {
                fn_1_10A4CC(0, layer->texMtx);
                mtxa_from_mtx(g_ord_tbl_stuff->matrices[0]);
                mtxa_translate(&bgObj->pos);
                mtxa_rotate_z(bgObj->rotZ);
                mtxa_rotate_y(bgObj->rotY);
                mtxa_rotate_x(bgObj->rotX);
                mtxa_scale(&bgObj->scale);
                GXLoadPosMtxImm(mtxa->mtxA, 0);
                GXLoadNrmMtxImm(mtxa->mtxA, 0);
                avdisp_draw_model_culled_sort_auto(bgObj->model);
            }
        }
        g_something_with_texture_scroll_2(0);
    }
    bg_disp_base();
}

void bg_sun_item_coin_coli(void) {}

int fn_1_7A4A0(int index, struct StageBgModel *bgObj)
{
    struct BgSunWork *work = world_theme.work;

    switch (index)
    {
    case 0:  // SUN_GROUND
    case 1:  // SUN_CLOUD_
        if (bgObj->model != NULL && work->layersCount < 8)
        {
            work->layers[work->layersCount].bgObj = bgObj;
            work->layersCount++;
        }
        break;
    }
    return 1;
}
