/* Staff roll: the credits sequence and its minigame, shown after clearing the game. */
#include "types.h"
#include "ape_internal.h"
#include "avdisp.h"
#include "ball.h"
#include "bitmap.h"
#include "camera.h"
#include "dip.h"
#include "effect.h"
#include "event.h"
#include "input.h"
#include "libc.h"
#include "macros.h"
#include "mathutil.h"
#include "mode.h"
#include "os.h"
#include "rand.h"
#include "sprite.h"
#include "stage.h"
#include "textdraw.h"

struct StaffRollItem
{
    /*0x00*/ s16 unk0;
    /*0x02*/ s8 unk2;
    /*0x04*/ s32 unk4;
    /*0x08*/ Vec pos;
    /*0x14*/ float unk14;
    /*0x18*/ float unk18;
    u8 unk1C[4];
    /*0x20*/ float unk20;
    u8 unk24[0x54 - 0x24];
}; /* size = 0x54 */

struct StaffRollName
{
    /*0x00*/ s16 unk0;
    /*0x02*/ s16 unk2;
    /*0x04*/ s8 unk4;
    /*0x08*/ float unk8;
    /*0x0C*/ float unkC;
    /*0x10*/ s16 unk10;
    /*0x12*/ s16 unk12;
    /*0x14*/ u8 *unk14;
}; /* size = 0x18 */

struct Letter
{
    /*0x00*/ float unk0;
    /*0x04*/ float unk4;
    /*0x08*/ Vec unk8;
    /*0x14*/ Vec unk14;
    /*0x20*/ Vec unk20;
    /*0x2C*/ Vec unk2C;
    /*0x38*/ Vec unk38;
    u8 unk44[0x80 - 0x44];
    /*0x80*/ s32 unk80;
    /*0x84*/ s8 unk84;
    /*0x86*/ s16 unk86;
    /*0x88*/ s16 unk88;
    /*0x8A*/ u8 unk8A;
}; /* size = 0x8C */

struct RatingName
{
    /*0x00*/ s32 unk0;
    /*0x04*/ char *unk4;
    /*0x08*/ char *unk8;
}; /* size = 0xC */

struct RatingNameIntl
{
    /*0x00*/ s32 unk0;
    /*0x04*/ char *unk4;
    /*0x08*/ char *unk8;
    /*0x0C*/ char *unkC;
    /*0x10*/ char *unk10;
    /*0x14*/ char *unk14;
}; /* size = 0x18 */

struct StaffRollFlags
{
    u8 b7 : 1;
    u8 b6 : 1;
    u8 b5 : 1;
    u8 b4 : 1;
    u8 b3 : 1;
    u8 b2 : 1;
    u8 b1 : 1;
    u8 b0 : 1;
};


void fn_2_15D10(void);
static void dead_literals(int i);
void staff_roll_init(void);
void g_smth_with_staff_roll_init_7(void);
void game_roll_draw_func(void);
void smd_staff_roll_dest(void);
void g_smth_with_staff_roll_init_1(void);
void g_smth_with_staff_roll_init_2(void);
void fn_2_16FD8(void);
static float dead_lit_72(void);
void fn_2_17114(void);
static void dead_lit_3(float *p);
void fn_2_17788(float *p, int t, s8 mode);
void g_staff_roll_name_align(void);
void fn_2_183F4(void);
static double dead_lit_11(void);
void fn_2_186F4(void);
void fn_2_189B0(int i, int j);
void fn_2_18C48(int i, int j, Vec *pos, Vec *vel);
int fn_2_18E24(u8 c);
static double dead_lit_10(void);
void g_staff_roll_banana_set(struct StaffRollItem *items_);
void g_smth_with_staff_roll_init_4(void);
int fn_2_192FC(int n);
void fn_2_19510(void);
void fn_2_19850(int i, struct StaffRollItem *items, int pattern);
void fn_2_199B4(struct StaffRollItem *items);
void fn_2_19C98(struct StaffRollItem *items);
void fn_2_19FC8(struct StaffRollItem *items);
void fn_2_1A290(struct StaffRollItem *items);
void g_smth_with_staff_roll_init_3(struct StaffRollItem *items);
void fn_2_1A658(struct StaffRollItem *items);
void fn_2_1A740(struct StaffRollItem *items);
void fn_2_1ABD8(struct StaffRollItem *items);
void fn_2_1AF10(void);
void fn_2_1B0A4(void);
void fn_2_1B1BC(void);
void fn_2_1B534(void);
void g_smth_with_staff_roll_init_6(void);
void fn_2_1B788(void);
void fn_2_1B9CC(void);
void staff_roll_results_draw_final_score(void);
void fn_2_1C0E4(void);
void staff_roll_results_draw_monkey_rating(void);
void staff_roll_results_draw_banana_count(void);
void staff_roll_results_draw_big_banana_count(void);
void staff_roll_results_draw_item_count(void);
void staff_roll_results_draw_letter_count(void);
float fn_2_1D368(void);
void fn_2_1D3D0(void);
void fn_2_1D574(void);
void fn_2_1D74C(void);
void fn_2_1D8F0(void);
void g_smth_with_staff_roll_init_5(void);
void fn_2_1E030(void);
void fn_2_1E0AC(void);
void fn_2_1E134(void);

extern s32 lbl_80198B38;
extern struct GMA *init_common_gma;
extern s32 active_monkey_id[];
int g_some_item_collision_check(Vec *a, Vec *b, float c, Vec *d, Vec *e, float f);
extern s32 locale;
struct PauseMenuInput
{
    s32 unk0;
    u32 unk4;
};
extern struct PauseMenuInput g_related_to_pause_menu_input;
extern int stage_heap;
extern s32 g_player_id_for_sound;
extern s32 g_smth_with_sound;
#define PAD_BUTTON_START 0x1000
struct Ape *g_load_ape_variant0(int chara, int lod, int scene);
void SoundReq(int id);
void avdisp_set_post_mult_color(float r, float g, float b, float a);
void call_SoundReqID_arg_0(int soundId);
void call_free_bmp_by_id(int id);
void create_go_sprite(int time);
void create_player_num_and_ready_sprites(int time);
void debug_textdraw_set_pos(int x, int y);
void dest_all_events(void);
void destroy_sprite_with_unique_id(int tag);
void draw_ape(struct Ape *ape);
void draw_ui_box(s32 bmpId);
void event_init(int id);
void fade_screen_to_color(s32 type, u32 color, int duration);
void fn_1_1132D0(struct Ape *ape, int b, int c, int d);
void fn_1_4EAFC(void *m, int color, int c);
void fn_1_AE99C(struct Sprite *sprite);
void fn_1_D4870(void);
void fn_80069860(float alpha);
void g_SoftStreamStart_with_some_defaults_2(int id);
void g_change_music_volume(int track, u32 time, s8 volume);
void g_draw_bg(void);
void g_draw_stage(void);
void g_maybe_some_bg_effect_handler(void);
void g_reset_cm_course(void);
void g_rumble_controller(int pad, int a, int b);
void g_smth_with_ape_rotation(struct Ape *ape, Vec *arg1);
void g_something_calls_sndPopGroup(void);
void g_something_with_stopping_music_or_sfx(int track);
void g_very_similar_to_puts(const char *);
void init_ui_element_sprite_with_defaults(void);
void load_bmp_by_id(int id);
void load_stage(int stage);
void mtxa_to_quat(Quat *q);
void ord_tbl_draw_nodes(void);
void quat_slerp(Quat *out, Quat *a, Quat *b, float t);
void reset_ui_widescreen_scale_mtx(void);
void set_sub_mode_destination(void (*func)(void));
void set_ui_element_sprite_alpha(float alpha);
void set_ui_element_sprite_depth(float z);
void set_ui_element_sprite_pos(float x, float y);
void set_ui_element_sprite_scale(float x, float y);
void set_ui_widescreen_scale_mtx(int width);
void textdraw_printf(char *fmt, ...);
void textdraw_reset(void);
void textdraw_set_alignment(s8 alignment);
void textdraw_set_alpha(float alpha);
void textdraw_set_depth(float depth);
void textdraw_set_flags(u32 flags);
void textdraw_set_font(int fontId);
void textdraw_set_mul_color(u32 color);
void textdraw_set_pos(float x, float y);
void textdraw_set_scale(float x, float y);
void u_reset_light_group_stack(int unused);
int fn_2_18E24(u8 c);

s32 B_D00;
u8 B_D04;
float B_D08;
s32 B_D0C;
s8 B_D10;
s8 B_D11;
s32 B_D14;
s32 B_D18;
s32 B_D1C;
s32 staff_roll_final_score;
s32 B_D24;
s32 staff_roll_banana_count;
s32 staff_roll_big_banana_count;
s32 staff_roll_item_count;
s32 staff_roll_hittable_object_count;
s32 staff_roll_hit_percentage;
s32 B_D3C;
Vec B_D40;
s32 B_D4C;
float B_D50;
s32 B_D54;
struct StaffRollFlags B_D58;
#ifdef __MWERKS__
float B_D5C;
s32 B_D60[25];
s32 B_DC4[25];
float B_E28;
#else
/* fn_2_192FC and the hit counters index these per-second histograms with
 * 0..25 and n-1 (B_D60[25] is B_DC4[0], B_DC4[25] is B_E28, B_DC4[-1] is
 * B_D60[24]); on GC they are adjacent bss, on PC one object keeps that. */
static struct
{
    float d5c;
    s32 d60[25];
    s32 dc4[25];
    float e28;
} B_D5C_E28;
#define B_D5C B_D5C_E28.d5c
#define B_D60 B_D5C_E28.d60
#define B_DC4 B_D5C_E28.dc4
#define B_E28 B_D5C_E28.e28
#endif
struct StaffRollItem B_E2C[0x5A];
struct StaffRollItem B_2BB4[45];
Vec B_3A78;
float B_3A84;
s32 B_3A88;
s32 B_3A8C;
s32 g_some_pausemenu_var4;
s32 B_3A94;
Mtx B_3A98;
struct StaffRollItem B_3AC8[0x82];
s32 staff_roll_banana_percentage;
s32 B_6574;
struct Ape *g_some_ape;
Vec B_657C;
float B_6588;
float B_658C;
Vec B_6590;
Vec B_659C;

#define B_D20 staff_roll_final_score
#define B_D28 staff_roll_banana_count
#define B_D2C staff_roll_big_banana_count
#define B_D30 staff_roll_item_count
#define B_D34 staff_roll_hittable_object_count
#define B_D38 staff_roll_hit_percentage
#define B_3A90 g_some_pausemenu_var4
#define B_6570 staff_roll_banana_percentage
#define B_6578 g_some_ape

extern u32 D_3170[];
extern u8 D_32E0[];
extern u16 D_3304[];
extern float D_3348[];
extern char D_33CC[];
extern struct StaffRollName D_3580[];
extern char D_38C8[];
extern struct StaffRollName D_3C10[];
extern struct Letter D_4108[0x58][0x19];
extern float D_4F428[];
extern s8 D_4F588[];
extern s8 D_4F5E0[];
extern s8 D_4F638[5][0x35];
extern Vec D_4F744[];
extern u32 D_4FB04[];
extern s32 D_4FD5C[];
#ifdef __MWERKS__
extern s32 D_4FD60[];
#else
#define D_4FD60 (&D_4FD5C[1])
#endif
extern Vec D_4FD70[];
extern float D_4FED8[];
extern Vec D_4FF14[];
extern u32 D_5052C[];
extern Vec *D_53934[];
extern float D_539C8;
extern float D_539CC;
extern s32 D_539D0;
extern s32 D_539D4;
extern s32 D_539D8;
extern s32 D_539DC;
extern s32 D_539E0;
extern s32 D_539E4;
extern s32 D_539E8;
extern Vec D_539EC[];
extern char D_53A34[];
extern struct RatingName D_54214[];
extern char D_544F0[];
extern struct RatingNameIntl D_55398[];
extern char D_55950[];
extern char *D_559A4[];
extern char D_559BC[];
extern char *D_559F4[];
extern char D_55A0C[];
extern char *D_55A3C[];
extern char D_55A54[];
extern char *D_55A70[];
extern char D_55A74[];
extern char *D_55A90[];
extern char D_55A94[];
extern char *D_55AB0[];
extern char D_55AB4[];
extern char *D_55AFC[];
extern char D_55B14[];
extern char *D_55B58[];
extern char D_55B70[];
extern char *D_55BAC[];
extern char *D_55BC4[];
extern char D_55BC8[];
extern char *D_55BEC[];
extern char D_55BF0[];
extern char *D_55C14[];
extern char D_55C18[];
extern char *D_55C64[];
extern char D_55C7C[];
extern char *D_55CC4[];
extern char D_55CDC[];
extern char *D_55D38[];
extern char D_55D50[];
extern char *D_55D98[];
extern char D_55DB0[];
extern char *D_55DFC[];
extern char D_55E14[];
extern char *D_55E54[];
extern char D_55E6C[];
extern char *D_55E98[];
extern u32 D_55EB0[];

u32 D_3170[] = {
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
    0x18000000, 0x000F0000
};

u8 D_32E0[] = {
    65,
    66,
    67,
    68,
    69,
    70,
    71,
    72,
    73,
    74,
    75,
    76,
    77,
    78,
    79,
    80,
    81,
    82,
    83,
    84,
    85,
    86,
    87,
    88,
    89,
    90,
    43,
    38,
    44,
    46,
    47,
    50,
    45,
    0,
    0,
    0
};

u16 D_3304[] = {
    0x1,
    0x7,
    0xc,
    0x10,
    0x13,
    0x17,
    0x1a,
    0x1c,
    0x1d,
    0x1e,
    0x1f,
    0x20,
    0x22,
    0x24,
    0x26,
    0x28,
    0x2e,
    0x30,
    0x31,
    0x38,
    0x3b,
    0x3d,
    0x3e,
    0x3f,
    0x40,
    0x41,
    0x0,
    0x3,
    0xe,
    0x2c,
    0x37,
    0x3a,
    0x1d,
    0x0
};

float D_3348[] = {
    1.0f,
    1.0f,
    1.0f,
    1.10000002f,
    1.14999998f,
    1.0f,
    1.0f,
    1.10000002f,
    0.850000024f,
    0.899999976f,
    1.14999998f,
    1.10000002f,
    1.29999995f,
    1.25f,
    1.20000005f,
    1.0f,
    1.0f,
    1.0f,
    1.0f,
    1.0f,
    1.29999995f,
    1.0f,
    1.20000005f,
    1.0f,
    1.0f,
    1.0f,
    1.0f,
    1.0f,
    0.899999976f,
    0.899999976f,
    0.899999976f,
    1.0f,
    0.899999976f
};

/* staff roll text pools as bytes: as u32 words the letters came out byte-reversed in
 * groups of four on little endian */
char D_33CC[0x1B4] =
    "SUPER+MONKEY+BALL+2\000""STAFF+ROLL\000\000""DIRECTOR\000\000\000\000"
    "GAME+PROGRAM\000\000\000\000""PROGRAMMERS\000""SYSTEM\000\000""PARTY+GAME\000\000"
    "PROGRAMMER\000\000""2D+SYSTEM\000\000\000""LEVEL+DESIGN\000\000\000\000"
    "STAGE+DESIGN\000\000\000\000""STAGE+DESIGNERS\000""MOTION+DESIGN\000\000\000"
    "MOTION+DESIGNERS\000\000\000\000""DESIGNERS\000\000\000""STORY+MODE+MOVIE\000\000\000\000"
    "2D+DESIGNER\000""CHARACTER+DESIGNER\000\000""ASSISTANT+DESIGNERS\000"
    "PLANNERS\000\000\000\000""SOUND+DESIGNER\000\000""AND+DESIGN\000\000"
    "PARTY+GAME+PROGRAM\000\000""MONKEY+VOICE\000\000\000\000""PUBLICITY\000\000\000"
    "COOPERATION\000""SPECIAL+THANKS\000\000""&\000\000\000""PRODUCER\000\000\000\000"
    "AMUSEMENT+VISION\000\000\000\000";

struct StaffRollName D_3580[] = {
    {0, 0, 0, 0.0f, -10.0f, 19, 0, (u8 *)D_33CC},
    {0, 0, 0, 0.0f, -5.0f, 10, 1, (u8 *)D_33CC + 0x14},
    {0, 2, 0, 0.0f, 30.0f, 8, 2, (u8 *)D_33CC + 0x20},
    {0, 3, 0, 0.0f, 31.2000008f, 12, 3, (u8 *)D_33CC + 0x2C},
    {0, 2, 0, 0.0f, 45.0f, 11, 5, (u8 *)D_33CC + 0x3C},
    {0, 3, 0, 0.0f, 46.2000008f, 6, 6, (u8 *)D_33CC + 0x48},
    {0, 2, 0, 0.0f, 75.0f, 11, 11, (u8 *)D_33CC + 0x3C},
    {0, 3, 0, 0.0f, 76.1999969f, 10, 12, (u8 *)D_33CC + 0x50},
    {0, 2, 0, 0.0f, 115.0f, 10, 19, (u8 *)D_33CC + 0x5C},
    {0, 3, 0, 0.0f, 116.199997f, 9, 20, (u8 *)D_33CC + 0x68},
    {0, 2, 0, 0.0f, 130.0f, 8, 22, (u8 *)D_33CC + 0x20},
    {0, 3, 0, 0.0f, 131.199997f, 12, 23, (u8 *)D_33CC + 0x74},
    {0, 2, 0, 0.0f, 145.0f, 8, 25, (u8 *)D_33CC + 0x20},
    {0, 3, 0, 0.0f, 146.199997f, 12, 26, (u8 *)D_33CC + 0x84},
    {0, 2, 0, 0.0f, 160.0f, 15, 28, (u8 *)D_33CC + 0x94},
    {0, 2, 0, 0.0f, 200.0f, 8, 35, (u8 *)D_33CC + 0x20},
    {0, 3, 0, 0.0f, 201.199997f, 13, 36, (u8 *)D_33CC + 0xA4},
    {0, 2, 0, 0.0f, 215.0f, 16, 38, (u8 *)D_33CC + 0xB4},
    {0, 3, 0, 0.0f, 245.0f, 9, 43, (u8 *)D_33CC + 0xC8},
    {0, 2, 0, 0.0f, 246.199997f, 16, 44, (u8 *)D_33CC + 0xD4},
    {0, 2, 0, 0.0f, 265.0f, 11, 47, (u8 *)D_33CC + 0xE8},
    {0, 2, 0, 0.0f, 280.0f, 18, 49, (u8 *)D_33CC + 0xF4},
    {0, 2, 0, 0.0f, 295.0f, 19, 51, (u8 *)D_33CC + 0x108},
    {0, 2, 0, 0.0f, 340.0f, 8, 56, (u8 *)D_33CC + 0x11C},
    {0, 2, 0, 0.0f, 360.0f, 14, 59, (u8 *)D_33CC + 0x128},
    {0, 2, 0, 0.0f, 385.0f, 10, 63, (u8 *)D_33CC + 0x138},
    {0, 3, 0, 0.0f, 386.200012f, 18, 64, (u8 *)D_33CC + 0x144},
    {0, 2, 0, 0.0f, 400.0f, 12, 66, (u8 *)D_33CC + 0x158},
    {0, 2, 0, 0.0f, 430.0f, 9, 70, (u8 *)D_33CC + 0x168},
    {0, 2, 0, 0.0f, 450.0f, 11, 73, (u8 *)D_33CC + 0x174},
    {0, 2, 0, 0.0f, 470.0f, 14, 78, (u8 *)D_33CC + 0x180},
    {0, 2, 0, 0.0f, 510.0f, 8, 82, (u8 *)D_33CC + 0x20},
    {0, 3, 0, 0.0f, 511.200012f, 1, 83, (u8 *)D_33CC + 0x190},
    {0, 3, 0, 0.0f, 512.400024f, 8, 84, (u8 *)D_33CC + 0x194},
    {0, 0, 0, 0.0f, 525.0f, 16, 86, (u8 *)D_33CC + 0x1A0}
};

char D_38C8[0x348] =
    "HISASHI+ENDO\000\000\000\000""EIJI+TAKAKI\000""TETSUYA+KAKU\000\000\000\000"
    "KOTA+MATSUMOTO\000\000""YOSHINORI+SUZUKI\000\000\000\000""TETSU+YOSHIMITSU\000\000\000\000"
    "NORIO+HAGA\000\000""YASUMASA+MATSUMORI\000\000""YOUICHI+ISHIKAWA\000\000\000\000"
    "KOJI+TOKIEDA\000\000\000\000""MIE+KAWANISHI\000\000\000""JUN+TOKUHARA\000\000\000\000"
    "JUNICHI+YAMADA\000\000""DAISUKE+SATO\000\000\000\000""YUKIO+ODA\000\000\000"
    "KUMIKO+SAKURAI\000\000""KEN-ICHIRO+YASUTOMI\000""SANAE+TAKEDA\000\000\000\000"
    "MARI+SASAKI\000""JUN+YUKAWA\000\000""KAZUHIRO+IZAKI\000\000""SAKAE+OSUMI\000"
    "MICHIO+SHIRAKO\000\000""KENTAROU+NIINA\000\000""TAKAYUKI+SORIMACHI\000\000"
    "HIROKI+KOIKE\000\000\000\000""EIKO+TAKAHASHI\000\000""CHIGAKU+FUKUHARA\000\000\000\000"
    "MIKA+KOJIMA\000""NAOKI+SOMEYA\000\000\000\000""SAIZO+NAGAI\000""MIHO+NAKAMURA\000\000\000"
    "MICHIHIKO+HATOYAMA\000\000""YUKINOBU+ARIKAWA\000\000\000\000"
    "HIROYUKI+SAKAMOTO\000\000\000""HIDENORI+SHOJI\000\000""HARUYOSHI+TOMITA\000\000\000\000"
    "ASPECT+CO.,LTD.\000""KAORU+MOROTA\000\000\000\000""KONAMI+YOSHIDA\000\000""RIO+NATSUKI\000"
    "PATRICK+HARLAN\000\000""MASAE+OTOSHI\000\000\000\000""KOJI+UEDA\000\000\000"
    "COMPANY,INC.\000\000\000\000""DOLE+FOOD\000\000\000""DOLE+JAPAN,LTD.\000"
    "AVANT+INC.\000\000""HAIKYO\000\000""STURM+CO.,LTD.\000\000""T/S+MUSIC+CO.,LTD.\000\000"
    "RYUJI+IUCHI\000""TOSHIHIRO+NAGOSHI\000\000\000";

struct StaffRollName D_3C10[] = {
    {0, 4, 0, 0.0f, 35.0f, 12, 4, (u8 *)D_38C8},
    {0, 4, 0, 0.0f, 50.0f, 11, 7, (u8 *)D_38C8 + 0x10},
    {0, 4, 1, 0.0f, 55.0f, 12, 8, (u8 *)D_38C8 + 0x1C},
    {0, 4, 1, 0.0f, 60.0f, 14, 9, (u8 *)D_38C8 + 0x2C},
    {0, 4, 2, 0.0f, 65.0f, 16, 10, (u8 *)D_38C8 + 0x3C},
    {0, 4, 2, 0.0f, 80.0f, 16, 13, (u8 *)D_38C8 + 0x50},
    {0, 4, 0, 0.0f, 85.0f, 10, 14, (u8 *)D_38C8 + 0x64},
    {0, 4, 1, 0.0f, 90.0f, 18, 15, (u8 *)D_38C8 + 0x70},
    {0, 4, 1, 0.0f, 95.0f, 16, 16, (u8 *)D_38C8 + 0x84},
    {0, 4, 2, 0.0f, 100.0f, 12, 17, (u8 *)D_38C8 + 0x98},
    {0, 4, 2, 0.0f, 105.0f, 13, 18, (u8 *)D_38C8 + 0xA8},
    {0, 4, 0, 0.0f, 120.0f, 12, 21, (u8 *)D_38C8 + 0xB8},
    {0, 4, 0, 0.0f, 135.0f, 14, 24, (u8 *)D_38C8 + 0xC8},
    {0, 4, 0, 0.0f, 150.0f, 12, 27, (u8 *)D_38C8 + 0xD8},
    {0, 4, 0, 0.0f, 165.0f, 9, 29, (u8 *)D_38C8 + 0xE8},
    {0, 4, 1, 0.0f, 170.0f, 14, 30, (u8 *)D_38C8 + 0xF4},
    {0, 4, 1, 0.0f, 175.0f, 19, 31, (u8 *)D_38C8 + 0x104},
    {0, 4, 2, 0.0f, 180.0f, 12, 32, (u8 *)D_38C8 + 0x118},
    {0, 4, 1, 0.0f, 185.0f, 11, 33, (u8 *)D_38C8 + 0x128},
    {0, 4, 1, 0.0f, 190.0f, 10, 34, (u8 *)D_38C8 + 0x134},
    {0, 4, 0, 0.0f, 205.0f, 14, 39, (u8 *)D_38C8 + 0x140},
    {0, 4, 0, 0.0f, 220.0f, 11, 57, (u8 *)D_38C8 + 0x150},
    {0, 4, 1, 0.0f, 225.0f, 14, 42, (u8 *)D_38C8 + 0x15C},
    {0, 4, 1, 0.0f, 230.0f, 14, 43, (u8 *)D_38C8 + 0x16C},
    {0, 4, 2, 0.0f, 235.0f, 18, 44, (u8 *)D_38C8 + 0x17C},
    {0, 4, 0, 0.0f, 250.0f, 12, 45, (u8 *)D_38C8 + 0x190},
    {0, 4, 2, 0.0f, 255.0f, 14, 46, (u8 *)D_38C8 + 0x1A0},
    {0, 4, 0, 0.0f, 270.0f, 16, 48, (u8 *)D_38C8 + 0x1B0},
    {0, 4, 0, 0.0f, 285.0f, 11, 50, (u8 *)D_38C8 + 0x1C4},
    {0, 4, 0, 0.0f, 300.0f, 12, 52, (u8 *)D_38C8 + 0x1D0},
    {0, 4, 1, 0.0f, 305.0f, 11, 53, (u8 *)D_38C8 + 0x1E0},
    {0, 4, 1, 0.0f, 310.0f, 13, 54, (u8 *)D_38C8 + 0x1EC},
    {0, 4, 2, 0.0f, 315.0f, 18, 55, (u8 *)D_38C8 + 0x1FC},
    {0, 4, 0, 0.0f, 345.0f, 16, 52, (u8 *)D_38C8 + 0x210},
    {0, 4, 1, 0.0f, 350.0f, 17, 53, (u8 *)D_38C8 + 0x224},
    {0, 4, 0, 0.0f, 365.0f, 14, 55, (u8 *)D_38C8 + 0x238},
    {0, 4, 2, 0.0f, 370.0f, 16, 56, (u8 *)D_38C8 + 0x248},
    {0, 4, 0, 0.0f, 390.0f, 15, 60, (u8 *)D_38C8 + 0x25C},
    {0, 4, 0, 0.0f, 405.0f, 12, 62, (u8 *)D_38C8 + 0x26C},
    {0, 4, 1, 0.0f, 410.0f, 14, 63, (u8 *)D_38C8 + 0x27C},
    {0, 4, 2, 0.0f, 415.0f, 11, 64, (u8 *)D_38C8 + 0x28C},
    {0, 4, 1, 0.0f, 420.0f, 14, 65, (u8 *)D_38C8 + 0x298},
    {0, 4, 0, 0.0f, 435.0f, 12, 66, (u8 *)D_38C8 + 0x2A8},
    {0, 4, 1, 0.0f, 440.0f, 9, 67, (u8 *)D_38C8 + 0x2B8},
    {0, 4, 0, 0.0f, 455.0f, 12, 69, (u8 *)D_38C8 + 0x2C4},
    {0, 5, 0, 0.0f, 456.200012f, 9, 70, (u8 *)D_38C8 + 0x2D4},
    {0, 4, 0, 0.0f, 460.0f, 15, 71, (u8 *)D_38C8 + 0x2E0},
    {0, 4, 2, 0.0f, 465.0f, 10, 76, (u8 *)D_38C8 + 0x2F0},
    {0, 4, 0, 0.0f, 475.0f, 6, 73, (u8 *)D_38C8 + 0x2FC},
    {0, 4, 0, 0.0f, 480.0f, 14, 74, (u8 *)D_38C8 + 0x304},
    {0, 4, 1, 0.0f, 485.0f, 18, 75, (u8 *)D_38C8 + 0x314},
    {0, 4, 1, 0.0f, 490.0f, 11, 76, (u8 *)D_38C8 + 0x328},
    {0, 1, 0, 0.0f, 515.0f, 17, 80, (u8 *)D_38C8 + 0x334}
};

struct Letter D_4108[0x58][0x19] = {0};

float D_4F428[] = {
    0.0f,
    0.0f,
    0.0f,
    0.0f,
    0.0f,
    0.0f,
    0.0f,
    0.0f,
    0.0f,
    0.0f,
    0.0f,
    0.0f,
    0.0f,
    0.0f,
    0.0f,
    0.0f,
    0.0f,
    0.0f,
    0.0f,
    0.0f,
    0.0f,
    0.0f,
    0.0f,
    0.0f,
    0.0f,
    0.0f,
    0.0f,
    0.0f,
    0.0f,
    0.0f,
    0.0f,
    0.0f,
    0.0f,
    0.0f,
    0.0f,
    0.0f,
    0.0f,
    0.0f,
    0.0f,
    0.0f,
    0.0f,
    0.0f,
    0.0f,
    0.0f,
    0.0f,
    0.0f,
    0.0f,
    0.0f,
    0.0f,
    0.0f,
    0.0f,
    0.0f,
    0.0f,
    0.0f,
    0.0f,
    0.0f,
    0.0f,
    0.0f,
    0.0f,
    0.0f,
    0.0f,
    0.0f,
    0.0f,
    0.0f,
    0.0f,
    0.0f,
    0.0f,
    0.0f,
    0.0f,
    0.0f,
    0.0f,
    0.0f,
    0.0f,
    0.0f,
    0.0f,
    0.0f,
    0.0f,
    0.0f,
    0.0f,
    0.0f,
    0.0f,
    0.0f,
    0.0f,
    0.0f,
    0.0f,
    0.0f,
    0.0f,
    0.0f
};

s8 D_4F588[] = {
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0
};

s8 D_4F5E0[] = {
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0
};

s8 D_4F638[5][0x35] = {
    0, 0, 1, 1, 2, 2, 0, 1, 1, 2, 2, 0, 0, 0, 0, 1, 1, 2, 1, 1, 0, 0, 1, 1, 2, 0, 1, 0, 0, 0, 1, 1, 2, 0, 1, 0, 2, 0, 0, 1, 2, 2, 0, 1, 0, 0, 0, 2, 0, 0, 1, 1, 0,
    0, 0, 2, 2, 1, 0, 0, 0, 2, 1, 1, 0, 0, 0, 0, 2, 2, 1, 2, 2, 0, 0, 2, 2, 1, 0, 1, 0, 0, 0, 2, 2, 1, 0, 2, 0, 1, 0, 0, 2, 1, 1, 0, 2, 0, 0, 0, 1, 1, 0, 2, 2, 0,
    0, 0, 1, 2, 2, 1, 0, 0, 1, 2, 2, 0, 0, 0, 0, 1, 2, 2, 1, 2, 0, 0, 1, 2, 2, 0, 1, 0, 0, 0, 2, 1, 1, 0, 1, 0, 2, 0, 0, 1, 2, 2, 0, 2, 0, 0, 0, 2, 0, 0, 1, 1, 0,
    0, 0, 2, 1, 1, 0, 0, 1, 1, 2, 2, 0, 0, 0, 0, 1, 1, 2, 2, 1, 0, 0, 2, 1, 1, 0, 1, 0, 0, 0, 1, 1, 2, 0, 1, 0, 2, 0, 0, 1, 1, 0, 0, 1, 0, 0, 0, 2, 1, 1, 2, 2, 0,
    0, 0, 2, 2, 0, 2, 0, 1, 2, 2, 1, 0, 0, 0, 0, 1, 2, 2, 1, 1, 0, 0, 1, 2, 2, 0, 1, 0, 0, 0, 1, 2, 2, 0, 1, 0, 2, 0, 0, 1, 2, 0, 0, 1, 0, 0, 0, 1, 0, 2, 1, 1, 0,
};

Vec D_4F744[] = {
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f}
};

u32 D_4FB04[] = {
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000
};

/* item model ids: D_4FD5C[] is indexed by item type 0..2, past its one entry into
 * D_4FD60's. Separate symbols need not be adjacent on PC (reading past D_4FD5C
 * crashed the staff roll), so there they are one array. */
#ifdef __MWERKS__
s32 D_4FD5C[] = {
    64
};

s32 D_4FD60[] = {
    37,
    64,
    37,
    37
};
#else
s32 D_4FD5C[] = {
    64,
    37,
    64,
    37,
    37
};
#endif

Vec D_4FD70[] = {
    {0.0f, 83.0f, 0.0f},
    {0.0f, 103.0f, 0.0f},
    {0.0f, 113.0f, 0.0f},
    {0.0f, 123.0f, 0.0f},
    {0.0f, 133.0f, 0.0f},
    {0.0f, 143.0f, 0.0f},
    {0.0f, 153.0f, 0.0f},
    {0.0f, 163.0f, 0.0f},
    {0.0f, 173.0f, 0.0f},
    {0.0f, 183.0f, 0.0f},
    {0.0f, 193.0f, 0.0f},
    {0.0f, 203.0f, 0.0f},
    {0.0f, 213.0f, 0.0f},
    {0.0f, 223.0f, 0.0f},
    {0.0f, 233.0f, 0.0f},
    {0.0f, 243.0f, 0.0f},
    {0.0f, 253.0f, 0.0f},
    {0.0f, 263.0f, 0.0f},
    {0.0f, 273.0f, 0.0f},
    {0.0f, 283.0f, 0.0f},
    {0.0f, 293.0f, 0.0f},
    {0.0f, 303.0f, 0.0f},
    {0.0f, 313.0f, 0.0f},
    {0.0f, 323.0f, 0.0f},
    {0.0f, 333.0f, 0.0f},
    {0.0f, 343.0f, 0.0f},
    {0.0f, 353.0f, 0.0f},
    {0.0f, 363.0f, 0.0f},
    {0.0f, 373.0f, 0.0f},
    {0.0f, 383.0f, 0.0f}
};

float D_4FED8[] = {
    30.0f,
    70.0f,
    90.0f,
    120.0f,
    150.0f,
    170.0f,
    200.0f,
    230.0f,
    250.0f,
    280.0f,
    300.0f,
    330.0f,
    360.0f,
    410.0f,
    440.0f
};

Vec D_4FF14[] = {
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f}
};

u32 D_5052C[] = {
    0xC0A80000, 0x419A0000, 0x00000000, 0xC0A80000, 0x41960000, 0x00000000,
    0xC0A80000, 0x41920000, 0x00000000, 0xC0A80000, 0x418E0000, 0x00000000,
    0xC0A80000, 0x418A0000, 0x00000000, 0x40D80000, 0x418A0000, 0x00000000,
    0x40D80000, 0x41860000, 0x00000000, 0x40D80000, 0x41820000, 0x00000000,
    0x40D80000, 0x417C0000, 0x00000000, 0x40D80000, 0x41740000, 0x00000000,
    0xC0A80000, 0x415C0000, 0x00000000, 0xC0A80000, 0x41540000, 0x00000000,
    0xC0A80000, 0x414C0000, 0x00000000, 0xC0A80000, 0x41440000, 0x00000000,
    0xC0A80000, 0x413C0000, 0x00000000, 0x3FA00000, 0x41100000, 0x00000000,
    0x40100000, 0x410C0000, 0x00000000, 0x40400000, 0x41080000, 0x00000000,
    0x40700000, 0x41000000, 0x00000000, 0x40900000, 0x40F00000, 0x00000000,
    0x40A80000, 0x40D80000, 0x00000000, 0x40B80000, 0x40C00000, 0x00000000,
    0x40C00000, 0x40A80000, 0x00000000, 0x40C00000, 0x40900000, 0x00000000,
    0x40B00000, 0x40700000, 0x00000000, 0x40A00000, 0x40400000, 0x00000000,
    0x40880000, 0x40200000, 0x00000000, 0x40400000, 0x3FE00000, 0x00000000,
    0x40000000, 0x3FA00000, 0x00000000, 0x3F400000, 0x3F400000, 0x00000000,
    0x40980000, 0x41920000, 0x00000000, 0x40980000, 0x418E0000, 0x00000000,
    0x40980000, 0x418A0000, 0x00000000, 0x40980000, 0x41860000, 0x00000000,
    0x40980000, 0x41820000, 0x00000000, 0xBF400000, 0x415C0000, 0x00000000,
    0xBE800000, 0x415C0000, 0x00000000, 0x3E800000, 0x415C0000, 0x00000000,
    0x3F400000, 0x415C0000, 0x00000000, 0x3FA00000, 0x415C0000, 0x00000000,
    0xC0800000, 0x41300000, 0x00000000, 0xC0800000, 0x41280000, 0x00000000,
    0xC0800000, 0x41200000, 0x00000000, 0xC0800000, 0x41180000, 0x00000000,
    0xC0800000, 0x41100000, 0x00000000, 0x3E800000, 0x40F00000, 0x00000000,
    0x3F400000, 0x40F00000, 0x00000000, 0x3FA00000, 0x40F00000, 0x00000000,
    0x3FE00000, 0x40F00000, 0x00000000, 0x40100000, 0x40F00000, 0x00000000,
    0x40A80000, 0x40980000, 0x00000000, 0x40A80000, 0x40880000, 0x00000000,
    0x40A80000, 0x40700000, 0x00000000, 0x40A80000, 0x40500000, 0x00000000,
    0x40A80000, 0x40300000, 0x00000000, 0xC0000000, 0x40100000, 0x00000000,
    0xBFC00000, 0x40100000, 0x00000000, 0xBF800000, 0x40100000, 0x00000000,
    0xBF000000, 0x40100000, 0x00000000, 0x00000000, 0x40100000, 0x00000000,
    0x40D00000, 0x419C0000, 0x00000000, 0xC0200000, 0x419A0000, 0x00000000,
    0xC0800000, 0x41980000, 0x00000000, 0xBF000000, 0x41960000, 0x00000000,
    0xC0980000, 0x41920000, 0x00000000, 0x40B00000, 0x41920000, 0x00000000,
    0x3F400000, 0x41900000, 0x00000000, 0x40100000, 0x418A0000, 0x00000000,
    0x40800000, 0x418A0000, 0x00000000, 0xC0B00000, 0x41880000, 0x00000000,
    0xC0A00000, 0x41640000, 0x00000000, 0x40100000, 0x41640000, 0x00000000,
    0x3E800000, 0x415C0000, 0x00000000, 0x40600000, 0x415C0000, 0x00000000,
    0xC0880000, 0x41580000, 0x00000000, 0xBF400000, 0x41500000, 0x00000000,
    0x40900000, 0x41500000, 0x00000000, 0xC0600000, 0x414C0000, 0x00000000,
    0xC0100000, 0x41440000, 0x00000000, 0x40B00000, 0x41440000, 0x00000000,
    0x40500000, 0x40800000, 0x00000000, 0xC0980000, 0x40700000, 0x00000000,
    0x3FE00000, 0x40700000, 0x00000000, 0x3F000000, 0x40500000, 0x00000000,
    0x40900000, 0x40400000, 0x00000000, 0xC0800000, 0x40300000, 0x00000000,
    0xBF000000, 0x40200000, 0x00000000, 0xC0300000, 0x40000000, 0x00000000,
    0xBFC00000, 0x3FE00000, 0x00000000, 0x40A80000, 0x3FE00000, 0x00000000,
    0x40E80000, 0x419C0000, 0x00000000, 0x40E80000, 0x41940000, 0x00000000,
    0x40E80000, 0x418C0000, 0x00000000, 0x40E80000, 0x41840000, 0x00000000,
    0x40E80000, 0x41640000, 0x00000000, 0xC0B80000, 0x41540000, 0x00000000,
    0xC0980000, 0x41540000, 0x00000000, 0xC0700000, 0x41540000, 0x00000000,
    0xC0300000, 0x41540000, 0x00000000, 0xBFE00000, 0x41540000, 0x00000000,
    0xBF400000, 0x41540000, 0x00000000, 0x3E800000, 0x41540000, 0x00000000,
    0x3FA00000, 0x41540000, 0x00000000, 0x40100000, 0x41540000, 0x00000000,
    0x40500000, 0x41540000, 0x00000000, 0x40880000, 0x41540000, 0x00000000,
    0x40A80000, 0x41540000, 0x00000000, 0x40C80000, 0x41540000, 0x00000000,
    0x40E80000, 0x41540000, 0x00000000, 0xC0B80000, 0x41440000, 0x00000000,
    0xC0B80000, 0x411C0000, 0x00000000, 0xC0B80000, 0x41100000, 0x00000000,
    0xC0B80000, 0x41040000, 0x00000000, 0xC0B80000, 0x40F00000, 0x00000000,
    0xC0B80000, 0x40D80000, 0x00000000, 0xC0B80000, 0x40980000, 0x00000000,
    0xC0B80000, 0x40800000, 0x00000000, 0xC0B80000, 0x40500000, 0x00000000,
    0xC0B80000, 0x40200000, 0x00000000, 0xC0B80000, 0x3FE00000, 0x00000000,
    0xC0A80000, 0x41960000, 0x00000000, 0xC0700000, 0x41960000, 0x00000000,
    0xC0100000, 0x41960000, 0x00000000, 0xBFA00000, 0x41900000, 0x00000000,
    0xC0A80000, 0x418A0000, 0x00000000, 0xC0700000, 0x418A0000, 0x00000000,
    0xC0100000, 0x418A0000, 0x00000000, 0x3FE00000, 0x41580000, 0x00000000,
    0x40500000, 0x41580000, 0x00000000, 0x40980000, 0x41580000, 0x00000000,
    0x40C80000, 0x41580000, 0x00000000, 0x3FE00000, 0x41440000, 0x00000000,
    0x40500000, 0x41440000, 0x00000000, 0x40980000, 0x41440000, 0x00000000,
    0x40C80000, 0x41440000, 0x00000000, 0xC0900000, 0x410C0000, 0x00000000,
    0xC0400000, 0x410C0000, 0x00000000, 0xBFC00000, 0x410C0000, 0x00000000,
    0x00000000, 0x410C0000, 0x00000000, 0xC0900000, 0x40E80000, 0x00000000,
    0xC0400000, 0x40E80000, 0x00000000, 0xBFC00000, 0x40E80000, 0x00000000,
    0x00000000, 0x40E80000, 0x00000000, 0xC0400000, 0x40500000, 0x00000000,
    0xBF000000, 0x40500000, 0x00000000, 0x40000000, 0x40500000, 0x00000000,
    0xC0400000, 0x40100000, 0x00000000, 0xBFE00000, 0x40100000, 0x00000000,
    0x3F400000, 0x40100000, 0x00000000, 0x40000000, 0x40100000, 0x00000000,
    0xC0B80000, 0x41960000, 0x00000000, 0xC0900000, 0x41960000, 0x00000000,
    0xC0500000, 0x41960000, 0x00000000, 0xC0000000, 0x41960000, 0x00000000,
    0xBF400000, 0x41960000, 0x00000000, 0x3F000000, 0x41960000, 0x00000000,
    0x3FE00000, 0x41960000, 0x00000000, 0x40400000, 0x41960000, 0x00000000,
    0x40880000, 0x41960000, 0x00000000, 0x40B00000, 0x41960000, 0x00000000,
    0x40D80000, 0x41960000, 0x00000000, 0xC0A80000, 0x41680000, 0x00000000,
    0xC0A80000, 0x415C0000, 0x00000000, 0xC0A80000, 0x41500000, 0x00000000,
    0xC0A80000, 0x41440000, 0x00000000, 0x40D00000, 0x41180000, 0x00000000,
    0x40D00000, 0x410C0000, 0x00000000, 0x40D00000, 0x41000000, 0x00000000,
    0x40D00000, 0x40E80000, 0x00000000, 0xC0B00000, 0x40700000, 0x00000000,
    0xC0880000, 0x40700000, 0x00000000, 0xC0400000, 0x40700000, 0x00000000,
    0xBFE00000, 0x40700000, 0x00000000, 0xBF000000, 0x40700000, 0x00000000,
    0x3F400000, 0x40700000, 0x00000000, 0x40000000, 0x40700000, 0x00000000,
    0x40500000, 0x40700000, 0x00000000, 0x40900000, 0x40700000, 0x00000000,
    0x40B80000, 0x40700000, 0x00000000, 0x40E00000, 0x40700000, 0x00000000,
    0xC0400000, 0x419A0000, 0x00000000, 0xBFE00000, 0x41980000, 0x00000000,
    0xBF000000, 0x41960000, 0x00000000, 0x3F400000, 0x41940000, 0x00000000,
    0x40000000, 0x41920000, 0x00000000, 0x40500000, 0x41900000, 0x00000000,
    0x40900000, 0x418E0000, 0x00000000, 0x40B80000, 0x418C0000, 0x00000000,
    0x40E00000, 0x418A0000, 0x00000000, 0x40500000, 0x41640000, 0x00000000,
    0x40000000, 0x41600000, 0x00000000, 0x3F400000, 0x41580000, 0x00000000,
    0xBF000000, 0x41540000, 0x00000000, 0xBFE00000, 0x414C0000, 0x00000000,
    0xC0400000, 0x41480000, 0x00000000, 0xC0880000, 0x41400000, 0x00000000,
    0xC0B00000, 0x413C0000, 0x00000000, 0x3FE00000, 0x410C0000, 0x00000000,
    0x40200000, 0x41080000, 0x00000000, 0x40500000, 0x41040000, 0x00000000,
    0x40800000, 0x41000000, 0x00000000, 0x40980000, 0x40F80000, 0x00000000,
    0x40B00000, 0x40F00000, 0x00000000, 0x40C80000, 0x40E80000, 0x00000000,
    0x3F400000, 0x40880000, 0x00000000, 0xBF000000, 0x40700000, 0x00000000,
    0xBFE00000, 0x40500000, 0x00000000, 0xC0400000, 0x40300000, 0x00000000,
    0xC0880000, 0x40100000, 0x00000000, 0xC0B00000, 0x3FE00000, 0x00000000,
    0xC0880000, 0x419C0000, 0x00000000, 0x40400000, 0x419C0000, 0x00000000,
    0xC0400000, 0x41900000, 0x00000000, 0x3FA00000, 0x41900000, 0x00000000,
    0x40880000, 0x41900000, 0x00000000, 0xBFC00000, 0x41860000, 0x00000000,
    0x40B00000, 0x41860000, 0x00000000, 0xBFE00000, 0x41680000, 0x00000000,
    0x40A80000, 0x41680000, 0x00000000, 0xC0400000, 0x41540000, 0x00000000,
    0x3E800000, 0x41540000, 0x00000000, 0x40880000, 0x41540000, 0x00000000,
    0xC0900000, 0x41400000, 0x00000000, 0x40300000, 0x41400000, 0x00000000,
    0xC0700000, 0x41140000, 0x00000000, 0x40300000, 0x41140000, 0x00000000,
    0xC0300000, 0x41000000, 0x00000000, 0x40800000, 0x41000000, 0x00000000,
    0x3F400000, 0x40F80000, 0x00000000, 0xBFE00000, 0x40E00000, 0x00000000,
    0x40A80000, 0x40E00000, 0x00000000, 0xC0100000, 0x40880000, 0x00000000,
    0x40A00000, 0x40880000, 0x00000000, 0xC0400000, 0x40600000, 0x00000000,
    0x40880000, 0x40600000, 0x00000000, 0x3F000000, 0x40500000, 0x00000000,
    0xC0700000, 0x40300000, 0x00000000, 0x40600000, 0x40300000, 0x00000000,
    0xC0900000, 0x40000000, 0x00000000, 0x40300000, 0x40000000, 0x00000000,
    0xC0B00000, 0x41960000, 0x00000000, 0xC0900000, 0x41960000, 0x00000000,
    0xC0600000, 0x41960000, 0x00000000, 0xC0200000, 0x41960000, 0x00000000,
    0xBFC00000, 0x41960000, 0x00000000, 0xBF000000, 0x41960000, 0x00000000,
    0x3F000000, 0x41960000, 0x00000000, 0x3FC00000, 0x41960000, 0x00000000,
    0x40200000, 0x41960000, 0x00000000, 0x40600000, 0x41960000, 0x00000000,
    0x40900000, 0x41960000, 0x00000000, 0x40B00000, 0x41920000, 0x00000000,
    0x40C80000, 0x418C0000, 0x00000000, 0x40D00000, 0x41840000, 0x00000000,
    0x40C00000, 0x41580000, 0x00000000, 0x40C00000, 0x41080000, 0x00000000,
    0x40D00000, 0x40700000, 0x00000000, 0x40C80000, 0x40300000, 0x00000000,
    0x40B00000, 0x40000000, 0x00000000, 0xC0B00000, 0x3FC00000, 0x00000000,
    0xC0900000, 0x3FC00000, 0x00000000, 0xC0600000, 0x3FC00000, 0x00000000,
    0xC0200000, 0x3FC00000, 0x00000000, 0xBFC00000, 0x3FC00000, 0x00000000,
    0xBF000000, 0x3FC00000, 0x00000000, 0x3F000000, 0x3FC00000, 0x00000000,
    0x3FC00000, 0x3FC00000, 0x00000000, 0x40200000, 0x3FC00000, 0x00000000,
    0x40600000, 0x3FC00000, 0x00000000, 0x40900000, 0x3FC00000, 0x00000000,
    0x40700000, 0x419A0000, 0x00000000, 0x40100000, 0x41980000, 0x00000000,
    0x40A00000, 0x41920000, 0x00000000, 0x40500000, 0x418C0000, 0x00000000,
    0x40C00000, 0x418A0000, 0x00000000, 0xC0880000, 0x41600000, 0x00000000,
    0x40100000, 0x41600000, 0x00000000, 0xC0200000, 0x415C0000, 0x00000000,
    0x40D00000, 0x415C0000, 0x00000000, 0x40800000, 0x41500000, 0x00000000,
    0xBF400000, 0x414C0000, 0x00000000, 0xC0A00000, 0x41440000, 0x00000000,
    0x40B00000, 0x41440000, 0x00000000, 0xC0300000, 0x41400000, 0x00000000,
    0x3FC00000, 0x41400000, 0x00000000, 0x3FE00000, 0x41100000, 0x00000000,
    0x40880000, 0x41100000, 0x00000000, 0x40C80000, 0x41100000, 0x00000000,
    0x40500000, 0x40F00000, 0x00000000, 0x40B00000, 0x40E00000, 0x00000000,
    0xBFC00000, 0x40700000, 0x00000000, 0x40880000, 0x40700000, 0x00000000,
    0xC0980000, 0x40600000, 0x00000000, 0x3F000000, 0x40600000, 0x00000000,
    0xC0500000, 0x40300000, 0x00000000, 0x40400000, 0x40200000, 0x00000000,
    0xBFE00000, 0x40100000, 0x00000000, 0x40B00000, 0x40100000, 0x00000000,
    0xC0900000, 0x3FE00000, 0x00000000, 0x3FC00000, 0x3FE00000, 0x00000000,
    0xC0400000, 0x41980000, 0x00000000, 0xBFE00000, 0x41980000, 0x00000000,
    0xBF000000, 0x41980000, 0x00000000, 0x3F400000, 0x41980000, 0x00000000,
    0x40000000, 0x41980000, 0x00000000, 0x40500000, 0x41980000, 0x00000000,
    0x40900000, 0x41980000, 0x00000000, 0x40B80000, 0x41980000, 0x00000000,
    0xC0800000, 0x41920000, 0x00000000, 0xC0880000, 0x418A0000, 0x00000000,
    0xC0880000, 0x41680000, 0x00000000, 0xC0880000, 0x41580000, 0x00000000,
    0xC0700000, 0x414C0000, 0x00000000, 0xC0300000, 0x41480000, 0x00000000,
    0xBFE00000, 0x41440000, 0x00000000, 0x40000000, 0x41140000, 0x00000000,
    0x40500000, 0x41100000, 0x00000000, 0x40880000, 0x410C0000, 0x00000000,
    0x40A00000, 0x41000000, 0x00000000, 0x40B00000, 0x40E00000, 0x00000000,
    0x40B00000, 0x40880000, 0x00000000, 0x40A00000, 0x40400000, 0x00000000,
    0x40880000, 0x40100000, 0x00000000, 0x40500000, 0x3FC00000, 0x00000000,
    0xC0880000, 0x3FA00000, 0x00000000, 0xC0500000, 0x3FA00000, 0x00000000,
    0xC0000000, 0x3FA00000, 0x00000000, 0xBF400000, 0x3FA00000, 0x00000000,
    0x3F000000, 0x3FA00000, 0x00000000, 0x3FE00000, 0x3FA00000, 0x00000000,
    0x40C80000, 0x419C0000, 0x00000000, 0x40B00000, 0x41980000, 0x00000000,
    0x40C80000, 0x41940000, 0x00000000, 0x40B00000, 0x41900000, 0x00000000,
    0x40C80000, 0x418C0000, 0x00000000, 0xC0A80000, 0x41600000, 0x00000000,
    0xC0900000, 0x415C0000, 0x00000000, 0xC0A80000, 0x41540000, 0x00000000,
    0xC0900000, 0x414C0000, 0x00000000, 0xC0A80000, 0x41480000, 0x00000000,
    0xC0600000, 0x41100000, 0x00000000, 0x40C80000, 0x410C0000, 0x00000000,
    0xC0600000, 0x41080000, 0x00000000, 0x40B80000, 0x41080000, 0x00000000,
    0xC0600000, 0x41000000, 0x00000000, 0x40C80000, 0x41000000, 0x00000000,
    0xC0600000, 0x40F00000, 0x00000000, 0x40B80000, 0x40F00000, 0x00000000,
    0x40C80000, 0x40E80000, 0x00000000, 0xC0600000, 0x40E00000, 0x00000000,
    0x40980000, 0x40880000, 0x00000000, 0xC0A80000, 0x40800000, 0x00000000,
    0x40980000, 0x40600000, 0x00000000, 0xC0A80000, 0x40500000, 0x00000000,
    0x40980000, 0x40300000, 0x00000000, 0xC0A80000, 0x40200000, 0x00000000,
    0x40980000, 0x40000000, 0x00000000, 0xC0A80000, 0x3FE00000, 0x00000000,
    0x40980000, 0x3FA00000, 0x00000000, 0xC0A80000, 0x3F800000, 0x00000000,
    0xC0A00000, 0x419A0000, 0x00000000, 0xC0700000, 0x41940000, 0x00000000,
    0xC0A00000, 0x418E0000, 0x00000000, 0xC0700000, 0x41880000, 0x00000000,
    0xC0A00000, 0x41600000, 0x00000000, 0xC0800000, 0x41540000, 0x00000000,
    0xC0A00000, 0x41480000, 0x00000000, 0xC0800000, 0x410C0000, 0x00000000,
    0x40A00000, 0x410C0000, 0x00000000, 0xC0A00000, 0x41000000, 0x00000000,
    0xC0400000, 0x41000000, 0x00000000, 0x40800000, 0x41000000, 0x00000000,
    0x40C00000, 0x41000000, 0x00000000, 0xC0800000, 0x40E80000, 0x00000000,
    0x40A00000, 0x40E80000, 0x00000000, 0xC0A80000, 0x40600000, 0x00000000,
    0xC0300000, 0x40600000, 0x00000000, 0xBE800000, 0x40600000, 0x00000000,
    0x40100000, 0x40600000, 0x00000000, 0x40980000, 0x40600000, 0x00000000,
    0xC0800000, 0x40300000, 0x00000000, 0xBFC00000, 0x40300000, 0x00000000,
    0x3F800000, 0x40300000, 0x00000000, 0x40600000, 0x40300000, 0x00000000,
    0x40C00000, 0x40300000, 0x00000000, 0xC0A80000, 0x40000000, 0x00000000,
    0xC0300000, 0x40000000, 0x00000000, 0xBE800000, 0x40000000, 0x00000000,
    0x40100000, 0x40000000, 0x00000000, 0x40980000, 0x40000000, 0x00000000,
    0x40E00000, 0x419C0000, 0x00000000, 0x40980000, 0x41980000, 0x00000000,
    0x40B80000, 0x41960000, 0x00000000, 0x40900000, 0x41900000, 0x00000000,
    0x40500000, 0x418A0000, 0x00000000, 0x3FA00000, 0x41880000, 0x00000000,
    0x40000000, 0x41840000, 0x00000000, 0x00000000, 0x41820000, 0x00000000,
    0x40400000, 0x41820000, 0x00000000, 0x3F400000, 0x417C0000, 0x00000000,
    0x3FE00000, 0x41780000, 0x00000000, 0xBF000000, 0x41700000, 0x00000000,
    0xBFE00000, 0x41640000, 0x00000000, 0xC0400000, 0x41580000, 0x00000000,
    0xC0000000, 0x41540000, 0x00000000, 0xC0880000, 0x414C0000, 0x00000000,
    0xC0B00000, 0x41400000, 0x00000000, 0xC0B80000, 0x41180000, 0x00000000,
    0xC0A00000, 0x41140000, 0x00000000, 0xC0880000, 0x41100000, 0x00000000,
    0xC0600000, 0x410C0000, 0x00000000, 0xC0300000, 0x41080000, 0x00000000,
    0xC0000000, 0x41040000, 0x00000000, 0xBFA00000, 0x41000000, 0x00000000,
    0xBF000000, 0x40F80000, 0x00000000, 0xBFA00000, 0x40880000, 0x00000000,
    0xC0100000, 0x40700000, 0x00000000, 0xC0500000, 0x40500000, 0x00000000,
    0xC0880000, 0x40300000, 0x00000000, 0xC0A80000, 0x40100000, 0x00000000,
    0x40A00000, 0x419A0000, 0x00000000, 0xC0A00000, 0x41980000, 0x00000000,
    0xBE800000, 0x41920000, 0x00000000, 0x40000000, 0x41920000, 0x00000000,
    0xC0200000, 0x41900000, 0x00000000, 0x40600000, 0x41900000, 0x00000000,
    0xC0880000, 0x418E0000, 0x00000000, 0x40A80000, 0x418A0000, 0x00000000,
    0x3FA00000, 0x41880000, 0x00000000, 0xBF000000, 0x41840000, 0x00000000,
    0xC0A80000, 0x41820000, 0x00000000, 0x40300000, 0x41820000, 0x00000000,
    0xC0100000, 0x41800000, 0x00000000, 0xC0700000, 0x41700000, 0x00000000,
    0x3F800000, 0x416C0000, 0x00000000, 0x40A00000, 0x41680000, 0x00000000,
    0xBF400000, 0x41600000, 0x00000000, 0x40000000, 0x415C0000, 0x00000000,
    0xC0A80000, 0x41580000, 0x00000000, 0xC0300000, 0x41580000, 0x00000000,
    0x40600000, 0x410C0000, 0x00000000, 0x40B80000, 0x41080000, 0x00000000,
    0x40100000, 0x41040000, 0x00000000, 0x40900000, 0x41000000, 0x00000000,
    0x40600000, 0x40E80000, 0x00000000, 0xC0100000, 0x40880000, 0x00000000,
    0xC0600000, 0x40700000, 0x00000000, 0xC0000000, 0x40500000, 0x00000000,
    0xC0900000, 0x40400000, 0x00000000, 0xC0400000, 0x40100000, 0x00000000,
    0xC0880000, 0x41960000, 0x00000000, 0x3F400000, 0x41960000, 0x00000000,
    0x40B00000, 0x41960000, 0x00000000, 0xC0880000, 0x418E0000, 0x00000000,
    0x3F400000, 0x418E0000, 0x00000000, 0x40B00000, 0x418E0000, 0x00000000,
    0xC0880000, 0x41860000, 0x00000000, 0x3F400000, 0x41860000, 0x00000000,
    0x40B00000, 0x41860000, 0x00000000, 0xC0880000, 0x417C0000, 0x00000000,
    0x3F400000, 0x417C0000, 0x00000000, 0x40B00000, 0x417C0000, 0x00000000,
    0xC0880000, 0x41140000, 0x00000000, 0x3F400000, 0x41140000, 0x00000000,
    0x40B00000, 0x41140000, 0x00000000, 0xC0880000, 0x41040000, 0x00000000,
    0x3F400000, 0x41040000, 0x00000000, 0x40B00000, 0x41040000, 0x00000000,
    0xC0880000, 0x40E80000, 0x00000000, 0x3F400000, 0x40E80000, 0x00000000,
    0x40B00000, 0x40E80000, 0x00000000, 0xC0880000, 0x40880000, 0x00000000,
    0x3F400000, 0x40880000, 0x00000000, 0x40B00000, 0x40880000, 0x00000000,
    0xC0880000, 0x40500000, 0x00000000, 0x3F400000, 0x40500000, 0x00000000,
    0x40B00000, 0x40500000, 0x00000000, 0xC0880000, 0x40100000, 0x00000000,
    0x3F400000, 0x40100000, 0x00000000, 0x40B00000, 0x40100000, 0x00000000,
    0xC0B00000, 0x418E0000, 0x00000000, 0xC0880000, 0x418E0000, 0x00000000,
    0xC0400000, 0x418E0000, 0x00000000, 0xBFE00000, 0x418E0000, 0x00000000,
    0xBF000000, 0x418E0000, 0x00000000, 0x3F400000, 0x418E0000, 0x00000000,
    0x40000000, 0x418E0000, 0x00000000, 0x40500000, 0x418E0000, 0x00000000,
    0x40900000, 0x418E0000, 0x00000000, 0x40B80000, 0x418E0000, 0x00000000,
    0x40B80000, 0x41860000, 0x00000000, 0x40B80000, 0x417C0000, 0x00000000,
    0x40B80000, 0x416C0000, 0x00000000, 0x40B80000, 0x415C0000, 0x00000000,
    0xC0B00000, 0x414C0000, 0x00000000, 0xC0880000, 0x414C0000, 0x00000000,
    0xC0400000, 0x414C0000, 0x00000000, 0xBFE00000, 0x414C0000, 0x00000000,
    0xBF000000, 0x414C0000, 0x00000000, 0x3F400000, 0x414C0000, 0x00000000,
    0x40000000, 0x414C0000, 0x00000000, 0x40500000, 0x414C0000, 0x00000000,
    0x40900000, 0x414C0000, 0x00000000, 0x40B80000, 0x414C0000, 0x00000000,
    0xC0400000, 0x41040000, 0x00000000, 0x3F400000, 0x41040000, 0x00000000,
    0x40900000, 0x41040000, 0x00000000, 0xC0400000, 0x40500000, 0x00000000,
    0x3F400000, 0x40500000, 0x00000000, 0x40900000, 0x40500000, 0x00000000,
    0xC0900000, 0x41980000, 0x00000000, 0xC0500000, 0x41980000, 0x00000000,
    0xC0000000, 0x41980000, 0x00000000, 0xBF400000, 0x41980000, 0x00000000,
    0x3F000000, 0x41980000, 0x00000000, 0x3FE00000, 0x41980000, 0x00000000,
    0x40400000, 0x41980000, 0x00000000, 0x40880000, 0x41980000, 0x00000000,
    0x40B00000, 0x41980000, 0x00000000, 0x40D80000, 0x41980000, 0x00000000,
    0xC0900000, 0x41900000, 0x00000000, 0x40D80000, 0x418E0000, 0x00000000,
    0xC0900000, 0x41880000, 0x00000000, 0x40D80000, 0x41840000, 0x00000000,
    0xC0900000, 0x41800000, 0x00000000, 0x40D80000, 0x41740000, 0x00000000,
    0xC0900000, 0x41700000, 0x00000000, 0xC0900000, 0x41600000, 0x00000000,
    0x40D80000, 0x41600000, 0x00000000, 0xC0900000, 0x41500000, 0x00000000,
    0x40D80000, 0x414C0000, 0x00000000, 0xC0900000, 0x41400000, 0x00000000,
    0xC0900000, 0x41180000, 0x00000000, 0xC0900000, 0x410C0000, 0x00000000,
    0xC0900000, 0x41000000, 0x00000000, 0xC0900000, 0x40E80000, 0x00000000,
    0xC0900000, 0x40900000, 0x00000000, 0xC0900000, 0x40700000, 0x00000000,
    0xC0900000, 0x40400000, 0x00000000, 0xC0900000, 0x40100000, 0x00000000,
    0x40D80000, 0x419C0000, 0x00000000, 0x40B80000, 0x41980000, 0x00000000,
    0x40D80000, 0x41960000, 0x00000000, 0x40B80000, 0x41920000, 0x00000000,
    0x40D80000, 0x41900000, 0x00000000, 0x40B80000, 0x418C0000, 0x00000000,
    0x40D80000, 0x418A0000, 0x00000000, 0x40D80000, 0x41640000, 0x00000000,
    0x40D80000, 0x41580000, 0x00000000, 0x40B80000, 0x41500000, 0x00000000,
    0x40980000, 0x41480000, 0x00000000, 0x40700000, 0x41400000, 0x00000000,
    0x40300000, 0x41380000, 0x00000000, 0x3FE00000, 0x41300000, 0x00000000,
    0x3F400000, 0x41280000, 0x00000000, 0xBE800000, 0x41200000, 0x00000000,
    0xBFA00000, 0x41180000, 0x00000000, 0xC0100000, 0x41100000, 0x00000000,
    0xC0500000, 0x41080000, 0x00000000, 0xC0880000, 0x41000000, 0x00000000,
    0xC0A80000, 0x40F00000, 0x00000000, 0xC0A80000, 0x40D80000, 0x00000000,
    0xC0A80000, 0x40900000, 0x00000000, 0xC0800000, 0x40880000, 0x00000000,
    0xC0A80000, 0x40700000, 0x00000000, 0xC0800000, 0x40600000, 0x00000000,
    0xC0A80000, 0x40400000, 0x00000000, 0xC0800000, 0x40300000, 0x00000000,
    0xC0A80000, 0x40100000, 0x00000000, 0xC0800000, 0x40000000, 0x00000000,
    0x40700000, 0x419A0000, 0x00000000, 0x40A80000, 0x419A0000, 0x00000000,
    0x40D80000, 0x419A0000, 0x00000000, 0x40400000, 0x41920000, 0x00000000,
    0x40900000, 0x41920000, 0x00000000, 0x40C00000, 0x41920000, 0x00000000,
    0x40D00000, 0x415C0000, 0x00000000, 0xBFE00000, 0x41580000, 0x00000000,
    0xC0400000, 0x41500000, 0x00000000, 0xBF000000, 0x41500000, 0x00000000,
    0x40C80000, 0x41440000, 0x00000000, 0xC0700000, 0x413C0000, 0x00000000,
    0x3E800000, 0x41380000, 0x00000000, 0x40C00000, 0x412C0000, 0x00000000,
    0xC0880000, 0x41240000, 0x00000000, 0x3F400000, 0x41240000, 0x00000000,
    0x40B00000, 0x41140000, 0x00000000, 0x3FC00000, 0x41100000, 0x00000000,
    0xC0900000, 0x410C0000, 0x00000000, 0x40980000, 0x41000000, 0x00000000,
    0x40100000, 0x40F80000, 0x00000000, 0xC0980000, 0x40E80000, 0x00000000,
    0x40600000, 0x40E80000, 0x00000000, 0xC0B80000, 0x40800000, 0x00000000,
    0xC0880000, 0x40800000, 0x00000000, 0xC0300000, 0x40800000, 0x00000000,
    0xBFA00000, 0x40800000, 0x00000000, 0xC0A00000, 0x40400000, 0x00000000,
    0xC0600000, 0x40400000, 0x00000000, 0xC0000000, 0x40400000, 0x00000000,
    0xC0B00000, 0x41640000, 0x00000000, 0xC0B00000, 0x41540000, 0x00000000,
    0xC0B00000, 0x41440000, 0x00000000, 0xC0B00000, 0x41340000, 0x00000000,
    0xC0900000, 0x41340000, 0x00000000, 0xC0600000, 0x41340000, 0x00000000,
    0xC0200000, 0x41340000, 0x00000000, 0xBFC00000, 0x41340000, 0x00000000,
    0xBF000000, 0x41340000, 0x00000000, 0x3F000000, 0x41340000, 0x00000000,
    0x3FC00000, 0x41340000, 0x00000000, 0x40200000, 0x41340000, 0x00000000,
    0x40600000, 0x41340000, 0x00000000, 0x40900000, 0x41340000, 0x00000000,
    0x40B00000, 0x41340000, 0x00000000, 0x40D80000, 0x41200000, 0x00000000,
    0x40D80000, 0x41140000, 0x00000000, 0x40D80000, 0x41080000, 0x00000000,
    0xC0880000, 0x40F00000, 0x00000000, 0xC0500000, 0x40F00000, 0x00000000,
    0xC0100000, 0x40F00000, 0x00000000, 0xBFA00000, 0x40F00000, 0x00000000,
    0xBE800000, 0x40F00000, 0x00000000, 0x3F400000, 0x40F00000, 0x00000000,
    0x3FE00000, 0x40F00000, 0x00000000, 0x40300000, 0x40F00000, 0x00000000,
    0x40700000, 0x40F00000, 0x00000000, 0x40980000, 0x40F00000, 0x00000000,
    0x40B80000, 0x40F00000, 0x00000000, 0x40D80000, 0x40F00000, 0x00000000,
    0xC0900000, 0x41600000, 0x00000000, 0xC0600000, 0x41600000, 0x00000000,
    0xC0200000, 0x41600000, 0x00000000, 0xBFC00000, 0x41600000, 0x00000000,
    0xBF000000, 0x41600000, 0x00000000, 0x3F000000, 0x41600000, 0x00000000,
    0x3FC00000, 0x41600000, 0x00000000, 0x40200000, 0x41600000, 0x00000000,
    0x40600000, 0x41600000, 0x00000000, 0x40900000, 0x41600000, 0x00000000,
    0x40B00000, 0x41600000, 0x00000000, 0x40D00000, 0x41600000, 0x00000000,
    0x40D00000, 0x41500000, 0x00000000, 0x40D00000, 0x41400000, 0x00000000,
    0x40D00000, 0x412C0000, 0x00000000, 0xC0A80000, 0x41280000, 0x00000000,
    0xC0A80000, 0x41140000, 0x00000000, 0xC0A80000, 0x41040000, 0x00000000,
    0xC0A80000, 0x40E00000, 0x00000000, 0xC0880000, 0x40E00000, 0x00000000,
    0xC0500000, 0x40E00000, 0x00000000, 0xC0100000, 0x40E00000, 0x00000000,
    0xBFA00000, 0x40E00000, 0x00000000, 0xBE800000, 0x40E00000, 0x00000000,
    0x3F400000, 0x40E00000, 0x00000000, 0x3FE00000, 0x40E00000, 0x00000000,
    0x40300000, 0x40E00000, 0x00000000, 0x40700000, 0x40E00000, 0x00000000,
    0x40980000, 0x40E00000, 0x00000000, 0x40B80000, 0x40E00000, 0x00000000,
    0xC0980000, 0x41940000, 0x00000000, 0xC0700000, 0x41940000, 0x00000000,
    0xC0300000, 0x41940000, 0x00000000, 0xBFE00000, 0x41940000, 0x00000000,
    0xBF400000, 0x41940000, 0x00000000, 0x3E800000, 0x41940000, 0x00000000,
    0x3FA00000, 0x41940000, 0x00000000, 0x40100000, 0x41940000, 0x00000000,
    0x40500000, 0x41940000, 0x00000000, 0x40880000, 0x41940000, 0x00000000,
    0x40A80000, 0x41940000, 0x00000000, 0x40C80000, 0x41940000, 0x00000000,
    0xC0A00000, 0x415C0000, 0x00000000, 0x40C80000, 0x41500000, 0x00000000,
    0xC0A00000, 0x41340000, 0x00000000, 0x40C80000, 0x412C0000, 0x00000000,
    0xC0A00000, 0x410C0000, 0x00000000, 0x40C80000, 0x41080000, 0x00000000,
    0xC0A00000, 0x40500000, 0x00000000, 0xC0800000, 0x40500000, 0x00000000,
    0xC0400000, 0x40500000, 0x00000000, 0xC0000000, 0x40500000, 0x00000000,
    0xBF800000, 0x40500000, 0x00000000, 0x00000000, 0x40500000, 0x00000000,
    0x3F800000, 0x40500000, 0x00000000, 0x40000000, 0x40500000, 0x00000000,
    0x40400000, 0x40500000, 0x00000000, 0x40800000, 0x40500000, 0x00000000,
    0x40A00000, 0x40500000, 0x00000000, 0x40C00000, 0x40500000, 0x00000000,
    0x40B00000, 0x41980000, 0x00000000, 0x40980000, 0x41960000, 0x00000000,
    0x40800000, 0x41940000, 0x00000000, 0x40500000, 0x41920000, 0x00000000,
    0x40200000, 0x41900000, 0x00000000, 0x3FE00000, 0x418E0000, 0x00000000,
    0x3F800000, 0x418C0000, 0x00000000, 0x3E800000, 0x418A0000, 0x00000000,
    0x3E800000, 0x415C0000, 0x00000000, 0xBF000000, 0x41580000, 0x00000000,
    0xBFA00000, 0x41540000, 0x00000000, 0xC0000000, 0x41500000, 0x00000000,
    0xC0300000, 0x414C0000, 0x00000000, 0xC0600000, 0x41480000, 0x00000000,
    0xC0880000, 0x41440000, 0x00000000, 0xC0A00000, 0x41400000, 0x00000000,
    0xC0B80000, 0x413C0000, 0x00000000, 0xC0B80000, 0x41080000, 0x00000000,
    0xC0980000, 0x41000000, 0x00000000, 0xC0700000, 0x40F00000, 0x00000000,
    0xC0300000, 0x40E00000, 0x00000000, 0xBFE00000, 0x40D00000, 0x00000000,
    0xBF400000, 0x40C00000, 0x00000000, 0x3E800000, 0x40B00000, 0x00000000,
    0x3FA00000, 0x40A00000, 0x00000000, 0x40100000, 0x40900000, 0x00000000,
    0x40500000, 0x40800000, 0x00000000, 0x40880000, 0x40600000, 0x00000000,
    0x40A80000, 0x40400000, 0x00000000, 0x40C80000, 0x40200000, 0x00000000,
    0xC0980000, 0x41980000, 0x00000000, 0xC0600000, 0x41980000, 0x00000000,
    0xC0100000, 0x41980000, 0x00000000, 0xBF800000, 0x41980000, 0x00000000,
    0x3E800000, 0x41980000, 0x00000000, 0x3FC00000, 0x41980000, 0x00000000,
    0x40300000, 0x41980000, 0x00000000, 0x40800000, 0x41960000, 0x00000000,
    0x40A00000, 0x41900000, 0x00000000, 0x40B00000, 0x418A0000, 0x00000000,
    0x40B80000, 0x41680000, 0x00000000, 0x40A00000, 0x41540000, 0x00000000,
    0x40700000, 0x41480000, 0x00000000, 0xC0000000, 0x41100000, 0x00000000,
    0xC0600000, 0x41080000, 0x00000000, 0xC0900000, 0x40F00000, 0x00000000,
    0xC0A00000, 0x40C80000, 0x00000000, 0xC0A00000, 0x40A00000, 0x00000000,
    0xC0980000, 0x40800000, 0x00000000, 0xC0800000, 0x40500000, 0x00000000,
    0xC0400000, 0x40300000, 0x00000000, 0xC0000000, 0x40200000, 0x00000000,
    0xBF800000, 0x40200000, 0x00000000, 0x00000000, 0x40200000, 0x00000000,
    0x3F800000, 0x40200000, 0x00000000, 0x40000000, 0x40200000, 0x00000000,
    0x40400000, 0x40200000, 0x00000000, 0x40800000, 0x40200000, 0x00000000,
    0x40A00000, 0x40200000, 0x00000000, 0x40C00000, 0x40200000, 0x00000000,
    0xC0900000, 0x41980000, 0x00000000, 0x40B00000, 0x41980000, 0x00000000,
    0xC0800000, 0x41940000, 0x00000000, 0x40A00000, 0x41940000, 0x00000000,
    0xC0600000, 0x41900000, 0x00000000, 0x40900000, 0x41900000, 0x00000000,
    0xBFE00000, 0x41640000, 0x00000000, 0x40100000, 0x41640000, 0x00000000,
    0xBF400000, 0x41540000, 0x00000000, 0x3FA00000, 0x41540000, 0x00000000,
    0x3E800000, 0x41480000, 0x00000000, 0x3F400000, 0x41140000, 0x00000000,
    0x00000000, 0x41080000, 0x00000000, 0x3FC00000, 0x41080000, 0x00000000,
    0xBF400000, 0x40F80000, 0x00000000, 0x40100000, 0x40F80000, 0x00000000,
    0xBFC00000, 0x40E00000, 0x00000000, 0x40400000, 0x40E00000, 0x00000000,
    0xC0100000, 0x40C80000, 0x00000000, 0x40700000, 0x40C80000, 0x00000000,
    0xC0400000, 0x40B00000, 0x00000000, 0x40900000, 0x40B00000, 0x00000000,
    0xC0700000, 0x40980000, 0x00000000, 0x40A80000, 0x40980000, 0x00000000,
    0xC0900000, 0x40800000, 0x00000000, 0x40C00000, 0x40800000, 0x00000000,
    0xC0A80000, 0x40500000, 0x00000000, 0x40D80000, 0x40500000, 0x00000000,
    0xC0C00000, 0x40200000, 0x00000000, 0x40F00000, 0x40200000, 0x00000000,
    0x40A00000, 0x419A0000, 0x00000000, 0xC0980000, 0x41960000, 0x00000000,
    0x3FA00000, 0x41960000, 0x00000000, 0xC0000000, 0x41940000, 0x00000000,
    0x40200000, 0x41900000, 0x00000000, 0x40800000, 0x41900000, 0x00000000,
    0xC0600000, 0x418E0000, 0x00000000, 0xBF000000, 0x418E0000, 0x00000000,
    0xC0100000, 0x418C0000, 0x00000000, 0x40B80000, 0x418A0000, 0x00000000,
    0xBFE00000, 0x41540000, 0x00000000, 0xBF000000, 0x41540000, 0x00000000,
    0x3F400000, 0x41540000, 0x00000000, 0x40000000, 0x41540000, 0x00000000,
    0x40500000, 0x41540000, 0x00000000, 0xC0A80000, 0x41100000, 0x00000000,
    0x40D80000, 0x41100000, 0x00000000, 0x40C00000, 0x41040000, 0x00000000,
    0xC0880000, 0x41000000, 0x00000000, 0x40A80000, 0x40F00000, 0x00000000,
    0xC0500000, 0x40E00000, 0x00000000, 0x40900000, 0x40D80000, 0x00000000,
    0xC0100000, 0x40C00000, 0x00000000, 0x40700000, 0x40C00000, 0x00000000,
    0x40400000, 0x40A80000, 0x00000000, 0xBFA00000, 0x40A00000, 0x00000000,
    0x40100000, 0x40900000, 0x00000000, 0xBE800000, 0x40800000, 0x00000000,
    0x3FC00000, 0x40700000, 0x00000000, 0x3F400000, 0x40400000, 0x00000000,
    0xC0A00000, 0x41960000, 0x00000000, 0xC0A00000, 0x41900000, 0x00000000,
    0xC0A00000, 0x418A0000, 0x00000000, 0xC0A00000, 0x41600000, 0x00000000,
    0xC0A00000, 0x41540000, 0x00000000, 0xC0A00000, 0x41480000, 0x00000000,
    0x40C00000, 0x410C0000, 0x00000000, 0xC0A00000, 0x41080000, 0x00000000,
    0xC0A00000, 0x40F80000, 0x00000000, 0x40C00000, 0x40F80000, 0x00000000,
    0xC0A00000, 0x40E00000, 0x00000000, 0x40C00000, 0x40D80000, 0x00000000,
    0xC0A00000, 0x40C80000, 0x00000000, 0x40C00000, 0x40B80000, 0x00000000,
    0xC0A00000, 0x40B00000, 0x00000000, 0xC0A00000, 0x40980000, 0x00000000,
    0x40C00000, 0x40980000, 0x00000000, 0xC0A00000, 0x40800000, 0x00000000,
    0x40C00000, 0x40700000, 0x00000000, 0xC0A00000, 0x40500000, 0x00000000,
    0x40C00000, 0x40300000, 0x00000000, 0xC0A00000, 0x40200000, 0x00000000,
    0xC0700000, 0x40200000, 0x00000000, 0xC0200000, 0x40200000, 0x00000000,
    0xBFA00000, 0x40200000, 0x00000000, 0x00000000, 0x40200000, 0x00000000,
    0x3FA00000, 0x40200000, 0x00000000, 0x40200000, 0x40200000, 0x00000000,
    0x40700000, 0x40200000, 0x00000000, 0x40A00000, 0x40200000, 0x00000000,
    0x3F000000, 0x419C0000, 0x00000000, 0xBF800000, 0x419A0000, 0x00000000,
    0x40100000, 0x419A0000, 0x00000000, 0xC0100000, 0x41960000, 0x00000000,
    0x40700000, 0x41960000, 0x00000000, 0x40980000, 0x41900000, 0x00000000,
    0xC0500000, 0x418E0000, 0x00000000, 0x40B80000, 0x41880000, 0x00000000,
    0xC0880000, 0x41860000, 0x00000000, 0x40C80000, 0x417C0000, 0x00000000,
    0xC0A00000, 0x41740000, 0x00000000, 0x40D80000, 0x41680000, 0x00000000,
    0xC0B00000, 0x415C0000, 0x00000000, 0xC0B80000, 0x41400000, 0x00000000,
    0xC0B80000, 0x41200000, 0x00000000, 0xC0B00000, 0x41080000, 0x00000000,
    0xC0A80000, 0x40E00000, 0x00000000, 0xC0B00000, 0x40B00000, 0x00000000,
    0xC0900000, 0x40B00000, 0x00000000, 0xC0B00000, 0x40900000, 0x00000000,
    0xC0900000, 0x40900000, 0x00000000, 0xC0B00000, 0x40600000, 0x00000000,
    0xC0900000, 0x40600000, 0x00000000, 0xC0600000, 0x40600000, 0x00000000,
    0xC0B00000, 0x40200000, 0x00000000, 0xC0900000, 0x40200000, 0x00000000,
    0xC0600000, 0x40200000, 0x00000000, 0xC0100000, 0x40200000, 0x00000000,
    0xBF400000, 0x40000000, 0x00000000, 0x3F400000, 0x3FE00000, 0x00000000,
    0xC0400000, 0x410C0000, 0x00000000, 0xBE800000, 0x41080000, 0x00000000,
    0x40400000, 0x41080000, 0x00000000, 0x40C80000, 0x41040000, 0x00000000,
    0xC0A00000, 0x40F80000, 0x00000000, 0x3FC00000, 0x40F80000, 0x00000000,
    0x40880000, 0x40F80000, 0x00000000, 0xC0100000, 0x40E00000, 0x00000000,
    0x40B00000, 0x40E00000, 0x00000000, 0xC0600000, 0x40D80000, 0x00000000,
    0xBE800000, 0x40D80000, 0x00000000, 0x40100000, 0x40D80000, 0x00000000,
    0x40700000, 0x40C80000, 0x00000000, 0xC0800000, 0x40B00000, 0x00000000,
    0xBFE00000, 0x40B00000, 0x00000000, 0x3E800000, 0x40B00000, 0x00000000,
    0x40880000, 0x40A80000, 0x00000000, 0x40C00000, 0x40A80000, 0x00000000,
    0x3FC00000, 0x40980000, 0x00000000, 0x40400000, 0x40900000, 0x00000000,
    0xBF800000, 0x40880000, 0x00000000, 0xC0700000, 0x40700000, 0x00000000,
    0x40980000, 0x40700000, 0x00000000, 0xC0100000, 0x40600000, 0x00000000,
    0x40000000, 0x40500000, 0x00000000, 0xC0900000, 0x40300000, 0x00000000,
    0x40700000, 0x40300000, 0x00000000, 0x40C80000, 0x40300000, 0x00000000,
    0xBFA00000, 0x40200000, 0x00000000, 0x3F800000, 0x40200000, 0x00000000,
    0x40000000, 0x419A0000, 0x00000000, 0x40500000, 0x419A0000, 0x00000000,
    0x40900000, 0x419A0000, 0x00000000, 0x40B80000, 0x419A0000, 0x00000000,
    0x40E00000, 0x419A0000, 0x00000000, 0x40500000, 0x41900000, 0x00000000,
    0x40900000, 0x41900000, 0x00000000, 0x40B80000, 0x41900000, 0x00000000,
    0x40E00000, 0x41900000, 0x00000000, 0x40900000, 0x41860000, 0x00000000,
    0x40B80000, 0x41860000, 0x00000000, 0x40E00000, 0x41860000, 0x00000000,
    0x40B80000, 0x41780000, 0x00000000, 0x40E00000, 0x41780000, 0x00000000,
    0x40E00000, 0x41640000, 0x00000000, 0xC0B80000, 0x40C00000, 0x00000000,
    0xC0B80000, 0x40A00000, 0x00000000, 0xC0980000, 0x40A00000, 0x00000000,
    0xC0B80000, 0x40800000, 0x00000000, 0xC0980000, 0x40800000, 0x00000000,
    0xC0700000, 0x40800000, 0x00000000, 0xC0B80000, 0x40400000, 0x00000000,
    0xC0980000, 0x40400000, 0x00000000, 0xC0700000, 0x40400000, 0x00000000,
    0xC0300000, 0x40400000, 0x00000000, 0xC0B80000, 0x40000000, 0x00000000,
    0xC0980000, 0x40000000, 0x00000000, 0xC0700000, 0x40000000, 0x00000000,
    0xC0300000, 0x40000000, 0x00000000, 0xBFE00000, 0x40000000, 0x00000000,
    0xC0A80000, 0x41960000, 0x00000000, 0x40C80000, 0x418E0000, 0x00000000,
    0xC0A80000, 0x418C0000, 0x00000000, 0xC0A80000, 0x41640000, 0x00000000,
    0x40D00000, 0x41640000, 0x00000000, 0xC0A80000, 0x41540000, 0x00000000,
    0x40D00000, 0x41540000, 0x00000000, 0xC0A80000, 0x41440000, 0x00000000,
    0x40D00000, 0x41440000, 0x00000000, 0xC0B00000, 0x410C0000, 0x00000000,
    0xC0800000, 0x410C0000, 0x00000000, 0xC0200000, 0x410C0000, 0x00000000,
    0xBF400000, 0x410C0000, 0x00000000, 0x3F400000, 0x410C0000, 0x00000000,
    0x40100000, 0x410C0000, 0x00000000, 0x40700000, 0x410C0000, 0x00000000,
    0x40A80000, 0x410C0000, 0x00000000, 0x40D80000, 0x410C0000, 0x00000000,
    0x40D80000, 0x40E80000, 0x00000000, 0x40D80000, 0x40B80000, 0x00000000,
    0x40D80000, 0x40800000, 0x00000000, 0xC0B00000, 0x40200000, 0x00000000,
    0xC0800000, 0x40200000, 0x00000000, 0xC0100000, 0x40200000, 0x00000000,
    0xBF400000, 0x40200000, 0x00000000, 0x3F400000, 0x40200000, 0x00000000,
    0x40100000, 0x40200000, 0x00000000, 0x40700000, 0x40200000, 0x00000000,
    0x40A80000, 0x40200000, 0x00000000, 0x40D80000, 0x40200000, 0x00000000,
    0xC0600000, 0x41100000, 0x00000000, 0xC0100000, 0x41100000, 0x00000000,
    0xBF400000, 0x41100000, 0x00000000, 0x3F400000, 0x41100000, 0x00000000,
    0x40100000, 0x41100000, 0x00000000, 0x40800000, 0x41100000, 0x00000000,
    0x40B00000, 0x41100000, 0x00000000, 0xC0900000, 0x41080000, 0x00000000,
    0xC0980000, 0x40E80000, 0x00000000, 0xC0880000, 0x40C80000, 0x00000000,
    0xC0600000, 0x40B80000, 0x00000000, 0xC0200000, 0x40B80000, 0x00000000,
    0xBFC00000, 0x40B80000, 0x00000000, 0xBE800000, 0x40B80000, 0x00000000,
    0x3F400000, 0x40B80000, 0x00000000, 0x3FE00000, 0x40B80000, 0x00000000,
    0x40300000, 0x40B80000, 0x00000000, 0x40700000, 0x40B80000, 0x00000000,
    0x40980000, 0x40B00000, 0x00000000, 0x40B00000, 0x40A00000, 0x00000000,
    0x40B80000, 0x40880000, 0x00000000, 0x40B00000, 0x40600000, 0x00000000,
    0x40980000, 0x40300000, 0x00000000, 0xC0880000, 0x40200000, 0x00000000,
    0xC0300000, 0x40200000, 0x00000000, 0xBF800000, 0x40200000, 0x00000000,
    0x3F400000, 0x40200000, 0x00000000, 0x3FE00000, 0x40200000, 0x00000000,
    0x40300000, 0x40200000, 0x00000000, 0x40700000, 0x40200000, 0x00000000,
    0xC0400000, 0x410C0000, 0x00000000, 0xBE800000, 0x41080000, 0x00000000,
    0x40400000, 0x41080000, 0x00000000, 0x40C80000, 0x41040000, 0x00000000,
    0xC0A00000, 0x40F80000, 0x00000000, 0x3FC00000, 0x40F80000, 0x00000000,
    0x40880000, 0x40F80000, 0x00000000, 0xC0100000, 0x40E00000, 0x00000000,
    0x40B00000, 0x40E00000, 0x00000000, 0xC0600000, 0x40D80000, 0x00000000,
    0xBE800000, 0x40D80000, 0x00000000, 0x40100000, 0x40D80000, 0x00000000,
    0x40700000, 0x40C80000, 0x00000000, 0xC0800000, 0x40B00000, 0x00000000,
    0xBFE00000, 0x40B00000, 0x00000000, 0x3E800000, 0x40B00000, 0x00000000,
    0x40880000, 0x40A80000, 0x00000000, 0x40C00000, 0x40A80000, 0x00000000,
    0x3FC00000, 0x40980000, 0x00000000, 0x40400000, 0x40900000, 0x00000000,
    0xBF800000, 0x40880000, 0x00000000, 0xC0700000, 0x40700000, 0x00000000,
    0x40980000, 0x40700000, 0x00000000, 0xC0100000, 0x40600000, 0x00000000,
    0x40000000, 0x40500000, 0x00000000, 0xC0900000, 0x40300000, 0x00000000,
    0x40700000, 0x40300000, 0x00000000, 0x40C80000, 0x40300000, 0x00000000,
    0xBFA00000, 0x40200000, 0x00000000, 0x3F800000, 0x40200000, 0x00000000,
    0xC0400000, 0x410C0000, 0x00000000, 0xBE800000, 0x41080000, 0x00000000,
    0x40400000, 0x41080000, 0x00000000, 0x40C80000, 0x41040000, 0x00000000,
    0xC0A00000, 0x40F80000, 0x00000000, 0x3FC00000, 0x40F80000, 0x00000000,
    0x40880000, 0x40F80000, 0x00000000, 0xC0100000, 0x40E00000, 0x00000000,
    0x40B00000, 0x40E00000, 0x00000000, 0xC0600000, 0x40D80000, 0x00000000,
    0xBE800000, 0x40D80000, 0x00000000, 0x40100000, 0x40D80000, 0x00000000,
    0x40700000, 0x40C80000, 0x00000000, 0xC0800000, 0x40B00000, 0x00000000,
    0xBFE00000, 0x40B00000, 0x00000000, 0x3E800000, 0x40B00000, 0x00000000,
    0x40880000, 0x40A80000, 0x00000000, 0x40C00000, 0x40A80000, 0x00000000,
    0x3FC00000, 0x40980000, 0x00000000, 0x40400000, 0x40900000, 0x00000000,
    0xBF800000, 0x40880000, 0x00000000, 0xC0700000, 0x40700000, 0x00000000,
    0x40980000, 0x40700000, 0x00000000, 0xC0100000, 0x40600000, 0x00000000,
    0x40000000, 0x40500000, 0x00000000, 0xC0900000, 0x40300000, 0x00000000,
    0x40700000, 0x40300000, 0x00000000, 0x40C80000, 0x40300000, 0x00000000,
    0xBFA00000, 0x40200000, 0x00000000, 0x3F800000, 0x40200000, 0x00000000,
    0xBF000000, 0x41A00000, 0x00000000, 0xBFC00000, 0x419C0000, 0x00000000,
    0xC0200000, 0x41980000, 0x00000000, 0xC0500000, 0x41940000, 0x00000000,
    0xC0800000, 0x41900000, 0x00000000, 0xC0980000, 0x418A0000, 0x00000000,
    0xC0A80000, 0x41800000, 0x00000000, 0xC0A80000, 0x416C0000, 0x00000000,
    0xC0A00000, 0x41580000, 0x00000000, 0xC0900000, 0x41480000, 0x00000000,
    0xC0700000, 0x413C0000, 0x00000000, 0xC0400000, 0x41340000, 0x00000000,
    0xC0000000, 0x412C0000, 0x00000000, 0xBF800000, 0x41280000, 0x00000000,
    0x00000000, 0x41240000, 0x00000000, 0x3F800000, 0x41200000, 0x00000000,
    0x40000000, 0x411C0000, 0x00000000, 0x40300000, 0x41140000, 0x00000000,
    0x40600000, 0x410C0000, 0x00000000, 0x40800000, 0x40F80000, 0x00000000,
    0x40800000, 0x40D80000, 0x00000000, 0x40600000, 0x40C00000, 0x00000000,
    0x40300000, 0x40B00000, 0x00000000, 0x3FE00000, 0x40A00000, 0x00000000,
    0x3F400000, 0x40980000, 0x00000000, 0x3F400000, 0x40800000, 0x00000000,
    0x3F400000, 0x40500000, 0x00000000, 0x3F400000, 0x40200000, 0x00000000,
    0x3F400000, 0x3FE00000, 0x00000000, 0x3F400000, 0x3F800000, 0x00000000,
    0xBFE00000, 0x419E0000, 0x00000000, 0xC0900000, 0x41980000, 0x00000000,
    0x3F400000, 0x41980000, 0x00000000, 0xC0900000, 0x418E0000, 0x00000000,
    0xBFE00000, 0x418E0000, 0x00000000, 0x3F400000, 0x41860000, 0x00000000,
    0xC0900000, 0x41840000, 0x00000000, 0xBFE00000, 0x41780000, 0x00000000,
    0xC0900000, 0x41700000, 0x00000000, 0x3F400000, 0x416C0000, 0x00000000,
    0xBFE00000, 0x415C0000, 0x00000000, 0x3F400000, 0x414C0000, 0x00000000,
    0x40600000, 0x41400000, 0x00000000, 0xBFE00000, 0x413C0000, 0x00000000,
    0x3F400000, 0x41280000, 0x00000000, 0x40600000, 0x41240000, 0x00000000,
    0xBFE00000, 0x41140000, 0x00000000, 0x3F400000, 0x41040000, 0x00000000,
    0x40600000, 0x41000000, 0x00000000, 0x40B80000, 0x40F00000, 0x00000000,
    0x3F400000, 0x40C80000, 0x00000000, 0x40B80000, 0x40C80000, 0x00000000,
    0x40600000, 0x40C00000, 0x00000000, 0x40B80000, 0x40A00000, 0x00000000,
    0x3F400000, 0x40900000, 0x00000000, 0x40600000, 0x40880000, 0x00000000,
    0x40B80000, 0x40500000, 0x00000000, 0x3F400000, 0x40300000, 0x00000000,
    0x40600000, 0x3FC00000, 0x00000000, 0x3F400000, 0x3F800000, 0x00000000
};

Vec * D_53934[] = {
    (Vec *)((char *)D_5052C + 0x32A0),
    (Vec *)((char *)D_5052C + 0x438),
    (Vec *)((char *)D_5052C + 0x5A0),
    (Vec *)((char *)D_5052C + 0x708),
    (Vec *)((char *)D_5052C + 0x870),
    (Vec *)((char *)D_5052C + 0x9D8),
    (Vec *)((char *)D_5052C + 0xB40),
    (Vec *)((char *)D_5052C + 0xCA8),
    (Vec *)((char *)D_5052C + 0xE10),
    (Vec *)((char *)D_5052C + 0xF78),
    (Vec *)((char *)D_5052C + 0x10E0),
    (Vec *)((char *)D_5052C + 0x1248),
    (Vec *)((char *)D_5052C + 0x13B0),
    (Vec *)((char *)D_5052C + 0x1518),
    (Vec *)((char *)D_5052C + 0x1680),
    (Vec *)((char *)D_5052C + 0x17E8),
    (Vec *)((char *)D_5052C + 0x1950),
    (Vec *)((char *)D_5052C + 0x1AB8),
    (Vec *)((char *)D_5052C + 0x1C20),
    (Vec *)((char *)D_5052C + 0x1D88),
    (Vec *)((char *)D_5052C + 0x1EF0),
    (Vec *)((char *)D_5052C + 0x2058),
    (Vec *)((char *)D_5052C + 0x21C0),
    (Vec *)((char *)D_5052C + 0x2328),
    (Vec *)((char *)D_5052C + 0x2490),
    (Vec *)((char *)D_5052C + 0x25F8),
    (Vec *)((char *)D_5052C + 0x2760),
    (Vec *)((char *)D_5052C + 0x28C8),
    (Vec *)((char *)D_5052C + 0x2A30),
    (Vec *)((char *)D_5052C + 0x2B98),
    (Vec *)((char *)D_5052C + 0x2D00),
    (Vec *)((char *)D_5052C),
    (Vec *)((char *)D_5052C + 0x168),
    (Vec *)((char *)D_5052C + 0x2D0),
    (Vec *)((char *)D_5052C + 0x2E68),
    (Vec *)((char *)D_5052C + 0x2FD0),
    (Vec *)((char *)D_5052C + 0x3138)
};

float D_539C8 = 0.0500000007f;

float D_539CC = -20.0f;

s32 D_539D0 = 30;

s32 D_539D4 = 15;

s32 D_539D8 = 10000;

s32 D_539DC = 100;

s32 D_539E0 = 100;

s32 D_539E4 = 10000;

s32 D_539E8 = 100;

Vec D_539EC[] = {
    {1.20000005f, 1.20000005f, 0.0f},
    {-1.20000005f, -1.20000005f, 0.0f},
    {1.20000005f, -1.20000005f, 0.0f},
    {-1.20000005f, 1.20000005f, 0.0f},
    {1.20000005f, 0.0f, 0.0f},
    {-1.20000005f, 0.0f, 0.0f}
};

char D_53A34[0x7E0] =
    "\202\311\202\361\202\260\202\361\000\000\000\000""Human\000\000\000"
    "\203{\203m\203{\000\000""Bonobo\000\000""\203S\203\212\203\211\000\000""Gorilla\000"
    "\203`\203\223\203p\203\223\203W\201[\000\000\000\000""Chimpanzee\000\000"
    "\203I\203\211\203\223\203E\201[\203^\203\223\000\000""Orangutan\000\000\000"
    "\203V\203\203\201[\203}\203\223\203M\203{\203\223\000\000\000\000""Siamang Gibbon\000\000"
    "\203\217\203E\203\217\203E\203e\203i\203K\203U\203\213\000\000""Moloch Gibbon\000\000\000"
    "\203{\203E\203V\203e\203i\203K\203U\203\213\000\000\000\000""Capped Gibbon\000\000\000"
    "\203V\203\215\203e\203e\203i\203K\203U\203\213\000\000\000\000""White-Handed Gibbon\000"
    "\203Q\203\211\203_\203q\203q\000\000""Gelada Baboon\000\000\000"
    "\203}\203\223\203h\203\212\203\213\000\000""Mandrill\000\000\000\000"
    "\203}\203\223\203g\203q\203q\000\000""Hamadryas Baboon\000\000\000\000"
    "\203L\203C\203\215\203q\203q\000\000""Yellow Baboon\000\000\000"
    "\203u\203\211\203b\203N\203G\201[\203v\000\000""Black Ape\000\000\000"
    "\203j\203z\203\223\203U\203\213\000\000""Japanese Macaque\000\000\000\000"
    "\203J\203j\203N\203C\203U\203\213\000\000\000\000""Long-Tailed Macaque\000"
    "\203T\203o\203\223\203i\203\202\203\223\203L\201[\000\000\000\000""Savanna Monkey\000\000"
    "\203u\203\211\203b\203U\203O\203G\203m\203\223\000\000\000\000""de Brazza's Monkey\000\000"
    "\203V\203V\203I\203U\203\213\000\000""Lion-Tailed Macaque\000"
    "\203u\203^\203I\203U\203\213\000\000""Pig-Tailed Macaque\000\000"
    "\203x\203j\203K\203I\203U\203\213\000\000\000\000""Stump-Tailed Macaque\000\000\000\000"
    "\203{\203\223\203l\203b\203g\203U\203\213\000\000""Bonnet Monkey\000\000\000"
    "\203^\203\211\203|\203A\203\223\000\000""Talapoin Monkey\000"
    "\203z\203z\203W\203\215\203}\203\223\203K\203x\203C\000\000"
    "Grey-Cheeked Mangabey\000\000\000""\203p\203^\203X\203U\203\213\000\000"
    "Patas Monkey\000\000\000\000"
    "\203V\203\215\203G\203\212\203}\203\223\203K\203x\203C\000\000"
    "White-Collared Mangabey\000""\203e\203\223\203O\203U\203\213\000\000"
    "Proboscis Monkey\000\000\000\000""\203L\203\223\203V\203R\203E\000\000"
    "Golden Snub-Nosed Monkey\000\000\000\000"
    "\203V\203\213\203o\201[\203\213\203g\203\223\000\000""Silvered Leaf-Monkey\000\000\000\000"
    "\203h\203D\203N\203\202\203\223\203L\201[\000\000""Douc Langur\000"
    "\203n\203k\203}\203\223\203\211\203\223\203O\201[\203\213\000\000""Hanuman Langur\000\000"
    "\203_\203X\203L\201[\203\213\203g\203\223\000\000""Dusky Langur\000\000\000\000"
    "\203A\203r\203V\203j\203A\203R\203\215\203u\203X\000\000"
    "Abyssinian Black and White Colobus\000\000""\203t\203T\203I\203}\203L\203U\203\213\000\000"
    "Brown Tufted Capuchin\000\000\000""\203\212\203X\203U\203\213\000\000\000\000"
    "Squirrel Monkey\000""\203_\203X\203L\201[\203e\203B\203e\203B\000\000\000\000"
    "Titi Monkey\000""\203\210\203U\203\213\000\000""Douroucouli\000"
    "\203u\203\223\203{\203\213\203g\203E\201[\203\212\201[\203\202\203\223\203L\201[\000\000"
    "Humboldt's Wooly Monkey\000""\203N\203\202\203U\203\213\000\000\000\000"
    "Spider Monkey\000\000\000""\203A\203J\203z\203G\203U\203\213\000\000\000\000"
    "Red Howler Monkey\000\000\000""\203N\203\215\203z\203G\203U\203\213\000\000\000\000"
    "Black Howler Monkey\000""\203V\203\215\203K\203I\203T\203L\000\000\000\000"
    "Mat-Faced Saki\000\000""\203n\203Q\203E\203A\203J\203\212\000\000\000\000"
    "Bare-Faced Uakari\000\000\000"
    "\203Q\203\213\203f\203B\203\202\203\223\203L\201[\000\000\000\000"
    "Goeldi's Marmoset\000\000\000"
    "\203\211\203C\203I\203\223\203^\203}\203\212\203\223\000\000\000\000"
    "Golden Lion Tamarin\000"
    "\203G\203\223\202\330\203\211\201[\203^\203}\203\212\203\223\000\000""Emperor Tamarin\000"
    "\203\217\203^\203{\203E\203V\203p\203\223\203V\203F\000\000""Cotton-Top Tamarin\000\000"
    "\203s\203O\203~\201[\203}\201[\203\202\203Z\203b\203g\000\000\000\000"
    "Pigmy Marmoset\000\000""\203j\203V\203\201\203K\203l\203U\203\213\000\000"
    "Western Tarsier\000""\203A\203C\203A\203C\000\000\000\000""Aye-Aye\000"
    "\203I\203I\203K\203\211\203S\000\000""Thick-Tailed Bush Baby\000\000"
    "\203u\203b\203V\203\205\203x\203C\203r\201[\000\000\000\000""Bush Baby\000\000\000"
    "\203|\203b\203g\201[\000\000\000\000""Potto\000\000\000"
    "\203X\203\215\201[\203\215\203\212\203X\000\000\000\000""Slow Loris\000\000"
    "\203X\203\214\203\223\203_\201[\203\215\203\212\203X\000\000\000\000"
    "Slender Loris\000\000\000""\203x\203\215\201[\203V\203t\203@\203J\000\000"
    "Verreaux's Sifaka\000\000\000""\203A\203o\203q\000\000""Avahi\000\000\000"
    "\203C\203\223\203h\203\212\000\000\000\000""Indris\000\000"
    "\203G\203\212\203}\203L\203L\203c\203l\203U\203\213\000\000""Ruffed Lemur\000\000\000\000"
    "\203\217\203I\203L\203c\203l\203U\203\213\000\000""Ring-tailed Lemur\000\000\000"
    "\203n\203C\203C\203\215\203l\203Y\203~\203L\203c\203l\203U\203\213\000\000\000\000"
    "Grey Mouse Lemur\000\000\000\000";

struct RatingName D_54214[] = {
    {10000, (char *)D_53A34, (char *)D_53A34 + 0xC},
    {1460, (char *)D_53A34 + 0x14, (char *)D_53A34 + 0x1C},
    {1430, (char *)D_53A34 + 0x24, (char *)D_53A34 + 0x2C},
    {1410, (char *)D_53A34 + 0x34, (char *)D_53A34 + 0x44},
    {1380, (char *)D_53A34 + 0x50, (char *)D_53A34 + 0x60},
    {1350, (char *)D_53A34 + 0x6C, (char *)D_53A34 + 0x80},
    {1320, (char *)D_53A34 + 0x90, (char *)D_53A34 + 0xA4},
    {1290, (char *)D_53A34 + 0xB4, (char *)D_53A34 + 0xC8},
    {1260, (char *)D_53A34 + 0xD8, (char *)D_53A34 + 0xEC},
    {1230, (char *)D_53A34 + 0x100, (char *)D_53A34 + 0x10C},
    {1200, (char *)D_53A34 + 0x11C, (char *)D_53A34 + 0x128},
    {1170, (char *)D_53A34 + 0x134, (char *)D_53A34 + 0x140},
    {1140, (char *)D_53A34 + 0x154, (char *)D_53A34 + 0x160},
    {1110, (char *)D_53A34 + 0x170, (char *)D_53A34 + 0x180},
    {1080, (char *)D_53A34 + 0x18C, (char *)D_53A34 + 0x198},
    {1050, (char *)D_53A34 + 0x1AC, (char *)D_53A34 + 0x1BC},
    {1020, (char *)D_53A34 + 0x1D0, (char *)D_53A34 + 0x1E4},
    {990, (char *)D_53A34 + 0x1F4, (char *)D_53A34 + 0x208},
    {960, (char *)D_53A34 + 0x21C, (char *)D_53A34 + 0x228},
    {930, (char *)D_53A34 + 0x23C, (char *)D_53A34 + 0x248},
    {900, (char *)D_53A34 + 0x25C, (char *)D_53A34 + 0x26C},
    {870, (char *)D_53A34 + 0x284, (char *)D_53A34 + 0x294},
    {840, (char *)D_53A34 + 0x2A4, (char *)D_53A34 + 0x2B0},
    {810, (char *)D_53A34 + 0x2C0, (char *)D_53A34 + 0x2D4},
    {780, (char *)D_53A34 + 0x2EC, (char *)D_53A34 + 0x2F8},
    {750, (char *)D_53A34 + 0x308, (char *)D_53A34 + 0x31C},
    {720, (char *)D_53A34 + 0x334, (char *)D_53A34 + 0x340},
    {690, (char *)D_53A34 + 0x354, (char *)D_53A34 + 0x360},
    {660, (char *)D_53A34 + 0x37C, (char *)D_53A34 + 0x38C},
    {630, (char *)D_53A34 + 0x3A4, (char *)D_53A34 + 0x3B4},
    {600, (char *)D_53A34 + 0x3C0, (char *)D_53A34 + 0x3D4},
    {570, (char *)D_53A34 + 0x3E4, (char *)D_53A34 + 0x3F4},
    {540, (char *)D_53A34 + 0x404, (char *)D_53A34 + 0x418},
    {510, (char *)D_53A34 + 0x43C, (char *)D_53A34 + 0x44C},
    {480, (char *)D_53A34 + 0x464, (char *)D_53A34 + 0x470},
    {450, (char *)D_53A34 + 0x480, (char *)D_53A34 + 0x494},
    {420, (char *)D_53A34 + 0x4A0, (char *)D_53A34 + 0x4A8},
    {390, (char *)D_53A34 + 0x4B4, (char *)D_53A34 + 0x4D0},
    {360, (char *)D_53A34 + 0x4E8, (char *)D_53A34 + 0x4F4},
    {330, (char *)D_53A34 + 0x504, (char *)D_53A34 + 0x514},
    {300, (char *)D_53A34 + 0x528, (char *)D_53A34 + 0x538},
    {270, (char *)D_53A34 + 0x54C, (char *)D_53A34 + 0x55C},
    {240, (char *)D_53A34 + 0x56C, (char *)D_53A34 + 0x57C},
    {210, (char *)D_53A34 + 0x590, (char *)D_53A34 + 0x5A4},
    {180, (char *)D_53A34 + 0x5B8, (char *)D_53A34 + 0x5CC},
    {150, (char *)D_53A34 + 0x5E0, (char *)D_53A34 + 0x5F4},
    {120, (char *)D_53A34 + 0x604, (char *)D_53A34 + 0x618},
    {90, (char *)D_53A34 + 0x62C, (char *)D_53A34 + 0x644},
    {60, (char *)D_53A34 + 0x654, (char *)D_53A34 + 0x664},
    {30, (char *)D_53A34 + 0x674, (char *)D_53A34 + 0x680},
    {0, (char *)D_53A34 + 0x688, (char *)D_53A34 + 0x694},
    {-50, (char *)D_53A34 + 0x6AC, (char *)D_53A34 + 0x6C0},
    {-100, (char *)D_53A34 + 0x6CC, (char *)D_53A34 + 0x6D8},
    {-150, (char *)D_53A34 + 0x6E0, (char *)D_53A34 + 0x6F0},
    {-200, (char *)D_53A34 + 0x6FC, (char *)D_53A34 + 0x710},
    {-250, (char *)D_53A34 + 0x720, (char *)D_53A34 + 0x730},
    {-300, (char *)D_53A34 + 0x744, (char *)D_53A34 + 0x74C},
    {-350, (char *)D_53A34 + 0x754, (char *)D_53A34 + 0x760},
    {-400, (char *)D_53A34 + 0x768, (char *)D_53A34 + 0x77C},
    {-450, (char *)D_53A34 + 0x78C, (char *)D_53A34 + 0x79C},
    {-9999, (char *)D_53A34 + 0x7B0, (char *)D_53A34 + 0x7CC}
};

char D_544F0[0xEA8] =
    "human\000\000\000""Zwergschimpanse\000""Gorille\000""Gorila\000\000""Schimpanse\000\000"
    "Chimpanz\351\000\000\000""Chimpanc\351\000\000\000""Scimpanz\350\000\000\000"
    "Orang-Utan\000\000""Orang-outan\000""Orangut\341n\000\000\000""Siamang\000"
    "Gib\363n Siamang\000\000\000""Gibbone Siamang\000""Silbergibbon\000\000\000\000"
    "Gibbon cendr\351\000\000\000""Gib\363n Moloch\000\000\000\000""Gibbone Moloch\000\000"
    "Kappengibbon\000\000\000\000""Gibbon \340 bonnet\000""Gib\363n Copete\000\000\000\000"
    "Gibbone Incappucciato\000\000\000""Wei\337handgibbon\000\000"
    "Gibbon \340 mains blanches\000""Gib\363n Mano-blanca\000\000\000"
    "Gibbone dalle Mani Bianche\000\000""Blutbrustpavian\000""G\351lada\000\000"
    "Babuino Gelada\000\000""Babbuino Gelada\000""Mandril\000""Mandrillo\000\000\000"
    "Mantelpavian\000\000\000\000""Hamadryas\000\000\000""Babuino Hamadryas\000\000\000"
    "Babbuino Hamadryas\000\000""Babbuino Giallo\000""Babuino Amarillo\000\000\000\000"
    "Babouin janue\000\000\000""Steppenpavian\000\000\000""Makak\000\000\000"
    "Macaque noir\000\000\000\000""Mono Negro\000\000""Scimmia Nera\000\000\000\000"
    "Japanmakak\000\000""Macaque japonais\000\000\000\000""Macaco Japon\351s\000\000"
    "Macaco Giapponese\000\000\000""Langschwanzmakak\000\000\000\000""Macaque crabier\000"
    "Macaco Cola-larga\000\000\000""Macaco a Coda Lunga\000""Gr\374ne Meerkatze\000"
    "Grivet\000\000""Mono Sabana\000""Scimmia della Savana\000\000\000\000""Brazzameerkatze\000"
    "Cercopith\350que de Brazza\000""Mono de Brazza\000\000"
    "Scimmia di de Brazza\000\000\000\000""Bartaffe\000\000\000\000"
    "Macaque \340 queue de lion\000""Macaco Cola-l\351on\000\000\000\000"
    "Macaco Coda di Leone\000\000\000\000""Schweinsaffe\000\000\000\000"
    "Macaque \340 queue de cochon\000\000\000""Macaco Cola-cerdo\000\000\000"
    "Macaco Coda di Maiale\000\000\000""Tibetmakak\000\000""Macaque \340 queue courte\000\000"
    "Macaco Cola-mu\361\363n\000\000\000""Macaco Coda Mozza\000\000\000"
    "Indischer Hutaffe\000\000\000""Macaque Bonnet chinois\000\000""Mono Bonete\000"
    "Scimmia col Berretto\000\000\000\000""Zwergmeerkatze\000\000""Talapoin\000\000\000\000"
    "Mono Talapoin\000\000\000""Scimmia Talapoina\000\000\000""Mantelmangabe\000\000\000"
    "Mangabey \340 joues grises\000""Mangabey carrillo-gris\000\000"
    "Mangabey dalle Gote Rosse\000\000\000""Husarenaffe\000""Patas\000\000\000"
    "Mono Patas\000\000""Sciammia Patas\000\000""Halsbandmangabe\000"
    "Mangabey \340 collier blanc\000\000\000\000""Mangabey cuello-blanco\000\000"
    "Mangabey dal Collare Bianco\000""Nasenaffe\000\000\000""Nasique\000"
    "Mono Proboscis\000\000""Scimmia Proboscide\000\000""Goldstumpfnase\000\000"
    "Rhinopith\350que dor\351\000\000""Mono Dorado Morro-chato\000"
    "Scimmia d'Oro dal Naso all'Ins\371\000""Haubenlangur\000\000\000\000"
    "Semnopith\350que \340 coiffe\000\000""Mono Hoja Plateada\000\000"
    "Scimmia Foglie d'Argento\000\000\000\000""Kleideraffe\000""Douc\000\000\000\000"
    "Langur Douc\000""Entello Douc\000\000\000\000""Hanuman-Langur\000\000""Entelle\000"
    "Langur Hanuman\000\000""Entello Hanuman\000""Brillenlangur\000\000\000"
    "Semnopith\350que obscur\000\000\000\000""Langur Oscuro\000\000\000""Entello Langur\000\000"
    "Ostafrikanischer Guereza\000\000\000\000""Colobe noir et blanc\000\000\000\000"
    "Abssynian Negro y Colobus Blanco\000\000\000\000""Colobus Abissino Bianco e Nero\000\000"
    "Gehaupter Kapuziner\000""Capucin\000""Capuchino Copete Marr\363n\000"
    "Cappuccino dal Ciuffo Marrone\000\000\000""Totenkopfaffe\000\000\000"
    "Singe \351cureuil\000\000""Mono Ardilla\000\000\000\000""Scimmia Scoiattolo\000\000"
    "Springaffe\000\000""Callic\350be\000\000\000""Tit\355\000\000\000\000"
    "Scimmia Titi\000\000\000\000""Nachtaffe\000\000\000""Humboldts Wollaffe\000\000"
    "Lagothriche de Humboldt\000""Mono Lanudo de Humboldt\000"
    "Scimmia Lanosa di Humboldt\000\000""Klammeraffe\000""Singe araign\351e\000\000"
    "Mono Ara\361a\000\000""Scimmia Ragno\000\000\000""Roter Br\374llaffe\000"
    "Singe hurleur roux\000\000""Mono Aullador Rojo\000\000""Scimmia Rossa Ululante\000\000"
    "Schwarzer Br\374llaffe\000""Singe hurleur noir\000\000""Mono Aullador Negro\000"
    "Scimmia Nera Ululante\000\000\000""Blasskopfsaki\000\000\000""Saki\000\000\000\000"
    "Saki con gre\361as\000""Saki dalla Fassia Variopinta\000\000\000\000"
    "Schwarzkopfuakari\000\000\000""Ouakari\000""Uakari Lampi\361as\000"
    "Uakari dalla Faccia Scoperta\000\000\000\000""Goeldis Wei\337b\374schelaffe\000"
    "Ouistiti de Goeldi\000\000""Tit\355 Goeldi\000""Marmoset di Goeldi\000\000"
    "L\366wenaffe\000\000\000""Tamarin-lion dor\351\000\000\000"
    "Tit\355 Le\363n Dorado\000\000\000\000""Tamarin Leone d'Oro\000"
    "Kaiserschnurrbarttamarin\000\000\000\000""Tamarin empereur\000\000\000\000"
    "T\355ti Emperador\000\000""Tamarin Imperatore\000\000""Lisztaffe\000\000\000"
    "Pinch\351\000\000""Tit\355 Copete Algod\363n\000"
    "Tamarin dalla Punta di Cotone\000\000\000""Zwergwei\337b\374schelaffe\000\000\000\000"
    "Ouistiti nain\000\000\000""T\355ti Pigmeo\000""Callitricide Pigmea\000"
    "Sundakoboldmaki\000""Tarsier\000""Tarsier Occidental\000\000""Tarsio del West\000"
    "Fingertier\000\000""Riesengalago\000\000\000\000""Galago \340 queue \351paisse\000\000"
    "Lem\372rido Cola Gruesa\000\000\000\000""Galagone dalla Coda Grossa\000\000"
    "Galago\000\000""Lem\372rido\000\000\000\000""Galagone\000\000\000\000"
    "Plumplori\000\000\000""Nyctic\350be coucang\000\000\000""Loris perezoso\000\000"
    "Lori Lento\000\000""Schlanklori\000""Nyctic\350be nain\000\000""Loris delgado\000\000\000"
    "Lori Gracile\000\000\000\000""Larvensifaka\000\000\000\000""Propith\350que de Verreaux\000"
    "Sifaka Verreaux\000""Sifaka di Verreaux\000\000""Wollmaki\000\000\000\000"
    "Indri\000\000\000""Vari\000\000\000\000""Lemur vari\000\000"
    "L\351mur desgre\361ado\000\000\000\000""Lemure Arruffato\000\000\000\000"
    "Katta\000\000\000""Maki Caffa\000\000""L\351mur Cola Anillo\000\000\000"
    "Lemure dalla Coda ad Anello\000""Grauer Mausmaki\000""Hapalemur\000\000\000"
    "L\351mur Rat\363n Gris\000\000\000\000""Lemure Topo Grigio\000\000";

struct RatingNameIntl D_55398[] = {
    {10000, (char *)D_53A34 + 0xC, (char *)D_544F0, (char *)D_544F0, (char *)D_544F0, (char *)D_544F0},
    {1460, (char *)D_53A34 + 0x1C, (char *)D_544F0 + 0x8, (char *)D_53A34 + 0x1C, (char *)D_53A34 + 0x1C, (char *)D_53A34 + 0x1C},
    {1430, (char *)D_53A34 + 0x2C, (char *)D_53A34 + 0x2C, (char *)D_544F0 + 0x18, (char *)D_544F0 + 0x20, (char *)D_53A34 + 0x2C},
    {1410, (char *)D_53A34 + 0x44, (char *)D_544F0 + 0x28, (char *)D_544F0 + 0x34, (char *)D_544F0 + 0x40, (char *)D_544F0 + 0x4C},
    {1380, (char *)D_53A34 + 0x60, (char *)D_544F0 + 0x58, (char *)D_544F0 + 0x64, (char *)D_544F0 + 0x70, (char *)D_53A34 + 0x60},
    {1350, (char *)D_53A34 + 0x80, (char *)D_544F0 + 0x7C, (char *)D_544F0 + 0x7C, (char *)D_544F0 + 0x84, (char *)D_544F0 + 0x94},
    {1320, (char *)D_53A34 + 0xA4, (char *)D_544F0 + 0xA4, (char *)D_544F0 + 0xB4, (char *)D_544F0 + 0xC4, (char *)D_544F0 + 0xD4},
    {1290, (char *)D_53A34 + 0xC8, (char *)D_544F0 + 0xE4, (char *)D_544F0 + 0xF4, (char *)D_544F0 + 0x104, (char *)D_544F0 + 0x114},
    {1260, (char *)D_53A34 + 0xEC, (char *)D_544F0 + 0x12C, (char *)D_544F0 + 0x13C, (char *)D_544F0 + 0x154, (char *)D_544F0 + 0x168},
    {1230, (char *)D_53A34 + 0x10C, (char *)D_544F0 + 0x184, (char *)D_544F0 + 0x194, (char *)D_544F0 + 0x19C, (char *)D_544F0 + 0x1AC},
    {1200, (char *)D_53A34 + 0x128, (char *)D_53A34 + 0x128, (char *)D_53A34 + 0x128, (char *)D_544F0 + 0x1BC, (char *)D_544F0 + 0x1C4},
    {1170, (char *)D_53A34 + 0x140, (char *)D_544F0 + 0x1D0, (char *)D_544F0 + 0x1E0, (char *)D_544F0 + 0x1EC, (char *)D_544F0 + 0x200},
    {1140, (char *)D_53A34 + 0x160, (char *)D_544F0 + 0x214, (char *)D_544F0 + 0x224, (char *)D_544F0 + 0x238, (char *)D_544F0 + 0x248},
    {1110, (char *)D_53A34 + 0x180, (char *)D_544F0 + 0x258, (char *)D_544F0 + 0x260, (char *)D_544F0 + 0x270, (char *)D_544F0 + 0x27C},
    {1080, (char *)D_53A34 + 0x198, (char *)D_544F0 + 0x28C, (char *)D_544F0 + 0x298, (char *)D_544F0 + 0x2AC, (char *)D_544F0 + 0x2BC},
    {1050, (char *)D_53A34 + 0x1BC, (char *)D_544F0 + 0x2D0, (char *)D_544F0 + 0x2E4, (char *)D_544F0 + 0x2F4, (char *)D_544F0 + 0x308},
    {1020, (char *)D_53A34 + 0x1E4, (char *)D_544F0 + 0x31C, (char *)D_544F0 + 0x32C, (char *)D_544F0 + 0x334, (char *)D_544F0 + 0x340},
    {990, (char *)D_53A34 + 0x208, (char *)D_544F0 + 0x358, (char *)D_544F0 + 0x368, (char *)D_544F0 + 0x380, (char *)D_544F0 + 0x390},
    {960, (char *)D_53A34 + 0x228, (char *)D_544F0 + 0x3A8, (char *)D_544F0 + 0x3B4, (char *)D_544F0 + 0x3CC, (char *)D_544F0 + 0x3E0},
    {930, (char *)D_53A34 + 0x248, (char *)D_544F0 + 0x3F8, (char *)D_544F0 + 0x408, (char *)D_544F0 + 0x424, (char *)D_544F0 + 0x438},
    {900, (char *)D_53A34 + 0x26C, (char *)D_544F0 + 0x450, (char *)D_544F0 + 0x45C, (char *)D_544F0 + 0x474, (char *)D_544F0 + 0x488},
    {870, (char *)D_53A34 + 0x294, (char *)D_544F0 + 0x49C, (char *)D_544F0 + 0x4B0, (char *)D_544F0 + 0x4C8, (char *)D_544F0 + 0x4D4},
    {840, (char *)D_53A34 + 0x2B0, (char *)D_544F0 + 0x4EC, (char *)D_544F0 + 0x4FC, (char *)D_544F0 + 0x508, (char *)D_544F0 + 0x518},
    {810, (char *)D_53A34 + 0x2D4, (char *)D_544F0 + 0x52C, (char *)D_544F0 + 0x53C, (char *)D_544F0 + 0x554, (char *)D_544F0 + 0x56C},
    {780, (char *)D_53A34 + 0x2F8, (char *)D_544F0 + 0x588, (char *)D_544F0 + 0x594, (char *)D_544F0 + 0x59C, (char *)D_544F0 + 0x5A8},
    {750, (char *)D_53A34 + 0x31C, (char *)D_544F0 + 0x5B8, (char *)D_544F0 + 0x5C8, (char *)D_544F0 + 0x5E4, (char *)D_544F0 + 0x5FC},
    {720, (char *)D_53A34 + 0x340, (char *)D_544F0 + 0x618, (char *)D_544F0 + 0x624, (char *)D_544F0 + 0x62C, (char *)D_544F0 + 0x63C},
    {690, (char *)D_53A34 + 0x360, (char *)D_544F0 + 0x650, (char *)D_544F0 + 0x660, (char *)D_544F0 + 0x674, (char *)D_544F0 + 0x68C},
    {660, (char *)D_53A34 + 0x38C, (char *)D_544F0 + 0x6AC, (char *)D_544F0 + 0x6BC, (char *)D_544F0 + 0x6D4, (char *)D_544F0 + 0x6E8},
    {630, (char *)D_53A34 + 0x3B4, (char *)D_544F0 + 0x704, (char *)D_544F0 + 0x710, (char *)D_544F0 + 0x718, (char *)D_544F0 + 0x724},
    {600, (char *)D_53A34 + 0x3D4, (char *)D_544F0 + 0x734, (char *)D_544F0 + 0x744, (char *)D_544F0 + 0x74C, (char *)D_544F0 + 0x75C},
    {570, (char *)D_53A34 + 0x3F4, (char *)D_544F0 + 0x76C, (char *)D_544F0 + 0x77C, (char *)D_544F0 + 0x794, (char *)D_544F0 + 0x7A4},
    {540, (char *)D_53A34 + 0x418, (char *)D_544F0 + 0x7B4, (char *)D_544F0 + 0x7D0, (char *)D_544F0 + 0x7E8, (char *)D_544F0 + 0x80C},
    {510, (char *)D_53A34 + 0x44C, (char *)D_544F0 + 0x82C, (char *)D_544F0 + 0x840, (char *)D_544F0 + 0x848, (char *)D_544F0 + 0x860},
    {480, (char *)D_53A34 + 0x470, (char *)D_544F0 + 0x880, (char *)D_544F0 + 0x890, (char *)D_544F0 + 0x8A0, (char *)D_544F0 + 0x8B0},
    {450, (char *)D_53A34 + 0x494, (char *)D_544F0 + 0x8C4, (char *)D_544F0 + 0x8D0, (char *)D_544F0 + 0x8DC, (char *)D_544F0 + 0x8E4},
    {420, (char *)D_53A34 + 0x4A8, (char *)D_544F0 + 0x8F4, (char *)D_53A34 + 0x4A8, (char *)D_53A34 + 0x4A8, (char *)D_53A34 + 0x4A8},
    {390, (char *)D_53A34 + 0x4D0, (char *)D_544F0 + 0x900, (char *)D_544F0 + 0x914, (char *)D_544F0 + 0x92C, (char *)D_544F0 + 0x944},
    {360, (char *)D_53A34 + 0x4F4, (char *)D_544F0 + 0x960, (char *)D_544F0 + 0x96C, (char *)D_544F0 + 0x97C, (char *)D_544F0 + 0x988},
    {330, (char *)D_53A34 + 0x514, (char *)D_544F0 + 0x998, (char *)D_544F0 + 0x9A8, (char *)D_544F0 + 0x9BC, (char *)D_544F0 + 0x9D0},
    {300, (char *)D_53A34 + 0x538, (char *)D_544F0 + 0x9E8, (char *)D_544F0 + 0x9FC, (char *)D_544F0 + 0xA10, (char *)D_544F0 + 0xA24},
    {270, (char *)D_53A34 + 0x55C, (char *)D_544F0 + 0xA3C, (char *)D_544F0 + 0xA4C, (char *)D_544F0 + 0xA54, (char *)D_544F0 + 0xA64},
    {240, (char *)D_53A34 + 0x57C, (char *)D_544F0 + 0xA84, (char *)D_544F0 + 0xA98, (char *)D_544F0 + 0xAA0, (char *)D_544F0 + 0xAB0},
    {210, (char *)D_53A34 + 0x5A4, (char *)D_544F0 + 0xAD0, (char *)D_544F0 + 0xAE8, (char *)D_544F0 + 0xAFC, (char *)D_544F0 + 0xB08},
    {180, (char *)D_53A34 + 0x5CC, (char *)D_544F0 + 0xB1C, (char *)D_544F0 + 0xB28, (char *)D_544F0 + 0xB3C, (char *)D_544F0 + 0xB50},
    {150, (char *)D_53A34 + 0x5F4, (char *)D_544F0 + 0xB64, (char *)D_544F0 + 0xB80, (char *)D_544F0 + 0xB94, (char *)D_544F0 + 0xBA4},
    {120, (char *)D_53A34 + 0x618, (char *)D_544F0 + 0xBB8, (char *)D_544F0 + 0xBC4, (char *)D_544F0 + 0xBCC, (char *)D_544F0 + 0xBE0},
    {90, (char *)D_53A34 + 0x644, (char *)D_544F0 + 0xC00, (char *)D_544F0 + 0xC18, (char *)D_544F0 + 0xC28, (char *)D_544F0 + 0xC34},
    {60, (char *)D_53A34 + 0x664, (char *)D_544F0 + 0xC48, (char *)D_544F0 + 0xC58, (char *)D_544F0 + 0xC60, (char *)D_544F0 + 0xC74},
    {30, (char *)D_53A34 + 0x680, (char *)D_544F0 + 0xC84, (char *)D_53A34 + 0x680, (char *)D_53A34 + 0x680, (char *)D_53A34 + 0x680},
    {0, (char *)D_53A34 + 0x694, (char *)D_544F0 + 0xC90, (char *)D_544F0 + 0xCA0, (char *)D_544F0 + 0xCB8, (char *)D_544F0 + 0xCD0},
    {-50, (char *)D_53A34 + 0x6C0, (char *)D_544F0 + 0xCEC, (char *)D_544F0 + 0xCEC, (char *)D_544F0 + 0xCF4, (char *)D_544F0 + 0xD00},
    {-100, (char *)D_53A34 + 0x6D8, (char *)D_53A34 + 0x6D8, (char *)D_53A34 + 0x6D8, (char *)D_53A34 + 0x6D8, (char *)D_53A34 + 0x6D8},
    {-150, (char *)D_53A34 + 0x6F0, (char *)D_544F0 + 0xD0C, (char *)D_544F0 + 0xD18, (char *)D_544F0 + 0xD2C, (char *)D_544F0 + 0xD3C},
    {-200, (char *)D_53A34 + 0x710, (char *)D_544F0 + 0xD48, (char *)D_544F0 + 0xD54, (char *)D_544F0 + 0xD64, (char *)D_544F0 + 0xD74},
    {-250, (char *)D_53A34 + 0x730, (char *)D_544F0 + 0xD84, (char *)D_544F0 + 0xD94, (char *)D_544F0 + 0xDAC, (char *)D_544F0 + 0xDBC},
    {-300, (char *)D_53A34 + 0x74C, (char *)D_544F0 + 0xDD0, (char *)D_53A34 + 0x74C, (char *)D_53A34 + 0x74C, (char *)D_53A34 + 0x74C},
    {-350, (char *)D_53A34 + 0x760, (char *)D_544F0 + 0xDDC, (char *)D_544F0 + 0xDDC, (char *)D_53A34 + 0x760, (char *)D_53A34 + 0x760},
    {-400, (char *)D_53A34 + 0x77C, (char *)D_544F0 + 0xDE4, (char *)D_544F0 + 0xDEC, (char *)D_544F0 + 0xDF8, (char *)D_544F0 + 0xE0C},
    {-450, (char *)D_53A34 + 0x79C, (char *)D_544F0 + 0xE20, (char *)D_544F0 + 0xE28, (char *)D_544F0 + 0xE34, (char *)D_544F0 + 0xE48},
    {-9999, (char *)D_53A34 + 0x7CC, (char *)D_544F0 + 0xE64, (char *)D_544F0 + 0xE74, (char *)D_544F0 + 0xE80, (char *)D_544F0 + 0xE94}
};

char D_55950[0x54] =
    "You got all the bananas. +%d\000\000\000\000""ger\000""fra\000""spa\000""ita\000"
    "\203o\203i\203i\202\360 \202\272\202\361\202\324 \202\306\202\350\202\334\202\265\202\275 : +%d\000\000\000\000";

char * D_559A4[] = {
    (char *)D_55950,
    (char *)D_55950 + 0x20,
    (char *)D_55950 + 0x24,
    (char *)D_55950 + 0x28,
    (char *)D_55950 + 0x2C,
    (char *)D_55950 + 0x30
};

char D_559BC[0x38] =
    "You got no bananas.\000"
    "\203o\203i\203i\202\360 \202\320\202\306\202\302\202\340 \202\306\202\352\202\334\202\271\202\361\202\305\202\265\202\275\000\000";

char * D_559F4[] = {
    (char *)D_559BC,
    (char *)D_55950 + 0x20,
    (char *)D_55950 + 0x24,
    (char *)D_55950 + 0x28,
    (char *)D_55950 + 0x2C,
    (char *)D_559BC + 0x14
};

char D_55A0C[0x30] =
    "You got %d bananas.\000"
    "\203o\203i\203i\202\360 %d\202\331\202\361 \202\306\202\350\202\334\202\265\202\275\000\000";

char * D_55A3C[] = {
    (char *)D_55A0C,
    (char *)D_55950 + 0x20,
    (char *)D_55950 + 0x24,
    (char *)D_55950 + 0x28,
    (char *)D_55950 + 0x2C,
    (char *)D_55A0C + 0x14
};

char D_55A54[0x1C] =
    "\203o\203i\203i\202\360 %d \202\331\202\361 \202\306\202\350\202\334\202\265\202\275\000";

char * D_55A70[] = {
    (char *)D_55A54
};

char D_55A74[0x1C] =
    "\203o\203i\203i\202\360 %d \202\333\202\361 \202\306\202\350\202\334\202\265\202\275\000";

char * D_55A90[] = {
    (char *)D_55A74
};

char D_55A94[0x1C] =
    "\203o\203i\203i\202\360 %d \202\332\202\361 \202\306\202\350\202\334\202\265\202\275\000";

char * D_55AB0[] = {
    (char *)D_55A94
};

char D_55AB4[0x48] =
    "You got all the big bananas.+%d\000"
    "\202\250\202\250\202\253\202\310\203o\203i\203i\202\360 \202\272\202\361\202\324 \202\306\202\350\202\334\202\265\202\275\201F+%d\000";

char * D_55AFC[] = {
    (char *)D_55AB4,
    (char *)D_55950 + 0x20,
    (char *)D_55950 + 0x24,
    (char *)D_55950 + 0x28,
    (char *)D_55950 + 0x2C,
    (char *)D_55AB4 + 0x20
};

char D_55B14[0x44] =
    "You got no big bananas.\000"
    "\202\250\202\250\202\253\202\310\203o\203i\203i\202\360 \202\320\202\306\202\302\202\340 \202\306\202\352\202\334\202\271\202\361\202\305\202\265\202\275\000\000";

char * D_55B58[] = {
    (char *)D_55B14,
    (char *)D_55950 + 0x20,
    (char *)D_55950 + 0x24,
    (char *)D_55950 + 0x28,
    (char *)D_55950 + 0x2C,
    (char *)D_55B14 + 0x18
};

char D_55B70[0x3C] =
    "You got %d big bananas.\000"
    "\202\250\202\250\202\253\202\310\203o\203i\203i\202\360 %d \202\331\202\361 \202\306\202\350\202\334\202\265\202\275\000";

char * D_55BAC[] = {
    (char *)D_55B70,
    (char *)D_55950 + 0x20,
    (char *)D_55950 + 0x24,
    (char *)D_55950 + 0x28,
    (char *)D_55950 + 0x2C,
    (char *)D_55B70 + 0x18
};

char * D_55BC4[] = {
    (char *)D_55B70 + 0x18
};

char D_55BC8[0x24] =
    "\202\250\202\250\202\253\202\310\203o\203i\203i\202\360 %d \202\333\202\361 \202\306\202\350\202\334\202\265\202\275\000";

char * D_55BEC[] = {
    (char *)D_55BC8
};

char D_55BF0[0x24] =
    "\202\250\202\250\202\253\202\310\203o\203i\203i\202\360 %d \202\332\202\361 \202\306\202\350\202\334\202\265\202\275\000";

char * D_55C14[] = {
    (char *)D_55BF0
};

char D_55C18[0x4C] =
    "You got all the immortal stars. +%d\000"
    "\202\336\202\304\202\253\203X\203^\201[\202\360 \202\272\202\361\202\324 \202\306\202\350\202\334\202\265\202\275\201F+%d\000\000\000";

char * D_55C64[] = {
    (char *)D_55C18,
    (char *)D_55950 + 0x20,
    (char *)D_55950 + 0x24,
    (char *)D_55950 + 0x28,
    (char *)D_55950 + 0x2C,
    (char *)D_55C18 + 0x24
};

char D_55C7C[0x48] =
    "You got no immortal stars.\000\000"
    "\202\336\202\304\202\253\203X\203^\201[\202\360 \202\320\202\306\202\302\202\340 \202\306\202\352\202\334\202\271\202\361\202\305\202\265\202\275\000\000\000\000";

char * D_55CC4[] = {
    (char *)D_55C7C,
    (char *)D_55950 + 0x20,
    (char *)D_55950 + 0x24,
    (char *)D_55950 + 0x28,
    (char *)D_55950 + 0x2C,
    (char *)D_55C7C + 0x1C
};

char D_55CDC[0x5C] =
    "You got %d immortal stars.\000\000""%d ger\000\000""%d fra\000\000""%d spa\000\000"
    "%d ita\000\000"
    "\202\336\202\304\202\253\203X\203^\201[\202\360 %d \203R \202\306\202\350\202\334\202\265\202\275\000";

char * D_55D38[] = {
    (char *)D_55CDC,
    (char *)D_55CDC + 0x1C,
    (char *)D_55CDC + 0x24,
    (char *)D_55CDC + 0x2C,
    (char *)D_55CDC + 0x34,
    (char *)D_55CDC + 0x3C
};

char D_55D50[0x48] =
    "You hit all the letters.+%d\000"
    "\202\267\202\327\202\304\202\314 \202\340\202\266\202\311 \202\324\202\302\202\251\202\301\202\304\202\265\202\334\202\242\202\334\202\265\202\275\201F+%d\000";

char * D_55D98[] = {
    (char *)D_55D50,
    (char *)D_55950 + 0x20,
    (char *)D_55950 + 0x24,
    (char *)D_55950 + 0x28,
    (char *)D_55950 + 0x2C,
    (char *)D_55D50 + 0x1C
};

char D_55DB0[0x4C] =
    "You did not hit a letter.+%d\000\000\000\000"
    "\202\242\202\277\202\307\202\340 \202\340\202\266\202\311 \202\324\202\302\202\251\202\350\202\334\202\271\202\361\202\305\202\265\202\275 : +%d\000\000";

char * D_55DFC[] = {
    (char *)D_55DB0,
    (char *)D_55950 + 0x20,
    (char *)D_55950 + 0x24,
    (char *)D_55950 + 0x28,
    (char *)D_55950 + 0x2C,
    (char *)D_55DB0 + 0x20
};

char D_55E14[0x40] =
    "You hit the letters %d times.\000\000\000"
    "\202\340\202\266\202\311 %d \202\251\202\242 \202\324\202\302\202\251\202\350\202\334\202\265\202\275\000\000\000";

char * D_55E54[] = {
    (char *)D_55E14,
    (char *)D_55CDC + 0x1C,
    (char *)D_55CDC + 0x24,
    (char *)D_55CDC + 0x2C,
    (char *)D_55CDC + 0x34,
    (char *)D_55E14 + 0x20
};

char D_55E6C[0x2C] =
    "Your Monkey Rating\000\000"
    "\202\240\202\310\202\275\202\314\203\202\203\223\203L\201[\203\214\203x\203\213\000\000";

char * D_55E98[] = {
    (char *)D_55E6C,
    (char *)D_55950 + 0x20,
    (char *)D_55950 + 0x24,
    (char *)D_55950 + 0x28,
    (char *)D_55950 + 0x2C,
    (char *)D_55E6C + 0x14
};

u32 D_55EB0[] = {
    0x0000005F, 0x43A00000, 0x43700000, 0x40A00000, 0x3F800000, 0x3F800000,
    0x00000000, 0x00000000, 0x3F800000, 0x3F800000, 0x00000000, 0x3F800000,
    0xFFFFFFFF, 0x0000000A, 0xFFFFFFFF, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000
};
#define S_55950 ((char *)D_55950 + 0x0)
#define S_559BC ((char *)D_559BC + 0x0)
#define S_55A0C ((char *)D_55A0C + 0x0)
#define S_55B2C ((char *)D_55B14 + 0x18)
#define S_55B88 ((char *)D_55B70 + 0x18)
#define S_55B14 ((char *)D_55B14 + 0x0)
#define S_55B70 ((char *)D_55B70 + 0x0)
#define S_55C3C ((char *)D_55C18 + 0x24)
#define S_55C98 ((char *)D_55C7C + 0x1C)
#define S_55C18 ((char *)D_55C18 + 0x0)
#define S_55C7C ((char *)D_55C7C + 0x0)
#define S_55CDC ((char *)D_55CDC + 0x0)
#define S_55E14 ((char *)D_55E14 + 0x0)
#define D_56148 "\x83\x6F\x83\x69\x83\x69\x82\xF0\x20\x82\xBA\x82\xF1\x82\xD4\x20\x82\xC6\x82\xE8\x82\xDC\x82\xB5\x82\xBD\x81\x46\x2B\x25\x64"
#define D_56168 "\x83\x6F\x83\x69\x83\x69\x82\xF0\x20\x82\xD0\x82\xC6\x82\xC2\x82\xE0\x82\xC6\x82\xEA\x82\xDC\x82\xB9\x82\xF1\x82\xC5\x82\xB5\x82\xBD"
#define D_5618C "\x83\x6F\x83\x69\x83\x69\x82\xF0\x20\x25\x64\x96\x7B\x20\x82\xC6\x82\xE8\x82\xDC\x82\xB5\x82\xBD"
#define D_561F8 "\x82\xA8\x82\xA8\x82\xAB\x82\xC8\x83\x6F\x83\x69\x83\x69\x82\xF0\x20\x82\xBA\x82\xF1\x82\xD4\x20\x82\xC6\x82\xE8\x82\xDC\x82\xB5\x82\xBD\x3A\x20\x2B\x25\x64"
#define D_56220 "\x59\x6F\x75\x20\x67\x6F\x74\x20\x61\x6C\x6C\x20\x74\x68\x65\x20\x62\x69\x67\x20\x62\x61\x6E\x61\x6E\x61\x73\x2E\x20\x2B\x25\x64"
#define D_56294 "\x82\xDE\x82\xC4\x82\xAB\x83\x58\x83\x5E\x81\x5B\x82\xF0\x20\x25\x64\x83\x52\x20\x82\xC6\x82\xE8\x82\xDC\x82\xB5\x82\xBD"
#define D_562B4 "\x82\xB7\x82\xD7\x82\xC4\x82\xCC\x20\x82\xE0\x82\xB6\x82\xC9\x20\x82\xD4\x82\xC2\x82\xA9\x82\xC1\x82\xC4\x82\xB5\x82\xDC\x82\xA2\x82\xDC\x82\xB5\x82\xBD\x81\x46\x20\x2B\x25\x64"
#define D_562E4 "\x82\xA2\x82\xBF\x82\xC7\x82\xE0\x20\x82\xE0\x82\xB6\x82\xC9\x20\x82\xD4\x82\xC2\x82\xA9\x82\xE8\x82\xDC\x82\xB9\x82\xF1\x82\xC5\x82\xB5\x82\xBD\x81\x46\x20\x2B\x25\x64"
#define D_56310 "\x82\xE0\x82\xB6\x82\xC9\x20\x25\x64\x82\xA9\x82\xA2\x20\x82\xD4\x82\xC2\x82\xA9\x82\xE8\x82\xDC\x82\xB5\x82\xBD"
#define D_56330 "\x59\x6F\x75\x20\x68\x69\x74\x20\x61\x6C\x6C\x20\x74\x68\x65\x20\x6C\x65\x74\x74\x65\x72\x73\x2E\x20\x2B\x25\x64"
#define D_56350 "\x59\x6F\x75\x20\x64\x69\x64\x20\x6E\x6F\x74\x20\x68\x69\x74\x20\x61\x20\x6C\x65\x74\x74\x65\x72\x2E\x20\x2B\x25\x64"
static void bss_order(void)
{
    memset(&B_D00, 0, 0);
    memset(&B_D04, 0, 0);
    memset(&B_D08, 0, 0);
    memset(&B_D0C, 0, 0);
    memset(&B_D10, 0, 0);
    memset(&B_D11, 0, 0);
    memset(&B_D14, 0, 0);
    memset(&B_D18, 0, 0);
    memset(&B_D1C, 0, 0);
    memset(&staff_roll_final_score, 0, 0);
    memset(&B_D24, 0, 0);
    memset(&staff_roll_banana_count, 0, 0);
    memset(&staff_roll_big_banana_count, 0, 0);
    memset(&staff_roll_item_count, 0, 0);
    memset(&staff_roll_hittable_object_count, 0, 0);
    memset(&staff_roll_hit_percentage, 0, 0);
    memset(&B_D3C, 0, 0);
    memset(&B_D40, 0, 0);
    memset(&B_D4C, 0, 0);
    memset(&B_D50, 0, 0);
    memset(&B_D54, 0, 0);
    memset(&B_D58, 0, 0);
    memset(&B_D5C, 0, 0);
    memset(&B_D60, 0, 0);
    memset(&B_DC4, 0, 0);
    memset(&B_E28, 0, 0);
    memset(&B_E2C, 0, 0);
    memset(&B_2BB4, 0, 0);
    memset(&B_3A78, 0, 0);
    memset(&B_3A84, 0, 0);
    memset(&B_3A88, 0, 0);
    memset(&B_3A8C, 0, 0);
    memset(&g_some_pausemenu_var4, 0, 0);
    memset(&B_3A94, 0, 0);
    memset(&B_3A98, 0, 0);
    memset(&B_3AC8, 0, 0);
    memset(&staff_roll_banana_percentage, 0, 0);
    memset(&B_6574, 0, 0);
    memset(&g_some_ape, 0, 0);
    memset(&B_657C, 0, 0);
    memset(&B_6588, 0, 0);
    memset(&B_658C, 0, 0);
    memset(&B_6590, 0, 0);
    memset(&B_659C, 0, 0);
}

void fn_2_15D10(void)
{
    fade_screen_to_color(0x100, 0xFFFFFF, 0x3C);
    B_D40.x = 0.0f;
    B_D40.y = -10.0f;
    B_D40.z = 0.0f;
    B_D4C = 600;
    B_D0C = 0;
    B_D50 = 0.0f;
    B_D28 = 0;
    B_D2C = 0;
    B_D34 = 0;
    B_D54 = 0;
    B_D38 = 0;
    B_D30 = 0;
    B_D14 = 0;
    B_D1C = 0;
    B_D20 = 0;
    B_D24 = 0;
    B_D08 = 0.0f;
    D_539CC = -20.0f;
    B_D58.b7 = 1;
    B_D58.b6 = 0;
    B_D58.b5 = 0;
    B_D58.b2 = 0;
    B_D58.b3 = 0;
    B_D58.b1 = 0;
    B_D3C = 0;
    B_D5C = 0.003f;
    memset(B_D60, 0, sizeof(B_D60));
    memset(B_DC4, 0, sizeof(B_DC4));
    B_E28 = 2.0f;
    g_staff_roll_name_align();
    g_staff_roll_banana_set(B_E2C);
    g_smth_with_staff_roll_init_3(B_2BB4);
    g_smth_with_staff_roll_init_4();
    g_smth_with_staff_roll_init_5();
    B_D08 = 0.0f;
    B_3A78.x = B_3A78.y = B_3A78.z = 0.0f;
    B_3A84 = 0.0f;
    B_D50 = 0.0f;
}

static void dead_literals(int i)
{
    B_D5C = -6.5f;
    B_E28 = B_D5C * 20.0;
    B_D5C = 1.0f;
    B_E28 = i;
}

void staff_roll_init(void)
{
    int oldHeap;
    int i;

    printf("staff_roll_init");
    if (g_some_other_flags & 4)
        g_very_similar_to_puts("[STAFF_ROLL] init  \n");
    dest_all_events();
    load_stage(0xC5);
    fade_screen_to_color(0x100, 0xFFFFFF, 0x3C);
    event_init(0);
    event_init(0x14);
    event_init(0xE);
    event_init(4);
    event_init(7);
    event_init(0x11);
    event_init(0xB);
    event_init(0x13);
    g_something_calls_sndPopGroup();
    g_SoftStreamStart_with_some_defaults_2(0x32);
    B_D04 = 0;
    oldHeap = OSSetCurrentHeap(stage_heap);
    load_bmp_by_id(3);
    OSSetCurrentHeap(oldHeap);
    load_bmp_by_id(2);
    g_reset_cm_course();
    lbl_1_bss_243C = 0;
    event_init(0x10);
    enable_cameras_for_active_players();
    camera_set_state(0x3F);
    for (i = 0; i < 4; i++)
        set_camera_view_settings(i, 0.0f, 0.0f, 1.0f, 1.0f);
    B_D58.b4 = 0;
    if (modeCtrl.courseFlags & 0x40000)
        B_3A8C = 0;
    else
    {
        B_D58.b4 = 1;
        g_related_to_pause_menu_input.unk4 |= 2;
        B_3A8C = modeCtrl.currPlayer;
    }
    B_D00 = 0;
    B_3A90 = 0;
    B_E28 = 2.0f;
    memset(B_D60, 0, sizeof(B_D60));
    memset(B_DC4, 0, sizeof(B_DC4));
    memset(D_4F428, 0, sizeof(D_4F428));
    B_D58.b7 = 1;
    B_D58.b6 = 0;
    B_D58.b5 = 0;
    B_D58.b3 = 0;
    B_D58.b2 = 0;
    B_D58.b1 = 0;
    B_D3C = 0;
    B_D5C = 0.003f;
    B_3A94 = 0;
    B_D4C = 600;
    D_539C8 = 0.05f;
    D_539CC = -20.0f;
    B_D08 = 0.0f;
    B_D0C = 0;
    g_smth_with_staff_roll_init_1();
    g_smth_with_staff_roll_init_2();
    g_staff_roll_banana_set(B_E2C);
    g_staff_roll_name_align();
    g_smth_with_staff_roll_init_3(B_2BB4);
    g_smth_with_staff_roll_init_4();
    g_smth_with_staff_roll_init_5();
    g_smth_with_staff_roll_init_6();
    set_sub_mode_destination(smd_staff_roll_dest);
}

void g_smth_with_staff_roll_init_7(void)
{
    int i;
    int total;

    if (g_some_other_flags & 0xA)
        return;
    if (B_3A90 != 0)
    {
        B_D00 = 0;
        B_3A90 = 0;
    }
    balls[B_3A8C].pos = B_D40;
    mtxa_from_mtx(B_3A98);
    mtxa_from_identity();
    mtxa_rotate_x(0xB07);
    mtxa_to_mtx(B_3A98);
    if (B_D58.b3)
    {
        fn_2_16FD8();
        fn_2_17114();
    }
    fn_2_1B788();
    fn_2_1B9CC();
    switch (B_D00)
    {
    case 0:
        fn_2_15D10();
        if (B_D04 == 1)
        {
            g_something_with_stopping_music_or_sfx(0x32);
            B_D04 = 0;
            g_SoftStreamStart_with_some_defaults_2(0x32);
        }
        g_current_camera->lookAt.z = 0.0f;
        g_current_camera->lookAt.x = 0.0f;
        B_D00 = 1;
        break;
    case 1:
        fn_2_1D8F0();
        if (B_D58.b4 && B_D4C < 540 && (pad_status_groups[player_pad_map[B_3A8C]].unk0[2].button & PAD_BUTTON_START))
        {
            B_D00 = 8;
            B_D08 = 0.0f;
            B_D58.b4 = 0;
        }
        if (B_D4C < 0)
        {
            B_D4C = 0;
            B_D00 = 2;
        }
        break;
    case 2:
        B_D4C++;
        camera_set_state(3);
        g_current_camera->eye.x = 0.0f;
        g_current_camera->lookAt.x = 0.0f;
        g_current_camera->eye.y = -2.8f;
        g_current_camera->lookAt.y = 1.5f;
        g_current_camera->eye.z = 10.0f;
        g_current_camera->lookAt.z = -5.5f;
        if (B_D58.b4 && (pad_status_groups[player_pad_map[B_3A8C]].unk0[2].button & PAD_BUTTON_START))
        {
            B_D00 = 6;
            B_D08 = 0.0f;
            B_D58.b4 = 0;
        }
        fn_2_186F4();
        fn_2_19510();
        fn_2_199B4(B_E2C);
        if (D_539CC > 300.0 && D_539CC < 360.0)
            fn_2_19FC8(B_3AC8);
        fn_2_1A658(B_2BB4);
        fn_2_1A740(B_2BB4);
        if (D_539CC >= 532.0f)
        {
            B_D08 = 0.0f;
            B_D00 = 3;
        }
        break;
    case 3:
        debug_textdraw_set_pos(1, 1);
        B_D08 += 1.0f;
        if (B_D08 >= 90.0)
        {
            B_D08 = 0.0f;
            B_D00 = 4;
        }
        break;
    case 4:
        total = B_D14;
        if (B_D34 == 0)
            total += D_539E8;
        if (B_D34 >= B_D38 && B_D34 != 0)
            total += D_539E4;
        if (B_D30 >= D_539D4 && B_D30 != 0)
            total += D_539E0;
        if (B_D2C != 0 && B_D2C >= D_539D0)
            total += D_539DC;
        if (B_D28 >= B_6570)
            total += D_539D8;
        B_D20 = total;
        B_D00 = 5;
        for (i = 0; i < 25; i++)
        {
            printf("%d:hist:%2d \t", i, B_DC4[i]);
            printf("div:%2d\n", B_D60[i]);
        }
        printf("hit_count :%d/%d:moji_num  \tper:%4.3f%\n", B_D34, B_D38, 100.0 * ((float)B_D34 / (float)B_D38));
        printf("banana_cnt:%d/%d:banana_sum\tper:%4.3f%\n", B_D28, B_6570, 100.0 * ((float)B_D28 / (float)B_6570));
        printf("b_bana_cnt:%d/%d:sr_bba_sum\tper:%4.3f%\n", B_D2C, D_539D0, 100.0 * ((float)B_D2C / (float)D_539D0));
        printf("item_count:%d/%d:item_sum  \tper:%4.3f%\n", B_D30, D_539D4, 100.0 * ((float)B_D30 / (float)D_539D4));
        printf("final_score:%d\n", B_D20);
        break;
    case 5:
        B_D08 += 1.0f;
        if (B_D08 >= 600.0f)
        {
            B_D08 = 0.0f;
            B_D00 = 6;
        }
        else if (pad_status_groups[player_pad_map[B_3A8C]].unk0[2].button & PAD_BUTTON_A)
        {
            B_D08 += 60.0f;
            if (B_D08 >= 540.0f && B_D24 == B_D20)
            {
                B_D08 = 0.0f;
                B_D00 = 6;
            }
        }
        break;
    case 6:
        B_D08 += 1.0f;
        if (720.0f == B_D08)
        {
            B_D04 = 1;
            g_change_music_volume(0x32, 300, 0);
        }
        if (B_D08 > 900.0f)
        {
            B_D00 = 7;
            B_D08 = lbl_80198B38;
        }
        break;
    case 7:
        B_D08 -= 1.0f;
        if (!B_D08)
            modeCtrl.submodeTimer = 0;
        break;
    case 8:
        if (!B_D08)
            fade_screen_to_color(0x101, 0, 0x1D);
        else if (30.0f == B_D08)
        {
            camera_set_state(3);
            g_current_camera->eye.x = 0.0f;
            g_current_camera->lookAt.x = 0.0f;
            g_current_camera->eye.y = -2.8f;
            g_current_camera->lookAt.y = 1.5f;
            g_current_camera->eye.z = 10.0f;
            g_current_camera->lookAt.z = -5.5f;
            fade_screen_to_color(0x100, 0, 0x1E);
        }
        B_D40.y += 0.25;
        B_D08 += 1.0f;
        if (60.0f == B_D08)
        {
            B_D00 = 6;
            B_D08 = 0.0f;
        }
        break;
    }
}

void game_roll_draw_func(void)
{
    camera_apply_viewport_if_changed(B_3A8C);
    fn_1_D4870();
    g_maybe_some_bg_effect_handler();
    u_reset_light_group_stack(B_3A8C);
    g_draw_stage();
    g_draw_bg();
    g_draw_effects();
    switch (B_D00)
    {
    case 0:
    case 4:
        break;
    case 1:
        if (B_D58.b1)
            fn_2_1E0AC();
        break;
    case 2:
        fn_2_183F4();
        fn_2_19C98(B_E2C);
        if (D_539CC > 300.0 && D_539CC < 360.0)
            fn_2_1A290(B_3AC8);
        fn_2_1ABD8(B_2BB4);
        staff_roll_results_draw_final_score();
        break;
    case 3:
        staff_roll_results_draw_final_score();
        fn_2_1C0E4();
        init_ui_element_sprite_with_defaults();
        set_ui_element_sprite_depth(5.0f);
        B_6574 = -150;
        set_ui_element_sprite_pos(320.0f, B_6574);
        set_ui_element_sprite_scale(1.3f, 1.5f);
        break;
    case 5:
        B_6574 = (B_6574 < 240) ? B_6574 + 15 : 240;
        set_ui_element_sprite_alpha(fn_2_1D368());
        set_ui_element_sprite_pos(320.0f, B_6574);
        set_ui_widescreen_scale_mtx(0x140);
        draw_ui_box(0x5F);
        reset_ui_widescreen_scale_mtx();
        if (B_D08 >= 60.0f)
            staff_roll_results_draw_banana_count();
        if (B_D08 >= 120.0f)
            staff_roll_results_draw_big_banana_count();
        if (B_D08 >= 180.0f)
            staff_roll_results_draw_item_count();
        if (B_D08 >= 240.0f)
            staff_roll_results_draw_letter_count();
        if (B_D08 >= 300.0f)
            staff_roll_results_draw_monkey_rating();
        if (B_D08 >= 300.0f)
            staff_roll_results_draw_final_score();
        break;
    case 6:
        if (B_D08 <= 300.0f)
            fn_2_1D3D0();
        else if (B_D08 <= 600.0f)
            fn_2_1D574();
        else if (B_D08 <= 900.0f)
            fn_2_1D74C();
        break;
    case 7:
        fade_screen_to_color(0x101, 0, 0x41);
        break;
    }
    fn_2_1B534();
    if (B_6578 != NULL)
        draw_ape(B_6578);
    if (B_D58.b7)
        fn_2_1AF10();
    if (B_D00 == 2)
    {
        if (B_D58.b6)
            fn_2_1B0A4();
        if (B_D58.b5)
            fn_2_1B1BC();
    }
    ord_tbl_draw_nodes();
    if (B_D00 != 1)
        fn_2_1E134();
}

void smd_staff_roll_dest(void)
{
    switch (sub_mode)
    {
    case 0x53:
    case 0x54:
        set_sub_mode_destination(smd_staff_roll_dest);
        return;
    }
    printf("staff_roll_dest\n");
    if (g_related_to_pause_menu_input.unk4 & 2)
        g_related_to_pause_menu_input.unk4 &= ~2;
    g_something_with_freeing_chara_heap_ape(g_some_ape);
    g_some_ape = NULL;
    g_something_calls_sndPopGroup();
    destroy_sprite_with_unique_id(0);
    call_free_bmp_by_id(2);
    call_free_bmp_by_id(3);
}

void g_smth_with_staff_roll_init_1(void)
{
    current_ball = &balls[B_3A8C];
    B_D40.x = B_657C.x = 0.0f;
    B_D40.y = B_657C.y = -10.0f;
    B_D40.z = B_657C.z = 0.0f;
    B_3A78.x = B_3A78.y = B_3A78.z = 0.0f;
    B_D50 = 0.0f;
    B_3A84 = 0.0f;
    B_6588 = 0.005f;
    B_658C = 1.0 / B_6588;
}

void g_smth_with_staff_roll_init_2(void)
{
    B_D14 = 0;
    B_D18 = 0;
    B_D1C = 0;
    B_D20 = 0;
    B_D24 = 0;
    B_D28 = 0;
    B_6570 = 880;
    B_D2C = 0;
    D_539D0 = 30;
    B_D30 = 0;
    D_539D4 = 15;
    B_D34 = 0;
    B_D38 = 0;
    D_539D8 = 10000;
    D_539DC = 100;
    D_539E0 = 100;
    D_539E4 = 10000;
    D_539E8 = 100;
}

void fn_2_16FD8(void)
{
    if (B_D00 != 2)
        D_539C8 = 0.0f;
    if (dip_switches & 0x20000000)
    {
        if (pad_status_groups[player_pad_map[B_3A8C]].unk0[0].button & 0x40)
        {
            if (pad_status_groups[player_pad_map[B_3A8C]].unk0[0].button & 0x200)
                D_539C8 = -0.1f;
            else
                D_539C8 = 0.2f;
        }
        else
            D_539C8 = 0.045f;
    }
    else
        D_539C8 = 0.045f;
    D_539CC += D_539C8;
}

static float dead_lit_72(void)
{
    return 7.2f;
}

void fn_2_17114(void)
{
    float limit;
    u32 i;
    int j;

    mtxa_mult_right(B_3A98);
    if (!B_D08 && B_D00 <= 6)
    {
        if ((float)pad_status_groups[player_pad_map[B_3A8C]].unk0[0].stickX < -60.0f)
            B_3A78.x += B_6588;
        else if (60.0f < (float)pad_status_groups[player_pad_map[B_3A8C]].unk0[0].stickX)
            B_3A78.x += B_6588;
        else
            B_3A78.x += (float)pad_status_groups[player_pad_map[B_3A8C]].unk0[0].stickX / (60.0f * B_658C);
        if ((float)pad_status_groups[player_pad_map[B_3A8C]].unk0[0].stickY < -60.0f)
            B_3A78.y += B_6588;
        else if (60.0f < (float)pad_status_groups[player_pad_map[B_3A8C]].unk0[0].stickY)
            B_3A78.y += B_6588;
        else
            B_3A78.y += (float)pad_status_groups[player_pad_map[B_3A8C]].unk0[0].stickY / (60.0f * B_658C);
    }
    B_3A78.x *= 0.97;
    B_3A78.y *= 0.97;
    B_657C.x = B_D40.x;
    B_657C.y = B_D40.y;
    B_657C.z = B_D40.z;
    B_D40.x += B_3A78.x;
    B_D40.y += B_3A78.y;
    limit = (B_D0C != 0) ? 6.2f : 7.2f;
    if (B_D00 == 2)
    {
        if (B_D40.x < -limit)
        {
            B_3A78.x = 0.0f;
            B_D40.x = -limit;
        }
        if (limit < B_D40.x)
        {
            B_3A78.x = 0.0f;
            B_D40.x = limit;
        }
        if (B_D40.y < -5.27f)
        {
            B_3A78.y = 0.0f;
            B_D40.y = -5.27f;
        }
        if (5.27f < B_D40.y)
        {
            B_3A78.y = 0.0f;
            B_D40.y = 5.27f;
        }
    }
    for (i = 0; i < 0x58; i++)
    {
        for (j = 0; j < D_4108[i][0].unk86; j++)
        {
            D_4108[i][j].unk20.x = D_4108[i][j].unk14.x;
            D_4108[i][j].unk20.y = D_4108[i][j].unk14.y;
            D_4108[i][j].unk20.z = D_4108[i][j].unk14.z;
            D_4108[i][j].unk14.x = D_4108[i][j].unk8.x;
            D_4108[i][j].unk14.z = D_4108[i][j].unk8.z;
            D_4108[i][j].unk14.y = D_4108[i][j].unk8.y - D_539CC + D_4108[i][j].unk4 / 2.0;
            D_4108[i][j].unk2C.y = D_4108[i][j].unk8.y - D_539CC;
        }
    }
    switch (B_D00)
    {
    case 3:
    case 4:
    case 5:
        B_D40.y += 0.2f;
        break;
    case 6:
        B_D40.y += 0.25f;
        break;
    }
}

static void dead_lit_3(float *p)
{
    *p = 3.0f;
    *p += 0.03;
    *p = 1.2f;
}

void fn_2_17788(float *p, int t, s8 mode)
{
    switch (mode)
    {
    case 0:
        if (g_some_other_flags & 0xA)
            break;
        if (t < 10)
            *p += math_sin(t * 0x62);
        else if (*p > 0.0)
            *p -= math_sin(t << 3);
        break;
    case 2:
        if (*p >= 0.0)
            *p -= 0.0015;
        break;
    case 3:
        if (t >= 120 && *p < 3.0f)
            *p += 0.03;
        else if (t <= 60)
            *p -= 0.03;
        if (*p >= 3.0f)
            *p = 3.0f;
        if (*p <= 1.2f)
            *p = 1.2f;
        break;
    case 5:
        if (*p >= 0.0)
            *p -= 0.01;
        if (*p < 0.0)
            *p = 0.0f;
        break;
    case 6:
        if (t >= 120 && *p < 0.4)
            *p += 0.01;
        else if (t <= 60 && *p > 0.0)
            *p -= 0.01;
        break;
    case 7:
        *p = 0.4f;
        *p += 0.1 * math_sin(t << 12);
        break;
    }
}

void g_staff_roll_name_align(void)
{
    u32 i;
    int j;
    int k;
    float w0;
    float w1;
    int pattern;
    int offset;
    int idx;
    struct StaffRollName *names[0x58];

    debug_textdraw_set_pos(0x14, 1);
    pattern = (rand() & 0x7FFF) % 5;
    printf("name align pattern:%d\n", pattern);
    for (i = 0; i < 0x35; i++)
        D_4F5E0[i] = D_4F638[pattern][i];
    for (i = 0; i < 0x58; i++)
    {
        if (i < 0x35)
        {
            idx = i;
            names[i] = &D_3C10[idx];
        }
        else
        {
            idx = i - 0x35;
            names[i] = &D_3580[idx];
        }
        D_4F588[i] = (s8)names[i]->unk0;
        if (i >= 0x35)
            D_4F5E0[i] = names[i]->unk4;
        for (j = 0; j < names[i]->unk10; j++)
        {
            D_4108[i][j].unk20.y = D_4108[i][j].unk2C.y = D_4108[i][j].unk8.y = names[i]->unkC;
            if (names[i]->unk14[j] == '-')
            {
                D_4108[i][j].unk8.y = 0.4 + names[i]->unkC;
                D_4108[i][j].unk20.y = D_4108[i][j].unk2C.y = D_4108[i][j].unk8.y;
            }
            D_4108[i][j].unk20.z = D_4108[i][j].unk2C.z = D_4108[i][j].unk8.z = 0.0f;
            D_4108[i][j].unk0 = 0.0f;
            if (j == 0 || names[i]->unk14[j - 1] == '+')
            {
                D_4108[i][j].unk4 = 0.75f;
                D_4108[i][j].unk84 = 1;
            }
            else
            {
                D_4108[i][j].unk4 = 0.55f;
                D_4108[i][j].unk84 = 0;
            }
            D_4108[i][j].unk38.x = 0.0f;
            D_4108[i][j].unk38.y = 0.0f;
            D_4108[i][j].unk38.z = 0.0f;
            D_4108[i][j].unk80 = 0;
            D_4108[i][j].unk86 = names[i]->unk10;
            D_4108[i][j].unk88 = names[i]->unk12;
            D_4108[i][j].unk8A = names[i]->unk14[j];
            if (D_4108[i][j].unk8A != '+')
                staff_roll_hit_percentage++;
        }
        names[i]->unk8 = 0.0f;
        w0 = 0.0f;
        w1 = 0.0f;
        D_4108[i][0].unk8.x = names[i]->unk8;
        D_4108[i][0].unk2C.x = D_4108[i][0].unk8.x;
        for (j = 1; j <= names[i]->unk10; j++)
        {
            if (names[i]->unk14[j] == '+')
                D_4108[i][j].unk8.x = 0.55f + D_4108[i][j - 1].unk8.x;
            else
            {
                for (k = 0; k < 0x21; k++)
                {
                    if (D_32E0[k] == (u8)names[i]->unk14[j])
                        w0 = D_3348[k] * D_4108[i][j].unk4 / 2.0;
                    if (D_32E0[k] == (u8)names[i]->unk14[j - 1])
                        w1 = D_3348[k] * D_4108[i][j - 1].unk4 / 2.0;
                    if (w0 && w1)
                        D_4108[i][j].unk8.x = w0 + D_4108[i][j - 1].unk8.x + w1;
                }
            }
            D_4108[i][j].unk2C.x = D_4108[i][j].unk8.x;
            if (j >= names[i]->unk10)
                D_4F428[i] = D_4108[i][j].unk8.x;
        }
        switch (names[i]->unk2)
        {
        case 0:
        case 1:
        case 2:
        case 3:
            for (j = 0; j <= names[i]->unk10; j++)
            {
                D_4108[i][j].unk8.x -= D_4F428[i] / 2.0 - 0.5;
                D_4108[i][j].unk2C.x = D_4108[i][j].unk8.x;
            }
            break;
        case 4:
            {
                int align = D_4F5E0[i];

                if (align == 0)
                    offset = -D_4F428[i] / 2.0;
                else if (align == 1)
                    offset = -7;
                else if (align == 2)
                    offset = 7.2f - D_4F428[i];
            }
            for (j = 0; j <= names[i]->unk10; j++)
            {
                D_4108[i][j].unk8.x += offset;
                D_4108[i][j].unk2C.x = D_4108[i][j].unk8.x;
            }
            break;
        case 5:
            {
                int x = D_4108[i - 1][0].unk8.x;

                for (j = 0; j <= names[i]->unk10; j++)
                {
                    D_4108[i][j].unk8.x += x;
                    D_4108[i][j].unk2C.x = D_4108[i][j].unk8.x;
                }
            }
            break;
        }
    }
}

void fn_2_183F4(void)
{
    u32 i;
    int j;
    int model;

    for (i = 0; i < 0x58; i++)
    {
        for (j = 0; j < D_4108[i][0].unk86; j++)
        {
            if (D_539CC <= 10.269999980926514 + D_4108[i][j].unk8.y && D_4108[i][j].unk8.y - 10.269999980926514 <= D_539CC)
            {
                D_4F588[i] = 1;
                if (D_4108[i][j].unk8A != '+')
                {
                    model = fn_2_18E24(D_4108[i][j].unk8A) & 0xFFFF;
                    mtxa_from_mtxb();
                    mtxa_mult_right(B_3A98);
                    mtxa_translate_xyz(D_4108[i][j].unk2C.x, D_4108[i][j].unk2C.y, D_4108[i][j].unk2C.z);
                    if (D_4108[i][j].unk8A == '-')
                    {
                        mtxa_scale_xyz(0.4f, 1.0f, 1.0f);
                        mtxa_rotate_z(-0x4000);
                    }
                    if (D_4108[i][j].unk84 != 0)
                        mtxa_scale_xyz(0.6500000358f, 0.75f, 0.55f);
                    else if (D_4108[i][j].unk84 == 0)
                        mtxa_scale_s(D_4108[i][j].unk4);
                    load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
                    avdisp_draw_model_unculled_sort_auto(stage_gma->modelEntries[model].model);
                }
            }
            else if (D_4108[i][j].unk2C.y < -8.269999980926514)
                D_4108[i][j].unk0 = 3.0f;
        }
    }
    avdisp_set_alpha(1.0f);
}

static double dead_lit_11(void)
{
    return 1.1;
}

void fn_2_186F4(void)
{
    float radius = 0.4f;
    float ballRadius = B_D50 / 2.0;
    u32 i;
    int j;

    if (B_D10 > 0)
        B_D10--;
    if (B_D11 > 0)
        B_D11--;
    for (i = 0; i < 0x58; i++)
    {
        if (D_4F588[i] == 1)
        {
            for (j = 0; j < D_4108[i][0].unk86; j++)
            {
                if (D_4108[i][j].unk8A != '+')
                {
                    if (0.0f == D_4108[i][j].unk0 && D_539CC > -19.0
                     && g_some_item_collision_check(&D_4108[i][j].unk20, &D_4108[i][j].unk14, radius, &B_657C, &B_D40, ballRadius) != 0)
                    {
                        fn_2_189B0(i, j);
                        D_4108[i][j].unk38.x = D_4108[i][j].unk38.y = D_4108[i][j].unk38.z = 0.15f;
                    }
                    if (2.0f == D_4108[i][j].unk0)
                    {
                        D_4108[i][j].unk80++;
                        fn_2_18C48(i, j, &D_4108[i][j].unk8, &D_4108[i][j].unk38);
                    }
                }
            }
        }
    }
}

void fn_2_189B0(int i, int j)
{
    struct Effect effect;

    D_4108[i][j].unk0 = 2.0f;
    memset(&effect, 0, sizeof(effect));
    effect.type = 5;
    effect.playerId = B_3A8C;
    effect.pos = B_D40;
    effect.scale.y = 0.6f;
    for (i = 0; i < 3; i++)
    {
        effect.vel.x = 0.11 * B_3A78.x + (0.25 * RAND_FLOAT() - 0.125);
        effect.vel.y = 0.25 * RAND_FLOAT() - 0.125;
        effect.vel.z = 0.25 * RAND_FLOAT() - 0.125;
        spawn_effect(&effect);
    }
    g_rumble_controller(player_pad_map[B_3A8C], 1, 5);
    if (B_D0C == 0 && B_D10 == 0)
    {
        SoundReq(8);
        B_D10 = 5;
    }
    if (B_D0C == 0 && B_D11 == 0)
    {
        g_player_id_for_sound = 0;
        g_smth_with_sound = active_monkey_id[B_3A8C];
        call_SoundReqID_arg_0(0x17);
        B_D11 = 0x1E;
    }
    if (B_D0C == 0)
        B_D14--;
    if (B_D0C != 0 && B_D10 == 0)
    {
        SoundReq(8);
        B_D10 = 5;
    }
    B_D34++;
}

void fn_2_18C48(int i, int j, Vec *pos, Vec *vel)
{
    pos->x += vel->x;
    pos->y += vel->y;
    pos->z += vel->z;
    vel->y -= 0.0098;
    if (D_4108[i][j].unk80 < 10)
        D_4108[i][j].unk4 += math_sin(D_4108[i][j].unk80 * 0x62);
    else if (D_4108[i][j].unk80 >= 10 && D_4108[i][j].unk4 > 0.55f)
        D_4108[i][j].unk4 -= math_sin(D_4108[i][j].unk80 * 0x18);
    if (D_4108[i][j].unk2C.y < -8.269999980926514)
        D_4108[i][j].unk0 = 3.0f;
}

int fn_2_18E24(u8 c)
{
    int i;
    int model;

    for (i = 0; i < 0x21; i++)
    {
        if ((int)c >= 'a' && (int)c <= 'z')
            c -= 0x20;
        if (c == D_32E0[i])
        {
            model = D_3304[i];
            break;
        }
    }
    return model;
}

static double dead_lit_10(void)
{
    return 10.0;
}

void g_staff_roll_banana_set(struct StaffRollItem *items_)
{
    struct StaffRollItem *items = items_;
    int i;
    float unused;
    int idx;

    printf("sr_banana_set\n");
    for (i = 0; i < 90; i++, items++)
    {
        unused = (rand() & 0x7FFF) % 10;
        items->unk0 = 2;
        items->unk2 = 0;
        items->unk20 = 0.8f;
        items->unk4 = 0;
        if (i < 30)
        {
            idx = i;
            items->pos = D_53934[31][idx];
            items->pos.y += 20.0;
        }
        else if (i >= 30 && i < 60)
        {
            idx = i - 30;
            items->pos = D_53934[33][idx];
            items->pos.y += 40.0;
        }
        else if (i >= 60 && i < 90)
        {
            idx = i - 60;
            items->pos = D_53934[0][idx];
        }
    }
    for (i = 0; i < 130; i++)
    {
        items = &B_3AC8[i];
        items->unk0 = 2;
        items->unk2 = 0;
        items->unk20 = 0.8f;
        items->unk4 = 0;
        items->pos.x = (rand() & 0x7FFF) % 14 - 6.5;
        items->pos.y = (rand() & 0x7FFF) % 20;
        items->pos.z = 0.0f;
        items->pos.y += 320.0;
    }
}

void g_smth_with_staff_roll_init_4(void)
{
    float rows[13] = {-6.0f, -5.0f, -4.0f, -3.0f, -2.0f, -1.0f, 0.0f, 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f};
    int i;
    int n = 0;

    D_4FF14[0].z = 0.0f;
    D_4FF14[0].y = 0.1f;
    D_4FF14[0].x = -6.0f;
    for (i = 1; i < 130; i++)
    {
        D_4FF14[i].z = 0.0f;
        D_4FF14[i].x = rows[i / 10];
        if (i % 10 == 0)
        {
            n++;
            if (n % 2 != 0)
                D_4FF14[i].y = 1.1f;
            else
                D_4FF14[i].y = 0.1f;
        }
        else
            D_4FF14[i].y = 2.0 + D_4FF14[i - 1].y;
    }
}

int fn_2_192FC(int n)
{
    s8 group;
    s8 base;
    s16 result;
    s8 a = 15;
    s8 b = 20;
    s8 c = 23;
    s8 d = 25;

    switch (n)
    {
    case 3:
    case 7:
    case 10:
    case 14:
    case 17:
    case 19:
    case 22:
        group = 1;
        break;
    case 1:
    case 11:
        group = 2;
        break;
    case 6:
    case 8:
    case 12:
    case 16:
    case 18:
    case 23:
        group = 3;
        break;
    case 2:
    case 4:
    case 9:
    case 13:
        group = 4;
        break;
    case 5:
    case 20:
    case 21:
        group = 5;
        break;
    case 24:
        group = 6;
        break;
    default:
        group = 0;
        break;
    }
    if (n == 16)
        B_D60[16] /= 4;
    if (n != 15)
    {
        if (group != 0)
        {
            base = (group - 1) * 5;
            if (B_D60[B_3A88 - 1] <= a)
                result = base + 1;
            else if (B_D60[B_3A88 - 1] <= b)
                result = base + 2;
            else if (B_D60[B_3A88 - 1] <= c)
                result = base + 3;
            else if (B_D60[B_3A88 - 1] <= d)
                result = base + 4;
            else
                result = base + 5;
        }
        if (result == B_DC4[n - 1])
        {
            if (result % 5 != 0)
                result++;
            else if (result % 5 == 0)
                result--;
        }
        B_DC4[n] = result;
    }
    else
        result = -1;
    return result;
}

void fn_2_19510(void)
{
    int i;
    int pattern;

    if (D_539CC >= 520.0)
        return;
    if ((int)D_539CC % 20 == 0)
    {
        B_D54 = D_539CC / 20.0f;
        B_3A88 = B_D54;
        if (B_D54 == 15)
            pattern = -1;
        if (B_D54 != 15)
            pattern = fn_2_192FC(B_D54);
    }
    else
        B_D54 = 0;
    if (B_D54 >= 1 && B_D54 < 25 && pattern != -1)
    {
        if (B_D54 % 3 == 2)
        {
            for (i = 60; i < 90; i++)
            {
                if (B_E2C[i].unk2 != 0)
                    fn_2_19850(i, B_E2C, pattern);
            }
        }
        else if (B_D54 % 3 == 0)
        {
            for (i = 0; i < 30; i++)
            {
                if (B_E2C[i].unk2 != 0)
                    fn_2_19850(i, B_E2C, pattern);
            }
        }
        else if (B_D54 % 3 == 1)
        {
            for (i = 30; i < 60; i++)
            {
                if (B_E2C[i].unk2 != 0)
                    fn_2_19850(i, B_E2C, pattern);
            }
        }
    }
    else if (pattern == -1)
    {
        B_3AC8[0].pos.x = -6.5f;
        B_3AC8[0].pos.y = 320.0f;
        for (i = 0; i < 130; i++)
        {
            B_3AC8[i].unk0 = 2;
            B_3AC8[i].unk2 = 0;
            B_3AC8[i].unk20 = 0.8f;
            B_3AC8[i].unk4 = 0;
            B_3AC8[i].pos = D_4FF14[i];
            B_3AC8[i].pos.y += 320.0;
        }
    }
}

void fn_2_19850(int i, struct StaffRollItem *items, int pattern)
{
    s8 idx;
    struct StaffRollItem *item = &items[i];
    float unused;

    if (i < 30)
        idx = i;
    else if (i < 60)
        idx = i - 30;
    else if (i < 90)
        idx = i - 60;
    unused = (rand() & 0x7FFF) % 50;
    item->unk2 = 0;
    item->unk20 = 0.8f;
    item->unk4 = 0;
    item->pos = D_53934[pattern][idx];
    item->pos.x -= 0.25;
    item->pos.y += B_D54 * 20 + 20;
}

void fn_2_199B4(struct StaffRollItem *items)
{
    int i;
    Vec a;
    Vec b;
    float radius;
    int hit;
    float ballR;
    float dz;
    float dy;
    float dx;
    struct StaffRollItem *item;

    item = items;
    for (i = 0; i < 90; i++, item++)
    {
        if (item->unk2 == 1 && item->unk2 != 3)
        {
            a = item->pos;
            b = B_D40;
            ballR = B_D50;
            radius = item->unk20 / 2.0 + ballR / 2.0;
            dx = a.x - b.x;
            dy = a.y - b.y - D_539CC;
            dz = a.z - b.z;
            if (math_sqrt(dz * dz + (dx * dx + dy * dy)) < radius && a.y >= 1.1)
                hit = 1;
            else
                hit = 0;
            if (hit)
            {
                item->unk2 = 2;
                B_D58.b2 = 1;
                B_D28++;
                B_D60[B_3A88]++;
                B_D14++;
                item->unk2 = 3;
            }
        }
        if (item->unk2 == 3 && item->pos.y > 1.0)
        {
            item->unk4++;
            if (item->unk4 >= 10)
            {
                if (item->pos.y - D_539CC < 4.5)
                    item->pos.y += 0.35;
                if (item->pos.y - D_539CC > 3.0 && item->pos.x > -6.8)
                    item->pos.x -= 0.4;
            }
            if (item->pos.y - D_539CC > 4.9 && item->pos.x < -6.9)
                item->unk2 = 4;
            else if (B_D00 >= 3)
                item->unk2 = 4;
        }
    }
    if (B_D58.b2)
    {
        call_SoundReqID_arg_0(0x3B003);
        B_D58.b2 = 0;
    }
}

void fn_2_19C98(struct StaffRollItem *items)
{
    int i;
    struct StaffRollItem *item = items;

    for (i = 0; i < 90; i++, item++)
    {
        if (item->pos.y < 10.269999980926514 + D_539CC && item->unk2 == 0)
            item->unk2 = 1;
        if (item->unk2 == 3 && item->pos.y > 1.0)
        {
            mtxa_from_mtxb();
            avdisp_set_post_mult_color(1.0f, 1.0f, 1.0f, 1.0f);
            mtxa_mult_right(B_3A98);
            mtxa_translate_xyz(item->pos.x, item->pos.y - D_539CC, item->pos.z);
            mtxa_rotate_y(g_font_char_var_3 << 12);
            fn_2_17788(&item->unk20, item->unk4, 0);
            mtxa_scale_s(item->unk20);
            load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
            avdisp_draw_model_culled_sort_auto(init_common_gma->modelEntries[D_4FD5C[item->unk0]].model);
        }
        else if (item->unk2 == 1 && item->pos.y > 1.0 && B_D00 == 2)
        {
            mtxa_from_mtxb();
            avdisp_set_post_mult_color(1.0f, 1.0f, 1.0f, 1.0f);
            mtxa_mult_right(B_3A98);
            mtxa_translate_xyz(item->pos.x, item->pos.y - D_539CC, item->pos.z);
            mtxa_scale_s(item->unk20);
            load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
            avdisp_draw_model_culled_sort_auto(init_common_gma->modelEntries[D_4FD5C[item->unk0]].model);
        }
        if (B_D00 == 3)
        {
            item->unk4++;
            mtxa_from_mtxb();
            avdisp_set_post_mult_color(1.0f, 1.0f, 1.0f, 1.0f);
            mtxa_mult_right(B_3A98);
            mtxa_translate_xyz(item->pos.x, item->pos.y - D_539CC, item->pos.z);
            if (item->unk20 > 0.0)
                item->unk20 -= 0.03;
            mtxa_scale_s(item->unk20);
            mtxa_rotate_y(g_font_char_var_3 << 12);
            fn_80069860(0.5f);
            load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
            avdisp_draw_model_culled_sort_auto(init_common_gma->modelEntries[D_4FD5C[item->unk0]].model);
        }
    }
}

void fn_2_19FC8(struct StaffRollItem *items)
{
    int i;
    Vec a;
    Vec b;
    float radius;
    int hit;
    float ballR;
    float dz;
    float dy;
    float dx;
    struct StaffRollItem *item;

    item = items;
    for (i = 0; i < 130; i++, item++)
    {
        if (item->unk2 == 1 && item->unk2 != 3)
        {
            a = item->pos;
            b = B_D40;
            ballR = B_D50;
            radius = item->unk20 / 2.0 + ballR / 2.0;
            dx = a.x - b.x;
            dy = a.y - b.y - D_539CC;
            dz = a.z - b.z;
            if (math_sqrt(dz * dz + (dx * dx + dy * dy)) < radius && a.y >= 1.1)
                hit = 1;
            else
                hit = 0;
            if (hit)
            {
                item->unk2 = 2;
                B_D58.b2 = 1;
                B_D28++;
                B_D60[B_3A88]++;
                B_D14++;
                item->unk2 = 3;
            }
        }
        if (item->unk2 == 3 && item->pos.y > 1.0)
        {
            item->unk4++;
            if (item->unk4 >= 10)
            {
                if (item->pos.y - D_539CC < 4.5)
                    item->pos.y += 0.35;
                if (item->pos.y - D_539CC > 3.0 && item->pos.x > -6.8)
                    item->pos.x -= 0.4;
            }
            if (item->pos.y - D_539CC > 4.9 && item->pos.x < -6.9)
                item->unk2 = 4;
        }
    }
    if (B_D58.b2)
    {
        call_SoundReqID_arg_0(3);
        B_D58.b2 = 0;
    }
}

void fn_2_1A290(struct StaffRollItem *items)
{
    int i;
    struct StaffRollItem *item = items;

    for (i = 0; i < 130; i++, item++)
    {
        if (item->pos.y < 10.269999980926514 + D_539CC && item->unk2 == 0)
            item->unk2 = 1;
        if (item->unk2 == 3 && item->pos.y > 1.0)
        {
            mtxa_from_mtxb();
            avdisp_set_post_mult_color(1.0f, 1.0f, 1.0f, 1.0f);
            mtxa_mult_right(B_3A98);
            mtxa_translate_xyz(item->pos.x, item->pos.y - D_539CC, item->pos.z);
            mtxa_rotate_y(g_font_char_var_3 << 12);
            fn_2_17788(&item->unk20, item->unk4, 0);
            mtxa_scale_s(item->unk20);
            load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
            avdisp_draw_model_culled_sort_auto(init_common_gma->modelEntries[0x40].model);
        }
        else if (item->unk2 == 1 && item->pos.y > 1.0 && B_D00 == 2)
        {
            mtxa_from_mtxb();
            avdisp_set_post_mult_color(1.0f, 1.0f, 1.0f, 1.0f);
            mtxa_mult_right(B_3A98);
            mtxa_translate_xyz(item->pos.x, item->pos.y - D_539CC, item->pos.z);
            mtxa_scale_s(item->unk20);
            load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
            avdisp_draw_model_culled_sort_auto(init_common_gma->modelEntries[0x40].model);
        }
    }
}

void g_smth_with_staff_roll_init_3(struct StaffRollItem *items)
{
    int i;
    s16 count = 45;
    struct StaffRollItem *item = items;

    for (i = 0; i < count; i++, item++)
    {
        if (i < 15)
            item->unk0 = 0;
        else if (i < count)
            item->unk0 = 1;
        if (item->unk0 == 0)
        {
            item->pos.x = (rand() & 0x7FFF) % 4 - 2;
            item->pos.y = (int)D_4FED8[i];
            item->pos.z = 0.0f;
            item->unk20 = 0.05f;
        }
        if (item->unk0 == 1)
        {
            item->pos = D_4FD70[i - 15];
            item->pos.x = (rand() & 0x7FFF) % 14 - 6.5;
            item->unk20 = 1.5f;
        }
        item->unk2 = 0;
    }
}

void fn_2_1A658(struct StaffRollItem *items)
{
    int i;
    s16 count = 15;
    struct StaffRollItem *item = items;
    float x;

    for (i = 0; i < count; i++, item++)
    {
        x = item->pos.x;
        item->unk14 = 0.1 * math_sin(g_font_char_var_3 << 8);
        item->unk18 = item->pos.y + 0.1 * math_sin(g_font_char_var_3 << 10);
        item->pos.x = item->unk14 + x;
        item->pos.y = item->unk18;
    }
}

void fn_2_1A740(struct StaffRollItem *items)
{
    u8 first = 1;
    int i;
    Vec a;
    Vec b;
    float radius;
    int hit;
    float ballR;
    float dz;
    float dy;
    float dx;
    s16 count = 45;
    struct StaffRollItem *item;
    int t;

    item = items;
    for (i = 0; i < count; i++, item++)
    {
        if (item->unk2 == 1)
        {
            if (D_539CC > 5.0)
            {
                a = item->pos;
                b = B_D40;
                ballR = B_D50;
                radius = 0.6499999761581421 + ballR / 2.0;
                dx = a.x - b.x;
            dy = a.y - b.y - D_539CC;
            dz = a.z - b.z;
            if (math_sqrt(dz * dz + (dx * dx + dy * dy)) < radius && a.y >= 1.1)
                hit = 1;
            else
                hit = 0;
            if (hit)
                {
                    if (item->unk0 == 0)
                    {
                        if (B_D0C == 0)
                            SoundReq(0x1F);
                        B_D0C += 360;
                        B_D30++;
                        B_D60[B_3A88]++;
                    }
                    else if (item->unk0 == 1)
                    {
                        call_SoundReqID_arg_0(0x39);
                        B_D2C++;
                        B_D60[B_3A88] += 5;
                        B_D60[B_3A88]++;
                    }
                    B_D14 += 10;
                    item->unk2 = 2;
                }
            }
        }
        else if (item->unk2 == 2)
        {
            if (item->unk0 == 0 && first)
            {
                first = 0;
                t = B_D0C;
                if (t >= 120 && B_D50 < 3.0f)
                    B_D50 += 0.03;
                else if (t <= 60)
                    B_D50 -= 0.03;
                if (B_D50 >= 3.0f)
                    B_D50 = 3.0f;
                if (B_D50 <= 1.2f)
                    B_D50 = 1.2f;
                if (t > 1)
                {
                    if (B_D58.b6)
                        B_D58.b6 = 0;
                    else if (!B_D58.b6)
                        B_D58.b6 = 1;
                    B_D58.b5 = 1;
                }
                else if (t <= 1)
                {
                    B_D58.b6 = 0;
                    B_D58.b5 = 0;
                }
                if (t == 15)
                    SoundReq(0x1F);
                if (B_D0C > 0)
                    B_D0C--;
                if (B_D0C == 0)
                {
                    B_D50 = 1.2f;
                    item->unk2 = 4;
                    first = 1;
                }
            }
            else if (item->unk0 == 1)
            {
                item->unk4++;
                if (item->unk4 >= 10)
                {
                    if (item->pos.y - D_539CC < 4.5)
                        item->pos.y += 0.35;
                    if (item->pos.y - D_539CC > 3.0 && item->pos.x > -6.8)
                        item->pos.x -= 0.4;
                }
                if (item->pos.y - D_539CC > 4.9 && item->pos.x < -6.9)
                    item->unk2 = 4;
                else if (B_D00 >= 3)
                    item->unk2 = 4;
            }
        }
    }
}

void fn_2_1ABD8(struct StaffRollItem *items)
{
    int i;
    int count = 45;
    struct StaffRollItem *item = items;

    for (i = 0; i < count; i++, item++)
    {
        if (item->pos.y < 7.269999980926514 + D_539CC && item->unk2 == 0)
            item->unk2 = 1;
        if (item->unk2 != 4 && item->pos.y < D_539CC - 7.269999980926514 && item->unk2 != 2)
            item->unk2 = 4;
        if (item->unk2 == 2)
        {
            mtxa_from_mtxb();
            avdisp_set_post_mult_color(1.0f, 1.0f, 1.0f, 1.0f);
            mtxa_mult_right(B_3A98);
            mtxa_translate_xyz(item->pos.x, item->pos.y - D_539CC, item->pos.z);
            if (item->unk0 == 0)
            {
                mtxa_rotate_y(g_font_char_var_3 << 12);
                mtxa_rotate_z(g_font_char_var_3 << 9);
                fn_2_17788(&item->unk20, 120, 2);
            }
            if (item->unk0 == 1)
            {
                mtxa_rotate_y(g_font_char_var_3 << 12);
                fn_2_17788(&item->unk20, item->unk4, 5);
            }
            mtxa_scale_s(item->unk20);
            load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
            avdisp_draw_model_unculled_sort_auto(init_common_gma->modelEntries[D_4FD60[item->unk0]].model);
        }
        else if (item->unk2 == 1)
        {
            mtxa_from_mtxb();
            avdisp_set_post_mult_color(1.0f, 1.0f, 1.0f, 1.0f);
            mtxa_mult_right(B_3A98);
            mtxa_translate_xyz(item->pos.x, item->pos.y - D_539CC, item->pos.z);
            if (item->unk0 == 0)
                mtxa_rotate_z(g_font_char_var_3 << 9);
            if (item->unk0 == 1)
                mtxa_rotate_y(g_font_char_var_3 << 9);
            mtxa_scale_s(item->unk20);
            load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
            avdisp_draw_model_unculled_sort_auto(init_common_gma->modelEntries[D_4FD60[item->unk0]].model);
        }
    }
}

void fn_2_1AF10(void)
{
    int a;
    int b;
    int v = B_3A8C;

    if (modeCtrl.playerCount > 1)
    {
        if (v == 0)
            a = 0;
        if (v == 1)
            a = 1;
        if (v == 2)
            a = 2;
        if (v == 3)
            a = 3;
    }
    else
        a = 3;
    b = 3;
    if (B_D00 == 1)
    {
        mtxa_from_mtxb_translate(&B_D40);
        if (D_539CC >= 532.0f && B_D50 >= 1.2f)
            B_D50 = 1.2f;
        mtxa_rotate_y(0x4000);
        mtxa_rotate_x(0x2000);
        mtxa_scale_s(B_D50);
        load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
        fn_1_4EAFC((Mtx *)mtxa, a, b);
    }
    else
    {
        mtxa_from_mtxb();
        mtxa_mult_right(B_3A98);
        mtxa_translate(&B_D40);
        mtxa_rotate_y(0x4000);
        mtxa_rotate_x(0x2000);
        mtxa_scale_s(B_D50);
        load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
        fn_1_4EAFC((Mtx *)mtxa, a, b);
    }
}

void fn_2_1B0A4(void)
{
    Vec pos;
    struct GMAModel *model = init_common_gma->modelEntries[0x2F].model;

    pos.x = B_D40.x + 0.1 * B_D40.x;
    pos.y = B_D40.y + 0.1 * B_D40.y;
    pos.z = B_D40.z - 1.0;
    avdisp_set_alpha(0.5f);
    mtxa_from_mtxb();
    mtxa_mult_right(B_3A98);
    mtxa_translate(&pos);
    fn_2_17788(&B_3A84, B_D0C, 6);
    mtxa_scale_s(B_3A84);
    load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
    avdisp_set_z_mode(1, 3, 0);
    avdisp_draw_model_unculled_sort_always(model);
    avdisp_set_alpha(1.0f);
}

void fn_2_1B1BC(void)
{
    struct GMAModel *model = init_common_gma->modelEntries[0x2B].model;
    float scale = 0.35f;
    s16 i;

    mtxa_from_mtxb();
    mtxa_mult_right(B_3A98);
    mtxa_translate(&B_D40);
    mtxa_scale_s(scale);
    mtxa_rotate_y(g_font_char_var_3 << 12);
    mtxa_rotate_x(0x2000);
    load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
    avdisp_set_post_mult_color(1.0f, 0.7f, 0.1f, 1.0f);
    avdisp_set_z_mode(1, 3, 0);
    avdisp_draw_model_unculled_sort_always(model);
    avdisp_set_alpha(1.0f);
    scale = 0.4f;
    mtxa_from_mtxb();
    mtxa_mult_right(B_3A98);
    mtxa_translate(&B_D40);
    mtxa_scale_s(scale);
    mtxa_rotate_x(g_font_char_var_3 << 7);
    mtxa_rotate_y(g_font_char_var_3 << 12);
    load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
    avdisp_set_post_mult_color(0.1f, 1.0f, 0.2f, 1.0f);
    avdisp_set_z_mode(1, 3, 0);
    avdisp_draw_model_unculled_sort_always(model);
    avdisp_set_alpha(1.0f);
    mtxa_from_mtxb();
    mtxa_mult_right(B_3A98);
    mtxa_translate(&B_D40);
    mtxa_scale_s(0.4f);
    mtxa_rotate_x(g_font_char_var_3 << 10);
    mtxa_rotate_y(g_font_char_var_3 << 9);
    load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
    set_post_mult_color(1.0f, 0.7f, 0.1f, 1.0f);
    avdisp_set_z_mode(1, 3, 0);
    avdisp_draw_model_unculled_sort_always(model);
    avdisp_set_alpha(1.0f);
    for (i = 0; i < 6; i++)
    {
        struct GMAModel *star = init_common_gma->modelEntries[0x25].model;

        avdisp_set_alpha(1.0f);
        mtxa_from_mtxb();
        mtxa_mult_right(B_3A98);
        mtxa_translate(&B_D40);
        if (i < 4)
        {
            mtxa_rotate_z(g_font_char_var_3 << 8);
            mtxa_rotate_x(g_font_char_var_3 << 10);
            mtxa_rotate_y(g_font_char_var_3 << 9);
        }
        else if (i < 6)
        {
            mtxa_rotate_x(g_font_char_var_3 << 7);
            mtxa_rotate_y(g_font_char_var_3 << 12);
        }
        mtxa_translate(&D_539EC[i]);
        mtxa_rotate_x(g_font_char_var_3 << 12);
        mtxa_scale_s(0.02f);
        load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
        avdisp_set_post_mult_color(1.0f, 1.0f, 1.0f, 1.0f);
        avdisp_draw_model_unculled_sort_auto(star);
    }
}

void fn_2_1B534(void)
{
    struct Effect effect;
    Vec pos;

    if (!(g_some_other_flags & 0xA) && !(g_some_other_flags & 8) && B_D00 != 6)
    {
        pos.x = B_D40.x;
        pos.y = B_D40.y;
        pos.z = 0.28 * B_D40.y;
        if (B_D50 >= 3.0f)
            pos.y = B_D40.y - 1.0;
        memset(&effect, 0, sizeof(effect));
        effect.type = 4;
        effect.playerId = B_3A8C;
        effect.pos = pos;
        spawn_effect(&effect);
        if (B_D40.y > 5.27f)
            fn_1_6869C(4);
    }
}

void g_smth_with_staff_roll_init_6(void)
{
    int lod = 1;

    g_some_ape = g_load_ape_variant0(active_monkey_id[B_3A8C], lod, 0);
    B_6590.x = 0.0f;
    B_6590.y = 0.0f;
    B_6590.z = 1.0f;
    g_some_ape->pos.x = B_D40.x;
    g_some_ape->pos.y = B_D40.y - 0.14;
    g_some_ape->pos.z = B_D40.z;
    g_some_ape->unk270.x = 0.0f;
    g_some_ape->unk270.y = 0.0f;
    g_some_ape->unk270.z = 0.0f;
    g_some_ape->colorIndex = B_3A8C;
    g_some_ape->scale = B_D50 - 0.05;
    fn_1_1132D0(g_some_ape, 0, 1, 0);
    g_smth_with_ape_rotation(g_some_ape, &B_6590);
}

void fn_2_1B788(void)
{
    Quat q;

    mtxa_from_quat(&g_some_ape->rot);
    g_some_ape->pos.x = B_D40.x;
    g_some_ape->pos.y = B_D40.y + -0.014 * B_D40.y;
    g_some_ape->pos.z = 0.28 * B_D40.y;
    g_some_ape->unk270.x = B_3A78.x;
    g_some_ape->unk270.y = B_3A78.y;
    g_some_ape->unk270.z = 0.0f;
    g_some_ape->scale = B_D50 - 0.1;
    if (B_D58.b1)
    {
        fn_1_1132D0(g_some_ape, 0, 7, 4);
        if (B_D4C <= lbl_80198B38 * 3)
        {
            mtxa_push();
            mtxa_from_identity();
            mtxa_to_quat(&q);
            mtxa_pop();
            quat_slerp(&g_some_ape->rot, &g_some_ape->rot, &q, 0.02f);
        }
        else
        {
            B_6590.x = 0.0f;
            B_6590.y = 0.0f;
            B_6590.z = 1.0f;
            g_smth_with_ape_rotation(g_some_ape, &B_6590);
        }
    }
    else if (!B_D58.b3)
    {
        fn_1_1132D0(g_some_ape, 0, 7, 4);
        B_6590.x = 0.0f;
        B_6590.y = 0.0f;
        B_6590.z = 1.0f;
        g_smth_with_ape_rotation(g_some_ape, &B_6590);
    }
    else if (B_D0C != 0)
        fn_1_1132D0(g_some_ape, 0, 7, 8);
    else
    {
        fn_1_1132D0(g_some_ape, 0, 7, 4);
        B_6590.x = 0.0f;
        B_6590.y = 0.0f;
        B_6590.z = -1.0f;
        g_smth_with_ape_rotation(g_some_ape, &B_6590);
    }
    g_ape_anim_body(g_some_ape);
}

void fn_2_1B9CC(void)
{
    if (B_D00 == 3)
    {
        if (2.0f == B_D08)
            call_SoundReqID_arg_0(8);
        if (4.0f == B_D08)
            call_SoundReqID_arg_0(0x1E9);
    }
}


u32 D_560F8[] = {0x3A000000, 0x25640000};
u8 D_56100[] = {0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFD, 0xE9, 0x00, 0x6B, 0xFF, 0xF7};
u32 D_5610C[] = {0x00000000, 0x00FFFFFF, 0x00FDE900, 0x006BFFF7};

void staff_roll_results_draw_final_score(void)
{
    char *labels[6] = {"SCORE", "SCORE", "SCORE", "SCORE", "SCORE", "\x83\x58\x83\x52\x83\x41"};
    struct Sprite *sprite;
    float alpha;

    if (B_D14 > B_D1C)
        B_D1C++;
    if (B_D14 < B_D1C)
        B_D1C--;
    if (B_D00 == 5)
    {
        alpha = fn_2_1D368();
        if (staff_roll_final_score - 100 > B_D24 && staff_roll_final_score >= 1000)
            B_D24 += 100;
        else if (staff_roll_final_score - 10 > B_D24)
            B_D24 += 10;
        if (staff_roll_final_score > B_D24)
            B_D24++;
        if (staff_roll_final_score < B_D24)
            B_D24--;
        if (B_D08 > 420.0f)
            B_D24 = staff_roll_final_score;
        sprite = create_sprite();
        if (sprite != NULL)
        {
            sprite->fontId = 3;
            sprite->textAlign = 1;
            sprite->mulR = D_56100[0];
            sprite->mulG = D_56100[1];
            sprite->mulB = D_56100[2];
            sprite->x = 323.0f;
            sprite->y = 343.0f;
            sprite->opacity = alpha;
            sprite->depth = 2.0f;
            sprite->flags |= 0x81000000;
            sprite->widescreenArg = 0x140;
            if (B_D18 == 0)
            {
                sprite->scaleX = 1.0f;
                sprite->scaleY = 1.0f;
            }
            sprintf(sprite->text, "%6d", B_D24);
            fn_1_AE99C(sprite);
            if (sprite != NULL)
            {
                if (staff_roll_final_score >= 0)
                {
                    sprite->mulR = D_56100[6];
                    sprite->mulG = D_56100[7];
                    sprite->mulB = D_56100[8];
                }
                else
                {
                    sprite->mulR = D_56100[9];
                    sprite->mulG = D_56100[10];
                    sprite->mulB = D_56100[11];
                }
                sprite->flags |= 0x200000;
                sprite->flags |= 0x81000000;
                sprite->widescreenArg = 0x140;
                sprite->x = 320.0f;
                sprite->y = 340.0f;
                sprite->depth = 1.0f;
                if (B_D18 == 0)
                {
                    sprite->scaleX = 1.0f;
                    sprite->scaleY = 1.0f;
                }
                sprintf(sprite->text, "%6d", B_D24);
                fn_1_AE99C(sprite);
            }
        }
        destroy_sprite_with_unique_id(0);
        set_ui_widescreen_scale_mtx(0x140);
        textdraw_reset();
        textdraw_set_flags(0x80000000);
        textdraw_set_font(3);
        textdraw_set_alignment(7);
        textdraw_set_mul_color(0);
        textdraw_set_depth(2.0f);
        textdraw_set_alpha(alpha);
        textdraw_set_pos(323.0f, 343.0f);
        textdraw_printf("%s", labels[locale]);
        textdraw_set_depth(1.0f);
        textdraw_set_mul_color(D_5610C[2]);
        textdraw_set_pos(320.0f, 340.0f);
        textdraw_printf("%s", labels[locale]);
        reset_ui_widescreen_scale_mtx();
    }
    else
    {
        alpha = 1.0f;
        sprite = create_sprite();
        if (sprite != NULL)
        {
            sprite->type = 1;
            sprite->x = 40.0f;
            sprite->y = 40.0f;
            sprite->bmpId = 0xC;
            sprite->textAlign = 4;
            sprite->scaleX = 0.35f;
            sprite->scaleY = 0.35f;
            sprite->flags |= 0x1000000;
            sprite->widescreenArg = 0x140;
            if (B_D00 == 1)
            {
                if (B_D4C <= 60)
                    sprite->opacity = 1.0 - (float)B_D4C / 60.0;
                else
                    sprite->opacity = 0.0f;
            }
            if (B_D00 == 3)
            {
                sprite->opacity = 1.0 - B_D08 / 60.0;
                if (sprite->opacity < 0.0)
                    sprite->opacity = 0.0f;
            }
            if (B_D00 == 2)
                sprite->opacity = 1.0f;
            sprite->flags |= 0x200000;
            sprintf(sprite->text, "bw");
            fn_1_AE99C(sprite);
        }
        destroy_sprite_with_unique_id(0);
        sprite = create_sprite();
        if (sprite != NULL)
        {
            sprite->fontId = 3;
            sprite->textAlign = 0;
            sprite->x = 60.0f;
            sprite->y = 30.0f;
            sprite->scaleX = 1.0f;
            sprite->scaleY = alpha;
            sprite->flags |= 0x80000000;
            sprite->flags |= 0x1000000;
            sprite->widescreenArg = 0x140;
            if (B_D14 >= 0)
            {
                sprite->mulR = D_56100[6];
                sprite->mulG = D_56100[7];
                sprite->mulB = D_56100[8];
            }
            else if (B_D14 < 0)
            {
                sprite->mulR = D_56100[9];
                sprite->mulG = D_56100[10];
                sprite->mulB = D_56100[11];
            }
            if (B_D00 == 1)
            {
                if (B_D4C < 60)
                    sprite->opacity = 1.0 - (float)B_D4C / 60.0;
                else
                    sprite->opacity = 0.0f;
            }
            if (B_D00 == 3)
            {
                sprite->opacity = 1.0 - B_D08 / 60.0f;
                if (sprite->opacity < 0.0)
                    sprite->opacity = 0.0f;
            }
            if (B_D00 == 2)
                sprite->opacity = 1.0f;
            sprite->flags |= 0x200000;
            sprintf(sprite->text, "%4d", B_D1C);
        }
        fn_1_AE99C(sprite);
        destroy_sprite_with_unique_id(0);
    }
}

void fn_2_1C0E4(void)
{
    struct Sprite *sprite = create_sprite();

    if (sprite != NULL)
    {
        sprite->fontId = 9;
        sprite->x = 320.0f;
        sprite->y = 240.0f;
        sprite->textAlign = 4;
        sprite->mulR = D_56100[3];
        sprite->mulG = D_56100[4];
        sprite->mulB = D_56100[5];
        sprite->scaleX = 0.8f;
        sprite->scaleY = 0.8f;
        sprite->mulR = 0xFF;
        sprite->mulG = 0xFF;
        sprite->mulB = 0xFF;
        sprite->flags |= 0x1000000;
        sprite->widescreenArg = 0x140;
        if (B_D08 < 10.0f)
            sprite->opacity = B_D08 / 10.0f;
        else if (B_D08 < 60.0f)
            sprite->opacity = 1.0f;
        else
            sprite->opacity = 1.0 - (B_D08 - 60.0f) / 60.0f;
        sprintf(sprite->text, "GOAL");
        fn_1_AE99C(sprite);
    }
    destroy_sprite_with_unique_id(0);
}

void staff_roll_results_draw_monkey_rating(void)
{
    int i;
    char *buf;
    float alpha = fn_2_1D368();

    set_ui_widescreen_scale_mtx(0x140);
    textdraw_reset();
    textdraw_set_flags(0x80000000);
    textdraw_set_font(3);
    textdraw_set_alignment(4);
    textdraw_set_mul_color(0);
    textdraw_set_depth(2.0f);
    textdraw_set_alpha(alpha);
    textdraw_set_pos(323.0f, 273.0f);
    textdraw_printf("%s", D_55E98[locale]);
    textdraw_set_mul_color(D_5610C[2]);
    textdraw_set_depth(1.0f);
    textdraw_set_pos(320.0f, 270.0f);
    textdraw_printf("%s", D_55E98[locale]);
    buf = "  ";
    for (i = 0; i < 61; i++)
    {
        if (i == 60)
        {
            switch (locale)
            {
            case 0:
                strcpy(buf, D_54214[i].unk8);
                break;
            case 5:
                strcpy(buf, D_54214[i].unk4);
                break;
            case 1:
                strcpy(buf, D_55398[i].unk8);
                break;
            case 2:
                strcpy(buf, D_55398[i].unkC);
                break;
            case 3:
                strcpy(buf, D_55398[i].unk10);
                break;
            case 4:
                strcpy(buf, D_55398[i].unk14);
                break;
            default:
                strcpy(buf, D_54214[i].unk8);
                break;
            }
            break;
        }
        if (staff_roll_final_score >= D_54214[i].unk0)
        {
            switch (locale)
            {
            case 0:
                strcpy(buf, D_54214[i].unk8);
                break;
            case 5:
                strcpy(buf, D_54214[i].unk4);
                break;
            case 1:
                strcpy(buf, D_55398[i].unk8);
                break;
            case 2:
                strcpy(buf, D_55398[i].unkC);
                break;
            case 3:
                strcpy(buf, D_55398[i].unk10);
                break;
            case 4:
                strcpy(buf, D_55398[i].unk14);
                break;
            default:
                strcpy(buf, D_54214[i].unk8);
                break;
            }
            break;
        }
    }
    textdraw_reset();
    textdraw_set_scale(0.9f, 0.9f);
    textdraw_set_flags(0x80000000);
    textdraw_set_font(3);
    textdraw_set_alignment(4);
    textdraw_set_mul_color(0);
    textdraw_set_depth(2.0f);
    textdraw_set_alpha(alpha);
    textdraw_set_pos(323.0f, 303.0f);
    textdraw_printf("%s", buf);
    textdraw_set_mul_color(D_5610C[2]);
    textdraw_set_depth(1.0f);
    textdraw_set_alpha(alpha);
    textdraw_set_pos(320.0f, 300.0f);
    textdraw_printf("%s", buf);
    reset_ui_widescreen_scale_mtx();
}

void staff_roll_results_draw_banana_count(void)
{
    char *a[3] = {D_56148, D_56168, D_5618C};
    char *b[3] = {S_55950, S_559BC, S_55A0C};
    int x = 10.0 * ((float)staff_roll_banana_count / 10.0 - staff_roll_banana_count / 10);
    float alpha = fn_2_1D368();

    set_ui_widescreen_scale_mtx(0x140);
    textdraw_reset();
    textdraw_set_scale(0.9f, 0.9f);
    textdraw_set_flags(0x80000000);
    textdraw_set_font(3);
    textdraw_set_alignment(4);
    textdraw_set_mul_color(0);
    textdraw_set_depth(2.0f);
    textdraw_set_alpha(alpha);
    textdraw_set_pos(323.0f, 133.0f);
    if (staff_roll_banana_count >= staff_roll_banana_percentage)
        textdraw_printf(D_559A4[locale], D_539D8);
    else if (staff_roll_banana_count == 0)
        textdraw_printf(D_559F4[locale]);
    else if (locale == 5)
    {
        switch (staff_roll_banana_count % 10)
        {
        case 0:
        case 1:
        case 6:
        case 8:
            textdraw_printf(D_55A90[0], staff_roll_banana_count);
            break;
        case 2:
        case 4:
        case 5:
        case 7:
        case 9:
            textdraw_printf(D_55A70[0], staff_roll_banana_count);
            break;
        case 3:
            textdraw_printf(D_55AB0[0], staff_roll_banana_count);
            break;
        }
    }
    else
        textdraw_printf(D_55A3C[locale], staff_roll_banana_count);
    textdraw_set_mul_color(D_5610C[2]);
    textdraw_set_depth(1.0f);
    textdraw_set_alpha(alpha);
    textdraw_set_pos(320.0f, 130.0f);
    if (staff_roll_banana_count >= staff_roll_banana_percentage)
        textdraw_printf(D_559A4[locale], D_539D8);
    else if (staff_roll_banana_count == 0)
        textdraw_printf(D_559F4[locale]);
    else if (locale == 5)
    {
        switch (staff_roll_banana_count % 10)
        {
        case 0:
        case 1:
        case 6:
        case 8:
            textdraw_printf(D_55A90[0], staff_roll_banana_count);
            break;
        case 2:
        case 4:
        case 5:
        case 7:
        case 9:
            textdraw_printf(D_55A70[0], staff_roll_banana_count);
            break;
        case 3:
            textdraw_printf(D_55AB0[0], staff_roll_banana_count);
            break;
        }
    }
    else
        textdraw_printf(D_55A3C[locale], staff_roll_banana_count);
    textdraw_reset();
    reset_ui_widescreen_scale_mtx();
}

void staff_roll_results_draw_big_banana_count(void)
{
    char *a[3] = {D_561F8, S_55B2C, S_55B88};
    char *b[3] = {D_56220, S_55B14, S_55B70};
    float alpha = fn_2_1D368();

    set_ui_widescreen_scale_mtx(0x140);
    textdraw_reset();
    textdraw_set_scale(0.9f, 0.9f);
    textdraw_set_flags(0x80000000);
    textdraw_set_font(3);
    textdraw_set_alignment(4);
    textdraw_set_mul_color(0);
    textdraw_set_depth(2.0f);
    textdraw_set_alpha(alpha);
    textdraw_set_pos(323.0f, 163.0f);
    if (staff_roll_big_banana_count != 0 && staff_roll_big_banana_count >= D_539D0)
        textdraw_printf(D_55AFC[locale], D_539DC);
    else if (staff_roll_big_banana_count == 0)
        textdraw_printf(D_55B58[locale]);
    else if (locale == 5)
    {
        switch (staff_roll_big_banana_count % 10)
        {
        case 0:
        case 1:
        case 6:
        case 8:
            textdraw_printf(D_55BEC[0], staff_roll_big_banana_count);
            break;
        case 2:
        case 4:
        case 5:
        case 7:
        case 9:
            textdraw_printf(D_55BC4[0], staff_roll_big_banana_count);
            break;
        case 3:
            textdraw_printf(D_55C14[0], staff_roll_big_banana_count);
            break;
        }
    }
    else
        textdraw_printf(D_55BAC[locale], staff_roll_big_banana_count);
    textdraw_set_mul_color(0xFDE900);
    textdraw_set_depth(1.0f);
    textdraw_set_alpha(alpha);
    textdraw_set_pos(320.0f, 160.0f);
    if (staff_roll_big_banana_count != 0 && staff_roll_big_banana_count >= D_539D0)
        textdraw_printf(D_55AFC[locale], D_539DC);
    else if (staff_roll_big_banana_count == 0)
        textdraw_printf(D_55B58[locale]);
    else if (locale == 5)
    {
        switch (staff_roll_big_banana_count % 10)
        {
        case 0:
        case 1:
        case 6:
        case 8:
            textdraw_printf(D_55BEC[0], staff_roll_big_banana_count);
            break;
        case 2:
        case 4:
        case 5:
        case 7:
        case 9:
            textdraw_printf(D_55BC4[0], staff_roll_big_banana_count);
            break;
        case 3:
            textdraw_printf(D_55C14[0], staff_roll_big_banana_count);
            break;
        }
    }
    else
        textdraw_printf(D_55BAC[locale], staff_roll_big_banana_count);
    textdraw_reset();
    reset_ui_widescreen_scale_mtx();
}

void staff_roll_results_draw_item_count(void)
{
    char *a[3] = {S_55C3C, S_55C98, D_56294};
    char *b[3] = {S_55C18, S_55C7C, S_55CDC};
    float alpha = fn_2_1D368();

    set_ui_widescreen_scale_mtx(0x140);
    textdraw_reset();
    textdraw_set_scale(0.9f, 0.9f);
    textdraw_set_flags(0x80000000);
    textdraw_set_font(3);
    textdraw_set_alignment(4);
    textdraw_set_mul_color(0);
    textdraw_set_depth(2.0f);
    textdraw_set_alpha(alpha);
    textdraw_set_pos(323.0f, 193.0f);
    if (staff_roll_item_count >= D_539D4 && staff_roll_item_count != 0)
        textdraw_printf(D_55C64[locale], D_539E0);
    else if (staff_roll_item_count == 0)
        textdraw_printf(D_55CC4[locale]);
    else
        textdraw_printf(D_55D38[locale], staff_roll_item_count);
    textdraw_set_mul_color(0xFDE900);
    textdraw_set_depth(1.0f);
    textdraw_set_alpha(alpha);
    textdraw_set_pos(320.0f, 190.0f);
    if (staff_roll_item_count >= D_539D4 && staff_roll_item_count != 0)
        textdraw_printf(D_55C64[locale], D_539E0);
    else if (staff_roll_item_count == 0)
        textdraw_printf(D_55CC4[locale]);
    else
        textdraw_printf(D_55D38[locale], staff_roll_item_count);
    textdraw_reset();
    reset_ui_widescreen_scale_mtx();
}

void staff_roll_results_draw_letter_count(void)
{
    char *a[3] = {D_562B4, D_562E4, D_56310};
    char *b[3] = {D_56330, D_56350, S_55E14};
    float alpha = fn_2_1D368();

    set_ui_widescreen_scale_mtx(0x140);
    textdraw_reset();
    textdraw_set_scale(0.9f, 0.9f);
    textdraw_set_flags(0x80000000);
    textdraw_set_font(3);
    textdraw_set_alignment(4);
    textdraw_set_mul_color(0);
    textdraw_set_depth(2.0f);
    textdraw_set_alpha(alpha);
    textdraw_set_pos(323.0f, 223.0f);
    if (staff_roll_hittable_object_count >= staff_roll_hit_percentage)
        textdraw_printf(D_55D98[locale], D_539E4);
    else if (staff_roll_hittable_object_count == 0)
        textdraw_printf(D_55DFC[locale], D_539E8);
    else
        textdraw_printf(D_55E54[locale], staff_roll_hittable_object_count);
    textdraw_set_mul_color(0xFDE900);
    textdraw_set_depth(1.0f);
    textdraw_set_alpha(alpha);
    textdraw_set_pos(320.0f, 220.0f);
    if (staff_roll_hittable_object_count >= staff_roll_hit_percentage)
        textdraw_printf(D_55D98[locale], D_539E4);
    else if (staff_roll_hittable_object_count == 0)
        textdraw_printf(D_55DFC[locale], D_539E8);
    else
        textdraw_printf(D_55E54[locale], staff_roll_hittable_object_count);
    textdraw_reset();
    reset_ui_widescreen_scale_mtx();
}

float fn_2_1D368(void)
{
    if (B_D08 >= 540.0f && B_D08 <= 600.0f)
        return (B_D08 - 600.0f) / -60.0f;
    return 1.0f;
}

void fn_2_1D3D0(void)
{
    float alpha;
    struct Sprite *sprite;

    if (B_D08 >= 60.0f && B_D08 <= 105.0f)
        alpha = (B_D08 - 60.0f) / 45.0f;
    else if (B_D08 > 105.0f && B_D08 < 255.0f)
        alpha = 1.0f;
    else if (B_D08 >= 255.0f && B_D08 <= 300.0f)
        alpha = (B_D08 - 300.0f) / -45.0f;
    else
        alpha = 0.0f;
    sprite = create_sprite();
    if (sprite != NULL)
    {
        sprite->type = 1;
        sprite->x = 322.0f;
        sprite->y = 210.0f;
        sprite->bmpId = 0x202;
        sprite->textAlign = 4;
        sprite->opacity = alpha;
        sprite->flags |= 0x1000000;
        sprite->widescreenArg = 0x140;
        sprintf(sprite->text, "work");
        fn_1_AE99C(sprite);
    }
    destroy_sprite_with_unique_id(0);
}

void fn_2_1D574(void)
{
    float alpha;
    struct Sprite *sprite;

    if (B_D08 >= 305.0f && B_D08 <= 350.0f)
        alpha = (B_D08 - 305.0f) / 45.0f;
    else if (B_D08 > 350.0f && B_D08 < 555.0f)
        alpha = 1.0f;
    else if (B_D08 >= 555.0f && B_D08 <= 600.0f)
        alpha = (B_D08 - 600.0f) / -45.0f;
    else
        alpha = 0.0f;
    sprite = create_sprite();
    if (sprite != NULL)
    {
        sprite->type = 1;
        sprite->x = 180.0f;
        sprite->y = 240.0f;
        sprite->textAlign = 4;
        sprite->bmpId = 0xD;
        sprite->opacity = alpha;
        sprintf(sprite->text, "work");
        fn_1_AE99C(sprite);
    }
    destroy_sprite_with_unique_id(0);
    textdraw_set_mul_color(0xFFFFFF);
    textdraw_set_alignment(4);
    textdraw_set_font(1);
    textdraw_set_flags(0x80000000);
    textdraw_set_alpha(alpha);
    textdraw_set_pos(320.0f, 240.0f);
    textdraw_printf("AMUSEMENT VISION/SEGA,2001,2002");
}

void fn_2_1D74C(void)
{
    float alpha;
    struct Sprite *sprite;

    if (B_D08 >= 605.0f && B_D08 <= 650.0f)
        alpha = (B_D08 - 605.0f) / 45.0f;
    else if (B_D08 > 650.0f && B_D08 < 855.0f)
        alpha = 1.0f;
    else if (B_D08 >= 855.0f && B_D08 <= 900.0f)
        alpha = (B_D08 - 900.0f) / -45.0f;
    else
        alpha = 0.0f;
    sprite = create_sprite();
    if (sprite != NULL)
    {
        sprite->type = 1;
        sprite->x = 320.0f;
        sprite->y = 209.0f;
        sprite->bmpId = 0x200;
        sprite->textAlign = 4;
        sprite->opacity = alpha;
        sprite->flags |= 0x1000000;
        sprite->widescreenArg = 0x140;
        sprintf(sprite->text, "work");
        fn_1_AE99C(sprite);
    }
    destroy_sprite_with_unique_id(0);
}

void fn_2_1D8F0(void)
{
    float f5;
    int ang;

    B_D4C--;
    if (B_D4C == 180)
        create_player_num_and_ready_sprites(120);
    else if (B_D4C == 60)
        create_go_sprite(60);
    camera_set_state(3);
    if (B_D4C <= lbl_80198B38 * 3)
    {
        if (g_current_camera->eye.y > -2.8f)
            g_current_camera->eye.y -= 0.05;
        if (g_current_camera->lookAt.y < 1.5f)
            g_current_camera->lookAt.y += 0.01;
        if (g_current_camera->lookAt.z > -5.5f)
            g_current_camera->lookAt.z -= 0.1;
    }
    else
    {
        g_current_camera->lookAt.x = 0.0f;
        g_current_camera->lookAt.z = 0.0f;
    }
    g_current_camera->eye.x = B_E28 * math_sin(3.1415927f * ((B_D4C << 7) / 23.0));
    g_current_camera->eye.z = B_E28 * math_sin(16384.0 + 3.1415927f * ((B_D4C << 7) / 23.0));
    f5 = (600.0 - 0.5 * lbl_80198B38 - B_D4C) / 100.0;
    if (!B_D58.b1)
    {
        if (B_D40.y < 0.0f)
        {
            g_current_camera->lookAt.y = g_some_ape->pos.y = B_D40.y;
            B_E28 = 10.0f;
            if (B_D4C <= 600.0 - 0.5 * lbl_80198B38)
                B_D40.y += f5;
            g_current_camera->eye.y = abs((int)B_D40.y);
        }
        else
            B_D58.b1 = 1;
    }
    else if (B_D58.b1)
    {
        if (B_D4C <= 4.5 * lbl_80198B38 && B_E28 < 10.0)
            B_E28 += 0.05;
        else if (B_D4C <= lbl_80198B38 * 9 && B_E28 > 2.0)
            B_E28 -= 0.07;
        fn_2_1E030();
        B_D40.y += 0.01 * math_sin(g_font_char_var_3 << 9);
    }
    g_some_ape->pos.y = B_D40.y;
    g_some_ape->pos = B_D40;
    if (B_D50 < 1.2f && B_D4C <= 600.0 - 0.5 * lbl_80198B38)
        B_D50 += 0.03;
    if (B_D4C < 0)
    {
        B_D58.b1 = 0;
        B_D58.b3 = 1;
        B_D50 = 1.2f;
    }
}

void g_smth_with_staff_roll_init_5(void)
{
    int i;

    for (i = 0; i < 80; i++)
    {
        D_4F744[i].x = ((rand() & 0x7FFF) % 20 - 10.0) / 2.0;
        D_4F744[i].z = ((rand() & 0x7FFF) % 20 - 10.0) / 2.0;
        D_4F744[i].y = (rand() & 0x7FFF) % 40 - 60.0f;
    }
}

void fn_2_1E030(void)
{
    int i;

    for (i = 0; i < 80; i++)
        D_4F744[i].y += B_D5C;
    B_D5C += 0.003f;
    if (B_D4C == lbl_80198B38 * 5)
        B_D3C = 1;
}

void fn_2_1E0AC(void)
{
    int i;

    for (i = 0; i < 80; i++)
    {
        mtxa_from_mtxb_translate(&D_4F744[i]);
        mtxa_scale_s(0.8f);
        load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
        avdisp_draw_model_unculled_sort_auto(init_common_gma->modelEntries[0x40].model);
    }
}

void fn_2_1E134(void)
{
    struct SpriteDrawRequest req = {0x304, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0, 1.0f, -1, 6, -1, 0};
    int i;

    if (B_D00 == 2)
    {
        if (B_D4C < 60)
            req.opacity = (float)B_D4C / 60.0;
        else
            req.opacity = 1.0f;
    }
    else
        req.opacity = 1.0f;
    set_ui_widescreen_scale_mtx(0x140);
    for (i = 0; i < 31; i++)
    {
        req.y = i << 4;
        req.flags |= 10;
        req.x = -9.0f;
        draw_sprite_draw_request(&req);
        req.flags |= 10;
        req.x = 649.0f;
        draw_sprite_draw_request(&req);
    }
    reset_ui_widescreen_scale_mtx();
}
