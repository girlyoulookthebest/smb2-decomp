/* Sand background: heat shimmer render effect, pyramid clouds and refractive fountains. */

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

struct BgSndRipple
{
    /*0x00*/ struct GMAModel *unk0;
    /*0x04*/ float unk4;
    /*0x08*/ float unk8;
    /*0x0C*/ s16 unkC;
    /*0x0E*/ s16 unkE;
};

struct BgSndFountain
{
    /*0x00*/ Vec unk0;
    /*0x0C*/ float unkC;
    /*0x10*/ float unk10;
    /*0x14*/ float unk14;
    /*0x18*/ Vec unk18;
    /*0x24*/ float unk24;
    /*0x28*/ float unk28;
    /*0x2C*/ float unk2C;
    /*0x30*/ float unk30;
    /*0x34*/ struct BgSndRipple unk34[6];
};

struct BgSndWork
{
    /*0x000*/ s32 unk0;
    /*0x004*/ Vec unk4;
    /*0x010*/ float unk10;
    /*0x014*/ float unk14;
    /*0x018*/ float unk18;
    /*0x01C*/ Vec unk1C;
    /*0x028*/ Vec unk28;
    /*0x034*/ GXTexObj *shadowMapTex;
    /*0x038*/ GXTexObj *unk38;
    /*0x03C*/ GXTexObj *sandTex;
    /*0x040*/ u32 unk40;
    /*0x044*/ struct StageBgModel *pyramidClouds[8];
    /*0x064*/ Vec unk64;
    /*0x070*/ Vec unk70;
    /*0x07C*/ u32 unk7C;
    /*0x080*/ struct BgSndFountain unk80[8];
    /*0x520*/ struct StageBgModel *fountainWater[6];
    /*0x538*/ struct GMAModel *unk538[2];
    /*0x540*/ struct GMAModel *waterAUraModel;
    /*0x544*/ struct GMAModel *waterBModel;
    /*0x548*/ struct GMAModel *waterBUraModel;
    /*0x54C*/ GXTexObj *fountainIndirectTex;
    /*0x550*/ GXTexObj *fountainGradientTex;
    /*0x554*/ struct GMAModel *unk554;
    /*0x558*/ struct GMAModel *unk558;
    /*0x55C*/ struct GMAModel *unk55C;
    /*0x560*/ struct GMAModel *unk560;
    /*0x564*/ struct BgSndFountain *unk564;
};

struct MyDrawNode
{
    struct OrdTblNode node;
    s32 unk8;
    struct StageBgModel *unkC;
    Mtx unk10;
};

struct MyDrawNode2
{
    struct OrdTblNode node;
    s32 unk8;
    s32 unkC;
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
    void (*disp)(int, struct RenderEffect *);
};

struct BgSndHeatWork
{
    /*0x00*/ GXTexObj unk0;
    /*0x20*/ void *unk20;
    /*0x24*/ GXTexObj unk24;
    /*0x44*/ void *unk44;
    /*0x48*/ GXTexObj *unk48;
};

void *OSAllocFromHeap(void *heap, u32 size);
void OSFreeToHeap(void *heap, void *ptr);
void GXInitTexObj(GXTexObj *obj, void *image, u16 width, u16 height, int format, int wrapS, int wrapT, u8 mipmap);
u32 GXGetTexBufferSize(u16 width, u16 height, u32 format, u8 mipmap, u8 maxLod);
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
void GXSetTevIndirect_cached(int stage, int indStage, int format, int biasSel, int mtxSel, int wrapS, int wrapT,
    int addPrev, int utcLod, int alphaSel);
void g_GXSetIndTexMtx_wrapper(int id, float mtx[2][3], int scale);
void g_something_with_GX_vtx_desc(u32 attrs);
void *g_yet_another_unk_draw_func(void *func);
void g_smth_with_bg_model_names(struct BgModelSearch *search, int (*callback)(int, struct GMAModelEntry *));
void fn_1_D8130(int, int, struct RenderEffect *);
void fn_1_E2F1C(void);
struct OrdTblNode *ord_tbl_get_entry_for_pos(Vec *pos);
void *ord_tbl_alloc_node(u32 size);
void fn_1_CD418(struct OrdTblNode *list, struct OrdTblNode *node);
void vec_set_len(float len, Vec *src, Vec *dst);
void vec_to_euler_xy(Vec *v, s16 *x, s16 *y);
extern void *stage_heap;
extern void *lbl_801988E0;

u32 lbl_1_data_311D8[] =
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

static struct BgModelSearch lbl_1_data_31434[] =
{
    { BG_MDL_CMP_FULL, "SND_KAGE_LOW_MAP_A" },
    { BG_MDL_CMP_FULL, "SND_STAGE_SAND_A" },
    { BG_MDL_CMP_FULL, "SND_FUNSUI_WATER_A_URA" },
    { BG_MDL_CMP_FULL, "SND_FUNSUI_WATER_B" },
    { BG_MDL_CMP_FULL, "SND_FUNSUI_WATER_B_URA" },
    { BG_MDL_CMP_FULL, "SND_FUNSUI_IND_A" },
    { BG_MDL_CMP_FULL, "SND_FUNSUI_GRAD_A" },
    { BG_MDL_CMP_FULL, "SND_FUNSUI_UP_TATE" },
    { BG_MDL_CMP_FULL, "SND_FUNSUI_UP_YOKO" },
    { BG_MDL_CMP_FULL, "SND_FUNSUI_DOWN_TATE" },
    { BG_MDL_CMP_FULL, "SND_FUNSUI_DOWN_YOKO" },
    { BG_MDL_CMP_END,  NULL },
};

static struct BgModelSearch lbl_1_data_314B4[] =
{
    { BG_MDL_CMP_PREFIX, "SND_PIRAM_CLOUD" },
    { BG_MDL_CMP_PREFIX, "SND_FUNSUI_WATER" },
    { BG_MDL_CMP_END,    NULL },
};

void fn_1_74B1C(struct RenderEffect *);
void fn_1_74C5C(struct RenderEffect *);
void fn_1_74CF4(struct RenderEffect *);
void mtxa_from_translate_xyz(float x, float y, float z);
void GXSetTexCoordGen2_cached(int dst, int func, int src, int mtx, int normalize, int postMtx);
void fn_1_74D18(int, struct RenderEffect *);
void fn_1_75484(struct MyDrawNode *);
void fn_1_75558(struct MyDrawNode2 *);
void fn_1_7580C(struct BallEnvArg *);
void fn_1_75EF4(struct BallEnvArg *);
void fn_1_767E0(struct BallEnvArg *);
int fn_1_76BA0(int, struct GMAModelEntry *);
int fn_1_76C48(int, struct StageBgModel *);

static struct RenderEffectFuncs lbl_1_data_314D0 =
{
    fn_1_74B1C,
    fn_1_74C5C,
    fn_1_74CF4,
    fn_1_74D18,
};

void bg_snd_init(void)
{
    struct BgSndWork *work = world_theme.work;
    Vec sp20;
    float temp_f0;
    int var_r29;
    int var_r28;
    struct BgSndFountain *var_r27;
    struct BgSndRipple *var_r26;

    bg_init_base();
    if (work->unk0 == 0)
    {
        g_smth_with_bg_model_names(lbl_1_data_31434, fn_1_76BA0);
        work->unk0 = 1;
    }
    work->unk40 = 0;
    work->unk7C = 0;
    g_init_stagedef_bg_fg_models(
        stagedef->bgModels,
        stagedef->bgModelCount,
        lbl_1_data_314B4,
        fn_1_76C48);
    g_init_stagedef_bg_fg_models(
        stagedef->fgModels,
        stagedef->fgModelCount,
        lbl_1_data_314B4,
        fn_1_76C48);
    if (modeCtrl.unk30 < 2)
    {
        struct RenderEffect rendEfc;

        memset(&rendEfc, 0, sizeof(rendEfc));
        rendEfc.cameraMask = 0xFFFF;
        rendEfc.funcs = &lbl_1_data_314D0;
        fn_1_D8130(0, 0, &rendEfc);
    }
    work->unk4.x = 6.0f + (RAND_FLOAT() - 0.5f);
    work->unk4.y = 4.0f + (RAND_FLOAT() - 0.5f);
    work->unk4.z = 1.0f;
    work->unk10 = 0.0f;
    work->unk14 = 0.0f;
    work->unk18 = 0.0f;
    work->unk1C.x = RAND_FLOAT() - 0.5f;
    work->unk1C.y = 1.0f;
    work->unk1C.z = RAND_FLOAT() - 0.5f;
    vec_set_len(0.02f + (0.01f * (RAND_FLOAT() - 0.5f)), &work->unk1C, &work->unk1C);
    mtxa_from_rotate_z(rand() & 0x7FFF);
    mtxa_tf_vec_xyz(0.0f, 0.45f + (0.1f * RAND_FLOAT()), 0.0f, &work->unk28);
    temp_f0 = work->unk4.x / 6.0f;
    work->unk28.x *= temp_f0 * temp_f0;
    temp_f0 = work->unk4.y / 4.0f;
    work->unk28.y *= temp_f0 * temp_f0;
    work->unk64.x = RAND_FLOAT();
    work->unk64.y = RAND_FLOAT();
    work->unk64.z = RAND_FLOAT();
    sp20.x = 0.0016666667f;
    sp20.y = 0.0f;
    sp20.z = 0.0f;
    mtxa_from_rotate_z(rand() & 0x7FFF);
    mtxa_tf_vec(&sp20, &work->unk70);

    var_r27 = work->unk80;
    for (var_r29 = work->unk7C; var_r29 > 0; var_r29--, var_r27++)
    {
        var_r27->unkC =  0.008333334f * (0.5f - RAND_FLOAT());
        var_r27->unk10 = 0.008333334f * (1.0f + (0.1f * RAND_FLOAT()));
        var_r27->unk14 = 0.0f;
        var_r27->unk24 = 0.010000001f * (0.5f - RAND_FLOAT());
        var_r27->unk28 = 0.010000001f * (1.0f + (0.1f * RAND_FLOAT()));
        var_r27->unk2C = 0.0f;
        var_r27->unk30 = 0.7f + (0.2f * RAND_FLOAT());
        var_r26 = var_r27->unk34;
        for (var_r28 = 6; var_r28 > 0; var_r28--, var_r26++)
        {
            if (var_r28 < 3)
                var_r26->unk0 = work->unk558;
            else
                var_r26->unk0 = work->unk560;
            var_r26->unk4 = RAND_FLOAT();
            var_r26->unk8 = var_r27->unk28 * RAND_FLOAT();
            var_r26->unkC = (rand() & 0x7FFF);
            var_r26->unkE = (rand() & 0x7F) - 0x40;
        }
    }
}

void bg_snd_tick(void)
{
    struct BgSndWork *work = world_theme.work;
    int var_r30;
    int var_r29;
    struct BgSndFountain *var_r28;
    struct BgSndRipple *var_r27;

    bg_tick_base();
    if (is_game_paused_and_in_view_stage())
        return;

    work->unk1C.y += 0.001f * (0.03f - work->unk1C.y);
    work->unk10 += work->unk1C.x;
    work->unk14 += work->unk1C.y;
    work->unk18 += work->unk1C.z;
    work->unk64.x += work->unk70.x;
    work->unk64.y += work->unk70.y;

    var_r28 = work->unk80;
    for (var_r30 = work->unk7C; var_r30 > 0; var_r30--, var_r28++)
    {
        float temp_f28;

        var_r28->unk0.x += var_r28->unkC;
        var_r28->unk0.y += var_r28->unk10;
        var_r28->unk0.z += var_r28->unk14;
        var_r28->unk18.x += var_r28->unk24;
        var_r28->unk18.y += var_r28->unk28;
        var_r28->unk18.z += var_r28->unk2C;
        temp_f28 = __fabs(var_r28->unk28);

        var_r27 = var_r28->unk34;
        for (var_r29 = 6; var_r29 > 0; var_r29--, var_r27++)
        {
            var_r27->unk8 += 0.05f * (temp_f28 - var_r27->unk8);
            var_r27->unk4 += var_r27->unk8;
            var_r27->unkE -= var_r27->unkE >> 5;
            var_r27->unkC += var_r27->unkE;
            if (var_r27->unk4 > 1.0f)
            {
                while (var_r27->unk4 > 1.0f)
                    var_r27->unk4 -= 1.0f;
                var_r27->unk8 = 3.0f * temp_f28 * RAND_FLOAT();
                var_r27->unkC = rand() & 0x7FFF;
                var_r27->unkE = (rand() & 0x3FF) - 0x200;
            }
        }
    }
}

void bg_snd_dest(void) {}

void bg_snd_disp(void)
{
    int i;
    struct BgSndWork *work = world_theme.work;
    struct StageBgModel **var_r27;
    u32 var_r30;
    struct StageBgModel *temp_r28;
    struct OrdTblNode *temp_r25;
    struct MyDrawNode *temp_r24;
    struct MyDrawNode2 *temp_r24_2;

    var_r30 = g_smth_with_challenge_mode_var_and_3p();

    var_r27 = work->pyramidClouds;
    for (i = work->unk40; i > 0; i--, var_r27++)
        (*var_r27)->flags &= 0xFFFEFFFF;

    var_r27 = work->fountainWater;
    for (i = work->unk7C; i > 0; i--, var_r27++)
        (*var_r27)->flags &= 0xFFFEFFFF;

    bg_disp_base();
    if (work->unk40 != 0)
    {
        var_r27 = work->pyramidClouds;
        for (i = work->unk40; i > 0; i--, var_r27++)
        {
            temp_r28 = *var_r27;
            if (temp_r28->flags & var_r30)
            {
                mtxa_from_mtx(g_ord_tbl_stuff->matrices[0]);
                mtxa_translate(&temp_r28->pos);
                mtxa_rotate_z(temp_r28->rotZ);
                mtxa_rotate_y(temp_r28->rotY);
                mtxa_rotate_x(temp_r28->rotX);
                mtxa_scale(&temp_r28->scale);
                temp_r25 = ord_tbl_get_entry_for_pos(&temp_r28->model->boundSphereCenter);
                temp_r24 = ord_tbl_alloc_node(sizeof(*temp_r24));
                if (temp_r24 != NULL)
                {
                    temp_r24->node.drawFunc = (void *)fn_1_75484;
                    temp_r24->unk8 = get_active_light_group();
                    temp_r24->unkC = temp_r28;
                    mtxa_to_mtx(temp_r24->unk10);
                    fn_1_CD418(temp_r25, &temp_r24->node);
                }
            }

        }
    }

    if (work->unk7C != 0)
    {
        mtxa_from_mtx(g_ord_tbl_stuff->matrices[0]);

        var_r27 = work->fountainWater;
        for (i = 0; i < work->unk7C; i++, var_r27++)
        {
            temp_r28 = *var_r27;
            if (temp_r28->flags & var_r30)
            {
                temp_r25 = ord_tbl_get_entry_for_pos(&temp_r28->pos);
                temp_r24_2 = ord_tbl_alloc_node(sizeof(*temp_r24_2));
                if (temp_r24_2 != NULL)
                {
                    temp_r24_2->node.drawFunc = (void *)fn_1_75558;
                    temp_r24_2->unk8 = get_active_light_group();
                    temp_r24_2->unkC = i;
                    fn_1_CD418(temp_r25, &temp_r24_2->node);
                }
            }
        }
    }
}

void bg_snd_item_coin_coli(void) {}

void fn_1_74B1C(struct RenderEffect *rendEfc)
{
    struct BgSndHeatWork *temp_r3;
    u32 texSize;

    rendEfc->enableFlags = 8;
    temp_r3 = OSAllocFromHeap(stage_heap, sizeof(*temp_r3));
    if (temp_r3 == NULL)
    {
        rendEfc->state = 0;
        return;
    }
    rendEfc->work = temp_r3;
    temp_r3->unk20 = lbl_801988E0;
    GXInitTexObj(&temp_r3->unk0, temp_r3->unk20, 640, 448, 4, 0, 0, 0);
    texSize = GXGetTexBufferSize(640, 448, 1, 0, 0);
    temp_r3->unk44 = OSAllocFromHeap(stage_heap, texSize);
    if (temp_r3->unk44 == NULL)
    {
        OSFreeToHeap(stage_heap, temp_r3);
        rendEfc->state = 0;
        return;
    }
    GXInitTexObj(&temp_r3->unk24, temp_r3->unk44, 640, 448, 1, 0, 0, 0);
    temp_r3->unk48 = ((struct BgSndWork *)world_theme.work)->shadowMapTex;
    ((struct BgSndWork *)world_theme.work)->unk38 = &temp_r3->unk0;
}

void fn_1_74C5C(struct RenderEffect *arg0)
{
    if (arg0->work != NULL)
    {
        if (((struct BgSndHeatWork *)arg0->work)->unk44 != NULL)
            OSFreeToHeap(stage_heap, ((struct BgSndHeatWork *)arg0->work)->unk44);
        OSFreeToHeap(stage_heap, arg0->work);
    }
    if (world_theme.bgId == 7)
        ((struct BgSndWork *)world_theme.work)->unk38 = NULL;
}

void fn_1_74CF4(struct RenderEffect *arg0)
{
    if (world_theme.bgId != 7)
    {
        arg0->state = 3;
        return;
    }
}

void fn_1_74D18(int arg0, struct RenderEffect *arg1)
{
    float sp20[2][3];
    Vec sp14;
    s16 sp12;
    s16 sp10;
    float temp1;
    float temp_f9;
    float fbWidth;
    float fbHeight;
    u16 left;
    u16 top;
    struct BgSndHeatWork *temp_r30 = arg1->work;
    struct BgSndWork *work = world_theme.work;
    int tevStage;
    int texCoord;
    int indStage;
    int texMtx;
    int indMtx;
    u16 width;
    u16 height;
    float temp_f31;
    float temp_f30;
    float temp_f29;
    float temp_f28;

    fbWidth  = current_render_mode->fbWidth;
    fbHeight = current_render_mode->efbHeight;
    left   = fbWidth * g_current_camera->sub28.viewportX;
    top    = fbHeight * g_current_camera->sub28.viewportY;
    width  = fbWidth * g_current_camera->sub28.viewportW;
    height = fbHeight * g_current_camera->sub28.viewportH;
    left   = left & ~1;
    top    = top & ~1;
    width  = (width + 1) & ~1;
    height = (height + 1) & ~1;

    GXSetTexCopySrc_cached(left, top, width, height);
    GXSetTexCopyDst_cached(width, height, 4, 0);
    GXCopyTex(temp_r30->unk20, 0);
    GXInitTexObj(&temp_r30->unk0, temp_r30->unk20, width, height, 4, 0, 0, 0);
    GXSetTexCopyDst_cached(width, height, 0x39, 0);
    GXCopyTex(temp_r30->unk44, 0);
    GXInitTexObj(&temp_r30->unk24, temp_r30->unk44, width, height, 1, 0, 0, 0);
    GXSetNumChans_cached(0);
    GXLoadTexObj_cached(&temp_r30->unk0, 0);
    GXLoadTexObj_cached(temp_r30->unk48, 1);
    GXLoadTexObj_cached(&temp_r30->unk24, 2);
    sp14.x = 0.0f;
    sp14.y = 0.0f;
    sp14.z = -1.0f;
    mtxa_from_mtx(g_ord_tbl_stuff->matrices[0]);
    mtxa_rigid_inv_tf_vec(&sp14, &sp14);
    vec_to_euler_xy(&sp14, &sp12, &sp10);
    temp_f9 = 32768.0f / g_current_camera->sub28.view.fov;
    temp1 = -temp_f9 * work->unk4.x / g_current_camera->sub28.view.aspect;
    temp_f9 = temp_f9 * work->unk4.y;
    sp14.x = work->unk10 + (sp10 / 65536.0f) * temp1;
    sp14.y = work->unk14 + (sp12 / 65536.0f) * temp_f9;
    mtxa_from_translate_xyz(sp14.x, sp14.y, 0.0f);
    mtxa_scale(&work->unk4);
    GXLoadTexMtxImm(mtxa->mtxA, 0x1E, 1);
    tevStage = 0;
    indStage = 0;
    texCoord = 0;
    texMtx = 0x1E;
    indMtx = 1;
    GXSetTexCoordGen2_cached(texCoord, 1, 4, 0x3C, 0, 0x7D);
    g_GXSetTevOrder_wrapper(tevStage, texCoord, 0, 4);
    GXSetTevKAlphaSel_cached(tevStage, 3);
    GXSetTevColorIn_cached(tevStage, 0xF, 0xF, 0xF, 8);
    GXSetTevColorOp_cached(tevStage, 0, 0, 0, 0, 0);
    GXSetTevAlphaIn_cached(tevStage, 7, 7, 7, 6);
    GXSetTevAlphaOp_cached(tevStage, 0, 0, 0, 1, 0);
    GXSetTexCoordGen2_cached(texCoord + 1, 1, 4, texMtx, 0, 0x7D);
    sp20[0][0] = work->unk28.x;
    sp20[0][1] = 0.0f;
    sp20[0][2] = 0.0f;
    sp20[1][0] = 0.0f;
    sp20[1][1] = work->unk28.y;
    sp20[1][2] = 0.0f;
    g_GXSetIndTexMtx_wrapper(indMtx, sp20, -4);
    GXSetIndTexOrder_cached(indStage, texCoord + 1, 1);
    GXSetTevIndirect_cached(tevStage, indStage, 0, 7, indMtx, 0, 0, 0, 0, 0);
    tevStage++;
    texCoord += 2;
    indStage++;
    g_GXSetTevIndirect_zero_if_different(tevStage);
    g_GXSetTevOrder_wrapper(tevStage, texCoord - 2, 2, 4);
    GXSetTevSwapMode_cached(tevStage, 0, 1);
    GXSetTevColorIn_cached(tevStage, 0xF, 0xF, 0xF, 0);
    GXSetTevColorOp_cached(tevStage, 0, 0, 0, 1, 0);
    GXSetTevAlphaIn_cached(tevStage, 7, 0, 4, 7);
    GXSetTevAlphaOp_cached(tevStage, 0, 0, 0, 1, 0);
    tevStage++;
    GXSetNumTevStages_cached(tevStage);
    GXSetNumTexGens_cached(texCoord);
    GXSetNumIndStages_cached(indStage);
    GXSetBlendMode_cached(1, 4, 5, 0);
    GXSetZMode_cached(0, 3, 0);
    fn_1_E2F1C();
    GXSetCullMode_cached(2);
    g_something_with_GX_vtx_desc((1 << 9) | (1 << 13));
    mtxa_from_identity();
    GXLoadPosMtxImm(mtxa->mtxA, 0);
    temp_f31 = g_current_camera->sub28.view.tanHalfFov * g_current_camera->sub28.view.aspect;
    temp_f30 = g_current_camera->sub28.view.tanHalfFov;
    temp_f29 = temp_f31 * g_current_camera->sub28.view.unk28;
    temp_f28 = temp_f30 * g_current_camera->sub28.view.unk2C;
    GXBegin(0x80, 0, 4);
        GXPosition3f32(-temp_f31 - temp_f29, temp_f30 - temp_f28, -1.0f);
        GXTexCoord2f32(0.0f, 0.0f);
        GXPosition3f32(temp_f31 - temp_f29, temp_f30 - temp_f28, -1.0f);
        GXTexCoord2f32(1.0f, 0.0f);
        GXPosition3f32(temp_f31 - temp_f29, -temp_f30 - temp_f28, -1.0f);
        GXTexCoord2f32(1.0f, 1.0f);
        GXPosition3f32(-temp_f31 - temp_f29, -temp_f30 - temp_f28, -1.0f);
        GXTexCoord2f32(0.0f, 1.0f);
    GXEnd();
    GXSetZMode_cached(1, 3, 1);
    GXSetTevSwapMode_cached(0, 0, 0);
}

void fn_1_75484(struct MyDrawNode *arg0)
{
    // Unused; only present so these constants land in .rodata at the original offsets
    float dead1 = 0.8f;
    float dead2 = 0.0010416667f;
    float dead3 = 0.16666667f;
    float dead4 = -0.00052083336f;
    float dead5 = 0.0013888889f;
    float dead6 = 0.0625f;
    struct BgSndWork *work = world_theme.work;
    struct StageBgModel *temp_r30 = arg0->unkC;

    load_light_group_cached(arg0->unk8);
    mtxa_from_translate(&work->unk64);
    fn_1_10A4CC(0, mtxa->mtxA);
    g_something_with_texture_scroll_2(1);
    avdisp_set_z_mode(1, 3, 0);
    mtxa_from_mtx(arg0->unk10);
    GXLoadPosMtxImm(mtxa->mtxA, 0);
    GXLoadNrmMtxImm(mtxa->mtxA, 0);
    avdisp_draw_model_culled_sort_never(temp_r30->model);
    avdisp_set_z_mode(1, 3, 1);
    g_something_with_texture_scroll_2(0);
}

void fn_1_75558(struct MyDrawNode2 *arg0)
{
    int i;
    struct BgSndWork *work = world_theme.work;
    struct StageBgModel *temp_r29;
    struct BgSndFountain *temp_r28;
    void * var_r28;
    void * var_r31_2;
    struct BgSndRipple *var_r27;

    load_light_group_cached(arg0->unk8);
    temp_r29 = work->fountainWater[arg0->unkC];
    temp_r28 = &work->unk80[arg0->unkC];
    work->unk564 = temp_r28;
    mtxa_from_mtx(g_ord_tbl_stuff->matrices[0]);
    mtxa_translate(&temp_r29->pos);
    mtxa_rotate_z(temp_r29->rotZ);
    mtxa_rotate_y(temp_r29->rotY);
    mtxa_rotate_x(temp_r29->rotX);
    mtxa_scale(&temp_r29->scale);
    avdisp_set_z_mode(1, 3, 0);

    var_r27 = temp_r28->unk34;
    for (i = 6; i > 0; i--, var_r27++)
    {
        mtxa_push();
        mtxa_rotate_y(var_r27->unkC);
        GXLoadPosMtxImm(mtxa->mtxA, 0);
        GXLoadNrmMtxImm(mtxa->mtxA, 0);
        set_post_mult_color(1.0f, 1.0f, 1.0f, 0.25f * math_sin(32768.0f * var_r27->unk4));
        avdisp_draw_model_culled_sort_never(var_r27->unk0);
        mtxa_pop();
    }
    set_post_mult_color(1.0f, 1.0f, 1.0f, 1.0f);
    mtxa_push();
    mtxa_rotate_y(224.0f * world_theme.animTimer);
    GXLoadPosMtxImm(mtxa->mtxA, 0);
    GXLoadNrmMtxImm(mtxa->mtxA, 0);
    avdisp_draw_model_culled_sort_never(work->unk554);
    avdisp_draw_model_culled_sort_never(work->unk55C);
    mtxa_pop();
    mtxa_push();
    mtxa_from_translate(&temp_r28->unk0);
    fn_1_10A4CC(0, mtxa->mtxA);
    mtxa_pop();
    g_something_with_texture_scroll_2(1);
    if (work->unk38 != 0)
    {
        var_r28 = fn_1_7580C;
        var_r31_2 = fn_1_75EF4;
    }
    else
    {
        var_r28 = fn_1_767E0;
        var_r31_2 = fn_1_767E0;
    }
    GXLoadPosMtxImm(mtxa->mtxA, 0);
    GXLoadNrmMtxImm(mtxa->mtxA, 0);
    g_yet_another_unk_draw_func(var_r31_2);
    avdisp_draw_model_culled_sort_never(work->waterBUraModel);
    g_yet_another_unk_draw_func(var_r28);
    avdisp_draw_model_culled_sort_never(work->waterAUraModel);
    g_yet_another_unk_draw_func(var_r31_2);
    avdisp_draw_model_culled_sort_never(work->waterBModel);
    g_yet_another_unk_draw_func(var_r28);
    avdisp_draw_model_culled_sort_never(temp_r29->model);
    g_yet_another_unk_draw_func(NULL);
    g_something_with_texture_scroll_2(0);
    avdisp_set_z_mode(1, 3, 1);
}

struct TevStageInfo lbl_1_bss_87F50;

void fn_1_7580C(struct BallEnvArg *arg0)
{
    struct BgSndWork *work = world_theme.work;
    struct TevStageInfo sp64;
    Mtx sp1C;
    float sp4C[2][3];
    GXColor sp14;
    struct BgSndFountain *temp_r26;

    sp64 = arg0->tev;
    if (arg0->unk0 == 0
     && sp64.tevStage == lbl_1_bss_87F50.tevStage
     && sp64.texCoordId == lbl_1_bss_87F50.texCoordId
     && sp64.tevIndStage == lbl_1_bss_87F50.tevIndStage
     && sp64.texMtxId == lbl_1_bss_87F50.texMtxId
     && sp64.unk1C == lbl_1_bss_87F50.unk1C
     && sp64.texMapId == lbl_1_bss_87F50.texMapId)
    {
        sp64.tevIndStage += 2;
        sp64.texCoordId += 2;
        sp64.texMtxId += 6;
        sp64.unk1C += 2;
        sp64.texMapId += 2;
        arg0->tev = sp64;
        return;
    }
    lbl_1_bss_87F50 = sp64;
    temp_r26 = work->unk564;
    sp4C[0][0] = temp_r26->unk30;
    sp4C[0][1] = 0.0f;
    sp4C[0][2] = 0.0f;
    sp4C[1][0] = 0.0f;
    sp4C[1][1] = temp_r26->unk30;
    sp4C[1][2] = 0.0f;
    g_GXSetIndTexMtx_wrapper(sp64.unk1C, sp4C, -2);
    sp4C[0][0] = 0.6f * temp_r26->unk30;
    sp4C[0][1] = 0.0f;
    sp4C[0][2] = 0.0f;
    sp4C[1][0] = 0.0f;
    sp4C[1][1] = 0.8f * temp_r26->unk30;
    sp4C[1][2] = 0.0f;
    g_GXSetIndTexMtx_wrapper(sp64.unk1C + 1, sp4C, -2);
    sp4C[0][0] = 0.4f;
    sp4C[0][1] = 0.0f;
    sp4C[0][2] = 0.0f;
    sp4C[1][0] = 0.0f;
    sp4C[1][1] = -0.6f;
    sp4C[1][2] = 0.0f;
    g_GXSetIndTexMtx_wrapper(sp64.unk1C + 2, sp4C, 0);
    GXLoadTexObj_cached(work->fountainIndirectTex, sp64.texMapId);
    GXLoadTexObj_cached(work->unk38, sp64.texMapId + 1);
    GXLoadTexObj_cached(work->fountainGradientTex, sp64.texMapId + 2);
    mtxa_push();
    mtxa_from_translate(&temp_r26->unk18);
    GXLoadTexMtxImm(mtxa->mtxA, sp64.texMtxId, 0);
    mtxa_pop();
    {
        int mtx = sp64.texMtxId;
        GXSetTexCoordGen2_cached(sp64.texCoordId, 1, 4, mtx, 0, 0x7D);
    }
    GXSetIndTexOrder_cached(sp64.tevIndStage, sp64.texCoordId, sp64.texMapId);
    GXSetTevIndirect_cached(sp64.tevStage - 1, sp64.tevIndStage, 0, 7, sp64.unk1C, 0, 0, 0, 0, 0);
    sp64.tevIndStage += 1;
    sp64.texCoordId += 1;
    sp64.texMtxId += 3;
    sp64.unk1C += 1;
    sp64.texMapId += 1;
    GXSetIndTexOrder_cached(sp64.tevIndStage, sp64.texCoordId - 1, sp64.texMapId - 1);
    GXSetTevIndirect_cached(sp64.tevStage, sp64.tevIndStage, 0, 7, sp64.unk1C, 0, 0, 0, 0, 0);
    {
        int mtx = sp64.texMtxId;
        GXSetTexCoordGen2_cached(sp64.texCoordId, 0, 0, mtx, 0, 0x7D);
    }
    sp1C[0][0] = 1.05f * (-0.5f * g_current_camera->sub28.view.invTanHalfFov * (1.0f / g_current_camera->sub28.view.aspect));
    sp1C[0][1] = 0.0f;
    sp1C[0][2] = 0.5f + (-0.5f * g_current_camera->sub28.view.unk28);
    sp1C[0][3] = 0.0f;
    sp1C[1][0] = 0.0f;
    sp1C[1][1] = 1.05f * (0.5f * g_current_camera->sub28.view.invTanHalfFov);
    sp1C[1][2] = (0.5f + (-0.5f * g_current_camera->sub28.view.unk2C)) - 0.01f;
    sp1C[1][3] = 0.0f;
    sp1C[2][0] = 0.0f;
    sp1C[2][1] = 0.0f;
    sp1C[2][2] = 1.0f;
    sp1C[2][3] = 0.0f;
    mtxa_push();
    mtxa_mult_left(sp1C);
    GXLoadTexMtxImm(mtxa->mtxA, sp64.texMtxId, 0);
    mtxa_pop();
    sp14.r = 0x40;
    sp14.g = 0x50;
    sp14.b = 0x80;
    GXSetTevKColor_cached(3, sp14);
    GXSetTevKColorSel_cached(sp64.tevStage, 0xF);
    g_GXSetTevOrder_wrapper(sp64.tevStage, sp64.texCoordId, sp64.texMapId, 0xFF);
    GXSetTevColorIn_cached(sp64.tevStage, 0xF, 8, 0xE, 0);
    GXSetTevColorOp_cached(sp64.tevStage, 0, 0, 0, 1, 0);
    GXSetTevAlphaIn_cached(sp64.tevStage, 7, 7, 7, 0);
    GXSetTevAlphaOp_cached(sp64.tevStage, 0, 0, 0, 1, 0);
    sp64.tevStage += 1;
    sp64.tevIndStage += 1;
    sp64.texCoordId += 1;
    sp64.texMapId += 1;
    sp64.unk1C += 1;
    sp64.texMtxId += 3;
    arg0->tev = sp64;
}

void fn_1_75EF4(struct BallEnvArg *arg0)
{
    struct BgSndWork *work = world_theme.work;
    struct TevStageInfo sp64;
    Mtx sp1C;
    float sp4C[2][3];
    GXColor sp14;
    struct BgSndFountain *temp_r26;

    sp64 = arg0->tev;
    if (arg0->unk0 == 0
     && sp64.tevStage == lbl_1_bss_87F50.tevStage
     && sp64.texCoordId == lbl_1_bss_87F50.texCoordId
     && sp64.tevIndStage == lbl_1_bss_87F50.tevIndStage
     && sp64.texMtxId == lbl_1_bss_87F50.texMtxId
     && sp64.unk1C == lbl_1_bss_87F50.unk1C
     && sp64.texMapId == lbl_1_bss_87F50.texMapId)
    {
        sp64.tevIndStage += 2;
        sp64.texCoordId += 2;
        sp64.texMtxId += 6;
        sp64.unk1C += 2;
        sp64.texMapId += 2;
        sp64.tevStage += 1;
        sp64.tevIndStage += 1;
        sp64.texCoordId += 2;
        sp64.texMapId += 1;
        sp64.unk1C += 1;
        sp64.texMtxId += 3;
        arg0->tev = sp64;
        return;
    }

    lbl_1_bss_87F50 = sp64;
    temp_r26 = work->unk564;
    sp4C[0][0] = temp_r26->unk30;
    sp4C[0][1] = 0.0f;
    sp4C[0][2] = 0.0f;
    sp4C[1][0] = 0.0f;
    sp4C[1][1] = temp_r26->unk30;
    sp4C[1][2] = 0.0f;
    g_GXSetIndTexMtx_wrapper(sp64.unk1C, sp4C, -2);
    sp4C[0][0] = 0.6f * temp_r26->unk30;
    sp4C[0][1] = 0.0f;
    sp4C[0][2] = 0.0f;
    sp4C[1][0] = 0.0f;
    sp4C[1][1] = 0.8f * temp_r26->unk30;
    sp4C[1][2] = 0.0f;
    g_GXSetIndTexMtx_wrapper(sp64.unk1C + 1, sp4C, -2);
    sp4C[0][0] = 0.4f;
    sp4C[0][1] = 0.0f;
    sp4C[0][2] = 0.0f;
    sp4C[1][0] = 0.0f;
    sp4C[1][1] = -0.6f;
    sp4C[1][2] = 0.0f;
    g_GXSetIndTexMtx_wrapper(sp64.unk1C + 2, sp4C, 0);
    GXLoadTexObj_cached(work->fountainIndirectTex, sp64.texMapId);
    GXLoadTexObj_cached(work->unk38, sp64.texMapId + 1);
    GXLoadTexObj_cached(work->fountainGradientTex, sp64.texMapId + 2);
    mtxa_push();
    mtxa_from_translate(&temp_r26->unk18);
    GXLoadTexMtxImm(mtxa->mtxA, sp64.texMtxId, 0);
    mtxa_pop();
    {
        int mtx = sp64.texMtxId;
        GXSetTexCoordGen2_cached(sp64.texCoordId, 1, 4, mtx, 0, 0x7D);
    }
    GXSetIndTexOrder_cached(sp64.tevIndStage, sp64.texCoordId, sp64.texMapId);
    GXSetTevIndirect_cached(sp64.tevStage - 1, sp64.tevIndStage, 0, 7, sp64.unk1C, 0, 0, 0, 0, 0);
    sp64.tevIndStage += 1;
    sp64.texCoordId += 1;
    sp64.texMtxId += 3;
    sp64.unk1C += 1;
    sp64.texMapId += 1;
    GXSetIndTexOrder_cached(sp64.tevIndStage, sp64.texCoordId - 1, sp64.texMapId - 1);
    GXSetTevIndirect_cached(sp64.tevStage, sp64.tevIndStage, 0, 7, sp64.unk1C, 0, 0, 0, 0, 0);
    {
        int mtx = sp64.texMtxId;
        GXSetTexCoordGen2_cached(sp64.texCoordId, 0, 0, mtx, 0, 0x7D);
    }
    sp1C[0][0] = 1.05f * (-0.5f * g_current_camera->sub28.view.invTanHalfFov * (1.0f / g_current_camera->sub28.view.aspect));
    sp1C[0][1] = 0.0f;
    sp1C[0][2] = 0.5f + (-0.5f * g_current_camera->sub28.view.unk28);
    sp1C[0][3] = 0.0f;
    sp1C[1][0] = 0.0f;
    sp1C[1][1] = 1.05f * (0.5f * g_current_camera->sub28.view.invTanHalfFov);
    sp1C[1][2] = (0.5f + (-0.5f * g_current_camera->sub28.view.unk2C)) - 0.01f;
    sp1C[1][3] = 0.0f;
    sp1C[2][0] = 0.0f;
    sp1C[2][1] = 0.0f;
    sp1C[2][2] = 1.0f;
    sp1C[2][3] = 0.0f;
    mtxa_push();
    mtxa_mult_left(sp1C);
    GXLoadTexMtxImm(mtxa->mtxA, sp64.texMtxId, 0);
    mtxa_pop();
    sp14.r = 0x40;
    sp14.g = 0x50;
    sp14.b = 0x80;
    GXSetTevKColor_cached(3, sp14);
    GXSetTevKColorSel_cached(sp64.tevStage, 0xF);
    g_GXSetTevOrder_wrapper(sp64.tevStage, sp64.texCoordId, sp64.texMapId, 0xFF);
    GXSetTevColorIn_cached(sp64.tevStage, 0xF, 8, 0xE, 0);
    GXSetTevColorOp_cached(sp64.tevStage, 0, 0, 0, 1, 0);
    GXSetTevAlphaIn_cached(sp64.tevStage, 7, 7, 7, 0);
    GXSetTevAlphaOp_cached(sp64.tevStage, 0, 0, 0, 1, 0);
    sp64.tevStage += 1;
    sp64.tevIndStage += 1;
    sp64.texCoordId += 1;
    sp64.texMapId += 1;
    sp64.unk1C += 1;
    sp64.texMtxId += 3;
    GXSetIndTexOrder_cached(sp64.tevIndStage, sp64.texCoordId - 2, sp64.texMapId - 2);
    GXSetTevIndirect_cached(sp64.tevStage, sp64.tevIndStage, 0, 0, sp64.unk1C, 0, 0, 0, 0, 0);
    {
        int mtx = sp64.texMtxId;
        GXSetTexCoordGen2_cached(sp64.texCoordId, 1, 0, mtx, 0, 0x7D);
    }
    mtxa_push();
    mtxa_from_translate_xyz(0.0016666667f * world_theme.animTimer, 0.0f, 0.0f);
    mtxa_scale_xyz(0.1f, -0.025f, 1.0f);
    mtxa_rotate_y(world_theme.unkA4 << 7);
    mtxa_rotate_x(0x400);
    mtxa_translate_xyz(0.0f, 10.0f, 0.0f);
    GXLoadTexMtxImm(mtxa->mtxA, sp64.texMtxId, 1);
    mtxa_pop();
    GXSetTevSwapMode_cached(sp64.tevStage, 0, 1);
    g_GXSetTevOrder_wrapper(sp64.tevStage, sp64.texCoordId, sp64.texMapId, 0xFF);
    GXSetTevColorIn_cached(sp64.tevStage, 0xF, 0xF, 0xF, 0);
    GXSetTevColorOp_cached(sp64.tevStage, 0, 0, 0, 1, 0);
    GXSetTevAlphaIn_cached(sp64.tevStage, 7, 0, 4, 7);
    GXSetTevAlphaOp_cached(sp64.tevStage, 0, 0, 0, 1, 0);
    sp64.tevStage += 1;
    sp64.tevIndStage += 1;
    sp64.texCoordId += 1;
    sp64.texMapId += 1;
    sp64.unk1C += 1;
    sp64.texMtxId += 3;
    arg0->tev = sp64;
}

void fn_1_767E0(struct BallEnvArg *arg0)
{
    struct BgSndWork *work = world_theme.work;
    struct TevStageInfo sp2C;
    float sp14[2][3];
    struct BgSndFountain *temp_r28;

    sp2C = arg0->tev;
    if (arg0->unk0 == 0
     && sp2C.tevStage == lbl_1_bss_87F50.tevStage
     && sp2C.texCoordId == lbl_1_bss_87F50.texCoordId
     && sp2C.tevIndStage == lbl_1_bss_87F50.tevIndStage
     && sp2C.texMtxId == lbl_1_bss_87F50.texMtxId
     && sp2C.unk1C == lbl_1_bss_87F50.unk1C
     && sp2C.texMapId == lbl_1_bss_87F50.texMapId)
    {
        sp2C.tevIndStage += 1;
        sp2C.texCoordId += 1;
        sp2C.texMtxId += 3;
        sp2C.unk1C += 1;
        sp2C.texMapId += 1;
        arg0->tev = sp2C;
        return;
    }
    lbl_1_bss_87F50 = sp2C;
    temp_r28 = work->unk564;
    sp14[0][0] = temp_r28->unk30;
    sp14[0][1] = 0.0f;
    sp14[0][2] = 0.0f;
    sp14[1][0] = 0.0f;
    sp14[1][1] = temp_r28->unk30;
    sp14[1][2] = 0.0f;
    g_GXSetIndTexMtx_wrapper(sp2C.unk1C, sp14, -2);
    GXLoadTexObj_cached(work->fountainIndirectTex, sp2C.texMapId);
    mtxa_push();
    mtxa_from_translate(&temp_r28->unk18);
    GXLoadTexMtxImm(mtxa->mtxA, sp2C.texMtxId, 0);
    mtxa_pop();
    {
        int mtx = sp2C.texMtxId;
        GXSetTexCoordGen2_cached(sp2C.texCoordId, 1, 4, mtx, 0, 0x7D);
    }
    GXSetIndTexOrder_cached(sp2C.tevIndStage, sp2C.texCoordId, sp2C.texMapId);
    GXSetTevIndirect_cached(sp2C.tevStage - 1, sp2C.tevIndStage, 0, 7, sp2C.unk1C, 0, 0, 0, 0, 0);
    sp2C.tevIndStage += 1;
    sp2C.texCoordId += 1;
    sp2C.texMtxId += 3;
    sp2C.unk1C += 1;
    sp2C.texMapId += 1;
    arg0->tev = sp2C;
}

int fn_1_76BA0(int index, struct GMAModelEntry *entry)
{
    struct BgSndWork *work = world_theme.work;

    switch (index)
    {
    case 0:  // SND_KAGE_LOW_MAP_A
        work->shadowMapTex = &entry->model->texObjs[0];
        break;
    case 1:  // SND_STAGE_SAND_A
        work->sandTex = &entry->model->texObjs[0];
        break;
    case 2:  // SND_FUNSUI_WATER_A_URA
        work->waterAUraModel = entry->model;
        break;
    case 3:  // SND_FUNSUI_WATER_B
        work->waterBModel = entry->model;
        break;
    case 4:  // SND_FUNSUI_WATER_B_URA
        work->waterBUraModel = entry->model;
        break;
    case 5:  // SND_FUNSUI_IND_A
        work->fountainIndirectTex = &entry->model->texObjs[0];
        break;
    case 6:  // SND_FUNSUI_GRAD_A
        work->fountainGradientTex = &entry->model->texObjs[0];
        break;
    case 7:  // SND_FUNSUI_UP_TATE "fountain up vertical"
    case 8:  // SND_FUNSUI_UP_YOKO "fountain up horizontal"
    case 9:  // SND_FUNSUI_DOWN_TATE "fountain down verical"
    case 10:  // SND_FUNSUI_DOWN_YOKO " fountain down horizontal"
        work->unk538[index] = entry->model;
        break;
    }
    return 1;
}

int fn_1_76C48(int index, struct StageBgModel *bgObj)
{
    struct BgSndWork *work = world_theme.work;

    switch (index)
    {
    case 0:  // SND_PIRAM_CLOUD
        if (bgObj->model != NULL && work->unk40 < 8)
        {
            work->pyramidClouds[work->unk40] = bgObj;
            work->unk40++;
        }
        break;
    case 1:  // SND_FUNSUI_WATER
        if (bgObj->model != NULL && work->unk7C < 8)
        {
            work->fountainWater[work->unk7C] = bgObj;
            work->unk7C++;
        }
        break;
    }
    return 1;
}
