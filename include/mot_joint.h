#ifndef MOT_JOINT_H
#define MOT_JOINT_H

#include "types.h"
#include "mathutil.h"

struct MotionChannel
{
    /*0x00*/ u16 keyframeCount;
    /*0x02*/ u16 currKeyframe;
    /*0x04*/ u16 *times;
    /*0x08*/ u8 *valueTypes;  /* per-keyframe value type (0-5), see s_keyframeValueCounts */
    /*0x0C*/ float *values;
};  /* size = 0x10 */

struct AnimJoint
{
    /*0x000*/ u32 flags;
    /*0x004*/ u8 filler4[0x60-0x4];
    /*0x060*/ struct MotionChannel channels[10];  /* unk, x, y, z, rotX, rotY, rotZ, scaleX, scaleY, scaleZ */
    /*0x100*/ u8 filler100[0x20C-0x100];
    /*0x20C*/ float unk20C;
    /*0x210*/ Vec pos;
    /*0x21C*/ u8 filler21C[0x228-0x21C];
    /*0x228*/ Vec rot;
    /*0x234*/ Mtx rotateMtx;
    /*0x264*/ Vec scale;
    /*0x270*/ Mtx unk270;
};  /* size = 0x2A0 */

struct JointPosRemap
{
    u16 unk0;
    u16 jointIdx;
};  /* size = 0x4 */

struct JointRotRemap
{
    u16 unk0;
    u16 jointIdx;
    float scaleX;
    float scaleY;
    float scaleZ;
    float offsetX;
    float offsetY;
    float offsetZ;
};  /* size = 0x1C */

void u_interpolate_joint_motion(struct AnimJoint *joints, struct JointRotRemap *b, struct JointPosRemap *c, float t, u32 d);
void u_interp_pos_motion(struct AnimJoint *a, struct AnimJoint *b, float t, u32 c);
void u_interp_rot_motion(struct AnimJoint *a, struct AnimJoint *b, struct JointRotRemap *c, float t, u32 d);
void u_interp_scale_motion(struct AnimJoint *a, float t);
void u_interp_chan0_motion(struct AnimJoint *a, float t);
float interpolate_channel_keyframes(struct MotionChannel *chan, float t);
float interpolate_linear(float v0, float t0, float v1, float t1, float t);
void read_channel_keyframe_values(struct MotionChannel *chan, float *val1, float *val2, float *val3);
void seek_channel_next_keyframe(struct MotionChannel *chan);
void seek_channel_prev_keyframe(struct MotionChannel *chan);

#endif
