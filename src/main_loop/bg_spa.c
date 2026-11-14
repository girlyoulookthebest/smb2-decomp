/* Space background: meteors, Saturn's indirect-warp env callback. */

#include "types.h"
#include "avdisp.h"
#include "background.h"
#include "camera.h"
#include "effect.h"
#include "gx.h"
#include "libc.h"
#include "mathutil.h"
#include "mode.h"
#include "nl2ngc.h"
#include "rand.h"
#include "stage.h"

#define MAX(a, b) (((a) > (b)) ? (a) : (b))

struct BgSpaWork
{
    /*0x00*/ s32 unk0;
    /*0x04*/ Vec corePos;
    /*0x10*/ float coreScale;
    /*0x14*/ s32 meteorModelCount;
    /*0x18*/ struct GMAModel *meteorModels[8];
    /*0x38*/ s32 unk38;
    /*0x3C*/ float unk3C;
    /*0x40*/ struct StageBgModel *saturn;
    /*0x44*/ GXTexObj *saturnIndMap;
    /*0x48*/ Vec unk48;
    /*0x54*/ float unk54;
    /*0x58*/ float unk58;
    /*0x5C*/ float unk5C;
    /*0x60*/ float unk60;
    /*0x64*/ Mtx unk64;
    /*0x94*/ float unk94[2][3];
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


void fn_1_D70EC(Vec *);
void fn_1_D7184(float);
extern struct
{
    /*0x00*/ u32 flags;
} g_some_render_flag;

static struct BgModelSearch lbl_1_data_316B8[] =
{
    { BG_MDL_CMP_PREFIX, "SPA_INSEKI_" },
    { BG_MDL_CMP_FULL,   "SPA_DOSEI_IND_MAP" },
    { BG_MDL_CMP_END },
};

static struct BgModelSearch lbl_1_data_316EC[] =
{
    { BG_MDL_CMP_FULL, "SPA_YOUSAI_CORE" },
    { BG_MDL_CMP_FULL, "SPA_DOSEI" },
    { BG_MDL_CMP_END },
};

void fn_1_79B24(struct BallEnvArg *);
int fn_1_79E70(int, struct GMAModelEntry *);
int fn_1_79ED8(int, struct StageBgModel *);

struct TevStageInfo lbl_1_bss_87FB0;

void bg_spa_init(void)
{
    Vec spB4;
    struct Effect effect;
    struct BgSpaWork *work = world_theme.work;
    int i;
    float temp_f31;

    bg_init_base();
    fn_1_D68D4();
    switch (current_stage_id)
    {
    case 101:
        spB4.x = -913.7f;
        spB4.y = 464.7f;
        spB4.z = 1066.2f;
        break;
    case 105:
        spB4.x = -1005.8f;
        spB4.y = 469.1f;
        spB4.z = 974.0f;
        break;
    case 109:
        spB4.x = 4200.0f;
        spB4.y = 1407.3f;
        spB4.z = 0.0f;
        break;
    case 112:
        spB4.x = 2089.0f;
        spB4.y = 718.6f;
        spB4.z = 10.6f;
        break;
    default:
        spB4.x = 1350.0f;
        spB4.y = 470.0f;
        spB4.z = 0.0f;
        break;
    }
    fn_1_D70EC(&spB4);
    fn_1_D7184(0.5f);
    world_theme.unk8 |= 1;
    if (work->unk0 == 0)
    {
        g_smth_with_bg_model_names(lbl_1_data_316B8, fn_1_79E70);
        work->unk0 = 1;
    }
    work->corePos.x = 0.0f;
    work->corePos.y = 0.0f;
    work->corePos.z = 0.0f;
    work->coreScale = 230.0f;
    work->saturn = NULL;

    g_init_stagedef_bg_fg_models(
        stagedef->bgModels,
        stagedef->bgModelCount,
        lbl_1_data_316EC,
        fn_1_79ED8);
    g_init_stagedef_bg_fg_models(
        stagedef->fgModels,
        stagedef->fgModelCount,
        lbl_1_data_316EC,
        fn_1_79ED8);

    switch (modeCtrl.gameType)
    {
    case 1:
        if (main_mode == 1)
            work->unk3C = 1.0f;
        else
            work->unk3C = (5 - modeCtrl.playerCount) / 4.0f;
        break;
    default:
        work->unk3C = 1.0f;
        break;
    }

    work->unk38 = 15;
    memset(&effect, 0, sizeof(effect));
    effect.type = 9;
    i = work->unk3C * ((rand() & 0xF) + 48);
    while (i > 0)
    {
        effect.model = work->meteorModels[(rand() & 0x7FFF) % work->meteorModelCount];
        effect.scale.x = 12.0f * RAND_FLOAT() + 1.0f;
        effect.pos.x = (RAND_FLOAT() - 0.5f) * 2000.0f;
        effect.pos.y = (RAND_FLOAT() - 0.5f) * 500.0f + effect.pos.x * -0.5f;
        effect.pos.z = (RAND_FLOAT() - 0.5f) * 6000.0f;
        temp_f31 = mathutil_vec_len(&effect.pos);
        if (temp_f31 < 1.7391304f * work->coreScale)
        {
            if (abs(effect.pos.y) < 1.1920929e-7f)
                effect.pos.y = 0.01f;
            vec_set_len(1.7391304f * work->coreScale * (1.0f + RAND_FLOAT()), &effect.pos, &effect.pos);
        }
        effect.rotX = rand() & 0x7FFF;
        effect.rotY = rand() & 0x7FFF;
        effect.rotZ = rand() & 0x7FFF;
        temp_f31 = 2.0f / effect.scale.x;
        effect.unkA4 = temp_f31 * ((rand() & 0x3FF) - 0x200);
        effect.unkA6 = temp_f31 * ((rand() & 0x3FF) - 0x200);
        effect.unkA8 = temp_f31 * ((rand() & 0x3FF) - 0x200);
        effect.colorFactor = 1.0f;
        spawn_effect(&effect);
        i -= 1;
    }
    memset(&effect, 0, sizeof(effect));
    effect.type = 8;
    effect.colorFactor = 1.0f;
    i = work->unk3C * ((rand() & 0xF) + 64);
    while (i > 0)
    {
        effect.model = work->meteorModels[(rand() & 0x7FFF) % work->meteorModelCount];
        effect.scale.x = RAND_FLOAT() * 8.0f + 1.0f;
        effect.pos.x = (RAND_FLOAT() - 0.5f);
        effect.pos.y = (RAND_FLOAT() - 0.5f) * 0.5f + effect.pos.x * -0.75f;
        effect.pos.z = (RAND_FLOAT() - 0.5f);
        if (effect.pos.y == 0.0f)
            effect.pos.y = 0.1f;
        vec_set_len(1.3043479f * work->coreScale * (1.0f + RAND_FLOAT()), &effect.pos, &effect.pos);
        effect.unk98.x = 0.0f;
        effect.unk98.y = 0.0f;
        effect.unk98.z = 1.2000000476837158 + 0.20000000298023224 * (RAND_FLOAT() - 0.5);
        effect.vel.x = effect.unk98.x + 0.2f * (RAND_FLOAT() - 0.5f);
        effect.vel.y = effect.unk98.y + 0.2f * (RAND_FLOAT() - 0.5f);
        effect.vel.z = effect.unk98.z + 0.5f * RAND_FLOAT();
        effect.rotX = rand() & 0x7FFF;
        effect.rotY = rand() & 0x7FFF;
        effect.rotZ = rand() & 0x7FFF;
        temp_f31 = 1.0f / effect.scale.x;
        effect.unkA4 = temp_f31 * ((rand() & 0x3FF) - 0x200);
        effect.unkA6 = temp_f31 * ((rand() & 0x3FF) - 0x200);
        effect.unkA8 = temp_f31 * ((rand() & 0x3FF) - 0x200);
        effect.unk52 = (rand() & 0xFF) - 0x80;
        effect.unk54 = (rand() & 0xFF) - 0x80;
        effect.unk56 = (rand() & 0xFF) - 0x80;
        effect.colorFactor = 1.0f;
        spawn_effect(&effect);
        i -= 1;
    }
    work->unk54 = 0.0033333334f * (0.9f + 0.2f * RAND_FLOAT());
    work->unk58 = 0.0018518518f * (0.9f + 0.2f * RAND_FLOAT());
    work->unk5C = 0.0f;
    work->unk60 = ((0.2f * RAND_FLOAT()) + 0.9f) * 3.0f;
    work->unk94[0][0] = 0.05f * RAND_FLOAT() + 0.2f;
    work->unk94[0][1] = 0.0f;
    work->unk94[0][2] = 0.0f;
    work->unk94[1][0] = 0.0f;
    work->unk94[1][1] = 0.05f * RAND_FLOAT() + 0.2f;
    work->unk94[1][2] = 0.0f;
}

void bg_spa_tick(void)
{
    struct BgSpaWork *work = world_theme.work;

    fn_1_D6920();
    bg_tick_base();
    if (!is_game_paused_and_in_view_stage())
    {
        work->unk38--;
        if (work->unk38 < 0)
        {
            struct Effect effect;
            float var_f31;
            float temp_f31;

            work->unk38 = (15.0f * (1.0f + RAND_FLOAT())) / work->unk3C;
            memset(&effect, 0, sizeof(effect));
            effect.type = 8;
            effect.model = work->meteorModels[(rand() & 0x7FFF) % work->meteorModelCount];
            effect.scale.x = RAND_FLOAT() * 8.0f + 1.0f;
            effect.pos.x = effect.scale.x * (175.0f * (RAND_FLOAT() - 0.5f));
            effect.pos.y = effect.scale.x * (150.0f * (RAND_FLOAT() - 0.5f)) + effect.pos.x * -0.75f;
            effect.pos.z = 1.3043479f * -work->coreScale * effect.scale.x;
            if (mode_info.timerMax != 0)
                var_f31 = 2.0f - ((float)mode_info.timerCurr / (float)mode_info.timerMax);
            else
                var_f31 = 1.0f;
            effect.unk98.x = 0.0f;
            effect.unk98.y = 0.0f;
            effect.unk98.z = var_f31 * (1.2000000476837158 + 0.20000000298023224 * (RAND_FLOAT() - 0.5));
            effect.vel.x = effect.unk98.x + 0.2f * (RAND_FLOAT() - 0.5f);
            effect.vel.y = effect.unk98.y + 0.2f * (RAND_FLOAT() - 0.5f);
            effect.vel.z = effect.unk98.z + 0.5f * RAND_FLOAT();
            effect.rotX = rand() & 0x7FFF;
            effect.rotY = rand() & 0x7FFF;
            effect.rotZ = rand() & 0x7FFF;
            temp_f31 = 1.0f / effect.scale.x;
            effect.unkA4 = (s16) (temp_f31 * (f32) ((rand() & 0x3FF) - 0x200));
            effect.unkA6 = (s16) (temp_f31 * (f32) ((rand() & 0x3FF) - 0x200));
            effect.unkA8 = (s16) (temp_f31 * (f32) ((rand() & 0x3FF) - 0x200));
            effect.unk52 = (rand() & 0xFF) - 0x80;
            effect.unk54 = (rand() & 0xFF) - 0x80;
            effect.unk56 = (rand() & 0xFF) - 0x80;
            effect.colorFactor = 0.0f;
            spawn_effect(&effect);
        }
        work->unk48.x += work->unk54;
        work->unk48.y += work->unk58;
        work->unk48.z += work->unk5C;
        mtxa_from_translate(&work->unk48);
        mtxa_scale_s(work->unk60);
        mtxa_to_mtx(work->unk64);
    }
}

void bg_spa_dest(void) {}

void bg_spa_disp(void)
{
    struct StageBgModel *saturnObj = ((struct BgSpaWork *)world_theme.work)->saturn;
    float scale;
    u8 unused[8];

    if (saturnObj != NULL)
    {
        saturnObj->flags &= 0xFFFEFFFF;
        mtxa_from_mtx(g_ord_tbl_stuff->matrices[0]);
        mtxa_translate(&saturnObj->pos);
        mtxa_rotate_z(saturnObj->rotZ);
        mtxa_rotate_y(saturnObj->rotY);
        mtxa_rotate_x(saturnObj->rotX);
        mtxa_scale(&saturnObj->scale);
        GXLoadPosMtxImm(mtxa->mtxA, 0U);
        GXLoadNrmMtxImm(mtxa->mtxA, 0U);
        scale = MAX(MAX(saturnObj->scale.x, saturnObj->scale.y), saturnObj->scale.z);
        avdisp_set_scale_factor(scale);
        if (!(g_some_render_flag.flags & 4))
        {
            g_yet_another_unk_draw_func(fn_1_79B24);
            avdisp_draw_model_culled_sort_auto(saturnObj->model);
            g_yet_another_unk_draw_func(NULL);
        }
        else
            avdisp_draw_model_culled_sort_auto(saturnObj->model);
    }
    bg_disp_base();
}

void bg_spa_item_coin_coli(void) {}

void fn_1_79B24(struct BallEnvArg *arg0)
{
    struct BgSpaWork *work = world_theme.work;
    struct TevStageInfo sp14 = arg0->tev;

    if (arg0->unk0 == 0
     && sp14.tevStage == lbl_1_bss_87FB0.tevStage
     && sp14.texCoordId == lbl_1_bss_87FB0.texCoordId
     && sp14.texMtxId == lbl_1_bss_87FB0.texMtxId
     && sp14.texMapId == lbl_1_bss_87FB0.texMapId
     && sp14.tevIndStage == lbl_1_bss_87FB0.tevIndStage
     && sp14.unk1C == lbl_1_bss_87FB0.unk1C)
    {
        sp14.texCoordId += 1;
        sp14.texMtxId += 3;
        sp14.texMapId += 1;
        sp14.tevIndStage += 1;
        sp14.unk1C += 1;
        arg0->tev = sp14;
        return;
    }
    lbl_1_bss_87FB0 = sp14;
    GXLoadTexObj_cached(work->saturnIndMap, sp14.texMapId);
    tex_coord_gen(sp14.texCoordId, 1, 4, sp14.texMtxId);
    GXLoadTexMtxImm(work->unk64, sp14.texMtxId, 1);
    GXSetIndTexOrder_cached(sp14.tevIndStage, sp14.texCoordId, sp14.texMapId);
    GXSetTevIndirect_cached(sp14.tevStage - 1, sp14.tevIndStage, 0, 7, sp14.unk1C, 0, 0, 0, 0, 0);
    g_GXSetIndTexMtx_wrapper(sp14.unk1C, work->unk94, -3);
    sp14.texCoordId += 1;
    sp14.texMtxId += 3;
    sp14.texMapId += 1;
    sp14.tevIndStage += 1;
    sp14.unk1C += 1;
    arg0->tev = sp14;
}

int fn_1_79E70(int index, struct GMAModelEntry *entry)
{
    struct BgSpaWork *work = world_theme.work;

    switch (index)
    {
    case 0:  // SPA_INSEKI_ "meteorite"
        if (work->meteorModelCount < 8)
        {
            work->meteorModels[work->meteorModelCount] = entry->model;
            work->meteorModelCount++;
        }
        break;
    case 1:  // SPA_DOSEI_IND_MAP
        work->saturnIndMap = &entry->model->texObjs[0];
        break;
    }
    return 1;
}

int fn_1_79ED8(int index, struct StageBgModel *bgObj)
{
    struct BgSpaWork *work = world_theme.work;

    switch (index)
    {
    case 0:  // SPA_YOUSAI_CORE
        work->corePos = bgObj->pos;
        work->coreScale = MAX(MAX(bgObj->scale.x, bgObj->scale.y), bgObj->scale.z);
        work->coreScale *= 230.0f;
        break;
    case 1:  // SPA_DOSEI  "Saturn"
        if (bgObj->model != NULL)
            work->saturn = bgObj;
        break;
    }
    return 1;
}
