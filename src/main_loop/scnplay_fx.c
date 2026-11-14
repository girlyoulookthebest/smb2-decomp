/* SOFTFX: the scene player's list of soft effects, and one message handler and
 * draw function per effect type. */
#include "camera.h"
#include "types.h"
#include "ape_internal.h"
#include "avdisp.h"
#include "background.h"
#include "ball.h"
#include "dip.h"
#include "effect.h"
#include "event.h"
#include "gx.h"
#include "libc.h"
#include "light.h"
#include "mathutil.h"
#include "mot_joint.h"
#include "pool.h"
#include "rand.h"
#include "shadow.h"
#include "stcoli.h"
#ifndef __MWERKS__
#include "frustum.h" /* prototypes: float args must not be promoted to double on x86 */
#endif

/* The scene player's struct Instance (scnplay.c), as seen by the effects:
 * every field up to ape must keep Instance's layout (also on 64-bit hosts). */
struct SceneObjDesc
{
    /*0x00*/ u16 kind;
    /*0x02*/ u16 idx;
    /*0x04*/ u16 unk4;
    /*0x06*/ u16 color;
    /*0x08*/ char *name;
    /*0x0C*/ u32 flags;
    /*0x10*/ struct AnimJoint *joints;  /* joints[0].scale is the object's scale */
    /*0x14*/ Mtx mtx;
    /*0x44*/ void *dialog;
    /*0x48*/ struct Ape *ape;
};

/* Fields every soft effect starts with; what follows depends on the type. */
#define SOFT_FX_HEADER \
    /*0x00*/ s8 state; \
    /*0x04*/ int heap; \
    /*0x08*/ struct SoftFx *prev; \
    /*0x0C*/ struct SoftFx *next; \
    /*0x10*/ void *owner; \
    /*0x14*/ int type; \
    /*0x18*/ struct SceneObjDesc *desc; \
    /*0x1C*/ u32 drawMask;

struct SoftFx
{
    SOFT_FX_HEADER
    /*0x20*/ GXTexObj tex;
};

struct SoftFxTimed
{
    SOFT_FX_HEADER
    /*0x20*/ int timer;
};

/* A piece of debris flying off an ape, leaving a trail of effects behind it. */
struct SoftFxDebris
{
    SOFT_FX_HEADER
    /*0x20*/ int timer;
    /*0x24*/ float dist;
    /*0x28*/ s16 rotX;
    /*0x2A*/ s16 rotY;
    /*0x2C*/ s16 rotZ;
    /*0x2E*/ s16 rotVelX;
    /*0x30*/ s16 rotVelY;
    /*0x32*/ s16 rotVelZ;
    /*0x34*/ Vec pos;
    /*0x40*/ Vec vel;
};

struct ScenePlayer
{
    /*0x00*/ void *instances;
    /*0x04*/ s32 count;
    /*0x08*/ s32 stage;
    /*0x0C*/ int sceneId;
    /*0x10*/ u8 filler10[2];
    /*0x12*/ u16 frame;
    /*0x14*/ u16 frameMax;
};

/* An effect attached to a scene object, tracking its motion. */
struct SoftFxAttached
{
    SOFT_FX_HEADER
    /*0x20*/ u32 flags;
    /*0x24*/ struct GMAModel *model;
    /*0x28*/ int started;
    /*0x2C*/ int unk2C;
    /*0x30*/ int unk30;
    /*0x34*/ Vec pos;
    /*0x40*/ Vec vel;
    /*0x4C*/ Vec prevPos;
    /*0x58*/ Vec prevVel;
    /*0x64*/ Mtx mtx;
    /*0x94*/ float unk94;
    /*0x98*/ float unk98;
    /*0x9C*/ int effectId;
};

struct ApeShadowPart
{
    int bone;
    float radius;
    GXColor color;
};

struct CharaShadowParts
{
    int charaId;
    struct ApeShadowPart *parts;
};

struct ShadowTexInfo
{
    /*0x00*/ u8 unk0;
    /*0x04*/ GXTexObj texObj;
};  /* size = 0x24 */

/* Request handed to the shadow renderer (same layout as shadows.c's). */
struct ShadowCastReq
{
    /*0x00*/ void *obj;
    /*0x04*/ Vec lightPos;
    /*0x10*/ float unk10;
    /*0x14*/ Vec pos;
    /*0x20*/ float radius;
    /*0x24*/ Vec up;
    /*0x30*/ s16 unk30;
    /*0x32*/ u16 cameraMask;
    /*0x34*/ float unk34;
    /*0x38*/ float unk38;
    /*0x3C*/ GXColor color;
    /*0x40*/ struct ShadowTexInfo tex;
};  /* size = 0x64 */

extern struct GMA *g_bg_gma;
extern struct Light s_lightPool[];
extern Vec lbl_1_data_2E418[];
extern struct BgLightInfo lbl_1_bss_2F48;
extern struct GMA *efcmdlGma;
extern float lbl_1_bss_6FB7C;
extern float lbl_1_bss_6FB80;
extern float lbl_1_bss_6FB84;

void *get_GmaBuffer_entry(struct GMA *gma, char *name);
void *GXGetTexObjData(GXTexObj *obj);
int GXGetTexObjFmt(GXTexObj *obj);
u16 GXGetTexObjHeight(GXTexObj *obj);
u8 GXGetTexObjMipMap(GXTexObj *obj);
u16 GXGetTexObjWidth(GXTexObj *obj);
void GXInitTexObj(GXTexObj *obj, void *image, u16 width, u16 height, int format, int wrapS, int wrapT, u8 mipmap);
void GXInitTexObjLOD(GXTexObj *obj, int minFilt, int magFilt, float minLod, float maxLod, float lodBias, int a, int b, int c);
void fn_1_D4650(struct ShadowCastReq *req);

int fn_1_144204(struct SoftFx *fx);
int fn_1_144330(struct SoftFx *fx);
int fn_1_144690(struct SoftFx *fx);
int fn_1_144964(struct SoftFx *fx);
int fn_1_144FFC(struct SoftFxTimed *fx);
int fn_1_145834(struct SoftFxDebris *fx);
int fn_1_1459E8(struct SoftFxDebris *fx);
int fn_1_145E54(struct SoftFx *fx);
void fn_1_146218(struct SoftFxAttached *fx);
void fn_1_146364(struct SoftFxAttached *fx);
void fn_1_1463A8(struct SoftFxAttached *fx);
void fn_1_146678(struct SoftFxAttached *fx);
void fn_1_146784(struct SoftFxAttached *fx);
void fn_1_147290(struct SoftFxAttached *fx);
void fn_1_14695C(struct SoftFxAttached *fx);
void fn_1_146A60(struct SoftFxAttached *fx);
void fn_1_146C5C(struct SoftFxAttached *fx);
void fn_1_146F78(struct SoftFxAttached *fx);
void fn_1_147B5C(struct SoftFxAttached *fx);
void fn_1_148A24(struct SoftFxAttached *fx);
void fn_1_148CA0(struct SoftFxAttached *fx);
void fn_1_1490B0(struct SoftFxAttached *fx);
void fn_1_149930(struct SoftFxAttached *fx);
void fn_1_149B98(struct SoftFxAttached *fx);
void fn_1_149CE4(struct SoftFxAttached *fx);
void fn_1_149FF0(struct SoftFxAttached *fx);
void fn_1_14AD18(struct SoftFxAttached *fx);
void fn_1_14B19C(struct SoftFxAttached *fx);
void fn_1_14B608(struct SoftFxAttached *fx);
void fn_1_14BAAC(struct SoftFxAttached *fx);
void fn_1_14BBD4(struct SoftFxAttached *fx);
void fn_1_14BCFC(struct SoftFxAttached *fx);

/* The list head; only the header of the effect is used. */
/* One entry of a chain's part layout. */
struct ChainTbl
{
    /*0x00*/ s16 kind;
    /*0x02*/ s16 bone;
    /*0x04*/ Vec offset;
};  /* size = 0x10 */

struct ChainPart
{
    /*0x00*/ s16 kind;
    /*0x02*/ s16 bone;
    /*0x04*/ Vec offset;
    /*0x10*/ Vec pos;
    /*0x1C*/ Vec prevPos;
    /*0x28*/ Vec vel;
    /*0x34*/ Vec prevVel;
    /*0x40*/ float unk40;
    /*0x44*/ float unk44;
};  /* size = 0x48 */

struct SoftFxChain
{
    SOFT_FX_HEADER
    /*0x20*/ u32 timer;
    /*0x24*/ Vec v24;
    /*0x30*/ Vec v30;
    /*0x3C*/ int count;
    /*0x40*/ struct ChainPart parts[1];
};

struct BoneTbl
{
    /*0x00*/ int bone;
    /*0x04*/ Vec offset;
};  /* size = 0x10 */

struct BonePart
{
    /*0x00*/ s16 bone;
    /*0x04*/ Vec offset;
    /*0x10*/ Vec pos;
    /*0x1C*/ Vec prevPos;
    /*0x28*/ Vec vel;
    /*0x34*/ Vec prevVel;
    /*0x40*/ float unk40;
    /*0x44*/ float unk44;
};  /* size = 0x48 */

struct SoftFxBones
{
    SOFT_FX_HEADER
    /*0x20*/ u32 timer;
    /*0x24*/ int count;
    /*0x28*/ struct BonePart parts[1];
};

/* Animation frame ranges during which a bone trails particles. */
struct BoneAnimRange
{
    /*0x00*/ s16 bone;
    /*0x02*/ s16 unk2;
    /*0x04*/ s16 unk4;
    /*0x06*/ s16 unk6;
    /*0x08*/ float start;
    /*0x0C*/ float end;
};  /* size = 0x10 */

struct SoftFxScreen
{
    SOFT_FX_HEADER
    /*0x20*/ float unk20;
    /*0x24*/ float unk24;
    /*0x28*/ float unk28;
    /*0x2C*/ u8 filler2C[0x34 - 0x2C];
    /*0x34*/ float unk34;
    /*0x38*/ float unk38;
    /*0x3C*/ float unk3C;
    /*0x40*/ float unk40;
    /*0x44*/ int captured;
    /*0x48*/ GXTexObj tex;
    /*0x68*/ void *texBuf;
};

struct SoftFxGun
{
    SOFT_FX_HEADER
    /*0x20*/ Vec pos;
    /*0x2C*/ Vec vel;
    /*0x38*/ Vec muzzle;
    /*0x44*/ Vec muzzlePos;
    /*0x50*/ Vec muzzleVel;
    /*0x5C*/ Vec dir;
    /*0x68*/ Vec scale;
    /*0x74*/ float gunScale;
    /*0x78*/ S16Vec rot;
    /*0x80*/ struct GMAModel *model;
};

struct SoftFxGunBeam
{
    SOFT_FX_HEADER
    /*0x20*/ struct GMAModel *beam;
    /*0x24*/ struct GMAModel *light;
    /*0x28*/ float unk28;
    /*0x2C*/ u32 timer;
};

struct ZQuery
{
    float unused;
    u32 z;
};

struct ZQueryList
{
    struct ZQueryList *next;
    int count;
    struct ZQuery *queries;
    struct ZQuery items[20];
};

void fn_1_117E94(int count, Vec *points, struct ZQuery *queries);
u32 fn_1_118164(float z);
void fn_1_1181E8(struct ZQueryList *list);
void fn_1_118210(struct ZQueryList *list);
void fn_1_D70EC(Vec *pos);
float fn_1_15FA2C(void);

struct SoftFxLensFlare
{
    SOFT_FX_HEADER
    /*0x020*/ struct GMAModel *model;
    /*0x024*/ Vec offsets[20];
    /*0x114*/ float vis[20];
    /*0x164*/ Vec points[20];
    /*0x254*/ struct ZQueryList lists[4];
};

struct SoftFxWarp
{
    SOFT_FX_HEADER
    /*0x20*/ float unk20;
    /*0x24*/ int captured;
    /*0x28*/ GXTexObj texObj;
    /*0x48*/ void *texBuf;
    /*0x4C*/ float indMtx[2][3];
    /*0x64*/ Vec texScale;
    /*0x70*/ Vec texTranslate;
    /*0x7C*/ GXTexObj *tex;
};

struct LaserPartDef
{
    int bone;
    Vec offset;
    float size;
};

struct LaserPart
{
    /*0x00*/ int bone;
    /*0x04*/ Vec offset;
    /*0x10*/ float size;
    /*0x14*/ Vec pos;
    /*0x20*/ Vec prevPos;
    /*0x2C*/ Vec vel;
    /*0x38*/ Vec prevVel;
    /*0x44*/ float unk44;
    /*0x48*/ u8 filler48[0x4C - 0x48];
};

struct SoftFxLaser
{
    SOFT_FX_HEADER
    /*0x20*/ int unk20;
    /*0x24*/ Vec unk24;
    /*0x30*/ Vec unk30;
    /*0x3C*/ int count;
    /*0x40*/ struct LaserPart parts[8];
};

struct SoftFxScreen2
{
    SOFT_FX_HEADER
    /*0x20*/ float unk20;
    /*0x24*/ float unk24;
    /*0x28*/ float unk28;
    /*0x2C*/ float unk2C;
    /*0x30*/ float unk30;
    /*0x34*/ int captured;
    /*0x38*/ GXTexObj tex;
    /*0x58*/ void *texBuf;
};

struct SoftFxNode
{
    SOFT_FX_HEADER
};

typedef int (*SoftFxHandler)(struct SoftFx *fx, int msg);
typedef void (*SoftFxDrawFunc)(struct SoftFx *fx, int mask);

int fn_1_144190(struct SoftFx *fx, int msg);
int fn_1_14419C(struct SoftFx *fx, int msg);
int fn_1_144634(struct SoftFx *fx, int msg);
int fn_1_144914(struct SoftFx *fx, int msg);
int fn_1_144F94(struct SoftFxTimed *fx, int msg);
int fn_1_1457CC(struct SoftFxDebris *fx, int msg);
int fn_1_145DF8(struct SoftFx *fx, int msg);
int fn_1_1461B0(struct SoftFxAttached *fx, int msg);
int fn_1_14C578(struct SoftFx *fx, int msg);
int fn_1_14D284(struct SoftFx *fx, int msg);
int fn_1_14DAE0(struct SoftFx *fx, int msg);
int fn_1_14E49C(struct SoftFx *fx, int msg);
int fn_1_14ED48(struct SoftFx *fx, int msg);
int fn_1_14F338(struct SoftFx *fx, int msg);
int fn_1_14F9B8(struct SoftFx *fx, int msg);
int fn_1_1504B0(struct SoftFx *fx, int msg);
int fn_1_15103C(struct SoftFx *fx, int msg);
struct CoreBrokenPiece
{
    /*0x00*/ int active;
    /*0x04*/ s16 state;
    /*0x06*/ s16 timer;
    /*0x08*/ Vec pos;
    /*0x14*/ Vec vel;
    /*0x20*/ Vec dir;
    /*0x2C*/ S16Vec rot;
    /*0x34*/ Vec rotVel;
    /*0x40*/ struct GMAModel *model;
};

struct CoreSpark
{
    /*0x00*/ Vec pos;
    /*0x0C*/ s16 rotX;
    /*0x0E*/ s16 rotY;
    /*0x10*/ s16 pad10;
    /*0x12*/ s16 life;
    /*0x14*/ float scale;
    /*0x18*/ float fade;
    /*0x1C*/ float x1C;
};

struct CoreRocket
{
    /*0x00*/ int active;
    /*0x04*/ Vec pos;
    /*0x10*/ Vec vel;
    /*0x1C*/ s16 rotX;
    /*0x1E*/ s16 rotY;
    /*0x20*/ u8 pad20[4];
    /*0x24*/ Vec offset;
    /*0x30*/ Vec angle;
    /*0x3C*/ Vec angleVel;
    /*0x48*/ int timer;
    /*0x4C*/ float scale;
    /*0x50*/ float trail;
};

struct SoftFxCoreBroken
{
    SOFT_FX_HEADER
    /*0x0020*/ s16 phase;
    /*0x0022*/ s16 timer;
    /*0x0024*/ int counter;
    /*0x0028*/ int count;
    /*0x002C*/ struct CoreBrokenPiece pieces[48];
    /*0x0CEC*/ int sparkIdx;
    /*0x0CF0*/ struct CoreSpark sparks[512];
    /*0x4CF0*/ int rocketIdx;
    /*0x4CF4*/ struct CoreRocket rockets[64];
};

int fn_1_1518A4(struct SoftFx *fx, int msg);
void fn_1_151918(struct SoftFx *fx);
void fn_1_1519E4(struct SoftFx *fx);
void fn_1_1519E8(struct SoftFx *fx);
void fn_1_151BFC(struct SoftFxCoreBroken *self);
void fn_1_151CC4(struct SoftFxCoreBroken *self);
void fn_1_151FD4(struct SoftFxCoreBroken *self);
void fn_1_152094(struct SoftFxCoreBroken *self);
void fn_1_1520C4(struct SoftFxCoreBroken *self);
void fn_1_1520C8(struct SoftFxCoreBroken *self);
void fn_1_152664(struct SoftFxCoreBroken *self);
void fn_1_1528BC(struct SoftFxCoreBroken *self, struct CoreRocket *src);
void fn_1_152984(struct SoftFxCoreBroken *self);
void fn_1_152988(struct SoftFxCoreBroken *self);
void fn_1_152A70(struct SoftFxCoreBroken *self);
void fn_1_152B8C(struct SoftFxCoreBroken *self, struct CoreSpark *src);
void fn_1_144198(struct SoftFx *fx, int mask);
void fn_1_144630(struct SoftFx *fx, int mask);
void fn_1_144910(struct SoftFx *fx, int mask);
void fn_1_144F90(struct SoftFx *fx, int mask);
void fn_1_1457C8(struct SoftFx *fx, int mask);
void fn_1_145DF4(struct SoftFx *fx, int mask);
void fn_1_1461AC(struct SoftFx *fx, int mask);
void fn_1_14C574(struct SoftFx *fx, int mask);
void fn_1_14D280(struct SoftFx *fx, int mask);
void fn_1_14DADC(struct SoftFx *fx, int mask);
void fn_1_14DDB0(struct SoftFx *fx, int mask);
void fn_1_14E6C4(struct SoftFx *fx, int mask);
void fn_1_14F244(struct SoftFx *fx, int mask);
void fn_1_14F4C8(struct SoftFx *fx, int mask);
void fn_1_1504AC(struct SoftFx *fx, int mask);
void fn_1_1509AC(struct SoftFx *fx, int mask);
void fn_1_151308(struct SoftFx *fx, int mask);
void fn_1_151BBC(struct SoftFx *fx, int mask);

void fn_1_143F34(u32 mask);
void fn_1_143FB8(void);
void fn_1_144150(struct SoftFx *fx);


u32 lbl_1_data_6F780[] = {
    0xFE000102, 0x03040506, 0x0708090A, 0x0B0CFE0D,
    0x0E0FFF00, 0x00010203, 0x04050607, 0x08090A0B,
    0x0C0D0E0F, 0xFF000000, 0x31000000, 0x00000000,
    0x3800B400, 0x00010000, 0x01000000, 0x00020000,
    0x10004C00, 0x00030000, 0x01000000, 0x00040000,
    0x18000000, 0x00090000, 0x40000000, 0x00000000,
    0x01000000, 0x000B0000, 0x18000000, 0x000C0000,
    0x18000000, 0x00050000, 0x40000000, 0x00000000,
    0x01000000, 0x00070000, 0x18000000, 0x00080000,
    0x3800B400, 0x000D0000, 0x40000000, 0x00000000,
    0x18000000, 0x00140000, 0x40000000, 0x00000000,
    0x01000000, 0x00160000, 0x18000000, 0x00170000,
    0x18000000, 0x00180000, 0x18000000, 0x000F0000,
    0x40000000, 0x00000000, 0x01000000, 0x00110000,
    0x18000000, 0x00120000, 0x18000000, 0x00130000,
    0x18000000, 0x00000000, 0x18000000, 0x00010000,
    0x18000000, 0x00020000, 0x18000000, 0x00030000,
    0x18000000, 0x00040000, 0x18000000, 0x00050000,
    0x18000000, 0x00060000, 0x18000000, 0x00070000,
    0x18000000, 0x00080000, 0x18000000, 0x00090000,
    0x18000000, 0x000A0000, 0x18000000, 0x000B0000,
    0x18000000, 0x000C0000, 0x18000000, 0x000D0000,
    0x18000000, 0x000E0000, 0x18000000, 0x000F0000,
    0x010A0204, 0x0806FF00,
};

char *lbl_1_data_6FD90[] = {
    "SCNPLAY_FX_CHARASHADOW",
    "SCNPLAY_FX_CHARASHADOW_EZ",
    "SCNPLAY_FX_BALL_FLYING",
    "SCNPLAY_FX_BALL_FLYAWAY",
    "SCNPLAY_FX_S1A_AIRSHIP_SHADOW",
    "SCNPLAY_FX_S1A_VACUUM_WIND",
    "SCNPLAY_FX_S1A_ISLAND_WIND",
    "SCNPLAY_FX_DMY007",
    "SCNPLAY_FX_DMY008",
    "SCNPLAY_FX_BOMBODY",
    "SCNPLAY_FX_CHARAATTACK",
    "SCNPLAY_FX_BOMFLY",
    "SCNPLAY_FX_BOMFIRE",
    "SCNPLAY_FX_BIGEXP",
    "SCNPLAY_FX_PLANEFALL",
    "SCNPLAY_FX_SPLASH",
    "SCNPLAY_FX_FACEBUBBLE",
    "SCNPLAY_FX_DMY017",
    "SCNPLAY_FX_FOOTBUBBLE",
    "SCNPLAY_FX_SCREWBUBBLE",
    "SCNPLAY_FX_WHA_SPLASH",
    "SCNPLAY_FX_DMY021",
    "SCNPLAY_FX_WARN_LIGHT",
    "SCNPLAY_FX_LGUN",
    "SCNPLAY_FX_LGUN_BULLET",
    "SCNPLAY_FX_SWEAT",
    "SCNPLAY_FX_DMY027",
    "SCNPLAY_FX_BATHUP_STEAM",
    "SCNPLAY_FX_BATHUP_SHINE",
    "SCNPLAY_FX_GEARBIGEXP",
    "SCNPLAY_FX_CHARAEXP",
    "SCNPLAY_FX_ROCKETBUNER",
    "SCNPLAY_FX_ECHARGE",
    "SCNPLAY_FX_COREBREAK",
    "SCNPLAY_FX_ESPHERE",
    "SCNPLAY_FX_DMY035",
    "SCNPLAY_FX_DMY036",
    "SCNPLAY_FX_GOODBY_STAR",
    "SCNPLAY_FX_MADCRASH",
    "SCNPLAY_FX_PHOTOGRAPH",
    "SCNPLAY_FX_BANANA_FALL",
    "SCNPLAY_FX_MONOTONE",
    "SCNPLAY_FX_WATER_FILTER",
    "SCNPLAY_FX_DMY043",
    "SCNPLAY_FX_VOLCANO_SMK",
    "SCNPLAY_FX_BBEAM_EFC",
    "SCNPLAY_FX_SBEAM_EFC",
    "SCNPLAY_FX_LENSFLARE",
    "SCNPLAY_FX_CHARASHADOW_EZ_BALL",
    "SCNPLAY_FX_WATER_FILTER_MILD",
    "SCNPLAY_FX_SPLASH_MILD",
};

/* Allocation size per effect type.  On GC these are the literal sizes; on PC
 * the same sizes are taken from the struct each handler uses, so they also
 * hold where the pointer-bearing header grows (64-bit hosts).  Checked against
 * the literals on 32-bit hosts below. */
#ifdef __MWERKS__
#define FX_SIZE(lit, size) (lit)
#else
#include <stddef.h>
#define FX_SIZE(lit, size) (size)
#endif
#define FX_NODE         FX_SIZE(0x20, sizeof(struct SoftFxNode))
#define FX_TEX          FX_SIZE(0x40, sizeof(struct SoftFx))
#define FX_TIMED        FX_SIZE(0x24, sizeof(struct SoftFxTimed))
#define FX_DEBRIS       FX_SIZE(0x4C, sizeof(struct SoftFxDebris))
#define FX_ATTACHED     FX_SIZE(0xA0, sizeof(struct SoftFxAttached))
#define FX_LASER        FX_SIZE(0x760, offsetof(struct SoftFxLaser, parts) + 24 * sizeof(struct LaserPart))
#define FX_CHAIN        FX_SIZE(0xD0, offsetof(struct SoftFxChain, parts) + 2 * sizeof(struct ChainPart))
#define FX_BONES        FX_SIZE(0xB8, offsetof(struct SoftFxBones, parts) + 2 * sizeof(struct BonePart))
#define FX_GUN          FX_SIZE(0x84, sizeof(struct SoftFxGun))
#define FX_GUN_BEAM     FX_SIZE(0x30, sizeof(struct SoftFxGunBeam))
#define FX_SCREEN       FX_SIZE(0x6C, sizeof(struct SoftFxScreen))
#define FX_SCREEN2      FX_SIZE(0x5C, sizeof(struct SoftFxScreen2))
#define FX_WARP         FX_SIZE(0x80, sizeof(struct SoftFxWarp))
#define FX_LENS_FLARE   FX_SIZE(0x504, sizeof(struct SoftFxLensFlare))
#define FX_CORE_BROKEN  FX_SIZE(0x61F4, sizeof(struct SoftFxCoreBroken))

u32 s_softFxSizes[] = {
    FX_NODE, FX_NODE, FX_DEBRIS, FX_DEBRIS, FX_TEX, FX_TIMED,
    FX_NODE, FX_NODE, FX_NODE, FX_ATTACHED, FX_ATTACHED, FX_ATTACHED,
    FX_ATTACHED, FX_ATTACHED, FX_ATTACHED, FX_LASER, FX_CHAIN, FX_NODE,
    FX_CHAIN, FX_CHAIN, FX_ATTACHED, FX_NODE, FX_ATTACHED, FX_GUN,
    FX_GUN_BEAM, FX_BONES, FX_NODE, FX_ATTACHED, FX_ATTACHED, FX_ATTACHED,
    FX_ATTACHED, FX_ATTACHED, FX_ATTACHED, FX_CORE_BROKEN, FX_ATTACHED, FX_NODE,
    FX_NODE, FX_ATTACHED, FX_ATTACHED, FX_SCREEN, FX_ATTACHED, FX_SCREEN2,
    FX_WARP, FX_NODE, FX_ATTACHED, FX_ATTACHED, FX_ATTACHED, FX_LENS_FLARE,
    FX_NODE, FX_WARP, FX_LASER,
};

#if !defined(__MWERKS__) && __SIZEOF_POINTER__ == 4
/* pc_port: the sizeof-based table must equal the GC literals on 32-bit hosts. */
#define FX_CHECK(lit, size) typedef char fx_size_check_##lit[(size) == (lit) ? 1 : -1];
FX_CHECK(0x20, sizeof(struct SoftFxNode))
FX_CHECK(0x40, sizeof(struct SoftFx))
FX_CHECK(0x24, sizeof(struct SoftFxTimed))
FX_CHECK(0x4C, sizeof(struct SoftFxDebris))
FX_CHECK(0xA0, sizeof(struct SoftFxAttached))
FX_CHECK(0x760, offsetof(struct SoftFxLaser, parts) + 24 * sizeof(struct LaserPart))
FX_CHECK(0xD0, offsetof(struct SoftFxChain, parts) + 2 * sizeof(struct ChainPart))
FX_CHECK(0xB8, offsetof(struct SoftFxBones, parts) + 2 * sizeof(struct BonePart))
FX_CHECK(0x84, sizeof(struct SoftFxGun))
FX_CHECK(0x30, sizeof(struct SoftFxGunBeam))
FX_CHECK(0x6C, sizeof(struct SoftFxScreen))
FX_CHECK(0x5C, sizeof(struct SoftFxScreen2))
FX_CHECK(0x80, sizeof(struct SoftFxWarp))
FX_CHECK(0x504, sizeof(struct SoftFxLensFlare))
FX_CHECK(0x61F4, sizeof(struct SoftFxCoreBroken))
#undef FX_CHECK
#endif

SoftFxHandler s_softFxHandlers[] = {
    fn_1_145DF8, fn_1_144634, (SoftFxHandler)fn_1_1457CC, (SoftFxHandler)fn_1_1457CC,
    fn_1_14419C, (SoftFxHandler)fn_1_144F94, fn_1_144914, fn_1_144190,
    fn_1_144190, (SoftFxHandler)fn_1_1461B0, (SoftFxHandler)fn_1_1461B0, (SoftFxHandler)fn_1_1461B0,
    (SoftFxHandler)fn_1_1461B0, (SoftFxHandler)fn_1_1461B0, (SoftFxHandler)fn_1_1461B0, fn_1_14F9B8,
    fn_1_14C578, fn_1_144190, fn_1_14C578, fn_1_14C578,
    (SoftFxHandler)fn_1_1461B0, fn_1_144190, (SoftFxHandler)fn_1_1461B0, fn_1_14ED48,
    fn_1_14F338, fn_1_14D284, fn_1_144190, (SoftFxHandler)fn_1_1461B0,
    (SoftFxHandler)fn_1_1461B0, (SoftFxHandler)fn_1_1461B0, (SoftFxHandler)fn_1_1461B0, (SoftFxHandler)fn_1_1461B0,
    (SoftFxHandler)fn_1_1461B0, fn_1_1518A4, (SoftFxHandler)fn_1_1461B0, fn_1_144190,
    fn_1_144190, (SoftFxHandler)fn_1_1461B0, (SoftFxHandler)fn_1_1461B0, fn_1_14DAE0,
    (SoftFxHandler)fn_1_1461B0, fn_1_14E49C, fn_1_1504B0, fn_1_144190,
    (SoftFxHandler)fn_1_1461B0, (SoftFxHandler)fn_1_1461B0, (SoftFxHandler)fn_1_1461B0, fn_1_15103C,
    fn_1_144634, fn_1_1504B0, fn_1_14F9B8,
    NULL,
};

SoftFxDrawFunc s_softFxDrawers[] = {
    fn_1_1461AC, fn_1_144910, fn_1_145DF4, fn_1_145DF4,
    fn_1_144630, fn_1_1457C8, fn_1_144F90, fn_1_144198,
    fn_1_144198, fn_1_14C574, fn_1_14C574, fn_1_14C574,
    fn_1_14C574, fn_1_14C574, fn_1_14C574, fn_1_1504AC,
    fn_1_14D280, fn_1_144198, fn_1_14D280, fn_1_14D280,
    fn_1_14C574, fn_1_144198, fn_1_14C574, fn_1_14F244,
    fn_1_14F4C8, fn_1_14DADC, fn_1_144198, fn_1_14C574,
    fn_1_14C574, fn_1_14C574, fn_1_14C574, fn_1_14C574,
    fn_1_14C574, fn_1_151BBC, fn_1_14C574, fn_1_144198,
    fn_1_144198, fn_1_14C574, fn_1_14C574, fn_1_14DDB0,
    fn_1_14C574, fn_1_14E6C4, fn_1_1509AC, fn_1_144198,
    fn_1_14C574, fn_1_14C574, fn_1_14C574, fn_1_151308,
    fn_1_144910, fn_1_1509AC, fn_1_1504AC,
    NULL,
};

struct SoftFxNode lbl_1_bss_DD640;

int fn_1_143CC0(void)
{
    memset(&lbl_1_bss_DD640, 0, sizeof(lbl_1_bss_DD640));
    return 1;
}

int g_something_to_do_with_soft_fx(void *owner, int type, void *param)
{
    u32 size;
    int heap;
    struct SoftFx *fx;

    if (type < 0 || type >= 51)
    {
        printf("[ Warning ] Illegal SOFTFX Requested [%d]!!\n", type);
        return 0;
    }
    heap = currentHeap;
    size = s_softFxSizes[type];
    fx = OSAllocFromHeap(heap, size);
    if (fx == NULL)
    {
        printf("[ Error ] Not Enough Memory For SOFTFX[%d]!!\n", type);
        return 0;
    }
    memset(fx, 0, size);
    fx->state = 1;
    fx->heap = heap;
    fx->owner = owner;
    fx->type = type;
    fx->desc = (struct SceneObjDesc *)param;
    s_softFxHandlers[type](fx, 0);
    if (fx->state == 3)
    {
        OSFreeToHeap(fx->heap, fx);
        return 0;
    }
    s_softFxHandlers[type](fx, 1);
    fx->next = lbl_1_bss_DD640.next;
    fx->prev = (struct SoftFx *)&lbl_1_bss_DD640;
    if (fx->next != NULL)
        fx->next->prev = fx;
    lbl_1_bss_DD640.next = fx;
    return 1;
}

void fn_1_143E68(void)
{
    struct SoftFx *fx;
    struct SoftFx *next;
    SoftFxHandler handler;

    if (g_some_other_flags & 0xA)
        return;
    fx = lbl_1_bss_DD640.next;
    while (fx != NULL)
    {
        next = fx->next;
        handler = s_softFxHandlers[fx->type];
        handler(fx, 2);
        if (fx->state == 3)
        {
            fx->state = 0;
            handler(fx, 2);
            fn_1_144150(fx);
        }
        fx = next;
    }
}

void fn_1_143F34(u32 mask)
{
    struct SoftFx *fx;
    struct SoftFx *next;

    fx = lbl_1_bss_DD640.next;
    while (fx != NULL)
    {
        next = fx->next;
        if (fx->drawMask & mask)
            s_softFxDrawers[fx->type](fx, mask);
        fx = next;
    }
}

void fn_1_143FB8(void)
{
    struct SoftFx *fx;
    struct SoftFx *next;

    fx = lbl_1_bss_DD640.next;
    while (fx != NULL)
    {
        next = fx->next;
        s_softFxHandlers[fx->type](fx, 5);
        fx = next;
    }
}

int fn_1_144020(void)
{
    struct SoftFx *fx;
    struct SoftFx *next;

    fx = lbl_1_bss_DD640.next;
    while (fx != NULL)
    {
        next = fx->next;
        s_softFxHandlers[fx->type](fx, 4);
        fn_1_144150(fx);
        fx = next;
    }
    lbl_1_bss_DD640.next = NULL;
    return 1;
}

int fn_1_1440B0(void *owner)
{
    struct SoftFx *fx = lbl_1_bss_DD640.next;

    while (fx != NULL && fx->owner != owner)
        fx = fx->next;
    if (fx == NULL)
        return 0;
    s_softFxHandlers[fx->type](fx, 3);
    if (fx->state == 3)
        fn_1_144150(fx);
    return 1;
}

void fn_1_144150(struct SoftFx *fx)
{
    struct SoftFx *prev = fx->prev;
    struct SoftFx *next = fx->next;

    prev->next = next;
    if (next != NULL)
        next->prev = prev;
    OSFreeToHeap(fx->heap, fx);
}

int fn_1_144190(struct SoftFx *fx, int msg)
{
    return 1;
}

void fn_1_144198(struct SoftFx *fx, int mask) {}

int fn_1_14419C(struct SoftFx *fx, int msg)
{
    int ret = 1;

    switch (msg)
    {
    case 0:
        ret = fn_1_144204(fx);
        break;
    case 3:
        fx->state = 3;
        break;
    case 5:
        ret = fn_1_144330(fx);
        break;
    }
    return ret;
}

int fn_1_144204(struct SoftFx *fx)
{
    struct SoftFx *self;
    struct GMAModel *model;
    GXTexObj *tex;

    if (fx->desc == NULL)
    {
        fx->state = 3;
        return 0;
    }
    self = fx;
    model = get_GmaBuffer_entry(stage_gma, "NODISP_SHIPSHADOW");
    if (model != NULL)
        tex = model->texObjs;
    else
        tex = init_common_gma->modelEntries[94].model->texObjs;
    GXInitTexObj(&self->tex, GXGetTexObjData(tex), GXGetTexObjWidth(tex), GXGetTexObjHeight(tex),
        GXGetTexObjFmt(tex), 0, 0, GXGetTexObjMipMap(tex));
    GXInitTexObjLOD(&self->tex, 1, 1, 0.0f, 0.0f, 0.0f, 0, 0, 0);
    return 1;
}

int fn_1_144330(struct SoftFx *fx)
{
    struct SoftFx *self = fx;
    struct ShadowCastReq req;
    Vec lightDir;
    Vec pos;
    s16 rotY;
    struct SceneObjDesc *desc = fx->desc;
    struct Light *best;
    s16 rotX;
    struct Light *light;
    int i;
    struct GMAModel *model;
    float f;
    float bestVal;
    static struct ShadowTexInfo lbl_1_data_7013C = { 1 };

    model = get_GmaBuffer_entry(stage_gma, desc->name);
    if (model == NULL)
        model = get_GmaBuffer_entry(g_bg_gma, desc->name);
    if (model == NULL)
        model = get_GmaBuffer_entry(init_common_gma, desc->name);
    if (model == NULL)
        model = init_common_gma->modelEntries[90].model;
    i = 0;
    light = alloc_pool_light(1, (s16)i);
    if (light == NULL)
    {
        rotX = s_lightPool->rotX;
        rotY = s_lightPool->rotY;
    }
    else
    {
        f = light->blue + (light->red + light->green);
        bestVal = f;
        best = light;
        i++;
        while ((light = alloc_pool_light(1, (s16)i)) != NULL)
        {
            f = light->blue + (light->red + light->green);
            if (bestVal < f)
            {
                bestVal = f;
                best = light;
            }
            i++;
        }
        rotX = best->rotX;
        rotY = best->rotY;
    }
    mtxa_from_rotate_y(rotY);
    mtxa_rotate_x(rotX);
    lightDir.x = 0.0f;
    lightDir.y = 0.0f;
    lightDir.z = -15.0f;
    mtxa_tf_vec(&lightDir, &lightDir);
    req.unk30 = 4;
    req.unk34 = 0.0f;
    req.unk38 = 0.05f;
    req.up.x = 0.0f;
    req.up.y = -1.0f;
    req.up.z = 0.0f;
    mtxa_tf_vec(&req.up, &req.up);
    memcpy_handler(&lbl_1_data_7013C.texObj, &self->tex, sizeof(GXTexObj));
    req.tex = lbl_1_data_7013C;
    req.obj = fx;
    req.color.r = 0x6A;
    req.color.g = 0x68;
    req.color.b = 0x60;
    mtxa_from_mtx(self->desc->mtx);
    mtxa_translate(&model->boundSphereCenter);
    mathutil_mtxA_get_translate(&pos);
    req.lightPos.x = lightDir.x + pos.x;
    req.lightPos.y = lightDir.y + pos.y;
    req.lightPos.z = lightDir.z + pos.z;
    req.unk10 = 200.0f;
    req.pos = pos;
    req.radius = 1.2f * model->boundSphereRadius;
    req.cameraMask = 0xFFFF;
    fn_1_D4650(&req);
    return 1;
}

void fn_1_144630(struct SoftFx *fx, int mask)
{
}

int fn_1_144634(struct SoftFx *fx, int msg)
{
    int ret = 1;

    switch (msg)
    {
    case 3:
        fx->state = 3;
        break;
    case 5:
        ret = fn_1_144690(fx);
        break;
    }
    return ret;
}

int fn_1_144690(struct SoftFx *fx)
{
    struct ShadowReq req;
    Mtx mtx;
    struct RaycastHit hit;
    Vec normal;
    Vec pos;
    struct Ape *ape = fx->desc->ape;
    float scale = ape->scale;
    float size;
    float r, g, b;
    float t;

    switch (fx->type)
    {
    case 0x30:
        size = 0.7f;
        break;
    default:
        size = 0.4f;
        break;
    }
    req.unk20 = 0.0f;
    req.unk24 = 0.025f;
    req.model = init_common_gma->modelEntries[103].model;
    mtxa_from_quat(&ape->rot);
    mtxa_to_mtx(mtx);
    mtxa_from_translate(&ape->pos);
    mtxa_scale_s(ape->scale);
    mtxa_translate(&ape->unk264);
    mtxa_mult_right(mtx);
    mtxa_mult_right(g_get_ape_anim_smth(ape, 1));
    mathutil_mtxA_get_translate(&pos);
    if ((int)raycast_stage_down(&pos, &hit, &normal) == 0)
        return 1;
    t = 1.0 - (1.0 / (5.0f * scale)) * (pos.y - hit.pos.y);
    if (t < 0.0f)
        return 1;
    r = 255.0f * (0.75f * (1.0f - lbl_1_bss_2F48.ambient.r));
    g = 255.0f * (0.75f * (1.0f - lbl_1_bss_2F48.ambient.g));
    b = 255.0f * (0.75f * (1.0f - lbl_1_bss_2F48.ambient.b));
    req.color.r = r * t;
    req.color.g = g * t;
    req.color.b = b * t;
    req.scale.x = size * (scale * (1.0f + 2.0f * (1.0f - t)));
    req.scale.y = req.scale.x;
    req.scale.z = req.scale.x;
    vec_to_euler(&hit.normal, &req.rot);
    req.rot.z = 0;
    req.pos = hit.pos;
    submit_shadow_req(&req);
    return 1;
}

void fn_1_144910(struct SoftFx *fx, int mask)
{
}

int fn_1_144914(struct SoftFx *fx, int msg)
{
    int ret = 1;

    switch (msg)
    {
    case 3:
        fx->state = 3;
        break;
    case 2:
        ret = fn_1_144964(fx);
        break;
    }
    return ret;
}

int fn_1_144964(struct SoftFx *fx)
{
    struct Effect e;
    struct RaycastHit hit;
    Vec v;
    int i;
    struct Ape *ape;

    memset(&e, 0, sizeof(e));
    e.type = 0x27;
    e.model = init_common_gma->modelEntries[65].model;
    for (i = 0; i < 0x40; i++)
    {
        ape = get_ape_by_idx(i);
        if (ape != NULL)
        {
            e.timer = 300;
            e.u_otherTimer = 60;
            mtxa_from_rotate_y(rand() & 0x7FFF);
            mtxa_rotate_x(rand() & 0x7FFF);
            mtxa_tf_vec_xyz(0.0f, 0.0f, -1.0f, &v);
            e.pos = ape->pos;
            e.pos.x += 0.25f * v.x;
            e.pos.y += 0.25f * v.y;
            e.pos.z += 0.25f * v.z;
            e.vel.x = 0.08f * (v.x * RAND_FLOAT());
            e.vel.y = 0.08f * (v.y * RAND_FLOAT());
            e.vel.z = 0.08f * (v.z * RAND_FLOAT());
            e.rotX = (s16)(rand() & 0x7FFF);
            e.rotY = (s16)(rand() & 0x7FFF);
            e.rotZ = (s16)(rand() & 0x7FFF);
            e.unk52 = 6144.0f * (RAND_FLOAT() - 0.5f);
            e.unk54 = 8192.0f * (RAND_FLOAT() - 0.5f);
            e.unk56 = 4096.0f * (RAND_FLOAT() - 0.5f);
            e.scale.x = 0.0f;
            e.scale.y = 0.25f + 0.75f * RAND_FLOAT();
            e.unk8C.x = 5.0f;
            e.unk8C.y = 25.0f;
            e.unk8C.z = 5.0f;
            e.unk98.x = 0.01f + 0.01f * RAND_FLOAT();
            spawn_effect(&e);
        }
    }
    e.type = 0x28;
    e.model = get_GmaBuffer_entry(stage_gma, "STORM_SMOKE");
    if (e.model != NULL)
    {
        for (i = 0; i < 0x40; i++)
        {
            ape = get_ape_by_idx(i);
            if (ape != NULL)
            {
                e.timer = 60.0f * (0.3f + RAND_FLOAT());
                e.u_otherTimer = 60;
                e.pos = ape->pos;
                if ((int)raycast_stage_down(&e.pos, &hit, &v) != 0)
                    e.pos = hit.pos;
                e.vel.x = 0.1f * (RAND_FLOAT() - 0.5f);
                e.vel.y = 0.01f * RAND_FLOAT();
                e.vel.z = 0.1f * (RAND_FLOAT() - 0.5f);
                e.rotZ = (s16)(rand() & 0x7FFF);
                e.unk56 = 6144.0f * (RAND_FLOAT() - 0.5f);
                e.scale.x = 0.0f;
                e.scale.y = 1.0f + 2.0f * RAND_FLOAT();
                e.unk8C.x = 5.0f;
                e.unk8C.y = 25.0f;
                e.unk8C.z = 5.0f;
                e.unk98.x = 0.005f + 0.005f * RAND_FLOAT();
                spawn_effect(&e);
            }
        }
    }
    return 1;
}

void fn_1_144F90(struct SoftFx *fx, int mask)
{
}

int fn_1_144F94(struct SoftFxTimed *fx, int msg)
{
    int ret = 1;

    switch (msg)
    {
    case 0:
        fx->timer = 0;
        break;
    case 3:
        fx->state = 3;
        break;
    case 2:
        ret = fn_1_144FFC(fx);
        break;
    }
    return ret;
}

int fn_1_144FFC(struct SoftFxTimed *fx)
{
    struct Effect e;
    struct RaycastHit hit;
    Vec v;
    Vec center;
    int i;
    struct Ape *ape;
    struct SceneObjDesc *desc = fx->desc;
    struct GMAModel *model;
    float power;

    model = get_GmaBuffer_entry(stage_gma, desc->name);
    if (model == NULL)
        model = get_GmaBuffer_entry(g_bg_gma, desc->name);
    if (model == NULL)
        model = get_GmaBuffer_entry(init_common_gma, desc->name);
    if (model == NULL)
        model = init_common_gma->modelEntries[90].model;
    mtxa_from_mtx(fx->desc->mtx);
    mtxa_translate(&model->boundSphereCenter);
    mathutil_mtxA_get_translate(&center);
    fx->timer++;
    power = (float)fx->timer > 75.0f ? 1.0f : 1.0f - math_sin(16384.0f + 16384.0f * (float)fx->timer / 75.0f);
    memset(&e, 0, sizeof(e));
    if ((float)fx->timer > 108.0f)
    {
        e.type = 0x27;
        e.model = init_common_gma->modelEntries[65].model;
        for (i = 0; i < 0x40; i++)
        {
            ape = get_ape_by_idx(i);
            if (ape != NULL)
            {
                e.timer = 300;
                e.u_otherTimer = 60;
                mtxa_from_rotate_y(rand() & 0x7FFF);
                mtxa_rotate_x(rand() & 0x7FFF);
                mtxa_tf_vec_xyz(0.0f, 0.0f, -1.0f, &v);
                e.pos = ape->pos;
                e.pos.x += 0.25f * v.x;
                e.pos.y += 0.25f * v.y;
                e.pos.z += 0.25f * v.z;
                e.vel.x = 0.08f * (v.x * RAND_FLOAT());
                e.vel.y = 0.08f * (v.y * RAND_FLOAT());
                e.vel.z = 0.08f * (v.z * RAND_FLOAT());
                e.rotX = (s16)(rand() & 0x7FFF);
                e.rotY = (s16)(rand() & 0x7FFF);
                e.rotZ = (s16)(rand() & 0x7FFF);
                e.unk52 = 6144.0f * (RAND_FLOAT() - 0.5f);
                e.unk54 = 8192.0f * (RAND_FLOAT() - 0.5f);
                e.unk56 = 4096.0f * (RAND_FLOAT() - 0.5f);
                e.scale.x = 0.0f;
                e.scale.y = 0.25f + 0.75f * RAND_FLOAT();
                e.unk8C = center;
                e.unk98.x = 0.01f + 0.01f * RAND_FLOAT();
                spawn_effect(&e);
            }
        }
    }
    e.type = 0x28;
    e.model = get_GmaBuffer_entry(stage_gma, "STORM_SMOKE");
    if (e.model != NULL)
    {
        for (i = 0; i < 0x40; i++)
        {
            ape = get_ape_by_idx(i);
            if (ape != NULL)
            {
                e.timer = 60.0f * (0.3f + 1.5f * (power * RAND_FLOAT()));
                e.u_otherTimer = 60;
                e.pos = ape->pos;
                if ((int)raycast_stage_down(&e.pos, &hit, &v) != 0)
                    e.pos = hit.pos;
                e.vel.x = 0.1f * (RAND_FLOAT() - 0.5f);
                e.vel.y = 0.01f * RAND_FLOAT();
                e.vel.z = 0.1f * (RAND_FLOAT() - 0.5f);
                e.rotZ = (s16)(rand() & 0x7FFF);
                e.unk56 = 4096.0f * (RAND_FLOAT() - 0.5f);
                e.scale.x = 0.0f;
                e.scale.y = power * (1.0f + 4.0f * RAND_FLOAT());
                e.unk8C = center;
                e.unk98.x = 0.005f + 0.005f * RAND_FLOAT();
                spawn_effect(&e);
            }
        }
    }
    return 1;
}

void fn_1_1457C8(struct SoftFx *fx, int mask)
{
}

int fn_1_1457CC(struct SoftFxDebris *fx, int msg)
{
    int ret = 1;

    switch (msg)
    {
    case 0:
        ret = fn_1_145834(fx);
        break;
    case 3:
        fx->state = 3;
        break;
    case 2:
        ret = fn_1_1459E8(fx);
        break;
    }
    return ret;
}

int fn_1_145834(struct SoftFxDebris *fx)
{
    struct Ape *ape = fx->desc->ape;
    struct SoftFxDebris *self = fx;

    if (fx->desc == NULL)
    {
        fx->state = 3;
        return 0;
    }
    self->rotX = (rand() & 0x7FFF) - 0x7FFF;
    self->rotY = (rand() & 0x7FFF) - 0x7FFF;
    self->rotZ = (rand() & 0x7FFF) - 0x7FFF;
    self->rotVelX = 2184.5334f * (RAND_FLOAT() - 0.5f);
    self->rotVelY = 2184.5334f * (RAND_FLOAT() - 0.5f);
    self->rotVelZ = 546.13336f * (RAND_FLOAT() - 0.5f);
    self->pos = ape->pos;
    self->vel.x = 0.0f;
    self->vel.y = 0.0f;
    self->vel.z = 0.0f;
    self->timer = 0;
    return 1;
}

int fn_1_1459E8(struct SoftFxDebris *fx)
{
    struct Effect e;
    Mtx mtx;
    Vec color;
    Vec pos;
    Vec delta;
    Vec prevPos;
    Vec prevVel;
    struct SceneObjDesc *desc = fx->desc;
    struct Ape *ape = desc->ape;
    struct SoftFxDebris *self = fx;
    float scale = ape->scale;
    int spawn = self->timer > 1;
    float d;
    float total;

    switch (desc->color)
    {
    case 2:
        color = lbl_1_data_2E418[2];
        break;
    case 3:
        color = lbl_1_data_2E418[1];
        break;
    case 1:
        color = lbl_1_data_2E418[0];
        break;
    case 4:
    default:
        color = lbl_1_data_2E418[3];
        break;
    }
    self->rotX += self->rotVelX;
    self->rotY += self->rotVelY;
    self->rotZ += self->rotVelZ;
    prevPos = self->pos;
    prevVel = self->vel;
    mtxa_from_quat(&ape->rot);
    mtxa_to_mtx(mtx);
    mtxa_from_translate(&ape->pos);
    mtxa_mult_right(mtx);
    mtxa_rotate_y(self->rotY);
    mtxa_rotate_x(self->rotX);
    mtxa_rotate_z(self->rotZ);
    mtxa_scale_s(scale);
    mtxa_tf_point_xyz(0.0f, 0.0f, 0.6f, &pos);
    delta.x = pos.x - prevPos.x;
    delta.y = pos.y - prevPos.y;
    delta.z = pos.z - prevPos.z;
    self->dist += 4.0f * mathutil_vec_len(&delta) / scale;
    if (self->dist > 1.0f)
    {
        d = self->dist;
        if (d > 4.0f)
            d = 4.0f;
        memset(&e, 0, sizeof(e));
        e.type = 0x29;
        e.playerId = 0;
        e.unk18 = color.x;
        e.unk1C = color.y;
        e.unk20 = color.z;
        e.scale.x = 1.0f + 0.125f * (scale - 1.0f);
        total = d;
        while (d > 1.0f)
        {
            float t = d / total;
            float u = 1.0f - t;

            e.pos.x = pos.x * t + prevPos.x * u;
            e.pos.y = pos.y * t + prevPos.y * u;
            e.pos.z = pos.z * t + prevPos.z * u;
            e.vel.x = 0.5f * (delta.x * t + prevVel.x * u);
            e.vel.y = 0.5f * (delta.y * t + prevVel.y * u);
            e.vel.z = 0.5f * (delta.z * t + prevVel.z * u);
            if (spawn)
                spawn_effect(&e);
            d -= 1.0f;
        }
        self->dist = d;
    }
    self->pos = pos;
    self->vel = delta;
    if (self->timer < 0x10000)
        self->timer++;
    return 1;
}

void fn_1_145DF4(struct SoftFx *fx, int mask)
{
}

int fn_1_145DF8(struct SoftFx *fx, int msg)
{
    int ret = 1;

    switch (msg)
    {
    case 3:
        fx->state = 3;
        break;
    case 4:
        break;
    case 5:
        ret = fn_1_145E54(fx);
        break;
    }
    return ret;
}

int fn_1_145E54(struct SoftFx *fx)
{
    static struct ApeShadowPart lbl_1_data_7016C[] =
    {
        { 1, 0.2f, { 0xE8, 0xF8, 0xFF, 0xC0 } },
        { 8, 0.1f, { 0x74, 0x7C, 0x80, 0xFF } },
        { 12, 0.1f, { 0x74, 0x7C, 0x80, 0xFF } },
        { 18, 0.1f, { 0x74, 0x7C, 0x80, 0xFF } },
        { 23, 0.1f, { 0x74, 0x7C, 0x80, 0xFF } },
        { -1, 0.0f, { 0, 0, 0, 0 } },
    };
    static struct ApeShadowPart lbl_1_data_701B4[] =
    {
        { 1, 0.25f, { 0xEC, 0xF8, 0xFF, 0xFF } },
        { 8, 0.2f, { 0x74, 0x7C, 0x80, 0xFF } },
        { 12, 0.2f, { 0x74, 0x7C, 0x80, 0xFF } },
        { 18, 0.2f, { 0x74, 0x7C, 0x80, 0xFF } },
        { 23, 0.2f, { 0x74, 0x7C, 0x80, 0xFF } },
        { -1, 0.0f, { 0, 0, 0, 0 } },
    };
    static struct CharaShadowParts lbl_1_data_701FC[] =
    {
        { 3, lbl_1_data_701B4 },
    };
    struct ShadowReq req;
    Mtx mtx;
    struct RaycastHit hit;
    Vec tmp;
    Vec pos;
    struct Ape *ape = fx->desc->ape;
    int i;
    struct CharaShadowParts *entry = lbl_1_data_701FC;
    struct ApeShadowPart *parts;
    float ambR, ambG, ambB;
    float scale;
    int found;

    for (i = 1; i > 0; i--, entry++)
    {
        if (ape->charaId == entry->charaId)
        {
            parts = entry->parts;
            break;
        }
    }
    if (i == 0)
        parts = lbl_1_data_7016C;
    req.unk20 = 0.0f;
    req.unk24 = 0.025f;
    req.model = init_common_gma->modelEntries[103].model;
    mtxa_from_quat(&ape->rot);
    mtxa_to_mtx(mtx);
    mtxa_from_translate(&ape->pos);
    mtxa_scale_s(ape->scale);
    mtxa_translate(&ape->unk264);
    mtxa_mult_right(mtx);
    ambR = lbl_1_bss_2F48.ambient.r;
    ambG = lbl_1_bss_2F48.ambient.g;
    ambB = lbl_1_bss_2F48.ambient.b;
    scale = ape->scale;
    for (; parts->bone >= 0; parts++)
    {
        mtxa_push();
        mtxa_mult_right(g_get_ape_anim_smth(ape, parts->bone));
        mathutil_mtxA_get_translate(&pos);
        pos.y += 0.2f;
        found = raycast_stage_down(&pos, &hit, &tmp);
        mtxa_pop();
        if (found)
        {
            float a = 1.0 - (1.0 / (2.0f * scale)) * (pos.y - hit.pos.y);
            float r, g, b;

            if (a < 0.0f)
                return 1;
            r = parts->color.r * (0.75f * (1.0f - ambR));
            g = parts->color.g * (0.75f * (1.0f - ambG));
            b = parts->color.b * (0.75f * (1.0f - ambB));
            req.color.r = r * a;
            req.color.g = g * a;
            req.color.b = b * a;
            req.scale.x = (1.0f + 2.0f * (1.0f - a)) * (scale * parts->radius);
            req.scale.y = req.scale.x;
            req.scale.z = req.scale.x;
            vec_to_euler(&hit.normal, &req.rot);
            req.rot.z = 0;
            req.pos = hit.pos;
            submit_shadow_req(&req);
        }
    }
    return 1;
}

void fn_1_1461AC(struct SoftFx *fx, int mask)
{
}

int fn_1_1461B0(struct SoftFxAttached *fx, int msg)
{
    switch (msg)
    {
    case 0:
        fn_1_146218(fx);
        break;
    case 2:
        fn_1_1463A8(fx);
        break;
    case 3:
        fx->state = 3;
        break;
    case 4:
        fn_1_146364(fx);
        break;
    }
    return 1;
}

void fn_1_146218(struct SoftFxAttached *fx)
{
    struct SceneObjDesc *desc = fx->desc;
    struct GMAModel *model = NULL;

    if (desc != NULL)
    {
        switch (desc->kind)
        {
        case 0:
            model = get_GmaBuffer_entry(stage_gma, desc->name);
            if (model == NULL)
                model = get_GmaBuffer_entry(g_bg_gma, desc->name);
            if (model == NULL)
                model = get_GmaBuffer_entry(init_common_gma, desc->name);
            break;
        }
    }
    fx->model = model;
    switch (fx->type)
    {
    case 13:
    case 14:
        fx->flags |= 1;
        break;
    }
    if (desc != NULL)
        mtxa_from_mtx(desc->mtx);
    else
        mtxa_from_identity();
    mtxa_to_mtx(fx->mtx);
    if ((fx->flags & 1) && fx->model != NULL)
        mtxa_translate(&fx->model->boundSphereCenter);
    mathutil_mtxA_get_translate(&fx->pos);
    fx->started = 0;
    fx->unk2C = 0;
    fx->effectId = -1;
}

void fn_1_146364(struct SoftFxAttached *fx)
{
    if (fx->effectId >= 0)
    {
        fn_1_6870C(fx->effectId);
        fx->effectId = -1;
    }
}

void fn_1_1463A8(struct SoftFxAttached *fx)
{
    Vec pos;
    Vec prevPos;
    Vec vel;
    Vec prevVel;
    struct SoftFxAttached *self = fx;
    struct SceneObjDesc *desc = self->desc;

    prevPos = self->pos;
    prevVel = self->vel;
    if (desc == NULL)
    {
        mtxa_from_identity();
    }
    else
    {
        switch (desc->kind)
        {
        case 1:
            mtxa_from_translate(&desc->ape->pos);
            break;
        default:
            mtxa_from_mtx(desc->mtx);
            break;
        }
    }
    if ((self->flags & 1) && self->model != NULL)
        mtxa_translate(&self->model->boundSphereCenter);
    mathutil_mtxA_get_translate(&pos);
    vel.x = pos.x - prevPos.x;
    vel.y = pos.y - prevPos.y;
    vel.z = pos.z - prevPos.z;
    self->pos = pos;
    self->vel = vel;
    self->prevPos = prevPos;
    self->prevVel = prevVel;
    if (self->started == 0)
    {
        self->started = 1;
        if (desc != NULL)
            mtx_copy(desc->mtx, self->mtx);
        return;
    }
    switch (self->type)
    {
    case 9:
        fn_1_146678(self);
        break;
    case 11:
        fn_1_146A60(self);
        break;
    case 12:
        fn_1_146C5C(self);
        break;
    case 13:
        fn_1_146F78(self);
        break;
    case 14:
        fn_1_147B5C(self);
        break;
    case 27:
        fn_1_148A24(self);
        break;
    case 28:
        fn_1_148CA0(self);
        break;
    case 29:
        fn_1_1490B0(self);
        break;
    case 30:
        fn_1_149930(self);
        break;
    case 31:
        fn_1_149B98(self);
        break;
    case 37:
        fn_1_149CE4(self);
        break;
    case 40:
        fn_1_149FF0(self);
        break;
    case 45:
    case 46:
        fn_1_14AD18(self);
        break;
    case 20:
        fn_1_14B19C(self);
        break;
    case 22:
        fn_1_14695C(self);
        break;
    case 44:
        fn_1_14BCFC(self);
        break;
    case 38:
        fn_1_14B608(self);
        break;
    case 32:
        fn_1_14BAAC(self);
        break;
    case 34:
        fn_1_14BBD4(self);
        break;
    case 10:
        fn_1_146784(self);
        break;
    }
    if (desc != NULL)
        mtx_copy(desc->mtx, self->mtx);
}

void fn_1_146678(struct SoftFxAttached *fx)
{
    struct Effect e;
    Vec pos;
    Vec prevPos;
    Vec vel;
    Vec prevVel;

    pos = fx->pos;
    vel = fx->vel;
    prevPos = fx->prevPos;
    prevVel = fx->prevVel;
    switch (fx->unk2C)
    {
    case 0:
        fx->unk2C = 1;
        memset(&e, 0, sizeof(e));
        e.type = 0x2A;
        e.model = (void *)fx->desc;
        e.unk8C.y = 5.5f;
        e.unk54 = 0x444;
        e.scale.x = 5.0f;
        e.unk18 = 1.6f;
        e.unk1C = 0.85f;
        e.unk20 = 0.8f;
        fx->effectId = spawn_effect(&e);
        break;
    }
}

void fn_1_146784(struct SoftFxAttached *fx)
{
    struct Effect e;
    Vec pos;
    Vec prevPos;
    Vec vel;
    Vec prevVel;
    S16Vec rot;
    struct SceneObjDesc *desc = fx->desc;
    int i;
    float scale;

    pos = fx->pos;
    vel = fx->vel;
    prevPos = fx->prevPos;
    prevVel = fx->prevVel;
    switch (desc->kind)
    {
    case 1:
        scale = desc->ape->scale;
        break;
    default:
        scale = desc->joints->scale.x;
        break;
    }
    switch (fx->unk2C)
    {
    case 0:
        memset(&e, 0, sizeof(e));
        e.type = 5;
        e.scale.y = scale;
        vec_to_euler(&vel, &rot);
        mtxa_from_translate(&pos);
        mtxa_rotate_y(rot.y);
        mtxa_rotate_x(rot.x);
        mtxa_rotate_z(rand() & 0x7FFF);
        for (i = 18; i > 0; i--)
        {
            mtxa_push();
            mtxa_rotate_z((i << 16) / 18);
            mtxa_tf_point_xyz(0.0f, 0.5f * scale, 0.0f, &e.pos);
            mtxa_tf_vec_xyz(0.0f, scale * (0.1f + 0.05f * RAND_FLOAT()), 0.0f, &e.vel);
            spawn_effect(&e);
            mtxa_pop();
        }
        break;
    }
}

void fn_1_14695C(struct SoftFxAttached *fx)
{
    struct Effect e;
    Vec pos;
    Vec vel;
    struct SoftFxAttached *self = fx;
    struct AnimJoint *linkedJoints = self->desc->joints;
    float scale;

    pos = self->pos;
    vel = self->vel;
    scale = linkedJoints->scale.x;
    switch (self->unk2C)
    {
    case 0:
        self->unk2C = 1;
        memset(&e, 0, sizeof(e));
        e.type = 0x40;
        e.model = (void *)self->desc;
        e.unk8C.y = 0.0f;
        e.rotY = 0x4000;
        e.unk54 = 0x400;
        e.scale.x = 3.0f * scale;
        e.unk18 = 1.6f;
        e.unk1C = 0.85f;
        e.unk20 = 0.8f;
        self->effectId = spawn_effect(&e);
        break;
    }
}

void fn_1_146A60(struct SoftFxAttached *fx)
{
    struct Effect e;
    Vec pos;
    Vec prevPos;
    Vec vel;
    Vec prevVel;

    pos = fx->pos;
    vel = fx->vel;
    prevPos = fx->prevPos;
    prevVel = fx->prevVel;
    switch (fx->unk2C)
    {
    case 0:
        fx->unk2C = 1;
        memset(&e, 0, sizeof(e));
        e.type = 0x2B;
        e.model = (void *)fx->desc;
        e.pos = pos;
        e.vel = vel;
        e.rotX = (rand() & 0x7FFF) - 0x7FFF;
        e.rotY = (rand() & 0x7FFF) - 0x7FFF;
        e.unk52 = 1092.2667f * (RAND_FLOAT() - 0.5f);
        e.unk54 = 2184.5334f * (RAND_FLOAT() - 0.5f);
        e.unk8C.x = 0.0f;
        e.unk8C.y = 5.25f;
        e.unk8C.z = 0.0f;
        e.scale.y = 5.0f;
        e.unk18 = 1.0f;
        e.unk1C = 0.8f;
        e.unk20 = 0.3f;
        fx->effectId = spawn_effect(&e);
        break;
    }
}

void fn_1_146C5C(struct SoftFxAttached *fx)
{
    struct Effect e;
    Vec pos;
    Vec prevPos;
    Vec vel;
    Vec prevVel;
    int i;
    struct AnimJoint *linkedJoints;
    float c;

    pos = fx->pos;
    vel = fx->vel;
    prevPos = fx->prevPos;
    prevVel = fx->prevVel;
    switch (fx->unk2C)
    {
    case 0:
        fx->unk2C = 1;
        linkedJoints = fx->desc->joints;
        memset(&e, 0, sizeof(e));
        e.type = 0x2D;
        e.model = efcmdlGma->modelEntries[5].model;
        e.pos = pos;
        e.scale.x = 5.0f * linkedJoints->scale.x;
        e.scale.y = e.scale.x * (0.1f + 0.2f * RAND_FLOAT());
        e.unk18 = 0.75f;
        e.unk1C = 0.7125f;
        e.unk20 = 0.525f;
        spawn_effect(&e);
        e.type = 0x2E;
        for (i = 2; i > 0; i--)
        {
            e.model = efcmdlGma->modelEntries[15].model;
            e.rotX = (rand() & 0x7FFF) - 0x7FFF;
            e.rotY = (rand() & 0x7FFF) - 0x7FFF;
            e.rotZ = (rand() & 0x7FFF) - 0x7FFF;
            e.unk56 = 1092.2667f * (RAND_FLOAT() - 0.5f);
            e.scale.x = linkedJoints->scale.x * (2.0f * (1.0f + 0.4f * RAND_FLOAT()));
            e.scale.y = e.scale.x * (0.75f + 0.75f * RAND_FLOAT());
            c = 0.25f + 0.25f * RAND_FLOAT();
            e.unk18 = c;
            e.unk1C = 0.95f * c;
            e.unk20 = 0.7f * c;
            spawn_effect(&e);
        }
        break;
    }
}

void fn_1_146F78(struct SoftFxAttached *fx)
{
    struct Effect e;
    Vec pos;
    Vec prevPos;
    Vec vel;
    Vec prevVel;
    struct ScenePlayer *player = (struct ScenePlayer *)&lbl_1_bss_DD4F4;
    float k;
    float t;

    pos = fx->pos;
    vel = fx->vel;
    prevPos = fx->prevPos;
    prevVel = fx->prevVel;
    switch (player->sceneId)
    {
    case 10:
        t = 0.08f;
        break;
    default:
        t = 1.0f;
        break;
    }
    switch (fx->unk2C)
    {
    case 0:
        fx->unk2C = 1;
        if (player->sceneId == 2)
            fn_1_147290(fx);
        memset(&e, 0, sizeof(e));
        e.type = 0x30;
        e.model = efcmdlGma->modelEntries[2].model;
        e.pos = pos;
        e.vel = vel;
        e.scale.x = 0.0f;
        e.scale.y = 3.0f;
        e.unk18 = 0.9f;
        e.unk1C = 0.71999997f;
        e.unk20 = 0.27f;
        spawn_effect(&e);
        /* fall through */
    case 1:
        memset(&e, 0, sizeof(e));
        e.type = 0x2B;
        e.model = NULL;
        e.timer = 60;
        e.pos = pos;
        k = 60.0f * t;
        e.vel = vel;
        e.vel.x += k * (RAND_FLOAT() - 0.5f);
        e.vel.y += k * (RAND_FLOAT() - 0.5f);
        e.vel.z += k * (RAND_FLOAT() - 0.5f);
        e.scale.y = t * (25.0f * (0.5f + RAND_FLOAT()));
        e.unk18 = 1.0f;
        e.unk1C = 0.8f;
        e.unk20 = 0.3f;
        spawn_effect(&e);
        break;
    }
}

static char *lbl_1_data_702FC[] =
{
    "BROKEN_GAWA_A",
    "BROKEN_GAWA_B",
    "BROKEN_GAWA_C",
    "BROKEN_GAWA_D",
    "BROKEN_GAWA_E",
    "BROKEN_GAWA_F",
};
static char *lbl_1_data_7033C[] =
{
    "BROKEN_TEKKOTU_A",
    "BROKEN_TEKKOTU_B",
};

void fn_1_147290(struct SoftFxAttached *fx)
{
    struct Effect e;
    Vec center;
    Vec pos;
    Vec vel;
    Vec tp;
    Vec dir;
    S16Vec rot;
    struct SoftFxAttached *self = fx;
    struct AnimJoint *linkedJoints = fx->desc->joints;
    char **names;
    int i;
    int fromCenter;
    struct GMAModel *model;
    float f;

    mtxa_from_mtx(fx->desc->mtx);
    mathutil_mtxA_get_translate(&center);
    pos = self->pos;
    vel = self->vel;
    mtxa_to_euler_yxz(&rot.y, &rot.x, &rot.z);
    memset(&e, 0, sizeof(e));
    e.type = 0x2F;
    names = lbl_1_data_702FC;
    for (i = 6; i > 0; i--, names++)
    {
        model = get_GmaBuffer_entry(stage_gma, *names);
        if (model != NULL)
        {
            e.model = model;
            e.pos = center;
            e.vel = vel;
            e.scale = linkedJoints->scale;
            mtxa_from_mtx(fx->desc->mtx);
            mtxa_translate(&model->boundSphereCenter);
            mathutil_mtxA_get_translate(&tp);
            dir.x = tp.x - pos.x;
            dir.y = tp.y - pos.y;
            dir.z = tp.z - pos.z;
            vec_normalize_len(&dir);
            e.unk70 = dir;
            e.unk64.x = 0.98f;
            e.unk64.y = 0.98f;
            e.unk64.z = 0.9f;
            f = 1.0f + 9.0f * RAND_FLOAT();
            e.vel.x += f * dir.x;
            e.vel.y += f * dir.y;
            e.vel.z += f * dir.z;
            e.rotX = rot.x;
            e.rotY = rot.y;
            e.rotZ = rot.z;
            e.unk52 = 512.0f * (RAND_FLOAT() - 0.5f);
            e.unk54 = 512.0f * (RAND_FLOAT() - 0.5f);
            e.unk56 = 512.0f * (RAND_FLOAT() - 0.5f);
            e.unkA4 = 7.5f * RAND_FLOAT();
            f = 1.0f + 0.2f * RAND_FLOAT();
            e.unk18 = f;
            e.unk1C = 0.8f * f;
            e.unk20 = 0.4f * f;
            spawn_effect(&e);
        }
    }
    for (i = 0x30; i > 0; i--)
    {
        if (i & 1)
        {
            fromCenter = 0;
            model = get_GmaBuffer_entry(stage_gma, lbl_1_data_7033C[(u32)(rand() & 0x7FFF) % 2]);
        }
        else
        {
            fromCenter = 1;
            model = get_GmaBuffer_entry(stage_gma, lbl_1_data_702FC[(u32)(rand() & 0x7FFF) % 6]);
        }
        if (model != NULL)
        {
            e.model = model;
            if (fromCenter)
                e.pos = center;
            else
                e.pos = pos;
            e.vel = vel;
            f = 0.2f + 0.8f * RAND_FLOAT();
            e.scale = linkedJoints->scale;
            e.scale.x *= f;
            e.scale.y *= f;
            e.scale.z *= f;
            if (fromCenter)
            {
                mtxa_from_mtx(fx->desc->mtx);
                mtxa_translate(&model->boundSphereCenter);
                mathutil_mtxA_get_translate(&tp);
                dir.x = tp.x - pos.x;
                dir.y = tp.y - pos.y;
                dir.z = tp.z - pos.z;
                vec_normalize_len(&dir);
                e.unk70 = dir;
                e.unk64.x = 0.98f;
                e.unk64.y = 0.98f;
                e.unk64.z = 0.9f;
            }
            else
            {
                mtxa_from_rotate_y((rand() & 0x7FFF) - 0x7FFF);
                mtxa_rotate_x((rand() & 0x7FFF) - 0x7FFF);
                mtxa_tf_vec_xyz(0.0f, 0.0f, -1.0f, &dir);
                e.unk70 = dir;
                e.unk64.x = 0.98f;
                e.unk64.y = 0.98f;
                e.unk64.z = 0.98f;
            }
            f = 5.0f + 27.0f * RAND_FLOAT();
            e.vel.x += f * dir.x;
            e.vel.y += f * fabs(dir.y);
            e.vel.z += f * dir.z;
            e.rotX = rot.x;
            e.rotY = rot.y;
            e.rotZ = rot.z;
            f = 512.0f / e.scale.x;
            e.unk52 = f * (RAND_FLOAT() - 0.5f);
            e.unk54 = f * (RAND_FLOAT() - 0.5f);
            e.unk56 = f * (RAND_FLOAT() - 0.5f);
            e.unkA4 = 60.0f * (0.25f + 0.5f * RAND_FLOAT());
            f = 1.0f + 0.5f * RAND_FLOAT();
            e.unk18 = f;
            e.unk1C = 0.8f * f;
            e.unk20 = 0.4f * f;
            e.colorFactor = 1.0f;
            spawn_effect(&e);
        }
    }
}

void fn_1_147B5C(struct SoftFxAttached *fx)
{
    struct Effect e;
    Vec center;
    Vec pos;
    Vec prevPos;
    Vec vel;
    Vec prevVel;
    Vec dir;
    Vec tmp;
    Vec out;
    S16Vec rot;
    struct AnimJoint *linkedJoints;
    int i;
    int fromCenter;
    struct GMAModel *model;
    float f;

    linkedJoints = fx->desc->joints;
    mtxa_from_mtx(fx->desc->mtx);
    mathutil_mtxA_get_translate(&center);
    mtxa_to_euler_yxz(&rot.y, &rot.x, &rot.z);
    pos = fx->pos;
    vel = fx->vel;
    prevPos = fx->prevPos;
    prevVel = fx->prevVel;
    switch (fx->unk2C)
    {
    case 0:
        fx->unk2C = 1;
        fx->unk30 = 0;
        model = fx->model;
        if (model != NULL)
        {
            memset(&e, 0, sizeof(e));
            e.type = 0x2B;
            e.model = (struct GMAModel *)fx->desc;
            e.pos = pos;
            e.vel = vel;
            e.rotX = (rand() & 0x7FFF) - 0x7FFF;
            e.rotY = (rand() & 0x7FFF) - 0x7FFF;
            e.unk52 = 1092.2667f * (RAND_FLOAT() - 0.5f);
            e.unk54 = 2184.5334f * (RAND_FLOAT() - 0.5f);
            for (i = 4; i > 0; i--)
            {
                e.timer = 60.0f * (1.0f + 1.5f * RAND_FLOAT());
                mtxa_from_translate_xyz(0.0f, 100.0f, 0.0f);
                mtxa_scale_xyz(0.5f, 0.5f, 1.0f);
                mtxa_rotate_y((rand() & 0x7FFF) - 0x7FFF);
                mtxa_rotate_x((rand() & 0x7FFF) - 0x7FFF);
                mtxa_tf_point_xyz(0.0f, 0.0f, 250.0f, &e.unk8C);
                e.scale.y = 10.0f + 30.0f * RAND_FLOAT();
                e.unk18 = 1.0f;
                e.unk1C = 0.8f;
                e.unk20 = 0.3f;
                spawn_effect(&e);
            }
        }
        memset(&e, 0, sizeof(e));
        e.type = 0x2F;
        for (i = (rand() & 7) + 0x20; i > 0; i--)
        {
            if (rand() & 1)
            {
                fromCenter = 0;
                model = get_GmaBuffer_entry(stage_gma, lbl_1_data_7033C[(u32)(rand() & 0x7FFF) % 2]);
            }
            else
            {
                fromCenter = 1;
                model = get_GmaBuffer_entry(stage_gma, lbl_1_data_702FC[(u32)(rand() & 0x7FFF) % 6]);
            }
            if (model != NULL)
            {
                e.model = model;
                e.timer = 60.0f * (1.5f + RAND_FLOAT());
                mtxa_from_rotate_y((rand() & 0x7FFF) - 0x7FFF);
                mtxa_rotate_x((rand() & 0x7FFF) - 0x7FFF);
                mtxa_tf_vec_xyz(0.0f, 0.0f, 200.0f + 600.0f * RAND_FLOAT(), &tmp);
                e.pos.x = pos.x + tmp.x;
                e.pos.y = pos.y + tmp.y;
                e.pos.z = pos.z + tmp.z;
                mtxa_rotate_y(8192.0f * (RAND_FLOAT() - 0.5f));
                mtxa_tf_vec_xyz(0.0f, 0.0f, 1.0f + 40.0f * RAND_FLOAT(), &tmp);
                e.vel.x = vel.x + tmp.x;
                e.vel.y = vel.y + tmp.y;
                e.vel.z = vel.z + tmp.z;
                f = 0.2f + 0.8f * RAND_FLOAT();
                e.scale = linkedJoints->scale;
                e.scale.x *= f;
                e.scale.y *= f;
                e.scale.z *= f;
                if (fromCenter)
                {
                    mtxa_from_mtx(fx->desc->mtx);
                    mtxa_translate(&model->boundSphereCenter);
                    mathutil_mtxA_get_translate(&tmp);
                    dir.x = tmp.x - center.x;
                    dir.y = tmp.y - center.y;
                    dir.z = tmp.z - center.z;
                    vec_normalize_len(&dir);
                    e.unk70 = dir;
                    e.unk64.x = 0.99f;
                    e.unk64.y = 0.99f;
                    e.unk64.z = 0.96f;
                }
                else
                {
                    mtxa_from_rotate_y((rand() & 0x7FFF) - 0x7FFF);
                    mtxa_rotate_x((rand() & 0x7FFF) - 0x7FFF);
                    mtxa_tf_vec_xyz(0.0f, 0.0f, -1.0f, &dir);
                    e.unk70 = dir;
                    e.unk64.x = 0.99f;
                    e.unk64.y = 0.99f;
                    e.unk64.z = 0.99f;
                }
                e.rotX = rot.x;
                e.rotY = rot.y;
                e.rotZ = rot.z;
                f = 128.0f / e.scale.x;
                e.unk52 = f * (RAND_FLOAT() - 0.5f);
                e.unk54 = f * (RAND_FLOAT() - 0.5f);
                e.unk56 = f * (RAND_FLOAT() - 0.5f);
                e.unkA4 = 60.0f * (0.01f + 0.1f * RAND_FLOAT());
                f = 0.5f + 0.25f * RAND_FLOAT();
                e.unk18 = f;
                e.unk1C = 0.8f * f;
                e.unk20 = 0.4f * f;
                spawn_effect(&e);
            }
        }
        /* fallthrough */
    case 1:
        fx->unk30--;
        if (fx->unk30 < 0)
        {
            fx->unk30 = rand() & 3;
            memset(&e, 0, sizeof(e));
            e.type = 0x2F;
            if (rand() & 1)
            {
                fromCenter = 0;
                model = get_GmaBuffer_entry(stage_gma, lbl_1_data_7033C[(u32)(rand() & 0x7FFF) % 2]);
            }
            else
            {
                fromCenter = 1;
                model = get_GmaBuffer_entry(stage_gma, lbl_1_data_702FC[(u32)(rand() & 0x7FFF) % 6]);
            }
            if (model != NULL)
            {
                e.model = model;
                e.timer = 60.0f * (1.5f + RAND_FLOAT());
                e.pos = pos;
                e.vel = vel;
                f = 0.2f + 0.8f * RAND_FLOAT();
                e.scale = linkedJoints->scale;
                e.scale.x *= f;
                e.scale.y *= f;
                e.scale.z *= f;
                if (fromCenter)
                {
                    mtxa_from_mtx(fx->desc->mtx);
                    mtxa_translate(&model->boundSphereCenter);
                    mathutil_mtxA_get_translate(&tmp);
                    dir.x = tmp.x - center.x;
                    dir.y = tmp.y - center.y;
                    dir.z = tmp.z - center.z;
                    vec_normalize_len(&dir);
                    e.unk70 = dir;
                    e.unk64.x = 0.99f;
                    e.unk64.y = 0.99f;
                    e.unk64.z = 0.96f;
                }
                else
                {
                    mtxa_from_rotate_y((rand() & 0x7FFF) - 0x7FFF);
                    mtxa_rotate_x((rand() & 0x7FFF) - 0x7FFF);
                    mtxa_tf_vec_xyz(0.0f, 0.0f, -1.0f, &dir);
                    e.unk70 = dir;
                    e.unk64.x = 0.99f;
                    e.unk64.y = 0.99f;
                    e.unk64.z = 0.99f;
                }
                f = 5.0f + 27.0f * RAND_FLOAT();
                e.vel.x += f * dir.x;
                e.vel.y += f * fabs(dir.y);
                e.vel.z += f * dir.z;
                mtxa_from_mtx(fx->desc->mtx);
                mtxa_invert();
                mtxa_mult_left(fx->mtx);
                tmp.x = model->boundSphereCenter.x + e.vel.x;
                tmp.y = model->boundSphereCenter.y + e.vel.y;
                tmp.z = model->boundSphereCenter.z + e.vel.z;
                mtxa_tf_vec(&tmp, &out);
                e.vel.x += tmp.x - out.x;
                e.vel.y += tmp.y - out.y;
                e.vel.z += tmp.z - out.z;
                e.pos.x += e.vel.x;
                e.pos.y += e.vel.y;
                e.pos.z += e.vel.z;
                e.rotX = rot.x;
                e.rotY = rot.y;
                e.rotZ = rot.z;
                f = 128.0f / e.scale.x;
                e.unk52 = f * (RAND_FLOAT() - 0.5f);
                e.unk54 = f * (RAND_FLOAT() - 0.5f);
                e.unk56 = f * (RAND_FLOAT() - 0.5f);
                e.unkA4 = 60.0f * (0.01f + 0.1f * RAND_FLOAT());
                f = 0.5f + 0.25f * RAND_FLOAT();
                e.unk18 = f;
                e.unk1C = 0.8f * f;
                e.unk20 = 0.4f * f;
                spawn_effect(&e);
            }
        }
        break;
    }
}

static s16 lbl_1_data_70344[] =
{
    3, 1, 5, 6, 9, 10, 13, 15, 16, 18, 19, 20, 21, 23, 24, 0,
};

void fn_1_148A24(struct SoftFxAttached *fx)
{
    struct Effect e;
    Mtx m;
    Vec pos;
    Vec vel;
    struct SoftFxAttached *self = fx;
    struct Ape *ape = fx->desc->ape;
    float scale = ape->scale;

    mtxa_from_quat(&ape->rot);
    mtxa_to_mtx(m);
    mtxa_from_translate(&ape->pos);
    mtxa_scale_s(ape->scale);
    mtxa_translate(&ape->unk264);
    mtxa_mult_right(m);
    mtxa_to_mtx(m);
    pos = self->pos;
    vel = self->vel;
    switch (self->unk2C)
    {
    case 0:
        self->unk2C = 1;
        /* fallthrough */
    case 1:
        memset(&e, 0, sizeof(e));
        e.type = 0x33;
        e.model = efcmdlGma->modelEntries[26].model;
        e.timer = 60.0f * (0.5f + 0.5f * RAND_FLOAT());
        mtxa_from_mtx(m);
        mtxa_mult_right(g_get_ape_anim_smth(ape, lbl_1_data_70344[g_font_char_var_3 % 15]));
        mathutil_mtxA_get_translate(&e.pos);
        e.vel = vel;
        e.scale.x = scale * (0.25f + 0.5f * RAND_FLOAT());
        e.scale.y = e.scale.x * (0.016666668f * RAND_FLOAT());
        e.scale.z = scale;
        spawn_effect(&e);
        break;
    }
}

void fn_1_148CA0(struct SoftFxAttached *fx)
{
    struct Effect e;
    Mtx m;
    Vec pos;
    Vec vel;
    struct SoftFxAttached *self = fx;
    struct Ape *ape = fx->desc->ape;
    float scale = ape->scale;
    float k;

    mtxa_from_quat(&ape->rot);
    mtxa_to_mtx(m);
    mtxa_from_translate(&ape->pos);
    mtxa_scale_s(ape->scale);
    mtxa_translate(&ape->unk264);
    mtxa_mult_right(m);
    mtxa_to_mtx(m);
    pos = self->pos;
    vel = self->vel;
    switch (self->unk2C)
    {
    case 0:
        self->unk2C = 1;
        /* fallthrough */
    case 1:
        memset(&e, 0, sizeof(e));
        e.type = 0x34;
        e.timer = 60.0f * (0.25f + 0.25f * RAND_FLOAT());
        mtxa_from_mtx(m);
        mtxa_mult_right(g_get_ape_anim_smth(ape, lbl_1_data_70344[g_font_char_var_3 % 15]));
        mathutil_mtxA_get_translate(&e.pos);
        k = 0.5f * scale;
        e.pos.x += k * (RAND_FLOAT() - 0.5f);
        e.pos.y += k * (RAND_FLOAT() - 0.5f);
        e.pos.z += k * (RAND_FLOAT() - 0.5f);
        e.scale.x = 0.0f;
        e.scale.y = scale * (0.005f + 0.025f * RAND_FLOAT());
        e.scale.z = 0.5f * scale;
        e.rotZ = 0;
        e.unk56 = 0;
        e.unkA8 = 0;
        e.unk8C.x = 1.0f;
        e.unk8C.y = 0.3f + 0.2f * RAND_FLOAT();
        e.unk8C.z = 1.0f;
        e.unk18 = 0.25f + 0.75f * RAND_FLOAT();
        e.unk1C = 0.25f + 0.75f * RAND_FLOAT();
        e.unk20 = 0.25f + 0.75f * RAND_FLOAT();
        spawn_effect(&e);
        break;
    }
}

void fn_1_1490B0(struct SoftFxAttached *fx)
{
    struct Effect e;
    Vec pos;
    Vec prevPos;
    Vec vel;
    Vec prevVel;
    Vec v;
    struct SceneObjDesc *desc;
    struct SoftFxAttached *self;
    struct ScenePlayer *player;
    struct AnimJoint *linkedJoints;
    float k;
    float scale;
    float sizeScale;
    float velScale;
    int interval;

    desc = fx->desc;
    player = (struct ScenePlayer *)&lbl_1_bss_DD4F4;
    self = fx;
    linkedJoints = desc != NULL ? desc->joints : NULL;
    scale = linkedJoints != NULL ? linkedJoints->scale.x : 1.0f;
    switch (player->sceneId)
    {
    case 10:
        sizeScale = 0.3f;
        interval = 6;
        velScale = 0.15f;
        break;
    default:
        sizeScale = scale;
        interval = 4;
        velScale = scale;
        break;
    }
    pos = self->pos;
    vel = self->vel;
    prevPos = self->prevPos;
    prevVel = self->prevVel;
    switch (self->unk2C)
    {
    case 0:
        self->unk2C = 1;
        self->unk98 = (float)player->frame / (float)player->frameMax;
        memset(&e, 0, sizeof(e));
        e.type = 0x30;
        e.model = efcmdlGma->modelEntries[2].model;
        e.pos = pos;
        e.vel = vel;
        e.scale.x = 0.0f;
        e.scale.y = 4.0f * scale;
        e.unk18 = 1.0f;
        e.unk1C = 0.8f;
        e.unk20 = 0.3f;
        spawn_effect(&e);
        /* fallthrough */
    case 1:
        k = (float)player->frame / (float)player->frameMax;
        scale = (k - self->unk98) / (1.0f - self->unk98);
        if (g_font_char_var_3 % interval == 0)
        {
            memset(&e, 0, sizeof(e));
            memset(&e, 0, sizeof(e));
            e.type = 0x2D;
            e.model = efcmdlGma->modelEntries[8].model;
            k = 150.0f * sizeScale;
            v.x = pos.x + k * (RAND_FLOAT() - 0.5f);
            v.y = pos.y + k * (RAND_FLOAT() - 0.5f);
            v.z = pos.z + k * (RAND_FLOAT() - 0.5f);
            e.pos = v;
            e.scale.x = 0.5f * sizeScale;
            e.scale.y = e.scale.x * (0.1f + 0.2f * RAND_FLOAT());
            k = 1.5f + 0.5f * RAND_FLOAT();
            e.unk18 = k;
            e.unk1C = 0.8f * k;
            e.unk20 = 0.5f * k;
            spawn_effect(&e);

            e.type = 0x2E;
            e.model = efcmdlGma->modelEntries[18].model;
            e.rotX = (rand() & 0x7FFF) - 0x7FFF;
            e.rotY = (rand() & 0x7FFF) - 0x7FFF;
            e.rotZ = (rand() & 0x7FFF) - 0x7FFF;
            e.unk56 = 1092.2667f * (RAND_FLOAT() - 0.5f);
            e.scale.x = sizeScale * (0.2f * (1.0f + 0.4f * RAND_FLOAT()));
            e.scale.y = e.scale.x * (1.5f + 1.5f * RAND_FLOAT());
            k = 1.0f + 0.5f * RAND_FLOAT();
            e.unk18 = k;
            e.unk1C = 0.95f * k;
            e.unk20 = 0.5f * k;
            spawn_effect(&e);

            e.type = 0x2B;
            e.model = NULL;
            e.timer = 60.0f * (0.5f + 0.5f * RAND_FLOAT());
            e.scale.y = velScale * (8.0f * (0.05f + RAND_FLOAT()));
            mtxa_from_mtx(cameras[0].unk14C);
            mtxa_tf_point(&e.pos, &v);
            mtxa_push();
            k = e.scale.y * (2.0f + 10.0f * RAND_FLOAT());
            v.z = 0.0f;
            vec_set_len(k, &v, &v);
            mtxa_from_rotate_z(12288.0f * (RAND_FLOAT() - 0.5f));
            mtxa_tf_vec(&v, &v);
            mtxa_pop();
            mtxa_rigid_inv_tf_point(&v, &v);
            v.x -= e.pos.x;
            v.y -= e.pos.y;
            v.z -= e.pos.z;
            vec_normalize_len(&v);
            k = 0.1f * e.scale.y + (2.0f + 30.0f * scale) * velScale * (0.5f + 0.5f * RAND_FLOAT());
            e.vel.x += v.x * k;
            e.vel.y += v.y * k;
            e.vel.z += v.z * k;
            e.unk18 = 1.0f;
            e.unk1C = 0.8f;
            e.unk20 = 0.3f;
            spawn_effect(&e);
        }
        break;
    }
}

static s16 lbl_1_data_70364[] = { 3, 1, 5, 6, 9, 10 };

void fn_1_149930(struct SoftFxAttached *fx)
{
    struct Effect e;
    Vec pos;
    Vec vel;
    struct SceneObjDesc *desc = fx->desc;
    struct Ape *ape;
    float scale;
    float r;

    if (desc != NULL && desc->kind == 1)
    {
        ape = desc->ape;
        if (ape != NULL)
        {
            scale = ape->scale;
            pos = fx->pos;
            vel = fx->vel;
            switch (fx->unk2C)
            {
            case 0:
                fx->unk2C = 1;
                /* fallthrough */
            case 1:
                if ((rand() & 3) == 0)
                {
                    memset(&e, 0, sizeof(e));
                    e.type = 0x36;
                    e.timer = 60.0f * (0.25f + 0.5f * RAND_FLOAT());
                    e.model = (void *)(sptr)ape->apeIdx; /* an ape index, not a pointer */
                    e.u_otherTimer = ape->unk88;
                    e.playerId = lbl_1_data_70364[(u32)(rand() & 0x7FFF) % 6];
                    mtxa_from_rotate_y((rand() & 0x7FFF) - 0x7FFF);
                    mtxa_rotate_x((rand() & 0x7FFF) - 0x7FFF);
                    mtxa_tf_vec_xyz(0.0f, 0.0f, 0.2f, &e.unk8C);
                    e.scale.x = 0.0f;
                    e.scale.y = scale * (0.2f + 0.2f * RAND_FLOAT());
                    e.scale.z = scale;
                    r = RAND_FLOAT();
                    e.unk18 = 1.0f;
                    e.unk1C = 0.6f + 0.2f * r;
                    e.unk20 = 0.2f + 0.2f * r;
                    spawn_effect(&e);
                }
                break;
            }
        }
    }
}

void fn_1_149B98(struct SoftFxAttached *fx)
{
    struct Effect e;
    Vec pos;
    Vec prevPos;
    Vec vel;
    Vec prevVel;
    struct AnimJoint *linkedJoints = fx->desc->joints;
    float scale;

    pos = fx->pos;
    vel = fx->vel;
    prevPos = fx->prevPos;
    prevVel = fx->prevVel;
    scale = linkedJoints->scale.x;
    switch (fx->unk2C)
    {
    case 0:
        fx->unk2C = 1;
        memset(&e, 0, sizeof(e));
        e.type = 0x37;
        e.model = (void *)fx->desc;
        e.pos = pos;
        e.vel = vel;
        e.scale.y = 2.5f * scale;
        e.unk18 = 1.0f;
        e.unk1C = 0.9f;
        e.unk20 = 0.4f;
        fx->effectId = spawn_effect(&e);
        break;
    }
}

void fn_1_149CE4(struct SoftFxAttached *fx)
{
    struct Effect e;
    Vec pos;
    Vec vel;
    struct SceneObjDesc *desc = fx->desc;
    struct AnimJoint *linkedJoints = desc->joints;
    int i;
    float scale;
    float r;

    pos = fx->pos;
    vel = fx->vel;
    switch (desc->kind)
    {
    case 1:
        scale = desc->ape->scale;
        break;
    default:
        scale = linkedJoints->scale.x;
        break;
    }
    switch (fx->unk2C)
    {
    case 0:
        fx->unk2C = 1;
        memset(&e, 0, sizeof(e));
        e.type = 0x34;
        for (i = 2; i > 0; i--)
        {
            e.timer = 60.0f * (0.5f + 0.25f * RAND_FLOAT());
            e.pos = pos;
            e.vel.x = 0.5f * vel.x;
            e.vel.y = 0.5f * vel.y;
            e.vel.z = 0.5f * vel.z;
            e.scale.x = 0.0f;
            e.scale.y = scale * (0.25f + 0.05f * RAND_FLOAT());
            e.scale.z = 0.5f * scale;
            e.rotZ = (rand() & 0x7FFF) - 0x7FFF;
            e.unk56 = 1092.2667f * (1.0f + 0.5f * RAND_FLOAT());
            if (i & 1)
                e.unk56 = -e.unk56;
            e.unkA8 = (rand() & 0x7FFF) - 0x7FFF;
            e.unk8C.x = 1.0f;
            e.unk8C.y = 0.8f + 0.2f * RAND_FLOAT();
            e.unk8C.z = 1.0f;
            r = RAND_FLOAT();
            e.unk18 = 1.0f;
            e.unk1C = 0.5f + 0.5f * r;
            e.unk20 = 0.5f + 0.5f * r;
            spawn_effect(&e);
        }
        break;
    }
}

void fn_1_149FF0(struct SoftFxAttached *fx)
{
    struct Effect e;
    struct RaycastHit hit;
    Vec pos;
    Vec vel;
    Vec v;
    struct SoftFxAttached *self;
    struct SceneObjDesc *desc;
    struct AnimJoint *linkedJoints;
    int i;
    float scale;
    float r;

    desc = fx->desc;
    self = fx;
    linkedJoints = desc->joints;
    pos = self->pos;
    vel = self->vel;
    scale = linkedJoints->scale.x;
    switch (self->unk2C)
    {
    case 0:
        self->unk2C = 1;
        memset(&e, 0, sizeof(e));
        e.type = 0x34;
        for (i = 2; i > 0; i--)
        {
            e.timer = 60.0f * (0.5f + 0.25f * RAND_FLOAT());
            e.pos = pos;
            e.vel.x = 0.5f * vel.x;
            e.vel.y = 0.5f * vel.y;
            e.vel.z = 0.5f * vel.z;
            e.scale.x = 0.0f;
            e.scale.y = scale * (0.5f + 0.1f * RAND_FLOAT());
            e.scale.z = 0.5f * scale;
            e.rotZ = (rand() & 0x7FFF) - 0x7FFF;
            e.unk56 = 1092.2667f * (1.0f + 0.5f * RAND_FLOAT());
            if (i & 1)
                e.unk56 = -e.unk56;
            e.unkA8 = (rand() & 0x7FFF) - 0x7FFF;
            e.unk8C.x = 1.0f;
            e.unk8C.y = 0.8f + 0.2f * RAND_FLOAT();
            e.unk8C.z = 1.0f;
            r = RAND_FLOAT();
            e.unk18 = 1.0f;
            e.unk1C = 0.9f + 0.1f * r;
            e.unk20 = 0.125f + 0.5f * r;
            spawn_effect(&e);
        }
        self->unk30 = 0x30;
        /* fallthrough */
    case 1:
        self->unk30--;
        if (self->unk30 <= 0)
        {
            self->unk2C = 2;
            if ((int)raycast_stage_down(&pos, &hit, NULL) == 0)
                hit.pos.y = -20000.0f;
            memset(&e, 0, sizeof(e));
            e.type = 0x39;
            e.model = init_common_gma->modelEntries[68].model;
            e.pos = pos;
            e.unk98.x = 0.0f;
            e.unk98.z = 0.0f;
            e.scale.x = 0.0f;
            e.unk64 = hit.pos;
            e.unk70 = hit.normal;
            mtxa_from_rotate_y((rand() & 0x7FFF) - 0x7FFF);
            mtxa_rotate_x((rand() & 0x7FFF) - 0x7FFF);
            mtxa_rotate_z((rand() & 0x7FFF) - 0x7FFF);
            for (i = 0x30; i > 0; i--)
            {
                mtxa_push();
                mtxa_rotate_y((i << 16) / 48);
                r = 0.8f + 0.2f * RAND_FLOAT();
                mtxa_tf_vec_xyz(scale * r, 0.0f, 0.0f, &v);
                mtxa_pop();
                e.vel.x = 0.5f * vel.x + v.x;
                e.vel.y = 0.5f * vel.y + v.y;
                e.vel.z = 0.5f * vel.z + v.z;
                e.unk98.y = scale * (-0.008f * (0.8f + 0.4f * RAND_FLOAT()));
                e.scale.y = scale * (1.5f + RAND_FLOAT());
                e.rotX = (rand() & 0x7FFF) - 0x7FFF;
                e.rotY = (rand() & 0x7FFF) - 0x7FFF;
                e.rotZ = (rand() & 0x7FFF) - 0x7FFF;
                e.unk52 = 8192.0f * (RAND_FLOAT() - 0.5f);
                e.unk54 = 4096.0f * (RAND_FLOAT() - 0.5f);
                e.unk56 = 2048.0f * (RAND_FLOAT() - 0.5f);
                spawn_effect(&e);
            }
            e.model = init_common_gma->modelEntries[65].model;
            mtxa_rotate_x(0x4000);
            for (i = 0x30; i > 0; i--)
            {
                mtxa_push();
                mtxa_rotate_y((i << 16) / 48);
                r = 0.4f + 0.1f * RAND_FLOAT();
                mtxa_tf_vec_xyz(scale * r, 0.0f, 0.0f, &v);
                mtxa_pop();
                e.vel.x = 0.5f * vel.x + v.x;
                e.vel.y = 0.5f * vel.y + v.y;
                e.vel.z = 0.5f * vel.z + v.z;
                e.unk98.y = scale * (-0.008f * (0.8f + 0.4f * RAND_FLOAT()));
                e.scale.y = scale * (1.5f + RAND_FLOAT());
                e.rotX = (rand() & 0x7FFF) - 0x7FFF;
                e.rotY = (rand() & 0x7FFF) - 0x7FFF;
                e.rotZ = (rand() & 0x7FFF) - 0x7FFF;
                e.unk52 = 8192.0f * (RAND_FLOAT() - 0.5f);
                e.unk54 = 4096.0f * (RAND_FLOAT() - 0.5f);
                e.unk56 = 2048.0f * (RAND_FLOAT() - 0.5f);
                e.unk64 = hit.pos;
                e.unk70 = hit.normal;
                spawn_effect(&e);
            }
            self->unk30 = 0x1E;
        }
        break;
    case 2:
        if (self->unk30 > 0)
            self->unk30--;
        else
        {
            if ((int)raycast_stage_down(&pos, &hit, NULL) == 0)
                hit.pos.y = -20000.0f;
            memset(&e, 0, sizeof(e));
            e.type = 0x39;
            if (rand() & 1)
                e.model = init_common_gma->modelEntries[65].model;
            else
                e.model = init_common_gma->modelEntries[68].model;
            v.x = RAND_FLOAT() - 0.5f;
            v.y = RAND_FLOAT() - 0.5f;
            v.z = RAND_FLOAT() - 0.5f;
            vec_set_len(scale * (32.0f * RAND_FLOAT()), &v, &v);
            e.pos.x = pos.x + v.x;
            e.pos.y = pos.y + v.y;
            e.pos.z = pos.z + v.z;
            vec_normalize_len(&v);
            r = 0.4f * scale;
            e.vel.x = 0.5f * vel.x + v.x * r;
            e.vel.y = 0.5f * vel.y + v.y * r;
            e.vel.z = 0.5f * vel.z + v.z * r;
            e.scale.x = 0.0f;
            e.scale.y = scale * (1.0f + 2.5f * RAND_FLOAT());
            e.unk98.x = 0.0f;
            e.unk98.y = scale * (-0.008f * (0.9f + 0.2f * RAND_FLOAT()));
            e.unk98.z = 0.0f;
            e.rotX = (rand() & 0x7FFF) - 0x7FFF;
            e.rotY = (rand() & 0x7FFF) - 0x7FFF;
            e.rotZ = (rand() & 0x7FFF) - 0x7FFF;
            e.unk52 = 3072.0f * (RAND_FLOAT() - 0.5f);
            e.unk54 = 1536.0f * (RAND_FLOAT() - 0.5f);
            e.unk56 = 768.0f * (RAND_FLOAT() - 0.5f);
            spawn_effect(&e);
        }
        break;
    }
}

void fn_1_14AD18(struct SoftFxAttached *fx)
{
    struct Effect e;
    Vec pos;
    Vec vel;
    struct SoftFxAttached *self;
    struct SceneObjDesc *desc;
    struct AnimJoint *linkedJoints;
    float scale;

    desc = fx->desc;
    self = fx;
    linkedJoints = desc->joints;
    switch (desc->kind)
    {
    case 1:
        scale = desc->ape->scale;
        break;
    default:
        scale = linkedJoints->scale.x;
        break;
    }
    pos = self->pos;
    vel = self->vel;
    switch (self->unk2C)
    {
    case 0:
        self->unk2C = 1;
        /* fallthrough */
    case 1:
        self->unk30++;
        if ((self->unk30 & 3) == 0)
        {
            memset(&e, 0, sizeof(e));
            e.type = 0x3A;
            e.timer = 60.0f * (0.25f + 0.25f * RAND_FLOAT());
            e.pos = pos;
            e.vel.x = vel.x;
            e.vel.y = vel.y;
            e.vel.z = vel.z;
            switch (fx->type)
            {
            case 0x2D:
                e.model = get_GmaBuffer_entry(stage_gma, "GUN_BIGLIGHT01");
                e.scale.x = 0.0f;
                e.scale.y = scale * (1.5f + 1.5f * RAND_FLOAT());
                break;
            case 0x2E:
                e.model = get_GmaBuffer_entry(stage_gma, "GUN_LIGHT01");
                e.scale.x = scale * (0.75f + 1.5f * RAND_FLOAT());
                e.scale.y = 0.0f;
                break;
            }
            e.scale.z = 10.0f * scale;
            if (e.model != NULL)
                spawn_effect(&e);
        }
        memset(&e, 0, sizeof(e));
        e.type = 0x3A;
        e.timer = 60.0f * (0.2f + 0.175f * RAND_FLOAT());
        e.pos = pos;
        e.vel.x = vel.x;
        e.vel.y = vel.y;
        e.vel.z = vel.z;
        e.unk8C.x = 0.0f;
        e.unk8C.y = -0.5f;
        e.unk8C.z = 0.0f;
        switch (fx->type)
        {
        case 0x2D:
            e.model = get_GmaBuffer_entry(stage_gma, "GUN_BIGLIGHT02");
            e.scale.x = 0.0f;
            e.scale.y = scale * (0.75f + 1.5f * RAND_FLOAT());
            break;
        case 0x2E:
            e.model = get_GmaBuffer_entry(stage_gma, "GUN_LIGHT02");
            e.scale.x = scale * (0.75f + 1.5f * RAND_FLOAT());
            e.scale.y = 0.0f;
            break;
        }
        e.scale.z = 10.0f * scale;
        e.rotZ = 65536.0f * (RAND_FLOAT() - 0.5f);
        if (e.model != NULL)
            spawn_effect(&e);
        break;
    }
}

void fn_1_14B19C(struct SoftFxAttached *fx)
{
    struct Effect e;
    Mtx m;
    Vec pos;
    Vec vel;
    struct SceneObjDesc *desc;
    struct SoftFxAttached *self;
    struct AnimJoint *linkedJoints;
    struct Ape *ape;
    float scale;
    float k;

    desc = fx->desc;
    self = fx;
    linkedJoints = desc->joints;
    switch (desc->kind)
    {
    case 1:
        ape = desc->ape;
        scale = ape->scale;
        mtxa_from_quat(&ape->rot);
        mtxa_to_mtx(m);
        mtxa_from_translate(&ape->pos);
        mtxa_scale_s(ape->scale);
        mtxa_translate(&ape->unk264);
        mtxa_mult_right(m);
        mtxa_mult_right(g_get_ape_anim_smth(ape, 3));
        switch (desc->ape->charaId)
        {
        case 14:
            mtxa_mult_right(g_get_ape_anim_smth(ape, 3));
            mtxa_tf_point_xyz(-0.2f + 0.1f * lbl_1_bss_6FB84, 0.45f + 0.1f * lbl_1_bss_6FB80,
                              -0.78f + 0.1f * lbl_1_bss_6FB7C, &pos);
            break;
        default:
            pos = self->pos;
            break;
        }
        break;
    default:
        scale = linkedJoints->scale.x;
        pos = self->pos;
        break;
    }
    vel = self->vel;
    switch (self->unk2C)
    {
    case 0:
        self->unk2C = 1;
        /* fallthrough */
    case 1:
        self->unk98 = self->unk98 + 0.1f * (1.0 - self->unk98);
        k = self->unk98;
        memset(&e, 0, sizeof(e));
        e.type = 0x3B;
        e.model = efcmdlGma->modelEntries[24].model;
        e.timer = 60.0f * (k * (0.65f + 0.1f * RAND_FLOAT()));
        e.pos = pos;
        k = 0.2f * scale;
        e.vel.x = vel.x + k * (0.1f * (RAND_FLOAT() - 0.5f));
        e.vel.y = vel.y + k * (0.8f + 0.4f * RAND_FLOAT());
        e.vel.z = vel.z + k * (0.1f * (RAND_FLOAT() - 0.5f));
        e.scale.x = scale * (0.2f * (1.0f + RAND_FLOAT()));
        e.scale.z = scale;
        e.rotZ = 65536.0f * (RAND_FLOAT() - 0.5f);
        e.unk56 = 512.0f * (RAND_FLOAT() - 0.5f);
        e.colorFactor = RAND_FLOAT();
        spawn_effect(&e);
        break;
    }
}

void fn_1_14B608(struct SoftFxAttached *fx)
{
    struct Effect e;
    Vec pos;
    Vec vel;
    struct SceneObjDesc *desc;
    struct SoftFxAttached *self;
    struct AnimJoint *linkedJoints;
    float scale;
    float k;
    float a;
    float n;

    desc = fx->desc;
    self = fx;
    linkedJoints = desc->joints;
    scale = linkedJoints->scale.x;
    pos = self->pos;
    vel = self->vel;
    switch (self->unk2C)
    {
    case 0:
        self->unk2C = 1;
        /* fallthrough */
    case 1:
        self->unk98 = self->unk98 + 0.025f * (1.0 - self->unk98);
        a = self->unk98;
        n = 0.5f + (self->unk94 + 4.0f * (1.0f - a));
        if (n > 1.0f)
        {
            memset(&e, 0, sizeof(e));
            e.type = 0x3D;
            e.model = efcmdlGma->modelEntries[21].model;
            while (n > 1.0f)
            {
                e.timer = 60.0f * (0.65f + 0.2f * RAND_FLOAT());
                e.pos = pos;
                e.pos.x = e.pos.x + (RAND_FLOAT() - 0.5f);
                e.pos.z = e.pos.z + (RAND_FLOAT() - 0.5f);
                k = 0.3f * ((1.0f - 0.9f * a) * scale);
                e.vel.x = vel.x + k * (0.8f * (RAND_FLOAT() - 0.5f));
                e.vel.y = vel.y + k * (0.2f + RAND_FLOAT());
                e.vel.z = vel.z + k * (0.8f * (RAND_FLOAT() - 0.5f));
                e.scale.x = scale * (0.4f * (0.25f + RAND_FLOAT()));
                e.scale.y = e.scale.x * (0.05f * RAND_FLOAT());
                e.scale.z = scale;
                e.rotZ = 65536.0f * (RAND_FLOAT() - 0.5f);
                e.unk56 = 512.0f * (RAND_FLOAT() - 0.5f);
                e.colorFactor = (1.0f - a) * (0.5f + 0.5f * RAND_FLOAT());
                spawn_effect(&e);
                n -= 1.0f;
            }
        }
        self->unk94 = n;
        break;
    }
}

void fn_1_14BAAC(struct SoftFxAttached *fx)
{
    struct Effect e;
    Vec pos;
    Vec vel;
    float scale = fx->desc->joints->scale.x;

    pos = fx->pos;
    vel = fx->vel;
    switch (fx->unk2C)
    {
    case 0:
        fx->unk2C = 1;
        memset(&e, 0, sizeof(e));
        e.type = 0x3E;
        e.model = efcmdlGma->modelEntries[2].model;
        e.scale.x = 5.0f * scale;
        e.pos = pos;
        e.vel = vel;
        e.unk18 = 0.8f;
        e.unk1C = 1.0f;
        e.unk20 = 0.7f;
        spawn_effect(&e);
        break;
    case 1:
        break;
    }
}

void fn_1_14BBD4(struct SoftFxAttached *fx)
{
    struct Effect e;
    Vec pos;
    Vec vel;
    float scale = fx->desc->joints->scale.x;

    pos = fx->pos;
    vel = fx->vel;
    switch (fx->unk2C)
    {
    case 0:
        fx->unk2C = 1;
        memset(&e, 0, sizeof(e));
        e.type = 0x41;
        e.model = efcmdlGma->modelEntries[2].model;
        e.scale.x = 5.0f * scale;
        e.pos = pos;
        e.vel = vel;
        e.unk18 = 0.8f;
        e.unk1C = 1.0f;
        e.unk20 = 0.7f;
        spawn_effect(&e);
        break;
    case 1:
        break;
    }
}

void fn_1_14BCFC(struct SoftFxAttached *fx)
{
    struct Effect e;
    Vec pos;
    Vec vel;
    Vec top;
    struct SceneObjDesc *desc;
    struct SoftFxAttached *self;
    struct AnimJoint *linkedJoints;
    float scale;
    float k;
    float remain;
    float total;
    float u;
    float t;
    float dx;
    float dy;
    float dz;

    desc = fx->desc;
    self = fx;
    linkedJoints = desc->joints;
    scale = linkedJoints->scale.x;
    pos = self->pos;
    vel = self->vel;
    switch (self->unk2C)
    {
    case 0:
        self->unk2C = 1;
        memset(&e, 0, sizeof(e));
        e.type = 0x3C;
        e.model = efcmdlGma->modelEntries[24].model;
        top = pos;
        top.y = top.y + 300.0f * scale;
        remain = mathutil_vec_distance(&pos, &top);
        total = remain;
        while (remain > 0.0f)
        {
            t = remain / total;
            u = 1.0f - t;
            e.timer = (int)(600.0f * t) + 1;
            e.pos.x = pos.x * t + top.x * u;
            e.pos.y = pos.y * t + top.y * u;
            e.pos.z = pos.z * t + top.z * u;
            k = 0.5f * scale;
            e.vel.x = vel.x + k * (0.1f * (RAND_FLOAT() - 0.5f));
            e.vel.y = vel.y + k * (0.8f + 0.4f * RAND_FLOAT());
            e.vel.z = vel.z + k * (0.1f * (RAND_FLOAT() - 0.5f));
            e.scale.x = scale * (30.0f * (0.5f + 0.5f * RAND_FLOAT()));
            e.scale.y = ((90.0f - e.scale.x) / 600.0f) * (0.5f + 0.5f * RAND_FLOAT());
            e.vel.z = vel.z;
            e.rotZ = 65536.0f * (RAND_FLOAT() - 0.5f);
            e.unk56 = 256.0f * (RAND_FLOAT() - 0.5f);
            e.unk8C.x = 0.25f * (0.2f + 0.8f * RAND_FLOAT());
            rand();
            e.unk8C.y = 0.0f;
            rand();
            e.unk8C.z = 0.0f;
            e.colorFactor = 1.0f;
            k = 600.0f - e.timer;
            e.scale.x = e.scale.x + e.scale.y * k;
            dx = 0.25f * (0.5f * u);
            dy = 0.0f;
            dz = 0.0f;
            e.pos.x = e.pos.x + dx * k;
            e.pos.y = e.pos.y + dy * k;
            e.pos.z = e.pos.z + dz * k;
            e.vel.x = e.vel.x + dx;
            e.vel.y = e.vel.y + dy;
            e.vel.z = e.vel.z + dz;
            spawn_effect(&e);
            remain -= 56.0f * mathutil_vec_len(&e.vel);
        }
        /* fallthrough */
    case 1:
        self->unk98 = self->unk98 + 0.1f * (1.0 - self->unk98);
        if (g_font_char_var_3 % 56 == 0)
        {
            memset(&e, 0, sizeof(e));
            e.type = 0x3C;
            e.model = efcmdlGma->modelEntries[24].model;
            e.timer = 600;
            e.pos = pos;
            k = 0.5f * scale;
            e.vel.x = vel.x + k * (0.1f * (RAND_FLOAT() - 0.5f));
            e.vel.y = vel.y + k * (0.8f + 0.4f * RAND_FLOAT());
            e.vel.z = vel.z + k * (0.1f * (RAND_FLOAT() - 0.5f));
            e.scale.x = scale * (30.0f * (0.5f + 0.5f * RAND_FLOAT()));
            e.scale.y = ((90.0f - e.scale.x) / 600.0f) * (0.5f + 0.5f * RAND_FLOAT());
            e.rotZ = 65536.0f * (RAND_FLOAT() - 0.5f);
            e.unk56 = 512.0f * (RAND_FLOAT() - 0.5f);
            e.unk8C.x = 0.25f * (0.2f + 0.8f * RAND_FLOAT());
            rand();
            e.unk8C.y = 0.0f;
            rand();
            e.unk8C.z = 0.0f;
            spawn_effect(&e);
        }
        break;
    }
}

static struct ChainTbl lbl_1_data_703A8[] =
{
    { 0, 0x13, { 0.0f, 0.2f, 0.0f } },
    { 0, 0x18, { 0.0f, 0.2f, 0.0f } },
};

static struct ChainTbl lbl_1_data_703C8[] =
{
    { 1, 3, { 0.22f, 0.07f, 0.0f } },
};

void fn_1_14C5E0(struct SoftFxChain *fx);
void fn_1_14C7F8(struct SoftFxChain *fx);
void fn_1_14CA74(struct SoftFxChain *fx, struct ChainPart *part, float scale);
void fn_1_14CEDC(struct SoftFxChain *fx, struct ChainPart *part, float scale);

void fn_1_14C574(struct SoftFx *fx, int mask)
{
}

int fn_1_14C578(struct SoftFx *fx, int msg)
{
    int r = 1;

    switch (msg)
    {
    case 0:
        fn_1_14C5E0((struct SoftFxChain *)fx);
        break;
    case 3:
        fx->state = 3;
        break;
    case 2:
        fn_1_14C7F8((struct SoftFxChain *)fx);
        break;
    }
    return r;
}

void fn_1_14C5E0(struct SoftFxChain *fx)
{
    struct SceneObjDesc *desc = fx->desc;
    int i;
    struct ChainPart *part;
    struct ChainTbl *tbl;

    fx->timer = 0;
    switch (world_theme.bgId)
    {
    case 0x1D:
    case 0xB:
    case 0x28:
        switch (current_stage_id)
        {
        case 0x191:
        case 0x18D:
            fx->v24.x = 0.0f;
            fx->v24.y = -10.0f;
            fx->v24.z = 0.0f;
            break;
        default:
            fx->v24.x = 0.0f;
            fx->v24.y = 0.0f;
            fx->v24.z = 0.0f;
            break;
        }
        fx->v30.x = 0.0f;
        fx->v30.y = 1.0f;
        fx->v30.z = 0.0f;
        break;
    default:
        fx->v24.x = 0.0f;
        fx->v24.y = 20000.0f;
        fx->v24.z = 0.0f;
        fx->v30.x = 0.0f;
        fx->v30.y = 1.0f;
        fx->v30.z = 0.0f;
        break;
    }
    switch (desc->kind)
    {
    case 1:
        switch (fx->type)
        {
        case 0x10:
            fx->count = 1;
            tbl = lbl_1_data_703C8;
            break;
        default:
            fx->count = 2;
            tbl = lbl_1_data_703A8;
            break;
        }
        part = fx->parts;
        for (i = fx->count; i > 0; i--, part++, tbl++)
        {
            part->kind = tbl->kind;
            part->bone = tbl->bone;
            part->offset = tbl->offset;
        }
        break;
    default:
        fx->count = 1;
        part = fx->parts;
        part->kind = 0;
        part->bone = -1;
        break;
    }
    part = fx->parts;
    for (i = fx->count; i > 0; i--, part++)
        part->unk40 = RAND_FLOAT();
}

void fn_1_14C7F8(struct SoftFxChain *fx)
{
    int i;
    struct SoftFxChain *self;
    struct SceneObjDesc *desc;
    Mtx m;
    int flag;
    struct ChainPart *part;
    struct Ape *ape;
    struct AnimJoint *linkedJoints;
    float scale;

    desc = fx->desc;
    self = fx;
    ape = desc->ape;
    linkedJoints = desc->joints;
    flag = self->timer > 1;
    switch (desc->kind)
    {
    case 1:
        mtxa_from_quat(&ape->rot);
        mtxa_to_mtx(m);
        mtxa_from_translate(&ape->pos);
        mtxa_scale_s(ape->scale);
        mtxa_translate(&ape->unk264);
        mtxa_mult_right(m);
        mtxa_to_mtx(m);
        scale = ape->scale;
        break;
    default:
        scale = linkedJoints->scale.x;
        break;
    }
    part = self->parts;
    for (i = self->count; i > 0; i--, part++)
    {
        part->prevPos = part->pos;
        part->prevVel = part->vel;
        if (part->bone < 0)
        {
            mtxa_from_mtx(desc->mtx);
            mtxa_translate(&part->offset);
            mathutil_mtxA_get_translate(&part->pos);
        }
        else
        {
            mtxa_from_mtx(m);
            mtxa_mult_right(g_get_ape_anim_smth(ape, part->bone));
            mtxa_translate(&part->offset);
            mathutil_mtxA_get_translate(&part->pos);
        }
        part->vel.x = part->pos.x - part->prevPos.x;
        part->vel.y = part->pos.y - part->prevPos.y;
        part->vel.z = part->pos.z - part->prevPos.z;
        if (flag == 0)
        {
            part->prevPos = part->pos;
            part->prevVel = part->vel;
        }
    }
    if (flag)
    {
        part = self->parts;
        for (i = self->count; i > 0; i--, part++)
        {
            switch (part->kind)
            {
            case 0:
                fn_1_14CA74(self, part, scale);
                break;
            case 1:
                fn_1_14CEDC(self, part, scale);
                break;
            }
        }
    }
    if (self->timer < 0x10000)
        self->timer++;
}

void fn_1_14CA74(struct SoftFxChain *fx, struct ChainPart *part, float scale)
{
    float u;
    Vec cur;
    Vec prev;
    Vec vel;
    Vec prevVel;
    Vec dir;
    float n;
    float total;
    float t;
    struct Effect e;
    float d;
    float len;

    vel = part->vel;
    len = mathutil_vec_len(&vel);
    len = (10.0f / scale) * len;
    n = part->unk40 + len;
    if (n > 1.0f)
    {
        memset(&e, 0, sizeof(e));
        e.type = 0x32;
        e.model = efcmdlGma->modelEntries[33].model;
        e.unk64 = fx->v24;
        e.unk70 = fx->v30;
        cur = part->pos;
        prev = part->prevPos;
        prevVel = part->prevVel;
        dir.x = vel.x - prevVel.x;
        dir.y = vel.y - prevVel.y;
        dir.z = vel.z - prevVel.z;
        vec_normalize_len(&dir);
        total = n;
        while (n > 1.0f)
        {
            t = n / total;
            u = 1.0f - t;
            e.timer = 60.0f * (2.0f + 2.0f * RAND_FLOAT());
            e.pos.x = cur.x * t + prev.x * u;
            e.pos.y = cur.y * t + prev.y * u;
            e.pos.z = cur.z * t + prev.z * u;
            e.vel.x = vel.x * t + prevVel.x * u;
            e.vel.y = vel.y * t + prevVel.y * u;
            e.vel.z = vel.z * t + prevVel.z * u;
            d = mathutil_vec_dot_prod(&e.vel, &dir);
            e.vel.x -= dir.x * d;
            e.vel.y -= dir.y * d;
            e.vel.z -= dir.z * d;
            e.vel.x *= 0.8f;
            e.vel.y *= 0.8f;
            e.vel.z *= 0.8f;
            d *= 1.1f;
            e.vel.x += dir.x * d;
            e.vel.y += dir.y * d;
            e.vel.z += dir.z * d;
            u = mathutil_vec_len(&e.vel);
            e.scale.y = scale * (0.01f + 0.02f * RAND_FLOAT()) + (0.35f * u) * RAND_FLOAT();
            e.scale.x = 0.0f;
            e.scale.z = scale;
            spawn_effect(&e);
            n -= 1.0f;
        }
    }
    part->unk40 = n;
}

void fn_1_14CEDC(struct SoftFxChain *fx, struct ChainPart *part, float scale)
{
    struct Effect e;
    Vec cur;
    Vec prev;
    Vec vel;
    Vec prevVel;
    Vec dir;
    struct Ape *ape = fx->desc->ape;
    float n;
    float total;
    float t;
    float u;
    float k;
    float rate;

    vel = part->vel;
    k = 0.01f * ape->unkB0;
    if (k < 0.0f)
        k = 0.0f;
    if (k == part->unk44)
        k = 0.0f;
    else
    {
        switch (ape->faceExpr)
        {
        case 0:
            k = 1.0f - k;
            break;
        }
        part->unk44 = k;
    }
    rate = 4.0f * (k - 0.5f);
    if (rate < 0.016666668f)
        rate = 0.016666668f;
    n = part->unk40 + rate;
    if (n > 1.0f)
    {
        memset(&e, 0, sizeof(e));
        e.type = 0x32;
        e.model = efcmdlGma->modelEntries[33].model;
        e.unk64 = fx->v24;
        e.unk70 = fx->v30;
        cur = part->pos;
        prev = part->prevPos;
        prevVel = part->prevVel;
        dir.x = vel.x - prev.x;
        dir.y = vel.y - prev.y;
        dir.z = vel.z - prev.z;
        vec_normalize_len(&dir);
        total = n;
        while (n > 1.0f)
        {
            t = n / total;
            u = 1.0f - t;
            e.timer = 60.0f * (2.0f + 2.0f * RAND_FLOAT());
            e.pos.x = cur.x * t + prev.x * u;
            e.pos.y = cur.y * t + prev.y * u;
            e.pos.z = cur.z * t + prev.z * u;
            e.vel.x = 0.8f * (vel.x * t + prevVel.x * u);
            e.vel.y = 0.8f * (vel.y * t + prevVel.y * u);
            e.vel.z = 0.8f * (vel.z * t + prevVel.z * u);
            e.scale.y = scale * (0.02f + (0.05f * k) * RAND_FLOAT());
            e.scale.x = 0.0f;
            e.scale.z = scale;
            spawn_effect(&e);
            n -= 1.0f;
        }
    }
    part->unk40 = n;
}

void fn_1_14D280(struct SoftFx *fx, int mask)
{
}

static struct BoneTbl lbl_1_data_703D8[] =
{
    { 12, { 0.1f, 0.0f, 0.0f } },
    { 8, { 0.1f, 0.0f, 0.0f } },
};

static struct BoneAnimRange lbl_1_data_703F8[] =
{
    { 12, 33, 100, 3, 54.0f, 70.0f },
    { 8, 33, 100, 5, 20.0f, 40.0f },
    { 12, 33, 100, 6, 20.0f, 40.0f },
    { 8, 33, 100, 7, 20.0f, 40.0f },
    { 12, 33, 100, 9, 20.0f, 40.0f },
    { 8, 33, 100, 10, 20.0f, 40.0f },
    { -1 },
};

void fn_1_14D314(struct SoftFxBones *fx);
void fn_1_14D3F8(struct SoftFxBones *fx);
void fn_1_14D644(struct SoftFxBones *fx, struct BonePart *part, float scale);

int fn_1_14D284(struct SoftFx *fx, int msg)
{
    int r = 1;

    switch (msg)
    {
    case 0:
        if (fx->desc == NULL || fx->desc->kind != 1)
            fx->state = 3;
        else
            fn_1_14D314((struct SoftFxBones *)fx);
        break;
    case 3:
        fx->state = 3;
        break;
    case 2:
        fn_1_14D3F8((struct SoftFxBones *)fx);
        break;
    }
    return r;
}

void fn_1_14D314(struct SoftFxBones *fx)
{
    int i;
    struct BonePart *part;
    struct BoneTbl *tbl;

    fx->timer = 0;
    fx->count = 2;
    tbl = lbl_1_data_703D8;
    part = fx->parts;
    for (i = fx->count; i > 0; i--, part++, tbl++)
    {
        part->bone = tbl->bone;
        part->offset = tbl->offset;
    }
    part = fx->parts;
    for (i = fx->count; i > 0; i--, part++)
        part->unk40 = RAND_FLOAT();
}

void fn_1_14D3F8(struct SoftFxBones *fx)
{
    int i;
    struct SoftFxBones *self;
    struct SceneObjDesc *desc;
    Mtx m;
    int flag;
    struct BonePart *part;
    struct Ape *ape;
    struct AnimJoint *linkedJoints;
    float scale;

    desc = fx->desc;
    self = fx;
    ape = desc->ape;
    linkedJoints = desc->joints;
    flag = self->timer > 1;
    switch (desc->kind)
    {
    case 1:
        mtxa_from_quat(&ape->rot);
        mtxa_to_mtx(m);
        mtxa_from_translate(&ape->pos);
        mtxa_scale_s(ape->scale);
        mtxa_translate(&ape->unk264);
        mtxa_mult_right(m);
        mtxa_to_mtx(m);
        scale = ape->scale;
        break;
    default:
        scale = linkedJoints->scale.x;
        break;
    }
    part = self->parts;
    for (i = self->count; i > 0; i--, part++)
    {
        part->prevPos = part->pos;
        part->prevVel = part->vel;
        if (part->bone < 0)
        {
            mtxa_from_mtx(desc->mtx);
            mtxa_translate(&part->offset);
            mathutil_mtxA_get_translate(&part->pos);
        }
        else
        {
            mtxa_from_mtx(m);
            mtxa_mult_right(g_get_ape_anim_smth(ape, part->bone));
            mtxa_translate(&part->offset);
            mathutil_mtxA_get_translate(&part->pos);
        }
        part->vel.x = part->pos.x - part->prevPos.x;
        part->vel.y = part->pos.y - part->prevPos.y;
        part->vel.z = part->pos.z - part->prevPos.z;
        if (flag == 0)
        {
            part->prevPos = part->pos;
            part->prevVel = part->vel;
        }
    }
    if (flag)
    {
        part = self->parts;
        for (i = self->count; i > 0; i--, part++)
            fn_1_14D644(self, part, scale);
    }
    if (self->timer < 0x10000)
        self->timer++;
}

void fn_1_14D644(struct SoftFxBones *fx, struct BonePart *part, float scale)
{
    struct Effect e;
    Vec cur;
    Vec prev;
    Vec vel;
    Vec prevVel;
    struct Ape *ape;
    float frame;
    struct BoneAnimRange *tbl;
    s16 bone;
    s16 anim;
    s16 a;
    s16 b;
    float dx;
    float dy;
    float n;
    float total;
    float t;
    float u;
    float k;
    float speed;
    float dz;

    vel = part->vel;
    ape = fx->desc->ape;
    frame = fn_1_1135E8(ape);
    bone = part->bone;
    anim = ape->unk68;
    a = ape->unk6C;
    b = ape->unk70;
    k = 0.0f;
    for (tbl = lbl_1_data_703F8; tbl->bone >= 0; tbl++)
    {
        if (bone == tbl->bone && anim == tbl->unk2 && a == tbl->unk4 && b == tbl->unk6
         && frame >= tbl->start && frame <= tbl->end)
        {
            k = 1.0f;
            break;
        }
    }
    speed = mathutil_vec_len(&vel) / scale;
    k = k * (75.0f * speed);
    n = part->unk40 + k;
    if (n > 1.0f)
    {
        memset(&e, 0, sizeof(e));
        e.type = 0x35;
        cur = part->pos;
        prev = part->prevPos;
        prevVel = part->prevVel;
        total = n;
        while (n > 1.0f)
        {
            t = n / total;
            u = 1.0f - t;
            e.timer = 60.0f * (0.25f + 10.0f * (speed * RAND_FLOAT()));
            e.pos.x = cur.x * t + prev.x * u;
            e.pos.y = cur.y * t + prev.y * u;
            e.pos.z = cur.z * t + prev.z * u;
            k = 0.1f * scale;
            dx = k * (RAND_FLOAT() - 0.5f);
            dy = k * (RAND_FLOAT() - 0.5f);
            dz = k * (RAND_FLOAT() - 0.5f);
            e.pos.x += dx;
            e.pos.y += dy;
            e.pos.z += dz;
            e.vel.x = 0.1f * dx + (vel.x * t + prevVel.x * u);
            e.vel.y = 0.1f * dy + (vel.y * t + prevVel.y * u);
            e.vel.z = 0.1f * dz + (vel.z * t + prevVel.z * u);
            e.scale.y = scale * (0.005f + speed * (0.25f * RAND_FLOAT()));
            e.scale.x = 0.0f;
            e.scale.z = scale;
            spawn_effect(&e);
            n -= 1.0f;
        }
    }
    part->unk40 = n;
}

void fn_1_14DADC(struct SoftFx *fx, int mask)
{
}

u32 GXGetTexBufferSize(u16 width, u16 height, u32 format, u8 mipmap, u8 maxLOD);
void fn_1_14DB54(struct SoftFxScreen *fx);
void fn_1_14DC28(struct SoftFxScreen *fx);
void fn_1_14DC78(struct SoftFxScreen *fx);

int fn_1_14DAE0(struct SoftFx *fx, int msg)
{
    int r = 1;

    switch (msg)
    {
    case 0:
        fn_1_14DB54((struct SoftFxScreen *)fx);
        break;
    case 2:
        fn_1_14DC78((struct SoftFxScreen *)fx);
        break;
    case 3:
        fx->state = 3;
        break;
    case 4:
        fn_1_14DC28((struct SoftFxScreen *)fx);
        break;
    }
    return r;
}

void fn_1_14DB54(struct SoftFxScreen *fx)
{
    struct SoftFxScreen *self = fx;
    int w;
    int h;
    u32 size;

    fx->drawMask = 8;
    w = current_render_mode->fbWidth >> 1;
    h = current_render_mode->efbHeight >> 1;
    size = GXGetTexBufferSize(w, h, 1, 0, 0);
    self->texBuf = alloc_from_heap_or_panic(currentHeap, size, "scnplay_fx.c", 0x149C);
    self->unk20 = 0.0f;
    self->unk24 = 0.0f;
    self->unk28 = -1.0f;
    self->unk38 = 192.0f;
    self->unk3C = 144.0f;
    self->unk40 = 48.0f;
}

void fn_1_14DC28(struct SoftFxScreen *fx)
{
    if (fx->texBuf != NULL)
    {
        OSFreeToHeap(currentHeap, fx->texBuf);
        fx->texBuf = NULL;
    }
}

void fn_1_14DC78(struct SoftFxScreen *fx)
{
    struct SoftFxScreen *self = fx;

    self->unk34 += -0.00025f;
    self->unk34 *= 0.99f;
    self->unk28 += self->unk34;
    if (self->unk28 < -1.2f)
    {
        self->unk28 = -1.2f;
        if (self->unk34 < 0.0f)
            self->unk34 *= -(0.4f + 0.1f * RAND_FLOAT());
    }
    self->unk38 = self->unk38 + 0.08f * (64.0f - self->unk38);
    self->unk3C = self->unk3C + 0.09f * (48.0f - self->unk3C);
    self->unk40 = self->unk40 + 0.1f * (16.0f - self->unk40);
}

static inline void f32_to_u8(register float *in, register u8 *out)
{
#ifdef __MWERKS__
    asm
    {
        lfs f1, 0(in)
        psq_st f1, 0(out), 1, 2
    }
#else
    *out = pc_psq_st_u8(*in);
#endif
}

void GXGetViewportv(float *viewport);
void GXGetProjectionv(float *proj);
void GXSetTexCopySrc_cached(u16 left, u16 top, u16 width, u16 height);
void GXSetTexCopyDst_cached(u16 width, u16 height, int format, u8 mipmap);
void GXCopyTex(void *dest, u8 clear);
void GXInvalidateTexAll(void);
void GXSetNumChans_cached(u8 n);
void GXSetNumTexGens_cached(u8 n);
void GXSetNumTevStages_cached(u8 n);
void GXSetNumIndStages_cached(u8 n);
void GXSetCullMode_cached(int mode);
void GXLoadTexObj_cached(GXTexObj *obj, int map);
void GXSetTexCoordGen2_cached(int dst, int func, int src, int mtx, int normalize, int postMtx);
void GXSetTevKColor_cached(int id, GXColor color);
void GXSetTevKColorSel_cached(int stage, int sel);
void GXSetTevKAlphaSel_cached(int stage, int sel);
void GXSetTevColorIn_cached(int stage, int a, int b, int c, int d);
void GXSetTevColorOp_cached(int stage, int op, int bias, int scale, u8 clamp, int outReg);
void GXSetTevAlphaIn_cached(int stage, int a, int b, int c, int d);
void GXSetTevAlphaOp_cached(int stage, int op, int bias, int scale, u8 clamp, int outReg);
void GXSetTevSwapMode_cached(int stage, int rasSel, int texSel);
void GXSetTevSwapModeTable_cached(int id, int r, int g, int b, int a);
void GXSetIndTexOrder_cached(int stage, int coord, int map);
void GXSetTevIndirect_cached(int stage, int indStage, int format, int biasSel, int mtx, int wrapS, int wrapT,
    int addPrev, int utcLod, int alphaSel);
void GXSetZMode_cached(u8 compare, int func, u8 update);
void GXSetBlendMode_cached(int type, int src, int dst, int op);
void g_GXSetTevOrder_wrapper(int stage, int coord, int map, int color);
void g_GXSetTevIndirect_zero_if_different(int stage);
void g_GXSetIndTexMtx_wrapper(int id, float mtx[2][3], s8 scale);
void g_something_with_raster_state(int);
void g_something_with_GX_vtx_desc(u32 attrs);
void fn_800668FC(void);

void fn_1_14DDDC(struct SoftFxScreen *fx);
void fn_1_14E320(struct SoftFxScreen *fx);

void fn_1_14DDB0(struct SoftFx *fx, int mask)
{
    switch (mask)
    {
    case 8:
        fn_1_14DDDC((struct SoftFxScreen *)fx);
        break;
    }
}

void fn_1_14DDDC(struct SoftFxScreen *fx)
{
    float vp[6];
    float proj[7];
    Vec pos;
    Vec c;
    GXColor color;
    GXColor color2;
    int w;
    int h;
    void *buf;
    s16 rot;
    float k;
    float right;
    float left;
    float bottom;
    float top;

    if (fx->captured == 0)
    {
        buf = fx->texBuf;
        GXGetViewportv(vp);
        w = (int)vp[2] >> 1;
        h = (int)vp[3] >> 1;
        GXSetTexCopySrc_cached((int)vp[0], (int)vp[1], (int)vp[2], (int)vp[3]);
        GXSetTexCopyDst_cached(w, h, 1, 1);
        GXCopyTex(buf, 0);
        GXInitTexObj(&fx->tex, buf, w, h, 1, 0, 0, 0);
        GXInvalidateTexAll();
        fx->captured = 1;
    }
    fn_1_14E320(fx);
    fn_800668FC();
    g_something_with_GX_vtx_desc((1 << 9) | (1 << 13));
    GXGetProjectionv(proj);
    pos = *(Vec *)&fx->unk20;
    rot = 2048.0f * ((-1.0f - pos.z) / 0.20000005f);
    mtxa_from_translate(&pos);
    mtxa_rotate_z(rot);
    GXLoadPosMtxImm(mtxa->mtxA, 0);
    c.x = 192.0f;
    c.y = 176.0f;
    c.z = 128.0f;
    f32_to_u8(&c.x, &color.r);
    f32_to_u8(&c.y, &color.g);
    f32_to_u8(&c.z, &color.b);
    GXSetTevKColor_cached(0, color);
    k = 1.0f / proj[1];
    left = -1.0f * (1.05f - proj[2]) * k;
    right = -1.0f * (-1.05f - proj[2]) * k;
    k = 1.0f / proj[3];
    bottom = -1.0f * (-1.05f - proj[4]) * k;
    top = -1.0f * (1.05f - proj[4]) * k;
    GXBegin(0x80, 0, 4);
    {
        float y = bottom;
        GXPosition3f32(left, y, 0.0f);
    }
    GXTexCoord2f32(0.0f, 0.0f);
    {
        float x = right;
        GXPosition3f32(x, bottom, 0.0f);
    }
    GXTexCoord2f32(1.0f, 0.0f);
    {
        float y = top;
        GXPosition3f32(right, y, 0.0f);
    }
    GXTexCoord2f32(1.0f, 1.0f);
    GXPosition3f32(left, top, 0.0f);
    GXTexCoord2f32(0.0f, 1.0f);
    f32_to_u8(&fx->unk38, &color2.r);
    f32_to_u8(&fx->unk3C, &color2.g);
    f32_to_u8(&fx->unk40, &color2.b);
    GXSetTevKColor_cached(0, color2);
    k = 1.0f / proj[1];
    left = -1.0f * (1.0f - proj[2]) * k;
    right = -1.0f * (-1.0f - proj[2]) * k;
    k = 1.0f / proj[3];
    bottom = -1.0f * (-1.0f - proj[4]) * k;
    top = -1.0f * (1.0f - proj[4]) * k;
    GXBegin(0x80, 0, 4);
    {
        float y = bottom;
        GXPosition3f32(left, y, 0.0f);
    }
    GXTexCoord2f32(0.0f, 0.0f);
    {
        float x = right;
        GXPosition3f32(x, bottom, 0.0f);
    }
    GXTexCoord2f32(1.0f, 0.0f);
    {
        float y = top;
        GXPosition3f32(right, y, 0.0f);
    }
    GXTexCoord2f32(1.0f, 1.0f);
    GXPosition3f32(left, top, 0.0f);
    GXTexCoord2f32(0.0f, 1.0f);
}

void fn_1_14E320(struct SoftFxScreen *fx)
{
    GXColor color;

    GXSetNumChans_cached(0);
    GXSetCullMode_cached(0);
    GXLoadTexObj_cached(&fx->tex, 0);
    f32_to_u8(&fx->unk38, &color.r);
    f32_to_u8(&fx->unk3C, &color.g);
    f32_to_u8(&fx->unk40, &color.b);
    GXSetTevKColor_cached(0, color);
    GXSetTexCoordGen2_cached(0, 1, 4, 0x3C, 0, 0x7D);
    g_GXSetTevOrder_wrapper(0, 0, 0, 0xFF);
    GXSetTevKColorSel_cached(0, 0xC);
    GXSetTevKAlphaSel_cached(0, 0);
    g_GXSetTevIndirect_zero_if_different(0);
    GXSetTevColorIn_cached(0, 8, 0xF, 0xF, 0xE);
    GXSetTevColorOp_cached(0, 0, 0, 0, 1, 0);
    GXSetTevAlphaIn_cached(0, 7, 7, 7, 6);
    GXSetTevAlphaOp_cached(0, 0, 0, 0, 1, 0);
    GXSetNumTevStages_cached(1);
    GXSetNumTexGens_cached(1);
    GXSetZMode_cached(1, 7, 0);
    g_something_with_raster_state(0);
    GXSetBlendMode_cached(0, 1, 0, 0);
}

void fn_1_14E510(struct SoftFxScreen2 *fx);
void fn_1_14E5DC(struct SoftFxScreen2 *fx);
void fn_1_14E62C(struct SoftFxScreen2 *fx);

int fn_1_14E49C(struct SoftFx *fx, int msg)
{
    int r = 1;

    switch (msg)
    {
    case 0:
        fn_1_14E510((struct SoftFxScreen2 *)fx);
        break;
    case 2:
        fn_1_14E62C((struct SoftFxScreen2 *)fx);
        break;
    case 3:
        fx->state = 3;
        break;
    case 4:
        fn_1_14E5DC((struct SoftFxScreen2 *)fx);
        break;
    }
    return r;
}

void fn_1_14E510(struct SoftFxScreen2 *fx)
{
    struct SoftFxScreen2 *self = fx;
    int w;
    int h;
    u32 size;

    fx->drawMask = 8;
    w = current_render_mode->fbWidth >> 1;
    h = current_render_mode->efbHeight >> 1;
    size = GXGetTexBufferSize(w, h, 1, 0, 0);
    self->texBuf = alloc_from_heap_or_panic(currentHeap, size, "scnplay_fx.c", 0x162E);
    self->unk20 = 1.0f;
    self->unk24 = 0.0f;
    self->unk28 = 192.0f;
    self->unk2C = 128.0f;
    self->unk30 = 0.0f;
}

void fn_1_14E5DC(struct SoftFxScreen2 *fx)
{
    if (fx->texBuf != NULL)
    {
        OSFreeToHeap(currentHeap, fx->texBuf);
        fx->texBuf = NULL;
    }
}

void fn_1_14E62C(struct SoftFxScreen2 *fx)
{
    fx->unk24 = fx->unk24 + 0.05f * (0.0033333334f - fx->unk24);
    fx->unk20 += fx->unk24;
    fx->unk28 = fx->unk28 + 0.08f * (48.0f - fx->unk28);
    fx->unk2C = fx->unk2C + 0.09f * (32.0f - fx->unk2C);
    fx->unk30 = fx->unk30 + 0.1f * -fx->unk30;
}

void fn_1_14E6F0(struct SoftFxScreen2 *fx);
void fn_1_14EABC(struct SoftFxScreen2 *fx);

void fn_1_14E6C4(struct SoftFx *fx, int mask)
{
    switch (mask)
    {
    case 8:
        fn_1_14E6F0((struct SoftFxScreen2 *)fx);
        break;
    }
}

void fn_1_14E6F0(struct SoftFxScreen2 *fx)
{
    float vp[6];
    float proj[7];
    GXColor color;
    int w;
    int h;
    void *buf;
    float k;
    float s;
    float right;
    float left;
    float bottom;
    float top;
    float u;

    if (fx->captured == 0)
    {
        buf = fx->texBuf;
        GXGetViewportv(vp);
        w = (int)vp[2] >> 1;
        h = (int)vp[3] >> 1;
        GXSetTexCopySrc_cached((int)vp[0], (int)vp[1], (int)vp[2], (int)vp[3]);
        GXSetTexCopyDst_cached(w, h, 1, 1);
        GXCopyTex(buf, 0);
        GXInitTexObj(&fx->tex, buf, w, h, 1, 0, 0, 0);
        GXInvalidateTexAll();
        fx->captured = 1;
    }
    fn_1_14EABC(fx);
    fn_800668FC();
    g_something_with_GX_vtx_desc((1 << 9) | (1 << 13) | (1 << 14));
    GXGetProjectionv(proj);
    mtxa_from_translate_xyz(0.0f, 0.0f, -1.0f);
    GXLoadPosMtxImm(mtxa->mtxA, 0);
    f32_to_u8(&fx->unk28, &color.r);
    f32_to_u8(&fx->unk2C, &color.g);
    f32_to_u8(&fx->unk30, &color.b);
    GXSetTevKColor_cached(0, color);
    s = fx->unk20;
    k = 1.0f / proj[1];
    left = k * (-1.0f * (s - proj[2]));
    right = k * (-1.0f * (-s - proj[2]));
    k = 1.0f / proj[3];
    bottom = k * (-1.0f * (-s - proj[4]));
    top = k * (-1.0f * (s - proj[4]));
    u = -0.2f + 0.1f * lbl_1_bss_6FB84;
    GXBegin(0x80, 0, 4);
    {
        float y = bottom;
        GXPosition3f32(left, y, 0.0f);
    }
    GXTexCoord2f32(0.0f, 0.0f);
    GXTexCoord2f32(u, 0.2f + u);
    {
        float x = right;
        GXPosition3f32(x, bottom, 0.0f);
    }
    GXTexCoord2f32(1.0f, 0.0f);
    GXTexCoord2f32(u, 0.2f + u);
    {
        float y = top;
        GXPosition3f32(right, y, 0.0f);
    }
    GXTexCoord2f32(1.0f, 1.0f);
    GXTexCoord2f32(u, u - 0.2f);
    GXPosition3f32(left, top, 0.0f);
    GXTexCoord2f32(0.0f, 1.0f);
    GXTexCoord2f32(u, u - 0.2f);
}

void fn_1_14EABC(struct SoftFxScreen2 *fx)
{
    struct SoftFxScreen2 *self = fx;
    float m[2][3];
    GXColor color;

    GXSetNumChans_cached(0);
    GXSetCullMode_cached(0);
    GXLoadTexObj_cached(init_common_gma->modelEntries[97].model->texObjs, 0);
    GXLoadTexObj_cached(&self->tex, 1);
    m[0][0] = 0.2f + 0.1f * lbl_1_bss_6FB80;
    m[0][1] = 0.0f;
    m[0][2] = 0.0f;
    m[1][0] = 0.0f;
    m[1][1] = 0.0f;
    m[1][2] = 0.0f;
    g_GXSetIndTexMtx_wrapper(1, m, 0);
    f32_to_u8(&self->unk28, &color.r);
    f32_to_u8(&self->unk2C, &color.g);
    f32_to_u8(&self->unk30, &color.b);
    GXSetTevKColor_cached(0, color);
    GXSetTevSwapModeTable_cached(2, 1, 1, 1, 1);
    GXSetTexCoordGen2_cached(0, 1, 5, 0x3C, 0, 0x7D);
    g_GXSetTevOrder_wrapper(0, 0, 0, 0xFF);
    GXSetTevSwapMode_cached(0, 0, 2);
    GXSetTevKColorSel_cached(0, 0xC);
    GXSetTevKAlphaSel_cached(0, 0);
    GXSetTevColorIn_cached(0, 0xC, 0xF, 8, 0xE);
    GXSetTevColorOp_cached(0, 0, 0, 0, 1, 0);
    GXSetTevAlphaIn_cached(0, 7, 7, 7, 6);
    GXSetTevAlphaOp_cached(0, 0, 0, 0, 1, 0);
    GXSetTexCoordGen2_cached(1, 1, 4, 0x3C, 0, 0x7D);
    GXSetIndTexOrder_cached(0, 1, 1);
    GXSetTevIndirect_cached(0, 0, 0, 0, 1, 0, 0, 0, 0, 0);
    GXSetNumTevStages_cached(1);
    GXSetNumTexGens_cached(2);
    GXSetNumIndStages_cached(1);
    GXSetZMode_cached(1, 7, 0);
    g_something_with_raster_state(0);
    GXSetBlendMode_cached(0, 1, 0, 0);
}

void fn_1_14EDB0(struct SoftFxGun *fx);
void fn_1_14EFE8(struct SoftFxGun *fx);

int fn_1_14ED48(struct SoftFx *fx, int msg)
{
    int r = 1;

    switch (msg)
    {
    case 0:
        fn_1_14EDB0((struct SoftFxGun *)fx);
        break;
    case 2:
        fn_1_14EFE8((struct SoftFxGun *)fx);
        break;
    case 3:
        fx->state = 3;
        break;
    }
    return r;
}

void fn_1_14EDB0(struct SoftFxGun *fx)
{
    struct SceneObjDesc *desc;
    struct ScenePlayer *player;
    struct SoftFxGun *self;
    struct Ape *ape;
    Mtx mtx;

    player = (struct ScenePlayer *)&lbl_1_bss_DD4F4;
    desc = fx->desc;
    self = fx;
    fx->drawMask = 4;
    self->gunScale = 0.1f;
    switch (player->sceneId)
    {
    case 5:
        self->rot.x = 0;
        self->rot.y = 0;
        self->rot.z = 0;
        break;
    default:
        self->rot.x = 0x2A00;
        self->rot.y = 0;
        self->rot.z = 0;
        break;
    }
    switch (desc->kind)
    {
    case 1:
        ape = desc->ape;
        mtxa_from_quat(&ape->rot);
        mtxa_to_mtx(mtx);
        mtxa_from_translate(&ape->pos);
        mtxa_scale_s(ape->scale);
        mtxa_translate(&ape->unk264);
        mtxa_mult_right(mtx);
        mtxa_mult_right(g_get_ape_anim_smth(ape, 0xC));
        mtxa_rotate_z(self->rot.z);
        mtxa_rotate_y(self->rot.y);
        mtxa_rotate_x(self->rot.x);
        mtxa_scale_s(self->gunScale);
        self->scale.x = 0.15 * ape->scale;
        self->scale.y = 0.15 * ape->scale;
        self->scale.z = 0.15 * ape->scale;
        self->muzzle.x = 2.2f;
        self->muzzle.y = 0.83f;
        self->muzzle.z = 0.0f;
        break;
    default:
        ape = (void *)fx->desc->joints;
        mtxa_from_mtx(desc->mtx);
        self->scale = ((struct AnimJoint *)ape)->scale;
        self->muzzle.x = 0.0f;
        self->muzzle.y = 0.0f;
        self->muzzle.z = 0.0f;
        break;
    }
    mtxa_rotate_z(self->rot.z);
    mtxa_rotate_y(self->rot.y);
    mtxa_rotate_x(self->rot.x);
    mtxa_scale_s(self->gunScale);
    mtxa_tf_point_xyz(2.0600002f, 0.47f, -0.10999998f, &self->pos);
    mtxa_translate_xyz(2.0600002f, 0.47f, -0.10999998f);
    mtxa_tf_point(&self->muzzle, &self->muzzlePos);
    mtxa_tf_vec_xyz(1.0f, 0.0f, 0.0f, &self->dir);
    vec_normalize_len(&self->dir);
    self->model = get_GmaBuffer_entry(stage_gma, "GUN");
}

void fn_1_14EFE8(struct SoftFxGun *fx)
{
    struct SceneObjDesc *desc = fx->desc;
    struct SoftFxGun *self = fx;
    struct Ape *ape;
    Mtx mtx;
    Vec prev;
    Vec p;
    Vec d;

    switch (desc->kind)
    {
    case 1:
        ape = desc->ape;
        mtxa_from_quat(&ape->rot);
        mtxa_to_mtx(mtx);
        mtxa_from_translate(&ape->pos);
        mtxa_scale_s(ape->scale);
        mtxa_translate(&ape->unk264);
        mtxa_mult_right(mtx);
        mtxa_mult_right(g_get_ape_anim_smth(ape, 0xC));
        self->scale.x = 0.15 * ape->scale;
        self->scale.y = 0.15 * ape->scale;
        self->scale.z = 0.15 * ape->scale;
        mtxa_rotate_z(self->rot.z);
        mtxa_rotate_y(self->rot.y);
        mtxa_rotate_x(self->rot.x);
        mtxa_scale_s(self->gunScale);
        break;
    default:
        ape = (void *)fx->desc->joints;
        mtxa_from_mtx(desc->mtx);
        self->scale = ((struct AnimJoint *)ape)->scale;
        break;
    }
    mtxa_tf_point_xyz(2.0600002f, 0.47f, -0.10999998f, &p);
    prev = self->pos;
    d.x = p.x - prev.x;
    d.y = p.y - prev.y;
    d.z = p.z - prev.z;
    self->pos = p;
    self->vel = d;
    mtxa_translate_xyz(2.0600002f, 0.47f, -0.10999998f);
    mtxa_tf_point(&self->muzzle, &p);
    mtxa_tf_vec_xyz(1.0f, 0.0f, 0.0f, &self->dir);
    vec_normalize_len(&self->dir);
    prev = self->muzzlePos;
    d.x = p.x - prev.x;
    d.y = p.y - prev.y;
    d.z = p.z - prev.z;
    self->muzzlePos = p;
    self->muzzleVel = d;
}

void fn_1_14F244(struct SoftFx *fx, int mask)
{
    struct SoftFxGun *self;
    struct SceneObjDesc *desc;
    struct GMAModel *model;
    struct Ape *ape;
    Mtx mtx;

    desc = fx->desc;
    self = (struct SoftFxGun *)fx;
    model = self->model;
    if (model == NULL)
        return;
    switch (desc->kind)
    {
    case 1:
        ape = desc->ape;
        mtxa_from_quat(&ape->rot);
        mtxa_to_mtx(mtx);
        mtxa_from_mtxb_translate(&ape->pos);
        mtxa_scale_s(ape->scale);
        mtxa_translate(&ape->unk264);
        mtxa_mult_right(mtx);
        mtxa_mult_right(g_get_ape_anim_smth(ape, 0xC));
        mtxa_rotate_z(self->rot.z);
        mtxa_rotate_y(self->rot.y);
        mtxa_rotate_x(self->rot.x);
        mtxa_scale_s(self->gunScale);
        break;
    default:
        mtxa_from_mtxb();
        mtxa_mult_right(desc->mtx);
        break;
    }
    mtxa_translate_xyz(2.0600002f, 0.47f, -0.10999998f);
    avdisp_draw_model_unculled_sort_always(model);
}

void fn_1_14F3A0(struct SoftFxGunBeam *fx);
void fn_1_14F488(struct SoftFxGunBeam *fx);

int fn_1_14F338(struct SoftFx *fx, int msg)
{
    int r = 1;

    switch (msg)
    {
    case 0:
        fn_1_14F3A0((struct SoftFxGunBeam *)fx);
        break;
    case 2:
        fn_1_14F488((struct SoftFxGunBeam *)fx);
        break;
    case 3:
        fx->state = 3;
        break;
    }
    return r;
}

void fn_1_14F3A0(struct SoftFxGunBeam *fx)
{
    struct SoftFxGunBeam *self = fx;
    struct GMAModel *model;

    self->timer = 0;
    fx->drawMask = 8;
    model = get_GmaBuffer_entry(stage_gma, "GUN_BEAM");
    if (model == NULL)
    {
        model = get_GmaBuffer_entry(stage_gma, "GUN_BIGBEAM");
        if (model == NULL)
            model = efcmdlGma->modelEntries[2].model;
    }
    self->beam = model;
    model = get_GmaBuffer_entry(stage_gma, "GUN_LIGHT01");
    if (model == NULL)
    {
        model = get_GmaBuffer_entry(stage_gma, "GUN_BIGLIGHT01");
        if (model == NULL)
            model = efcmdlGma->modelEntries[2].model;
    }
    self->light = model;
}

void fn_1_14F488(struct SoftFxGunBeam *fx)
{
    fx->unk28 = fx->unk28 + 0.2f * (1.0f - fx->unk28);
    fx->timer++;
}

static inline struct SoftFx *next_gun(struct SoftFx *fx)
{
    struct SoftFx *p;

    for (p = fx->next; p != NULL; p = p->next)
    {
        if (p->type == 0x17)
            break;
    }
    return p;
}

void fn_1_14F4C8(struct SoftFx *fx, int mask)
{
    struct GMAModel *beam;
    struct SoftFxGunBeam *self;
    struct GMAModel *light;
    int noObj;
    struct SceneObjDesc *desc;
    struct SoftFx *gun;
    s16 rot;
    Vec target;
    Vec muzzle;
    Vec v;
    Vec dir;
    float scale;
    float d;
    float len;
    float inv;
    float fade;
    float s;
    float base;
    float cur;
    float k;

    desc = fx->desc;
    self = (struct SoftFxGunBeam *)fx;
    beam = self->beam;
    light = self->light;
    switch (desc->kind)
    {
    case 0:
        noObj = 0;
        mtxa_from_mtx(desc->mtx);
        mathutil_mtxA_get_translate(&target);
        scale = desc->joints->scale.x;
        break;
    default:
        noObj = 1;
        break;
    }
    avdisp_set_z_mode(1, 3, 0);
    gun = next_gun((struct SoftFx *)&lbl_1_bss_DD640);
    while (gun != NULL)
    {
        struct SoftFxGun *g = (struct SoftFxGun *)gun;

        muzzle = g->muzzlePos;
        if (noObj)
        {
            scale = g->scale.x;
            if (scale <= 1.1920929e-07f)
            {
                gun = next_gun(gun);
                continue;
            }
            dir = g->dir;
            k = 48.0f * scale;
            target.x = muzzle.x + dir.x * k;
            target.y = muzzle.y + dir.y * k;
            target.z = muzzle.z + dir.z * k;
        }
        mtxa_from_mtxb_translate(&muzzle);
        mtxa_sq_from_identity();
        s = self->unk28 * scale;
        mtxa_scale_s(s / light->boundSphereRadius);
        if (g_is_sphere_visible(&light->boundSphereCenter, s))
        {
            mathutil_mtxA_get_translate(&v);
            if (v.z < -(0.1f + s))
            {
                k = (v.z + s) / v.z;
                v.x *= k;
                v.y *= k;
                v.z = v.z + s;
                mtxa_scale_s(k);
                mathutil_mtxA_set_translate(&v);
            }
            if (self->timer & 1)
                mtxa_scale_s(0.8f);
            load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
            avdisp_draw_model_unculled_sort_never(light);
        }
        d = mathutil_vec_distance(&muzzle, &target);
        len = d;
        inv = 1.0f / len;
        s = scale;
        {
            float phase = 0.0625f * (self->timer & 0xF);
            d = d - scale * phase;
            rot = 16384.0f * phase;
        }
        fade = len - 2.5f * scale;
        base = self->unk28 * s / beam->boundSphereRadius;
        while (d > scale)
        {
            float t = d * inv;
            float u = 1.0f - t;

            v.x = muzzle.x * t + target.x * u;
            v.y = muzzle.y * t + target.y * u;
            v.z = muzzle.z * t + target.z * u;
            cur = base;
            if (d > fade)
                cur *= math_sin(0x4000 + (int)(16384.0f * ((d - fade) / (len - fade))));
            mtxa_from_mtxb_translate(&v);
            mtxa_sq_from_identity();
            mtxa_scale_s(cur);
            if (g_is_sphere_visible(&beam->boundSphereCenter, s))
            {
                mtxa_rotate_z(rot);
                load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
                avdisp_draw_model_unculled_sort_never(beam);
            }
            d -= scale;
            rot += 0x2000;
        }
        gun = next_gun(gun);
    }
    avdisp_set_z_mode(1, 3, 1);
}

void fn_1_14FA20(struct SoftFxLaser *fx);
void fn_1_14FC28(struct SoftFxLaser *fx);

int fn_1_14F9B8(struct SoftFx *fx, int msg)
{
    int r = 1;

    switch (msg)
    {
    case 0:
        fn_1_14FA20((struct SoftFxLaser *)fx);
        break;
    case 3:
        fx->state = 3;
        break;
    case 2:
        fn_1_14FC28((struct SoftFxLaser *)fx);
        break;
    }
    return r;
}

static struct LaserPartDef lbl_1_data_70494[] =
{
    { 12, { 0.0f, 0.2f, 0.0f }, 0.3f },
    { 8, { 0.0f, 0.2f, 0.0f }, 0.3f },
    { 19, { 0.0f, 0.2f, 0.0f }, 0.3f },
    { 24, { 0.0f, 0.2f, 0.0f }, 0.3f },
    { 3, { 0.0f, 0.0f, 0.0f }, 0.4f },
    { 1, { 0.0f, 0.0f, 0.0f }, 0.2f },
    { 5, { 0.0f, 0.0f, 0.0f }, 0.2f },
    { 9, { 0.0f, 0.0f, 0.0f }, 0.2f },
};

static struct LaserPartDef lbl_1_data_70534[] =
{
    { 5, { 0.0f, 0.0f, 0.0f }, 0.5f },
    { 7, { -1.0f, 0.0f, 0.0f }, 0.5f },
    { 9, { 0.0f, 0.0f, 0.0f }, 0.5f },
    { 11, { 0.0f, 0.0f, 0.0f }, 2.0f },
};

void fn_1_14FA20(struct SoftFxLaser *fx)
{
    struct SoftFxLaser *self;
    struct SceneObjDesc *desc;
    int i;
    struct LaserPart *part;
    struct LaserPartDef *def;

    desc = fx->desc;
    self = fx;
    if (desc == NULL)
    {
        OSReport("[ WARNING!! ] Need Obj or Character!\n");
        fx->state = 3;
        return;
    }
    self->unk20 = 0;
    switch (world_theme.bgId)
    {
    case 0xB:
    case 0x1D:
    case 0x28:
        switch (current_stage_id)
        {
        case 0x18D:
        case 0x191:
            self->unk24.x = 0.0f;
            self->unk24.y = -10.0f;
            self->unk24.z = 0.0f;
            break;
        default:
            self->unk24.x = 0.0f;
            self->unk24.y = 0.0f;
            self->unk24.z = 0.0f;
            break;
        }
        self->unk30.x = 0.0f;
        self->unk30.y = 1.0f;
        self->unk30.z = 0.0f;
        break;
    default:
        self->unk24.x = 0.0f;
        self->unk24.y = 20000.0f;
        self->unk24.z = 0.0f;
        self->unk30.x = 0.0f;
        self->unk30.y = 1.0f;
        self->unk30.z = 0.0f;
        break;
    }
    switch (desc->kind)
    {
    case 1:
        switch (desc->ape->charaId)
        {
        case 0xE:
            self->count = 4;
            def = lbl_1_data_70534;
            break;
        default:
            self->count = 8;
            def = lbl_1_data_70494;
            break;
        }
        part = self->parts;
        for (i = self->count; i > 0; i--, part++, def++)
        {
            part->bone = def->bone;
            part->offset = def->offset;
            part->size = def->size;
        }
        break;
    default:
        self->count = 1;
        part = self->parts;
        part->bone = -1;
        part->size = 1.0f;
        break;
    }
    part = self->parts;
    for (i = self->count; i > 0; i--, part++)
        part->unk44 = 0.99f;
}

void fn_1_14FE88(struct SoftFxLaser *fx, struct LaserPart *part, float scale);

void fn_1_14FC28(struct SoftFxLaser *fx)
{
    Mtx mtx;
    int i;
    struct SoftFxLaser *self;
    struct SceneObjDesc *desc;
    int spawn;
    struct LaserPart *part;
    struct Ape *ape;
    struct AnimJoint *linkedJoints;
    float scale;

    desc = fx->desc;
    self = fx;
    ape = desc->ape;
    linkedJoints = desc->joints;
    spawn = self->unk20 > 1;
    switch (desc->kind)
    {
    case 1:
        mtxa_from_quat(&ape->rot);
        mtxa_to_mtx(mtx);
        mtxa_from_translate(&ape->pos);
        mtxa_scale_s(ape->scale);
        mtxa_translate(&ape->unk264);
        mtxa_mult_right(mtx);
        mtxa_to_mtx(mtx);
        scale = ape->scale;
        break;
    default:
        scale = 1.0f;
        break;
    }
    part = self->parts;
    for (i = self->count; i > 0; i--, part++)
    {
        part->prevPos = part->pos;
        part->prevVel = part->vel;
        if (part->bone < 0)
        {
            mtxa_from_mtx(desc->mtx);
            mtxa_translate(&part->offset);
            mathutil_mtxA_get_translate(&part->pos);
            part->size = linkedJoints->scale.x;
        }
        else
        {
            mtxa_from_mtx(mtx);
            mtxa_mult_right(g_get_ape_anim_smth(ape, part->bone));
            mtxa_translate(&part->offset);
            mathutil_mtxA_get_translate(&part->pos);
        }
        part->vel.x = part->pos.x - part->prevPos.x;
        part->vel.y = part->pos.y - part->prevPos.y;
        part->vel.z = part->pos.z - part->prevPos.z;
        if (spawn == 0)
        {
            part->prevPos = part->pos;
            part->prevVel = part->vel;
        }
    }
    if (spawn != 0)
    {
        part = self->parts;
        for (i = self->count; i > 0; i--, part++)
            fn_1_14FE88(self, part, scale);
    }
    if (self->unk20 < 0x10000)
        self->unk20++;
}

void fn_1_14FE88(struct SoftFxLaser *fx, struct LaserPart *part, float scale)
{
    struct Effect e;
    Vec curPos;
    Vec prevPos;
    Vec curVel;
    Vec prevVel;
    Vec d;
    Vec normal;
    Vec origin;
    float n;
    float size;
    float k;
    float total;
    float t;
    float u;
    float f;
    float len;

    size = part->size * scale;
    curVel = part->vel;
    prevVel = part->prevVel;
    len = mathutil_vec_len(&curVel);
    len = (1.5f / size) * len;
    n = part->unk44 + len;
    if (n > 1.0f)
    {
        origin = fx->unk24;
        normal = fx->unk30;
        memset(&e, 0, sizeof(e));
        switch (fx->type)
        {
        case 0x32:
            e.type = 0x43;
            break;
        default:
            e.type = 0x31;
            break;
        }
        e.model = efcmdlGma->modelEntries[21].model;
        e.unk64 = origin;
        e.unk70 = normal;
        curPos = part->pos;
        prevPos = part->prevPos;
        total = n;
        while (n > 1.0f)
        {
            t = n / total;
            u = 1.0f - t;
            e.timer = 60.0f * (0.5f + RAND_FLOAT());
            e.colorFactor = 0.0f;
            e.pos.x = curPos.x * t + prevPos.x * u;
            e.pos.y = curPos.y * t + prevPos.y * u;
            e.pos.z = curPos.z * t + prevPos.z * u;
            d.x = e.pos.x - origin.x;
            d.y = e.pos.y - origin.y;
            d.z = e.pos.z - origin.z;
            f = -mathutil_vec_dot_prod(&d, &normal);
            e.pos.x = e.pos.x + f * normal.x;
            e.pos.y = e.pos.y + f * normal.y;
            e.pos.z = e.pos.z + f * normal.z;
            k = 1.0 - fabs(f) / (3.0f * size);
            if (k < 0.0f)
            {
                n -= 1.0f;
                continue;
            }
            e.vel.x = 0.5f * (curVel.x * t + prevVel.x * u);
            e.vel.y = 0.5f * (curVel.y * t + prevVel.y * u);
            e.vel.z = 0.5f * (curVel.z * t + prevVel.z * u);
            f = -mathutil_vec_dot_prod(&e.vel, &normal);
            e.vel.x = e.vel.x + f * normal.x;
            e.vel.y = e.vel.y + f * normal.y;
            e.vel.z = e.vel.z + f * normal.z;
            u = 0.5f * mathutil_vec_len(&e.vel);
            f = 2.0 * fabs(f);
            e.unk8C.x = size * (0.35f + 0.7f * RAND_FLOAT());
            f = 0.1f * size + k * (f + u);
            e.unk8C.y = f * (0.15f + 0.35f * RAND_FLOAT());
            e.unk98.x = 0.025f * e.unk8C.x;
            e.unk98.y = e.unk8C.y * (1.0f + 0.5f * RAND_FLOAT());
            e.unk98.z = -0.02f;
            f = (1.0f + mathutil_vec_len(&e.vel)) / size;
            e.rotZ = (rand() & 0x7FFF) - 0x7FFF;
            e.unk56 = 6144.0f * (f * (RAND_FLOAT() - 0.5f));
            spawn_effect(&e);
            n -= 1.0f;
        }
    }
    part->unk44 = n;
}

void fn_1_1504AC(struct SoftFx *fx, int mask) {}

void fn_1_150524(struct SoftFxWarp *fx);
void fn_1_1506A0(struct SoftFxWarp *fx);
void fn_1_1506F0(struct SoftFxWarp *fx);

int fn_1_1504B0(struct SoftFx *fx, int msg)
{
    int r = 1;

    switch (msg)
    {
    case 0:
        fn_1_150524((struct SoftFxWarp *)fx);
        break;
    case 2:
        fn_1_1506F0((struct SoftFxWarp *)fx);
        break;
    case 3:
        fx->state = 3;
        break;
    case 4:
        fn_1_1506A0((struct SoftFxWarp *)fx);
        break;
    }
    return r;
}

void fn_1_150524(struct SoftFxWarp *fx)
{
    struct SoftFxWarp *self = fx;
    int w;
    int h;
    u32 size;

    fx->drawMask = 8;
    w = current_render_mode->fbWidth >> 1;
    h = current_render_mode->efbHeight >> 1;
    size = GXGetTexBufferSize(w, h, 4, 0, 0);
    self->texBuf = alloc_from_heap_or_panic(currentHeap, size, "scnplay_fx.c", 0x1BB1);
    self->captured = 0;
    self->tex = efcmdlGma->modelEntries[20].model->texObjs;
    self->texScale.x = 2.0f + 2.0f * RAND_FLOAT();
    self->texScale.y = 2.0f + 2.0f * RAND_FLOAT();
    self->texScale.z = 1.0f;
    self->unk20 = 1.0f + RAND_FLOAT();
}

void fn_1_1506A0(struct SoftFxWarp *fx)
{
    if (fx->texBuf != NULL)
    {
        OSFreeToHeap(currentHeap, fx->texBuf);
        fx->texBuf = NULL;
    }
}

void fn_1_1506F0(struct SoftFxWarp *fx)
{
    struct SoftFxWarp *self = fx;
    float t;
    float k;

    self->unk20 = self->unk20 + 0.05f * -self->unk20;
    t = self->unk20;
    switch (fx->type)
    {
    case 0x31:
        k = 0.05f * (0.8f + 0.2f * t);
        self->indMtx[0][0] = 0.0f;
        self->indMtx[0][1] = k;
        self->indMtx[0][2] = 0.0f;
        k = -0.1f * (0.8f + 0.2f * t);
        self->indMtx[1][0] = 0.0f;
        self->indMtx[1][1] = k;
        self->indMtx[1][2] = 0.0f;
        k = 0.01f + 0.4f * t;
        self->texTranslate.x += k;
        self->texTranslate.y += k;
        break;
    default:
        k = 0.1f * (0.2f + 0.8f * t);
        self->indMtx[0][0] = 0.0f;
        self->indMtx[0][1] = k * (0.2f + RAND_FLOAT());
        self->indMtx[0][2] = 0.0f;
        k = 0.05f * (0.5f + 0.5f * t);
        self->indMtx[1][0] = 0.0f;
        self->indMtx[1][1] = k * (0.2f + RAND_FLOAT());
        self->indMtx[1][2] = 0.0f;
        if (rand() & 1)
            self->indMtx[0][0] = -self->indMtx[0][0];
        if (rand() & 1)
            self->indMtx[0][1] = -self->indMtx[0][1];
        if (rand() & 1)
            self->indMtx[1][0] = -self->indMtx[1][0];
        if (rand() & 1)
            self->indMtx[1][1] = -self->indMtx[1][1];
        self->texTranslate.x += t * (RAND_FLOAT() - 0.5f);
        self->texTranslate.y += t * (RAND_FLOAT() - 0.5f);
        break;
    }
}

void fn_1_1509D8(struct SoftFxWarp *fx);
void fn_1_150E08(struct SoftFxWarp *fx);

void fn_1_1509AC(struct SoftFx *fx, int mask)
{
    switch (mask)
    {
    case 8:
        fn_1_1509D8((struct SoftFxWarp *)fx);
        break;
    }
}

void fn_1_1509D8(struct SoftFxWarp *fx)
{
    float vp[6];
    float proj[7];
    float rgba[4];
    GXColor color;
    struct SoftFxWarp *self;
    int w;
    int h;
    void *buf;
    float right;
    float left;
    float top;
    float bottom;
    float a;
    float k;

    self = fx;
    buf = self->texBuf;
    GXGetViewportv(vp);
    w = (int)vp[2] >> 1;
    h = (int)vp[3] >> 1;
    if (self->captured == 0)
    {
        GXSetTexCopySrc_cached((int)vp[0], (int)vp[1], (int)vp[2], (int)vp[3]);
        GXSetTexCopyDst_cached(w, h, 4, 1);
        GXCopyTex(buf, 0);
        GXInitTexObj(&self->texObj, buf, w, h, 4, 0, 0, 0);
        GXInvalidateTexAll();
        self->captured = 1;
    }
    fn_1_150E08(fx);
    fn_800668FC();
    g_something_with_GX_vtx_desc((1 << 9) | (1 << 13));
    GXGetProjectionv(proj);
    mtxa_from_translate_xyz(0.0f, 0.0f, -1.0f);
    GXLoadPosMtxImm(mtxa->mtxA, 0);
    switch (self->type)
    {
    case 0x31:
        a = 0.7f;
        break;
    default:
        a = 4.0f * (1.0f - self->unk20);
        break;
    }
    if (a < 0.0f)
        a = 0.0f;
    else if (a > 1.0f)
        a = 1.0f;
    color.r = 0xF0;
    color.g = 0xF4;
    color.b = 0xFF;
    rgba[3] = 224.0f * a;
    f32_to_u8(&rgba[3], &color.a);
    GXSetTevKColor_cached(0, color);
    k = 1.0f / proj[1];
    left = (-1.0f * (1.0f - proj[2])) * k;
    right = (-1.0f * (-1.0f - proj[2])) * k;
    k = 1.0f / proj[3];
    bottom = (-1.0f * (-1.0f - proj[4])) * k;
    top = (-1.0f * (1.0f - proj[4])) * k;
    GXBegin(0x80, 0, 4);
    {
        float y = bottom;
        GXPosition3f32(left, y, 0.0f);
    }
    GXTexCoord2f32(0.0f, 0.0f);
    {
        float x = right;
        GXPosition3f32(x, bottom, 0.0f);
    }
    GXTexCoord2f32(1.0f, 0.0f);
    {
        float y = top;
        GXPosition3f32(right, y, 0.0f);
    }
    GXTexCoord2f32(1.0f, 1.0f);
    GXPosition3f32(left, top, 0.0f);
    GXTexCoord2f32(0.0f, 1.0f);
    GXSetTexCopySrc_cached((int)vp[0], (int)vp[1], (int)vp[2], (int)vp[3]);
    GXSetTexCopyDst_cached(w, h, 4, 1);
    GXCopyTex(buf, 0);
    GXInitTexObj(&self->texObj, buf, w, h, 4, 0, 0, 0);
}

void fn_1_150E08(struct SoftFxWarp *fx)
{
    struct SoftFxWarp *self = fx;

    GXSetNumChans_cached(0);
    GXSetCullMode_cached(0);
    GXLoadTexObj_cached(&self->texObj, 0);
    GXLoadTexObj_cached(self->tex, 1);
    mtxa_from_translate(&self->texTranslate);
    mtxa_scale(&self->texScale);
    GXLoadTexMtxImm(mtxa->mtxA, 0x1E, 1);
    mtxa_from_translate_xyz(0.12f * (self->indMtx[0][0] + self->indMtx[0][1]),
                            0.22f * (self->indMtx[1][0] + self->indMtx[1][1]), 0.0f);
    GXLoadTexMtxImm(mtxa->mtxA, 0x21, 1);
    g_GXSetIndTexMtx_wrapper(1, self->indMtx, 0);
    GXSetTexCoordGen2_cached(0, 1, 4, 0x21, 0, 0x7D);
    g_GXSetTevOrder_wrapper(0, 0, 0, 0xFF);
    GXSetTevKColorSel_cached(0, 0xC);
    GXSetTevKAlphaSel_cached(0, 0x1C);
    GXSetTevColorIn_cached(0, 0xF, 8, 0xE, 0xF);
    GXSetTevColorOp_cached(0, 0, 0, 0, 1, 0);
    GXSetTevAlphaIn_cached(0, 7, 7, 7, 6);
    GXSetTevAlphaOp_cached(0, 0, 0, 0, 1, 0);
    GXSetTexCoordGen2_cached(1, 1, 4, 0x1E, 0, 0x7D);
    GXSetIndTexOrder_cached(0, 1, 1);
    GXSetTevIndirect_cached(0, 0, 0, 7, 1, 0, 0, 0, 0, 0);
    GXSetNumTevStages_cached(1);
    GXSetNumTexGens_cached(2);
    GXSetNumIndStages_cached(1);
    GXSetZMode_cached(1, 7, 0);
    g_something_with_raster_state(0);
    GXSetBlendMode_cached(1, 4, 5, 0);
}

void fn_1_1510B0(struct SoftFxLensFlare *fx);
void fn_1_151248(struct SoftFxLensFlare *fx);
void fn_1_1512AC(struct SoftFxLensFlare *fx);

int fn_1_15103C(struct SoftFx *fx, int msg)
{
    int r = 1;

    switch (msg)
    {
    case 0:
        fn_1_1510B0((struct SoftFxLensFlare *)fx);
        break;
    case 2:
        fn_1_1512AC((struct SoftFxLensFlare *)fx);
        break;
    case 3:
        fx->state = 3;
        break;
    case 4:
        fn_1_151248((struct SoftFxLensFlare *)fx);
        break;
    }
    return r;
}

void fn_1_1510B0(struct SoftFxLensFlare *fx)
{
    struct SoftFxLensFlare *self = fx;
    int i;
    Vec *p;
    struct GMAModel *model;
    float r;
    float t;

    fx->drawMask = 8;
    model = get_GmaBuffer_entry(stage_gma, "POD_RENZ_FREA_A");
    if (model == NULL)
        model = efcmdlGma->modelEntries[8].model;
    self->model = model;
    for (i = 0; i < 4; i++)
    {
        self->lists[i].count = 20;
        self->lists[i].queries = self->lists[i].items;
    }
    memset(self->vis, 0, sizeof(self->vis));
    r = 247.54787f;
    p = self->offsets;
    for (i = 20; i > 0; i--, p++)
    {
        mtxa_from_rotate_z(65536.0f * (4.0f * i / 20.0f));
        t = i / 20.0f;
        t = (0.3f * r) * t;
        mtxa_tf_vec_xyz(t, 0.0f, 0.0f, p);
    }
    fn_1_D68D4();
    world_theme.unk8 |= 1;
}

void fn_1_151248(struct SoftFxLensFlare *fx)
{
    int i;
    struct ZQueryList *list = fx->lists;

    for (i = 4; i > 0; i--, list++)
        fn_1_118210(list);
    world_theme.unk8 &= ~1;
}

void fn_1_1512AC(struct SoftFxLensFlare *fx)
{
    Vec pos;

    mtxa_from_mtx(fx->desc->mtx);
    mathutil_mtxA_get_translate(&pos);
    fn_1_D70EC(&pos);
    fn_1_D6920();
}

void fn_1_151334(struct SoftFxLensFlare *fx);

void fn_1_151308(struct SoftFx *fx, int mask)
{
    switch (mask)
    {
    case 8:
        fn_1_151334((struct SoftFxLensFlare *)fx);
        break;
    }
}

void fn_1_151334(struct SoftFxLensFlare *fx)
{
    Mtx m;
    float proj[7];
    Vec v;
    struct SoftFxLensFlare *self = fx;
    struct GMAModel *model;
    int i;
    Vec *o;
    struct ZQueryList *list;
    int rot;
    float *f;
    Vec *p;
    struct ZQuery *q;
    u32 z;
    struct SceneObjDesc *desc;
    float sx;
    float sy;
    float dist;
    float pk;
    float sum;
    float bright;
    float fade;
    float aspect;
    float c;

    desc = fx->desc;
    model = self->model;
    aspect = fn_1_15FA2C();
    mtxa_from_mtxb();
    mtxa_mult_right(desc->mtx);
    mtxa_translate(&model->boundSphereCenter);
    GXGetProjectionv(proj);
    pk = 1.0f / proj[3];
    mathutil_mtxA_get_translate(&v);
    if (v.z >= -0.1f)
        return;
    c = 1.0f / v.z;
    sx = proj[2] + c * (v.x * proj[1]);
    sy = proj[4] + c * (v.y * proj[3]);
    list = &self->lists[g_something_with_camera3];
    z = fn_1_118164(v.z + model->boundSphereRadius);
    q = list->queries;
    f = self->vis;
    for (i = 20; i > 0; i--, f++, q++)
    {
        if (q->z > z)
            *f = *f + 0.4f * (1.0f - *f);
        else
            *f = *f + 0.5f * (-0.1f - *f);
    }
    p = self->points;
    o = self->offsets;
    for (i = 20; i > 0; i--, p++, o++)
    {
        *p = v;
        p->x += o->x;
        p->y += o->y;
    }
    fn_1_117E94(20, self->points, list->queries);
    fn_1_1181E8(list);
    mtxa_sq_from_identity();
    mtxa_to_mtx(m);
    dist = -1.1f / v.z;
    mtxa_from_identity();
    mtxa_scale_s(dist);
    mtxa_mult_right(m);
    mathutil_mtxA_get_translate(&v);
    sum = 0.0f;
    f = self->vis;
    mtxa_push();
    for (i = 20; i > 0; i--, f++)
    {
        if (*f < 0.0f)
            continue;
        sum += *f;
    }
    sum *= 0.1f;
    sum -= 0.8f;
    bright = sum * (-v.z / mathutil_vec_len(&v));
    fade = bright;
    if (fade > 1.0f)
        fade = 1.0f;
    else if (fade < 0.0f)
        fade = 0.0f;
    fade = 1.0f - 0.7f * fade;
    avdisp_set_z_mode(1, 3, 0);
    rot = 24576.0f + 16384.0f * (0.7f * sx + sy);
    o = self->offsets;
    f = self->vis;
    mtxa_push();
    for (i = 20; i > 0; i--, f++, o++)
    {
        sum = *f;
        if (sum <= 0.0f)
            continue;
        mtxa_push();
        mtxa_translate(o);
        mtxa_rotate_z(rot + (i << 16) / 20);
        c = fade * (0.4f * sum);
        set_post_mult_color(c, c, c, 1.0f);
        mtxa_scale_s(aspect * (pk * ((0.2f * (1.0f + sum)) * -v.z)) / (model->boundSphereRadius * dist));
        g_smth_with_gpu_and_locked_cache_mtx();
        avdisp_draw_model_unculled_sort_never(model);
        mtxa_pop();
    }
    mtxa_pop();
    fade_color_base_default();
    avdisp_set_z_mode(1, 3, 1);
    sum = bright;
    rot = 24576.0f + 8192.0f * (sx + 0.5f * sy);
    if (sum > 0.0f)
    {
        avdisp_set_z_mode(1, 3, 0);
        mtxa_rotate_z(rot);
        c = sum;
        if (c > 1.0f)
            c = 1.0f;
        set_post_mult_color(c, c, c, 1.0f);
        mtxa_scale_s(aspect * ((4.0f * -v.z) * pk) / (model->boundSphereRadius * dist));
        g_smth_with_gpu_and_locked_cache_mtx();
        avdisp_draw_model_unculled_sort_never(model);
        set_post_mult_color(1.0f, 1.0f, 1.0f, 1.0f);
        avdisp_set_z_mode(1, 3, 1);
    }
    mtxa_pop();
}

// Letters of the CORE_BROKEN_* models
char *lbl_1_data_70624[26] = {
    "A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M",
    "N", "O", "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z",
};
struct GMAModel *lbl_1_bss_DD660[26];

int fn_1_1518A4(struct SoftFx *fx, int msg)
{
    int r = 1;

    switch (msg)
    {
    case 0:
        fn_1_151918(fx);
        break;
    case 2:
        fn_1_1519E8(fx);
        break;
    case 3:
        fx->state = 3;
        break;
    case 4:
        fn_1_1519E4(fx);
        break;
    }
    return r;
}

void fn_1_151918(struct SoftFx *fx)
{
    struct SoftFxCoreBroken *self = (struct SoftFxCoreBroken *)fx;
    int i;
    char buf[128];

    fx->drawMask = 8;
    self->phase = 0;
    {
        char **names = lbl_1_data_70624;
        struct GMAModel **models = lbl_1_bss_DD660;

        for (i = 26; i > 0; i--, names++, models++)
        {
            sprintf(buf, "CORE_BROKEN_%s", *names);
            *models = get_GmaBuffer_entry(stage_gma, buf);
        }
    }
    fn_1_151BFC(self);
    fn_1_1520C4(self);
    fn_1_152984(self);
}

void fn_1_1519E4(struct SoftFx *fx) {}

void fn_1_1519E8(struct SoftFx *fx)
{
    struct SoftFxCoreBroken *self = (struct SoftFxCoreBroken *)fx;
    struct CoreRocket e;
    float angle;
    int i;

    switch (self->phase)
    {
    case 0:
        self->phase = 1;
        self->timer = 108;
    case 1:
        self->timer--;
        if (self->timer < 0)
            self->phase = 2;
        break;
    case 2:
        self->phase = 3;
        self->timer = 7;
        memset(&e, 0, sizeof(e));
        e.scale = 0.2f;
        e.timer = 12;
        e.angleVel.x = 273.06668f;
        e.angleVel.y = 0.26666668f;
        angle = 65536.0f * RAND_FLOAT();
        for (i = 3; i > 0; i--)
        {
            e.angle.y = angle + 21845.334f * (float)i;
            fn_1_1528BC(self, &e);
        }
    case 3:
        self->timer--;
        if (self->timer < 0)
            self->phase = 4;
        break;
    case 4:
        self->phase = 5;
        fn_1_152094(self);
        break;
    case 5:
        break;
    case 6:
        break;
    }
    fn_1_151CC4(self);
    fn_1_1520C8(self);
    fn_1_152988(self);
    self->counter++;
}

void fn_1_151BBC(struct SoftFx *fx, int mask)
{
    struct SoftFxCoreBroken *self = (struct SoftFxCoreBroken *)fx;

    fn_1_151FD4(self);
    fn_1_152A70(self);
    fn_1_152664(self);
}

void fn_1_151BFC(struct SoftFxCoreBroken *self)
{
    int i;
    struct CoreBrokenPiece *piece;

    self->count = 26;
    piece = self->pieces;
    {
        struct GMAModel **models = lbl_1_bss_DD660;

        for (i = self->count; i > 0; i--, piece++, models++)
        {
            struct GMAModel *m = *models;

            piece->active = 0;
            piece->model = m;
            if (m != NULL)
                piece->pos = m->boundSphereCenter;
            piece->dir.x = piece->pos.x;
            piece->dir.y = piece->pos.y - 101.5f;
            piece->dir.z = piece->pos.z;
            vec_normalize_len(&piece->dir);
        }
    }
}

void fn_1_151CC4(struct SoftFxCoreBroken *self)
{
    int i;
    struct CoreBrokenPiece *piece = self->pieces;

    for (i = self->count; i > 0; i--, piece++)
    {
        if (piece->active == 0)
            continue;
        switch (piece->state)
        {
        case 0:
            piece->vel.y += -0.004f;
            break;
        case 1:
            piece->state = 2;
            piece->timer = 30.0f * math_sqrt(mathutil_sum_of_sq_2(piece->pos.x, piece->pos.z));
        case 2:
            piece->timer--;
            if (piece->timer < 0)
            {
                float k = 0.05f * RAND_FLOAT();

                piece->vel.x += piece->dir.x * k;
                piece->vel.y += piece->dir.y * k;
                piece->vel.z += piece->dir.z * k;
                piece->rotVel.x += 2048.0f * (RAND_FLOAT() - 0.5f);
                piece->rotVel.y += 2048.0f * (RAND_FLOAT() - 0.5f);
                piece->rotVel.z += 2048.0f * (RAND_FLOAT() - 0.5f);
                piece->state = 0;
            }
            break;
        }
        piece->vel.x *= 0.98f;
        piece->vel.y *= 0.98f;
        piece->vel.z *= 0.98f;
        piece->pos.x += piece->vel.x;
        piece->pos.y += piece->vel.y;
        piece->pos.z += piece->vel.z;
        piece->rotVel.x *= 0.98f;
        piece->rotVel.y *= 0.98f;
        piece->rotVel.z *= 0.98f;
        piece->rot.x += (s16)piece->rotVel.x;
        piece->rot.y += (s16)piece->rotVel.y;
        piece->rot.z += (s16)piece->rotVel.z;
    }
}

void fn_1_151FD4(struct SoftFxCoreBroken *self)
{
    int i;
    struct CoreBrokenPiece *piece = self->pieces;

    for (i = self->count; i > 0; i--, piece++)
    {
        struct GMAModel *model;

        if (piece->active == 0)
            continue;
        model = piece->model;
        if (model == NULL)
            continue;
        mtxa_from_mtxb_translate(&piece->pos);
        mtxa_rotate_y(piece->rot.y);
        mtxa_rotate_x(piece->rot.x);
        mtxa_rotate_z(piece->rot.z);
        mtxa_translate_neg(&model->boundSphereCenter);
        if (g_is_sphere_visible(&model->boundSphereCenter, model->boundSphereRadius))
        {
            load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
            avdisp_draw_model_unculled_sort_never(model);
        }
    }
}

void fn_1_152094(struct SoftFxCoreBroken *self)
{
    struct CoreBrokenPiece *piece = self->pieces;
    int i = self->count;

    for (; i > 0; i--, piece++)
    {
        piece->active = 1;
        piece->state = 1;
    }
}

void fn_1_1520C4(struct SoftFxCoreBroken *self) {}

void fn_1_1520C8(struct SoftFxCoreBroken *self)
{
    struct CoreRocket child;
    struct CoreSpark spark;
    Vec angle;
    Vec vel;
    Vec oldPos;
    Vec v;
    int i;
    struct CoreRocket *r = self->rockets;
    float n;
    float t;
    float u;
    float total;
    float ax;

    for (i = 64; i > 0; i--, r++)
    {
        if (r->active == 0)
            continue;
        angle = r->angle;
        vel = r->angleVel;
        r->angle.x += r->angleVel.x;
        r->angle.y += r->angleVel.y;
        n = 0.25f * r->scale;
        r->offset.x = n * (RAND_FLOAT() - 0.5f);
        r->offset.y = n * (RAND_FLOAT() - 0.5f);
        r->offset.z = n * (RAND_FLOAT() - 0.5f);
        mtxa_from_translate_xyz(0.0f, 101.5f, 0.0f);
        mtxa_rotate_y(r->angle.y);
        mtxa_rotate_x(r->angle.x);
        oldPos = r->pos;
        mtxa_tf_point_xyz(0.0f, -4.5f, 0.0f, &r->pos);
        mtxa_tf_vec_xyz(0.0f, -1.0f, 0.0f, &v);
        vec_to_euler_xy(&v, &r->rotX, &r->rotY);
        r->vel.x = r->pos.x - oldPos.x;
        r->vel.y = r->pos.y - oldPos.y;
        r->vel.z = r->pos.z - oldPos.z;
        r->timer--;
        if (r->timer < 0)
        {
            r->scale *= 0.9f;
            memcpy(&child, r, sizeof(child));
            r->timer = 60.0f * (0.125f + 0.125f * RAND_FLOAT());
            child.timer = 60.0f * (0.125f + 0.125f * RAND_FLOAT());
            v.x = __fabs(r->angleVel.y);
            v.y = 1.25f * r->angleVel.x;
            r->angleVel.x = v.x;
            r->angleVel.y = v.y;
            child.angleVel.x = v.x;
            child.angleVel.y = -v.y;
            fn_1_1528BC(self, &child);
        }
        n = r->trail + (2.0f * mathutil_vec_distance(&r->pos, &oldPos)) / r->scale;
        if (n > 1.0f)
        {
            memset(&spark, 0, sizeof(spark));
            total = n;
            while (n > 1.0f)
            {
                t = n / total;
                u = 1.0f - t;
                spark.life = 60.0f * (0.2f + 0.05f * RAND_FLOAT());
                ax = r->angle.x * t + angle.x * u;
                mtxa_from_rotate_y(r->angle.y * t + angle.y * u);
                mtxa_rotate_x(ax);
                mtxa_tf_vec_xyz(0.0f, -1.0f, 0.0f, &v);
                spark.pos.x = 4.5f * v.x;
                spark.pos.y = 101.5f + 4.5f * v.y;
                spark.pos.z = 4.5f * v.z;
                vec_to_euler_xy(&v, &spark.rotX, &spark.rotY);
                spark.rotX += 0x8000;
                spark.x1C = 1.0f;
                spark.fade = 1.0f;
                spark.scale = r->scale * (0.5f + RAND_FLOAT());
                fn_1_152B8C(self, &spark);
                n -= 1.0f;
            }
        }
        r->trail = n;
    }
}

void fn_1_152664(struct SoftFxCoreBroken *self)
{
    int i;
    struct CoreRocket *r;
    struct GMAModel *body;
    struct GMAModel *glow;
    float s;
    float k;
    Vec t;

    glow = efcmdlGma->modelEntries[5].model;
    body = efcmdlGma->modelEntries[6].model;
    avdisp_set_z_mode(1, 3, 0);
    set_post_mult_color(0.75f, 0.6f, 0.3f, 1.0f);
    r = self->rockets;
    for (i = 64; i > 0; i--, r++)
    {
        if (r->active == 0)
            continue;
        s = r->scale;
        mtxa_from_mtxb_translate(&r->pos);
        mtxa_translate(&r->offset);
        mtxa_push();
        mtxa_rotate_y(r->rotY);
        mtxa_rotate_x(r->rotX);
        mtxa_scale_s(s / body->boundSphereRadius);
        if (g_is_sphere_visible(&body->boundSphereCenter, s))
        {
            load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
            avdisp_draw_model_unculled_sort_never(body);
        }
        mtxa_pop();
        mtxa_push();
        mtxa_sq_from_identity();
        mtxa_scale_s(s / glow->boundSphereRadius);
        if (g_is_sphere_visible(&glow->boundSphereCenter, s))
        {
            mathutil_mtxA_get_translate(&t);
            if (t.z < 0.0f)
            {
                if (t.z < -(0.2f + s))
                {
                    k = (t.z + s) / t.z;
                    t.x *= k;
                    t.y *= k;
                    t.z = t.z + s;
                }
                else
                {
                    k = -0.2f / t.z;
                    t.x *= k;
                    t.y *= k;
                    t.z = -0.2f;
                }
                mathutil_mtxA_set_translate(&t);
                mtxa_scale_s(k);
                load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
                avdisp_draw_model_unculled_sort_never(glow);
            }
        }
        mtxa_pop();
    }
    fade_color_base_default();
    avdisp_set_z_mode(1, 3, 1);
}

void fn_1_1528BC(struct SoftFxCoreBroken *self, struct CoreRocket *src)
{
    struct CoreRocket *r = &self->rockets[self->rocketIdx];

    self->rocketIdx--;
    if (self->rocketIdx < 0)
        self->rocketIdx += 64;
    memcpy(r, src, sizeof(*r));
    r->active = 1;
    mtxa_from_translate_xyz(0.0f, 101.5f, 0.0f);
    mtxa_rotate_y(r->angle.y);
    mtxa_rotate_x(r->angle.x);
    mtxa_tf_point_xyz(0.0f, -4.5f, 0.0f, &r->pos);
}

void fn_1_152984(struct SoftFxCoreBroken *self) {}

void fn_1_152988(struct SoftFxCoreBroken *self)
{
    int i;
    struct CoreSpark *s = self->sparks;

    for (i = 512; i > 0; i--, s++)
    {
        if (s->life == 0)
            continue;
        s->x1C *= 0.1f;
        if (s->life < 7.5f)
        {
            float t = 1.0f / (s->life + 1);

            s->fade *= 1.0f - t;
        }
        else
            s->fade += 0.2f * (1.0f - s->fade);
        s->life--;
    }
}

void fn_1_152A70(struct SoftFxCoreBroken *self)
{
    int i;
    struct CoreSpark *s;
    struct GMAModel *model;
    float scale;

    avdisp_set_z_mode(1, 3, 0);
    model = efcmdlGma->modelEntries[5].model;
    s = self->sparks;
    for (i = 512; i > 0; i--, s++)
    {
        if (s->life == 0)
            continue;
        scale = s->scale * s->fade;
        mtxa_from_mtxb_translate(&s->pos);
        mtxa_rotate_y(s->rotY);
        mtxa_rotate_x(s->rotX);
        mtxa_scale_s(scale / model->boundSphereRadius);
        if (g_is_sphere_visible(&model->boundSphereCenter, scale))
        {
            set_post_mult_color(1.0f, 0.8f, 0.4f, 1.0f);
            g_smth_with_gpu_and_locked_cache_mtx();
            avdisp_draw_model_unculled_sort_never(model);
        }
    }
    fade_color_base_default();
    avdisp_set_z_mode(1, 3, 1);
}

void fn_1_152B8C(struct SoftFxCoreBroken *self, struct CoreSpark *src)
{
    struct CoreSpark *s = &self->sparks[self->sparkIdx];

    self->sparkIdx--;
    if (self->sparkIdx < 0)
        self->sparkIdx += 512;
    memcpy(s, src, sizeof(*s));
}
