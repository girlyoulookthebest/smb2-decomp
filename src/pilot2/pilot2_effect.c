/* Effects for the Monkey Target 2 minigame. */
#include "types.h"
#include "avdisp.h"
#include "ball.h"
#include "camera.h"
#include "effect.h"
#include "libc.h"
#include "macros.h"
#include "mathutil.h"
#include "pool.h"
#include "rand.h"

extern struct GMA *minigameGma[];
extern u32 g_some_render_flag;

static void fn_13_A8DC(struct Effect *effect);
static void fn_13_A8F0(struct Effect *effect);
static void fn_13_A95C(struct Effect *effect);
static void fn_13_AA08(struct Effect *effect);
static void fn_13_AA0C(struct Effect *effect);
static void fn_13_AAA4(struct Effect *effect);
static void fn_13_AC80(struct Effect *effect);
static void fn_13_AD00(struct Effect *effect);
static void fn_13_AD04(struct Effect *effect);
static void fn_13_ADFC(struct Effect *effect);
static void fn_13_AF88(struct Effect *effect);
static void fn_13_B01C(struct Effect *effect);

static struct EffectFuncs lbl_13_data_E90[3] = {
    { fn_13_A8DC, fn_13_A8F0, fn_13_A95C, fn_13_AA08 },
    { fn_13_AA0C, fn_13_AAA4, fn_13_AC80, fn_13_AD00 },
    { fn_13_AD04, fn_13_ADFC, fn_13_AF88, fn_13_B01C },
};

static s16 lbl_13_data_EC0[40] = {
    3,  4,  5,  6,  7,  8,  9,  10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22,
    23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 0,
};

void fn_13_A88C(void)
{
    int i;

    for (i = 0; i < 3; i++)
        overwrite_effect_funcs(i + 0x27, &lbl_13_data_E90[i]);
}

static void fn_13_A8DC(struct Effect *effect)
{
    effect->u_otherTimer = -1;
    effect->timer = 100;
}

static void fn_13_A8F0(struct Effect *effect)
{
    effect->u_otherTimer++;
    if ((u32)effect->u_otherTimer < 39U)
    {
        effect->model = minigameGma[0]->modelEntries[lbl_13_data_EC0[effect->u_otherTimer]].model;
        return;
    }
    g_poolInfo.effectPool.statusList[effect->poolIndex] = 3;
}

static void fn_13_A95C(struct Effect *effect)
{
    if ((g_some_render_flag & 4) == 0)
    {
        mtxa_from_mtxb_translate(&effect->pos);
        mtxa_sq_from_identity();
        mtxa_scale(&effect->scale);
        avdisp_set_scale_factor(MAX(effect->scale.x, MAX(effect->scale.y, effect->scale.z)));
        avdisp_draw_model_culled_sort_auto(effect->model);
    }
}

static void fn_13_AA08(struct Effect *effect) {}

static void fn_13_AA0C(struct Effect *effect)
{
    struct Ball *ball = current_ball;

    effect->u_otherTimer = 30;
    effect->unk8C.x = effect->pos.x - ball->pos.x;
    effect->unk8C.y = effect->pos.y - ball->pos.y;
    effect->unk8C.z = effect->pos.z - ball->pos.z;
    effect->unk98.x = 0.5f * effect->scale.x;
    effect->unk98.y = 0.5f * effect->scale.y;
    effect->unk98.z = 0.5f * effect->scale.z;
    effect->unkA6 = 0x1000;
}

static void fn_13_AAA4(struct Effect *effect)
{
    struct Ball *ball = &balls[effect->playerId];
    float t;
    float y;

    effect->u_otherTimer--;
    if (effect->u_otherTimer == 0)
    {
        struct Effect spawned;

        memcpy_handler(&spawned, effect, sizeof(spawned));
        spawned.type = 0x29;
        spawned.timer = 0;
        spawned.playerId = effect->playerId;
        spawned.cameras = effect->cameras;
        spawn_effect(&spawned);
        effect->timer = 1;
        return;
    }
    t = (30.0f - effect->u_otherTimer) / 30.0f;
    y = 0.75f + 0.5f * t;
    effect->unk8C.x *= 0.8f;
    effect->unk8C.y += 0.2f * (y - effect->unk8C.y);
    effect->unk8C.z *= 0.8f;
    effect->pos.x = ball->pos.x + effect->unk8C.x;
    effect->pos.y = ball->pos.y + effect->unk8C.y;
    effect->pos.z = ball->pos.z + effect->unk8C.z;
    effect->unk54 += (effect->unkA6 - effect->unk54) >> 3;
    effect->rotY += effect->unk54;
    effect->scale.x += 0.2f * (effect->unk98.x - effect->scale.x);
    effect->scale.y += 0.2f * (effect->unk98.y - effect->scale.y);
    effect->scale.z += 0.2f * (effect->unk98.z - effect->scale.z);
}

static void fn_13_AC80(struct Effect *effect)
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

static void fn_13_AD00(struct Effect *effect) {}

static void fn_13_AD04(struct Effect *effect)
{
    effect->timer = 60.0f * (0.25f + 0.125f * (rand() / 32767.0f));
    effect->cameras = 1 << effect->playerId;
    mtxa_from_mtx(cameras[effect->playerId].unk14C);
    mtxa_tf_point(&effect->pos, &effect->pos);
    mtxa_tf_vec(&effect->vel, &effect->vel);
    mtxa_rotate_y(effect->rotY);
    mtxa_rotate_x(effect->rotX);
    mtxa_rotate_z(effect->rotZ);
    mtxa_to_euler_yxz(&effect->rotY, &effect->rotX, &effect->rotZ);
}

static void fn_13_ADFC(struct Effect *effect)
{
    struct Camera *camera;
    float targetZ;
    float targetX;
    float targetY;
    float radius;
    float scale;
    float targetS;
    float x;

    if (effect->flags & 0x10000000)
        x = 0.0f;
    else
        x = 640.0f;
    camera = &cameras[effect->playerId];
    radius = effect->model->boundSphereRadius;
    targetZ = -2.0f;
    scale = 2.0f * camera->sub28.view.tanHalfFov;
    x = (x - 320.0f) / 240.0f;
    targetX = scale * (x - camera->sub28.view.unk28);
    targetY = (0.9f - camera->sub28.view.unk2C) * scale;
    scale = 0.053333335f * scale;
    targetS = scale / radius;
    scale = 1.0f / effect->timer;
    effect->vel.x = scale * (targetX - effect->pos.x);
    effect->vel.y = scale * (targetY - effect->pos.y);
    effect->vel.z = scale * (targetZ - effect->pos.z);
    effect->pos.x += effect->vel.x;
    effect->pos.y += effect->vel.y;
    effect->pos.z += effect->vel.z;
    effect->scale.x += scale * (targetS - effect->scale.x);
    effect->scale.y = effect->scale.x;
    effect->scale.z = effect->scale.x;
    effect->unk54 -= effect->unk54 >> 7;
    effect->rotX += effect->unk52;
    effect->rotY += effect->unk54;
    effect->rotZ += effect->unk56;
}

static void fn_13_AF88(struct Effect *effect)
{
    if ((g_some_render_flag & 4) == 0)
    {
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

static void fn_13_B01C(struct Effect *effect) {}
