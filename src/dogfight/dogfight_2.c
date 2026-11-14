/* Dogfight effects. */
#include "types.h"
#include "ape_internal.h"
#include "avdisp.h"
#include "ball.h"
#include "camera.h"
#include "dip.h"
#include "effect.h"
#include "frustum.h"
#include "libc.h"
#include "macros.h"
#include "mathutil.h"
#include "rand.h"

extern struct GMA *minigameGma[];
extern u32 g_some_render_flag;
extern float lbl_80198B34;

struct LineVert
{
    Vec pos;
    GXColor color;
};

void PSVECAdd(Vec *a, Vec *b, Vec *dst);
void PSVECScale(Vec *src, Vec *dst, float scale);

static inline float fn_17_scale(float k, Vec *pos)
{
    float inv;
    float height;
    float q;
    float d;

    height = 240.0f * g_current_camera->sub28.viewportH;
    inv = 1.0f / height;
    d = g_current_camera->sub28.view.tanHalfFov * mathutil_vec_distance(&g_current_camera->eye, pos);
    q = height / d;
    if (q < k) {
        return inv * (k * d);
    }
    return 1.0f;
}

static inline float fn_17_scale2(float k, Vec *pos)
{
    float inv;
    float height;
    float q;
    float d;
    float m = k;

    height = 240.0f * g_current_camera->sub28.viewportH;
    inv = 1.0f / height;
    d = g_current_camera->sub28.view.tanHalfFov * mathutil_vec_distance(&g_current_camera->eye, pos);
    q = height / d;
    if (q < m) {
        return inv * (m * d);
    }
    return 1.0f;
}

void fn_17_97C0(struct Effect *effect)
{
    float var_f0;
    float var_f1;

    if ((g_some_render_flag & 4) == 0) {
        mtxa_from_mtxb_translate(&effect->pos);
        mtxa_sq_from_identity();
        mtxa_scale_s(fn_17_scale2(2.0f, &effect->pos));
        mtxa_scale(&effect->scale);
        if (effect->playerId == g_current_camera->idx) {
            avdisp_set_alpha(0.7f);
        }
        if (effect->scale.y > effect->scale.z) {
            var_f0 = effect->scale.y;
        } else {
            var_f0 = effect->scale.z;
        }
        if (effect->scale.x > var_f0) {
            var_f1 = effect->scale.x;
        } else if (effect->scale.y > effect->scale.z) {
            var_f1 = effect->scale.y;
        } else {
            var_f1 = effect->scale.z;
        }
        avdisp_set_scale_factor(var_f1);
        avdisp_draw_model_culled_sort_always(effect->model);
    }
}

void fn_17_996C(void)
{

}

void fn_17_9970(struct Effect *effect)
{
    struct Ball *ball = current_ball;

    effect->u_otherTimer = 0x1E;
    effect->unk8C.x = effect->pos.x - ball->pos.x;
    effect->unk8C.y = effect->pos.y - ball->pos.y;
    effect->unk8C.z = effect->pos.z - ball->pos.z;
    effect->unk98.x = (0.5f * effect->scale.x);
    effect->unk98.y = (0.5f * effect->scale.y);
    effect->unk98.z = (0.5f * effect->scale.z);
    effect->unkA6 = 0x1000;
}

void fn_17_9A08(struct Effect *effect)
{
    struct Effect sp8;
    struct Ball *temp_r4_188;
    float temp_f1;
    float temp_f2;

    temp_r4_188 = &balls[effect->playerId];
    effect->u_otherTimer -= 1;
    if (effect->u_otherTimer == 0) {
        memcpy_handler(&sp8, effect, 0xB0U);
        sp8.type = 0x2A;
        sp8.timer = 0;
        sp8.playerId = effect->playerId;
        sp8.cameras = effect->cameras;
        spawn_effect(&sp8);
        effect->timer = 1;
        return;
    }
    temp_f2 = (30.0f - effect->u_otherTimer) / 30.0f;
    temp_f1 = 0.75f + 0.5f * temp_f2;
    effect->unk8C.x *= 0.80000001f;
    effect->unk8C.y += 0.2f * (temp_f1 - effect->unk8C.y);
    effect->unk8C.z *= 0.80000001f;
    effect->pos.x = (temp_r4_188->pos.x + effect->unk8C.x);
    effect->pos.y = (temp_r4_188->pos.y + effect->unk8C.y);
    effect->pos.z = (temp_r4_188->pos.z + effect->unk8C.z);
    effect->unk54 += ((effect->unkA6 - effect->unk54) >> 3);
    effect->rotY += effect->unk54;
    effect->scale.x += (0.2f * (effect->unk98.x - effect->scale.x));
    effect->scale.y += (0.2f * (effect->unk98.y - effect->scale.y));
    effect->scale.z += (0.2f * (effect->unk98.z - effect->scale.z));
}

void fn_17_9BE4(struct Effect *effect)
{
    mtxa_from_mtxb();
    mtxa_translate(&effect->pos);
    mtxa_rotate_y(effect->rotY);
    mtxa_rotate_x(effect->rotX);
    mtxa_rotate_z(effect->rotZ);
    mtxa_scale_xyz(effect->scale.x, effect->scale.y, effect->scale.z);
    load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
    avdisp_set_scale_factor(effect->scale.x);
    avdisp_draw_model_culled_sort_never(effect->model);
}

void fn_17_9C64(void)
{

}

void fn_17_9C68(struct Effect *effect)
{
    effect->timer = (60.0f * (0.25f + (0.125f * (rand() / 32767.0f))));
    effect->cameras = (u16) (1 << effect->playerId);
    mtxa_from_mtx(cameras[effect->playerId].unk14C);
    mtxa_tf_point(&effect->pos, &effect->pos);
    mtxa_tf_vec(&effect->vel, &effect->vel);
    mtxa_rotate_y(effect->rotY);
    mtxa_rotate_x(effect->rotX);
    mtxa_rotate_z(effect->rotZ);
    mtxa_to_euler_yxz(&effect->rotY, &effect->rotX, &effect->rotZ);
}

#ifdef __MWERKS__
struct DogProjectionConstants
{
    float targetX, targetY, centerX;
};
static const struct DogProjectionConstants dog_projection_constants = {
    120.0f, 70.0f, 320.0f
};
/* Seed the shared zero literal between the two projection tables. */
static inline float dog_projection_zero(void)
{
    return 0.0f;
}
static void *dog_projection_zero_anchor = (void *)dog_projection_zero;
struct DogProjectionScale
{
    float depth, scale;
};
static const struct DogProjectionScale dog_projection_scale = { -2.0f, 0.053333335f };
/* Fixed volatile FPRs preserve the projection and interpolation lifetimes.
 * Pointer and integer operands remain assigned by the compiler. */
void fn_17_9D60(register struct Effect *effect)
{
    register const char *constants;
    register struct GMAModel *model;
    register struct Camera *cam;
    register s32 scratch;
    union {
        double value;
        struct
        {
            u32 hi, lo;
        } words;
    } timerBits;

    asm {
        lis model, (dog_projection_constants - 0x40)@ha
        addi constants, model, (dog_projection_constants - 0x40)@l
    }
    if ((effect->flags & 0x10000000) != 0) {
        asm {
            lfs f3, 0x40(constants)
            lfs f5, 0x44(constants)
        }
    } else {
        asm {
            lfs f3, 0x48(constants)
            lfs f5, 0x4c(constants)
        }
    }
    cam = &cameras[effect->playerId];
    asm {
        lwz model, effect->model
        lfs f6, model->boundSphereRadius
        lfs f0, 0x50(constants)
        lfs f2, 0x0(constants)
        lfs f1, cam->sub28.view.tanHalfFov
        fmuls f1, f2, f1
        lfs f2, 0x48(constants)
        fsubs f4, f3, f2
        lfs f3, 0x4(constants)
        lfs f2, cam->sub28.viewportH
        fmuls f3, f3, f2
        lfs f2, cam->sub28.viewportW
        fdivs f2, f3, f2
        fdivs f3, f4, f2
        lfs f2, cam->sub28.view.unk28
        fsubs f2, f3, f2
        fmuls f4, f1, f2
        lfs f2, 0x4(constants)
        fsubs f2, f5, f2
        fneg f3, f2
        lfs f2, 0x4(constants)
        fdivs f3, f3, f2
        lfs f2, cam->sub28.view.unk2C
        fsubs f2, f3, f2
        fmuls f5, f1, f2
        lfs f2, 0x54(constants)
        fmuls f1, f2, f1
        fdivs f6, f1, f6
        lfs f3, 0x8(constants)
        lwz scratch, effect->timer
        lfd f2, 0x28(constants)
        xoris scratch, scratch, 0x8000
        stw scratch, timerBits.words.lo
        lis scratch, 0x4330
        stw scratch, timerBits.words.hi
        lfd f1, timerBits.value
        fsubs f1, f1, f2
        fdivs f1, f3, f1
        lfs f2, effect->pos.x
        fsubs f2, f4, f2
        fmuls f2, f1, f2
        stfs f2, effect->vel.x
        lfs f2, effect->pos.y
        fsubs f2, f5, f2
        fmuls f2, f1, f2
        stfs f2, effect->vel.y
        lfs f2, effect->pos.z
        fsubs f0, f0, f2
        fmuls f0, f1, f0
        stfs f0, effect->vel.z
        lfs f2, effect->pos.x
        lfs f0, effect->vel.x
        fadds f0, f2, f0
        stfs f0, effect->pos.x
        lfs f2, effect->pos.y
        lfs f0, effect->vel.y
        fadds f0, f2, f0
        stfs f0, effect->pos.y
        lfs f2, effect->pos.z
        lfs f0, effect->vel.z
        fadds f0, f2, f0
        stfs f0, effect->pos.z
        lfs f2, effect->scale.x
        lfs f0, effect->scale.x
        fsubs f0, f6, f0
        fmuls f0, f1, f0
        fadds f0, f2, f0
        stfs f0, effect->scale.x
        lfs f0, effect->scale.x
        stfs f0, effect->scale.y
        lfs f0, effect->scale.x
        stfs f0, effect->scale.z

    }
    effect->unk54 -= effect->unk54 >> 7;
    effect->rotX += effect->unk52;
    effect->rotY += effect->unk54;
    effect->rotZ += effect->unk56;
}
#else
static inline void fn_17_inl(struct Effect *effect, struct Camera *cam, float a, float b)
{
    float s;
    float z;
    float w;
    float x;
    float y;
    float rate;

    s = effect->model->boundSphereRadius;
    z = -2.0f;
    w = 2.0f;
    w *= cam->sub28.view.tanHalfFov;
    x = w * ((a - 320.0f) / ((240.0f * cam->sub28.viewportH) / cam->sub28.viewportW) - cam->sub28.view.unk28);
    y = w * (-(b - 240.0f) / 240.0f - cam->sub28.view.unk2C);
    s = (0.053333335f * w) / s;
    rate = 1.0f / effect->timer;
    effect->vel.x = rate * (x - effect->pos.x);
    effect->vel.y = rate * (y - effect->pos.y);
    effect->vel.z = rate * (z - effect->pos.z);
    effect->pos.x += effect->vel.x;
    effect->pos.y += effect->vel.y;
    effect->pos.z += effect->vel.z;
    effect->scale.x += rate * (s - effect->scale.x);
    effect->scale.y = effect->scale.x;
    effect->scale.z = effect->scale.x;
}

void fn_17_9D60(struct Effect *effect)
{
    float a;
    float b;
    struct Camera *cam;

    if ((effect->flags & 0x10000000) != 0) {
        a = 120.0f;
        b = 70.0f;
    } else {
        a = 320.0f;
        b = 0.0f;
    }
    cam = &cameras[effect->playerId];
    fn_17_inl(effect, cam, a, b);
    effect->unk54 -= effect->unk54 >> 7;
    effect->rotX += effect->unk52;
    effect->rotY += effect->unk54;
    effect->rotZ += effect->unk56;
}

#endif

void fn_17_9F14(struct Effect *effect)
{
    if ((g_some_render_flag & 4) == 0) {
        mtxa_from_translate(&effect->pos);
        mtxa_rotate_y(effect->rotY);
        mtxa_rotate_x(effect->rotX);
        mtxa_rotate_z(effect->rotZ);
        mtxa_scale_xyz(effect->scale.x, effect->scale.y, effect->scale.z);
        load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
        avdisp_set_scale_factor(effect->scale.x);
        avdisp_draw_model_culled_sort_never(effect->model);
    }
}

void fn_17_9FA8(void)
{

}

void fn_17_9FAC(struct Effect *effect)
{
    effect->flags = (effect->flags | 2);
    effect->timer = ((0.10000000000000001 * lbl_80198B34) + (0.29999999999999999 * lbl_80198B34 * (rand() / 32767.0f)));
    effect->u_otherTimer = 0;
    effect->scale.x = 1.0f;
    effect->cameras = (u16) (1 << effect->playerId);
    effect->u_otherTimer = 0;
}

void fn_17_A084(struct Effect *effect)
{
    PSVECScale(&effect->vel, &effect->vel, 0.99199998f);
    PSVECAdd(&effect->pos, &effect->vel, &effect->pos);
    if (effect->timer < 0xA) {
        effect->unk18 *= 0.95999999999999996;
        effect->unk1C *= 0.87;
        effect->unk20 *= 0.85999999999999999;
        effect->scale.x = (float)effect->timer / 24.0;
    }
    if (effect->colorFactor > 0.0) {
        effect->colorFactor -= 0.125;
        if (effect->colorFactor < 0.0) {
            effect->colorFactor = 0.0f;
        }
    }
}

void fn_17_A190(struct Effect *effect)
{
    struct LineVert v[2];
    Vec sp8;
    float var_f1_863;
    float temp_f1_787;

    if (effect->u_otherTimer == 0) {
        effect->u_otherTimer = 1;
        return;
    }
    if ((g_some_other_flags & 0xA) == 0) {
        mtxa_from_mtx(g_ord_tbl_stuff->matrices[3]);
        mtxa_tf_point(&effect->prevPos, &effect->unk8C);
        mtxa_from_mtxb();
        mtxa_tf_point(&effect->pos, &effect->unk98);
    }
    v[0].pos = effect->unk98;
    v[1].pos.x = 1.5 * effect->unk8C.x - 0.5 * v[0].pos.x;
    v[1].pos.y = 1.5 * effect->unk8C.y - 0.5 * v[0].pos.y;
    v[1].pos.z = 1.5 * effect->unk8C.z - 0.5 * v[0].pos.z;
    mtxa_from_identity();
    temp_f1_787 = 0.5 * mathutil_vec_distance(&v[0].pos, &v[1].pos);
    sp8.x = 0.5 * (v[0].pos.x + v[1].pos.y);
    sp8.y = 0.5 * (v[0].pos.y + v[1].pos.y);
    sp8.z = 0.5 * (v[0].pos.z + v[1].pos.z);
    if (g_is_sphere_visible(&sp8, temp_f1_787) != 0) {
        v[0].color.r = 255.0 * effect->unk18;
        v[0].color.g = 255.0 * effect->unk1C;
        v[0].color.b = 255.0 * effect->unk20;
        v[0].color.a = 0xFF;
        v[1].color.r = 96.0 * effect->unk18;
        v[1].color.g = 88.0 * effect->unk1C;
        v[1].color.b = 8.0 * effect->unk20;
        v[1].color.a = 0xFF;
        if (v[0].pos.z < -3.0) {
            var_f1_863 = 1.5f;
        } else if (v[0].pos.z > 0.0) {
            var_f1_863 = 4.5f;
        } else {
            var_f1_863 = 1.5 + (3.0 + v[0].pos.z);
        }
        fn_1_E2FD0(12.0 * var_f1_863);
        fn_1_E2FE0(1, 1, 1, 0);
        fn_1_E349C(&v[0], &v[1]);
    }
}

void fn_17_A494(void)
{

}

void fn_17_A498(struct Effect *effect)
{
    struct Ball *ball;

    ball = &balls[effect->playerId];
    effect->model = minigameGma[0]->modelEntries[0x6E].model;
    effect->timer = 0.2 * lbl_80198B34 + lbl_80198B34 * (0.4 + 0.1 * ((float)rand() / 32767.0f));
    effect->u_otherTimer = effect->timer;
    effect->pos = ball->pos;
    PSVECScale(&ball->vel, &effect->vel, 0.80000001f);
    effect->scale.y = 1.0 + 0.5 * ((float)rand() / 32767.0f);
    effect->rotZ = rand() & 0x7FFF;
}

void fn_17_A5F4(struct Effect *effect)
{
    PSVECAdd(&effect->pos, &effect->vel, &effect->pos);
    PSVECScale(&effect->vel, &effect->vel, 0.995 * (60.0f / lbl_80198B34));
    effect->scale.x = effect->scale.y * (1.0 - (float)effect->timer / (float)effect->u_otherTimer);
    if (effect->timer <= 0.2 * lbl_80198B34) {
        effect->colorFactor = (float)effect->timer / (0.2 * lbl_80198B34);
        return;
    }
    effect->colorFactor = 1.0f;
}

void fn_17_A750(struct Effect *effect)
{
    float scale = 12.0f;

    if ((g_some_render_flag & 4) == 0) {
        mtxa_from_mtxb_translate(&effect->pos);
        mtxa_sq_from_identity();
        mtxa_rotate_z(effect->rotZ);
        scale = fn_17_scale(scale, &effect->pos);
        scale *= effect->scale.x;
        mtxa_scale_s(scale);
        avdisp_set_alpha(effect->colorFactor);
        avdisp_set_scale_factor(effect->scale.x * scale);
        avdisp_draw_model_culled_sort_always(effect->model);
    }
}

void fn_17_A8A8(void)
{

}
