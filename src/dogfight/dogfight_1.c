/* Dogfight cameras: the per-mode tick table and the two puff effects. */
#include "types.h"
#include "ape_internal.h"
#include "avdisp.h"
#include "ball.h"
#include "camera.h"
#include "dip.h"
#include "effect.h"
#include "mathutil.h"
#include "mode.h"
#include "pool.h"
#include "rand.h"
#include "stage.h"

struct Keyframe
{
    s32 frame;
    float value;
    float tangentIn;
    float tangentOut;
};

struct CurveSet
{
    struct Keyframe **curves;
    s8 count;
};

struct ViewOffset
{
    Vec a;
    Vec b;
};

struct DogfightPlayer
{
    u32 flags;
    u8 filler4[8];
    s32 timer;
    float unk10;
    u8 filler14[0x9C];
};

struct DogfightSpawn
{
    u8 filler0[0xC];
    s16 unkC;
    u8 fillerE[2];
};

extern struct GMA *minigameGma[];
extern u32 g_some_render_flag;
extern float lbl_80198B34;
extern s8 lbl_1_bss_D4D2D;
extern s16 lbl_17_bss_10;
extern s16 lbl_17_bss_14;
extern s8 lbl_17_bss_28;
extern struct DogfightPlayer lbl_17_bss_48[];
extern struct DogfightSpawn *lbl_17_bss_344;
extern s32 lbl_17_bss_350;
void PSVECAdd(Vec *a, Vec *b, Vec *dst);
void PSVECSubtract(Vec *a, Vec *b, Vec *dst);
float fn_1_D2550(float time, struct Keyframe *frames);

void fn_17_7790(struct Camera *camera, struct Ball *ball);
void fn_17_79C4(struct Camera *camera, struct Ball *ball);
void fn_17_7C0C(struct Camera *camera, struct Ball *ball);
void fn_17_8314(struct Camera *camera, struct Ball *ball);
void fn_17_833C(struct Camera *camera, struct Ball *ball);
void fn_17_8660(struct Camera *camera, struct Ball *ball);
void fn_17_8958(struct Camera *camera, struct Ball *ball);
void fn_17_8AF4(struct Camera *camera, struct Ball *ball);
void fn_17_8CEC(struct Camera *camera, struct Ball *ball);
void fn_17_8CF0(struct Camera *camera, struct Ball *ball);
void fn_17_8EAC(struct Camera *camera, struct Ball *ball);
void fn_17_93E4(struct Ball *ball, struct ViewOffset *offs, Vec *eye, Vec *lookAt);
void fn_17_94C8(struct Keyframe *frames, int count);
void fn_17_95A4(struct Effect *effect);
void fn_17_95B8(struct Effect *effect);
void fn_17_9658(struct Effect *effect);
void fn_17_9704(struct Effect *effect);
void fn_17_9708(struct Effect *effect);
void fn_17_971C(struct Effect *effect);
void fn_17_97C0(struct Effect *effect);
void fn_17_996C(struct Effect *effect);
void fn_17_9970(struct Effect *effect);
void fn_17_9A08(struct Effect *effect);
void fn_17_9BE4(struct Effect *effect);
void fn_17_9C64(struct Effect *effect);
void fn_17_9C68(struct Effect *effect);
void fn_17_9D60(struct Effect *effect);
void fn_17_9F14(struct Effect *effect);
void fn_17_9FA8(struct Effect *effect);
void fn_17_9FAC(struct Effect *effect);
void fn_17_A084(struct Effect *effect);
void fn_17_A190(struct Effect *effect);
void fn_17_A494(struct Effect *effect);
void fn_17_A498(struct Effect *effect);
void fn_17_A5F4(struct Effect *effect);
void fn_17_A750(struct Effect *effect);
void fn_17_A8A8(struct Effect *effect);

void (*lbl_17_data_B28[])(struct Camera *, struct Ball *) =
{
    fn_17_7790,
    fn_17_8660,
    fn_17_8958,
    fn_17_79C4,
    fn_17_7C0C,
    fn_17_8314,
    fn_17_833C,
    fn_17_8AF4,
    fn_17_8CEC,
    fn_17_8CF0,
    fn_17_8EAC,
};

static struct Keyframe lbl_17_data_B54[] =
{
    {0, -1423.0f, 0.0f, 0.0f},
    {240, -228.0f, 0.0f, 0.0f},
};

static struct Keyframe lbl_17_data_B74[] =
{
    {0, 731.0f, 0.0f, 0.0f},
    {240, 216.0f, 0.0f, 0.0f},
};

static struct Keyframe lbl_17_data_B94[] =
{
    {0, 1441.0f, 0.0f, 0.0f},
    {240, -65.0f, 0.0f, 0.0f},
};

static struct Keyframe lbl_17_data_BB4[] =
{
    {0, 41.0f, 0.0f, 0.0f},
    {240, 33.0f, 0.0f, 0.0f},
};

static struct Keyframe lbl_17_data_BD4[] =
{
    {0, 28.0f, 0.0f, 0.0f},
    {240, 18.0f, 0.0f, 0.0f},
};

static struct Keyframe lbl_17_data_BF4[] =
{
    {0, 0.0f, 0.0f, 0.0f},
    {240, 0.0f, 0.0f, 0.0f},
};

static struct Keyframe lbl_17_data_C14[] =
{
    {0, -38.0f, 0.0f, 0.0f},
    {240, 196.0f, 0.0f, 0.0f},
};

static struct Keyframe lbl_17_data_C34[] =
{
    {0, 847.0f, 0.0f, 0.0f},
    {240, 429.0f, 0.0f, 0.0f},
};

static struct Keyframe lbl_17_data_C54[] =
{
    {0, 932.0f, 0.0f, 0.0f},
    {240, 68.0f, 0.0f, 0.0f},
};

static struct Keyframe lbl_17_data_C74[] =
{
    {0, -11.0f, 0.0f, 0.0f},
    {240, 0.0f, 0.0f, 0.0f},
};

static struct Keyframe lbl_17_data_C94[] =
{
    {0, 603.0f, 0.0f, 0.0f},
    {240, 546.0f, 0.0f, 0.0f},
};

static struct Keyframe lbl_17_data_CB4[] =
{
    {0, 0.0f, 0.0f, 0.0f},
    {240, -10.0f, 0.0f, 0.0f},
};

static struct Keyframe lbl_17_data_CD4[] =
{
    {0, -490.0f, 0.0f, 0.0f},
    {240, 35.0f, 0.0f, 0.0f},
};

static struct Keyframe lbl_17_data_CF4[] =
{
    {0, 857.0f, 0.0f, 0.0f},
    {240, 300.0f, 0.0f, 0.0f},
};

static struct Keyframe lbl_17_data_D14[] =
{
    {0, 245.0f, 0.0f, 0.0f},
    {240, 346.0f, 0.0f, 0.0f},
};

static struct Keyframe lbl_17_data_D34[] =
{
    {0, -47.0f, 0.0f, 0.0f},
    {240, -24.0f, 0.0f, 0.0f},
};

static struct Keyframe lbl_17_data_D54[] =
{
    {0, 497.0f, 0.0f, 0.0f},
    {240, 609.0f, 0.0f, 0.0f},
};

static struct Keyframe lbl_17_data_D74[] =
{
    {0, 0.0f, 0.0f, 0.0f},
    {240, 0.0f, 0.0f, 0.0f},
};

static struct Keyframe *lbl_17_data_D94[] = {lbl_17_data_B54, lbl_17_data_B74, lbl_17_data_B94};

static struct Keyframe *lbl_17_data_DA0[] = {lbl_17_data_BB4, lbl_17_data_BD4, lbl_17_data_BF4};

static struct Keyframe *lbl_17_data_DAC[] = {lbl_17_data_C14, lbl_17_data_C34, lbl_17_data_C54};

static struct Keyframe *lbl_17_data_DB8[] = {lbl_17_data_C74, lbl_17_data_C94, lbl_17_data_CB4};

static struct Keyframe *lbl_17_data_DC4[] = {lbl_17_data_CD4, lbl_17_data_CF4, lbl_17_data_D14};

static struct Keyframe *lbl_17_data_DD0[] = {lbl_17_data_D34, lbl_17_data_D54, lbl_17_data_D74};

static struct CurveSet lbl_17_data_DDC[] =
{
    {lbl_17_data_D94, 2},
    {lbl_17_data_DAC, 2},
    {lbl_17_data_DC4, 2},
};

static struct CurveSet lbl_17_data_DF4[] =
{
    {lbl_17_data_DA0, 2},
    {lbl_17_data_DB8, 2},
    {lbl_17_data_DD0, 2},
};

struct ViewOffset lbl_17_data_E0C[] =
{
    {{0.30000001f, -0.30000001f, 0.60000002f}, {0.30000001f, -0.30000001f, -20.0f}},
    {{0.0f, -0.5f, -2.0f}, {0.0f, 0.0f, 0.0f}},
    {{1.8f, 1.0f, 0.0f}, {0.0f, -0.2f, 0.0f}},
    {{-1.8f, 1.0f, 0.0f}, {0.0f, -0.2f, 0.0f}},
    {{2.5f, -1.0f, -2.5f}, {0.0f, 0.0f, 0.0f}},
};

struct EffectFuncs lbl_17_data_E88[] __attribute__((aligned(8))) =
{
    {fn_17_95A4, fn_17_95B8, fn_17_9658, fn_17_9704},
    {fn_17_9708, fn_17_971C, fn_17_97C0, fn_17_996C},
    {fn_17_9970, fn_17_9A08, fn_17_9BE4, fn_17_9C64},
    {fn_17_9C68, fn_17_9D60, fn_17_9F14, fn_17_9FA8},
    {fn_17_9FAC, fn_17_A084, fn_17_A190, fn_17_A494},
    {fn_17_A498, fn_17_A5F4, fn_17_A750, fn_17_A8A8},
};

static s16 lbl_17_data_EE8[] =
{
    4, 5, 6, 7, 8, 9, 10, 11, 12, 13,
    14, 15, 16, 17, 18, 19, 20, 21, 22, 23,
    24, 25, 26, 27, 28, 29, 30, 31, 32, 33,
    34, 35, 36, 37, 38, 39, 40, 41, 42, 0,
};

static s16 lbl_17_data_F38[] =
{
    46, 47, 48, 49, 50, 51, 52, 53, 54, 55,
    56, 57, 58, 59, 60, 61, 62, 63, 64, 65,
    66, 67, 68, 69,
};

Vec lbl_17_bss_380;
float lbl_17_bss_38C[4];
float lbl_17_bss_39C[4];
s8 lbl_17_bss_3AC;
s8 lbl_17_bss_3AD;
s16 lbl_17_bss_3AE;
struct ViewOffset *lbl_17_bss_3B0[4];

void fn_17_7750(struct Camera *camera, struct Ball *ball)
{
    lbl_17_data_B28[camera->unk1F](camera, ball);
}

void fn_17_7790(struct Camera *camera, struct Ball *ball)
{
    Vec w;

    camera_clear(camera);
    {
        Vec zero = {0.0f, 0.0f, 0.0f};
        Vec *p;

        p = &zero;
        lbl_17_bss_380 = *p;
    }
    lbl_17_bss_38C[ball->playerId] = 1.8f;
    lbl_17_bss_39C[camera->idx] = 0.0f;
    camera->lookAt.x = ball->pos.x;
    camera->lookAt.y = ball->pos.y + lbl_17_bss_39C[camera->idx];
    camera->lookAt.z = ball->pos.z;
    mtxa_from_translate(&camera->lookAt);
    if (lbl_17_bss_14 < 0xD) {
        mtxa_rotate_y(lbl_17_bss_344[ball->playerId].unkC - 0x8000);
    } else {
        mtxa_rotate_y(lbl_17_bss_344[ball->playerId].unkC);
    }
    w.x = 0.0f;
    w.y = 1.0f;
    w.z = lbl_17_bss_38C[ball->playerId];
    mtxa_tf_point(&w, &camera->eye);
    w.x = 0.0f;
    w.y = 0.0f;
    w.z = lbl_17_bss_38C[ball->playerId];
    mtxa_tf_point(&w, &camera->unkB4);
    w.x = camera->lookAt.x - camera->eye.x;
    w.y = camera->lookAt.y - camera->eye.y;
    w.z = camera->lookAt.z - camera->eye.z;
    camera->rotY = math_atan2(w.x, w.z) - 0x8000;
    camera->rotX = math_atan2(w.y, math_sqrt(mathutil_sum_of_sq_2(w.x, w.z)));
    camera->rotZ = 0;
}

void fn_17_79C4(struct Camera *camera, struct Ball *ball)
{
    fn_17_7790(camera, ball);
    camera->unk1F = 4;
    fn_17_7C0C(camera, ball);
}

void fn_17_7C0C(struct Camera *camera, struct Ball *ball)
{
    Vec diff;
    Vec oldEye;
    Vec oldLookAt;
    float target;
    float zoom;
    float t;
    s16 pitch;
    s16 yaw;
    int flag;
    struct DogfightPlayer *player;
    s16 roll;
    s16 heading;
    int delta;
    int delta3;

    player = &lbl_17_bss_48[ball->playerId];
    if ((g_some_other_flags & 0xA) == 0) {
        if (lbl_17_bss_14 < 0xB) {
            flag = 0;
        } else {
            if (camera->sub28.view.fov >= 0x2700) {
                target = 0.75f;
            } else {
                target = 0.6f;
            }
            zoom = 3.1f;
            lbl_17_bss_38C[ball->playerId] += 0.02f * (zoom - lbl_17_bss_38C[ball->playerId]);
            lbl_17_bss_39C[camera->idx] += 0.05f * (target - lbl_17_bss_39C[camera->idx]);
            flag = 1;
        }
        oldEye = camera->eye;
        oldLookAt = camera->lookAt;
        if ((player->flags & 8) != 0 && (player->flags & 0x200) == 0) {
            lbl_17_bss_38C[ball->playerId] += 5.0f / lbl_80198B34;
        }
        PSVECSubtract(&camera->unkB4, &camera->lookAt, &diff);
        diff.x += camera->lookAt.x;
        diff.y += camera->lookAt.y;
        diff.z += camera->lookAt.z;
        if (flag == 0) {
            PSVECSubtract(&camera->lookAt, &diff, &diff);
        }
        mtxa_from_mtx(ball->unk30);
        mtxa_tf_point(&lbl_17_bss_380, &camera->lookAt);
        camera->lookAt.y += lbl_17_bss_39C[camera->idx];
        if (flag != 0) {
            PSVECSubtract(&camera->lookAt, &diff, &diff);
        }
        pitch = math_atan2(diff.y, math_sqrt(mathutil_sum_of_sq_2(diff.x, diff.z)));
        if ((player->flags & 8) == 0) {
            if (pitch < -0x3800) {
                pitch = -0x3800;
            } else if (pitch > 0x2800) {
                pitch = 0x2800;
            }
            camera->unkC0 *= 0.98f;
        }
        pitch = camera->unkC0 += 0.2f * (pitch - camera->unkC0);
        lbl_17_bss_380.z += 0.1f * -lbl_17_bss_380.z;
        yaw = math_atan2(diff.x, diff.z) - 0x8000;
        delta = (s16)(yaw - camera->rotY);
        delta *= 0.6f;
        delta = delta < -0x1A0 ? -0x1A0 : (delta > 0x1A0 ? 0x1A0 : delta);
        yaw = camera->rotY + delta;
        if ((player->flags & 8) == 0) {
            if (lbl_17_bss_14 < 0xB) {
                t = (10.0 * lbl_80198B34 - (float)player->timer) / (10.0f * lbl_80198B34);
                if (t < 0.0f) {
                    t = 0.0f;
                }
                heading = ball->unk92 - (int)(32768.0f * t);
            } else {
                heading = ball->unk92;
            }
            delta = (s16)(heading - yaw);
            yaw += delta >> 3;
        }
        delta = (s16)(yaw - camera->rotY);
        delta = delta < -0x300 ? -0x300 : (delta > 0x300 ? 0x300 : delta);
        yaw = camera->rotY + delta;
        if ((player->flags & 8) == 0) {
            roll = ball->rotZ;
            if (roll > 0x4000) {
                roll = 0x8000 - roll;
            } else if (roll < -0x4000) {
                roll = -0x8000 - roll;
            }
            roll = 0.025f * (roll * player->unk10);
            delta3 = (s16)(0.75f * roll) - camera->rotZ;
            camera->rotZ += (s16)(0.04f * delta3);
        } else {
            camera->rotZ -= 0.05f * camera->rotZ;
        }
        mtxa_from_translate(&camera->lookAt);
        mtxa_rotate_y(yaw);
        mtxa_rotate_x(pitch);
        diff.x = 0.0f;
        diff.y = 0.0f;
        diff.z = lbl_17_bss_38C[ball->playerId];
        mtxa_tf_point(&diff, &camera->unkB4);
        camera->rotY = yaw;
        camera->rotX = pitch - 0x200;
        mtxa_from_translate(&camera->lookAt);
        mtxa_rotate_y(camera->rotY);
        mtxa_rotate_x(camera->rotX);
        diff.x = 0.0f;
        diff.y = 0.0f;
        diff.z = lbl_17_bss_38C[ball->playerId];
        mtxa_tf_point(&diff, &camera->eye);
        PSVECSubtract(&camera->eye, &oldEye, &camera->eyeVel);
        PSVECSubtract(&camera->lookAt, &oldLookAt, &camera->lookAtVel);
    }
}

void fn_17_8314(struct Camera *camera, struct Ball *ball)
{
    camera->unk1F = 6;
    fn_17_833C(camera, ball);
}

void fn_17_833C(struct Camera *camera, struct Ball *ball)
{
    Vec v;
    float target;

    if ((g_some_other_flags & 0xA) == 0) {
        switch (ball->ape->charaId) {
        case 3:
            target = 1.8f;
            break;
        default:
            target = 1.6f;
            break;
        case 2:
            target = 1.4f;
            break;
        }
        lbl_17_bss_39C[camera->idx] += 0.05f * (0.12f - lbl_17_bss_39C[camera->idx]);
        lbl_17_bss_38C[ball->playerId] += 0.02f * (target - lbl_17_bss_38C[ball->playerId]);
        camera->rotX += 0.02f * (0x1400 - camera->rotX);
        camera->rotY += 0.02f * (s16)((s16)(ball->rotY - 0x7800) - camera->rotY);
        camera->rotZ -= 0.02f * camera->rotZ;
        camera->lookAt.x = ball->pos.x;
        camera->lookAt.y = ball->pos.y + lbl_17_bss_39C[camera->idx];
        camera->lookAt.z = ball->pos.z;
        mtxa_from_translate(&camera->lookAt);
        mtxa_rotate_y(camera->rotY);
        mtxa_rotate_x(camera->rotX);
        v.x = 0.0f;
        v.y = 0.0f;
        v.z = lbl_17_bss_38C[ball->playerId];
        mtxa_tf_point(&v, &camera->eye);
        PSVECSubtract(&camera->lookAt, &camera->eye, &v);
        camera->rotY = math_atan2(v.x, v.z) - 0x8000;
        camera->rotX = math_atan2(v.y, math_sqrt(mathutil_sum_of_sq_2(v.x, v.z)));
    }
}

void fn_17_8660(struct Camera *camera, struct Ball *ball)
{
    struct CurveSet *a;
    struct CurveSet *b;

    fn_17_7790(camera, ball);
    a = &lbl_17_data_DDC[lbl_17_bss_10];
    b = &lbl_17_data_DF4[lbl_17_bss_10];
    fn_17_94C8(a->curves[0], a->count);
    fn_17_94C8(a->curves[1], a->count);
    fn_17_94C8(a->curves[2], a->count);
    fn_17_94C8(b->curves[0], a->count);
    fn_17_94C8(b->curves[1], a->count);
    fn_17_94C8(b->curves[2], a->count);
    camera->unk1F = 2;
    fn_17_8958(camera, ball);
}

void fn_17_8958(struct Camera *camera, struct Ball *ball)
{
    Vec v;
    struct CurveSet *a;
    struct CurveSet *b;
    float t;

    a = &lbl_17_data_DDC[lbl_17_bss_10];
    b = &lbl_17_data_DF4[lbl_17_bss_10];
    t = lbl_17_bss_350;
    if (t < 0.0) {
        t = 0.0f;
    }
    if (t > 240.0f) {
        t = 240.0f;
    }
    camera->eye.x = fn_1_D2550(t, a->curves[0]);
    camera->eye.y = fn_1_D2550(t, a->curves[1]);
    camera->eye.z = fn_1_D2550(t, a->curves[2]);
    camera->lookAt.x = fn_1_D2550(t, b->curves[0]);
    camera->lookAt.y = fn_1_D2550(t, b->curves[1]);
    camera->lookAt.z = fn_1_D2550(t, b->curves[2]);
    PSVECSubtract(&camera->lookAt, &camera->eye, &v);
    camera->rotY = math_atan2(v.x, v.z) - 0x8000;
    camera->rotX = math_atan2(v.y, math_sqrt(mathutil_sum_of_sq_2(v.x, v.z)));
    camera->rotZ = 0;
}

void fn_17_8AF4(struct Camera *camera, struct Ball *ball)
{
    Vec v;

    switch (current_stage_id) {
    case 0x98:
        {
            Vec a = {-170.5f, 56.5f, -144.5f};
            Vec *p;

            p = &a;
            camera->eye = *p;
        }
        {
            Vec b = {-114.0f, 48.5f, -112.0f};
            Vec *p;

            p = &b;
            camera->lookAt = *p;
        }
        break;
    case 0x99:
        {
            Vec c = {0.0f, 525.0f, 391.5f};
            Vec *p;

            p = &c;
            camera->eye = *p;
        }
        {
            Vec d = {0.0f, 525.0f, 386.0f};
            Vec *p;

            p = &d;
            camera->lookAt = *p;
        }
        break;
    case 0x9A:
    default:
        {
            Vec e = {11.0f, 333.0f, 81.0f};
            Vec *p;

            p = &e;
            camera->eye = *p;
        }
        {
            Vec f = {-9.0f, 357.0f, 62.0f};
            Vec *p;

            p = &f;
            camera->lookAt = *p;
        }
        break;
    }
    PSVECSubtract(&camera->lookAt, &camera->eye, &v);
    camera->rotY = math_atan2(v.x, v.z) - 0x8000;
    camera->rotX = math_atan2(v.y, math_sqrt(mathutil_sum_of_sq_2(v.x, v.z)));
    camera->rotZ = 0;
    camera->unk1F = 8;
}

void fn_17_8CEC(struct Camera *camera, struct Ball *ball) {}

void fn_17_8CF0(struct Camera *camera, struct Ball *ball)
{
    struct Ball *b;

    camera->timerCurr = 8.0 * lbl_80198B34;
    lbl_17_bss_3AC = (rand() & 0x7FFF) % modeCtrl.unk30;
    lbl_17_bss_3AD = -1;
    lbl_17_bss_3AE = 0;
    lbl_17_bss_3B0[lbl_17_bss_3AC] = &lbl_17_data_E0C[(u32)(rand() & 0x7FFF) % 5];
    b = &balls[lbl_17_bss_3AC];
    camera->lookAt = b->pos;
    mtxa_from_quat(&b->ape->rot);
    mtxa_tf_vec_xyz(0.0f, 0.0f, -3.0f, &camera->eye);
    PSVECAdd(&camera->eye, &b->pos, &camera->eye);
    camera->unk1F = 0xA;
    camera->timerCurr = lbl_80198B34 * (8.0 + (float)((rand() & 0x7FFF) % 4));
    fn_17_8EAC(camera, b);
}

void fn_17_8EAC(struct Camera *camera, struct Ball *ball)
{
    Vec eyeA;
    Vec lookAtA;
    Vec eyeB;
    Vec lookAtB;
    Vec v;
    struct Ball *a;
    struct Ball *b;
    float s;
    float r;

    if ((g_some_other_flags & 0xA) == 0) {
        if (camera->timerCurr > 0) {
            camera->timerCurr--;
            if (camera->timerCurr == 0) {
                lbl_17_bss_3AD = (rand() & 0x7FFF) % lbl_17_bss_28;
                if (lbl_17_bss_3AC != lbl_17_bss_3AD
                 && (lbl_1_bss_D4D2D == 0
                  || (lbl_1_bss_D4D2D == 1 && (lbl_17_bss_48[lbl_17_bss_3AD].flags & 8) == 0))) {
                    camera->timerCurr = lbl_80198B34 * (8.0 + (float)((rand() & 0x7FFF) % 4));
                    lbl_17_bss_3AE = 1.5 * lbl_80198B34;
                    lbl_17_bss_3B0[lbl_17_bss_3AD] = &lbl_17_data_E0C[(u32)(rand() & 0x7FFF) % 5];
                } else {
                    camera->timerCurr = 1;
                }
            }
        }
        if (lbl_17_bss_3AE > 0) {
            a = &balls[lbl_17_bss_3AC];
            b = &balls[lbl_17_bss_3AD];
            lbl_17_bss_3AE--;
            fn_17_93E4(a, lbl_17_bss_3B0[lbl_17_bss_3AC], &eyeA, &lookAtA);
            fn_17_93E4(b, lbl_17_bss_3B0[lbl_17_bss_3AD], &eyeB, &lookAtB);
            if ((double)lbl_17_bss_3AE > 0.5 * lbl_80198B34) {
                s = (float)((double)lbl_17_bss_3AE - 0.5 * lbl_80198B34);
                s = math_sqrt(s / (double)lbl_80198B34);
                r = 1.0 - s;
                camera->lookAt.x = s * lookAtA.x + r * lookAtB.x;
                camera->lookAt.y = s * lookAtA.y + r * lookAtB.y;
                camera->lookAt.z = s * lookAtA.z + r * lookAtB.z;
                camera->eye = eyeA;
            } else if ((double)lbl_17_bss_3AE > 0.0) {
                s = (float)(double)lbl_17_bss_3AE;
                s = s / (0.5 * lbl_80198B34);
                s *= s;
                r = 1.0 - s;
                camera->lookAt = lookAtB;
                camera->eye.x = s * eyeA.x + r * eyeB.x;
                camera->eye.y = s * eyeA.y + r * eyeB.y;
                camera->eye.z = s * eyeA.z + r * eyeB.z;
            } else {
                lbl_17_bss_3AC = lbl_17_bss_3AD;
                lbl_17_bss_3AD = -1;
                camera->lookAt = lookAtB;
                camera->eye = eyeB;
            }
        } else {
            fn_17_93E4(&balls[lbl_17_bss_3AC], lbl_17_bss_3B0[lbl_17_bss_3AC], &camera->eye, &camera->lookAt);
        }
        PSVECSubtract(&camera->lookAt, &camera->eye, &v);
        camera->rotY = math_atan2(v.x, v.z) - 0x8000;
        camera->rotX = math_atan2(v.y, math_sqrt(mathutil_sum_of_sq_2(v.x, v.z)));
        camera->rotZ = 0;
    }
}

void fn_17_93E4(struct Ball *ball, struct ViewOffset *offs, Vec *eye, Vec *lookAt)
{
    if (mathutil_sum_of_sq_3(ball->vel.x, 0.0f, ball->vel.z) < 2.143347050754458e-05) {
        mtxa_from_quat(&ball->ape->rot);
    } else {
        mtxa_from_rotate_y(math_atan2(ball->vel.x, ball->vel.z) + 0x8000);
    }
    mtxa_tf_vec(&offs->a, eye);
    PSVECAdd(eye, &ball->pos, eye);
    mtxa_tf_vec(&offs->b, lookAt);
    PSVECAdd(lookAt, &ball->pos, lookAt);
}

void fn_17_94C8(struct Keyframe *frames, int count)
{
    struct Keyframe *a;
    struct Keyframe *b;
    struct Keyframe *c;
    float slope;
    int i;

    for (i = 1; i < count - 1; i++) {
        a = &frames[i - 1];
        b = &frames[i];
        c = &frames[i + 1];
        slope = (c->value - a->value) / (float)(c->frame - a->frame);
        b->tangentIn = slope;
        b->tangentOut = slope;
    }
}

void fn_17_9554(void)
{
    int i;

    for (i = 0; i < 6; i++) {
        overwrite_effect_funcs(i + 0x27, &lbl_17_data_E88[i]);
    }
}

void fn_17_95A4(struct Effect *effect)
{
    effect->u_otherTimer = -1;
    effect->timer = -1;
}

void fn_17_95B8(struct Effect *effect)
{
    effect->u_otherTimer++;
    PSVECAdd(&effect->pos, &effect->vel, &effect->pos);
    if ((u32)effect->u_otherTimer < 0x27) {
        effect->model = minigameGma[0]->modelEntries[lbl_17_data_EE8[effect->u_otherTimer]].model;
    } else {
        g_poolInfo.effectPool.statusList[effect->poolIndex] = 3;
    }
}

void fn_17_9658(struct Effect *effect)
{
    float a;
    float b;

    if ((g_some_render_flag & 4) == 0) {
        mtxa_from_mtxb_translate(&effect->pos);
        mtxa_sq_from_identity();
        mtxa_scale(&effect->scale);
        if (effect->scale.y > effect->scale.z) {
            a = effect->scale.y;
        } else {
            a = effect->scale.z;
        }
        if (effect->scale.x > a) {
            b = effect->scale.x;
        } else if (effect->scale.y > effect->scale.z) {
            b = effect->scale.y;
        } else {
            b = effect->scale.z;
        }
        avdisp_set_scale_factor(b);
        avdisp_draw_model_culled_sort_always(effect->model);
    }
}

void fn_17_9704(struct Effect *effect) {}

void fn_17_9708(struct Effect *effect)
{
    effect->u_otherTimer = -1;
    effect->timer = -1;
}

void fn_17_971C(struct Effect *effect)
{
    effect->u_otherTimer++;
    PSVECAdd(&effect->pos, &effect->vel, &effect->pos);
    if ((u32)effect->u_otherTimer < 0x30) {
        struct GMAModelEntry *entries = minigameGma[0]->modelEntries;
        int frame = effect->u_otherTimer >> 1;

        effect->model = entries[lbl_17_data_F38[frame]].model;
    } else {
        g_poolInfo.effectPool.statusList[effect->poolIndex] = 3;
    }
}
