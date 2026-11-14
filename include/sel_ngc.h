#ifndef SEL_NGC_H
#define SEL_NGC_H

/* Shared header for the sel_ngc REL (module 3, NGC mode select / main menu). */
#include "types.h"
#include "background.h" /* must precede stage.h (world_theme macro trick) */
#include "baseball.h"
#include "mathutil.h"
#include "mode.h"
#include "input.h"
#include "gx.h"
#include "avdisp.h"
#include "stage.h"
#include "dip.h"
#include "sprite.h"
#include "sel.h"
#include "pool.h"
#include "effect.h"
#include "bitmap.h"
#include "camera.h"
#include "light.h"
#include "libc.h"
#include "course.h"
#include "ape_internal.h"
#include "sound.h"
#include "textdraw.h"
#include "os.h"
#include "event.h"
#include "option.h"
#include "boat.h"

#ifndef M2C_FIELD
#define M2C_FIELD(expr, type, offset) (*(type)((u8 *)(expr) + (offset)))
#define M2C_BITWISE(type, expr) (expr)
#define M2C_UNK s32
#define M2C_ERROR(x) 0
#endif

/* Menu screen table (data 0xBD80 menu_screen_list, 0x10 bytes each). */
struct MenuEntry
{
    /*0x00*/ u8 next_screen_id;
    /*0x01*/ u8 pad1[3];
    /*0x04*/ char *name[6];        /* en de fr es it ja */
    /*0x1C*/ char *description[6];
};

struct MenuScreen
{
    /*0x00*/ u8 unk0;
    /*0x01*/ u8 entry_count;
    /*0x02*/ u8 pad2[2];
    /*0x04*/ struct MenuEntry *entries;
    /*0x08*/ void (*tick)(void); /* core: the tick functions take no arguments */
    /*0x0C*/ u32 bitflag;
};

enum
{
    MENUSCREEN_MODE_SELECT = 0,
    MENUSCREEN_PARTY_GAME_SELECT = 1,
    MENUSCREEN_OPTIONS = 2,
    MENUSCREEN_RETURN_TO_TITLE_SCREEN = 3,
    MENUSCREEN_NUMBER_OF_PLAYERS = 4,
    MENUSCREEN_CHARACTER_SELECT_1 = 5,
};

/* ---- externs from the main DOL / other RELs not declared in include/ ---- */
void GXCopyTex(void *dest, u8 clear);
void GXGetProjectionv(float *proj);
void GXGetViewportv(float *vp);
void GXInitTexObj(GXTexObj *obj, void *image, u16 width, u16 height, int format, int wrapS, int wrapT, u8 mipmap);
void GXInvalidateTexAll(void);
void GXLoadTexMtxImm(Mtx m, int id, int type);
void GXLoadTexObj_cached(GXTexObj *obj, int map);
void GXSetCullMode_cached(int mode);
void GXSetNumChans_cached(u8 n);
void GXSetNumIndStages_cached(u8 n);
void GXSetNumTevStages_cached(u8 n);
void GXSetNumTexGens_cached(u8 n);
void GXSetTevAlphaIn_cached(int stage, int a, int b, int c, int d);
void GXSetTevAlphaOp_cached(int stage, int op, int bias, int scale, u8 clamp, int outReg);
void GXSetTevColorIn_cached(int stage, int a, int b, int c, int d);
void GXSetTevColorOp_cached(int stage, int op, int bias, int scale, u8 clamp, int outReg);
void GXSetTevKAlphaSel_cached(int stage, int sel);
void GXSetTevKColorSel_cached(int stage, int sel);
void GXSetTevKColor_cached(int id, GXColor color);
void GXSetTexCoordGen2_cached(int dst, int func, int src, int mtx, int normalize, int postMtx);
void GXSetTexCopyDst_cached(u16 width, u16 height, int format, u8 mipmap);
void GXSetTexCopySrc_cached(u16 left, u16 top, u16 width, u16 height);
u32 OSGetSoundMode(void);
void OSTicksToCalendarTime(s64 ticks, void *date);
void SoundGroupLoad(int);
extern s32 active_monkey_id[];
int are_all_extra_monkeys_unlocked(void);
int are_all_gifts_unlocked(void);
int are_all_movies_unlocked(void);
int are_all_party_games_unlocked(void);
void call_SoundReqID_arg_0(int soundId);
void call_SoundReqID_arg_1(int id);
void create_how_to_sprite(void);
int disc_queue_distance(int group);
void disc_queue_status(int group);
void draw_ui_box(s32 bmpId);
M2C_UNK draw_ui_box_only_borders();
void draw_ui_element(s32 bmpId);
void fn_1_1132D0(struct Ape *ape, int b, int c, int d);
void fn_1_11DE54();
void fn_1_12950(void);
M2C_UNK fn_1_13F6B8();
u8 fn_1_16C474(void);
M2C_UNK fn_1_16C4F8();
M2C_UNK fn_1_16C55C();
M2C_UNK fn_1_16C7F8();
int fn_1_16CEBC(void);
M2C_UNK fn_1_16CEE4();
int fn_1_16CFD4(void);
M2C_UNK fn_1_16CFFC();
int fn_1_16D350(int id);
void fn_1_34828(void);
void fn_1_358E8(void);
void fn_1_35918(int track);
void fn_1_36140(void);
u32 fn_1_363C8(int track);
void fn_1_367E4(int mode);
void fn_1_73900(u32 a);
struct Sprite *fn_1_AED58(int tag);
int fn_1_B2FB0(void);
void fn_1_D0EC4(void);
int fn_1_D1344(void);
void fn_1_D4870(void);
void fn_1_D6D84(int i);
void fn_1_D6F9C(int i);
void fn_1_E3164(int n, Vec *points, GXColor *color);
M2C_UNK fn_1_EFE64();
int fn_1_F0204(void);
void fn_1_F0314(void);
void fn_1_F0330(void);
M2C_UNK fn_1_F034C();
M2C_UNK fn_1_F0380();
void fn_1_F83A4(void);
void fn_1_F844C(void);
struct Preview; /* main_loop/preview.c; layout in the sprite section below */
void fn_1_F9CAC(struct Preview *preview, int index);
void fn_1_F9CBC(struct Preview *preview);
void fn_1_F9E1C(void *tex);
void fn_1_F9EAC(struct Preview *preview, float x, float y, float z, float width, float height, u32 color0, u32 color1);
void fn_1_FD62C(u8 a);
u8 fn_1_FD63C(void);
void fn_8000A380(struct StickPoint *point, struct StickPoint *cal);
void g_GXSetTevIndirect_zero_if_different(int stage);
void g_GXSetTevOrder_wrapper(int stage, int coord, int map, int color);
s16 g_SoftStreamStart_with_some_defaults(int track);
void g_SoftStreamStart_with_some_defaults_2(int id);
s16 g_another_SoftStreamStart_wrapper_of_some_sort(int track, u8 volume);
extern s16 g_another_stage_id;
extern u8 g_auto_reload_setting;
void g_change_music_volume(int track, u32 time, s8 volume);
int g_check_current_track(int track);
M2C_UNK g_check_if_partygame_unlocked();
u8 g_check_some_memcard_field(void);
extern u8 g_cm_entry_list9;
void g_display_playpoint_or_gift_message_child(int id, int state, struct TextBox *tb);
void g_draw_bg(void);
void g_draw_stage(void);
extern u8 g_focused_root_menu;
int g_get_bmp_height(int bmpId);
int g_get_bmp_width(int bmpId);
int g_get_result_code(void);
int g_get_some_memcard_var(void);
int g_is_movie_unlocked(int id);
extern s8 g_last_used_memcard_slot_2;
void g_load_preview_texture(void *tex, char *file, int id, u16 w, u16 h, int fmt);
void g_load_sprite_tex_from_dvd(struct Preview *preview);
void g_load_stage_for_menu_bg(s8, int);
s16 g_maybe_related_to_music_crossfading(int track);
void g_maybe_some_bg_effect_handler(void);
void g_save_game_data(void);
void g_set_some_sel_ngc_global_func_ptrs(void (*a)(void), void (*b)(void), void (*c)(void), void (*d)(void));
M2C_UNK g_set_some_ui_element_sprite_flag();
void g_set_ui_element_sprite_flags(int flags);
void g_set_up_debugtext_buffer(void);
M2C_UNK g_smth_related_to_music();
void g_smth_with_playpoint_or_gift_msg(int id, char *msg);
extern u8 g_some_gift_menu_flags;
void g_some_loading_function(void);
int g_some_printf_function_4(int id, char *fmt, ...);
void g_some_replay_func4(int a, int b, struct MemcardRequest *req);
void g_something_calls_sndPopGroup(void);
void g_something_with_GX_vtx_desc(u32 attrs);
M2C_UNK g_something_with_assinging_unlockables();
M2C_UNK g_something_with_name_entry_get_course();
void g_something_with_raster_state(int a);
void g_something_with_rend_efc(int mask);
void g_something_with_stopping_music_or_sfx(int track);
void g_sprintf_memcard_error(int a, char *buf);
int g_swapDiscQueueGroup(int group);
void g_textdraw_set_counter(u16 counter);
M2C_UNK get_num_of_unlocked_party_games();
s8 get_num_unlocked_monkeys(void);
M2C_UNK get_play_point_count();
void init_ui_element_sprite_with_defaults(void);
int is_able_to_unlock_gift(void);
int is_able_to_unlock_party_game(void);
int is_master_unlocked(void);
int is_staff_credits_game_unlocked(void);
extern u8 lbl_1_bss_12AC[0x2C];
extern u8 lbl_1_bss_16D0;
extern u8 lbl_1_bss_16DC;
extern u8 lbl_1_bss_16E8;
extern u8 lbl_1_bss_16F0;
extern s8 lbl_1_bss_1700;
extern u8 lbl_1_bss_1704;
extern u8 lbl_1_bss_1708;
extern u8 lbl_1_bss_1718;
extern u8 lbl_1_bss_1720;
extern u8 lbl_1_bss_172C;
extern u8 lbl_1_bss_1738;
extern s32 lbl_1_bss_1744[2];
extern u8 lbl_1_bss_175C[0xC];
extern s32 lbl_1_bss_6FB68;
extern s32 lbl_1_bss_6FB6C;
extern float lbl_1_bss_6FB7C;
extern float lbl_1_bss_6FB84;
extern u8 lbl_1_bss_A5C40;
extern s32 lbl_1_bss_CF3D8;
extern u8 lbl_1_bss_D1320[];
extern u8 lbl_1_data_65980[];
extern u8 lbl_80081AC0;
extern u8 lbl_80145210;
extern void *lbl_801988E0;
struct Ape *load_ape(int chara, int lod, int scene, int mesh);
void load_stage(int stage);
void mtxa_rotate_y_sin_cos(float sin, float cos);
void mtxa_rotate_z_sin_cos(float sin, float cos);
extern s8 number_of_starting_monkeys;
s16 play_track_and_fade_out_other_tracks(int track, int fadeTime, s8 volume);
void preload_ape(int chara, int lod, int mesh, int scene);
void queue_disc_read(char *file);
void read_stage_name_from_dvd(int stageId, char *buf, int size);
extern struct Light s_lightPool[];
void set_render_mode(int overscanIdx);
void set_ui_element_sprite_add_color_from_u32(u32 color);
void set_ui_element_sprite_alpha(float alpha);
void set_ui_element_sprite_depth(float z);
M2C_UNK set_ui_element_sprite_mult_color_from_u32();
void set_ui_element_sprite_pos(float x, float y);
void set_ui_element_sprite_rot_z(int rot);
void set_ui_element_sprite_scale(float x, float y);
void smd_game_over_save_child(void);
extern u8 stageselect_is_storymode;
extern u8 storymode_save_files[];
#ifdef __MWERKS__
unsigned long strlen(const char *s);
#endif
M2C_UNK textdraw_clear_border();
void textdraw_clear_flags(u32 flags);
void textdraw_printf(char *fmt, ...);
void textdraw_set_add_color(u32 color);
void textdraw_set_border(void);
void textdraw_set_rot_z(int rot);
void textdraw_set_spacing(float x, float y);
M2C_UNK unlock_movie();
M2C_UNK unlock_party_game();
M2C_UNK unlock_staff_credits_game();

/* ---- sel_ngc work area (bss 0x0-0x190, owned by sel_ngc.c) ----
 * Base-addressed in sel_ngc.c; other TUs reach fields via symbol+addend, so
 * dtk names several interior addresses. Phase 2 should turn this into a struct. */
extern u8 g_currently_visible_menu_screen;   /* bss 0x00 */
extern u8 g_next_menu_screen;                /* bss 0x01 */
extern s32 g_active_pads_bitfield;           /* bss 0x4C */
extern u8 g_language_id;                     /* bss 0x50 (core: 1 byte, every TU reads it with lbz) */
extern s32 locked_menu_items;                /* bss 0x54 (core: bitmask of locked entries, read with lwz) */
extern struct MenuScreen menu_screen_list[]; /* data 0xBD80 */


/* ---- sprite ---- */
/* sel_ngc_bg.c */
extern struct Ape *menu_apes[4];
void g_load_select_apes(void);
void fn_3_1CB58(void);
void g_load_some_apes(void);
int fn_3_1CC74(void);
void menu_draw_background(void);
void g_draw_menu_bg_overlay(GXColor *color, int texFmt);
void free_some_apes(void);
void g_draw_stage_0x9f_for_menu_bg(void);
void g_draw_stage_0x8a_for_menu_bg(void);
void g_draw_stage_for_menu_bg_2(void);
void fn_3_1DAA0(struct BallEnvArg *arg);
/* defined in other sel_ngc TUs, called from the sprite/bg/preview TUs
 * (signatures taken from the callees' asm) */
int g_is_screen_in_screen_stack(s8 screenId); /* sel_ngc.c */
u8 *fn_3_6A14(int playerIdx);                 /* sel_ngc.c */
void fn_3_23080(void);                        /* sel_ngc_option.c */
/* Streamed preview texture; same layout as the (file-local) definition in
 * src/main_loop/preview.c, which owns the g_load_preview_texture/fn_1_F9* API. */
struct PreviewFileInfo
{
    /*0x00*/ struct
    {
        /*0x00*/ u8 filler0[0x2C];
        /*0x2C*/ void *userData;
    } cb;
    /*0x30*/ u32 startAddr;
    /*0x34*/ u32 length;
    /*0x38*/ u32 unk38;
}; /* size = 0x3C, DVDFileInfo */
struct Preview
{
    /*0x00*/ s8 state;
    /*0x01*/ s8 isLoading;
    /*0x02*/ u16 flags;
    /*0x04*/ int index;
    /*0x08*/ GXTexObj texObj;
    /*0x28*/ void *imagePtr;
    /*0x2C*/ u32 size;
    /*0x30*/ u16 width;
    /*0x32*/ u16 height;
    /*0x34*/ int format;
    /*0x38*/ void *heap;
    /*0x3C*/ struct PreviewFileInfo fileInfo;
}; /* size = 0x78 */
/* sel_ngc_preview.c */
void sprite_practice_stage_preview_tick(s8 *status, struct Sprite *sprite);
void sprite_practice_stage_preview_disp(struct Sprite *sprite);
void sprite_practice_stage_preview_mask_disp(s8 *status, struct Sprite *sprite);
void create_practice_mode_preview_sprites(void);
void fn_3_1E1AC(void);
void fn_3_1E1DC(s8 *status, struct Sprite *sprite);
void fn_3_1E458(s8 *status, struct Sprite *sprite);
void create_fight_stage_preview_sprites(void);
void fn_3_1EA0C(void);
void fn_3_1EA6C(s8 *status, struct Sprite *sprite);
void fn_3_1EC64(int game);
void fn_3_1EE08(int game);
void fn_3_1EE90(int game);
void fn_3_1EF40(void);
/* void fn_3_1EF64(void); -- not declared here yet: sel_ngc_option.c still has
 * `extern u8 fn_3_1EF64[]` (it stores the address in a table). */

/* ---- core ---- */
/* Layouts used by sel_ngc.c. The SelNgcMenuWork etc. types describe sel_ngc.c's own bss; the
 * SelNgcSettings* types mirror the sel.c globals (src/main_loop/sel.c, same layout, with arrays
 * where the menu indexes them). The externs above keep declaring those globals as u8 so the other
 * TUs' byte-offset code keeps compiling; sel_ngc.c re-declares them with these types. */
/* sel_ngc.c bss 0x00. Other TUs still address it as u8 + byte offset. */
struct SelNgcMenuWork
{
    /*0x00*/ s8 currScreen; /* current menu screen (MENUSCREEN_*) */
    /*0x01*/ s8 nextScreen;
    /*0x02*/ u8 filler2[0x2];
    /*0x04*/ s32 unk4;
    /*0x08*/ s8 unk8; /* transition timer */
    /*0x09*/ u8 filler9[0x3];
    /*0x0C*/ s32 unkC;
    /*0x10*/ s32 unk10;
    /*0x14*/ s32 unk14;
    /*0x18*/ s32 unk18;
    /*0x1C*/ u8 unk1C;
    /*0x1D*/ u8 unk1D;
    /*0x1E*/ u8 unk1E;
    /*0x1F*/ u8 filler1F[0x1];
    /*0x20*/ s32 unk20[4]; /* per player */
    /*0x30*/ u8 unk30;
    /*0x31*/ u8 filler31[0x3];
    /*0x34*/ s32 unk34;
    /*0x38*/ char *unk38; /* description text of the selected entry */
    /*0x3C*/ s8 unk3C;
    /*0x3D*/ u8 filler3D[0x3];
}; /* size = 0x40 */
/* sel_ngc.c bss 0x44 */
struct SelNgcBss44
{
    /*0x00*/ s8 unk0; /* music track (-1 = none) */
    /*0x01*/ u8 filler1[0x3];
    /*0x04*/ s32 unk4;
}; /* size = 0x8 */
/* sel_ngc.c bss 0x58 (follows locked_menu_items) */
struct SelNgcBss58
{
    /*0x00*/ s8 unk0;
    /*0x01*/ s8 unk1;
    /*0x02*/ u8 filler2[0x2];
    /*0x04*/ s32 unk4;
    /*0x08*/ s32 unk8;
    /*0x0C*/ s32 unkC;
}; /* size = 0x10 */
/* = sel.c struct SelSettings9 (lbl_1_bss_16DC) */
struct SelNgcSettings16DC
{
    /*0x00*/ s32 unk0; /* flags */
    /*0x04*/ u8 unk4;
    /*0x05*/ u8 unk5[3];
    /*0x08*/ u8 unk8[4];
}; /* size = 0xC */
/* sel_ngc.c bss 0x178 */
struct SelNgcBss178
{
    /*0x00*/ s8 unk0; /* cursor */
    /*0x01*/ u8 filler1[0x3];
    /*0x04*/ struct SelNgcSettings16DC unk4; /* saved copy of lbl_1_bss_16DC */
    /*0x10*/ s32 unk10; /* saved option value (restored on cancel) */
    /*0x14*/ u8 filler14[0x4];
}; /* size = 0x18 */
/* = sel.c struct SelMenuState (g_focused_root_menu) */
struct SelNgcRootMenu
{
    /*0x00*/ u8 unk0[0x58]; /* per-screen cursor position */
    /*0x58*/ u8 unk58[4]; /* per-player character */
    /*0x5C*/ s32 unk5C;
    /*0x60*/ u8 unk60;
    /*0x61*/ u8 filler61[0x3];
}; /* size = 0x64 */
/* = sel.c struct SelStorySettings (stageselect_is_storymode) */
struct SelNgcStorySettings
{
    /*0x00*/ u8 unk0;
    /*0x01*/ u8 unk1[2];
    /*0x03*/ u8 unk3[2][0xA];
    /*0x17*/ u8 unk17;
}; /* size = 0x18 */
/* = sel.c struct SelSettings8 */
struct SelNgcSettings16D0
{
    /*0x00*/ u8 unk0;
    /*0x01*/ u8 unk1[6];
    /*0x07*/ u8 filler7[0x1];
    /*0x08*/ s32 unk8; /* flags */
}; /* size = 0xC */
/* = sel.c struct SelSettingsA */
struct SelNgcSettings16E8
{
    /*0x00*/ u8 unk0;
    /*0x01*/ u8 unk1;
    /*0x02*/ u8 filler2[0x2];
    /*0x04*/ s32 unk4;
}; /* size = 0x8 */
/* = sel.c struct SelSettingsB */
struct SelNgcSettings16F0
{
    /*0x00*/ u8 unk0;
    /*0x01*/ u8 unk1[5];
    /*0x06*/ u8 unk6;
    /*0x07*/ u8 unk7;
    /*0x08*/ u8 unk8;
    /*0x09*/ u8 unk9[4];
    /*0x0D*/ u8 fillerD[0x3];
}; /* size = 0x10 */
/* = sel.c struct SelSettingsC */
struct SelNgcSettings1700
{
    /*0x00*/ u8 unk0;
    /*0x01*/ s8 unk1;
    /*0x02*/ u8 filler2[0x2];
}; /* size = 0x4 */
/* = sel.c struct SelSettingsD */
struct SelNgcSettings1704
{
    /*0x00*/ u8 unk0;
    /*0x01*/ u8 unk1;
    /*0x02*/ u8 filler2[0x2];
}; /* size = 0x4 */
/* = sel.c struct SelSettingsE */
struct SelNgcSettings1708
{
    /*0x00*/ u8 unk0;
    /*0x01*/ u8 unk1[3];
    /*0x04*/ u16 unk4;
    /*0x06*/ u8 filler6[0x2];
}; /* size = 0x8 */
/* = sel.c struct SelSettingsF (g_auto_reload_setting) */
struct SelNgcAutoReload
{
    /*0x00*/ u32 unk0; /* flags */
    /*0x04*/ u8 unk4;
    /*0x05*/ u8 filler5[0x3];
}; /* size = 0x8 */
/* = sel.c struct SelSettingsG */
struct SelNgcSettings1718
{
    /*0x00*/ u8 unk0;
    /*0x01*/ u8 unk1;
    /*0x02*/ u8 unk2;
    /*0x03*/ u8 unk3;
    /*0x04*/ s32 unk4;
}; /* size = 0x8 */
/* = sel.c struct SelSettingsH (lbl_1_bss_1720, lbl_1_bss_172C) */
struct SelNgcSettingsH
{
    /*0x00*/ u8 unk0;
    /*0x01*/ u8 unk1[4];
    /*0x05*/ u8 unk5;
    /*0x06*/ u8 unk6;
    /*0x07*/ u8 filler7[0x1];
    /*0x08*/ s32 unk8;
}; /* size = 0xC */
/* = sel.c struct SelSettingsI */
struct SelNgcSettings1738
{
    /*0x00*/ u8 unk0;
    /*0x01*/ u8 unk1;
    /*0x02*/ u8 unk2;
    /*0x03*/ u8 unk3;
    /*0x04*/ u8 unk4;
    /*0x05*/ u8 unk5[5];
    /*0x0A*/ u8 unkA;
    /*0x0B*/ u8 fillerB[0x1];
}; /* size = 0xC */

/* ---- sprite (sel_ngc.c functions called from sel_ngc_sprite.c) ---- */
void fn_3_198(s32 draw, s32 index);
void g_something_with_menus(s32 arg0);
void g_get_some_challenge_mode_difficulty_info(s32 arg0);
void g_get_some_category_count(s32 arg0, s32 *arg1, s32 *arg2);

#endif
