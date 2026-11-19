/* sel_ngc.c: Main menu state machine: menu screen stack, per-screen tick handlers, menu tables. */

/*
 * sel_ngc.h declares the menu work area (g_currently_visible_menu_screen) and the sel.c menu
 * settings globals as plain bytes, because the other sel_ngc TUs still reach their fields through
 * byte offsets. This file accesses them through the struct types in the "core" section of
 * sel_ngc.h (same layout as the definitions in src/main_loop/sel.c and course.c), so the byte
 * declarations are renamed out of the way while the headers are read and the objects are
 * re-declared with their real types below.
 */
#define g_currently_visible_menu_screen g_currently_visible_menu_screen_as_u8
#define g_focused_root_menu g_focused_root_menu_as_u8
#define stageselect_is_storymode stageselect_is_storymode_as_u8
#define lbl_1_bss_16D0 lbl_1_bss_16D0_as_u8
#define lbl_1_bss_16DC lbl_1_bss_16DC_as_u8
#define lbl_1_bss_16E8 lbl_1_bss_16E8_as_u8
#define lbl_1_bss_16F0 lbl_1_bss_16F0_as_u8
#define lbl_1_bss_1700 lbl_1_bss_1700_as_s8
#define lbl_1_bss_1704 lbl_1_bss_1704_as_u8
#define lbl_1_bss_1708 lbl_1_bss_1708_as_u8
#define g_auto_reload_setting g_auto_reload_setting_as_u8
#define lbl_1_bss_1718 lbl_1_bss_1718_as_u8
#define lbl_1_bss_1720 lbl_1_bss_1720_as_u8
#define lbl_1_bss_172C lbl_1_bss_172C_as_u8
#define lbl_1_bss_1738 lbl_1_bss_1738_as_u8
#define g_cm_entry_list9 g_cm_entry_list9_as_u8
#define lbl_1_bss_A5C40 lbl_1_bss_A5C40_as_u8
/* complete types are defined below; file-scope tags so the header prototypes refer to them */
struct TextBox;
struct MemcardRequest;
struct StickPoint;
#include "sel_ngc.h"
#undef g_currently_visible_menu_screen
#undef g_focused_root_menu
#undef stageselect_is_storymode
#undef lbl_1_bss_16D0
#undef lbl_1_bss_16DC
#undef lbl_1_bss_16E8
#undef lbl_1_bss_16F0
#undef lbl_1_bss_1700
#undef lbl_1_bss_1704
#undef lbl_1_bss_1708
#undef g_auto_reload_setting
#undef lbl_1_bss_1718
#undef lbl_1_bss_1720
#undef lbl_1_bss_172C
#undef lbl_1_bss_1738
#undef g_cm_entry_list9
#undef lbl_1_bss_A5C40

/* sel.c (src/main_loop/sel.c) menu settings */
extern struct SelNgcRootMenu g_focused_root_menu;         /* sel.c struct SelMenuState */
extern struct SelNgcStorySettings stageselect_is_storymode;
extern struct SelNgcSettings16D0 lbl_1_bss_16D0;          /* fight */
extern struct SelNgcSettings16DC lbl_1_bss_16DC;
extern struct SelNgcSettings16E8 lbl_1_bss_16E8;
extern struct SelNgcSettings16F0 lbl_1_bss_16F0;
extern struct SelNgcSettings1700 lbl_1_bss_1700;
extern struct SelNgcSettings1704 lbl_1_bss_1704;
extern struct SelNgcSettings1708 lbl_1_bss_1708;
extern struct SelNgcAutoReload g_auto_reload_setting;
extern struct SelNgcSettings1718 lbl_1_bss_1718;
extern struct SelNgcSettingsH lbl_1_bss_1720;
extern struct SelNgcSettingsH lbl_1_bss_172C;             /* baseball */
extern struct SelNgcSettings1738 lbl_1_bss_1738;
/* course.c */
extern struct CourseCommand g_cm_entry_list9[];

/* textbox.c (file-local there, like in the other TUs that fill one in) */
struct TextBox
{
    /*0x00*/ s32 state;
    /*0x04*/ s32 timerMax;
    /*0x08*/ u32 timer;
    /*0x0C*/ s16 x;
    /*0x0E*/ s16 y;
    /*0x10*/ float width;
    /*0x14*/ s8 columns;
    /*0x15*/ s8 rows;
    /*0x16*/ s8 style;
    /*0x17*/ s8 unk17;
    /*0x18*/ s8 unk18;
    /*0x19*/ s8 unk19;
    /*0x1A*/ u8 filler1A[2];
    /*0x1C*/ void (*callback)(struct TextBox *);
    /*0x20*/ s32 id;
    /*0x24*/ u32 textColor;
    /*0x28*/ s8 textAlign;
    /*0x29*/ u8 filler29[3];
    /*0x2C*/ u32 flags;
    /*0x30*/ float textScale;
}; /* size = 0x34 */
extern struct TextBox lbl_1_bss_A5C40[4]; /* textbox.c: the message boxes */

/* memcard request (same layout as in src/main_loop/adv.c and src/option/option.c) */
struct MemcardRequest
{
    /*0x00*/ u8 unk0;
    u8 filler1[3];
    /*0x04*/ u32 flags;
    /*0x08*/ char *fileName;
    /*0x0C*/ void *files;
    u8 filler10[8];
}; /* size = 0x18 */

/* main.dol (src/main_loop/unlock.c); sel_ngc.h declares these without a prototype */
int fn_1_16C4F8(void);
int g_check_if_partygame_unlocked(int id);
int get_play_point_count(void);

/* Defined in the other sel_ngc TUs (signatures from the callees' asm). */
void create_gamedata_sprite(void);
void create_main_menu_sprites(void);
void create_practice_mode_stage_select_sprite(void);
void destroy_gamedata_sprite(void);
void fn_3_14998(void);
void fn_3_14A60(void);
int fn_3_1B984(int arg0);
void fn_3_1BBA0(void);
void fn_3_1C514(void);
void fn_3_1CA00(void);
void fn_3_20618(void);
void fn_3_20698(void);
void fn_3_20A28(void);
void fn_3_21D50(void);
void fn_3_2304C(void);
void fn_3_2345C(void);
void fn_3_234DC(void);
void fn_3_23DB8(void);
void fn_3_23DE4(void);
void fn_3_23EA8(void);
void fn_3_242B4(void);
void fn_3_2462C(void);
void fn_3_246E8(void);
void fn_3_2523C(void);
void fn_3_26F40(void);
void fn_3_26FC0(void);
void fn_3_27780(void);
void fn_3_277A4(void);
void fn_3_290CC(void);
void g_blink_selected_menu_item(void);
void g_display_game_settings_sprite(void);
void g_draw_controller_sprites(void);
void g_draw_gift_menu(void);
void g_set_some_option_menu_sprite_param(void);
void g_something_with_menus5(void);
void menu_option_game_data_tick_child(void);

/* This file. The option value handlers take (draw, index): draw != 0 prints the value text,
 * index selects the setting (player / row). Ticks take no arguments. */
void fn_3_1119C(void);
void fn_3_11390(s32 arg0, s32 arg1);
void fn_3_11840(s32 arg0, s32 arg1);
void fn_3_11860(s32 arg0, s32 arg1);
void fn_3_11884(s32 arg0, s32 arg1);
void fn_3_11BB8(s32 arg0, s32 arg1);
void fn_3_11FD4(s32 arg0, s32 arg1);
void fn_3_123E4(s32 arg0, s32 arg1);
void fn_3_12644(s32 arg0, s32 arg1);
void fn_3_12BAC(s32 arg0, s32 arg1);
void fn_3_12BCC(s32 arg0, s32 arg1);
void fn_3_12BF0(s32 arg0, s32 arg1);
void fn_3_12FA8(s32 arg0, s32 arg1);
void fn_3_13370(s32 arg0, s32 arg1);
void fn_3_137D4(s32 arg0, s32 arg1);
void fn_3_13C28(s32 arg0, s32 arg1);
void g_handle_starting_monkeys_count(s32 arg0, s32 arg1);
void fn_3_198(s32 draw, s32 index);
void fn_3_32E0(s32 arg0);
void fn_3_6EC8(void);
void g_something_with_menus(s32 arg0);
void menu_baseball_mode_select_tick(void);
void menu_baseball_stadium_select_tick(void);
void menu_billiards_mode_select_tick(void);
void menu_billiards_rule_select_tick(void);
void menu_boat_course_select_tick(void);
void menu_boat_mode_select_entries_tick(void);
void menu_bowling_level_select_tick(void);
void menu_bowling_mode_select_tick(void);
void menu_bowling_rule_select_tick(void);
void menu_character_select_tick(void);
void menu_dogfight_mode_select_tick(void);
void menu_dogfight_stage_select_tick(void);
void menu_fight_mode_select_tick(void);
void menu_fight_no_of_wins_tick(void);
void menu_fight_stage_select_tick(void);
void menu_gameplay_settings_tick(void);
void menu_golf_mode_select_tick(void);
void menu_level_select_tick(void);
void menu_main_game_select_tick(void);
void menu_mode_select_tick(void);
void menu_number_of_players_tick(void);
void menu_option_controller_tick(void);
void menu_option_game_data_tick(void);
void menu_option_gift_tick(void);
void menu_option_play_points_tick(void);
void menu_option_ranking_tick(void);
void menu_option_replay_tick(void);
void menu_option_screen_tick(void);
void menu_option_tick(void);
void menu_party_game_select_tick(void);
void menu_race_course_select_tick(void);
void menu_race_mode_select_tick(void);
void menu_return_to_title(void);
void menu_shot_stage_select_tick(void);
void menu_soccer_mode_select_tick(void);
void menu_stage_select_tick(void);
void menu_start_baseball(void);
void menu_start_billiards(void);
void menu_start_boat(void);
void menu_start_bowling(void);
void menu_start_challenge_mode(void);
void menu_start_dogfight(void);
void menu_start_fight(void);
void menu_start_golf(void);
void menu_start_practice_mode(void);
void menu_start_race(void);
void menu_start_shot(void);
void menu_start_soccer(void);
void menu_start_story_mode(void);
void menu_start_target(void);
void menu_start_tennis(void);
void menu_tennis_court_select_tick(void);
void menu_tennis_mode_select_tick(void);
void menu_tennis_pair_select_tick(void);

/* ---- .bss (0x190 bytes) ---- */
struct SelNgcMenuWork g_currently_visible_menu_screen;
s32 lbl_3_bss_40;
struct SelNgcBss44 lbl_3_bss_44;
s32 g_active_pads_bitfield;
u8 g_language_id;
s32 locked_menu_items;
struct SelNgcBss58 lbl_3_bss_58;
#ifdef __MWERKS__
u8 lbl_3_bss_68;
#else
/* used as 4 bytes, one per player ((&lbl_3_bss_68)[0..3]); on GC the other
 * three are the padding before lbl_3_bss_6C */
u8 lbl_3_bss_68_4[4];
#define lbl_3_bss_68 (lbl_3_bss_68_4[0])
#endif
s32 lbl_3_bss_6C[3];
char lbl_3_bss_78[0x100];
struct SelNgcBss178 lbl_3_bss_178;

#include "sel_ngc_data.h"

#include "sel_ngc_data.inc"

void fn_3_198(s32 draw, s32 index) {
    switch ((s8) g_currently_visible_menu_screen.currScreen) {
    case 0xB:
        g_handle_starting_monkeys_count(draw, index);
        break;
    case 0x12:
        fn_3_11840(draw, index);
        break;
    case 0x13:
        fn_3_11860(draw, index);
        break;
    case 0x19:
    case 0x1A:
        fn_3_11884(draw, index);
        break;
    case 0x1D:
        fn_3_11BB8(draw, index);
        break;
    case 0x21:
    case 0x22:
        fn_3_11FD4(draw, index);
        break;
    case 0x29:
        fn_3_123E4(draw, index);
        break;
    case 0x2E:
        fn_3_12BAC(draw, index);
        break;
    case 0x2F:
        fn_3_12BCC(draw, index);
        break;
    case 0x33:
    case 0x34:
    case 0x35:
    case 0x36:
        fn_3_12BF0(draw, index);
        break;
    case 0x3A:
    case 0x3B:
        fn_3_12FA8(draw, index);
        break;
    case 0x3F:
    case 0x40:
        fn_3_13370(draw, index);
        break;
    case 0x45:
    case 0x46:
        fn_3_137D4(draw, index);
        break;
    case 0x4D:
    case 0x4E:
        fn_3_13C28(draw, index);
        break;
    default:
        break;
    }
}

void fn_3_260(void) {
    struct Sprite *temp_r3;
    struct Sprite *temp_r3_2;
    struct Sprite *temp_r3_3;
    struct Sprite *temp_r3_4;
    struct Sprite *temp_r3_5;
    struct Sprite *temp_r3_6;

    if ((s32) lbl_3_bss_40 > 0) {
        lbl_3_bss_40 -= 1;
    }
    if (((s32) g_currently_visible_menu_screen.unk34 != 0) && ((g_check_current_track(5) != 0) || (g_check_current_track(0x5A) != 0) || ((s32) lbl_3_bss_40 <= 0))) {
        if ((fn_1_363C8(5) == 1) || (fn_1_363C8(0x5A) == 1)) {
            temp_r3 = get_sprite_with_unique_id(5);
            if (temp_r3 != NULL) {
                temp_r3->userVar = 1;
            }
        } else if ((fn_1_363C8(5) == 3) || (fn_1_363C8(0x5A) == 3)) {
            temp_r3_2 = get_sprite_with_unique_id(6);
            if (temp_r3_2 != NULL) {
                temp_r3_2->userVar = 1;
            }
        } else if ((fn_1_363C8(5) == 5) || (fn_1_363C8(0x5A) == 5)) {
            temp_r3_3 = get_sprite_with_unique_id(7);
            if (temp_r3_3 != NULL) {
                temp_r3_3->userVar = 1;
            }
        } else if ((fn_1_363C8(5) == 9) || (fn_1_363C8(0x5A) == 9) || ((s32) lbl_3_bss_40 == 0)) {
            fade_screen_to_color(0x100, 0U, 0x1E);
            lbl_3_bss_40 = -1;
        }
        if (fn_1_B2FB0() != 0) {
            g_currently_visible_menu_screen.unk34 = 0;
            temp_r3_4 = get_sprite_with_unique_id(5);
            if (temp_r3_4 != NULL) {
                temp_r3_4->userVar = 2;
            }
            temp_r3_5 = get_sprite_with_unique_id(6);
            if (temp_r3_5 != NULL) {
                temp_r3_5->userVar = 2;
            }
            temp_r3_6 = get_sprite_with_unique_id(7);
            if (temp_r3_6 != NULL) {
                temp_r3_6->userVar = 2;
            }
            fn_3_1BBA0();
            fn_3_1CA00();
        }
    }
}

s32 g_b_button_handler_for_sel_ngc(void) {
    s32 var_r31;
    u32 var_r6;

    var_r31 = 0;
    if (((g_is_screen_in_screen_stack(5) == 0) || ((s8) g_currently_visible_menu_screen.currScreen == 5)) && ((g_is_screen_in_screen_stack(6) == 0) || ((s8) g_currently_visible_menu_screen.currScreen == 6)) && ((g_is_screen_in_screen_stack(0x3E) == 0) || ((s8) g_currently_visible_menu_screen.currScreen == 0x3E)) && ((g_is_screen_in_screen_stack(0x43) == 0) || ((s8) g_currently_visible_menu_screen.currScreen == 0x43))) {
        return 0;
    }
    var_r6 = 0;
loop_13:
    if (var_r6 < (u32) modeCtrl.playerCount) {
        if ((s32) player_pad_map[var_r6] != -1) {
            var_r31 |= 1 << player_pad_map[var_r6];
        }
        var_r6 += 1;
        goto loop_13;
    }
    return var_r31;
}

s32 did_any_pad_press_input(u32 arg0) {
    s32 var_r0;
    s32 var_r0_2;
    s32 var_r0_3;
    s32 var_r0_4;
    s32 var_r0_5;
    s32 var_r0_6;
    s32 var_r0_7;
    s32 var_r0_8;
    s32 var_r0_9;
    s32 var_r5;

    var_r5 = 0;
    switch (arg0) {
    case 0:
        var_r0 = 0;
        if ((((u32) g_active_pads_bitfield == 0U) && (((s32) (pad_status_groups->unk0[4].button & 1) != 0) || ((s32) (pad_status_groups[1].unk0[4].button & 1) != 0) || ((s32) (pad_status_groups[2].unk0[4].button & 1) != 0) || ((s32) (pad_status_groups[3].unk0[4].button & 1) != 0) || ((s32) (analog_inputs[0][4] & 1) != 0) || ((s32) (analog_inputs[1][4] & 1) != 0) || ((s32) (analog_inputs[2][4] & 1) != 0) || ((s32) (analog_inputs[3][4] & 1) != 0))) || (((u32) g_active_pads_bitfield != 0U) && ((((u32) (g_active_pads_bitfield & 1) != 0) && (((s32) (pad_status_groups->unk0[4].button & 1) != 0) || ((s32) (analog_inputs[0][4] & 1) != 0))) || (((u32) (g_active_pads_bitfield & 2) != 0) && (((s32) (pad_status_groups[1].unk0[4].button & 1) != 0) || ((s32) (analog_inputs[1][4] & 1) != 0))) || (((u32) (g_active_pads_bitfield & 4) != 0) && (((s32) (pad_status_groups[2].unk0[4].button & 1) != 0) || ((s32) (analog_inputs[2][4] & 1) != 0))) || (((u32) (g_active_pads_bitfield & 8) != 0) && (((s32) (pad_status_groups[3].unk0[4].button & 1) != 0) || ((s32) (analog_inputs[3][4] & 1) != 0)))))) {
            var_r0 = 1;
        }
        var_r5 = var_r0;
        break;
    case 1:
        var_r0_2 = 0;
        if ((((u32) g_active_pads_bitfield == 0U) && (((s32) (pad_status_groups->unk0[4].button & 2) != 0) || ((s32) (pad_status_groups[1].unk0[4].button & 2) != 0) || ((s32) (pad_status_groups[2].unk0[4].button & 2) != 0) || ((s32) (pad_status_groups[3].unk0[4].button & 2) != 0) || ((s32) (analog_inputs[0][4] & 2) != 0) || ((s32) (analog_inputs[1][4] & 2) != 0) || ((s32) (analog_inputs[2][4] & 2) != 0) || ((s32) (analog_inputs[3][4] & 2) != 0))) || (((u32) g_active_pads_bitfield != 0U) && ((((u32) (g_active_pads_bitfield & 1) != 0) && (((s32) (pad_status_groups->unk0[4].button & 2) != 0) || ((s32) (analog_inputs[0][4] & 2) != 0))) || (((u32) (g_active_pads_bitfield & 2) != 0) && (((s32) (pad_status_groups[1].unk0[4].button & 2) != 0) || ((s32) (analog_inputs[1][4] & 2) != 0))) || (((u32) (g_active_pads_bitfield & 4) != 0) && (((s32) (pad_status_groups[2].unk0[4].button & 2) != 0) || ((s32) (analog_inputs[2][4] & 2) != 0))) || (((u32) (g_active_pads_bitfield & 8) != 0) && (((s32) (pad_status_groups[3].unk0[4].button & 2) != 0) || ((s32) (analog_inputs[3][4] & 2) != 0)))))) {
            var_r0_2 = 1;
        }
        var_r5 = var_r0_2;
        break;
    case 2:
        var_r0_3 = 0;
        if ((((u32) g_active_pads_bitfield == 0U) && (((s32) (pad_status_groups->unk0[4].button & 8) != 0) || ((s32) (pad_status_groups[1].unk0[4].button & 8) != 0) || ((s32) (pad_status_groups[2].unk0[4].button & 8) != 0) || ((s32) (pad_status_groups[3].unk0[4].button & 8) != 0) || ((s32) (analog_inputs[0][4] & 8) != 0) || ((s32) (analog_inputs[1][4] & 8) != 0) || ((s32) (analog_inputs[2][4] & 8) != 0) || ((s32) (analog_inputs[3][4] & 8) != 0))) || (((u32) g_active_pads_bitfield != 0U) && ((((u32) (g_active_pads_bitfield & 1) != 0) && (((s32) (pad_status_groups->unk0[4].button & 8) != 0) || ((s32) (analog_inputs[0][4] & 8) != 0))) || (((u32) (g_active_pads_bitfield & 2) != 0) && (((s32) (pad_status_groups[1].unk0[4].button & 8) != 0) || ((s32) (analog_inputs[1][4] & 8) != 0))) || (((u32) (g_active_pads_bitfield & 4) != 0) && (((s32) (pad_status_groups[2].unk0[4].button & 8) != 0) || ((s32) (analog_inputs[2][4] & 8) != 0))) || (((u32) (g_active_pads_bitfield & 8) != 0) && (((s32) (pad_status_groups[3].unk0[4].button & 8) != 0) || ((s32) (analog_inputs[3][4] & 8) != 0)))))) {
            var_r0_3 = 1;
        }
        var_r5 = var_r0_3;
        break;
    case 3:
        var_r0_4 = 0;
        if ((((u32) g_active_pads_bitfield == 0U) && (((s32) (pad_status_groups->unk0[4].button & 4) != 0) || ((s32) (pad_status_groups[1].unk0[4].button & 4) != 0) || ((s32) (pad_status_groups[2].unk0[4].button & 4) != 0) || ((s32) (pad_status_groups[3].unk0[4].button & 4) != 0) || ((s32) (analog_inputs[0][4] & 4) != 0) || ((s32) (analog_inputs[1][4] & 4) != 0) || ((s32) (analog_inputs[2][4] & 4) != 0) || ((s32) (analog_inputs[3][4] & 4) != 0))) || (((u32) g_active_pads_bitfield != 0U) && ((((u32) (g_active_pads_bitfield & 1) != 0) && (((s32) (pad_status_groups->unk0[4].button & 4) != 0) || ((s32) (analog_inputs[0][4] & 4) != 0))) || (((u32) (g_active_pads_bitfield & 2) != 0) && (((s32) (pad_status_groups[1].unk0[4].button & 4) != 0) || ((s32) (analog_inputs[1][4] & 4) != 0))) || (((u32) (g_active_pads_bitfield & 4) != 0) && (((s32) (pad_status_groups[2].unk0[4].button & 4) != 0) || ((s32) (analog_inputs[2][4] & 4) != 0))) || (((u32) (g_active_pads_bitfield & 8) != 0) && (((s32) (pad_status_groups[3].unk0[4].button & 4) != 0) || ((s32) (analog_inputs[3][4] & 4) != 0)))))) {
            var_r0_4 = 1;
        }
        var_r5 = var_r0_4;
        break;
    case 4:
        var_r0_5 = 0;
        if ((((u32) g_active_pads_bitfield == 0U) && (((s32) (pad_status_groups->unk0[2].button & 0x40) != 0) || ((s32) (pad_status_groups[1].unk0[2].button & 0x40) != 0) || ((s32) (pad_status_groups[2].unk0[2].button & 0x40) != 0) || ((s32) (pad_status_groups[3].unk0[2].button & 0x40) != 0))) || (((u32) g_active_pads_bitfield != 0U) && ((((u32) (g_active_pads_bitfield & 1) != 0) && ((s32) (pad_status_groups->unk0[2].button & 0x40) != 0)) || (((u32) (g_active_pads_bitfield & 2) != 0) && ((s32) (pad_status_groups[1].unk0[2].button & 0x40) != 0)) || (((u32) (g_active_pads_bitfield & 4) != 0) && ((s32) (pad_status_groups[2].unk0[2].button & 0x40) != 0)) || (((u32) (g_active_pads_bitfield & 8) != 0) && ((s32) (pad_status_groups[3].unk0[2].button & 0x40) != 0))))) {
            var_r0_5 = 1;
        }
        var_r5 = var_r0_5;
        break;
    case 5:
        var_r0_6 = 0;
        if ((((u32) g_active_pads_bitfield == 0U) && (((s32) (pad_status_groups->unk0[2].button & 0x20) != 0) || ((s32) (pad_status_groups[1].unk0[2].button & 0x20) != 0) || ((s32) (pad_status_groups[2].unk0[2].button & 0x20) != 0) || ((s32) (pad_status_groups[3].unk0[2].button & 0x20) != 0))) || (((u32) g_active_pads_bitfield != 0U) && ((((u32) (g_active_pads_bitfield & 1) != 0) && ((s32) (pad_status_groups->unk0[2].button & 0x20) != 0)) || (((u32) (g_active_pads_bitfield & 2) != 0) && ((s32) (pad_status_groups[1].unk0[2].button & 0x20) != 0)) || (((u32) (g_active_pads_bitfield & 4) != 0) && ((s32) (pad_status_groups[2].unk0[2].button & 0x20) != 0)) || (((u32) (g_active_pads_bitfield & 8) != 0) && ((s32) (pad_status_groups[3].unk0[2].button & 0x20) != 0))))) {
            var_r0_6 = 1;
        }
        var_r5 = var_r0_6;
        break;
    case 6:
        var_r0_7 = 0;
        if ((((u32) g_active_pads_bitfield == 0U) && (((s32) (pad_status_groups->unk0[2].button & 0x100) != 0) || ((s32) (pad_status_groups[1].unk0[2].button & 0x100) != 0) || ((s32) (pad_status_groups[2].unk0[2].button & 0x100) != 0) || ((s32) (pad_status_groups[3].unk0[2].button & 0x100) != 0))) || (((u32) g_active_pads_bitfield != 0U) && ((((u32) (g_active_pads_bitfield & 1) != 0) && ((s32) (pad_status_groups->unk0[2].button & 0x100) != 0)) || (((u32) (g_active_pads_bitfield & 2) != 0) && ((s32) (pad_status_groups[1].unk0[2].button & 0x100) != 0)) || (((u32) (g_active_pads_bitfield & 4) != 0) && ((s32) (pad_status_groups[2].unk0[2].button & 0x100) != 0)) || (((u32) (g_active_pads_bitfield & 8) != 0) && ((s32) (pad_status_groups[3].unk0[2].button & 0x100) != 0))))) {
            var_r0_7 = 1;
        }
        var_r5 = var_r0_7;
        break;
    case 7:
        var_r0_8 = 0;
        if ((((u32) g_active_pads_bitfield == 0U) && (((s32) (pad_status_groups->unk0[2].button & 0x200) != 0) || ((s32) (pad_status_groups[1].unk0[2].button & 0x200) != 0) || ((s32) (pad_status_groups[2].unk0[2].button & 0x200) != 0) || ((s32) (pad_status_groups[3].unk0[2].button & 0x200) != 0))) || (((u32) g_active_pads_bitfield != 0U) && ((((u32) (g_active_pads_bitfield & 1) != 0) && ((s32) (pad_status_groups->unk0[2].button & 0x200) != 0)) || (((u32) (g_active_pads_bitfield & 2) != 0) && ((s32) (pad_status_groups[1].unk0[2].button & 0x200) != 0)) || (((u32) (g_active_pads_bitfield & 4) != 0) && ((s32) (pad_status_groups[2].unk0[2].button & 0x200) != 0)) || (((u32) (g_active_pads_bitfield & 8) != 0) && ((s32) (pad_status_groups[3].unk0[2].button & 0x200) != 0))))) {
            var_r0_8 = 1;
        }
        var_r5 = var_r0_8;
        break;
    case 8:
        var_r0_9 = 0;
        if ((((u32) g_active_pads_bitfield == 0U) && (((s32) (pad_status_groups->unk0[2].button & 0x400) != 0) || ((s32) (pad_status_groups[1].unk0[2].button & 0x400) != 0) || ((s32) (pad_status_groups[2].unk0[2].button & 0x400) != 0) || ((s32) (pad_status_groups[3].unk0[2].button & 0x400) != 0))) || (((u32) g_active_pads_bitfield != 0U) && ((((u32) (g_active_pads_bitfield & 1) != 0) && ((s32) (pad_status_groups->unk0[2].button & 0x400) != 0)) || (((u32) (g_active_pads_bitfield & 2) != 0) && ((s32) (pad_status_groups[1].unk0[2].button & 0x400) != 0)) || (((u32) (g_active_pads_bitfield & 4) != 0) && ((s32) (pad_status_groups[2].unk0[2].button & 0x400) != 0)) || (((u32) (g_active_pads_bitfield & 8) != 0) && ((s32) (pad_status_groups[3].unk0[2].button & 0x400) != 0))))) {
            var_r0_9 = 1;
        }
        var_r5 = var_r0_9;
        break;
    }
    return var_r5;
}

void fn_3_14D8(s32 arg0) {
    struct TextBox tb;

    if ((arg0 != 0) || (lbl_1_bss_A5C40[0].state == 0)) {
        memset(&tb, 0, sizeof(tb));
        tb.flags = 0x74;
        tb.x = 0x21C;
        tb.y = 0x6E;
        tb.rows = 1;
        tb.style = 5;
        tb.textScale = 0.5f;
        if (arg0 == 0) {
            g_display_playpoint_or_gift_message_child(0, 1, &tb);
        } else {
            g_display_playpoint_or_gift_message_child(0, 1, &tb);
        }
        g_some_printf_function_4(0, lbl_3_rodata_74[(s8) g_language_id], get_play_point_count());
    }
}

void fn_3_15F8(void) {
    g_display_playpoint_or_gift_message_child(0, 0x14, NULL);
}

void g_create_main_menu(void) {
    s32 var_r31;

    modeCtrl.submodeTimer = 0;
    modeCtrl.gameType = 0;
    modeCtrl.unk30 = 4;
    g_set_up_debugtext_buffer();
    event_init(0);
    event_init(4);
    event_init(5);
    event_init(0x10);
    event_init(0x11);
    event_init(0x13);
    event_init(0xE);
    event_init(0x14);
    set_camera_view_settings(0, 0.0f, 0.0f, 1.0f, 1.0f);
    set_camera_view_settings(1, 0.0f, 0.0f, 1.0f, 1.0f);
    enable_camera(0);
    enable_camera(1);
    camera_set_state_for_mask(1U, 0x50);
    camera_set_state_for_mask(2U, 0x57);
    g_poolInfo.playerPool.statusList[0] = 2;
    g_poolInfo.playerPool.statusList[1] = 2;
    g_poolInfo.playerPool.statusList[2] = 2;
    g_poolInfo.playerPool.statusList[3] = 2;
    g_currently_visible_menu_screen.unkC = 1;
    g_currently_visible_menu_screen.unk10 = 0;
    g_currently_visible_menu_screen.unk14 = 0;
    g_currently_visible_menu_screen.unk18 = 0;
    locked_menu_items = 0;
    lbl_3_bss_58.unk0 = -1;
    lbl_3_bss_58.unk1 = 0;
    lbl_3_bss_44.unk0 = -1;
    lbl_3_bss_58.unk4 = 0;
    load_bmp_by_id(0x17);
    load_bmp_by_id(0xC);
    load_bmp_by_id(5);
    create_main_menu_sprites();
    fn_1_358E8();
    var_r31 = 0;
loop_4:
    if (var_r31 < 0xC) {
        if (lbl_3_rodata_28[var_r31] >= 0) {
            g_SoftStreamStart_with_some_defaults(lbl_3_rodata_28[var_r31]);
        }
        var_r31 += 1;
        goto loop_4;
    }
    fn_1_12950();
    sub_mode_request = 0x20;
}

void g_initialize_sel_ngc(void) {
    struct TextBox tb;
    s32 var_r3;
    s32 var_r4;
    s32 var_r4_2;
    s32 var_r4_3;
    s32 var_r5;
    s32 var_r5_2;
    s32 var_r5_3;
    s8 var_r0;
    struct MenuScreen *temp_r3;

    if ((u32) (g_some_other_flags & 0xA) == 0) {
        g_load_stage_for_menu_bg(-1, 0);
        g_create_main_menu();
        g_currently_visible_menu_screen.unk34 = 1;
        lbl_3_bss_40 = 0x12C;
        g_currently_visible_menu_screen.currScreen = 0;
        g_currently_visible_menu_screen.unk4 = 0;
        g_currently_visible_menu_screen.unk8 = 0;
        sel_menu_info.menuStackPtr = 0;
        if ((s32) g_focused_root_menu.unk5C == 0) {
            var_r4 = 0;
            if ((u32) (dip_switches & 0x400000) != 0) {
                var_r4 = 4;
            } else {
                var_r5 = 0;
loop_8:
                if (var_r5 < 4) {
                    if ((s8) (u8) pad_status_groups[var_r5].unk0[0].err == 0) {
                        var_r4 += 1;
                    }
                    var_r5 += 1;
                    goto loop_8;
                }
            }
            if ((s32) (var_r4 - 1) < 0) {
                var_r0 = 0;
            } else {
                var_r4_2 = 0;
                if ((u32) (dip_switches & 0x400000) != 0) {
                    var_r4_2 = 4;
                } else {
                    var_r5_2 = 0;
loop_17:
                    if (var_r5_2 < 4) {
                        if ((s8) (u8) pad_status_groups[var_r5_2].unk0[0].err == 0) {
                            var_r4_2 += 1;
                        }
                        var_r5_2 += 1;
                        goto loop_17;
                    }
                }
                if ((s32) (var_r4_2 - 1) > 3) {
                    var_r0 = 3;
                } else {
                    var_r4_3 = 0;
                    if ((u32) (dip_switches & 0x400000) != 0) {
                        var_r4_3 = 4;
                    } else {
                        var_r5_3 = 0;
loop_26:
                        if (var_r5_3 < 4) {
                            if ((s8) (u8) pad_status_groups[var_r5_3].unk0[0].err == 0) {
                                var_r4_3 += 1;
                            }
                            var_r5_3 += 1;
                            goto loop_26;
                        }
                    }
                    var_r0 = var_r4_3 - 1;
                }
            }
            g_focused_root_menu.unk0[4] = var_r0;
            g_focused_root_menu.unk5C = 1;
        }
        temp_r3 = &menu_screen_list[(s8) g_currently_visible_menu_screen.currScreen];
        temp_r3->tick();
        fn_1_36140();
        fn_1_34828();
        if (are_all_party_games_unlocked() != 0) {
            var_r3 = 0x5A;
        } else {
            var_r3 = 5;
        }
        play_track_and_fade_out_other_tracks(var_r3, 0x1E, 0x64);
        if (are_all_party_games_unlocked() == 0) {
            if (lbl_1_bss_A5C40[0].state == 0) {
                memset(&tb, 0, sizeof(tb));
                tb.flags = 0x74;
                tb.x = 0x21C;
                tb.y = 0x6E;
                tb.rows = 1;
                tb.style = 5;
                tb.textScale = 0.5f;
                g_display_playpoint_or_gift_message_child(0, 1, &tb);
                g_some_printf_function_4(0, lbl_3_rodata_74[(s8) g_language_id], get_play_point_count());
            }
        }
        modeCtrl.courseFlags = 0;
        sel_menu_info.unkC &= 0xFFFFFFFE;
    }
}

void g_reinitialize_sel_ngc(void) {
    struct MemcardRequest req;
    char buf[0x20];
    s32 var_r3;
    s32 var_r3_2;
    s32 var_r3_3;
    s32 var_r3_4;
    s32 var_r3_5;
    s32 var_r3_6;
    s32 var_r5;
    s8 var_r30;
    struct MenuScreen *temp_r3;
    u8 *temp_r3_2;

    if ((u32) (g_some_other_flags & 0xA) == 0) {
        g_currently_visible_menu_screen.unk34 = 0;
        fade_screen_to_color(0x102, 0U, 1);
        fade_screen_to_color(0x100, 0U, 0x1E);
        lbl_3_bss_58.unk8 = 0;
        if (((u8) storymode_save_files[795] != 0) && ((s8) g_focused_root_menu.unk0[0] != 2)) {
            g_save_game_data();
            if (fn_1_F0204() != 0) {
                memset(&req, 0, sizeof(req));
                req.unk0 = 0;
                g_sprintf_memcard_error(0, buf);
                req.fileName = buf;
                req.files = storymode_save_files;
                if ((s32) lbl_1_bss_CF3D8 != 0) {
                    req.flags |= 0x20000000;
                }
                g_some_replay_func4(0, (s32) (u8) g_last_used_memcard_slot_2, &req);
                lbl_3_bss_58.unk8 = 1;
            }
        }
        if (((u32) (modeCtrl.courseFlags & 0x10000) != 0) || ((u32) (modeCtrl.courseFlags & 0x8000) != 0)) {
            if ((s8) g_focused_root_menu.unk0[0] == 0) {
                g_load_stage_for_menu_bg(-2, 0);
            } else if ((s8) g_focused_root_menu.unk0[0] == 1) {
                g_load_stage_for_menu_bg((s8) g_focused_root_menu.unk0[1], 0);
            }
        } else {
            g_load_stage_for_menu_bg(-1, 0);
        }
        g_create_main_menu(); /* draft: m2c read stale r3 here */
        if ((s8) (u8) sel_menu_info.menuStackPtr == 0) {
            printf("### ERROR ###\nmode_cnt is 0 at sel_ngc_reinit.");
        }
        g_currently_visible_menu_screen.unk4 = 0;
        g_currently_visible_menu_screen.unk8 = 0;
        if (((u32) (modeCtrl.courseFlags & 0x10000) != 0) || ((u32) (modeCtrl.courseFlags & 0x8000) != 0)) {
            sel_menu_info.menuStackPtr = (u8) sel_menu_info.menuStackPtr - 1;
            if ((s8) g_focused_root_menu.unk0[0] == 0) {
                if (are_all_party_games_unlocked() != 0) {
                    var_r3 = 0x5A;
                } else {
                    var_r3 = 5;
                }
                play_track_and_fade_out_other_tracks(var_r3, 0x1E, 0x64);
            } else {
                if (are_all_party_games_unlocked() != 0) {
                    var_r3_2 = 0x5A;
                } else {
                    var_r3_2 = 5;
                }
                g_another_SoftStreamStart_wrapper_of_some_sort(var_r3_2, 1U);
                var_r30 = (s8) lbl_3_rodata_28[(s8) g_focused_root_menu.unk0[1]];
                if (var_r30 < 0) {
                    var_r30 = 0;
                }
                if (g_maybe_related_to_music_crossfading((s32) var_r30) == -1) {
                    if ((s8) (u8) lbl_3_bss_44.unk0 != -1) {
                        g_something_with_stopping_music_or_sfx((s32) (s8) (u8) lbl_3_bss_44.unk0);
                    }
                    g_SoftStreamStart_with_some_defaults_2((s32) var_r30);
                    lbl_3_bss_44.unk0 = (u8) var_r30;
                } else {
                    g_change_music_volume((s32) var_r30, 0x1EU, 0x64);
                }
            }
            g_currently_visible_menu_screen.currScreen = sel_menu_info.menuStack[(s8) (u8) sel_menu_info.menuStackPtr];
        } else {
            if ((s8) g_focused_root_menu.unk0[0] == 2) {
                fn_1_36140();
                fn_1_34828();
            } else {
                sel_menu_info.menuStackPtr = 1;
            }
            if (are_all_party_games_unlocked() != 0) {
                var_r3_3 = 0x5A;
            } else {
                var_r3_3 = 5;
            }
            if (g_maybe_related_to_music_crossfading(var_r3_3) == -1) {
                if (are_all_party_games_unlocked() != 0) {
                    var_r3_4 = 0x5A;
                } else {
                    var_r3_4 = 5;
                }
                play_track_and_fade_out_other_tracks(var_r3_4, 0x1E, 0x64);
            } else {
                if (are_all_party_games_unlocked() != 0) {
                    var_r3_5 = 0x5A;
                } else {
                    var_r3_5 = 5;
                }
                g_change_music_volume(var_r3_5, 0x1EU, 0x64);
            }
            if ((s8) g_focused_root_menu.unk0[0] == 2) {
                g_currently_visible_menu_screen.currScreen = menu_screen_list[2].entries[(s8) g_focused_root_menu.unk0[2]].next_screen_id;
                if (are_all_party_games_unlocked() != 0) {
                    var_r3_6 = 0x5A;
                } else {
                    var_r3_6 = 5;
                }
                g_change_music_volume(var_r3_6, 0x1EU, 0x3C);
            } else {
                g_currently_visible_menu_screen.currScreen = sel_menu_info.menuStack[(s8) (u8) sel_menu_info.menuStackPtr];
            }
        }
        if (g_is_screen_in_screen_stack(0x42) != 0) {
            if ((s8) lbl_1_bss_172C.unk0 == 2) {
                lbl_1_bss_172C.unk0 = 1;
            } else {
                lbl_1_bss_172C.unk0 = 0;
            }
        }
        temp_r3 = &menu_screen_list[(s8) g_currently_visible_menu_screen.currScreen];
        temp_r3->tick();
        g_currently_visible_menu_screen.unk1D = (s8) (modeCtrl.playerCount - 1);
        g_currently_visible_menu_screen.unk1E = 0U;
        var_r5 = 0;
loop_61:
        if (var_r5 < (s32) (modeCtrl.playerCount - 1)) {
            g_currently_visible_menu_screen.unk1E = (u8) (g_currently_visible_menu_screen.unk1E | (1 << var_r5));
            var_r5 += 1;
            goto loop_61;
        }
        if (((u8) ((u8) g_currently_visible_menu_screen.currScreen - 5) <= 1U) || ((s8) g_currently_visible_menu_screen.currScreen == 0x3E)) {
            g_currently_visible_menu_screen.unk1D = 0;
            g_currently_visible_menu_screen.unk1E = 0U;
        }
        in_practice_mode = 0;
        temp_r3_2 = &g_focused_root_menu.unk0[(s8) g_currently_visible_menu_screen.currScreen];
        lbl_3_bss_58.unk1 = (u8) *temp_r3_2;
        fn_3_1C514();
        fn_3_1BBA0();
        if ((fn_3_1B984(1) == 0) && (g_is_screen_in_screen_stack(2) == 0)) {
            fn_3_1CA00();
        }
        modeCtrl.courseFlags = 0;
    }
}

s32 g_called_when_fading_to_storymode(void) {
    s32 var_r4;
    s8 var_r0;
    struct MenuScreen *temp_r30;
    struct MenuScreen *temp_r4;
    struct MenuScreen *temp_r4_2;
    struct MenuScreen *temp_r4_3;
    u8 temp_r0;

    if ((s8) (u8) g_currently_visible_menu_screen.unk8 == 0) {
        return 0;
    }
    temp_r30 = &menu_screen_list[(s8) g_currently_visible_menu_screen.nextScreen];
    if (((u32) (temp_r30->bitflag & 0x100) != 0) && ((s8) (u8) g_currently_visible_menu_screen.unk8 > 0x3C) && (did_any_pad_press_input(7U) != 0) && ((s32) g_currently_visible_menu_screen.unkC == 0) && ((s32) g_currently_visible_menu_screen.unk18 == 0)) {
        call_SoundReqID_arg_0(0x6B);
        g_currently_visible_menu_screen.unk8 = 0U;
        g_currently_visible_menu_screen.unkC = 0;
        g_currently_visible_menu_screen.unk10 = 0;
        g_currently_visible_menu_screen.unk14 = 0;
        g_currently_visible_menu_screen.unk18 = 1;
        sel_menu_info.menuStackPtr = (u8) sel_menu_info.menuStackPtr - 1;
        return 0;
    }
    if (((u32) (temp_r30->bitflag & 0x100) != 0) && ((s8) (u8) g_currently_visible_menu_screen.unk8 == 0x3C)) {
        var_r4 = 0;
        if (((s8) g_focused_root_menu.unk0[0] == 0) && ((s8) g_focused_root_menu.unk0[7] == 0)) {
            var_r4 = 1;
        }
        if (var_r4 != 0) {
            var_r0 = 1;
        } else {
            var_r0 = 0;
        }
        fade_screen_to_color(0x101, 0U, (s32) (s8) (u8) g_currently_visible_menu_screen.unk8);
        g_change_music_volume(-1, (u32) (s8) (u8) g_currently_visible_menu_screen.unk8, var_r0);
    }
    temp_r0 = (u8) g_currently_visible_menu_screen.unk8 - 1;
    g_currently_visible_menu_screen.unk8 = temp_r0;
    if ((s8) temp_r0 > 0) {
        return 1;
    }
    if ((s32) g_currently_visible_menu_screen.unk4 != 0) {
        sel_menu_info.menuStackPtr = (u8) sel_menu_info.menuStackPtr - 1;
    }
    if ((s32) g_currently_visible_menu_screen.unk4 != 0) {
        temp_r4 = &menu_screen_list[(s8) g_currently_visible_menu_screen.nextScreen];
        if ((u32) (temp_r4->bitflag & 1) != 0) {
            g_currently_visible_menu_screen.unk14 = 1;
            if ((temp_r4->tick) != NULL) {
                temp_r4->tick();
            }
            g_currently_visible_menu_screen.unk14 = 0;
            return 1;
        }
        temp_r4_2 = &menu_screen_list[(s8) g_currently_visible_menu_screen.currScreen];
        if ((u32) (temp_r4_2->bitflag & 0x20000) != 0) {
            g_currently_visible_menu_screen.unk14 = 1;
            if ((temp_r4_2->tick) != NULL) {
                temp_r4_2->tick();
            }
            g_currently_visible_menu_screen.unk14 = 0;
        }
        goto block_34;
    }
    temp_r4_3 = &menu_screen_list[(s8) g_currently_visible_menu_screen.currScreen];
    if ((u32) (temp_r4_3->bitflag & 2) != 0) {
        g_currently_visible_menu_screen.unk14 = 1;
        if ((temp_r4_3->tick) != NULL) {
            temp_r4_3->tick();
        }
        g_currently_visible_menu_screen.unk14 = 0;
    }
block_34:
    if ((s32) g_currently_visible_menu_screen.unk4 != 0) {
        if (((u8) ((u8) g_currently_visible_menu_screen.currScreen - 5) <= 1U) || ((s8) g_currently_visible_menu_screen.currScreen == 0x3E) || ((s8) g_currently_visible_menu_screen.currScreen == 0x43)) {
            fn_3_6EC8();
        } else {
            g_focused_root_menu.unk0[(s8) g_currently_visible_menu_screen.currScreen] = (u8) lbl_3_bss_58.unk1;
        }
    }
    g_currently_visible_menu_screen.currScreen = (u8) g_currently_visible_menu_screen.nextScreen;
    if ((s32) g_currently_visible_menu_screen.unk4 == 0) {
        g_currently_visible_menu_screen.unkC = 1;
    } else {
        g_currently_visible_menu_screen.unk18 = 1;
    }
    g_currently_visible_menu_screen.unk4 = 0;
    if ((s8) g_currently_visible_menu_screen.unk30 > 0) {
        if ((s8) g_currently_visible_menu_screen.unk30 > 1) {
            g_currently_visible_menu_screen.unk30 = (u8) -0x3C;
        } else if ((s8) g_currently_visible_menu_screen.unk30 >= 0) {
            g_display_playpoint_or_gift_message_child(1, 0x14, NULL);
            g_currently_visible_menu_screen.unk30 = 0U;
        }
    }
    locked_menu_items = 0;
    lbl_3_bss_58.unk0 = -1;
    lbl_3_bss_58.unk1 = (u8) g_focused_root_menu.unk0[(s8) g_currently_visible_menu_screen.currScreen];
    g_currently_visible_menu_screen.unk38 = 0;
    return 0;
}

void g_sel_ngc_item_selected(s32 arg0) {
    s8 temp_r28_2;
    s8 temp_r29;
    s8 temp_r30;
    s8 temp_r31;
    struct MenuScreen *temp_r28;
    struct MenuScreen *temp_r4;
    struct MenuScreen *temp_r4_2;
    struct MenuScreen *temp_r4_3;
    u8 temp_r5;
    u8 temp_r5_2;
    u8 temp_r5_3;

    temp_r28 = &menu_screen_list[(s8) g_currently_visible_menu_screen.currScreen];
    if ((s8) (u8) sel_menu_info.menuStackPtr == 0xA) {
        printf("sel_mkb2_mem2.mode_cnt >= SEL_MKB2_MODE_STK_NUM\n");
        return;
    }
    if (arg0 != 0) {
        call_SoundReqID_arg_0(0x6A);
    }
    g_currently_visible_menu_screen.unk10 = 1;
    g_currently_visible_menu_screen.nextScreen = (u8) temp_r28->entries[(s8) g_focused_root_menu.unk0[(s8) g_currently_visible_menu_screen.currScreen]].next_screen_id;
    if ((u32) (menu_screen_list[(s8) g_currently_visible_menu_screen.nextScreen].bitflag & 0x100) != 0) {
        g_currently_visible_menu_screen.unk8 = 0x78;
    } else {
        g_currently_visible_menu_screen.unk8 = 0x3C;
    }
    temp_r5 = (u8) g_currently_visible_menu_screen.currScreen;
    sel_menu_info.menuStack[(s8) (u8) sel_menu_info.menuStackPtr] = temp_r5;
    sel_menu_info.menuStackPtr = (u8) sel_menu_info.menuStackPtr + 1;
    if ((u32) (menu_screen_list[(s8) g_currently_visible_menu_screen.nextScreen].bitflag & 0x200) != 0) {
        temp_r29 = (s8) g_currently_visible_menu_screen.currScreen;
        g_currently_visible_menu_screen.currScreen = (u8) g_currently_visible_menu_screen.nextScreen;
        temp_r4 = &menu_screen_list[(s8) g_currently_visible_menu_screen.currScreen];
        if ((s8) (u8) sel_menu_info.menuStackPtr == 0xA) {
            printf("sel_mkb2_mem2.mode_cnt >= SEL_MKB2_MODE_STK_NUM\n");
        } else {
            g_currently_visible_menu_screen.unk10 = 1;
            g_currently_visible_menu_screen.nextScreen = (u8) temp_r4->entries[(s8) g_focused_root_menu.unk0[(s8) g_currently_visible_menu_screen.currScreen]].next_screen_id;
            if ((u32) (menu_screen_list[(s8) g_currently_visible_menu_screen.nextScreen].bitflag & 0x100) != 0) {
                g_currently_visible_menu_screen.unk8 = 0x78;
            } else {
                g_currently_visible_menu_screen.unk8 = 0x3C;
            }
            temp_r5_2 = (u8) g_currently_visible_menu_screen.currScreen;
            sel_menu_info.menuStack[(s8) (u8) sel_menu_info.menuStackPtr] = temp_r5_2;
            sel_menu_info.menuStackPtr = (u8) sel_menu_info.menuStackPtr + 1;
            if ((u32) (menu_screen_list[(s8) g_currently_visible_menu_screen.nextScreen].bitflag & 0x200) != 0) {
                temp_r31 = (s8) g_currently_visible_menu_screen.currScreen;
                g_currently_visible_menu_screen.currScreen = (u8) g_currently_visible_menu_screen.nextScreen;
                temp_r4_2 = &menu_screen_list[(s8) g_currently_visible_menu_screen.currScreen];
                if ((s8) (u8) sel_menu_info.menuStackPtr == 0xA) {
                    printf("sel_mkb2_mem2.mode_cnt >= SEL_MKB2_MODE_STK_NUM\n");
                } else {
                    g_currently_visible_menu_screen.unk10 = 1;
                    g_currently_visible_menu_screen.nextScreen = (u8) temp_r4_2->entries[(s8) g_focused_root_menu.unk0[(s8) g_currently_visible_menu_screen.currScreen]].next_screen_id;
                    if ((u32) (menu_screen_list[(s8) g_currently_visible_menu_screen.nextScreen].bitflag & 0x100) != 0) {
                        g_currently_visible_menu_screen.unk8 = 0x78;
                    } else {
                        g_currently_visible_menu_screen.unk8 = 0x3C;
                    }
                    temp_r5_3 = (u8) g_currently_visible_menu_screen.currScreen;
                    sel_menu_info.menuStack[(s8) (u8) sel_menu_info.menuStackPtr] = temp_r5_3;
                    sel_menu_info.menuStackPtr = (u8) sel_menu_info.menuStackPtr + 1;
                    if ((u32) (menu_screen_list[(s8) g_currently_visible_menu_screen.nextScreen].bitflag & 0x200) != 0) {
                        temp_r30 = (s8) g_currently_visible_menu_screen.currScreen;
                        g_currently_visible_menu_screen.currScreen = (u8) g_currently_visible_menu_screen.nextScreen;
                        temp_r4_3 = &menu_screen_list[(s8) g_currently_visible_menu_screen.currScreen];
                        if ((s8) (u8) sel_menu_info.menuStackPtr == 0xA) {
                            printf("sel_mkb2_mem2.mode_cnt >= SEL_MKB2_MODE_STK_NUM\n");
                        } else {
                            g_currently_visible_menu_screen.unk10 = 1;
                            g_currently_visible_menu_screen.nextScreen = (u8) temp_r4_3->entries[(s8) g_focused_root_menu.unk0[(s8) g_currently_visible_menu_screen.currScreen]].next_screen_id;
                            if ((u32) (menu_screen_list[(s8) g_currently_visible_menu_screen.nextScreen].bitflag & 0x100) != 0) {
                                g_currently_visible_menu_screen.unk8 = 0x78;
                            } else {
                                g_currently_visible_menu_screen.unk8 = 0x3C;
                            }
                            sel_menu_info.menuStack[(s8) (u8) sel_menu_info.menuStackPtr] = (u8) g_currently_visible_menu_screen.currScreen;
                            sel_menu_info.menuStackPtr = (u8) sel_menu_info.menuStackPtr + 1;
                            if ((u32) (menu_screen_list[(s8) g_currently_visible_menu_screen.nextScreen].bitflag & 0x200) != 0) {
                                temp_r28_2 = (s8) g_currently_visible_menu_screen.currScreen;
                                g_currently_visible_menu_screen.currScreen = (u8) g_currently_visible_menu_screen.nextScreen;
                                g_sel_ngc_item_selected(0);
                                g_currently_visible_menu_screen.currScreen = (u8) temp_r28_2;
                            }
                            g_blink_selected_menu_item();
                        }
                        g_currently_visible_menu_screen.currScreen = (u8) temp_r30;
                    }
                    g_blink_selected_menu_item();
                }
                g_currently_visible_menu_screen.currScreen = (u8) temp_r31;
            }
            g_blink_selected_menu_item();
        }
        g_currently_visible_menu_screen.currScreen = (u8) temp_r29;
    }
    g_blink_selected_menu_item();
}

void g_b_button_pressed_on_menu(s32 arg0) {
    s8 temp_r28;
    s8 temp_r29;
    s8 temp_r30;
    s8 temp_r31;

    if (arg0 != 0) {
        call_SoundReqID_arg_0(0x6B);
    }
    if ((s8) (u8) sel_menu_info.menuStackPtr == 0) {
        g_currently_visible_menu_screen.unk4 = 1;
        g_currently_visible_menu_screen.nextScreen = 3U;
        g_currently_visible_menu_screen.unk8 = 0x1EU;
        fade_screen_to_color(0x101, 0U, (s32) (s8) (u8) g_currently_visible_menu_screen.unk8);
        g_change_music_volume(-1, (u32) (s8) (u8) g_currently_visible_menu_screen.unk8, 0);
        return;
    }
    g_currently_visible_menu_screen.unk4 = 1;
    g_currently_visible_menu_screen.nextScreen = sel_menu_info.menuStack[(s8) (u8) sel_menu_info.menuStackPtr - 1];
    g_currently_visible_menu_screen.unk8 = 0xFU;
    if ((u32) (menu_screen_list[(s8) g_currently_visible_menu_screen.nextScreen].bitflag & 0x200) != 0) {
        temp_r29 = (s8) g_currently_visible_menu_screen.currScreen;
        g_currently_visible_menu_screen.currScreen = (u8) g_currently_visible_menu_screen.nextScreen;
        sel_menu_info.menuStackPtr = (u8) sel_menu_info.menuStackPtr - 1;
        if ((s8) (u8) sel_menu_info.menuStackPtr == 0) {
            g_currently_visible_menu_screen.unk4 = 1;
            g_currently_visible_menu_screen.nextScreen = 3U;
            g_currently_visible_menu_screen.unk8 = 0x1EU;
            fade_screen_to_color(0x101, 0U, (s32) (s8) (u8) g_currently_visible_menu_screen.unk8);
            g_change_music_volume(-1, (u32) (s8) (u8) g_currently_visible_menu_screen.unk8, 0);
        } else {
            g_currently_visible_menu_screen.unk4 = 1;
            g_currently_visible_menu_screen.nextScreen = sel_menu_info.menuStack[(s8) (u8) sel_menu_info.menuStackPtr - 1];
            g_currently_visible_menu_screen.unk8 = 0xFU;
            if ((u32) (menu_screen_list[(s8) g_currently_visible_menu_screen.nextScreen].bitflag & 0x200) != 0) {
                temp_r31 = (s8) g_currently_visible_menu_screen.currScreen;
                g_currently_visible_menu_screen.currScreen = (u8) g_currently_visible_menu_screen.nextScreen;
                sel_menu_info.menuStackPtr = (u8) sel_menu_info.menuStackPtr - 1;
                if ((s8) (u8) sel_menu_info.menuStackPtr == 0) {
                    g_currently_visible_menu_screen.unk4 = 1;
                    g_currently_visible_menu_screen.nextScreen = 3U;
                    g_currently_visible_menu_screen.unk8 = 0x1EU;
                    fade_screen_to_color(0x101, 0U, (s32) (s8) (u8) g_currently_visible_menu_screen.unk8);
                    g_change_music_volume(-1, (u32) (s8) (u8) g_currently_visible_menu_screen.unk8, 0);
                } else {
                    g_currently_visible_menu_screen.unk4 = 1;
                    g_currently_visible_menu_screen.nextScreen = sel_menu_info.menuStack[(s8) (u8) sel_menu_info.menuStackPtr - 1];
                    g_currently_visible_menu_screen.unk8 = 0xFU;
                    if ((u32) (menu_screen_list[(s8) g_currently_visible_menu_screen.nextScreen].bitflag & 0x200) != 0) {
                        temp_r30 = (s8) g_currently_visible_menu_screen.currScreen;
                        g_currently_visible_menu_screen.currScreen = (u8) g_currently_visible_menu_screen.nextScreen;
                        sel_menu_info.menuStackPtr = (u8) sel_menu_info.menuStackPtr - 1;
                        if ((s8) (u8) sel_menu_info.menuStackPtr == 0) {
                            g_currently_visible_menu_screen.unk4 = 1;
                            g_currently_visible_menu_screen.nextScreen = 3U;
                            g_currently_visible_menu_screen.unk8 = 0x1EU;
                            fade_screen_to_color(0x101, 0U, (s32) (s8) (u8) g_currently_visible_menu_screen.unk8);
                            g_change_music_volume(-1, (u32) (s8) (u8) g_currently_visible_menu_screen.unk8, 0);
                        } else {
                            g_currently_visible_menu_screen.unk4 = 1;
                            g_currently_visible_menu_screen.nextScreen = sel_menu_info.menuStack[(s8) (u8) sel_menu_info.menuStackPtr - 1];
                            g_currently_visible_menu_screen.unk8 = 0xFU;
                            if ((u32) (menu_screen_list[(s8) g_currently_visible_menu_screen.nextScreen].bitflag & 0x200) != 0) {
                                temp_r28 = (s8) g_currently_visible_menu_screen.currScreen;
                                g_currently_visible_menu_screen.currScreen = (u8) g_currently_visible_menu_screen.nextScreen;
                                sel_menu_info.menuStackPtr = (u8) sel_menu_info.menuStackPtr - 1;
                                g_b_button_pressed_on_menu(0);
                                g_currently_visible_menu_screen.currScreen = (u8) temp_r28;
                            } else {
                                g_something_with_menus5();
                            }
                        }
                        g_currently_visible_menu_screen.currScreen = (u8) temp_r30;
                    } else {
                        g_something_with_menus5();
                    }
                }
                g_currently_visible_menu_screen.currScreen = (u8) temp_r31;
            } else {
                g_something_with_menus5();
            }
        }
        g_currently_visible_menu_screen.currScreen = (u8) temp_r29;
        return;
    }
    g_something_with_menus5();
}

#ifndef __MWERKS__
int pc_party_game_available(int gameType);    /* pc_port pc_party_avail.c */
const char *pc_party_unavail_desc(int canOpen);

/* PC: entry <entry> of the current screen is a party game the port can't run */
static int pc_sel_ngc_entry_unavailable(s8 entry) {
    return (s8) g_currently_visible_menu_screen.currScreen == MENUSCREEN_PARTY_GAME_SELECT
        && entry >= 0 && entry < 12 && !pc_party_game_available(lbl_3_rodata_4[entry]);
}
#endif

void fn_3_32E0(s32 arg0) {
    s32 temp_r25;
    s32 temp_r25_2;
    s32 temp_r3;
    s32 temp_r3_2;
    s32 var_r25;
    s32 var_r30;
    s32 var_r4;
    s32 var_r4_2;
    s32 var_r5;
    s32 var_r5_2;
    s32 var_r6;
    s32 var_r6_2;
    struct MenuScreen *temp_r26;
    s8 *temp_r31; /* cursor of the current screen */
    s8 prevCursor;
    u8 var_r0;

    temp_r26 = &menu_screen_list[(s8) g_currently_visible_menu_screen.currScreen];
    temp_r31 = (s8 *) &g_focused_root_menu.unk0[(s8) g_currently_visible_menu_screen.currScreen];
    if ((u32) (temp_r26->bitflag & 0x10) != 0) {
        var_r30 = 6;
    } else if ((u32) (temp_r26->bitflag & 0x20) != 0) {
        var_r30 = 3;
    } else if ((u32) (temp_r26->bitflag & 0x40) != 0) {
        var_r30 = 1;
    } else {
        var_r30 = 4;
    }
    if ((s8) (u8) lbl_3_bss_58.unk0 != -1) {
        var_r0 = (u8) lbl_3_bss_58.unk0;
    } else {
        var_r0 = temp_r26->entry_count;
    }
    var_r25 = 1;
    if (((s8) g_currently_visible_menu_screen.currScreen == 1) && (g_check_if_partygame_unlocked(lbl_3_rodata_4[(s8) g_focused_root_menu.unk0[1]]) == 0) && (is_able_to_unlock_party_game() != 0)) {
        var_r25 = 0;
    }
    if ((did_any_pad_press_input(6U) != 0) && ((s32) g_currently_visible_menu_screen.unkC == 0) && ((s32) g_currently_visible_menu_screen.unk18 == 0) && ((u32) (temp_r26->bitflag & 8) == 0)) {
#ifndef __MWERKS__
        /* PC: a party game that isn't decompiled yet can't start: buzz (a
         * locked one still takes the unlock path below) */
        if (pc_sel_ngc_entry_unavailable(*temp_r31) && ((u32) (locked_menu_items & (1 << (s8) *temp_r31)) == 0)) {
            call_SoundReqID_arg_0(0x64);
        } else
#endif
        if ((u32) (locked_menu_items & (1 << (s8) *temp_r31)) != 0) {
            if (var_r25 != 0) {
                call_SoundReqID_arg_0(0x64);
            }
        } else {
            g_sel_ngc_item_selected(1);
        }
    } else {
        if ((did_any_pad_press_input(7U) != 0) && ((s32) g_currently_visible_menu_screen.unkC == 0) && ((s32) g_currently_visible_menu_screen.unk18 == 0)) {
            g_b_button_pressed_on_menu(1);
            return;
        }
        if ((did_any_pad_press_input(1U) != 0) || (did_any_pad_press_input(0U) != 0)) {
            temp_r25 = temp_r26->bitflag & 0x80;
            temp_r3 = did_any_pad_press_input(1U);
            prevCursor = *temp_r31;
            if (temp_r25 != 0) {
                var_r5 = (s32) ((s8) var_r0 + 1) / var_r30;
            } else {
                var_r5 = 1;
            }
            if (temp_r25 != 0) {
                var_r6 = (s8) var_r0 + 1;
            } else if ((s32) ((s8) var_r0 + 1) < var_r30) {
                var_r6 = (s8) var_r0 + 1;
            } else {
                var_r6 = var_r30;
            }
            if (temp_r25 != 0) {
                var_r4 = 0;
            } else {
                var_r4 = var_r30 * ((s8) *temp_r31 / var_r30);
            }
            if (temp_r3 != 0) {
                *temp_r31 = (s8) (*temp_r31 + var_r5);
                *temp_r31 = (s8) (*temp_r31 % var_r6);
                *temp_r31 = (s8) (*temp_r31 + var_r4);
            } else {
                *temp_r31 = (s8) (*temp_r31 - var_r5);
                if ((s8) *temp_r31 < 0) {
                    *temp_r31 = (s8) (*temp_r31 + var_r6);
                }
                *temp_r31 = (s8) (*temp_r31 % var_r6);
                *temp_r31 = (s8) (*temp_r31 + var_r4);
            }
            if ((arg0 != 0) && (prevCursor != *temp_r31)) {
                call_SoundReqID_arg_0(0x6C);
            }
        } else if (((s32) ((s8) var_r0 / var_r30) > 0) && ((did_any_pad_press_input(3U) != 0) || (did_any_pad_press_input(2U) != 0))) {
            temp_r25_2 = temp_r26->bitflag & 0x80;
            temp_r3_2 = did_any_pad_press_input(3U);
            prevCursor = *temp_r31;
            if (temp_r25_2 != 0) {
                var_r5_2 = 1;
            } else {
                var_r5_2 = var_r30;
            }
            if (temp_r25_2 != 0) {
                var_r6_2 = (s32) ((s8) var_r0 + 1) / var_r30;
            } else {
                var_r6_2 = (s8) var_r0 + 1;
            }
            if (temp_r25_2 != 0) {
                var_r4_2 = var_r6_2 * ((s8) *temp_r31 / var_r6_2);
            } else {
                var_r4_2 = 0;
            }
            if (temp_r3_2 != 0) {
                *temp_r31 = (s8) (*temp_r31 + var_r5_2);
                *temp_r31 = (s8) (*temp_r31 % var_r6_2);
                *temp_r31 = (s8) (*temp_r31 + var_r4_2);
            } else {
                *temp_r31 = (s8) (*temp_r31 - var_r5_2);
                if ((s8) *temp_r31 < 0) {
                    *temp_r31 = (s8) (*temp_r31 + var_r6_2);
                }
                *temp_r31 = (s8) (*temp_r31 % var_r6_2);
                *temp_r31 = (s8) (*temp_r31 + var_r4_2);
            }
            if ((arg0 != 0) && (prevCursor != *temp_r31)) {
                call_SoundReqID_arg_0(0x6C);
            }
        }
    }
}

s32 g_is_screen_in_screen_stack(s8 arg0) {
    s32 var_r5;

    var_r5 = (s8) (u8) sel_menu_info.menuStackPtr - 1;
loop_4:
    if (var_r5 < 0) {
        return 0;
    }
    if ((s8) sel_menu_info.menuStack[var_r5] == arg0) {
        return 1;
    }
    var_r5 -= 1;
    goto loop_4;
}

#ifndef __MWERKS__
/* PC port: on Mode Select, X (or Esc) opens the "Quit to Desktop?" yes/no
 * box (pc_quit.c), shown in the free first row of the button prompts as
 * "X:Quit" (sel_ngc_sprite.c); B still goes back to the title screen.
 * TRUE while the box handles the input (the menu's own input is skipped). */
int pc_menu_quit_tick(int where, int can_open, unsigned int open_buttons);

static int pc_mode_select_quit_input(void) {
    int canOpen = (s8) g_currently_visible_menu_screen.currScreen == MENUSCREEN_MODE_SELECT
        && (s8) (u8) sel_menu_info.menuStackPtr == 0
        && (s32) g_currently_visible_menu_screen.unk4 == 0
        && (s8) (u8) g_currently_visible_menu_screen.unk8 == 0
        && (s32) g_currently_visible_menu_screen.unk34 == 0
        && (s32) g_currently_visible_menu_screen.unk14 == 0
        && (s8) g_currently_visible_menu_screen.nextScreen == MENUSCREEN_MODE_SELECT;

    switch (pc_menu_quit_tick(2 /* PCMQ_MODE_SELECT */, canOpen, 0x400 /* X */)) {
    case -1:
        return 0;
    case 1: /* PCQ_MOVED */
        call_SoundReqID_arg_0(0x6C);
        break;
    case 2: /* PCQ_NO */
        call_SoundReqID_arg_0(0x66);
        break;
    case 3: /* PCQ_YES */
    case 4: /* PCQ_OPENED */
        call_SoundReqID_arg_0(0x65);
        break;
    }
    return 1;
}
#endif

void menu_tick(void) {
    struct Light spC;
    s32 var_r0;
    s32 var_r0_2;
    s32 var_r0_3;
    s32 var_r29;
    s32 var_r30;
    s32 var_r3;
    s32 var_r3_2;
    s32 var_r3_3;
    s32 var_r3_4;
    s32 var_r4;
    s32 var_r4_2;
    s32 var_r4_3;
    s32 var_r4_4;
    struct MenuScreen *temp_r29;
    struct MenuScreen *temp_r30;
    struct MenuScreen *temp_r3_2;
    struct Sprite *temp_r3;
    u32 var_r6;
    u8 temp_r0;

    g_language_id = (s8) locale;
    if (((u32) (g_some_other_flags & 0xA) == 0) && (g_check_some_memcard_field() == 0)) {
        if ((s32) lbl_3_bss_58.unk8 != 0) {
            if (g_get_some_memcard_var() != 0) {
                smd_game_over_save_child();
            }
            lbl_3_bss_58.unk8 = 0;
        }
        var_r0 = 0;
        var_r4 = (s8) (u8) sel_menu_info.menuStackPtr - 1;
loop_10:
        if (var_r4 < 0) {
            var_r3 = 0;
        } else if ((s8) sel_menu_info.menuStack[var_r4] == 5) {
            var_r3 = 1;
        } else {
            var_r4 -= 1;
            goto loop_10;
        }
        if ((var_r3 == 0) || ((s8) g_currently_visible_menu_screen.currScreen == 5)) {
            var_r4_2 = (s8) (u8) sel_menu_info.menuStackPtr - 1;
loop_18:
            if (var_r4_2 < 0) {
                var_r3_2 = 0;
            } else if ((s8) sel_menu_info.menuStack[var_r4_2] == 6) {
                var_r3_2 = 1;
            } else {
                var_r4_2 -= 1;
                goto loop_18;
            }
            if ((var_r3_2 == 0) || ((s8) g_currently_visible_menu_screen.currScreen == 6)) {
                var_r4_3 = (s8) (u8) sel_menu_info.menuStackPtr - 1;
loop_26:
                if (var_r4_3 < 0) {
                    var_r3_3 = 0;
                } else if ((s8) sel_menu_info.menuStack[var_r4_3] == 0x3E) {
                    var_r3_3 = 1;
                } else {
                    var_r4_3 -= 1;
                    goto loop_26;
                }
                if ((var_r3_3 == 0) || ((s8) g_currently_visible_menu_screen.currScreen == 0x3E)) {
                    var_r4_4 = (s8) (u8) sel_menu_info.menuStackPtr - 1;
loop_34:
                    if (var_r4_4 < 0) {
                        var_r3_4 = 0;
                    } else if ((s8) sel_menu_info.menuStack[var_r4_4] == 0x43) {
                        var_r3_4 = 1;
                    } else {
                        var_r4_4 -= 1;
                        goto loop_34;
                    }
                    if ((var_r3_4 == 0) || ((s8) g_currently_visible_menu_screen.currScreen == 0x43)) {
                        var_r0 = 0;
                    } else {
                        goto block_39;
                    }
                } else {
                    goto block_39;
                }
            } else {
                goto block_39;
            }
        } else {
block_39:
            var_r6 = 0;
loop_43:
            if (var_r6 < (u32) modeCtrl.playerCount) {
                if ((s32) player_pad_map[var_r6] != -1) {
                    var_r0 |= 1 << player_pad_map[var_r6];
                }
                var_r6 += 1;
                goto loop_43;
            }
        }
        g_active_pads_bitfield = var_r0;
        fn_3_260();
        if ((s32) g_currently_visible_menu_screen.unk34 != 0) {
            g_active_pads_bitfield = 0x10;
        }
        temp_r30 = &menu_screen_list[(s8) g_currently_visible_menu_screen.nextScreen];
        var_r29 = 0;
loop_56:
        if (var_r29 >= (s8) temp_r30->entry_count) {
            var_r0_2 = 0;
        } else {
            temp_r3 = get_sprite_with_unique_id(var_r29 + 0x3C);
            if ((temp_r3 != NULL) && ((s32) temp_r3->userVar != 0) && ((s32) temp_r3->userVar != 2) && ((s32) temp_r3->userVar != 0xB) && ((s32) temp_r3->userVar != 0x15) && ((s32) temp_r3->userVar != 0x1F) && ((s32) temp_r3->userVar != 0x29)) {
                var_r0_2 = 1;
            } else {
                var_r29 += 1;
                goto loop_56;
            }
        }
        if (var_r0_2 != 0) {
            var_r0_3 = 1;
        } else {
            var_r0_3 = 0;
        }
        lbl_3_bss_44.unk4 = var_r0_3;
        if (((s16) current_stage_id != (s16) sel_menu_info.unkE) && (disc_queue_distance(3) <= 0) && ((s8) (u8) g_currently_visible_menu_screen.unk8 == 0) && ((s32) lbl_3_bss_44.unk4 == 0)) {
            event_dest(4);
            event_dest(5);
            event_dest(0xE);
            event_dest(0x14);
            fn_1_73900((u32) sel_menu_info.unkE);
            load_stage((s32) sel_menu_info.unkE);
            cameras->timerCurr = 1;
            event_init(4);
            event_init(5);
            event_init(0xE);
            event_init(0x14);
            memset(&spC, 0, 0x4CU);
            spC.id = 8;
            spC.inst = 0;
            spC.type = 0;
            spC.red = 1.0f;
            spC.green = 1.0f;
            spC.blue = 1.0f;
            spC.rotX = 0x2000;
            spC.rotY = 0x6000;
            add_light_to_pool(&spC);
        }
        if ((s8) g_currently_visible_menu_screen.unk30 > 0) {
            if ((s8) g_currently_visible_menu_screen.unk30 > 1) {
                g_currently_visible_menu_screen.unk30 = (u8) (g_currently_visible_menu_screen.unk30 - 1);
            }
        } else if ((s8) g_currently_visible_menu_screen.unk30 < 0) {
            temp_r0 = g_currently_visible_menu_screen.unk30 + 1;
            g_currently_visible_menu_screen.unk30 = temp_r0;
            if ((s8) temp_r0 == -0x1E) {
                g_display_playpoint_or_gift_message_child(1, 0x14, NULL);
            }
        }
        fn_3_1CB58();
        var_r30 = 0;
loop_89:
        if (var_r30 < 4) {
            if ((s32) g_currently_visible_menu_screen.unk20[var_r30] > 0) {
                g_currently_visible_menu_screen.unk20[var_r30] -= 1;
                if ((s32) (g_currently_visible_menu_screen.unk20[var_r30] % 60) == 0x2C) {
                    g_smth_with_sound = (u32) var_r30;
                    g_player_id_for_sound = 0;
                    if ((s8) g_currently_visible_menu_screen.currScreen == 0x3E) {
                        g_smth_with_sound = (u32) (s8) lbl_3_rodata_94[var_r30];
                    }
                    switch (var_r30) {              /* irregular */
                    case 0:
                        call_SoundReqID_arg_0(0x0100004F);
                        break;
                    case 1:
                        call_SoundReqID_arg_0(0x01D8004F);
                        break;
                    case 2:
                        call_SoundReqID_arg_0(0x28004F);
                        break;
                    case 3:
                        call_SoundReqID_arg_0(0xFC004F);
                        break;
                    }
                }
            }
            var_r30 += 1;
            goto loop_89;
        }
        g_currently_visible_menu_screen.unkC = 0;
        g_currently_visible_menu_screen.unk18 = 0;
        g_currently_visible_menu_screen.unk10 = 0;
        if (get_sprite_with_unique_id(0xA) == NULL) {
            if (g_called_when_fading_to_storymode() == 0) {
                temp_r29 = &menu_screen_list[(s8) g_currently_visible_menu_screen.currScreen];
#ifndef __MWERKS__
                if (pc_mode_select_quit_input() == 0)
#endif
                if ((u32) (temp_r29->bitflag & 4) == 0) {
                    fn_3_32E0(1);
                }
                if (((temp_r29->tick != NULL) && ((s32) g_currently_visible_menu_screen.unk4 == 0)) || ((u32) (temp_r29->bitflag & 0x1000) != 0)) {
                    temp_r29->tick();
                }
            } else {
                temp_r3_2 = &menu_screen_list[(s8) g_currently_visible_menu_screen.currScreen];
                if ((u32) (temp_r3_2->bitflag & 0x1000) != 0) {
                    temp_r3_2->tick();
                }
            }
        } else if ((did_any_pad_press_input(7U) != 0) && ((s32) g_currently_visible_menu_screen.unkC == 0) && ((s32) g_currently_visible_menu_screen.unk18 == 0) && (get_sprite_with_unique_id(0xA) != NULL)) {
            call_SoundReqID_arg_1(0x70);
            destroy_sprite_with_unique_id(0xA);
            modeCtrl.gameType = 0;
        }
        if (((s32) g_currently_visible_menu_screen.unkC != 0) || ((s32) g_currently_visible_menu_screen.unk18 != 0)) {
            fn_3_1BBA0();
            if (fn_3_1B984(1) == 0) {
                fn_3_1CA00();
            }
        }
        if ((s32) g_currently_visible_menu_screen.unk34 == 0) {
            g_something_with_menus(1);
        }
    }
}

void g_something_with_menus(s32 arg0) {
    f32 temp_f28;
    f32 temp_f30;
    f32 temp_f31;
    f32 var_f26;
    f32 var_f27;
    f32 var_f29;
    s32 var_r0;
    s32 var_r0_10;
    s32 var_r0_2;
    s32 var_r0_4;
    s32 var_r0_5;
    s32 var_r0_6;
    s32 var_r0_7;
    s32 var_r0_8;
    s32 var_r0_9;
    s32 var_r26;
    s32 var_r28;
    s32 var_r30;
    s32 var_r4;
    s32 var_r4_2;
    s32 var_r4_3;
    s32 var_r4_4;
    s32 var_r4_5;
    s32 var_r4_6;
    s8 var_r0_3;
    struct MenuScreen *temp_r29;
    struct Sprite *temp_r27;
    u32 temp_r0;
    u32 temp_r3;
    u32 var_r3;
    u32 var_r3_2;

    temp_r29 = &menu_screen_list[(s8) g_currently_visible_menu_screen.currScreen];
    set_ui_widescreen_scale_mtx(0x140);
    textdraw_reset();
    textdraw_set_font(3);
    textdraw_set_drop_shadow();
    textdraw_set_mul_color(0xFFFF00U);
    textdraw_set_alignment(4);
    textdraw_set_flags(0x80000000U);
    if ((u32) (temp_r29->bitflag & 0x10) != 0) {
        var_r28 = 6;
    } else if ((u32) (temp_r29->bitflag & 0x20) != 0) {
        var_r28 = 3;
    } else if ((u32) (temp_r29->bitflag & 0x40) != 0) {
        var_r28 = 1;
    } else {
        var_r28 = 4;
    }
    if ((s8) temp_r29->entry_count == 1) {
        var_r0 = 0xB4;
    } else if ((s8) temp_r29->entry_count == 2) {
        var_r0 = 0xA0;
    } else if ((u32) (temp_r29->bitflag & 0x10) != 0) {
        var_r0 = 0x64;
    } else if ((u32) (temp_r29->bitflag & 0x20) != 0) {
        var_r0 = 0xB4;
    } else if ((u32) (temp_r29->bitflag & 0x40) != 0) {
        var_r0 = 0;
    } else {
        var_r0 = 0x96;
    }
    temp_f31 = (f32) var_r0;
    if ((u32) (temp_r29->bitflag & 0x40) != 0) {
        var_r0_2 = 0xB4 / (s32) ((s8) temp_r29->entry_count + 1);
    } else {
        var_r0_2 = 0x50;
    }
    temp_f30 = (f32) var_r0_2;
    if ((s8) temp_r29->entry_count < (s32) (var_r28 - 1)) {
        var_r0_3 = (s8) temp_r29->entry_count;
    } else {
        var_r0_3 = var_r28 - 1;
    }
    var_f29 = 3.2e+02f - (temp_f31 * (0.5f * (f32) var_r0_3));
    temp_f28 = 2.2e+02f - (temp_f30 * (0.5f * (f32) ((s8) temp_r29->entry_count / var_r28)));
    textdraw_set_alignment(4);
    var_r30 = 0;
loop_151:
    if (var_r30 <= (s8) temp_r29->entry_count) {
        var_f27 = 0.0f;
        var_f26 = 0.0f;
        if (var_r30 == (s8) g_focused_root_menu.unk0[(s8) g_currently_visible_menu_screen.currScreen]) {
            var_r26 = 1;
        } else {
            var_r26 = 0;
        }
        if ((u32) (temp_r29->bitflag & 0x10000) != 0) {
            var_r26 = 0;
        }
        temp_r27 = get_sprite_with_unique_id(var_r30 + 0x3C);
        if ((s8) g_currently_visible_menu_screen.currScreen == 4) {
            var_r4 = (s8) (u8) sel_menu_info.menuStackPtr - 1;
loop_35:
            if (var_r4 < 0) {
                var_r0_4 = 0;
            } else if ((s8) sel_menu_info.menuStack[var_r4] == 0x1F) {
                var_r0_4 = 1;
            } else {
                var_r4 -= 1;
                goto loop_35;
            }
            if ((var_r0_4 != 0) && ((s8) lbl_1_bss_16F0.unk7 != 1)) {
                if (var_r30 >= (s8) sel_menu_info.unkB) {
                    temp_r27->userVar = 0;
                    goto block_150;
                }
                var_f29 = 3.2e+02f - (0.5f * temp_f31);
                goto block_76;
            }
            goto block_42;
        }
block_42:
        if ((s8) g_currently_visible_menu_screen.currScreen == 4) {
            var_r4_2 = (s8) (u8) sel_menu_info.menuStackPtr - 1;
loop_47:
            if (var_r4_2 < 0) {
                var_r0_5 = 0;
            } else if ((s8) sel_menu_info.menuStack[var_r4_2] == 0x3D) {
                var_r0_5 = 1;
            } else {
                var_r4_2 -= 1;
                goto loop_47;
            }
            if (var_r0_5 != 0) {
                if (var_r30 >= 2) {
                    temp_r27->userVar = 0;
                    goto block_150;
                }
                var_f29 = 3.2e+02f - (0.5f * temp_f31);
                goto block_76;
            }
            goto block_53;
        }
block_53:
        if ((s8) g_currently_visible_menu_screen.currScreen == 4) {
            var_r4_3 = (s8) (u8) sel_menu_info.menuStackPtr - 1;
loop_58:
            if (var_r4_3 < 0) {
                var_r0_6 = 0;
            } else if ((s8) sel_menu_info.menuStack[var_r4_3] == 0x42) {
                var_r0_6 = 1;
            } else {
                var_r4_3 -= 1;
                goto loop_58;
            }
            if (var_r0_6 != 0) {
                if (var_r30 >= 2) {
                    temp_r27->userVar = 0;
                    goto block_150;
                }
                var_f29 = 3.2e+02f - (0.5f * temp_f31);
                goto block_76;
            }
            goto block_64;
        }
block_64:
        if ((s8) g_currently_visible_menu_screen.currScreen == 4) {
            var_r4_4 = (s8) (u8) sel_menu_info.menuStackPtr - 1;
loop_69:
            if (var_r4_4 < 0) {
                var_r0_7 = 0;
            } else if ((s8) sel_menu_info.menuStack[var_r4_4] == 0x48) {
                var_r0_7 = 1;
            } else {
                var_r4_4 -= 1;
                goto loop_69;
            }
            if ((var_r0_7 != 0) && ((s8) lbl_1_bss_1738.unk0 == 0)) {
                if (var_r30 >= 2) {
                    temp_r27->userVar = 0;
                    goto block_150;
                }
                var_f29 = 3.2e+02f - (0.5f * temp_f31);
                goto block_76;
            }
            goto block_76;
        }
block_76:
        if ((u32) (temp_r29->bitflag & 0x40) != 0) {
            if ((u32) (menu_screen_list[(s8) temp_r29->entries[(s8) g_focused_root_menu.unk0[(s8) g_currently_visible_menu_screen.currScreen]].next_screen_id].bitflag & 0x100) == 0) {
                if ((var_r30 != (s8) temp_r29->entry_count) || ((s8) temp_r29->entry_count == 0)) {
                    var_f27 = (f32) (lbl_1_bss_6FB6C - 0x96);
                    textdraw_set_alignment(1);
                } else {
                    textdraw_set_alignment(4);
                }
                var_f26 = 15.0f;
                goto block_89;
            }
        } else {
            if ((u32) (temp_r29->bitflag & 0x400) != 0) {
                var_f26 = 15.0f;
            } else if (((u32) (temp_r29->bitflag & 0x4000) != 0) || ((u32) (temp_r29->bitflag & 0x8000) != 0) || ((u32) (temp_r29->bitflag & 0x40000) != 0)) {
                var_f26 = 15.0f;
            }
block_89:
            if ((u32) (temp_r29->bitflag & 0x80) != 0) {
                var_r4_5 = var_r30 % ((s32) ((s8) temp_r29->entry_count + 1) / var_r28);
            } else {
                var_r4_5 = var_r30 / var_r28;
            }
            if ((u32) (temp_r29->bitflag & 0x80) != 0) {
                var_r0_8 = var_r30 / (s32) ((s32) ((s8) temp_r29->entry_count + 1) / var_r28);
            } else {
                var_r0_8 = var_r30 % var_r28;
            }
            textdraw_set_pos(var_f27 + (var_f29 + (temp_f31 * (f32) var_r0_8)), var_f26 + (temp_f28 + (temp_f30 * (f32) var_r4_5)));
            if ((arg0 == 0) && (temp_r27 != NULL) && ((s32) temp_r27->userVar <= 1)) {
                if ((u32) (temp_r29->bitflag & 0x80) != 0) {
                    var_r0_9 = var_r30 / (s32) ((s32) ((s8) temp_r29->entry_count + 1) / var_r28);
                } else {
                    var_r0_9 = var_r30 % var_r28;
                }
                temp_r27->x = var_f27 + (var_f29 + (temp_f31 * (f32) var_r0_9));
                if ((u32) (temp_r29->bitflag & 0x80) != 0) {
                    var_r0_10 = var_r30 % ((s32) ((s8) temp_r29->entry_count + 1) / var_r28);
                } else {
                    var_r0_10 = var_r30 / var_r28;
                }
                temp_r27->y = var_f26 + (temp_f28 + (temp_f30 * (f32) var_r0_10));
            }
            if ((fn_3_1B984(1) != 0) && (arg0 != 0) && ((u32) (temp_r29->bitflag & 0x400) == 0) && ((u32) (temp_r29->bitflag & 0x800) == 0) && ((u32) (temp_r29->bitflag & 0x2000) == 0) && ((u32) (temp_r29->bitflag & 0x4000) == 0) && ((u32) (temp_r29->bitflag & 0x8000) == 0) && ((u32) (temp_r29->bitflag & 0x40000) == 0) && ((u8) ((u8) g_currently_visible_menu_screen.currScreen - 0x2C) > 1U) && ((u8) ((u8) g_currently_visible_menu_screen.currScreen - 0x17) > 1U)) {
                if (var_r26 != 0) {
                    var_r3 = 0xFF8000;
                } else {
                    var_r3 = 0xFFFF00;
                }
                textdraw_set_mul_color(var_r3);
                var_r4_6 = 0;
                if ((var_r26 != 0) && ((s32) g_currently_visible_menu_screen.unk4 == 0)) {
                    temp_r3 = (u32) (s8) (u8) g_currently_visible_menu_screen.unk8 >> 0x1FU;
                    temp_r0 = ((s8) (u8) g_currently_visible_menu_screen.unk8 << 0x1D) - temp_r3;
                    if ((s32) (((temp_r0 << 3) | (temp_r0 >> 0x1DU)) + temp_r3) >= 4) {
                        var_r4_6 = 1;
                    }
                }
                if (var_r4_6 != 0) {
                    var_r3_2 = 0xFFFFFF;
                } else {
                    var_r3_2 = 0;
                }
                textdraw_set_add_color(var_r3_2);
                if (((s8) g_currently_visible_menu_screen.currScreen == 4) && (var_r30 >= (s8) sel_menu_info.unkB)) {
                    textdraw_set_mul_color(0x202000U);
                }
                textdraw_print(temp_r29->entries[var_r30].name[(s8) g_language_id]);
            }
            if (((u32) (menu_screen_list[(s8) temp_r29->entries[(s8) g_focused_root_menu.unk0[(s8) g_currently_visible_menu_screen.currScreen]].next_screen_id].bitflag & 0x100) == 0) && ((u32) (temp_r29->bitflag & 0x40) != 0) && ((var_r30 != (s8) temp_r29->entry_count) || ((s8) temp_r29->entry_count == 0)) && (arg0 != 0)) {
                textdraw_set_pos(1.5e+02f + (var_f29 + (temp_f31 * (f32) (var_r30 % var_r28))), var_f26 + (temp_f28 + (temp_f30 * (f32) (var_r30 / var_r28))));
                textdraw_set_alignment(7);
                switch ((s8) g_currently_visible_menu_screen.currScreen) {
                case 0xB:
                    g_handle_starting_monkeys_count(1, var_r30);
                    break;
                case 0x12:
                    fn_3_11840(1, var_r30);
                    break;
                case 0x13:
                    fn_3_11860(1, var_r30);
                    break;
                case 0x19:
                case 0x1A:
                    fn_3_11884(1, var_r30);
                    break;
                case 0x1D:
                    fn_3_11BB8(1, var_r30);
                    break;
                case 0x21:
                case 0x22:
                    fn_3_11FD4(1, var_r30);
                    break;
                case 0x29:
                    fn_3_123E4(1, var_r30);
                    break;
                case 0x2E:
                    fn_3_12BAC(1, var_r30);
                    break;
                case 0x2F:
                    fn_3_12BCC(1, var_r30);
                    break;
                case 0x33:
                case 0x34:
                case 0x35:
                case 0x36:
                    fn_3_12BF0(1, var_r30);
                    break;
                case 0x3A:
                case 0x3B:
                    fn_3_12FA8(1, var_r30);
                    break;
                case 0x3F:
                case 0x40:
                    fn_3_13370(1, var_r30);
                    break;
                case 0x45:
                case 0x46:
                    fn_3_137D4(1, var_r30);
                    break;
                case 0x4D:
                case 0x4E:
                    fn_3_13C28(1, var_r30);
                    break;
                }
            }
block_150:
            var_r30 += 1;
            goto loop_151;
        }
    }
    textdraw_set_mul_color(0xFFFF00U);
    textdraw_set_add_color(0U);
    reset_ui_widescreen_scale_mtx();
}

void menu_mode_select_tick(void) {
    struct TextBox tb;
    struct MemcardRequest req;
    char buf[0x20];
    u8 temp_r0;

    if ((u32) (dip_switches & 0x10) != 0) {
        locked_menu_items = (s32) (locked_menu_items | 4);
    }
    if ((s8) g_focused_root_menu.unk0[(s8) g_currently_visible_menu_screen.currScreen] == 1) {
        if (are_all_party_games_unlocked() != 0) {
            g_currently_visible_menu_screen.unk38 = lbl_3_data_F28[(s8) g_language_id];
        } else if (is_able_to_unlock_party_game() != 0) {
            g_currently_visible_menu_screen.unk38 = lbl_3_data_E80[(s8) g_language_id];
        } else {
            g_currently_visible_menu_screen.unk38 = NULL;
        }
    } else {
        g_currently_visible_menu_screen.unk38 = NULL;
    }
    if ((s32) g_currently_visible_menu_screen.unkC != 0) {
        if ((s8) g_focused_root_menu.unk0[(s8) g_currently_visible_menu_screen.currScreen] == 2) {
            g_focused_root_menu.unk0[(s8) g_currently_visible_menu_screen.currScreen] = g_focused_root_menu.unk60;
        }
    } else if ((s32) g_currently_visible_menu_screen.unk18 != 0) {
        g_load_stage_for_menu_bg(-1, 1);
        if (((s8) g_focused_root_menu.unk0[(s8) g_currently_visible_menu_screen.currScreen] != 1) && (are_all_party_games_unlocked() == 0)) {
            if (lbl_1_bss_A5C40[0].state == 0) {
                memset(&tb, 0, sizeof(tb));
                tb.flags = 0x74;
                tb.x = 0x21C;
                tb.y = 0x6E;
                tb.rows = 1;
                tb.style = 5;
                tb.textScale = 0.5f;
                g_display_playpoint_or_gift_message_child(0, 1, &tb);
                g_some_printf_function_4(0, lbl_3_rodata_74[(s8) g_language_id], get_play_point_count());
            }
        }
        if (((s8) g_focused_root_menu.unk0[(s8) g_currently_visible_menu_screen.currScreen] == 2) && ((u8) storymode_save_files[795] != 0)) {
            g_save_game_data();
            if (fn_1_F0204() != 0) {
                memset(&req, 0, sizeof(req));
                req.unk0 = 0;
                g_sprintf_memcard_error(0, buf);
                req.fileName = buf;
                req.files = storymode_save_files;
                if ((s32) lbl_1_bss_CF3D8 != 0) {
                    req.flags |= 0x20000000;
                }
                g_some_replay_func4(0, (s32) (u8) g_last_used_memcard_slot_2, &req);
                lbl_3_bss_58.unk8 = 1;
            }
        }
    } else if (((s32) g_currently_visible_menu_screen.unk14 == 0) && ((s32) g_currently_visible_menu_screen.unk10 != 0)) {
        if ((s8) g_focused_root_menu.unk0[(s8) g_currently_visible_menu_screen.currScreen] == 0) {
            g_load_stage_for_menu_bg(-2, 1);
        }
        if ((s8) g_focused_root_menu.unk0[(s8) g_currently_visible_menu_screen.currScreen] != 1) {
            g_display_playpoint_or_gift_message_child(0, 0x14, NULL);
        }
    }
    if ((s32) g_currently_visible_menu_screen.unk10 != 0) {
        temp_r0 = g_focused_root_menu.unk0[(s8) g_currently_visible_menu_screen.currScreen];
        switch ((s8) temp_r0) {                     /* irregular */
        case 0:
            call_SoundReqID_arg_0(0x212);
            g_focused_root_menu.unk60 = 0U;
            return;
        case 1:
            call_SoundReqID_arg_0(0x21B);
            g_focused_root_menu.unk60 = 1U;
            return;
        case 2:
            call_SoundReqID_arg_0(0x394);
            break;
        }
    }
}

void menu_party_game_select_tick(void) {
    struct TextBox tb;
    s32 temp_r0;
    s32 var_r3;
    s32 var_r3_2;
    s8 temp_r27;
    s8 temp_r29;
    s8 var_r27;
    struct MenuScreen *temp_r28;
    struct Sprite *temp_r3;
    u8 temp_r4;

    temp_r28 = &menu_screen_list[(s8) g_currently_visible_menu_screen.currScreen];
    temp_r27 = (s8) g_focused_root_menu.unk0[1];
    temp_r29 = lbl_3_rodata_4[(s8) g_focused_root_menu.unk0[(s8) g_currently_visible_menu_screen.currScreen]];
    locked_menu_items = 0;
    if ((u32) (dip_switches & 0x10) != 0) {
        locked_menu_items = (s32) (locked_menu_items | 0x580);
    }
    if (g_check_if_partygame_unlocked(0xA) == 0) {
        locked_menu_items = (s32) (locked_menu_items | 0x40);
    }
    if (g_check_if_partygame_unlocked(9) == 0) {
        locked_menu_items = (s32) (locked_menu_items | 0x80);
    }
    if (g_check_if_partygame_unlocked(0xD) == 0) {
        locked_menu_items = (s32) (locked_menu_items | 0x100);
    }
    if (g_check_if_partygame_unlocked(0xC) == 0) {
        locked_menu_items = (s32) (locked_menu_items | 0x200);
    }
    if (g_check_if_partygame_unlocked(0xF) == 0) {
        locked_menu_items = (s32) (locked_menu_items | 0x400);
    }
    if (g_check_if_partygame_unlocked(0x10) == 0) {
        locked_menu_items = (s32) (locked_menu_items | 0x800);
    }
    if (g_check_if_partygame_unlocked(temp_r29) != 0) {
        temp_r28->bitflag &= 0xFFFFFFF7;
    }
    if ((s32) g_currently_visible_menu_screen.unkC != 0) {
        if (are_all_party_games_unlocked() == 0) {
            if (lbl_1_bss_A5C40[0].state == 0) {
                memset(&tb, 0, sizeof(tb));
                tb.flags = 0x74;
                tb.x = 0x21C;
                tb.y = 0x6E;
                tb.rows = 1;
                tb.style = 5;
                tb.textScale = 0.5f;
                g_display_playpoint_or_gift_message_child(0, 1, &tb);
                g_some_printf_function_4(0, lbl_3_rodata_74[(s8) g_language_id], get_play_point_count());
            }
        }
        lbl_3_bss_58.unkC = 0x1E;
    } else if ((s32) g_currently_visible_menu_screen.unk18 != 0) {
        g_load_stage_for_menu_bg(-1, 1);
        if ((s8) (u8) lbl_3_bss_44.unk0 != -1) {
            g_change_music_volume((s32) (s8) (u8) lbl_3_bss_44.unk0, 0x3CU, 1);
        }
        if (are_all_party_games_unlocked() != 0) {
            var_r3 = 0x5A;
        } else {
            var_r3 = 5;
        }
        g_change_music_volume(var_r3, 0x3CU, 0x64);
        if (are_all_party_games_unlocked() == 0) {
            if (lbl_1_bss_A5C40[0].state == 0) {
                memset(&tb, 0, sizeof(tb));
                tb.flags = 0x74;
                tb.x = 0x21C;
                tb.y = 0x6E;
                tb.rows = 1;
                tb.style = 5;
                tb.textScale = 0.5f;
                g_display_playpoint_or_gift_message_child(0, 1, &tb);
                g_some_printf_function_4(0, lbl_3_rodata_74[(s8) g_language_id], get_play_point_count());
            }
        }
        lbl_3_bss_58.unkC = 0;
        fn_3_1EF40();
    } else if ((s32) g_currently_visible_menu_screen.unk14 != 0) {
        if ((s8) (u8) lbl_3_bss_44.unk0 != lbl_3_rodata_28[temp_r27]) {
            var_r27 = (s8) lbl_3_rodata_28[temp_r27];
            if (var_r27 < 0) {
                var_r27 = 0;
            }
            if (g_maybe_related_to_music_crossfading((s32) var_r27) == -1) {
                if ((s8) (u8) lbl_3_bss_44.unk0 != -1) {
                    g_something_with_stopping_music_or_sfx((s32) (s8) (u8) lbl_3_bss_44.unk0);
                }
                g_SoftStreamStart_with_some_defaults_2((s32) var_r27);
                lbl_3_bss_44.unk0 = (u8) var_r27;
            } else {
                g_change_music_volume((s32) var_r27, 0x1EU, 0x64);
            }
        }
    } else if ((s32) g_currently_visible_menu_screen.unk10 != 0) {
        g_load_stage_for_menu_bg(temp_r27, 1);
        call_SoundReqID_arg_0(lbl_3_rodata_10[temp_r27]);
        if (are_all_party_games_unlocked() != 0) {
            var_r3_2 = 0x5A;
        } else {
            var_r3_2 = 5;
        }
        g_change_music_volume(var_r3_2, 0x3CU, 1);
        if ((s8) (u8) lbl_3_bss_44.unk0 == lbl_3_rodata_28[temp_r27]) {
            g_change_music_volume((s32) (s8) (u8) lbl_3_bss_44.unk0, 0x3CU, 0x64);
        }
        fn_3_1EE90(temp_r27);
        g_display_playpoint_or_gift_message_child(0, 0x14, NULL);
    } else {
        if (did_any_pad_press_input(8U) != 0) {
            temp_r4 = g_focused_root_menu.unk0[(s8) g_currently_visible_menu_screen.currScreen];
            if (((u32) (dip_switches & 0x10) == 0) || ((s8) temp_r4 < 6)) {
                modeCtrl.gameType = (s32) lbl_3_rodata_4[(s8) temp_r4];
                call_SoundReqID_arg_1(0x6E);
                create_how_to_sprite();
            }
        }
        if ((g_check_if_partygame_unlocked(temp_r29) == 0) && (is_able_to_unlock_party_game() != 0) && ((s32) lbl_3_bss_58.unkC == 0) && (did_any_pad_press_input(6U) != 0) && ((s32) g_currently_visible_menu_screen.unkC == 0) && ((s32) g_currently_visible_menu_screen.unk18 == 0)
#ifndef __MWERKS__
            /* PC: A on an unavailable game that isn't locked (the first six
             * never are) only buzzes; the original starts those instead, so
             * this never ran for them */
            && !(pc_sel_ngc_entry_unavailable((s8) g_focused_root_menu.unk0[(s8) g_currently_visible_menu_screen.currScreen])
                 && ((u32) (locked_menu_items & (1 << (s8) g_focused_root_menu.unk0[(s8) g_currently_visible_menu_screen.currScreen])) == 0))
#endif
            ) {
            temp_r3 = get_sprite_with_unique_id(0xC);
            if (temp_r3 != NULL) {
                temp_r3->userVar = 0;
            }
            unlock_party_game(temp_r29);
            memset(&tb, 0, sizeof(tb));
            tb.flags = 0x74;
            tb.x = 0x21C;
            tb.y = 0x6E;
            tb.rows = 1;
            tb.style = 5;
            tb.textScale = 0.5f;
            g_display_playpoint_or_gift_message_child(0, 1, &tb);
            g_some_printf_function_4(0, lbl_3_rodata_74[(s8) g_language_id], get_play_point_count());
            temp_r0 = 1 << (s8) g_focused_root_menu.unk0[(s8) g_currently_visible_menu_screen.currScreen];
            locked_menu_items = (s32) (locked_menu_items & ~(temp_r0 | temp_r0));
            if (are_all_party_games_unlocked() != 0) {
                g_display_playpoint_or_gift_message_child(0, 0x14, NULL);
                g_smth_related_to_music(0x5A, 0x78, 0x78, 0x64, 0);
            }
            lbl_3_bss_58.unkC = 0x1E;
        }
    }
    if ((s32) lbl_3_bss_58.unkC > 0) {
        lbl_3_bss_58.unkC = (s32) (lbl_3_bss_58.unkC - 1);
    }
#ifndef __MWERKS__
    /* PC: the description box says why a not yet decompiled game can't be
     * played (and that it can still be opened with Play Points) */
    temp_r4 = g_focused_root_menu.unk0[(s8) g_currently_visible_menu_screen.currScreen];
    if (pc_sel_ngc_entry_unavailable((s8) temp_r4)) {
        g_currently_visible_menu_screen.unk38 = (char *) pc_party_unavail_desc(
            ((u32) (locked_menu_items & (1 << (s8) temp_r4)) != 0) && is_able_to_unlock_party_game() != 0);
        return;
    }
#endif
    if ((u32) (locked_menu_items & (1 << (s8) g_focused_root_menu.unk0[(s8) g_currently_visible_menu_screen.currScreen])) != 0) {
        if (is_able_to_unlock_party_game() != 0) {
            g_currently_visible_menu_screen.unk38 = lbl_3_data_1BAC[(s8) g_language_id];
            return;
        }
        g_currently_visible_menu_screen.unk38 = lbl_3_data_1C74[(s8) g_language_id];
        return;
    }
    g_currently_visible_menu_screen.unk38 = NULL;
}

void menu_option_tick(void) {
    s32 var_r3;
    s32 var_r3_2;

    if (are_all_party_games_unlocked() == 0) {
        locked_menu_items = (s32) (locked_menu_items | 2);
        if ((s8) g_focused_root_menu.unk0[(s8) g_currently_visible_menu_screen.currScreen] == 1) {
            fn_3_32E0(0);
            if ((did_any_pad_press_input(2U) != 0) || (did_any_pad_press_input(3U) != 0)) {
                g_focused_root_menu.unk0[(s8) g_currently_visible_menu_screen.currScreen] = 0;
            }
        }
    } else {
        locked_menu_items = (s32) (locked_menu_items & 0xFFFFFFFD);
    }
    if (((s8) g_focused_root_menu.unk0[(s8) g_currently_visible_menu_screen.currScreen] == 1) && (are_all_gifts_unlocked() != 0)) {
        g_currently_visible_menu_screen.unk38 = lbl_3_data_221C[(s8) g_language_id];
    } else {
        g_currently_visible_menu_screen.unk38 = NULL;
    }
    if ((s32) g_currently_visible_menu_screen.unkC != 0) {
        if (are_all_party_games_unlocked() != 0) {
            var_r3 = 0x5A;
        } else {
            var_r3 = 5;
        }
        g_change_music_volume(var_r3, 0x1EU, 0x3C);
        return;
    }
    if ((s32) g_currently_visible_menu_screen.unk18 != 0) {
        fn_3_1CA00();
        return;
    }
    if (((s32) g_currently_visible_menu_screen.unk14 != 0) && ((s32) g_currently_visible_menu_screen.unk4 != 0)) {
        if (are_all_party_games_unlocked() != 0) {
            var_r3_2 = 0x5A;
        } else {
            var_r3_2 = 5;
        }
        g_change_music_volume(var_r3_2, 0x1EU, 0x64);
    }
}

void menu_return_to_title(void) {
    main_mode_request = 0;
    sub_mode_request = 0x15;
}

void menu_number_of_players_tick(void) {
    s32 var_r0;
    s32 var_r0_10;
    s32 var_r0_11;
    s32 var_r0_13;
    s32 var_r0_14;
    s32 var_r0_15;
    s32 var_r0_16;
    s32 var_r0_2;
    s32 var_r0_3;
    s32 var_r0_4;
    s32 var_r0_5;
    s32 var_r0_6;
    s32 var_r0_7;
    s32 var_r0_8;
    s32 var_r0_9;
    s32 var_r4;
    s32 var_r4_10;
    s32 var_r4_11;
    s32 var_r4_12;
    s32 var_r4_13;
    s32 var_r4_14;
    s32 var_r4_15;
    s32 var_r4_16;
    s32 var_r4_17;
    s32 var_r4_18;
    s32 var_r4_19;
    s32 var_r4_20;
    s32 var_r4_2;
    s32 var_r4_3;
    s32 var_r4_4;
    s32 var_r4_5;
    s32 var_r4_6;
    s32 var_r4_7;
    s32 var_r4_8;
    s32 var_r4_9;
    s32 var_r5;
    s32 var_r5_10;
    s32 var_r5_11;
    s32 var_r5_12;
    s32 var_r5_13;
    s32 var_r5_14;
    s32 var_r5_15;
    s32 var_r5_16;
    s32 var_r5_17;
    s32 var_r5_18;
    s32 var_r5_19;
    s32 var_r5_21;
    s32 var_r5_22;
    s32 var_r5_23;
    s32 var_r5_24;
    s32 var_r5_25;
    s32 var_r5_26;
    s32 var_r5_27;
    s32 var_r5_28;
    s32 var_r5_29;
    s32 var_r5_2;
    s32 var_r5_30;
    s32 var_r5_31;
    s32 var_r5_32;
    s32 var_r5_33;
    s32 var_r5_34;
    s32 var_r5_3;
    s32 var_r5_4;
    s32 var_r5_5;
    s32 var_r5_6;
    s32 var_r5_7;
    s32 var_r5_8;
    s32 var_r5_9;
    s32 var_r6;
    s32 var_r6_2;
    s32 var_r6_3;
    s32 var_r6_4;
    s32 var_r6_5;
    s32 var_r6_6;
    s32 var_r6_7;
    s8 var_r0_12;
    s8 var_r4_21;
    s8 var_r5_20;

    var_r5 = (s8) (u8) sel_menu_info.menuStackPtr - 1;
loop_4:
    if (var_r5 < 0) {
        var_r4 = 0;
    } else if ((s8) sel_menu_info.menuStack[var_r5] == 0xF) {
        var_r4 = 1;
    } else {
        var_r5 -= 1;
        goto loop_4;
    }
    var_r4_2 = var_r4 != 0;
    if (var_r4_2 == 0) {
        var_r5_2 = (s8) (u8) sel_menu_info.menuStackPtr - 1;
loop_11:
        if (var_r5_2 < 0) {
            var_r4_3 = 0;
        } else if ((s8) sel_menu_info.menuStack[var_r5_2] == 0x15) {
            var_r4_3 = 1;
        } else {
            var_r5_2 -= 1;
            goto loop_11;
        }
        var_r4_2 = var_r4_3 != 0;
    }
    var_r4_4 = var_r4_2 != 0;
    if (var_r4_4 == 0) {
        var_r5_3 = (s8) (u8) sel_menu_info.menuStackPtr - 1;
loop_19:
        if (var_r5_3 < 0) {
            var_r4_5 = 0;
        } else if ((s8) sel_menu_info.menuStack[var_r5_3] == 0x1C) {
            var_r4_5 = 1;
        } else {
            var_r5_3 -= 1;
            goto loop_19;
        }
        var_r4_4 = var_r4_5 != 0;
    }
    var_r4_6 = var_r4_4 != 0;
    if (var_r4_6 == 0) {
        var_r5_4 = (s8) (u8) sel_menu_info.menuStackPtr - 1;
loop_27:
        if (var_r5_4 < 0) {
            var_r4_7 = 0;
        } else if ((s8) sel_menu_info.menuStack[var_r5_4] == 0x2B) {
            var_r4_7 = 1;
        } else {
            var_r5_4 -= 1;
            goto loop_27;
        }
        var_r4_6 = var_r4_7 != 0;
    }
    var_r4_8 = var_r4_6 != 0;
    if (var_r4_8 == 0) {
        var_r5_5 = (s8) (u8) sel_menu_info.menuStackPtr - 1;
loop_35:
        if (var_r5_5 < 0) {
            var_r4_9 = 0;
        } else if ((s8) sel_menu_info.menuStack[var_r5_5] == 0x31) {
            var_r4_9 = 1;
        } else {
            var_r5_5 -= 1;
            goto loop_35;
        }
        var_r4_8 = var_r4_9 != 0;
    }
    var_r4_10 = var_r4_8 != 0;
    if (var_r4_10 == 0) {
        var_r5_6 = (s8) (u8) sel_menu_info.menuStackPtr - 1;
loop_43:
        if (var_r5_6 < 0) {
            var_r4_11 = 0;
        } else if ((s8) sel_menu_info.menuStack[var_r5_6] == 0x38) {
            var_r4_11 = 1;
        } else {
            var_r5_6 -= 1;
            goto loop_43;
        }
        var_r4_10 = var_r4_11 != 0;
    }
    var_r4_12 = var_r4_10 != 0;
    if (var_r4_12 == 0) {
        var_r5_7 = (s8) (u8) sel_menu_info.menuStackPtr - 1;
loop_51:
        if (var_r5_7 < 0) {
            var_r4_13 = 0;
        } else if ((s8) sel_menu_info.menuStack[var_r5_7] == 0x3D) {
            var_r4_13 = 1;
        } else {
            var_r5_7 -= 1;
            goto loop_51;
        }
        var_r4_12 = var_r4_13 != 0;
    }
    var_r4_14 = var_r4_12 != 0;
    if (var_r4_14 == 0) {
        var_r5_8 = (s8) (u8) sel_menu_info.menuStackPtr - 1;
loop_59:
        if (var_r5_8 < 0) {
            var_r4_15 = 0;
        } else if ((s8) sel_menu_info.menuStack[var_r5_8] == 0x42) {
            var_r4_15 = 1;
        } else {
            var_r5_8 -= 1;
            goto loop_59;
        }
        var_r4_14 = var_r4_15 != 0;
    }
    var_r0 = var_r4_14 != 0;
    if (var_r0 == 0) {
        var_r5_9 = (s8) (u8) sel_menu_info.menuStackPtr - 1;
loop_67:
        if (var_r5_9 < 0) {
            var_r4_16 = 0;
        } else if ((s8) sel_menu_info.menuStack[var_r5_9] == 0x48) {
            var_r4_16 = 1;
        } else {
            var_r5_9 -= 1;
            goto loop_67;
        }
        var_r0 = var_r4_16 != 0;
    }
    if ((s32) g_currently_visible_menu_screen.unkC != 0) {
        var_r5_10 = (s8) (u8) sel_menu_info.menuStackPtr - 1;
loop_75:
        if (var_r5_10 < 0) {
            var_r4_17 = 0;
        } else if ((s8) sel_menu_info.menuStack[var_r5_10] == 0x1F) {
            var_r4_17 = 1;
        } else {
            var_r5_10 -= 1;
            goto loop_75;
        }
        if ((var_r4_17 == 0) || ((s8) lbl_1_bss_16F0.unk7 == 1)) {
            var_r5_11 = (s8) (u8) sel_menu_info.menuStackPtr - 1;
loop_83:
            if (var_r5_11 < 0) {
                var_r4_18 = 0;
            } else if ((s8) sel_menu_info.menuStack[var_r5_11] == 0x3D) {
                var_r4_18 = 1;
            } else {
                var_r5_11 -= 1;
                goto loop_83;
            }
            if (var_r4_18 == 0) {
                var_r5_12 = (s8) (u8) sel_menu_info.menuStackPtr - 1;
loop_90:
                if (var_r5_12 < 0) {
                    var_r4_19 = 0;
                } else if ((s8) sel_menu_info.menuStack[var_r5_12] == 0x42) {
                    var_r4_19 = 1;
                } else {
                    var_r5_12 -= 1;
                    goto loop_90;
                }
                if (var_r4_19 == 0) {
                    var_r5_13 = (s8) (u8) sel_menu_info.menuStackPtr - 1;
loop_97:
                    if (var_r5_13 < 0) {
                        var_r4_20 = 0;
                    } else if ((s8) sel_menu_info.menuStack[var_r5_13] == 0x48) {
                        var_r4_20 = 1;
                    } else {
                        var_r5_13 -= 1;
                        goto loop_97;
                    }
                    if ((var_r4_20 != 0) && ((s8) lbl_1_bss_1738.unk0 == 0)) {
                        goto block_101;
                    }
                } else {
                    goto block_101;
                }
            } else {
                goto block_101;
            }
        } else {
block_101:
            if ((s8) g_focused_root_menu.unk0[(s8) g_currently_visible_menu_screen.currScreen] < 1) {
                var_r4_21 = (s8) g_focused_root_menu.unk0[(s8) g_currently_visible_menu_screen.currScreen];
            } else {
                var_r4_21 = 1;
            }
            g_focused_root_menu.unk0[(s8) g_currently_visible_menu_screen.currScreen] = (u8) var_r4_21;
        }
        goto block_163;
    }
    if ((s32) g_currently_visible_menu_screen.unk14 != 0) {
        modeCtrl.playerCount = (s8) g_focused_root_menu.unk0[(s8) g_currently_visible_menu_screen.currScreen] + 1;
        var_r5_14 = (s8) (u8) sel_menu_info.menuStackPtr - 1;
loop_110:
        if (var_r5_14 < 0) {
            var_r0_2 = 0;
        } else if ((s8) sel_menu_info.menuStack[var_r5_14] == 0x24) {
            var_r0_2 = 1;
        } else {
            var_r5_14 -= 1;
            goto loop_110;
        }
        if ((var_r0_2 != 0) && ((s8) (u8) lbl_1_bss_1700.unk0 != 2)) {
            g_currently_visible_menu_screen.nextScreen = 6;
        } else {
            var_r5_15 = (s8) (u8) sel_menu_info.menuStackPtr - 1;
loop_119:
            if (var_r5_15 < 0) {
                var_r0_3 = 0;
            } else if ((s8) sel_menu_info.menuStack[var_r5_15] == 0x3D) {
                var_r0_3 = 1;
            } else {
                var_r5_15 -= 1;
                goto loop_119;
            }
            if (var_r0_3 != 0) {
                g_currently_visible_menu_screen.nextScreen = 0x3E;
            } else {
                var_r5_16 = (s8) (u8) sel_menu_info.menuStackPtr - 1;
loop_127:
                if (var_r5_16 < 0) {
                    var_r0_4 = 0;
                } else if ((s8) sel_menu_info.menuStack[var_r5_16] == 0x42) {
                    var_r0_4 = 1;
                } else {
                    var_r5_16 -= 1;
                    goto loop_127;
                }
                if (var_r0_4 != 0) {
                    g_currently_visible_menu_screen.nextScreen = 0x43;
                } else {
                    var_r5_17 = (s8) (u8) sel_menu_info.menuStackPtr - 1;
loop_135:
                    if (var_r5_17 < 0) {
                        var_r0_5 = 0;
                    } else if ((s8) sel_menu_info.menuStack[var_r5_17] == 0x48) {
                        var_r0_5 = 1;
                    } else {
                        var_r5_17 -= 1;
                        goto loop_135;
                    }
                    if ((var_r0_5 != 0) && ((s8) lbl_1_bss_1738.unk0 == 1) && ((s32) modeCtrl.playerCount == 2)) {
                        g_currently_visible_menu_screen.nextScreen = 0x49;
                    } else {
                        var_r5_18 = (s8) (u8) sel_menu_info.menuStackPtr - 1;
loop_145:
                        if (var_r5_18 < 0) {
                            var_r0_6 = 0;
                        } else if ((s8) sel_menu_info.menuStack[var_r5_18] == 0x48) {
                            var_r0_6 = 1;
                        } else {
                            var_r5_18 -= 1;
                            goto loop_145;
                        }
                        if ((var_r0_6 != 0) && ((s8) lbl_1_bss_1738.unk0 == 1) && ((s32) modeCtrl.playerCount == 3)) {
                            g_currently_visible_menu_screen.nextScreen = 0x4A;
                        } else {
                            var_r5_19 = (s8) (u8) sel_menu_info.menuStackPtr - 1;
loop_155:
                            if (var_r5_19 < 0) {
                                var_r0_7 = 0;
                            } else if ((s8) sel_menu_info.menuStack[var_r5_19] == 0x48) {
                                var_r0_7 = 1;
                            } else {
                                var_r5_19 -= 1;
                                goto loop_155;
                            }
                            if ((var_r0_7 != 0) && ((s8) lbl_1_bss_1738.unk0 == 1) && ((s32) modeCtrl.playerCount == 4)) {
                                g_currently_visible_menu_screen.nextScreen = 0x4B;
                            } else {
                                g_currently_visible_menu_screen.nextScreen = 5;
                            }
                        }
                    }
                }
            }
        }
        return;
    }
block_163:
    if (var_r0 != 0) {
        var_r5_20 = 0;
        if ((u32) (dip_switches & 0x400000) != 0) {
            var_r5_20 = 4;
        } else {
            var_r6 = 0;
loop_170:
            if (var_r6 < 4) {
                if ((s8) (u8) pad_status_groups[var_r6].unk0[0].err == 0) {
                    var_r5_20 += 1;
                }
                var_r6 += 1;
                goto loop_170;
            }
        }
        sel_menu_info.unkB = (u8) var_r5_20;
    } else {
        sel_menu_info.unkB = 4;
    }
    var_r5_21 = (s8) (u8) sel_menu_info.menuStackPtr - 1;
loop_177:
    if (var_r5_21 < 0) {
        var_r0_8 = 0;
    } else if ((s8) sel_menu_info.menuStack[var_r5_21] == 0x1F) {
        var_r0_8 = 1;
    } else {
        var_r5_21 -= 1;
        goto loop_177;
    }
    if ((var_r0_8 != 0) && ((s8) lbl_1_bss_16F0.unk7 != 1)) {
        sel_menu_info.unkB = 2;
        g_currently_visible_menu_screen.unk3C = 2;
    } else {
        var_r5_22 = (s8) (u8) sel_menu_info.menuStackPtr - 1;
loop_186:
        if (var_r5_22 < 0) {
            var_r0_9 = 0;
        } else if ((s8) sel_menu_info.menuStack[var_r5_22] == 0x3D) {
            var_r0_9 = 1;
        } else {
            var_r5_22 -= 1;
            goto loop_186;
        }
        if (var_r0_9 != 0) {
            g_currently_visible_menu_screen.unk3C = 2;
        } else {
            var_r5_23 = (s8) (u8) sel_menu_info.menuStackPtr - 1;
loop_194:
            if (var_r5_23 < 0) {
                var_r0_10 = 0;
            } else if ((s8) sel_menu_info.menuStack[var_r5_23] == 0x42) {
                var_r0_10 = 1;
            } else {
                var_r5_23 -= 1;
                goto loop_194;
            }
            if (var_r0_10 != 0) {
                g_currently_visible_menu_screen.unk3C = 2;
            } else {
                var_r5_24 = (s8) (u8) sel_menu_info.menuStackPtr - 1;
loop_202:
                if (var_r5_24 < 0) {
                    var_r0_11 = 0;
                } else if ((s8) sel_menu_info.menuStack[var_r5_24] == 0x48) {
                    var_r0_11 = 1;
                } else {
                    var_r5_24 -= 1;
                    goto loop_202;
                }
                if ((var_r0_11 != 0) && ((s8) lbl_1_bss_1738.unk0 == 0)) {
                    g_currently_visible_menu_screen.unk3C = 2;
                } else {
                    g_currently_visible_menu_screen.unk3C = 4;
                }
            }
        }
    }
    if ((s8) sel_menu_info.unkB > 0) {
        if ((s8) g_focused_root_menu.unk0[(s8) g_currently_visible_menu_screen.currScreen] < (s32) ((s8) sel_menu_info.unkB - 1)) {
            var_r0_12 = (s8) g_focused_root_menu.unk0[(s8) g_currently_visible_menu_screen.currScreen];
        } else {
            var_r0_12 = (s8) sel_menu_info.unkB - 1;
        }
        g_focused_root_menu.unk0[(s8) g_currently_visible_menu_screen.currScreen] = (u8) var_r0_12;
    }
    var_r5_25 = (s8) (u8) sel_menu_info.menuStackPtr - 1;
loop_217:
    if (var_r5_25 < 0) {
        var_r0_13 = 0;
    } else if ((s8) sel_menu_info.menuStack[var_r5_25] == 0x3D) {
        var_r0_13 = 1;
    } else {
        var_r5_25 -= 1;
        goto loop_217;
    }
    if (var_r0_13 != 0) {
        var_r5_26 = 0;
        if ((u32) (dip_switches & 0x400000) != 0) {
            var_r5_26 = 4;
        } else {
            var_r6_2 = 0;
loop_226:
            if (var_r6_2 < 4) {
                if ((s8) (u8) pad_status_groups[var_r6_2].unk0[0].err == 0) {
                    var_r5_26 += 1;
                }
                var_r6_2 += 1;
                goto loop_226;
            }
        }
        if (var_r5_26 < 2) {
            var_r5_27 = 0;
            if ((u32) (dip_switches & 0x400000) != 0) {
                var_r5_27 = 4;
            } else {
                var_r6_3 = 0;
loop_234:
                if (var_r6_3 < 4) {
                    if ((s8) (u8) pad_status_groups[var_r6_3].unk0[0].err == 0) {
                        var_r5_27 += 1;
                    }
                    var_r6_3 += 1;
                    goto loop_234;
                }
            }
        } else {
            var_r5_27 = 2;
        }
        lbl_3_bss_58.unk0 = (s8) (var_r5_27 - 1);
    } else {
        var_r5_28 = (s8) (u8) sel_menu_info.menuStackPtr - 1;
loop_242:
        if (var_r5_28 < 0) {
            var_r0_14 = 0;
        } else if ((s8) sel_menu_info.menuStack[var_r5_28] == 0x42) {
            var_r0_14 = 1;
        } else {
            var_r5_28 -= 1;
            goto loop_242;
        }
        if (var_r0_14 != 0) {
            var_r5_29 = 0;
            if ((u32) (dip_switches & 0x400000) != 0) {
                var_r5_29 = 4;
            } else {
                var_r6_4 = 0;
loop_251:
                if (var_r6_4 < 4) {
                    if ((s8) (u8) pad_status_groups[var_r6_4].unk0[0].err == 0) {
                        var_r5_29 += 1;
                    }
                    var_r6_4 += 1;
                    goto loop_251;
                }
            }
            if (var_r5_29 < 2) {
                var_r5_30 = 0;
                if ((u32) (dip_switches & 0x400000) != 0) {
                    var_r5_30 = 4;
                } else {
                    var_r6_5 = 0;
loop_259:
                    if (var_r6_5 < 4) {
                        if ((s8) (u8) pad_status_groups[var_r6_5].unk0[0].err == 0) {
                            var_r5_30 += 1;
                        }
                        var_r6_5 += 1;
                        goto loop_259;
                    }
                }
            } else {
                var_r5_30 = 2;
            }
            lbl_3_bss_58.unk0 = (s8) (var_r5_30 - 1);
        } else {
            var_r5_31 = (s8) (u8) sel_menu_info.menuStackPtr - 1;
loop_267:
            if (var_r5_31 < 0) {
                var_r0_15 = 0;
            } else if ((s8) sel_menu_info.menuStack[var_r5_31] == 0x48) {
                var_r0_15 = 1;
            } else {
                var_r5_31 -= 1;
                goto loop_267;
            }
            if ((var_r0_15 != 0) && ((s8) lbl_1_bss_1738.unk0 == 0)) {
                var_r5_32 = 0;
                if ((u32) (dip_switches & 0x400000) != 0) {
                    var_r5_32 = 4;
                } else {
                    var_r6_6 = 0;
loop_277:
                    if (var_r6_6 < 4) {
                        if ((s8) (u8) pad_status_groups[var_r6_6].unk0[0].err == 0) {
                            var_r5_32 += 1;
                        }
                        var_r6_6 += 1;
                        goto loop_277;
                    }
                }
                if (var_r5_32 < 2) {
                    var_r5_33 = 0;
                    if ((u32) (dip_switches & 0x400000) != 0) {
                        var_r5_33 = 4;
                    } else {
                        var_r6_7 = 0;
loop_285:
                        if (var_r6_7 < 4) {
                            if ((s8) (u8) pad_status_groups[var_r6_7].unk0[0].err == 0) {
                                var_r5_33 += 1;
                            }
                            var_r6_7 += 1;
                            goto loop_285;
                        }
                    }
                } else {
                    var_r5_33 = 2;
                }
                lbl_3_bss_58.unk0 = (s8) (var_r5_33 - 1);
            } else {
                lbl_3_bss_58.unk0 = (s8) (sel_menu_info.unkB - 1);
            }
        }
    }
    var_r5_34 = (s8) (u8) sel_menu_info.menuStackPtr - 1;
loop_294:
    if (var_r5_34 < 0) {
        var_r0_16 = 0;
    } else if ((s8) sel_menu_info.menuStack[var_r5_34] == 7) {
        var_r0_16 = 1;
    } else {
        var_r5_34 -= 1;
        goto loop_294;
    }
    if ((var_r0_16 != 0) && ((s8) g_focused_root_menu.unk0[7] == 1)) {
        if ((s8) g_focused_root_menu.unk0[(s8) g_currently_visible_menu_screen.currScreen] == 0) {
            g_currently_visible_menu_screen.unk38 = lbl_3_data_25A0[(s8) g_language_id];
            return;
        }
        g_currently_visible_menu_screen.unk38 = lbl_3_data_2748[(s8) g_language_id];
    }
}

u8 *fn_3_6A14(s32 arg0) {
    s32 var_r0;
    s32 var_r0_2;
    s32 var_r0_3;
    s32 var_r5;
    s32 var_r5_2;
    s32 var_r5_3;

    var_r5 = (s8) (u8) sel_menu_info.menuStackPtr - 1;
loop_4:
    if (var_r5 < 0) {
        var_r0 = 0;
    } else if ((s8) sel_menu_info.menuStack[var_r5] == 0x1F) {
        var_r0 = 1;
    } else {
        var_r5 -= 1;
        goto loop_4;
    }
    if (var_r0 != 0) {
        if (arg0 >= (s32) modeCtrl.playerCount) {
            return &lbl_1_bss_16F0.unk9[arg0];
        }
        return &g_focused_root_menu.unk58[arg0];
    }
    var_r5_2 = (s8) (u8) sel_menu_info.menuStackPtr - 1;
loop_14:
    if (var_r5_2 < 0) {
        var_r0_2 = 0;
    } else if ((s8) sel_menu_info.menuStack[var_r5_2] == 0x3D) {
        var_r0_2 = 1;
    } else {
        var_r5_2 -= 1;
        goto loop_14;
    }
    if (var_r0_2 != 0) {
        return &lbl_1_bss_1720.unk1[arg0];
    }
    var_r5_3 = (s8) (u8) sel_menu_info.menuStackPtr - 1;
loop_22:
    if (var_r5_3 < 0) {
        var_r0_3 = 0;
    } else if ((s8) sel_menu_info.menuStack[var_r5_3] == 0x42) {
        var_r0_3 = 1;
    } else {
        var_r5_3 -= 1;
        goto loop_22;
    }
    if (var_r0_3 != 0) {
        return &lbl_1_bss_172C.unk1[arg0];
    }
    return &g_focused_root_menu.unk58[arg0];
}

void fn_3_6B88(void) {
    s32 sp8;
    s32 var_r0;
    s32 var_r0_2;
    s32 var_r0_3;
    s32 var_r0_4;
    s32 var_r0_5;
    s32 var_r0_6;
    s32 var_r3;
    s32 var_r5;
    s32 var_r5_2;
    s32 var_r5_3;
    s32 var_r5_4;
    s32 var_r5_5;
    s32 var_r5_6;
    u8 *var_r4;

    var_r3 = 0;
loop_29:
    var_r5 = (s8) (u8) sel_menu_info.menuStackPtr - 1;
loop_33:
    if (var_r5 < 0) {
        var_r0 = 0;
    } else if ((s8) sel_menu_info.menuStack[var_r5] == 0x1F) {
        var_r0 = 1;
    } else {
        var_r5 -= 1;
        goto loop_33;
    }
    if (var_r0 != 0) {
        if ((s32) modeCtrl.playerCount <= 0) {
            {
                sp8 = 4;
            }
        } else {
            sp8 = 4;
        }
    } else {
        var_r5_2 = (s8) (u8) sel_menu_info.menuStackPtr - 1;
loop_47:
        if (var_r5_2 < 0) {
            var_r0_2 = 0;
        } else if ((s8) sel_menu_info.menuStack[var_r5_2] == 0x3D) {
            var_r0_2 = 1;
        } else {
            var_r5_2 -= 1;
            goto loop_47;
        }
        if (var_r0_2 != 0) {
            {
                sp8 = 2;
            }
        } else {
            var_r5_3 = (s8) (u8) sel_menu_info.menuStackPtr - 1;
loop_57:
            if (var_r5_3 < 0) {
                var_r0_3 = 0;
            } else if ((s8) sel_menu_info.menuStack[var_r5_3] == 0x42) {
                var_r0_3 = 1;
            } else {
                var_r5_3 -= 1;
                goto loop_57;
            }
            if (var_r0_3 != 0) {
                {
                    sp8 = 2;
                }
            } else {
                sp8 = 4;
            }
        }
    }
    if (var_r3 < sp8) {
        var_r5_4 = (s8) (u8) sel_menu_info.menuStackPtr - 1;
loop_5:
        if (var_r5_4 < 0) {
            var_r0_4 = 0;
        } else if ((s8) sel_menu_info.menuStack[var_r5_4] == 0x1F) {
            var_r0_4 = 1;
        } else {
            var_r5_4 -= 1;
            goto loop_5;
        }
        if (var_r0_4 != 0) {
            if (var_r3 >= (s32) modeCtrl.playerCount) {
                var_r4 = &lbl_1_bss_16F0.unk9[var_r3];
            } else {
                var_r4 = &g_focused_root_menu.unk58[var_r3];
            }
        } else {
            var_r5_5 = (s8) (u8) sel_menu_info.menuStackPtr - 1;
loop_15:
            if (var_r5_5 < 0) {
                var_r0_5 = 0;
            } else if ((s8) sel_menu_info.menuStack[var_r5_5] == 0x3D) {
                var_r0_5 = 1;
            } else {
                var_r5_5 -= 1;
                goto loop_15;
            }
            if (var_r0_5 != 0) {
                var_r4 = &lbl_1_bss_1720.unk1[var_r3];
            } else {
                var_r5_6 = (s8) (u8) sel_menu_info.menuStackPtr - 1;
loop_23:
                if (var_r5_6 < 0) {
                    var_r0_6 = 0;
                } else if ((s8) sel_menu_info.menuStack[var_r5_6] == 0x42) {
                    var_r0_6 = 1;
                } else {
                    var_r5_6 -= 1;
                    goto loop_23;
                }
                if (var_r0_6 != 0) {
                    var_r4 = &lbl_1_bss_172C.unk1[var_r3];
                } else {
                    var_r4 = &g_focused_root_menu.unk58[var_r3];
                }
            }
        }
        *(&lbl_3_bss_68 + var_r3) = *var_r4;
        var_r3 += 1;
        goto loop_29;
    }
}

void fn_3_6EC8(void) {
    s32 sp8;
    s32 var_r0;
    s32 var_r0_2;
    s32 var_r0_3;
    s32 var_r0_4;
    s32 var_r0_5;
    s32 var_r0_6;
    s32 var_r3;
    s32 var_r5;
    s32 var_r5_2;
    s32 var_r5_3;
    s32 var_r5_4;
    s32 var_r5_6;
    s32 var_r5_7;
    u8 *var_r5_5;

    var_r3 = 0;
loop_29:
    var_r5 = (s8) (u8) sel_menu_info.menuStackPtr - 1;
loop_33:
    if (var_r5 < 0) {
        var_r0 = 0;
    } else if ((s8) sel_menu_info.menuStack[var_r5] == 0x1F) {
        var_r0 = 1;
    } else {
        var_r5 -= 1;
        goto loop_33;
    }
    if (var_r0 != 0) {
        if ((s32) modeCtrl.playerCount <= 0) {
            {
                sp8 = 4;
            }
        } else {
            sp8 = 4;
        }
    } else {
        var_r5_2 = (s8) (u8) sel_menu_info.menuStackPtr - 1;
loop_47:
        if (var_r5_2 < 0) {
            var_r0_2 = 0;
        } else if ((s8) sel_menu_info.menuStack[var_r5_2] == 0x3D) {
            var_r0_2 = 1;
        } else {
            var_r5_2 -= 1;
            goto loop_47;
        }
        if (var_r0_2 != 0) {
            {
                sp8 = 2;
            }
        } else {
            var_r5_3 = (s8) (u8) sel_menu_info.menuStackPtr - 1;
loop_57:
            if (var_r5_3 < 0) {
                var_r0_3 = 0;
            } else if ((s8) sel_menu_info.menuStack[var_r5_3] == 0x42) {
                var_r0_3 = 1;
            } else {
                var_r5_3 -= 1;
                goto loop_57;
            }
            if (var_r0_3 != 0) {
                {
                    sp8 = 2;
                }
            } else {
                sp8 = 4;
            }
        }
    }
    if (var_r3 < sp8) {
        var_r5_4 = (s8) (u8) sel_menu_info.menuStackPtr - 1;
loop_5:
        if (var_r5_4 < 0) {
            var_r0_4 = 0;
        } else if ((s8) sel_menu_info.menuStack[var_r5_4] == 0x1F) {
            var_r0_4 = 1;
        } else {
            var_r5_4 -= 1;
            goto loop_5;
        }
        if (var_r0_4 != 0) {
            if (var_r3 >= (s32) modeCtrl.playerCount) {
                var_r5_5 = &lbl_1_bss_16F0.unk9[var_r3];
            } else {
                var_r5_5 = &g_focused_root_menu.unk58[var_r3];
            }
        } else {
            var_r5_6 = (s8) (u8) sel_menu_info.menuStackPtr - 1;
loop_15:
            if (var_r5_6 < 0) {
                var_r0_5 = 0;
            } else if ((s8) sel_menu_info.menuStack[var_r5_6] == 0x3D) {
                var_r0_5 = 1;
            } else {
                var_r5_6 -= 1;
                goto loop_15;
            }
            if (var_r0_5 != 0) {
                var_r5_5 = &lbl_1_bss_1720.unk1[var_r3];
            } else {
                var_r5_7 = (s8) (u8) sel_menu_info.menuStackPtr - 1;
loop_23:
                if (var_r5_7 < 0) {
                    var_r0_6 = 0;
                } else if ((s8) sel_menu_info.menuStack[var_r5_7] == 0x42) {
                    var_r0_6 = 1;
                } else {
                    var_r5_7 -= 1;
                    goto loop_23;
                }
                if (var_r0_6 != 0) {
                    var_r5_5 = &lbl_1_bss_172C.unk1[var_r3];
                } else {
                    var_r5_5 = &g_focused_root_menu.unk58[var_r3];
                }
            }
        }
        *var_r5_5 = *(&lbl_3_bss_68 + var_r3);
        var_r3 += 1;
        goto loop_29;
    }
}

void menu_character_select_tick(void) {
    struct TextBox tb;
    /* PC: one struct, not 16 separate locals; their stack order is only the GC compiler's */
    struct SpriteDrawRequest req;
    s8 sp14[4]; /* pad of each player slot */
    f32 temp_f28;
    f32 temp_f28_2;
    f32 temp_f29;
    f32 temp_f30;
    f32 temp_f31;
    s32 temp_r23;
    s32 var_r0;
    s32 var_r0_10;
    s32 var_r0_11;
    s32 var_r0_12;
    s32 var_r0_13;
    s32 var_r0_14;
    s32 var_r0_15;
    s32 var_r0_16;
    s32 var_r0_17;
    s32 var_r0_18;
    s32 var_r0_19;
    s32 var_r0_20;
    s32 var_r0_21;
    s32 var_r0_22;
    s32 var_r0_23;
    s32 var_r0_24;
    s32 var_r0_25;
    s32 var_r0_26;
    s32 var_r0_27;
    s32 var_r0_28;
    s32 var_r0_29;
    s32 var_r0_2;
    s32 var_r0_30;
    s32 var_r0_31;
    s32 var_r0_32;
    s32 var_r0_33;
    s32 var_r0_34;
    s32 var_r0_35;
    s32 var_r0_37;
    s32 var_r0_38;
    s32 var_r0_39;
    s32 var_r0_3;
    s32 var_r0_40;
    s32 var_r0_41;
    s32 var_r0_42;
    s32 var_r0_44;
    s32 var_r0_45;
    s32 var_r0_46;
    s32 var_r0_47;
    s32 var_r0_49;
    s32 var_r0_4;
    s32 var_r0_50;
    s32 var_r0_52;
    s32 var_r0_53;
    s32 var_r0_54;
    s32 var_r0_55;
    s32 var_r0_56;
    s32 var_r0_57;
    s32 var_r0_58;
    s32 var_r0_5;
    s32 var_r0_6;
    s32 var_r0_7;
    s32 var_r0_8;
    s32 var_r24_2;
    s32 var_r25;
    s32 var_r25_2;
    s32 var_r25_3;
    s32 var_r26;
    s32 var_r28;
    s32 var_r29;
    s32 var_r29_2;
    s32 var_r31;
    s32 var_r3;
    s32 var_r3_10;
    s32 var_r3_11;
    s32 var_r3_12;
    s32 var_r3_13;
    s32 var_r3_14;
    s32 var_r3_15;
    s32 var_r3_16;
    s32 var_r3_17;
    s32 var_r3_18;
    s32 var_r3_19;
    s32 var_r3_20;
    s32 var_r3_21;
    s32 var_r3_22;
    s32 var_r3_23;
    s32 var_r3_24;
    s32 var_r3_25;
    s32 var_r3_26;
    s32 var_r3_27;
    s32 var_r3_28;
    s32 var_r3_29;
    s32 var_r3_2;
    s32 var_r3_30;
    s32 var_r3_31;
    s32 var_r3_32;
    s32 var_r3_33;
    s32 var_r3_34;
    s32 var_r3_35;
    s32 var_r3_36;
    s32 var_r3_3;
    s32 var_r3_4;
    s32 var_r3_5;
    s32 var_r3_6;
    s32 var_r3_7;
    s32 var_r3_8;
    s32 var_r3_9;
    s32 var_r4;
    s32 var_r4_10;
    s32 var_r4_11;
    s32 var_r4_12;
    s32 var_r4_13;
    s32 var_r4_14;
    s32 var_r4_15;
    s32 var_r4_16;
    s32 var_r4_17;
    s32 var_r4_18;
    s32 var_r4_19;
    s32 var_r4_20;
    s32 var_r4_21;
    s32 var_r4_22;
    s32 var_r4_23;
    s32 var_r4_24;
    s32 var_r4_25;
    s32 var_r4_26;
    s32 var_r4_27;
    s32 var_r4_28;
    s32 var_r4_29;
    s32 var_r4_2;
    s32 var_r4_30;
    s32 var_r4_31;
    s32 var_r4_32;
    s32 var_r4_33;
    s32 var_r4_34;
    s32 var_r4_35;
    s32 var_r4_36;
    s32 var_r4_37;
    s32 var_r4_38;
    s32 var_r4_39;
    s32 var_r4_3;
    s32 var_r4_40;
    s32 var_r4_41;
    s32 var_r4_42;
    s32 var_r4_43;
    s32 var_r4_44;
    s32 var_r4_45;
    s32 var_r4_46;
    s32 var_r4_47;
    s32 var_r4_48;
    s32 var_r4_49;
    s32 var_r4_4;
    s32 var_r4_50;
    s32 var_r4_51;
    s32 var_r4_52;
    s32 var_r4_53;
    s32 var_r4_54;
    s32 var_r4_55;
    s32 var_r4_56;
    s32 var_r4_57;
    s32 var_r4_58;
    s32 var_r4_59;
    s32 var_r4_5;
    s32 var_r4_60;
    s32 var_r4_61;
    s32 var_r4_62;
    s32 var_r4_63;
    s32 var_r4_64;
    s32 var_r4_65;
    s32 var_r4_66;
    s32 var_r4_67;
    s32 var_r4_68;
    s32 var_r4_69;
    s32 var_r4_6;
    s32 var_r4_70;
    s32 var_r4_71;
    s32 var_r4_72;
    s32 var_r4_7;
    s32 var_r4_8;
    s32 var_r4_9;
    s32 var_r5;
    s32 var_r5_2;
    s32 var_r5_3;
    s32 var_r5_4;
    s32 var_r5_5;
    s32 var_r5_6;
    s32 var_r5_7;
    s32 var_r7;
    s32 var_r7_3;
    s32 var_r7_4;
    s8 temp_r24;
    s8 var_r0_51;
    s8 var_r0_9;
    s8 var_r24;
    s8 var_r31_2;
    s8 var_r4_73;
    struct Sprite *temp_r3_2;
    u32 temp_r0_3;
    u32 temp_r0_4;
    u32 temp_r25;
    u32 temp_r3;
    u32 temp_r3_3;
    u32 temp_r3_4;
    u32 var_r3_37;
    u8 *var_r0_36;
    u8 *var_r0_43;
    u8 *var_r0_48;
    u8 *var_r7_2;
    u8 temp_r0;
    u8 temp_r0_2;
    u8 temp_r21;
    u8 temp_r22;

    var_r4 = (s8) (u8) sel_menu_info.menuStackPtr - 1;
loop_4:
    if (var_r4 < 0) {
        var_r3 = 0;
    } else if ((s8) sel_menu_info.menuStack[var_r4] == 0xF) {
        var_r3 = 1;
    } else {
        var_r4 -= 1;
        goto loop_4;
    }
    var_r3_2 = var_r3 != 0;
    if (var_r3_2 != 0) {
        var_r4_2 = (s8) (u8) sel_menu_info.menuStackPtr - 1;
loop_11:
        if (var_r4_2 < 0) {
            var_r3_3 = 0;
        } else if ((s8) sel_menu_info.menuStack[var_r4_2] == 4) {
            var_r3_3 = 1;
        } else {
            var_r4_2 -= 1;
            goto loop_11;
        }
        var_r3_2 = var_r3_3 != 0;
    }
    var_r3_4 = var_r3_2 != 0;
    if (var_r3_4 == 0) {
        var_r4_3 = (s8) (u8) sel_menu_info.menuStackPtr - 1;
loop_19:
        if (var_r4_3 < 0) {
            var_r3_5 = 0;
        } else if ((s8) sel_menu_info.menuStack[var_r4_3] == 0x15) {
            var_r3_5 = 1;
        } else {
            var_r4_3 -= 1;
            goto loop_19;
        }
        var_r3_4 = var_r3_5 != 0;
    }
    var_r3_6 = var_r3_4 != 0;
    if (var_r3_6 == 0) {
        var_r4_4 = (s8) (u8) sel_menu_info.menuStackPtr - 1;
loop_27:
        if (var_r4_4 < 0) {
            var_r3_7 = 0;
        } else if ((s8) sel_menu_info.menuStack[var_r4_4] == 0x1C) {
            var_r3_7 = 1;
        } else {
            var_r4_4 -= 1;
            goto loop_27;
        }
        var_r3_6 = var_r3_7 != 0;
    }
    var_r3_8 = var_r3_6 != 0;
    if (var_r3_8 == 0) {
        var_r4_5 = (s8) (u8) sel_menu_info.menuStackPtr - 1;
loop_35:
        if (var_r4_5 < 0) {
            var_r3_9 = 0;
        } else if ((s8) sel_menu_info.menuStack[var_r4_5] == 0x2B) {
            var_r3_9 = 1;
        } else {
            var_r4_5 -= 1;
            goto loop_35;
        }
        var_r3_10 = var_r3_9 != 0;
        if (var_r3_10 != 0) {
            var_r4_6 = (s8) (u8) sel_menu_info.menuStackPtr - 1;
loop_42:
            if (var_r4_6 < 0) {
                var_r3_11 = 0;
            } else if ((s8) sel_menu_info.menuStack[var_r4_6] == 4) {
                var_r3_11 = 1;
            } else {
                var_r4_6 -= 1;
                goto loop_42;
            }
            var_r3_10 = var_r3_11 != 0;
        }
        var_r3_8 = var_r3_10 != 0;
    }
    var_r3_12 = var_r3_8 != 0;
    if (var_r3_12 == 0) {
        var_r4_7 = (s8) (u8) sel_menu_info.menuStackPtr - 1;
loop_51:
        if (var_r4_7 < 0) {
            var_r3_13 = 0;
        } else if ((s8) sel_menu_info.menuStack[var_r4_7] == 0x31) {
            var_r3_13 = 1;
        } else {
            var_r4_7 -= 1;
            goto loop_51;
        }
        var_r3_12 = var_r3_13 != 0;
    }
    var_r3_14 = var_r3_12 != 0;
    if (var_r3_14 == 0) {
        var_r4_8 = (s8) (u8) sel_menu_info.menuStackPtr - 1;
loop_59:
        if (var_r4_8 < 0) {
            var_r3_15 = 0;
        } else if ((s8) sel_menu_info.menuStack[var_r4_8] == 0x38) {
            var_r3_15 = 1;
        } else {
            var_r4_8 -= 1;
            goto loop_59;
        }
        var_r3_14 = var_r3_15 != 0;
    }
    var_r3_16 = var_r3_14 != 0;
    if (var_r3_16 == 0) {
        var_r4_9 = (s8) (u8) sel_menu_info.menuStackPtr - 1;
loop_67:
        if (var_r4_9 < 0) {
            var_r3_17 = 0;
        } else if ((s8) sel_menu_info.menuStack[var_r4_9] == 0x3D) {
            var_r3_17 = 1;
        } else {
            var_r4_9 -= 1;
            goto loop_67;
        }
        var_r3_18 = var_r3_17 != 0;
        if (var_r3_18 != 0) {
            var_r3_18 = (s8) lbl_1_bss_1720.unk0 != 1;
        }
        var_r3_16 = var_r3_18 != 0;
    }
    var_r3_19 = var_r3_16 != 0;
    if (var_r3_19 == 0) {
        var_r4_10 = (s8) (u8) sel_menu_info.menuStackPtr - 1;
loop_77:
        if (var_r4_10 < 0) {
            var_r3_20 = 0;
        } else if ((s8) sel_menu_info.menuStack[var_r4_10] == 0x42) {
            var_r3_20 = 1;
        } else {
            var_r4_10 -= 1;
            goto loop_77;
        }
        var_r3_21 = var_r3_20 != 0;
        if (var_r3_21 != 0) {
            var_r3_21 = (s8) lbl_1_bss_172C.unk0 == 0;
        }
        var_r3_19 = var_r3_21 != 0;
    }
    var_r3_22 = var_r3_19 != 0;
    if (var_r3_22 == 0) {
        var_r4_11 = (s8) (u8) sel_menu_info.menuStackPtr - 1;
loop_87:
        if (var_r4_11 < 0) {
            var_r3_23 = 0;
        } else if ((s8) sel_menu_info.menuStack[var_r4_11] == 0x48) {
            var_r3_23 = 1;
        } else {
            var_r4_11 -= 1;
            goto loop_87;
        }
        var_r3_22 = var_r3_23 != 0;
    }
    var_r0 = var_r3_22 != 0;
    if (var_r0 != 0) {
        var_r0 = modeCtrl.playerCount != 1;
    }
    var_r26 = var_r0;
    var_r4_12 = (s8) (u8) sel_menu_info.menuStackPtr - 1;
loop_96:
    if (var_r4_12 < 0) {
        var_r3_24 = 0;
    } else if ((s8) sel_menu_info.menuStack[var_r4_12] == 0x1F) {
        var_r3_24 = 1;
    } else {
        var_r4_12 -= 1;
        goto loop_96;
    }
    var_r3_25 = var_r3_24 != 0;
    if (var_r3_25 != 0) {
        var_r3_25 = (s8) lbl_1_bss_16F0.unk6 == 0;
    }
    var_r3_26 = var_r3_25 != 0;
    if (var_r3_26 == 0) {
        var_r4_13 = (s8) (u8) sel_menu_info.menuStackPtr - 1;
loop_105:
        if (var_r4_13 < 0) {
            var_r3_27 = 0;
        } else if ((s8) sel_menu_info.menuStack[var_r4_13] == 0x3D) {
            var_r3_27 = 1;
        } else {
            var_r4_13 -= 1;
            goto loop_105;
        }
        var_r3_28 = var_r3_27 != 0;
        if (var_r3_28 != 0) {
            var_r3_28 = (s8) lbl_1_bss_1720.unk0 != 1;
        }
        var_r3_26 = var_r3_28 != 0;
    }
    var_r3_29 = var_r3_26 != 0;
    if (var_r3_29 == 0) {
        var_r4_14 = (s8) (u8) sel_menu_info.menuStackPtr - 1;
loop_115:
        if (var_r4_14 < 0) {
            var_r3_30 = 0;
        } else if ((s8) sel_menu_info.menuStack[var_r4_14] == 0x42) {
            var_r3_30 = 1;
        } else {
            var_r4_14 -= 1;
            goto loop_115;
        }
        var_r3_31 = var_r3_30 != 0;
        if (var_r3_31 != 0) {
            var_r3_31 = (s8) lbl_1_bss_172C.unk0 == 0;
        }
        var_r3_29 = var_r3_31 != 0;
    }
    var_r0_2 = var_r3_29 != 0;
    if (var_r0_2 == 0) {
        var_r4_15 = (s8) (u8) sel_menu_info.menuStackPtr - 1;
loop_125:
        if (var_r4_15 < 0) {
            var_r3_32 = 0;
        } else if ((s8) sel_menu_info.menuStack[var_r4_15] == 0x48) {
            var_r3_32 = 1;
        } else {
            var_r4_15 -= 1;
            goto loop_125;
        }
        var_r3_33 = var_r3_32 != 0;
        if (var_r3_33 != 0) {
            var_r3_33 = (s8) lbl_1_bss_1738.unk0 != 2;
        }
        var_r0_2 = var_r3_33 != 0;
    }
    var_r3_34 = (s8) g_focused_root_menu.unk0[0] == 0;
    if (var_r3_34 == 0) {
        var_r4_16 = (s8) (u8) sel_menu_info.menuStackPtr - 1;
loop_135:
        if (var_r4_16 < 0) {
            var_r3_35 = 0;
        } else if ((s8) sel_menu_info.menuStack[var_r4_16] == 0x24) {
            var_r3_35 = 1;
        } else {
            var_r4_16 -= 1;
            goto loop_135;
        }
        var_r3_34 = var_r3_35 != 0;
    }
    var_r4_17 = var_r3_34 != 0;
    if (var_r4_17 == 0) {
        var_r4_18 = (s8) (u8) sel_menu_info.menuStackPtr - 1;
loop_143:
        if (var_r4_18 < 0) {
            var_r3_36 = 0;
        } else if ((s8) sel_menu_info.menuStack[var_r4_18] == 0x28) {
            var_r3_36 = 1;
        } else {
            var_r4_18 -= 1;
            goto loop_143;
        }
        var_r4_17 = var_r3_36 != 0;
    }
    temp_r23 = (modeCtrl.playerCount == 1) & var_r4_17;
    var_r28 = modeCtrl.playerCount;
    g_load_some_apes();
    if ((u32) (dip_switches & 0x400000) != 0) {
        var_r26 = 0;
    }
    if (var_r0_2 != 0) {
        var_r4_19 = (s8) (u8) sel_menu_info.menuStackPtr - 1;
loop_153:
        if (var_r4_19 < 0) {
            var_r0_3 = 0;
        } else if ((s8) sel_menu_info.menuStack[var_r4_19] == 0x48) {
            var_r0_3 = 1;
        } else {
            var_r4_19 -= 1;
            goto loop_153;
        }
        if ((var_r0_3 != 0) && ((s8) lbl_1_bss_1738.unk0 == 1)) {
            var_r28 = 4;
        } else {
            var_r4_20 = (s8) (u8) sel_menu_info.menuStackPtr - 1;
loop_162:
            if (var_r4_20 < 0) {
                var_r0_4 = 0;
            } else if ((s8) sel_menu_info.menuStack[var_r4_20] == 0x3D) {
                var_r0_4 = 1;
            } else {
                var_r4_20 -= 1;
                goto loop_162;
            }
            if (var_r0_4 == 0) {
                var_r4_21 = (s8) (u8) sel_menu_info.menuStackPtr - 1;
loop_169:
                if (var_r4_21 < 0) {
                    var_r0_5 = 0;
                } else if ((s8) sel_menu_info.menuStack[var_r4_21] == 0x42) {
                    var_r0_5 = 1;
                } else {
                    var_r4_21 -= 1;
                    goto loop_169;
                }
                if (var_r0_5 == 0) {
                    var_r4_22 = (s8) (u8) sel_menu_info.menuStackPtr - 1;
loop_176:
                    if (var_r4_22 < 0) {
                        var_r0_6 = 0;
                    } else if ((s8) sel_menu_info.menuStack[var_r4_22] == 0x48) {
                        var_r0_6 = 1;
                    } else {
                        var_r4_22 -= 1;
                        goto loop_176;
                    }
                    if ((var_r0_6 != 0) && ((s8) lbl_1_bss_1738.unk0 == 0)) {
                        goto block_180;
                    }
                    var_r28 = (s32) (s8) sel_menu_info.unkB;
                } else {
                    goto block_180;
                }
            } else {
block_180:
                var_r28 = 2;
            }
        }
    }
    if ((s8) (u8) g_currently_visible_menu_screen.unk8 == 0) {
        if ((s32) g_currently_visible_menu_screen.unk14 != 0) {
            var_r4_23 = 0;
loop_186:
            if (var_r4_23 < 4) {
                g_currently_visible_menu_screen.unk20[var_r4_23] = 0;
                var_r4_23 += 1;
                goto loop_186;
            }
            if ((s32) g_currently_visible_menu_screen.unk4 == 0) {
                var_r4_24 = (s8) (u8) sel_menu_info.menuStackPtr - 1;
loop_192:
                if (var_r4_24 < 0) {
                    var_r0_7 = 0;
                } else if ((s8) sel_menu_info.menuStack[var_r4_24] == 7) {
                    var_r0_7 = 1;
                } else {
                    var_r4_24 -= 1;
                    goto loop_192;
                }
                if (var_r0_7 != 0) {
                    var_r4_25 = (s8) (u8) sel_menu_info.menuStackPtr - 1;
loop_199:
                    if (var_r4_25 < 0) {
                        var_r0_8 = 0;
                    } else if ((s8) sel_menu_info.menuStack[var_r4_25] == 4) {
                        var_r0_8 = 1;
                    } else {
                        var_r4_25 -= 1;
                        goto loop_199;
                    }
                    if (var_r0_8 != 0) {
                        if (is_master_unlocked() != 0) {
                            var_r0_9 = 9;
                        } else {
                            var_r0_9 = 8;
                        }
                        g_currently_visible_menu_screen.nextScreen = var_r0_9;
                    } else {
                        goto block_206;
                    }
                } else {
block_206:
                    var_r4_26 = (s8) (u8) sel_menu_info.menuStackPtr - 1;
loop_210:
                    if (var_r4_26 < 0) {
                        var_r0_10 = 0;
                    } else if ((s8) sel_menu_info.menuStack[var_r4_26] == 7) {
                        var_r0_10 = 1;
                    } else {
                        var_r4_26 -= 1;
                        goto loop_210;
                    }
                    if (var_r0_10 != 0) {
                        g_currently_visible_menu_screen.nextScreen = 0xA;
                    } else {
                        var_r4_27 = (s8) (u8) sel_menu_info.menuStackPtr - 1;
loop_218:
                        if (var_r4_27 < 0) {
                            var_r0_11 = 0;
                        } else if ((s8) sel_menu_info.menuStack[var_r4_27] == 0xF) {
                            var_r0_11 = 1;
                        } else {
                            var_r4_27 -= 1;
                            goto loop_218;
                        }
                        if ((var_r0_11 != 0) && ((u32) (lbl_1_bss_16D0.unk8 & 8) != 0)) {
                            g_currently_visible_menu_screen.nextScreen = 0x13;
                        } else {
                            var_r4_28 = (s8) (u8) sel_menu_info.menuStackPtr - 1;
loop_227:
                            if (var_r4_28 < 0) {
                                var_r0_12 = 0;
                            } else if ((s8) sel_menu_info.menuStack[var_r4_28] == 0xF) {
                                var_r0_12 = 1;
                            } else {
                                var_r4_28 -= 1;
                                goto loop_227;
                            }
                            if ((var_r0_12 != 0) && ((u32) (lbl_1_bss_16D0.unk8 & 0x10) != 0)) {
                                g_currently_visible_menu_screen.nextScreen = 0x11;
                            } else {
                                var_r4_29 = (s8) (u8) sel_menu_info.menuStackPtr - 1;
loop_236:
                                if (var_r4_29 < 0) {
                                    var_r0_13 = 0;
                                } else if ((s8) sel_menu_info.menuStack[var_r4_29] == 0xF) {
                                    var_r0_13 = 1;
                                } else {
                                    var_r4_29 -= 1;
                                    goto loop_236;
                                }
                                if (var_r0_13 != 0) {
                                    g_currently_visible_menu_screen.nextScreen = 0x10;
                                } else {
                                    var_r4_30 = (s8) (u8) sel_menu_info.menuStackPtr - 1;
loop_244:
                                    if (var_r4_30 < 0) {
                                        var_r0_14 = 0;
                                    } else if ((s8) sel_menu_info.menuStack[var_r4_30] == 0x15) {
                                        var_r0_14 = 1;
                                    } else {
                                        var_r4_30 -= 1;
                                        goto loop_244;
                                    }
                                    if (var_r0_14 != 0) {
                                        g_currently_visible_menu_screen.nextScreen = 0x16;
                                    } else {
                                        var_r4_31 = (s8) (u8) sel_menu_info.menuStackPtr - 1;
loop_252:
                                        if (var_r4_31 < 0) {
                                            var_r0_15 = 0;
                                        } else if ((s8) sel_menu_info.menuStack[var_r4_31] == 0x1C) {
                                            var_r0_15 = 1;
                                        } else {
                                            var_r4_31 -= 1;
                                            goto loop_252;
                                        }
                                        if (var_r0_15 != 0) {
                                            g_currently_visible_menu_screen.nextScreen = 0x1D;
                                        } else {
                                            var_r4_32 = (s8) (u8) sel_menu_info.menuStackPtr - 1;
loop_260:
                                            if (var_r4_32 < 0) {
                                                var_r0_16 = 0;
                                            } else if ((s8) sel_menu_info.menuStack[var_r4_32] == 0x1F) {
                                                var_r0_16 = 1;
                                            } else {
                                                var_r4_32 -= 1;
                                                goto loop_260;
                                            }
                                            if ((var_r0_16 != 0) && ((s8) lbl_1_bss_16F0.unk6 == 0) && ((((s8) lbl_1_bss_16F0.unk7 != 1) && ((s32) modeCtrl.playerCount == 2)) || (((s8) lbl_1_bss_16F0.unk7 == 1) && ((s32) modeCtrl.playerCount == 4)))) {
                                                g_currently_visible_menu_screen.nextScreen = 0x21;
                                            } else {
                                                var_r4_33 = (s8) (u8) sel_menu_info.menuStackPtr - 1;
loop_273:
                                                if (var_r4_33 < 0) {
                                                    var_r0_17 = 0;
                                                } else if ((s8) sel_menu_info.menuStack[var_r4_33] == 0x1F) {
                                                    var_r0_17 = 1;
                                                } else {
                                                    var_r4_33 -= 1;
                                                    goto loop_273;
                                                }
                                                if (var_r0_17 != 0) {
                                                    g_currently_visible_menu_screen.nextScreen = 0x22;
                                                } else {
                                                    var_r4_34 = (s8) (u8) sel_menu_info.menuStackPtr - 1;
loop_281:
                                                    if (var_r4_34 < 0) {
                                                        var_r0_18 = 0;
                                                    } else if ((s8) sel_menu_info.menuStack[var_r4_34] == 0x3D) {
                                                        var_r0_18 = 1;
                                                    } else {
                                                        var_r4_34 -= 1;
                                                        goto loop_281;
                                                    }
                                                    if ((var_r0_18 != 0) && ((s8) lbl_1_bss_1720.unk0 == 2)) {
                                                        g_currently_visible_menu_screen.nextScreen = 0x41;
                                                        g_SoftStreamStart_with_some_defaults_2(3);
                                                        fn_1_35918(3);
                                                    } else {
                                                        var_r4_35 = (s8) (u8) sel_menu_info.menuStackPtr - 1;
loop_290:
                                                        if (var_r4_35 < 0) {
                                                            var_r0_19 = 0;
                                                        } else if ((s8) sel_menu_info.menuStack[var_r4_35] == 0x3D) {
                                                            var_r0_19 = 1;
                                                        } else {
                                                            var_r4_35 -= 1;
                                                            goto loop_290;
                                                        }
                                                        if ((var_r0_19 != 0) && ((s8) lbl_1_bss_1720.unk0 != 1) && ((s32) modeCtrl.playerCount == 2)) {
                                                            g_currently_visible_menu_screen.nextScreen = 0x3F;
                                                        } else {
                                                            var_r4_36 = (s8) (u8) sel_menu_info.menuStackPtr - 1;
loop_300:
                                                            if (var_r4_36 < 0) {
                                                                var_r0_20 = 0;
                                                            } else if ((s8) sel_menu_info.menuStack[var_r4_36] == 0x3D) {
                                                                var_r0_20 = 1;
                                                            } else {
                                                                var_r4_36 -= 1;
                                                                goto loop_300;
                                                            }
                                                            if (var_r0_20 != 0) {
                                                                g_currently_visible_menu_screen.nextScreen = 0x40;
                                                            } else {
                                                                var_r4_37 = (s8) (u8) sel_menu_info.menuStackPtr - 1;
loop_308:
                                                                if (var_r4_37 < 0) {
                                                                    var_r0_21 = 0;
                                                                } else if ((s8) sel_menu_info.menuStack[var_r4_37] == 0x24) {
                                                                    var_r0_21 = 1;
                                                                } else {
                                                                    var_r4_37 -= 1;
                                                                    goto loop_308;
                                                                }
                                                                if ((var_r0_21 != 0) && ((s8) (u8) lbl_1_bss_1700.unk0 == 2)) {
                                                                    g_currently_visible_menu_screen.nextScreen = 0x26;
                                                                } else {
                                                                    var_r4_38 = (s8) (u8) sel_menu_info.menuStackPtr - 1;
loop_317:
                                                                    if (var_r4_38 < 0) {
                                                                        var_r0_22 = 0;
                                                                    } else if ((s8) sel_menu_info.menuStack[var_r4_38] == 0x24) {
                                                                        var_r0_22 = 1;
                                                                    } else {
                                                                        var_r4_38 -= 1;
                                                                        goto loop_317;
                                                                    }
                                                                    if (var_r0_22 != 0) {
                                                                        g_currently_visible_menu_screen.nextScreen = 0x27;
                                                                    } else {
                                                                        var_r4_39 = (s8) (u8) sel_menu_info.menuStackPtr - 1;
loop_325:
                                                                        if (var_r4_39 < 0) {
                                                                            var_r0_23 = 0;
                                                                        } else if ((s8) sel_menu_info.menuStack[var_r4_39] == 0x28) {
                                                                            var_r0_23 = 1;
                                                                        } else {
                                                                            var_r4_39 -= 1;
                                                                            goto loop_325;
                                                                        }
                                                                        if (var_r0_23 != 0) {
                                                                            g_currently_visible_menu_screen.nextScreen = 0x29;
                                                                        } else {
                                                                            var_r4_40 = (s8) (u8) sel_menu_info.menuStackPtr - 1;
loop_333:
                                                                            if (var_r4_40 < 0) {
                                                                                var_r0_24 = 0;
                                                                            } else if ((s8) sel_menu_info.menuStack[var_r4_40] == 0x2B) {
                                                                                var_r0_24 = 1;
                                                                            } else {
                                                                                var_r4_40 -= 1;
                                                                                goto loop_333;
                                                                            }
                                                                            if ((var_r0_24 != 0) && ((s32) (lbl_1_bss_1708.unk4 & 0x10) != 0)) {
                                                                                g_currently_visible_menu_screen.nextScreen = 0x2F;
                                                                            } else {
                                                                                var_r4_41 = (s8) (u8) sel_menu_info.menuStackPtr - 1;
loop_342:
                                                                                if (var_r4_41 < 0) {
                                                                                    var_r0_25 = 0;
                                                                                } else if ((s8) sel_menu_info.menuStack[var_r4_41] == 0x2B) {
                                                                                    var_r0_25 = 1;
                                                                                } else {
                                                                                    var_r4_41 -= 1;
                                                                                    goto loop_342;
                                                                                }
                                                                                if ((var_r0_25 != 0) && ((s32) (lbl_1_bss_1708.unk4 & 1) != 0)) {
                                                                                    g_currently_visible_menu_screen.nextScreen = 0x2D;
                                                                                } else {
                                                                                    var_r4_42 = (s8) (u8) sel_menu_info.menuStackPtr - 1;
loop_351:
                                                                                    if (var_r4_42 < 0) {
                                                                                        var_r0_26 = 0;
                                                                                    } else if ((s8) sel_menu_info.menuStack[var_r4_42] == 0x2B) {
                                                                                        var_r0_26 = 1;
                                                                                    } else {
                                                                                        var_r4_42 -= 1;
                                                                                        goto loop_351;
                                                                                    }
                                                                                    if (var_r0_26 != 0) {
                                                                                        g_currently_visible_menu_screen.nextScreen = 0x2C;
                                                                                    } else {
                                                                                        var_r4_43 = (s8) (u8) sel_menu_info.menuStackPtr - 1;
loop_359:
                                                                                        if (var_r4_43 < 0) {
                                                                                            var_r0_27 = 0;
                                                                                        } else if ((s8) sel_menu_info.menuStack[var_r4_43] == 0x31) {
                                                                                            var_r0_27 = 1;
                                                                                        } else {
                                                                                            var_r4_43 -= 1;
                                                                                            goto loop_359;
                                                                                        }
                                                                                        if (var_r0_27 != 0) {
                                                                                            g_currently_visible_menu_screen.nextScreen = 0x32;
                                                                                        } else {
                                                                                            var_r4_44 = (s8) (u8) sel_menu_info.menuStackPtr - 1;
loop_367:
                                                                                            if (var_r4_44 < 0) {
                                                                                                var_r0_28 = 0;
                                                                                            } else if ((s8) sel_menu_info.menuStack[var_r4_44] == 0x38) {
                                                                                                var_r0_28 = 1;
                                                                                            } else {
                                                                                                var_r4_44 -= 1;
                                                                                                goto loop_367;
                                                                                            }
                                                                                            if (var_r0_28 != 0) {
                                                                                                g_currently_visible_menu_screen.nextScreen = 0x39;
                                                                                            } else {
                                                                                                var_r4_45 = (s8) (u8) sel_menu_info.menuStackPtr - 1;
loop_375:
                                                                                                if (var_r4_45 < 0) {
                                                                                                    var_r0_29 = 0;
                                                                                                } else if ((s8) sel_menu_info.menuStack[var_r4_45] == 0x48) {
                                                                                                    var_r0_29 = 1;
                                                                                                } else {
                                                                                                    var_r4_45 -= 1;
                                                                                                    goto loop_375;
                                                                                                }
                                                                                                if (var_r0_29 != 0) {
                                                                                                    g_currently_visible_menu_screen.nextScreen = 0x4C;
                                                                                                }
                                                                                            }
                                                                                        }
                                                                                    }
                                                                                }
                                                                            }
                                                                        }
                                                                    }
                                                                }
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        } else {
            if ((s32) g_currently_visible_menu_screen.unkC != 0) {
                fn_3_6B88();
                g_currently_visible_menu_screen.unk1D = 0U;
                g_currently_visible_menu_screen.unk1E = 0U;
                var_r4_46 = 0;
loop_382:
                if (var_r4_46 < 4) {
                    g_currently_visible_menu_screen.unk20[var_r4_46] = 0;
                    player_pad_map[var_r4_46] = -1;
                    var_r4_46 += 1;
                    goto loop_382;
                }
                var_r4_47 = (s8) (u8) sel_menu_info.menuStackPtr - 1;
loop_387:
                if (var_r4_47 < 0) {
                    var_r0_30 = 0;
                } else if ((s8) sel_menu_info.menuStack[var_r4_47] == 0x24) {
                    var_r0_30 = 1;
                } else {
                    var_r4_47 -= 1;
                    goto loop_387;
                }
                if ((var_r0_30 != 0) && ((s8) (u8) lbl_1_bss_1700.unk0 != 2)) {
                    menu_screen_list[(s8) g_currently_visible_menu_screen.currScreen].entries->next_screen_id = 0x27;
                } else {
                    var_r4_48 = (s8) (u8) sel_menu_info.menuStackPtr - 1;
loop_396:
                    if (var_r4_48 < 0) {
                        var_r0_31 = 0;
                    } else if ((s8) sel_menu_info.menuStack[var_r4_48] == 0x3D) {
                        var_r0_31 = 1;
                    } else {
                        var_r4_48 -= 1;
                        goto loop_396;
                    }
                    if ((var_r0_31 != 0) && ((s8) lbl_1_bss_1720.unk0 == 2)) {
                        menu_screen_list[(s8) g_currently_visible_menu_screen.currScreen].entries->next_screen_id = 0x41;
                        g_SoftStreamStart_with_some_defaults_2(3);
                        fn_1_35918(3);
                    } else {
                        var_r4_49 = (s8) (u8) sel_menu_info.menuStackPtr - 1;
loop_405:
                        if (var_r4_49 < 0) {
                            var_r0_32 = 0;
                        } else if ((s8) sel_menu_info.menuStack[var_r4_49] == 0x3D) {
                            var_r0_32 = 1;
                        } else {
                            var_r4_49 -= 1;
                            goto loop_405;
                        }
                        if (var_r0_32 != 0) {
                            menu_screen_list[(s8) g_currently_visible_menu_screen.currScreen].entries->next_screen_id = 0x3F;
                        }
                    }
                }
                g_set_some_option_menu_sprite_param();
            } else if ((s32) g_currently_visible_menu_screen.unk18 != 0) {
                fn_3_6B88();
                var_r4_50 = 0;
loop_413:
                if (var_r4_50 < 4) {
                    g_currently_visible_menu_screen.unk20[var_r4_50] = 0;
                    var_r4_50 += 1;
                    goto loop_413;
                }
                if (var_r26 != 0) {
                    g_currently_visible_menu_screen.unk1E = 0U;
                    var_r4_51 = 0;
loop_417:
                    if (var_r4_51 < var_r28) {
                        player_pad_map[var_r4_51] = -1;
                        var_r4_51 += 1;
                        goto loop_417;
                    }
                    g_currently_visible_menu_screen.unk1D = 0U;
                } else {
                    g_currently_visible_menu_screen.unk1E = (u8) (g_currently_visible_menu_screen.unk1E & ~(1 << (s8) g_currently_visible_menu_screen.unk1D));
                    player_pad_map[(s8) g_currently_visible_menu_screen.unk1D] = -1;
                }
            }
            if (var_r26 != 0) {
                if ((s8) g_currently_visible_menu_screen.unk30 == 0) {
                    var_r4_52 = 0;
                    if ((u32) (dip_switches & 0x400000) != 0) {
                        var_r4_52 = 4;
                    } else {
                        var_r5 = 0;
loop_428:
                        if (var_r5 < 4) {
                            if ((s8) (u8) pad_status_groups[var_r5].unk0[0].err == 0) {
                                var_r4_52 += 1;
                            }
                            var_r5 += 1;
                            goto loop_428;
                        }
                    }
                    if ((s8) sel_menu_info.unkB != var_r4_52) {
                        memset(&tb, 0, sizeof(tb));
                        tb.flags = 4;
                        tb.x = 0x140;
                        tb.y = 0x118;
                        tb.rows = 1;
                        tb.style = 0xD;
                        g_display_playpoint_or_gift_message_child(1, 1, &tb);
                        g_smth_with_playpoint_or_gift_msg(1, lbl_3_rodata_5C[(s8) g_language_id]);
                        g_currently_visible_menu_screen.unk30 = 0x1EU;
                        g_currently_visible_menu_screen.unk1E = 0U;
                        var_r4_53 = 0;
loop_432:
                        if (var_r4_53 < var_r28) {
                            player_pad_map[var_r4_53] = -1;
                            var_r4_53 += 1;
                            goto loop_432;
                        }
                        g_currently_visible_menu_screen.unk1D = 0U;
                    } else {
                        goto block_434;
                    }
                } else {
block_434:
                    if ((s8) g_currently_visible_menu_screen.unk30 != 0) {
                        var_r4_54 = 0;
                        if ((u32) (dip_switches & 0x400000) != 0) {
                            var_r4_54 = 4;
                        } else {
                            var_r5_2 = 0;
loop_441:
                            if (var_r5_2 < 4) {
                                if ((s8) (u8) pad_status_groups[var_r5_2].unk0[0].err == 0) {
                                    var_r4_54 += 1;
                                }
                                var_r5_2 += 1;
                                goto loop_441;
                            }
                        }
                        if ((s8) sel_menu_info.unkB == var_r4_54) {
                            if ((s8) g_currently_visible_menu_screen.unk30 > 1) {
                                g_currently_visible_menu_screen.unk30 = (u8) -0x3C;
                            } else if ((s8) g_currently_visible_menu_screen.unk30 >= 0) {
                                g_display_playpoint_or_gift_message_child(1, 0x14, NULL);
                                g_currently_visible_menu_screen.unk30 = 0U;
                            }
                        }
                    }
                }
            }
            if ((did_any_pad_press_input(7U) != 0) && ((s32) g_currently_visible_menu_screen.unkC == 0) && ((s32) g_currently_visible_menu_screen.unk18 == 0) && ((s8) g_currently_visible_menu_screen.unk1D == 0)) {
                g_b_button_pressed_on_menu(1);
            } else if ((s8) g_currently_visible_menu_screen.unk30 == 0) {
                var_r24 = 0;
                var_r29 = 0;
loop_642:
                if (var_r24 >= 4) {
                    var_r24_2 = 0;
loop_697:
                    if (var_r24_2 < 4) {
                        if ((s8) (u8) pad_status_groups[var_r24_2].unk0[0].err == 0) {
                            if ((var_r26 != 0) && ((var_r0_2 == 0) || ((s32) modeCtrl.playerCount != 1))) {
                                var_r25 = 0;
loop_683:
                                if (var_r25 >= var_r28) {

                                } else {
                                    var_r4_55 = (s8) (u8) sel_menu_info.menuStackPtr - 1;
loop_653:
                                    if (var_r4_55 < 0) {
                                        var_r0_33 = 0;
                                    } else if ((s8) sel_menu_info.menuStack[var_r4_55] == 0x48) {
                                        var_r0_33 = 1;
                                    } else {
                                        var_r4_55 -= 1;
                                        goto loop_653;
                                    }
                                    if ((var_r0_33 != 0) && ((s8) lbl_1_bss_1738.unk0 == 1)) {
                                        if (((s32) modeCtrl.playerCount == 3) && (var_r25 == 2) && ((s32) player_pad_map[3] != -1)) {
                                            var_r25 = 3;
                                        } else if (((s32) modeCtrl.playerCount == 2) && ((s8) lbl_1_bss_1738.unk1 == 0) && (var_r25 < 2) && (((s32) player_pad_map[2] != -1) || ((s32) player_pad_map[3] != -1))) {
                                            if ((s32) player_pad_map[3] != -1) {
                                                var_r25 = 3;
                                            } else {
                                                var_r25 = 2;
                                            }
                                        } else if (((s32) modeCtrl.playerCount == 2) && ((s8) lbl_1_bss_1738.unk1 == 1) && (var_r25 < 2) && ((s32) player_pad_map[var_r25 + 2] != -1) && ((s32) player_pad_map[var_r25] != -1)) {
                                            var_r25 += 2;
                                        }
                                    }
                                    if ((var_r24_2 == (s32) player_pad_map[var_r25]) && ((s32) (pad_status_groups[var_r24_2].unk0[2].button & 0x200) != 0) && ((s32) (g_currently_visible_menu_screen.unk1E & (1 << var_r25)) != 0) && ((s32) g_currently_visible_menu_screen.unkC == 0) && ((s32) g_currently_visible_menu_screen.unk18 == 0)) {
                                        call_SoundReqID_arg_0(0x66);
                                        g_currently_visible_menu_screen.unk1E = (u8) (g_currently_visible_menu_screen.unk1E & ~(1 << var_r25));
                                        g_currently_visible_menu_screen.unk1D = (u8) (g_currently_visible_menu_screen.unk1D - 1);
                                        player_pad_map[var_r25] = -1;
                                    } else {
                                        var_r25 += 1;
                                        goto loop_683;
                                    }
                                }
                            } else if ((var_r0_2 == 0) || ((s8) g_currently_visible_menu_screen.unk1D <= 0) || (var_r24_2 == (s32) player_pad_map[0])) {
                                var_r25_2 = var_r28 - 1;
loop_695:
                                if (var_r25_2 >= 0) {
                                    if (((s32) (pad_status_groups[var_r24_2].unk0[2].button & 0x200) != 0) && ((s32) (g_currently_visible_menu_screen.unk1E & (1 << var_r25_2)) != 0) && ((s32) g_currently_visible_menu_screen.unkC == 0) && ((s32) g_currently_visible_menu_screen.unk18 == 0)) {
                                        call_SoundReqID_arg_0(0x66);
                                        g_currently_visible_menu_screen.unk1E = (u8) (g_currently_visible_menu_screen.unk1E & ~(1 << var_r25_2));
                                        g_currently_visible_menu_screen.unk1D = (u8) (g_currently_visible_menu_screen.unk1D - 1);
                                        player_pad_map[var_r25_2] = -1;
                                    } else {
                                        var_r25_2 -= 1;
                                        goto loop_695;
                                    }
                                }
                            }
                        }
                        var_r24_2 += 1;
                        goto loop_697;
                    }
                } else {
                    var_r31 = 0;
                    if ((s8) (u8) pad_status_groups[var_r24].unk0[0].err == 0) {
                        var_r25_3 = 0;
loop_612:
                        if (var_r25_3 < var_r28) {
                            if ((var_r26 != 0) && ((var_r0_2 == 0) || ((s32) modeCtrl.playerCount != 1))) {
                                sp14[var_r29] = var_r24;
                                if (var_r29 < (s32) modeCtrl.playerCount) {
                                    if ((s32) player_pad_map[var_r29] != -1) {
                                        var_r4_56 = (s8) (u8) sel_menu_info.menuStackPtr - 1;
loop_465:
                                        if (var_r4_56 < 0) {
                                            var_r0_34 = 0;
                                        } else if ((s8) sel_menu_info.menuStack[var_r4_56] == 0x48) {
                                            var_r0_34 = 1;
                                        } else {
                                            var_r4_56 -= 1;
                                            goto loop_465;
                                        }
                                        if ((var_r0_34 != 0) && ((s8) lbl_1_bss_1738.unk0 == 1)) {
                                            if ((s32) modeCtrl.playerCount == 3) {
                                                switch ((s8) lbl_1_bss_1738.unk1) { /* switch 1; irregular */
                                                case 0: /* switch 1 */
                                                    if (var_r29 != 0) {
                                                        if ((((s8) lbl_1_bss_1738.unk1 == 1) && (var_r29 == 2)) || (((s8) lbl_1_bss_1738.unk1 == 2) && (var_r29 == 1))) {
                                                            goto block_476;
                                                        }
                                                    } else {
block_476:
                                                        var_r29 = 3;
                                                    }
                                                    break;
                                                }
                                            } else if (((s32) modeCtrl.playerCount == 2) && ((s8) lbl_1_bss_1738.unk1 == 0) && ((s32) player_pad_map[0] != -1) && ((s32) player_pad_map[1] != -1)) {
                                                if ((s32) player_pad_map[2] == -1) {
                                                    var_r29 = 2;
                                                } else {
                                                    var_r29 = 3;
                                                }
                                            } else if (((s32) modeCtrl.playerCount == 2) && ((s8) lbl_1_bss_1738.unk1 == 1) && ((s32) player_pad_map[var_r29] != -1)) {
                                                var_r29 += 2;
                                            }
                                            goto block_493;
                                        }
                                        goto block_611;
                                    }
                                    goto block_493;
                                }
                            } else {
                                var_r29 = var_r25_3;
                                if ((var_r29 <= (s8) g_currently_visible_menu_screen.unk1D) && ((var_r0_2 == 0) || (var_r25_3 <= (s32) (modeCtrl.playerCount - 1)) || (var_r24 == (s32) player_pad_map[0]))) {
block_493:
                                    var_r4_57 = (s8) (u8) sel_menu_info.menuStackPtr - 1;
loop_497:
                                    if (var_r4_57 < 0) {
                                        var_r0_35 = 0;
                                    } else if ((s8) sel_menu_info.menuStack[var_r4_57] == 0x1F) {
                                        var_r0_35 = 1;
                                    } else {
                                        var_r4_57 -= 1;
                                        goto loop_497;
                                    }
                                    if (var_r0_35 != 0) {
                                        if (var_r29 >= (s32) modeCtrl.playerCount) {
                                            var_r0_36 = &lbl_1_bss_16F0.unk9[var_r29];
                                        } else {
                                            var_r0_36 = &g_focused_root_menu.unk58[var_r29];
                                        }
                                    } else {
                                        var_r4_58 = (s8) (u8) sel_menu_info.menuStackPtr - 1;
loop_507:
                                        if (var_r4_58 < 0) {
                                            var_r0_37 = 0;
                                        } else if ((s8) sel_menu_info.menuStack[var_r4_58] == 0x3D) {
                                            var_r0_37 = 1;
                                        } else {
                                            var_r4_58 -= 1;
                                            goto loop_507;
                                        }
                                        if (var_r0_37 != 0) {
                                            var_r0_36 = &lbl_1_bss_1720.unk1[var_r29];
                                        } else {
                                            var_r4_59 = (s8) (u8) sel_menu_info.menuStackPtr - 1;
loop_515:
                                            if (var_r4_59 < 0) {
                                                var_r0_38 = 0;
                                            } else if ((s8) sel_menu_info.menuStack[var_r4_59] == 0x42) {
                                                var_r0_38 = 1;
                                            } else {
                                                var_r4_59 -= 1;
                                                goto loop_515;
                                            }
                                            if (var_r0_38 != 0) {
                                                var_r0_36 = &lbl_1_bss_172C.unk1[var_r29];
                                            } else {
                                                var_r0_36 = &g_focused_root_menu.unk58[var_r29];
                                            }
                                        }
                                    }
                                    if (((((s32) (pad_status_groups[var_r24].unk0[4].button & 2) != 0) || ((s32) (analog_inputs[var_r24][4] & 2) != 0)) && ((s32) (g_currently_visible_menu_screen.unk1E & (1 << var_r29)) == 0)) || ((((s32) (pad_status_groups[var_r24].unk0[4].button & 1) != 0) || ((s32) (analog_inputs[var_r24][4] & 1) != 0)) && ((s32) (g_currently_visible_menu_screen.unk1E & (1 << var_r29)) == 0))) {
                                        if ((((s32) (pad_status_groups[var_r24].unk0[4].button & 1) != 0) || ((s32) (analog_inputs[var_r24][4] & 1) != 0)) && ((s32) (g_currently_visible_menu_screen.unk1E & (1 << var_r29)) == 0)) {
                                            temp_r0 = *var_r0_36 - 1;
                                            *var_r0_36 = temp_r0;
                                            if ((s8) temp_r0 < 0) {
                                                *var_r0_36 = 3;
                                            }
                                        }
                                        if ((((s32) (pad_status_groups[var_r24].unk0[4].button & 2) != 0) || ((s32) (analog_inputs[var_r24][4] & 2) != 0)) && ((s32) (g_currently_visible_menu_screen.unk1E & (1 << var_r29)) == 0)) {
                                            temp_r0_2 = *var_r0_36 + 1;
                                            *var_r0_36 = temp_r0_2;
                                            if ((s8) temp_r0_2 > 3) {
                                                *var_r0_36 = 0;
                                            }
                                        }
                                        call_SoundReqID_arg_0(0x6C);
                                    } else if (((s32) (pad_status_groups[var_r24].unk0[2].button & 0x100) != 0) && ((s32) (g_currently_visible_menu_screen.unk1E & (1 << var_r29)) == 0) && ((s32) g_currently_visible_menu_screen.unkC == 0) && ((s32) g_currently_visible_menu_screen.unk18 == 0)) {
                                        var_r5_3 = 0;
                                        var_r4_60 = 0;
loop_573:
                                        if (var_r4_60 < 4) {
                                            if ((s32) (g_currently_visible_menu_screen.unk1E & (1 << var_r4_60)) != 0) {
                                                var_r7 = (s8) (u8) sel_menu_info.menuStackPtr - 1;
loop_547:
                                                if (var_r7 < 0) {
                                                    var_r0_39 = 0;
                                                } else if ((s8) sel_menu_info.menuStack[var_r7] == 0x1F) {
                                                    var_r0_39 = 1;
                                                } else {
                                                    var_r7 -= 1;
                                                    goto loop_547;
                                                }
                                                if (var_r0_39 != 0) {
                                                    if (var_r4_60 >= (s32) modeCtrl.playerCount) {
                                                        var_r7_2 = &lbl_1_bss_16F0.unk9[var_r4_60];
                                                    } else {
                                                        var_r7_2 = &g_focused_root_menu.unk58[var_r4_60];
                                                    }
                                                } else {
                                                    var_r7_3 = (s8) (u8) sel_menu_info.menuStackPtr - 1;
loop_557:
                                                    if (var_r7_3 < 0) {
                                                        var_r0_40 = 0;
                                                    } else if ((s8) sel_menu_info.menuStack[var_r7_3] == 0x3D) {
                                                        var_r0_40 = 1;
                                                    } else {
                                                        var_r7_3 -= 1;
                                                        goto loop_557;
                                                    }
                                                    if (var_r0_40 != 0) {
                                                        var_r7_2 = &lbl_1_bss_1720.unk1[var_r4_60];
                                                    } else {
                                                        var_r7_4 = (s8) (u8) sel_menu_info.menuStackPtr - 1;
loop_565:
                                                        if (var_r7_4 < 0) {
                                                            var_r0_41 = 0;
                                                        } else if ((s8) sel_menu_info.menuStack[var_r7_4] == 0x42) {
                                                            var_r0_41 = 1;
                                                        } else {
                                                            var_r7_4 -= 1;
                                                            goto loop_565;
                                                        }
                                                        if (var_r0_41 != 0) {
                                                            var_r7_2 = &lbl_1_bss_172C.unk1[var_r4_60];
                                                        } else {
                                                            var_r7_2 = &g_focused_root_menu.unk58[var_r4_60];
                                                        }
                                                    }
                                                }
                                                if ((s8) *var_r0_36 == (s8) *var_r7_2) {
                                                    var_r5_3 += 1;
                                                }
                                            }
                                            var_r4_60 += 1;
                                            goto loop_573;
                                        }
                                        g_currently_visible_menu_screen.unk1E = (u8) (g_currently_visible_menu_screen.unk1E | (1 << var_r29));
                                        if ((s32) g_currently_visible_menu_screen.unk20[(s8) *var_r0_36] == 0) {
                                            g_currently_visible_menu_screen.unk20[(s8) *var_r0_36] = 0x2D;
                                        } else if ((s32) g_currently_visible_menu_screen.unk20[(s8) *var_r0_36] < (s32) (var_r5_3 * 0x3C)) {
                                            g_currently_visible_menu_screen.unk20[(s8) *var_r0_36] += 0x3C;
                                        }
                                        player_pad_map[var_r29] = (s32) var_r24;
                                        if ((s8) g_currently_visible_menu_screen.unk1D == (s32) (var_r28 - 1)) {
                                            var_r4_61 = 0;
loop_608:
                                            if (var_r4_61 < 4) {
                                                var_r5_4 = (s8) (u8) sel_menu_info.menuStackPtr - 1;
loop_584:
                                                if (var_r5_4 < 0) {
                                                    var_r0_42 = 0;
                                                } else if ((s8) sel_menu_info.menuStack[var_r5_4] == 0x1F) {
                                                    var_r0_42 = 1;
                                                } else {
                                                    var_r5_4 -= 1;
                                                    goto loop_584;
                                                }
                                                if (var_r0_42 != 0) {
                                                    if (var_r4_61 >= (s32) modeCtrl.playerCount) {
                                                        var_r0_43 = &lbl_1_bss_16F0.unk9[var_r4_61];
                                                    } else {
                                                        var_r0_43 = &g_focused_root_menu.unk58[var_r4_61];
                                                    }
                                                } else {
                                                    var_r5_5 = (s8) (u8) sel_menu_info.menuStackPtr - 1;
loop_594:
                                                    if (var_r5_5 < 0) {
                                                        var_r0_44 = 0;
                                                    } else if ((s8) sel_menu_info.menuStack[var_r5_5] == 0x3D) {
                                                        var_r0_44 = 1;
                                                    } else {
                                                        var_r5_5 -= 1;
                                                        goto loop_594;
                                                    }
                                                    if (var_r0_44 != 0) {
                                                        var_r0_43 = &lbl_1_bss_1720.unk1[var_r4_61];
                                                    } else {
                                                        var_r5_6 = (s8) (u8) sel_menu_info.menuStackPtr - 1;
loop_602:
                                                        if (var_r5_6 < 0) {
                                                            var_r0_45 = 0;
                                                        } else if ((s8) sel_menu_info.menuStack[var_r5_6] == 0x42) {
                                                            var_r0_45 = 1;
                                                        } else {
                                                            var_r5_6 -= 1;
                                                            goto loop_602;
                                                        }
                                                        if (var_r0_45 != 0) {
                                                            var_r0_43 = &lbl_1_bss_172C.unk1[var_r4_61];
                                                        } else {
                                                            var_r0_43 = &g_focused_root_menu.unk58[var_r4_61];
                                                        }
                                                    }
                                                }
                                                active_monkey_id[var_r4_61] = (s32) (s8) *var_r0_43;
                                                var_r4_61 += 1;
                                                goto loop_608;
                                            }
                                            g_sel_ngc_item_selected(1);
                                            var_r31 = 1;
                                        } else {
                                            g_currently_visible_menu_screen.unk1D = (u8) (g_currently_visible_menu_screen.unk1D + 1);
                                            call_SoundReqID_arg_0(0x65);
                                        }
                                    } else {
                                        goto block_611;
                                    }
                                } else {
block_611:
                                    var_r25_3 += 1;
                                    goto loop_612;
                                }
                            }
                        }
                        var_r4_62 = (s8) (u8) sel_menu_info.menuStackPtr - 1;
loop_617:
                        if (var_r4_62 < 0) {
                            var_r0_46 = 0;
                        } else if ((s8) sel_menu_info.menuStack[var_r4_62] == 0x48) {
                            var_r0_46 = 1;
                        } else {
                            var_r4_62 -= 1;
                            goto loop_617;
                        }
                        if ((var_r0_46 != 0) && ((s8) lbl_1_bss_1738.unk0 == 1)) {
                            if ((s32) modeCtrl.playerCount == 3) {
                                if (((s8) lbl_1_bss_1738.unk1 == 0) && (var_r29 == 3)) {
                                    var_r29 = 1;
                                } else if (((s8) lbl_1_bss_1738.unk1 == 2) && (var_r29 == 3)) {
                                    var_r29 = 2;
                                } else {
                                    var_r29 += 1;
                                }
                            } else if (((s32) modeCtrl.playerCount == 2) && ((s8) lbl_1_bss_1738.unk1 == 0) && ((s32) player_pad_map[0] != -1) && ((s32) player_pad_map[1] != -1)) {
                                var_r29 = 0;
                            } else if (((s32) modeCtrl.playerCount == 2) && ((s8) lbl_1_bss_1738.unk1 == 1) && (var_r29 == 2)) {
                                var_r29 = 1;
                            } else {
                                var_r29 += 1;
                            }
                        } else {
                            var_r29 += 1;
                        }
                        if (var_r31 == 0) {
                            goto block_641;
                        }
                    } else {
block_641:
                        var_r24 += 1;
                        goto loop_642;
                    }
                }
            }
        }
    }
    g_currently_visible_menu_screen.unk38 = NULL;
    if ((s32) g_currently_visible_menu_screen.unkC == 0) {
        g_currently_visible_menu_screen.unk1C = 0U;
        set_ui_widescreen_scale_mtx(0x140);
        textdraw_reset();
        textdraw_set_alignment(4);
        req.scaleY = 1.0f;
        req.rotation = 0;
        req.unk30 = -1;
        req.flags = 0xA;
        req.mulColor = -1;
        req.addColor = 0;
        temp_r3 = (u32) (96.0 * fabs(math_sin((s16) (g_font_char_var_3 * 0x180))));
        temp_r25 = temp_r3 | ((temp_r3 << 0x10) | (temp_r3 << 8));
        var_r29_2 = 0;
loop_879:
        if (var_r29_2 < 4) {
            temp_r3_2 = get_sprite_with_unique_id(var_r29_2 + 0x3C);
            if (temp_r3_2 != NULL) {
                temp_f31 = temp_r3_2->unk58;
                temp_f30 = 9e+01f + temp_r3_2->y;
                textdraw_set_alpha(temp_r3_2->opacity * temp_r3_2->scaleX);
                req.u1 = 0.0f;
                req.v1 = 0.0f;
                req.u2 = 1.0f;
                req.v2 = 1.0f;
                req.opacity = temp_r3_2->opacity * temp_r3_2->scaleX;
                if (var_r28 > 1) {
                    req.bmpId = 0x1713;
                    req.x = temp_f31;
                    req.y = temp_f30;
                    req.z = 0.3f;
                    req.scaleX = 2.66666f * (f32) var_r28;
                    draw_sprite_draw_request(&req);
                }
                var_r31_2 = 0;
loop_877:
                if (var_r31_2 < var_r28) {
                    temp_f29 = 3e+01f;
                    temp_f28 = temp_f31 - (0.5f * (f32) (var_r28 - 1) * temp_f29);
                    textdraw_set_scale(0.8f, 0.8f);
                    req.bmpId = 0x1712;
                    temp_r3_3 = (u32) var_r31_2 >> 0x1FU;
                    temp_r0_3 = (var_r31_2 << 0x1E) - temp_r3_3;
                    req.x = temp_f28 + ((f32) (((temp_r0_3 << 2) | (temp_r0_3 >> 0x1EU)) + temp_r3_3) * temp_f29);
                    req.y = temp_f30;
                    req.z = 0.2f;
                    req.scaleX = 1.0f;
                    draw_sprite_draw_request(&req);
                    var_r4_63 = (s8) (u8) sel_menu_info.menuStackPtr - 1;
loop_708:
                    if (var_r4_63 < 0) {
                        var_r0_47 = 0;
                    } else if ((s8) sel_menu_info.menuStack[var_r4_63] == 0x1F) {
                        var_r0_47 = 1;
                    } else {
                        var_r4_63 -= 1;
                        goto loop_708;
                    }
                    if (var_r0_47 != 0) {
                        if (var_r31_2 >= (s32) modeCtrl.playerCount) {
                            var_r0_48 = &lbl_1_bss_16F0.unk9[var_r31_2];
                        } else {
                            var_r0_48 = &g_focused_root_menu.unk58[var_r31_2];
                        }
                    } else {
                        var_r4_64 = (s8) (u8) sel_menu_info.menuStackPtr - 1;
loop_718:
                        if (var_r4_64 < 0) {
                            var_r0_49 = 0;
                        } else if ((s8) sel_menu_info.menuStack[var_r4_64] == 0x3D) {
                            var_r0_49 = 1;
                        } else {
                            var_r4_64 -= 1;
                            goto loop_718;
                        }
                        if (var_r0_49 != 0) {
                            var_r0_48 = &lbl_1_bss_1720.unk1[var_r31_2];
                        } else {
                            var_r4_65 = (s8) (u8) sel_menu_info.menuStackPtr - 1;
loop_726:
                            if (var_r4_65 < 0) {
                                var_r0_50 = 0;
                            } else if ((s8) sel_menu_info.menuStack[var_r4_65] == 0x42) {
                                var_r0_50 = 1;
                            } else {
                                var_r4_65 -= 1;
                                goto loop_726;
                            }
                            if (var_r0_50 != 0) {
                                var_r0_48 = &lbl_1_bss_172C.unk1[var_r31_2];
                            } else {
                                var_r0_48 = &g_focused_root_menu.unk58[var_r31_2];
                            }
                        }
                    }
                    if ((var_r29_2 == (s8) *var_r0_48) && (((var_r26 != 0) && ((var_r0_2 == 0) || ((s32) modeCtrl.playerCount != 1))) || (var_r31_2 <= (s8) g_currently_visible_menu_screen.unk1D))) {
                        textdraw_set_depth(0.1f);
                        if (temp_r23 != 0) {
                            var_r0_51 = 3;
                        } else {
                            var_r0_51 = var_r31_2;
                        }
                        temp_r24 = (s8) lbl_3_rodata_C4[var_r0_51];
                        if ((s32) player_pad_map[var_r31_2] == -1) {
                            var_r3_37 = temp_r25;
                        } else {
                            var_r3_37 = 0;
                        }
                        textdraw_set_add_color(var_r3_37);
                        temp_r3_4 = (u32) var_r31_2 >> 0x1FU;
                        temp_r0_4 = (var_r31_2 << 0x1E) - temp_r3_4;
                        temp_f28_2 = temp_f28 + ((f32) (((temp_r0_4 << 2) | (temp_r0_4 >> 0x1EU)) + temp_r3_4) * temp_f29);
                        var_r4_66 = (s8) (u8) sel_menu_info.menuStackPtr - 1;
loop_746:
                        if (var_r4_66 < 0) {
                            var_r0_52 = 0;
                        } else if ((s8) sel_menu_info.menuStack[var_r4_66] == 0x48) {
                            var_r0_52 = 1;
                        } else {
                            var_r4_66 -= 1;
                            goto loop_746;
                        }
                        if ((var_r0_52 != 0) && ((s8) lbl_1_bss_1738.unk0 == 1) && (var_r31_2 > (s32) (modeCtrl.playerCount - 1))) {
                            textdraw_set_scale(0.4f, 0.5f);
                            if ((s32) modeCtrl.playerCount == 1) {
                                if (var_r31_2 < 2) {
                                    textdraw_set_scale(0.71999997f, 0.48000002f);
                                    textdraw_set_pos(temp_f28_2, temp_f30);
                                    textdraw_set_font(0x39);
                                    textdraw_set_mul_color(0x462308U);
                                    textdraw_printf("%d\n%d", 5, 4);
                                    textdraw_set_mul_color(0xFFFFFFU);
                                    textdraw_set_scale(0.71999997f, 0.56f);
                                    textdraw_set_pos(temp_f28_2, temp_f30);
                                    textdraw_set_font((s32) temp_r24);
                                    textdraw_set_depth(0.095f);
                                    textdraw_printf("%d\n%d", 5, 4);
                                    textdraw_set_scale(0.8f, 0.8f);
                                } else {
                                    textdraw_set_scale(0.71999997f, 0.48000002f);
                                    textdraw_set_pos(temp_f28_2, temp_f30);
                                    textdraw_set_font(0x39);
                                    textdraw_set_mul_color(0x462308U);
                                    textdraw_printf("%d\n%d", 6, 4);
                                    textdraw_set_mul_color(0xFFFFFFU);
                                    textdraw_set_scale(0.71999997f, 0.56f);
                                    textdraw_set_pos(temp_f28_2, temp_f30);
                                    textdraw_set_font((s32) temp_r24);
                                    textdraw_set_depth(0.095f);
                                    textdraw_printf("%d\n%d", 6, 4);
                                    textdraw_set_scale(0.8f, 0.8f);
                                }
                                g_currently_visible_menu_screen.unk38 = lbl_3_data_2AC0[(s8) g_language_id];
                                goto block_777;
                            }
                            if ((s32) modeCtrl.playerCount == 3) {
                                temp_r22 = lbl_1_bss_1738.unk1;
                                if ((s32) player_pad_map[(s8) lbl_3_rodata_D4[(s8) temp_r22]] != -1) {
                                    temp_r21 = lbl_3_rodata_D4[(s8) temp_r22];
                                    textdraw_set_scale(0.71999997f, 0.48000002f);
                                    textdraw_set_pos(temp_f28_2, temp_f30);
                                    textdraw_set_font(0x39);
                                    textdraw_set_mul_color(0x462308U);
                                    textdraw_printf("%d\n%d", (s8) temp_r21, 4);
                                    textdraw_set_mul_color(0xFFFFFFU);
                                    textdraw_set_scale(0.71999997f, 0.56f);
                                    textdraw_set_pos(temp_f28_2, temp_f30);
                                    textdraw_set_font((s32) temp_r24);
                                    textdraw_set_depth(0.095f);
                                    textdraw_printf("%d\n%d", (s8) temp_r21, 4);
                                    textdraw_set_scale(0.8f, 0.8f);
                                    if (((s32) player_pad_map[(s8) lbl_3_rodata_D8[(s8) temp_r22]] == -1) || ((s32) player_pad_map[(s8) lbl_3_rodata_DC[(s8) temp_r22]] == -1)) {
                                        g_currently_visible_menu_screen.unk38 = lbl_3_data_2B94[(s8) g_language_id];
                                    } else {
                                        g_currently_visible_menu_screen.unk38 = lbl_3_data_2AC0[(s8) g_language_id];
                                    }
                                    goto block_777;
                                }
                            } else if ((s8) lbl_1_bss_1738.unk1 == 0) {
                                if (((s32) player_pad_map[0] != -1) && ((s32) player_pad_map[1] != -1) && ((var_r31_2 != 3) || ((s32) player_pad_map[2] != -1))) {
                                    textdraw_set_scale(0.71999997f, 0.48000002f);
                                    textdraw_set_pos(temp_f28_2, temp_f30);
                                    textdraw_set_font(0x39);
                                    textdraw_set_mul_color(0x462308U);
                                    textdraw_printf("%d\n%d", 6, 4);
                                    textdraw_set_mul_color(0xFFFFFFU);
                                    textdraw_set_scale(0.71999997f, 0.56f);
                                    textdraw_set_pos(temp_f28_2, temp_f30);
                                    textdraw_set_font((s32) temp_r24);
                                    textdraw_set_depth(0.095f);
                                    textdraw_printf("%d\n%d", 6, 4);
                                    textdraw_set_scale(0.8f, 0.8f);
                                    goto block_773;
                                }
                            } else if (((var_r31_2 != 2) || ((s32) player_pad_map[0] != -1)) && ((var_r31_2 != 3) || ((s32) player_pad_map[1] != -1))) {
                                textdraw_set_scale(0.71999997f, 0.48000002f);
                                textdraw_set_pos(temp_f28_2, temp_f30);
                                textdraw_set_font(0x39);
                                textdraw_set_mul_color(0x462308U);
                                textdraw_printf("%d\n%d", (s8) (var_r31_2 - 2), 4);
                                textdraw_set_mul_color(0xFFFFFFU);
                                textdraw_set_scale(0.71999997f, 0.56f);
                                textdraw_set_pos(temp_f28_2, temp_f30);
                                textdraw_set_font((s32) temp_r24);
                                textdraw_set_depth(0.095f);
                                textdraw_printf("%d\n%d", (s8) (var_r31_2 - 2), 4);
                                textdraw_set_scale(0.8f, 0.8f);
block_773:
                                if (((s32) player_pad_map[0] == -1) || ((s32) player_pad_map[1] == -1)) {
                                    g_currently_visible_menu_screen.unk38 = lbl_3_data_2B94[(s8) g_language_id];
                                } else {
                                    g_currently_visible_menu_screen.unk38 = lbl_3_data_2AC0[(s8) g_language_id];
                                }
block_777:
                                g_currently_visible_menu_screen.unk1C = (u8) (g_currently_visible_menu_screen.unk1C | (1 << var_r31_2));
                            }
                        } else if ((var_r0_2 != 0) && (var_r31_2 > (s32) (modeCtrl.playerCount - 1))) {
                            textdraw_set_pos(temp_f28_2, temp_f30);
                            textdraw_set_font(0x39);
                            textdraw_set_mul_color(0x462308U);
                            textdraw_printf("%d", 4);
                            textdraw_set_mul_color(0xFFFFFFU);
                            textdraw_set_pos(temp_f28_2, temp_f30);
                            textdraw_set_font((s32) temp_r24);
                            textdraw_set_depth(0.095f);
                            textdraw_printf("%d", 4);
                            g_currently_visible_menu_screen.unk1C = (u8) (g_currently_visible_menu_screen.unk1C | (1 << var_r31_2));
                            if ((s8 *) g_currently_visible_menu_screen.unk38 == NULL) {
                                var_r4_67 = (s8) (u8) sel_menu_info.menuStackPtr - 1;
loop_785:
                                if (var_r4_67 < 0) {
                                    var_r0_53 = 0;
                                } else if ((s8) sel_menu_info.menuStack[var_r4_67] == 0x3D) {
                                    var_r0_53 = 1;
                                } else {
                                    var_r4_67 -= 1;
                                    goto loop_785;
                                }
                                if ((var_r0_53 != 0) && ((s8) lbl_1_bss_1720.unk0 == 2)) {
                                    g_currently_visible_menu_screen.unk38 = lbl_3_data_9E8C[(s8) g_language_id];
                                } else {
                                    var_r4_68 = (s8) (u8) sel_menu_info.menuStackPtr - 1;
loop_794:
                                    if (var_r4_68 < 0) {
                                        var_r0_54 = 0;
                                    } else if ((s8) sel_menu_info.menuStack[var_r4_68] == 0x3D) {
                                        var_r0_54 = 1;
                                    } else {
                                        var_r4_68 -= 1;
                                        goto loop_794;
                                    }
                                    if (var_r0_54 != 0) {
                                        g_currently_visible_menu_screen.unk38 = lbl_3_data_9D34[(s8) g_language_id];
                                    } else {
                                        var_r4_69 = (s8) (u8) sel_menu_info.menuStackPtr - 1;
loop_802:
                                        if (var_r4_69 < 0) {
                                            var_r0_55 = 0;
                                        } else if ((s8) sel_menu_info.menuStack[var_r4_69] == 0x42) {
                                            var_r0_55 = 1;
                                        } else {
                                            var_r4_69 -= 1;
                                            goto loop_802;
                                        }
                                        if (var_r0_55 != 0) {
                                            g_currently_visible_menu_screen.unk38 = lbl_3_data_A394[(s8) g_language_id];
                                        } else {
                                            g_currently_visible_menu_screen.unk38 = lbl_3_data_2AC0[(s8) g_language_id];
                                        }
                                    }
                                }
                            }
                        } else {
                            textdraw_set_pos(temp_f28_2, temp_f30);
                            textdraw_set_font(0x39);
                            textdraw_set_mul_color(0x462308U);
                            textdraw_printf("%d", var_r31_2);
                            textdraw_set_mul_color(0xFFFFFFU);
                            textdraw_set_pos(temp_f28_2, temp_f30);
                            textdraw_set_font((s32) temp_r24);
                            textdraw_set_depth(0.095f);
                            textdraw_printf("%d", var_r31_2);
                            g_currently_visible_menu_screen.unk1C = (u8) (g_currently_visible_menu_screen.unk1C | (1 << var_r31_2));
                            var_r4_70 = (s8) (u8) sel_menu_info.menuStackPtr - 1;
loop_811:
                            if (var_r4_70 < 0) {
                                var_r0_56 = 0;
                            } else if ((s8) sel_menu_info.menuStack[var_r4_70] == 0x3D) {
                                var_r0_56 = 1;
                            } else {
                                var_r4_70 -= 1;
                                goto loop_811;
                            }
                            if ((var_r0_56 != 0) && ((s32) modeCtrl.playerCount == 1) && ((s32) player_pad_map[var_r31_2] == -1)) {
                                switch (var_r29_2) { /* switch 2; irregular */
                                case 0:             /* switch 2 */
                                    g_currently_visible_menu_screen.unk38 = lbl_3_data_9A54[(s8) g_language_id];
                                    break;
                                case 1:             /* switch 2 */
                                    g_currently_visible_menu_screen.unk38 = lbl_3_data_9B1C[(s8) g_language_id];
                                    break;
                                case 2:             /* switch 2 */
                                    g_currently_visible_menu_screen.unk38 = lbl_3_data_9BEC[(s8) g_language_id];
                                    break;
                                case 3:             /* switch 2 */
                                    g_currently_visible_menu_screen.unk38 = lbl_3_data_9CD4[(s8) g_language_id];
                                    break;
                                }
                            } else {
                                var_r4_71 = (s8) (u8) sel_menu_info.menuStackPtr - 1;
loop_831:
                                if (var_r4_71 < 0) {
                                    var_r0_57 = 0;
                                } else if ((s8) sel_menu_info.menuStack[var_r4_71] == 0x3D) {
                                    var_r0_57 = 1;
                                } else {
                                    var_r4_71 -= 1;
                                    goto loop_831;
                                }
                                if ((var_r0_57 != 0) && ((s8) lbl_1_bss_1720.unk0 == 2) && ((s32) modeCtrl.playerCount == 2)) {
                                    g_currently_visible_menu_screen.unk38 = lbl_3_data_9DEC[(s8) g_language_id];
                                } else {
                                    var_r4_72 = (s8) (u8) sel_menu_info.menuStackPtr - 1;
loop_841:
                                    if (var_r4_72 < 0) {
                                        var_r0_58 = 0;
                                    } else if ((s8) sel_menu_info.menuStack[var_r4_72] == 0x24) {
                                        var_r0_58 = 1;
                                    } else {
                                        var_r4_72 -= 1;
                                        goto loop_841;
                                    }
                                    if ((var_r0_58 != 0) && ((s32) player_pad_map[var_r31_2] == -1)) {
                                        switch (var_r29_2) { /* switch 3; irregular */
                                        case 0:     /* switch 3 */
                                            if ((s8) (u8) lbl_1_bss_1700.unk0 != 2) {
                                                g_currently_visible_menu_screen.unk38 = lbl_3_data_2CD0[(s8) g_language_id];
                                            } else {
                                                g_currently_visible_menu_screen.unk38 = lbl_3_data_3184[(s8) g_language_id];
                                            }
                                            break;
                                        case 1:     /* switch 3 */
                                            if ((s8) (u8) lbl_1_bss_1700.unk0 != 2) {
                                                g_currently_visible_menu_screen.unk38 = lbl_3_data_2E04[(s8) g_language_id];
                                            } else {
                                                g_currently_visible_menu_screen.unk38 = lbl_3_data_3288[(s8) g_language_id];
                                            }
                                            break;
                                        case 2:     /* switch 3 */
                                            if ((s8) (u8) lbl_1_bss_1700.unk0 != 2) {
                                                g_currently_visible_menu_screen.unk38 = lbl_3_data_2F44[(s8) g_language_id];
                                            } else {
                                                g_currently_visible_menu_screen.unk38 = lbl_3_data_3398[(s8) g_language_id];
                                            }
                                            break;
                                        case 3:     /* switch 3 */
                                            if ((s8) (u8) lbl_1_bss_1700.unk0 != 2) {
                                                g_currently_visible_menu_screen.unk38 = lbl_3_data_3078[(s8) g_language_id];
                                            } else {
                                                g_currently_visible_menu_screen.unk38 = lbl_3_data_349C[(s8) g_language_id];
                                            }
                                            break;
                                        }
                                    }
                                }
                            }
                            if ((((s32) player_pad_map[var_r31_2] != -1) || ((var_r26 != 0) && ((var_r0_2 == 0) || (var_r31_2 <= (s32) (modeCtrl.playerCount - 1))))) && ((s32) player_pad_map[var_r31_2] >= -1) && ((s32) player_pad_map[var_r31_2] <= 3) && ((s32) g_currently_visible_menu_screen.unk4 == 0) && ((s8) g_currently_visible_menu_screen.unk30 == 0)) {
                                textdraw_set_pos(temp_f28_2, 2e+01f + temp_f30);
                                textdraw_set_font(0x34);
                                textdraw_set_scale(0.9f, 0.9f);
                                textdraw_set_add_color(0U);
                                var_r5_7 = var_r31_2 * 4;
                                if ((s32) player_pad_map[var_r31_2] == -1) {
                                    var_r4_73 = sp14[var_r31_2];
                                } else {
                                    var_r5_7 = var_r31_2 * 4;
                                    var_r4_73 = (s8) player_pad_map[var_r31_2];
                                }
                                textdraw_printf("%d", var_r4_73, var_r5_7);
                            }
                        }
                    }
                    var_r31_2 += 1;
                    goto loop_877;
                }
            }
            var_r29_2 += 1;
            goto loop_879;
        }
        reset_ui_widescreen_scale_mtx();
    }
}

void menu_gameplay_settings_tick(void) {
    struct MenuScreen *temp_r30;
    struct MenuScreen *temp_r3;
    struct MenuScreen *temp_r3_2;
    struct MenuScreen *temp_r3_3;

    temp_r30 = &menu_screen_list[(s8) g_currently_visible_menu_screen.currScreen];
    if (((s32) g_currently_visible_menu_screen.unkC != 0) || ((s32) g_currently_visible_menu_screen.unk18 != 0)) {
        g_focused_root_menu.unk0[(s8) g_currently_visible_menu_screen.currScreen] = temp_r30->entry_count;
        lbl_3_bss_6C[0] = 0;
        lbl_3_bss_6C[1] = 0;
        if ((u32) (menu_screen_list[(s8) temp_r30->entries[(s8) g_focused_root_menu.unk0[(s8) g_currently_visible_menu_screen.currScreen]].next_screen_id].bitflag & 0x100) != 0) {
            g_display_game_settings_sprite();
        }
    }
    lbl_3_bss_6C[2] = 0;
    switch ((s8) g_currently_visible_menu_screen.currScreen) { /* switch 1 */
    case 0xB:                                       /* switch 1 */
        g_handle_starting_monkeys_count(0, (s8) g_focused_root_menu.unk0[g_currently_visible_menu_screen.currScreen]);
        break;
    case 0x12:                                      /* switch 1 */
        fn_3_11840(0, (s8) g_focused_root_menu.unk0[g_currently_visible_menu_screen.currScreen]);
        break;
    case 0x13:                                      /* switch 1 */
        fn_3_11860(0, (s8) g_focused_root_menu.unk0[g_currently_visible_menu_screen.currScreen]);
        break;
    case 0x19:                                      /* switch 1 */
    case 0x1A:                                      /* switch 1 */
        fn_3_11884(0, (s8) g_focused_root_menu.unk0[g_currently_visible_menu_screen.currScreen]);
        break;
    case 0x1D:                                      /* switch 1 */
        fn_3_11BB8(0, (s8) g_focused_root_menu.unk0[g_currently_visible_menu_screen.currScreen]);
        break;
    case 0x21:                                      /* switch 1 */
    case 0x22:                                      /* switch 1 */
        fn_3_11FD4(0, (s8) g_focused_root_menu.unk0[g_currently_visible_menu_screen.currScreen]);
        break;
    case 0x29:                                      /* switch 1 */
        fn_3_123E4(0, (s8) g_focused_root_menu.unk0[g_currently_visible_menu_screen.currScreen]);
        break;
    case 0x2E:                                      /* switch 1 */
        fn_3_12BAC(0, (s8) g_focused_root_menu.unk0[g_currently_visible_menu_screen.currScreen]);
        break;
    case 0x2F:                                      /* switch 1 */
        fn_3_12BCC(0, (s8) g_focused_root_menu.unk0[g_currently_visible_menu_screen.currScreen]);
        break;
    case 0x33:                                      /* switch 1 */
    case 0x34:                                      /* switch 1 */
    case 0x35:                                      /* switch 1 */
    case 0x36:                                      /* switch 1 */
        fn_3_12BF0(0, (s8) g_focused_root_menu.unk0[g_currently_visible_menu_screen.currScreen]);
        break;
    case 0x3A:                                      /* switch 1 */
    case 0x3B:                                      /* switch 1 */
        fn_3_12FA8(0, (s8) g_focused_root_menu.unk0[g_currently_visible_menu_screen.currScreen]);
        break;
    case 0x3F:                                      /* switch 1 */
    case 0x40:                                      /* switch 1 */
        fn_3_13370(0, (s8) g_focused_root_menu.unk0[g_currently_visible_menu_screen.currScreen]);
        break;
    case 0x45:                                      /* switch 1 */
    case 0x46:                                      /* switch 1 */
        fn_3_137D4(0, (s8) g_focused_root_menu.unk0[g_currently_visible_menu_screen.currScreen]);
        break;
    case 0x4D:                                      /* switch 1 */
    case 0x4E:                                      /* switch 1 */
        fn_3_13C28(0, (s8) g_focused_root_menu.unk0[g_currently_visible_menu_screen.currScreen]);
        break;
    }
    if ((s32) lbl_3_bss_6C[0] != 0) {
        if ((did_any_pad_press_input(6U) != 0) && ((s32) g_currently_visible_menu_screen.unkC == 0) && ((s32) g_currently_visible_menu_screen.unk18 == 0)) {
            temp_r3 = &menu_screen_list[(s8) g_currently_visible_menu_screen.currScreen];
            lbl_3_bss_6C[0] = 0;
            temp_r3->bitflag &= 0xFFFFFFFB;
            call_SoundReqID_arg_0(0x65);
            return;
        }
        if ((did_any_pad_press_input(7U) != 0) && ((s32) g_currently_visible_menu_screen.unkC == 0) && ((s32) g_currently_visible_menu_screen.unk18 == 0)) {
            temp_r3_2 = &menu_screen_list[(s8) g_currently_visible_menu_screen.currScreen];
            lbl_3_bss_6C[0] = 0;
            temp_r3_2->bitflag &= 0xFFFFFFFB;
            call_SoundReqID_arg_0(0x66);
            lbl_3_bss_6C[1] = 1;
            switch ((s8) g_currently_visible_menu_screen.currScreen) { /* switch 2 */
            case 0xB:                               /* switch 2 */
                g_handle_starting_monkeys_count(0, (s8) g_focused_root_menu.unk0[g_currently_visible_menu_screen.currScreen]);
                break;
            case 0x12:                              /* switch 2 */
                fn_3_11840(0, (s8) g_focused_root_menu.unk0[g_currently_visible_menu_screen.currScreen]);
                break;
            case 0x13:                              /* switch 2 */
                fn_3_11860(0, (s8) g_focused_root_menu.unk0[g_currently_visible_menu_screen.currScreen]);
                break;
            case 0x19:                              /* switch 2 */
            case 0x1A:                              /* switch 2 */
                fn_3_11884(0, (s8) g_focused_root_menu.unk0[g_currently_visible_menu_screen.currScreen]);
                break;
            case 0x1D:                              /* switch 2 */
                fn_3_11BB8(0, (s8) g_focused_root_menu.unk0[g_currently_visible_menu_screen.currScreen]);
                break;
            case 0x21:                              /* switch 2 */
            case 0x22:                              /* switch 2 */
                fn_3_11FD4(0, (s8) g_focused_root_menu.unk0[g_currently_visible_menu_screen.currScreen]);
                break;
            case 0x29:                              /* switch 2 */
                fn_3_123E4(0, (s8) g_focused_root_menu.unk0[g_currently_visible_menu_screen.currScreen]);
                break;
            case 0x2E:                              /* switch 2 */
                fn_3_12BAC(0, (s8) g_focused_root_menu.unk0[g_currently_visible_menu_screen.currScreen]);
                break;
            case 0x2F:                              /* switch 2 */
                fn_3_12BCC(0, (s8) g_focused_root_menu.unk0[g_currently_visible_menu_screen.currScreen]);
                break;
            case 0x33:                              /* switch 2 */
            case 0x34:                              /* switch 2 */
            case 0x35:                              /* switch 2 */
            case 0x36:                              /* switch 2 */
                fn_3_12BF0(0, (s8) g_focused_root_menu.unk0[g_currently_visible_menu_screen.currScreen]);
                break;
            case 0x3A:                              /* switch 2 */
            case 0x3B:                              /* switch 2 */
                fn_3_12FA8(0, (s8) g_focused_root_menu.unk0[g_currently_visible_menu_screen.currScreen]);
                break;
            case 0x3F:                              /* switch 2 */
            case 0x40:                              /* switch 2 */
                fn_3_13370(0, (s8) g_focused_root_menu.unk0[g_currently_visible_menu_screen.currScreen]);
                break;
            case 0x45:                              /* switch 2 */
            case 0x46:                              /* switch 2 */
                fn_3_137D4(0, (s8) g_focused_root_menu.unk0[g_currently_visible_menu_screen.currScreen]);
                break;
            case 0x4D:                              /* switch 2 */
            case 0x4E:                              /* switch 2 */
                fn_3_13C28(0, (s8) g_focused_root_menu.unk0[g_currently_visible_menu_screen.currScreen]);
                break;
            }
            lbl_3_bss_6C[1] = 0;
        }
    } else if ((did_any_pad_press_input(6U) != 0) && ((s32) g_currently_visible_menu_screen.unkC == 0) && ((s32) g_currently_visible_menu_screen.unk18 == 0)) {
        if ((s8) g_focused_root_menu.unk0[(s8) g_currently_visible_menu_screen.currScreen] == (s8) temp_r30->entry_count) {
            g_sel_ngc_item_selected(1);
            return;
        }
        if ((s32) lbl_3_bss_6C[2] != 0) {
            call_SoundReqID_arg_0(0x64);
            return;
        }
        temp_r3_3 = &menu_screen_list[(s8) g_currently_visible_menu_screen.currScreen];
        lbl_3_bss_6C[0] = 1;
        temp_r3_3->bitflag |= 4;
        call_SoundReqID_arg_0(0x65);
    }
}

void menu_main_game_select_tick(void) {
    s32 var_r6;
    s32 var_r6_2;
    u8 temp_r0;
    u8 temp_r4;

    if ((g_smth_with_cm_entries_in_main_menu(0, 0, 0U) == 0) && (g_smth_with_cm_entries_in_main_menu(1, 0, 0U) == 0) && (g_smth_with_cm_entries_in_main_menu(2, 0, 0U) == 0) && (g_is_storymode_stage_unlocked(0, -1) == 0)) {
        locked_menu_items = 4;
    } else {
        locked_menu_items = 0;
    }
    if ((u32) (dip_switches & 0x10) != 0) {
        locked_menu_items = 4;
    }
    if (((s8) g_focused_root_menu.unk0[(s8) g_currently_visible_menu_screen.currScreen] == 1) && (are_all_party_games_unlocked() != 0)) {
        g_currently_visible_menu_screen.unk38 = lbl_3_data_3978[(s8) g_language_id];
    } else if (((s8) g_focused_root_menu.unk0[(s8) g_currently_visible_menu_screen.currScreen] == 2) && ((u32) (locked_menu_items & 4) != 0)) {
        g_currently_visible_menu_screen.unk38 = lbl_3_data_3A44[(s8) g_language_id];
    } else {
        g_currently_visible_menu_screen.unk38 = NULL;
    }
    if ((s32) g_currently_visible_menu_screen.unk10 == 0) {
        if (did_any_pad_press_input(8U) != 0) {
            temp_r4 = g_focused_root_menu.unk0[(s8) g_currently_visible_menu_screen.currScreen];
            if (((u32) (dip_switches & 0x10) == 0) || ((s8) temp_r4 < 6)) {
                modeCtrl.gameType = (s32) lbl_3_rodata_0[(s8) temp_r4];
                call_SoundReqID_arg_1(0x6E);
                create_how_to_sprite();
            }
        }
        return;
    }
    temp_r0 = g_focused_root_menu.unk0[(s8) g_currently_visible_menu_screen.currScreen];
    switch ((s8) temp_r0) {                         /* irregular */
    case 0:
        modeCtrl.playerCount = 1;
        *active_monkey_id = 0;
        var_r6 = 1;
loop_29:
        if (var_r6 < 4) {
            player_pad_map[var_r6] = -1;
            var_r6 += 1;
            goto loop_29;
        }
        var_r6_2 = 0;
loop_34:
        if (var_r6_2 < 4) {
            if (((s8) (u8) pad_status_groups[var_r6_2].unk0[0].err != 0) || ((s32) (pad_status_groups[var_r6_2].unk0[2].button & 0x100) == 0)) {
                var_r6_2 += 1;
                goto loop_34;
            }
        }
        *player_pad_map = var_r6_2;
        call_SoundReqID_arg_0(0x33A);
        return;
    case 1:
        call_SoundReqID_arg_0(0x33B);
        return;
    case 2:
        modeCtrl.playerCount = 1;
        in_practice_mode = 0;
        g_some_course_length = 1;
        call_SoundReqID_arg_0(0x33C);
        return;
    }
}

void menu_level_select_tick(void) {
    u8 temp_r0;

    if ((u32) (dip_switches & 0x10) != 0) {
        locked_menu_items = (s32) (locked_menu_items | 0xE);
    }
    if (get_num_unlocked_monkeys() > 3) {
        g_currently_visible_menu_screen.unk38 = lbl_3_data_3CC0[(s8) g_language_id];
        menu_screen_list[(s8) g_currently_visible_menu_screen.currScreen].entries[(s8) g_focused_root_menu.unk0[(s8) g_currently_visible_menu_screen.currScreen]].next_screen_id = 0xB;
    } else {
        menu_screen_list[(s8) g_currently_visible_menu_screen.currScreen].entries[(s8) g_focused_root_menu.unk0[(s8) g_currently_visible_menu_screen.currScreen]].next_screen_id = 0xD;
    }
    if ((s32) g_currently_visible_menu_screen.unk10 != 0) {
        temp_r0 = g_focused_root_menu.unk0[(s8) g_currently_visible_menu_screen.currScreen];
        switch ((s8) temp_r0) {                     /* irregular */
        case 0:
            call_SoundReqID_arg_0(0x216);
            return;
        case 1:
            call_SoundReqID_arg_0(0x217);
            return;
        case 2:
            call_SoundReqID_arg_0(0x218);
            return;
        case 3:
            call_SoundReqID_arg_0(0x349);
            break;
        }
    }
}

void g_get_some_challenge_mode_difficulty_info(s32 arg0) {
    u8 temp_r4;

    if (arg0 == 0) {
        temp_r4 = stageselect_is_storymode.unk1[arg0];
        if ((s8) temp_r4 == 7) {
            modeCtrl.difficulty = 2;
            modeCtrl.courseFlags |= 8;
            modeCtrl.courseFlags |= 0x10;
            modeCtrl.courseFlags |= 0x800000;
            return;
        }
        if ((s8) temp_r4 == 6) {
            modeCtrl.difficulty = 2;
            modeCtrl.courseFlags |= 8;
            modeCtrl.courseFlags |= 0x10;
            modeCtrl.courseFlags &= 0xFF7FFFFF;
            return;
        }
        if ((s8) temp_r4 >= 3) {
            modeCtrl.difficulty = stageselect_is_storymode.unk1[arg0] % 3;
            modeCtrl.courseFlags |= 8;
            modeCtrl.courseFlags &= 0xFFFFFFEF;
            modeCtrl.courseFlags &= 0xFF7FFFFF;
            return;
        }
        modeCtrl.difficulty = stageselect_is_storymode.unk1[arg0] % 3;
        modeCtrl.courseFlags &= 0xFFFFFFF7;
        modeCtrl.courseFlags &= 0xFFFFFFEF;
        modeCtrl.courseFlags &= 0xFF7FFFFF;
    }
}

void g_get_some_category_count(s32 arg0, s32 *arg1, s32 *arg2) {
    s32 var_r0;
    s32 var_r3;

    if (arg0 == 0) {
        var_r0 = 3;
    } else {
        var_r0 = 0xA;
    }
    *arg1 = var_r0;
    if (arg0 == 0) {
        var_r3 = g_get_some_difficulty_count_2(modeCtrl.difficulty, modeCtrl.courseFlags);
    } else {
        var_r3 = 0xA;
    }
    *arg2 = var_r3;
    if (arg0 == 0) {
        if (g_smth_with_cm_entries_in_main_menu(2, 0, 0x800000U) != 0) {
            *arg1 = 8;
            return;
        }
        if (g_smth_with_cm_entries_in_main_menu(2, 0, 0x10U) != 0) {
            *arg1 = 7;
            return;
        }
        if ((g_smth_with_cm_entries_in_main_menu(0, 0, 8U) != 0) || (g_smth_with_cm_entries_in_main_menu(1, 0, 8U) != 0) || (g_smth_with_cm_entries_in_main_menu(2, 0, 8U) != 0)) {
            *arg1 = 6;
        }
    }
}

void menu_stage_select_tick(void) {
    f32 var_f31;
    f32 var_f31_2;
    s32 temp_r0;
    s32 temp_r0_2;
    s32 temp_r0_3;
    s32 temp_r27;
    s32 temp_r28;
    s32 temp_r3_4;
    s32 temp_r4_3;
    s32 temp_r4_4;
    s32 temp_r4_5;
    s32 temp_r4_7;
    s32 temp_r6;
    s32 var_r25;
    s32 var_r26;
    s32 var_r26_2;
    s32 var_r26_3;
    s32 var_r26_4;
    s32 var_r26_5;
    s32 var_r27;
    s32 var_r27_2;
    s32 var_r29_2;
    s32 var_r29_3;
    s32 var_r3;
    s32 var_r3_10;
    s32 var_r3_11;
    s32 var_r3_2;
    s32 var_r3_3;
    s32 var_r3_4;
    s32 var_r3_5;
    s32 var_r3_6;
    s32 var_r3_7;
    s32 var_r3_8;
    s32 var_r3_9;
    s32 var_r4;
    s32 var_r6;
    s8 temp_r24;
    s8 temp_r24_2;
    s8 temp_r24_3;
    s8 temp_r24_4;
    s8 temp_r24_5;
    s8 var_r24;
    s8 var_r24_2;
    s8 var_r29;
    u32 var_r3_12;
    u32 var_r3_13;
    u8 temp_r24_6;
    u8 temp_r25;
    u8 temp_r25_2;
    u8 temp_r26;
    u8 temp_r26_2;
    u8 temp_r26_3;
    u8 temp_r4;
    u8 temp_r4_2;
    u8 temp_r4_6;
    u8 temp_r4_8;
    u8 temp_r5;

    if ((s32) g_currently_visible_menu_screen.unkC != 0) {
        var_r29 = (s8) stageselect_is_storymode.unk0;
        modeCtrl.submodeTimer = 2;
        call_SoundReqID_arg_0(0x23D);
        stageselect_is_storymode.unk17 = (u8) (stageselect_is_storymode.unk17 & 0xFFFFFFFE);
        modeCtrl.difficulty = 0;
        modeCtrl.courseFlags = 0;
        stageselect_is_storymode.unk0 = 0;
        if ((s8) stageselect_is_storymode.unk1[0] == 7) {
            modeCtrl.difficulty = 2;
            modeCtrl.courseFlags |= 8;
            modeCtrl.courseFlags |= 0x10;
            modeCtrl.courseFlags |= 0x800000;
        } else if ((s8) stageselect_is_storymode.unk1[0] == 6) {
            modeCtrl.difficulty = 2;
            modeCtrl.courseFlags |= 8;
            modeCtrl.courseFlags |= 0x10;
            modeCtrl.courseFlags &= 0xFF7FFFFF;
        } else if ((s8) stageselect_is_storymode.unk1[0] >= 3) {
            modeCtrl.difficulty = stageselect_is_storymode.unk1[0] % 3;
            modeCtrl.courseFlags |= 8;
            modeCtrl.courseFlags &= 0xFFFFFFEF;
            modeCtrl.courseFlags &= 0xFF7FFFFF;
        } else {
            modeCtrl.difficulty = stageselect_is_storymode.unk1[0] % 3;
            modeCtrl.courseFlags &= 0xFFFFFFF7;
            modeCtrl.courseFlags &= 0xFFFFFFEF;
            modeCtrl.courseFlags &= 0xFF7FFFFF;
        }
        g_get_some_difficulty_count_2(modeCtrl.difficulty, modeCtrl.courseFlags);
        if (g_smth_with_cm_entries_in_main_menu(2, 0, 0x800000U) != 0) {

        } else if (g_smth_with_cm_entries_in_main_menu(2, 0, 0x10U) != 0) {

        } else if ((g_smth_with_cm_entries_in_main_menu(0, 0, 8U) == 0) && (g_smth_with_cm_entries_in_main_menu(1, 0, 8U) == 0) && (g_smth_with_cm_entries_in_main_menu(2, 0, 8U) != 0)) {

        }
        if (g_smth_with_cm_entries_in_main_menu(modeCtrl.difficulty, (s8) stageselect_is_storymode.unk3[0][(s8) stageselect_is_storymode.unk1[0]] + 1, modeCtrl.courseFlags) == 0) {
            if (g_smth_with_cm_entries_in_main_menu(0, 0, 0U) != 0) {
                stageselect_is_storymode.unk1[0] = 0U;
            } else if (g_smth_with_cm_entries_in_main_menu(1, 0, 0U) != 0) {
                stageselect_is_storymode.unk1[0] = 1U;
            } else if (g_smth_with_cm_entries_in_main_menu(2, 0, 0U) != 0) {
                stageselect_is_storymode.unk1[0] = 2U;
            }
            if ((s8) stageselect_is_storymode.unk1[0] == 7) {
                modeCtrl.difficulty = 2;
                modeCtrl.courseFlags |= 8;
                modeCtrl.courseFlags |= 0x10;
                modeCtrl.courseFlags |= 0x800000;
            } else if ((s8) stageselect_is_storymode.unk1[0] == 6) {
                modeCtrl.difficulty = 2;
                modeCtrl.courseFlags |= 8;
                modeCtrl.courseFlags |= 0x10;
                modeCtrl.courseFlags &= 0xFF7FFFFF;
            } else if ((s8) stageselect_is_storymode.unk1[0] >= 3) {
                modeCtrl.difficulty = stageselect_is_storymode.unk1[0] % 3;
                modeCtrl.courseFlags |= 8;
                modeCtrl.courseFlags &= 0xFFFFFFEF;
                modeCtrl.courseFlags &= 0xFF7FFFFF;
            } else {
                modeCtrl.difficulty = stageselect_is_storymode.unk1[0] % 3;
                modeCtrl.courseFlags &= 0xFFFFFFF7;
                modeCtrl.courseFlags &= 0xFFFFFFEF;
                modeCtrl.courseFlags &= 0xFF7FFFFF;
            }
            temp_r27 = g_get_some_difficulty_count_2(modeCtrl.difficulty, modeCtrl.courseFlags);
            if (g_smth_with_cm_entries_in_main_menu(2, 0, 0x800000U) != 0) {

            } else if (g_smth_with_cm_entries_in_main_menu(2, 0, 0x10U) != 0) {

            } else if ((g_smth_with_cm_entries_in_main_menu(0, 0, 8U) == 0) && (g_smth_with_cm_entries_in_main_menu(1, 0, 8U) == 0) && (g_smth_with_cm_entries_in_main_menu(2, 0, 8U) != 0)) {

            }
            var_r26 = 0;
loop_38:
            if (var_r26 < 3) {
                stageselect_is_storymode.unk3[0][var_r26] = 0;
                var_r26 += 1;
                goto loop_38;
            }
            var_r24 = 0;
loop_42:
            if (var_r24 < temp_r27) {
                if (g_smth_with_cm_entries_in_main_menu(modeCtrl.difficulty, var_r24 + 1, modeCtrl.courseFlags) == 0) {
                    var_r24 += 1;
                    goto loop_42;
                }
            }
            stageselect_is_storymode.unk3[0][(s8) stageselect_is_storymode.unk1[0]] = var_r24;
            if ((s8) stageselect_is_storymode.unk1[0] == 7) {
                modeCtrl.difficulty = 2;
                modeCtrl.courseFlags |= 8;
                modeCtrl.courseFlags |= 0x10;
                modeCtrl.courseFlags |= 0x800000;
            } else if ((s8) stageselect_is_storymode.unk1[0] == 6) {
                modeCtrl.difficulty = 2;
                modeCtrl.courseFlags |= 8;
                modeCtrl.courseFlags |= 0x10;
                modeCtrl.courseFlags &= 0xFF7FFFFF;
            } else if ((s8) stageselect_is_storymode.unk1[0] >= 3) {
                modeCtrl.difficulty = stageselect_is_storymode.unk1[0] % 3;
                modeCtrl.courseFlags |= 8;
                modeCtrl.courseFlags &= 0xFFFFFFEF;
                modeCtrl.courseFlags &= 0xFF7FFFFF;
            } else {
                modeCtrl.difficulty = stageselect_is_storymode.unk1[0] % 3;
                modeCtrl.courseFlags &= 0xFFFFFFF7;
                modeCtrl.courseFlags &= 0xFFFFFFEF;
                modeCtrl.courseFlags &= 0xFF7FFFFF;
            }
            g_get_some_difficulty_count_2(modeCtrl.difficulty, modeCtrl.courseFlags);
            if (g_smth_with_cm_entries_in_main_menu(2, 0, 0x800000U) != 0) {

            } else if (g_smth_with_cm_entries_in_main_menu(2, 0, 0x10U) != 0) {

            } else if ((g_smth_with_cm_entries_in_main_menu(0, 0, 8U) == 0) && (g_smth_with_cm_entries_in_main_menu(1, 0, 8U) == 0) && (g_smth_with_cm_entries_in_main_menu(2, 0, 8U) != 0)) {

            }
        }
        if (g_is_storymode_stage_unlocked((s32) (s8) stageselect_is_storymode.unk1[1], (s32) (s8) stageselect_is_storymode.unk3[1][(s8) stageselect_is_storymode.unk1[1]]) == 0) {
            var_r25 = 0;
            var_r24_2 = 0xA;
loop_66:
            if (var_r24_2 >= 0) {
                stageselect_is_storymode.unk1[1] = (u8) var_r24_2;
                var_r26_2 = 0xA;
loop_63:
                if (var_r26_2 >= 0) {
                    if (g_is_storymode_stage_unlocked((s32) (s8) stageselect_is_storymode.unk1[1], var_r26_2) != 0) {
                        stageselect_is_storymode.unk3[1][(s8) stageselect_is_storymode.unk1[1]] = (s8) var_r26_2;
                        var_r25 = 1;
                    } else {
                        var_r26_2 -= 1;
                        goto loop_63;
                    }
                }
                if (var_r25 == 0) {
                    var_r24_2 -= 1;
                    goto loop_66;
                }
            }
            if (g_is_storymode_stage_unlocked((s32) (s8) stageselect_is_storymode.unk1[1], -1) == 0) {
                var_r29 = 0;
            }
        }
        stageselect_is_storymode.unk0 = (u8) var_r29;
        if ((s8) stageselect_is_storymode.unk0 == 0) {
            temp_r4 = stageselect_is_storymode.unk1[(s8) stageselect_is_storymode.unk0];
            if ((s8) temp_r4 == 7) {
                modeCtrl.difficulty = 2;
                modeCtrl.courseFlags |= 8;
                modeCtrl.courseFlags |= 0x10;
                modeCtrl.courseFlags |= 0x800000;
            } else if ((s8) temp_r4 == 6) {
                modeCtrl.difficulty = 2;
                modeCtrl.courseFlags |= 8;
                modeCtrl.courseFlags |= 0x10;
                modeCtrl.courseFlags &= 0xFF7FFFFF;
            } else if ((s8) temp_r4 >= 3) {
                modeCtrl.difficulty = stageselect_is_storymode.unk1[(s8) stageselect_is_storymode.unk0] % 3;
                modeCtrl.courseFlags |= 8;
                modeCtrl.courseFlags &= 0xFFFFFFEF;
                modeCtrl.courseFlags &= 0xFF7FFFFF;
            } else {
                modeCtrl.difficulty = stageselect_is_storymode.unk1[(s8) stageselect_is_storymode.unk0] % 3;
                modeCtrl.courseFlags &= 0xFFFFFFF7;
                modeCtrl.courseFlags &= 0xFFFFFFEF;
                modeCtrl.courseFlags &= 0xFF7FFFFF;
            }
        }
        temp_r24 = (s8) stageselect_is_storymode.unk0;
        if (temp_r24 == 0) {

        }
        if (temp_r24 == 0) {
            var_r3 = g_get_some_difficulty_count_2(modeCtrl.difficulty, modeCtrl.courseFlags);
        } else {
            var_r3 = 0xA;
        }
        if (temp_r24 == 0) {
            var_r3 = g_smth_with_cm_entries_in_main_menu(2, 0, 0x800000U);
            if (var_r3 != 0) {

            } else {
                var_r3 = g_smth_with_cm_entries_in_main_menu(2, 0, 0x10U);
                if (var_r3 != 0) {

                } else {
                    var_r3 = g_smth_with_cm_entries_in_main_menu(0, 0, 8U);
                    if (var_r3 == 0) {
                        var_r3 = g_smth_with_cm_entries_in_main_menu(1, 0, 8U);
                        if (var_r3 == 0) {
                            var_r3 = g_smth_with_cm_entries_in_main_menu(2, 0, 8U);
                            if (var_r3 != 0) {

                            }
                        }
                    }
                }
            }
        }
        create_practice_mode_stage_select_sprite();
        create_practice_mode_preview_sprites();
        if ((s8) stageselect_is_storymode.unk0 == 1) {
            var_r3_2 = get_story_mode_stage_id((s32) (s8) stageselect_is_storymode.unk1[(s8) stageselect_is_storymode.unk0], (s32) (s8) stageselect_is_storymode.unk3[(s8) stageselect_is_storymode.unk0][(s8) stageselect_is_storymode.unk1[(s8) stageselect_is_storymode.unk0]]);
        } else {
            var_r3_2 = fn_1_A3C84(modeCtrl.difficulty, (s32) (s8) stageselect_is_storymode.unk3[(s8) stageselect_is_storymode.unk0][(s8) stageselect_is_storymode.unk1[(s8) stageselect_is_storymode.unk0]], modeCtrl.courseFlags);
        }
        read_stage_name_from_dvd(var_r3_2, lbl_3_bss_78, 0x80);
        goto block_96;
    }
    if ((s32) g_currently_visible_menu_screen.unk14 != 0) {
        fn_3_1E1AC();
        return;
    }
block_96:
    if ((s32) modeCtrl.submodeTimer > 0) {
        temp_r0 = modeCtrl.submodeTimer - 1;
        modeCtrl.submodeTimer = temp_r0;
        if (temp_r0 == 0) {
            if ((s8) stageselect_is_storymode.unk0 == 0) {
                temp_r4_2 = stageselect_is_storymode.unk1[(s8) stageselect_is_storymode.unk0];
                if ((s8) temp_r4_2 == 7) {
                    modeCtrl.difficulty = 2;
                    modeCtrl.courseFlags |= 8;
                    modeCtrl.courseFlags |= 0x10;
                    modeCtrl.courseFlags |= 0x800000;
                } else if ((s8) temp_r4_2 == 6) {
                    modeCtrl.difficulty = 2;
                    modeCtrl.courseFlags |= 8;
                    modeCtrl.courseFlags |= 0x10;
                    modeCtrl.courseFlags &= 0xFF7FFFFF;
                } else if ((s8) temp_r4_2 >= 3) {
                    modeCtrl.difficulty = stageselect_is_storymode.unk1[(s8) stageselect_is_storymode.unk0] % 3;
                    modeCtrl.courseFlags |= 8;
                    modeCtrl.courseFlags &= 0xFFFFFFEF;
                    modeCtrl.courseFlags &= 0xFF7FFFFF;
                } else {
                    modeCtrl.difficulty = stageselect_is_storymode.unk1[(s8) stageselect_is_storymode.unk0] % 3;
                    modeCtrl.courseFlags &= 0xFFFFFFF7;
                    modeCtrl.courseFlags &= 0xFFFFFFEF;
                    modeCtrl.courseFlags &= 0xFF7FFFFF;
                }
            }
            temp_r24_2 = (s8) stageselect_is_storymode.unk0;
            if (temp_r24_2 == 0) {

            }
            if (temp_r24_2 == 0) {
                g_get_some_difficulty_count_2(modeCtrl.difficulty, modeCtrl.courseFlags);
            }
            if (temp_r24_2 == 0) {
                if (g_smth_with_cm_entries_in_main_menu(2, 0, 0x800000U) != 0) {

                } else if (g_smth_with_cm_entries_in_main_menu(2, 0, 0x10U) != 0) {

                } else if ((g_smth_with_cm_entries_in_main_menu(0, 0, 8U) == 0) && (g_smth_with_cm_entries_in_main_menu(1, 0, 8U) == 0) && (g_smth_with_cm_entries_in_main_menu(2, 0, 8U) != 0)) {

                }
            }
            if ((s8) stageselect_is_storymode.unk0 == 1) {
                var_r3_3 = get_story_mode_stage_id((s32) (s8) stageselect_is_storymode.unk1[(s8) stageselect_is_storymode.unk0], (s32) (s8) stageselect_is_storymode.unk3[(s8) stageselect_is_storymode.unk0][(s8) stageselect_is_storymode.unk1[(s8) stageselect_is_storymode.unk0]]);
            } else {
                var_r3_3 = fn_1_A3C84(modeCtrl.difficulty, (s32) (s8) stageselect_is_storymode.unk3[(s8) stageselect_is_storymode.unk0][(s8) stageselect_is_storymode.unk1[(s8) stageselect_is_storymode.unk0]], modeCtrl.courseFlags);
            }
            read_stage_name_from_dvd(var_r3_3, lbl_3_bss_78, 0x80);
        }
    }
    temp_r24_3 = (s8) stageselect_is_storymode.unk0;
    if (temp_r24_3 == 0) {
        var_r29_2 = 3;
    } else {
        var_r29_2 = 0xA;
    }
    if (temp_r24_3 == 0) {
        var_r3_4 = g_get_some_difficulty_count_2(modeCtrl.difficulty, modeCtrl.courseFlags);
    } else {
        var_r3_4 = 0xA;
    }
    var_r27 = var_r3_4;
    if (temp_r24_3 == 0) {
        if (g_smth_with_cm_entries_in_main_menu(2, 0, 0x800000U) != 0) {
            var_r29_2 = 8;
        } else if (g_smth_with_cm_entries_in_main_menu(2, 0, 0x10U) != 0) {
            var_r29_2 = 7;
        } else if ((g_smth_with_cm_entries_in_main_menu(0, 0, 8U) != 0) || (g_smth_with_cm_entries_in_main_menu(1, 0, 8U) != 0) || (g_smth_with_cm_entries_in_main_menu(2, 0, 8U) != 0)) {
            var_r29_2 = 6;
        }
    }
    if ((did_any_pad_press_input(6U) != 0) && ((s32) g_currently_visible_menu_screen.unkC == 0) && ((s32) g_currently_visible_menu_screen.unk18 == 0)) {
        var_r26_3 = 0;
        if ((s32) (stageselect_is_storymode.unk17 & 1) == 0) {
            if ((s8) stageselect_is_storymode.unk0 == 0) {
                if (g_smth_with_cm_entries_in_main_menu(modeCtrl.difficulty, 0, modeCtrl.courseFlags) != 0) {
                    var_r26_3 = 1;
                }
            } else if (g_is_storymode_stage_unlocked((s32) (s8) stageselect_is_storymode.unk1[(s8) stageselect_is_storymode.unk0], -1) != 0) {
                var_r26_3 = 1;
            }
            if (var_r26_3 != 0) {
                stageselect_is_storymode.unk17 = (u8) (stageselect_is_storymode.unk17 | 1);
                call_SoundReqID_arg_0(0x65);
                if ((s8) stageselect_is_storymode.unk0 == 1) {
                    var_r3_5 = get_story_mode_stage_id((s32) (s8) stageselect_is_storymode.unk1[(s8) stageselect_is_storymode.unk0], (s32) (s8) stageselect_is_storymode.unk3[(s8) stageselect_is_storymode.unk0][(s8) stageselect_is_storymode.unk1[(s8) stageselect_is_storymode.unk0]]);
                } else {
                    var_r3_5 = fn_1_A3C84(modeCtrl.difficulty, (s32) (s8) stageselect_is_storymode.unk3[(s8) stageselect_is_storymode.unk0][(s8) stageselect_is_storymode.unk1[(s8) stageselect_is_storymode.unk0]], modeCtrl.courseFlags);
                }
                read_stage_name_from_dvd(var_r3_5, lbl_3_bss_78, 0x80);
            } else {
                call_SoundReqID_arg_0(0x64);
            }
        } else {
            if ((s8) stageselect_is_storymode.unk0 == 0) {
                if (g_smth_with_cm_entries_in_main_menu(modeCtrl.difficulty, (s8) stageselect_is_storymode.unk3[(s8) stageselect_is_storymode.unk0][(s8) stageselect_is_storymode.unk1[(s8) stageselect_is_storymode.unk0]] + 1, modeCtrl.courseFlags) != 0) {
                    var_r26_3 = 1;
                }
            } else if (g_is_storymode_stage_unlocked((s32) (s8) stageselect_is_storymode.unk1[(s8) stageselect_is_storymode.unk0], (s32) (s8) stageselect_is_storymode.unk3[(s8) stageselect_is_storymode.unk0][(s8) stageselect_is_storymode.unk1[(s8) stageselect_is_storymode.unk0]]) != 0) {
                var_r26_3 = 1;
            }
            if (var_r26_3 != 0) {
                g_sel_ngc_item_selected(1);
            } else {
                call_SoundReqID_arg_0(0x64);
            }
        }
    } else if ((did_any_pad_press_input(7U) != 0) && ((s32) g_currently_visible_menu_screen.unkC == 0) && ((s32) g_currently_visible_menu_screen.unk18 == 0)) {
        if ((s32) (stageselect_is_storymode.unk17 & 1) == 0) {
            g_b_button_pressed_on_menu(1);
        } else {
            stageselect_is_storymode.unk17 = (u8) (stageselect_is_storymode.unk17 & 0xFFFFFFFE);
            call_SoundReqID_arg_0(0x66);
        }
    } else if ((did_any_pad_press_input(3U) != 0) || (did_any_pad_press_input(2U) != 0)) {
        if ((s32) (stageselect_is_storymode.unk17 & 1) != 0) {
            temp_r26 = stageselect_is_storymode.unk1[(s8) stageselect_is_storymode.unk0];
            temp_r25 = stageselect_is_storymode.unk3[(s8) stageselect_is_storymode.unk0][(s8) temp_r26];
            temp_r4_3 = ((s8) stageselect_is_storymode.unk0 * 0xA) + (s8) temp_r26;
            temp_r4_4 = var_r27 - 1 + 1;
            if (did_any_pad_press_input(3U) != 0) {
                stageselect_is_storymode.unk3[0][temp_r4_3] = (u8) (s8) (stageselect_is_storymode.unk3[0][temp_r4_3] + 0xA);
                stageselect_is_storymode.unk3[0][temp_r4_3] = (u8) (s8) (stageselect_is_storymode.unk3[0][temp_r4_3] % temp_r4_4);
                stageselect_is_storymode.unk3[0][temp_r4_3] = (u8) (s8) stageselect_is_storymode.unk3[0][temp_r4_3];
            } else {
                stageselect_is_storymode.unk3[0][temp_r4_3] = (u8) (s8) (stageselect_is_storymode.unk3[0][temp_r4_3] - 0xA);
                if ((s8) stageselect_is_storymode.unk3[0][temp_r4_3] < 0) {
                    stageselect_is_storymode.unk3[0][temp_r4_3] = (u8) (s8) (stageselect_is_storymode.unk3[0][temp_r4_3] + temp_r4_4);
                }
                stageselect_is_storymode.unk3[0][temp_r4_3] = (u8) (s8) (stageselect_is_storymode.unk3[0][temp_r4_3] % temp_r4_4);
                stageselect_is_storymode.unk3[0][temp_r4_3] = (u8) (s8) stageselect_is_storymode.unk3[0][temp_r4_3];
            }
            if ((s8) temp_r25 != (s8) stageselect_is_storymode.unk3[(s8) stageselect_is_storymode.unk0][(s8) temp_r26]) {
                call_SoundReqID_arg_0(0x6C);
                if ((s8) stageselect_is_storymode.unk0 == 1) {
                    var_r3_6 = get_story_mode_stage_id((s32) (s8) stageselect_is_storymode.unk1[(s8) stageselect_is_storymode.unk0], (s32) (s8) stageselect_is_storymode.unk3[(s8) stageselect_is_storymode.unk0][(s8) stageselect_is_storymode.unk1[(s8) stageselect_is_storymode.unk0]]);
                } else {
                    var_r3_6 = fn_1_A3C84(modeCtrl.difficulty, (s32) (s8) stageselect_is_storymode.unk3[(s8) stageselect_is_storymode.unk0][(s8) stageselect_is_storymode.unk1[(s8) stageselect_is_storymode.unk0]], modeCtrl.courseFlags);
                }
                read_stage_name_from_dvd(var_r3_6, lbl_3_bss_78, 0x80);
            }
        }
    } else if ((did_any_pad_press_input(1U) != 0) || (did_any_pad_press_input(0U) != 0) || (did_any_pad_press_input(4U) != 0) || (did_any_pad_press_input(5U) != 0)) {
        if (((s32) (stageselect_is_storymode.unk17 & 1) == 0) || (did_any_pad_press_input(4U) != 0) || (did_any_pad_press_input(5U) != 0)) {
            temp_r26_2 = stageselect_is_storymode.unk1[(s8) stageselect_is_storymode.unk0];
            var_r27_2 = 0;
            if ((did_any_pad_press_input(1U) != 0) || (did_any_pad_press_input(5U) != 0)) {
                var_r27_2 = 1;
            }
            temp_r4_5 = var_r29_2 - 1;
            if ((s32) (temp_r4_5 + 1) < var_r29_2) {
                var_r6 = temp_r4_5 + 1;
            } else {
                var_r6 = var_r29_2;
            }
            temp_r0_2 = var_r29_2 * ((s8) stageselect_is_storymode.unk1[(s8) stageselect_is_storymode.unk0] / var_r29_2);
            if (var_r27_2 != 0) {
                stageselect_is_storymode.unk1[(s8) stageselect_is_storymode.unk0] = (u8) (s8) (stageselect_is_storymode.unk1[(s8) stageselect_is_storymode.unk0] + 1);
                stageselect_is_storymode.unk1[(s8) stageselect_is_storymode.unk0] = (u8) (s8) (stageselect_is_storymode.unk1[(s8) stageselect_is_storymode.unk0] % var_r6);
                stageselect_is_storymode.unk1[(s8) stageselect_is_storymode.unk0] = (u8) (s8) (stageselect_is_storymode.unk1[(s8) stageselect_is_storymode.unk0] + temp_r0_2);
            } else {
                stageselect_is_storymode.unk1[(s8) stageselect_is_storymode.unk0] = (u8) (s8) (stageselect_is_storymode.unk1[(s8) stageselect_is_storymode.unk0] - 1);
                if ((s8) stageselect_is_storymode.unk1[(s8) stageselect_is_storymode.unk0] < 0) {
                    stageselect_is_storymode.unk1[(s8) stageselect_is_storymode.unk0] = (u8) (s8) (stageselect_is_storymode.unk1[(s8) stageselect_is_storymode.unk0] + var_r6);
                }
                stageselect_is_storymode.unk1[(s8) stageselect_is_storymode.unk0] = (u8) (s8) (stageselect_is_storymode.unk1[(s8) stageselect_is_storymode.unk0] % var_r6);
                stageselect_is_storymode.unk1[(s8) stageselect_is_storymode.unk0] = (u8) (s8) (stageselect_is_storymode.unk1[(s8) stageselect_is_storymode.unk0] + temp_r0_2);
            }
            if ((s8) stageselect_is_storymode.unk0 == 0) {
                temp_r4_6 = stageselect_is_storymode.unk1[(s8) stageselect_is_storymode.unk0];
                if ((s8) temp_r4_6 == 7) {
                    modeCtrl.difficulty = 2;
                    modeCtrl.courseFlags |= 8;
                    modeCtrl.courseFlags |= 0x10;
                    modeCtrl.courseFlags |= 0x800000;
                } else if ((s8) temp_r4_6 == 6) {
                    modeCtrl.difficulty = 2;
                    modeCtrl.courseFlags |= 8;
                    modeCtrl.courseFlags |= 0x10;
                    modeCtrl.courseFlags &= 0xFF7FFFFF;
                } else if ((s8) temp_r4_6 >= 3) {
                    modeCtrl.difficulty = stageselect_is_storymode.unk1[(s8) stageselect_is_storymode.unk0] % 3;
                    modeCtrl.courseFlags |= 8;
                    modeCtrl.courseFlags &= 0xFFFFFFEF;
                    modeCtrl.courseFlags &= 0xFF7FFFFF;
                } else {
                    modeCtrl.difficulty = stageselect_is_storymode.unk1[(s8) stageselect_is_storymode.unk0] % 3;
                    modeCtrl.courseFlags &= 0xFFFFFFF7;
                    modeCtrl.courseFlags &= 0xFFFFFFEF;
                    modeCtrl.courseFlags &= 0xFF7FFFFF;
                }
            }
            temp_r24_4 = (s8) stageselect_is_storymode.unk0;
            if (temp_r24_4 == 0) {
                var_r29_2 = 3;
            } else {
                var_r29_2 = 0xA;
            }
            if (temp_r24_4 == 0) {
                var_r3_7 = g_get_some_difficulty_count_2(modeCtrl.difficulty, modeCtrl.courseFlags);
            } else {
                var_r3_7 = 0xA;
            }
            var_r27 = var_r3_7;
            if (temp_r24_4 == 0) {
                if (g_smth_with_cm_entries_in_main_menu(2, 0, 0x800000U) != 0) {
                    var_r29_2 = 8;
                } else if (g_smth_with_cm_entries_in_main_menu(2, 0, 0x10U) != 0) {
                    var_r29_2 = 7;
                } else if ((g_smth_with_cm_entries_in_main_menu(0, 0, 8U) != 0) || (g_smth_with_cm_entries_in_main_menu(1, 0, 8U) != 0) || (g_smth_with_cm_entries_in_main_menu(2, 0, 8U) != 0)) {
                    var_r29_2 = 6;
                }
            }
            if ((s8) temp_r26_2 != (s8) stageselect_is_storymode.unk1[(s8) stageselect_is_storymode.unk0]) {
                call_SoundReqID_arg_0(0x6C);
                if ((s8) stageselect_is_storymode.unk0 == 1) {
                    var_r3_8 = get_story_mode_stage_id((s32) (s8) stageselect_is_storymode.unk1[(s8) stageselect_is_storymode.unk0], (s32) (s8) stageselect_is_storymode.unk3[(s8) stageselect_is_storymode.unk0][(s8) stageselect_is_storymode.unk1[(s8) stageselect_is_storymode.unk0]]);
                } else {
                    var_r3_8 = fn_1_A3C84(modeCtrl.difficulty, (s32) (s8) stageselect_is_storymode.unk3[(s8) stageselect_is_storymode.unk0][(s8) stageselect_is_storymode.unk1[(s8) stageselect_is_storymode.unk0]], modeCtrl.courseFlags);
                }
                read_stage_name_from_dvd(var_r3_8, lbl_3_bss_78, 0x80);
            }
        } else {
            temp_r25_2 = stageselect_is_storymode.unk1[(s8) stageselect_is_storymode.unk0];
            temp_r26_3 = stageselect_is_storymode.unk3[(s8) stageselect_is_storymode.unk0][(s8) temp_r25_2];
            temp_r3_4 = did_any_pad_press_input(1U);
            temp_r6 = var_r27 - 1;
            temp_r4_7 = ((s8) stageselect_is_storymode.unk0 * 0xA) + (s8) temp_r25_2;
            if ((s32) (temp_r6 + 1) < 0xA) {
                var_r4 = temp_r6 + 1;
            } else {
                var_r4 = 0xA;
            }
            temp_r0_3 = (stageselect_is_storymode.unk3[0][temp_r4_7] / 10) * 0xA;
            if (temp_r3_4 != 0) {
                stageselect_is_storymode.unk3[0][temp_r4_7] = (u8) (s8) (stageselect_is_storymode.unk3[0][temp_r4_7] + 1);
                stageselect_is_storymode.unk3[0][temp_r4_7] = (u8) (s8) (stageselect_is_storymode.unk3[0][temp_r4_7] % var_r4);
                stageselect_is_storymode.unk3[0][temp_r4_7] = (u8) (s8) (stageselect_is_storymode.unk3[0][temp_r4_7] + temp_r0_3);
            } else {
                stageselect_is_storymode.unk3[0][temp_r4_7] = (u8) (s8) (stageselect_is_storymode.unk3[0][temp_r4_7] - 1);
                if ((s8) stageselect_is_storymode.unk3[0][temp_r4_7] < 0) {
                    stageselect_is_storymode.unk3[0][temp_r4_7] = (u8) (s8) (stageselect_is_storymode.unk3[0][temp_r4_7] + var_r4);
                }
                stageselect_is_storymode.unk3[0][temp_r4_7] = (u8) (s8) (stageselect_is_storymode.unk3[0][temp_r4_7] % var_r4);
                stageselect_is_storymode.unk3[0][temp_r4_7] = (u8) (s8) (stageselect_is_storymode.unk3[0][temp_r4_7] + temp_r0_3);
            }
            if ((s8) temp_r26_3 != (s8) stageselect_is_storymode.unk3[(s8) stageselect_is_storymode.unk0][(s8) temp_r25_2]) {
                call_SoundReqID_arg_0(0x6C);
                if ((s8) stageselect_is_storymode.unk0 == 1) {
                    var_r3_9 = get_story_mode_stage_id((s32) (s8) stageselect_is_storymode.unk1[(s8) stageselect_is_storymode.unk0], (s32) (s8) stageselect_is_storymode.unk3[(s8) stageselect_is_storymode.unk0][(s8) stageselect_is_storymode.unk1[(s8) stageselect_is_storymode.unk0]]);
                } else {
                    var_r3_9 = fn_1_A3C84(modeCtrl.difficulty, (s32) (s8) stageselect_is_storymode.unk3[(s8) stageselect_is_storymode.unk0][(s8) stageselect_is_storymode.unk1[(s8) stageselect_is_storymode.unk0]], modeCtrl.courseFlags);
                }
                read_stage_name_from_dvd(var_r3_9, lbl_3_bss_78, 0x80);
            }
        }
    } else if (did_any_pad_press_input(8U) != 0) {
        stageselect_is_storymode.unk0 = (u8) (s8) (stageselect_is_storymode.unk0 ^ 1);
        if ((s8) stageselect_is_storymode.unk0 == 0) {
            temp_r4_8 = stageselect_is_storymode.unk1[(s8) stageselect_is_storymode.unk0];
            if ((s8) temp_r4_8 == 7) {
                modeCtrl.difficulty = 2;
                modeCtrl.courseFlags |= 8;
                modeCtrl.courseFlags |= 0x10;
                modeCtrl.courseFlags |= 0x800000;
            } else if ((s8) temp_r4_8 == 6) {
                modeCtrl.difficulty = 2;
                modeCtrl.courseFlags |= 8;
                modeCtrl.courseFlags |= 0x10;
                modeCtrl.courseFlags &= 0xFF7FFFFF;
            } else if ((s8) temp_r4_8 >= 3) {
                modeCtrl.difficulty = stageselect_is_storymode.unk1[(s8) stageselect_is_storymode.unk0] % 3;
                modeCtrl.courseFlags |= 8;
                modeCtrl.courseFlags &= 0xFFFFFFEF;
                modeCtrl.courseFlags &= 0xFF7FFFFF;
            } else {
                modeCtrl.difficulty = stageselect_is_storymode.unk1[(s8) stageselect_is_storymode.unk0] % 3;
                modeCtrl.courseFlags &= 0xFFFFFFF7;
                modeCtrl.courseFlags &= 0xFFFFFFEF;
                modeCtrl.courseFlags &= 0xFF7FFFFF;
            }
        }
        temp_r24_5 = (s8) stageselect_is_storymode.unk0;
        if (temp_r24_5 == 0) {
            var_r29_2 = 3;
        } else {
            var_r29_2 = 0xA;
        }
        if (temp_r24_5 == 0) {
            var_r3_10 = g_get_some_difficulty_count_2(modeCtrl.difficulty, modeCtrl.courseFlags);
        } else {
            var_r3_10 = 0xA;
        }
        var_r27 = var_r3_10;
        if (temp_r24_5 == 0) {
            if (g_smth_with_cm_entries_in_main_menu(2, 0, 0x800000U) != 0) {
                var_r29_2 = 8;
            } else if (g_smth_with_cm_entries_in_main_menu(2, 0, 0x10U) != 0) {
                var_r29_2 = 7;
            } else if ((g_smth_with_cm_entries_in_main_menu(0, 0, 8U) != 0) || (g_smth_with_cm_entries_in_main_menu(1, 0, 8U) != 0) || (g_smth_with_cm_entries_in_main_menu(2, 0, 8U) != 0)) {
                var_r29_2 = 6;
            }
        }
        call_SoundReqID_arg_0(0x6C);
        temp_r5 = stageselect_is_storymode.unk1[(s8) stageselect_is_storymode.unk0];
        if ((s8) stageselect_is_storymode.unk0 == 0) {
            if (g_smth_with_cm_entries_in_main_menu(modeCtrl.difficulty, (s8) stageselect_is_storymode.unk3[(s8) stageselect_is_storymode.unk0][(s8) temp_r5] + 1, modeCtrl.courseFlags) == 0) {
                stageselect_is_storymode.unk17 = (u8) (stageselect_is_storymode.unk17 & 0xFFFFFFFE);
            }
        } else if (g_is_storymode_stage_unlocked((s32) (s8) temp_r5, (s32) (s8) stageselect_is_storymode.unk3[(s8) stageselect_is_storymode.unk0][(s8) temp_r5]) == 0) {
            stageselect_is_storymode.unk17 = (u8) (stageselect_is_storymode.unk17 & 0xFFFFFFFE);
        }
        if ((s8) stageselect_is_storymode.unk0 == 1) {
            var_r3_11 = get_story_mode_stage_id((s32) (s8) stageselect_is_storymode.unk1[(s8) stageselect_is_storymode.unk0], (s32) (s8) stageselect_is_storymode.unk3[(s8) stageselect_is_storymode.unk0][(s8) stageselect_is_storymode.unk1[(s8) stageselect_is_storymode.unk0]]);
        } else {
            var_r3_11 = fn_1_A3C84(modeCtrl.difficulty, (s32) (s8) stageselect_is_storymode.unk3[(s8) stageselect_is_storymode.unk0][(s8) stageselect_is_storymode.unk1[(s8) stageselect_is_storymode.unk0]], modeCtrl.courseFlags);
        }
        read_stage_name_from_dvd(var_r3_11, lbl_3_bss_78, 0x80);
    }
    if ((s32) (stageselect_is_storymode.unk17 & 1) != 0) {
        g_currently_visible_menu_screen.unk38 = NULL;
        if ((s8) stageselect_is_storymode.unk0 == 1) {
            if (g_is_storymode_stage_unlocked((s32) (s8) stageselect_is_storymode.unk1[(s8) stageselect_is_storymode.unk0], (s32) (s8) stageselect_is_storymode.unk3[(s8) stageselect_is_storymode.unk0][(s8) stageselect_is_storymode.unk1[(s8) stageselect_is_storymode.unk0]]) == 0) {
                g_currently_visible_menu_screen.unk38 = lbl_3_data_3E40[(s8) g_language_id];
            }
        } else if (g_smth_with_cm_entries_in_main_menu(modeCtrl.difficulty, (s8) stageselect_is_storymode.unk3[(s8) stageselect_is_storymode.unk0][(s8) stageselect_is_storymode.unk1[(s8) stageselect_is_storymode.unk0]] + 1, modeCtrl.courseFlags) == 0) {
            g_currently_visible_menu_screen.unk38 = lbl_3_data_3F18[(s8) g_language_id];
        }
    } else if ((s8) stageselect_is_storymode.unk0 == 1) {
        if (g_is_storymode_stage_unlocked((s32) (s8) stageselect_is_storymode.unk1[(s8) stageselect_is_storymode.unk0], (s32) (s8) stageselect_is_storymode.unk3[(s8) stageselect_is_storymode.unk0][(s8) stageselect_is_storymode.unk1[(s8) stageselect_is_storymode.unk0]]) != 0) {
            g_currently_visible_menu_screen.unk38 = lbl_3_data_3F70[(s8) g_language_id];
        } else {
            g_currently_visible_menu_screen.unk38 = lbl_3_data_4044[(s8) g_language_id];
        }
    } else if (g_smth_with_cm_entries_in_main_menu(modeCtrl.difficulty, (s8) stageselect_is_storymode.unk3[(s8) stageselect_is_storymode.unk0][(s8) stageselect_is_storymode.unk1[(s8) stageselect_is_storymode.unk0]] + 1, modeCtrl.courseFlags) != 0) {
        g_currently_visible_menu_screen.unk38 = lbl_3_data_4098[(s8) g_language_id];
    } else {
        g_currently_visible_menu_screen.unk38 = lbl_3_data_4170[(s8) g_language_id];
    }
    temp_r24_6 = stageselect_is_storymode.unk1[(s8) stageselect_is_storymode.unk0];
    textdraw_reset();
    textdraw_set_flags(0x80000000U);
    textdraw_set_font(3);
    textdraw_set_drop_shadow();
    textdraw_set_spacing(0.0f, 4.0f);
    textdraw_set_alignment(1);
    textdraw_set_pos((f32) 0x64, (f32) 0xB4);
    if ((s8) stageselect_is_storymode.unk0 == 1) {
        textdraw_set_mul_color(0xFFFF00U);
        textdraw_set_scale(0.64000005f, 0.8f);
    }
    var_r26_4 = 0;
loop_298:
    if (var_r26_4 < var_r29_2) {
        if (var_r26_4 == (s8) temp_r24_6) {
            var_f31 = 0.8f;
        } else {
            var_f31 = 0.6f;
        }
        if (var_r26_4 == (s8) temp_r24_6) {
            if ((s32) (stageselect_is_storymode.unk17 & 1) == 0) {
                var_r3_12 = 0xFF8000;
            } else {
                var_r3_12 = 0xFFFF00;
            }
        } else {
            var_r3_12 = 0x808000;
        }
        textdraw_set_mul_color(var_r3_12);
        textdraw_set_scale(0.8f * var_f31, var_f31);
        var_r26_4 += 1;
        goto loop_298;
    }
    textdraw_set_alignment(4);
    temp_r28 = (s32) (2.6e+02f - (0.5f * (24.0f * (f32) ((var_r27 / 10) - 1))));
    var_r26_5 = 0;
loop_317:
    if (var_r26_5 < var_r27) {
        var_r29_3 = 0;
        if ((s8) stageselect_is_storymode.unk0 == 0) {
            if (g_smth_with_cm_entries_in_main_menu(modeCtrl.difficulty, var_r26_5 + 1, modeCtrl.courseFlags) != 0) {
                var_r29_3 = 1;
            }
        } else if (g_is_storymode_stage_unlocked((s32) (s8) temp_r24_6, var_r26_5) != 0) {
            var_r29_3 = 1;
        }
        if (var_r29_3 != 0) {
            if (var_r26_5 == (s8) stageselect_is_storymode.unk3[(s8) stageselect_is_storymode.unk0][(s8) temp_r24_6]) {
                if ((s32) (stageselect_is_storymode.unk17 & 1) != 0) {
                    var_f31_2 = 0.8f;
                } else {
                    var_f31_2 = 0.6f;
                }
                if ((s32) (stageselect_is_storymode.unk17 & 1) != 0) {
                    var_r3_13 = 0xFF8000;
                } else {
                    var_r3_13 = 0xFFFF00;
                }
            } else {
                var_f31_2 = 0.6f;
                var_r3_13 = 0xC0C000;
            }
        } else {
            var_f31_2 = 0.6f;
            var_r3_13 = 0x202000;
        }
        textdraw_set_mul_color(var_r3_13);
        textdraw_set_scale(0.8f * var_f31_2, var_f31_2);
        textdraw_set_pos((f32) 0x64 + (3e+01f * (f32) (var_r26_5 % 10)), (f32) temp_r28 + (24.0f * (f32) (var_r26_5 / 10)));
        var_r26_5 += 1;
        goto loop_317;
    }
}

void menu_start_story_mode(void) {
    s32 var_r4;

    modeCtrl.courseFlags |= 4;
    g_related_to_pause_menu_input.flags &= 0xFFFFFFFD;
    var_r4 = 0;
loop_2:
    if (var_r4 < (s32) modeCtrl.playerCount) {
        g_poolInfo.playerPool.statusList[var_r4] = 2;
        var_r4 += 1;
        goto loop_2;
    }
loop_5:
    if (var_r4 < 4) {
        g_poolInfo.playerPool.statusList[var_r4] = 0;
        var_r4 += 1;
        goto loop_5;
    }
    modeCtrl.currPlayer = 0;
    modeCtrl.gameType = 0xE;
    modeCtrl.unk30 = 1;
    modeCtrl.unk40 = 0;
    main_mode_request = 2;
    sub_mode_request = 0x57;
}

void menu_start_challenge_mode(void) {
    s32 var_r0;
    s32 var_r4;
    s32 var_r4_2;
    s8 var_r0_2;
    u8 var_r5;

    modeCtrl.courseFlags |= 4;
    g_related_to_pause_menu_input.flags &= 0xFFFFFFFD;
    var_r4 = 0;
loop_2:
    if (var_r4 < (s32) modeCtrl.playerCount) {
        g_poolInfo.playerPool.statusList[var_r4] = 2;
        var_r4 += 1;
        goto loop_2;
    }
loop_5:
    if (var_r4 < 4) {
        g_poolInfo.playerPool.statusList[var_r4] = 0;
        var_r4 += 1;
        goto loop_5;
    }
    modeCtrl.currPlayer = 0;
    modeCtrl.gameType = 0;
    modeCtrl.unk30 = 1;
    modeCtrl.unk40 = 0;
    var_r4_2 = (s8) (u8) sel_menu_info.menuStackPtr - 1;
loop_10:
    if (var_r4_2 < 0) {
        var_r0 = 0;
    } else if ((s8) sel_menu_info.menuStack[var_r4_2] == 9) {
        var_r0 = 1;
    } else {
        var_r4_2 -= 1;
        goto loop_10;
    }
    if (var_r0 != 0) {
        var_r5 = g_focused_root_menu.unk0[9];
    } else {
        var_r5 = g_focused_root_menu.unk0[8];
    }
    if ((s8) var_r5 < 2) {
        var_r0_2 = (s8) var_r5;
    } else {
        var_r0_2 = 2;
    }
    modeCtrl.difficulty = (s32) var_r0_2;
    modeCtrl.courseFlags |= 1;
    modeCtrl.courseFlags &= 0xFF7FFFE7;
    if ((s8) var_r5 > 2) {
        modeCtrl.courseFlags |= 8;
        modeCtrl.courseFlags |= 0x10;
        modeCtrl.courseFlags |= 0x02000000;
    }
    init_course();
    g_another_stage_id = mode_info.currStageId;
    main_mode_request = 2;
    sub_mode_request = 0x3C;
}

void menu_start_practice_mode(void) {
    s32 var_r4;
    s32 var_r5;
    s8 var_r4_2;
    u32 var_r6;

    modeCtrl.courseFlags |= 4;
    g_related_to_pause_menu_input.flags &= 0xFFFFFFFD;
    var_r4 = 0;
loop_2:
    if (var_r4 < (s32) modeCtrl.playerCount) {
        g_poolInfo.playerPool.statusList[var_r4] = 2;
        var_r4 += 1;
        goto loop_2;
    }
loop_5:
    if (var_r4 < 4) {
        g_poolInfo.playerPool.statusList[var_r4] = 0;
        var_r4 += 1;
        goto loop_5;
    }
    modeCtrl.currPlayer = 0;
    modeCtrl.gameType = 2;
    modeCtrl.unk30 = 1;
    modeCtrl.unk40 = 0;
    if ((s8) stageselect_is_storymode.unk0 == 0) {
        var_r5 = modeCtrl.difficulty;
        var_r4_2 = (s8) stageselect_is_storymode.unk3[(s8) stageselect_is_storymode.unk0][(s8) stageselect_is_storymode.unk1[(s8) stageselect_is_storymode.unk0]];
        var_r6 = modeCtrl.courseFlags;
    } else {
        mode_info.storyWorld = (s16) (s8) stageselect_is_storymode.unk1[(s8) stageselect_is_storymode.unk0];
        mode_info.storyStage = (s16) (s8) stageselect_is_storymode.unk3[(s8) stageselect_is_storymode.unk0][(s8) stageselect_is_storymode.unk1[(s8) stageselect_is_storymode.unk0]];
        var_r5 = 0;
        var_r4_2 = 0;
        var_r6 = 0;
    }
    g_stage_id_in_practice_mode->floor = var_r4_2 + 1;
    g_stage_id_in_practice_mode->difficulty = (s16) var_r5;
    g_stage_id_in_practice_mode->courseFlags = var_r6;
    g_something_with_cm_entries_practice_mode();
    in_practice_mode = 1;
    modeCtrl.courseFlags |= 1;
    init_course();
    if ((s8) stageselect_is_storymode.unk0 == 0) {
        mode_info.currStageId = (s16) g_cm_entry_list9[0].value;
        g_another_stage_id = mode_info.currStageId;
        lbl_1_bss_1744[1] = (s8) stageselect_is_storymode.unk3[(s8) stageselect_is_storymode.unk0][(s8) stageselect_is_storymode.unk1[(s8) stageselect_is_storymode.unk0]] + 1;
        lbl_1_bss_1744[0] = 1;
    } else {
        modeCtrl.courseFlags |= 0x400000;
        modeCtrl.courseFlags &= 0xFF7FFFE7;
        g_another_stage_id = get_story_mode_stage_id((s32) mode_info.storyWorld, (s32) mode_info.storyStage);
    }
    main_mode_request = 2;
    sub_mode_request = 0x3C;
}

void menu_race_mode_select_tick(void) {
    u8 temp_r0;

    if ((s32) g_currently_visible_menu_screen.unk10 != 0) {
        temp_r0 = g_focused_root_menu.unk0[(s8) g_currently_visible_menu_screen.currScreen];
        switch ((s8) temp_r0) {                     /* irregular */
        case 0:
            call_SoundReqID_arg_0(0x21F);
            lbl_1_bss_16D0.unk8 = (s32) (lbl_1_bss_16D0.unk8 & 0xFFFFFFE7);
            return;
        case 1:
            call_SoundReqID_arg_0(0x220);
            lbl_1_bss_16D0.unk8 = (s32) (lbl_1_bss_16D0.unk8 | 8);
            lbl_1_bss_16D0.unk8 = (s32) (lbl_1_bss_16D0.unk8 & 0xFFFFFFEF);
            return;
        case 2:
            call_SoundReqID_arg_0(0x221);
            lbl_1_bss_16D0.unk8 = (s32) (lbl_1_bss_16D0.unk8 | 0x10);
            lbl_1_bss_16D0.unk8 = (s32) (lbl_1_bss_16D0.unk8 & 0xFFFFFFF7);
            modeCtrl.playerCount = 1;
            break;
        }
    }
}

void menu_race_course_select_tick(void) {
    if ((s32) g_currently_visible_menu_screen.unkC != 0) {
        call_SoundReqID_arg_0(0x239);
    }
    if ((s32) g_currently_visible_menu_screen.unk14 != 0) {
        lbl_1_bss_16D0.unk0 = g_focused_root_menu.unk0[(s8) g_currently_visible_menu_screen.currScreen];
    }
}

void menu_start_race(void) {
    s32 var_r4;

    modeCtrl.courseFlags |= 4;
    g_related_to_pause_menu_input.flags &= 0xFFFFFFFD;
    var_r4 = 0;
loop_2:
    if (var_r4 < (s32) modeCtrl.playerCount) {
        g_poolInfo.playerPool.statusList[var_r4] = 2;
        var_r4 += 1;
        goto loop_2;
    }
loop_5:
    if (var_r4 < 4) {
        g_poolInfo.playerPool.statusList[var_r4] = 0;
        var_r4 += 1;
        goto loop_5;
    }
    modeCtrl.currPlayer = 0;
    modeCtrl.gameType = 3;
    modeCtrl.unk30 = modeCtrl.playerCount;
    modeCtrl.unk40 = 1;
    main_mode_request = 4;
    sub_mode_request = 0xB9;
}

void menu_fight_mode_select_tick(void) {
    u8 temp_r0;

    if ((s32) g_currently_visible_menu_screen.unk10 != 0) {
        temp_r0 = g_focused_root_menu.unk0[(s8) g_currently_visible_menu_screen.currScreen];
        switch ((s8) temp_r0) {                     /* irregular */
        case 0:
            call_SoundReqID_arg_0(0x356);
            lbl_1_bss_16DC.unk0 &= 0xFFFFFFFD;
            return;
        case 1:
            call_SoundReqID_arg_0(0x357);
            lbl_1_bss_16DC.unk0 |= 2;
            break;
        }
    }
}

void menu_fight_no_of_wins_tick(void) {
    s8 var_r0;

    if ((s32) g_currently_visible_menu_screen.unkC != 0) {
        call_SoundReqID_arg_0(0x390);
    }
    if ((s8) g_focused_root_menu.unk0[(s8) g_currently_visible_menu_screen.currScreen] == 0) {
        var_r0 = 0x17;
    } else {
        var_r0 = 0x18;
    }
    menu_screen_list[(s8) g_currently_visible_menu_screen.currScreen].entries[(s8) g_focused_root_menu.unk0[(s8) g_currently_visible_menu_screen.currScreen]].next_screen_id = (u8) var_r0;
    if ((s32) g_currently_visible_menu_screen.unk10 != 0) {
        lbl_1_bss_16DC.unk4 = (s8) (g_focused_root_menu.unk0[(s8) g_currently_visible_menu_screen.currScreen] + 1);
    }
}

void menu_fight_stage_select_tick(void) {
    s32 var_r29;
    u8 temp_r0;
    u8 temp_r0_2;

    if ((s8) lbl_1_bss_16DC.unk4 == 1) {
        var_r29 = 1;
    } else {
        var_r29 = 3;
    }
    g_currently_visible_menu_screen.unk38 = menu_screen_list[(s8) g_currently_visible_menu_screen.currScreen].entries[(s8) lbl_1_bss_16DC.unk5[(s8) (u8) lbl_3_bss_178.unk0]].description[(s8) g_language_id];
    if ((s32) g_currently_visible_menu_screen.unkC != 0) {
        call_SoundReqID_arg_0(0x23D);
        lbl_3_bss_178.unk0 = 0U;
        lbl_3_bss_178.unk4 = lbl_1_bss_16DC;
        fn_3_14998();
        create_fight_stage_preview_sprites();
        return;
    }
    if ((s32) g_currently_visible_menu_screen.unk18 != 0) {
        lbl_3_bss_178.unk4 = lbl_1_bss_16DC;
        fn_3_14998();
        create_fight_stage_preview_sprites();
        return;
    }
    if ((s32) g_currently_visible_menu_screen.unk14 != 0) {
        fn_3_1EA0C();
        return;
    }
    if ((did_any_pad_press_input(6U) != 0) && ((s32) g_currently_visible_menu_screen.unkC == 0) && ((s32) g_currently_visible_menu_screen.unk18 == 0)) {
        if ((s8) (u8) lbl_3_bss_178.unk0 == (s32) (var_r29 - 1)) {
            g_sel_ngc_item_selected(1);
            if ((u32) (lbl_1_bss_16DC.unk0 & 2) != 0) {
                g_currently_visible_menu_screen.nextScreen = 0x1A;
            }
        } else {
            call_SoundReqID_arg_0(0x65);
            lbl_3_bss_178.unk0 = (u8) ((u8) lbl_3_bss_178.unk0 + 1);
        }
    } else {
        if ((did_any_pad_press_input(7U) != 0) && ((s32) g_currently_visible_menu_screen.unkC == 0) && ((s32) g_currently_visible_menu_screen.unk18 == 0)) {
            lbl_1_bss_16DC.unk5[(s8) (u8) lbl_3_bss_178.unk0] = (u8) lbl_3_bss_178.unk4.unk5[(s8) (u8) lbl_3_bss_178.unk0];
            if ((s8) (u8) lbl_3_bss_178.unk0 == 0) {
                g_b_button_pressed_on_menu(1);
                return;
            }
            call_SoundReqID_arg_0(0x66);
            lbl_3_bss_178.unk0 = (u8) ((u8) lbl_3_bss_178.unk0 - 1);
            return;
        }
        if (did_any_pad_press_input(0U) != 0) {
            temp_r0 = lbl_1_bss_16DC.unk5[(s8) (u8) lbl_3_bss_178.unk0] - 1;
            lbl_1_bss_16DC.unk5[(s8) (u8) lbl_3_bss_178.unk0] = temp_r0;
            if ((s8) temp_r0 < 0) {
                lbl_1_bss_16DC.unk5[(s8) (u8) lbl_3_bss_178.unk0] = 2;
            }
            call_SoundReqID_arg_0(0x6C);
            return;
        }
        if (did_any_pad_press_input(1U) != 0) {
            temp_r0_2 = lbl_1_bss_16DC.unk5[(s8) (u8) lbl_3_bss_178.unk0] + 1;
            lbl_1_bss_16DC.unk5[(s8) (u8) lbl_3_bss_178.unk0] = temp_r0_2;
            if ((s8) temp_r0_2 > 2) {
                lbl_1_bss_16DC.unk5[(s8) (u8) lbl_3_bss_178.unk0] = 0;
            }
            call_SoundReqID_arg_0(0x6C);
        }
    }
}

void menu_start_fight(void) {
    s32 var_r4;

    modeCtrl.courseFlags |= 4;
    g_related_to_pause_menu_input.flags &= 0xFFFFFFFD;
    var_r4 = 0;
loop_2:
    if (var_r4 < (s32) modeCtrl.playerCount) {
        g_poolInfo.playerPool.statusList[var_r4] = 2;
        var_r4 += 1;
        goto loop_2;
    }
loop_5:
    if (var_r4 < 4) {
        g_poolInfo.playerPool.statusList[var_r4] = 0;
        var_r4 += 1;
        goto loop_5;
    }
    modeCtrl.currPlayer = 0;
    modeCtrl.gameType = 4;
    modeCtrl.unk30 = modeCtrl.playerCount;
    modeCtrl.unk40 = 1;
    main_mode_request = 4;
    sub_mode_request = 0xBD;
}

void menu_start_target(void) {
    s32 var_r4;

    modeCtrl.courseFlags |= 4;
    g_related_to_pause_menu_input.flags &= 0xFFFFFFFD;
    var_r4 = 0;
loop_2:
    if (var_r4 < (s32) modeCtrl.playerCount) {
        g_poolInfo.playerPool.statusList[var_r4] = 2;
        var_r4 += 1;
        goto loop_2;
    }
loop_5:
    if (var_r4 < 4) {
        g_poolInfo.playerPool.statusList[var_r4] = 0;
        var_r4 += 1;
        goto loop_5;
    }
    modeCtrl.currPlayer = 0;
    modeCtrl.gameType = 5;
    modeCtrl.unk30 = modeCtrl.playerCount;
    modeCtrl.unk40 = 1;
    main_mode_request = 4;
    sub_mode_request = 0xBF;
}

void menu_billiards_mode_select_tick(void) {
    s32 var_r4;
    s32 var_r5;
    u8 temp_r0;

    if ((s32) g_currently_visible_menu_screen.unk10 != 0) {
        temp_r0 = g_focused_root_menu.unk0[(s8) g_currently_visible_menu_screen.currScreen];
        switch ((s8) temp_r0) {                     /* irregular */
        case 0:
            call_SoundReqID_arg_0(0x226);
            lbl_1_bss_16F0.unk6 = 0;
            var_r4 = 0;
            if ((u32) (dip_switches & 0x400000) != 0) {
                var_r4 = 4;
            } else {
                var_r5 = 0;
loop_11:
                if (var_r5 < 4) {
                    if ((s8) (u8) pad_status_groups[var_r5].unk0[0].err == 0) {
                        var_r4 += 1;
                    }
                    var_r5 += 1;
                    goto loop_11;
                }
            }
            modeCtrl.playerCount = var_r4;
            return;
        case 1:
            call_SoundReqID_arg_0(0x227);
            lbl_1_bss_16F0.unk6 = 1;
            modeCtrl.playerCount = 1;
            break;
        }
    }
}

void menu_billiards_rule_select_tick(void) {
    u8 temp_r0;

    if ((s32) g_currently_visible_menu_screen.unk10 != 0) {
        lbl_1_bss_16F0.unk7 = (u8) g_focused_root_menu.unk0[(s8) g_currently_visible_menu_screen.currScreen];
        temp_r0 = g_focused_root_menu.unk0[(s8) g_currently_visible_menu_screen.currScreen];
        switch ((s8) temp_r0) {                     /* irregular */
        case 0:
            call_SoundReqID_arg_0(0x34C);
            return;
        case 1:
            call_SoundReqID_arg_0(0x34B);
            return;
        case 2:
            call_SoundReqID_arg_0(0x34D);
            return;
        case 3:
            call_SoundReqID_arg_0(0x34A);
            break;
        }
    }
}

void menu_start_billiards(void) {
    s32 var_r4;

    modeCtrl.courseFlags |= 4;
    g_related_to_pause_menu_input.flags &= 0xFFFFFFFD;
    var_r4 = 0;
loop_2:
    if (var_r4 < (s32) modeCtrl.playerCount) {
        g_poolInfo.playerPool.statusList[var_r4] = 2;
        var_r4 += 1;
        goto loop_2;
    }
loop_5:
    if (var_r4 < 4) {
        g_poolInfo.playerPool.statusList[var_r4] = 0;
        var_r4 += 1;
        goto loop_5;
    }
    modeCtrl.currPlayer = 0;
    modeCtrl.gameType = 6;
    modeCtrl.unk30 = 1;
    modeCtrl.unk40 = 0;
    main_mode_request = 4;
    sub_mode_request = 0xAD;
}

void menu_bowling_mode_select_tick(void) {
    u8 temp_r0;

    if ((s32) g_currently_visible_menu_screen.unk10 != 0) {
        temp_r0 = g_focused_root_menu.unk0[(s8) g_currently_visible_menu_screen.currScreen];
        switch ((s8) temp_r0) {                     /* irregular */
        case 0:
            call_SoundReqID_arg_0(0x356);
            lbl_1_bss_1700.unk0 = 0;
            return;
        case 1:
            call_SoundReqID_arg_0(0x34F);
            lbl_1_bss_1700.unk0 = 2;
            return;
        case 2:
            call_SoundReqID_arg_0(0x229);
            lbl_1_bss_1700.unk0 = 1;
            modeCtrl.playerCount = 1;
            break;
        }
    }
}

void menu_bowling_rule_select_tick(void) {
    u8 temp_r0;

    if ((s32) g_currently_visible_menu_screen.unk10 != 0) {
        temp_r0 = g_focused_root_menu.unk0[(s8) g_currently_visible_menu_screen.currScreen];
        switch ((s8) temp_r0) {                     /* irregular */
        case 0:
            call_SoundReqID_arg_0(0x350);
            lbl_1_bss_1700.unk0 = 0;
            return;
        case 1:
            call_SoundReqID_arg_0(0x351);
            lbl_1_bss_1700.unk0 = 3;
            return;
        case 2:
            call_SoundReqID_arg_0(0x352);
            lbl_1_bss_1700.unk0 = 4;
            break;
        }
    }
}

void menu_bowling_level_select_tick(void) {
    u8 temp_r0;
    u8 temp_r3;

    temp_r3 = fn_1_16C474();
    g_currently_visible_menu_screen.unk38 = NULL;
    if (temp_r3 >= 2U) {
        locked_menu_items = 0;
    } else if (temp_r3 == 1) {
        locked_menu_items = 4;
        if ((s8) g_focused_root_menu.unk0[(s8) g_currently_visible_menu_screen.currScreen] == 2) {
            g_currently_visible_menu_screen.unk38 = lbl_3_data_78B8[(s8) g_language_id];
        }
    } else {
        locked_menu_items = 6;
        if ((s8) g_focused_root_menu.unk0[(s8) g_currently_visible_menu_screen.currScreen] != 0) {
            g_currently_visible_menu_screen.unk38 = lbl_3_data_77E0[(s8) g_language_id];
        }
    }
    if ((s32) g_currently_visible_menu_screen.unk10 != 0) {
        temp_r0 = g_focused_root_menu.unk0[(s8) g_currently_visible_menu_screen.currScreen];
        switch ((s8) temp_r0) {                     /* irregular */
        case 0:
            call_SoundReqID_arg_0(0x353);
            lbl_1_bss_1700.unk1 = 0;
            return;
        case 1:
            call_SoundReqID_arg_0(0x354);
            lbl_1_bss_1700.unk1 = 1;
            return;
        case 2:
            call_SoundReqID_arg_0(0x355);
            lbl_1_bss_1700.unk1 = 2;
            break;
        }
    }
}

void menu_start_bowling(void) {
    s32 var_r4;

    modeCtrl.courseFlags |= 4;
    g_related_to_pause_menu_input.flags &= 0xFFFFFFFD;
    var_r4 = 0;
loop_2:
    if (var_r4 < (s32) modeCtrl.playerCount) {
        g_poolInfo.playerPool.statusList[var_r4] = 2;
        var_r4 += 1;
        goto loop_2;
    }
loop_5:
    if (var_r4 < 4) {
        g_poolInfo.playerPool.statusList[var_r4] = 0;
        var_r4 += 1;
        goto loop_5;
    }
    modeCtrl.currPlayer = 0;
    modeCtrl.gameType = 7;
    modeCtrl.unk30 = 1;
    modeCtrl.unk40 = 0;
    main_mode_request = 4;
    sub_mode_request = 0xAB;
}

void menu_golf_mode_select_tick(void) {
    u8 temp_r0;

    if ((s32) g_currently_visible_menu_screen.unk10 != 0) {
        temp_r0 = g_focused_root_menu.unk0[(s8) g_currently_visible_menu_screen.currScreen];
        switch ((s8) temp_r0) {                     /* irregular */
        case 0:
            call_SoundReqID_arg_0(0x22A);
            lbl_1_bss_1704.unk0 = 0;
            return;
        case 1:
            call_SoundReqID_arg_0(0x22B);
            lbl_1_bss_1704.unk0 = 1;
            modeCtrl.playerCount = 2;
            break;
        }
    }
}

void menu_start_golf(void) {
    s32 var_r4;

    modeCtrl.courseFlags |= 4;
    g_related_to_pause_menu_input.flags &= 0xFFFFFFFD;
    var_r4 = 0;
loop_2:
    if (var_r4 < (s32) modeCtrl.playerCount) {
        g_poolInfo.playerPool.statusList[var_r4] = 2;
        var_r4 += 1;
        goto loop_2;
    }
loop_5:
    if (var_r4 < 4) {
        g_poolInfo.playerPool.statusList[var_r4] = 0;
        var_r4 += 1;
        goto loop_5;
    }
    modeCtrl.currPlayer = 0;
    modeCtrl.gameType = 8;
    modeCtrl.unk30 = 1;
    modeCtrl.unk40 = 0;
    main_mode_request = 4;
    sub_mode_request = 0xBB;
}

void menu_boat_mode_select_entries_tick(void) {
    u8 temp_r0;

    if ((s32) g_currently_visible_menu_screen.unk10 != 0) {
        temp_r0 = g_focused_root_menu.unk0[(s8) g_currently_visible_menu_screen.currScreen];
        switch ((s8) temp_r0) {                     /* irregular */
        case 0:
            call_SoundReqID_arg_0(0x21F);
            lbl_1_bss_1708.unk4 = (u16) (lbl_1_bss_1708.unk4 & ~0x11);
            return;
        case 1:
            call_SoundReqID_arg_0(0x220);
            lbl_1_bss_1708.unk4 = (u16) (lbl_1_bss_1708.unk4 | 0x10);
            lbl_1_bss_1708.unk4 = (u16) (lbl_1_bss_1708.unk4 & 0xFFFFFFFE);
            return;
        case 2:
            call_SoundReqID_arg_0(0x221);
            lbl_1_bss_1708.unk4 = (u16) (lbl_1_bss_1708.unk4 | 1);
            lbl_1_bss_1708.unk4 = (u16) (lbl_1_bss_1708.unk4 & 0xFFFFFFEF);
            modeCtrl.playerCount = 1;
            break;
        }
    }
}

void menu_boat_course_select_tick(void) {
    if ((s32) g_currently_visible_menu_screen.unkC != 0) {
        fn_3_14A60();
        call_SoundReqID_arg_0(0x239);
    } else if ((s32) g_currently_visible_menu_screen.unk18 != 0) {
        fn_3_14A60();
    }
    if ((s32) g_currently_visible_menu_screen.unk14 != 0) {
        lbl_1_bss_1708.unk0 = g_focused_root_menu.unk0[(s8) g_currently_visible_menu_screen.currScreen];
    }
}

void menu_start_boat(void) {
    s32 var_r4;

    modeCtrl.courseFlags |= 4;
    g_related_to_pause_menu_input.flags &= 0xFFFFFFFD;
    var_r4 = 0;
loop_2:
    if (var_r4 < (s32) modeCtrl.playerCount) {
        g_poolInfo.playerPool.statusList[var_r4] = 2;
        var_r4 += 1;
        goto loop_2;
    }
loop_5:
    if (var_r4 < 4) {
        g_poolInfo.playerPool.statusList[var_r4] = 0;
        var_r4 += 1;
        goto loop_5;
    }
    modeCtrl.currPlayer = 0;
    modeCtrl.gameType = 0xA;
    modeCtrl.unk30 = modeCtrl.playerCount;
    modeCtrl.unk40 = 1;
    main_mode_request = 4;
    sub_mode_request = 0xC1;
}

void menu_shot_stage_select_tick(void) {
    s32 temp_r3;

    temp_r3 = fn_1_16C4F8();
    g_currently_visible_menu_screen.unk38 = NULL;
    if ((u8) temp_r3 >= 2U) {
        locked_menu_items = 0;
    } else if ((u8) temp_r3 == 1) {
        locked_menu_items = 4;
        if ((s8) g_focused_root_menu.unk0[(s8) g_currently_visible_menu_screen.currScreen] == 2) {
            g_currently_visible_menu_screen.unk38 = lbl_3_data_8BA8[(s8) g_language_id];
        }
    } else {
        locked_menu_items = 6;
        if ((s8) g_focused_root_menu.unk0[(s8) g_currently_visible_menu_screen.currScreen] != 0) {
            g_currently_visible_menu_screen.unk38 = lbl_3_data_8AC0[(s8) g_language_id];
        }
    }
    if ((s32) g_currently_visible_menu_screen.unkC != 0) {
        call_SoundReqID_arg_0(0x23D);
    }
    if ((s32) g_currently_visible_menu_screen.unk14 != 0) {
        g_auto_reload_setting.unk4 = (u8) g_focused_root_menu.unk0[(s8) g_currently_visible_menu_screen.currScreen];
        switch ((s32) modeCtrl.playerCount) {       /* irregular */
        case 1:
            g_currently_visible_menu_screen.nextScreen = 0x33;
            return;
        case 2:
            g_currently_visible_menu_screen.nextScreen = 0x34;
            return;
        case 3:
            g_currently_visible_menu_screen.nextScreen = 0x35;
            return;
        case 4:
            g_currently_visible_menu_screen.nextScreen = 0x36;
            break;
        }
    }
}

void menu_start_shot(void) {
    s32 var_r4;

    modeCtrl.courseFlags |= 4;
    g_related_to_pause_menu_input.flags &= 0xFFFFFFFD;
    var_r4 = 0;
loop_2:
    if (var_r4 < (s32) modeCtrl.playerCount) {
        g_poolInfo.playerPool.statusList[var_r4] = 2;
        var_r4 += 1;
        goto loop_2;
    }
loop_5:
    if (var_r4 < 4) {
        g_poolInfo.playerPool.statusList[var_r4] = 0;
        var_r4 += 1;
        goto loop_5;
    }
    modeCtrl.currPlayer = 0;
    modeCtrl.gameType = 9;
    modeCtrl.unk30 = 1;
    modeCtrl.unk40 = 0;
    main_mode_request = 4;
    sub_mode_request = 0xAF;
}

void menu_dogfight_mode_select_tick(void) {
    u8 temp_r0;

    if ((s32) g_currently_visible_menu_screen.unk10 != 0) {
        temp_r0 = g_focused_root_menu.unk0[(s8) g_currently_visible_menu_screen.currScreen];
        switch ((s8) temp_r0) {                     /* irregular */
        case 0:
            call_SoundReqID_arg_0(0x356);
            break;
        case 1:
            call_SoundReqID_arg_0(0x357);
            break;
        }
        lbl_1_bss_1718.unk1 = (u8) g_focused_root_menu.unk0[(s8) g_currently_visible_menu_screen.currScreen];
    }
}

void menu_dogfight_stage_select_tick(void) {
    if ((s32) g_currently_visible_menu_screen.unkC != 0) {
        call_SoundReqID_arg_0(0x23D);
    }
    if (((s32) g_currently_visible_menu_screen.unkC != 0) || ((s32) g_currently_visible_menu_screen.unk18 != 0)) {
        fn_3_1EC64(0);
    }
    if ((s32) g_currently_visible_menu_screen.unk14 != 0) {
        fn_3_1EE08(0);
        if ((s32) g_currently_visible_menu_screen.unk4 == 0) {
            lbl_1_bss_1718.unk3 = (u8) g_focused_root_menu.unk0[(s8) g_currently_visible_menu_screen.currScreen];
            if ((s8) lbl_1_bss_1718.unk1 == 1) {
                g_currently_visible_menu_screen.nextScreen = 0x3B;
            }
        }
    }
}

void menu_start_dogfight(void) {
    s32 var_r4;

    modeCtrl.courseFlags |= 4;
    g_related_to_pause_menu_input.flags &= 0xFFFFFFFD;
    var_r4 = 0;
loop_2:
    if (var_r4 < (s32) modeCtrl.playerCount) {
        g_poolInfo.playerPool.statusList[var_r4] = 2;
        var_r4 += 1;
        goto loop_2;
    }
loop_5:
    if (var_r4 < 4) {
        g_poolInfo.playerPool.statusList[var_r4] = 0;
        var_r4 += 1;
        goto loop_5;
    }
    modeCtrl.currPlayer = 0;
    modeCtrl.gameType = 0xD;
    modeCtrl.unk30 = modeCtrl.playerCount;
    modeCtrl.unk40 = 1;
    main_mode_request = 4;
    sub_mode_request = 0xC7;
}

void menu_soccer_mode_select_tick(void) {
    u8 temp_r0;

    if ((s32) g_currently_visible_menu_screen.unk18 != 0) {
        g_something_with_stopping_music_or_sfx(3);
    }
    if ((s32) g_currently_visible_menu_screen.unk10 != 0) {
        temp_r0 = g_focused_root_menu.unk0[(s8) g_currently_visible_menu_screen.currScreen];
        switch ((s8) temp_r0) {                     /* irregular */
        case 0:
            call_SoundReqID_arg_0(0x358);
            break;
        case 1:
            call_SoundReqID_arg_0(0x359);
            modeCtrl.playerCount = 1;
            break;
        case 2:
            call_SoundReqID_arg_0(0x3AA);
            break;
        }
        lbl_1_bss_1720.unk0 = g_focused_root_menu.unk0[(s8) g_currently_visible_menu_screen.currScreen];
    }
}

void menu_start_soccer(void) {
    s32 var_r4;

    modeCtrl.courseFlags |= 4;
    g_related_to_pause_menu_input.flags &= 0xFFFFFFFD;
    var_r4 = 0;
loop_2:
    if (var_r4 < (s32) modeCtrl.playerCount) {
        g_poolInfo.playerPool.statusList[var_r4] = 2;
        var_r4 += 1;
        goto loop_2;
    }
loop_5:
    if (var_r4 < 4) {
        g_poolInfo.playerPool.statusList[var_r4] = 0;
        var_r4 += 1;
        goto loop_5;
    }
    modeCtrl.currPlayer = 0;
    modeCtrl.gameType = 0xC;
    modeCtrl.unk30 = modeCtrl.playerCount;
    modeCtrl.unk40 = 1;
    main_mode_request = 4;
    sub_mode_request = 0xC5;
}

void menu_baseball_mode_select_tick(void) {
    u8 temp_r0;

    if ((s32) g_currently_visible_menu_screen.unk10 != 0) {
        temp_r0 = g_focused_root_menu.unk0[(s8) g_currently_visible_menu_screen.currScreen];
        switch ((s8) temp_r0) {                     /* irregular */
        case 0:
            call_SoundReqID_arg_0(0x358);
            break;
        case 1:
            call_SoundReqID_arg_0(0x359);
            modeCtrl.playerCount = 1;
            break;
        }
        lbl_1_bss_172C.unk0 = g_focused_root_menu.unk0[(s8) g_currently_visible_menu_screen.currScreen];
    }
}

void menu_baseball_stadium_select_tick(void) {
    if ((s32) g_currently_visible_menu_screen.unkC != 0) {
        call_SoundReqID_arg_0(0x38E);
    }
    if (((s32) g_currently_visible_menu_screen.unkC != 0) || ((s32) g_currently_visible_menu_screen.unk18 != 0)) {
        fn_3_1EC64(1);
    }
    if ((s32) g_currently_visible_menu_screen.unk14 != 0) {
        fn_3_1EE08(1);
        if ((s32) g_currently_visible_menu_screen.unk4 == 0) {
            lbl_1_bss_172C.unk1[2] = (u8) g_focused_root_menu.unk0[(s8) g_currently_visible_menu_screen.currScreen];
            if ((s32) modeCtrl.playerCount == 2) {
                g_currently_visible_menu_screen.nextScreen = 0x46;
            }
        }
    }
}

void menu_start_baseball(void) {
    s32 var_r4;

    modeCtrl.courseFlags |= 4;
    g_related_to_pause_menu_input.flags &= 0xFFFFFFFD;
    var_r4 = 0;
loop_2:
    if (var_r4 < (s32) modeCtrl.playerCount) {
        g_poolInfo.playerPool.statusList[var_r4] = 2;
        var_r4 += 1;
        goto loop_2;
    }
loop_5:
    if (var_r4 < 4) {
        g_poolInfo.playerPool.statusList[var_r4] = 0;
        var_r4 += 1;
        goto loop_5;
    }
    modeCtrl.currPlayer = 0;
    modeCtrl.gameType = 0xF;
    modeCtrl.unk30 = modeCtrl.playerCount;
    modeCtrl.unk40 = 1;
    if ((s8) lbl_1_bss_172C.unk0 == 1) {
        lbl_1_bss_172C.unk0 = 2;
    } else if ((s32) modeCtrl.playerCount == 1) {
        lbl_1_bss_172C.unk0 = 1;
    } else {
        lbl_1_bss_172C.unk0 = 0;
    }
    main_mode_request = 4;
    sub_mode_request = 0xC9;
}

void menu_tennis_mode_select_tick(void) {
    u8 temp_r0;

    if ((s32) g_currently_visible_menu_screen.unk10 != 0) {
        temp_r0 = g_focused_root_menu.unk0[(s8) g_currently_visible_menu_screen.currScreen];
        switch ((s8) temp_r0) {                     /* irregular */
        case 0:
            call_SoundReqID_arg_0(0x38C);
            break;
        case 1:
            call_SoundReqID_arg_0(0x38D);
            break;
        case 2:
            call_SoundReqID_arg_0(0x359);
            modeCtrl.playerCount = 1;
            break;
        }
        lbl_1_bss_1738.unk0 = g_focused_root_menu.unk0[(s8) g_currently_visible_menu_screen.currScreen];
    }
}

void menu_tennis_pair_select_tick(void) {
    if ((s32) g_currently_visible_menu_screen.unkC != 0) {
        call_SoundReqID_arg_0(0x3C6);
    }
    if ((s32) g_currently_visible_menu_screen.unk14 != 0) {
        lbl_1_bss_1738.unk1 = (u8) g_focused_root_menu.unk0[(s8) g_currently_visible_menu_screen.currScreen];
    }
}

void menu_tennis_court_select_tick(void) {
    if ((s32) g_currently_visible_menu_screen.unkC != 0) {
        call_SoundReqID_arg_0(0x38F);
    }
    if (((s32) g_currently_visible_menu_screen.unkC != 0) || ((s32) g_currently_visible_menu_screen.unk18 != 0)) {
        fn_3_1EC64(2);
    }
    if ((s32) g_currently_visible_menu_screen.unk14 != 0) {
        fn_3_1EE08(2);
        if (((s32) g_currently_visible_menu_screen.unk4 == 0) && (((lbl_1_bss_1738.unk2 = (u8) g_focused_root_menu.unk0[(s8) g_currently_visible_menu_screen.currScreen], (((s8) lbl_1_bss_1738.unk0 == 0) != 0)) && ((s32) modeCtrl.playerCount == 2)) || (((s8) lbl_1_bss_1738.unk0 == 1) && ((s32) modeCtrl.playerCount == 4)))) {
            g_currently_visible_menu_screen.nextScreen = 0x4E;
        }
    }
}

void menu_start_tennis(void) {
    s32 var_r4;

    modeCtrl.courseFlags |= 4;
    g_related_to_pause_menu_input.flags &= 0xFFFFFFFD;
    var_r4 = 0;
loop_2:
    if (var_r4 < (s32) modeCtrl.playerCount) {
        g_poolInfo.playerPool.statusList[var_r4] = 2;
        var_r4 += 1;
        goto loop_2;
    }
loop_5:
    if (var_r4 < 4) {
        g_poolInfo.playerPool.statusList[var_r4] = 0;
        var_r4 += 1;
        goto loop_5;
    }
    modeCtrl.currPlayer = 0;
    modeCtrl.gameType = 0x10;
    modeCtrl.unk30 = modeCtrl.playerCount;
    modeCtrl.unk40 = 1;
    main_mode_request = 4;
    sub_mode_request = 0xCB;
}

void menu_option_replay_tick(void) {
    s32 var_r3;

    if (((s32) g_currently_visible_menu_screen.unkC != 0) || ((s32) g_currently_visible_menu_screen.unk18 != 0)) {
        if (are_all_party_games_unlocked() != 0) {
            var_r3 = 0x5A;
        } else {
            var_r3 = 5;
        }
        g_change_music_volume(var_r3, 0x1EU, 0x3C);
        fn_3_2462C();
        g_set_some_option_menu_sprite_param();
        return;
    }
    if ((s32) g_currently_visible_menu_screen.unk14 != 0) {
        fn_3_2523C();
        return;
    }
    fn_3_246E8();
}

void menu_option_play_points_tick(void) {
    if (((s32) g_currently_visible_menu_screen.unkC != 0) || ((s32) g_currently_visible_menu_screen.unk18 != 0)) {
        fn_3_26F40();
        g_set_some_option_menu_sprite_param();
        return;
    }
    if ((s32) g_currently_visible_menu_screen.unk14 != 0) {
        fn_3_27780();
        return;
    }
    fn_3_26FC0();
}

void menu_option_gift_tick(void) {
    struct TextBox tb;

    if (((s32) g_currently_visible_menu_screen.unkC != 0) || ((s32) g_currently_visible_menu_screen.unk18 != 0)) {
        fn_3_277A4();
        if (are_all_gifts_unlocked() == 0) {
            if (lbl_1_bss_A5C40[0].state == 0) {
                memset(&tb, 0, sizeof(tb));
                tb.flags = 0x74;
                tb.x = 0x21C;
                tb.y = 0x6E;
                tb.rows = 1;
                tb.style = 5;
                tb.textScale = 0.5f;
                g_display_playpoint_or_gift_message_child(0, 1, &tb);
                g_some_printf_function_4(0, lbl_3_rodata_74[(s8) g_language_id], get_play_point_count());
            }
        }
        g_set_some_option_menu_sprite_param();
        return;
    }
    if ((s32) g_currently_visible_menu_screen.unk14 != 0) {
        fn_3_290CC();
        g_display_playpoint_or_gift_message_child(0, 0x14, NULL);
        return;
    }
    g_draw_gift_menu();
}

void menu_option_ranking_tick(void) {
    if (((s32) g_currently_visible_menu_screen.unkC != 0) || ((s32) g_currently_visible_menu_screen.unk18 != 0)) {
        fn_3_2345C();
        g_set_some_option_menu_sprite_param();
        return;
    }
    if ((s32) g_currently_visible_menu_screen.unk14 != 0) {
        fn_3_23DB8();
        return;
    }
    fn_3_234DC();
}

void menu_option_game_data_tick(void) {
    if (((s32) g_currently_visible_menu_screen.unkC != 0) || ((s32) g_currently_visible_menu_screen.unk18 != 0)) {
        create_gamedata_sprite();
        g_set_some_option_menu_sprite_param();
        return;
    }
    if ((s32) g_currently_visible_menu_screen.unk14 != 0) {
        destroy_gamedata_sprite();
        return;
    }
    menu_option_game_data_tick_child();
}

void menu_option_controller_tick(void) {
    if (((s32) g_currently_visible_menu_screen.unkC != 0) || ((s32) g_currently_visible_menu_screen.unk18 != 0)) {
        g_draw_controller_sprites();
        g_set_some_option_menu_sprite_param();
        return;
    }
    if ((s32) g_currently_visible_menu_screen.unk14 != 0) {
        fn_3_2304C();
        return;
    }
    fn_3_21D50();
}

#ifndef __MWERKS__
/* PC port: the Game Screen page also holds the PC video/audio settings
 * (pc_port/src/game/shim/pc_settings_menu.c). */
void pc_screen_page_create(void);
void pc_screen_page_tick(void);
void pc_screen_page_destroy(void);
#define fn_3_20618 pc_screen_page_create
#define fn_3_20698 pc_screen_page_tick
#define fn_3_20A28 pc_screen_page_destroy
#endif
void menu_option_screen_tick(void) {
    if (((s32) g_currently_visible_menu_screen.unkC != 0) || ((s32) g_currently_visible_menu_screen.unk18 != 0)) {
        fn_3_20618();
        g_set_some_option_menu_sprite_param();
        return;
    }
    if ((s32) g_currently_visible_menu_screen.unk14 != 0) {
        fn_3_20A28();
        return;
    }
    fn_3_20698();
}
#ifndef __MWERKS__
#undef fn_3_20618
#undef fn_3_20698
#undef fn_3_20A28
#endif

void fn_3_1119C(void) {
    if (((s32) g_currently_visible_menu_screen.unkC != 0) || ((s32) g_currently_visible_menu_screen.unk18 != 0)) {
        fn_3_23DE4();
        g_set_some_option_menu_sprite_param();
        return;
    }
    if ((s32) g_currently_visible_menu_screen.unk14 != 0) {
        fn_3_242B4();
        return;
    }
    fn_3_23EA8();
}

void g_handle_starting_monkeys_count(s32 arg0, s32 arg1) {
    char buf[0x100];
    u8 temp_r29;

    if (((s32) g_currently_visible_menu_screen.unkC != 0) && (arg0 == 0)) {
        call_SoundReqID_arg_0(0x22D);
    }
    if (arg1 != 0) {

    } else if (arg0 == 0) {
        if ((s32) lbl_3_bss_6C[0] != 0) {
            temp_r29 = (u8) number_of_starting_monkeys;
            if (((s8) (u8) number_of_starting_monkeys < get_num_unlocked_monkeys()) && (did_any_pad_press_input(1U) != 0)) {
                number_of_starting_monkeys = (u8) number_of_starting_monkeys + 1;
            } else if (((s8) (u8) number_of_starting_monkeys > 1) && (did_any_pad_press_input(0U) != 0)) {
                number_of_starting_monkeys = (u8) number_of_starting_monkeys - 1;
            }
            if ((s8) temp_r29 != (s8) (u8) number_of_starting_monkeys) {
                call_SoundReqID_arg_0(0x65);
            }
        } else if ((s32) lbl_3_bss_6C[1] != 0) {
            number_of_starting_monkeys = (s8) lbl_3_bss_178.unk10;
        } else {
            lbl_3_bss_178.unk10 = (s32) (s8) (u8) number_of_starting_monkeys;
        }
    } else {
        sprintf(buf, "%d", (s8) (u8) number_of_starting_monkeys);
    }
    if (arg0 == 1) {
        textdraw_print_and_fit_to_width(70.0f, buf);
    }
}

void fn_3_11390(s32 arg0, s32 arg1) {
    char buf[0x100];
    s8 temp_r30;
    s8 var_r0;
    char *var_r5;
    char *var_r5_2;
    char *var_r5_3;

    if (((s32) g_currently_visible_menu_screen.unkC != 0) && (arg0 == 0)) {
        call_SoundReqID_arg_0(0x22D);
        if ((s32) modeCtrl.playerCount == 1) {
            lbl_1_bss_16D0.unk8 = (s32) (lbl_1_bss_16D0.unk8 | 1);
        }
    }
    switch (arg1) {                                 /* irregular */
    case 0:
        if (arg0 == 0) {
            temp_r30 = (s8) lbl_1_bss_16D0.unk1[(s8) lbl_1_bss_16D0.unk0];
            if ((s32) lbl_3_bss_6C[0] != 0) {
                if (did_any_pad_press_input(0U) != 0) {
                    lbl_1_bss_16D0.unk1[(s8) lbl_1_bss_16D0.unk0] = (u8) (lbl_1_bss_16D0.unk1[(s8) lbl_1_bss_16D0.unk0] - 1);
                }
                if (did_any_pad_press_input(1U) != 0) {
                    lbl_1_bss_16D0.unk1[(s8) lbl_1_bss_16D0.unk0] = (u8) (lbl_1_bss_16D0.unk1[(s8) lbl_1_bss_16D0.unk0] + 1);
                }
                if ((s8) lbl_1_bss_16D0.unk1[(s8) lbl_1_bss_16D0.unk0] < 1) {
                    var_r0 = 1;
                } else if ((s8) lbl_1_bss_16D0.unk1[(s8) lbl_1_bss_16D0.unk0] > 0x32) {
                    var_r0 = 0x32;
                } else {
                    var_r0 = (s8) lbl_1_bss_16D0.unk1[(s8) lbl_1_bss_16D0.unk0];
                }
                lbl_1_bss_16D0.unk1[(s8) lbl_1_bss_16D0.unk0] = var_r0;
                if (temp_r30 != (s8) lbl_1_bss_16D0.unk1[(s8) lbl_1_bss_16D0.unk0]) {
                    call_SoundReqID_arg_0(0x65);
                }
            } else if ((s32) lbl_3_bss_6C[1] != 0) {
                lbl_1_bss_16D0.unk1[(s8) lbl_1_bss_16D0.unk0] = (s8) lbl_3_bss_178.unk10;
            } else {
                lbl_3_bss_178.unk10 = (s32) (s8) lbl_1_bss_16D0.unk1[(s8) lbl_1_bss_16D0.unk0];
            }
            g_currently_visible_menu_screen.unk38 = NULL;
        } else {
            sprintf(buf, "%d", (s8) lbl_1_bss_16D0.unk1[(s8) lbl_1_bss_16D0.unk0]);
        }
        break;
    case 1:
        if (arg0 == 0) {
            if ((s32) modeCtrl.playerCount != 1) {
                if ((s32) lbl_3_bss_6C[0] != 0) {
                    if ((did_any_pad_press_input(0U) != 0) || (did_any_pad_press_input(1U) != 0)) {
                        lbl_1_bss_16D0.unk8 = (s32) (lbl_1_bss_16D0.unk8 ^ 1);
                        call_SoundReqID_arg_0(0x65);
                    }
                } else if ((s32) lbl_3_bss_6C[1] != 0) {
                    lbl_1_bss_16D0.unk8 = (s32) lbl_3_bss_178.unk10;
                } else {
                    lbl_3_bss_178.unk10 = (s32) lbl_1_bss_16D0.unk8;
                }
            } else {
                lbl_3_bss_6C[2] = 1;
                g_currently_visible_menu_screen.unk38 = lbl_3_data_57C4[(s8) g_language_id];
            }
        } else {
            if ((u32) (lbl_1_bss_16D0.unk8 & 1) != 0) {
                var_r5 = lbl_3_data_9D4;
            } else {
                var_r5 = lbl_3_data_9D8;
            }
            sprintf(buf, lbl_3_data_9D0, var_r5);
        }
        break;
    case 2:
        if (arg0 == 0) {
            if ((s32) lbl_3_bss_6C[0] != 0) {
                if ((did_any_pad_press_input(0U) != 0) || (did_any_pad_press_input(1U) != 0)) {
                    lbl_1_bss_16D0.unk8 = (s32) (lbl_1_bss_16D0.unk8 ^ 2);
                    call_SoundReqID_arg_0(0x65);
                }
            } else if ((s32) lbl_3_bss_6C[1] != 0) {
                lbl_1_bss_16D0.unk8 = (s32) lbl_3_bss_178.unk10;
            } else {
                lbl_3_bss_178.unk10 = (s32) lbl_1_bss_16D0.unk8;
            }
            g_currently_visible_menu_screen.unk38 = NULL;
        } else {
            if ((u32) (lbl_1_bss_16D0.unk8 & 2) != 0) {
                var_r5_2 = lbl_3_data_9D4;
            } else {
                var_r5_2 = lbl_3_data_9D8;
            }
            sprintf(buf, lbl_3_data_9D0, var_r5_2);
        }
        break;
    case 3:
        if (arg0 == 0) {
            if ((s32) lbl_3_bss_6C[0] != 0) {
                if ((did_any_pad_press_input(0U) != 0) || (did_any_pad_press_input(1U) != 0)) {
                    lbl_1_bss_16D0.unk8 = (s32) (lbl_1_bss_16D0.unk8 ^ 4);
                    call_SoundReqID_arg_0(0x65);
                }
            } else if ((s32) lbl_3_bss_6C[1] != 0) {
                lbl_1_bss_16D0.unk8 = (s32) lbl_3_bss_178.unk10;
            } else {
                lbl_3_bss_178.unk10 = (s32) lbl_1_bss_16D0.unk8;
            }
            g_currently_visible_menu_screen.unk38 = NULL;
        } else {
            if ((u32) (lbl_1_bss_16D0.unk8 & 4) != 0) {
                var_r5_3 = lbl_3_data_9D4;
            } else {
                var_r5_3 = lbl_3_data_9D8;
            }
            sprintf(buf, lbl_3_data_9D0, var_r5_3);
        }
        break;
    default:
        if (arg0 == 0) {
            g_currently_visible_menu_screen.unk38 = NULL;
        }
        break;
    }
    if (arg0 == 1) {
        textdraw_print_and_fit_to_width(70.0f, buf);
    }
}

void fn_3_11840(s32 arg0, s32 arg1) {
    fn_3_11390(arg0, arg1);
}

void fn_3_11860(s32 arg0, s32 arg1) {
    fn_3_11390(arg0, arg1 + 1);
}

void fn_3_11884(s32 arg0, s32 arg1) {
    char buf[0x100];
    s8 temp_r29;
    char *var_r5;

    if (((s32) g_currently_visible_menu_screen.unkC != 0) && (arg0 == 0)) {
        call_SoundReqID_arg_0(0x22D);
        if ((s32) modeCtrl.playerCount == 1) {
            lbl_1_bss_16DC.unk0 |= 1;
        } else if ((s32) modeCtrl.playerCount == 4) {
            lbl_1_bss_16DC.unk0 &= 0xFFFFFFFE;
        }
    }
    if (arg1 != 0) {
        if ((arg1 >= 0) && (arg1 < 5)) {
            if (arg0 == 0) {
                temp_r29 = (s8) lbl_1_bss_16DC.unk5[arg1 + 2];
                if ((s32) lbl_3_bss_6C[0] != 0) {
                    if ((did_any_pad_press_input(0U) != 0) && ((s8) lbl_1_bss_16DC.unk5[arg1 + 2] > 0)) {
                        lbl_1_bss_16DC.unk5[arg1 + 2] = (u8) (lbl_1_bss_16DC.unk5[arg1 + 2] - 1);
                    }
                    if ((did_any_pad_press_input(1U) != 0) && ((s8) lbl_1_bss_16DC.unk5[arg1 + 2] < 4)) {
                        lbl_1_bss_16DC.unk5[arg1 + 2] = (u8) (lbl_1_bss_16DC.unk5[arg1 + 2] + 1);
                    }
                    if (temp_r29 != (s8) lbl_1_bss_16DC.unk5[arg1 + 2]) {
                        call_SoundReqID_arg_0(0x65);
                    }
                } else if ((s32) lbl_3_bss_6C[1] != 0) {
                    lbl_1_bss_16DC.unk5[arg1 + 2] = (u8) (s8) lbl_3_bss_178.unk10;
                } else {
                    lbl_3_bss_178.unk10 = (s32) (s8) lbl_1_bss_16DC.unk5[arg1 + 2];
                }
                g_currently_visible_menu_screen.unk38 = NULL;
            } else {
                sprintf(buf, "%d", (s8) lbl_1_bss_16DC.unk5[arg1 + 2] + 1);
            }
        } else if (arg0 == 0) {
            g_currently_visible_menu_screen.unk38 = NULL;
        }
    } else if (arg0 == 0) {
        switch ((s32) modeCtrl.playerCount) {       /* irregular */
        case 2:
        case 3:
            if ((s32) lbl_3_bss_6C[0] != 0) {
                if ((did_any_pad_press_input(0U) != 0) || (did_any_pad_press_input(1U) != 0)) {
                    lbl_1_bss_16DC.unk0 ^= 1;
                    call_SoundReqID_arg_0(0x65);
                }
            } else if ((s32) lbl_3_bss_6C[1] != 0) {
                lbl_1_bss_16DC.unk0 = lbl_3_bss_178.unk10;
            } else {
                lbl_3_bss_178.unk10 = (s32) lbl_1_bss_16DC.unk0;
            }
            break;
        case 4:
            lbl_3_bss_6C[2] = 1;
            g_currently_visible_menu_screen.unk38 = lbl_3_data_583C[(s8) g_language_id];
            break;
        default:
            lbl_3_bss_6C[2] = 1;
            g_currently_visible_menu_screen.unk38 = lbl_3_data_57C4[(s8) g_language_id];
            break;
        }
    } else {
        if ((u32) (lbl_1_bss_16DC.unk0 & 1) != 0) {
            var_r5 = lbl_3_data_9D4;
        } else {
            var_r5 = lbl_3_data_9D8;
        }
        sprintf(buf, lbl_3_data_9D0, var_r5);
    }
    if (arg0 == 1) {
        textdraw_print_and_fit_to_width(70.0f, buf);
    }
}

void fn_3_11BB8(s32 arg0, s32 arg1) {
    char buf[0x100];
    s8 temp_r27;
    s8 temp_r27_2;
    s8 var_r28;
    char *var_r5;

    if ((s32) modeCtrl.playerCount == 1) {
        var_r28 = 5;
    } else if ((s32) modeCtrl.playerCount == 2) {
        var_r28 = 2;
    } else {
        var_r28 = 1;
    }
    if (((s32) g_currently_visible_menu_screen.unkC != 0) && (arg0 == 0)) {
        call_SoundReqID_arg_0(0x22D);
        if ((s8) lbl_1_bss_16E8.unk1 != 1) {
            lbl_1_bss_16E8.unk1 = (u8) var_r28;
        }
    }
    switch (arg1) {                                 /* irregular */
    case 0:
        if (arg0 == 0) {
            temp_r27 = (s8) lbl_1_bss_16E8.unk0;
            if ((s32) lbl_3_bss_6C[0] != 0) {
                if ((did_any_pad_press_input(0U) != 0) && ((s8) lbl_1_bss_16E8.unk0 > 3)) {
                    lbl_1_bss_16E8.unk0 -= 3;
                }
                if ((did_any_pad_press_input(1U) != 0) && ((s8) lbl_1_bss_16E8.unk0 < 6)) {
                    lbl_1_bss_16E8.unk0 += 3;
                }
                if (temp_r27 != (s8) lbl_1_bss_16E8.unk0) {
                    call_SoundReqID_arg_0(0x6C);
                }
            } else if ((s32) lbl_3_bss_6C[1] != 0) {
                lbl_1_bss_16E8.unk0 = (u8) (s8) lbl_3_bss_178.unk10;
            } else {
                lbl_3_bss_178.unk10 = (s32) (s8) lbl_1_bss_16E8.unk0;
            }
            g_currently_visible_menu_screen.unk38 = NULL;
        } else {
            sprintf(buf, "%d", (s8) lbl_1_bss_16E8.unk0);
        }
        break;
    case 1:
        if (arg0 == 0) {
            if ((s32) modeCtrl.playerCount <= 2) {
                temp_r27_2 = (s8) lbl_1_bss_16E8.unk1;
                if ((s32) lbl_3_bss_6C[0] != 0) {
                    if ((did_any_pad_press_input(0U) != 0) && ((s8) lbl_1_bss_16E8.unk1 > 1)) {
                        lbl_1_bss_16E8.unk1 = 1U;
                    }
                    if ((did_any_pad_press_input(1U) != 0) && ((s8) lbl_1_bss_16E8.unk1 < var_r28)) {
                        lbl_1_bss_16E8.unk1 = (u8) var_r28;
                    }
                    if (temp_r27_2 != (s8) lbl_1_bss_16E8.unk1) {
                        call_SoundReqID_arg_0(0x6C);
                    }
                } else if ((s32) lbl_3_bss_6C[1] != 0) {
                    lbl_1_bss_16E8.unk1 = (u8) (s8) lbl_3_bss_178.unk10;
                } else {
                    lbl_3_bss_178.unk10 = (s32) (s8) lbl_1_bss_16E8.unk1;
                }
            }
            if ((s32) modeCtrl.playerCount == 2) {
                g_currently_visible_menu_screen.unk38 = lbl_3_data_64DC[(s8) g_language_id];
            } else if ((s32) modeCtrl.playerCount >= 3) {
                lbl_3_bss_6C[2] = 1;
                g_currently_visible_menu_screen.unk38 = lbl_3_data_6598[(s8) g_language_id];
            } else {
                g_currently_visible_menu_screen.unk38 = NULL;
            }
        } else {
            sprintf(buf, "%d", (s8) lbl_1_bss_16E8.unk1);
        }
        break;
    case 2:
        if (arg0 == 0) {
            if ((s32) lbl_3_bss_6C[0] != 0) {
                if ((did_any_pad_press_input(0U) != 0) || (did_any_pad_press_input(1U) != 0)) {
                    lbl_1_bss_16E8.unk4 = (s32) (lbl_1_bss_16E8.unk4 ^ 2);
                    call_SoundReqID_arg_0(0x65);
                }
            } else if ((s32) lbl_3_bss_6C[1] != 0) {
                lbl_1_bss_16E8.unk4 = (s32) lbl_3_bss_178.unk10;
            } else {
                lbl_3_bss_178.unk10 = (s32) lbl_1_bss_16E8.unk4;
            }
            g_currently_visible_menu_screen.unk38 = NULL;
        } else {
            if ((u32) (lbl_1_bss_16E8.unk4 & 2) != 0) {
                var_r5 = lbl_3_data_9D4;
            } else {
                var_r5 = lbl_3_data_9D8;
            }
            sprintf(buf, lbl_3_data_9D0, var_r5);
        }
        break;
    default:
        if (arg0 == 0) {
            g_currently_visible_menu_screen.unk38 = NULL;
        }
        break;
    }
    if (arg0 == 1) {
        textdraw_print_and_fit_to_width(70.0f, buf);
    }
}

void fn_3_11FD4(s32 arg0, s32 arg1) {
    char buf[0x100];
    s32 var_r26;
    s32 var_r3;
    s32 var_r3_2;
    s32 var_r3_3;
    s8 temp_r30;
    u8 temp_r26;
    char *var_r5;

    var_r3 = 0;
    if (((s8) lbl_1_bss_16F0.unk6 == 0) && ((s8) lbl_1_bss_16F0.unk7 == 1)) {
        var_r3 = 1;
    }
    if (((s32) g_currently_visible_menu_screen.unkC != 0) && (arg0 == 0)) {
        call_SoundReqID_arg_0(0x22D);
        if (var_r3 == 0) {
            lbl_1_bss_16F0.unk0 = (u8) (s8) (lbl_1_bss_16F0.unk0 | 1);
        }
    }
    switch (arg1) {                                 /* irregular */
    case 0:
        if (arg0 == 0) {
            if ((s32) lbl_3_bss_6C[0] != 0) {
                if ((did_any_pad_press_input(0U) != 0) || (did_any_pad_press_input(1U) != 0)) {
                    lbl_1_bss_16F0.unk8 = (u8) (s8) (1 - lbl_1_bss_16F0.unk8);
                    call_SoundReqID_arg_0(0x65);
                }
            } else if ((s32) lbl_3_bss_6C[1] != 0) {
                lbl_1_bss_16F0.unk8 = (u8) (s8) lbl_3_bss_178.unk10;
            } else {
                lbl_3_bss_178.unk10 = (s32) (s8) lbl_1_bss_16F0.unk8;
            }
        } else {
            if ((s8) lbl_1_bss_16F0.unk8 == 1) {
                var_r5 = lbl_3_data_9D4;
            } else {
                var_r5 = lbl_3_data_9D8;
            }
            sprintf(buf, lbl_3_data_9D0, var_r5);
        }
        break;
    case 1:
        if (arg0 == 0) {
            temp_r30 = (s8) lbl_1_bss_16F0.unk0;
            if (var_r3 != 0) {
                var_r26 = 1;
            } else {
                var_r26 = 2;
            }
            if ((s32) lbl_3_bss_6C[0] != 0) {
                if ((did_any_pad_press_input(0U) != 0) && ((s8) lbl_1_bss_16F0.unk0 > 1)) {
                    lbl_1_bss_16F0.unk0 = (u8) (s8) (lbl_1_bss_16F0.unk0 - var_r26);
                }
                if ((did_any_pad_press_input(1U) != 0) && ((s8) lbl_1_bss_16F0.unk0 < 0x63)) {
                    lbl_1_bss_16F0.unk0 = (u8) (s8) (lbl_1_bss_16F0.unk0 + var_r26);
                }
                if (temp_r30 != (s8) lbl_1_bss_16F0.unk0) {
                    call_SoundReqID_arg_0(0x6C);
                }
            } else if ((s32) lbl_3_bss_6C[1] != 0) {
                lbl_1_bss_16F0.unk0 = (u8) (s8) lbl_3_bss_178.unk10;
            } else {
                lbl_3_bss_178.unk10 = (s32) (s8) lbl_1_bss_16F0.unk0;
            }
        } else {
            sprintf(buf, "%d", (s8) lbl_1_bss_16F0.unk0);
        }
        break;
    case 2:
        var_r3_2 = 0;
        if (((s8) lbl_1_bss_16F0.unk6 == 1) || ((s8) lbl_1_bss_16F0.unk7 == 1)) {
            var_r3_2 = 1;
        }
        if (var_r3_2 != 0) {
            var_r3_3 = 4;
        } else {
            var_r3_3 = active_monkey_id[1];
        }
        if (arg0 == 0) {
            temp_r26 = lbl_1_bss_16F0.unk1[var_r3_3];
            if ((s8) g_currently_visible_menu_screen.currScreen != 0x21) {
                if ((s32) lbl_3_bss_6C[0] != 0) {
                    if ((did_any_pad_press_input(0U) != 0) && ((s8) lbl_1_bss_16F0.unk1[var_r3_3] > 0)) {
                        lbl_1_bss_16F0.unk1[var_r3_3] = (u8) (lbl_1_bss_16F0.unk1[var_r3_3] - 1);
                    }
                    if ((did_any_pad_press_input(1U) != 0) && ((s8) lbl_1_bss_16F0.unk1[var_r3_3] < 4)) {
                        lbl_1_bss_16F0.unk1[var_r3_3] = (u8) (lbl_1_bss_16F0.unk1[var_r3_3] + 1);
                    }
                    if ((s8) temp_r26 != (s8) lbl_1_bss_16F0.unk1[var_r3_3]) {
                        call_SoundReqID_arg_0(0x6C);
                    }
                } else if ((s32) lbl_3_bss_6C[1] != 0) {
                    lbl_1_bss_16F0.unk1[var_r3_3] = (u8) (s8) lbl_3_bss_178.unk10;
                } else {
                    lbl_3_bss_178.unk10 = (s32) (s8) lbl_1_bss_16F0.unk1[var_r3_3];
                }
            }
        } else {
            sprintf(buf, lbl_3_data_9D0, lbl_3_rodata_144[(s8) lbl_1_bss_16F0.unk1[var_r3_3]][(s8) g_language_id]);
        }
        break;
    }
    if (arg0 == 1) {
        textdraw_print_and_fit_to_width(70.0f, buf);
    }
}

void fn_3_123E4(s32 arg0, s32 arg1) {
    char buf[0x100];
    s8 temp_r28;
    u8 temp_r0;
    u8 temp_r0_2;

    if (((s32) g_currently_visible_menu_screen.unkC != 0) && (arg0 == 0)) {
        call_SoundReqID_arg_0(0x22D);
    }
    if (arg1 != 0) {
        if (arg0 == 0) {
            g_currently_visible_menu_screen.unk38 = NULL;
        }
    } else if (arg0 == 0) {
        temp_r28 = (s8) lbl_1_bss_1704.unk1;
        if ((s32) lbl_3_bss_6C[0] != 0) {
            if (did_any_pad_press_input(0U) != 0) {
                temp_r0 = lbl_1_bss_1704.unk1 - 1;
                lbl_1_bss_1704.unk1 = temp_r0;
                if ((s8) temp_r0 < 0) {
                    lbl_1_bss_1704.unk1 = 2U;
                }
            }
            if (did_any_pad_press_input(1U) != 0) {
                temp_r0_2 = lbl_1_bss_1704.unk1 + 1;
                lbl_1_bss_1704.unk1 = temp_r0_2;
                if ((s8) temp_r0_2 > 2) {
                    lbl_1_bss_1704.unk1 = 0U;
                }
            }
            if (temp_r28 != (s8) lbl_1_bss_1704.unk1) {
                call_SoundReqID_arg_0(0x6C);
            }
        } else if ((s32) lbl_3_bss_6C[1] != 0) {
            lbl_1_bss_1704.unk1 = (u8) (s8) lbl_3_bss_178.unk10;
        } else {
            lbl_3_bss_178.unk10 = (s32) (s8) lbl_1_bss_1704.unk1;
        }
        if ((s8) lbl_1_bss_1704.unk1 == 1) {
            g_currently_visible_menu_screen.unk38 = lbl_3_data_7CDC[(s8) g_language_id];
        } else if ((s8) lbl_1_bss_1704.unk1 == 2) {
            g_currently_visible_menu_screen.unk38 = lbl_3_data_7DA0[(s8) g_language_id];
        } else {
            g_currently_visible_menu_screen.unk38 = NULL;
        }
    } else {
        sprintf(buf, lbl_3_data_9D0, lbl_3_rodata_1BC[(s8) lbl_1_bss_1704.unk1][(s8) g_language_id]);
    }
    if (arg0 == 1) {
        textdraw_print_and_fit_to_width(70.0f, buf);
    }
}

void fn_3_12644(s32 arg0, s32 arg1) {
    char buf[0x100];
    u8 temp_r30;
    u8 var_r0;
    char *var_r5;
    char *var_r5_2;
    char *var_r5_3;

    if (((s32) g_currently_visible_menu_screen.unkC != 0) && (arg0 == 0)) {
        call_SoundReqID_arg_0(0x22D);
        if ((s32) modeCtrl.playerCount == 1) {
            lbl_1_bss_1708.unk4 = (u16) (lbl_1_bss_1708.unk4 | 2);
        } else if ((s32) modeCtrl.playerCount == 4) {
            lbl_1_bss_1708.unk4 = (u16) (lbl_1_bss_1708.unk4 & 0xFFFFFFFD);
        }
        if ((u8) lbl_1_bss_1708.unk0 == 2) {
            lbl_1_bss_1708.unk1[lbl_1_bss_1708.unk0] = 1;
        }
    }
    switch (arg1) {                                 /* switch 1; irregular */
    case 0:                                         /* switch 1 */
        if (arg0 == 0) {
            temp_r30 = lbl_1_bss_1708.unk1[lbl_1_bss_1708.unk0];
            if ((u8) lbl_1_bss_1708.unk0 == 2) {
                lbl_3_bss_6C[2] = 1;
                g_currently_visible_menu_screen.unk38 = lbl_3_data_8738[(s8) g_language_id];
            } else {
                if ((s32) lbl_3_bss_6C[0] != 0) {
                    if (did_any_pad_press_input(0U) != 0) {
                        lbl_1_bss_1708.unk1[lbl_1_bss_1708.unk0] = (u8) (lbl_1_bss_1708.unk1[lbl_1_bss_1708.unk0] - 1);
                    }
                    if (did_any_pad_press_input(1U) != 0) {
                        lbl_1_bss_1708.unk1[lbl_1_bss_1708.unk0] = (u8) (lbl_1_bss_1708.unk1[lbl_1_bss_1708.unk0] + 1);
                    }
                    if ((u8) lbl_1_bss_1708.unk1[lbl_1_bss_1708.unk0] < 1U) {
                        var_r0 = 1;
                    } else if ((u8) lbl_1_bss_1708.unk1[lbl_1_bss_1708.unk0] > 0x32U) {
                        var_r0 = 0x32;
                    } else {
                        var_r0 = lbl_1_bss_1708.unk1[lbl_1_bss_1708.unk0];
                    }
                    lbl_1_bss_1708.unk1[lbl_1_bss_1708.unk0] = var_r0;
                    if ((s32) temp_r30 != (s32) lbl_1_bss_1708.unk1[lbl_1_bss_1708.unk0]) {
                        call_SoundReqID_arg_0(0x65);
                    }
                } else if ((s32) lbl_3_bss_6C[1] != 0) {
                    lbl_1_bss_1708.unk1[lbl_1_bss_1708.unk0] = (u8) lbl_3_bss_178.unk10;
                } else {
                    lbl_3_bss_178.unk10 = (s32) lbl_1_bss_1708.unk1[lbl_1_bss_1708.unk0];
                }
                g_currently_visible_menu_screen.unk38 = NULL;
            }
        } else {
            sprintf(buf, "%d", lbl_1_bss_1708.unk1[lbl_1_bss_1708.unk0]);
        }
        break;
    case 1:                                         /* switch 1 */
        if (arg0 == 0) {
            switch ((s32) modeCtrl.playerCount) {   /* switch 2; irregular */
            case 2:                                 /* switch 2 */
            case 3:                                 /* switch 2 */
                if ((s32) lbl_3_bss_6C[0] != 0) {
                    if ((did_any_pad_press_input(0U) != 0) || (did_any_pad_press_input(1U) != 0)) {
                        lbl_1_bss_1708.unk4 = (u16) (lbl_1_bss_1708.unk4 ^ 2);
                        call_SoundReqID_arg_0(0x65);
                    }
                } else if ((s32) lbl_3_bss_6C[1] != 0) {
                    lbl_1_bss_1708.unk4 = (u16) lbl_3_bss_178.unk10;
                } else {
                    lbl_3_bss_178.unk10 = (s32) lbl_1_bss_1708.unk4;
                }
                g_currently_visible_menu_screen.unk38 = NULL;
                break;
            case 4:                                 /* switch 2 */
                lbl_3_bss_6C[2] = 1;
                g_currently_visible_menu_screen.unk38 = lbl_3_data_583C[(s8) g_language_id];
                break;
            default:                                /* switch 2 */
                lbl_3_bss_6C[2] = 1;
                g_currently_visible_menu_screen.unk38 = lbl_3_data_57C4[(s8) g_language_id];
                break;
            }
        } else {
            if ((s32) (lbl_1_bss_1708.unk4 & 2) != 0) {
                var_r5 = lbl_3_data_9D4;
            } else {
                var_r5 = lbl_3_data_9D8;
            }
            sprintf(buf, lbl_3_data_9D0, var_r5);
        }
        break;
    case 2:                                         /* switch 1 */
        if (arg0 == 0) {
            if ((s32) lbl_3_bss_6C[0] != 0) {
                if ((did_any_pad_press_input(0U) != 0) || (did_any_pad_press_input(1U) != 0)) {
                    lbl_1_bss_1708.unk4 = (u16) (lbl_1_bss_1708.unk4 ^ 8);
                    call_SoundReqID_arg_0(0x65);
                }
            } else if ((s32) lbl_3_bss_6C[1] != 0) {
                lbl_1_bss_1708.unk4 = (u16) lbl_3_bss_178.unk10;
            } else {
                lbl_3_bss_178.unk10 = (s32) lbl_1_bss_1708.unk4;
            }
            g_currently_visible_menu_screen.unk38 = NULL;
        } else {
            if ((s32) (lbl_1_bss_1708.unk4 & 8) != 0) {
                var_r5_2 = lbl_3_data_9D4;
            } else {
                var_r5_2 = lbl_3_data_9D8;
            }
            sprintf(buf, lbl_3_data_9D0, var_r5_2);
        }
        break;
    case 3:                                         /* switch 1 */
        if (arg0 == 0) {
            if ((s32) lbl_3_bss_6C[0] != 0) {
                if ((did_any_pad_press_input(0U) != 0) || (did_any_pad_press_input(1U) != 0)) {
                    lbl_1_bss_1708.unk4 = (u16) (lbl_1_bss_1708.unk4 ^ 4);
                    call_SoundReqID_arg_0(0x65);
                }
            } else if ((s32) lbl_3_bss_6C[1] != 0) {
                lbl_1_bss_1708.unk4 = (u16) lbl_3_bss_178.unk10;
            } else {
                lbl_3_bss_178.unk10 = (s32) lbl_1_bss_1708.unk4;
            }
            g_currently_visible_menu_screen.unk38 = NULL;
        } else {
            if ((s32) (lbl_1_bss_1708.unk4 & 4) != 0) {
                var_r5_3 = lbl_3_data_9D4;
            } else {
                var_r5_3 = lbl_3_data_9D8;
            }
            sprintf(buf, lbl_3_data_9D0, var_r5_3);
        }
        break;
    default:                                        /* switch 1 */
        if (arg0 == 0) {
            g_currently_visible_menu_screen.unk38 = NULL;
        }
        break;
    }
    if (arg0 == 1) {
        textdraw_print_and_fit_to_width(70.0f, buf);
    }
}

void fn_3_12BAC(s32 arg0, s32 arg1) {
    fn_3_12644(arg0, arg1);
}

void fn_3_12BCC(s32 arg0, s32 arg1) {
    fn_3_12644(arg0, arg1 + 1);
}

void fn_3_12BF0(s32 arg0, s32 arg1) {
    char buf[0x100];
    char *var_r5;
    char *var_r5_2;
    char *var_r5_3;
    char *var_r5_4;

    if (((s32) g_currently_visible_menu_screen.unkC != 0) && (arg0 == 0)) {
        call_SoundReqID_arg_0(0x22D);
    }
    switch (arg1) {                                 /* irregular */
    case 0:
        if (arg0 == 0) {
            if ((s32) lbl_3_bss_6C[0] != 0) {
                if ((did_any_pad_press_input(0U) != 0) || (did_any_pad_press_input(1U) != 0)) {
                    g_auto_reload_setting.unk0 ^= 4;
                    call_SoundReqID_arg_0(0x65);
                }
            } else if ((s32) lbl_3_bss_6C[1] != 0) {
                g_auto_reload_setting.unk0 = lbl_3_bss_178.unk10;
            } else {
                lbl_3_bss_178.unk10 = (s32) g_auto_reload_setting.unk0;
            }
            g_currently_visible_menu_screen.unk38 = 0;
        } else {
            if ((u32) (g_auto_reload_setting.unk0 & 4) == 0) {
                var_r5 = lbl_3_data_9D4;
            } else {
                var_r5 = lbl_3_data_9D8;
            }
            sprintf(buf, lbl_3_data_9D0, var_r5);
        }
        break;
    case 1:
        if ((arg0 == 0) && ((s32) modeCtrl.playerCount >= 2)) {
            if ((s32) lbl_3_bss_6C[0] != 0) {
                if ((did_any_pad_press_input(0U) != 0) || (did_any_pad_press_input(1U) != 0)) {
                    g_auto_reload_setting.unk0 ^= 8;
                    call_SoundReqID_arg_0(0x65);
                }
            } else if ((s32) lbl_3_bss_6C[1] != 0) {
                g_auto_reload_setting.unk0 = lbl_3_bss_178.unk10;
            } else {
                lbl_3_bss_178.unk10 = (s32) g_auto_reload_setting.unk0;
            }
            g_currently_visible_menu_screen.unk38 = 0;
        } else {
            if ((u32) (g_auto_reload_setting.unk0 & 8) == 0) {
                var_r5_2 = lbl_3_data_9D4;
            } else {
                var_r5_2 = lbl_3_data_9D8;
            }
            sprintf(buf, lbl_3_data_9D0, var_r5_2);
        }
        break;
    case 2:
        if ((arg0 == 0) && ((s32) modeCtrl.playerCount >= 3)) {
            if ((s32) lbl_3_bss_6C[0] != 0) {
                if ((did_any_pad_press_input(0U) != 0) || (did_any_pad_press_input(1U) != 0)) {
                    g_auto_reload_setting.unk0 ^= 0x10;
                    call_SoundReqID_arg_0(0x65);
                }
            } else if ((s32) lbl_3_bss_6C[1] != 0) {
                g_auto_reload_setting.unk0 = lbl_3_bss_178.unk10;
            } else {
                lbl_3_bss_178.unk10 = (s32) g_auto_reload_setting.unk0;
            }
            g_currently_visible_menu_screen.unk38 = 0;
        } else {
            if ((u32) (g_auto_reload_setting.unk0 & 0x10) == 0) {
                var_r5_3 = lbl_3_data_9D4;
            } else {
                var_r5_3 = lbl_3_data_9D8;
            }
            sprintf(buf, lbl_3_data_9D0, var_r5_3);
        }
        break;
    case 3:
        if ((arg0 == 0) && ((s32) modeCtrl.playerCount >= 4)) {
            if ((s32) lbl_3_bss_6C[0] != 0) {
                if ((did_any_pad_press_input(0U) != 0) || (did_any_pad_press_input(1U) != 0)) {
                    g_auto_reload_setting.unk0 ^= 0x20;
                    call_SoundReqID_arg_0(0x65);
                }
            } else if ((s32) lbl_3_bss_6C[1] != 0) {
                g_auto_reload_setting.unk0 = lbl_3_bss_178.unk10;
            } else {
                lbl_3_bss_178.unk10 = (s32) g_auto_reload_setting.unk0;
            }
            g_currently_visible_menu_screen.unk38 = 0;
        } else {
            if ((u32) (g_auto_reload_setting.unk0 & 0x20) == 0) {
                var_r5_4 = lbl_3_data_9D4;
            } else {
                var_r5_4 = lbl_3_data_9D8;
            }
            sprintf(buf, lbl_3_data_9D0, var_r5_4);
        }
        break;
    default:
        if (arg0 == 0) {
            g_currently_visible_menu_screen.unk38 = 0;
        }
        break;
    }
    if (arg0 == 1) {
        textdraw_print_and_fit_to_width(70.0f, buf);
    }
}

void fn_3_12FA8(s32 arg0, s32 arg1) {
    char buf[0x100];
    s32 var_r27;
    s8 temp_r30;
    s8 var_r0;
    char *var_r5;

    var_r27 = arg1;
    if (((s32) g_currently_visible_menu_screen.unkC != 0) && (arg0 == 0)) {
        call_SoundReqID_arg_0(0x22D);
    }
    if ((s8) lbl_1_bss_1718.unk1 == 1) {
        var_r27 += 1;
    }
    switch (var_r27) {                              /* irregular */
    case 0:
        if (arg0 == 0) {
            temp_r30 = (s8) lbl_1_bss_1718.unk0;
            if ((s32) lbl_3_bss_6C[0] != 0) {
                if (did_any_pad_press_input(0U) != 0) {
                    lbl_1_bss_1718.unk0 -= 1;
                }
                if (did_any_pad_press_input(1U) != 0) {
                    lbl_1_bss_1718.unk0 += 1;
                }
                if ((s8) lbl_1_bss_1718.unk0 < 0) {
                    var_r0 = 0;
                } else if ((s8) lbl_1_bss_1718.unk0 > 2) {
                    var_r0 = 2;
                } else {
                    var_r0 = (s8) lbl_1_bss_1718.unk0;
                }
                lbl_1_bss_1718.unk0 = (u8) var_r0;
                if (temp_r30 != (s8) lbl_1_bss_1718.unk0) {
                    call_SoundReqID_arg_0(0x65);
                }
            } else if ((s32) lbl_3_bss_6C[1] != 0) {
                lbl_1_bss_1718.unk0 = (u8) (s8) lbl_3_bss_178.unk10;
            } else {
                lbl_3_bss_178.unk10 = (s32) (s8) lbl_1_bss_1718.unk0;
            }
            g_currently_visible_menu_screen.unk38 = NULL;
        } else {
            sprintf(buf, lbl_3_data_A40, (s8) lbl_1_data_65980[(s8) lbl_1_bss_1718.unk0], lbl_3_rodata_204[0][(s8) g_language_id]);
        }
        break;
    case 1:
        if (arg0 == 0) {
            if (((s32) modeCtrl.playerCount != 1) && ((s32) modeCtrl.playerCount != 4)) {
                if ((s32) lbl_3_bss_6C[0] != 0) {
                    if ((did_any_pad_press_input(0U) != 0) || (did_any_pad_press_input(1U) != 0)) {
                        lbl_1_bss_1718.unk4 = (s32) (lbl_1_bss_1718.unk4 ^ 1);
                        call_SoundReqID_arg_0(0x65);
                    }
                } else if ((s32) lbl_3_bss_6C[1] != 0) {
                    lbl_1_bss_1718.unk4 = (s32) lbl_3_bss_178.unk10;
                } else {
                    lbl_3_bss_178.unk10 = (s32) lbl_1_bss_1718.unk4;
                }
            }
            if ((s32) modeCtrl.playerCount == 4) {
                lbl_3_bss_6C[2] = 1;
                g_currently_visible_menu_screen.unk38 = lbl_3_data_583C[(s8) g_language_id];
            } else if ((s32) modeCtrl.playerCount == 1) {
                lbl_3_bss_6C[2] = 1;
                g_currently_visible_menu_screen.unk38 = lbl_3_data_57C4[(s8) g_language_id];
            }
        } else if ((s32) modeCtrl.playerCount == 1) {
            strcpy(buf, lbl_3_data_9D4);
        } else if ((s32) modeCtrl.playerCount == 4) {
            strcpy(buf, lbl_3_data_9D8);
        } else {
            if ((u32) (lbl_1_bss_1718.unk4 & 1) != 0) {
                var_r5 = lbl_3_data_9D4;
            } else {
                var_r5 = lbl_3_data_9D8;
            }
            sprintf(buf, lbl_3_data_9D0, var_r5);
        }
        break;
    default:
        if (arg0 == 0) {
            g_currently_visible_menu_screen.unk38 = NULL;
        }
        break;
    }
    if (arg0 == 1) {
        textdraw_print_and_fit_to_width(70.0f, buf);
    }
}

void fn_3_13370(s32 arg0, s32 arg1) {
    char buf[0x100];
    s8 temp_r28;
    s8 temp_r28_2;

    if (((s32) g_currently_visible_menu_screen.unkC != 0) && (arg0 == 0)) {
        call_SoundReqID_arg_0(0x22D);
    }
    if ((did_any_pad_press_input(6U) != 0) && ((s32) g_currently_visible_menu_screen.unkC == 0) && ((s32) g_currently_visible_menu_screen.unk18 == 0) && (arg0 == 0)) {
        g_SoftStreamStart_with_some_defaults_2(3);
        fn_1_35918(3);
    }
    switch (arg1) {                                 /* switch 1; irregular */
    case 0:                                         /* switch 1 */
        if (arg0 == 0) {
            temp_r28 = (s8) lbl_1_bss_1720.unk1[2];
            if ((s32) lbl_3_bss_6C[0] != 0) {
                if (did_any_pad_press_input(1U) != 0) {
                    switch ((s8) lbl_1_bss_1720.unk1[2]) { /* switch 2; irregular */
                    case 2:                         /* switch 2 */
                        lbl_1_bss_1720.unk1[2] = 3U;
                        break;
                    case 3:                         /* switch 2 */
                        lbl_1_bss_1720.unk1[2] = 5U;
                        break;
                    case 5:                         /* switch 2 */
                        lbl_1_bss_1720.unk1[2] = 0xAU;
                        break;
                    case 10:                        /* switch 2 */
                        lbl_1_bss_1720.unk1[2] = 0x14U;
                        break;
                    }
                }
                if (did_any_pad_press_input(0U) != 0) {
                    switch ((s8) lbl_1_bss_1720.unk1[2]) { /* switch 3 */
                    case 4:                         /* switch 3 */
                    case 6:                         /* switch 3 */
                    case 7:                         /* switch 3 */
                    case 8:                         /* switch 3 */
                    case 9:                         /* switch 3 */
                    case 11:                        /* switch 3 */
                    case 12:                        /* switch 3 */
                    case 13:                        /* switch 3 */
                    case 14:                        /* switch 3 */
                    case 15:                        /* switch 3 */
                    case 16:                        /* switch 3 */
                    case 17:                        /* switch 3 */
                    case 18:                        /* switch 3 */
                    case 19:                        /* switch 3 */
                        break;
                    case 20:                        /* switch 3 */
                        lbl_1_bss_1720.unk1[2] = 0xAU;
                        break;
                    case 10:                        /* switch 3 */
                        lbl_1_bss_1720.unk1[2] = 5U;
                        break;
                    case 5:                         /* switch 3 */
                        lbl_1_bss_1720.unk1[2] = 3U;
                        break;
                    case 3:                         /* switch 3 */
                        lbl_1_bss_1720.unk1[2] = 2U;
                        break;
                    }
                }
                if (temp_r28 != (s8) lbl_1_bss_1720.unk1[2]) {
                    call_SoundReqID_arg_0(0x6C);
                }
            } else if ((s32) lbl_3_bss_6C[1] != 0) {
                lbl_1_bss_1720.unk1[2] = (u8) (s8) lbl_3_bss_178.unk10;
            } else {
                lbl_3_bss_178.unk10 = (s32) (s8) lbl_1_bss_1720.unk1[2];
            }
        } else {
            sprintf(buf, lbl_3_data_A40, (s8) lbl_1_bss_1720.unk1[2], lbl_3_rodata_204[1][(s8) g_language_id]);
        }
        break;
    case 1:                                         /* switch 1 */
        if (arg0 == 0) {
            if ((s32) lbl_3_bss_6C[0] != 0) {
                if ((did_any_pad_press_input(0U) != 0) || (did_any_pad_press_input(1U) != 0)) {
                    lbl_1_bss_1720.unk1[3] = (u8) (s8) (1 - lbl_1_bss_1720.unk1[3]);
                    call_SoundReqID_arg_0(0x6C);
                }
            } else if ((s32) lbl_3_bss_6C[1] != 0) {
                lbl_1_bss_1720.unk1[3] = (u8) (s8) lbl_3_bss_178.unk10;
            } else {
                lbl_3_bss_178.unk10 = (s32) (s8) lbl_1_bss_1720.unk1[3];
            }
        } else if ((s8) lbl_1_bss_1720.unk1[3] == 0) {
            sprintf(buf, lbl_3_data_9D8);
        } else {
            sprintf(buf, lbl_3_data_9D4);
        }
        break;
    case 2:                                         /* switch 1 */
        if (arg0 == 0) {
            temp_r28_2 = (s8) lbl_1_bss_1720.unk5;
            if ((s32) lbl_3_bss_6C[0] != 0) {
                if ((did_any_pad_press_input(0U) != 0) && ((s8) lbl_1_bss_1720.unk5 > 0)) {
                    lbl_1_bss_1720.unk5 = (u8) (lbl_1_bss_1720.unk5 - 1);
                }
                if ((did_any_pad_press_input(1U) != 0) && ((s8) lbl_1_bss_1720.unk5 < 4)) {
                    lbl_1_bss_1720.unk5 = (u8) (lbl_1_bss_1720.unk5 + 1);
                }
                if (temp_r28_2 != (s8) lbl_1_bss_1720.unk5) {
                    call_SoundReqID_arg_0(0x6C);
                }
            } else if ((s32) lbl_3_bss_6C[1] != 0) {
                lbl_1_bss_1720.unk5 = (u8) (s8) lbl_3_bss_178.unk10;
            } else {
                lbl_3_bss_178.unk10 = (s32) (s8) lbl_1_bss_1720.unk5;
            }
        } else {
            sprintf(buf, lbl_3_data_9D0, lbl_3_rodata_204[2 + (s8) lbl_1_bss_1720.unk5][(s8) g_language_id]);
        }
        break;
    }
    if (arg0 == 1) {
        textdraw_print_and_fit_to_width(7e+01f, buf);
    }
}

void fn_3_137D4(s32 arg0, s32 arg1) {
    char buf[0x100];
    s32 var_r27;
    u8 var_r5;
    char *var_r5_2;

    var_r27 = arg1;
    if (((s32) g_currently_visible_menu_screen.unkC != 0) && (arg0 == 0)) {
        call_SoundReqID_arg_0(0x22D);
    }
    if (((s8) g_currently_visible_menu_screen.currScreen == 0x46) && (var_r27 >= 2)) {
        var_r27 += 1;
    }
    switch (var_r27) {                              /* irregular */
    case 0:
        if (arg0 == 0) {
            if ((s32) lbl_3_bss_6C[0] != 0) {
                if ((did_any_pad_press_input(0U) != 0) && ((s8) lbl_1_bss_172C.unk1[3] > 0)) {
                    lbl_1_bss_172C.unk1[3] = (u8) (lbl_1_bss_172C.unk1[3] - 1);
                    call_SoundReqID_arg_0(0x6C);
                } else if ((did_any_pad_press_input(1U) != 0) && ((s8) lbl_1_bss_172C.unk1[3] < 2)) {
                    lbl_1_bss_172C.unk1[3] = (u8) (lbl_1_bss_172C.unk1[3] + 1);
                    call_SoundReqID_arg_0(0x6C);
                }
            } else if ((s32) lbl_3_bss_6C[1] != 0) {
                lbl_1_bss_172C.unk1[3] = (u8) (s8) lbl_3_bss_178.unk10;
            } else {
                lbl_3_bss_178.unk10 = (s32) (s8) lbl_1_bss_172C.unk1[3];
            }
        } else {
            var_r5 = lbl_1_bss_172C.unk1[3];
            if (((s8) var_r5 == 1) && ((s8) lbl_1_bss_172C.unk0 == 0) && ((s32) modeCtrl.playerCount == 2)) {
                var_r5 = 3;
            }
            sprintf(buf, lbl_3_data_9D0, lbl_3_rodata_2AC[(s8) var_r5][(s8) g_language_id]);
        }
        break;
    case 1:
        if (arg0 == 0) {
            if ((s32) lbl_3_bss_6C[0] != 0) {
                if ((did_any_pad_press_input(0U) != 0) && ((s8) lbl_1_bss_172C.unk5 > 1)) {
                    lbl_1_bss_172C.unk5 = (u8) (lbl_1_bss_172C.unk5 - 1);
                    call_SoundReqID_arg_0(0x6C);
                } else if ((did_any_pad_press_input(1U) != 0) && ((s8) lbl_1_bss_172C.unk5 < 9)) {
                    lbl_1_bss_172C.unk5 = (u8) (lbl_1_bss_172C.unk5 + 1);
                    call_SoundReqID_arg_0(0x6C);
                }
            } else if ((s32) lbl_3_bss_6C[1] != 0) {
                lbl_1_bss_172C.unk5 = (u8) (s8) lbl_3_bss_178.unk10;
            } else {
                lbl_3_bss_178.unk10 = (s32) (s8) lbl_1_bss_172C.unk5;
            }
        } else {
            sprintf(buf, "%d", (s8) lbl_1_bss_172C.unk5);
        }
        break;
    case 2:
        if (arg0 == 0) {
            if ((s32) lbl_3_bss_6C[0] != 0) {
                if ((did_any_pad_press_input(0U) != 0) && ((s8) lbl_1_bss_172C.unk6 > 0)) {
                    lbl_1_bss_172C.unk6 = (u8) (lbl_1_bss_172C.unk6 - 1);
                    call_SoundReqID_arg_0(0x6C);
                } else if ((did_any_pad_press_input(1U) != 0) && ((s8) lbl_1_bss_172C.unk6 < 4)) {
                    lbl_1_bss_172C.unk6 = (u8) (lbl_1_bss_172C.unk6 + 1);
                    call_SoundReqID_arg_0(0x6C);
                }
            } else if ((s32) lbl_3_bss_6C[1] != 0) {
                lbl_1_bss_172C.unk6 = (u8) (s8) lbl_3_bss_178.unk10;
            } else {
                lbl_3_bss_178.unk10 = (s32) (s8) lbl_1_bss_172C.unk6;
            }
        } else {
            sprintf(buf, "%d", (s8) lbl_1_bss_172C.unk6 + 1);
        }
        break;
    case 3:
        if (arg0 == 0) {
            if ((s32) lbl_3_bss_6C[0] != 0) {
                if ((did_any_pad_press_input(0U) != 0) || (did_any_pad_press_input(1U) != 0)) {
                    lbl_1_bss_172C.unk8 = (s32) (lbl_1_bss_172C.unk8 ^ 1);
                    call_SoundReqID_arg_0(0x65);
                }
            } else if ((s32) lbl_3_bss_6C[1] != 0) {
                lbl_1_bss_172C.unk8 = (s32) lbl_3_bss_178.unk10;
            } else {
                lbl_3_bss_178.unk10 = (s32) lbl_1_bss_172C.unk8;
            }
        } else {
            if ((u32) (lbl_1_bss_172C.unk8 & 1) != 0) {
                var_r5_2 = lbl_3_data_9D4;
            } else {
                var_r5_2 = lbl_3_data_9D8;
            }
            sprintf(buf, lbl_3_data_9D0, var_r5_2);
        }
        break;
    }
    if (arg0 == 1) {
        textdraw_print_and_fit_to_width(70.0f, buf);
    }
}

void fn_3_13C28(s32 arg0, s32 arg1) {
    char buf[0x100];
    s32 var_r28;
    s32 var_r3;
    s32 var_r4;
    s8 temp_r28;
    s8 temp_r28_2;
    u8 temp_r29;
    char *var_r5;

    var_r28 = arg1;
    if (((s32) g_currently_visible_menu_screen.unkC != 0) && (arg0 == 0)) {
        call_SoundReqID_arg_0(0x22D);
    }
    if (((s8) g_currently_visible_menu_screen.currScreen == 0x4E) && (var_r28 >= 2)) {
        var_r28 += 1;
    }
    switch (var_r28) {                              /* irregular */
    case 0:
        if (arg0 == 0) {
            temp_r28 = (s8) lbl_1_bss_1738.unk3;
            if ((s32) lbl_3_bss_6C[0] != 0) {
                if ((did_any_pad_press_input(0U) != 0) && ((s8) lbl_1_bss_1738.unk3 > 2)) {
                    lbl_1_bss_1738.unk3 = (u8) (lbl_1_bss_1738.unk3 - 1);
                }
                if ((did_any_pad_press_input(1U) != 0) && ((s8) lbl_1_bss_1738.unk3 < 6)) {
                    lbl_1_bss_1738.unk3 = (u8) (lbl_1_bss_1738.unk3 + 1);
                }
                if (temp_r28 != (s8) lbl_1_bss_1738.unk3) {
                    call_SoundReqID_arg_0(0x6C);
                }
            } else if ((s32) lbl_3_bss_6C[1] != 0) {
                lbl_1_bss_1738.unk3 = (u8) (s8) lbl_3_bss_178.unk10;
            } else {
                lbl_3_bss_178.unk10 = (s32) (s8) lbl_1_bss_1738.unk3;
            }
        } else {
            sprintf(buf, "%d", (s8) lbl_1_bss_1738.unk3);
        }
        break;
    case 1:
        if (arg0 == 0) {
            temp_r28_2 = (s8) lbl_1_bss_1738.unk4;
            if ((s32) lbl_3_bss_6C[0] != 0) {
                if ((did_any_pad_press_input(0U) != 0) && ((s8) lbl_1_bss_1738.unk4 > 1)) {
                    lbl_1_bss_1738.unk4 = (u8) (lbl_1_bss_1738.unk4 - 2);
                }
                if ((did_any_pad_press_input(1U) != 0) && ((s8) lbl_1_bss_1738.unk4 < 5)) {
                    lbl_1_bss_1738.unk4 = (u8) (lbl_1_bss_1738.unk4 + 2);
                }
                if (temp_r28_2 != (s8) lbl_1_bss_1738.unk4) {
                    call_SoundReqID_arg_0(0x6C);
                }
            } else if ((s32) lbl_3_bss_6C[1] != 0) {
                lbl_1_bss_1738.unk4 = (u8) (s8) lbl_3_bss_178.unk10;
            } else {
                lbl_3_bss_178.unk10 = (s32) (s8) lbl_1_bss_1738.unk4;
            }
        } else {
            sprintf(buf, "%d", (s8) lbl_1_bss_1738.unk4);
        }
        break;
    case 2:
        var_r4 = 0;
        if (((s8) lbl_1_bss_1738.unk0 == 0) && ((s32) modeCtrl.playerCount == 1)) {
            var_r4 = 1;
        }
        if (var_r4 != 0) {
            var_r3 = active_monkey_id[1];
        } else {
            var_r3 = 4;
        }
        if (arg0 == 0) {
            temp_r29 = lbl_1_bss_1738.unk5[var_r3];
            if ((s32) lbl_3_bss_6C[0] != 0) {
                if ((did_any_pad_press_input(0U) != 0) && ((s8) lbl_1_bss_1738.unk5[var_r3] > 0)) {
                    lbl_1_bss_1738.unk5[var_r3] = (u8) (lbl_1_bss_1738.unk5[var_r3] - 1);
                }
                if ((did_any_pad_press_input(1U) != 0) && ((s8) lbl_1_bss_1738.unk5[var_r3] < 4)) {
                    lbl_1_bss_1738.unk5[var_r3] = (u8) (lbl_1_bss_1738.unk5[var_r3] + 1);
                }
                if ((s8) temp_r29 != (s8) lbl_1_bss_1738.unk5[var_r3]) {
                    call_SoundReqID_arg_0(0x6C);
                }
            } else if ((s32) lbl_3_bss_6C[1] != 0) {
                lbl_1_bss_1738.unk5[var_r3] = (u8) (s8) lbl_3_bss_178.unk10;
            } else {
                lbl_3_bss_178.unk10 = (s32) (s8) lbl_1_bss_1738.unk5[var_r3];
            }
        } else {
            sprintf(buf, lbl_3_data_9D0, lbl_3_rodata_30C[(s8) lbl_1_bss_1738.unk5[var_r3]][(s8) g_language_id]);
        }
        break;
    case 3:
        if (arg0 == 0) {
            if ((s32) lbl_3_bss_6C[0] != 0) {
                if ((did_any_pad_press_input(0U) != 0) || (did_any_pad_press_input(1U) != 0)) {
                    lbl_1_bss_1738.unkA = (u8) (s8) (lbl_1_bss_1738.unkA ^ 1);
                    call_SoundReqID_arg_0(0x65);
                }
            } else if ((s32) lbl_3_bss_6C[1] != 0) {
                lbl_1_bss_1738.unkA = (u8) (s8) lbl_3_bss_178.unk10;
            } else {
                lbl_3_bss_178.unk10 = (s32) (s8) lbl_1_bss_1738.unkA;
            }
        } else {
            if ((s8) lbl_1_bss_1738.unkA != 0) {
                var_r5 = lbl_3_data_9D4;
            } else {
                var_r5 = lbl_3_data_9D8;
            }
            sprintf(buf, lbl_3_data_9D0, var_r5);
        }
        break;
    }
    if (arg0 == 1) {
        textdraw_print_and_fit_to_width(70.0f, buf);
    }
}
