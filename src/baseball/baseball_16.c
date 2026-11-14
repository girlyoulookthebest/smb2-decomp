#include "baseball.h"

void fn_18_E540(u8 arg0)
{
    lbl_18_bss_1080 = arg0;
}

void fn_18_E550(struct Ball *ball)
{
    struct Camera *cam = cameras;
    s16 rotX;
    s16 rotY;
    struct BBLift *p = &lbl_18_bss_970.unk1F8;

    p->unkC = (Vec){0.0f, 5.5f, -5.0f};
    mtxa_from_translate(&cam->eye);
    mtxa_rotate_z(cam->rotZ);
    mtxa_rotate_y(cam->rotY);
    mtxa_rotate_x(cam->rotX);
    mtxa_tf_point(&p->unkC, &ball->pos);
    fn_18_146C(ball->playerId, 1, 4);
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
}

void fn_18_E794(void)
{
}

void fn_18_E798(struct Ball *ball)
{
    struct PhysicsBall phys;

    fn_18_E8C8(ball);
    init_physicsball_from_ball(ball, &phys);
    fn_1_4AF48(ball, &phys);
    position_ball(ball, &phys);
}

s32 fn_18_E7EC(void)
{
    struct BBLift *p = &lbl_18_bss_970.unk1F8;

    if (p->unkC.y > 0.6)
    {
        p->unkC.y -= 0.1f;
        return 0;
    }
    p->unkC.y = 0.6f;
    return 1;
}

s32 fn_18_E838(void)
{
    struct BBLift *p = &lbl_18_bss_970.unk1F8;
    Vec w = (Vec){0.0f, 5.5f, -5.0f};

    if (w.y > p->unkC.y)
    {
        p->unkC.y += 0.1f;
        return 0;
    }
    p->unkC.y = w.y;
    return 1;
}

