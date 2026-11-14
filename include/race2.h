#ifndef RACE2_H
#define RACE2_H

#include "types.h"
#include "ape_internal.h"
#include "avdisp.h"
#include "ball.h"
#include "camera.h"
#include "dip.h"
#include "effect.h"
#include "frustum.h"
#include "light.h"
#include "mathutil.h"
#include "mode.h"
#include "pool.h"
#include "rand.h"
#include "stage.h"
#include "stcoli.h"
#include "world.h"

struct RaceCheckpoint
{
    s32 unk0;
    float unk4;
};

struct RacePlayer
{
    /*0x000*/ u16 unk0;
    /*0x002*/ s8 unk2;
    /*0x003*/ s8 unk3;
    /*0x004*/ u16 unk4;
    /*0x006*/ s16 unk6;
    /*0x008*/ s16 unk8;
    /*0x00A*/ s16 unkA;
    /*0x00C*/ s16 unkC;
    /*0x00E*/ s16 unkE;
    /*0x010*/ s16 unk10;
    /*0x012*/ u8 filler12[0x2];
    /*0x014*/ s32 unk14;
    /*0x018*/ Vec unk18;
    /*0x024*/ Vec unk24;
    /*0x030*/ float unk30;
    /*0x034*/ u32 flags;
    /*0x038*/ u32 unk38;
    /*0x03C*/ s16 unk3C;
    /*0x03E*/ u16 unk3E;
    /*0x040*/ u16 unk40;
    /*0x042*/ s16 unk42;
    /*0x044*/ s16 unk44;
    /*0x046*/ s16 unk46;
    /*0x048*/ struct RaceCheckpoint unk48[0x32];
    /*0x1D8*/ s32 unk1D8;
    /*0x1DC*/ float unk1DC;
    /*0x1E0*/ s32 unk1E0;
    /*0x1E4*/ float unk1E4;
    /*0x1E8*/ u8 unk1E8;
    /*0x1E9*/ u8 unk1E9;
    /*0x1EA*/ u8 unk1EA;
    /*0x1EB*/ u8 filler1EB[0x1];
    /*0x1EC*/ s16 unk1EC;
    /*0x1EE*/ u8 filler1EE[0x2];
    /*0x1F0*/ float unk1F0;
    /*0x1F4*/ float unk1F4;
    /*0x1F8*/ float unk1F8;
    /*0x1FC*/ float unk1FC;
    /*0x200*/ float unk200;
    /*0x204*/ float unk204;
    /*0x208*/ float unk208;
    /*0x20C*/ float unk20C;
    /*0x210*/ float unk210;
    /*0x214*/ float unk214;
    /*0x218*/ float unk218;
    /*0x21C*/ float unk21C;
    /*0x220*/ Vec unk220[0x8];
    /*0x280*/ float unk280[0x8];
    /*0x2A0*/ s16 unk2A0;
    /*0x2A2*/ u8 filler2A2[0x2];
    /*0x2A4*/ float unk2A4;
    /*0x2A8*/ float unk2A8;
    /*0x2AC*/ Vec unk2AC;
    /*0x2B8*/ s16 unk2B8;
    /*0x2BA*/ s16 unk2BA;
    /*0x2BC*/ u16 unk2BC;
    /*0x2BE*/ u16 unk2BE;
    /*0x2C0*/ u16 unk2C0;
    /*0x2C2*/ u16 unk2C2;
    /*0x2C4*/ u16 unk2C4;
    /*0x2C6*/ u16 unk2C6;
    /*0x2C8*/ u16 unk2C8[0x3];
    /*0x2CE*/ s16 unk2CE;
    /*0x2D0*/ float unk2D0;
    /*0x2D4*/ Quat unk2D4;
    /*0x2E4*/ Quat unk2E4;
    /*0x2F4*/ u8 unk2F4[0x8];
    /*0x2FC*/ u8 unk2FC;
    /*0x2FD*/ u8 unk2FD;
    /*0x2FE*/ u8 filler2FE[0x2];
    /*0x300*/ float unk300;
    /*0x304*/ s16 unk304[0xa];
    /*0x318*/ s16 unk318;
    /*0x31A*/ u8 filler31A[0x2];
    /*0x31C*/ s32 unk31C;
    /*0x320*/ float unk320[0x5];
    /*0x334*/ s16 unk334;
    /*0x336*/ s16 unk336;
    /*0x338*/ s16 unk338;
    /*0x33A*/ u8 unk33A[0x20];
    /*0x35A*/ s16 unk35A;
    /*0x35C*/ s8 unk35C;
    /*0x35D*/ s8 unk35D;
    /*0x35E*/ s16 unk35E;
};  /* size = 0x360 */

struct Race2EffectLink
{
    u32 slot0;
    u32 slot1;
};

struct Race2DrawNode
{
    void *next;
    void (*func)(struct Race2DrawNode *);
    s32 lightGroup;
    s16 unkC;
    s16 unkE;
};

struct Race2Course
{
    u16 unk0;
    u16 unk2;
    u16 unk4;
    u16 unk6;
    float unk8;
    float unkC;
    u8 unk10;
    u8 unk11[3];
};

struct Race2MapInfo
{
    s32 bmpId;
    s32 unk4;
    s32 unk8;
    float unkC;
    float unk10;
    float unk14;
    float unk18;
};

struct Race2CourseInfo
{
    char **unk0;
    s16 unk4;
    s16 unk6;
    float unk8;
    float unkC;
    float unk10;
    u8 unk14[7];
    void *unk1C[7];
    void *unk38;
    s32 unk3C;
    s32 unk40;
    s32 unk44;
    float unk48;
    float unk4C;
    float unk50;
    float unk54;
    s16 unk58;
    s16 unk5A;
    float unk5C;
    u16 unk60;
    Vec unk64;
};  /* size = 0x70 */

extern struct GMA *minigameGma;
extern struct GMA *init_common_gma;
extern struct GMAModel *lbl_10_bss_58[53];
extern struct GMAModel *lbl_10_bss_148[8];
extern struct Race2EffectLink lbl_10_bss_1D8[512];
extern struct Race2Course lbl_10_bss_1208;
extern struct Race2CourseInfo lbl_10_data_2ADC[];
extern struct RacePlayer lbl_10_bss_1230[8];
extern struct RacePlayer *lbl_10_bss_2D68[8];
extern s16 lbl_10_data_F48[];

void PSVECScale(Vec *src, Vec *dst, float scale);
void PSVECAdd(Vec *a, Vec *b, Vec *dst);
void PSVECSubtract(Vec *a, Vec *b, Vec *dst);
void vec_to_euler_xy(Vec *v, s16 *x, s16 *y);
void fn_10_181EC(struct StageExtraData *data, float t, Vec *out);

extern float lbl_80198B34;

#endif
