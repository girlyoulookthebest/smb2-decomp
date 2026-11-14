#include "ball.h"
#include "camera.h"
#include "mathutil.h"
#include "mode.h"
#include "stage.h"
#include "world.h"
#include "ape_internal.h"

int disc_queue_status(int a);
void call_SoundReqID_arg_0(int a);
void g_reset_ball(struct Ball *ball);
void g_maybe_sets_number_of_starting_monkeys(struct Ball *ball);
void set_ape_anim(struct Ape *ape, int anim);
void handle_ball_stage_coli(struct Ball *ball, struct PhysicsBall *physBall);
void fn_1_4AF48(struct Ball *ball, struct PhysicsBall *physBall);
void position_ball(struct Ball *ball, struct PhysicsBall *physBall);
void mtxa_to_quat(Quat *q);

#define FLT_EPSILON 1.1920928955078125e-07f

void g_challenge_mode_start(struct Ball *ball);
void apply_velocity(struct Ball *ball, struct PhysicsBall *physBall);

void fn_2_7198(void)
{
    disc_queue_status(5);
}

void fn_2_71BC(struct Ball *ball)
{
    g_maybe_sets_number_of_starting_monkeys(ball);
    g_challenge_mode_start(ball);
}

void g_challenge_mode_start(struct Ball *ball)
{

    g_reset_ball(ball);
    ball->pos.x = stagedef->startPos->pos.x;
    ball->pos.y = stagedef->startPos->pos.y + 0.5 * (24.0 * (24.0 * ball->accel));
    ball->pos.z = stagedef->startPos->pos.z;
    ball->prevPos.x = ball->pos.x;
    ball->prevPos.y = ball->pos.y;
    ball->prevPos.z = ball->pos.z;
    mtxa_from_translate(&ball->pos);
    mtxa_to_mtx(ball->unk30);
    ball->flags |= 0x10;
    ball->ape->flags |= 0x20;
    ball->state = 0;
    ball->speed = ball->unkCC = 0.0f;
    {
        Vec vec = { 0.0f, 0.0f, 0.0f };
        Vec *pv = &vec;

        ball->unkC0 = *pv;
    }
    ball->ape->flags &= ~0x4000;
    {
        Quat quat = { 0.0f, 0.0f, 0.0f, 1.0f };
        Quat *pq = &quat;

        ball->unkB0 = *pq;
    }
    ball->unkA0 = ball->unkB0;
}

void fn_2_73B4(struct Ball *ball)
{
    float t;
    float dy;

    ball->prevPos.x = ball->pos.x;
    ball->prevPos.y = ball->pos.y;
    ball->prevPos.z = ball->pos.z;
    t = modeCtrl.submodeTimer;
    dy = stagedef->startPos->pos.y - ball->pos.y;
    ball->vel.x = 0.0f;
    ball->vel.y = dy / t + 0.5 * (ball->accel * t);
    ball->vel.z = 0.0f;
    ball->rotX = 0x2000;
    ball->rotY = (s16)(stagedef->startPos->yrot - 0x4000);
    ball->rotZ = 0;
    mtxa_from_translate(&ball->pos);
    mtxa_rotate_y(ball->rotY);
    mtxa_rotate_x(ball->rotX);
    mtxa_rotate_z(ball->rotZ);
    mtxa_to_mtx(ball->unk30);
    mtxa_to_mtx(ball->unkD0);
    ball->flags &= ~0x10ull;
    ball->flags |= 0x4000;
    if (ball->ape != NULL)
        ball->ape->flags &= ~0x20;
    call_SoundReqID_arg_0(0x1E);
    ball->state = 0x20;
    ball->speed = ball->unkCC = 0.0f;
    {
        Vec vec = { 0.0f, 0.0f, 0.0f };
        Vec *pv = &vec;

        ball->unkC0 = *pv;
    }
    ball->ape->flags &= ~0x4000;
    {
        Quat quat = { 0.0f, 0.0f, 0.0f, 1.0f };
        Quat *pq = &quat;

        ball->unkA0 = *pq;
    }
    mtxa_to_quat(&ball->unkB0);
    set_ape_anim(ball->ape, 5);
}

void challenge_mode_physics(struct Ball *ball)
{
    struct PhysicsBall physBall;

    apply_velocity(ball, &physBall);
    handle_ball_stage_coli(ball, &physBall);
    fn_1_4AF48(ball, &physBall);
    position_ball(ball, &physBall);
    ball->unk80++;
}

static void apply_accel(struct Ball *ball)
{
    Vec accel;
    Vec vel;
    Quat quat;

    accel.x = 0.0f;
    accel.y = -ball->accel;
    accel.z = 0.0f;
    if ((ball->flags & 0x200) != 0)
        accel.y = -accel.y;
    else if ((ball->flags & 0x100) != 0)
        accel.y = 0.0f;
    if ((ball->prevFlags & 1) != 0)
    {
        float f;

        vel = ball->vel;
        f = mathutil_sum_of_sq_2(vel.x, vel.z);
        if (f > FLT_EPSILON)
        {
            float dot;
            float scale;

            scale = 1.0 / math_sqrt(f);
            vel.x *= scale;
            vel.z *= scale;
            dot = vel.x * -math_sin(cameras[ball->playerId].rotY)
                + vel.z * -math_sin(cameras[ball->playerId].rotY + 0x4000);
            if (dot < 0.0)
            {
                scale = 1.0 + -0.65 * -dot;
                accel.x *= scale;
                accel.y *= scale;
                accel.z *= scale;
            }
        }
    }
    {
        Vec down = { 0.0f, -1.0f, 0.0f };

        quat_from_dirs(&quat, &down, &world_infos[ball->playerId].unk10);
    }
    mtxa_from_quat(&quat);
    mtxa_tf_vec(&accel, &accel);
    ball->vel.x += accel.x;
    ball->vel.y += accel.y;
    ball->vel.z += accel.z;
}

void apply_velocity(struct Ball *ball, struct PhysicsBall *physBall)
{
    Vec up;

    ball->prevPos.x = ball->pos.x;
    ball->prevPos.y = ball->pos.y;
    ball->prevPos.z = ball->pos.z;
    ball->speed = mathutil_vec_len(&ball->vel);
    ball->flags &= ~0x20ull;
    mtxa_from_identity();
    mtxa_rotate_x(world_infos[ball->playerId].xrot);
    mtxa_rotate_z(world_infos[ball->playerId].zrot);
    up.x = 0.0f;
    up.y = 1.0f;
    up.z = 0.0f;
    mtxa_tf_vec(&up, &up);
    apply_accel(ball);
    ball->pos.x += ball->vel.x;
    ball->pos.y += ball->vel.y;
    ball->pos.z += ball->vel.z;
}

void g_smth_with_mystery_3(struct Ball *ball)
{
    g_reset_ball(ball);
    ball->flags |= 0x10;
    if (stagedef->unkD4 != NULL)
    {
        ball->prevPos = ((struct StartPos *)stagedef->unkD4)->pos;
        ball->pos = ball->prevPos;
        ball->rotX = ((struct StartPos *)stagedef->unkD4)->xrot;
        ball->rotY = ((struct StartPos *)stagedef->unkD4)->yrot;
        ball->rotZ = ((struct StartPos *)stagedef->unkD4)->zrot;
    }
    ball->pos.y += 0.32;
    ball->prevPos.y += 0.32;
    ball->rotX += 0x2000;
    mtxa_from_translate(&ball->pos);
    mtxa_rotate_y(ball->rotY);
    mtxa_rotate_x(ball->rotX);
    mtxa_rotate_z(ball->rotZ);
    mtxa_to_mtx(ball->unk30);
    mtxa_to_mtx(ball->unkD0);
    ball->state = 0;
    ball->speed = ball->unkCC = 0.0f;
    {
        Vec vec = { 0.0f, 0.0f, 0.0f };
        Vec *pv = &vec;

        ball->unkC0 = *pv;
    }
    ball->ape->flags &= ~0x4000;
    ball->ape->flags |= 0x1000000;
    mtxa_to_quat(&ball->ape->rot);
    mtxa_to_quat(&ball->unkA0);
    mtxa_to_quat(&ball->unkB0);
}
