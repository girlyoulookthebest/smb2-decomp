#include "baseball.h"

void baseball_1_dead(struct Ape *ape)
{
    fn_1_113190(ape, 0x16, 0.0f, 0, 0);
}

void fn_18_9F4(struct Ape *ape)
{
    struct Ball *ball = &balls[ape->ballIdx];
    struct RaycastHit hit;
    u16 b;
    u16 a;
    int r28;

    if (fn_18_334() != 1 && ape != NULL && stagedef != NULL)
    {
        raycast_stage_down(&ball->pos, &hit, 0);
        ape->flags &= ~0x13;
        if (!(hit.flags & 1) && ball->vel.y < -0.16203703f)
            ape->flags |= 2;
        else if (mathutil_vec_len(&ball->unkC0) < 0.00027777778f)
            ape->flags |= 1;
        r28 = (ball->flags & 0x1000) != 0;
        r28 |= (ape->flags & 3) == 0;
        g_ape_upright(ape);
        if (r28 != 0)
            fn_1_45A8C(ape);
        else
        {
            mtxa_from_quat(&ape->rot);
            mtxa_normalize_basis();
            if (ape->flags & 2)
                fn_1_46958(ape);
        }
        if (ball->flags & 0x20)
            mathutil_vec_len(&ball->vel);
        fn_1_45CA8(ape);
        mtxa_to_quat(&ape->rot);
        fn_18_1548(ball->playerId, &a, &b);
        fn_1_113190(ape, 0x16, 0.0f, a, b);
        g_ape_anim_body(ape);
        if (!(ape->flags & 8))
            g_move_ape_into_ball(ape);
        g_ape_anim_head(ape, &ball->unk10C);
    }
}

void fn_18_C4C(struct Ape *ape)
{
    struct Ball *ball = &balls[ape->ballIdx];
    struct RaycastHit hit;
    u16 b;
    u16 a;
    int r28;

    if (fn_18_334() != 1 && ape != NULL && stagedef != NULL)
    {
        raycast_stage_down(&ball->pos, &hit, 0);
        ape->flags &= ~0x13;
        if (!(hit.flags & 1) && ball->vel.y < -0.16203703f)
            ape->flags |= 2;
        else if (mathutil_vec_len(&ball->unkC0) < 0.00027777778f)
            ape->flags |= 1;
        r28 = (ball->flags & 0x1000) != 0;
        r28 |= (ape->flags & 3) == 0;
        g_ape_upright(ape);
        if (r28 != 0)
            fn_1_45A8C(ape);
        else
        {
            mtxa_from_quat(&ape->rot);
            mtxa_normalize_basis();
            if (ape->flags & 2)
                fn_1_46958(ape);
        }
        if (ball->flags & 0x20)
            mathutil_vec_len(&ball->vel);
        mtxa_to_quat(&ape->rot);
        fn_18_1548(ball->playerId, &a, &b);
        fn_1_113190(ape, 0x16, 0.0f, a, b);
        g_ape_anim_body(ape);
        if (!(ape->flags & 8))
            g_move_ape_into_ball(ape);
        g_ape_anim_head(ape, &ball->unk10C);
    }
}

