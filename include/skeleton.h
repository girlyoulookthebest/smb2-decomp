#ifndef SKELETON_H
#define SKELETON_H

#include "types.h"

struct BoneHandle
{
    /*0x00*/ u8 flags;
    /*0x01*/ s8 rotX;
    /*0x02*/ s8 rotY;
    /*0x03*/ s8 rotZ;
    /*0x04*/ u16 mirrorBone;
    u8 unk6[2];
};  /* size = 0x8 */

struct BoneKey
{
    /*0x00*/ u16 type;
    /*0x02*/ u16 frame;
    /*0x04*/ float value;
    /*0x08*/ float tanIn;
    /*0x0C*/ float tanOut;
};  /* size = 0x10 */

struct Bone
{
    /*0x000*/ char *name;
    /*0x004*/ u16 flags;
    /*0x006*/ u16 parent;
    /*0x008*/ Vec pos;
    /*0x014*/ float len;
    /*0x018*/ Vec unk18;
    /*0x024*/ float unk24;
    /*0x028*/ Mtx unk28;
    /*0x058*/ Mtx unk58;
    /*0x088*/ Mtx mtx;
    /*0x0B8*/ Vec scale;
    /*0x0C4*/ Vec unkC4;
    /*0x0D0*/ struct BoneHandle *handle;
    /*0x0D4*/ u16 keyCount[18];
    /*0x0F8*/ u16 keyIdx[18];
    /*0x11C*/ void *keys[18];
    /*0x164*/ Quat quat;
};  /* size = 0x174 */

struct AnimClock
{
    /*0x00*/ u16 flags;
    /*0x02*/ u16 loopStart;
    /*0x04*/ u16 end;
    /*0x06*/ u16 unk6;
    /*0x08*/ float speed;
    /*0x0C*/ float frame;
    /*0x10*/ float length;
    /*0x14*/ u16 loops;
    u8 unk16[2];
};  /* size = 0x18 */

struct AnimInfo
{
    u8 unk0[2];
    /*0x02*/ u16 loopStart;
    /*0x04*/ u16 end;
    /*0x06*/ u16 unk6;
};

struct MtaClip
{
    /*0x00*/ u16 length;
    /*0x02*/ u16 boneCount;
};

struct Mta
{
    /*0x00*/ u32 count;  /* top bit: offsets have been relocated */
    /*0x04*/ char * DPTR names;
    /*0x08*/ struct MtaClip * DPTR clips[1];
};

struct Skeleton
{
    /*0x00*/ u16 boneCount;
    /*0x02*/ u16 flags;
    /*0x04*/ float speed;
    /*0x08*/ struct Bone *bones;
    /*0x0C*/ float scaleX;
    /*0x10*/ float scaleY;
    /*0x14*/ float scaleZ;
    /*0x18*/ Vec unk18;
    /*0x24*/ struct Mta *mta;
    /*0x28*/ float blendFrame;
    /*0x2C*/ float blendLength;
    /*0x30*/ Vec unk30;
    /*0x3C*/ struct BoneHandle *handles;
    /*0x40*/ struct AnimClock clock;
    /*0x58*/ struct AnimClock prevClock;
};

#endif
