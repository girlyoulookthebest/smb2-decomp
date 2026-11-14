/* Bowling pins: rigid bodies built from seven spheres, colliding with the lane, the ball and each other. */
#include "types.h"
#include "avdisp.h"
#include "ball.h"
#include "camera.h"
#include "dip.h"
#include "gx.h"
#include "input.h"
#include "mathutil.h"
#include "mode.h"
#include "rand.h"

struct PinSphere
{
    Vec pos;
    float radius;
    float mass;
};

struct PinConfig
{
    u32 allContacts;
    Vec center;
    Vec invInertia;
    float mass;
    float restitution;
    float friction;
    s32 modelId;
    s32 unk2C;
    const struct PinSphere *spheres;
    u8 sphereCount;
};

struct Pin
{
    u32 flags;
    Vec spherePos[7];
    Vec prevSpherePos[7];
    Vec pos;
    Vec vel;
    Vec angVel;
    Mtx rot;
    u32 contacts;
    u32 prevContacts;
    s16 unk108;
};

enum
{
    PIN_ACTIVE = 1 << 0,
    PIN_RESTING = 1 << 1,
    PIN_HIT = 1 << 2,
    PIN_HELD = 1 << 3,
    PIN_LIFTED = 1 << 4,
    PIN_SWEPT = 1 << 5,
};

extern struct GMA *minigameGma[];
extern u32 g_some_render_flag;

struct Pin lbl_5_bss_19170[10];
struct Pin lbl_5_bss_19BE8[3][10];

void PSVECAdd(Vec *a, Vec *b, Vec *out);
void PSVECSubtract(Vec *a, Vec *b, Vec *out);
void PSVECScale(Vec *in, Vec *out, float scale);
void GXLoadPosMtxImm(Mtx mtx, u32 id);
void GXLoadNrmMtxImm(Mtx mtx, u32 id);
void mtx_from_identity(Mtx mtx);
int fn_1_A74E4(Vec *a, Vec *b, float ra, Vec *c, Vec *d, float rb);
void g_math_unk7(Quat *quat, float radius, Vec *out, Vec *in);
int call_SoundReqID_arg_1_copy(u32 soundId);
void call_SoundReqID_arg_0(int soundId);
void SoundDop(u16 soundId, u16 pitch);
void SoundPan(u16 soundId, s8 pan, s8 span);
void g_rumble_controller(int pad, int cmd, int dur);
float fn_5_16D30(Vec *pos, Vec *prevPos, float radius, Vec *normal, float *b, int *c);

void fn_5_12CA4(struct Pin *pin);
void fn_5_12D9C(struct Pin *pin);
void fn_5_131F0(Vec *vel, Vec *normal, float restitution, float friction, Vec *out);
void fn_5_132F0(struct Pin *pin, Vec *impulse, int index);
void fn_5_134A4(struct Pin *pin, int index, Vec *impulse, Vec *out);
void fn_5_135E0(struct Pin *pin, struct Ball *ball);
void fn_5_13DDC(int index);
void fn_5_14730(struct Pin *pin, Vec *offset);
int fn_5_147D4(float *depth, Vec *prevA, Vec *posA, float radiusA, Vec *prevB, Vec *posB, float radiusB);
void fn_5_148AC(float *closest, float *end, Vec *prevA, Vec *posA, Vec *prevB, Vec *posB);
void fn_5_14A80(Vec *pos, const Vec *listener, s16 rotY, u8 *pan, u8 *span);

static const struct PinSphere pinSpheres[7] = {
    {{0.0f, 1.58f, 0.0f}, 0.16f, 0.4f},
    {{0.0f, 1.58f, 0.0f}, 0.16f, 0.4f},
    {{0.0f, 1.3f, 0.0f}, 0.12f, 0.3f},
    {{0.0f, 1.06f, 0.0f}, 0.12f, 0.3f},
    {{0.0f, 0.7f, 0.0f}, 0.24f, 0.4f},
    {{0.0f, 0.5f, 0.0f}, 0.275f, 0.5f},
    {{0.0f, 0.2f, 0.0f}, 0.2f, 0.8f},
};

static const struct PinConfig pinConfig = {
    0x40,
    {0.0f, 0.8f, 0.0f},
    {2.3076923f, 16.666666f, 2.3076923f},
    1.644f,
    0.7f,
    0.04f,
    2,
    2,
    pinSpheres,
    7,
};

static const Vec pinListener = {0.0f, 0.8f, -102.18229f};

void fn_5_12234(void)
{
    int i;

    for (i = 0; i < 10; i++)
        lbl_5_bss_19170[i].flags = 0;
}

void fn_5_12264(struct Ball *ball)
{
    int i;
    struct Pin *pin;

    if ((g_some_other_flags & 0xA) == 0)
    {
        for (i = 0, pin = lbl_5_bss_19170; i < 10; i++, pin++)
        {
            if (pin->flags != 0 && (pin->flags & (PIN_HELD | PIN_LIFTED)) == 0)
            {
                if ((pin->flags & PIN_HIT) && pin->pos.y < -5.0f)
                    pin->flags = 0;
                if ((pin->flags & PIN_RESTING) == 0)
                {
                    float sq;
                    float scale;

                    PSVECScale(&pin->vel, &pin->vel, 0.99f);
                    PSVECScale(&pin->angVel, &pin->angVel, 0.99f);
                    pin->vel.y -= 0.012495;
                    PSVECAdd(&pin->vel, &pin->pos, &pin->pos);
                    sq = mathutil_vec_sq_len(&pin->angVel);
                    if (sq > 16000000.0)
                    {
                        scale = 4000.0 * math_rsqrt(sq);
                        PSVECScale(&pin->angVel, &pin->angVel, scale);
                    }
                    mtxa_from_rotate_y((s16)pin->angVel.y);
                    mtxa_rotate_x((s16)pin->angVel.x);
                    mtxa_rotate_z((s16)pin->angVel.z);
                    mtxa_mult_right(pin->rot);
                    mtxa_to_mtx(pin->rot);
                    fn_5_12CA4(pin);
                }
                fn_5_135E0(pin, ball);
                fn_5_13DDC(i);
                if ((pin->flags & PIN_RESTING) == 0)
                    fn_5_12D9C(pin);
            }
        }
    }
}

void fn_5_12448(void)
{
    int i;
    struct Pin *pin;

    for (i = 0, pin = lbl_5_bss_19170; i < 10; i++, pin++)
    {
        if (pin->flags != 0 && ((g_some_render_flag & 4) == 0 || !(pin->pos.y < -0.665)))
        {
            mtxa_from_mtxb();
            mtxa_translate(&pin->pos);
            mtxa_mult_right(pin->rot);
            mtxa_translate_xyz(-pinConfig.center.x, -pinConfig.center.y, -pinConfig.center.z);
            GXLoadPosMtxImm(mtxa->mtxA, 0);
            GXLoadNrmMtxImm(mtxa->mtxA, 0);
            avdisp_draw_model_culled_sort_auto(minigameGma[0]->modelEntries[pinConfig.modelId].model);
        }
    }
}

void fn_5_1256C(Vec *pos, s8 index)
{
    struct Pin *pin = &lbl_5_bss_19170[index];

    if (index >= 0 && index < 10)
    {
        pin->flags = 3;
        pin->contacts = 0;
        pin->prevContacts = 0;
        pin->pos = *pos;
        pin->vel.x = 0.0f;
        pin->vel.y = 0.0f;
        pin->vel.z = 0.0f;
        mtx_from_identity(pin->rot);
        pin->angVel.x = 0.0f;
        pin->angVel.y = 0.0f;
        pin->angVel.z = 0.0f;
        pin->unk108 = 0;
        fn_5_12CA4(pin);
        fn_5_12CA4(pin);
    }
}

void fn_5_1266C(void)
{
    int i;
    struct Pin *pin;

    for (i = 0, pin = lbl_5_bss_19170; i < 10; i++, pin++)
    {
        if ((pin->flags & PIN_ACTIVE) == 0 || (pin->flags & PIN_HIT))
            pin->flags = 0;
        else
            pin->flags = 3;
    }
}

int fn_5_126C8(s8 *outCount)
{
    int mask = 0;
    s8 count = 0;
    int i;
    struct Pin *pin;

    for (i = 0, pin = lbl_5_bss_19170; i < 10; i++, pin++)
    {
        if (pin->flags & PIN_ACTIVE)
        {
            pin->flags &= ~PIN_LIFTED;
            if ((pin->flags & PIN_HIT) == 0)
            {
                mask |= 1 << i;
                count++;
                pin->flags |= PIN_RESTING;
            }
            else if (pin->contacts == pinConfig.allContacts && (pin->flags & PIN_RESTING)
                  && pin->pos.x < 2.39 && pin->pos.x > -2.39)
            {
                mask |= 1 << i;
                count++;
                pin->flags |= PIN_RESTING;
                pin->flags &= ~PIN_HIT;
            }
            else
                pin->flags |= PIN_HIT;
        }
    }
    *outCount = count;
    return mask;
}

void fn_5_127C8(void)
{
    int i;
    struct Pin *pin;

    for (i = 0, pin = lbl_5_bss_19170; i < 10; i++, pin++)
    {
        if (pin->flags & PIN_ACTIVE)
            pin->flags |= PIN_HIT;
    }
}

void fn_5_1280C(void)
{
    int i;
    struct Pin *pin;

    for (i = 0, pin = lbl_5_bss_19170; i < 10; i++, pin++)
    {
        if ((pin->flags & PIN_ACTIVE) && (pin->flags & PIN_HIT) == 0)
            pin->flags |= PIN_LIFTED;
    }
}

void fn_5_12860(float y)
{
    int i;
    struct Pin *pin;

    for (i = 0, pin = lbl_5_bss_19170; i < 10; i++, pin++)
    {
        if ((pin->flags & PIN_ACTIVE) && (pin->flags & PIN_HIT) == 0)
        {
            pin->flags &= ~PIN_LIFTED;
            pin->flags |= PIN_HELD;
            pin->pos.y = y;
        }
    }
}

void fn_5_128C4(float z)
{
    int i;
    struct Pin *pin;

    for (i = 0, pin = lbl_5_bss_19170; i < 10; i++, pin++)
    {
        if ((pin->flags & PIN_ACTIVE) && (pin->flags & PIN_HIT))
        {
            float maxOver = 0.0f;
            int j;

            for (j = 0; j < pinConfig.sphereCount; j++)
            {
                float over = (pin->spherePos[j].z + pinConfig.spheres[j].radius) - z;

                if (over > maxOver)
                    maxOver = over;
            }
            if (maxOver != 0.0)
            {
                Vec push;

                pin->flags &= ~PIN_LIFTED;
                pin->flags &= ~PIN_RESTING;
                pin->flags |= PIN_SWEPT;
                push.x = 0.0f;
                push.y = 0.0f;
                push.z = -maxOver;
                fn_5_14730(pin, &push);
                pin->angVel.x += (float)((rand() & 0x1FF) - 0x100);
                pin->angVel.z += (float)((rand() & 0x7F) - 0x40);
            }
        }
    }
}

int fn_5_12A68(void)
{
    int settled = 1;
    Vec up = {0.0f, 1.0f, 0.0f};
    int i;
    struct Pin *pin;

    for (i = 0, pin = lbl_5_bss_19170; i < 10; i++, pin++)
    {
        if ((pin->flags & PIN_ACTIVE) && (pin->flags & PIN_HIT) && (pin->flags & PIN_RESTING) == 0
         && !(pin->pos.y < -1.5))
        {
            Vec axis;
            float dot;

            axis.x = pin->rot[0][1];
            axis.y = pin->rot[1][1];
            axis.z = pin->rot[2][1];
            dot = mathutil_vec_dot_prod(&up, &axis);
            if (!(dot < 0.707106781)
             || !(__fabs(pin->vel.x) + __fabs(pin->vel.y) + __fabs(pin->vel.z) < 0.02)
             || !(__fabs(pin->angVel.x) + __fabs(pin->angVel.y) + __fabs(pin->angVel.z) < 100.0))
                settled = 0;
        }
    }
    return settled;
}

void fn_5_12BAC(int slot)
{
    int i;

    if (slot > -1 && slot < 3)
    {
        for (i = 0; i < 10; i++)
            lbl_5_bss_19BE8[slot][i] = lbl_5_bss_19170[i];
    }
}

void fn_5_12C28(int slot)
{
    int i;

    if (slot > -1 && slot < 3)
    {
        for (i = 0; i < 10; i++)
            lbl_5_bss_19170[i] = lbl_5_bss_19BE8[slot][i];
    }
}

void fn_5_12CA4(struct Pin *pin)
{
    int i;

    mtxa_from_mtx(pin->rot);
    mathutil_mtxA_set_translate(&pin->pos);
    mtxa_translate_xyz(-pinConfig.center.x, -pinConfig.center.y, -pinConfig.center.z);
    for (i = 0; i < pinConfig.sphereCount; i++)
    {
        pin->prevSpherePos[i] = pin->spherePos[i];
        mtxa_tf_point((Vec *)&pinConfig.spheres[i].pos, &pin->spherePos[i]);
    }
}

void fn_5_12D9C(struct Pin *pin)
{
    int contactCount = 0;
    float maxDepth = 0.0f;
    int deepest = 0;
    Vec normalSum = {0.0f, 0.0f, 0.0f};
    Vec normal;
    Vec impulse;
    float elasticity;
    int spC;
    u8 pan;
    u8 span;
    int i;
    Vec *sphere;
    Vec *prevSphere;

    pin->prevContacts = pin->contacts;
    pin->contacts = 0;
    sphere = pin->spherePos;
    prevSphere = pin->prevSpherePos;
    for (i = 0; i < pinConfig.sphereCount; i++, sphere++, prevSphere++)
    {
        float depth = fn_5_16D30(sphere, prevSphere, pinConfig.spheres[i].radius, &normal, &elasticity, &spC);

        if (!(depth <= 0.0))
        {
            pin->contacts |= 1 << i;
            contactCount++;
            if (depth > maxDepth)
            {
                maxDepth = depth;
                deepest = i;
                PSVECAdd(&normal, &normalSum, &normalSum);
            }
        }
    }
    if (maxDepth != 0.0)
    {
        float speed;
        float bounce;

        if (1.4210855e-14f >= mathutil_vec_sq_len(&normalSum))
            normalSum.y = 1.0f;
        else
            vec_normalize_len(&normalSum);
        bounce = pinConfig.restitution * elasticity;
        fn_5_131F0(&pin->vel, &normalSum, bounce, pinConfig.friction, &impulse);
        pin->vel.x += impulse.x;
        pin->vel.y += impulse.y - 0.7 * 0.012495;
        pin->vel.z += impulse.z;
        PSVECScale(&impulse, &impulse, 170.0f);
        fn_5_132F0(pin, &impulse, deepest);
        speed = mathutil_vec_len(&impulse);
        if (speed > 20.0)
        {
            int ids[3] = {0x97, 0x96, 0x95};
            float t;
            int vol;
            int level;
            int hit;

            t = ((0.99 / 100.0) * speed < 0.99) ? (0.99 / 100.0) * speed : 0.99;
            vol = 47.0f * t - 20.0f;
            level = 3.0f * t;
            hit = call_SoundReqID_arg_1_copy((((s8)vol & 0x7F) << 11) | ids[(s8)level]);
            if (hit != -1)
            {
                SoundDop((u16)hit, (u16)((rand() & 0xFFF) + 0x1800));
                fn_5_14A80(&pin->pos, &pinListener, cameras[modeCtrl.currPlayer].rotY, &pan, &span);
                SoundPan((u16)hit, (s8)pan, (s8)span);
            }
        }
        if (contactCount < 3)
        {
            PSVECScale(&pin->angVel, &pin->angVel, 0.98f);
            if (0.02499 > __fabs(pin->vel.x) + __fabs(pin->vel.y) + __fabs(pin->vel.z)
             && 10.0 > __fabs(pin->angVel.x) + __fabs(pin->angVel.y) + __fabs(pin->angVel.z)
             && pin->rot[1][1] >= 1.0)
            {
                pin->flags |= PIN_RESTING;
                pin->flags &= ~PIN_HIT;
            }
        }
        else
        {
            PSVECScale(&pin->angVel, &pin->angVel, 0.5f);
            if (0.02499 > __fabs(pin->vel.x) + __fabs(pin->vel.y) + __fabs(pin->vel.z)
             && 10.0 > __fabs(pin->angVel.x) + __fabs(pin->angVel.y) + __fabs(pin->angVel.z))
                pin->flags |= PIN_RESTING;
        }
        {
            Vec offset;
            float scale = 0.8 * maxDepth;

            PSVECScale(&normalSum, &offset, scale);
            fn_5_14730(pin, &offset);
        }
    }
}

void fn_5_131F0(Vec *vel, Vec *normal, float restitution, float friction, Vec *out)
{
    Vec normalVel;
    Vec tangentVel;
    float dot = mathutil_vec_dot_prod(normal, vel);
    float scale = -1.0 - restitution;

    PSVECScale(normal, &normalVel, dot);
    PSVECSubtract(vel, &normalVel, &tangentVel);
    out->x = scale * normalVel.x - friction * tangentVel.x;
    out->y = scale * normalVel.y - friction * tangentVel.y;
    out->z = scale * normalVel.z - friction * tangentVel.z;
}

void fn_5_132F0(struct Pin *pin, Vec *impulse, int index)
{
    Vec delta;
    Vec spin;
    Vec back;
    Vec slide;
    Vec dir;

    PSVECSubtract(&pin->spherePos[index], &pin->prevSpherePos[index], &delta);
    PSVECSubtract(impulse, &delta, &delta);
    fn_5_134A4(pin, index, &delta, &spin);
    if (pin->prevContacts != pin->contacts)
        PSVECScale(&spin, &spin, pinConfig.restitution);
    else
    {
        float dot;

        PSVECSubtract(&pin->prevSpherePos[index], &pin->spherePos[index], &back);
        dir = *impulse;
        vec_normalize_len(&dir);
        dot = mathutil_vec_dot_prod(&back, &dir);
        slide.x = 0.71 * (back.x - dir.x * dot);
        slide.y = 0.71 * (back.y - dir.y * dot);
        slide.z = 0.71 * (back.z - dir.z * dot);
        fn_5_14730(pin, &slide);
    }
    PSVECAdd(&spin, &pin->angVel, &pin->angVel);
}

void fn_5_134A4(struct Pin *pin, int index, Vec *impulse, Vec *out)
{
    Vec torque;
    Vec arm;
    float scale;

    PSVECSubtract(&pin->spherePos[index], &pin->pos, &arm);
    mathutil_vec_cross_prod(&arm, impulse, &torque);
    scale = 30.0 + 5.0 / pinConfig.spheres[index].mass;
    torque.x *= pinConfig.invInertia.x * scale;
    torque.y *= pinConfig.invInertia.y * scale;
    torque.z *= pinConfig.invInertia.z * scale;
    *out = torque;
}

void fn_5_135E0(struct Pin *pin, struct Ball *ball)
{
    Vec vel;
    Vec ballAlong;
    Vec pinAlong;
    Vec relative;
    Vec ballPoint;
    Vec pinPoint;
    Vec normal;
    Vec toPin;
    Vec surface;
    Vec spin;
    Vec pinPush;
    float depth;
    u8 pan;
    u8 span;
    Vec *ballPos = &ball->pos;
    Vec *ballPrev = &ball->prevPos;
    Vec *ballVel = &ball->vel;
    int i;
    Vec *sphere;
    Vec *prevSphere;
    int deepest;
    Vec *hit;
    Vec *prevHit;
    float maxDepth;
    float pinMass;
    float ballMass;
    float restitution;
    float ballRadius;
    float dot;

    ballRadius = ball->currRadius;
    ballMass = 7.26f;
    sphere = pin->spherePos;
    prevSphere = pin->prevSpherePos;
    maxDepth = 0.0f;
    i = 0;
    for (; i < pinConfig.sphereCount; i++, sphere++, prevSphere++)
    {
        fn_5_147D4(&depth, prevSphere, sphere, pinConfig.spheres[i].radius, ballPrev, ballPos, ballRadius);
        if (depth > maxDepth)
        {
            maxDepth = depth;
            deepest = i;
            hit = sphere;
            prevHit = prevSphere;
        }
    }
    if (maxDepth != 0.0)
    {
        float invTotal;
        float pinDepth;
        float ballDepth;
        float sq;
        float speed;

        pin->flags &= ~PIN_RESTING;
        pin->flags |= PIN_HIT;
        pinMass = 0.7 * pinConfig.mass;
        restitution = pinConfig.restitution;
        ballPoint = *ballPos;
        pinPoint = *hit;
        fn_1_A74E4(ballPrev, &ballPoint, ballRadius, prevHit, &pinPoint, pinConfig.spheres[deepest].radius);
        PSVECSubtract(&pinPoint, &ballPoint, &normal);
        if (vec_normalize_len(&normal) <= 1.1920929e-07f)
        {
            normal.x = 0.0f;
            normal.y = 0.0f;
            normal.z = -1.0f;
        }
        PSVECSubtract(&ballPoint, &pinPoint, &toPin);
        vec_normalize_len(&toPin);
        g_math_unk7(&ball->unkB0, ball->currRadius, &surface, &toPin);
        PSVECScale(&surface, &surface, 10.0f);
        PSVECAdd(ballVel, &surface, &vel);
        dot = mathutil_vec_dot_prod(&normal, &vel);
        PSVECScale(&normal, &ballAlong, dot);
        PSVECSubtract(hit, prevHit, &pinAlong);
        dot = mathutil_vec_dot_prod(&normal, &pinAlong);
        PSVECScale(&normal, &pinAlong, dot);
        PSVECSubtract(&ballAlong, &pinAlong, &relative);
        invTotal = 1.0 / (ballMass + pinMass);
        {
            float cx = invTotal * (ballMass * ballAlong.x + pinMass * pinAlong.x);
            float cy = invTotal * (ballMass * ballAlong.y + pinMass * pinAlong.y);
            float cz = invTotal * (ballMass * ballAlong.z + pinMass * pinAlong.z);
            float rx = invTotal * (restitution * relative.x);
            float ry = invTotal * (restitution * relative.y);
            float rz = invTotal * (restitution * relative.z);
            float bx = cx - rx * pinMass;
            float by = cy - ry * pinMass;
            float bz = cz - rz * pinMass;
            float px = cx + rx * ballMass;
            float py = 0.012495 + (cy + ry * ballMass);
            float pz = cz + rz * ballMass;

            ballVel->x += bx - ballAlong.x;
            ballVel->y += by - ballAlong.y;
            ballVel->z += bz - ballAlong.z;
            pin->vel.x += px - pinAlong.x;
            pin->vel.y += py - pinAlong.y;
            pin->vel.z += pz - pinAlong.z;
        }
        PSVECScale(&relative, &relative, 200.0f);
        fn_5_134A4(pin, deepest, &relative, &spin);
        PSVECAdd(&spin, &pin->angVel, &pin->angVel);
        {
        Vec ballPush = {0.0f, 0.0f, 0.0f};
        pinDepth = 0.8 * (maxDepth * (pinMass * invTotal));
        ballDepth = 0.8 * (maxDepth * (ballMass * invTotal));
        pinPush.x = pin->pos.x - ballPos->x;
        pinPush.y = 0.0f;
        pinPush.z = pin->pos.z - ballPos->z;
        sq = mathutil_vec_sq_len(&pinPush);
        if (sq > 1.4210855e-14f)
        {
            float inv = math_rsqrt(sq);

            pinPush.x *= pinDepth * inv;
            pinPush.z *= pinDepth * inv;
            ballPush.x *= -ballDepth * inv;
            ballPush.z *= -ballDepth * inv;
        }
        else
        {
            pinPush.x = 0.0f;
            pinPush.z = pinDepth;
            ballPush.x = 0.0f;
            ballPush.z = -ballDepth;
        }
        fn_5_14730(pin, &pinPush);
        PSVECAdd(&ballPush, ballPos, ballPos);
        speed = mathutil_vec_len(&relative);
        if (speed > 10.0)
        {
            int ids[3] = {0x94, 0x93, 0x92};
            int ballIds[3] = {0x17, 0x18, 0x1A};
            float t;
            int vol;
            int level;
            int sound;

            t = ((0.99 / 64.0) * speed < 0.99) ? (0.99 / 64.0) * speed : 0.99;
            vol = 47.0f * t - 20.0f;
            level = 3.0f * t;
            sound = call_SoundReqID_arg_1_copy((((s8)vol & 0x7F) << 11) | ids[(s8)level]);
            if (sound != -1)
            {
                SoundDop((u16)sound, (u16)((rand() & 0xFFF) + 0x1800));
                fn_5_14A80(&pin->pos, &pinListener, cameras[modeCtrl.currPlayer].rotY, &pan, &span);
                SoundPan((u16)sound, (s8)pan, (s8)span);
            }
            call_SoundReqID_arg_0(ballIds[(s8)level]);
            g_rumble_controller(player_pad_map[modeCtrl.currPlayer], 1,
                                (15.0f * t < 2.0f) ? 2.0f : ((15.0f * t > 15.0f) ? 15.0f : 15.0f * t));
        }
        }
    }
}

void fn_5_13DDC(int index)
{
    Vec normal;
    Vec aAlong;
    Vec bAlong;
    Vec relative;
    Vec aPoint;
    Vec bPoint;
    Vec delta;
    Vec impulse;
    Vec spin;
    Vec dir;
    Vec aPush;
    Vec bPush;
    float depth;
    u8 pan;
    u8 span;
    int i;
    struct Pin *a = &lbl_5_bss_19170[index];
    struct Pin *b;
    int j;
    int k;
    int bestA;
    int bestB;
    float maxDepth;
    float massA;
    float massB;
    float restitution;
    float invTotal;

    for (i = index + 1, b = &lbl_5_bss_19170[i]; i < 10; i++, b++)
    {
        maxDepth = 0.0f;
        if ((b->flags & PIN_ACTIVE) && (b->flags & (PIN_HELD | PIN_LIFTED)) == 0)
        {
            if (!(3.0625 < mathutil_vec_sq_distance(&a->pos, &b->pos)))
            {
                for (j = 0; j < pinConfig.sphereCount; j++)
                {
                    for (k = 0; k < pinConfig.sphereCount; k++)
                    {
                        fn_5_147D4(&depth, &a->prevSpherePos[j], &a->spherePos[j], pinConfig.spheres[j].radius,
                                   &b->prevSpherePos[k], &b->spherePos[k], pinConfig.spheres[k].radius);
                        if (depth > maxDepth)
                        {
                            maxDepth = depth;
                            bestA = j;
                            bestB = k;
                        }
                    }
                }
                if (maxDepth != 0.0)
                {
                    float sq;
                    float speed;
                    float dot;

                    a->flags &= ~PIN_RESTING;
                    b->flags &= ~PIN_RESTING;
                    a->flags |= PIN_HIT;
                    b->flags |= PIN_HIT;
                    massA = pinConfig.spheres[bestA].mass;
                    massB = pinConfig.spheres[bestB].mass;
                    restitution = pinConfig.restitution;
                    aPoint = a->spherePos[bestA];
                    bPoint = b->spherePos[bestB];
                    fn_1_A74E4(&a->prevSpherePos[bestA], &aPoint, pinConfig.spheres[j].radius,
                               &b->prevSpherePos[bestB], &bPoint, pinConfig.spheres[k].radius);
                    PSVECSubtract(&aPoint, &bPoint, &normal);
                    if (vec_normalize_len(&normal) == 0.0)
                    {
                        normal.x = 0.0f;
                        normal.y = 0.0f;
                        normal.z = 0.1f;
                    }
                    PSVECSubtract(&a->spherePos[bestA], &a->prevSpherePos[bestA], &delta);
                    dot = mathutil_vec_dot_prod(&normal, &delta);
                    PSVECScale(&normal, &aAlong, dot);
                    PSVECSubtract(&b->spherePos[bestB], &b->prevSpherePos[bestB], &delta);
                    dot = mathutil_vec_dot_prod(&normal, &delta);
                    PSVECScale(&normal, &bAlong, dot);
                    PSVECSubtract(&aAlong, &bAlong, &relative);
                    if ((a->flags & PIN_SWEPT) || (b->flags & PIN_SWEPT))
                        PSVECScale(&relative, &relative, 0.3f);
                    invTotal = 1.0 / (massA + massB);
                    {
                        float cx = invTotal * (massA * aAlong.x + massB * bAlong.x);
                        float cy = invTotal * (massA * aAlong.y + massB * bAlong.y);
                        float cz = invTotal * (massA * aAlong.z + massB * bAlong.z);
                        float rx = invTotal * (restitution * relative.x);
                        float ry = invTotal * (restitution * relative.y);
                        float rz = invTotal * (restitution * relative.z);
                        float ax = cx - rx * massB;
                        float ay = cy - ry * massB;
                        float az = cz - rz * massB;
                        float bx = cx + rx * massA;
                        float by = cy + ry * massA;
                        float bz = cz + rz * massA;

                        a->vel.x += ax - aAlong.x;
                        a->vel.y += ay - aAlong.y;
                        a->vel.z += az - aAlong.z;
                        b->vel.x += bx - bAlong.x;
                        b->vel.y += by - bAlong.y;
                        b->vel.z += bz - bAlong.z;
                    }
                    PSVECScale(&relative, &impulse, 90.0 * massB);
                    fn_5_134A4(b, bestB, &impulse, &spin);
                    PSVECAdd(&spin, &b->angVel, &b->angVel);
                    PSVECScale(&relative, &impulse, -90.0 * massA);
                    fn_5_134A4(a, bestA, &impulse, &spin);
                    PSVECAdd(&spin, &a->angVel, &a->angVel);
                    dir.x = a->pos.x - b->pos.x;
                    dir.y = 0.0f;
                    dir.z = a->pos.z - b->pos.z;
                    sq = mathutil_vec_sq_len(&dir);
                    if (sq <= 1.4210855e-14f)
                    {
                        dir.x = 0.0f;
                        dir.z = -1.0f;
                    }
                    else
                    {
                        float inv = math_rsqrt(sq);

                        dir.x *= inv;
                        dir.z *= inv;
                    }
                    {
                        float pushA = 0.8 * (maxDepth * (massA * invTotal));
                        float pushB = 0.8 * (maxDepth * (massB * invTotal));

                        PSVECScale(&dir, &aPush, pushA);
                        PSVECScale(&dir, &bPush, -pushB);
                    }
                    fn_5_14730(a, &aPush);
                    fn_5_14730(b, &bPush);
                    speed = mathutil_vec_len(&relative);
                    if (speed > 0.01)
                    {
                        int sounds[3] = {0x91, 0x90, 0x8F};
                        float t;
                        int vol;
                        int level;
                        int sound;

                        t = ((0.99 / 0.4) * speed < 0.99) ? (0.99 / 0.4) * speed : 0.99;
                        vol = 47.0f * t - 20.0f;
                        level = 3.0f * t;
                        sound = call_SoundReqID_arg_1_copy((((s8)vol & 0x7F) << 11) | sounds[(s8)level]);
                        if (sound != -1)
                        {
                            SoundDop((u16)sound, (u16)((rand() & 0xFFF) + 0x1800));
                            fn_5_14A80(&a->pos, &pinListener, cameras[modeCtrl.currPlayer].rotY, &pan, &span);
                            SoundPan((u16)sound, (s8)pan, (s8)span);
                        }
                    }
                }
            }
        }
    }
}

void fn_5_14730(struct Pin *pin, Vec *offset)
{
    int i;
    Vec *sphere;
    Vec *prevSphere;

    PSVECAdd(offset, &pin->pos, &pin->pos);
    sphere = pin->spherePos;
    prevSphere = pin->prevSpherePos;
    for (i = 0; i < pinConfig.sphereCount; i++, sphere++, prevSphere++)
    {
        PSVECAdd(offset, sphere, sphere);
        PSVECAdd(offset, prevSphere, prevSphere);
    }
}

int fn_5_147D4(float *depth, Vec *prevA, Vec *posA, float radiusA, Vec *prevB, Vec *posB, float radiusB)
{
    float closest;
    float end;
    float radii;
    float radiiSq;

    fn_5_148AC(&closest, &end, prevA, posA, prevB, posB);
    radii = radiusA + radiusB;
    radiiSq = radii * radii;
    if (closest < radiiSq)
    {
        if (end < radiiSq)
            *depth = radii - math_sqrt(end);
        else
            *depth = 0.0f;
        return 1;
    }
    *depth = 0.0f;
    return 0;
}

void fn_5_148AC(float *closest, float *end, Vec *prevA, Vec *posA, Vec *prevB, Vec *posB)
{
    Vec relVel;
    Vec start;
    Vec finish;
    Vec nearest;
    float lenSq;

    PSVECSubtract(prevB, prevA, &start);
    PSVECSubtract(posB, posA, &finish);
    PSVECSubtract(&finish, &start, &relVel);
    lenSq = mathutil_vec_sq_len(&relVel);
    if (lenSq > 0.0)
    {
        float t = (-(relVel.x * start.x) - relVel.y * start.y) - relVel.z * start.z;

        if (0.0 <= t && t <= lenSq)
        {
            t /= lenSq;
            nearest.x = start.x + relVel.x * t;
            nearest.y = start.y + relVel.y * t;
            nearest.z = start.z + relVel.z * t;
            *closest = mathutil_vec_sq_len(&nearest);
            *end = mathutil_vec_sq_len(&finish);
            return;
        }
        *end = *closest = mathutil_vec_sq_len(&finish);
        return;
    }
    *end = *closest = mathutil_vec_sq_len(&finish);
}

void fn_5_14A80(Vec *pos, const Vec *listener, s16 rotY, u8 *pan, u8 *span)
{
    Vec local;
    s16 angle;
    u8 left;
    u8 right;

    mtxa_push();
    mtxa_from_rotate_y(rotY);
    mathutil_mtxA_set_translate((Vec *)listener);
    mtxa_tf_point(pos, &local);
    angle = math_atan2(local.x, local.z);
    mtxa_pop();
    left = abs(angle + 0x4000) >> 8;
    right = abs(angle - 0x8000) >> 8;
    if (left >= 0x80)
        left = 0xFF - left;
    if (right >= 0x80)
        right = 0xFF - right;
    *pan = left;
    *span = right;
}
