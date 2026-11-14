/* Cameras for the Monkey Target 2 minigame. */
#include "types.h"
#include "ape_internal.h"
#include "ball.h"
#include "camera.h"
#include "dip.h"
#include "input.h"
#include "mathutil.h"
#include "mode.h"
#include "rand.h"
#include "stage.h"
#include "pilot2.h"

extern u32 lbl_1_bss_D4A10;
extern float lbl_80198B34;

struct CameraOffset
{
    Vec lookAt;
    Vec eye;
};

static void fn_13_8B74(struct Camera *camera, struct Ball *ball);
static void fn_13_8EA8(struct Camera *camera, struct Ball *ball);
static void fn_13_9420(struct Camera *camera, struct Ball *ball);
static void fn_13_9C28(struct Camera *camera, struct Ball *ball);
static void fn_13_9CCC(struct Camera *camera, struct Ball *ball);
static void fn_13_A0C8(struct Camera *camera, struct Ball *ball);
static void fn_13_A184(struct Camera *camera, struct Ball *ball);
static void fn_13_A188(struct Camera *camera, struct Ball *ball);
static void fn_13_A2D4(struct Camera *camera, struct Ball *ball);
static void fn_13_A7A8(struct Ball *ball, struct CameraOffset *offset, Vec *eye, Vec *lookAt);

static void (*lbl_13_data_E08[])(struct Camera *, struct Ball *) = {
    fn_13_8B74, fn_13_8EA8, fn_13_9420, fn_13_9C28, fn_13_9CCC,
    fn_13_A0C8, fn_13_A184, fn_13_A188, fn_13_A2D4,
};

static struct CameraOffset lbl_13_data_E2C[4] = {
    { { 0.0f, 5.0f, -6.0f }, { 0.0f, 0.0f, 0.0f } },
    { { 0.0f, 3.0f, 3.0f }, { 0.0f, 0.0f, 0.0f } },
    { { 0.0f, 2.0f, -1.0f }, { 0.0f, 0.0f, 0.0f } },
    { { 0.0f, 0.5f, 1.2f }, { 0.0f, 0.0f, 0.0f } },
};

static s16 lbl_13_bss_2D8;
static Vec lbl_13_bss_2DC[5];
static float lbl_13_bss_318;  /* distance */
static float lbl_13_bss_31C;  /* look-at height */
static s8 lbl_13_bss_320;  /* focused ball */
static s8 lbl_13_bss_321;  /* next ball, or -1 */
static s16 lbl_13_bss_322;  /* transition timer */
static struct CameraOffset *lbl_13_bss_324[5];

void fn_13_89DC(void)
{
    int i;

    for (i = 0; i < modeCtrl.playerCount; i++)
        preload_ape(lbl_13_bss_224[i], 0, 1, 0x1A);
}

void fn_13_8A44(void)
{
    preload_ape(lbl_13_bss_224[0], 1, 0, 0);
    queue_stage_load(0xC8);
}

void fn_13_8A88(struct Camera *camera)
{
    struct Ball *ball;

    if (lbl_1_bss_D4A10 & 1)
    {
        if (lbl_13_bss_70[camera->idx].unk42 > 0)
            ball = &balls[lbl_13_bss_210[lbl_13_bss_70[camera->idx].unk41]];
        else
            ball = &balls[camera->idx];
    }
    else
        ball = &balls[camera->idx];
    lbl_13_data_E08[camera->unk1F](camera, ball);
}

static void fn_13_8B74(struct Camera *camera, struct Ball *ball)
{
    Vec v;
    struct Ball *target;

    camera_clear(camera);
    camera->unk114 = 0;
    camera->unkC0 = 0;
    lbl_13_bss_2D8 = -0x700;
    lbl_13_bss_2DC[ball->playerId] = (Vec){ 0.0f, 0.0f, 0.0f };
    if (lbl_1_bss_D4A10 & 1)
    {
        switch (lbl_13_bss_10)
        {
        default:
            lbl_13_bss_318 = 6.0f;
            break;
        case 3:
            lbl_13_bss_318 = 5.0f;
            break;
        case 2:
            lbl_13_bss_318 = 4.5f;
            break;
        }
    }
    else
        lbl_13_bss_318 = 3.0f;
    switch (lbl_13_bss_24)
    {
    case 0:
    case 4:
        lbl_13_bss_31C = 0.8f;
        break;
    default:
        lbl_13_bss_31C = 0.5f;
        break;
    }
    camera->lookAt.x = ball->pos.x;
    camera->lookAt.y = ball->pos.y + lbl_13_bss_31C;
    camera->lookAt.z = ball->pos.z;
    mtxa_from_translate(&camera->lookAt);
    mtxa_rotate_y(stagedef->startPos->yrot);
    v.x = 0.0f;
    v.y = 1.0f;
    v.z = lbl_13_bss_318;
    mtxa_tf_point(&v, &camera->eye);
    v.x = 0.0f;
    v.y = 0.0f;
    v.z = lbl_13_bss_318;
    mtxa_tf_point(&v, &camera->unkB4);
    v.x = camera->lookAt.x - camera->eye.x;
    v.y = camera->lookAt.y - camera->eye.y;
    v.z = camera->lookAt.z - camera->eye.z;
    camera->rotY = math_atan2(v.x, v.z) - 0x8000;
    camera->rotX = math_atan2(v.y, math_sqrt(mathutil_sum_of_sq_2(v.x, v.z)));
    camera->rotZ = 0;
    camera->unk1F = 1;
    if (lbl_1_bss_D4A10 & 1)
    {
        if (lbl_13_bss_70[camera->idx].unk42 > 0)
            target = &balls[lbl_13_bss_210[lbl_13_bss_70[camera->idx].unk41]];
        else
            target = &balls[camera->idx];
    }
    else
        target = &balls[camera->idx];
    lbl_13_data_E08[camera->unk1F](camera, target);
}

static void fn_13_8EA8(struct Camera *camera, struct Ball *ball)
{
    Vec v;
    float eyeX;
    float eyeY;
    float eyeZ;
    float lookAtX;
    float lookAtY;
    float lookAtZ;
    float lenSq;
    float rsqrt;
    s16 pitch;
    s16 yaw;
    int d;
    int turn;

    if (g_some_other_flags & 0xA)
        return;
    eyeX = camera->eye.x;
    eyeY = camera->eye.y;
    eyeZ = camera->eye.z;
    lookAtX = camera->lookAt.x;
    lookAtY = camera->lookAt.y;
    lookAtZ = camera->lookAt.z;
    v.x = camera->unkB4.x - camera->lookAt.x;
    v.y = camera->unkB4.y - camera->lookAt.y;
    v.z = camera->unkB4.z - camera->lookAt.z;
    lenSq = mathutil_sum_of_sq_3(v.x, v.y, v.z);
    if (lenSq > 1.1920929e-07f)
    {
        rsqrt = math_rsqrt(lenSq);
        v.x *= rsqrt;
        v.y *= rsqrt;
        v.z *= rsqrt;
    }
    else
    {
        v.x = 1.0f;
        v.y = 0.0f;
        v.z = 0.0f;
    }
    v.x = 0.75f * v.x + camera->lookAt.x;
    v.y = 0.75f * v.y + camera->lookAt.y;
    v.z = 0.75f * v.z + camera->lookAt.z;
    camera->lookAt.x = ball->pos.x;
    camera->lookAt.y = ball->pos.y + lbl_13_bss_31C;
    camera->lookAt.z = ball->pos.z;
    v.x = camera->lookAt.x - v.x;
    v.y = camera->lookAt.y - v.y;
    v.z = camera->lookAt.z - v.z;
    pitch = math_atan2(v.y, math_sqrt(mathutil_sum_of_sq_2(v.x, v.z)));
    yaw = math_atan2(v.x, v.z) - 0x8000;
    d = (s16)(yaw - camera->rotY);
    d = d < -0x200 ? -0x200 : (d > 0x200 ? 0x200 : d);
    yaw = camera->rotY + d;
    if (!(camera->flags & 2) && !(ball->flags & 0x1000))
    {
        d = (s16)(ball->unk92 - yaw);
        d = d > 0x800 ? d - 0x800 : (d < -0x800 ? d + 0x800 : 0);
        d >>= 7;
        turn = camera->unk114;
        if (d == 0)
            turn = 0;
        else if ((turn < 0 && d > 0) || (turn > 0 && d < 0))
            turn = 0;
        else if (d < 0)
            if (d < turn - 4)
                turn -= 4;
            else
                turn = d;
        else
            turn = d > turn + 4 ? turn + 4 : d;
        yaw = yaw + turn;
        d = (s16)(yaw - camera->rotY);
        d = d < -0x300 ? -0x300 : (d > 0x300 ? 0x300 : d);
        yaw = camera->rotY + d;
    }
    if (pitch < -0x2800)
        pitch = -0x2800;
    else if (pitch > 0x2800)
        pitch = 0x2800;
    camera->unkC0 *= 0.98f;
    pitch = camera->unkC0 = camera->unkC0 + 0.2 * (pitch - camera->unkC0);
    mtxa_from_translate(&camera->lookAt);
    mtxa_rotate_y(yaw);
    mtxa_rotate_x(pitch);
    v.x = 0.0f;
    v.y = 0.0f;
    v.z = lbl_13_bss_318;
    mtxa_tf_point(&v, &camera->unkB4);
    camera->unk114 = yaw - camera->rotY;
    camera->rotY = yaw;
    camera->rotX = pitch - 0xB00;
    mtxa_from_translate(&camera->lookAt);
    mtxa_rotate_y(camera->rotY);
    mtxa_rotate_x(camera->rotX);
    v.x = 0.0f;
    v.y = 0.0f;
    v.z = math_sqrt(mathutil_sum_of_sq_2(lbl_13_bss_318, 1.0f));
    mtxa_tf_point(&v, &camera->eye);
    camera->eyeVel.x = camera->eye.x - eyeX;
    camera->eyeVel.y = camera->eye.y - eyeY;
    camera->eyeVel.z = camera->eye.z - eyeZ;
    camera->lookAtVel.x = camera->lookAt.x - lookAtX;
    camera->lookAtVel.y = camera->lookAt.y - lookAtY;
    camera->lookAtVel.z = camera->lookAt.z - lookAtZ;
}

static void fn_13_9420(struct Camera *camera, struct Ball *ball)
{
    Vec v;
    Vec eye;
    Vec lookAt;
    s16 pitch;
    s16 yaw;
    struct Pilot2Player *player = &lbl_13_bss_70[ball->playerId];
    float lenSq;
    float rsqrt;
    s16 roll;
    int d;
    int turn;

    if (g_some_other_flags & 0xA)
        return;
    eye = camera->eye;
    lookAt = camera->lookAt;
    if (lbl_13_bss_10 >= 4)
        lbl_13_bss_31C = lbl_13_bss_31C + 0.01f * ((lbl_13_bss_234[camera->idx] == 3 ? 0.4f : 0.5f) - lbl_13_bss_31C);
    else
        lbl_13_bss_31C = 0.8f;
    v.x = camera->unkB4.x - camera->lookAt.x;
    v.y = camera->unkB4.y - camera->lookAt.y;
    v.z = camera->unkB4.z - camera->lookAt.z;
    lenSq = mathutil_sum_of_sq_3(v.x, v.y, v.z);
    if (lenSq > 1.1920929e-07f)
    {
        rsqrt = math_rsqrt(lenSq);
        v.x *= rsqrt;
        v.y *= rsqrt;
        v.z *= rsqrt;
    }
    else
    {
        v.x = 1.0f;
        v.y = 0.0f;
        v.z = 0.0f;
    }
    v.x = 0.75f * v.x + camera->lookAt.x;
    v.y = 0.75f * v.y + camera->lookAt.y;
    v.z = 0.75f * v.z + camera->lookAt.z;
    mtxa_from_mtx(ball->unk30);
    mtxa_tf_point(&lbl_13_bss_2DC[ball->playerId], &camera->lookAt);
    camera->lookAt.y += lbl_13_bss_31C;
    v.x = camera->lookAt.x - v.x;
    v.y = camera->lookAt.y - v.y;
    v.z = camera->lookAt.z - v.z;
    if ((pad_status_groups[player->unk40].unk0[0].button & 0x800) && ball->unk154 < 8)
    {
        pitch = camera->unkC0 = camera->unkC0 + 0.1f * (-0x2800 - camera->unkC0);
        lbl_13_bss_2DC[ball->playerId].z += 0.1f * (-0.5f - lbl_13_bss_2DC[ball->playerId].z);
    }
    else
    {
        if (ball->unk80 < 0x3C)
            pitch = 0;
        else
            pitch = math_atan2(v.y, math_sqrt(mathutil_sum_of_sq_2(v.x, v.z)));
        if (pitch < -0x2000)
            pitch = -0x2000;
        else if (pitch > 0x2000)
            pitch = 0x2000;
        pitch = camera->unkC0 = camera->unkC0 + 0.2f * (pitch - camera->unkC0);
        lbl_13_bss_2DC[ball->playerId].z += 0.1f * -lbl_13_bss_2DC[ball->playerId].z;
    }
    yaw = math_atan2(v.x, v.z) - 0x8000;
    if (ball->unk154 == 4)
        yaw -= pad_status_groups[player->unk40].unk0[0].substickX * 0x32;
    d = (s16)(yaw - camera->rotY);
    d = d < -0x200 ? -0x200 : (d > 0x200 ? 0x200 : d);
    yaw = camera->rotY + d;
    if (!(camera->flags & 2) && !(ball->flags & 0x1000))
    {
        d = (s16)(ball->unk92 - yaw);
        d = d > 0x800 ? d - 0x800 : (d < -0x800 ? d + 0x800 : 0);
        d >>= 7;
        turn = camera->unk114;
        if (d == 0)
            turn = 0;
        else if ((turn < 0 && d > 0) || (turn > 0 && d < 0))
            turn = 0;
        else if (d < 0)
            if (d < turn - 4)
                turn -= 4;
            else
                turn = d;
        else
            turn = d > turn + 4 ? turn + 4 : d;
        yaw = yaw + turn;
        d = (s16)(yaw - camera->rotY);
        d = d < -0x300 ? -0x300 : (d > 0x300 ? 0x300 : d);
        yaw = camera->rotY + d;
    }
    if (player->unk10 > 0.0f)
    {
        int z;

        roll = ball->rotZ;
        if (roll > 0x4000)
            roll = 0x8000 - roll;
        else if (roll < -0x4000)
            roll = -0x8000 - roll;
        roll = 0.025f * (roll * player->unk10);
        z = roll / 2 - camera->rotZ;
        camera->rotZ += (s16)(0.04f * z);
    }
    else
        camera->rotZ -= 0.05f * camera->rotZ;
    mtxa_from_translate(&camera->lookAt);
    mtxa_rotate_y(yaw);
    mtxa_rotate_x(pitch);
    v.x = 0.0f;
    v.y = 0.0f;
    v.z = lbl_13_bss_318;
    mtxa_tf_point(&v, &camera->unkB4);
    camera->unk114 = yaw - camera->rotY;
    camera->rotY = yaw;
    camera->rotX = pitch - 0x700;
    mtxa_from_translate(&camera->lookAt);
    mtxa_rotate_y(camera->rotY);
    mtxa_rotate_x(camera->rotX);
    v.x = 0.0f;
    v.y = 0.0f;
    v.z = math_sqrt(mathutil_sum_of_sq_2(lbl_13_bss_318, 1.0f));
    mtxa_tf_point(&v, &camera->eye);
    PSVECSubtract(&camera->eye, &eye, &camera->eyeVel);
    PSVECSubtract(&camera->lookAt, &lookAt, &camera->lookAtVel);
}

static void fn_13_9C28(struct Camera *camera, struct Ball *ball)
{
    camera->unk114 = 0;
    if (lbl_13_bss_70[ball->playerId].unk42 > 0)
    {
        if (current_stage_id == 0x9C)
        {
            if (ball->pos.x > 0.0f)
                camera->rotY = 0x2800;
            else
                camera->rotY = -0x7800;
        }
        else
            camera->rotY = 0;
    }
    camera->unk1F = 4;
    fn_13_9CCC(camera, ball);
}

static void fn_13_9CCC(struct Camera *camera, struct Ball *ball)
{
    Vec v;
    Vec eye;
    Vec lookAt;
    float lenSq;
    float rsqrt;
    s16 pitch;
    s16 yaw;

    if (g_some_other_flags & 0xA)
        return;
    eye = camera->eye;
    lookAt = camera->lookAt;
    v.x = camera->unkB4.x - camera->lookAt.x;
    v.y = camera->unkB4.y - camera->lookAt.y;
    v.z = camera->unkB4.z - camera->lookAt.z;
    lenSq = mathutil_sum_of_sq_3(v.x, v.y, v.z);
    if (lenSq > 1.1920929e-07f)
    {
        rsqrt = math_rsqrt(lenSq);
        v.x *= rsqrt;
        v.y *= rsqrt;
        v.z *= rsqrt;
    }
    else
    {
        v.x = 1.0f;
        v.y = 0.0f;
        v.z = 0.0f;
    }
    v.x = 0.75f * v.x + camera->lookAt.x;
    v.y = 0.75f * v.y + camera->lookAt.y;
    v.z = 0.75f * v.z + camera->lookAt.z;
    camera->lookAt.x = ball->pos.x;
    camera->lookAt.y = ball->pos.y + lbl_13_bss_31C;
    camera->lookAt.z = ball->pos.z;
    v.x = camera->lookAt.x - v.x;
    v.y = camera->lookAt.y - v.y;
    v.z = camera->lookAt.z - v.z;
    pitch = math_atan2(v.y, math_sqrt(mathutil_sum_of_sq_2(v.x, v.z)));
    if (camera->unk114 < 0x50)
        camera->unk114++;
    else
        camera->unk114 = 0x50;
    if (current_stage_id == 0x9C && ball->pos.y < 3.0f && camera->unk114 > 0x20)
        camera->unk114 = 0x20;
    yaw = camera->rotY + camera->unk114;
    camera->rotZ -= 0.05 * camera->rotZ;
    pitch = camera->unkC0 = camera->unkC0 + 0.2 * (pitch - camera->unkC0);
    mtxa_from_translate(&camera->lookAt);
    mtxa_rotate_y(yaw);
    mtxa_rotate_x(pitch);
    v.x = 0.0f;
    v.y = 0.0f;
    v.z = lbl_13_bss_318;
    mtxa_tf_point(&v, &camera->unkB4);
    camera->unk114 = yaw - camera->rotY;
    camera->rotY = yaw;
    camera->rotX = pitch - 0x700;
    mtxa_from_translate(&camera->lookAt);
    mtxa_rotate_y(camera->rotY);
    mtxa_rotate_x(camera->rotX);
    v.x = 0.0f;
    v.y = 0.0f;
    v.z = math_sqrt(mathutil_sum_of_sq_2(lbl_13_bss_318, 1.0f));
    mtxa_tf_point(&v, &camera->eye);
    PSVECSubtract(&camera->eye, &eye, &camera->eyeVel);
    PSVECSubtract(&camera->lookAt, &lookAt, &camera->lookAtVel);
}

static void fn_13_A0C8(struct Camera *camera, struct Ball *ball)
{
    Vec v;

    camera->eye.x = 0.0f;
    camera->eye.y = 157.0f;
    camera->eye.z = 391.5f;
    camera->lookAt.x = 0.0f;
    camera->lookAt.y = 152.0f;
    camera->lookAt.z = 386.0f;
    PSVECSubtract(&camera->lookAt, &camera->eye, &v);
    camera->rotY = math_atan2(v.x, v.z) - 0x8000;
    camera->rotX = math_atan2(v.y, math_sqrt(mathutil_sum_of_sq_2(v.x, v.z)));
    camera->rotZ = 0;
    camera->unk1F = 6;
}

static void fn_13_A184(struct Camera *camera, struct Ball *ball) {}

static void fn_13_A188(struct Camera *camera, struct Ball *ball)
{
    struct Ball *focus;

    camera->timerCurr = 8.0 * lbl_80198B34;
    lbl_13_bss_320 = (rand() & 0x7FFF) % modeCtrl.unk30;
    lbl_13_bss_321 = -1;
    lbl_13_bss_322 = 0;
    lbl_13_bss_324[lbl_13_bss_320] = lbl_13_data_E2C;
    focus = &balls[lbl_13_bss_320];
    camera->lookAt = focus->pos;
    mtxa_from_quat(&focus->ape->rot);
    mtxa_tf_vec_xyz(0.0f, 0.0f, -3.0f, &camera->eye);
    PSVECAdd(&camera->eye, &focus->pos, &camera->eye);
    camera->timerCurr = lbl_80198B38 * 0xF;
    camera->unk1F = 8;
    fn_13_A2D4(camera, focus);
}

static void fn_13_A2D4(struct Camera *camera, struct Ball *ball)
{
    Vec eye;
    Vec lookAt;
    Vec nextEye;
    Vec nextLookAt;
    Vec v;
    struct Ball *cur;
    struct Ball *next;
    float t;
    float u;

    if (g_some_other_flags & 0xA)
        return;
    if (camera->timerCurr > 0)
    {
        camera->timerCurr--;
        if (camera->timerCurr == 0)
        {
            camera->timerCurr = lbl_80198B34 * (8.0 + (float)((rand() & 0x7FFF) % 4));
            do
            {
                lbl_13_bss_321 = (rand() & 0x7FFF) % modeCtrl.unk30;
            } while (lbl_13_bss_320 == lbl_13_bss_321);
            lbl_13_bss_322 = 1.5 * lbl_80198B34;
            lbl_13_bss_324[lbl_13_bss_321] = &lbl_13_data_E2C[(u32)(rand() & 0x7FFF) % 4];
        }
    }
    if (lbl_13_bss_322 > 0)
    {
        cur = &balls[lbl_13_bss_320];
        next = &balls[lbl_13_bss_321];
        lbl_13_bss_322--;
        fn_13_A7A8(cur, lbl_13_bss_324[lbl_13_bss_320], &eye, &lookAt);
        fn_13_A7A8(next, lbl_13_bss_324[lbl_13_bss_321], &nextEye, &nextLookAt);
        if (lbl_13_bss_322 > 0.5 * lbl_80198B34)
        {
            t = lbl_13_bss_322 - 0.5 * lbl_80198B34;
            t = math_sqrt(t / (double)lbl_80198B34);
            u = 1.0 - t;
            camera->lookAt.x = t * lookAt.x + u * nextLookAt.x;
            camera->lookAt.y = t * lookAt.y + u * nextLookAt.y;
            camera->lookAt.z = t * lookAt.z + u * nextLookAt.z;
            camera->eye = eye;
        }
        else if (lbl_13_bss_322 > 0.0)
        {
            t = (double)lbl_13_bss_322;
            t /= 0.5 * lbl_80198B34;
            t = t * t;
            u = 1.0 - t;
            camera->lookAt = nextLookAt;
            camera->eye.x = t * eye.x + u * nextEye.x;
            camera->eye.y = t * eye.y + u * nextEye.y;
            camera->eye.z = t * eye.z + u * nextEye.z;
        }
        else
        {
            lbl_13_bss_320 = lbl_13_bss_321;
            lbl_13_bss_321 = -1;
            camera->lookAt = nextLookAt;
            camera->eye = nextEye;
        }
    }
    else
        fn_13_A7A8(&balls[lbl_13_bss_320], lbl_13_bss_324[lbl_13_bss_320], &camera->eye, &camera->lookAt);
    if (camera->eye.y < 10.0)
        camera->eye.y = 10.0f;
    PSVECSubtract(&camera->lookAt, &camera->eye, &v);
    camera->rotY = math_atan2(v.x, v.z) - 0x8000;
    camera->rotX = math_atan2(v.y, math_sqrt(mathutil_sum_of_sq_2(v.x, v.z)));
    camera->rotZ = 0;
}

static void fn_13_A7A8(struct Ball *ball, struct CameraOffset *offset, Vec *eye, Vec *lookAt)
{
    if (mathutil_sum_of_sq_3(ball->vel.x, 0.0f, ball->vel.z) < 2.143347050754458e-05)
        mtxa_from_quat(&ball->ape->rot);
    else
        mtxa_from_rotate_y(math_atan2(ball->vel.x, ball->vel.z) + 0x8000);
    mtxa_tf_vec(&offset->lookAt, eye);
    PSVECAdd(eye, &ball->pos, eye);
    mtxa_tf_vec(&offset->eye, lookAt);
    PSVECAdd(lookAt, &ball->pos, lookAt);
}
