#ifndef BALL_H
#define BALL_H

#include "types.h"
#include "mathutil.h"

struct RaycastHit
{
    /*0x00*/ u32 flags;
    /*0x04*/ Vec pos;
    /*0x10*/ Vec normal;
};  /* size = 0x1C */

struct Ball
{
    /*0x00*/ s8 status;  /* 2 = in play */
    /*0x01*/ s8 unk1;  /* ball size index */
    /*0x02*/ s8 unk2;  /* lives */
    /*0x03*/ s8 state;
    /*0x04*/ Vec pos;
    /*0x10*/ Vec prevPos;
    /*0x1C*/ Vec vel;
    /*0x28*/ s16 rotX;
    /*0x2A*/ s16 rotY;
    /*0x2C*/ s16 rotZ;
    /*0x2E*/ s8 playerId;
    /*0x2F*/ s8 rank;  /* finishing place in competition mode */
    /*0x30*/ Mtx unk30;
    /*0x60*/ S16Vec unk60;  /* visual rotation velocity */
    /*0x66*/ s16 unk66;
    /*0x68*/ float currRadius;
    /*0x6C*/ float accel;
    /*0x70*/ float restitution;
    /*0x74*/ float modelScale;
    /*0x78*/ s32 bananas;
    /*0x7C*/ s32 score;
    /*0x80*/ s32 unk80;  /* frames since ... (camera pitch follows only after 60) */
    /*0x84*/ Vec unk84;
    /*0x90*/ s16 unk90;
    /*0x92*/ s16 unk92;  /* heading */
    /*0x94*/ u32 unk94;
    /*0x98*/ u64 flags;
    /*0xA0*/ Quat unkA0;  /* ape rotation */
    /*0xB0*/ Quat unkB0;  /* ball rotation */
    /*0xC0*/ Vec unkC0;
    /*0xCC*/ float unkCC;
    /*0xD0*/ Mtx unkD0;
    /*0x100*/ float speed;
    /*0x104*/ struct Ape * DPTR ape;  /* DPTR: Ball stays 0x1B0 on x64 (literal 0x1B0 sizes) */
    /*0x108*/ s32 unk108;
    /*0x10C*/ Vec unk10C;  /* point the ape looks at */
    /*0x118*/ float unk118;
    /*0x11C*/ Vec unk11C;  /* last collision normal */
    /*0x128*/ u64 prevFlags;  /* flags at the start of the frame */
    /*0x130*/ s16 unk130;
    /*0x132*/ s16 winStreak;
    /*0x134*/ s16 unk134;
    /*0x136*/ s16 goalTime;  /* clock when the goal was reached */
    /*0x138*/ u32 unk138;
    /*0x13C*/ float unk13C;  /* hardest collision speed */
    /*0x140*/ s32 unk140;
    /*0x144*/ s32 bonus;  /* points for this round */
    /*0x148*/ s32 unk148;
    /*0x14C*/ float unk14C;  /* physical radius */
    /*0x150*/ u32 unk150;
    /*0x154*/ s8 unk154;
    /*0x155*/ s8 unk155;
    /*0x156*/ u8 colorId;
    /*0x157*/ u8 unk157;
    /*0x158*/ s16 unk158;
    /*0x15A*/ s16 unk15A;
    /*0x15C*/ Vec unk15C;
    /*0x168*/ float unk168[4];
    /*0x178*/ s8 unk178;
    /*0x179*/ u8 unk179;  /* model LOD */
    u8 unk17A[2];
    /*0x17C*/ struct RaycastHit unk17C;
    /*0x198*/ u8 unk198[0x1B0 - 0x198];
}; /* size = 0x1B0 */

/* The ape's animation state (bodyAnim, faceAnim, ...) is a struct Skeleton
 * followed by its Bone array (skeleton.h). Left incomplete so that every
 * access goes through struct Skeleton / struct Bone, whose layout differs
 * on x64. */
struct ApeAnim;

struct ApeFlagInfo
{
    /*0x00*/ u16 flags;  /* 1: RateIntp, 2: CalcIntp, 4: Face Dir, 8: Protect, 0x10: No loop */
    u8 unk2[0x12];
    /*0x14*/ float unk14;  /* "float" */
    /*0x18*/ float speed;
};

struct ApePartList;
struct ApeModelTable;

struct ApeAnimOut
{
    /*0x00*/ u16 a;
    /*0x02*/ u16 b;
    /*0x04*/ float t;
    /*0x08*/ u16 flags;
    u8 unkA[2];
};  /* size = 0xC */

struct Ape
{
    /*0x00*/ sptr unk0;  /* ApeData * (APE_DATA); sptr == s32 on GC */
    /*0x04*/ void *unk4;
    /*0x08*/ void *unk8;
    /*0x0C*/ struct ApeAnim *bodyAnim;
    /*0x10*/ void *unk10;
    /*0x14*/ sptr unk14;  /* struct Mta * */
    /*0x18*/ struct ApeAnim *faceAnim;
    /*0x1C*/ void *unk1C;
    /*0x20*/ void *unk20;
    /*0x24*/ struct ApeAnim *rhandAnim;
    /*0x28*/ void *unk28;
    /*0x2C*/ void *bodyMta;
    /*0x30*/ struct ApeAnim *lhandAnim;
    /*0x34*/ void *unk34;
    /*0x38*/ void *unk38;
    /*0x3C*/ struct ApeAnim *unk3C;
    /*0x40*/ void *unk40;
    /*0x44*/ void *handMta;
    /*0x48*/ struct ApePartList *partList;
    /*0x4C*/ struct ApeModelTable *modelTable;
    /*0x50*/ struct TPL *tpl;
    u8 unk54[0x5C - 0x54];
    /*0x5C*/ void (*drawFunc)();
    /*0x60*/ float unk60;
    /*0x64*/ u16 unk64;
    /*0x66*/ u16 unk66;
    /*0x68*/ s16 unk68;
    u8 unk6A[2];
    /*0x6C*/ s32 unk6C;
    /*0x70*/ s32 unk70;
    /*0x74*/ s16 unk74;
    /*0x76*/ s8 meshType;
    u8 unk77;
    /*0x78*/ s32 unk78;
    /*0x7C*/ s32 unk7C;
    /*0x80*/ s16 unk80;
    /*0x82*/ s16 unk82;
    /*0x84*/ s16 robParam;
    /*0x86*/ s8 unk86;  /* animation state; 4 = idle */
    /*0x87*/ s8 robStep;
    /*0x88*/ u32 unk88;
    /*0x8C*/ u16 rhandId;
    /*0x8E*/ u16 lhandId;
    /*0x90*/ u16 faceExpr;
    /*0x92*/ u16 unk92;
    u8 unk94[4];
    /*0x98*/ float lhandBlend;
    /*0x9C*/ float rhandBlend;
    /*0xA0*/ u16 unkA0;
    /*0xA2*/ u16 unkA2;
    /*0xA4*/ u16 unkA4;
    u8 unkA6[2];
    /*0xA8*/ s32 unkA8;
    /*0xAC*/ s32 unkAC;
    /*0xB0*/ float unkB0;
    /*0xB4*/ float unkB4;
    /*0xB8*/ float unkB8;
    /*0xBC*/ float unkBC;
    /*0xC0*/ s32 unkC0;
    /*0xC4*/ void *partModels[0x28];
    /*0x164*/ void *eyeModels[0xF];
    /*0x1A0*/ void *eyeModelsAlt[0xF];
    /*0x1DC*/ void *faceModels[0xA];
    /*0x204*/ void *handModels[4];
    /*0x214*/ float unk214;
    /*0x218*/ u16 unk218;
    u8 unk21A[2];
    /*0x21C*/ s32 unk21C;
    /*0x220*/ s32 unk220;
    /*0x224*/ s32 unk224;
    u8 unk228[0x23C - 0x228];
    /*0x23C*/ float unk23C;
    /*0x240*/ float unk240;
    /*0x244*/ s32 charaId;
    /*0x248*/ u32 flags;
    /*0x24C*/ s32 unk24C;
    /*0x250*/ s32 unk250;
    /*0x254*/ s32 unk254;
    /*0x258*/ Vec pos;
    /*0x264*/ Vec unk264;
    /*0x270*/ Vec unk270;
    /*0x27C*/ s32 unk27C;
    /*0x280*/ float scale;
    u8 unk284[4];
    /*0x288*/ Quat rot;
    /*0x298*/ s32 apeIdx;
    /*0x29C*/ s32 unk29C;
    /*0x2A0*/ s32 lod;
    /*0x2A4*/ u32 unk2A4;
    /*0x2A8*/ Quat unk2A8;
    /*0x2B8*/ s32 unk2B8;
    /*0x2BC*/ s32 colorIndex;
    u8 unk2C0[8];
    /*0x2C8*/ s8 ballIdx;
    /*0x2C9*/ u8 cameraVisibleMask;
    /*0x2CA*/ s16 unk2CA;
    /*0x2CC*/ struct ApeAnimOut unk2CC[3];
};  /* size = 0x2F0 */

#define BALL_FLAG_09 0x200
#define BALL_FLAG_11 0x800
#define BALL_FLAG_GOAL 0x1000

extern struct Ball balls[];
extern struct Ball *current_ball;

#define BALL_FLAG_08 0x100
#define BALL_FLAG_10 0x400
#define BALL_FLAG_TIMEOVER 0x8000
#define BALL_FLAG_REPLAY ((u64)1 << 40)  /* ball is following a replay (SMB1's BALL_FLAG_24) */

/* Run code for each ball in play, with current_ball pointing at it. */
#define BALL_FOREACH(code) \
    { \
        struct Ball *ball; \
        struct Ball *ballBackup_; \
        int i_; \
        ballBackup_ = current_ball; \
        ball = balls; \
        for (i_ = 0; i_ < 8; i_++, ball++) \
        { \
            if (ball->status == 2) \
            { \
                current_ball = ball; \
                { code } \
            } \
        } \
        current_ball = ballBackup_; \
    }

#endif
