#ifndef STAGE_H
#define STAGE_H

#include "types.h"
#include "mathutil.h"

extern s16 current_stage_id;
extern s16 world_theme;

struct StartPos
{
    /*0x00*/ Vec pos;
    /*0x0C*/ s16 xrot;
    /*0x0E*/ s16 yrot;
    /*0x10*/ s16 zrot;
};

struct StageGoal
{
    /*0x00*/ Vec pos;
    /*0x0C*/ s16 rotX;
    /*0x0E*/ s16 rotY;
    /*0x10*/ s16 rotZ;
    /*0x12*/ s8 type;
    /*0x13*/ u8 flags;  /* 1: casts a shadow */
}; /* size = 0x14 */

/* Box (unit cube scaled, then rotated) that counts as falling out. */
struct FalloutVolume
{
    /*0x00*/ Vec pos;
    /*0x0C*/ Vec scale;
    /*0x18*/ s16 rotX;
    /*0x1A*/ s16 rotY;
    /*0x1C*/ s16 rotZ;
    u8 unk1E[2];
}; /* size = 0x20 */

struct StageWormhole
{
    /*0x00*/ u32 unk0;
    /*0x04*/ Vec pos;
    /*0x10*/ s16 rotX;
    /*0x12*/ s16 rotY;
    /*0x14*/ s16 rotZ;
    u8 unk16[2];
    /*0x18*/ struct StageWormhole * DPTR dest;
}; /* size = 0x1C */

/* Collision triangle, stored in its own plane space (SMB1 layout). */
struct StageColiTri
{
    /*0x00*/ Vec pos;       /* vertex 1 in item group space */
    /*0x0C*/ Vec normal;
    /*0x18*/ S16Vec rot;    /* triangle space -> item group space */
    /*0x1E*/ u16 flags;
    /*0x20*/ Vec2d vert2;   /* vertex 2 in triangle space (vertex 1 is the origin) */
    /*0x28*/ Vec2d vert3;
    /*0x30*/ Vec2d edge2Normal;  /* edge 1's normal is (0, 1) */
    /*0x38*/ Vec2d edge3Normal;
};  /* size = 0x40 */

struct StageColiCone
{
    /*0x00*/ Vec pos;
    /*0x0C*/ S16Vec rot;
    /*0x12*/ u16 flags;
    /*0x14*/ Vec scale;
};  /* size = 0x20 */

struct StageColiSphere
{
    /*0x00*/ Vec pos;
    /*0x0C*/ float radius;
    /*0x10*/ u16 flags;
};  /* size = 0x14 */

struct StageColiCylinder
{
    /*0x00*/ Vec pos;
    /*0x0C*/ float radius;
    /*0x10*/ float height;
    /*0x14*/ S16Vec rot;
    /*0x1A*/ u16 flags;
};  /* size = 0x1C */

/* Per-item-group animation: a keyframe list per channel. */
struct StageItemGroupAnim
{
    /*0x00*/ s32 rotXKeyframeCount;
    /*0x04*/ struct StageKeyframe * DPTR rotXKeyframes;
    /*0x08*/ s32 rotYKeyframeCount;
    /*0x0C*/ struct StageKeyframe * DPTR rotYKeyframes;
    /*0x10*/ s32 rotZKeyframeCount;
    /*0x14*/ struct StageKeyframe * DPTR rotZKeyframes;
    /*0x18*/ s32 posXKeyframeCount;
    /*0x1C*/ struct StageKeyframe * DPTR posXKeyframes;
    /*0x20*/ s32 posYKeyframeCount;
    /*0x24*/ struct StageKeyframe * DPTR posYKeyframes;
    /*0x28*/ s32 posZKeyframeCount;
    /*0x2C*/ struct StageKeyframe * DPTR posZKeyframes;
};

/* Collision data of one item group. */
/* One level of detail; used when the model's projected size exceeds minSize. */
struct StageModelInfo
{
    u8 unk0[4];
    /*0x04*/ char * DPTR name;
    /*0x08*/ struct GMAModel * DPTR model;
    /*0x0C*/ float minSize;
};  /* size = 0x10 */

struct StageModelRef
{
    /*0x00*/ u32 flags;  /* 2: shadow type 4, 8: drawn in the translucent pass, 0x10: reflective */
    /*0x04*/ s32 lodCount;
    /*0x08*/ struct StageModelInfo * DPTR lods;
};

/* A placed copy of a stage model. */
struct StageModelInstance
{
    /*0x00*/ struct StageModelRef * DPTR modelRef;
    /*0x04*/ Vec pos;
    /*0x10*/ s16 rotX;
    /*0x12*/ s16 rotY;
    /*0x14*/ s16 rotZ;
    /*0x16*/ u16 flags;  /* hidden when it overlaps g_smth_with_challenge_mode_var_and_3p() */
    /*0x18*/ Vec scale;
}; /* size = 0x24 */

struct StageKeyframe;

struct StageKeyframeChannel
{
    /*0x00*/ s32 count;
    /*0x04*/ struct StageKeyframe * DPTR frames;
};

/* Animated debug marker drawn with init_common_gma model 35. */
struct StageGenerator
{
    /*0x00*/ char * DPTR name;
    /*0x04*/ float loopStart;  /* seconds */
    /*0x08*/ float loopEnd;
    /*0x0C*/ Vec pos;
    /*0x18*/ s16 rotX;
    /*0x1A*/ s16 rotY;
    /*0x1C*/ s16 rotZ;
    u8 unk1E[2];
    /*0x20*/ struct StageKeyframeChannel scaleX;
    /*0x28*/ struct StageKeyframeChannel scaleY;
    /*0x30*/ struct StageKeyframeChannel scaleZ;
    /*0x38*/ struct StageKeyframeChannel unk38;
    /*0x40*/ struct StageKeyframeChannel red;
    /*0x48*/ struct StageKeyframeChannel green;
    /*0x50*/ struct StageKeyframeChannel blue;
    /*0x58*/ struct StageKeyframeChannel alpha;
    u8 unk60[0x70 - 0x60];
}; /* size = 0x70 */

struct StageCollHeader
{
    /*0x00*/ Vec initPos;
    /*0x0C*/ S16Vec initRot;
    /*0x12*/ u16 animLoopType;  /* bit 0: play once (no looping) */
    /*0x14*/ struct StageItemGroupAnim * DPTR anim;
    /*0x18*/ Vec conveyorVel;
    /*0x24*/ struct StageColiTri * DPTR triangles;
    /*0x28*/ s16 * DPTR * DPTR gridCellTris;
    /*0x2C*/ float gridOriginX;
    /*0x30*/ float gridOriginZ;
    /*0x34*/ float gridStepX;
    /*0x38*/ float gridStepZ;
    /*0x3C*/ s32 gridCellCountX;
    /*0x40*/ s32 gridCellCountZ;
    /*0x44*/ s32 goalCount;
    /*0x48*/ struct StageGoal * DPTR goals;
    /*0x4C*/ s32 bumperCount;
    /*0x50*/ void * DPTR bumpers;
    /*0x54*/ s32 jamabarCount;
    /*0x58*/ void * DPTR jamabars;
    /*0x5C*/ s32 bananaCount;
    /*0x60*/ void * DPTR bananas;
    /*0x64*/ s32 coliConeCount;
    /*0x68*/ struct StageColiCone * DPTR coliCones;
    /*0x6C*/ s32 coliSphereCount;
    /*0x70*/ struct StageColiSphere * DPTR coliSpheres;
    /*0x74*/ s32 coliCylinderCount;
    /*0x78*/ struct StageColiCylinder * DPTR coliCylinders;
    /*0x7C*/ s32 falloutVolumeCount;
    /*0x80*/ struct FalloutVolume * DPTR falloutVolumes;
    /*0x84*/ s32 reflectiveCount;
    /*0x88*/ struct StageReflectiveModel * DPTR reflectives;
    /*0x8C*/ s32 instanceCount;
    /*0x90*/ struct StageModelInstance * DPTR instances;
    /*0x94*/ s32 modelRefCount;
    /*0x98*/ struct StageModelRef * DPTR * DPTR modelRefs;
    /*0x9C*/ s32 generatorCount;
    /*0xA0*/ struct StageGenerator * DPTR generators;
    /*0xA4*/ s16 animGroupId;
    /*0xA6*/ u8 unkA6;  /* per-player mask: skip collision for players whose bit is set */
    /*0xA7*/ u8 unkA7;  /* per-player mask: skip drawing for players whose bit is set */
    /*0xA8*/ s32 buttonCount;
    /*0xAC*/ void * DPTR buttons;
    u8 unkB0[4];
    /*0xB4*/ struct Sphere * DPTR boundSphere;  /* NULL if the item group has none */
    /*0xB8*/ float seesawSensitivity;
    /*0xBC*/ float seesawFriction;  /* "reset stiffness" */
    /*0xC0*/ float seesawSpring;  /* "rotation bounds" */
    /*0xC4*/ s32 wormholeCount;
    /*0xC8*/ struct StageWormhole * DPTR wormholes;
    /*0xCC*/ u32 initPlaybackState;
    /*0xD0*/ float animLoopStart;  /* seconds */
    /*0xD4*/ float animLoopEnd;
    /*0xD8*/ void * DPTR textureScroll;
    u8 unkDC[0x49C - 0xDC];
}; /* size = 0x49C */

struct StageFog
{
    /*0x00*/ u8 type;
};

/* One key of a stagedef animation channel (SMB1's struct Keyframe). */
struct StageKeyframe
{
    /*0x00*/ s32 easeType;  /* 0: step, 1: linear, 2: Hermite */
    /*0x04*/ float time;
    /*0x08*/ float value;
    /*0x0C*/ float tangentIn;
    /*0x10*/ float tangentOut;
}; /* size = 0x14 */


struct StageFogAnim
{
    /*0x00*/ struct StageKeyframeChannel start;
    /*0x08*/ struct StageKeyframeChannel end;
    /*0x10*/ struct StageKeyframeChannel red;
    /*0x18*/ struct StageKeyframeChannel green;
    /*0x20*/ struct StageKeyframeChannel blue;
    /*0x28*/ struct StageKeyframeChannel unk28;
};

/* Animated direction of the stage's directional light. */
struct StageLightAnim
{
    u8 unk0[8];
    /*0x08*/ struct StageKeyframeChannel dirX;
    /*0x10*/ struct StageKeyframeChannel dirY;
    /*0x18*/ struct StageKeyframeChannel dirZ;
    u8 unk20[0xA0 - 0x20];
}; /* size = 0xA0 */

struct StageReflectiveModel
{
    /*0x00*/ char * DPTR modelName;
    /*0x04*/ struct GMAModel * DPTR model;
    u8 unk8[0xC - 8];
}; /* size = 0xC */

/* Mirror plane for the flat reflection effect. */
struct StageReflectionPlane
{
    /*0x00*/ u32 flags;
    /*0x04*/ Vec pos;
    /*0x10*/ S16Vec rot;
    /*0x16*/ s16 itemgroupIdx;
    /*0x18*/ u16 drawMask;
    /*0x1A*/ s8 resShift;
    /*0x1B*/ s8 texFormat;
};

/* Twelve keyframe channels; only the eleven below hold pointers the loader fixes up. */
struct StageBgAnim
{
    /*0x00*/ float loopStart;  /* seconds */
    /*0x04*/ float loopEnd;
    /*0x08*/ struct StageKeyframeChannel scaleX;
    /*0x10*/ struct StageKeyframeChannel scaleY;
    /*0x18*/ struct StageKeyframeChannel scaleZ;
    /*0x20*/ struct StageKeyframeChannel rotX;
    /*0x28*/ struct StageKeyframeChannel rotY;
    /*0x30*/ struct StageKeyframeChannel rotZ;
    /*0x38*/ struct StageKeyframeChannel posX;
    /*0x40*/ struct StageKeyframeChannel posY;
    /*0x48*/ struct StageKeyframeChannel posZ;
    /*0x50*/ struct StageKeyframeChannel visible;
    /*0x58*/ struct StageKeyframeChannel translucency;
};

struct StageNightWindowAnim
{
    /*0x00*/ Vec pos;
    /*0x0C*/ s16 rotX;
    /*0x0E*/ s16 rotY;
    /*0x10*/ s16 rotZ;
    /*0x12*/ u8 id;
    u8 filler13[1];
}; /* size = 0x14 */

struct StageStormFireAnim
{
    /*0x00*/ Vec pos;
    /*0x0C*/ u8 frameOffset;
    u8 fillerD[3];
}; /* size = 0x10 */

struct StageEffectHeader
{
    /*0x00*/ s32 nightWindowAnimCount;
    /*0x04*/ struct StageNightWindowAnim * DPTR fx1Keyframes;
    /*0x08*/ s32 stormFireAnimCount;
    /*0x0C*/ struct StageStormFireAnim * DPTR fx2Keyframes;
    /*0x10*/ void * DPTR textureScroll;
};

/* Background or foreground model. */
/* Background spectator monkey (bg_guest.c) */
struct StageBgGuest
{
    /*0x00*/ Vec pos;
    /*0x0C*/ s16 rotX;
    /*0x0E*/ s16 rotY;
    /*0x10*/ s16 rotZ;
    /*0x12*/ s8 colorIdx;
    /*0x13*/ s8 apeIdx;
    /*0x14*/ float scale;
    u8 unk18[0x20 - 0x18];
};  /* size = 0x20 */

struct StageBgModel
{
    /*0x00*/ u32 flags;
    /*0x04*/ char * DPTR name;
    /*0x08*/ struct GMAModel * DPTR model;
    /*0x0C*/ Vec pos;
    /*0x18*/ s16 rotX;
    /*0x1A*/ s16 rotY;
    /*0x1C*/ s16 rotZ;
    /*0x1E*/ u16 animFlags;  /* top two bits select a world_theme timer */
    /*0x20*/ Vec scale;
    /*0x2C*/ float translucency;
    /*0x30*/ struct StageBgAnim * DPTR anim;
    /*0x34*/ struct StageEffectHeader * DPTR effect;
}; /* size = 0x38 */

struct StageExtraChannels
{
    /*0x00*/ struct StageKeyframeChannel a;
    /*0x08*/ struct StageKeyframeChannel b;
    /*0x10*/ struct StageKeyframeChannel c;
}; /* size = 0x18 */

struct StageExtraData
{
    /*0x00*/ struct StageExtraChannels channels;
    /*0x18*/ s32 listCount;
    /*0x1C*/ struct StageExtraChannels * DPTR list;
    /*0x20*/ struct StageKeyframeChannel unk20;
    /*0x28*/ struct StageKeyframeChannel unk28;
    /*0x30*/ struct StageKeyframeChannel unk30;
    /*0x38*/ struct StageKeyframeChannel unk38;
    /*0x40*/ struct StageKeyframeChannel unk40;
    /*0x48*/ struct StageKeyframeChannel unk48;
    /*0x50*/ struct StageKeyframeChannel unk50;
    /*0x58*/ struct StageKeyframeChannel unk58;
    /*0x60*/ struct StageKeyframeChannel unk60;
};

struct StageExtraAnims
{
    /*0x00*/ Vec cupPos;  /* golf stages: position of the cup */
    /*0x0C*/ struct StageItemGroupAnim * DPTR animA;
    /*0x10*/ struct StageItemGroupAnim * DPTR animB;
};

struct StageDef
{
    /*0x00*/ float loopStart;  /* seconds */
    /*0x04*/ float loopEnd;
    /*0x08*/ s32 itemGroupCount;
    /*0x0C*/ struct StageCollHeader * DPTR collHeaders;  /* one per item group */
    /*0x10*/ struct StartPos * DPTR startPos;
    /*0x14*/ float * DPTR falloutY;
    /*0x18*/ s32 goalCount;
    /*0x1C*/ struct StageGoal * DPTR goals;
    /*0x20*/ s32 bumperCount;
    /*0x24*/ void * DPTR bumpers;
    /*0x28*/ s32 jamabarCount;
    /*0x2C*/ void * DPTR jamabars;
    /*0x30*/ s32 bananaCount;
    /*0x34*/ void * DPTR bananas;
    /*0x38*/ s32 coliConeCount;
    /*0x3C*/ struct StageColiCone * DPTR coliCones;
    /*0x40*/ s32 coliSphereCount;
    /*0x44*/ struct StageColiSphere * DPTR coliSpheres;
    /*0x48*/ s32 coliCylinderCount;
    /*0x4C*/ struct StageColiCylinder * DPTR coliCylinders;
    /*0x50*/ s32 falloutVolumeCount;
    /*0x54*/ struct FalloutVolume * DPTR falloutVolumes;
    /*0x58*/ s32 bgModelCount;
    /*0x5C*/ struct StageBgModel * DPTR bgModels;
    /*0x60*/ s32 fgModelCount;
    /*0x64*/ struct StageBgModel * DPTR fgModels;
    /*0x68*/ struct StageExtraData * DPTR extraData;
    u8 unk6C[0x70 - 0x6C];
    /*0x70*/ s32 reflectiveCount;
    /*0x74*/ struct StageReflectiveModel * DPTR reflectives;
    /*0x78*/ struct StageExtraAnims * DPTR extraAnims;
    /*0x7C*/ s32 unk7C;
    /*0x80*/ void * DPTR unk80;
    /*0x84*/ s32 instanceCount;
    /*0x88*/ struct StageModelInstance * DPTR instances;
    /*0x8C*/ s32 modelListCount;
    /*0x90*/ struct StageModelRef * DPTR modelList;
    /*0x94*/ s32 modelRefCount;
    /*0x98*/ struct StageModelRef * DPTR * DPTR modelRefs;
    /*0x9C*/ s32 generatorCount;
    /*0xA0*/ struct StageGenerator * DPTR generators;
    /*0xA4*/ s8 bgTheme;
    u8 unkA5[0xA8 - 0xA5];
    /*0xA8*/ s32 buttonCount;
    /*0xAC*/ void * DPTR buttons;
    /*0xB0*/ struct StageFogAnim * DPTR fogAnim;
    /*0xB4*/ s32 wormholeCount;
    /*0xB8*/ struct StageWormhole * DPTR wormholes;
    /*0xBC*/ struct StageFog * DPTR fog;
    /*0xC0*/ s32 bgGuestCount;
    /*0xC4*/ struct StageBgGuest * DPTR bgGuests;
    /*0xC8*/ struct StageReflectionPlane * DPTR dynamicReflectionPlane;
    /*0xCC*/ s32 lightAnimCount;
    /*0xD0*/ struct StageLightAnim * DPTR lightAnim;
    /*0xD4*/ void * DPTR unkD4;
};

struct SeesawColiInfo;

/* Item group object driven through a table of handlers (see lbl_1_data_2EF80). */
struct Seesaw
{
    /*0x00*/ s16 itemGroupId;
    /*0x02*/ s16 type;  /* index into lbl_1_data_2EF80 */
    /*0x04*/ void *state;
    /*0x08*/ void (*init)(struct Seesaw *);
    /*0x0C*/ void (*dest)(struct Seesaw *);
    /*0x10*/ void (*tick)(struct Seesaw *);
    /*0x14*/ void (*coli)(struct Seesaw *, struct SeesawColiInfo *);
    /*0x18*/ void (*func5)(struct Seesaw *, void *);
    /*0x1C*/ void (*replay)(struct Seesaw *, int replay, float time);
};

struct SeesawFuncs
{
    s32 stateSize;
    s32 replayStateSize;
    void (*init)(struct Seesaw *);
    void (*dest)(struct Seesaw *);
    void (*tick)(struct Seesaw *);
    void (*coli)(struct Seesaw *, struct SeesawColiInfo *);
    void (*func5)(struct Seesaw *, void *);
    void (*replay)(struct Seesaw *, int replay, float time);
};

extern struct SeesawFuncs lbl_1_data_2EF80[];
extern s16 seesaw_count;

/* Moving parts of a stage (SMB1's animGroups). */
struct ItemGroup
{
    /*0x00*/ u32 playbackState;  /* low 3 bits: 0 play, 1 pause, 2 reverse, 3 fast forward, 4 rewind */
    /*0x04*/ s32 animFrame;
    /*0x08*/ Vec pos;
    /*0x14*/ Vec prevPos;
    /*0x20*/ S16Vec rot;
    /*0x26*/ S16Vec prevRot;
    /*0x2C*/ Mtx transform;
    /*0x5C*/ Mtx prevTransform;
    /*0x8C*/ Vec boundSphereCenter;
    /*0x98*/ float boundSphereRadius;
    /*0x9C*/ struct Seesaw *seesaw;
}; /* size = 0xA0 */

extern struct ItemGroup *itemgroups;

/* Stage parts whose geometry is animated in code (SMB1 DynamicStagePart). */
struct NlVtxTypeA;
struct NlVtxTypeB;
struct NlModel;

struct DynamicStagePart
{
    /*0x00*/ char *modelName;
    /*0x04*/ struct NlModel *origModel;
    /*0x08*/ void (*posNrmTexFunc)(struct NlVtxTypeB *);
    /*0x0C*/ void (*posColorTexFunc)(struct NlVtxTypeA *);
    /*0x10*/ int (*raycastDownFunc)(Vec *rayOrigin, Vec *outHitPos, Vec *outHitNormal);
    /*0x14*/ void *tempModel;
};

extern struct StageDef *stagedef;
extern struct DynamicStagePart *g_some_draw_var;  /* SMB1's dynamicStageParts */
extern struct GMA *stage_gma;

/* SMB1's lbl_80206DEC */
struct StageState
{
    /*0x00*/ s32 frameCounter;
    /*0x04*/ float timer;  /* frames */
    /*0x08*/ void (*callback)(void);
    /*0x0C*/ u8 unkC;  /* current player number (1-based) */
    /*0x0D*/ u8 unkD;
    u8 unkE[0x1C - 0xE];
    /*0x1C*/ u32 flags;  /* 1: stop the clock, 4: freeze item group animation, 8: stage has translucent models */
    u8 unk20[0x5C - 0x20];
}; /* size = 0x5C */

extern struct StageState g_some_frame_counter;

struct GMAModel *g_some_draw_func2(struct StageModelRef *ref);
struct GMAModel *g_some_draw_func4(struct StageModelRef *ref, float scale);

struct Sphere
{
    Vec pos;
    float radius;
};

struct ColiRect
{
    /*0x00*/ Vec pos;
    /*0x0C*/ S16Vec rot;
    /*0x12*/ u16 flags;  /* copied to the collision plane */
    /*0x14*/ Vec normal;
    /*0x20*/ float width;
    /*0x24*/ float height;
}; /* size = 0x28 */

struct ColiPlane
{
    /*0x00*/ Vec point;
    /*0x0C*/ Vec normal;
    /*0x18*/ u16 flags;  /* copied from the source triangle */
};  /* size = 0x1C */

struct ColiEdge
{
    Vec2d start;
    Vec2d end;
    Vec2d normal;  /* coplanar with the triangle, points inside */
};

struct ColiCircle
{
    Vec pos;
    float radius;
    S16Vec rot;
};

/* The ball's state as seen by the collision code, in one item group's local space. */
struct PhysicsBall
{
    /*0x00*/ u32 flags;
    /*0x04*/ Vec pos;
    /*0x10*/ Vec prevPos;
    /*0x1C*/ Vec vel;
    /*0x28*/ float radius;
    /*0x2C*/ float gravityAccel;
    /*0x30*/ float restitution;
    /*0x34*/ float hardestColiSpeed;
    /*0x38*/ Vec hardestColiPoint;
    /*0x44*/ Vec hardestColiNormal;
    u8 unk50[4];
    /*0x54*/ s32 hardestColiItemGroupId;
    /*0x58*/ float friction;
    /*0x5C*/ s32 itemGroupId;
};  /* size = 0x60 */

struct Ball;
void init_physicsball_from_ball(struct Ball *ball, struct PhysicsBall *physBall);
void collide_ball_with_stage(struct PhysicsBall *physBall, struct StageDef *stagedef);
void collide_ball_with_seesaws(int a, int playerId, struct PhysicsBall *physBall);
void apply_physicsball_to_ball(struct Ball *ball, struct PhysicsBall *physBall);
void tf_physball_to_itemgroup_space(struct PhysicsBall *physBall, int itemGroupId);
BOOL line_intersects_rect(Vec *lineStart, Vec *lineEnd, struct ColiRect *rect);
void mtxa_rigid_inv_tf_point(Vec *src, Vec *dst);

float evaluate_stagedef_keyframe(float time, int count, struct StageKeyframe *frames);

#endif
