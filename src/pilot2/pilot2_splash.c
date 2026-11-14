/* Homing projectiles for the Monkey Target 2 minigame. */
#include "types.h"
#include "ball.h"
#include "effect.h"
#include "input.h"
#include "mathutil.h"
#ifndef __MWERKS__
/* pc_port: undeclared in the original; an implicit declaration passes float args as double on x86 */
int g_some_item_collision_check(Vec *prevPos, Vec *pos, float radius, Vec *a, Vec *b, float c);
#endif

struct Missile
{
    /*0x00*/ s8 active;
    /*0x01*/ s8 unk1;
    /*0x02*/ s8 owner;
    /*0x03*/ s8 target;
    u8 unk4[4];
    /*0x08*/ int timer;
    /*0x0C*/ Vec pos;
    /*0x18*/ Vec prevPos;
    /*0x24*/ Vec vel;
    /*0x30*/ Vec targetPos;
    /*0x3C*/ Mtx mtx;
    /*0x6C*/ float radius;
    /*0x70*/ float unk70;
    u8 unk74[8];
}; /* size = 0x7C */

extern u32 g_some_other_flags;
void PSVECScale(Vec *src, Vec *dst, float scale);

void fn_13_11804(struct Missile *missile);
void fn_13_11884(struct Missile *missile);
void fn_13_11B5C(struct Missile *missile);

static void (*lbl_13_data_1478[])(struct Missile *) = { fn_13_11804 };

struct Missile lbl_13_bss_388[0x40];

void fn_13_11730(void)
{
    int i;

    for (i = 0; i < 0x40; i++)
    {
        struct Missile *missile = &lbl_13_bss_388[i];

        missile->active = 0;
        missile->radius = 1.0f;
        missile->unk70 = 0.1f;
    }
}

void fn_13_11780(void)
{
    int i;

    if (!(g_some_other_flags & 0xA))
    {
        for (i = 0; i < 0x40; i++)
        {
            struct Missile *missile = &lbl_13_bss_388[i];

            if (missile->active != 0)
                lbl_13_data_1478[0](missile);
        }
    }
}

void fn_13_11804(struct Missile *missile)
{
    float dead1 = 0.0004629629838746041f;
    double dead2 = 0.0;
    float dead3 = 0.0f;
    float dead4 = 0.046296294778585434f;
    double dead5 = 0.707;

    if (++missile->timer > 0x708)
    {
        missile->active = 0;
        return;
    }
    missile->prevPos = missile->pos;
    fn_13_11884(missile);
    PSVECAdd(&missile->pos, &missile->vel, &missile->pos);
    fn_13_11B5C(missile);
}

void fn_13_11884(struct Missile *missile)
{
    Vec up = { 0.0f, 1.0f, 0.0f };
    Vec axis;
    Vec localAxis;
    Vec fwd;
    Vec dir;
    Vec tmp;
    Quat quat;
    float speed;
    float len;
    float var;
    float angle;
    struct Ball *ball;

    speed = mathutil_vec_len(&missile->vel);
    fwd.x = -missile->mtx[0][2];
    fwd.y = -missile->mtx[1][2];
    fwd.z = -missile->mtx[2][2];
    if (speed < 1.8518518209457397f)
    {
        PSVECScale(&fwd, &tmp, 0.0069444444961845875f);
        PSVECAdd(&missile->vel, &tmp, &missile->vel);
    }
    if (missile->target != -1)
    {
        ball = &balls[missile->target];
        if (missile->timer < 0xF)
        {
            missile->targetPos = ball->pos;
            return;
        }
        PSVECSubtract(&ball->pos, &missile->pos, &dir);
        vec_normalize_len(&dir);
        if (!(mathutil_vec_dot_prod(&fwd, &dir) < 0.707f))
        {
            PSVECAdd(&ball->pos, &missile->targetPos, &tmp);
            PSVECScale(&tmp, &tmp, 0.5f);
            PSVECSubtract(&tmp, &missile->pos, &tmp);
            len = mathutil_vec_len(&tmp);
            var = speed;
            if (var < 0.46296295523643494f)
                var = 0.46296295523643494f;
            len /= var;
            PSVECScale(&ball->vel, &tmp, len);
            PSVECAdd(&ball->pos, &tmp, &missile->targetPos);
            PSVECSubtract(&missile->targetPos, &missile->pos, &dir);
            vec_normalize_len(&dir);
            mathutil_vec_cross_prod(&fwd, &dir, &axis);
            angle = vec_normalize_len(&axis);
            mtxa_from_mtx(missile->mtx);
            mtxa_rigid_inv_tf_vec(&axis, &localAxis);
            quat_from_axis_angle(&quat, &localAxis, 480.0f * angle);
            mtxa_from_quat(&quat);
            mtxa_mult_left(missile->mtx);
            mtxa_sq_to_mtx(missile->mtx);
            PSVECScale(&fwd, &missile->vel, speed);
        }
    }
}

void fn_13_11B5C(struct Missile *missile)
{
    struct Effect effect;
    int i;
    struct Ball *ball;

    for (i = 0; i < 5; i++)
    {
        ball = &balls[i];
        if (ball->status == 2 && ball->playerId != missile->owner
         && g_some_item_collision_check(&ball->prevPos, &ball->pos, ball->currRadius,
                                        &missile->prevPos, &missile->pos, missile->radius) != 0)
        {
            call_SoundReqID_arg_0(0x10B);
            call_SoundReqID_arg_0(0x1C);
            memset(&effect, 0, sizeof(effect));
            effect.type = 0x27;
            effect.playerId = ball->playerId;
            effect.pos.x = ball->pos.x;
            effect.pos.y = ball->pos.y - 1.0f;
            effect.pos.z = ball->pos.z;
            effect.scale = (Vec){ 3.5f, 4.5f, 3.5f };
            spawn_effect(&effect);
            g_rumble_controller(player_pad_map[ball->playerId], 1, 0x14);
            missile->active = 0;
        }
    }
}
