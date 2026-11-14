/* Keyframed joint animation (SMB1 mot_joint.c, extended with scale channels and
 * linear/step keyframe types). */
#include "macros.h"
#include "types.h"
#include "mathutil.h"
#include "mot_joint.h"

struct InterpolateStuff
{
    float time;
    float val1;
    float val2;
};

/* number of floats stored per keyframe for each value type */
const u8 s_keyframeValueCounts[] = { 0, 1, 2, 3, 1, 1 };

float u_crazy_interpolation_stuff(struct InterpolateStuff *a, struct InterpolateStuff *b, float c);

void u_interpolate_joint_motion(struct AnimJoint *joints, struct JointRotRemap *b, struct JointPosRemap *c, float t, u32 d)
{
    u32 flags;
    struct AnimJoint *joint = joints;

    if (b == NULL || c == NULL)
        d = 0;

    flags = joint->flags;
    while (flags != 0)
    {
        flags &= ~(3 << 14);
        joint->flags = flags;

        if (d != 0)
            u_interp_pos_motion(joint, &joints[c->jointIdx], t, d);
        else
            u_interp_pos_motion(joint, joint, t, d);
        if (flags & (1 << 2))
            c++;

        if (d != 0)
            u_interp_rot_motion(joint, &joints[b->jointIdx], b, t, d);
        else
            u_interp_rot_motion(joint, joint, b, t, d);
        if (flags & (1 << 3))
            b++;

        u_interp_scale_motion(joint, t);
        u_interp_chan0_motion(joint, t);
        joint++;
        flags = joint->flags;
    }
}

void u_interp_pos_motion(struct AnimJoint *a, struct AnimJoint *b, float t, u32 c)
{
    struct MotionChannel *chan = &a->channels[1];
    float zero = 0.0f;

    if (chan->keyframeCount != 0)
    {
        b->pos.x = interpolate_channel_keyframes(chan, t);
        if (c != 0)
            b->pos.x = -b->pos.x;
    }
    else
        b->pos.x = zero;
    chan++;

    if (chan->keyframeCount != 0)
        b->pos.y = interpolate_channel_keyframes(chan, t);
    else
        b->pos.y = zero;
    chan++;

    if (chan->keyframeCount != 0)
        b->pos.z = interpolate_channel_keyframes(chan, t);
    else
        b->pos.z = zero;
}

void u_interp_rot_motion(struct AnimJoint *a, struct AnimJoint *b, struct JointRotRemap *c, float t, u32 d)
{
    float radToS16;
    struct MotionChannel *chan;

    mtxa_from_identity();
    chan = &a->channels[6];
    radToS16 = 10430.3779296875f;
    if (chan->keyframeCount != 0)
    {
        float val = interpolate_channel_keyframes(chan, t);
        if (d != 0)
            val = c->offsetZ + val * c->scaleZ;
        b->rot.z = val;
        mtxa_rotate_z((s16)(radToS16 * val));
    }
    else
        b->rot.z = 0.0f;

    chan--;
    if (chan->keyframeCount != 0)
    {
        float val = interpolate_channel_keyframes(chan, t);
        if (d != 0)
            val = c->offsetY + val * c->scaleY;
        b->rot.y = val;
        mtxa_rotate_y((s16)(radToS16 * val));
    }
    else
        b->rot.y = 0.0f;

    chan--;
    if (chan->keyframeCount != 0)
    {
        float val = interpolate_channel_keyframes(chan, t);
        if (d != 0)
            val = c->offsetX + val * c->scaleX;
        b->rot.x = val;
        mtxa_rotate_x((s16)(radToS16 * val));
    }
    else
        b->rot.x = 0.0f;

    mtxa_sq_to_mtx(b->rotateMtx);
}

void u_interp_scale_motion(struct AnimJoint *a, float t)
{
    struct MotionChannel *chan = &a->channels[7];
    float one = 1.0f;

    if (chan->keyframeCount != 0)
        a->scale.x = interpolate_channel_keyframes(chan, t);
    else
        a->scale.x = one;
    chan++;

    if (chan->keyframeCount != 0)
        a->scale.y = interpolate_channel_keyframes(chan, t);
    else
        a->scale.y = one;
    chan++;

    if (chan->keyframeCount != 0)
        a->scale.z = interpolate_channel_keyframes(chan, t);
    else
        a->scale.z = one;
}

void u_interp_chan0_motion(struct AnimJoint *a, float t)
{
    struct MotionChannel *chan = &a->channels[0];

    if (chan->keyframeCount != 0)
        a->unk20C = interpolate_channel_keyframes(chan, t);
    else
        a->unk20C = 1.0f;
}

float interpolate_channel_keyframes(struct MotionChannel *chan, float t)
{
    struct MotionChannel prev;
    struct InterpolateStuff sp2C;
    struct InterpolateStuff sp20;
    float ret;
    float dummy1;
    float dummy2;
    u16 keyframeCount = chan->keyframeCount;
    u16 keyframe = chan->currKeyframe;
    u8 type = 0;

    // Find which keyframe we are on
    while (keyframe < keyframeCount)
    {
        float frameT = *chan->times;

        if (fabs(frameT - t) < 1.1920928955078125e-07)
        {
            type = 1;
            break;
        }
        else if (frameT > t)
        {
            if (keyframe != 0)
                type = 3;
            else
                type = 2;
            break;
        }
        keyframe++;
        seek_channel_next_keyframe(chan);
    }

    switch (type)
    {
    default:
        if (chan->currKeyframe < keyframeCount)
            seek_channel_prev_keyframe(chan);
        // fall through
    case 1:
    case 2:
        read_channel_keyframe_values(chan, &dummy1, &dummy2, &ret);
        break;
    case 3:
        sp20.time = *chan->times;
        read_channel_keyframe_values(chan, &sp20.val2, &dummy2, &sp20.val1);

        prev.times = chan->times;
        prev.valueTypes = chan->valueTypes;
        prev.values = chan->values;
        seek_channel_prev_keyframe(&prev);

        sp2C.time = *prev.times;
        read_channel_keyframe_values(&prev, &dummy1, &sp2C.val2, &sp2C.val1);

        switch (*prev.valueTypes)
        {
        default:
            ret = u_crazy_interpolation_stuff(&sp2C, &sp20, t);
            break;
        case 4:
            ret = interpolate_linear(sp2C.val1, sp2C.time, sp20.val1, sp20.time, t);
            break;
        case 5:
            ret = sp2C.val1;
            break;
        }
        break;
    }
    chan->currKeyframe = keyframe;
    return ret;
}

float u_crazy_interpolation_stuff(struct InterpolateStuff *a, struct InterpolateStuff *b, float c)
{
    float t0, unkx, unkx2, f5, dt, z1;

    t0 = a->time;
    z1 = a->val2;
    dt = b->time - t0;
    unkx = (c - t0) / dt;
    unkx2 = unkx * unkx;
    f5 = unkx * unkx2 - unkx2;
    dt *= (1.0 / 30.0);
    return a->val1
         + (f5 + f5 - unkx2) * (a->val1 - b->val1)
         + dt * (f5 * (z1 + b->val2) - z1 * (unkx2 - unkx));
}

float interpolate_linear(float v0, float t0, float v1, float t1, float t)
{
    return v0 + (v1 - v0) * (t - t0) / (t1 - t0);
}

void read_channel_keyframe_values(struct MotionChannel *chan, float *val1, float *val2, float *val3)
{
    switch (*chan->valueTypes)
    {
    default:
    case 0:
        *val1 = *val2 = *val3 = 0.0f;
        break;
    case 1:
    case 4:
    case 5:
        *val1 = *val2 = 0.0f;
        *val3 = chan->values[0];
        break;
    case 2:
        *val1 = *val2 = chan->values[0];
        *val3 = chan->values[1];
        break;
    case 3:
        *val1 = chan->values[0];
        *val2 = chan->values[1];
        *val3 = chan->values[2];
        break;
    }
}

// Advances channel pointers to the next keyframe
void seek_channel_next_keyframe(struct MotionChannel *chan)
{
    chan->times++;
    chan->values += s_keyframeValueCounts[*chan->valueTypes];
    chan->valueTypes++;
}

// Moves channel pointers back to the previous keyframe
void seek_channel_prev_keyframe(struct MotionChannel *chan)
{
    chan->times--;
    chan->valueTypes--;
    chan->values -= s_keyframeValueCounts[*chan->valueTypes];
}
