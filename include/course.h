#ifndef COURSE_H
#define COURSE_H

#include "types.h"

/* Challenge Mode course scripts (SMB1's course.c): each course is a list of commands.
 * A CMD_FLOOR/FLOOR_STAGE_ID command starts a floor; the CMD_IF/CMD_THEN commands after it
 * decide which floor to jump to once the floor is over. */

enum
{
    CMD_IF = 0,
    CMD_THEN = 1,
    CMD_FLOOR = 2,
    CMD_COURSE_END = 3,
};

enum
{
    IF_FLOOR_CLEAR = 0,
    IF_TIME_ELAPSED = 1,
    IF_GOAL_TYPE = 2,
};

enum
{
    THEN_JUMP_FLOOR = 0,
    THEN_CLEAR_NEXT_STAGE2 = 1,
    THEN_CLEAR_NEXT_STAGE = 2,
};

enum
{
    FLOOR_STAGE_ID = 0,
    FLOOR_TIME = 1,
};

struct CourseCommand
{
    /*0x00*/ u8 opcode;
    /*0x01*/ u8 type;
    /*0x04*/ s32 value;
    u8 unk8[0x1C - 0x8];
};  /* size = 0x1C */

/* Practice Mode: the course position the player picked (recplay.c reads this too). */
struct PracticeModeStageInfo
{
    /*0x00*/ s16 floor;
    /*0x02*/ s16 difficulty;
    /*0x04*/ u32 courseFlags;
};  /* size = 0x8 */

struct CourseJump
{
    /*0x00*/ s32 floor;
    /*0x04*/ s32 stageId;
};  /* size = 0x8 */

/* One floor a player has played, with the floors its goals lead to. */
struct CourseProgress
{
    /*0x00*/ s32 floor;
    /*0x04*/ s32 stageId;
    /*0x08*/ struct CourseJump jumps[3];
    /*0x20*/ s16 jumpCount;
    /*0x22*/ s16 jumpIdx;
};  /* size = 0x24 */

struct StoryModeStageInfo
{
    /*0x00*/ s16 stageId;
    /*0x02*/ s16 difficulty;
};  /* size = 0x4 */

struct StoryModeWorldInfo
{
    /*0x00*/ s16 stageCount;
    s16 unk2;
    /*0x04*/ struct StoryModeStageInfo *stages;
};  /* size = 0x8 */

struct StoryModeWorldState
{
    /*0x00*/ u16 flags;  /* 1: every stage beaten, 2: world not started yet */
    /*0x02*/ s16 beatenCount;
    /*0x04*/ s8 beatenStages[12];  /* stage indices in the order they were beaten */
    /*0x10*/ u32 stageFlags[10];  /* 1: beaten */
};  /* size = 0x38 */

extern struct CourseCommand *current_cm_entry;
extern s32 stage_jump_distance;
extern s32 in_practice_mode;
extern struct PracticeModeStageInfo g_stage_id_in_practice_mode[6];
extern s32 g_some_course_length;
extern struct CourseProgress cm_player_progress[4][3];
extern struct StoryModeWorldState sm_state[10];

extern struct CourseCommand *cm_courses[];
extern struct StoryModeWorldInfo sm_world_info[];

void clear_unlocked_cm_stages(void);
void init_course(void);
void event_course_init(void);
void event_course_tick(void);
void event_course_dest(void);
int get_current_cm_stage_time_limit(void);
int g_update_cm_course(int difficulty, int floor, u32 flags);
int calc_course_idx(int difficulty, u32 flags);
int g_get_some_difficulty_count_2(int difficulty, u32 flags);
BOOL g_are_on_final_course_level(int difficulty, int floor, u32 flags);
BOOL is_bonus_stage(int stageId);
void g_something_with_cm_entries_practice_mode(void);
int fn_1_A3C84(int difficulty, int floor, u32 flags);
BOOL g_smth_with_cm_entries_in_main_menu(int difficulty, int floor, u32 flags);
void fn_1_A3FA0(int bit);
void update_cm_unlocked_levels(int difficulty, int floor, u32 flags);
void clear_cm_player_progress(void);
void update_course_progress(void);
void g_something_with_cm_player_progress(void);
void g_save_cm_unlock_entries(void);
void g_load_cm_unlock_entries(void);
void clear_sm_state(void);
int get_world_stage_count(int world);
int get_world_unbeaten_stage_count(int world);
int fn_1_A4CA8(int world, int idx);
int get_story_mode_stage_id(int world, int stage);
int get_story_mode_stage_difficulty(int world, int stage);
BOOL fn_1_A4D5C(int world);
BOOL fn_1_A4DA4(void);
BOOL is_storymode_stage_beaten(int world, int stage);
BOOL g_is_storymode_stage_unlocked(int world, int stage);
void fn_1_A4F58(int world, int stage);
void unlock_storymode_stage(int world, int stage);
BOOL fn_1_A505C(int world);
void fn_1_A5080(int world);
void init_sm_state(int world, BOOL started);
void clear_unlocked_storymode_stages(void);
void g_save_storymode_unlock_entries(void);
void g_load_storymode_unlock_entries(void);
int get_storymode_stage_time_limit(int world, int stage);

#endif
