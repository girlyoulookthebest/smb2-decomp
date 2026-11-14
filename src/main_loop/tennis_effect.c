/* Tennis mini-game effects: ball trails, hit sparks and racket flashes, all
 * drawn from bt_effect.gma and registered over the generic effect types. */
#include "types.h"
#include "avdisp.h"
#include "effect.h"
#include "frustum.h"
#include "gx.h"
#include "libc.h"
#include "light.h"
#include "mathutil.h"
#include "pool.h"
#include "rand.h"

struct TrailVertex
{
    /*0x00*/ Vec pos;
    /*0x0C*/ GXColor color;
};  /* size = 0x10 */

struct TennisWork
{
    /*0x000*/ u8 filler0[0x288];
    /*0x288*/ struct GMA *gma;
};

extern struct TennisWork *lbl_19_bss_18;

extern void GXLoadNrmMtxImm(Mtx m, u32 id);
extern void GXLoadPosMtxImm(Mtx m, u32 id);
extern void fn_1_E2FD0(int width);
extern void fn_1_E2FE0(int a, int b, int c, int d);
extern void fn_1_E349C(void *a, void *b);
void vec_to_euler_xy(Vec *v, s16 *x, s16 *y);
BOOL DVDChangeDir(char *dir);
struct TPL *g_load_tpl(char *file);
struct GMA *g_load_gma(char *file, struct TPL *tpl);
void g_something_freeing_heap(void *p);
void g_something_freeing_heap_2(void *p);

struct TrailSegment
{
    Vec a;
    Vec b;
};  /* size = 0x18 */

struct GMA *lbl_1_bss_DD970;
s32 lbl_1_bss_DD974;
struct TPL *lbl_1_bss_DD978[2];

u32 lbl_1_data_79510[] = {
    0xFE000102, 0x03040506, 0x0708090A, 0x0B0CFE0D,
    0x0E0FFF00, 0x00010203, 0x04050607, 0x08090A0B,
    0x0C0D0E0F, 0xFF000000, 0x31000000, 0x00000000,
    0x3800B400, 0x00010000, 0x01000000, 0x00020000,
    0x10004C00, 0x00030000, 0x01000000, 0x00040000,
    0x18000000, 0x00090000, 0x40000000, 0x00000000,
    0x01000000, 0x000B0000, 0x18000000, 0x000C0000,
    0x18000000, 0x00050000, 0x40000000, 0x00000000,
    0x01000000, 0x00070000, 0x18000000, 0x00080000,
    0x3800B400, 0x000D0000, 0x40000000, 0x00000000,
    0x18000000, 0x00140000, 0x40000000, 0x00000000,
    0x01000000, 0x00160000, 0x18000000, 0x00170000,
    0x18000000, 0x00180000, 0x18000000, 0x000F0000,
    0x40000000, 0x00000000, 0x01000000, 0x00110000,
    0x18000000, 0x00120000, 0x18000000, 0x00130000,
    0x18000000, 0x00000000, 0x18000000, 0x00010000,
    0x18000000, 0x00020000, 0x18000000, 0x00030000,
    0x18000000, 0x00040000, 0x18000000, 0x00050000,
    0x18000000, 0x00060000, 0x18000000, 0x00070000,
    0x18000000, 0x00080000, 0x18000000, 0x00090000,
    0x18000000, 0x000A0000, 0x18000000, 0x000B0000,
    0x18000000, 0x000C0000, 0x18000000, 0x000D0000,
    0x18000000, 0x000E0000, 0x18000000, 0x000F0000,
};
struct TrailSegment lbl_1_data_79680[4] = { 0 };
u16 lbl_1_data_796E0[] = { 0x46, 0x47, 0x48, 0x49, 0x4A };

void fn_1_163B5C(struct Effect *effect)
{
    s16 pitch;
    s16 yaw;

    pitch = (rand() & 0x3FFF) + (rand() & 0x3FFF) - 0x4000;
    yaw = rand() & 0x7FFF;
    effect->vel.x = 0.7f * mathutil_cos(pitch) * math_sin(yaw);
    effect->vel.y = 0.7f * math_sin(pitch);
    effect->vel.z = 0.7f * mathutil_cos(pitch) * mathutil_cos(yaw);
    if ((rand() & 0x7FFF) % 2)
        effect->vel.x = -effect->vel.x;
    effect->model = lbl_1_bss_DD970->modelEntries[30].model;
    effect->timer = 0x30;
}

void fn_1_163C98(struct Effect *effect) {}

void fn_1_163C9C(struct Effect *effect)
{
    Vec pos;
    float t;

    t = (float)(effect->timer * effect->timer) / 2304.0f;
    pos.x = effect->pos.x + effect->vel.x * t;
    pos.y = effect->pos.y + effect->vel.y * t;
    pos.z = effect->pos.z + effect->vel.z * t;
    mtxa_from_mtxb_translate(&pos);
    mtxa_sq_from_identity();
    mtxa_scale_s(3.5f * t);
    load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
    avdisp_set_alpha(1.0f);
    avdisp_set_z_mode(1, 3, 0);
    avdisp_draw_model_unculled_sort_never(effect->model);
    avdisp_set_z_mode(1, 3, 1);
}

void fn_1_163DAC(Vec *pos)
{
    int unused[3] = { 0x1A, 0x18, 0x1C };

    if (lbl_1_bss_DD974 == 0)
    {
        struct Effect effect;

        memset(&effect, 0, sizeof(effect));
        effect.type = 0x32;
        effect.pos = *pos;
        spawn_effect(&effect);
    }
    lbl_1_bss_DD974 = (lbl_1_bss_DD974 + 1) % 2;
}

void fn_1_163E6C(struct Effect *effect) {}

void fn_1_163E70(struct Effect *effect)
{
    int models[] = { 0x1A, 0x1B, 0x18, 0x19, 0x1C, 0x1D };

    effect->model = lbl_1_bss_DD970->modelEntries[models[effect->state & 0xF]].model;
    effect->state ^= 1;
}

void fn_1_163EF4(struct Effect *effect)
{
    if (effect->timer <= 0x10)
    {
        GXSetBlendMode_cached(1, 4, 1, 0);
        avdisp_set_z_mode(1, 3, 0);
        mtxa_from_mtxb_translate(&effect->pos);
        mtxa_scale_s(0.001953125f * (effect->scale.x * (float)(0x100 - effect->timer * effect->timer)));
        if (effect->state < 0x10)
        {
            mtxa_rotate_z(0x4000);
            load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
            avdisp_draw_model_unculled_sort_never(effect->model);
        }
        else
        {
            mtxa_rotate_x(0x2000);
            load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
            avdisp_draw_model_unculled_sort_never(effect->model);
            mtxa_rotate_x(0x4000);
            load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
            avdisp_draw_model_unculled_sort_never(effect->model);
        }
        avdisp_set_z_mode(1, 3, 1);
    }
}

void fn_1_164030(struct Effect *effect)
{
    effect->model = lbl_1_bss_DD970->modelEntries[30].model;
}

void fn_1_16404C(struct Effect *effect)
{
    if (effect->timer < 0x20)
    {
        effect->pos.x = effect->pos.x + effect->vel.x;
        effect->pos.y = effect->pos.y + (effect->vel.y - 0.001f);
        effect->pos.z = effect->pos.z + effect->vel.z;
        effect->vel.x *= 0.965f;
        effect->vel.y *= 0.965f;
        effect->vel.z *= 0.965f;
    }
}

void fn_1_1640E4(struct Effect *effect)
{
    if (effect->timer < 0x20)
    {
        mtxa_from_mtxb_translate(&effect->pos);
        mtxa_sq_from_identity();
        mtxa_scale_s(effect->scale.x * (float)(0x20 - effect->timer) / 16.0f);
        load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
        avdisp_set_alpha((float)effect->timer / 32.0f);
        avdisp_set_z_mode(1, 3, 0);
        avdisp_draw_model_unculled_sort_never(effect->model);
        avdisp_set_z_mode(1, 3, 1);
    }
}

void fn_1_1641E4(Vec *pos, Vec *vel)
{
    struct Effect effect;
    float spread;

    memset(&effect, 0, sizeof(effect));
    effect.type = 0x2F;
    effect.pos = *pos;
    effect.vel = *vel;
    effect.scale.x = 1.5f;
    spread = mathutil_vec_len(vel) / 2.0f;
    effect.vel.x += spread * ((float)rand() / 32767.0f - 0.5f);
    effect.vel.y += spread * ((float)rand() / 32767.0f - 0.5f);
    effect.vel.z += spread * ((float)rand() / 32767.0f - 0.5f);
    effect.timer = (rand() & 0x7FFF) % 17 + 0x10;
    spawn_effect(&effect);
}

void fn_1_1643A8(struct Effect *effect)
{
    effect->prevPos = effect->pos;
    effect->flags = 2;
    effect->u_otherTimer = 0;
}

void fn_1_1643D4(struct Effect *effect)
{
    effect->pos.x = effect->pos.x + effect->vel.x;
    effect->pos.y = effect->pos.y + (effect->vel.y - effect->scale.x);
    effect->pos.z = effect->pos.z + effect->vel.z;
    effect->vel.x *= 0.962f;
    effect->vel.y *= 0.962f;
    effect->vel.z *= 0.962f;
}

void fn_1_164458(struct Effect *effect)
{
    struct TrailVertex b;
    struct TrailVertex a;
    u8 alpha;

    mtxa_from_mtxb();
    mtxa_tf_point(&effect->pos, &a.pos);
    mtxa_tf_point(&effect->prevPos, &b.pos);
    a.pos.x = a.pos.x + 8.0f * (a.pos.x - b.pos.x);
    a.pos.y = a.pos.y + 8.0f * (a.pos.y - b.pos.y);
    a.pos.z = a.pos.z + 8.0f * (a.pos.z - b.pos.z);
    mtxa_from_identity();
    alpha = (effect->timer >= 0x10) ? 0xFF : (u8)(effect->timer * 0xFF / 16);
    a.color.r = 255.0f * effect->unk18;
    a.color.g = 255.0f * effect->unk1C;
    a.color.b = 255.0f * effect->unk20;
    a.color.a = alpha;
    b.color.r = 96.0f * effect->unk18;
    b.color.g = 88.0f * effect->unk1C;
    b.color.b = 8.0f * effect->unk20;
    b.color.a = (u32)alpha >> 1;
    fn_1_E2FD0(0x1C);
    fn_1_E2FE0(1, 4, 1, 0);
    fn_1_E349C(&a, &b);
}

void fn_1_164620(struct Effect *effect)
{
    effect->model = lbl_1_bss_DD970->modelEntries[31].model;
}

void fn_1_16463C(struct Effect *effect) {}

void fn_1_164640(struct Effect *effect)
{
    if (effect->timer < 0x14)
    {
        mtxa_from_mtxb_translate(&effect->pos);
        mtxa_rotate_y(0x4000);
        mtxa_rotate_z(-effect->timer * 0xC00);
        mtxa_scale_s(effect->scale.x * (float)(effect->timer * effect->timer) / 400.0f);
        load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
        if (effect->timer > 0x10)
            avdisp_set_alpha((float)(0x14 - effect->timer) / 4.0f);
        avdisp_draw_model_unculled_sort_never(effect->model);
    }
}

void fn_1_16474C(struct Effect *effect)
{
    effect->rotX = (s16)(rand() & 0x7FFF);
    effect->rotY = (s16)(rand() & 0x7FFF);
    effect->rotZ = (s16)(rand() & 0x7FFF);
    effect->model = lbl_1_bss_DD970->modelEntries[35].model;
}

void fn_1_1647BC(struct Effect *effect) {}

void fn_1_1647C0(struct Effect *effect)
{
    if (effect->timer < 0x14)
    {
        mtxa_from_mtxb_translate(&effect->pos);
        mtxa_scale_s(0.0025f * (effect->scale.x * (float)(400 - effect->timer * effect->timer)));
        mtxa_rotate_z(0x4000);
        load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
        avdisp_set_alpha((float)(effect->timer * effect->timer) / 256.0f);
        avdisp_set_z_mode(1, 3, 0);
        avdisp_draw_model_unculled_sort_never(lbl_1_bss_DD970->modelEntries[36].model);
        avdisp_set_z_mode(1, 3, 1);
        mtxa_from_mtxb_translate(&effect->pos);
        mtxa_scale_s(0.0025f * (effect->scale.x * (float)(400 - effect->timer * effect->timer)));
        mtxa_rotate_y(effect->rotY);
        mtxa_rotate_x(effect->rotX);
        mtxa_rotate_z(effect->rotZ);
        load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
        avdisp_set_alpha((effect->timer < 10) ? (float)effect->timer / 10.0f : 1.0f);
        avdisp_set_z_mode(1, 3, 0);
        avdisp_draw_model_unculled_sort_never(effect->model);
        avdisp_set_z_mode(1, 3, 1);
    }
}

void fn_1_1649B0(struct Effect *effect)
{
    effect->timer = 0x10;
}

void fn_1_1649BC(struct Effect *effect)
{
    effect->pos.x = effect->pos.x + effect->vel.x;
    effect->pos.y = effect->pos.y + effect->vel.y;
    effect->pos.z = effect->pos.z + effect->vel.z;
    effect->vel.x *= 0.9f;
    effect->vel.y *= 0.9f;
    effect->vel.z *= 0.9f;
}

void fn_1_164A38(struct Effect *effect)
{
    mtxa_from_mtxb_translate(&effect->pos);
    mtxa_scale_s(effect->scale.x * (1.0f + (float)(0x10 - effect->timer) / 32.0f));
    mtxa_rotate_z(0x4000);
    load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
    avdisp_set_z_mode(1, 3, 0);
    avdisp_set_alpha((float)((0x10 - effect->timer) * effect->timer) / 256.0f);
    avdisp_draw_model_unculled_sort_never(lbl_1_bss_DD970->modelEntries[33].model);
    avdisp_set_alpha((float)(effect->timer * effect->timer) / 256.0f);
    avdisp_draw_model_unculled_sort_never(lbl_1_bss_DD970->modelEntries[34].model);
    avdisp_set_z_mode(1, 3, 1);
}


void fn_1_164B90(int type, Vec *pos, Vec *normal, float scale)
{
    int unused[3] = { 0x1A, 0x18, 0x1C };
    struct Effect effect;
    int i;
    float dot;

    memset(&effect, 0, sizeof(effect));
    effect.pos = *pos;
    effect.scale.x = scale;
    if (type == 2 || type == 3)
    {
        effect.type = 0x31;
        effect.timer = 0x14;
        spawn_effect(&effect);
        if (type == 3)
        {
            effect.timer = 0x1C;
            spawn_effect(&effect);
        }
    }
    else if (type == 0)
    {
        effect.type = 0x2F;
        for (i = 0; i < 0x18; i++)
        {
            effect.vel.x = scale * (0.06f * ((float)rand() / 32767.0f) - 0.03f);
            effect.vel.y = scale * (0.06f * ((float)rand() / 32767.0f) - 0.03f);
            effect.vel.z = scale * (0.06f * ((float)rand() / 32767.0f) - 0.03f);
            dot = mathutil_vec_dot_prod(normal, &effect.vel);
            effect.vel.x -= dot * normal->x;
            effect.vel.y -= dot * normal->y;
            effect.vel.z -= dot * normal->z;
            effect.timer = (rand() & 0x7FFF) % 8 + 0x20;
            spawn_effect(&effect);
        }
    }
    else if (type == 4)
    {
        effect.type = 0x33;
        for (i = 0; i < 3; i++)
        {
            effect.timer = i * 4 + 0x14;
            effect.scale.y = (i == 0) ? -1.0f : 0.0f;
            spawn_effect(&effect);
        }
        effect.type = 0x34;
        effect.vel.x = 0.3f * normal->x;
        effect.vel.y = 0.3f * normal->y;
        effect.vel.z = 0.3f * normal->z;
        spawn_effect(&effect);
    }
    else if (type == 4)
    {
        effect.type = 0x30;
        effect.unk18 = 1.0f;
        effect.unk1C = 1.0f;
        effect.unk20 = 1.0f;
        for (i = 0; i < 0x20; i++)
        {
            effect.vel.x = scale * (0.12f * ((float)rand() / 32767.0f) - 0.06f);
            effect.vel.y = scale * (0.06f * ((float)rand() / 32767.0f) - 0.03f);
            effect.vel.z = scale * (0.12f * ((float)rand() / 32767.0f) - 0.06f);
            dot = mathutil_vec_dot_prod(normal, &effect.vel);
            effect.vel.x -= dot * normal->x;
            effect.vel.y -= dot * normal->y;
            effect.vel.z -= dot * normal->z;
            effect.timer = (rand() & 0x7FFF) % 17 + 0x18;
            effect.scale.x = 0.0f;
            spawn_effect(&effect);
        }
    }
    else if (type == 1)
    {
        effect.type = 0x30;
        effect.unk18 = 1.0f;
        effect.unk1C = 0.9f;
        effect.unk20 = 0.5f;
        for (i = 0; i < (int)(24.0f * scale); i++)
        {
            int t;

            effect.vel.x = scale * (0.06f * ((float)rand() / 32767.0f) - 0.03f);
            effect.vel.y = scale * (0.005f * scale + (0.06f * ((float)rand() / 32767.0f) - 0.03f));
            effect.vel.z = scale * (0.06f * ((float)rand() / 32767.0f) - 0.03f);
            dot = mathutil_vec_dot_prod(normal, &effect.vel);
            effect.vel.x -= dot * normal->x;
            effect.vel.y -= dot * normal->y;
            effect.vel.z -= dot * normal->z;
            t = (rand() & 0x7FFF) % (int)(17.0f * scale);
            effect.timer = t + 0x10;
            effect.scale.x = 0.0008f * (float)t;
            spawn_effect(&effect);
        }
    }
    else
    {
        effect.type = 0x2E;
        effect.state = (type - 0x10) * 2;
        for (i = 0; i < 4; i++)
        {
            effect.timer = i * 2 + 0x10;
            spawn_effect(&effect);
        }
    }
}

void fn_1_165268(struct Effect *effect) {}

void fn_1_16526C(struct Effect *effect)
{
    effect->state -= 4;
}

void fn_1_16527C(struct Effect *effect)
{
    Vec pos;
    u8 alpha;

    mtxa_from_mtxb_translate(&effect->pos);
    mtxa_sq_from_identity();
    mtxa_tf_point_xyz(0.0f, 0.0f, 0.0f, &pos);
    g_something_with_GX_vtx_desc(0x2A00);
    GXSetBlendMode_cached(1, 4, 1, 0);
    g_something_with_raster_state(0);
    opti_GXSetChanCtrl(4, 0, 1, 1, 0, 2, 1);
    GXLoadTexObj_cached(effect->model->texObjs, 0);
    GXSetTexCoordGen2_cached(0, 1, 4, 0x3C, 0, 0x7D);
    g_GXSetTevOrder_wrapper(0, 0, 0, 4);
    g_GXSetTevColorIn_GXSetTevAlphaIn_wrapper(0, 0);
    g_GXSetTevIndirect_zero_if_different(0);
    GXSetNumTevStages_cached(1);
    GXSetNumTexGens_cached(1);
    GXSetNumIndStages_cached(0);
    GXSetNumChans_cached(1);
    alpha = 15.9375f * (float)effect->state;
    GXSetZMode_cached(1, 3, 0);
    GXSetCullMode_cached(0);
    mtxa_from_identity();
    GXLoadPosMtxImm(mtxa->mtxA, 0);
    GXLoadNrmMtxImm(mtxa->mtxA, 0);
    GXBegin(0x98, 0, 4);
    GXPosition3f32(pos.x - 0.15f, pos.y - 0.15f, pos.z);
    GXColor4u8(0xFF, 0xFF, 0xFF, alpha);
    GXTexCoord2f32(effect->scale.x, effect->scale.y);
    GXPosition3f32(pos.x - 0.15f, 0.15f + pos.y, pos.z);
    GXColor4u8(0xFF, 0xFF, 0xFF, alpha);
    GXTexCoord2f32(effect->scale.x, 0.5f + effect->scale.y);
    GXPosition3f32(0.15f + pos.x, pos.y - 0.15f, pos.z);
    GXColor4u8(0xFF, 0xFF, 0xFF, alpha);
    GXTexCoord2f32(0.5f + effect->scale.x, effect->scale.y);
    GXPosition3f32(0.15f + pos.x, 0.15f + pos.y, pos.z);
    GXColor4u8(0xFF, 0xFF, 0xFF, alpha);
    GXTexCoord2f32(0.5f + effect->scale.x, 0.5f + effect->scale.y);
    GXSetCullMode_cached(2);
    GXSetZMode_cached(1, 3, 1);
}

void fn_1_165644(struct Effect *effect)
{
    if (effect->scale.z > 0.0f)
    {
        mtxa_from_mtxb_translate(&effect->pos);
        mtxa_sq_from_identity();
        load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
        avdisp_set_alpha((float)effect->timer / effect->scale.z);
        avdisp_set_z_mode(1, 3, 0);
        avdisp_draw_model_unculled_sort_never(effect->model);
        avdisp_set_z_mode(1, 3, 1);
    }
    else
        fn_1_16527C(effect);
}

void fn_1_16570C(Vec *a, Vec *b, int kind)
{
    int models[] = { 0x12, 0x11, 0x10, 0x13 };
    struct Effect effect;
    int i;

    memset(&effect, 0, sizeof(effect));
    effect.type = 0x2D;
    if (kind == 0)
    {
        effect.pos = *a;
        effect.model = lbl_19_bss_18->gma->modelEntries[0].model;
        effect.timer = 6;
        effect.scale.x = 1.0f;
        effect.scale.z = 6.0f;
        spawn_effect(&effect);
    }
    else
    {
        effect.model = lbl_1_bss_DD970->modelEntries[models[kind - 1]].model;
        effect.timer = 4;
        effect.scale.z = 0.0f;
        effect.vel.x = a->x - b->x;
        effect.vel.y = a->y - b->y;
        effect.vel.z = a->z - b->z;
        switch (kind)
        {
        case 1:
            effect.scale.x = 0.0f;
            effect.scale.y = 0.0f;
            break;
        case 2:
            effect.scale.x = 0.5f;
            effect.scale.y = 0.0f;
            break;
        case 3:
            effect.scale.x = 0.5f;
            effect.scale.y = 0.5f;
            break;
        case 4:
            effect.scale.x = 0.0f;
            effect.scale.y = 0.5f;
            break;
        }
        for (i = 0; i < 4; i++)
        {
            float t1 = (float)(4 - i) / 4.0f;
            float t2 = (float)i / 4.0f;

            effect.pos.x = a->x * t1 + b->x * t2;
            effect.pos.y = a->y * t1 + b->y * t2;
            effect.pos.z = a->z * t1 + b->z * t2;
            effect.state = 0x10 - i;
            spawn_effect(&effect);
        }
    }
}

void fn_1_16598C(struct Effect *effect)
{
    effect->timer = effect->rotX;
}

void fn_1_165998(struct Effect *effect) {}

void fn_1_16599C(struct Effect *effect)
{
    u8 r;
    u8 g;
    u8 b;
    u8 a0;
    u8 a1;
    u8 c0;
    u8 c1;

    g_something_with_GX_vtx_desc(0xA00);
    GXSetBlendMode_cached(1, 4, 1, 0);
    g_something_with_raster_state(0);
    opti_GXSetChanCtrl(4, 0, 1, 1, 0, 2, 1);
    g_GXSetTevOrder_wrapper(0, 0xFF, 0xFF, 4);
    g_GXSetTevColorIn_GXSetTevAlphaIn_wrapper(0, 4);
    g_GXSetTevIndirect_zero_if_different(0);
    GXSetNumTevStages_cached(1);
    GXSetNumTexGens_cached(0);
    GXSetNumIndStages_cached(0);
    GXSetNumChans_cached(1);
    r = effect->unk52;
    g = effect->unk54;
    b = effect->unk56;
    a0 = effect->timer * effect->rotY / effect->rotX;
    a1 = (effect->timer - 1) * effect->rotY / effect->rotX;
    c0 = effect->timer * effect->rotZ / effect->rotX;
    c1 = (effect->timer - 1) * effect->rotZ / effect->rotX;
    GXSetCullMode_cached(0);
    mtxa_from_mtxb();
    GXLoadPosMtxImm(mtxa->mtxA, 0);
    GXLoadNrmMtxImm(mtxa->mtxA, 0);
    GXBegin(0x98, 0, 6);
    GXPosition3f32(effect->pos.x, effect->pos.y, effect->pos.z);
    GXColor4u8(r, g, b, c0);
    GXPosition3f32(effect->unk8C.x, effect->unk8C.y, effect->unk8C.z);
    GXColor4u8(r, g, b, c1);
    GXPosition3f32(
        (effect->pos.x + effect->vel.x) / 2.0f,
        (effect->pos.y + effect->vel.y) / 2.0f,
        (effect->pos.z + effect->vel.z) / 2.0f);
    GXColor4u8(r, g, b, a0);
    GXPosition3f32(
        (effect->unk8C.x + effect->unk98.x) / 2.0f,
        (effect->unk8C.y + effect->unk98.y) / 2.0f,
        (effect->unk8C.z + effect->unk98.z) / 2.0f);
    GXColor4u8(r, g, b, a1);
    GXPosition3f32(effect->vel.x, effect->vel.y, effect->vel.z);
    GXColor4u8(r, g, b, c0);
    GXPosition3f32(effect->unk98.x, effect->unk98.y, effect->unk98.z);
    GXColor4u8(r, g, b, c1);
    GXSetCullMode_cached(2);
}

void fn_1_165DAC(int idx, Vec *a, Vec *b, u8 *color, u8 alpha, int life, int firstFrame)
{
    struct Effect effect;

    if (firstFrame)
    {
        lbl_1_data_79680[idx].a = *a;
        lbl_1_data_79680[idx].b = *b;
    }
    else
    {
        memset(&effect, 0, sizeof(effect));
        effect.type = 0x2C;
        effect.pos = *a;
        effect.vel = *b;
        effect.unk8C = lbl_1_data_79680[idx].a;
        effect.unk98 = lbl_1_data_79680[idx].b;
        effect.unk52 = color[0];
        effect.unk54 = color[1];
        effect.unk56 = color[2];
        effect.rotY = color[3];
        effect.rotZ = alpha;
        effect.rotX = (s16)life;
        spawn_effect(&effect);
        lbl_1_data_79680[idx].a = *a;
        lbl_1_data_79680[idx].b = *b;
    }
}

void fn_1_165F6C(struct Effect *effect)
{
    effect->timer = (rand() & 0x7FFF) % 16 + 0x38;
    effect->scale.x = 0.25f + 0.06f * ((float)rand() / 32767.0f);
    effect->vel.y = 0.02f + 0.006f * ((float)rand() / 32767.0f);
    effect->rotY = (rand() & 0x7FFF) % 0x4000;
}

void fn_1_166064(struct Effect *effect)
{
    if (effect->timer < 0x40)
    {
        effect->model = lbl_1_bss_DD970->modelEntries[7 - effect->timer / 8].model;
        effect->pos.y += effect->vel.y;
        effect->pos.x += math_sin(effect->rotY) * (0.01f + 0.01f * ((float)rand() / 32767.0f));
        effect->pos.z += mathutil_cos(effect->rotY) * (0.01f + 0.01f * ((float)rand() / 32767.0f));
        effect->rotY += (rand() & 0x7FFF) % 0x800;
        effect->scale.x *= 1.017f + 0.02f * ((float)rand() / 32767.0f);
    }
}

void fn_1_16620C(struct Effect *effect)
{
    if (effect->timer < 0x40)
    {
        mtxa_from_mtxb_translate(&effect->pos);
        mtxa_sq_from_identity();
        mtxa_scale_s(effect->scale.x);
        load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
        avdisp_set_alpha((float)effect->timer / 64.0f);
        avdisp_set_z_mode(1, 3, 0);
        avdisp_draw_model_unculled_sort_never(lbl_1_bss_DD970->modelEntries[7].model);
        avdisp_set_z_mode(1, 3, 1);
    }
}

void fn_1_1662DC(struct Effect *effect)
{
    float spread;
    float damp;

    effect->timer = (rand() & 0x7FFF) % 16 + 0x11;
    effect->scale.x = 0.05f;
    effect->scale.y = -effect->vel.y * (7.5f + 5.0f * ((float)rand() / 32767.0f));
    spread = 0.3f * mathutil_vec_dot_prod(&effect->vel, &effect->vel);
    damp = 0.2f + 0.4f * ((float)rand() / 32767.0f);
    effect->vel.x = effect->vel.x * damp + spread * ((float)rand() / 32767.0f) - spread / 2.0f;
    effect->vel.z = effect->vel.z * damp + spread * ((float)rand() / 32767.0f) - spread / 2.0f;
    effect->vel.y *= -0.4f + 0.2f * ((float)rand() / 32767.0f);
}

void fn_1_1664E8(struct Effect *effect)
{
    int models[] = { 0xF, 0x17, 0xB };

    effect->model = lbl_1_bss_DD970->modelEntries[models[effect->state] - effect->timer / 8].model;
    effect->pos.x += effect->vel.x;
    effect->pos.y += effect->vel.y;
    effect->pos.z += effect->vel.z;
    effect->vel.y -= 0.0028f;
    effect->vel.x *= 0.95f;
    effect->vel.y *= 0.95f;
    effect->vel.z *= 0.95f;
    effect->scale.x += (effect->scale.y - effect->scale.x) / 16.0f;
}

void fn_1_1665E0(struct Effect *effect)
{
    mtxa_from_mtxb_translate(&effect->pos);
    mtxa_sq_from_identity();
    mtxa_scale_s(effect->scale.x);
    load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
    avdisp_set_alpha((float)(effect->timer * effect->timer) / 1024.0f);
    avdisp_set_z_mode(1, 3, 0);
    avdisp_draw_model_unculled_sort_never(effect->model);
    avdisp_set_z_mode(1, 3, 1);
}

void fn_1_16669C(int kind, Vec *pos, Vec *vel)
{
    struct Effect effect;
    int i;

    memset(&effect, 0, sizeof(effect));
    effect.type = 0x2A;
    effect.state = (s16)kind;
    effect.pos = *pos;
    effect.vel = *vel;
    for (i = 0; i < (kind == 2 ? 2 : 4); i++)
        spawn_effect(&effect);
}

void fn_1_16675C(struct Effect *effect)
{
    effect->model = init_common_gma->modelEntries[lbl_1_data_796E0[(rand() & 0x7FFF) % (sizeof(lbl_1_data_796E0) / sizeof(lbl_1_data_796E0[0]))]].model;
    effect->timer = 60.0f * (3.5f + (float)rand() / 32767.0f);
    effect->rotX = rand() & 0x7FFF;
    effect->rotY = rand() & 0x7FFF;
    effect->rotZ = rand() & 0x7FFF;
    effect->state = 0;
}

void fn_1_166864(struct Effect *effect)
{
    Vec v;
    Vec up;
    Vec normal;
    float vn;
    float k;
    float d;
    float relX;
    float relY;
    float r;
    float relZ;
    float dist;
    float pen;
    Vec face;

    effect->vel.y -= 0.001f;
    if (effect->vel.y < 0.0f)
    {
        mtxa_from_rotate_y(effect->rotY);
        mtxa_rotate_x(effect->rotX);
        mtxa_rotate_z(effect->rotZ);
        v.x = 0.0f;
        v.y = 1.0f;
        v.z = 0.0f;
        mtxa_tf_vec(&v, &v);
        {
            float t = -0.7 * mathutil_vec_dot_prod(&v, &effect->vel);

            effect->vel.x += v.x * t;
            effect->vel.y += v.y * t;
            effect->vel.z += v.z * t;
        }
    }
    effect->vel.x *= 0.95;
    effect->vel.y *= 0.95;
    effect->vel.z *= 0.95;
    effect->pos.x += effect->vel.x;
    effect->pos.y += effect->vel.y;
    effect->pos.z += effect->vel.z;
    k = 2560.0f / effect->scale.x;
    effect->unk52 += (s16)(k * (effect->vel.x + effect->vel.y));
    effect->unk54 += (s16)(k * (effect->vel.y + effect->vel.z));
    effect->unk56 += (s16)(k * (effect->vel.z + effect->vel.x));
    effect->unk52 -= effect->unk52 >> 5;
    effect->unk54 -= effect->unk54 >> 5;
    effect->unk56 -= effect->unk56 >> 5;
    effect->rotX += effect->unk52;
    effect->rotY += effect->unk54;
    effect->rotZ += effect->unk56;
    if (effect->pos.y + effect->vel.y < 0.1f)
    {
        effect->state = 1;
        effect->unk64.x = effect->pos.x;
        effect->unk64.y = 0.0f;
        effect->unk64.z = effect->pos.z;
        effect->unk70.x = 0.0f;
        effect->unk70.y = 1.0f;
        effect->unk70.z = 0.0f;
    }
    switch (effect->state)
    {
    case 0:
        return;
    default:
        normal = effect->unk70;
        up.x = 0.0f;
        up.y = 1.0f;
        up.z = 0.0f;
        mtxa_from_rotate_y(effect->rotY);
        mtxa_rotate_x(effect->rotX);
        mtxa_rotate_z(effect->rotZ);
        mtxa_tf_vec(&up, &up);
        d = up.x * normal.x + up.y * normal.y + up.z * normal.z;
        r = 1.1920929e-7f * (float)effect->poolIndex + 0.01f * (1.0 - __fabs(d));
        relX = effect->pos.x - effect->unk64.x;
        relY = effect->pos.y - effect->unk64.y;
        relZ = effect->pos.z - effect->unk64.z;
        dist = normal.x * relX + normal.y * relY + normal.z * relZ;
        if (dist < 0.005f + r)
        {
            pen = (0.005f + r) - dist;
            effect->pos.x += pen * normal.x;
            effect->pos.y += pen * normal.y;
            effect->pos.z += pen * normal.z;
            vn = effect->vel.x * normal.x + effect->vel.y * normal.y + effect->vel.z * normal.z;
            if (vn < 0.0)
            {
                vn = -vn;
                effect->vel.x += vn * normal.x;
                effect->vel.y += vn * normal.y;
                effect->vel.z += vn * normal.z;
                {
                    float t = 0.125 * __fabs(d);

                    effect->vel.x += t * (effect->unk80.x - effect->vel.x);
                    effect->vel.y += t * (effect->unk80.y - effect->vel.y);
                    effect->vel.z += t * (effect->unk80.z - effect->vel.z);
                }
                vn *= 0.5;
                effect->vel.x += vn * normal.x;
                effect->vel.y += vn * normal.y;
                effect->vel.z += vn * normal.z;
            }
            face = normal;
            if (d < 0.0)
            {
                face.x = -face.x;
                face.y = -face.y;
                face.z = -face.z;
            }
            up.x = 0.05 * face.x + 0.95 * up.x;
            up.y = 0.05 * face.y + 0.95 * up.y;
            up.z = 0.05 * face.z + 0.95 * up.z;
            vec_normalize_len(&up);
            {
                s16 oldRotX = effect->rotX;
                s16 oldRotZ = effect->rotZ;

                mtxa_from_rotate_y(-effect->rotY);
                mtxa_rotate_x(0);
                mtxa_tf_vec(&up, &up);
                effect->rotX = math_atan2(up.z, up.y);
                effect->rotZ = -math_atan2(up.x, math_sqrt(mathutil_sum_of_sq_2(up.z, up.y)));
                effect->unk52 >>= 2;
                effect->unk52 += (effect->rotX - oldRotX) >> 2;
                effect->unk54 -= effect->unk54 >> 4;
                effect->unk56 >>= 2;
                effect->unk56 += (effect->rotZ - oldRotZ) >> 2;
            }
        }
        break;
    }
}

void fn_1_166F60(struct Effect *effect)
{
    mtxa_from_mtxb_translate(&effect->pos);
    mtxa_rotate_y(effect->rotY);
    mtxa_rotate_x(effect->rotX);
    mtxa_rotate_z(effect->rotZ);
    mtxa_scale_s(effect->scale.x);
    avdisp_set_scale_factor(effect->scale.x);
    load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
    if (effect->timer < 0xC)
    {
        avdisp_set_alpha((float)effect->timer / 12.0f);
        avdisp_draw_model_culled_sort_always(effect->model);
    }
    else
        avdisp_draw_model_culled_sort_auto(effect->model);
}

void fn_1_167028(Vec *pos, Vec *vel)
{
    struct Effect effect;
    float spread;

    memset(&effect, 0, sizeof(effect));
    effect.type = 0x27;
    effect.pos = *pos;
    effect.vel = *vel;
    spread = 0.35f * mathutil_vec_len(vel);
    effect.vel.x += spread * ((float)rand() / 32767.0f - 0.5f);
    effect.vel.y += spread * ((float)rand() / 32767.0f - 0.5f);
    effect.vel.z += spread * ((float)rand() / 32767.0f - 0.5f);
    effect.scale.x = 0.4f * (1.0f + (float)rand() / 32767.0f);
    spawn_effect(&effect);
}

void fn_1_1671F0(struct Effect *effect)
{
    effect->model = init_common_gma->modelEntries[37].model;
    effect->timer = 60.0 * (0.25 + 0.75 * ((float)rand() / 32767.0f));
    effect->flags = 2;
    effect->u_otherTimer = 0;
    effect->rotX = 0;
    effect->rotY = rand() & 0x7FFF;
    effect->rotZ = rand() & 0x7FFF;
    effect->unk52 = 0;
    effect->unk54 = (rand() & 0xFFF) + 0x1000;
    effect->unk56 = 0;
    effect->colorFactor = 0.0f;
    effect->prevPos = effect->pos;
    effect->scale.x = 0.0f;
    effect->scale.y = 0.012f;
    effect->state = 0;
}

void fn_1_167324(struct Effect *effect)
{
    Vec normal;
    Vec rel;
    float t;

    effect->vel.y -= 0.00040000002f;
    effect->vel.x *= 0.985f;
    effect->vel.y *= 0.985f;
    effect->vel.z *= 0.985f;
    effect->pos.x += effect->vel.x;
    effect->pos.y += effect->vel.y;
    effect->pos.z += effect->vel.z;
    effect->unk52 -= effect->unk52 >> 2;
    effect->unk54 -= effect->unk54 >> 5;
    effect->unk56 -= effect->unk56 >> 3;
    effect->rotX += effect->unk52;
    effect->rotY += effect->unk54;
    effect->rotZ += effect->unk56;
    if (effect->pos.y + effect->vel.y < 0.0f)
    {
        effect->state = 1;
        effect->unk64.x = effect->pos.x;
        effect->unk64.y = effect->pos.y;
        effect->unk64.z = effect->pos.z;
        effect->unk70.x = 0.0f;
        effect->unk70.y = 1.0f;
        effect->unk70.z = 0.0f;
    }
    if (effect->timer < 0x18)
    {
        float f = (float)effect->timer;

        effect->scale.x *= f / (1.0f + f);
    }
    else
        effect->scale.x += 0.3312f * (effect->scale.y - effect->scale.x);
    switch (effect->state)
    {
    case 0:
        if (effect->colorFactor > 0.0)
        {
            effect->colorFactor -= 0.0625;
            if (effect->colorFactor < 0.0)
                effect->colorFactor = 0.0f;
        }
        break;
    case 1:
        normal = effect->unk70;
        rel.x = effect->pos.x - effect->unk64.x;
        rel.y = effect->pos.y - effect->unk64.y;
        rel.z = effect->pos.z - effect->unk64.z;
        t = mathutil_vec_dot_prod(&normal, &rel);
        effect->colorFactor += 0.25 * (t - effect->colorFactor);
        effect->unk8C.x = effect->pos.x - (t - 0.02) * normal.x;
        effect->unk8C.y = effect->pos.y - (t - 0.02) * normal.y;
        effect->unk8C.z = effect->pos.z - (t - 0.02) * normal.z;
        vec_to_euler_xy(&normal, &effect->unkA4, &effect->unkA6);
        effect->unkA4 += 0x8000;
        if (t < 8.090365f * effect->scale.x)
        {
            t = 8.090365f * effect->scale.x - t;
            effect->pos.x += t * normal.x;
            effect->pos.y += t * normal.y;
            effect->pos.z += t * normal.z;
            t = mathutil_vec_dot_prod(&effect->vel, &normal);
            if (t < 0.0)
            {
                t *= -1.0;
                effect->vel.x += t * normal.x;
                effect->vel.y += t * normal.y;
                effect->vel.z += t * normal.z;
                effect->vel.x += 0.029999971f * (effect->unk80.x - effect->vel.x);
                effect->vel.y += 0.029999971f * (effect->unk80.y - effect->vel.y);
                effect->vel.z += 0.029999971f * (effect->unk80.z - effect->vel.z);
                effect->unk52 += 131072.0f * (t * ((float)rand() / 32767.0f - 0.5f));
                effect->unk54 += 524288.0f * (t * ((float)rand() / 32767.0f - 0.5f));
                effect->unk56 += 131072.0f * (t * ((float)rand() / 32767.0f - 0.5f));
                effect->vel.x += t * normal.x;
                effect->vel.y += t * normal.y;
                effect->vel.z += t * normal.z;
            }
        }
        break;
    }
}

void fn_1_167950(struct Effect *effect)
{
    Vec a;
    Vec b;
    Vec p;
    Vec scale;
    float len;
    float size;
    float bright;
    struct GMAModel *model;
    int angle;
    s16 rot;
    float k;

    model = effect->model;
    mtxa_from_mtxb_translate(&effect->pos);
    mtxa_rotate_y(effect->rotY);
    mtxa_rotate_x(effect->rotX);
    mtxa_rotate_z(effect->rotZ);
    mtxa_push();
    mtxa_scale_s(effect->scale.x);
    if (g_is_sphere_visible_scaled(&model->boundSphereCenter, model->boundSphereRadius, effect->scale.x))
    {
        load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
        avdisp_draw_model_unculled_sort_never(model);
    }
    mtxa_pop();
    mathutil_mtxA_get_translate(&a);
    mtxa_from_mtxb();
    mtxa_tf_point(&effect->prevPos, &p);
    if (a.z < -0.1f && p.z < -0.1f && effect->scale.x > 0.1f * effect->scale.y)
    {
        b = a;
        angle = 24576.0f * ((a.x + a.y) / a.z);
        bright = 0.6f * (0.5f + effect->scale.x * (0.5f / effect->scale.y));
        if (bright > 1.0f)
            bright = 1.0f;
        model = init_common_gma->modelEntries[36].model;
        k = (0.1f + a.z) / a.z;
        size = 0.4f * effect->scale.x * k;
        len = size * model->boundSphereRadius;
        k = (0.1f + a.z) / b.z;
        b.x *= k;
        b.y *= k;
        b.z = 0.1f + a.z;
        k = (0.1f + p.z) / p.z;
        p.x *= k;
        p.y *= k;
        p.z = 0.1f + p.z;
        a.x = b.x - p.x;
        a.y = b.y - p.y;
        a.z = 0.0f;
        if (0.0f == a.x && 0.0f == a.y)
        {
            rot = 0;
            scale.x = size;
            scale.y = size;
            scale.z = size;
        }
        else
        {
            vec_set_len(len, &a, &a);
            b.x += a.x;
            b.y += a.y;
            p.x -= a.x;
            p.y -= a.y;
            rot = math_atan2(a.y, a.x);
            {
                float ratio = 0.5f * mathutil_vec_distance(&b, &p) / len;

                scale.x = size * ratio;
                scale.y = size / ratio;
                scale.z = size;
            }
        }
        mtxa_from_translate_xyz(0.5f * (b.x + p.x), 0.5f * (b.y + p.y), b.z);
        mtxa_rotate_z(rot);
        mtxa_scale(&scale);
        mtxa_rotate_z(angle - rot);
        avdisp_set_scale_factor(scale.x);
        avdisp_set_z_mode(1, 3, 0);
        set_post_mult_color(bright, bright, bright, 1.0f);
        avdisp_draw_model_culled_sort_auto(model);
        fade_color_base_default();
        avdisp_set_z_mode(1, 3, 1);
    }
    if (effect->colorFactor > 0.0f && effect->colorFactor < 0.6f)
    {
        size = effect->scale.x * (1.0f / effect->scale.y);
        bright = 0.1f * ((0.6f - effect->colorFactor) * size);
        mtxa_from_mtxb_translate(&effect->unk8C);
        mtxa_rotate_y(effect->unkA6);
        mtxa_rotate_x(effect->unkA4);
        mtxa_scale_s(bright);
        size *= 1.0f - 0.083333336f * effect->colorFactor;
        if (size > 1.0f)
            set_post_mult_color(0.71999997f, 0.63f, 0.54f, 1.0f);
        else
            set_post_mult_color(0.71999997f * size, 0.63f * size, 0.54f * size, 1.0f);
        avdisp_set_z_mode(1, 3, 0);
        avdisp_set_scale_factor(bright);
        avdisp_draw_model_unculled_sort_auto(init_common_gma->modelEntries[94].model);
        avdisp_set_z_mode(1, 3, 1);
        fade_color_base_default();
    }
}

void fn_1_167E74(Vec *pos)
{
    struct Effect effect;
    int i;

    memset(&effect, 0, sizeof(effect));
    effect.type = 0x28;
    for (i = 0; i < 4; i++)
    {
        effect.pos = *pos;
        effect.pos.x += 0.5f * ((float)rand() / 32767.0f - 0.5f);
        effect.pos.y += 0.5f * ((float)rand() / 32767.0f - 0.5f);
        effect.pos.z += 0.5f * ((float)rand() / 32767.0f - 0.5f);
        effect.vel.x = 0.02f * ((float)rand() / 32767.0f) - 0.01f;
        effect.vel.y = 0.02f * ((float)rand() / 32767.0f) - 0.01f;
        effect.vel.z = 0.02f * ((float)rand() / 32767.0f) - 0.01f;
        spawn_effect(&effect);
    }
}

/* Unreferenced in the shipped build, but its literal stays in .rodata. */
float fn_1_168080_unused(void)
{
    return 16.0f;
}

void fn_1_168080(struct Effect *effect)
{
    effect->timer = 60.0 * (0.25 + 0.75 * ((float)rand() / 32767.0f));
    effect->flags = 2;
    effect->u_otherTimer = 0;
    effect->scale.x = 1.0f;
    effect->unk18 = 0.9f;
    effect->unk1C = 0.8f;
    effect->unk20 = 0.4f;
    effect->state = 0;
}

void fn_1_168140(struct Effect *effect)
{
    Vec n;
    Vec d;
    float dot;

    effect->vel.y -= 0.0012f;
    effect->vel.x *= 0.992f;
    effect->vel.y *= 0.992f;
    effect->vel.z *= 0.992f;
    effect->pos.x += effect->vel.x;
    effect->pos.y += effect->vel.y;
    effect->pos.z += effect->vel.z;
    if (effect->pos.y + effect->vel.y < 0.0f)
    {
        effect->state = 1;
        effect->unk64.x = effect->pos.x;
        effect->unk64.y = effect->pos.y;
        effect->unk64.z = effect->pos.z;
    }
    if (effect->timer < 24)
    {
        effect->unk18 *= 0.96;
        effect->unk1C *= 0.87;
        effect->unk20 *= 0.86;
        effect->scale.x = (float)effect->timer / 24.0;
    }
    switch (effect->state)
    {
    case 0:
        if (effect->colorFactor > 0.0)
        {
            effect->colorFactor -= 0.125;
            if (effect->colorFactor < 0.0)
                effect->colorFactor = 0.0f;
        }
        break;
    case 1:
        n = effect->unk70;
        d.x = effect->pos.x - effect->unk64.x;
        d.y = effect->pos.y - effect->unk64.y;
        d.z = effect->pos.z - effect->unk64.z;
        dot = mathutil_vec_dot_prod(&n, &d);
        effect->colorFactor += 0.25 * (dot - effect->colorFactor);
        effect->unk8C.x = effect->pos.x - (dot - 0.02) * n.x;
        effect->unk8C.y = effect->pos.y - (dot - 0.02) * n.y;
        effect->unk8C.z = effect->pos.z - (dot - 0.02) * n.z;
        vec_to_euler_xy(&n, &effect->unkA4, &effect->unkA6);
        effect->unkA4 += 0x8000;
        if (dot < 0.025)
        {
            dot = 0.025 - dot;
            effect->pos.x += dot * n.x;
            effect->pos.y += dot * n.y;
            effect->pos.z += dot * n.z;
            dot = mathutil_vec_dot_prod(&effect->vel, &n);
            if (dot < 0.0)
            {
                dot *= -1.0;
                effect->vel.x += dot * n.x;
                effect->vel.y += dot * n.y;
                effect->vel.z += dot * n.z;
                effect->vel.x += 0.03 * (effect->unk80.x - effect->vel.x);
                effect->vel.y += 0.03 * (effect->unk80.y - effect->vel.y);
                effect->vel.z += 0.03 * (effect->unk80.z - effect->vel.z);
                dot *= 0.9;
                effect->vel.x += dot * n.x;
                effect->vel.y += dot * n.y;
                effect->vel.z += dot * n.z;
            }
        }
        break;
    }
}

void fn_1_1685A8(struct Effect *effect)
{
    struct TrailVertex b;
    struct TrailVertex a;
    Vec prev;
    Vec mid;
    float radius;
    float width;
    float size;
    float bright;

    mtxa_from_mtxb();
    mtxa_tf_point(&effect->prevPos, &prev);
    mtxa_from_mtxb();
    if (effect->u_otherTimer == 0)
    {
        effect->u_otherTimer = 1;
        return;
    }
    mtxa_tf_point(&effect->pos, &a.pos);
    b.pos.x = 1.5 * prev.x - 0.5 * a.pos.x;
    b.pos.y = 1.5 * prev.y - 0.5 * a.pos.y;
    b.pos.z = 1.5 * prev.z - 0.5 * a.pos.z;
    mtxa_from_identity();
    radius = 0.5f * mathutil_vec_distance(&a.pos, &b.pos);
    mid.x = 0.5f * (a.pos.x + b.pos.y);
    mid.y = 0.5f * (a.pos.y + b.pos.y);
    mid.z = 0.5f * (a.pos.z + b.pos.z);
    if (g_is_sphere_visible(&mid, radius))
    {
        a.color.r = 255.0f * effect->unk18;
        a.color.g = 255.0f * effect->unk1C;
        a.color.b = 255.0f * effect->unk20;
        a.color.a = 0xFF;
        b.color.r = 96.0f * effect->unk18;
        b.color.g = 88.0f * effect->unk1C;
        b.color.b = 8.0f * effect->unk20;
        b.color.a = 0xFF;
        if (a.pos.z < -3.0)
            width = 1.5f;
        else if (a.pos.z > 0.0)
            width = 4.5f;
        else
            width = 1.5 + (3.0 + a.pos.z);
        fn_1_E2FD0(9.0f * width);
        fn_1_E2FE0(1, 1, 1, 0);
        fn_1_E349C(&a, &b);
    }
    if (effect->colorFactor > 0.0 && effect->colorFactor < 0.5)
    {
        bright = 0.5 / (1.0 + 2.0 * effect->colorFactor);
        size = 0.10000000149011612 * ((0.25 - 0.5 * effect->colorFactor) * effect->scale.x);
        mtxa_from_mtxb_translate(&effect->unk8C);
        mtxa_rotate_y(effect->unkA6);
        mtxa_rotate_x(effect->unkA4);
        mtxa_scale_s(size);
        avdisp_set_z_mode(1, 3, 0);
        avdisp_set_scale_factor(size);
        set_post_mult_color(effect->unk18 * bright, effect->unk1C * bright, effect->unk20 * bright, 1.0f);
        avdisp_draw_model_unculled_sort_auto(init_common_gma->modelEntries[94].model);
        avdisp_set_z_mode(1, 3, 1);
        fade_color_base_default();
    }
}

void fn_1_168968(Vec *pos, Vec *vel, Vec *normal)
{
    struct Effect effect;
    float speed;

    speed = 2.0f * mathutil_vec_len(vel);
    memset(&effect, 0, sizeof(effect));
    effect.type = 0x29;
    effect.pos = *pos;
    effect.unk70 = *normal;
    effect.vel.x = 0.4f * vel->x + speed * (normal->x + (2.0f * ((float)rand() / 32767.0f) - 1.0f)) / 10.0f;
    effect.vel.y = 0.4f * vel->y + speed * (normal->y + (2.0f * ((float)rand() / 32767.0f) - 1.0f)) / 10.0f;
    effect.vel.z = 0.4f * vel->z + speed * (normal->z + (2.0f * ((float)rand() / 32767.0f) - 1.0f)) / 10.0f;
    spawn_effect(&effect);
}

void fn_1_168B60(void)
{
    int i;
    struct Effect *effect;
    s8 *poolStatus;
    struct TrailVertex b;
    struct TrailVertex a;
    Vec pos;
    u8 alpha;
    float t;

    poolStatus = g_poolInfo.effectPool.statusList;
    effect = effects;
    for (i = g_poolInfo.effectPool.count; i > 0; i--, poolStatus++, effect++)
    {
        if (*poolStatus == 0)
            continue;
        switch (effect->type)
        {
        case 0x27:
            mtxa_from_mtxb_translate(&effect->pos);
            mtxa_rotate_y(effect->rotY);
            mtxa_rotate_x(effect->rotX);
            mtxa_rotate_z(effect->rotZ);
            mtxa_scale_s(effect->scale.x);
            avdisp_set_scale_factor(effect->scale.x);
            load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
            if (effect->timer < 12)
            {
                avdisp_set_alpha((float)effect->timer / 12.0f);
                avdisp_draw_model_culled_sort_always(effect->model);
            }
            else
                avdisp_draw_model_culled_sort_auto(effect->model);
            break;
        case 0x28:
            fn_1_167950(effect);
            break;
        case 0x29:
            fn_1_1685A8(effect);
            break;
        case 0x2A:
            mtxa_from_mtxb_translate(&effect->pos);
            mtxa_sq_from_identity();
            mtxa_scale_s(effect->scale.x);
            load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
            avdisp_set_alpha((float)(effect->timer * effect->timer) / 1024.0f);
            avdisp_set_z_mode(1, 3, 0);
            avdisp_draw_model_unculled_sort_never(effect->model);
            avdisp_set_z_mode(1, 3, 1);
            break;
        case 0x2B:
            if (effect->timer < 64)
            {
                mtxa_from_mtxb_translate(&effect->pos);
                mtxa_sq_from_identity();
                mtxa_scale_s(effect->scale.x);
                load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
                avdisp_set_alpha((float)effect->timer / 64.0f);
                avdisp_set_z_mode(1, 3, 0);
                avdisp_draw_model_unculled_sort_never(lbl_1_bss_DD970->modelEntries[7].model);
                avdisp_set_z_mode(1, 3, 1);
            }
            break;
        case 0x2C:
            fn_1_16599C(effect);
            break;
        case 0x2D:
            if (effect->scale.z > 0.0f)
            {
                mtxa_from_mtxb_translate(&effect->pos);
                mtxa_sq_from_identity();
                load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
                avdisp_set_alpha((float)effect->timer / effect->scale.z);
                avdisp_set_z_mode(1, 3, 0);
                avdisp_draw_model_unculled_sort_never(effect->model);
                avdisp_set_z_mode(1, 3, 1);
            }
            else
                fn_1_16527C(effect);
            break;
        case 0x2E:
            if (effect->timer <= 16)
            {
                GXSetBlendMode_cached(1, 4, 1, 0);
                avdisp_set_z_mode(1, 3, 0);
                mtxa_from_mtxb_translate(&effect->pos);
                mtxa_scale_s(0.001953125f * (effect->scale.x * (float)(256 - effect->timer * effect->timer)));
                if (effect->state < 16)
                {
                    mtxa_rotate_z(0x4000);
                    load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
                    avdisp_draw_model_unculled_sort_never(effect->model);
                }
                else
                {
                    mtxa_rotate_x(0x2000);
                    load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
                    avdisp_draw_model_unculled_sort_never(effect->model);
                    mtxa_rotate_x(0x4000);
                    load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
                    avdisp_draw_model_unculled_sort_never(effect->model);
                }
                avdisp_set_z_mode(1, 3, 1);
            }
            break;
        case 0x2F:
            if (effect->timer < 32)
            {
                mtxa_from_mtxb_translate(&effect->pos);
                mtxa_sq_from_identity();
                mtxa_scale_s(effect->scale.x * (float)(32 - effect->timer) / 16.0f);
                load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
                avdisp_set_alpha((float)effect->timer / 32.0f);
                avdisp_set_z_mode(1, 3, 0);
                avdisp_draw_model_unculled_sort_never(effect->model);
                avdisp_set_z_mode(1, 3, 1);
            }
            break;
        case 0x30:
            mtxa_from_mtxb();
            mtxa_tf_point(&effect->pos, &a.pos);
            mtxa_tf_point(&effect->prevPos, &b.pos);
            a.pos.x += 8.0f * (a.pos.x - b.pos.x);
            a.pos.y += 8.0f * (a.pos.y - b.pos.y);
            a.pos.z += 8.0f * (a.pos.z - b.pos.z);
            mtxa_from_identity();
            alpha = (effect->timer >= 16) ? 0xFF : (u8)(effect->timer * 0xFF / 16);
            a.color.r = 255.0f * effect->unk18;
            a.color.g = 255.0f * effect->unk1C;
            a.color.b = 255.0f * effect->unk20;
            a.color.a = alpha;
            b.color.r = 96.0f * effect->unk18;
            b.color.g = 88.0f * effect->unk1C;
            b.color.b = 8.0f * effect->unk20;
            b.color.a = (u32)alpha >> 1;
            fn_1_E2FD0(0x1C);
            fn_1_E2FE0(1, 4, 1, 0);
            fn_1_E349C(&a, &b);
            break;
        case 0x31:
            if (effect->timer < 20)
            {
                mtxa_from_mtxb_translate(&effect->pos);
                mtxa_rotate_y(0x4000);
                mtxa_rotate_z(-effect->timer * 0xC00);
                mtxa_scale_s(effect->scale.x * (float)(effect->timer * effect->timer) / 400.0f);
                load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
                if (effect->timer > 16)
                    avdisp_set_alpha((float)(20 - effect->timer) / 4.0f);
                avdisp_draw_model_unculled_sort_never(effect->model);
            }
            break;
        case 0x33:
            if (effect->timer < 20)
            {
                mtxa_from_mtxb_translate(&effect->pos);
                mtxa_scale_s(0.0025f * (effect->scale.x * (float)(400 - effect->timer * effect->timer)));
                mtxa_rotate_z(0x4000);
                load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
                avdisp_set_alpha((float)(effect->timer * effect->timer) / 256.0f);
                avdisp_set_z_mode(1, 3, 0);
                avdisp_draw_model_unculled_sort_never(lbl_1_bss_DD970->modelEntries[36].model);
                avdisp_set_z_mode(1, 3, 1);
                mtxa_from_mtxb_translate(&effect->pos);
                mtxa_scale_s(0.0025f * (effect->scale.x * (float)(400 - effect->timer * effect->timer)));
                mtxa_rotate_y(effect->rotY);
                mtxa_rotate_x(effect->rotX);
                mtxa_rotate_z(effect->rotZ);
                load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
                avdisp_set_alpha(effect->timer < 10 ? (float)effect->timer / 10.0f : 1.0f);
                avdisp_set_z_mode(1, 3, 0);
                avdisp_draw_model_unculled_sort_never(effect->model);
                avdisp_set_z_mode(1, 3, 1);
            }
            break;
        case 0x34:
            mtxa_from_mtxb_translate(&effect->pos);
            mtxa_scale_s(effect->scale.x * (1.0f + (float)(16 - effect->timer) / 32.0f));
            mtxa_rotate_z(0x4000);
            load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
            avdisp_set_z_mode(1, 3, 0);
            avdisp_set_alpha((float)((16 - effect->timer) * effect->timer) / 256.0f);
            avdisp_draw_model_unculled_sort_never(lbl_1_bss_DD970->modelEntries[33].model);
            avdisp_set_alpha((float)(effect->timer * effect->timer) / 256.0f);
            avdisp_draw_model_unculled_sort_never(lbl_1_bss_DD970->modelEntries[34].model);
            avdisp_set_z_mode(1, 3, 1);
            break;
        case 0x32:
            t = (float)(effect->timer * effect->timer) / 2304.0f;
            pos.x = effect->pos.x + effect->vel.x * t;
            pos.y = effect->pos.y + effect->vel.y * t;
            pos.z = effect->pos.z + effect->vel.z * t;
            mtxa_from_mtxb_translate(&pos);
            mtxa_sq_from_identity();
            mtxa_scale_s(3.5f * t);
            load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
            avdisp_set_alpha(1.0f);
            avdisp_set_z_mode(1, 3, 0);
            avdisp_draw_model_unculled_sort_never(effect->model);
            avdisp_set_z_mode(1, 3, 1);
            break;
        }
    }
}

void fn_1_169660(void)
{
    struct EffectFuncs funcs27 = { fn_1_16675C, fn_1_166864, fn_1_166F60, NULL };
    struct EffectFuncs funcs28 = { fn_1_1671F0, fn_1_167324, fn_1_167950, NULL };
    struct EffectFuncs funcs29 = { fn_1_168080, fn_1_168140, fn_1_1685A8, NULL };
    struct EffectFuncs funcs2A = { fn_1_1662DC, fn_1_1664E8, fn_1_1665E0, NULL };
    struct EffectFuncs funcs2B = { fn_1_165F6C, fn_1_166064, fn_1_16620C, NULL };
    struct EffectFuncs funcs2C = { fn_1_16598C, fn_1_165998, fn_1_16599C, NULL };
    struct EffectFuncs funcs2D = { fn_1_165268, fn_1_16526C, fn_1_165644, NULL };
    struct EffectFuncs funcs2E = { fn_1_163E6C, fn_1_163E70, fn_1_163EF4, NULL };
    struct EffectFuncs funcs2F = { fn_1_164030, fn_1_16404C, fn_1_1640E4, NULL };
    struct EffectFuncs funcs30 = { fn_1_1643A8, fn_1_1643D4, fn_1_164458, NULL };
    struct EffectFuncs funcs31 = { fn_1_164620, fn_1_16463C, fn_1_164640, NULL };
    struct EffectFuncs funcs33 = { fn_1_16474C, fn_1_1647BC, fn_1_1647C0, NULL };
    struct EffectFuncs funcs34 = { fn_1_1649B0, fn_1_1649BC, fn_1_164A38, NULL };
    struct EffectFuncs funcs32 = { fn_1_163B5C, fn_1_163C98, fn_1_163C9C, NULL };

    overwrite_effect_funcs(0x27, &funcs27);
    overwrite_effect_funcs(0x28, &funcs28);
    overwrite_effect_funcs(0x29, &funcs29);
    overwrite_effect_funcs(0x2A, &funcs2A);
    overwrite_effect_funcs(0x2B, &funcs2B);
    overwrite_effect_funcs(0x2C, &funcs2C);
    overwrite_effect_funcs(0x2D, &funcs2D);
    overwrite_effect_funcs(0x2E, &funcs2E);
    overwrite_effect_funcs(0x2F, &funcs2F);
    overwrite_effect_funcs(0x30, &funcs30);
    overwrite_effect_funcs(0x31, &funcs31);
    overwrite_effect_funcs(0x33, &funcs33);
    overwrite_effect_funcs(0x34, &funcs34);
    overwrite_effect_funcs(0x32, &funcs32);
    DVDChangeDir("/tennis");
    lbl_1_bss_DD978[0] = g_load_tpl("bt_effect.tpl");
    lbl_1_bss_DD970 = g_load_gma("bt_effect.gma", lbl_1_bss_DD978[0]);
    DVDChangeDir("/");
}

void fn_1_169940(void)
{
    g_something_freeing_heap(lbl_1_bss_DD970);
    g_something_freeing_heap_2(lbl_1_bss_DD978[0]);
}

void fn_1_16997C(void) {}
