/* Monkey Fight 2 (mini_fight2.rel): declarations shared by the fight TUs. */
#ifndef FIGHT_H
#define FIGHT_H

#include "types.h"
#include "ape_internal.h"
#include "avdisp.h"
#include "background.h"
#include "ball.h"
#include "bitmap.h"
#include "camera.h"
#include "dip.h"
#include "effect.h"
#include "event.h"
#include "frustum.h"
#include "gx.h"
#include "input.h"
#include "item.h"
#include "libc.h"
#include "light.h"
#include "macros.h"
#include "mathutil.h"
#include "mode.h"
#include "pc_hfr.h"
#include "pool.h"
#include "rand.h"
#include "shadow.h"
#include "sound.h"
#include "sprite.h"
#include "stage.h"
#include "stcoli.h"
#include "stobj.h"
#include "textdraw.h"
#include "world.h"


/* ---- shared structs (coordinator). Add fields by splitting a filler in place; keep offsets
   commented; never reorder. Commit such edits separately ("fight.h struct: ..."). ---- */

/* One point of a fighter's ball trail (FightPlayer.trail). */
struct FightTrailPt
{
    /*0x00*/ u16 flags;  /* 1: drawn */
    /*0x02*/ s16 timer;
    /*0x04*/ Vec pos;
};  /* size 0x10 */

/* Ring buffer of the last 60 ball positions (FightPlayer+0x20, updated by fn_12_1D534). */
struct FightTrail
{
    /*0x00*/ s16 head;
    /*0x02*/ s16 count;
    /*0x04*/ s32 slowTimer;
    /*0x08*/ Vec prevVel;
    /*0x14*/ struct FightTrailPt pts[60];
};  /* size 0x3D4 */

/* One fighter (lbl_12_bss_1ACA0.players[ball->playerId]). */
struct FightPlayer
{
    /*0x000*/ s32 score;  /* KO points */
    /*0x004*/ s16 wins;  /* rounds won */
    /*0x006*/ s16 unk6;  /* flags; 8: draw the ball's ground marker; > 0: not targetable by the CPU */
    /*0x008*/ u16 chara;  /* active_monkey_id % 5, row of lbl_12_data_9CC4 (glove parameters) */
    /*0x00A*/ s16 hitBy[4];  /* per attacker: frames left to credit a knock-out to it */
    /*0x012*/ s16 stunTimer;  /* frames without punch control after being hit; freezes glove effects */
    /*0x014*/ s16 unk14;
    /*0x016*/ s16 rank;  /* 0 = leading; nonzero: not a round winner */
    /*0x018*/ u16 flags;  /* 1: CPU player, 2: out of the round/sudden death, 4: sudden death winner,
                             8, 0x10: trail on, 0x20: punched */
    /*0x01A*/ s16 unk1A;  /* countdown, decremented every frame of the round; ball-ball hit sound cooldown */
    /*0x01C*/ s16 unk1C;
    /*0x01E*/ s16 handicap;  /* from the sel menu setting, -2..+2 (d-pad in debug mode) */
    /*0x020*/ struct FightTrail trail;
};  /* size 0x3F4 on GC */

/* One segment of a glove's spring arm (struct FightGlove.segs). */
struct FightGloveSeg
{
    /*0x00*/ Vec pos;
    /*0x0C*/ Vec vel;
    /*0x18*/ s16 rotX;
    /*0x1A*/ s16 rotY;
    /*0x1C*/ s16 rotZ;
    /*0x1E*/ u8 filler1E[2];
};  /* size 0x20 */

/* A player's punching glove (lbl_12_bss_1ACA0.gloves[player], the extraData of its type 7 stobj). */
struct FightGlove
{
    /*0x000*/ s16 playerIdx;  /* index into FightWork.players / balls */
    /*0x002*/ s16 hitCooldown[4];  /* per target player */
    /*0x00A*/ s16 unkA;
    /*0x00C*/ s16 unkC;
    /*0x00E*/ s16 chargeTimer;
    /*0x010*/ s16 unk10;
    /*0x012*/ u8 filler12[0x14 - 0x12];
    /*0x014*/ float unk14;  /* ball speed kept while punching */
    /*0x018*/ float unk18;
    /*0x01C*/ float unk1C;  /* punch length in frames (times BLT INTERVAL) */
    /*0x020*/ float unk20;
    /*0x024*/ s8 radLevel;
    /*0x025*/ s8 powerLevel;
    /*0x026*/ s16 rangeLevel;
    /*0x028*/ s16 chargeLevel;
    /*0x02A*/ s16 unk2A;  /* voice cooldown */
    /*0x02C*/ s16 unk2C;
    /*0x02E*/ u16 flags;  /* 1, 2: reset arm, 4, 8: punching, 0x10: charging, 0x20: charged punch, 0x40: off */
    /*0x030*/ s16 unk30;
    /*0x032*/ s16 unk32;
    /*0x034*/ struct Stobj *stobj;  /* the glove's stage object */
    /*0x038*/ s32 sound38;  /* looping sound ids, -1 = none */
    /*0x03C*/ s32 sound3C;
    /*0x040*/ s32 sound40;
    /*0x044*/ s32 unk44;
    /*0x048*/ s32 prevPunch;
    /*0x04C*/ s32 prevCharge;
    /*0x050*/ struct GMAModel *armModel;
    /*0x054*/ struct GMAModel *unk54;
    /*0x058*/ float unk58;
    /*0x05C*/ struct FightGloveSeg segs[12];
};  /* size 0x1DC on GC */

/* Main Monkey Fight state (lbl_12_bss_1ACA0, defined in fight_3.c, first object of its .bss).
   NOTE: fight_3.c functions that touch >= 3 of its .bss objects address all of them off
   lbl_12_bss_1ACA0 (MWCC base pooling), e.g. lbl_12_bss_1C410 shows up as lbl_12_bss_1ACA0+0x1770. */
struct FightWork
{
    /*0x0000*/ u16 state;      /* current state, index into lbl_12_data_608 */
    /*0x0002*/ u16 nextState;  /* state to run next frame */
    /*0x0004*/ s16 round;      /* current round (0-based) */
    /*0x0006*/ u8 filler6[0x2];
    /*0x0008*/ struct FightPlayer players[4];
    /*0x0FD8*/ struct FightGlove gloves[4];
    /*0x1748*/ s32 unk1748;
    /*0x174C*/ s32 unk174C;  /* 1: a single leader */
    /*0x1750*/ s16 unk1750;  /* a random leader; the player the end-of-match camera follows */
    /*0x1752*/ s8 rankCounts[4];  /* players per rank (rankCounts[0]: round winners; > 1: sudden death) */
    /*0x1756*/ u8 filler1756[0x1758 - 0x1756];
    /*0x1758*/ u32 flags;      /* 1: story/tournament mode (lbl_1_bss_16DC & 1), 0x80 stops the CPU players */
    /*0x175C*/ s32 winsNeeded; /* rounds needed to win the match */
    /*0x1760*/ s32 unk1760;
};  /* size 0x1764 on GC */

/* Player marker floating above each ball (lbl_12_bss_1C428[i], fight_3.c), stride 0x3C. */
struct FightMarker
{
    /*0x00*/ s32 state;  /* 1: show, 2: shown, 3: hide */
    /*0x04*/ float scale;
    /*0x08*/ float targetScale;
    /*0x0C*/ s32 crownKicked;  /* the crown got its wobble kick */
    /*0x10*/ float crownScale;  /* crown over the leaders */
    /*0x14*/ float crownTilt;
    /*0x18*/ float crownTiltVel;
    /*0x1C*/ float crownLift;
    /*0x20*/ s32 rotZ[5];  /* per camera */
    /*0x34*/ float rotY;
    /*0x38*/ float rotYVel;
};  /* size 0x3C */

/* lbl_12_bss_1C518 (fight_3.c): per round log, 17 entries, reset to -1 by fn_12_16B44, written by
   fn_12_16BC4 (unk0[i]: player i's rank, unk4[i]: player i's score) */
struct Fight1C518
{
    /*0x00*/ s8 unk0[4];
    /*0x04*/ s16 unk4[4];
};  /* size 0xC */

extern struct FightWork lbl_12_bss_1ACA0;
/* separate objects right after it (bss 0x1C404-0x1C410, saved on mode entry, restored on exit) */
extern u8 lbl_12_bss_1C404;     /* modeCtrl.playerCount */
extern s8 lbl_12_bss_1C408[4];  /* g_poolInfo.playerPool.statusList[i] */
extern u8 lbl_12_bss_1C40C[4];  /* active_monkey_id[i] */
/* bss 0x1C410-0x1C428: separate objects (the code folds their offsets off the pooled base) */
extern s8 lbl_12_bss_1C410[3];  /* stage of fight round i % 3, index into lbl_12_data_668 */
extern struct GMAModel *lbl_12_bss_1C414;     /* minigameGma model 88: marker sub model (fn_12_150EC) */
extern struct GMAModel *lbl_12_bss_1C418[4];  /* marker model per player (fn_12_150EC) */
extern struct FightMarker lbl_12_bss_1C428[4];
extern struct Fight1C518 lbl_12_bss_1C518[17];
extern u8 lbl_12_bss_1C5E8[0x2804];  /* buffer, lbl_12_bss_1EE38.work points at it (fn_12_176B8) */

/* ---- fight.c (w1) ---- */

/* main_loop stobj.c callback table (fn_1_A8034) */
struct StobjFuncs
{
    void (*init)(struct Stobj *);
    void (*tick)(struct Stobj *);
    void (*disp)(struct Stobj *);
    void (*coli)(struct Stobj *, struct PhysicsBall *);
    void (*dest)(struct Stobj *);
    void (*cbF)(struct Stobj *);
};


/* fight.c functions used elsewhere */
void fn_12_14C(void);
void fn_12_18C(void);
float fn_12_457C(struct Stobj *stobj);  /* glove stobj collision radius (radius/power/charge levels) */
float fn_12_4658(struct Stobj *stobj);  /* glove stobj punch reach (range/charge levels) */
float fn_12_4710(struct Stobj *stobj);  /* glove stobj smoothed range extension (arm stretch) */
void fn_12_5050(struct FightGlove *glove, float speed);  /* glove breaks apart into effects */
void fn_12_56B8(struct FightGlove *glove);  /* unpowered glove hit: throw-off effect, shrink */

/* used by fight.c, defined elsewhere */
void fn_12_11300(void);  /* fight_3.c */
void fn_12_11910(void);
void fn_12_11A28(void);
void fn_12_11BC4(void);
void fn_12_1623C(struct Ball *victim, struct Ball *scorer, int points, int subtract, int index);
void fn_12_1D37C(struct Ball *ball);  /* fight_5.c */
void fn_12_20598(struct Camera *camera, struct Ball *ball);  /* fight_6.c */
void fn_12_23F60(int player);  /* fight_9.c */

/* ---- fight_1.c, fight_8.c (w2) ---- */
/* DOL/main_loop globals and functions used by fight_1.c/fight_8.c that no shared header declares */
extern struct GMA *minigameGma;
void g_stores_doubles2(float r, float g, float b, float a);
void mtxa_from_mtxb_translate_xyz(float x, float y, float z);
void g_rumble_controller(s32 pad, s32 a, s32 b);
int fn_1_A74E4(Vec *a, Vec *b, float ra, Vec *c, Vec *d, float rb);  /* swept sphere-sphere test */
void mtxa_to_quat(Quat *q);
/* defined in other fight TUs, used by fight_8.c */
void fn_12_16880(struct Ball *ball, int itemSubType);  /* fight_3.c: punch item picked up */
/* fight_1.c: item box stobj (breaks into 35 pieces and drops a punch item). One per stobj
   (lbl_12_bss_118[stobj->id], defined in fight.c), pointed to by the box stobj's extraData. */
struct Stobj;
struct FightBoxPiece
{
    /*0x00*/ s32 timer;  /* frames until it flies off; 0 attached, -1 gone */
    /*0x04*/ Vec pos;  /* wobble offset */
};  /* size 0x10 */
struct FightBox
{
    /*0x000*/ struct Stobj *stobj;  /* NULL: free */
    /*0x004*/ s16 unk4;
    /*0x006*/ s16 hitCount;
    /*0x008*/ s8 playerCooldown[4];  /* frames before a ball can hit it again */
    /*0x00C*/ s8 stobjCooldown[144];  /* per stobj id */
    /*0x09C*/ float glow;
    /*0x0A0*/ float alpha;
    /*0x0A4*/ u16 flags;
    /*0x0A6*/ s16 lastHitter;  /* ball index, -1 none */
    /*0x0A8*/ float coreScale;
    /*0x0AC*/ Vec wobble;  /* also read as angles */
    /*0x0B8*/ Vec wobbleVel;
    /*0x0C4*/ s32 pieceCount;
    /*0x0C8*/ struct FightBoxPiece pieces[35];
};  /* size 0x2F8 on GC */
extern struct GMAModel *lbl_12_bss_0[35];  /* fight.c: box piece models (reflection pass) */
extern struct GMAModel *lbl_12_bss_8C[35];  /* fight.c: box piece models */
extern struct FightBox lbl_12_bss_118[144];  /* fight.c */
extern s16 lbl_12_data_230[6];  /* fight.c: punch voice per monkey */
float fn_12_474C(struct Stobj *stobj);  /* fight.c: mass of a player stobj */
int fn_12_1804C(void);  /* fight_4.c: punch item subtype to drop */
/* fight_1.c */
void fn_12_5AF8(struct Stobj *stobj);  /* player stobj vs item boxes */
void fn_12_640C(void);  /* item boxes vs each other */
void fn_12_6B50(struct Stobj *stobj, Vec *pos, Vec *vel, float radius);  /* box hit */
void fn_12_6FB0(struct Stobj *stobj);  /* box init */
void fn_12_70B8(struct Stobj *stobj);  /* box tick */
void fn_12_83F8(struct Stobj *stobj);  /* box disp */
void fn_12_8970(struct Stobj *stobj, struct PhysicsBall *physBall);  /* box coli */
void fn_12_8EC0(struct Stobj *stobj);  /* box dest */
void fn_12_8ED0(void);  /* install the fight effect funcs */
void fn_12_8FB0(void);  /* restore the default effect funcs */
/* fight_8.c */
void fn_12_2301C(void);  /* install the punch item funcs */
void fn_12_23048(void);  /* restore the default punch item funcs */

/* ---- fight_2.c 0x9080 fragment and data (w3) ---- */
extern u16 lbl_12_data_5F0[10];       /* fight_2.c: minigameGma model of each digit */
void vec_to_euler_xy(Vec *v, s16 *x, s16 *y);
extern u32 g_some_render_flag;
extern Vec lbl_1_data_2E418[10];  /* main_loop ball.c: colour of each ball colorId */

/* ---- fight_2.c 0xCA84 fragment (w4) ---- */

/* One joint of a balloon ribbon (a skinned GMA model driven as a rope). */
struct FightRibbonJoint
{
    /*0x00*/ Vec pos;
    /*0x0C*/ Vec vel;
    /*0x18*/ u8 filler18[0x24 - 0x18];
    /*0x24*/ Vec dir;  /* unit vector toward the previous joint, this frame */
    /*0x30*/ float stiffness;  /* how fast the velocity follows the wind */
    /*0x34*/ Vec restDir;  /* rest direction toward the previous joint */
    /*0x40*/ float length;  /* rest distance to the previous joint */
    /*0x44*/ Vec nextDir;  /* rest direction toward the next joint */
    /*0x50*/ float nextLen;
    /*0x54*/ float invNextLen;
    /*0x58*/ s16 twist;
    /*0x5A*/ u16 flags;  /* 1: pinned (first joint or degenerate), 2: last joint */
};  /* size 0x5C */

#define FIGHT_RIBBON_MAX_JOINTS 32

/* A prize balloon with three ribbons (BALLOON_x, RIBON_UP_skin, RIBON_DOWN_R/L_skin).
   The balloon effect (fn_12_EB38..) keeps a pointer to it in effect->model. */
struct FightBalloon
{
    /*0x00*/ Vec pos;  /* in itemgroups[1] space */
    /*0x0C*/ s16 rotX;
    /*0x0E*/ s16 rotY;
    /*0x10*/ s16 rotZ;
    /*0x14*/ struct GMAModel *balloonModel;
    /*0x18*/ struct GMAModel *ribbonModels[3];  /* up, down right, down left */
    /*0x24*/ s16 jointCounts[3];  /* ribbonModels[i]->mtxCount */
    /*0x2C*/ struct FightRibbonJoint joints[3][FIGHT_RIBBON_MAX_JOINTS];
};  /* size 0x22AC on GC */

/* ---- fight_3.c 0x11300 fragment and data (w5) ---- */

/* main_loop sel.c struct SelSettings9: Monkey Fight menu settings */
struct FightSelSettings
{
    /*0x00*/ u32 flags;      /* 1: cpu players fill the empty slots, 2: ... (-> FightWork.unk1760) */
    /*0x04*/ s8 unk4;        /* -> FightWork.winsNeeded */
    /*0x05*/ s8 stageIdx[3]; /* stage of fight round i (-> lbl_12_bss_1C410) */
    /*0x08*/ u8 unk8[4];
};  /* size 0xC */
extern struct FightSelSettings lbl_1_bss_16DC;

/* main_loop window.c Monkey Fight debug menu values (see window_menu.inc) */
extern s16 lbl_1_bss_6FA74;  /* POWER LEVEL MAX */
extern s16 lbl_1_bss_6FA76;  /* RANGE LEVEL MAX */
extern s16 lbl_1_bss_6FA78;  /* RAD LEVEL MAX */
extern s16 lbl_1_bss_6FA7A;  /* CPU NUM */
extern f32 lbl_1_bss_6FA80;  /* ATK MOVE CNTL ACC MAX */
extern f32 lbl_1_bss_6FA84;  /* ATK MOVE CNTL ACC MIN */
extern f32 lbl_1_bss_6FA88;  /* ATK MOVE CNTL RATE */
extern s16 lbl_1_bss_6FA8C;  /* ATK MOVE CNTL CNTR */
extern f32 lbl_1_bss_6FA90;  /* WLD SLANT COEF */
extern s16 lbl_1_bss_6FA94;  /* BALL TAIL EFC */
extern s16 lbl_1_bss_6FA96;  /* POINT GET EFC */
extern f32 lbl_1_bss_6FA98;  /* MUTEKI SEC */
extern f32 lbl_1_bss_6FA9C;  /* KILL DECIDE SEC */
extern s16 lbl_1_bss_6FAA0;  /* START POINT */
extern s16 lbl_1_bss_6FAA2;  /* PUNCH HIT POINT */
extern s16 lbl_1_bss_6FAA4;  /* FALL POINT */
extern s16 lbl_1_bss_6FAA6;  /* KILL POINT */
extern s16 lbl_1_bss_6FAA8;  /* KILL TOP POINT */
extern s16 lbl_1_bss_6FAAA;  /* POINT SUB MODE */
extern f32 lbl_1_bss_6FAAC;  /* BLT BALL BREAK */
extern f32 lbl_1_bss_6FAB0;  /* BLT RANGE LV COEF */
extern f32 lbl_1_bss_6FAB4;  /* BLT RANGE */
extern f32 lbl_1_bss_6FAB8;  /* BLT COLI RAD LV COEF */
extern f32 lbl_1_bss_6FABC;  /* BLT COLI RAD */
extern f32 lbl_1_bss_6FAC0;  /* BLT REACT POWER */
extern f32 lbl_1_bss_6FAC4;  /* BLT POWER LV COEF */
extern f32 lbl_1_bss_6FAC8;  /* BLT POWER */
extern f32 lbl_1_bss_6FACC;  /* BLT SPD */
extern s32 lbl_1_bss_6FAD0;  /* BLT TIME */
extern s32 lbl_1_bss_6FAD4;  /* BLT SWING MODE */
extern s32 lbl_1_bss_6FAD8;  /* BLT TRIGER MODE */
extern s32 lbl_1_bss_6FADC;  /* BLT INTERVAL */
extern f32 lbl_1_bss_6FAE0;  /* BALL POWER */
extern u16 lbl_1_bss_6FAE4;  /* CAM ANG Y */
extern u16 lbl_1_bss_6FAE6;  /* CAM ANG X */

/* main_loop / DOL functions and globals used by fight_3.c */
extern s32 stage_heap;
extern s32 active_monkey_id[];
extern u32 g_player_id_for_sound;
extern u32 g_smth_with_sound;
int OSSetCurrentHeap(int heap);
void SoundGroupLoad(int);
void call_SoundReqID_arg_0(int soundId);
void dest_all_events(void);
void event_init(int id);
void fade_screen_to_color(s32 type, u32 color, int duration);
void fn_1_3061C(int a, int b);
void fn_1_358E8(void);
void fn_1_48384(struct Ball *ball);
void fn_1_56C18(void);
void fn_1_D4870(void);
void fn_1_D6D84(int i);
void free_minigame_gma_tpl(void);
s16 g_SoftStreamStart_with_some_defaults(int track);
void g_draw_ball_and_ape(void);
void g_draw_bg(void);
void g_draw_func_init(void);
void g_draw_stage(void);
void g_draw_stobjs(void);
void g_fade_track_volume(int a, int b);
void g_init_rendefc_for_stage(void);
void g_load_minigame_gma_tpl(int idx);
void g_maybe_some_bg_effect_handler(void);
void g_smth_with_rendefc_reflective_height(float h);
void g_something_calls_sndPopGroup(void);
void g_something_with_drawing_apes(void);
void g_something_with_iteratively_freeing_memory(void);
void g_something_with_rend_efc(int mask);
void gp_wait_wrapper(void);
void draw_shadow_reqs(void);
void load_commend_stage(void);
void load_stage(int stage);
void ord_tbl_draw_nodes(void);
void queue_stage_load(int stageId);

/* fight_3.c data */
struct FightRoundStage
{
    /*0x00*/ s16 unk0;     /* passed to fn_12_17910 */
    /*0x02*/ s16 stageId;
};
extern void (*lbl_12_data_608[])(void);  /* state functions, indexed by FightWork.state */
extern struct FightRoundStage lbl_12_data_668[3];
extern s32 lbl_12_data_674[20];  /* round announcement sound per round */
extern u16 lbl_12_data_6C4[4];   /* init_common_gma model per player */
extern GXColor lbl_12_data_6CC[4];
void fn_12_1674C(struct Ball *ball);  /* reset a ball's glove (stop sounds, glove stobj onto the ball) */
int fn_12_1755C(struct Ball *attacker, struct Ball *victim, int points);  /* points after handicaps */

/* ---- fight_3.c 0x14914 fragment (w6) ---- */

/* ---- fight_4.c (w7) ---- */

/* A punch landing on a stage item group (built by fight.c, handed to lbl_12_bss_1EE38.hitFunc). */
struct FightStageHit
{
    /*0x00*/ s32 type;       /* 0: ball rolling/bumping, 1: punch */
    /*0x04*/ s32 playerId;
    /*0x08*/ s32 itemGroup;  /* index into stagedef->collHeaders */
    /*0x0C*/ float power;
    /*0x10*/ Vec pos;
    /*0x1C*/ Vec dir;
};  /* size 0x28 */

/* Camera hooks of the stage gimmick (lbl_12_bss_1EE38.cameraFuncs, called by fight_7.c). */
struct FightStageCameraFuncs
{
    /*0x00*/ void (*init)(struct Camera *camera, struct Ball *ball);
    /*0x04*/ void (*tick)(struct Camera *camera, struct Ball *ball);
    /*0x08*/ void (*unk8)(void);
};  /* size 0xC */

/* Stage gimmick state (lbl_12_bss_1EE38, fight_4.c). The work buffer is lbl_12_bss_1C5E8 (fight_3.c). */
struct FightStage
{
    /*0x00*/ s32 type;  /* index into lbl_12_data_850 (init/tick/exit of each stage type) */
    /*0x04*/ struct Sphere flyIn;
    /*0x14*/ void (*hitFunc)(struct FightStageHit *hit);
    /*0x18*/ void *work;
    /*0x1C*/ void (*hooks[4])(void);  /* called by fight_2.c fn_12_C974/C9B8/C9FC/CA40;
                                          stage type 4 sets hooks[2] = fn_12_19300 (item group tint) */
    /*0x2C*/ struct FightStageCameraFuncs *cameraFuncs;
    /*0x30*/ s32 unk30;
};  /* size 0x34 on GC (bss symbol 0x38) */

extern struct FightStage lbl_12_bss_1EE38;
extern struct FightStartSet lbl_12_bss_1EDF0;  /* the stage start positions */

/* One timed stage object spawn (lbl_12_data_8A4/A64/C24 tables; ends with time -100). */
struct FightStageSpawn
{
    /*0x00*/ float time;  /* spawn once the remaining time ratio drops below this */
    /*0x04*/ s16 kind;    /* 1 = stobj, 2 = item */
    /*0x06*/ u16 playerMask;  /* bit (playerCount - 1) */
    /*0x08*/ S16Vec rot;
    /*0x0E*/ u8 fillerE[2];
    /*0x10*/ Vec pos;
};  /* size 0x1C */

/* Head of every stage type's work buffer (lbl_12_bss_1EE38.work). */
struct FightStageSpawner
{
    /*0x00*/ struct FightStageSpawn *next;
    /*0x04*/ struct FightStageSpawn *list;
    /*0x08*/ s32 maxObjs;
};

extern struct FightBalloon lbl_12_data_EF0[4];

void fn_12_17714(void);
void fn_12_17844(void);
void fn_12_178CC(struct FightStageHit *hit);
void fn_12_17910(int type);
u32 fn_12_17AB8(int itemGroup);  /* item group flags of the stage type */
void fn_12_17B94(struct FightStageSpawner *spawner);
void fn_12_18048(struct FightStageSpawner *spawner);

/* ---- fight_5.c, fight_6.c (w8) ---- */

/* Per-character fighter parameters (lbl_12_data_9CC4, fight_5.c), indexed by a character id (u16). */
struct FightCharaParam
{
    /*0x00*/ float unk0;
    /*0x04*/ float unk4;
    /*0x08*/ float unk8;
    /*0x0C*/ float unkC;
    /*0x10*/ s32 unk10;
};  /* size 0x14 */
extern struct FightCharaParam lbl_12_data_9CC4[5];

/* Start positions of a stage (lbl_12_bss_1EDF8 in fight_4.c, lbl_12_data_9B60 in fight_5.c). */
struct FightStartPos
{
    /*0x00*/ float unk0;
    /*0x04*/ Vec pos;
};  /* size 0x10 */

/* A set of start positions (lbl_12_bss_1EDF0; arrays of them go to fn_12_205E8 in fight_7.c). */
struct FightStartSet
{
    /*0x00*/ s16 unk0;  /* item group the set is attached to */
    /*0x02*/ s16 count;
    /*0x04*/ struct FightStartPos *list;
};  /* size 8 on GC */

/* Per-player ape animation state (lbl_12_bss_1EE70, fight_6.c). */
struct FightApeState
{
    /*0x00*/ u32 state;
    /*0x04*/ struct FightApeAnim *anim;  /* fn_12_1F34C / fn_12_1F398 */
};
extern struct FightApeState lbl_12_bss_1EE70[4];

/* ---- fight_7.c, fight_10.c (w9) ---- */

/* ---- fight_9.c 0x23858 fragment and data (w10) ---- */
extern GXColor lbl_12_data_A218[10];  /* colour per ball colorId */
extern u32 lbl_12_bss_1F644;
void fn_12_23858(void);
void fn_12_23F60(int playerId);
void fn_12_24050(int playerId);
void fn_12_25FA8(void);

/* ---- fight_9.c 0x26798 fragment (w11) ---- */

#ifdef __MWERKS__
unsigned long strlen(const char *s);  /* not in libc.h */
#endif

/* main_loop / DOL */
void fn_1_E60D0(int playerId, Vec *pos);  /* mini_commend.c: position over a podium/player */
void fn_1_E678C(Mtx mtx);                 /* mini_commend.c: copy the saved view matrix */
void g_math_unk15(Vec *in, Vec *out, float tanHalfFov);  /* project a point to screen coordinates */

s32 fn_12_174E8(void);  /* fight_3.c: TRUE if every active player is one win from the match (final round) */

/* ---- fight_11.c (w12) ---- */

/* CPU opponent state, one per player (lbl_12_bss_1F9D8[playerId], defined in fight_11.c). The AI
   writes a fake controller state into `pad`, which fight.c reads instead of pad_status_groups. */
/* The 36 evolving behaviour parameters of a CPU player (ranges in fight_11.c lbl_12_data_A2C0). */
struct FightCpuParams
{
    /*0x00*/ float unk00;
    /*0x04*/ float unk04;
    /*0x08*/ float unk08;
    /*0x0C*/ float unk0C;
    /*0x10*/ float unk10;
    /*0x14*/ float unk14;
    /*0x18*/ float unk18;
    /*0x1C*/ float unk1C;
    /*0x20*/ float unk20;
    /*0x24*/ float unk24;
    /*0x28*/ float unk28;
    /*0x2C*/ float unk2C;
    /*0x30*/ float unk30;
    /*0x34*/ float unk34;
    /*0x38*/ float unk38;
    /*0x3C*/ float unk3C;
    /*0x40*/ float unk40;
    /*0x44*/ float unk44;
    /*0x48*/ float unk48;
    /*0x4C*/ float unk4C;
    /*0x50*/ float unk50;
    /*0x54*/ float unk54;
    /*0x58*/ float unk58;
    /*0x5C*/ float unk5C;
    /*0x60*/ float unk60;
    /*0x64*/ float unk64;
    /*0x68*/ float unk68;
    /*0x6C*/ float unk6C;
    /*0x70*/ float unk70;
    /*0x74*/ float unk74;
    /*0x78*/ float unk78;
    /*0x7C*/ float unk7C;
    /*0x80*/ float unk80;
    /*0x84*/ float unk84;
    /*0x88*/ float unk88;
    /*0x8C*/ float unk8C;
};  /* size 0x90 */

struct FightCpu
{
    /*0x000*/ struct ControllerInfo pad;  /* [0] held, [1] prev, [2] pressed, [3] released */
    /*0x03C*/ struct FightCpuParams params;
    /*0x0CC*/ struct FightCpuParams paramSteps;  /* mutation step of each parameter */
    /*0x15C*/ s16 state;  /* 0 approach, 1 retreat, 2 fight */
    /*0x15E*/ u8 filler15E[2];
    /*0x160*/ Vec stick;  /* wanted move direction (x, z) */
    /*0x16C*/ Vec aim;  /* punch direction, zero: punch straight */
    /*0x178*/ s32 punch;
    /*0x17C*/ s32 guard;
    /*0x180*/ Vec targetPos;
    /*0x18C*/ Vec targetVel;
    /*0x198*/ float targetRadius;
    /*0x19C*/ float nearestDist;  /* distance to the surface of the nearest player or object */
    /*0x1A0*/ u32 targetFlags;  /* bits 0..2: the target player's FightWorkFD8 flags 8/0x10/0x20 */
    /*0x1A4*/ s32 punchTimer;
    /*0x1A8*/ s32 timer;
};  /* size 0x1AC */

extern struct FightCpu lbl_12_bss_1F9D8[4];

#endif /* FIGHT_H */
