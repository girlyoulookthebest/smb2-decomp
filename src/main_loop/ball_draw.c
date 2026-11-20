/* Ball and ape drawing (split from ball.c: separate literal pool at rodata 0x7600) */
#include "types.h"
#include "macros.h"
#include "mathutil.h"
#include "ball.h"
#include "mode.h"
#include "avdisp.h"
#include "gx.h"
#include "light.h"
#include "frustum.h"
#include "nl2ngc.h"
#include "libc.h"
#include "stcoli.h"
#include "dip.h"
#include "event.h"
#include "shadow.h"
#include "pc_hfr.h"

/* .data (0x2E700 - 0x2E950) */
u8 lbl_1_data_2E700[0x170] =
{
    0xFE, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0xFE, 0x0D,
    0x0E, 0x0F, 0xFF, 0x00, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B,
    0x0C, 0x0D, 0x0E, 0x0F, 0xFF, 0x00, 0x00, 0x00, 0x31, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x38, 0x00, 0xB4, 0x00, 0x00, 0x01, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00,
    0x10, 0x00, 0x4C, 0x00, 0x00, 0x03, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00,
    0x18, 0x00, 0x00, 0x00, 0x00, 0x09, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x01, 0x00, 0x00, 0x00, 0x00, 0x0B, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x0C, 0x00, 0x00,
    0x18, 0x00, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x01, 0x00, 0x00, 0x00, 0x00, 0x07, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00,
    0x38, 0x00, 0xB4, 0x00, 0x00, 0x0D, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x18, 0x00, 0x00, 0x00, 0x00, 0x14, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x01, 0x00, 0x00, 0x00, 0x00, 0x16, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x17, 0x00, 0x00,
    0x18, 0x00, 0x00, 0x00, 0x00, 0x18, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x0F, 0x00, 0x00,
    0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x11, 0x00, 0x00,
    0x18, 0x00, 0x00, 0x00, 0x00, 0x12, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x13, 0x00, 0x00,
    0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00,
    0x18, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00,
    0x18, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00,
    0x18, 0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x07, 0x00, 0x00,
    0x18, 0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x09, 0x00, 0x00,
    0x18, 0x00, 0x00, 0x00, 0x00, 0x0A, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x0B, 0x00, 0x00,
    0x18, 0x00, 0x00, 0x00, 0x00, 0x0C, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x0D, 0x00, 0x00,
    0x18, 0x00, 0x00, 0x00, 0x00, 0x0E, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x0F, 0x00, 0x00,
};
/* ball shadow colour per ball colour (GXColor: a u32 table read as GXColor gave a
 * byte-swapped colour on little-endian builds) */
GXColor lbl_1_data_2E870[10] =
{
    { 0x47, 0x5F, 0x5F, 0xFF }, { 0x5F, 0x57, 0x47, 0xFF }, { 0x47, 0x49, 0x5F, 0xFF }, { 0x5F, 0x4B, 0x5F, 0xFF }, { 0x5F, 0x4B, 0x4B, 0xFF },
    { 0x47, 0x5F, 0x4B, 0xFF }, { 0x5C, 0x5A, 0x5A, 0xFF }, { 0x47, 0x47, 0x49, 0xFF }, { 0x4E, 0x5F, 0x47, 0xFF }, { 0x4E, 0x47, 0x5F, 0xFF },
};
/* ball shell model ids, indexed by ball mode */
s16 lbl_1_data_2E898[6] = { 0x15, 0x16, 0x17, 0x14, 0x17, 0 };
s16 lbl_1_data_2E8A4[6] = { 0x19, 0x1A, 0x1B, 0x18, 0x1B, 0 };
s16 lbl_1_data_2E8B0[6] = { 0x0C, 0x0D, 0x0E, 0x0B, 0x0E, 0 };
s16 lbl_1_data_2E8BC[6] = { 0x10, 0x11, 0x12, 0x0F, 0x12, 0 };
GXColor ball_colors[10] =
{
    { 0xFF, 0x00, 0x00, 0x00 }, { 0x00, 0x4C, 0xFF, 0x00 }, { 0xFF, 0xE5, 0x00, 0x00 },
    { 0x00, 0xCC, 0x00, 0x00 }, { 0x00, 0xCC, 0xCC, 0x00 }, { 0xFF, 0x00, 0xCC, 0x00 },
    { 0x19, 0x33, 0x33, 0x00 }, { 0xFF, 0xFF, 0xE5, 0x00 }, { 0xB2, 0x00, 0xFF, 0x00 },
    { 0xB2, 0xFF, 0x00, 0x00 },
};
GXColor lbl_1_data_2E8F0 = { 0xFF, 0xFF, 0xFF, 0xFF };
u8 lbl_1_data_2E8F4[4] = { 0xC0, 0x40, 0x40, 0x00 };  /* indexed by lbl_1_bss_70F28.unk0 */
u8 lbl_1_data_2E8F8[4] = { 0x40, 0xC0, 0x5C, 0x00 };
struct ShadowTexInfo
{
    /*0x00*/ u8 unk0;
    /*0x04*/ GXTexObj texObj;
};  /* size = 0x24 */

struct ShadowTexInfo lbl_1_data_2E8FC = { 1 };

/* .bss (0x70F28 - 0x70F38) */
struct
{
    s8 unk0;
    Vec dir;  /* reflection direction for the env-map layer */
} lbl_1_bss_70F28;

void g_math_unk13(Vec *a, Vec *b, Vec *c);
void GXLoadTexMtxImm(Mtx m, u32 id, int type);
void GXLoadTexObj_cached(GXTexObj *obj, int map);
void g_GXSetTevIndirect_zero_if_different(int stage);
void GXSetTevSwapMode_cached(int stage, int rasSel, int texSel);
void GXSetTexCoordGen2_cached(int dst, int func, int src, int mtx, int normalize, int postMtx);
void g_GXSetTevOrder_wrapper(int stage, int coord, int map, int color);
void GXSetTevColorIn_cached(int stage, int a, int b, int c, int d);
void GXSetTevColorOp_cached(int stage, int op, int bias, int scale, int clamp, int reg);
void GXSetTevAlphaIn_cached(int stage, int a, int b, int c, int d);
void GXSetTevAlphaOp_cached(int stage, int op, int bias, int scale, int clamp, int reg);

/* Unreferenced in the shipped game (dead-stripped); only its literals (0.0f, -1.0f)
 * survive at the head of this file's rodata pool. */
float fn_1_ball_draw_stripped(float x)
{
    return x > 0.0f ? 0.0f : -1.0f;
}

/* Adds a sphere-mapped reflection layer to the ball material */
void fn_1_4CEDC(struct BallEnvArg *arg)
{
    struct TevStageInfo tev;

    mtxa_push();
    tev = arg->tev;
    mtxa->mtxA[0][3] = 0.0f;
    mtxa->mtxA[1][3] = 0.0f;
    mtxa->mtxA[2][3] = 0.0f;
    GXLoadTexMtxImm(mtxa->mtxA, tev.texMtxId, 0);
    GXLoadTexObj_cached(&init_common_gma->modelEntries[19].model->texObjs[0], tev.texMapId);
    {
        Vec a = { 0.0f, 0.0f, 0.0f };
        Vec b = { 0.0f, 1.0f, 0.0f };
        g_math_unk13(&a, &b, &lbl_1_bss_70F28.dir);
    }
    mtxa->mtxA[0][3] = 0.5f;
    mtxa->mtxA[1][0] *= -1.0f;
    mtxa->mtxA[1][1] *= -1.0f;
    mtxa->mtxA[1][2] *= -1.0f;
    mtxa->mtxA[1][3] = 0.5f;
    mtxa->mtxA[2][0] = 0.0f;
    mtxa->mtxA[2][1] = 0.0f;
    mtxa->mtxA[2][2] = 0.0f;
    mtxa->mtxA[2][3] = 1.0f;
    mtxa_scale_s(0.45f);
    GXLoadTexMtxImm(mtxa->mtxA, tev.postTexMtxId, 0);
    g_GXSetTevIndirect_zero_if_different(tev.tevStage);
    GXSetTevSwapMode_cached(tev.tevStage, 0, 0);
    GXSetTexCoordGen2_cached(tev.texCoordId, 0, 1, tev.texMtxId, 1, tev.postTexMtxId);
    g_GXSetTevOrder_wrapper(tev.tevStage, tev.texCoordId, tev.texMapId, 255);
    GXSetTevColorIn_cached(tev.tevStage, 15, 8, 0, 15);
    GXSetTevColorOp_cached(tev.tevStage, 0, 0, 0, 1, 0);
    GXSetTevAlphaIn_cached(tev.tevStage, 7, 7, 7, 0);
    GXSetTevAlphaOp_cached(tev.tevStage, 0, 0, 0, 1, 0);
    tev.tevStage++;
    tev.texCoordId++;
    tev.texMapId++;
    tev.texMtxId++;
    tev.postTexMtxId++;
    arg->tev = tev;
    mtxa_pop();
}

extern Vec lbl_1_bss_D4244;
void GXSetTevKColor_cached(int id, GXColor color);
void GXSetTevKAlphaSel_cached(int stage, int sel);

void fn_1_4D260(struct BallEnvArg *arg)
{
    struct TevStageInfo tev = arg->tev;
    struct BallEnvShape *shape = arg->shape;
    Mtx mtx;
    GXColor color;

    mtxa_push();
    {
        Vec a = { 0.0f, 0.0f, 0.0f };
        Vec b = { 0.0f, 1.0f, 0.0f };
        Vec c = lbl_1_bss_D4244;
        Vec d = lbl_1_bss_70F28.dir;
        Quat quat;

        d.x *= -0.9f;
        d.y *= -0.9f;
        d.z *= -0.9f;
        if (lbl_1_bss_70F28.unk0 == 0)
        {
            switch (shape->unk15)
            {
            case 0:
            case 1:
                quat_from_axis_angle(&quat, &d, -0x8000);
                mtxa_from_quat(&quat);
                mtxa_tf_point(&c, &c);
                break;
            }
        }
        c.x = 0.5f * (d.x + c.x);
        c.y = 0.5f * (d.y + c.y);
        c.z = 0.5f * (d.z + c.z);
        g_math_unk13(&a, &b, &c);
    }
    mtxa_to_mtx(mtx);
    mtxa->mtxA[0][3] = 0.5f;
    mtxa->mtxA[1][0] *= -1.0f;
    mtxa->mtxA[1][1] *= -1.0f;
    mtxa->mtxA[1][2] *= -1.0f;
    mtxa->mtxA[1][3] = 0.5f;
    mtxa->mtxA[2][0] = 0.0f;
    mtxa->mtxA[2][1] = 0.0f;
    mtxa->mtxA[2][2] = 0.0f;
    mtxa->mtxA[2][3] = 1.0f;
    mtxa_scale_s(0.5f);
    GXLoadTexMtxImm(mtxa->mtxA, 0x43, 0);

    mtxa_from_identity();
    mtxa->mtxA[0][0] = 0.0f;
    mtxa->mtxA[0][2] = 0.5f;
    mtxa->mtxA[0][3] = 0.5f;
    mtxa->mtxA[1][1] = 0.0f;
    mtxa->mtxA[2][2] = 0.0f;
    mtxa->mtxA[2][3] = 1.0f;
    mtxa_mult_right(mtx);
    GXLoadTexMtxImm(mtxa->mtxA, 0x46, 0);
    mtxa_pop();

    switch (shape->unk15)
    {
    case 0:
    case 1:
        color.a = lbl_1_data_2E8F4[lbl_1_bss_70F28.unk0];
        break;
    case 4:
    case 5:
        color.a = lbl_1_data_2E8F8[lbl_1_bss_70F28.unk0];
        break;
    }
    if (lbl_1_data_2E8F0.a < color.a)
        color.a = lbl_1_data_2E8F0.a;
    color.r = lbl_1_data_2E8F0.r;
    color.g = lbl_1_data_2E8F0.g;
    color.b = lbl_1_data_2E8F0.b;
    GXSetTevKColor_cached(1, color);

    GXSetTevKAlphaSel_cached(tev.tevStage - 2, 0x1D);
    GXSetTevColorIn_cached(tev.tevStage - 2, 15, 8, 14, 15);
    GXSetTevColorOp_cached(tev.tevStage - 2, 0, 0, 0, 1, 3);
    GXSetTevAlphaIn_cached(tev.tevStage - 2, 7, 4, 6, 7);
    GXSetTevAlphaOp_cached(tev.tevStage - 2, 0, 0, 0, 1, 3);
    if (shape->unk12 <= 1)
    {
        GXSetTevColorIn_cached(tev.tevStage - 1, 15, 8, 6, 10);
        GXSetTevColorOp_cached(tev.tevStage - 1, 0, 0, 0, 1, 0);
        GXSetTevAlphaIn_cached(tev.tevStage - 1, 7, 4, 3, 5);
        GXSetTevAlphaOp_cached(tev.tevStage - 1, 0, 0, 0, 1, 0);
    }
    else
    {
        GXSetTevColorIn_cached(tev.tevStage - 1, 15, 8, 6, 0);
        GXSetTevColorOp_cached(tev.tevStage - 1, 0, 0, 0, 1, 0);
        GXSetTevAlphaIn_cached(tev.tevStage - 1, 7, 4, 3, 0);
        GXSetTevAlphaOp_cached(tev.tevStage - 1, 0, 0, 0, 1, 0);
    }
}

/* Material callback for the ball shell: picks the reflection layers by shape type */
void fn_1_4D82C(struct BallEnvArg *arg)
{
    switch (arg->shape->unk15)
    {
    case 0:
    case 1:
        fn_1_4D260(arg);
        break;
    case 4:
    case 5:
        fn_1_4D260(arg);
        fn_1_4CEDC(arg);
        break;
    }
}

void g_smth_with_gpu_and_locked_cache_mtx(void);
void *g_yet_another_unk_draw_func(void *func);

/* Returns the shape that follows the given one and its display lists */
static inline struct GMAShape *next_shape(struct GMAShape *shape)
{
    u8 *ptr = shape->dispLists;
    int i;

    for (i = 0; i < 2; i++)
    {
        if (shape->dispListFlags & (1 << i))
            ptr += shape->dispListSizes[i];
    }
    if (shape->dispListFlags & (GMA_SHAPE_HAS_DLIST2 | GMA_SHAPE_HAS_DLIST3))
    {
        int j;
        struct GMAExtraDispLists *extra = (void *)ptr;

        ptr = extra->dispLists;
        for (j = 0; j < 2; j++)
            ptr += extra->dispListSizes[j];
    }
    return (void *)ptr;
}

/* Copies the ball color into the tinted shapes of the shell model */
static inline void tint_ball_shapes(struct GMAModel *model, GXColor *color)
{
    struct GMAShape *shape = (void *)((u8 *)model + model->headerSize);
    int i;

    for (i = 0; i < model->translucentShapeCount; i++)
    {
        switch (shape->unk15)
        {
        case 1:
        case 3:
        case 5:
            shape->materialColor.r = shape->ambientColor.r = color->r;
            shape->materialColor.g = shape->ambientColor.g = color->g;
            shape->materialColor.b = shape->ambientColor.b = color->b;
            break;
        }
        shape = next_shape(shape);
    }
}

static inline void set_ball_shapes_alpha(struct GMAModel *model, u8 alpha)
{
    struct GMAShape *shape = (void *)((u8 *)model + model->headerSize);
    int i;

    for (i = 0; i < model->translucentShapeCount; i++)
    {
        shape->alpha = alpha;
        shape = next_shape(shape);
    }
}

void fn_1_4D8A0(Mtx mtx, int colorIdx, int mode)
{
    struct GMAModelEntry *entries = init_common_gma->modelEntries;
    struct GMAModel *model;
    GXColor *color;
    void *prevFunc;

    mtxa_from_mtx(mtx);
    g_smth_with_gpu_and_locked_cache_mtx();
    color = &ball_colors[colorIdx];
    GXSetZCompLoc_cached(1);
    switch (mode)
    {
    default:
        prevFunc = g_yet_another_unk_draw_func(fn_1_4D82C);
        lbl_1_bss_70F28.unk0 = 0;

        /* inner shell */
        avdisp_set_z_mode(1, 3, 0);
        model = entries[lbl_1_data_2E898[mode]].model;
        tint_ball_shapes(model, color);
        {
            Vec dir;

            mtxa_tf_point(&model->boundSphereCenter, &dir);
            vec_normalize_len(&dir);
            lbl_1_bss_70F28.dir = dir;
        }
        avdisp_draw_model_unculled_sort_never(model);

        /* outer shell */
        avdisp_set_z_mode(1, 3, 1);
        model = entries[lbl_1_data_2E8A4[mode]].model;
        tint_ball_shapes(model, color);
        {
            Vec dir;

            mtxa_tf_point(&model->boundSphereCenter, &dir);
            vec_normalize_len(&dir);
            lbl_1_bss_70F28.dir = dir;
        }
        avdisp_draw_model_unculled_sort_never(model);
        g_yet_another_unk_draw_func(prevFunc);
        break;
    case 4:
        prevFunc = g_yet_another_unk_draw_func(fn_1_4D260);
        lbl_1_bss_70F28.unk0 = 2;
        model = entries[lbl_1_data_2E8A4[mode]].model;
        tint_ball_shapes(model, color);
        set_ball_shapes_alpha(model, 0x5A);
        {
            Vec dir;

            mtxa_tf_point(&model->boundSphereCenter, &dir);
            vec_normalize_len(&dir);
            lbl_1_bss_70F28.dir = dir;
        }
        avdisp_draw_model_unculled_sort_never(model);
        g_yet_another_unk_draw_func(prevFunc);
        break;
    }
    GXSetZCompLoc_cached(0);
}

/* Draws the ball shell with a fixed straight-down reflection direction */
void fn_1_4DDEC(Mtx mtx, int colorIdx, int mode)
{
    struct GMAModelEntry *entries = init_common_gma->modelEntries;
    struct GMAModel *model;
    GXColor *color;
    void *prevFunc;

    mtxa_from_mtx(mtx);
    g_smth_with_gpu_and_locked_cache_mtx();
    color = &ball_colors[colorIdx];
    prevFunc = g_yet_another_unk_draw_func(fn_1_4D82C);
    lbl_1_bss_70F28.unk0 = 0;

    avdisp_set_z_mode(1, 3, 0);
    model = entries[lbl_1_data_2E898[mode]].model;
    tint_ball_shapes(model, color);
    lbl_1_bss_70F28.dir.x = 0.0f;
    lbl_1_bss_70F28.dir.y = 0.0f;
    lbl_1_bss_70F28.dir.z = -1.0f;
    avdisp_draw_model_unculled_sort_never(model);

    avdisp_set_z_mode(1, 3, 1);
    model = entries[lbl_1_data_2E8A4[mode]].model;
    tint_ball_shapes(model, color);
    lbl_1_bss_70F28.dir.x = 0.0f;
    lbl_1_bss_70F28.dir.y = 0.0f;
    lbl_1_bss_70F28.dir.z = -1.0f;
    avdisp_draw_model_unculled_sort_never(model);
    g_yet_another_unk_draw_func(prevFunc);
}

/* Draws only the outer half of the ball shell */
void fn_1_4E12C(Mtx mtx, int mode)
{
    struct GMAModelEntry *entries = init_common_gma->modelEntries;
    struct GMAModel *model;
    void *prevFunc;

    mtxa_from_mtx(mtx);
    g_smth_with_gpu_and_locked_cache_mtx();
    prevFunc = g_yet_another_unk_draw_func(fn_1_4D82C);
    lbl_1_bss_70F28.unk0 = 1;
    avdisp_set_z_mode(1, 3, 1);
    model = entries[lbl_1_data_2E8B0[mode]].model;
    {
        Vec dir;

        mtxa_tf_point(&model->boundSphereCenter, &dir);
        vec_normalize_len(&dir);
        lbl_1_bss_70F28.dir = dir;
    }
    avdisp_draw_model_unculled_sort_never(model);
    g_yet_another_unk_draw_func(prevFunc);
}

void draw_storysel_ball_clear_half(Mtx mtx, int mode)
{
    struct GMAModelEntry *entries = init_common_gma->modelEntries;
    struct GMAModel *model;
    void *prevFunc;

    mtxa_from_mtx(mtx);
    g_smth_with_gpu_and_locked_cache_mtx();
    prevFunc = g_yet_another_unk_draw_func(fn_1_4D82C);
    lbl_1_bss_70F28.unk0 = 1;
    avdisp_set_z_mode(1, 3, 1);
    model = entries[lbl_1_data_2E8B0[mode]].model;
    lbl_1_bss_70F28.dir.x = 0.0f;
    lbl_1_bss_70F28.dir.y = 0.0f;
    lbl_1_bss_70F28.dir.z = -1.0f;
    avdisp_draw_model_unculled_sort_never(model);
    g_yet_another_unk_draw_func(prevFunc);
}

void fn_1_4E350(Mtx mtx, struct GMAModel *model, int colorIdx);

void fn_1_4E2FC(Mtx mtx, int colorIdx, int mode)
{
    fn_1_4E350(mtx, init_common_gma->modelEntries[lbl_1_data_2E8BC[mode]].model, colorIdx);
}

/* Draws a tinted ball shell model */
void fn_1_4E350(Mtx mtx, struct GMAModel *model, int colorIdx)
{
    GXColor *color;
    void *prevFunc;

    mtxa_from_mtx(mtx);
    g_smth_with_gpu_and_locked_cache_mtx();
    color = &ball_colors[colorIdx];
    prevFunc = g_yet_another_unk_draw_func(fn_1_4D82C);
    lbl_1_bss_70F28.unk0 = 1;
    avdisp_set_z_mode(1, 3, 1);
    tint_ball_shapes(model, color);
    {
        Vec dir;

        mtxa_tf_point(&model->boundSphereCenter, &dir);
        vec_normalize_len(&dir);
        lbl_1_bss_70F28.dir = dir;
    }
    avdisp_draw_model_unculled_sort_never(model);
    g_yet_another_unk_draw_func(prevFunc);
}

void fn_1_4E504(Mtx mtx, int colorIdx, int mode)
{
    struct GMAModelEntry *entries = init_common_gma->modelEntries;
    struct GMAModel *model;
    GXColor *color;
    void *prevFunc;

    mtxa_from_mtx(mtx);
    g_smth_with_gpu_and_locked_cache_mtx();
    color = &ball_colors[colorIdx];
    prevFunc = g_yet_another_unk_draw_func(fn_1_4D82C);
    lbl_1_bss_70F28.unk0 = 1;
    avdisp_set_z_mode(1, 3, 1);
    model = entries[lbl_1_data_2E8BC[mode]].model;
    tint_ball_shapes(model, color);
    lbl_1_bss_70F28.dir.x = 0.0f;
    lbl_1_bss_70F28.dir.y = 0.0f;
    lbl_1_bss_70F28.dir.z = -1.0f;
    avdisp_draw_model_unculled_sort_never(model);
    g_yet_another_unk_draw_func(prevFunc);
}

void fn_1_4E6FC(GXColor *color)
{
    lbl_1_data_2E8F0 = *color;
}

/* Deferred draw node queued for the ball shell */
struct BallShellDrawNode
{
    /*0x00*/ struct OrdTblNode node;
    /*0x08*/ s32 lightGroup;
    /*0x0C*/ u8 fillerC[4];
    /*0x10*/ Mtx mtx;
    /*0x40*/ s8 colorIdx;
    /*0x41*/ s8 mode;
};

/* SMB1's backgroundInfo; its first field is the s16 world theme */
#ifdef __MWERKS__
struct BackgroundInfo
{
    /*0x00*/ s16 bgId;
    /*0x02*/ u8 filler2[0x94 - 0x2];
    /*0x94*/ void *ballEnvFunc;
};
extern struct BackgroundInfo world_theme;
#else
/* PC: the real declaration (background.h).  It has native pointers before
 * ballEnvFunc, so on 64-bit the field is not at 0x94 (a local copy read
 * world_theme.unk8C there, and the ball environment pass never ran). */
#include "background.h"
#endif

extern struct
{
    u8 filler0[0x14];
    u32 ballDrawTime;
} perfInfo;

void fn_1_4E710(struct BallShellDrawNode *node)
{
    void *envFunc;

    perf_init_timer(8);
    load_light_group_cached(node->lightGroup);
    fn_1_4D8A0(node->mtx, node->colorIdx, node->mode);
    envFunc = world_theme.ballEnvFunc;
    if (envFunc != NULL)
    {
        struct GMAModel *model = init_common_gma->modelEntries[lbl_1_data_2E8A4[node->mode]].model;

        if (g_is_sphere_visible(&model->boundSphereCenter, model->boundSphereRadius))
        {
            envFunc = g_yet_another_unk_draw_func(envFunc);
            load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
            avdisp_draw_model_unculled_sort_never(model);
            g_yet_another_unk_draw_func(envFunc);
        }
    }
    perfInfo.ballDrawTime += perf_stop_timer(8);
}

static inline void draw_ball_outer_half_node(Mtx mtx, int mode)
{
    struct GMAModelEntry *entries = init_common_gma->modelEntries;
    struct GMAModel *model;
    void *prevFunc;

    mtxa_from_mtx(mtx);
    g_smth_with_gpu_and_locked_cache_mtx();
    prevFunc = g_yet_another_unk_draw_func(fn_1_4D82C);
    lbl_1_bss_70F28.unk0 = 1;
    avdisp_set_z_mode(1, 3, 1);
#ifdef __MWERKS__
    /* Reuse the completed mode lookup for the model pointer. */
    mode = (int)entries[lbl_1_data_2E8B0[mode]].model;
    model = (struct GMAModel *)mode;
#else
    model = entries[lbl_1_data_2E8B0[mode]].model;
#endif
    {
        Vec dir;

        mtxa_tf_point(&model->boundSphereCenter, &dir);
        vec_normalize_len(&dir);
        lbl_1_bss_70F28.dir = dir;
    }
    avdisp_draw_model_unculled_sort_never(model);
    g_yet_another_unk_draw_func(prevFunc);
}

void fn_1_4E810(struct BallShellDrawNode *node)
{
    load_light_group_cached(node->lightGroup);
    draw_ball_outer_half_node(node->mtx, node->mode);
}

void fn_1_4E904(struct BallShellDrawNode *node)
{
    load_light_group_cached(node->lightGroup);
    fn_1_4E2FC(node->mtx, node->colorIdx, node->mode);
}

struct OrdTblNode *fn_1_CCDC8(Vec *pos);
struct OrdTblNode *ord_tbl_get_entry_for_pos(Vec *pos);
void *ord_tbl_alloc_node(u32 size);
void fn_1_CD418(struct OrdTblNode *list, struct OrdTblNode *node);  /* ord_tbl_insert_node */

void fn_1_4EAFC(Mtx mtx, int colorIdx, int mode)
{
    Vec pos;
    struct BallShellDrawNode *node;
    struct OrdTblNode *entry;

    pos.x = mtx[0][3];
    pos.y = mtx[1][3];
    pos.z = mtx[2][3];
    entry = fn_1_CCDC8(&pos);
    node = ord_tbl_alloc_node(sizeof(*node));
    if (node != NULL)
    {
        node->node.drawFunc = (void *)fn_1_4E710;
        node->lightGroup = get_active_light_group();
        memcpy(node->mtx, mtx, sizeof(node->mtx));
        node->colorIdx = (s8)colorIdx;
        node->mode = (s8)mode;
        fn_1_CD418(entry, &node->node);
    }
}

void fn_1_4EBAC(Mtx mtx, int mode)
{
    Vec pos;
    struct BallShellDrawNode *node;
    struct OrdTblNode *entry;

    pos.x = mtx[0][3];
    pos.y = mtx[1][3];
    pos.z = mtx[2][3];
    entry = ord_tbl_get_entry_for_pos(&pos);
    node = ord_tbl_alloc_node(sizeof(*node));
    if (node != NULL)
    {
        node->node.drawFunc = (void *)fn_1_4E810;
        node->lightGroup = get_active_light_group();
        memcpy(node->mtx, mtx, sizeof(node->mtx));
        node->mode = (s8)mode;
        fn_1_CD418(entry, &node->node);
    }
}

void fn_1_4EC60(Mtx mtx, int colorIdx, int mode)
{
    Vec pos;
    struct BallShellDrawNode *node;
    struct OrdTblNode *entry;

    pos.x = mtx[0][3];
    pos.y = mtx[1][3];
    pos.z = mtx[2][3];
    entry = ord_tbl_get_entry_for_pos(&pos);
    node = ord_tbl_alloc_node(sizeof(*node));
    if (node != NULL)
    {
        node->node.drawFunc = (void *)fn_1_4E904;
        node->lightGroup = get_active_light_group();
        memcpy(node->mtx, mtx, sizeof(node->mtx));
        node->colorIdx = (s8)colorIdx;
        node->mode = (s8)mode;
        fn_1_CD418(entry, &node->node);
    }
}


void fn_1_4ED10(void)
{
    struct ShadowReq req;
    struct RaycastHit hit;
    Vec sp8;
    struct Ball *ball;
    int i;

    req.unk20 = 0.025f;
    req.unk24 = 0.05f;
    req.model = init_common_gma->modelEntries[103].model;

    ball = balls;
    for (i = 0; i < 8; i++, ball++)
    {
        float f;

        if (ball->status == 0 || ball->status == 4)
            continue;
        if ((ball->flags & 0x10) && !(ball->flags & 0x80))
            continue;
        if ((int)raycast_stage_down(&ball->pos, &hit, &sp8) == 0)
            continue;
        f = 1.0 - (ball->pos.y - hit.pos.y) * 0.2;
        if (f < 0.0f)
            continue;

        req.color = lbl_1_data_2E870[ball->colorId];
        req.color.r *= f;
        req.color.g *= f;
        req.color.b *= f;

        req.scale.x = ball->currRadius * (1.0f + 2.0f * (1.0f - f));
        req.scale.y = req.scale.x;
        req.scale.z = req.scale.x;

        vec_to_euler(&hit.normal, &req.rot);
        req.rot.z = ball->rotZ;
        req.pos = hit.pos;
        submit_shadow_req(&req);
    }
}

extern u32 monkey_flags;
extern u32 g_some_render_flag;
float fn_1_80D4(Vec *pos);
void g_smth_with_rendefc_reflective_height(float height);
void draw_ape(struct Ape *ape);
void mtxa_scale_xyz(float x, float y, float z);

void g_draw_ball_and_ape(void)
{
    struct Ball *ball;
    int i;
    struct RaycastHit hit;
    Quat quat;
    Vec up;

    if ((monkey_flags & (1 << 5)) && !(dip_switches & DIP_NO_STAGE))
    {
        perf_init_timer(8);
        ball = balls;
        for (i = 0; i < 8; i++, ball++)
        {
            if (ball->status == 0 || ball->status == 4)
                continue;
            perfInfo.ballDrawTime += perf_stop_timer(8);
            HFR_TAG(HFR_K_BALL, i);
            if (events[EVENT_APE].state != 0)
                draw_ape(ball->ape);
            perf_init_timer(8);
            if (ball->flags & 0x10)
            {
                HFR_UNTAG();
                continue;
            }
            if ((g_some_render_flag & (1 << 2)) && fn_1_80D4(&ball->pos) < 0.0f)
            {
                HFR_UNTAG();
                continue;
            }

            mtxa_from_mtxb();
            if (g_is_sphere_visible(&ball->pos, 0.5f * ball->modelScale))
            {
                g_smth_with_rendefc_reflective_height(-1.0f);
                mtxa_from_mtxb();
                mtxa_mult_right(ball->unk30);
                mtxa_scale_s(ball->modelScale);
                fn_1_4EAFC(mtxa->mtxA, ball->colorId, ball->unk179);
                g_smth_with_rendefc_reflective_height(0.0f);
            }

            if (dip_switches & DIP_STCOLI)
            {
                raycast_stage_down(&ball->pos, &hit, NULL);
                if (hit.flags & 1)
                {
                    up.x = 0.0f;
                    up.y = 1.0f;
                    up.z = 0.0f;
                    quat_from_dirs(&quat, &up, &hit.normal);
                    mtxa_from_quat(&quat);
                    mtxa->mtxA[0][3] = hit.pos.x;
                    mtxa->mtxA[1][3] = hit.pos.y;
                    mtxa->mtxA[2][3] = hit.pos.z;
                    mtxa_mult_left(mtxa->mtxB);
                    mtxa_scale_xyz(0.2f, 0.2f, 0.2f);
                    avdisp_set_scale_factor(0.2f);
                    load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
                    avdisp_draw_model_culled_sort_auto(init_common_gma->modelEntries[90].model);
                }
            }
            HFR_UNTAG();
        }
        perfInfo.ballDrawTime += perf_stop_timer(8);
    }
}

struct BallShadowReq
{
    /*0x00*/ struct Ball *ball;
    /*0x04*/ Vec lightPos;
    /*0x10*/ float unk10;
    /*0x14*/ Vec pos;
    /*0x20*/ float radius;
    /*0x24*/ Vec up;
    /*0x30*/ s16 unk30;
    /*0x32*/ u16 cameraMask;
    /*0x34*/ float unk34;
    /*0x38*/ float unk38;
    /*0x3C*/ GXColor color;
    /*0x40*/ struct ShadowTexInfo tex;
};  /* size = 0x64 */

extern struct Light s_lightPool[];
extern u32 lbl_1_bss_D4A10;
void fn_1_D4650(struct BallShadowReq *req);
u8 GXGetTexObjMipMap(GXTexObj *obj);
int GXGetTexObjFmt(GXTexObj *obj);
u16 GXGetTexObjHeight(GXTexObj *obj);
u16 GXGetTexObjWidth(GXTexObj *obj);
void *GXGetTexObjData(GXTexObj *obj);
void GXInitTexObj(GXTexObj *obj, void *image, u16 width, u16 height, int format, int wrapS, int wrapT, u8 mipmap);
void GXInitTexObjLOD(GXTexObj *obj, int minFilt, int magFilt, float minLod, float maxLod, float lodBias, u8 biasClamp,
    u8 doEdgeLod, int maxAniso);

void draw_ball_shadow(void)
{
    Vec lightOfs;
    struct BallShadowReq req;
    struct Ball *ball;
    int i;
    GXTexObj *tex;
    int r28 = 0;

    switch (modeCtrl.gameType)
    {
    case 3:
    case 10:
        return;
    case 4:
        if ((world_theme.bgId == 6 || world_theme.bgId == 8) && modeCtrl.playerCount > 3)
        {
            fn_1_4ED10();
            return;
        }
        break;
    case 5:
    case 13:
        if (modeCtrl.unk30 > 2 || (lbl_1_bss_D4A10 & 1))
        {
            fn_1_4ED10();
            return;
        }
        break;
    default:
        if (modeCtrl.unk30 > 2)
        {
            fn_1_4ED10();
            return;
        }
        break;
    }

    mtxa_from_rotate_y(s_lightPool[0].rotY);
    mtxa_rotate_x(s_lightPool[0].rotX);
    lightOfs.x = 0.0f;
    lightOfs.y = 0.0f;
    lightOfs.z = -15.0f;
    mtxa_tf_vec(&lightOfs, &lightOfs);

    switch (modeCtrl.gameType)
    {
    case 1:
        req.unk30 = 6;
        req.unk34 = 0.1f;
        req.unk38 = 0.2f;
        break;
    default:
        req.unk30 = 0x46;
        req.unk34 = 0.025f;
        req.unk38 = 0.05f;
        break;
    }

    req.up.x = 0.0f;
    req.up.y = 1.0f;
    req.up.z = 0.0f;
    mtxa_tf_vec(&req.up, &req.up);

    tex = &init_common_gma->modelEntries[94].model->texObjs[0];
    GXInitTexObj(&lbl_1_data_2E8FC.texObj,
        GXGetTexObjData(tex),
        GXGetTexObjWidth(tex),
        GXGetTexObjHeight(tex),
        GXGetTexObjFmt(tex),
        0,
        0,
        GXGetTexObjMipMap(tex));
    GXInitTexObjLOD(&lbl_1_data_2E8FC.texObj, 1, 1, 0.0f, 0.0f, 0.0f, 0, 0, 0);
    req.tex = lbl_1_data_2E8FC;

    ball = balls;
    for (i = 0; i < 8; i++, ball++)
    {
        if (ball->status == 0 || ball->status == 4)
            continue;
        if ((ball->flags & 0x10) && !(ball->flags & 0x80))
            continue;
        req.ball = ball;
        req.color = lbl_1_data_2E870[ball->colorId];
        req.lightPos.x = lightOfs.x + ball->pos.x;
        req.lightPos.y = lightOfs.y + ball->pos.y;
        req.lightPos.z = lightOfs.z + ball->pos.z;
        req.unk10 = 5.0f;
        req.pos = ball->pos;
        req.radius = 1.4f * ball->currRadius;
        if (r28 != 0)
            req.cameraMask = 0xFFFF;
        else
        {
            switch (modeCtrl.gameType)
            {
            case 4:
            case 5:
            case 8:
            case 13:
                req.cameraMask = 0xFFFF;
                break;
            default:
                req.cameraMask = 1 << ball->playerId;
                break;
            }
        }
        fn_1_D4650(&req);
    }
}
