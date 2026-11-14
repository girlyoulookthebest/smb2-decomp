/* Jungle 2 background: drifting smoke-screen layers (JUN_WATER0 / JUN_SPLA_EDGE_*)
 * rendered through an indirect texture stage, each with its own scroll, scale and
 * random indirect matrix. */

#include "types.h"
#include "avdisp.h"
#include "background.h"
#include "camera.h"
#include "gx.h"
#include "mathutil.h"
#include "mode.h"
#include "nl2ngc.h"
#include "rand.h"
#include "stage.h"
#ifndef __MWERKS__
#include "frustum.h" /* prototypes: float args must not be promoted to double on x86 */
#endif

struct BgJun2Entry
{
    /*0x00*/ struct StageBgModel *bg;
    /*0x04*/ GXTexObj *texObjs;
    /*0x08*/ float *texScroll;
    /*0x0C*/ s32 useTev;
    /*0x10*/ Vec pos;
    /*0x1C*/ Vec vel;
    /*0x28*/ Vec scale;
    /*0x34*/ Mtx mtx;
    /*0x64*/ float indMtx[2][3];
};  /* size = 0x7C */

struct BgJun2Work
{
    /*0x000*/ s32 initialized;
    /*0x004*/ GXTexObj *texObjs;
    /*0x008*/ s32 count;
    /*0x00C*/ struct BgJun2Entry entries[16];
    /*0x7CC*/ struct BgJun2Entry *current;
};

struct BgJun2Node
{
    /*0x00*/ struct OrdTblNode node;
    /*0x08*/ struct BgJun2Work *work;
    /*0x0C*/ struct BgJun2Entry *entry;
    /*0x10*/ Mtx mtx;
};  /* size = 0x40 */

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

void g_smth_with_bg_model_names(struct BgModelSearch *search, int (*callback)(int, struct GMAModelEntry *));
void g_something_with_texture_scroll(float *scroll);
void g_smth_with_gpu_and_locked_cache_mtx(void);
void *g_yet_another_unk_draw_func(void *func);
struct OrdTblNode *ord_tbl_get_entry_for_pos(Vec *pos);
void *ord_tbl_alloc_node(u32 size);
void fn_1_CD418(struct OrdTblNode *list, struct OrdTblNode *node);
void GXLoadTexObj_cached(GXTexObj *obj, int map);
void GXLoadTexMtxImm(Mtx m, int id, int type);
void g_GXSetIndTexMtx_wrapper(int id, float mtx[2][3], int scale);
void GXSetTexCoordGen2_cached(int dst, int func, int src, int mtx, int normalize, int postMtx);
static inline void tex_coord_gen(int dst, int func, int src, int mtx)
{
    GXSetTexCoordGen2_cached(dst, func, src, mtx, 0, 0x7D);
}
void GXSetIndTexOrder_cached(int stage, int coord, int map);
void GXSetTevIndirect_cached(int stage, int indStage, int format, int biasSel, int mtxSel, int wrapS, int wrapT,
    int addPrev, int utcLod, int alphaSel);

void fn_1_9F4BC(struct BgJun2Work *work);
void fn_1_9F89C(struct BgJun2Work *work, int mask);
void fn_1_9F950(struct BgJun2Work *work, int mask);
void fn_1_9FAF4(struct BgJun2Node *node);
void fn_1_9FBC0(struct BallEnvArg *arg);
int fn_1_9FD94(int, struct GMAModelEntry *);
int fn_1_9FDC0(int, struct StageBgModel *);

static struct BgModelSearch s_modelSearch[] =
{
    { BG_MDL_CMP_FULL, "JUN_SMOKE_SCREEN_A" },
    { BG_MDL_CMP_END, NULL },
};

static struct BgModelSearch s_bgModelSearch[] =
{
    { BG_MDL_CMP_PREFIX, "JUN_WATER0" },
    { BG_MDL_CMP_PREFIX, "JUN_SPLA_EDGE_" },
    { BG_MDL_CMP_END, NULL },
};

void bg_jun2_init(void)
{
    struct BgJun2Work *work = world_theme.work;

    bg_init_base();
    if (!work->initialized)
    {
        g_smth_with_bg_model_names(s_modelSearch, fn_1_9FD94);
        work->initialized = 1;
    }
    work->count = 0;
    g_init_stagedef_bg_fg_models(
        stagedef->bgModels,
        stagedef->bgModelCount,
        s_bgModelSearch,
        fn_1_9FDC0);
    g_init_stagedef_bg_fg_models(
        stagedef->fgModels,
        stagedef->fgModelCount,
        s_bgModelSearch,
        fn_1_9FDC0);
    fn_1_9F4BC(work);
}

void bg_jun2_tick(void)
{
    struct BgJun2Work *work = world_theme.work;
    int mask = g_smth_with_challenge_mode_var_and_3p();

    bg_tick_base();
    if (!is_game_paused_and_in_view_stage())
        fn_1_9F89C(work, mask);
}

void bg_jun2_dest(void) {}

void bg_jun2_disp(void)
{
    struct BgJun2Work *work = world_theme.work;

    fn_1_9F950(work, g_smth_with_challenge_mode_var_and_3p());
    bg_disp_base();
}

void bg_jun2_item_coin_coli(void) {}

void fn_1_9F4BC(struct BgJun2Work *work)
{
    int i;
    struct BgJun2Entry *entry;
    float *ts;
    struct StageEffectHeader *effect;

    entry = work->entries;
    for (i = work->count; i > 0; i--, entry++)
    {
        entry->texObjs = entry->bg->model->texObjs;
        entry->pos.x = RAND_FLOAT();
        entry->pos.y = RAND_FLOAT();
        entry->pos.z = 0.0f;
        effect = entry->bg->effect;
        ts = NULL;
        if (effect != NULL && (ts = effect->textureScroll) != NULL)
        {
            entry->vel.x = (1.0f / 60.0f) * (ts[0] * (0.1f + 0.3f * RAND_FLOAT()))
                         + (1.0f / 120.0f) * (ts[1] * (RAND_FLOAT() - 0.5f));
            entry->vel.y = (1.0f / 60.0f) * (ts[1] * (0.1f + 0.3f * RAND_FLOAT()))
                         + (1.0f / 120.0f) * (ts[0] * (RAND_FLOAT() - 0.5f));
            entry->vel.z = 0.0f;
        }
        else
        {
            entry->vel.x = (1.0f / 600.0f) * (RAND_FLOAT() - 0.5f);
            entry->vel.y = (1.0f / 300.0f) * (0.8f + 0.4f * RAND_FLOAT());
            entry->vel.z = 0.0f;
        }
        entry->scale.x = 2.0f * (1.0f + RAND_FLOAT());
        entry->scale.y = 2.0f * (1.0f + RAND_FLOAT());
        entry->scale.z = 1.0f;
        entry->indMtx[0][0] = 0.5f * (0.8f + 0.4f * RAND_FLOAT());
        entry->indMtx[0][1] = 0.0f;
        entry->indMtx[0][2] = 0.0f;
        entry->indMtx[1][0] = 0.0f;
        entry->indMtx[1][1] = 0.0f;
        entry->indMtx[1][1] = 0.5f * (0.8f + 0.4f * RAND_FLOAT());
        entry->texScroll = ts;
    }
}

void fn_1_9F89C(struct BgJun2Work *work, int mask)
{
    int i;
    struct BgJun2Entry *entry;

    entry = work->entries;
    for (i = work->count; i > 0; i--, entry++)
    {
        if (entry->bg->flags & mask)
        {
            entry->pos.x += entry->vel.x;
            entry->pos.y += entry->vel.y;
            entry->pos.z += entry->vel.z;
            mtxa_from_identity();
            mtxa_scale(&entry->scale);
            mtxa_translate(&entry->pos);
            mtxa_to_mtx(entry->mtx);
        }
    }
}

void fn_1_9F950(struct BgJun2Work *work, int mask)
{
    int i;
    struct BgJun2Entry *entry;
    void *prevFunc;
    struct GMAModel *model;
    struct StageBgModel *bg;

    if (g_some_render_flag.flags & 0x84)
        return;
    if (modeCtrl.unk30 > 1)
        prevFunc = g_yet_another_unk_draw_func(NULL);
    else
        prevFunc = g_yet_another_unk_draw_func(fn_1_9FBC0);
    g_something_with_texture_scroll_2(1);
    entry = work->entries;
    for (i = work->count; i > 0; i--, entry++)
    {
        bg = entry->bg;
        if (bg->flags & mask)
        {
            bg->flags &= ~0x10000;
            mtxa_from_mtx(g_ord_tbl_stuff->matrices[0]);
            mtxa_translate(&bg->pos);
            mtxa_rotate_z(bg->rotZ);
            mtxa_rotate_y(bg->rotY);
            mtxa_rotate_x(bg->rotX);
            mtxa_scale(&bg->scale);
            model = bg->model;
            if (g_is_sphere_visible(&model->boundSphereCenter, model->boundSphereRadius * bg->scale.x))
            {
                if (model->translucentShapeCount == 0)
                {
                    work->current = entry;
                    if (entry->texScroll != NULL)
                        g_something_with_texture_scroll(entry->texScroll);
                    g_smth_with_gpu_and_locked_cache_mtx();
                    avdisp_draw_model_culled_sort_never(model);
                }
                else
                {
                    struct BgJun2Node *node;
                    struct OrdTblNode *list = ord_tbl_get_entry_for_pos(&model->boundSphereCenter);

                    node = ord_tbl_alloc_node(sizeof(*node));
                    if (node != NULL)
                    {
                        node->node.drawFunc = (void (*)(struct OrdTblNode *))fn_1_9FAF4;
                        node->work = work;
                        node->entry = entry;
                        mtxa_to_mtx(node->mtx);
                        fn_1_CD418(list, &node->node);
                    }
                }
            }
        }
    }
    g_yet_another_unk_draw_func(prevFunc);
    g_something_with_texture_scroll_2(0);
}

void fn_1_9FAF4(struct BgJun2Node *node)
{
    struct BgJun2Entry *entry;
    struct GMAModel *model;
    void *prevFunc;
    struct BgJun2Work *work;

    work = node->work;
    entry = node->entry;
    model = entry->bg->model;
    if (entry->useTev == 0)
        prevFunc = g_yet_another_unk_draw_func(fn_1_9FBC0);
    avdisp_set_z_mode(1, 3, 0);
    g_something_with_texture_scroll_2(1);
    if (entry->texScroll != NULL)
        g_something_with_texture_scroll(entry->texScroll);
    mtxa_from_mtx(node->mtx);
    work->current = entry;
    g_smth_with_gpu_and_locked_cache_mtx();
    avdisp_draw_model_culled_sort_never(model);
    g_something_with_texture_scroll_2(0);
    avdisp_set_z_mode(1, 3, 1);
    if (entry->useTev == 0)
        g_yet_another_unk_draw_func(prevFunc);
}

void fn_1_9FBC0(struct BallEnvArg *arg)
{
    struct BgJun2Entry *entry = ((struct BgJun2Work *)world_theme.work)->current;
    struct TevStageInfo tev = arg->tev;
    int i;
    int src;

    GXLoadTexObj_cached(entry->texObjs, tev.texMapId);
    GXLoadTexMtxImm(entry->mtx, tev.texMtxId, 1);
    g_GXSetIndTexMtx_wrapper(tev.unk1C, entry->indMtx, 0);
    for (i = 0, src = 4; i < tev.tevStage; i++, src++)
    {
        tex_coord_gen(tev.texCoordId, 1, src, tev.texMtxId);
        GXSetIndTexOrder_cached(tev.tevIndStage, tev.texCoordId, tev.texMapId);
        GXSetTevIndirect_cached(i, tev.tevIndStage, 0, 0, tev.unk1C, 0, 0, 0, 0, 0);
        tev.texCoordId++;
        tev.tevIndStage++;
    }
    tev.texMapId++;
    tev.texMtxId += 3;
    arg->tev = tev;
}

int fn_1_9FD94(int index, struct GMAModelEntry *entry)
{
    struct BgJun2Work *work = world_theme.work;
    GXTexObj *texObjs = entry->model->texObjs;

    switch (index)
    {
    case 0:
        work->texObjs = texObjs;
        break;
    }
    return 1;
}

int fn_1_9FDC0(int index, struct StageBgModel *model)
{
    struct BgJun2Work *work = world_theme.work;

    switch (index)
    {
    case 0:
        if (work->count < 16)
        {
            work->entries[work->count].bg = model;
            work->entries[work->count].useTev = 0;
            work->count++;
        }
        break;
    case 1:
        if (work->count < 16)
        {
            work->entries[work->count].bg = model;
            if (modeCtrl.unk30 > 1)
                work->entries[work->count].useTev = 0;
            else
                work->entries[work->count].useTev = 1;
            work->count++;
        }
        break;
    }
    return 1;
}
