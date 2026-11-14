#include "baseball.h"

void fn_18_E06C(u8 arg0)
{
    memset(&lbl_18_bss_970.unkBC[arg0], 0, sizeof(lbl_18_bss_970.unkBC[arg0]));
}

u8 fn_18_E0AC(void)
{
    struct BBGamePl *p = &lbl_18_bss_970.unkBC[2];

    return p->unk6;
}

void fn_18_E0C0(s32 arg0)
{
    s32 i;
    struct Ball *ball;

    for (i = 2; i <= 5; i++)
    {
        ball = &balls[i];
        if (arg0 == 1)
        {
            ball->flags &= ~0x10;
            ball->ape->flags &= ~0x20;
            lbl_18_bss_2E0.unk11C |= 1;
        }
        else
        {
            ball->flags |= 0x10;
            ball->ape->flags |= 0x20;
            lbl_18_bss_2E0.unk11C &= ~1;
        }
    }
}

void fn_18_E17C(struct Ball *ball)
{
    struct Camera *cam = cameras;
    s16 rotX;
    s16 rotY;

    mtxa_from_translate(&cam->eye);
    mtxa_rotate_z(cam->rotZ);
    mtxa_rotate_y(cam->rotY);
    mtxa_rotate_x(cam->rotX);
    if (ball->playerId == 0)
    {
        ball->pos.x = -1.2f;
        ball->pos.y = -0.3f;
        ball->pos.z = -3.0f;
        mtxa_tf_point(&ball->pos, &ball->pos);
        fn_18_146C(ball->playerId, 1, 4);
    }
    else
    {
        ball->pos.x = 1.2f;
        ball->pos.y = -0.3f;
        ball->pos.z = -3.0f;
        mtxa_tf_point(&ball->pos, &ball->pos);
        fn_18_146C(ball->playerId, 0, 0);
    }
    ray_to_euler_xy(&cam->eye, &ball->pos, &rotX, &rotY);
    mtxa_from_identity();
    mtxa_rotate_y(rotY);
    mtxa_rotate_x(rotX);
    mtxa_rotate_z(0);
    mtxa_rotate_y(-0x8000);
    mtxa_to_quat(&ball->ape->rot);
    ball->prevPos = ball->pos;
    ball->vel.x = 0.0f;
    ball->vel.y = 0.0f;
    ball->vel.z = 0.0f;
    ball->rotX = 0x2000;
    ball->rotY = -0x4000;
    ball->rotZ = 0;
    mtxa_from_translate(&ball->pos);
    mtxa_rotate_y(ball->rotY);
    mtxa_rotate_x(ball->rotX);
    mtxa_rotate_z(ball->rotZ);
    mtxa_to_mtx(ball->unk30);
    mtxa_to_mtx(ball->unkD0);
    ball->speed = ball->unkCC = 0.0f;
    ball->unkC0 = (Vec){0.0f, 0.0f, 0.0f};
    ball->ape->flags &= ~0x4000;
    ball->unkB0 = (Quat){0.0f, 0.0f, 0.0f, 1.0f};
    ball->unkA0 = ball->unkB0;
    fn_18_E540(0);
}

void fn_18_E3E4(void)
{
}

void fn_18_E3E8(struct Ball *ball)
{
    struct PhysicsBall phys;
    Vec v;
    struct Camera *cam = cameras;

    switch (lbl_18_bss_1080)
    {
    case 0:
        break;
    case 1:
        mtxa_from_translate(&cam->eye);
        mtxa_rotate_z(cam->rotZ);
        mtxa_rotate_y(cam->rotY);
        mtxa_rotate_x(cam->rotX);
        if (ball->playerId == 0)
        {
            v.x = -1.0f;
            v.y = 0.0f;
            v.z = 0.0f;
        }
        else
        {
            v.x = 1.0f;
            v.y = 0.0f;
            v.z = 0.0f;
        }
        mtxa_tf_point(&v, &v);
        v.x -= cam->eye.x;
        v.y -= cam->eye.y;
        v.z -= cam->eye.z;
        vec_set_len(0.06f, &v, &ball->vel);
        break;
    case 2:
        break;
    }
    PSVECAdd(&ball->pos, &ball->vel, &ball->pos);
    init_physicsball_from_ball(ball, &phys);
    fn_1_4AF48(ball, &phys);
    position_ball(ball, &phys);
}

