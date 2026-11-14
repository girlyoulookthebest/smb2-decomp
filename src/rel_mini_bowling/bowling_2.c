/* Bowling camera: one mode per shot phase, each easing toward its own framing. */
#include "types.h"
#include "ball.h"
#include "camera.h"
#include "dip.h"
#include "input.h"
#include "mathutil.h"

#ifndef __MWERKS__
void PSVECSubtract(Vec *a, Vec *b, Vec *out);
#endif
#include "pool.h"

#define STICK_X() (pad_status_groups[player_pad_map[current_ball->playerId]].unk0[0].substickX)
#define STICK_Y() (pad_status_groups[player_pad_map[current_ball->playerId]].unk0[0].substickY)

extern u32 g_some_render_flag;

int fn_1_E5E08(void);
int fn_1_E5E44(void);
void fn_1_E5E78(int index, Vec *out);
void fn_1_E6384(Vec *out);
void fn_1_E678C(void *m);
int fn_5_113C8(void);
s8 fn_5_169E0(void);
s8 fn_5_16A20(void);
float fn_5_16B64(Vec *pos);
void fn_5_17350(float z, float *left, float *right);

/* Ease the camera toward a framing, then re-derive its angles and velocities. */
static void camera_approach(struct Camera *camera, Vec *eye, Vec *at, int fov, float speed)
{
    Vec prevEye;
    Vec prevAt;
    Vec delta;

    prevEye = camera->eye;
    prevAt = camera->lookAt;
    camera->eye.x += speed * (eye->x - camera->eye.x);
    camera->eye.y += speed * (eye->y - camera->eye.y);
    camera->eye.z += speed * (eye->z - camera->eye.z);
    camera->lookAt.x += speed * (at->x - camera->lookAt.x);
    camera->lookAt.y += speed * (at->y - camera->lookAt.y);
    camera->lookAt.z += speed * (at->z - camera->lookAt.z);
    PSVECSubtract(&camera->lookAt, &camera->eye, &delta);
    camera->rotY = math_atan2(delta.x, delta.z) - 0x8000;
    camera->rotX = math_atan2(delta.y, math_sqrt(mathutil_sum_of_sq_2(delta.x, delta.z)));
    camera->rotZ = 0;
    camera->sub28.view.fov += speed * (float)(fov - camera->sub28.view.fov);
    PSVECSubtract(&camera->eye, &prevEye, &camera->eyeVel);
    PSVECSubtract(&camera->lookAt, &prevAt, &camera->lookAtVel);
}

static void fn_5_D800(struct Camera *camera, struct Ball *ball);
static void fn_5_D9CC(struct Camera *camera, struct Ball *ball);
static void fn_5_DCAC(struct Camera *camera, struct Ball *ball);
static void fn_5_DF8C(struct Camera *camera, struct Ball *ball);
static void fn_5_E2E8(struct Camera *camera, struct Ball *ball);
static void fn_5_E5FC(struct Camera *camera, struct Ball *ball);
static void fn_5_E910(struct Camera *camera, struct Ball *ball);
static void fn_5_EB64(struct Camera *camera, struct Ball *ball);
static void fn_5_EE28(struct Camera *camera, struct Ball *ball);
static void fn_5_F054(struct Camera *camera, struct Ball *ball);
static void fn_5_F280(struct Camera *camera, struct Ball *ball);
static void fn_5_F540(struct Camera *camera, struct Ball *ball);
static void fn_5_F7F8(struct Camera *camera, struct Ball *ball);
static void fn_5_F9F4(struct Camera *camera, struct Ball *ball);
static void fn_5_FBF0(struct Camera *camera, struct Ball *ball);
static void fn_5_FE94(struct Camera *camera, struct Ball *ball);
static void fn_5_10180(struct Camera *camera, struct Ball *ball);
static void fn_5_104B8(struct Camera *camera, struct Ball *ball);
static void fn_5_10560(struct Camera *camera, struct Ball *ball);
static int fn_5_10A50(void);

Vec lbl_5_data_15E8 = {0.0f, 0.5f, -99.752289f};

void fn_5_D710(struct Camera *camera, struct Ball *ball)
{
    if ((g_some_other_flags & 0xA) == 0)
    {
        switch (camera->unk1F)
        {
        case 0:
            fn_5_D800(camera, ball);
            break;
        case 1:
            fn_5_D9CC(camera, ball);
            break;
        case 3:
            fn_5_DF8C(camera, ball);
            break;
        case 4:
            fn_5_E2E8(camera, ball);
            break;
        case 5:
            fn_5_E5FC(camera, ball);
            break;
        case 6:
            fn_5_E910(camera, ball);
            break;
        case 7:
            fn_5_EB64(camera, ball);
            break;
        case 8:
            fn_5_EE28(camera, ball);
            break;
        case 9:
            fn_5_F054(camera, ball);
            break;
        case 10:
            fn_5_F280(camera, ball);
            break;
        case 11:
            fn_5_F540(camera, ball);
            break;
        case 12:
            fn_5_F7F8(camera, ball);
            break;
        case 13:
            fn_5_F9F4(camera, ball);
            break;
        case 14:
            fn_5_FBF0(camera, ball);
            break;
        case 15:
            fn_5_FE94(camera, ball);
            break;
        case 16:
            fn_5_10180(camera, ball);
            break;
        case 17:
            fn_5_104B8(camera, ball);
            break;
        case 18:
            fn_5_10560(camera, ball);
            break;
        case 2:
            fn_5_DCAC(camera, ball);
            break;
        }
    }
}

static void fn_5_D800(struct Camera *camera, struct Ball *ball)
{
    Vec delta;

    camera_clear(camera);
    {
        Vec eye = {0.0f, 3.5f, 2.5f};
        Vec *p = &eye;
        camera->unkB4 = *p;
    }
    camera->eye = camera->unkB4;
    camera->lookAt = lbl_5_data_15E8;
    camera->eyeVel.x = 0.0f;
    camera->eyeVel.y = 0.0f;
    camera->eyeVel.z = 0.0f;
    camera->lookAtVel.x = 0.0f;
    camera->lookAtVel.y = 0.0f;
    camera->lookAtVel.z = 0.0f;
    PSVECSubtract(&camera->lookAt, &camera->eye, &delta);
    camera->rotY = math_atan2(delta.x, delta.z) - 0x8000;
    camera->rotX = math_atan2(delta.y, math_sqrt(mathutil_sum_of_sq_2(delta.x, delta.z)));
    camera->rotZ = 0;
    camera->unk26 = 9;
    camera->sub28.view.fov = 0x1300;
    cameras[ball->playerId].flags &= ~4;
    cameras[ball->playerId].flags |= 8;
    camera->unk1F = 3;
}

static void fn_5_D9CC(struct Camera *camera, struct Ball *ball)
{
    Vec eye;
    float z;

    eye = camera->eye;
    if (ball->pos.z < -98.01f)
        eye.x = eye.x + 0.1 * (ball->pos.x - eye.x);
    else
        eye.x = ball->pos.x;
    eye.y = 2.0f;
    z = 1.0 + ball->pos.z + ((1.0 / -ball->vel.z < 9.0) ? 1.0 / -ball->vel.z : 9.0);
    if (eye.z > z)
        eye.z = z;
    if (eye.z < -90.0)
        eye.z = -90.0f;
    camera_approach(camera, &eye, &lbl_5_data_15E8, 0x1800, 0.05f);
}

static void fn_5_DCAC(struct Camera *camera, struct Ball *ball)
{
    Vec eye;
    Vec at;
    float stickX = 0.016666666666666666 * STICK_X();
    float stickY = 0.016666666666666666 * STICK_Y();
    eye.x = ball->pos.x;
    eye.y = 0.77f;
    eye.z = 10.0 + ball->pos.z;

    if (eye.z < -90.0)
        eye.z = -90.0f;
    if (eye.x < -3.0)
        eye.x = -3.0f;
    if (eye.x > 3.0)
        eye.x = 3.0f;
    at.x = ball->pos.x;
    at.y = 10.0f;
    at.z = lbl_5_data_15E8.z;
    camera_approach(camera, &eye, &at, 0x1400, 0.05f);
}

static void fn_5_DF8C(struct Camera *camera, struct Ball *ball)
{
    Vec eye;
    Vec at;
    float stickX = 0.016666666666666666 * STICK_X();
    float stickY = 0.016666666666666666 * STICK_Y();
    eye.x = ball->pos.x;
    eye.y = 0.77f;
    eye.z = 10.0 + ball->pos.z;

    if (eye.z < -90.0)
        eye.z = -90.0f;
    if (eye.x < -3.0)
        eye.x = -3.0f;
    if (eye.x > 3.0)
        eye.x = 3.0f;
    at.x = ball->pos.x;
    at.y = 10.0f;
    at.z = lbl_5_data_15E8.z;
    if (0.0 != stickX || stickY > 0.0)
    {
        at.y = 4.2f;
        at.z = -30.0f;
        eye.x += 20.0 * stickX;
        if (stickY > 0.0f)
            eye.y += 15.0 * stickY;
    }
    camera_approach(camera, &eye, &at, 0x1400, 0.05f);
}

static void fn_5_E2E8(struct Camera *camera, struct Ball *ball)
{
    Vec eye;
    Vec at;
    eye.x = ball->pos.x;
    eye.y = 3.6f;
    eye.z = 9.9f + ball->pos.z;

    if (eye.z < -90.0)
        eye.z = -90.0f;
    if (eye.x < -3.0)
        eye.x = -3.0f;
    if (eye.x > 3.0)
        eye.x = 3.0f;
    at.x = ball->pos.x;
    at.y = -20.0f;
    at.z = lbl_5_data_15E8.z;
    eye.x += 0.083333333333333329 * STICK_X();
    at.x += -0.41666666666666669 * STICK_X();
    camera_approach(camera, &eye, &at, 0x1200, 0.05f);
}

static void fn_5_E5FC(struct Camera *camera, struct Ball *ball)
{
    Vec eye;
    Vec at;
    eye.x = ball->pos.x;
    eye.y = 8.6f;
    eye.z = 13.5f + ball->pos.z;

    if (eye.z < -90.0)
        eye.z = -90.0f;
    if (eye.x < -3.0)
        eye.x = -3.0f;
    if (eye.x > 3.0)
        eye.x = 3.0f;
    at.x = ball->pos.x;
    at.y = -42.0f;
    at.z = lbl_5_data_15E8.z;
    eye.x += 0.16666666666666666 * STICK_X();
    at.x += -0.75 * STICK_X();
    camera_approach(camera, &eye, &at, 0x1000, 0.05f);
}

static void fn_5_E910(struct Camera *camera, struct Ball *ball)
{
    Vec eye;
    Vec at;

    eye.x = ball->pos.x;
    eye.y = 3.6f;
    eye.z = 9.9f + ball->pos.z;
    if (eye.z < -90.0)
        eye.z = -90.0f;
    if (eye.x < -3.0)
        eye.x = -3.0f;
    if (eye.x > 3.0)
        eye.x = 3.0f;
    at.x = ball->pos.x;
    at.y = -20.0f;
    at.z = lbl_5_data_15E8.z;
    eye.x += 5.0;
    at.x += -25.0;
    camera_approach(camera, &eye, &at, 0x1200, 0.05f);
}

static void fn_5_EB64(struct Camera *camera, struct Ball *ball)
{
    Vec eye;
    Vec at;
    eye.x = ball->pos.x;
    eye.y = 0.6f;
    eye.z = -11.86f;

    if (fn_5_169E0() == 8)
    {
        float offsets[3] = {0.0f, 6.5f, 13.0f};
        at.x = lbl_5_data_15E8.x + offsets[fn_5_16A20()];
        at.y = lbl_5_data_15E8.y - 7.0;
        at.z = lbl_5_data_15E8.z;
    }
    else
    {
        at.x = lbl_5_data_15E8.x;
        at.y = lbl_5_data_15E8.y - 7.0;
        at.z = lbl_5_data_15E8.z;
    }
    camera_approach(camera, &eye, &at, 0x1000, 0.05f);
}

static void fn_5_EE28(struct Camera *camera, struct Ball *ball)
{
    Vec eye;
    Vec at;
    eye.x = ball->pos.x;
    eye.y = 1.5f;
    eye.z = 10.0 + ball->pos.z;
    at.x = lbl_5_data_15E8.x;
    at.y = lbl_5_data_15E8.y - 4.0;
    at.z = lbl_5_data_15E8.z;

    camera_approach(camera, &eye, &at, 0x1400, 0.05f);
}

static void fn_5_F054(struct Camera *camera, struct Ball *ball)
{
    Vec eye;
    Vec at;

    if (ball->pos.x > 0.0f)
        eye.x = 3.4f;
    else
        eye.x = -3.4f;
    eye.y = -5.39f;
    eye.z = -15.86f;
    at = ball->pos;
    camera_approach(camera, &eye, &at, 0x1800, 0.05f);
}

static void fn_5_F280(struct Camera *camera, struct Ball *ball)
{
    Vec eye;
    Vec at;
    s16 fov;

    eye.x = 0.0f;
    eye.y = 2.0f;
    eye.z = 10.0f;
    at.x = ball->pos.x;
    at.y = 0.8 * ball->pos.y + 0.2 * lbl_5_data_15E8.y;
    at.z = ball->pos.z;
    fov = math_atan2(8.0f, mathutil_vec_distance(&eye, &at));
    if (fov > 0x800)
        fov = 0x800;
    camera_approach(camera, &eye, &at, fov, 1.0f);
    camera->unk1F = 11;
}

static void fn_5_F540(struct Camera *camera, struct Ball *ball)
{
    Vec eye;
    Vec at;
    s16 fov;

    eye.x = 0.0f;
    eye.y = 2.0f;
    eye.z = 10.0f;
    at.x = ball->pos.x;
    at.y = 0.8 * ball->pos.y + 0.2 * lbl_5_data_15E8.y;
    at.z = ball->pos.z;
    fov = math_atan2(8.0f, mathutil_vec_distance(&eye, &at));
    if (fov > 0x800)
        fov = 0x800;
    camera_approach(camera, &eye, &at, fov, 0.2f);
}

static void fn_5_F7F8(struct Camera *camera, struct Ball *ball)
{
    Vec eye;
    Vec at;
    eye.x = 0.0f;
    eye.y = 0.25f;
    eye.z = -110.4f;
    at.x = 0.0f;
    at.y = 0.9f;
    at.z = -99.0f;

    camera_approach(camera, &eye, &at, 0x1800, 1.0f);
}

static void fn_5_F9F4(struct Camera *camera, struct Ball *ball)
{
    Vec eye;
    Vec at;
    eye.x = -4.4f;
    eye.y = -0.3f;
    eye.z = -90.6f;
    at.x = 0.5f;
    at.y = 0.7f;
    at.z = -99.3f;

    camera_approach(camera, &eye, &at, 0x1800, 1.0f);
}

static void fn_5_FBF0(struct Camera *camera, struct Ball *ball)
{
    Vec eye;
    Vec at;
    s16 fov;

    eye.x = 6.0f;
    eye.y = 6.0f;
    eye.z = -40.0f;
    at = ball->pos;
    if (at.y < -0.39f)
        at.y = -0.39f;
    fov = math_atan2(10.0f, mathutil_vec_distance(&eye, &at));
    camera_approach(camera, &eye, &at, fov, 1.0f);
    camera->unk1F = 15;
}

static void fn_5_FE94(struct Camera *camera, struct Ball *ball)
{
    Vec eye;
    Vec at;
    s16 fov;

    eye.x = 6.0f;
    eye.y = 6.0f;
    eye.z = -40.0f;
    at = ball->pos;
    if (at.y < -5.0f && at.z < -98.01f)
    {
        if (at.x < -2.4f)
            at.x = -2.4f;
        if (at.x > 2.4f)
            at.x = 2.4f;
    }
    if (at.y < -0.39f)
        at.y = -0.39f;
    fov = math_atan2(10.0f, mathutil_vec_distance(&eye, &at));
    camera_approach(camera, &eye, &at, fov, 0.2f);
}

static void fn_5_10180(struct Camera *camera, struct Ball *ball)
{
    Vec eye;
    Vec at;
    float left;
    float right;
    float floorY;

    mtxa_from_translate(&ball->pos);
    mtxa_rotate_y(ball->rotY);
    mtxa_translate_xyz(0.0f, 0.1f, -1.0f);
    mathutil_mtxA_get_translate(&eye);
    if (eye.z < -97.010002136230469)
        eye.z = -97.01f;
    fn_5_17350(eye.z, &left, &right);
    left += 0.01f;
    right -= 0.01f;
    if (eye.x < left)
        eye.x = left;
    else if (eye.x > right)
        eye.x = right;
    eye.y = 10.0f;
    floorY = 0.5f + fn_5_16B64(&eye);
    eye.y = 0.1f + ball->pos.y;
    if (eye.y < floorY)
        eye.y = floorY;
    {
        Vec v = {0.0f, 0.77f, -106.629f};
        Vec *p = &v;
        at = *p;
    }
    camera_approach(camera, &eye, &at, 0x2AAA, 1.0f);
}

static void fn_5_104B8(struct Camera *camera, struct Ball *ball)
{
    camera_clear(camera);
    camera->unk26 = 9;
    camera->sub28.view.unk28 = 0.0f;
    camera->sub28.view.unk2C = 0.0f;
    camera->sub28.view.fov = 0x1C71;
    if (fn_1_E5E08() == 0)
        camera->timerCurr = fn_1_E5E44() * 0x110;
    else
        camera->timerCurr = 0;
    camera->unk1F = 18;
    fn_5_10560(camera, ball);
}

static void fn_5_10560(struct Camera *camera, struct Ball *ball)
{
    int count;
    int index;
    int frame;
    float t;
    Vec pos;
    Vec intro;

    if ((g_some_other_flags & 0xA) == 0 || camera->unk1F != 18)
    {
        count = fn_1_E5E44();
        if (count > 0)
        {
            if (camera->timerCurr < count * 0x110)
            {
                index = camera->timerCurr / 272;
                frame = camera->timerCurr - index * 0x110;
                if (count > 2)
                {
                    switch (index)
                    {
                    case 0:
                        fn_1_E5E78(1, &pos);
                        break;
                    case 1:
                        fn_1_E5E78(0, &pos);
                        break;
                    case 2:
                        fn_1_E5E78(2, &pos);
                        break;
                    case 3:
                        fn_1_E5E78(3, &pos);
                        break;
                    }
                }
                else
                    fn_1_E5E78(index, &pos);
                if (frame >= 0xCC)
                {
                    camera->eye.x = (double)pos.x;
                    camera->eye.y = 0.3 + pos.y;
                    camera->eye.z = pos.z - 1.2 - 0.5;
                    camera->rotX = 0;
                    camera->rotY = -0x8000;
                    camera->rotZ = 0;
                }
                else
                {
                    t = (float)frame / 204.0f;
                    t = (3.0 + -2.0 * t) * (t * t);
                    camera->eye.x = pos.x;
                    camera->eye.y = 0.3 * t + pos.y * (1.0 - 0.75 * (1.0 - t));
                    camera->eye.z = pos.z - 1.2 - 0.5;
                    camera->rotX = 0;
                    camera->rotY = -0x8000;
                    camera->rotZ = 0;
                }
            }
            else
            {
                int d = camera->timerCurr - count * 0x110;
                s16 delta;

                if (d < 8)
                    t = d / 8.0;
                else
                    t = 1.0f;
                t = 1.0 - (1.0 - t) * (1.0 - t);
                fn_1_E6384(&intro);
                camera->eye.x = intro.x;
                camera->eye.y = 0.7 + intro.y;
                camera->eye.z = intro.z - 2.0 - 3.0 * t;
                if (fn_5_113C8() != 0)
                {
                    delta = (0.05 * (0x900 - camera->rotX) < 48.0) ? 0.05 * (0x900 - camera->rotX) : 48.0;
                }
                else
                    delta = 0.015 * -camera->rotX;
                camera->rotX += delta;
                camera->rotY = -0x8000;
                camera->rotZ = 0;
                if (delta == 0)
                    g_some_render_flag |= 0x20;
                else
                    g_some_render_flag &= ~0x20;
            }
            fn_1_E678C(mtxa);
            mtxa_translate(&camera->eye);
            mtxa_rotate_y(camera->rotY);
            mtxa_rotate_x(camera->rotX);
            mtxa_rotate_z(camera->rotZ);
            mathutil_mtxA_get_translate(&camera->eye);
            mtxa_to_euler_yxz(&camera->rotY, &camera->rotX, &camera->rotZ);
            mtxa_tf_point_xyz(0.0f, 0.0f, -4.0f, &camera->lookAt);
            camera->timerCurr++;
            if (fn_5_10A50() != 0 && camera->timerCurr < count * 0x110 && camera->timerCurr > 10)
                camera->timerCurr = count * 0x110;
            if (camera->timerCurr > 0x880)
                camera->timerCurr = 0x880;
        }
    }
}

static int fn_5_10A50(void)
{
    if (g_poolInfo.playerPool.statusList[0] != 0
     && (pad_status_groups[player_pad_map[0]].unk0[2].button & 0x100))
        return 1;
    if (g_poolInfo.playerPool.statusList[1] != 0
     && (pad_status_groups[player_pad_map[1]].unk0[2].button & 0x100))
        return 1;
    if (g_poolInfo.playerPool.statusList[2] != 0
     && (pad_status_groups[player_pad_map[2]].unk0[2].button & 0x100))
        return 1;
    if (g_poolInfo.playerPool.statusList[3] != 0
     && (pad_status_groups[player_pad_map[3]].unk0[2].button & 0x100))
        return 1;
    return 0;
}
