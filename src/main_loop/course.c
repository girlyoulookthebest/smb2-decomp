/* Challenge Mode course scripts and Story Mode world progress (SMB1's course.c). */

#include "types.h"
#include "course.h"
#include "dip.h"
#include "libc.h"
#include "mode.h"
#include "sprite.h"
#include "stage.h"

struct ScenInfo
{
    u8 unk0[4];
    /*0x04*/ s16 currentWorld;
    u8 unk6[2];
    /*0x08*/ u32 flags;
};

extern struct ScenInfo scen_info;
extern u8 storymode_save_files[];
extern u32 g_font_char_var_3;  /* global frame counter */

int get_next_player_idx(void);
void textdraw_reset(void);
void textdraw_set_font(int fontId);
void textdraw_set_scale(float scaleX, float scaleY);
void textdraw_set_mul_color(u32 color);
void textdraw_set_pos(float x, float y);
void textdraw_printf(const char *fmt, ...);
void textdraw_print(const char *str);

u8 cm_unlock_entries[18];  /* one bit per Challenge Mode floor played at least once */
struct CourseCommand *current_cm_entry;
s32 stage_jump_distance;
s32 in_practice_mode;
struct PracticeModeStageInfo g_stage_id_in_practice_mode[6];
s32 g_some_course_length;
struct CourseProgress cm_player_progress[4][3];
static s32 unused_course_pad[3];  /* only read into a dead local; occupies 12 bytes of .bss before sm_state */
struct StoryModeWorldState sm_state[10];
u8 storymode_unlock_entries[16];  /* one bit per Story Mode stage unlocked (13 bytes used) */

const int bonus_stage_id_list[] = { 0xCD, 0xE1, 0xE6, 0xF0, 0x109, 0x10E, 0x118, 0x122, 0x12C, 0 };

u32 lbl_1_data_333C0[0x5C] __attribute__((aligned(8))) =
{
    0xFE000102, 0x03040506, 0x0708090A, 0x0B0CFE0D, 0x0E0FFF00, 0x00010203,
    0x04050607, 0x08090A0B, 0x0C0D0E0F, 0xFF000000, 0x31000000, 0x00000000,
    0x3800B400, 0x00010000, 0x01000000, 0x00020000, 0x10004C00, 0x00030000,
    0x01000000, 0x00040000, 0x18000000, 0x00090000, 0x40000000, 0x00000000,
    0x01000000, 0x000B0000, 0x18000000, 0x000C0000, 0x18000000, 0x00050000,
    0x40000000, 0x00000000, 0x01000000, 0x00070000, 0x18000000, 0x00080000,
    0x3800B400, 0x000D0000, 0x40000000, 0x00000000, 0x18000000, 0x00140000,
    0x40000000, 0x00000000, 0x01000000, 0x00160000, 0x18000000, 0x00170000,
    0x18000000, 0x00180000, 0x18000000, 0x000F0000, 0x40000000, 0x00000000,
    0x01000000, 0x00110000, 0x18000000, 0x00120000, 0x18000000, 0x00130000,
    0x18000000, 0x00000000, 0x18000000, 0x00010000, 0x18000000, 0x00020000,
    0x18000000, 0x00030000, 0x18000000, 0x00040000, 0x18000000, 0x00050000,
    0x18000000, 0x00060000, 0x18000000, 0x00070000, 0x18000000, 0x00080000,
    0x18000000, 0x00090000, 0x18000000, 0x000A0000, 0x18000000, 0x000B0000,
    0x18000000, 0x000C0000, 0x18000000, 0x000D0000, 0x18000000, 0x000E0000,
    0x18000000, 0x000F0000,
};

BOOL is_stage_complete(struct CourseCommand *cmd);
BOOL entered_goal_has_type(struct CourseCommand *cmd);
BOOL did_beat_stage_faster_than(struct CourseCommand *cmd);
void calc_stage_jump_distance(struct CourseCommand *cmd);
void clear_next_cm_stage_id2(struct CourseCommand *cmd);
void clear_next_cm_stage_id(struct CourseCommand *cmd);

BOOL (*cm_entry_if_funcs[])(struct CourseCommand *) =
{
    is_stage_complete,
    did_beat_stage_faster_than,
    entered_goal_has_type,
    NULL,
};

void (*cm_entry_then_funcs[])(struct CourseCommand *) =
{
    calc_stage_jump_distance,
    clear_next_cm_stage_id2,
    clear_next_cm_stage_id,
    NULL,
};

struct CourseCommand beginner_noex_cm_entries[] =
{
    { CMD_FLOOR, FLOOR_STAGE_ID, 201 },
    { CMD_IF, IF_FLOOR_CLEAR },
    { CMD_THEN, THEN_JUMP_FLOOR, 1 },
    { CMD_FLOOR, FLOOR_STAGE_ID, 202 },
    { CMD_IF, IF_FLOOR_CLEAR },
    { CMD_THEN, THEN_JUMP_FLOOR, 1 },
    { CMD_FLOOR, FLOOR_STAGE_ID, 203 },
    { CMD_IF, IF_FLOOR_CLEAR },
    { CMD_THEN, THEN_JUMP_FLOOR, 1 },
    { CMD_FLOOR, FLOOR_STAGE_ID, 204 },
    { CMD_IF, IF_FLOOR_CLEAR },
    { CMD_THEN, THEN_JUMP_FLOOR, 1 },
    { CMD_FLOOR, FLOOR_STAGE_ID, 205 },
    { CMD_IF, IF_FLOOR_CLEAR },
    { CMD_THEN, THEN_JUMP_FLOOR, 1 },
    { CMD_FLOOR, FLOOR_STAGE_ID, 206 },
    { CMD_IF, IF_FLOOR_CLEAR },
    { CMD_THEN, THEN_JUMP_FLOOR, 1 },
    { CMD_FLOOR, FLOOR_STAGE_ID, 207 },
    { CMD_IF, IF_FLOOR_CLEAR },
    { CMD_THEN, THEN_JUMP_FLOOR, 1 },
    { CMD_FLOOR, FLOOR_STAGE_ID, 208 },
    { CMD_IF, IF_FLOOR_CLEAR },
    { CMD_THEN, THEN_JUMP_FLOOR, 1 },
    { CMD_FLOOR, FLOOR_STAGE_ID, 209 },
    { CMD_IF, IF_FLOOR_CLEAR },
    { CMD_THEN, THEN_JUMP_FLOOR, 1 },
    { CMD_FLOOR, FLOOR_STAGE_ID, 210 },
    { CMD_IF, IF_FLOOR_CLEAR },
    { CMD_THEN, THEN_CLEAR_NEXT_STAGE, 0 },
    { CMD_COURSE_END },
};

struct CourseCommand advanced_noex_cm_entries[] =
{
    { CMD_FLOOR, FLOOR_STAGE_ID, 221 },
    { CMD_IF, IF_FLOOR_CLEAR },
    { CMD_THEN, THEN_JUMP_FLOOR, 1 },
    { CMD_FLOOR, FLOOR_STAGE_ID, 222 },
    { CMD_IF, IF_FLOOR_CLEAR },
    { CMD_THEN, THEN_JUMP_FLOOR, 1 },
    { CMD_FLOOR, FLOOR_STAGE_ID, 223 },
    { CMD_IF, IF_FLOOR_CLEAR },
    { CMD_THEN, THEN_JUMP_FLOOR, 1 },
    { CMD_FLOOR, FLOOR_STAGE_ID, 224 },
    { CMD_IF, IF_FLOOR_CLEAR },
    { CMD_THEN, THEN_JUMP_FLOOR, 1 },
    { CMD_FLOOR, FLOOR_STAGE_ID, 225 },
    { CMD_IF, IF_FLOOR_CLEAR },
    { CMD_THEN, THEN_JUMP_FLOOR, 1 },
    { CMD_FLOOR, FLOOR_STAGE_ID, 226 },
    { CMD_IF, IF_FLOOR_CLEAR },
    { CMD_THEN, THEN_JUMP_FLOOR, 1 },
    { CMD_FLOOR, FLOOR_STAGE_ID, 227 },
    { CMD_IF, IF_FLOOR_CLEAR },
    { CMD_THEN, THEN_JUMP_FLOOR, 1 },
    { CMD_FLOOR, FLOOR_STAGE_ID, 228 },
    { CMD_IF, IF_FLOOR_CLEAR },
    { CMD_IF, IF_GOAL_TYPE },
    { CMD_THEN, THEN_JUMP_FLOOR, 1 },
    { CMD_IF, IF_FLOOR_CLEAR },
    { CMD_IF, IF_GOAL_TYPE, 1 },
    { CMD_THEN, THEN_JUMP_FLOOR, 2 },
    { CMD_IF, IF_FLOOR_CLEAR },
    { CMD_IF, IF_GOAL_TYPE, 2 },
    { CMD_THEN, THEN_JUMP_FLOOR, 3 },
    { CMD_FLOOR, FLOOR_STAGE_ID, 229 },
    { CMD_IF, IF_FLOOR_CLEAR },
    { CMD_THEN, THEN_JUMP_FLOOR, 1 },
    { CMD_FLOOR, FLOOR_STAGE_ID, 230 },
    { CMD_IF, IF_FLOOR_CLEAR },
    { CMD_THEN, THEN_JUMP_FLOOR, 1 },
    { CMD_FLOOR, FLOOR_STAGE_ID, 231 },
    { CMD_IF, IF_FLOOR_CLEAR },
    { CMD_THEN, THEN_JUMP_FLOOR, 1 },
    { CMD_FLOOR, FLOOR_STAGE_ID, 232 },
    { CMD_IF, IF_FLOOR_CLEAR },
    { CMD_THEN, THEN_JUMP_FLOOR, 1 },
    { CMD_FLOOR, FLOOR_STAGE_ID, 233 },
    { CMD_IF, IF_FLOOR_CLEAR },
    { CMD_THEN, THEN_JUMP_FLOOR, 1 },
    { CMD_FLOOR, FLOOR_STAGE_ID, 234 },
    { CMD_IF, IF_FLOOR_CLEAR },
    { CMD_THEN, THEN_JUMP_FLOOR, 1 },
    { CMD_FLOOR, FLOOR_STAGE_ID, 235 },
    { CMD_IF, IF_FLOOR_CLEAR },
    { CMD_THEN, THEN_JUMP_FLOOR, 1 },
    { CMD_FLOOR, FLOOR_STAGE_ID, 236 },
    { CMD_IF, IF_FLOOR_CLEAR },
    { CMD_IF, IF_GOAL_TYPE },
    { CMD_THEN, THEN_JUMP_FLOOR, 1 },
    { CMD_IF, IF_FLOOR_CLEAR },
    { CMD_IF, IF_GOAL_TYPE, 1 },
    { CMD_THEN, THEN_JUMP_FLOOR, 2 },
    { CMD_IF, IF_FLOOR_CLEAR },
    { CMD_IF, IF_GOAL_TYPE, 2 },
    { CMD_THEN, THEN_JUMP_FLOOR, 3 },
    { CMD_FLOOR, FLOOR_STAGE_ID, 237 },
    { CMD_IF, IF_FLOOR_CLEAR },
    { CMD_THEN, THEN_JUMP_FLOOR, 1 },
    { CMD_FLOOR, FLOOR_STAGE_ID, 238 },
    { CMD_IF, IF_FLOOR_CLEAR },
    { CMD_THEN, THEN_JUMP_FLOOR, 1 },
    { CMD_FLOOR, FLOOR_STAGE_ID, 239 },
    { CMD_IF, IF_FLOOR_CLEAR },
    { CMD_THEN, THEN_JUMP_FLOOR, 1 },
    { CMD_FLOOR, FLOOR_STAGE_ID, 240 },
    { CMD_IF, IF_FLOOR_CLEAR },
    { CMD_THEN, THEN_JUMP_FLOOR, 1 },
    { CMD_FLOOR, FLOOR_STAGE_ID, 241 },
    { CMD_IF, IF_FLOOR_CLEAR },
    { CMD_IF, IF_GOAL_TYPE },
    { CMD_THEN, THEN_JUMP_FLOOR, 1 },
    { CMD_IF, IF_FLOOR_CLEAR },
    { CMD_IF, IF_GOAL_TYPE, 1 },
    { CMD_THEN, THEN_JUMP_FLOOR, 2 },
    { CMD_FLOOR, FLOOR_STAGE_ID, 242 },
    { CMD_IF, IF_FLOOR_CLEAR },
    { CMD_THEN, THEN_JUMP_FLOOR, 1 },
    { CMD_FLOOR, FLOOR_STAGE_ID, 243 },
    { CMD_IF, IF_FLOOR_CLEAR },
    { CMD_THEN, THEN_JUMP_FLOOR, 1 },
    { CMD_FLOOR, FLOOR_STAGE_ID, 244 },
    { CMD_IF, IF_FLOOR_CLEAR },
    { CMD_THEN, THEN_JUMP_FLOOR, 1 },
    { CMD_FLOOR, FLOOR_STAGE_ID, 245 },
    { CMD_IF, IF_FLOOR_CLEAR },
    { CMD_THEN, THEN_JUMP_FLOOR, 1 },
    { CMD_FLOOR, FLOOR_STAGE_ID, 246 },
    { CMD_IF, IF_FLOOR_CLEAR },
    { CMD_IF, IF_GOAL_TYPE },
    { CMD_THEN, THEN_JUMP_FLOOR, 1 },
    { CMD_IF, IF_FLOOR_CLEAR },
    { CMD_IF, IF_GOAL_TYPE, 1 },
    { CMD_THEN, THEN_JUMP_FLOOR, 2 },
    { CMD_FLOOR, FLOOR_STAGE_ID, 247 },
    { CMD_IF, IF_FLOOR_CLEAR },
    { CMD_IF, IF_GOAL_TYPE },
    { CMD_THEN, THEN_JUMP_FLOOR, 1 },
    { CMD_IF, IF_FLOOR_CLEAR },
    { CMD_IF, IF_GOAL_TYPE, 1 },
    { CMD_THEN, THEN_JUMP_FLOOR, 2 },
    { CMD_IF, IF_FLOOR_CLEAR },
    { CMD_IF, IF_GOAL_TYPE, 2 },
    { CMD_THEN, THEN_JUMP_FLOOR, 3 },
    { CMD_FLOOR, FLOOR_STAGE_ID, 248 },
    { CMD_IF, IF_FLOOR_CLEAR },
    { CMD_THEN, THEN_JUMP_FLOOR, 1 },
    { CMD_FLOOR, FLOOR_STAGE_ID, 249 },
    { CMD_IF, IF_FLOOR_CLEAR },
    { CMD_THEN, THEN_JUMP_FLOOR, 1 },
    { CMD_FLOOR, FLOOR_STAGE_ID, 250 },
    { CMD_IF, IF_FLOOR_CLEAR },
    { CMD_THEN, THEN_CLEAR_NEXT_STAGE, 0 },
    { CMD_COURSE_END },
};

struct CourseCommand expert_noex_cm_entries[] =
{
    { CMD_FLOOR, FLOOR_STAGE_ID, 261 },
    { CMD_IF, IF_FLOOR_CLEAR },
    { CMD_THEN, THEN_JUMP_FLOOR, 1 },
    { CMD_FLOOR, FLOOR_STAGE_ID, 262 },
    { CMD_IF, IF_FLOOR_CLEAR },
    { CMD_IF, IF_GOAL_TYPE },
    { CMD_THEN, THEN_JUMP_FLOOR, 1 },
    { CMD_IF, IF_FLOOR_CLEAR },
    { CMD_IF, IF_GOAL_TYPE, 1 },
    { CMD_THEN, THEN_JUMP_FLOOR, 2 },
    { CMD_FLOOR, FLOOR_STAGE_ID, 263 },
    { CMD_FLOOR, FLOOR_TIME, 1800 },
    { CMD_IF, IF_FLOOR_CLEAR },
    { CMD_THEN, THEN_JUMP_FLOOR, 1 },
    { CMD_FLOOR, FLOOR_STAGE_ID, 264 },
    { CMD_IF, IF_FLOOR_CLEAR },
    { CMD_THEN, THEN_JUMP_FLOOR, 1 },
    { CMD_FLOOR, FLOOR_STAGE_ID, 265 },
    { CMD_IF, IF_FLOOR_CLEAR },
    { CMD_THEN, THEN_JUMP_FLOOR, 1 },
    { CMD_FLOOR, FLOOR_STAGE_ID, 266 },
    { CMD_IF, IF_FLOOR_CLEAR },
    { CMD_IF, IF_GOAL_TYPE },
    { CMD_THEN, THEN_JUMP_FLOOR, 1 },
    { CMD_IF, IF_FLOOR_CLEAR },
    { CMD_IF, IF_GOAL_TYPE, 1 },
    { CMD_THEN, THEN_JUMP_FLOOR, 2 },
    { CMD_IF, IF_FLOOR_CLEAR },
    { CMD_IF, IF_GOAL_TYPE, 2 },
    { CMD_THEN, THEN_JUMP_FLOOR, 3 },
    { CMD_FLOOR, FLOOR_STAGE_ID, 267 },
    { CMD_IF, IF_FLOOR_CLEAR },
    { CMD_THEN, THEN_JUMP_FLOOR, 1 },
    { CMD_FLOOR, FLOOR_STAGE_ID, 268 },
    { CMD_IF, IF_FLOOR_CLEAR },
    { CMD_THEN, THEN_JUMP_FLOOR, 1 },
    { CMD_FLOOR, FLOOR_STAGE_ID, 269 },
    { CMD_IF, IF_FLOOR_CLEAR },
    { CMD_THEN, THEN_JUMP_FLOOR, 1 },
    { CMD_FLOOR, FLOOR_STAGE_ID, 270 },
    { CMD_IF, IF_FLOOR_CLEAR },
    { CMD_THEN, THEN_JUMP_FLOOR, 1 },
    { CMD_FLOOR, FLOOR_STAGE_ID, 271 },
    { CMD_IF, IF_FLOOR_CLEAR },
    { CMD_THEN, THEN_JUMP_FLOOR, 1 },
    { CMD_FLOOR, FLOOR_STAGE_ID, 272 },
    { CMD_IF, IF_FLOOR_CLEAR },
    { CMD_IF, IF_GOAL_TYPE },
    { CMD_THEN, THEN_JUMP_FLOOR, 1 },
    { CMD_IF, IF_FLOOR_CLEAR },
    { CMD_IF, IF_GOAL_TYPE, 1 },
    { CMD_THEN, THEN_JUMP_FLOOR, 2 },
    { CMD_FLOOR, FLOOR_STAGE_ID, 273 },
    { CMD_IF, IF_FLOOR_CLEAR },
    { CMD_THEN, THEN_JUMP_FLOOR, 1 },
    { CMD_FLOOR, FLOOR_STAGE_ID, 274 },
    { CMD_IF, IF_FLOOR_CLEAR },
    { CMD_THEN, THEN_JUMP_FLOOR, 1 },
    { CMD_FLOOR, FLOOR_STAGE_ID, 275 },
    { CMD_IF, IF_FLOOR_CLEAR },
    { CMD_IF, IF_GOAL_TYPE },
    { CMD_THEN, THEN_JUMP_FLOOR, 1 },
    { CMD_IF, IF_FLOOR_CLEAR },
    { CMD_IF, IF_GOAL_TYPE, 1 },
    { CMD_THEN, THEN_JUMP_FLOOR, 2 },
    { CMD_IF, IF_FLOOR_CLEAR },
    { CMD_IF, IF_GOAL_TYPE, 2 },
    { CMD_THEN, THEN_JUMP_FLOOR, 3 },
    { CMD_FLOOR, FLOOR_STAGE_ID, 276 },
    { CMD_IF, IF_FLOOR_CLEAR },
    { CMD_THEN, THEN_JUMP_FLOOR, 1 },
    { CMD_FLOOR, FLOOR_STAGE_ID, 277 },
    { CMD_IF, IF_FLOOR_CLEAR },
    { CMD_THEN, THEN_JUMP_FLOOR, 1 },
    { CMD_FLOOR, FLOOR_STAGE_ID, 278 },
    { CMD_IF, IF_FLOOR_CLEAR },
    { CMD_IF, IF_GOAL_TYPE },
    { CMD_THEN, THEN_JUMP_FLOOR, 1 },
    { CMD_IF, IF_FLOOR_CLEAR },
    { CMD_IF, IF_GOAL_TYPE, 1 },
    { CMD_THEN, THEN_JUMP_FLOOR, 2 },
    { CMD_FLOOR, FLOOR_STAGE_ID, 279 },
    { CMD_IF, IF_FLOOR_CLEAR },
    { CMD_THEN, THEN_JUMP_FLOOR, 1 },
    { CMD_FLOOR, FLOOR_STAGE_ID, 280 },
    { CMD_IF, IF_FLOOR_CLEAR },
    { CMD_THEN, THEN_JUMP_FLOOR, 1 },
    { CMD_FLOOR, FLOOR_STAGE_ID, 281 },
    { CMD_IF, IF_FLOOR_CLEAR },
    { CMD_THEN, THEN_JUMP_FLOOR, 1 },
    { CMD_FLOOR, FLOOR_STAGE_ID, 282 },
    { CMD_IF, IF_FLOOR_CLEAR },
    { CMD_THEN, THEN_JUMP_FLOOR, 1 },
    { CMD_FLOOR, FLOOR_STAGE_ID, 283 },
    { CMD_IF, IF_FLOOR_CLEAR },
    { CMD_THEN, THEN_JUMP_FLOOR, 1 },
    { CMD_FLOOR, FLOOR_STAGE_ID, 284 },
    { CMD_IF, IF_FLOOR_CLEAR },
    { CMD_THEN, THEN_JUMP_FLOOR, 1 },
    { CMD_FLOOR, FLOOR_STAGE_ID, 285 },
    { CMD_IF, IF_FLOOR_CLEAR },
    { CMD_THEN, THEN_JUMP_FLOOR, 1 },
    { CMD_FLOOR, FLOOR_STAGE_ID, 286 },
    { CMD_IF, IF_FLOOR_CLEAR },
    { CMD_THEN, THEN_JUMP_FLOOR, 1 },
    { CMD_FLOOR, FLOOR_STAGE_ID, 287 },
    { CMD_IF, IF_FLOOR_CLEAR },
    { CMD_THEN, THEN_JUMP_FLOOR, 1 },
    { CMD_FLOOR, FLOOR_STAGE_ID, 288 },
    { CMD_IF, IF_FLOOR_CLEAR },
    { CMD_THEN, THEN_JUMP_FLOOR, 1 },
    { CMD_FLOOR, FLOOR_STAGE_ID, 289 },
    { CMD_IF, IF_FLOOR_CLEAR },
    { CMD_IF, IF_GOAL_TYPE },
    { CMD_THEN, THEN_JUMP_FLOOR, 1 },
    { CMD_IF, IF_FLOOR_CLEAR },
    { CMD_IF, IF_GOAL_TYPE, 1 },
    { CMD_THEN, THEN_JUMP_FLOOR, 2 },
    { CMD_IF, IF_FLOOR_CLEAR },
    { CMD_IF, IF_GOAL_TYPE, 2 },
    { CMD_THEN, THEN_JUMP_FLOOR, 3 },
    { CMD_FLOOR, FLOOR_STAGE_ID, 290 },
    { CMD_IF, IF_FLOOR_CLEAR },
    { CMD_THEN, THEN_JUMP_FLOOR, 1 },
    { CMD_FLOOR, FLOOR_STAGE_ID, 291 },
    { CMD_IF, IF_FLOOR_CLEAR },
    { CMD_THEN, THEN_JUMP_FLOOR, 1 },
    { CMD_FLOOR, FLOOR_STAGE_ID, 292 },
    { CMD_IF, IF_FLOOR_CLEAR },
    { CMD_THEN, THEN_JUMP_FLOOR, 1 },
    { CMD_FLOOR, FLOOR_STAGE_ID, 293 },
    { CMD_IF, IF_FLOOR_CLEAR },
    { CMD_THEN, THEN_JUMP_FLOOR, 1 },
    { CMD_FLOOR, FLOOR_STAGE_ID, 294 },
    { CMD_IF, IF_FLOOR_CLEAR },
    { CMD_IF, IF_GOAL_TYPE },
    { CMD_THEN, THEN_JUMP_FLOOR, 1 },
    { CMD_IF, IF_FLOOR_CLEAR },
    { CMD_IF, IF_GOAL_TYPE, 1 },
    { CMD_THEN, THEN_JUMP_FLOOR, 2 },
    { CMD_FLOOR, FLOOR_STAGE_ID, 295 },
    { CMD_IF, IF_FLOOR_CLEAR },
    { CMD_THEN, THEN_JUMP_FLOOR, 1 },
    { CMD_FLOOR, FLOOR_STAGE_ID, 296 },
    { CMD_IF, IF_FLOOR_CLEAR },
    { CMD_IF, IF_GOAL_TYPE },
    { CMD_THEN, THEN_JUMP_FLOOR, 1 },
    { CMD_IF, IF_FLOOR_CLEAR },
    { CMD_IF, IF_GOAL_TYPE, 1 },
    { CMD_THEN, THEN_JUMP_FLOOR, 2 },
    { CMD_FLOOR, FLOOR_STAGE_ID, 297 },
    { CMD_IF, IF_FLOOR_CLEAR },
    { CMD_IF, IF_GOAL_TYPE },
    { CMD_THEN, THEN_JUMP_FLOOR, 1 },
    { CMD_IF, IF_FLOOR_CLEAR },
    { CMD_IF, IF_GOAL_TYPE, 1 },
    { CMD_THEN, THEN_JUMP_FLOOR, 2 },
    { CMD_FLOOR, FLOOR_STAGE_ID, 298 },
    { CMD_IF, IF_FLOOR_CLEAR },
    { CMD_THEN, THEN_JUMP_FLOOR, 1 },
    { CMD_FLOOR, FLOOR_STAGE_ID, 299 },
    { CMD_IF, IF_FLOOR_CLEAR },
    { CMD_THEN, THEN_JUMP_FLOOR, 1 },
    { CMD_FLOOR, FLOOR_STAGE_ID, 300 },
    { CMD_IF, IF_FLOOR_CLEAR },
    { CMD_THEN, THEN_JUMP_FLOOR, 1 },
    { CMD_FLOOR, FLOOR_STAGE_ID, 301 },
    { CMD_IF, IF_FLOOR_CLEAR },
    { CMD_IF, IF_GOAL_TYPE },
    { CMD_THEN, THEN_JUMP_FLOOR, 1 },
    { CMD_IF, IF_FLOOR_CLEAR },
    { CMD_IF, IF_GOAL_TYPE, 1 },
    { CMD_THEN, THEN_JUMP_FLOOR, 2 },
    { CMD_IF, IF_FLOOR_CLEAR },
    { CMD_IF, IF_GOAL_TYPE, 2 },
    { CMD_THEN, THEN_JUMP_FLOOR, 3 },
    { CMD_FLOOR, FLOOR_STAGE_ID, 302 },
    { CMD_IF, IF_FLOOR_CLEAR },
    { CMD_THEN, THEN_JUMP_FLOOR, 1 },
    { CMD_FLOOR, FLOOR_STAGE_ID, 303 },
    { CMD_IF, IF_FLOOR_CLEAR },
    { CMD_THEN, THEN_JUMP_FLOOR, 1 },
    { CMD_FLOOR, FLOOR_STAGE_ID, 304 },
    { CMD_IF, IF_FLOOR_CLEAR },
    { CMD_IF, IF_GOAL_TYPE },
    { CMD_THEN, THEN_JUMP_FLOOR, 1 },
    { CMD_IF, IF_FLOOR_CLEAR },
    { CMD_IF, IF_GOAL_TYPE, 1 },
    { CMD_THEN, THEN_JUMP_FLOOR, 2 },
    { CMD_FLOOR, FLOOR_STAGE_ID, 305 },
    { CMD_IF, IF_FLOOR_CLEAR },
    { CMD_THEN, THEN_JUMP_FLOOR, 1 },
    { CMD_FLOOR, FLOOR_STAGE_ID, 306 },
    { CMD_IF, IF_FLOOR_CLEAR },
    { CMD_THEN, THEN_JUMP_FLOOR, 1 },
    { CMD_FLOOR, FLOOR_STAGE_ID, 307 },
    { CMD_IF, IF_FLOOR_CLEAR },
    { CMD_THEN, THEN_JUMP_FLOOR, 1 },
    { CMD_FLOOR, FLOOR_STAGE_ID, 308 },
    { CMD_IF, IF_FLOOR_CLEAR },
    { CMD_THEN, THEN_JUMP_FLOOR, 1 },
    { CMD_FLOOR, FLOOR_STAGE_ID, 309 },
    { CMD_IF, IF_FLOOR_CLEAR },
    { CMD_THEN, THEN_JUMP_FLOOR, 1 },
    { CMD_FLOOR, FLOOR_STAGE_ID, 310 },
    { CMD_IF, IF_FLOOR_CLEAR },
    { CMD_THEN, THEN_CLEAR_NEXT_STAGE, 0 },
    { CMD_COURSE_END },
};

struct CourseCommand beginner_ex_cm_entries[] =
{
    { CMD_FLOOR, FLOOR_STAGE_ID, 211 },
    { CMD_IF, IF_FLOOR_CLEAR },
    { CMD_THEN, THEN_JUMP_FLOOR, 1 },
    { CMD_FLOOR, FLOOR_STAGE_ID, 212 },
    { CMD_IF, IF_FLOOR_CLEAR },
    { CMD_THEN, THEN_JUMP_FLOOR, 1 },
    { CMD_FLOOR, FLOOR_STAGE_ID, 213 },
    { CMD_IF, IF_FLOOR_CLEAR },
    { CMD_THEN, THEN_JUMP_FLOOR, 1 },
    { CMD_FLOOR, FLOOR_STAGE_ID, 214 },
    { CMD_IF, IF_FLOOR_CLEAR },
    { CMD_IF, IF_GOAL_TYPE },
    { CMD_THEN, THEN_JUMP_FLOOR, 1 },
    { CMD_IF, IF_FLOOR_CLEAR },
    { CMD_IF, IF_GOAL_TYPE, 1 },
    { CMD_THEN, THEN_JUMP_FLOOR, 2 },
    { CMD_FLOOR, FLOOR_STAGE_ID, 215 },
    { CMD_IF, IF_FLOOR_CLEAR },
    { CMD_THEN, THEN_JUMP_FLOOR, 1 },
    { CMD_FLOOR, FLOOR_STAGE_ID, 216 },
    { CMD_IF, IF_FLOOR_CLEAR },
    { CMD_THEN, THEN_JUMP_FLOOR, 1 },
    { CMD_FLOOR, FLOOR_STAGE_ID, 217 },
    { CMD_IF, IF_FLOOR_CLEAR },
    { CMD_THEN, THEN_JUMP_FLOOR, 1 },
    { CMD_FLOOR, FLOOR_STAGE_ID, 218 },
    { CMD_IF, IF_FLOOR_CLEAR },
    { CMD_THEN, THEN_JUMP_FLOOR, 1 },
    { CMD_FLOOR, FLOOR_STAGE_ID, 219 },
    { CMD_IF, IF_FLOOR_CLEAR },
    { CMD_THEN, THEN_JUMP_FLOOR, 1 },
    { CMD_FLOOR, FLOOR_STAGE_ID, 220 },
    { CMD_IF, IF_FLOOR_CLEAR },
    { CMD_THEN, THEN_CLEAR_NEXT_STAGE, 0 },
    { CMD_COURSE_END },
};

struct CourseCommand advanced_ex_cm_entries[] =
{
    { CMD_FLOOR, FLOOR_STAGE_ID, 251 },
    { CMD_IF, IF_FLOOR_CLEAR },
    { CMD_THEN, THEN_JUMP_FLOOR, 1 },
    { CMD_FLOOR, FLOOR_STAGE_ID, 252 },
    { CMD_IF, IF_FLOOR_CLEAR },
    { CMD_THEN, THEN_JUMP_FLOOR, 1 },
    { CMD_FLOOR, FLOOR_STAGE_ID, 253 },
    { CMD_IF, IF_FLOOR_CLEAR },
    { CMD_THEN, THEN_JUMP_FLOOR, 1 },
    { CMD_FLOOR, FLOOR_STAGE_ID, 254 },
    { CMD_IF, IF_FLOOR_CLEAR },
    { CMD_THEN, THEN_JUMP_FLOOR, 1 },
    { CMD_FLOOR, FLOOR_STAGE_ID, 255 },
    { CMD_IF, IF_FLOOR_CLEAR },
    { CMD_THEN, THEN_JUMP_FLOOR, 1 },
    { CMD_FLOOR, FLOOR_STAGE_ID, 256 },
    { CMD_IF, IF_FLOOR_CLEAR },
    { CMD_THEN, THEN_JUMP_FLOOR, 1 },
    { CMD_FLOOR, FLOOR_STAGE_ID, 257 },
    { CMD_IF, IF_FLOOR_CLEAR },
    { CMD_THEN, THEN_JUMP_FLOOR, 1 },
    { CMD_FLOOR, FLOOR_STAGE_ID, 258 },
    { CMD_IF, IF_FLOOR_CLEAR },
    { CMD_THEN, THEN_JUMP_FLOOR, 1 },
    { CMD_FLOOR, FLOOR_STAGE_ID, 259 },
    { CMD_IF, IF_FLOOR_CLEAR },
    { CMD_THEN, THEN_JUMP_FLOOR, 1 },
    { CMD_FLOOR, FLOOR_STAGE_ID, 260 },
    { CMD_FLOOR, FLOOR_TIME, 1800 },
    { CMD_IF, IF_FLOOR_CLEAR },
    { CMD_THEN, THEN_CLEAR_NEXT_STAGE, 0 },
    { CMD_COURSE_END },
};

struct CourseCommand expert_ex_cm_entries[] =
{
    { CMD_FLOOR, FLOOR_STAGE_ID, 311 },
    { CMD_IF, IF_FLOOR_CLEAR },
    { CMD_THEN, THEN_JUMP_FLOOR, 1 },
    { CMD_FLOOR, FLOOR_STAGE_ID, 312 },
    { CMD_IF, IF_FLOOR_CLEAR },
    { CMD_IF, IF_GOAL_TYPE },
    { CMD_THEN, THEN_JUMP_FLOOR, 1 },
    { CMD_IF, IF_FLOOR_CLEAR },
    { CMD_IF, IF_GOAL_TYPE, 1 },
    { CMD_THEN, THEN_JUMP_FLOOR, 2 },
    { CMD_FLOOR, FLOOR_STAGE_ID, 313 },
    { CMD_IF, IF_FLOOR_CLEAR },
    { CMD_THEN, THEN_JUMP_FLOOR, 1 },
    { CMD_FLOOR, FLOOR_STAGE_ID, 314 },
    { CMD_IF, IF_FLOOR_CLEAR },
    { CMD_THEN, THEN_JUMP_FLOOR, 1 },
    { CMD_FLOOR, FLOOR_STAGE_ID, 315 },
    { CMD_IF, IF_FLOOR_CLEAR },
    { CMD_THEN, THEN_JUMP_FLOOR, 1 },
    { CMD_FLOOR, FLOOR_STAGE_ID, 316 },
    { CMD_IF, IF_FLOOR_CLEAR },
    { CMD_IF, IF_GOAL_TYPE },
    { CMD_THEN, THEN_JUMP_FLOOR, 1 },
    { CMD_IF, IF_FLOOR_CLEAR },
    { CMD_IF, IF_GOAL_TYPE, 1 },
    { CMD_THEN, THEN_JUMP_FLOOR, 2 },
    { CMD_IF, IF_FLOOR_CLEAR },
    { CMD_IF, IF_GOAL_TYPE, 2 },
    { CMD_THEN, THEN_JUMP_FLOOR, 3 },
    { CMD_FLOOR, FLOOR_STAGE_ID, 317 },
    { CMD_IF, IF_FLOOR_CLEAR },
    { CMD_THEN, THEN_JUMP_FLOOR, 1 },
    { CMD_FLOOR, FLOOR_STAGE_ID, 318 },
    { CMD_IF, IF_FLOOR_CLEAR },
    { CMD_THEN, THEN_JUMP_FLOOR, 1 },
    { CMD_FLOOR, FLOOR_STAGE_ID, 319 },
    { CMD_IF, IF_FLOOR_CLEAR },
    { CMD_THEN, THEN_JUMP_FLOOR, 1 },
    { CMD_FLOOR, FLOOR_STAGE_ID, 320 },
    { CMD_IF, IF_FLOOR_CLEAR },
    { CMD_THEN, THEN_CLEAR_NEXT_STAGE, 0 },
    { CMD_COURSE_END },
};

struct CourseCommand master_noex_cm_entries[] =
{
    { CMD_FLOOR, FLOOR_STAGE_ID, 321 },
    { CMD_IF, IF_FLOOR_CLEAR },
    { CMD_THEN, THEN_JUMP_FLOOR, 1 },
    { CMD_FLOOR, FLOOR_STAGE_ID, 322 },
    { CMD_IF, IF_FLOOR_CLEAR },
    { CMD_IF, IF_GOAL_TYPE },
    { CMD_THEN, THEN_JUMP_FLOOR, 1 },
    { CMD_IF, IF_FLOOR_CLEAR },
    { CMD_IF, IF_GOAL_TYPE, 1 },
    { CMD_THEN, THEN_JUMP_FLOOR, 2 },
    { CMD_FLOOR, FLOOR_STAGE_ID, 323 },
    { CMD_IF, IF_FLOOR_CLEAR },
    { CMD_THEN, THEN_JUMP_FLOOR, 1 },
    { CMD_FLOOR, FLOOR_STAGE_ID, 324 },
    { CMD_IF, IF_FLOOR_CLEAR },
    { CMD_THEN, THEN_JUMP_FLOOR, 1 },
    { CMD_FLOOR, FLOOR_STAGE_ID, 325 },
    { CMD_IF, IF_FLOOR_CLEAR },
    { CMD_THEN, THEN_JUMP_FLOOR, 1 },
    { CMD_FLOOR, FLOOR_STAGE_ID, 326 },
    { CMD_IF, IF_FLOOR_CLEAR },
    { CMD_THEN, THEN_JUMP_FLOOR, 1 },
    { CMD_FLOOR, FLOOR_STAGE_ID, 327 },
    { CMD_IF, IF_FLOOR_CLEAR },
    { CMD_THEN, THEN_JUMP_FLOOR, 1 },
    { CMD_FLOOR, FLOOR_STAGE_ID, 328 },
    { CMD_IF, IF_FLOOR_CLEAR },
    { CMD_THEN, THEN_JUMP_FLOOR, 1 },
    { CMD_FLOOR, FLOOR_STAGE_ID, 329 },
    { CMD_IF, IF_FLOOR_CLEAR },
    { CMD_THEN, THEN_JUMP_FLOOR, 1 },
    { CMD_FLOOR, FLOOR_STAGE_ID, 330 },
    { CMD_IF, IF_FLOOR_CLEAR },
    { CMD_THEN, THEN_CLEAR_NEXT_STAGE, 0 },
    { CMD_COURSE_END },
};

struct CourseCommand master_ex_cm_entries[] =
{
    { CMD_FLOOR, FLOOR_STAGE_ID, 331 },
    { CMD_IF, IF_FLOOR_CLEAR },
    { CMD_THEN, THEN_JUMP_FLOOR, 1 },
    { CMD_FLOOR, FLOOR_STAGE_ID, 332 },
    { CMD_IF, IF_FLOOR_CLEAR },
    { CMD_IF, IF_GOAL_TYPE },
    { CMD_THEN, THEN_JUMP_FLOOR, 1 },
    { CMD_IF, IF_FLOOR_CLEAR },
    { CMD_IF, IF_GOAL_TYPE, 1 },
    { CMD_THEN, THEN_JUMP_FLOOR, 2 },
    { CMD_IF, IF_FLOOR_CLEAR },
    { CMD_IF, IF_GOAL_TYPE, 2 },
    { CMD_THEN, THEN_JUMP_FLOOR, 3 },
    { CMD_FLOOR, FLOOR_STAGE_ID, 333 },
    { CMD_IF, IF_FLOOR_CLEAR },
    { CMD_THEN, THEN_JUMP_FLOOR, 1 },
    { CMD_FLOOR, FLOOR_STAGE_ID, 334 },
    { CMD_IF, IF_FLOOR_CLEAR },
    { CMD_IF, IF_GOAL_TYPE },
    { CMD_THEN, THEN_JUMP_FLOOR, 1 },
    { CMD_IF, IF_FLOOR_CLEAR },
    { CMD_IF, IF_GOAL_TYPE, 2 },
    { CMD_THEN, THEN_JUMP_FLOOR, 3 },
    { CMD_FLOOR, FLOOR_STAGE_ID, 335 },
    { CMD_IF, IF_FLOOR_CLEAR },
    { CMD_THEN, THEN_JUMP_FLOOR, 1 },
    { CMD_FLOOR, FLOOR_STAGE_ID, 336 },
    { CMD_IF, IF_FLOOR_CLEAR },
    { CMD_THEN, THEN_JUMP_FLOOR, 1 },
    { CMD_FLOOR, FLOOR_STAGE_ID, 337 },
    { CMD_IF, IF_FLOOR_CLEAR },
    { CMD_IF, IF_GOAL_TYPE },
    { CMD_THEN, THEN_JUMP_FLOOR, 1 },
    { CMD_IF, IF_FLOOR_CLEAR },
    { CMD_IF, IF_GOAL_TYPE, 1 },
    { CMD_THEN, THEN_JUMP_FLOOR, 2 },
    { CMD_FLOOR, FLOOR_STAGE_ID, 338 },
    { CMD_IF, IF_FLOOR_CLEAR },
    { CMD_IF, IF_GOAL_TYPE },
    { CMD_THEN, THEN_JUMP_FLOOR, 1 },
    { CMD_IF, IF_FLOOR_CLEAR },
    { CMD_IF, IF_GOAL_TYPE, 1 },
    { CMD_THEN, THEN_JUMP_FLOOR, 2 },
    { CMD_FLOOR, FLOOR_STAGE_ID, 339 },
    { CMD_IF, IF_FLOOR_CLEAR },
    { CMD_THEN, THEN_JUMP_FLOOR, 1 },
    { CMD_FLOOR, FLOOR_STAGE_ID, 340 },
    { CMD_IF, IF_FLOOR_CLEAR },
    { CMD_THEN, THEN_CLEAR_NEXT_STAGE, 0 },
    { CMD_COURSE_END },
};

struct CourseCommand g_cm_entry_list9[] =
{
    { CMD_FLOOR, FLOOR_STAGE_ID, 0 },
    { CMD_IF, IF_FLOOR_CLEAR },
    { CMD_THEN, THEN_JUMP_FLOOR, 1 },
    { CMD_FLOOR, FLOOR_STAGE_ID, 0 },
    { CMD_IF, IF_FLOOR_CLEAR },
    { CMD_THEN, THEN_JUMP_FLOOR, 1 },
    { CMD_FLOOR, FLOOR_STAGE_ID, 0 },
    { CMD_IF, IF_FLOOR_CLEAR },
    { CMD_THEN, THEN_JUMP_FLOOR, 1 },
    { CMD_FLOOR, FLOOR_STAGE_ID, 0 },
    { CMD_IF, IF_FLOOR_CLEAR },
    { CMD_THEN, THEN_JUMP_FLOOR, 1 },
    { CMD_FLOOR, FLOOR_STAGE_ID, 0 },
    { CMD_IF, IF_FLOOR_CLEAR },
    { CMD_THEN, THEN_CLEAR_NEXT_STAGE, 0 },
    { CMD_COURSE_END },
};

struct CourseCommand *cm_courses[] =
{
    beginner_noex_cm_entries,
    advanced_noex_cm_entries,
    expert_noex_cm_entries,
    beginner_ex_cm_entries,
    advanced_ex_cm_entries,
    expert_ex_cm_entries,
    master_noex_cm_entries,
    master_ex_cm_entries,
    master_ex_cm_entries,
    g_cm_entry_list9,
    g_cm_entry_list9,
    g_cm_entry_list9,
};

void clear_unlocked_cm_stages(void)
{
    int i;

    for (i = 0; i < 18; i++)
        cm_unlock_entries[i] = 0;
}

void init_course(void)
{
    current_cm_entry = cm_courses[calc_course_idx(modeCtrl.difficulty, modeCtrl.courseFlags)];
    mode_info.currStageId = current_cm_entry->value;
    current_cm_entry++;
    clear_cm_player_progress();
}

void event_course_init(void)
{
    stage_jump_distance = -1;
    update_course_progress();
    if (modeCtrl.gameType == GAMETYPE_MAIN_NORMAL)
        update_cm_unlocked_levels(modeCtrl.difficulty, mode_info.currFloor, modeCtrl.courseFlags);
}

void event_course_tick(void)
{
    int condResult;
    struct CourseCommand *cmd;
    int jumpCnt;
    int prevOpcode;

    if (g_some_other_flags & 0xA)
        return;
    cmd = current_cm_entry;
    prevOpcode = -1;
    condResult = 0;
    for (; cmd->opcode != CMD_COURSE_END; cmd++)
    {
        /* stop at the next floor's commands */
        if (cmd->opcode == CMD_FLOOR && cmd->type == FLOOR_STAGE_ID)
            return;

        switch (cmd->opcode)
        {
        case CMD_IF:
            if (prevOpcode != CMD_IF)
                condResult = cm_entry_if_funcs[cmd->type](cmd);
            else if (condResult != 0)
                condResult &= cm_entry_if_funcs[cmd->type](cmd);
            break;
        case CMD_THEN:
            if (condResult)
                cm_entry_then_funcs[cmd->type](cmd);
            break;
        }

        if (stage_jump_distance != -1)
        {
            condResult = FALSE;  // reused as "found" flag
            jumpCnt = 0;
            if (mode_info.unk22 != 1)
                stage_jump_distance = mode_info.unk22;
            while (cmd->opcode != CMD_COURSE_END)
            {
                if (cmd->opcode == CMD_FLOOR && cmd->type == FLOOR_STAGE_ID)
                {
                    jumpCnt++;
                    if (jumpCnt == stage_jump_distance)
                    {
                        condResult = TRUE;
                        break;
                    }
                }
                cmd++;
            }
            if (condResult)
                current_cm_entry = cmd + 1;
            else
            {
                clear_next_cm_stage_id(cmd);
                mode_info.currFloor += 10;
                stage_jump_distance = -1;
                return;
            }
            mode_info.currStageId = cmd->value;
            g_something_with_cm_player_progress();
            mode_info.unk22 = jumpCnt;
            if (modeCtrl.gameType != GAMETYPE_MAIN_PRACTICE)
                mode_info.currFloor += mode_info.unk22;
            stage_jump_distance = -1;
            return;
        }
        prevOpcode = cmd->opcode;
    }
}

void event_course_dest(void) {}

BOOL is_stage_complete(struct CourseCommand *cmd)
{
    if ((mode_info.flags & INFO_FLAG_GOAL) || (mode_info.flags & INFO_FLAG_BONUS_CLEAR))
        return TRUE;
    if (((mode_info.flags & INFO_FLAG_BONUS_STAGE) || modeCtrl.gameType == GAMETYPE_MAIN_COMPETITION)
     && ((mode_info.flags & INFO_FLAG_TIMEOVER) || (mode_info.flags & INFO_FLAG_FALLOUT)))
        return TRUE;
    return FALSE;
}

BOOL entered_goal_has_type(struct CourseCommand *cmd)
{
    int i;
    struct StageGoal *goal;

    goal = stagedef->collHeaders[0].goals;
    for (i = 0; i < 3; i++, goal++)
    {
        if (i == mode_info.goalEntered)
            break;
    }
    if (goal->type == (s8)cmd->value)
        return TRUE;
    return FALSE;
}

BOOL did_beat_stage_faster_than(struct CourseCommand *cmd)
{
    return mode_info.timerCurr >= cmd->value;
}

void calc_stage_jump_distance(struct CourseCommand *cmd)
{
    if (modeCtrl.gameType == GAMETYPE_MAIN_PRACTICE)
        stage_jump_distance = 1;
    else
        stage_jump_distance = mode_info.goalType + 1;
    if (current_stage_id == 0x14E)
        stage_jump_distance = cmd->value;
}

void clear_next_cm_stage_id2(struct CourseCommand *cmd)
{
    mode_info.currStageId = -1;
}

void clear_next_cm_stage_id(struct CourseCommand *cmd)
{
    mode_info.currStageId = -1;
}

int get_current_cm_stage_time_limit(void)
{
    int floorCnt;
    int idx;
    struct CourseCommand *cmd;

    if (in_practice_mode)
    {
        floorCnt = 0;
        idx = 0;
        if (g_stage_id_in_practice_mode[0].courseFlags & COURSE_FLAG_MASTER_EXTRA)
            cmd = cm_courses[7];
        else if (g_stage_id_in_practice_mode[0].courseFlags & COURSE_FLAG_MASTER)
            cmd = cm_courses[6];
        else
        {
            if (g_stage_id_in_practice_mode[0].courseFlags & COURSE_FLAG_EXTRA)
                idx = 3;
            cmd = cm_courses[g_stage_id_in_practice_mode[0].difficulty + idx];
        }
        while (cmd->opcode != CMD_COURSE_END)
        {
            if (cmd->opcode == CMD_FLOOR && cmd->type == FLOOR_STAGE_ID)
            {
                floorCnt++;
                if (floorCnt == g_stage_id_in_practice_mode[0].floor)
                {
                    if (cmd[1].opcode == CMD_FLOOR && cmd[1].type == FLOOR_TIME)
                        return cmd[1].value;
                    break;
                }
            }
            cmd++;
        }
        return 60 * 60;
    }
#ifndef __MWERKS__
    /* minigames reach here before any challenge course is set; on GC the
     * NULL read hit valid low memory */
    if (current_cm_entry == NULL)
        return 60 * 60;
#endif
    if (current_cm_entry->opcode == CMD_FLOOR && current_cm_entry->type == FLOOR_TIME)
        return current_cm_entry->value;
    return 60 * 60;
}

int g_update_cm_course(int difficulty, int floor, u32 flags)
{
    int stageId;

    current_cm_entry = cm_courses[calc_course_idx(difficulty, flags)];

    // Reuses stageId as the floor counter (matches regalloc)
    stageId = 1;
    while (stageId <= floor && current_cm_entry->opcode != CMD_COURSE_END)
    {
        if (current_cm_entry->opcode == CMD_FLOOR && current_cm_entry->type == FLOOR_STAGE_ID)
        {
            if (stageId == floor)
                break;
            stageId++;
        }
        current_cm_entry++;
    }

    stageId = current_cm_entry->value;
    if (modeCtrl.gameType == GAMETYPE_MAIN_COMPETITION)
    {
        if (stageId == 0x7E)
            stageId = 0x74;
        if (stageId == 0x7F)
            stageId = 0x73;
    }
    current_cm_entry++;
    return stageId;
}

int calc_course_idx(int difficulty, u32 flags)
{
    int idx = 0;

    if (flags & COURSE_FLAG_MASTER_EXTRA)
        return 7;
    if (flags & COURSE_FLAG_MASTER)
        return 6;
    if (flags & COURSE_FLAG_EXTRA)
        idx = 3;
    if (in_practice_mode)
        idx = 9;
    idx += difficulty;
    return idx;
}

int g_get_some_difficulty_count_2(int difficulty, u32 flags)
{
    int count;
    struct CourseCommand *cmd;

    if (in_practice_mode)
        return g_some_course_length;
    count = 0;
    for (cmd = cm_courses[calc_course_idx(difficulty, flags)]; cmd->opcode != CMD_COURSE_END; cmd++)
    {
        if (cmd->opcode == CMD_FLOOR && cmd->type == FLOOR_STAGE_ID)
            count++;
    }
    return count;
}

BOOL g_are_on_final_course_level(int difficulty, int floor, u32 flags)
{
    if (floor == g_get_some_difficulty_count_2(difficulty, flags))
        return TRUE;
    return FALSE;
}

BOOL is_bonus_stage(int stageId)
{
    BOOL isBonus;
    const volatile int *id;

    for (id = bonus_stage_id_list, isBonus = FALSE; *id != 0; id++)
    {
        if (*id == stageId)
        {
            isBonus = TRUE;
            break;
        }
    }
    return isBonus;
}

void g_something_with_cm_entries_practice_mode(void)
{
    int i;

    for (i = 0; i < g_some_course_length; i++)
    {
        g_cm_entry_list9[i * 3].value = g_update_cm_course(g_stage_id_in_practice_mode[i].difficulty, g_stage_id_in_practice_mode[i].floor, g_stage_id_in_practice_mode[i].courseFlags);
        g_cm_entry_list9[i * 3 + 2].type = FLOOR_STAGE_ID;
    }
    g_cm_entry_list9[(g_some_course_length - 1) * 3 + 2].type = 2;
}

int fn_1_A3C84(int difficulty, int floor, u32 flags)
{
    int floorCnt = 0;
    struct CourseCommand *cmd = cm_courses[calc_course_idx(difficulty, flags)];

    while (cmd->opcode != CMD_COURSE_END)
    {
        while (cmd->opcode != CMD_COURSE_END)
        {
            if (cmd->opcode == CMD_FLOOR && cmd->type == FLOOR_STAGE_ID)
            {
                floorCnt++;
                if (floorCnt - 1 == floor)
                    return cmd->value;
            }
            cmd++;
        }
    }
    return 0;
}

BOOL g_smth_with_cm_entries_in_main_menu(int difficulty, int floor, u32 flags)
{
    int bit = 0;
    int i;
    int count;

    if ((dip_switches & DIP_DEBUG) && (dip_switches & DIP_PLAY_STG_ALL))
        return TRUE;

    if (flags & COURSE_FLAG_MASTER_EXTRA)
        bit = 130;
    else if (flags & COURSE_FLAG_MASTER)
        bit = 120;
    else if (flags & COURSE_FLAG_EXTRA)
    {
        switch (difficulty)
        {
        case DIFFICULTY_BEGINNER:
            bit = 90;
            break;
        case DIFFICULTY_ADVANCED:
            bit = 100;
            break;
        case DIFFICULTY_EXPERT:
            bit = 110;
            break;
        }
    }
    else
    {
        switch (difficulty)
        {
        case DIFFICULTY_BEGINNER:
            bit = 0;
            break;
        case DIFFICULTY_ADVANCED:
            bit = 10;
            break;
        case DIFFICULTY_EXPERT:
            bit = 40;
            break;
        }
    }

    if (floor == 0)
    {
        count = g_get_some_difficulty_count_2(difficulty, flags);
        for (i = 0; i < count; i++)
        {
            if (cm_unlock_entries[(bit + i) / 8] & (1 << ((bit + i) % 8)))
                return TRUE;
        }
    }
    else
    {
        bit += floor - 1;
        if (cm_unlock_entries[bit / 8] & (1 << (bit % 8)))
            return TRUE;
    }
    return FALSE;
}

#pragma dont_inline on
void fn_1_A3FA0(int bit) {}
#pragma dont_inline off

void update_cm_unlocked_levels(int difficulty, int floor, u32 flags)
{
    int bit = 0;

    if (flags & COURSE_FLAG_MASTER_EXTRA)
        bit = 130;
    else if (flags & COURSE_FLAG_MASTER)
        bit = 120;
    else if (flags & COURSE_FLAG_EXTRA)
    {
        switch (difficulty)
        {
        case DIFFICULTY_BEGINNER:
            bit = 90;
            break;
        case DIFFICULTY_ADVANCED:
            bit = 100;
            break;
        case DIFFICULTY_EXPERT:
            bit = 110;
            break;
        }
    }
    else
    {
        switch (difficulty)
        {
        case DIFFICULTY_BEGINNER:
            bit = 0;
            break;
        case DIFFICULTY_ADVANCED:
            bit = 10;
            break;
        case DIFFICULTY_EXPERT:
            bit = 40;
            break;
        }
    }
    bit += floor - 1;
    cm_unlock_entries[bit / 8] |= 1 << (bit % 8);
}

void clear_cm_player_progress(void)
{
    int k;
    int j;
    int i;
    struct CourseProgress *rec;

    for (i = 0; i < 4; i++)
    {
        rec = cm_player_progress[i];
        for (j = 0; j < 3; j++, rec++)
        {
            rec->floor = 0;
            rec->stageId = -1;
            for (k = 0; k < 3; k++)
            {
                rec->jumps[k].floor = 0;
                rec->jumps[k].stageId = -1;
            }
            rec->jumpCount = -1;
            rec->jumpIdx = -1;
        }
    }
}

static void set_next_stage_id(void)
{
    int player;
    int next;
    struct CourseProgress *rec;

    player = modeCtrl.currPlayer;
    if (modeCtrl.gameType == GAMETYPE_MAIN_NORMAL)
        next = get_next_player_idx();
    else
        next = modeCtrl.currPlayer;
    rec = cm_player_progress[next];
    if (player == next)
        mode_info.unk32 = cm_player_progress[player][0].jumps[0].stageId;
    else if (rec->jumpIdx == -1)
        mode_info.unk32 = rec->stageId;
    else
        mode_info.unk32 = rec->jumps[rec->jumpIdx].stageId;
}

void update_course_progress(void)
{
    int i;
    int player;
    struct CourseProgress *rec;
    struct CourseCommand *p;
    struct CourseCommand *cmd;
    int stageId;
    int n;
    int next;

    player = modeCtrl.currPlayer;
    if (modeCtrl.gameType == GAMETYPE_MAIN_NORMAL)
        next = get_next_player_idx();
    else
        next = modeCtrl.currPlayer;

    if (mode_info.currFloor == cm_player_progress[player][0].floor)
    {
        set_next_stage_id();
        return;
    }

    // shift history records down by one
    i = 1;
    rec = &cm_player_progress[player][i];
    for (; i >= 0; i--, rec--)
    {
        if (rec->floor != 0)
            memcpy_handler(&rec[1], rec, sizeof(*rec));
    }

    rec = cm_player_progress[player];
    rec->floor = mode_info.currFloor;
    rec->stageId = current_stage_id;
    for (i = 0; i < 3; i++)
    {
        rec->jumps[i].floor = 0;
        rec->jumps[i].stageId = -1;
    }

    // collect the possible jumps from this floor's commands
    cmd = current_cm_entry;
    for (i = 0; i < 3 && cmd->opcode != CMD_COURSE_END; cmd++)
    {
        if (cmd->opcode == CMD_FLOOR && cmd->type == FLOOR_STAGE_ID)
            break;
        if (cmd->opcode == CMD_THEN)
        {
            switch (cmd->type)
            {
            case THEN_JUMP_FLOOR:
            default:
                rec->jumps[i].floor = mode_info.currFloor + cmd->value;
                n = cmd->value;
                p = current_cm_entry;
                stageId = -1;
                while (p->opcode != CMD_COURSE_END)
                {
                    if (p->opcode == CMD_FLOOR && p->type == FLOOR_STAGE_ID && --n <= 0)
                    {
                        stageId = p->value;
                        break;
                    }
                    p++;
                }
                rec->jumps[i].stageId = stageId;
                break;
            case THEN_CLEAR_NEXT_STAGE2:
            case THEN_CLEAR_NEXT_STAGE:
                rec->jumps[i].floor = -1;
                rec->jumps[i].stageId = -1;
                break;
            }
            i++;
        }
    }
    rec->jumpCount = i;
    rec->jumpIdx = -1;
    set_next_stage_id();
}

void g_something_with_cm_player_progress(void)
{
    int i;
    struct CourseProgress *rec;

    rec = cm_player_progress[modeCtrl.currPlayer];
    for (i = 0; i < 3; i++)
    {
        if (rec->jumps[i].stageId == mode_info.currStageId || mode_info.currStageId == -1)
        {
            rec->jumpIdx = i;
            if (mode_info.currStageId == -1)
                rec->jumps[rec->jumpIdx].stageId = -1;
            break;
        }
    }
    set_next_stage_id();
}

void sprite_debug_course_display_disp(struct Sprite *sprite)
{
    int digits;
    int i;
    int j;
    int x;
    int y;
    struct CourseProgress *rec;
    s32 unused = unused_course_pad[0];

    if (!(dip_switches & DIP_DISP))
        return;
    textdraw_reset();
    textdraw_set_font(sprite->fontId);
    textdraw_set_scale(sprite->scaleX, sprite->scaleY);
    j = 2;
    rec = &cm_player_progress[modeCtrl.currPlayer][j];
    x = sprite->x;
    y = sprite->y;
    for (; j >= 0; j--, rec--)
    {
        if (rec->floor == 0)
            continue;
        if (j == 0)
            textdraw_set_mul_color(0x00FF00);
        textdraw_set_pos(x, y);
        textdraw_printf("%d(%03d)", rec->floor, rec->stageId);
        if (j == 0)
            textdraw_set_mul_color(0xFFFFFF);
        i = rec->floor;
        digits = 6;
        while (i != 0)
        {
            i /= 10;
            if (i > 0)
                digits++;
        }
        for (i = 0; i < 3; i++)
        {
            if (j > 0 && i > 0)
                break;
            if (rec->jumps[i].floor == 0)
                break;
            textdraw_set_pos(x + sprite->scaleX * (digits * 8), y + sprite->scaleY * (i * 8));
            if (j > 0)
                textdraw_print("\x16>");
            else if (rec->jumpCount > 1 && i == 0)
                textdraw_print("\x13>");
            else if (rec->jumpCount > 1 && i == rec->jumpCount - 1)
                textdraw_print("\x1A>");
            else if (rec->jumpCount > 1)
                textdraw_print("\x11>");
            else
                textdraw_print("\x16>");
            if (j > 0)
                break;
            if (rec->jumps[i].floor == mode_info.currFloor && g_font_char_var_3 % 60 < 45)
                textdraw_set_mul_color(0x00FF00);
            textdraw_set_pos(16.0f * sprite->scaleX + (x + sprite->scaleX * (digits * 8)), y + sprite->scaleY * (i * 8));
            if (rec->jumps[i].stageId == -1)
                textdraw_print("CLEAR");
            else if (rec->jumps[i].stageId == -1)
                textdraw_print("END");
            else
                textdraw_printf("%d(%03d)", rec->jumps[i].floor, rec->jumps[i].stageId);
            textdraw_set_mul_color(0xFFFFFF);
        }
        x += 16.0f * sprite->scaleX + sprite->scaleX * (digits * 8);
    }
}

void g_save_cm_unlock_entries(void)
{
    int i;

    for (i = 0; i < 18; i++)
        storymode_save_files[0x305 + i] = cm_unlock_entries[i];
}

void g_load_cm_unlock_entries(void)
{
    int i;

    for (i = 0; i < 18; i++)
        cm_unlock_entries[i] = storymode_save_files[0x305 + i];
}

static char lbl_1_data_373E4[] = "";

struct StoryModeStageInfo sm_stage_infos_world1[] =
{
    { 201, 1 },
    { 202, 2 },
    { 203, 1 },
    { 204, 2 },
    { 1, 6 },
    { 2, 3 },
    { 3, 2 },
    { 4, 8 },
    { 5, 6 },
    { 6, 2 },
};

struct StoryModeStageInfo sm_stage_infos_world2[] =
{
    { 7, 2 },
    { 8, 5 },
    { 9, 3 },
    { 10, 6 },
    { 11, 7 },
    { 12, 3 },
    { 13, 7 },
    { 14, 6 },
    { 15, 8 },
    { 16, 6 },
};

struct StoryModeStageInfo sm_stage_infos_world3[] =
{
    { 231, 3 },
    { 232, 8 },
    { 233, 6 },
    { 234, 6 },
    { 235, 4 },
    { 236, 3 },
    { 237, 3 },
    { 238, 8 },
    { 239, 3 },
    { 17, 3 },
};

struct StoryModeStageInfo sm_stage_infos_world4[] =
{
    { 18, 2 },
    { 19, 8 },
    { 20, 5 },
    { 21, 1 },
    { 22, 6 },
    { 23, 9 },
    { 24, 6 },
    { 25, 8 },
    { 26, 8 },
    { 27, 10 },
};

struct StoryModeStageInfo sm_stage_infos_world5[] =
{
    { 28, 3 },
    { 29, 7 },
    { 30, 4 },
    { 31, 8 },
    { 32, 3 },
    { 33, 5 },
    { 34, 2 },
    { 35, 6 },
    { 36, 10 },
    { 37, 4 },
};

struct StoryModeStageInfo sm_stage_infos_world6[] =
{
    { 38, 8 },
    { 39, 8 },
    { 40, 5 },
    { 41, 5 },
    { 42, 5 },
    { 43, 9 },
    { 44, 9 },
    { 45, 5 },
    { 46, 9 },
    { 47, 2 },
};

struct StoryModeStageInfo sm_stage_infos_world7[] =
{
    { 281, 4 },
    { 282, 5 },
    { 283, 7 },
    { 284, 6 },
    { 285, 4 },
    { 286, 8 },
    { 287, 9 },
    { 288, 9 },
    { 289, 10 },
    { 48, 5 },
};

struct StoryModeStageInfo sm_stage_infos_world8[] =
{
    { 49, 9 },
    { 50, 8 },
    { 51, 10 },
    { 52, 9 },
    { 53, 8 },
    { 54, 9 },
    { 55, 10 },
    { 56, 10 },
    { 57, 6 },
    { 58, 7 },
};

struct StoryModeStageInfo sm_stage_infos_world9[] =
{
    { 59, 10 },
    { 60, 10 },
    { 61, 8 },
    { 62, 9 },
    { 63, 10 },
    { 64, 7 },
    { 65, 8 },
    { 66, 8 },
    { 67, 6 },
    { 68, 10 },
};

struct StoryModeStageInfo sm_stage_infos_world10[] =
{
    { 341, 8 },
    { 342, 9 },
    { 343, 9 },
    { 344, 9 },
    { 345, 8 },
    { 346, 9 },
    { 347, 7 },
    { 348, 8 },
    { 349, 9 },
    { 350, 6 },
};

struct StoryModeWorldInfo sm_world_info[] =
{
    { 10, 0, sm_stage_infos_world1 },
    { 10, 0, sm_stage_infos_world2 },
    { 10, 0, sm_stage_infos_world3 },
    { 10, 0, sm_stage_infos_world4 },
    { 10, 0, sm_stage_infos_world5 },
    { 10, 0, sm_stage_infos_world6 },
    { 10, 0, sm_stage_infos_world7 },
    { 10, 0, sm_stage_infos_world8 },
    { 10, 0, sm_stage_infos_world9 },
    { 10, 0, sm_stage_infos_world10 },
};

void clear_sm_state(void)
{
    memset(sm_state, 0, sizeof(sm_state));
    init_sm_state(0, FALSE);
}

int get_world_stage_count(int world)
{
    return sm_world_info[world].stageCount;
}

int get_world_unbeaten_stage_count(int world)
{
    int currentWorld = scen_info.currentWorld;
    int count;

    if (world < currentWorld)
    {
        count = get_world_stage_count(world);
        return count;
    }
    if (world == currentWorld)
        return sm_state[world].beatenCount;
    return 0;
}

int fn_1_A4CA8(int world, int idx)
{
    int currentWorld = scen_info.currentWorld;
    int result = idx;

    if (world < currentWorld)
        result = idx;
    else if (world == currentWorld)
    {
        if (sm_state[world].beatenCount <= idx)
            result = -1;
        else
            result = sm_state[world].beatenStages[idx];
    }
    else
        result = -1;
    return result;
}

int get_story_mode_stage_id(int world, int stage)
{
    return sm_world_info[world].stages[stage].stageId;
}

int get_story_mode_stage_difficulty(int world, int stage)
{
    return sm_world_info[world].stages[stage].difficulty;
}

BOOL fn_1_A4D5C(int world)
{
    int currentWorld = scen_info.currentWorld;

    if (world < currentWorld)
        return TRUE;
    if (world == currentWorld)
        return sm_state[world].flags & 1;
    return FALSE;
}

BOOL fn_1_A4DA4(void)
{
    BOOL result = TRUE;
    int world;

    for (world = 0; world < 10; world++)
    {
        if (!fn_1_A4D5C(world))
        {
            result = FALSE;
            break;
        }
    }
    return result;
}

BOOL is_storymode_stage_beaten(int world, int stage)
{
    int currentWorld = scen_info.currentWorld;
    struct StoryModeWorldState *state;
    u32 *flag;

    if (world < currentWorld)
        return TRUE;
    if (world == currentWorld)
    {
        state = &sm_state[world];
        flag = state->stageFlags + stage;
        return *flag & 1;
    }
    return FALSE;
}

static inline BOOL is_sm_stage_unlock_bit_set(int world, int stage)
{
    int bit = world * 10 + stage;

    return storymode_unlock_entries[bit / 8] & (1 << (bit % 8));
}

BOOL g_is_storymode_stage_unlocked(int world, int stage)
{
    if ((dip_switches & DIP_DEBUG) && (dip_switches & DIP_PLAY_STG_ALL))
        return TRUE;
    if (stage == -1)
    {
        for (stage++; stage < 10; stage++)
        {
            if (is_sm_stage_unlock_bit_set(world, stage))
                return TRUE;
        }
        return FALSE;
    }
    return is_sm_stage_unlock_bit_set(world, stage);
}

void fn_1_A4F58(int world, int stage)
{
    struct StoryModeWorldState *state;
    u32 *flag;

    if (world == scen_info.currentWorld)
    {
        state = &sm_state[world];
        flag = state->stageFlags + stage;
        if (!(*flag & 1))
        {
            *flag |= 1;
            state->beatenStages[state->beatenCount] = stage;
            state->beatenCount++;
            fn_1_A5080(world);
            scen_info.flags |= 0x40;
        }
    }
}

void unlock_storymode_stage(int world, int stage)
{
    int bit = world * 10 + stage;
    storymode_unlock_entries[bit / 8] |= 1 << (bit % 8);
    fn_1_A3FA0(bit);
}

BOOL fn_1_A505C(int world)
{
    return !(sm_state[world].flags & 2);
}

void fn_1_A5080(int world)
{
    struct StoryModeWorldState *state = &sm_state[world];
    int stageCount = get_world_stage_count(world);
    int beatenCount = state->beatenCount;

    if (stageCount <= beatenCount)
        state->flags |= 1;
    if (state->flags & 1)
        init_sm_state(world + 1, FALSE);
}

void init_sm_state(int world, BOOL started)
{
    struct StoryModeWorldState *state;

    if (world < 0 || world >= 10)
        return;
    state = &sm_state[world];
    if (started)
        state->flags &= ~2;
    else
        state->flags |= 2;
}

void clear_unlocked_storymode_stages(void)
{
    int i;

    for (i = 0; i < 13; i++)
        storymode_unlock_entries[i] = 0;
}

void g_save_storymode_unlock_entries(void)
{
    int i;

    for (i = 0; i < 13; i++)
        storymode_save_files[0x2F8 + i] = storymode_unlock_entries[i];
}

void g_load_storymode_unlock_entries(void)
{
    int i;

    for (i = 0; i < 13; i++)
        storymode_unlock_entries[i] = storymode_save_files[0x2F8 + i];
}

int get_storymode_stage_time_limit(int world, int stage)
{
    switch (sm_world_info[world].stages[stage].stageId)
    {
    case 30:
        return 30 * 60;
    default:
        return 60 * 60;
    }
}
