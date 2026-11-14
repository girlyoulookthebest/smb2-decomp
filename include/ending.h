#ifndef _ENDING_H_
#define _ENDING_H_

#include "types.h"
#include "ball.h"

/* One character in an ending scene. */
struct EndingChara
{
    /*0x00*/ struct Ape *ape;
    /*0x04*/ s16 unk4;
    /*0x06*/ s16 unk6;
    /*0x08*/ s16 unk8;
    /*0x0A*/ s16 unkA;
    /*0x0C*/ Vec pos;
    /*0x18*/ Vec vel;
    u8 unk24[0x30 - 0x24];
    /*0x30*/ s16 rotX;
    /*0x32*/ s16 rotY;
    /*0x34*/ s16 rotZ;
    /*0x36*/ u16 unk36;
    /*0x38*/ s16 unk38;
    /*0x3A*/ s16 unk3A;
    /*0x3C*/ struct GMAModel *unk3C;
    /*0x40*/ s32 unk40;
    /*0x44*/ Vec unk44;
    /*0x50*/ s16 unk50;
    /*0x52*/ s16 unk52;
    /*0x54*/ s16 unk54;
    /*0x56*/ s16 unk56;
    /*0x58*/ s16 unk58;
    /*0x5A*/ s16 unk5A;
    /*0x5C*/ s16 unk5C;
    u8 unk5E[0x70 - 0x5E];
    /*0x70*/ s32 unk70;
    /*0x74*/ Vec unk74;
}; /* size = 0x80 */

struct EndingUnkAA04
{
    /*0x00*/ s8 unk0;
    u8 unk1;
    /*0x02*/ s16 unk2;
    /*0x04*/ s16 unk4;
    /*0x08*/ Vec unk8;
    /*0x14*/ Vec unk14;
    /*0x20*/ Vec unk20;
    /*0x2C*/ Vec unk2C;
    /*0x38*/ s16 unk38;
    /*0x3A*/ s16 unk3A;
    /*0x3C*/ s16 unk3C;
    /*0x3E*/ s16 unk3E;
    /*0x40*/ s16 unk40;
    /*0x42*/ s16 unk42;
    /*0x44*/ Vec unk44;
}; /* size = 0x50 */

/* The ending camera: eye and look-at each either hold a fixed point, ease
 * towards one, or follow a point attached to a character. */
struct EndingCamera
{
    /*0x00*/ s16 eyeMode;
    /*0x02*/ s16 lookAtMode;
    /*0x04*/ Vec eye;
    /*0x10*/ Vec eyeVel;
    /*0x1C*/ s16 eyeChara;
    /*0x1E*/ s16 eyeBone;
    /*0x20*/ Vec eyeOffset;
    /*0x2C*/ Vec lookAt;
    /*0x38*/ Vec lookAtVel;
    /*0x44*/ s16 lookAtChara;
    /*0x46*/ s16 lookAtBone;
    /*0x48*/ Vec lookAtOffset;
}; /* size = 0x54 */

/* An ending message, split into lines pointing into a text buffer. */
struct EndingMsgLines
{
    /*0x00*/ s32 count;
    /*0x04*/ char *lines[6];
}; /* size = 0x1C */

/* A falling piece of debris or confetti. */
struct EndingBitAnchor
{
    /*0x00*/ u32 flags;
    /*0x04*/ Vec pos;
    /*0x10*/ s16 rotX;
    /*0x12*/ s16 rotY;
    /*0x14*/ s16 rotZ;
    u8 unk16[2];
    /*0x18*/ Vec scale;
}; /* size = 0x24 */

struct EndingBit
{
    /*0x00*/ s8 active;
    /*0x02*/ s16 state;
    /*0x04*/ s16 timer;
    /*0x08*/ u32 flags;
    /*0x0C*/ Vec pos;
    /*0x18*/ Vec vel;
    /*0x24*/ Vec scale;
    /*0x30*/ s16 rotX;
    /*0x32*/ s16 rotY;
    /*0x34*/ s16 rotZ;
    /*0x36*/ s16 rotVelX;
    /*0x38*/ s16 rotVelY;
    /*0x3A*/ s16 rotVelZ;
    /*0x3C*/ Vec unk3C;
    /*0x48*/ float unk48;
    /*0x4C*/ struct EndingBitAnchor *anchor;
    /*0x50*/ struct GMAModel *model;
}; /* size = 0x54 */

struct EndingWork
{
    /*0x0000*/ float timer;
    /*0x0004*/ struct EndingBit bits[0x200];
    /*0xA804*/ struct EndingChara charas[4];
    /*0xAA04*/ struct EndingUnkAA04 unkAA04;
    /*0xAA54*/ struct EndingCamera camera;
    /*0xAAA8*/ float unkAAA8;
    /*0xAAAC*/ float unkAAAC;
    /*0xAAB0*/ struct EndingMsgLines msg;
    /*0xAACC*/ s32 msgLine;
    /*0xAAD0*/ char msgBuf[0xC2D0 - 0xAAD0];
}; /* size = 0xC2D0 */

struct EndingInfo
{
    /*0x00*/ s16 state;
    /*0x02*/ s16 nextState;
    /*0x04*/ u32 flags;
    /*0x08*/ struct EndingWork *work;
}; /* size = 0xC */

extern struct EndingInfo lbl_1_bss_D3354;

#endif
