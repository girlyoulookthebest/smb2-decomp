/* Ball physics: per-frame update for each ball state, and lane collision response. */
#include "types.h"
#include "ball.h"
#include "camera.h"
#include "input.h"
#include "mathutil.h"
#include "mode.h"
#include "rand.h"
#include "sound.h"
#ifndef __MWERKS__
/* pc_port: the original calls these undeclared; on x86 an implicit
 * declaration passes the float argument as a double. */
void PSVECAdd(Vec *a, Vec *b, Vec *out);
void PSVECSubtract(Vec *a, Vec *b, Vec *out);
void PSVECScale(Vec *in, Vec *out, float scale);
#endif

void g_reset_ball(struct Ball *ball);
void mtxa_to_quat(Quat *q);
void call_SoundReqID_arg_0(int soundId);
int call_SoundReqID_arg_2(u32 soundId);
int call_SoundReqID_arg_1_copy(u32 soundId);
void SoundVol(u16 soundId, u8 vol);
void g_smth_with_ape_rotation(struct Ape *ape, Vec *arg1);
void g_rumble_controller(int pad, int cmd, int dur);
void fn_5_11A48(void);
float fn_5_16D30(Vec *pos, Vec *prevPos, float radius, Vec *a, float *b, int *c);

static void fn_5_C948(struct Ball *ball);
static void fn_5_CB90(struct Ball *ball);
static void fn_5_CE68(struct Ball *ball);
static void fn_5_CFBC(struct Ball *ball);
static void fn_5_D0F8(struct Ball *ball);
static int fn_5_D2C8(struct Ball *ball);

int lbl_5_data_15E0 = -1;
int lbl_5_data_15E4 = -1;

struct Ball lbl_5_bss_1E0[3];
Quat lbl_5_bss_6F0[3];

void fn_5_C734(struct Ball *ball)
{
    switch (ball->unk154)
    {
    case 0:
        fn_5_C948(ball);
        break;
    case 1:
        fn_5_CB90(ball);
        break;
    case 2:
        fn_5_CE68(ball);
        break;
    case 3:
        fn_5_CFBC(ball);
        break;
    case 4:
        fn_5_D0F8(ball);
        break;
    }
    if (ball->pos.y < -5.0 && !(ball->flags & 0x1000ULL) && lbl_5_data_15E4 == -1)
    {
        call_SoundReqID_arg_0(0x1D);
        lbl_5_data_15E4 = call_SoundReqID_arg_2(0x15);
    }
    ball->unk80++;
}

void fn_5_C850(int slot, struct Ball *ball)
{
    if (slot > -1 && slot < 3)
    {
        lbl_5_bss_1E0[slot] = *ball;
        lbl_5_bss_6F0[slot] = ball->ape->rot;
    }
}

void fn_5_C8CC(int slot, struct Ball *ball)
{
    if (slot > -1 && slot < 3)
    {
        *ball = lbl_5_bss_1E0[slot];
        ball->ape->rot = lbl_5_bss_6F0[slot];
    }
}

static void fn_5_C948(struct Ball *ball)
{
    Vec dir;

    g_reset_ball(ball);
    ball->pos.x = 0.0f;
    ball->pos.y = 0.11f;
    ball->pos.z = 0.0f;
    ball->prevPos = ball->pos;
    ball->vel.x = 0.0f;
    ball->vel.y = 0.0f;
    ball->vel.z = 0.0f;
    ball->rotX = 0;
    ball->rotY = 0;
    ball->rotZ = 0;
    ball->unk60.x = 0;
    ball->unk60.y = 0;
    ball->unk60.z = 0;
    ball->flags &= ~0x10ULL;
    ball->ape->flags &= ~0x4020;
    ball->speed = 0.0f;
    ball->unkCC = 0.0f;
    {
        Vec v = {0.0f, 0.0f, 0.0f};
        Vec *p = &v;
        ball->unkC0 = *p;
    }
    mtxa_from_identity();
    mtxa_to_quat(&ball->unkA0);
    mtxa_rotate_z(ball->rotZ);
    mtxa_rotate_y(ball->rotY);
    mtxa_rotate_x(ball->rotX);
    mtxa_to_mtx(ball->unk30);
    mtxa_to_mtx(ball->unkD0);
    mtxa_to_quat(&ball->unkB0);
    if (modeCtrl.playerCount == 1)
        ball->colorId = 3;
    else
        ball->colorId = ball->playerId;
    dir.x = 0.0f;
    dir.y = 0.0f;
    dir.z = -1.0f;
    g_smth_with_ape_rotation(ball->ape, &dir);
    if (lbl_5_data_15E0 != -1)
    {
        SoundOff(lbl_5_data_15E0);
        lbl_5_data_15E0 = -1;
    }
    if (lbl_5_data_15E4 != -1)
    {
        SoundOff(lbl_5_data_15E4);
        lbl_5_data_15E4 = -1;
    }
    fn_5_11A48();
    ball->unk80 = 0;
    ball->state = 0x19;
    ball->unk154 = 2;
}

static void fn_5_CB90(struct Ball *ball)
{
    int contact;

    ball->prevPos = ball->pos;
    ball->speed = mathutil_vec_len(&ball->vel);
    mtx_copy(ball->unk30, ball->unkD0);
    PSVECAdd(&ball->vel, &ball->pos, &ball->pos);
    ball->rotX += ball->unk60.x;
    ball->rotY += ball->unk60.y;
    ball->rotZ += ball->unk60.z;
    contact = fn_5_D2C8(ball);
    if (ball->flags & 1ULL)
    {
        float f = -math_sin(ball->rotY * 2) * (0.0004 + 0.0003 * ((float)abs(ball->rotZ) / 6144.0f));
        if (contact == 3)
            f *= 0.2;
        ball->vel.x += f;
    }
    ball->vel.y -= ball->accel;
    if (ball->vel.z > -0.05)
        ball->vel.z = -0.05f;
    mtxa_from_rotate_z(ball->rotZ);
    mtxa_rotate_y(ball->rotY);
    mtxa_rotate_x(ball->rotX);
    mtxa_to_quat(&ball->unkA0);
    mathutil_mtxA_set_translate(&ball->pos);
    mtxa_to_mtx(ball->unk30);
    mtxa_to_quat(&ball->unkB0);
    if (ball->pos.z < -104.34f && ball->pos.y > -0.83 && ball->pos.x > -3.49f && ball->pos.x < 3.49f)
        ball->flags |= 0x1000ULL;
    if (ball->ape->unk250 == 10 && !(ball->unk80 & 0xF))
    {
        int sounds[8] = {0x5A, 0x5B, 0x5C, 0x5D, 0x5E, 0x5F, 0x60, 0x61};
        call_SoundReqID_arg_0(sounds[rand() & 7]);
    }
}

static void fn_5_CE68(struct Ball *ball)
{
    Vec dir;

    ball->prevPos = ball->pos;
    ball->speed = mathutil_vec_len(&ball->vel);
    mtx_copy(ball->unk30, ball->unkD0);
    ball->vel.y -= ball->accel;
    PSVECAdd(&ball->vel, &ball->pos, &ball->pos);
    ball->rotX += ball->unk60.x;
    ball->rotY += ball->unk60.y;
    ball->rotZ += ball->unk60.z;
    mtxa_from_rotate_z(ball->rotZ);
    mtxa_rotate_y(ball->rotY);
    mtxa_rotate_x(ball->rotX);
    mtxa_to_quat(&ball->unkA0);
    mathutil_mtxA_set_translate(&ball->pos);
    mtxa_to_mtx(ball->unk30);
    mtxa_to_quat(&ball->unkB0);
    fn_5_D2C8(ball);
    dir.x = 0.0f;
    dir.y = 0.0f;
    dir.z = -1.0f;
    g_smth_with_ape_rotation(ball->ape, &dir);
}

static void fn_5_CFBC(struct Ball *ball)
{
    ball->pos.x = 0.0f;
    ball->pos.y = 11.0f;
    ball->pos.z = -100.0f;
    ball->prevPos.x = 0.0f;
    ball->prevPos.y = 11.0f;
    ball->prevPos.z = -100.0f;
    ball->vel.x = 0.0f;
    ball->vel.y = -3.0 * ball->accel;
    ball->vel.z = 1.0f;
    ball->rotX = 0;
    ball->rotY = 5;
    ball->rotZ = 0;
    ball->unk60.x = 0x1000;
    ball->unk60.y = 0;
    ball->unk60.z = 0;
    ball->speed = 0.0f;
    if (lbl_5_data_15E0 != -1)
    {
        SoundOff(lbl_5_data_15E0);
        lbl_5_data_15E0 = -1;
    }
    if (lbl_5_data_15E4 != -1)
    {
        SoundOff(lbl_5_data_15E4);
        lbl_5_data_15E4 = -1;
    }
    ball->unk80 = 0;
    ball->unk154 = 4;
}

static void fn_5_D0F8(struct Ball *ball)
{
    Vec dir;

    ball->prevPos = ball->pos;
    ball->speed = mathutil_vec_len(&ball->vel);
    mtx_copy(ball->unk30, ball->unkD0);
    ball->vel.y -= 0.03 * ball->accel;
    ball->vel.z *= 0.99;
    ball->unk60.x *= 0.99;
    ball->pos.y += ball->vel.y;
    ball->pos.z += ball->vel.z;
    ball->rotX += ball->unk60.x;
    ball->rotY += ball->unk60.y;
    ball->rotZ += ball->unk60.z;
    mtxa_from_rotate_z(ball->rotZ);
    mtxa_rotate_y(ball->rotY);
    mtxa_rotate_x(ball->rotX);
    mtxa_to_quat(&ball->unkA0);
    mathutil_mtxA_set_translate(&ball->pos);
    mtxa_to_mtx(ball->unk30);
    mtxa_to_quat(&ball->unkB0);
    fn_5_D2C8(ball);
    PSVECSubtract(&cameras[ball->playerId].eye, &ball->pos, &dir);
    g_smth_with_ape_rotation(ball->ape, &dir);
}

static int fn_5_D2C8(struct Ball *ball)
{
    Vec normal;
    Vec a;
    Vec b;
    Vec push;
    float rest;
    int contact;
    float depth;
    float d;
    float e;
    float t;
    float s;
    float vol;
    u8 volume;
    s8 pan;
    int rumble;

    depth = fn_5_16D30(&ball->pos, &ball->prevPos, ball->currRadius, &normal, &rest, &contact);
    if (depth <= 0.0f)
    {
        if (depth < -0.01)
            ball->flags &= ~1ULL;
        if (lbl_5_data_15E0 != -1)
        {
            SoundOff(lbl_5_data_15E0);
            lbl_5_data_15E0 = -1;
        }
        return contact;
    }
    d = mathutil_vec_dot_prod(&normal, &ball->vel);
    PSVECScale(&normal, &a, d);
    PSVECSubtract(&ball->vel, &a, &b);
    e = -1.0 - ball->restitution * rest;
    ball->vel.x += e * a.x - 0.001f * b.x;
    ball->vel.y += e * a.y - 0.001f * b.y;
    ball->vel.z += e * a.z - 0.001f * b.z;
    t = (__fabs(d) < 0.15) ? __fabs(d) : 0.15;
    if (t > 0.03 && lbl_5_data_15E0 == -1 && ((u32)contact <= 1 || contact == 9 || contact == 10))
        call_SoundReqID_arg_0(0xA09B);
    if (t > 0.1 && lbl_5_data_15E0 == -1)
    {
        s = (t - 0.1) / (0.15 - 0.1);
        pan = -15.0f + 10.0f * s;
        rumble = (25.0f * s < 25.0f) ? 25.0f * s : 25.0f;
        call_SoundReqID_arg_0(((pan & 0x7F) << 11) | 0x99);
        if (ball->unk154 != 4)
        {
            if (t > 0.1333)
                call_SoundReqID_arg_0(0x1A);
            else if (t > 0.1167)
                call_SoundReqID_arg_0(0x18);
            else
                call_SoundReqID_arg_0(0x17);
        }
        if (rumble >= 2)
            g_rumble_controller(player_pad_map[modeCtrl.currPlayer], 1, rumble);
    }
    if (lbl_5_data_15E0 == -1)
        lbl_5_data_15E0 = call_SoundReqID_arg_1_copy(0x9A);
    else
    {
        vol = (mathutil_vec_len(&ball->vel) < 0.4) ? mathutil_vec_len(&ball->vel) : 0.4;
        volume = 127.0f * vol / 0.4;
        SoundVol(lbl_5_data_15E0, volume);
    }
    PSVECScale(&normal, &push, depth);
    PSVECAdd(&push, &ball->pos, &ball->pos);
    ball->flags |= 1ULL;
    return contact;
}
