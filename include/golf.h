/* Monkey Golf 2 (golf2.rel): declarations shared by the golf TUs. */
#ifndef GOLF_H
#define GOLF_H

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
#include "pool.h"
#include "rand.h"
#include "shadow.h"
#include "sound.h"
#include "sprite.h"
#include "stage.h"
#include "stcoli.h"
#include "textdraw.h"
#include "world.h"

/* ---- golf_1.c / golf_3.c (w5) ---- */
/* golf.c data used by golf_1.c (rest declared in w1's section) */
extern s32 lbl_11_bss_29F4;  /* defined in golf.c: 4-aligned, before golf_1.c's 8-aligned .bss anchor 0x29F8 */
/* golf.c functions used by golf_1.c */
s16 fn_11_EB18(void);  /* current course */
u8 fn_11_EB28(void);
void fn_11_EB38(Vec *pos);
void fn_11_EB5C(Vec *pos);
void fn_11_EB80(Vec *pos);
void fn_11_EBA4(s16 *angle);
void fn_11_EBB8(s16 *angle);
void fn_11_EBCC(s16 *angle);
int fn_11_16FD8(void);
BOOL fn_11_171C8(void);
/* golf_1.c */
void fn_11_171E8(struct Ball *ball);
/* golf_3.c */
void fn_11_1CF04(void);
void fn_11_1D250(s16 enable);
/* ---- golf_4.c fn_11_252A8 (scorecard, worker w9) ---- */
/* main_loop: golf mode select (sel.c struct SelSettingsD; golf reads the fields signed) */
struct GolfSelSettings
{
    /*0x00*/ s8 unk0; /* 0 = stroke play, 1 = match play */
    /*0x01*/ s8 unk1; /* 0 = 18 holes, 1 = front 9, 2 = back 9 */
    /*0x02*/ u8 filler2[2];
};
extern struct GolfSelSettings lbl_1_bss_1704;
extern s32 active_monkey_id[];
extern s32 locale;
void textdraw_reset(void);
void textdraw_set_font(int fontId);
void textdraw_set_flags(u32 flags);
void textdraw_set_depth(float depth);
void textdraw_set_mul_color(u32 color);
void textdraw_set_pos(float x, float y);
void textdraw_set_scale(float x, float y);
void textdraw_print(char *str);
/* golf.c (w1) */
s8 fn_11_16FC4(int hole);              /* par of a hole */
u8 fn_11_E9A8(void);
u8 fn_11_EC00(int player, int hole);   /* strokes of a player on a hole */
u8 fn_11_EC1C(int player);             /* holes won (match play) */
u8 fn_11_EC30(void);                   /* current hole index */
u8 fn_11_EEFC(void);
u8 fn_11_EF34(void);
u8 fn_11_EFC4(u8 player);
/* ---- end golf_4.c fn_11_252A8 ---- */
/* ---- golf_4.c ---- */
/* {frame list, count}: animation sequence of sprite indices (golf_4.c .data 0x49E4..0x4A44) */
struct GolfFrameList
{
    const u16 *frames;
    u16 count;
};


/* DOL/main_loop functions used by golf_4.c that no shared header declares */
void reset_ui_widescreen_scale_mtx(void);
void set_ui_widescreen_scale_mtx(int width);
extern s32 locale;
void textdraw_print(char *str);
void textdraw_reset(void);
void textdraw_set_alignment(s8 alignment);
void textdraw_set_alpha(float alpha);
void textdraw_set_add_color(u32 color);
void textdraw_set_font_style(int style);
void textdraw_set_spacing(float x, float y);
void textdraw_set_depth(float depth);
void textdraw_set_flags(u32 flags);
void textdraw_set_font(int fontId);
void textdraw_set_mul_color(u32 color);
void textdraw_set_pos(float x, float y);
void textdraw_set_scale(float x, float y);
float textdraw_get_pixel_width_of_string(char *str);

/* golf.c functions used by golf_4.c (types inferred from the call sites) */
u8 fn_11_EEFC(void);
u8 fn_11_E9A8(void);
u8 fn_11_EC30(void);
s8 fn_11_EC54(void);
void fn_11_EC64(s8 layer);
u8 fn_11_EC98(void);
u8 fn_11_F220(u8 player);
void call_SoundReqID_arg_0(int soundId);
int fn_1_11465C(struct Ape *ape, int a, int b);
int fn_1_1146A4(struct Ape *ape, int a, int b);
void fn_1_1132D0(struct Ape *ape, int b, int c, int d);
void fn_11_139C8(float x, float y, float z, float *outX, float *outY, float tanHalfFov);
s8 fn_11_16FA4(void);  /* par of the current hole */
u8 fn_11_EC1C(int player);
u8 fn_11_EC00(int player, int hole);
s8 fn_11_16FC4(int hole);
u8 fn_11_EF34(void);
/* golf_4.c cross-fragment functions */
void fn_11_1D260(float x, float y, s32 chara, u8 mode);
void fn_11_1D71C(float x, float y, s32 chara, u8 mode);
void fn_11_1DB90(float x, float y, s32 chara, u8 mode);
void fn_11_1E104(s32 timer);
void fn_11_1E4C0(s32 x, s32 y, s32 n, char *text);
void fn_11_1FD20(void);
void fn_11_252A8(void);
void fn_11_2DBC0(void);
void fn_11_1E794(u32 timer);
void fn_11_1EC9C(s16 result, u32 timer);
void fn_11_1F7FC(s32 n);
void fn_11_1FA7C(s32 n);
void fn_11_2DE64(s16 *sel, float power);
void fn_11_2E068(s16 *sel, float unused, float t);
void fn_11_2E350(s16 *sel, float t);
void fn_11_2E638(s16 *sel);
void fn_11_2EBBC(u8 player);
void fn_11_2EDB4(s32 x, s32 y);
void fn_11_2EF2C(void);
void fn_11_2F0F8(void);
void fn_11_3071C(void);
void g_smth_with_golf_wind_velocity(float speed);

extern u32 lbl_11_bss_2AC0;
extern u32 lbl_11_bss_2AC4;
extern s16 lbl_11_bss_2AC8;
extern u32 lbl_11_bss_2ACC;
extern Vec lbl_11_bss_2AD0[4];
/* ---- end golf_4.c ---- */

/* golf_4.c fn_11_20858 (w8): from golf.c */
float fn_11_ECA8(void);  /* distance to the pin, in the display unit */
float fn_11_EDF8(void);  /* height difference to the pin, in the display unit */
extern s8 lbl_11_bss_7C; /* selects the distance unit sprite */
/* ---- golf.c (w1): data shared with the other golf TUs ---- */

/* golf.c .bss 0xEC: per player */
struct GolfScoreEC
{
    /*0x00*/ u8 unk0[7];
    /*0x07*/ u8 unk7;
    /*0x08*/ u16 unk8;  /* front nine total */
    /*0x0A*/ u16 unkA;  /* back nine total */
};

/* golf.c .bss 0x11C: per player */
struct GolfScore11C
{
    /*0x00*/ u8 unk0[3];
    /*0x03*/ u8 unk3;
    /*0x04*/ u16 unk4;
    /*0x06*/ u16 unk6;
};

/* golf.c .data 0x28 (unreferenced table) */
struct GolfData28
{
    u32 unk0;
    s16 unk4;
    s16 unk6;
};


extern struct Camera *lbl_11_bss_24;
extern Vec lbl_11_bss_38;
extern s32 lbl_11_bss_4C;
extern s32 lbl_11_bss_50;
extern float *lbl_11_bss_58;
extern f32 lbl_11_bss_5C;
extern s32 golf_wind_angle;
extern f32 golf_wind_velocity;
#ifndef __MWERKS__
void pc_golf_test_wind(void);
#endif
extern s32 lbl_11_bss_6C;
extern s32 lbl_11_bss_70;
extern s32 lbl_11_bss_74;
extern s32 lbl_11_bss_78;
extern s8 lbl_11_bss_7C;
extern s32 lbl_11_bss_80;
extern s32 lbl_11_bss_84;
extern s32 lbl_11_bss_94;
extern s8 lbl_11_bss_CA;
extern s32 lbl_11_bss_D0;
extern f32 lbl_11_bss_D4;
extern f32 lbl_11_bss_D8;
extern f32 lbl_11_bss_180;
extern f32 lbl_11_bss_184;
extern Vec lbl_11_bss_18C;
extern Vec lbl_11_bss_198[];
extern s32 lbl_11_bss_B18;
extern s32 lbl_11_bss_B38;
extern s32 lbl_11_bss_B40;
extern s32 lbl_11_bss_F14;
extern Vec lbl_11_bss_29E4;
extern f32 lbl_11_bss_29F0;
extern s32 lbl_11_bss_29F4;

/* ---- end golf.c (w1) ---- */

/* ---- golf.c: fn_11_900 (golf_900.inc, w2) ---- */

/* lbl_11_bss_30: golf main mode state (fn_11_900) */
#define GOLF_STATE_INIT 0x1
#define GOLF_STATE_TEE_SETUP 0x2       /* set up the next shot */
#define GOLF_STATE_TEE_INTRO 0x4       /* hole flyover before the first shot */
#define GOLF_STATE_AIM 0x8             /* aiming; also the ball in flight */
#define GOLF_STATE_SWING 0x10
#define GOLF_STATE_POWER 0x20          /* power/impact meter */
#define GOLF_STATE_CUP_IN 0x40
#define GOLF_STATE_GIVE_UP 0x80        /* stroke limit reached */
#define GOLF_STATE_REPLAY_INIT 0x100
#define GOLF_STATE_REPLAY_SHOT 0x200
#define GOLF_STATE_REPLAY_FLIGHT 0x400
#define GOLF_STATE_HOLE_END 0x800
#define GOLF_STATE_SCOREBOARD 0x1000
#define GOLF_STATE_RESULTS 0x2000
#define GOLF_STATE_RANKING 0x4000
#define GOLF_STATE_REPLAY_CAMERA 0x8000
#define GOLF_STATE_REPLAY_INTRO 0x10000
#define GOLF_STATE_CLEAR_INIT 0x20000
#define GOLF_STATE_CLEAR 0x40000

/* golf settings from the minigame menu: [0] 1 = match play, [1] course: 0 = 18 holes, 1 = holes 1-9, 2 = holes 10-18 */
extern s32 lbl_1_bss_6FB6C;  /* debug menu xint */
extern s32 active_monkey_id[];
extern u32 g_player_id_for_sound;
extern u32 g_smth_with_sound;
extern int lbl_80198B38;

void SoundIcsReq(u32 soundId, u8 intensity, s8 volume);
void call_SoundReqID_arg_0(int soundId);
void dest_all_events(void);
void event_dest(int id);
void event_init(int id);
BOOL did_ball_fallout(struct Ball *ball);
void event_vibration_dest(void);
void fade_screen_to_color(s32 type, u32 color, int duration);
void fn_1_1118BC(void);
void fn_1_1132D0(struct Ape *ape, int b, int c, int d);
void fn_1_113600(struct Ape *ape, float frame);
void fn_1_113768(struct Ape *ape, float b);
void fn_1_11DE54(int a, int b);
int fn_1_11DF64(int a, u8 *b, int c);
int fn_1_11E1C0(int a, void *b);
int fn_1_11E3A4(void);
void fn_1_3061C(int a, int b);
void fn_1_36140(void);
void fn_1_4AF48(struct Ball *ball, struct PhysicsBall *physBall);
void fn_1_E4708(s8 *charas, s8 *ranks);
int fn_1_E5DC8(void);
void free_minigame_gma_tpl(void);
void g_SoftStreamStart_with_some_defaults_2(int id);
void g_change_music_volume(int track, u32 time, s8 volume);
void g_fade_track_volume(int a, int b);
void g_load_minigame_gma_tpl(int idx);
void g_load_stage_for_menu_bg(s8, int);
s16 g_maybe_related_to_music_crossfading(int track);
void g_maybe_smth_with_music(int track, s8 volume);
void g_move_and_collide(struct Ball *ball, struct PhysicsBall *physBall);
void g_return_to_sel_ngc(int a);
void g_rumble_controller(s32 pad, s32 a, s32 b);
s16 g_smth_related_to_music(int track, int fadeTime, int otherFadeTime, s8 volume, s8 otherVolume);
void g_smth_with_ape_rotation(struct Ape *ape, Vec *arg1);
void g_something_calls_sndPopGroup(void);
void g_something_with_iteratively_freeing_memory(void);
void g_something_with_stopping_music_or_sfx(int track);
void load_bmp_by_id(int id);
void position_ball(struct Ball *ball, struct PhysicsBall *physBall);
void preload_ape(int chara, int lod, int mesh, int scene);
void quat_mult(Quat *dst, Quat *a, Quat *b);
void reset_ui_widescreen_scale_mtx(void);
void set_ui_widescreen_scale_mtx(int width);
void textdraw_set_font(int fontId);

/* golf_4.c */
void fn_11_2F0F8(void);
/* ---- end golf_900.inc ---- */
/* ---- golf.c functions used by golf_2.c (w6) ---- */
u8 fn_11_EB28(void);
u8 fn_11_EC00(int player, int idx);
u8 fn_11_EC30(void);
u8 fn_11_EC78(void);
void fn_11_F27C(Mtx m);

/* ---- golf_1.c globals used by golf_2.c (w6). Their .bss (0x2A48-0x2A80) sits before
   golf_2.c's own .bss anchor at 0x2A80, so they must be defined in golf_1.c. ---- */
extern s32 lbl_11_bss_2A48;  /* golfer idle voice cooldown */
extern u8 lbl_11_bss_2A4C;   /* golfer idle voice pick */
extern Mtx lbl_11_bss_2A50;  /* golfer placement matrix */

/* ---- golf_6.c (w6): ground pattern projected around the ball ---- */
struct GolfGroundTexParams
{
    /*0x00*/ struct GMAModel *model1;  /* texObjs[0]: scrolling pattern */
    /*0x04*/ struct GMAModel *model2;  /* texObjs[0]: height fade */
    /*0x08*/ Vec pos;                  /* base point of the height fade */
    /*0x14*/ float unk14;
    /*0x18*/ float unk18;
    /*0x1C*/ float height;
};
int fn_11_383A0(struct GolfGroundTexParams *params);
void fn_11_38774(void (*drawFunc)(void));

/* ---- golf.c fragment golf_129FC.inc (w4): functions used by other fragments and TUs ---- */

void fn_11_129FC(void);
void fn_11_1338C(u8 *r, u8 *g, u8 *b, int t);
void fn_11_13630(void);
void fn_11_138C8(float *outX, float *outY, float z, float x, float y, float unused);
void fn_11_139C8(float x, float y, float z, float *outX, float *outY, float unused);
void fn_11_13AC4(void);
void fn_11_13BAC(void);
void fn_11_13C08(void);
void fn_11_13CBC(void);
void fn_11_13E6C(void);
void fn_11_15248(void);
void fn_11_158B8(void);
void fn_11_15FB4(Vec *out, int count, Vec *points, int den, int num);
void fn_11_16318(void);
float fn_11_16ACC(float x, int n);
float fn_11_16B2C(int n, float x);
void fn_11_16B68(s8 *flag, float *maxDist, Vec *center);
s8 fn_11_16FA4(void);
s8 fn_11_16FC4(int hole);
int fn_11_16FD8(void);
int fn_11_170E8(int player);
BOOL fn_11_171C8(void);

/* ---- end golf_129FC.inc (w4) ---- */

/* ---- golf_5.c (w10): golf.c functions used by the camera, and its callback ---- */
s32 fn_11_EBE0(void);
s32 fn_11_EBF0(void);
u16 fn_11_EC44(void);
void fn_11_312F4(struct Camera *camera, struct Ball *ball);
/* ---- end golf_5.c ---- */

#endif
