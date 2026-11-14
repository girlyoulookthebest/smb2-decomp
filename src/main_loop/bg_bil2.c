/* Billiards background: scrolling "BILL_HIKARI" light models with an indirect-texture warp. */

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

struct BgBil2Entry
{
    /*0x00*/ struct StageBgModel *model;
    /*0x04*/ float texX;
    /*0x08*/ float texY;
    /*0x0C*/ float velX;
    /*0x10*/ float velY;
    /*0x14*/ float indMtx[2][3];
};  /* size = 0x2C */

struct BgBil2Work
{
    /*0x00*/ GXTexObj *texObj;
    /*0x04*/ s32 entryCount;
    /*0x08*/ struct BgBil2Entry entries[4];
    /*0xB8*/ struct BgBil2Entry *curEntry;
};

void GXLoadTexObj_cached(GXTexObj *obj, int map);
void GXLoadTexMtxImm(Mtx m, int id, int type);
void GXSetTexCoordGen2_cached(int dst, int func, int src, int mtx, int normalize, int postMtx);
static inline void tex_coord_gen(int dst, int func, int src, int mtx)
{
    GXSetTexCoordGen2_cached(dst, func, src, mtx, 0, 0x7D);
}
void GXSetIndTexOrder_cached(int stage, int coord, int map);
void g_GXSetIndTexMtx_wrapper(int id, float mtx[2][3], int scale);
void GXSetTevIndirect_cached(int stage, int indStage, int format, int biasSel, int mtxSel, int wrapS, int wrapT,
    int addPrev, int utcLod, int alphaSel);
void *g_yet_another_unk_draw_func(void *func);
void g_something_with_texture_scroll(float *scroll);
void g_load_ape_bg(void);
void fn_1_A00D0(void);
void fn_1_A01C8(void);
void fn_1_A0298(void);
void mtxa_from_translate_xyz(float x, float y, float z);

void fn_1_8EAF0(struct BgBil2Work *);
void fn_1_8EE04(struct BgBil2Work *);
void fn_1_8EE44(struct BgBil2Work *, u32);
void fn_1_8EF64(struct BallEnvArg *);
int fn_1_8F148(int, struct StageBgModel *);

static struct BgModelSearch lbl_1_data_325CC[] =
{
    { BG_MDL_CMP_PREFIX, "BILL_HIKARI" },
    { BG_MDL_CMP_END },
};

void bg_bil2_init(void)
{
    struct BgBil2Work *work = world_theme.work;

    bg_init_base();
    work->entryCount = 0;
    g_init_stagedef_bg_fg_models(stagedef->bgModels, stagedef->bgModelCount, lbl_1_data_325CC, fn_1_8F148);
    g_init_stagedef_bg_fg_models(stagedef->fgModels, stagedef->fgModelCount, lbl_1_data_325CC, fn_1_8F148);
    fn_1_8EAF0(work);
    g_load_ape_bg();
}

void bg_bil2_tick(void)
{
    struct BgBil2Work *work = world_theme.work;

    g_smth_with_challenge_mode_var_and_3p();
    bg_tick_base();
    if (!is_game_paused_and_in_view_stage())
    {
        fn_1_8EE04(work);
        fn_1_A00D0();
    }
}

void bg_bil2_dest(void)
{
    fn_1_A01C8();
}

void bg_bil2_disp(void)
{
    struct BgBil2Work *work = world_theme.work;
    u32 mask = g_smth_with_challenge_mode_var_and_3p();

    mtxa_from_mtxb();
    g_set_active_light_group(world_theme.unkA8);
    fn_1_8EE44(work, mask);
    bg_disp_base();
    fn_1_A0298();
}

void bg_bil2_item_coin_coli(int arg0) {}

void fn_1_8EAF0(struct BgBil2Work *work)
{
    int n;
    struct BgBil2Entry *entry = work->entries;
    struct StageBgModel *bgModel;
    float *scroll;

    for (n = work->entryCount; n > 0; n--, entry++)
    {
        bgModel = entry->model;
        entry->texX = (float)rand() / 32767.0f;
        entry->texY = (float)rand() / 32767.0f;
        if (bgModel->effect != NULL && bgModel->effect->textureScroll != NULL)
        {
            scroll = bgModel->effect->textureScroll;
            entry->velX = scroll[0] * (-(0.25f + 0.5f * ((float)rand() / 32767.0f)) / 60.0f);
            entry->velY = scroll[1] * (-(0.25f + 0.5f * ((float)rand() / 32767.0f)) / 60.0f);
        }
        else
        {
            entry->velX = -(0.25f + 0.5f * ((float)rand() / 32767.0f)) / 60.0f;
            entry->velY = -(0.25f + 0.5f * ((float)rand() / 32767.0f)) / 60.0f;
        }
        entry->indMtx[0][0] = 0.08f * (1.0f + (float)rand() / 32767.0f);
        entry->indMtx[0][1] = -0.04f * (1.0f + (float)rand() / 32767.0f);
        entry->indMtx[0][2] = 0.0f;
        entry->indMtx[1][0] = 0.0f;
        entry->indMtx[1][1] = 0.08f * (1.0f + (float)rand() / 32767.0f);
        entry->indMtx[1][2] = 0.04f * (1.0f + (float)rand() / 32767.0f);
    }
}

void fn_1_8EE04(struct BgBil2Work *work)
{
    int n;
    struct BgBil2Entry *entry = work->entries;

    for (n = work->entryCount; n > 0; n--, entry++)
    {
        entry->texX += entry->velX;
        entry->texY += entry->velY;
    }
}

void fn_1_8EE44(struct BgBil2Work *work, u32 mask)
{
    int n;
    struct BgBil2Entry *entry;
    void *prevFunc;
    struct GMAModel *model;
    struct StageBgModel *bgModel;

    prevFunc = g_yet_another_unk_draw_func(fn_1_8EF64);
    g_something_with_texture_scroll_2(1);
    entry = work->entries;
    for (n = work->entryCount; n > 0; n--, entry++)
    {
        bgModel = entry->model;
        if ((bgModel->flags & mask) != 0)
        {
            work->curEntry = entry;
            model = bgModel->model;
            bgModel->flags &= ~0x10000;
            if (bgModel->effect != NULL && bgModel->effect->textureScroll != NULL)
                g_something_with_texture_scroll(bgModel->effect->textureScroll);
            mtxa_from_mtx(g_ord_tbl_stuff->matrices[0]);
            mtxa_translate(&bgModel->pos);
            mtxa_rotate_z(bgModel->rotZ);
            mtxa_rotate_y(bgModel->rotY);
            mtxa_rotate_x(bgModel->rotX);
            mtxa_scale(&bgModel->scale);
            if (g_is_sphere_visible_scaled(&model->boundSphereCenter, model->boundSphereRadius, bgModel->scale.x))
                avdisp_draw_model_unculled_sort_always(model);
        }
    }
    g_something_with_texture_scroll_2(0);
    g_yet_another_unk_draw_func(prevFunc);
}

void fn_1_8EF64(struct BallEnvArg *arg)
{
    struct BgBil2Work *work = world_theme.work;
    struct BgBil2Entry *entry = work->curEntry;
    struct TevStageInfo sp10 = arg->tev;

    GXLoadTexObj_cached(work->texObj, sp10.texMapId);
    mtxa_push();
    mtxa_from_translate_xyz(entry->texX, entry->texY, 1.0f);
    GXLoadTexMtxImm(mtxa->mtxA, sp10.texMtxId, 1);
    mtxa_pop();
    g_GXSetIndTexMtx_wrapper(arg->tev.unk1C, entry->indMtx, 0);
    tex_coord_gen(sp10.texCoordId, 1, 4, sp10.texMtxId);
    GXSetIndTexOrder_cached(sp10.tevIndStage, sp10.texCoordId, sp10.texMapId);
    GXSetTevIndirect_cached(sp10.tevStage - 1, sp10.tevIndStage, 0, 0, sp10.unk1C, 0, 0, 0, 0, 0);
    sp10.tevIndStage++;
    sp10.texCoordId++;
    sp10.texMtxId += 3;
    sp10.texMapId++;
    sp10.unk1C++;
    arg->tev = sp10;
}

int fn_1_8F148(int index, struct StageBgModel *bgModel)
{
    struct BgBil2Work *work = world_theme.work;
    struct GMAModel *model;

    switch (index)
    {
    case 0:
        if (work->entryCount < 4)
        {
            work->entries[work->entryCount].model = bgModel;
            work->entryCount++;
            model = bgModel->model;
            if (model != NULL)
                work->texObj = model->texObjs;
        }
        break;
    }
    return 1;
}
