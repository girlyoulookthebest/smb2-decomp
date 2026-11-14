#include "baseball.h"

s32 fn_18_A1E4(void)
{
    u8 *p = lbl_18_bss_970.unk78;

    return p[5] == 1;
}

s32 fn_18_A210(s16 arg0)
{
    struct Ape *ape = balls[0].ape;

    if (ape->unk6C == 0x65 && ape->unk70 == arg0)
        return 1;
    return 0;
}

void fn_18_A248(s32 arg0)
{
    struct Ball *ball = balls;

    if (arg0 == 1)
    {
        ball->flags &= ~0x10;
        ball->ape->flags &= ~0x20;
        lbl_18_bss_2E0.unk44 |= 1;
    }
    else
    {
        ball->flags |= 0x10;
        ball->ape->flags |= 0x20;
        lbl_18_bss_2E0.unk44 &= ~1;
    }
}

void fn_18_A2F4(struct Ball *ball)
{
    struct PhysicsBall a;
    struct PhysicsBall b;

    init_physicsball_from_ball(ball, &a);
    init_physicsball_from_ball(&balls[1], &b);
    if (fn_18_1722C(&a, &b, 0.1f) == 1)
        apply_physicsball_to_ball(ball, &a);
}

