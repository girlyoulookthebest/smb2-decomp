#ifndef MODE_H
#define MODE_H

#include "types.h"
#include "mathutil.h"

/* SMB1's ModeControl, extended. Only fields used by decompiled code are named. */
struct ModeControl
{
    /*0x00*/ s32 submodeTimer;
    /*0x04*/ s32 difficulty;
    /*0x08*/ u32 courseFlags;
    u8 unkC[0x10 - 0xC];
    /*0x10*/ s32 unk10;
    /*0x14*/ s32 unk14;
    /*0x18*/ s32 unk18;
    /*0x1C*/ s32 unk1C;
    /*0x20*/ s32 menuSel;
    /*0x24*/ s32 playerCount;
    /*0x28*/ s32 gameType;
    /*0x2C*/ s32 currPlayer;  /* in turn-based modes, whose turn it is */
    /*0x30*/ s32 unk30;
    u8 unk34[0x3C - 0x34];
    /*0x3C*/ s32 unk3C;
    /*0x40*/ s16 unk40;
    /*0x42*/ u8 splitscreenMode;  /* split screen setting for 3 players */
    /*0x43*/ u8 unk43;
    /*0x44*/ s32 unk44;
    /*0x48*/ s32 unk48;
    /*0x4C*/ u8 wideMode;  /* 16:9 output enabled */
    u8 unk4D[0x50 - 0x4D];
}; /* size = 0x50 */

extern struct ModeControl modeCtrl;

/* modeCtrl.difficulty */
enum
{
    DIFFICULTY_BEGINNER = 0,
    DIFFICULTY_ADVANCED = 1,
    DIFFICULTY_EXPERT = 2,
};

/* modeCtrl.courseFlags */
enum
{
    COURSE_FLAG_EXTRA = 1 << 3,
    COURSE_FLAG_MASTER = 1 << 4,
    COURSE_FLAG_MASTER_EXTRA = 1 << 23,
};

/* modeCtrl.gameType */
enum
{
    GAMETYPE_MAIN_NORMAL = 0,
    GAMETYPE_MAIN_COMPETITION = 1,
    GAMETYPE_MAIN_PRACTICE = 2,
    GAMETYPE_MINI_FIGHT = 4,
};

enum
{
    MD_SEL = 1,
    MD_MINI = 4,
};

enum
{
    SMD_GAME_CONTINUE_INIT = 0x41,
    SMD_GAME_CONTINUE_MAIN = 0x42,
    SMD_GAME_RESULT_INIT = 0x4C,
    SMD_GAME_RESULT_MAIN = 0x4D,
    SMD_GAME_RESULT_MENU = 0x4E,
    SMD_MINI_BILLIARDS_MAIN = 0xAE,
    SMD_MINI_BILLIARDS_UNKC6 = 0xC6,
};

/* SMB1's InfoWork: state of the stage being played. */
struct ModeInfo
{
    /*0x00*/ u32 flags;  /* INFO_FLAG_* */
    /*0x04*/ s16 timerCurr;  /* clock, in 60ths of a second */
    /*0x06*/ s16 timerMax;
    /*0x08*/ u32 unk8;
    /*0x0C*/ s16 goalEntered;
    /*0x0E*/ s16 goalItemGroup;  /* item group the entered goal is attached to */
    /*0x10*/ Vec unk10;  /* ball velocity at the goal */
    /*0x1C*/ s16 unk1C;
    /*0x1E*/ s16 attempts;  /* tries on the current stage */
    /*0x20*/ s16 currFloor;
    /*0x22*/ s16 unk22;
    /*0x24*/ s32 bananasLeft;
    /*0x28*/ s16 livesLost;
    /*0x2A*/ s16 continuesUsed;
    /*0x2C*/ s16 unk2C;
    /*0x2E*/ s16 currStageId;
    /*0x30*/ s16 playerId;
    /*0x32*/ s16 unk32;
    /*0x34*/ s16 storyWorld;
    /*0x36*/ s16 storyStage;
    /*0x38*/ s8 goalType;
    u8 unk39[3];
};  /* size = 0x3C */

enum
{
    INFO_FLAG_GOAL = 1 << 0,
    INFO_FLAG_TIMEOVER = 1 << 1,
    INFO_FLAG_FALLOUT = 1 << 2,
    INFO_FLAG_TIMER_PAUSED = 1 << 3,
    INFO_FLAG_REPLAY = 1 << 4,
    INFO_FLAG_05 = 1 << 5,
    INFO_FLAG_BONUS_STAGE = 1 << 6,
    INFO_FLAG_07 = 1 << 7,
    INFO_FLAG_08 = 1 << 8,
    INFO_FLAG_BONUS_CLEAR = 1 << 9,
    INFO_FLAG_10 = 1 << 10,
    INFO_FLAG_11 = 1 << 11,
    INFO_FLAG_FINAL_FLOOR = 1 << 12,
    INFO_FLAG_13 = 1 << 13,
};

extern struct ModeInfo mode_info;
extern u32 frame_counter;
extern s32 main_mode;
extern s32 sub_mode;
extern s32 main_mode_request;
extern s32 sub_mode_request;

#endif
