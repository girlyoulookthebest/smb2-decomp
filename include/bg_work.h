#ifndef _BG_WORK_H_
#define _BG_WORK_H_

/* world_theme.work layouts that effect.c reads as well as the owning
 * bg_*.c file.  They used to be read through hard-coded 32-bit offsets
 * (BG_WORK(void **)[99], bgWork + 0xB0C, ...); sharing the definitions keeps
 * those reads right on x64, where the pointer fields grow. */

#include "types.h"
#include "gx.h"
#include "mathutil.h"

struct BallEnvArg;
struct GMAModel;
struct StageBgModel;

#ifndef __MWERKS__
void *pc_bg_work(void);  /* world_theme.work (background.c) */
#endif

struct BgEndWork
{
    /*0x00*/ s32 initialized;
    /*0x04*/ struct GMAModel *fountainWaterModel;
    /*0x08*/ struct GMAModel *waterModel;
    /*0x0C*/ GXTexObj *waterSpecTex;
    /*0x10*/ GXTexObj *cloudTex;
    /*0x14*/ void (*unk14)(struct BallEnvArg *);
    /*0x18*/ void (*unk18)(struct BallEnvArg *);
    /*0x1C*/ Vec unk1C;
    /*0x28*/ float unk28;
    /*0x2C*/ float unk2C;
    u8 filler30[4];
    /*0x34*/ float unk34;
    /*0x38*/ float unk38;
    /*0x3C*/ float unk3C;
    /*0x40*/ Vec unk40;
    /*0x4C*/ Vec unk4C;
    /*0x58*/ float unk58;
    /*0x5C*/ float unk5C;
    u8 filler60[4];
    /*0x64*/ Mtx unk64;
};

struct BgMstCloud
{
    /*0x00*/ struct StageBgModel *model;
    /*0x04*/ Vec texPos;
    /*0x10*/ Vec texVel;
    /*0x1C*/ Vec texTargetVel;
    /*0x28*/ Mtx texMtx;
};  /* size = 0x58 */

struct BgMstParadise
{
    /*0x00*/ struct StageBgModel *model;
    /*0x04*/ float spawnTimer;
    /*0x08*/ float spawnRate;
    /*0x0C*/ float spawnRateBase;
    /*0x10*/ s16 angle;
    /*0x12*/ s16 angleVel;
    /*0x14*/ Vec texPos;
    /*0x20*/ Vec texVel;
    /*0x2C*/ Vec texScale;
    /*0x38*/ Mtx texMtxA;
    /*0x68*/ Mtx texMtxB;
};  /* size = 0x98 */

struct BgMstWork
{
    /*0x000*/ s32 initialized;
    /*0x004*/ GXTexObj *cloudTex;
    /*0x008*/ s32 cloudCount;
    /*0x00C*/ struct BgMstCloud clouds[4];
    /*0x16C*/ s32 cloudVelRerolled;
    /*0x170*/ s32 paradiseCount;
    /*0x174*/ struct BgMstParadise paradises[16];
    /*0xAF4*/ s32 miniCloudCount;
    /*0xAF8*/ struct GMAModel *miniClouds[4];
    /*0xB08*/ struct GMAModel *sprayCloud;
    /*0xB0C*/ struct GMAModel *waterModel;
    /*0xB10*/ GXTexObj *waterSpecTex;
    /*0xB14*/ void (*waterEnvFunc)(struct BallEnvArg *);
    /*0xB18*/ Vec waterPos;
    /*0xB24*/ Vec waterVel;
    /*0xB30*/ Vec waterBaseScale;
    /*0xB3C*/ Vec waterScale;
    /*0xB48*/ Vec specPos;
    /*0xB54*/ Vec specVel;
    /*0xB60*/ Mtx specMtx;
};

struct BgWatWork
{
    /*0x000*/ s32 unk0;
    u8 filler4[0x10 - 0x4];
    /*0x010*/ s16 unk10;
    /*0x012*/ s16 unk12;
    /*0x014*/ s16 unk14;
    /*0x016*/ s16 unk16;
    /*0x018*/ s16 unk18;
    /*0x01A*/ s16 unk1A;
    /*0x01C*/ u16 unk1C;
    /*0x01E*/ u16 unk1E;
    /*0x020*/ float unk20;
    /*0x024*/ struct GMAModel *waterSurfaceMat;
    /*0x028*/ GXTexObj *causticTex;
    /*0x02C*/ GXTexObj *waterSurfaceTestTex;
    /*0x030*/ GXTexObj *lightmapTex;
    /*0x034*/ GXTexObj *lightmapStageTex;
    /*0x038*/ GXTexObj *lightmapGradTex;
    /*0x03C*/ Mtx unk3C;
    /*0x06C*/ Mtx unk6C;
    /*0x09C*/ Mtx unk9C;
    /*0x0CC*/ Mtx unkCC;
    /*0x0FC*/ Mtx unkFC;
    /*0x12C*/ Mtx unk12C;
    /*0x15C*/ Mtx unk15C;
    /*0x18C*/ struct GMAModel *bubbleModel;
    /*0x190*/ struct StageBgModel *waterSurface;
};

struct BgBnsStarpoint
{
    /*0x00*/ Vec u_pos;
    /*0x0C*/ s16 pulse;
    /*0x0E*/ s16 pulseSpeed;
    /*0x10*/ float red;
    /*0x14*/ float green;
    /*0x18*/ float blue;
};

struct BgBnsWork
{
    /*0x000*/ s32 initialized;
    /*0x004*/ struct StageBgModel *mainObj;
    /*0x008*/ struct GMAModel *shotstarModel;
    /*0x00C*/ struct GMAModel *starlightModel;
    /*0x010*/ s32 starpointCount;
    /*0x014*/ struct BgBnsStarpoint starpoints[64];
    /*0x714*/ GXTexObj *lightmapTexObjs;
    /*0x718*/ GXTexObj *lightmapATexObjs;
    /*0x71C*/ Mtx unk71C;
    /*0x74C*/ Mtx unk74C;
    /*0x77C*/ Mtx unk77C;
    /*0x7AC*/ Mtx unk7AC;
    /*0x7DC*/ Mtx unk7DC;
};

#endif
