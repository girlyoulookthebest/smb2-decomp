#ifndef BOAT_H
#define BOAT_H

#include "types.h"
#ifndef __MWERKS__
/* PC port: GCC scopes structs first named in a prototype to that prototype */
struct BoatItemInfo;
struct BoatWaveTev;
struct Item;
struct PhysicsBall;
struct Sprite;
struct Ball;
struct Camera;
struct Effect;
struct GMAModel;
struct Stage;
#endif
#include "ape_internal.h"
#include "avdisp.h"
#include "ball.h"
#include "camera.h"
#include "dip.h"
#include "effect.h"
#include "frustum.h"
#include "libc.h"
#include "light.h"
#include "mathutil.h"
#include "mode.h"
#include "pool.h"
#include "rand.h"
#include "stage.h"
#include "stcoli.h"
#include "world.h"

struct BoatTime
{
    u8 min;
    u8 sec;
    u8 cs;
};

struct BoatLap
{
    /*0x0*/ s32 unk0;
    /*0x4*/ float unk4;
    /*0x8*/ struct BoatTime unk8;
    /*0xB*/ u8 unkB;
};  /* size = 0xC */

struct BoatRecord
{
    /*0x0*/ u8 filler0[7];
    /*0x7*/ u8 chara;
    /*0x8*/ u8 time[3];
};  /* size = 0xB */

struct BoatCpuPoint
{
    /*0x0*/ float unk0;
    /*0x4*/ float unk4;
    /*0x8*/ s16 unk8;
};  /* size = 0xC */

#define BOAT_TICKS(t) ((u32) ((OSGetTick() - (t)) * 8) / ((*(u32 *) 0x800000F8 / 4) / 125000))

extern s16 lbl_14_data_1B4;

struct BoatCourse
{
    /*0x000*/ char **names;
    /*0x004*/ s16 stageId;
    /*0x006*/ s16 unk6;
    /*0x008*/ float unk8;
    /*0x00C*/ float unkC;
    /*0x010*/ u16 unk10;
    /*0x012*/ u16 unk12;
    /*0x014*/ float *unk14;
    /*0x018*/ float unk18;
    /*0x01C*/ float *unk1C[0x3];
};  /* size = 0x28 */

struct BoatStartInfo
{
    /*0x000*/ Vec unk0[0x4];
    /*0x030*/ u8 unk30[0x28];
};  /* size = 0x58 */

struct BoatInfo20
{
    /*0x000*/ u8 unk0;
    /*0x001*/ u8 unk1;
    /*0x002*/ u16 unk2;
};  /* size = 0x4 */

struct BoatObst
{
    /*0x000*/ s8 type;
    /*0x001*/ s8 owner;
    /*0x002*/ s16 timer;
    /*0x004*/ Vec pos;
    /*0x010*/ Vec prevPos;
    /*0x01C*/ u8 filler1C[0xC];
};  /* size = 0x28 */

struct BoatVortexVtx
{
    /*0x000*/ Vec pos;
    /*0x00C*/ u8 fillerC[0x34];
};  /* size = 0x40 */

struct BoatVortexMesh
{
    /*0x000*/ struct GMAModel *model;
    /*0x004*/ u32 *unk4;
    /*0x008*/ struct BoatVortexVtx *verts;
    /*0x00C*/ Vec *orig;
    /*0x010*/ s32 count;
};  /* size = 0x14 */

struct BoatVortex
{
    /*0x000*/ Vec pos;
    /*0x00C*/ float radius;
    /*0x010*/ float strength;
    /*0x014*/ float alpha;
    /*0x018*/ s16 timer;
    /*0x01A*/ s16 duration;
    /*0x01C*/ s16 angle;
    /*0x01E*/ s16 sound;
};  /* size = 0x20 */

struct BoatPlayer
{
    /*0x000*/ u8 unk0;
    /*0x001*/ s8 unk1;
    /*0x002*/ u8 unk2;
    /*0x003*/ u8 unk3;
    /*0x004*/ u8 unk4;
    /*0x005*/ u8 unk5;
    /*0x006*/ u8 unk6;
    /*0x007*/ u8 unk7;
    /*0x008*/ u32 unk8;
    /*0x00C*/ u32 unkC;
    /*0x010*/ s32 unk10;
    /*0x014*/ u8 unk14;
    /*0x015*/ u8 unk15;
    /*0x016*/ u8 unk16;
    /*0x017*/ u8 unk17;
    /*0x018*/ u8 unk18;
    /*0x019*/ u8 unk19;
    /*0x01A*/ u8 unk1A;
    /*0x01B*/ u8 unk1B;
    /*0x01C*/ u8 unk1C;
    /*0x01D*/ u8 unk1D;
    /*0x01E*/ s16 unk1E[0x4];
    /*0x026*/ s16 unk26[0x4];
    /*0x02E*/ u8 filler2E[0x2];
    /*0x030*/ float unk30;
    /*0x034*/ float unk34;
    /*0x038*/ s16 unk38;
    /*0x03A*/ s16 unk3A;
    /*0x03C*/ s32 unk3C;
    /*0x040*/ s32 unk40;
    /*0x044*/ float unk44;
    /*0x048*/ u8 filler48[0x4];
    /*0x04C*/ float unk4C;
    /*0x050*/ struct BoatLap unk50[0x32];
    /*0x2A8*/ struct BoatLap unk2A8;
    /*0x2B4*/ struct BoatLap unk2B4;
    /*0x2C0*/ struct BoatLap unk2C0;
    /*0x2CC*/ struct BoatLap unk2CC;
    /*0x2D8*/ u16 unk2D8;
    /*0x2DA*/ u8 filler2DA[0x2];
    /*0x2DC*/ float unk2DC;
    /*0x2E0*/ float unk2E0;
    /*0x2E4*/ Vec unk2E4;
    /*0x2F0*/ Vec unk2F0;
    /*0x2FC*/ float unk2FC;
    /*0x300*/ float unk300;
    /*0x304*/ float unk304;
    /*0x308*/ float unk308;
    /*0x30C*/ float unk30C;
    /*0x310*/ void *unk310;
    /*0x314*/ float unk314;
    /*0x318*/ float unk318;
    /*0x31C*/ Vec unk31C;
    /*0x328*/ Vec unk328;
    /*0x334*/ struct BoatCpuPoint *unk334;
    /*0x338*/ float unk338;
    /*0x33C*/ float unk33C;
    /*0x340*/ Vec unk340;
    /*0x34C*/ Vec unk34C;
    /*0x358*/ s16 unk358;
    /*0x35A*/ u8 filler35A[0x2];
    /*0x35C*/ float unk35C;
    /*0x360*/ Mtx unk360;
    /*0x390*/ Vec unk390;
    /*0x39C*/ Vec unk39C;
    /*0x3A8*/ float unk3A8;
    /*0x3AC*/ float unk3AC;
    /*0x3B0*/ float unk3B0;
    /*0x3B4*/ float unk3B4;
    /*0x3B8*/ Vec unk3B8;
    /*0x3C4*/ Vec unk3C4;
    /*0x3D0*/ Vec unk3D0;
    /*0x3DC*/ float unk3DC;
    /*0x3E0*/ s16 unk3E0;
    /*0x3E2*/ s16 unk3E2;
    /*0x3E4*/ s16 unk3E4;
    /*0x3E6*/ s16 unk3E6;
    /*0x3E8*/ s16 unk3E8;
    /*0x3EA*/ u8 filler3EA[0x2];
    /*0x3EC*/ u32 unk3EC;
    /*0x3F0*/ u16 unk3F0;
    /*0x3F2*/ u16 unk3F2;
    /*0x3F4*/ float unk3F4[0x8];
    /*0x414*/ Vec unk414[0x8];
    /*0x474*/ u16 unk474;
    /*0x476*/ u16 unk476;
    /*0x478*/ float unk478;
    /*0x47C*/ Vec unk47C;
    /*0x488*/ u16 unk488;
    /*0x48A*/ u16 unk48A;
    /*0x48C*/ float unk48C;
    /*0x490*/ float unk490;
    /*0x494*/ float unk494;
    /*0x498*/ u32 unk498[0x3];
    /*0x4A4*/ u32 unk4A4;
    /*0x4A8*/ u32 unk4A8;
    /*0x4AC*/ u32 unk4AC;
    /*0x4B0*/ u32 unk4B0;
    /*0x4B4*/ u32 unk4B4;
    /*0x4B8*/ u32 unk4B8;
    /*0x4BC*/ u32 unk4BC[0x5];
    /*0x4D0*/ u32 unk4D0[0x5];
    /*0x4E4*/ float unk4E4;
    /*0x4E8*/ s16 unk4E8;
    /*0x4EA*/ u8 filler4EA[0x2];
    /*0x4EC*/ u32 unk4EC;
};  /* size = 0x4F0 */


struct BoatEffectExtra
{
    /*0x00*/ u8 filler0[8];
    /*0x08*/ float unk8;
    /*0x0C*/ u8 fillerC[4];
};

struct BoatItemInfo
{
    /*0x00*/ u16 flags;
    /*0x02*/ s16 unk2;
    /*0x04*/ float unk4;
    /*0x08*/ float unk8;
    /*0x0C*/ float unkC;
    /*0x10*/ float unk10;
    /*0x14*/ s8 unk14;
    /*0x15*/ s8 unk15;
};

struct BoatParticleParam
{
    /*0x00*/ Vec *pos;
    /*0x04*/ Vec *vel;
    /*0x08*/ Vec accel;
    /*0x14*/ u32 flags;
    /*0x18*/ float velScale;
    /*0x1C*/ float drag;
    /*0x20*/ float scale;
    /*0x24*/ float posScale;
    /*0x28*/ s16 life;
    /*0x2A*/ s16 count;
    /*0x2C*/ s8 *models;
    /*0x30*/ s8 numModels;
    /*0x32*/ s16 fadeTime;
};

extern s16 lbl_14_bss_0;
extern s32 lbl_14_bss_4;
extern u32 lbl_14_bss_8;
extern u32 lbl_14_bss_C;
extern u32 lbl_14_bss_10;
extern s32 lbl_14_bss_14;
extern void * lbl_14_bss_18;
extern u32 lbl_14_bss_1C;
extern struct BoatInfo20 lbl_14_bss_20;
extern u8 lbl_14_bss_24;
extern u8 lbl_14_bss_25;
extern u8 lbl_14_bss_28[4];
extern u8 lbl_14_bss_2C[4];
extern s32 lbl_14_bss_30[0xC];
extern s32 lbl_14_bss_60;
extern u32 lbl_14_bss_64;
extern s8 lbl_14_bss_68;
extern u32 lbl_14_bss_6C;
extern void * lbl_14_bss_70;
extern void * lbl_14_bss_74;
extern void * lbl_14_bss_78;
extern void * lbl_14_bss_7C;
extern s32 lbl_14_bss_80;
extern s8 lbl_14_bss_84[8];
extern s8 lbl_14_bss_8C[8];
extern struct BoatRecord lbl_14_bss_94[2];
extern struct BoatInfo20 * lbl_14_bss_AC;
extern s32 lbl_14_bss_B0;
extern struct GMAModel * lbl_14_bss_B4[0x4D];
extern s32 (* lbl_14_bss_1E8)(float);
extern float lbl_14_bss_1EC;
extern struct GMAModel * lbl_14_bss_1F0[4];
extern struct GMAModel * lbl_14_bss_200;
extern struct GMAModel * lbl_14_bss_204;
extern struct BoatPlayer lbl_14_bss_208[8];
extern struct BoatPlayer * lbl_14_bss_2988[8];
extern struct BoatPlayer * lbl_14_bss_29A8;
extern Vec lbl_14_bss_29AC;
extern Vec lbl_14_bss_29B8;
extern struct BoatPlayer * lbl_14_bss_29C4[8];
extern struct BoatPlayer * lbl_14_bss_29E4[9];
extern sptr lbl_14_bss_2A78;
extern sptr lbl_14_bss_2A7C;
extern float lbl_14_bss_2A80;
extern struct BoatCourse * lbl_14_bss_2AE8;
extern u16 lbl_14_bss_2AEC;
extern u16 lbl_14_bss_2AEE;
extern struct BoatStartInfo lbl_14_bss_2AF0;
extern float lbl_14_bss_2B48;
extern struct BoatItemInfo lbl_14_bss_2B4C[0x100];
extern s32 lbl_14_bss_434C;
extern struct BoatEffectExtra lbl_14_bss_4350[0x200];
extern u16 * lbl_14_bss_6350[4];
extern struct BoatObst lbl_14_bss_B7E8[0x20];
extern s32 lbl_14_bss_BCE8;
extern struct BoatVortexMesh lbl_14_bss_BCEC[2];
extern struct BoatVortex lbl_14_bss_BD14[0x20];

extern float lbl_80198B34;
void PSVECScale(Vec *src, Vec *dst, float scale);
void PSVECAdd(Vec *a, Vec *b, Vec *dst);
void PSVECSubtract(Vec *a, Vec *b, Vec *dst);
extern int currentHeap;
extern u32 lbl_1_bss_D4328;
extern float lbl_1_data_657E4;
extern float lbl_1_data_657F8;
void fn_1_11355C(struct Ape *ape, float speed);
void fn_1_113190(struct Ape *ape, int b, float e, int c, int d);
void *fn_8006991C(struct GMAModel *model);
void *fn_80069870(struct GMAModel *model);
void SoundOffID(u32 id);
void *g_yet_another_unk_draw_func(void *func);
void g_GXSetIndTexMtx_wrapper(s32 id, void *mtx, s32 scale);
void g_rumble_controller(s32 pad, s32 a, s32 b);

void fn_14_0(void);
void _prolog(void);
void _epilog(void);
void _unresolved(void);
void fn_14_138(void);
void fn_14_674(void);
void fn_14_AD0(void);
void fn_14_AF0(void);
void fn_14_BC8(s16 arg0);
s16 fn_14_BE8(void);
void fn_14_BF8(void);
s32 fn_14_CB8(const void *arg0, const void *arg1);
void fn_14_DB0(void);
void fn_14_E24(void);
void g_load_boat(void);
void fn_14_16C0(void);
void fn_14_18B4(void);
void fn_14_193C(void);
void fn_14_1A24(void);
void fn_14_1A84(void);
void fn_14_1C54(void);
void fn_14_23C0(void);
void fn_14_2438(void);
void fn_14_2620(void);
void fn_14_2890(void);
void fn_14_2A48(void);
void fn_14_2AA4(void);
void fn_14_2BE4(void);
void fn_14_2F38(void);
void fn_14_3084(void);
void fn_14_31A4(void);
void fn_14_32FC(void);
void fn_14_33E8(u8 arg0);
void fn_14_3418(void);
void fn_14_469C(void);
void fn_14_4718(void);
void fn_14_4808(void);
void fn_14_4934(struct Ball *ball, struct BoatPlayer *pl);
void fn_14_49FC(void);
void fn_14_4BA0(void);
s32 fn_14_4FFC(Vec *pos, float *out);
s32 fn_14_5100(Vec *pos, float *out);
void fn_14_5240(struct Ball *ball, Vec *pos, float scale, s16 rot);
void fn_14_53D4(void);
void fn_14_58A0(void);
void fn_14_58F8(s32 group);
void fn_14_59D8(s32 group);
void fn_14_5AE0(s32 group);
void fn_14_5BF8(s32 group);
s32 fn_14_5D9C(float y);
s32 fn_14_5E08(float y);
void fn_14_5E10(struct GMAModel *model);
void fn_14_5F18(struct GMAModel *model, float scale);
void fn_14_6024(struct BoatPlayer *pl);
void fn_14_605C(void);
s32 fn_14_613C(struct GMAModel *model);
void fn_14_62A4(void);
void fn_14_639C(void);
void fn_14_642C(struct Ball *ball);
void fn_14_65E4(void);
void fn_14_66DC(struct Ball *ball);
void fn_14_789C(struct Ball *ball, s32 state);
void fn_14_78A8(struct Ball *ball, s32 state);
void fn_14_78F0(void);
void fn_14_7B50(void);
void fn_14_82BC(void);
void fn_14_8768(struct Ball *ball, struct BoatPlayer *pl);
s32 fn_14_8848(struct BoatLap *lap, struct BoatLap *best);
void fn_14_892C(void);
s32 fn_14_8AA0(void *arg0, void *arg1);
void fn_14_8D1C(void);
void fn_14_8E70(struct Ball *ball, struct BoatPlayer *pl);
void fn_14_9064(struct Ball *ball, struct BoatPlayer *pl, u16 item);
void fn_14_9A74(struct Ball *ball, struct BoatPlayer *pl, u16 item);
void fn_14_9F18(struct Ball *ball, struct BoatPlayer *pl);
void fn_14_A0E8(void);
void fn_14_A390(void);
s8 fn_14_A588(struct BoatPlayer *pl);
void fn_14_A6CC(struct Ball *ball);
void fn_14_B37C(struct Ball *ball);
void fn_14_B3B8(struct Ball *ball);
void fn_14_B420(struct Ball *ball);
void fn_14_B5F8(struct Ball *ball);
void fn_14_B66C(struct Ball *ball);
void fn_14_BA58(struct Ball *ball);
void fn_14_BC1C(struct Ball *ball);
void fn_14_BE04(struct Ball *ball);
void fn_14_BEF8(struct Ball *ball);
void fn_14_C01C(struct Ball *ball);
void fn_14_C200(struct Ball *ball);
void fn_14_C23C(struct Ball *ball);
void fn_14_C2D0(struct Ball *ball);
void fn_14_C370(struct Ball *ball);
void fn_14_C498(struct Ball *ball, struct BoatPlayer *pl);
void fn_14_C7A4(struct Ball *ball, struct BoatPlayer *pl);
float fn_14_CA68(struct Ball *ball, struct BoatPlayer *pl);
void fn_14_CB3C(struct Ball *ball, struct BoatPlayer *pl);
void fn_14_CED4(struct Ball *ball, struct BoatPlayer *pl);
void fn_14_CFA0(struct Ball *ball, struct BoatPlayer *pl);
void fn_14_D4B4(struct Ball *ball, struct BoatPlayer *pl);
void fn_14_D800(struct Ball *ball, struct BoatPlayer *pl);
void fn_14_D81C(u8 *value, u8 target, u8 rate);
void fn_14_D864(struct Ball *ball, struct BoatPlayer *pl);
void fn_14_D900(s16 *cur, s16 *target, float step);
void fn_14_DADC(struct Ball *ball, struct BoatPlayer *pl);
void fn_14_DD70(struct Ball *ball, struct BoatPlayer *pl);
void fn_14_E0FC(void);
void fn_14_E148(struct Camera *camera, struct Ball *ball);
void fn_14_E2C4(void);
void fn_14_E3CC(struct Camera *camera, s8 mode);
void fn_14_E3E4(void);
void fn_14_E50C(void);
void fn_14_E638(void);
void fn_14_E760(void);
void fn_14_EAE0(struct Camera *camera, struct Ball *ball);
void fn_14_ED00(struct Camera *camera, struct Ball *ball);
void fn_14_F3D4(struct Camera *camera, struct Ball *ball);
void fn_14_F40C(struct Camera *camera, struct Ball *ball);
void fn_14_F5E8(struct Camera *camera, struct Ball *ball);
void fn_14_F620(struct Camera *camera, struct Ball *ball);
void fn_14_F8DC(struct Camera *camera, struct Ball *ball);
void fn_14_F928(struct Camera *camera, struct Ball *ball);
void fn_14_FA14(struct Camera *camera, struct Ball *ball);
void fn_14_FB5C(struct Camera *camera, struct Ball *ball);
void fn_14_FD40(struct Camera *camera, struct Ball *ball);
void fn_14_FDA8(struct Camera *camera, struct Ball *ball);
void fn_14_FEDC(struct Camera *camera, struct Ball *ball);
void fn_14_1003C(struct Camera *camera, struct Ball *ball);
void fn_14_102C4(struct Camera *camera, struct Ball *ball);
void fn_14_1032C(struct Camera *camera, struct Ball *ball);
void fn_14_1045C(struct Camera *camera, struct Ball *ball);
void fn_14_108A4(struct Ball *ball, struct BoatPlayer *pl);
void fn_14_10B28(struct Ball *ball, struct BoatPlayer *pl);
void fn_14_10CE4(struct Ball *ball, struct BoatPlayer *pl);
void fn_14_10DE0(struct Ball *ball, struct BoatPlayer *pl);
void fn_14_10E58(struct Ball *ball, struct BoatPlayer *pl);
void fn_14_11298(struct Ball *ball, struct BoatPlayer *pl);
void fn_14_113A4(struct Ball *ball, struct BoatPlayer *pl);
void fn_14_11524(struct StageExtraChannels *path, float t, Vec *out);
float fn_14_115F4(struct StageExtraChannels *path, Vec *pos, float step);
float fn_14_11818(struct StageExtraChannels *path, Vec *pos, float t);
float fn_14_11CE8(struct StageExtraChannels *path, Vec *pos, float step);
float fn_14_11F0C(struct StageExtraChannels *path, Vec *pos, float t);
void fn_14_123D0(s32 frames, float frac, struct BoatTime *out);
s32 fn_14_12504(struct GMA *gma, char *name);
struct GMAModel *fn_14_12598(struct GMA *gma, char *name);
void fn_14_12638(struct Camera *cam, Vec *out, float x, float y);
void fn_14_1276C(struct BoatTime *a, struct BoatTime *b, struct BoatTime *out);
u32 fn_14_128E4(u32 seed);
u16 fn_14_12904(void);
float fn_14_12944(void);
void fn_14_129C0(Vec *axis, s16 angle);
void fn_14_12B24(s32 sound, void *pos);
void fn_14_12BF4(s32 sound, void *pos);
void fn_14_12C7C(void);
void fn_14_12F3C(void);
void fn_14_13144(void);
void fn_14_136D4(void);
void fn_14_13AB0(void);
void fn_14_13B94(void);
float fn_14_13C6C(Vec *pos);
s32 fn_14_13CA8(Vec *pos, struct RaycastHit *hit, u32 mask);
float fn_14_13E1C(Vec *pos, Vec *nrm);
float fn_14_1400C(Vec *pos, Vec *nrm);
float fn_14_14114(Vec *pos, Vec *normal);
void fn_14_14284(void);
void fn_14_14334(struct BoatWaveTev *tev);
void fn_14_14644(void);
void fn_14_14720(void);
s32 fn_14_14890(Vec *pos, struct RaycastHit *hit, u32 flags);
void fn_14_14B28(struct StageColiTri *in, struct StageColiTri *out);
s32 fn_14_1520C(Vec *pos, Vec *dir, struct StageColiTri *tri);
void fn_14_153B4(u8 course);
u8 fn_14_155F0(u8 course);
void fn_14_15610(void);
void fn_14_15614(void);
void fn_14_15618(void);
void fn_14_1561C(void);
void fn_14_15620(void);
void fn_14_15624(void);
void fn_14_156C0(void);
void fn_14_15720(void);
void fn_14_15834(void);
void fn_14_15880(struct Item *item, struct BoatItemInfo *info);
void fn_14_159CC(void);
void fn_14_15CA0(void);
void fn_14_15FEC(struct Item *item);
void fn_14_16090(struct Item *item);
void fn_14_160B4(struct Item *item);
void fn_14_16168(struct Item *item);
void fn_14_16810(struct Item *item, struct PhysicsBall *unused);
void fn_14_16ABC(struct Item *item);
void fn_14_16B90(struct Item *item);
void fn_14_171E0(struct Item *item, struct PhysicsBall *phys);
void fn_14_172D0(struct Item *item);
void fn_14_173B4(struct Item *item);
void fn_14_17A10(struct Item *item, struct PhysicsBall *phys);
void fn_14_17A90(struct Item *item);
void fn_14_17B64(struct Item *item);
void fn_14_18160(struct Item *item, struct PhysicsBall *phys);
void fn_14_18250(struct Item *item, float height);
void fn_14_183EC(struct Item *item, float height);
void fn_14_1853C(struct Item *item, float height);
void fn_14_1864C(void);
void fn_14_186A8(void);
void fn_14_186F4(struct Effect *effect, struct BoatEffectExtra *extra);
void fn_14_187A4(void);
void fn_14_18844(void);
void fn_14_188D8(void);
void fn_14_1895C(struct Effect *effect);
void fn_14_18990(struct Effect *effect);
void fn_14_189B4(struct Effect *effect);
void fn_14_18A20(struct Effect *effect);
void fn_14_18A24(struct Effect *effect);
void fn_14_18A94(struct Effect *effect);
void fn_14_18B50(struct Effect *effect);
void fn_14_18BFC(struct Effect *effect);
void fn_14_18C00(struct Effect *effect);
void fn_14_18C90(struct Effect *effect);
void fn_14_18D60(struct Effect *effect);
void fn_14_18E28(struct Effect *effect);
void fn_14_18E2C(struct Effect *effect);
void fn_14_18E48(struct Effect *effect);
void fn_14_18FB0(struct Effect *effect);
void fn_14_19050(struct Effect *effect);
void fn_14_19054(struct Effect *effect);
void fn_14_190C8(struct Effect *effect);
void fn_14_191A0(struct Effect *effect);
void fn_14_1928C(struct Effect *effect);
void fn_14_19290(struct Effect *effect);
void fn_14_1930C(struct Effect *effect);
void fn_14_19520(struct Effect *effect);
void fn_14_19614(struct Effect *effect);
void fn_14_19618(float seconds);
void fn_14_19A14(s8 *status, struct Sprite *sprite);
void fn_14_19BC8(struct Sprite *sprite);
void fn_14_19CDC(void);
void fn_14_19DB0(s8 *status, struct Sprite *sprite);
void fn_14_19F48(void);
void fn_14_1A0E4(s8 *status, struct Sprite *sprite);
void fn_14_1A1BC(u8 playerId);
void fn_14_1A5C8(void);
void fn_14_1A664(s8 *status, struct Sprite *sprite);
void fn_14_1A6EC(struct Sprite *sprite);
void fn_14_1A918(void);
void fn_14_1ABB8(s8 *status, struct Sprite *sprite);
void fn_14_1AC0C(void);
void fn_14_1AD14(float a, float b);
void fn_14_1ADF8(float a, float b);
void fn_14_1AED0(u8 playerId);
void fn_14_1B0CC(s8 *status, struct Sprite *sprite);
void fn_14_1B124(u8 playerId);
void fn_14_1B3AC(s8 *status, struct Sprite *sprite);
void fn_14_1B490(void);
void fn_14_1B550(u8 playerId);
void fn_14_1B7BC(s8 *status, struct Sprite *sprite);
void fn_14_1B8C4(u8 playerId);
void fn_14_1BD00(struct Sprite *sprite);
void fn_14_1BF00(s8 *status, struct Sprite *sprite);
void fn_14_1C37C(struct Sprite *sprite);
void fn_14_1C5D0(struct Sprite *sprite);
void fn_14_1C824(u8 playerId);
void fn_14_1C95C(struct Sprite *sprite);
void fn_14_1CBBC(u8 playerId);
void fn_14_1CE44(s8 *status, struct Sprite *sprite);
void fn_14_1CEC0(s8 *status, struct Sprite *sprite);
void fn_14_1CF24(u8 playerId);
void fn_14_1D190(struct Sprite *sprite);
u32 fn_14_1D380(u32 flag);
void fn_14_1D3B4(u8 playerId);
void fn_14_1D69C(s8 *status, struct Sprite *sprite);
void fn_14_1D7EC(u8 playerId);
void fn_14_1DCFC(s8 *status, struct Sprite *sprite);
void fn_14_1DD94(s8 *status, struct Sprite *sprite);
void fn_14_1DFC8(u8 playerId);
void fn_14_1E3A8(s8 *status, struct Sprite *sprite);
void fn_14_1E474(u8 playerId);
void fn_14_1E854(s8 *status, struct Sprite *sprite);
void fn_14_1EAE0(void);
void fn_14_1EB84(s8 *status, struct Sprite *sprite);
void fn_14_1EC1C(s16 index);
void fn_14_1EDC0(s8 *status, struct Sprite *sprite);
void fn_14_1F018(struct Sprite *sprite);
void fn_14_1F358(float x, float y);
void fn_14_1F4FC(struct Sprite *sprite);
void fn_14_1F64C(void);
void fn_14_1F7BC(struct Sprite *sprite);
void fn_14_1FC2C(u8 playerId);
void fn_14_20014(s8 *status, struct Sprite *sprite);
void fn_14_20124(u8 playerId);
void fn_14_2039C(struct Sprite *sprite);
void fn_14_208E4(void);
void fn_14_20B1C(s8 *status, struct Sprite *sprite);
void fn_14_20C24(void);
void fn_14_20DB0(s8 *status, struct Sprite *sprite);
void fn_14_20DC0(struct Sprite *sprite);
void fn_14_21098(void);
void fn_14_211EC(void);
void fn_14_213D4(void);
void fn_14_21470(void);
void fn_14_215F8(struct BoatParticleParam *param);
void fn_14_2190C(struct BoatParticleParam *param);
void fn_14_21980(void);
void fn_14_219C4(void);
void fn_14_21A04(void);
void fn_14_22024(struct Ball *ball, struct BoatPlayer *pl, struct PhysicsBall *phys);
void fn_14_220C0(Vec *normal, Vec *velA, float massA, Vec *velB, float massB);
void fn_14_22254(struct PhysicsBall *ball, struct StageDef *stagedef);
void fn_14_224A8(struct PhysicsBall *ball, struct StageColiTri *tri);
void fn_14_22760(struct PhysicsBall *ball, struct ColiPlane *coliPlane);
void fn_14_22A9C(void);
void fn_14_22B80(void);
void fn_14_22FBC(void);
void fn_14_234F4(void);
void fn_14_23554(struct BoatObst *req);
void fn_14_2363C(void);
void fn_14_236A4(struct Ball *ball, struct BoatPlayer *player);
void fn_14_23750(struct BoatObst *obst, struct BoatObst *req);
void fn_14_23830(struct BoatObst *obst, struct Ball *ball, struct BoatPlayer *player);
void fn_14_239C0(void);
void fn_14_23B78(void);
void fn_14_23CE0(void);
void fn_14_23E2C(void);
void fn_14_23EB4(Vec *pos, float a, float b, float c, s16 time);
float fn_14_23F98(Vec *pos, Vec *out);
void fn_14_241F4(struct BoatVortex *vortex);
void fn_14_242E0(void);
void fn_14_2435C(struct BallEnvArg *arg0);
s16 fn_14_246A4(struct BoatVortex *vortex);
void fn_14_24758(s16 sound);
void fn_14_247EC(void);

#endif
