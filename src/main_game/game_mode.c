/* Main game mode: the sub_mode state machine for normal play. */
#include "types.h"
#include "ape_internal.h"
#include "ball.h"
#include "camera.h"
#include "course.h"
#include "dip.h"
#include "effect.h"
#include "event.h"
#include "input.h"
#include "item.h"
#include "libc.h"
#include "light.h"
#include "macros.h"
#include "mathutil.h"
#include "mode.h"
#include "os.h"
#include "pool.h"
#include "rand.h"
#include "recplay.h"
#include "sprite.h"
#include "stage.h"
#include "world.h"

struct MemcardRequest
{
    /*0x00*/ u8 unk0;
    u8 filler1[3];
    /*0x04*/ u32 flags;
    /*0x08*/ char *fileName;
    /*0x0C*/ void *files;
    u8 filler10[8];
}; /* size = 0x18 */

struct SelStageChoice
{
    /*0x00*/ s32 unk0;
    /*0x04*/ u32 unk4;
}; /* size = 0x8 */

struct ScoreRecord
{
    u8 unk0[0x50];
};

struct TextBox
{
    /*0x00*/ int state;
    u8 unk4[0xC - 0x4];
    /*0x0C*/ s16 x;
    /*0x0E*/ s16 y;
    u8 unk10[0x14 - 0x10];
    /*0x14*/ s8 unk14;
    /*0x15*/ s8 lines;
    /*0x16*/ u8 unk16;
    u8 unk17[0x1C - 0x17];
    /*0x1C*/ void (*callback)(struct TextBox *tb);
    u8 unk20[0x2C - 0x20];
    /*0x2C*/ u32 unk2C;
    u8 unk30[0x34 - 0x30];
}; /* size = 0x34 */

struct PlayerRank
{
    s32 playerId;
    s32 bananas;
    s32 rank;
};

/* Saved play statistics: per-stage and per-course fall and goal counts. */
struct PlayStats
{
    u32 dipSwitches;
    u16 stageFalls[500];
    u16 stageGoals[500];
    u16 begFalls[10];
    u16 begGoals[10];
    u16 begExtraFalls[10];
    u16 begExtraGoals[10];
    u16 advFalls[30];
    u16 advGoals[30];
    u16 advExtraFalls[10];
    u16 advExtraGoals[10];
    u16 expFalls[50];
    u16 expGoals[50];
    u16 expExtraFalls[10];
    u16 expExtraGoals[10];
    u16 masterFalls[10];
    u16 masterGoals[10];
    u16 masterExtraFalls[10];
    u16 masterExtraGoals[10];
    u16 storyFalls[100];
    u16 storyGoals[100];
};

extern void (*sub_mode_funcs[])(void);
extern void (*g_some_func_ptr_related_to_sub_mode2)(void);
extern void (*test_draw_func_ptr)(void);
extern s16 g_another_stage_id;
extern s32 active_monkey_id[];
extern s32 g_loaded_player_score;
extern u32 g_some_render_flag;
extern u8 storymode_save_files[];
extern s32 lbl_1_bss_CF3D8;
extern s8 g_last_used_memcard_slot_2;
extern struct SelStageChoice lbl_1_bss_1744;
extern struct ScoreRecord lbl_1_bss_D30FC[];
extern struct Event events[];
extern u8 g_some_gift_menu_flags;
extern s32 lbl_1_bss_D3350;
extern s32 locale;
extern struct PlayStats lbl_1_bss_D1320;

void md_game_func(void);
void fn_2_1298(void);
void g_some_dest_function_handler(void);
int get_next_stage_id(void);
int g_decrement_active_ball_monkey_count(void);
void g_smth_with_ending_course(void);
void fn_2_64DC(int player);
int fn_2_64F4(void);
void fn_2_6538(void);
void fn_2_6568(void);
void fn_2_6AEC(void);
void g_maybe_some_goal_func(void);
void g_preload_stage_or_results(void);
void fn_2_7198(void);
void fn_2_881C(void);
void g_some_storymode_dest_function(void);
void smd_game_scenario_init_child(void);
void smd_game_scenario_tick_child(void);

void OSReport(const char *fmt, ...);
u32 *OSGetStackPointer(void);
void add_stage_time(int time);
void call_SoundReqID_arg_0(int soundId);
void call_SoundReqID_arg_1(int soundId);
void compare_play_points_with_99999_after_exit_game(void);
void create_bonus_finish_or_perfect_sprite(int time);
void create_continue_sprites(void);
void create_fallout_or_bonus_finish_sprite(int time);
void create_go_sprite(int time);
void create_goal_and_jump_to_stage_sprites(int time);
void create_hud_sprites(void);
void create_player_num_and_ready_sprites(int time);
void create_replay_sprite(int time);
void create_stage_loadin_text_sprites(void);
void create_time_over_sprite(int time);
void debug_textdraw_set_pos(int x, int y);
void debug_textdraw_vsprintf_buf_1(char *fmt, ...);
void dest_all_events(void);
void dest_all_sprites(void);
void event_dest(int id);
void event_freeze(int id);
void event_init(int id);
void event_restart(int id);
void fade_screen_to_color(s32 type, u32 color, int duration);
void fn_1_1118BC(void);
void fn_1_118504(void);
void fn_1_11C02C(void);
void fn_1_16C5B4(void);
int fn_1_16C8D8(void);
int fn_1_16C910(void);
void fn_1_16C92C(void);
int fn_1_16D3E0(void);
void fn_1_2810(void);
void fn_1_296C(void);
void fn_1_358E8(void);
void fn_1_365F0(int a, int b);
void fn_1_46FFC(void);
void fn_1_56C18(void);
void fn_1_5FD20(void);
void fn_1_5FDC0(void);
int fn_1_601D8(void);
void fn_1_602BC(void);
void fn_1_6030C(int a, int b);
float fn_1_629C4(int id, float time);
void fn_1_AD9B0(int time);
void fn_1_C02AC(void);
BOOL fn_1_D36C8(void);
int fn_1_F0204(void);
void fn_1_FBFFC(void);
int fn_1_FC208(int, struct ScoreRecord *);
void fn_1_FC5E8(void);
void free_minigame_gma_tpl(void);
void g_NameEntry2_InitFirst(void);
void g_SoftStreamStart_with_some_defaults_2(int id);
s16 g_SoftStreamStart_with_some_defaults(int track);
void g_advance_stage_animation(void);
s16 g_another_SoftStreamStart_wrapper_of_some_sort(int track, u8 volume);
void g_call_maybe_sets_number_of_starting_monkeys(struct Ball *ball);
void g_change_music_volume(int track, u32 time, s8 volume);
u8 g_check_some_memcard_field(void);
int g_check_current_track(int track);
void g_create_some_game_over_and_player_num_sprites(int time);
void g_create_stage_player_wrapper(u8 playerIdx, int mode, u8 apeChara, u8 colorId, u8 lod,
                                   void (*drawFunc)(struct Ape *));
void g_display_playpoint_or_gift_message(int, float, float);
void g_display_playpoint_or_gift_message_child(int id, int state, struct TextBox *tb);
void g_smth_with_playpoint_or_gift_msg(int id, char *msg);
void g_fade_track_volume(int a, int b);
int g_get_next_stage_id(void);
int g_get_result_code(void);
int g_get_some_memcard_var(void);
void g_handle_world_bgm(int a);
void g_init_rendefc_for_stage(void);
void g_load_minigame_gma_tpl(int idx);
void g_make_some_continue_screen_sprites(void);
s16 g_maybe_related_to_music_crossfading(int track);
void g_nameentry2_init(void);
void g_preload_ape_model_for_stageselect(void);
void g_reset_cm_course(void);
void g_save_game_data(void);
void g_set_smth_with_sound(int a);
void g_set_unlockables_status(void);
void g_set_up_debugtext_buffer(void);
void g_smth_with_ending_course_2(void);
void g_some_replay_func4(int a, int b, struct MemcardRequest *req);
void g_some_set_ape_anim(struct Ape *ape, int a, int b);
void g_something_calls_sndPopGroup(void);
void g_something_with_iteratively_freeing_memory(void);
void g_something_with_recplay2(void);
void g_something_with_recplay3(int param_1);
void g_something_with_score(void);
void g_something_with_sound9(void);
void g_something_with_stopping_music_or_sfx(int track);
void g_sprintf_memcard_error(int a, char *buf);
void g_very_similar_to_puts(const char *);
int get_next_player_idx(void);
struct Sprite *get_sprite_with_unique_id(int tag);
void load_bmp_by_id(int id);
void load_stage(int stage);
s16 play_track_and_fade_out_other_tracks(int, int, int);
void preload_ape(int chara, int lod, int mesh, int scene);
void queue_stage_load(int stageId);
void set_minimap_mode(int mode);
void set_sub_mode_destination(void (*func)(void));
void smd_game_over_save_child(void);
void toggle_minimap_zoom(void);
void call_free_bmp_by_id(int id);
void create_copyright_text_sprites(int arg0);
void create_go_to_extra_or_master_stages_sprites(int arg0);
void create_save_sprite(void);
void fn_1_16997C(void);
void fn_1_D391C(void);
void fn_1_EFFD4(int file);
void fn_1_FC304(void);
void fn_2_694C(void);
void fn_2_884C(void);
void g_preload_next_stage_files(int id, int scene, int frame);
void g_return_to_sel_ngc(int a);
void g_smth_with_staff_roll_init_7(void);
int g_swapDiscQueueGroup(int group);
int is_master_unlocked(void);
void set_storymode_bananas(int);
void set_storymode_score(int);
void staff_roll_init(void);
void unlock_master(void);

u32 lbl_2_data_0[] = {
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

const s16 lbl_2_rodata_0[] = { 35, 36, 37, 38, 39, 40, 41, 42, 43, 44 };

static s32 lbl_2_bss_0;
static s32 lbl_2_bss_4;
static s32 lbl_2_bss_8;
static s8 lbl_2_bss_C;
static struct PlayerRank lbl_2_bss_10[4];
static s8 lbl_2_bss_40;
void *lbl_2_bss_44;

void _prolog(void)
{
    g_some_func_ptr_related_to_sub_mode2 = md_game_func;
    test_draw_func_ptr = NULL;
    g_something_with_iteratively_freeing_memory();
    g_load_minigame_gma_tpl(0);
    g_very_similar_to_puts("RELOCATION MAIN_LOOP LOADED\n");
}

void _epilog(void)
{
    g_some_func_ptr_related_to_sub_mode2 = NULL;
    test_draw_func_ptr = NULL;
    dest_all_events();
    fn_1_1118BC();
    g_something_with_sound9();
    fn_1_5FDC0();
    free_minigame_gma_tpl();
    g_something_with_iteratively_freeing_memory();
    g_very_similar_to_puts("RELOCATION MAIN_LOOP PURGED\n");
}

void _unresolved(void)
{
    u32 i;
    u32 *sp;

    g_very_similar_to_puts("\nError: A called an unlinked function.\n");
    g_very_similar_to_puts("Address:      Back Chain    LR Save\n");
    i = 0;
    sp = OSGetStackPointer();
    while (sp != NULL && (u32)sp != 0xFFFFFFFF && i++ < 16)
    {
        printf("0x%08x:   0x%08x    0x%08x\n", sp, sp[0], sp[1]);
        sp = (u32 *)sp[0];
    }
    OSPanic("game_mode.c", 189, "\n");
}

void md_game_func(void)
{
    if (sub_mode <= 0x2F || sub_mode >= 0x70)
    {
        debug_textdraw_set_pos(10, 10);
        debug_textdraw_vsprintf_buf_1("sub_mode: error %d in Game", sub_mode);
    }
    else
    {
        sub_mode_funcs[sub_mode]();
    }
}

void smd_game_first_init(void)
{
    int i;

    modeCtrl.courseFlags &= ~0x40;
    modeCtrl.currPlayer = 0;
    dest_all_events();
    g_something_with_iteratively_freeing_memory();
    load_stage(0);
    fn_1_56C18();
    event_init(0x13);
    fn_1_358E8();
    g_SoftStreamStart_with_some_defaults(0x29);
    g_SoftStreamStart_with_some_defaults(0x2C);
    load_bmp_by_id(5);
    for (i = 0; i < 4; i++)
        g_call_maybe_sets_number_of_starting_monkeys(&balls[i]);
    set_minimap_mode(4);
    g_reset_cm_course();
    if (modeCtrl.gameType == 0 && modeCtrl.playerCount == 1)
        g_set_unlockables_status();
    if (lbl_1_bss_1744.unk0 != 0)
        mode_info.currFloor = lbl_1_bss_1744.unk4;
    lbl_2_bss_0 = -1;
    fn_1_FBFFC();
    fn_1_FC5E8();
    g_NameEntry2_InitFirst();
    fn_1_5FD20();
    if (modeCtrl.courseFlags & 1)
        fade_screen_to_color(0x100, 0, 30);
    switch (modeCtrl.gameType)
    {
    case 0:
        for (i = 0; i < 4; i++)
            set_camera_view_settings(i, 0.0f, 0.0f, 1.0f, 1.0f);
        break;
    default:
        if (modeCtrl.gameType == 1 && modeCtrl.playerCount == 3)
        {
            switch (modeCtrl.splitscreenMode)
            {
            case 0:
            default:
                camera_setup_splitscreen_viewports(3);
                break;
            case 1:
                set_camera_view_settings(0, 0.0f, 0.5f, 0.5f, 0.5f);
                set_camera_view_settings(1, 0.0f, 0.0f, 1.0f, 0.5f);
                set_camera_view_settings(2, 0.5f, 0.5f, 0.5f, 0.5f);
                set_camera_view_settings(3, 0.0f, 0.0f, 0.0f, 0.0f);
                break;
            case 2:
                set_camera_view_settings(0, 0.0f, 0.0f, 0.5f, 0.5f);
                set_camera_view_settings(1, 0.5f, 0.0f, 0.5f, 0.5f);
                set_camera_view_settings(2, 0.0f, 0.5f, 1.0f, 0.5f);
                set_camera_view_settings(3, 0.0f, 0.0f, 0.0f, 0.0f);
                break;
            case 3:
                camera_setup_splitscreen_viewports(4);
                break;
            }
        }
        else
        {
            camera_setup_splitscreen_viewports(modeCtrl.playerCount);
        }
        break;
    }
    sub_mode_request = 0x30;
}

void smd_game_restart_init(void)
{
    dest_all_events();
    g_something_with_iteratively_freeing_memory();
    fn_1_56C18();
    event_init(0x13);
    load_bmp_by_id(5);
    g_call_maybe_sets_number_of_starting_monkeys(&balls[modeCtrl.currPlayer]);
    set_minimap_mode(4);
    if (modeCtrl.gameType == 0 && modeCtrl.playerCount == 1)
        g_set_unlockables_status();
    sub_mode_request = 0x30;
}

void smd_game_ready_init(void)
{
    int next;

    if (g_some_other_flags & 0xA)
        return;
    modeCtrl.submodeTimer = 120;
    g_set_up_debugtext_buffer();
    dest_all_events();
    switch (modeCtrl.gameType)
    {
    case 0:
        if (!(modeCtrl.courseFlags & 0x100))
            fn_1_2810();
        else
            fn_1_296C();
        g_update_cm_course(modeCtrl.difficulty, mode_info.currFloor, modeCtrl.courseFlags);
        break;
    }
    lbl_2_bss_4 = mode_info.currFloor;
    load_stage(g_another_stage_id);
    event_init(0);
    event_init(1);
    event_init(2);
    event_init(3);
    event_init(4);
    event_init(9);
    event_init(5);
    event_init(7);
    if (modeCtrl.courseFlags & 1)
        event_init(0xA);
    event_init(0xF);
    event_init(0x10);
    event_init(0x11);
    event_init(0x13);
    event_init(6);
    event_init(0xE);
    event_init(0x15);
    event_init(0x14);
    event_init(0xB);
    g_init_rendefc_for_stage();
    event_freeze(1);
    g_create_stage_player_wrapper(modeCtrl.currPlayer, 30, active_monkey_id[modeCtrl.currPlayer],
                                  (modeCtrl.playerCount > 1) ? modeCtrl.currPlayer : 0xFF, 0,
                                  ape_default_anim);
    balls[modeCtrl.currPlayer].unk179 = 3;
    enable_camera(modeCtrl.currPlayer);
    camera_set_state(10);
    light_init(current_stage_id);
    switch (modeCtrl.gameType)
    {
    case 14:
        g_preload_ape_model_for_stageselect();
        break;
    default:
        next = get_next_stage_id();
        if (next != -1 && next != lbl_2_bss_0)
        {
            queue_stage_load(next);
            lbl_2_bss_0 = next;
            if (g_some_other_flags & 4)
                printf("pre_load_stage init:%d now:%d\n", next, current_stage_id);
        }
        next = get_next_player_idx();
        preload_ape(active_monkey_id[next], 0, 0, 1);
        if (g_some_other_flags & 4)
            printf("NewApeAllPreLoad:%d\n", active_monkey_id[next]);
        if (mode_info.flags & 0x1000)
            g_preload_stage_or_results();
        break;
    }
    mode_info.flags |= 0x108;
    if (modeCtrl.gameType == 1 && modeCtrl.playerCount == 3 && modeCtrl.splitscreenMode == 3)
    {
        cameras[3].flags |= 0x70;
        cameras[3].mode = 0x44;
    }
    if (mode_info.attempts == 1)
    {
        modeCtrl.submodeTimer = 360;
        camera_set_state(0x26);
        create_stage_loadin_text_sprites();
    }
    create_hud_sprites();
    g_handle_world_bgm(100);
    if (modeCtrl.gameType == 1)
    {
        int sounds[] = { 0x186, 0x187, 0x188, 0x189, 0x18A };
        int sound = sounds[mode_info.currFloor - 1];

        if (g_some_course_length == mode_info.currFloor)
            sound = 0x184;
        call_SoundReqID_arg_0(sound);
    }
    g_loaded_player_score = balls[modeCtrl.currPlayer].score;
    g_some_render_flag |= 0x40;
    modeCtrl.unk43 = 0;
    fade_screen_to_color(0x100, 0, 30);
    set_sub_mode_destination(fn_2_1298);
    sub_mode_request = 0x31;
    modeCtrl.unk3C = modeCtrl.submodeTimer;
}

void smd_game_ready_tick(void)
{
    if (g_some_other_flags & 0xA)
        return;
    if (mode_info.attempts == 1 && (merged_digital_inputs[0] & 0x100) && modeCtrl.submodeTimer > 120)
        modeCtrl.submodeTimer--;
    if (modeCtrl.submodeTimer == 120)
    {
        struct Sprite *sprite = get_sprite_with_unique_id(0xF);

        if (sprite != NULL)
            sprite->userVar = 15;
        create_player_num_and_ready_sprites(120);
        set_minimap_mode(3);
    }
    if (modeCtrl.submodeTimer == 24.0)
        BALL_FOREACH(ball->state = 0x1F;)
    if (merged_digital_inputs[2] & 0x100)
        toggle_minimap_zoom();
    if ((dip_switches & 1) && (merged_digital_inputs[2] & 8))
        add_stage_time(6000);
    if (--modeCtrl.submodeTimer <= 0)
        sub_mode_request = 0x32;
}

void smd_game_play_init(void)
{
    int i;

    if (g_some_other_flags & 0xA)
        return;
    event_restart(1);
    create_go_sprite(60);
    mode_info.flags &= ~0x108;
    fn_1_602BC();
    lbl_1_bss_71EC8.unk14 = 0;
    fn_1_5FDC0();
    switch (modeCtrl.gameType)
    {
    case 1:
        for (i = 0; i < modeCtrl.playerCount; i++)
        {
            lbl_1_bss_71EC8.unk0[i] = i + 13;
            fn_1_6030C(i, lbl_1_bss_71EC8.unk0[i]);
        }
        break;
    case 0:
        lbl_1_bss_71EC8.unk14 = modeCtrl.currPlayer;
        /* fall through */
    default:
        lbl_1_bss_71EC8.unk0[lbl_1_bss_71EC8.unk14] = fn_1_601D8();
        fn_1_6030C(lbl_1_bss_71EC8.unk14, lbl_1_bss_71EC8.unk0[lbl_1_bss_71EC8.unk14]);
        break;
    }
    g_something_with_recplay2();
    {
        struct World *world = world_infos;
        s8 *status = g_poolInfo.playerPool.statusList;
        int j;

        for (j = 0; j < g_poolInfo.playerPool.count; j++, world++, status++)
        {
            if (*status == 2)
                world->state = 1;
        }
    }
    camera_set_state(0x4B);
    sub_mode_request = 0x33;
}

void smd_game_play_tick(void)
{
    if (g_some_other_flags & 0xA)
        return;
    if (mode_info.timerCurr % 60 == 59 && !(dip_switches & 0x20))
    {
        if (mode_info.timerCurr <= 600)
        {
            call_SoundReqID_arg_0(6);
            call_SoundReqID_arg_0(lbl_2_rodata_0[mode_info.timerCurr / 60]);
        }
        else
        {
            call_SoundReqID_arg_0(0x3D806);
        }
        if (mode_info.timerCurr / 60 == 10)
        {
            call_SoundReqID_arg_0(7);
            fn_1_C02AC();
            g_fade_track_volume(1, 8);
            g_SoftStreamStart_with_some_defaults_2(0x29);
        }
    }
    if (mode_info.timerCurr <= 300 && mode_info.timerCurr % 60 == 43)
        call_SoundReqID_arg_0(6);
    if (mode_info.timerCurr <= 600 && mode_info.timerCurr % 60 == 51)
        call_SoundReqID_arg_0(6);
    if (merged_digital_inputs[2] & 0x100)
        toggle_minimap_zoom();
    if ((dip_switches & 1) && (merged_digital_inputs[2] & 8))
        add_stage_time(6000);
    if (mode_info.flags & 1)
    {
        mode_info.flags &= ~1;
        g_maybe_some_goal_func();
        sub_mode_request = 0x34;
    }
    else if (mode_info.flags & 2)
    {
        mode_info.flags &= ~2;
        BALL_FOREACH(ball->state = 0;)
        fn_2_6AEC();
        sub_mode_request = 0x38;
    }
    else if (mode_info.flags & 4)
    {
        mode_info.flags &= ~4;
        camera_set_state(4);
        g_something_with_score();
        fn_2_6AEC();
        sub_mode_request = 0x3A;
    }
    else if (mode_info.flags & 0x200)
    {
        mode_info.flags &= ~0x200;
        g_something_with_score();
        g_maybe_some_goal_func();
        sub_mode_request = 0x46;
    }
}

void fn_2_1298(void)
{
    switch (sub_mode)
    {
    case 0x31:
    case 0x32:
    case 0x33:
        set_sub_mode_destination(fn_2_1298);
        break;
    default:
        fn_2_7198();
        break;
    }
}

void smd_game_goal_init(void)
{
    int next;

    if (g_some_other_flags & 0xA)
        return;
    if (!(mode_info.flags & 0x2000))
    {
        modeCtrl.submodeTimer = 360;
        modeCtrl.courseFlags &= ~0x400;
    }
    else
    {
        modeCtrl.submodeTimer = 120;
        modeCtrl.courseFlags &= ~0x400;
        call_SoundReqID_arg_0(0xB);
    }
    modeCtrl.unk3C = modeCtrl.submodeTimer;
    switch (modeCtrl.gameType)
    {
    default:
        next = get_next_stage_id();
        if (next != -1 && next != lbl_2_bss_0)
        {
            queue_stage_load(next);
            lbl_2_bss_0 = next;
        }
        break;
    case 14:
        break;
    }
    set_minimap_mode(1);
    camera_set_state(0xE);
    if (!(mode_info.flags & 0x2000))
        create_goal_and_jump_to_stage_sprites(360);
    else
        create_time_over_sprite(120);
    if (modeCtrl.gameType != 1)
        increment_score(3, 0);
    if (mode_info.flags & 0x40)
        g_something_with_score();
    modeCtrl.unk43 = 1;
    if (g_maybe_related_to_music_crossfading(0x29) != -1)
        g_handle_world_bgm(100);
    sub_mode_request = 0x35;
}

void smd_game_goal_tick(void)
{
    int timeover;

    if (g_some_other_flags & 0xA)
        return;
    timeover = (mode_info.flags & 0x2000) != 0;
    if (timeover == 0 && modeCtrl.submodeTimer == 330)
        call_SoundReqID_arg_0(8);
    if (modeCtrl.submodeTimer == modeCtrl.unk3C - 60)
        g_something_with_recplay3(1);
    if (timeover == 0)
    {
        BALL_FOREACH(
            if (!(ball->flags & 0x200) && (ball->ape->flags & 0x4000))
            {
                modeCtrl.courseFlags |= 0x400;
                call_SoundReqID_arg_0(0x126);
            }
        )
    }
    if (timeover == 0 && (modeCtrl.courseFlags & 0x400) && modeCtrl.submodeTimer > 60
     && modeCtrl.submodeTimer < 240)
        modeCtrl.submodeTimer = 60;
    if (--modeCtrl.submodeTimer <= 0)
    {
        mode_info.flags &= ~0x2000;
        sub_mode_request = 0x36;
    }
}

void smd_game_goal_replay_init(void)
{
    int time;

    if (g_some_other_flags & 0xA)
        return;
    event_dest(0xB);
    modeCtrl.submodeTimer = 210;
    modeCtrl.submodeTimer = modeCtrl.submodeTimer + 300.0 * mathutil_vec_len(&mode_info.unk10);
    if (modeCtrl.submodeTimer > 300.0)
        modeCtrl.submodeTimer = 300;
    modeCtrl.courseFlags &= ~0x400;
    modeCtrl.unk18 = 30;
    BALL_FOREACH(ball->flags &= ~0x200;)
    BALL_FOREACH(ball->state = 9;)
    {
        struct World *world = world_infos;
        s8 *status = g_poolInfo.playerPool.statusList;
        int i;

        for (i = 0; i < g_poolInfo.playerPool.count; i++, world++, status++)
        {
            if (*status == 2)
                world->state = 6;
        }
    }
    camera_set_state(0x10);
    fn_1_46FFC();
    fn_1_6869C(0);
    fn_1_6869C(4);
    lbl_1_bss_71EC8.unk14 = mode_info.playerId;
    mode_info.flags |= 0x10;
    lbl_1_bss_71EC8.unk10 = MIN((float)(modeCtrl.submodeTimer - 60),
                                fn_1_61B18(lbl_1_bss_71EC8.unk0[lbl_1_bss_71EC8.unk14]));
    g_advance_stage_animation();
    time = fn_1_629C4(lbl_1_bss_71EC8.unk0[lbl_1_bss_71EC8.unk14], lbl_1_bss_71EC8.unk10);
    item_replay_init_all(time);
    fn_1_AD9B0(time);
    create_replay_sprite(modeCtrl.submodeTimer);
    if (modeCtrl.gameType != 1)
        increment_score(2, 0);
    g_something_with_score();
    g_set_smth_with_sound(1);
    sub_mode_request = 0x37;
}

void smd_game_goal_replay_tick(void)
{
    int next;

    if (g_some_other_flags & 0xA)
        return;
    if (current_ball->state == 0x20)
        mode_info.flags &= ~0x10;
    BALL_FOREACH(
        if (!(ball->flags & 0x200) && (ball->ape->flags & 0x4000))
        {
            ball->flags &= ~0x500;
            ball->flags |= 0x200;
            modeCtrl.courseFlags |= 0x400;
            call_SoundReqID_arg_0(0x126);
        }
    )
    if (modeCtrl.gameType == 1)
    {
        int gained = 0;

        BALL_FOREACH(
            if (!(g_font_char_var_3 & 3) && ball->bonus > 0)
            {
                int n = 1;

                if (ball->bonus > 100)
                    n = 41;
                else if (ball->bonus > 10)
                    n = 3;
                ball->bananas += n;
                ball->bonus -= n;
                ball->bananas = (ball->bananas < 999) ? ball->bananas : 999;
                gained = 1;
            }
        )
        if (gained != 0)
            call_SoundReqID_arg_0(0x67);
    }
    modeCtrl.unk18--;
    next = g_get_next_stage_id();
    switch (modeCtrl.gameType)
    {
    case 14:
        if (next < 0)
        {
            if (modeCtrl.submodeTimer == 60)
            {
                fade_screen_to_color(0x101, 0, modeCtrl.submodeTimer + 1);
                g_fade_track_volume(modeCtrl.submodeTimer, 2);
            }
        }
        else
        {
            if (fn_1_D36C8() == 0 && (merged_digital_inputs[2] & 0x100) && modeCtrl.unk18 < 0
             && (float)modeCtrl.submodeTimer > 15.0f)
                modeCtrl.submodeTimer = 15;
            if (modeCtrl.submodeTimer == 15.0f)
            {
                fade_screen_to_color(0x101, 0xFFFFFF, modeCtrl.submodeTimer + 1);
                play_track_and_fade_out_other_tracks(0, 60, 100);
            }
        }
        break;
    default:
        if (fn_1_D36C8() == 0 && (merged_digital_inputs[2] & 0x100) && modeCtrl.unk18 < 0 && next > 0)
            modeCtrl.submodeTimer = 0;
        if (next < 0 && modeCtrl.submodeTimer == 60)
        {
            fade_screen_to_color(0x101, 0, 61);
            g_fade_track_volume(60, 2);
        }
        break;
    }
    if (--modeCtrl.submodeTimer > 0)
        return;
    mode_info.flags &= ~0x10;
    if (modeCtrl.gameType == 1)
    {
        BALL_FOREACH(
            if (ball->bonus > 0)
            {
                ball->bananas += ball->bonus;
                ball->bonus = 0;
                ball->bananas = (ball->bananas < 999) ? ball->bananas : 999;
            }
        )
    }
    if (modeCtrl.gameType == 14)
    {
        fn_2_6568();
        fn_2_6538();
    }
    else if (next < 0)
    {
        if (modeCtrl.gameType == 1)
        {
            sub_mode_request = 0x4C;
        }
        else
        {
            int extra = 0;

            if ((modeCtrl.courseFlags & 1) && mode_info.continuesUsed == 0)
                extra = 1;
            if (extra != 0 && (modeCtrl.courseFlags & 8) && modeCtrl.difficulty != 2)
                extra = 0;
            if (extra != 0 && (modeCtrl.courseFlags & 0x10) && modeCtrl.difficulty != 2)
                extra = 0;
            if (extra != 0 && (modeCtrl.courseFlags & 0x800000))
                extra = 0;
            if (extra != 0)
            {
                sub_mode_request = 0x4A;
            }
            else
            {
                g_smth_with_ending_course();
                g_smth_with_ending_course_2();
                g_change_music_volume(12, 60, 0);
                main_mode_request = 6;
                sub_mode_request = 0xFC;
                modeCtrl.courseFlags |= 0x100040;
            }
        }
    }
    else
    {
        if (modeCtrl.gameType == 2)
        {
            mode_info.attempts++;
            g_fade_track_volume(100, 8);
        }
        else
        {
            mode_info.attempts = 1;
        }
        g_another_stage_id = (s16)next;
        sub_mode_request = 0x30;
    }
}

void smd_game_continue_init(void)
{
    if (g_some_other_flags & 0xA)
        return;
    g_smth_with_ending_course();
    fn_1_118504();
    modeCtrl.submodeTimer = 659;
    modeCtrl.unk10 = 1;
    modeCtrl.courseFlags &= ~4;
    event_dest(0);
    event_dest(1);
    event_dest(4);
    event_dest(5);
    event_dest(7);
    event_dest(9);
    event_dest(0x11);
    event_dest(0xB);
    event_init(0x11);
    load_stage(current_stage_id);
    if (modeCtrl.gameType == 0)
    {
        if (fn_1_16D3E0() == 0)
        {
            modeCtrl.unk10 = 0;
            modeCtrl.submodeTimer = 480;
            modeCtrl.courseFlags |= 4;
        }
        else
        {
            create_continue_sprites();
        }
    }
    else
    {
        create_continue_sprites();
    }
    u_smth_with_lights_smd_continue(current_stage_id);
    BALL_FOREACH(
        ball->state = 0x12;
        ball->ape->flags |= 0x100;
        g_some_set_ape_anim(ball->ape, 6, 0);
    )
    camera_set_state(0x19);
    if (modeCtrl.gameType == 0 && modeCtrl.playerCount == 1)
    {
        compare_play_points_with_99999_after_exit_game();
        g_display_playpoint_or_gift_message(1, 320.0f, 408.0f);
    }
    play_track_and_fade_out_other_tracks(12, 30, 100);
    g_preload_stage_or_results();
    sub_mode_request = 0x42;
}

void smd_game_continue_tick(void)
{
    int isStory = (modeCtrl.gameType == 0 && modeCtrl.playerCount == 1);
    int sel;

    if (g_some_other_flags & 0xA)
        return;
    if (!(modeCtrl.courseFlags & 4))
    {
        if (isStory == 0 || fn_1_16C8D8() != 0)
        {
            sel = modeCtrl.unk10;
            if ((merged_digital_inputs[2] & 1) || (merged_analog_inputs[2] & 1))
                sel = 1;
            if ((merged_digital_inputs[2] & 2) || (merged_analog_inputs[2] & 2))
                sel = 0;
            if (sel != modeCtrl.unk10)
            {
                call_SoundReqID_arg_0(9);
                modeCtrl.unk10 = sel;
            }
        }
        if ((isStory == 0 || fn_1_16C8D8() != 0) && (merged_digital_inputs[2] & 0x100))
        {
            current_ball->ape->flags &= ~0x100;
            if (modeCtrl.unk10 == 1)
            {
                mode_info.continuesUsed++;
                mode_info.attempts = 1;
                current_ball->ape->flags |= 0x800;
                g_some_set_ape_anim(current_ball->ape, 6, 1);
                g_display_playpoint_or_gift_message_child(0, 20, NULL);
                call_SoundReqID_arg_0(0xA);
                call_SoundReqID_arg_0(0x50);
                modeCtrl.submodeTimer = 90;
                modeCtrl.courseFlags |= 4;
                fn_1_16C92C();
            }
            else
            {
                g_display_playpoint_or_gift_message_child(0, 20, NULL);
                call_SoundReqID_arg_0(0x30);
                current_ball->ape->flags |= 0x40000;
                g_some_set_ape_anim(current_ball->ape, 6, 2);
                modeCtrl.submodeTimer = 60;
                modeCtrl.courseFlags |= 4;
            }
        }
    }
    else
    {
        if (isStory != 0 && modeCtrl.unk10 == 0 && modeCtrl.submodeTimer > 180
         && fn_1_16C8D8() != 0 && (merged_digital_inputs[2] & 0x100))
            modeCtrl.submodeTimer = 180;
        switch (modeCtrl.unk10)
        {
        case 1:
            if (modeCtrl.submodeTimer == 30)
                fade_screen_to_color(0x101, 0xFFFFFF, modeCtrl.submodeTimer - 1);
            break;
        case 0:
            if (modeCtrl.submodeTimer == 60)
            {
                if (modeCtrl.gameType == 0
                 && (fn_1_FC208(modeCtrl.difficulty, &lbl_1_bss_D30FC[modeCtrl.currPlayer]) >= 0
                  || (dip_switches & 0x41) == 0x41))
                    fade_screen_to_color(0x101, 0xFFFFFF, modeCtrl.submodeTimer - 1);
                else
                    fade_screen_to_color(0x101, 0, modeCtrl.submodeTimer - 1);
                fn_1_16C92C();
            }
            break;
        }
        if (--modeCtrl.submodeTimer > 0)
            return;
        fn_2_7198();
        if (modeCtrl.unk10 == 1)
        {
            fade_screen_to_color(0x100, 0xFFFFFF, 30);
            sub_mode_request = 0x43;
        }
        else if ((!(modeCtrl.courseFlags & 0x2000000) && (modeCtrl.courseFlags & 8))
              || ((modeCtrl.courseFlags & 0x2000000) && (modeCtrl.courseFlags & 0x800000)))
        {
            g_change_music_volume(12, 60, 0);
            main_mode_request = 6;
            sub_mode_request = 0xFC;
        }
        else
        {
            sub_mode_request = 0x44;
        }
    }
}

void smd_game_timeover_init(void)
{
    Vec shake;
    int i;

    if (g_some_other_flags & 0xA)
        return;
    modeCtrl.submodeTimer = 120;
    event_freeze(0);
    event_dest(1);
    event_dest(0xB);
    fn_1_365F0(0x29, 5);
    BALL_FOREACH(ball->state = 0;)
    set_minimap_mode(1);
    if (!(mode_info.flags & 0x40))
        call_SoundReqID_arg_0(0xB);
    camera_set_state(0x15);
    for (i = 0; i < 5; i++)
    {
        shake.x = 0.0f;
        shake.y = 0.75f;
        shake.z = rand() / 32767.0f - 0.5f;
        camera_start_shake(i, modeCtrl.submodeTimer, &shake);
    }
    g_something_with_recplay3(1);
    create_time_over_sprite(modeCtrl.submodeTimer);
    BALL_FOREACH(call_SoundReqID_arg_0(0x1C);)
    sub_mode_request = 0x39;
}

void smd_game_timeover_tick(void)
{
    if (g_some_other_flags & 0xA)
        return;
    if (g_check_current_track(0x29) != 0)
    {
        g_something_with_stopping_music_or_sfx(0x29);
        g_SoftStreamStart_with_some_defaults_2(0x2C);
    }
    if (modeCtrl.gameType == 1)
    {
        g_another_stage_id = (s16)g_get_next_stage_id();
        if (g_another_stage_id < 0 && modeCtrl.submodeTimer == 60)
        {
            fade_screen_to_color(0x101, 0, 61);
            g_fade_track_volume(60, 2);
        }
    }
    if (--modeCtrl.submodeTimer > 0)
        return;
    if (g_decrement_active_ball_monkey_count() != 0)
    {
        if ((mode_info.flags & 0x40) || modeCtrl.gameType == 1)
        {
            g_another_stage_id = (s16)g_get_next_stage_id();
            mode_info.attempts = (s16)((modeCtrl.gameType != 2) ? 1 : mode_info.attempts + 1);
        }
        else
        {
            mode_info.attempts++;
        }
        switch (modeCtrl.gameType)
        {
        case 1:
            if (g_another_stage_id < 0)
                sub_mode_request = 0x4C;
            else
                sub_mode_request = 0x30;
            break;
        case 14:
            sub_mode_request = 0x5A;
            break;
        default:
            sub_mode_request = 0x30;
            break;
        }
    }
    else
    {
        modeCtrl.unk14 = 1;
        switch (modeCtrl.gameType)
        {
        case 1:
            sub_mode_request = 0x3D;
            break;
        case 14:
            sub_mode_request = 0x5A;
            break;
        default:
            sub_mode_request = 0x41;
            break;
        }
    }
}

void smd_game_ringout_init(void)
{
    if (g_some_other_flags & 0xA)
        return;
    modeCtrl.submodeTimer = 270;
    modeCtrl.unk18 = 60;
    set_minimap_mode(1);
    call_SoundReqID_arg_0((current_ball->unk2 == 1) ? 0x51 : 0x1D);
    call_SoundReqID_arg_0(0x15);
    lbl_2_bss_8 = -1;
    create_fallout_or_bonus_finish_sprite(modeCtrl.submodeTimer);
    if (g_maybe_related_to_music_crossfading(0x29) != -1)
        g_handle_world_bgm(100);
    sub_mode_request = 0x3B;
}

void smd_game_ringout_tick(void)
{
    if (g_some_other_flags & 0xA)
        return;
    if (current_ball->state == 0x20)
        mode_info.flags &= ~0x10;
    if (mode_info.flags & 0x40)
    {
        if (modeCtrl.submodeTimer == 260.0)
        {
            g_something_with_recplay3(1);
            modeCtrl.submodeTimer = 110;
        }
    }
    else
    {
        if (modeCtrl.submodeTimer == 240.0)
        {
            g_something_with_recplay3(1);
            BALL_FOREACH(ball->state = 7;)
            {
                struct World *world = world_infos;
                s8 *status = g_poolInfo.playerPool.statusList;
                int i;

                for (i = 0; i < g_poolInfo.playerPool.count; i++, world++, status++)
                {
                    if (*status == 2)
                        world->state = 8;
                }
            }
            camera_set_state(5);
            lbl_1_bss_71EC8.unk10 = MIN(fn_1_61B18(lbl_1_bss_71EC8.unk0[lbl_1_bss_71EC8.unk14]), 120.0f);
            g_advance_stage_animation();
            mode_info.flags |= 0x10;
            item_replay_init_all(fn_1_629C4(lbl_1_bss_71EC8.unk0[lbl_1_bss_71EC8.unk14], lbl_1_bss_71EC8.unk10));
            create_replay_sprite(modeCtrl.submodeTimer);
            lbl_2_bss_8 = 30;
        }
        if (modeCtrl.submodeTimer == 150.0)
        {
            BALL_FOREACH(ball->state = 7;)
            camera_set_state(7);
            lbl_1_bss_71EC8.unk10 = MIN(fn_1_61B18(lbl_1_bss_71EC8.unk0[lbl_1_bss_71EC8.unk14]),
                                        (float)(modeCtrl.submodeTimer - 60));
            g_advance_stage_animation();
            mode_info.flags |= 0x10;
            item_replay_init_all(fn_1_629C4(lbl_1_bss_71EC8.unk0[lbl_1_bss_71EC8.unk14], lbl_1_bss_71EC8.unk10));
            lbl_2_bss_8 = 60;
        }
        switch (modeCtrl.gameType)
        {
        case 14:
            if (modeCtrl.submodeTimer == 30.0)
                modeCtrl.submodeTimer = 0;
            break;
        }
    }
    modeCtrl.unk18--;
    if ((merged_digital_inputs[2] & 0x100) && modeCtrl.unk18 < 0 && !(mode_info.flags & 0x240)
     && current_ball->unk2 > 1)
    {
        g_something_with_recplay3(1);
        modeCtrl.submodeTimer = 0;
    }
    if (lbl_2_bss_8 > 0)
        lbl_2_bss_8--;
    if (lbl_2_bss_8 == 0 && current_ball->vel.y < -0.2)
    {
        call_SoundReqID_arg_0((current_ball->unk2 == 1) ? 0x51 : 0x1D);
        call_SoundReqID_arg_0(0x15);
        lbl_2_bss_8 = -1;
    }
    if (--modeCtrl.submodeTimer > 0)
        return;
    mode_info.flags &= ~0x10;
    event_freeze(0);
    if (g_decrement_active_ball_monkey_count() != 0)
    {
        if (mode_info.flags & 0x40)
        {
            g_another_stage_id = (s16)g_get_next_stage_id();
            mode_info.attempts = (s16)((modeCtrl.gameType != 2) ? 1 : mode_info.attempts + 1);
        }
        else
        {
            mode_info.attempts++;
        }
        switch (modeCtrl.gameType)
        {
        case 14:
            sub_mode_request = 0x5A;
            break;
        default:
            sub_mode_request = 0x30;
            break;
        }
    }
    else
    {
        modeCtrl.unk14 = 0;
        switch (modeCtrl.gameType)
        {
        case 1:
            sub_mode_request = 0x3D;
            break;
        default:
            sub_mode_request = 0x41;
            break;
        }
    }
}

void smd_game_bonus_clear_init(void)
{
    if (g_some_other_flags & 0xA)
        return;
    modeCtrl.submodeTimer = 180;
    modeCtrl.courseFlags &= ~0x400;
    event_dest(1);
    event_dest(0xB);
    set_minimap_mode(1);
    BALL_FOREACH(ball->state = 5;)
    BALL_FOREACH(ball->flags |= 0x500;)
    camera_set_state(0xE);
    if (mode_info.flags & 0x400)
    {
        create_time_over_sprite(modeCtrl.submodeTimer);
    }
    else
    {
        create_bonus_finish_or_perfect_sprite(modeCtrl.submodeTimer);
        g_something_with_recplay3(1);
    }
    if (g_maybe_related_to_music_crossfading(0x29) != -1)
        g_handle_world_bgm(100);
    sub_mode_request = 0x47;
}

void smd_game_bonus_clear_tick(void)
{
    if (g_some_other_flags & 0xA)
        return;
    if (modeCtrl.submodeTimer == 150.0)
        g_something_with_recplay3(1);
    BALL_FOREACH(
        if (!(ball->flags & 0x200) && (ball->ape->flags & 0x4000))
        {
            ball->flags &= ~0x500;
            ball->flags |= 0x200;
            modeCtrl.courseFlags |= 0x400;
            call_SoundReqID_arg_0(0x126);
        }
    )
    if ((modeCtrl.courseFlags & 0x400) && modeCtrl.submodeTimer > 60 && modeCtrl.submodeTimer < 240)
        modeCtrl.submodeTimer = 60;
    g_another_stage_id = (s16)g_get_next_stage_id();
    if (g_another_stage_id < 0 && modeCtrl.submodeTimer == 60)
    {
        fade_screen_to_color(0x101, 0, 61);
        g_fade_track_volume(60, 2);
    }
    if (--modeCtrl.submodeTimer > 0)
        return;
    BALL_FOREACH(ball->flags &= ~0x200;)
    if (g_another_stage_id < 0 && modeCtrl.gameType == 1)
    {
        sub_mode_request = 0x4C;
    }
    else
    {
        mode_info.attempts = (s16)((modeCtrl.gameType != 2) ? 1 : mode_info.attempts + 1);
        sub_mode_request = 0x30;
    }
}

void smd_game_first_tick(void)
{
    if (g_some_other_flags & 0xA)
        return;
    event_dest(0x13);
    if (events[0x11].state == 0)
        event_init(0x11);
    event_init(0x13);
    modeCtrl.submodeTimer = 240;
    if (!(modeCtrl.courseFlags & 0x60))
        BALL_FOREACH(call_SoundReqID_arg_0(0x22C);)
    g_create_some_game_over_and_player_num_sprites(300);
    fade_screen_to_color(0x101, 0xFFFFFF, 1);
    play_track_and_fade_out_other_tracks(13, 30, 100);
    sub_mode_request = 0x3E;
}

void smd_game_over_init(void)
{
    struct MemcardRequest req;
    char buf[32];

    if (g_some_other_flags & 0xA)
        return;
    if (modeCtrl.submodeTimer == 90)
        call_SoundReqID_arg_0(0x240);
    if (modeCtrl.submodeTimer == 60)
        fade_screen_to_color(0x101, 0, lbl_80198B38);
    if (--modeCtrl.submodeTimer > 0)
        return;
    if (modeCtrl.playerCount == 1 && (modeCtrl.courseFlags & 0x100000))
    {
        sub_mode_request = 0x55;
    }
    else if (storymode_save_files[0x31B] != 0)
    {
        g_save_game_data();
        if (fn_1_F0204() != 0)
        {
            memset(&req, 0, sizeof(req));
            req.unk0 = 0;
            g_sprintf_memcard_error(0, buf);
            req.fileName = buf;
            req.files = storymode_save_files;
            if (lbl_1_bss_CF3D8 != 0)
                req.flags |= 0x20000000;
            g_some_replay_func4(0, (u8)g_last_used_memcard_slot_2, &req);
            g_another_SoftStreamStart_wrapper_of_some_sort(0x57, 1);
            g_change_music_volume(0x57, 180, 100);
            sub_mode_request = 0x51;
        }
        else
        {
            sub_mode_request = 0x52;
        }
    }
    else
    {
        sub_mode_request = 0x52;
    }
}

void smd_game_over_point_init(void)
{
    if (g_some_other_flags & 0xA)
        return;
    dest_all_events();
    event_init(0x11);
    event_init(0x13);
    compare_play_points_with_99999_after_exit_game();
    g_display_playpoint_or_gift_message(3, 320.0f, 360.0f);
    fade_screen_to_color(2, 0, 1);
    play_track_and_fade_out_other_tracks(0x57, 30, 100);
    sub_mode_request = 0x56;
}

void smd_game_over_point_tick(void)
{
    struct MemcardRequest req;
    char buf[32];

    if (g_some_other_flags & 0xA)
        return;
    if (fn_1_16C910() != 0)
        return;
    if (!(modeCtrl.courseFlags & 0x8000000))
    {
        if (storymode_save_files[0x31B] != 0)
        {
            g_save_game_data();
            if (fn_1_F0204() != 0)
            {
                memset(&req, 0, sizeof(req));
                req.unk0 = 0;
                g_sprintf_memcard_error(0, buf);
                req.fileName = buf;
                req.files = storymode_save_files;
                if (lbl_1_bss_CF3D8 != 0)
                    req.flags |= 0x20000000;
                g_some_replay_func4(0, (u8)g_last_used_memcard_slot_2, &req);
                sub_mode_request = 0x51;
            }
            else
            {
                sub_mode_request = 0x52;
            }
        }
        else
        {
            sub_mode_request = 0x52;
        }
    }
    else
    {
        sub_mode_request = 0x61;
        modeCtrl.courseFlags &= ~0x8000000;
    }
}

void smd_game_scenario_init(void)
{
    if (g_some_other_flags & 0xA)
        return;
    smd_game_scenario_init_child();
    set_sub_mode_destination(g_some_dest_function_handler);
    sub_mode_request = 0x58;
}

void smd_game_scenario_tick(void)
{
    if (g_some_other_flags & 0xA)
        return;
    smd_game_scenario_tick_child();
}

void g_some_dest_function_handler(void)
{
    switch (sub_mode)
    {
    case 0x58:
        set_sub_mode_destination(g_some_dest_function_handler);
        break;
    default:
        g_some_storymode_dest_function();
        break;
    }
}

void smd_game_scenario_return(void)
{
    if (g_some_other_flags & 0xA)
        return;
    fn_2_881C();
    set_sub_mode_destination(g_some_dest_function_handler);
    sub_mode_request = 0x58;
}

void smd_game_retry_init(void)
{
    if (g_some_other_flags & 0xA)
        return;
    modeCtrl.submodeTimer = -1;
    fn_1_16C5B4();
    dest_all_sprites();
    g_make_some_continue_screen_sprites();
    modeCtrl.unk10 = 0;
    camera_set_state(0x59);
    call_SoundReqID_arg_1(0x70);
    g_handle_world_bgm(100);
    event_freeze(0);
    event_freeze(2);
    event_freeze(3);
    event_freeze(4);
    event_freeze(5);
    event_freeze(6);
    event_freeze(7);
    event_freeze(9);
    event_freeze(0xE);
    event_freeze(0xF);
    sub_mode_request = 0x5B;
}

void smd_game_retry_tick(void)
{
    if (g_some_other_flags & 2)
        return;
    if (modeCtrl.submodeTimer > 0)
    {
        modeCtrl.submodeTimer--;
        if ((float)modeCtrl.submodeTimer > 15.0f && (merged_digital_inputs[2] & 0x100))
            modeCtrl.submodeTimer = 15;
        if (modeCtrl.submodeTimer == 15.0f)
        {
            fade_screen_to_color(0x101, 0xFFFFFF, modeCtrl.submodeTimer - 1);
            switch (modeCtrl.unk10)
            {
            case 1:
                play_track_and_fade_out_other_tracks(0, 60, 100);
                break;
            }
        }
        if (modeCtrl.submodeTimer == 0)
        {
            switch (modeCtrl.unk10)
            {
            case 0:
                sub_mode_request = 0x30;
                fade_screen_to_color(0x100, 0xFFFFFF, 15);
                break;
            case 1:
                fn_2_6538();
                break;
            }
        }
    }
    else
    {
        int prev = modeCtrl.unk10;
        int down = (merged_digital_inputs[2] & 2) || (merged_analog_inputs[2] & 2);
        int up = (merged_digital_inputs[2] & 1) || (merged_analog_inputs[2] & 1);

        if (up != 0)
        {
            if (modeCtrl.unk10 > 0)
                modeCtrl.unk10--;
            else
                modeCtrl.unk10 = 1;
        }
        if (down != 0)
        {
            if (modeCtrl.unk10 < 1)
                modeCtrl.unk10++;
            else
                modeCtrl.unk10 = 0;
        }
        if (prev != modeCtrl.unk10)
            call_SoundReqID_arg_1(9);
        if (merged_digital_inputs[2] & 0x100)
        {
            event_restart(0);
            event_restart(2);
            event_restart(3);
            event_restart(4);
            event_restart(5);
            event_restart(6);
            event_restart(7);
            event_restart(9);
            event_restart(0xE);
            event_restart(0xF);
            switch (modeCtrl.unk10)
            {
            case 0:
                modeCtrl.submodeTimer = 120;
                call_SoundReqID_arg_1(0xA);
                call_SoundReqID_arg_1(0x50);
                balls[modeCtrl.currPlayer].state = 5;
                balls[modeCtrl.currPlayer].unk15C = balls[modeCtrl.currPlayer].pos;
                balls[modeCtrl.currPlayer].flags |= 0x540;
                world_infos[modeCtrl.currPlayer].state = 6;
                break;
            case 1:
                modeCtrl.submodeTimer = 60;
                call_SoundReqID_arg_1(0x30);
                call_SoundReqID_arg_0(0x3E);
                break;
            }
        }
    }
}

s8 lbl_2_data_280[] = { 15, 16, 17, 18 };
s8 lbl_2_data_284[] = { 19, 20, 21, 22 };
s8 lbl_2_data_288[] = { 23, 24, 25, 26 };
s8 lbl_2_data_28C[] = { 27, 28, 29, 30 };

void smd_game_over_save(void)
{
    if (g_check_some_memcard_field() == 0)
    {
        if (g_get_some_memcard_var() == 0)
            OSReport("result_code: %d ", g_get_result_code());
        else
            smd_game_over_save_child();
        sub_mode_request = 0x52;
    }
}

void smd_game_over_dest(void)
{
    if (g_some_other_flags & 0xA)
        return;
    switch (modeCtrl.gameType)
    {
    case 1:
        g_poolInfo.playerPool.statusList[0] = 0;
        g_poolInfo.playerPool.statusList[1] = 0;
        g_poolInfo.playerPool.statusList[2] = 0;
        g_poolInfo.playerPool.statusList[3] = 0;
        modeCtrl.currPlayer = 0;
        current_ball = &balls[modeCtrl.currPlayer];
        g_something_calls_sndPopGroup();
        break;
    default:
        fn_2_64DC(modeCtrl.currPlayer);
        break;
    }
    if (fn_2_64F4() != 0)
    {
        camera_setup_singleplayer_viewport();
        modeCtrl.unk10 = 0;
        main_mode_request = 0;
        sub_mode_request = 0x15;
    }
    else
    {
        fade_screen_to_color(0x100, 0, lbl_80198B38);
        sub_mode_request = 0x30;
    }
}

char *lbl_2_data_308[] = {
    "h/NAMAEWO OSIETE!",
    "h/ONAMAE OSIETE!",
    "h/NAMAEWO OSIETEHOSHIIDEk/CHU!",
    "k/NAMAE OSIERO! UHO UHOHO!",
};

char *lbl_2_data_32C[] = {
    "ENTRY YOUR NAME!",
    "ENTRY YOUR NAME!",
    "ENTRY YOUR NAME!",
    "ENTRY YOUR NAME!",
};

void smd_game_nameentry_ready_init(void)
{
    if (g_some_other_flags & 0xA)
        return;
    printf("through [nameentry_ready_init]\n");
    modeCtrl.courseFlags &= ~0x20;
    if (!(modeCtrl.courseFlags & 1) && (dip_switches & 0x41) != 0x41)
        sub_mode_request = 0x3D;
    else
        sub_mode_request = 0x45;
}

void smd_game_nameentry_ready_tick(void)
{
    if (g_some_other_flags & 0xA)
        return;
    printf("through [nameentry_ready_main]\n");
    sub_mode_request = 0x3F;
}

void smd_game_nameentry_init(void)
{
    if (g_some_other_flags & 0xA)
        return;
    mode_info.flags &= ~8;
    printf("through [nameentry_init]\n");
    g_nameentry2_init();
    modeCtrl.submodeTimer = lbl_80198B38;
    sub_mode_request = 0x40;
}

void smd_game_nameentry_tick(void)
{
    if (g_some_other_flags & 0xA)
        return;
    fn_1_11C02C();
    if (modeCtrl.submodeTimer == 0)
    {
        sub_mode_request = (modeCtrl.courseFlags & 0x400000) ? 0x3D : 0x3D;
        mode_info.flags |= 8;
    }
}

void smd_game_roll_init(void)
{
    if (g_some_other_flags & 0xA)
        return;
    load_bmp_by_id(5);
    staff_roll_init();
    if (modeCtrl.courseFlags & 0x400000)
        g_preload_ape_model_for_stageselect();
    modeCtrl.submodeTimer = 600;
    sub_mode_request = 0x54;
}

void smd_game_roll_tick(void)
{
    if (g_some_other_flags & 0xA)
        return;
    g_smth_with_staff_roll_init_7();
    if (modeCtrl.submodeTimer > 0)
        return;
    if (modeCtrl.courseFlags & 0x40000)
    {
        g_return_to_sel_ngc(-1);
        modeCtrl.courseFlags &= ~0x40000;
    }
    else if (modeCtrl.courseFlags & 0x400000)
    {
        sub_mode_request = 0x5E;
    }
    else
    {
        sub_mode_request = 0x44;
    }
    g_some_gift_menu_flags |= 8;
    lbl_1_bss_D3350 |= 2;
}

void smd_game_extra_init(void)
{
    if (g_some_other_flags & 0xA)
        return;
    modeCtrl.submodeTimer = 600;
    if (modeCtrl.courseFlags & 0x10)
        modeCtrl.courseFlags |= 0x800000;
    if (modeCtrl.courseFlags & 8)
    {
        modeCtrl.courseFlags |= 0x10;
        if (is_master_unlocked() == 0)
            unlock_master();
    }
    modeCtrl.courseFlags |= 8;
    event_dest(0x10);
    event_dest(0x11);
    event_init(0x10);
    event_init(0x11);
    BALL_FOREACH(ball->state = 0xD;)
    enable_camera(modeCtrl.currPlayer);
    camera_set_state(0x33);
    create_go_to_extra_or_master_stages_sprites(0);
    fade_screen_to_color(0x100, 0, 120);
    g_handle_world_bgm(80);
    g_fade_track_volume(120, 3);
    sub_mode_request = 0x4B;
}

char *lbl_2_data_3EC[] = {
    "You finished without using a continue!",
    "",
    "",
    "",
    "",
    "\x83m\x81[\x83R\x83\x93\x83" "e\x83" "B\x83j\x83\x85\x81[\x83N\x83\x8A\x83" "A\x81" "A\x82\xA8\x82\xDF\x82\xC5\x82\xC6\x82\xA4\x81I",
};

char *lbl_2_data_434[] = {
    "You finished without using a continue!",
    "",
    "",
    "",
    "",
    "\x83m\x81[\x83R\x83\x93\x83" "e\x83" "B\x83j\x83\x85\x81[\x83N\x83\x8A\x83" "A\x82\xC8\x82\xF1\x82\xC4\x81" "A\x83X\x83" "b\x83S\x81`\x83" "C\x81I",
};

char *lbl_2_data_4B0[] = {
    "You compweted without a continue!",
    "",
    "",
    "",
    "",
    "\x83m\x81[\x83R\x83\x93\x83" "e\x83j\x83\x85\x81[\x83N\x83\x8A\x83" "A\x82\xBF\x82\xBD\x82\xCC\x82\xC5\x81" "A\x83G\x83N\x83X\x83g\x83\x89\x82\xC9 \x82\xB7\x82\xB7\x82\xDF\x82\xE9\x82\xC5\x83`\x83\x85\x81I",
};

char *lbl_2_data_518[] = {
    "I finished without using a continue!",
    "",
    "",
    "",
    "",
    "\x83m\x81[\x83R\x83\x93\x83" "e\x83" "B\x83j\x83\x85\x81[\x83N\x83\x8A\x83" "A\x82\xB5\x82\xBF\x82\xDC\x82\xC1\x82\xBD\x82\xBA\x81I",
};

char **lbl_2_data_530[] = {
    lbl_2_data_3EC,
    lbl_2_data_434,
    lbl_2_data_4B0,
    lbl_2_data_518,
};

char *lbl_2_data_594[] = {
    "Now you can try the Extra level!",
    "",
    "",
    "",
    "",
    "\x82\xB3\x82\xE7\x82\xC9 \x83G\x83N\x83X\x83g\x83\x89\x82\xC9 \x82\xC2\x82\xC3\x82\xAD\x82\xA9\x82\xE7 \x82\xAA\x82\xF1\x82\xCE\x82\xC1\x82\xC4\x82\xCB\x81I",
};

char *lbl_2_data_608[] = {
    "Now you can challenge the Extra level!",
    "",
    "",
    "",
    "",
    "\x82\xBB\x82\xF1\x82\xC8 \x82\xA0\x82\xC8\x82\xBD\x82\xCD\x81" "A\x83G\x83N\x83X\x83g\x83\x89\x82\xC9 \x83`\x83\x83\x83\x8C\x83\x93\x83W\x82\xB5\x82\xC4\x82\xCB\x81I",
};

char *lbl_2_data_650[] = {
    "Now go to da Extra level!",
    "",
    "",
    "",
    "",
    "\x82\xAA\x82\xF1\x82\xCE\x82\xE9\x82\xC5\x83`\x83\x85\x81I",
};

char *lbl_2_data_6BC[] = {
    "Now it's time to complete the Extra level!",
    "",
    "",
    "",
    "",
    "\x82\xC2\x82\xC3\x82\xAF\x82\xC4\x81" "A\x83G\x83N\x83X\x83g\x83\x89\x82\xC9 \x83`\x83\x83\x83\x8C\x83\x93\x83W\x82\xBE\x82\xBA\x81I",
};

char **lbl_2_data_6D4[] = {
    lbl_2_data_594,
    lbl_2_data_608,
    lbl_2_data_650,
    lbl_2_data_6BC,
};

char *lbl_2_data_73C[] = {
    "Congratulations! You completed the Extra level!",
    "",
    "",
    "",
    "",
    "\x83G\x83N\x83X\x83g\x83\x89 \x83\x8C\x83x\x83\x8B \x83N\x83\x8A\x83" "A\x81" "A\x82\xA8\x82\xDF\x82\xC5\x82\xC6\x82\xA4\x81I",
};

char *lbl_2_data_7A0[] = {
    "I want you to challenge ... ",
    "",
    "",
    "",
    "",
    "\x82\xB1\x82\xCC\x83G\x83N\x83X\x83g\x83\x89\x82\xF0 \x83N\x83\x8A\x83" "A\x82\xB5\x82\xBD \x82\xA0\x82\xC8\x82\xBD\x82\xC9\x82\xCD\x81" "c",
};

char *lbl_2_data_81C[] = {
    "I am so happy that I completed the Extra level!",
    "",
    "",
    "",
    "",
    "\x83G\x83N\x83X\x83g\x83\x89 \x83\x8C\x83x\x83\x8B\x82\xF0 \x83N\x83\x8A\x83" "A\x82\xC5\x82\xAB\x82\xC4\x81" "A\x82\xA4\x82\xEA\x82\xBF\x82\xA2\x82\xC5\x83`\x83\x85\x81I",
};

char *lbl_2_data_874[] = {
    "I completed the Extra level!",
    "",
    "",
    "",
    "",
    "\x83G\x83N\x83X\x83g\x83\x89\x82\xF0 \x83N\x83\x8A\x83" "A\x82\xB5\x82\xBD\x82\xBA\x81`\x82\xC1\x81I",
};

char **lbl_2_data_88C[] = {
    lbl_2_data_73C,
    lbl_2_data_7A0,
    lbl_2_data_81C,
    lbl_2_data_874,
};

char *lbl_2_data_8E0[] = {
    "Now try the Master level!",
    "",
    "",
    "",
    "",
    "\x82\xB3\x82\xE7\x82\xC9 \x82\xBD\x82\xC2\x82\xB6\x82\xF1\x83\x8C\x83x\x83\x8B\x82\xE0 \x83v\x83\x8C\x83" "C\x82\xB5\x82\xC4\x82\xCB\x81I",
};

char *lbl_2_data_940[] = {
    "the Master level.",
    "",
    "",
    "",
    "",
    "\x82\xBD\x82\xC2\x82\xB6\x82\xF1\x83\x8C\x83x\x83\x8B\x82\xC9\x82\xE0 \x83`\x83\x83\x83\x8C\x83\x93\x83W\x82\xB5\x82\xC4\x82\xD9\x82\xB5\x82\xA2\x82\xC8/cd8426/",
};

char *lbl_2_data_9B0[] = {
    "I'm gonna challenge the Master level!",
    "",
    "",
    "",
    "",
    "\x82\xB3\x82\xE7\x82\xC9 \x82\xBD\x82\xC2\x82\xB6\x82\xF1\x83\x8C\x83x\x83\x8B\x82\xC9 \x83`\x83\x83\x83\x8C\x83\x93\x83W\x82\xB7\x82\xE9\x82\xC5\x83`\x83\x85\x81I",
};

char *lbl_2_data_A30[] = {
    "I'm sure I can complete the Master level!",
    "",
    "",
    "",
    "",
    "\x82\xB1\x82\xCC \x82\xA2\x82\xAB\x82\xA8\x82\xA2\x82\xC5\x81" "A\x82\xBD\x82\xC2\x82\xB6\x82\xF1\x83\x8C\x83x\x83\x8B\x82\xE0 \x83N\x83\x8A\x83" "A\x82\xB5\x82\xC4\x82\xE2\x82\xE9\x82\xBC\x81`\x82\xC1\x81I`",
};

char **lbl_2_data_A48[] = {
    lbl_2_data_8E0,
    lbl_2_data_940,
    lbl_2_data_9B0,
    lbl_2_data_A30,
};

void smd_game_extra_tick(void)
{
    struct Effect effect;
    struct TextBox tb1;
    struct TextBox tb2;

    if (g_some_other_flags & 0xA)
        return;
    if (g_font_char_var_3 % 3 == 0)
    {
        memset(&effect, 0, sizeof(effect));
        effect.type = 4;
        effect.playerId = current_ball->playerId;
        effect.pos = current_ball->pos;
        spawn_effect(&effect);
    }
    if (modeCtrl.submodeTimer > 60 && modeCtrl.submodeTimer < 480 && (merged_digital_inputs[2] & 0x100))
        modeCtrl.submodeTimer = 60;
    if (modeCtrl.submodeTimer == 540)
    {
        memset(&tb1, 0, sizeof(tb1));
        tb1.unk2C = 4;
        tb1.unk16 = 11;
        tb1.x = 320;
        tb1.y = 200;
        tb1.lines = 1;
        tb1.callback = NULL;
        g_display_playpoint_or_gift_message_child(1, 1, &tb1);
        if (((modeCtrl.courseFlags & 8) && !(modeCtrl.courseFlags & 0x10)) || (modeCtrl.courseFlags & 0x800000))
            g_smth_with_playpoint_or_gift_msg(1, lbl_2_data_530[active_monkey_id[current_ball->playerId]][locale]);
        else
            g_smth_with_playpoint_or_gift_msg(1, lbl_2_data_88C[active_monkey_id[current_ball->playerId]][locale]);
    }
    if (modeCtrl.submodeTimer == 300)
    {
        memset(&tb2, 0, sizeof(tb2));
        tb2.unk2C = 4;
        tb2.unk16 = 11;
        tb2.x = 320;
        tb2.y = 200;
        tb2.lines = 1;
        tb2.callback = NULL;
        g_display_playpoint_or_gift_message_child(1, 21, &tb2);
        if (((modeCtrl.courseFlags & 8) && !(modeCtrl.courseFlags & 0x10)) || (modeCtrl.courseFlags & 0x800000))
            g_smth_with_playpoint_or_gift_msg(1, lbl_2_data_6D4[active_monkey_id[current_ball->playerId]][locale]);
        else
            g_smth_with_playpoint_or_gift_msg(1, lbl_2_data_A48[active_monkey_id[current_ball->playerId]][locale]);
    }
    if (modeCtrl.submodeTimer == 60)
    {
        g_display_playpoint_or_gift_message_child(1, 20, NULL);
        fade_screen_to_color(0x101, 0, 60);
        g_fade_track_volume(60, 2);
    }
    if (--modeCtrl.submodeTimer > 0)
        return;
    mode_info.currFloor = 1;
    init_course();
    g_another_stage_id = mode_info.currStageId;
    fade_screen_to_color(0, 0, 30);
    mode_info.attempts = 1;
    sub_mode_request = 0x30;
}

void smd_game_result_init(void)
{
    if (g_some_other_flags & 0xA)
        return;
    modeCtrl.submodeTimer = 0;
    lbl_2_bss_C = 0;
    event_dest(0x10);
    event_dest(0x11);
    event_init(0x10);
    event_init(0x11);
    if (current_stage_id == 0x7D || current_stage_id == 0x74)
    {
        g_some_frame_counter.flags |= 1;
        init_itemgroups();
    }
    BALL_FOREACH(ball->state = 0xB;)
    enable_camera(modeCtrl.currPlayer);
    camera_set_state(0x38);
    if (modeCtrl.playerCount == 3)
    {
        switch (modeCtrl.splitscreenMode)
        {
        case 0:
        default:
            camera_setup_splitscreen_viewports(3);
            break;
        case 1:
            set_camera_view_settings(0, 0.0f, 0.5f, 0.5f, 0.5f);
            set_camera_view_settings(1, 0.0f, 0.0f, 1.0f, 0.5f);
            set_camera_view_settings(2, 0.5f, 0.5f, 0.5f, 0.5f);
            set_camera_view_settings(3, 0.0f, 0.0f, 0.0f, 0.0f);
            break;
        case 2:
            set_camera_view_settings(0, 0.0f, 0.0f, 0.5f, 0.5f);
            set_camera_view_settings(1, 0.5f, 0.0f, 0.5f, 0.5f);
            set_camera_view_settings(2, 0.0f, 0.5f, 1.0f, 0.5f);
            set_camera_view_settings(3, 0.0f, 0.0f, 0.0f, 0.0f);
            break;
        case 3:
            camera_setup_splitscreen_viewports(4);
            cameras[3].flags |= 0x70;
            cameras[3].mode = 0xC;
            break;
        }
    }
    else
    {
        camera_setup_splitscreen_viewports(modeCtrl.playerCount);
    }
    fade_screen_to_color(0x100, 0, 60);
    g_fade_track_volume(62, 0);
    g_fade_track_volume(60, 3);
    call_SoundReqID_arg_0(0x1E0);
    sub_mode_request = 0x4D;
}

u32 lbl_2_data_A58[] = { 0x1DC, 0x1DD, 0x1DE, 0x1DF };

void smd_game_result_tick(void)
{
    int gained;
    int done;
    int i;

    if (g_some_other_flags & 0xA)
        return;
    if (modeCtrl.submodeTimer == 180)
    {
        for (i = 0; i < modeCtrl.playerCount; i++)
        {
            lbl_2_bss_10[i].playerId = -1;
            lbl_2_bss_10[i].bananas = -1;
            lbl_2_bss_10[i].rank = 0;
        }
        BALL_FOREACH(
            for (i = 0; i < modeCtrl.playerCount; i++)
            {
                if (ball->bananas >= lbl_2_bss_10[i].bananas)
                {
                    int k;

                    for (k = modeCtrl.playerCount - 2; k >= i; k--)
                        lbl_2_bss_10[k + 1] = lbl_2_bss_10[k];
                    lbl_2_bss_10[i].playerId = ball->playerId;
                    lbl_2_bss_10[i].bananas = ball->bananas;
                    break;
                }
            }
        )
        {
            int place = 0;
            int ties = 0;

            for (i = 0; i < modeCtrl.playerCount; i++)
            {
                if (i > 0)
                {
                    if (lbl_2_bss_10[i - 1].bananas != lbl_2_bss_10[i].bananas)
                    {
                        place += ties;
                        ties = 1;
                    }
                    else
                    {
                        ties++;
                    }
                }
                else
                {
                    place++;
                    ties = 1;
                }
                lbl_2_bss_10[i].rank = (place == modeCtrl.playerCount) ? 4 : place;
            }
        }
    }
    if (modeCtrl.submodeTimer > 180)
    {
        gained = 0;
        BALL_FOREACH(
            int finished = 0;

            if (modeCtrl.submodeTimer % 4 == 0 && ball->bananas > 0)
            {
                int n = 1;

                if (ball->bananas > 100)
                    n = 41;
                else if (ball->bananas > 10)
                    n = 3;
                ball->bonus += n;
                ball->bananas -= n;
                gained = 1;
                if (ball->bananas == 0)
                    finished = 1;
            }
            if (ball->rank == 0 && ball->bananas == 0)
                finished = 1;
            if (!finished)
                continue;
            for (i = 0; i < modeCtrl.playerCount; i++)
            {
                if (ball->playerId == lbl_2_bss_10[i].playerId)
                {
                    ball->rank = lbl_2_bss_10[i].rank;
                    break;
                }
            }
            call_SoundReqID_arg_0(0x68);
            if (ball->rank == 1)
                call_SoundReqID_arg_0(0x1B);
            else if (ball->rank == 4)
                call_SoundReqID_arg_0(0x3E);
            if (ball->rank == 1)
            {
                lbl_2_bss_40 = lbl_2_data_A58[ball->playerId];
                lbl_2_bss_C++;
            }
        )
        if (gained != 0)
            call_SoundReqID_arg_0(0x67);
    }
    if (++modeCtrl.submodeTimer < 240)
        return;
    done = 1;
    BALL_FOREACH(
        if (ball->bananas != 0)
            done = 0;
    )
    if (done != 0)
    {
        if (lbl_2_bss_C == 1)
            call_SoundReqID_arg_0(lbl_2_bss_40);
        else
            call_SoundReqID_arg_0(0x1D1);
        modeCtrl.submodeTimer = 600;
        sub_mode_request = 0x4E;
    }
}

static inline void spawn_result_confetti(struct Ball *ball)
{
    int i;
    struct Effect effect;
    Vec v;
    u16 cameraMask = 1 << ball->playerId;

    memset(&effect, 0, sizeof(effect));
    effect.type = 0;
    {
        float scale = 0.1f;

        mtxa_from_translate(&ball->pos);
        mtxa_translate_xyz(0.0f, 2.0f, 0.0f);
        i = 2;
        v.x = 0.0f;
        v.y = 0.0f;
        for (; i > 0; i--)
        {
            v.z = 0.5 * (scale * (1.0 + rand() / 32767.0f));
            mtxa_rotate_y((s16)(rand() & 0x7FFF));
            mtxa_rotate_x((s16)(rand() & 0x7FFF));
            mtxa_tf_point(&v, &effect.pos);
            effect.rotX = rand() & 0x7FFF;
            effect.rotY = rand() & 0x7FFF;
            effect.rotZ = rand() & 0x7FFF;
            effect.cameras = cameraMask;
            spawn_effect(&effect);
        }
    }
}

void smd_game_result_menu(void)
{
    if (g_some_other_flags & 0xA)
        return;
    if (lbl_2_bss_C == 1)
    {
        BALL_FOREACH(
            if (ball->rank == 1)
                spawn_result_confetti(ball);
        )
    }
    if (modeCtrl.submodeTimer > 60
     && ((merged_digital_inputs[2] & 0x100) || (merged_digital_inputs[2] & 0x1000)))
    {
        modeCtrl.submodeTimer = 60;
        call_SoundReqID_arg_0(0xD1);
    }
    if (modeCtrl.submodeTimer == 60)
    {
        fade_screen_to_color(0x101, 0xFFFFFF, 60);
        g_fade_track_volume(60, 2);
    }
    if (--modeCtrl.submodeTimer == 0)
        g_return_to_sel_ngc(modeCtrl.gameType);
}

void smd_game_intr_sel_init(void)
{
    if (g_some_other_flags & 0xA)
        return;
    dest_all_events();
    event_init(0x11);
    event_init(0x13);
    compare_play_points_with_99999_after_exit_game();
    g_display_playpoint_or_gift_message(3, 320.0f, 360.0f);
    fade_screen_to_color(2, 0, 1);
    play_track_and_fade_out_other_tracks(0x57, 30, 100);
    sub_mode_request = 0x50;
}

void smd_game_intr_sel_tick(void)
{
    if (g_some_other_flags & 0xA)
        return;
    if (fn_1_16C910() != 0)
        return;
    if (!(modeCtrl.courseFlags & 0x8000000))
    {
        g_return_to_sel_ngc(-1);
    }
    else
    {
        sub_mode_request = 0x61;
        modeCtrl.courseFlags &= ~0x8000000;
    }
}

void smd_game_sugg_save_init(void)
{
    if (g_some_other_flags & 0xA)
        return;
    if (storymode_save_files[0x31B] == 0)
    {
        modeCtrl.unk48 = 0;
        sub_mode_request = 0x62;
        fade_screen_to_color(0, 0, 1);
        fn_1_EFFD4(0);
    }
    else
    {
        g_return_to_sel_ngc(-1);
    }
}

void smd_game_sugg_save_tick(void)
{
    struct MemcardRequest req;
    char buf[32];

    if (g_some_other_flags & 0xA)
        return;
    switch (modeCtrl.unk48)
    {
    case 0:
        modeCtrl.unk48 = 1;
        modeCtrl.unk18 = 30;
        modeCtrl.courseFlags &= ~4;
        call_SoundReqID_arg_1(0x70);
        create_save_sprite();
        modeCtrl.unk10 = 1;
        /* fall through */
    case 1:
        if (modeCtrl.unk18 != 0)
        {
            modeCtrl.unk18--;
        }
        else
        {
            int sel = modeCtrl.unk10;
            int prev = sel;
            int chose;

            if ((merged_digital_inputs[2] & 1) || (merged_analog_inputs[2] & 1)
             || (merged_digital_inputs[2] & 2) || (merged_analog_inputs[2] & 2))
                sel ^= 1;
            if (sel != prev)
                call_SoundReqID_arg_0(0x6C);
            chose = 0;
            if (merged_digital_inputs[2] & 0x100)
            {
                chose = 1;
            }
            else if (merged_digital_inputs[2] & 0x200)
            {
                chose = 1;
                sel = 1;
            }
            if (chose != 0)
            {
                modeCtrl.unk48 = 2;
                modeCtrl.courseFlags |= 4;
                if (sel == 0)
                    call_SoundReqID_arg_0(0x6A);
                else
                    call_SoundReqID_arg_0(0x6B);
            }
            modeCtrl.unk10 = sel;
        }
        break;
    case 2:
        modeCtrl.unk48 = 3;
        modeCtrl.submodeTimer = 30;
        if (modeCtrl.unk10 == 0)
        {
            g_save_game_data();
            memset(&req, 0, sizeof(req));
            req.unk0 = 0;
            g_sprintf_memcard_error(0, buf);
            req.fileName = buf;
            req.files = storymode_save_files;
            if (lbl_1_bss_CF3D8 != 0)
                req.flags |= 0x20000000;
            g_some_replay_func4(0, (u8)g_last_used_memcard_slot_2, &req);
        }
        /* fall through */
    case 3:
        modeCtrl.submodeTimer--;
        if (modeCtrl.submodeTimer == 0)
        {
            if (modeCtrl.unk10 == 0)
                modeCtrl.unk48 = 4;
            else
                modeCtrl.unk48 = 6;
            dest_all_sprites();
        }
        break;
    case 4:
        modeCtrl.unk48 = 5;
        /* fall through */
    case 5:
        if (g_check_some_memcard_field() == 0)
        {
            if (g_get_some_memcard_var() == 0)
                OSReport("result_code: %d ", g_get_result_code());
            modeCtrl.unk48 = 6;
        }
        break;
    case 6:
        g_return_to_sel_ngc(-1);
        break;
    }
}

int get_next_stage_id(void)
{
    int id;

    switch (modeCtrl.gameType)
    {
    case 2:
        id = current_stage_id;
        break;
    default:
        if (modeCtrl.courseFlags & 1)
        {
            id = mode_info.unk32;
        }
        else
        {
            id = current_stage_id + 1;
            if (id > 420)
                id = -1;
        }
        break;
    }
    return id;
}

int g_decrement_active_ball_monkey_count(void)
{
    if ((mode_info.flags & 0x40) || modeCtrl.gameType == 1 || modeCtrl.gameType == 2 || modeCtrl.gameType == 14)
        return 1;
    mode_info.livesLost++;
    BALL_FOREACH(ball->unk2--;)
    switch (modeCtrl.gameType)
    {
    case 0:
        if (modeCtrl.playerCount == 1)
            fn_1_16C5B4();
        break;
    }
    return current_ball->unk2 > 0;
}

void g_smth_with_ending_course(void)
{
    fn_1_FC304();
}

void fn_2_64DC(int player)
{
    g_poolInfo.playerPool.statusList[player] = 0;
}

int fn_2_64F4(void)
{
    int ret = 1;
    s8 *status = g_poolInfo.playerPool.statusList;
    int i;

    for (i = 0; i < 4; i++, status++)
    {
        if (*status != 0)
        {
            ret = 0;
            break;
        }
    }
    return ret;
}

void fn_2_6538(void)
{
    mode_info.flags &= ~0x10;
    sub_mode_request = 0x59;
}

void fn_2_6568(void)
{
    struct Ball *ball = &balls[modeCtrl.currPlayer];

    set_storymode_score(ball->score);
    set_storymode_bananas(ball->bananas);
}

char *lbl_2_data_AD4[] = {
    "Demo version completed!",
    "You'll have to wait for the",
    "full version to continue!",
};

void smd_game_force_exit_init(void)
{
    struct TextBox tb;
    char **line;
    int i;
    struct Sprite *sprite;

    if (g_some_other_flags & 0xA)
        return;
    fade_screen_to_color(0x100, 0, 0);
    modeCtrl.submodeTimer = 600;
    modeCtrl.unk18 = 60;
    dest_all_events();
    event_init(0x11);
    event_init(0x13);
    memset(&tb, 0, sizeof(tb));
    tb.unk16 = 8;
    tb.x = 320;
    tb.y = 300;
    tb.unk14 = 0;
    tb.lines = 3;
    tb.unk2C |= 4;
    g_display_playpoint_or_gift_message_child(1, 1, &tb);
    for (line = lbl_2_data_AD4, i = 3; i > 0; i--, line++)
        g_smth_with_playpoint_or_gift_msg(1, *line);
    fn_1_16997C();
    create_copyright_text_sprites(0);
    load_bmp_by_id(3);
    sprite = create_sprite();
    if (sprite != NULL)
    {
        sprite->type = 1;
        sprite->tag = 3;
        sprite->x = 16.0f;
        sprite->y = 16.0f;
        sprite->mulR = 0xFF;
        sprite->mulG = 0xFF;
        sprite->mulB = 0xFF;
        sprite->textAlign = 0;
        sprite->bmpId = 0x30C;
        sprite->depth = 0.26f;
        sprite->mainFunc = NULL;
        strcpy(sprite->text, "RNK TITILE");
    }
    g_fade_track_volume(62, 0);
    set_sub_mode_destination(fn_2_694C);
    sub_mode_request = 0x5D;
}

void smd_game_force_exit_tick(void)
{
    if (g_some_other_flags & 0xA)
        return;
    if (modeCtrl.unk18 != 0)
        modeCtrl.unk18--;
    if ((float)modeCtrl.submodeTimer > 30.0f
     && ((merged_digital_inputs[2] & 0x100) || (merged_digital_inputs[2] & 0x1000)) && modeCtrl.unk18 == 0)
        modeCtrl.submodeTimer = 30;
    if (modeCtrl.submodeTimer == 30.0f)
    {
        fade_screen_to_color(0x101, 0xFFFFFF, modeCtrl.submodeTimer - 1);
        g_fade_track_volume(modeCtrl.submodeTimer - 1, 2);
    }
    modeCtrl.submodeTimer--;
    if (modeCtrl.submodeTimer < 0)
    {
        main_mode_request = 0;
        sub_mode_request = 3;
    }
}

void fn_2_694C(void)
{
    switch (sub_mode)
    {
    case 0x5C:
    case 0x5D:
        set_sub_mode_destination(fn_2_694C);
        break;
    default:
        call_free_bmp_by_id(3);
        break;
    }
}

void smd_game_scenscnplay_return(void)
{
    if (g_some_other_flags & 0xA)
        return;
    fn_2_884C();
    set_sub_mode_destination(g_some_dest_function_handler);
    sub_mode_request = 0x58;
}

void smd_game_force_over_init(void)
{
    if (g_some_other_flags & 0xA)
        return;
    modeCtrl.submodeTimer = 30;
    fade_screen_to_color(0x101, 0, modeCtrl.submodeTimer);
    g_fade_track_volume(modeCtrl.submodeTimer, 2);
    sub_mode_request = 0x60;
}

void smd_game_force_over_tick(void)
{
    if (g_some_other_flags & 0xA)
        return;
    modeCtrl.submodeTimer--;
    if (modeCtrl.submodeTimer < 0)
    {
        dest_all_sprites();
        sub_mode_request = 0x3D;
    }
}

void fn_2_6AEC(void)
{
    u16 *counter = &lbl_1_bss_D1320.stageFalls[current_stage_id];

    if (*counter < 0xFFFF)
        (*counter)++;
    if (modeCtrl.courseFlags & 0x400000)
    {
        counter = &lbl_1_bss_D1320.storyFalls[mode_info.storyStage + mode_info.storyWorld * 10];
        if (*counter < 0xFFFF)
            (*counter)++;
    }
    else if (modeCtrl.courseFlags & 1)
    {
        counter = NULL;
        if (modeCtrl.courseFlags & 0x10)
        {
            if (modeCtrl.courseFlags & 0x800000)
                counter = &lbl_1_bss_D1320.masterExtraFalls[lbl_2_bss_4 - 1];
            else
                counter = &lbl_1_bss_D1320.masterFalls[lbl_2_bss_4 - 1];
        }
        else
        {
            switch (modeCtrl.difficulty)
            {
            case 0:
                if (modeCtrl.courseFlags & 8)
                    counter = &lbl_1_bss_D1320.begExtraFalls[lbl_2_bss_4 - 1];
                else
                    counter = &lbl_1_bss_D1320.begFalls[lbl_2_bss_4 - 1];
                break;
            case 1:
                if (modeCtrl.courseFlags & 8)
                    counter = &lbl_1_bss_D1320.advExtraFalls[lbl_2_bss_4 - 1];
                else
                    counter = &lbl_1_bss_D1320.advFalls[lbl_2_bss_4 - 1];
                break;
            case 2:
                if (modeCtrl.courseFlags & 8)
                    counter = &lbl_1_bss_D1320.expExtraFalls[lbl_2_bss_4 - 1];
                else
                    counter = &lbl_1_bss_D1320.expFalls[lbl_2_bss_4 - 1];
                break;
            }
        }
        if (counter != NULL && *counter < 0xFFFF)
            (*counter)++;
    }
}

void g_maybe_some_goal_func(void)
{
    u16 *counter = &lbl_1_bss_D1320.stageGoals[current_stage_id];

    if (*counter < 0xFFFF)
        (*counter)++;
    if (modeCtrl.courseFlags & 0x400000)
    {
        counter = &lbl_1_bss_D1320.storyGoals[mode_info.storyStage + mode_info.storyWorld * 10];
        if (*counter < 0xFFFF)
            (*counter)++;
    }
    else if (modeCtrl.courseFlags & 1)
    {
        counter = NULL;
        if (modeCtrl.courseFlags & 0x10)
        {
            if (modeCtrl.courseFlags & 0x800000)
                counter = &lbl_1_bss_D1320.masterExtraGoals[lbl_2_bss_4 - 1];
            else
                counter = &lbl_1_bss_D1320.masterGoals[lbl_2_bss_4 - 1];
        }
        else
        {
            switch (modeCtrl.difficulty)
            {
            case 0:
                if (modeCtrl.courseFlags & 8)
                    counter = &lbl_1_bss_D1320.begExtraGoals[lbl_2_bss_4 - 1];
                else
                    counter = &lbl_1_bss_D1320.begGoals[lbl_2_bss_4 - 1];
                break;
            case 1:
                if (modeCtrl.courseFlags & 8)
                    counter = &lbl_1_bss_D1320.advExtraGoals[lbl_2_bss_4 - 1];
                else
                    counter = &lbl_1_bss_D1320.advGoals[lbl_2_bss_4 - 1];
                break;
            case 2:
                if (modeCtrl.courseFlags & 8)
                    counter = &lbl_1_bss_D1320.expExtraGoals[lbl_2_bss_4 - 1];
                else
                    counter = &lbl_1_bss_D1320.expGoals[lbl_2_bss_4 - 1];
                break;
            }
        }
        if (counter != NULL && *counter < 0xFFFF)
            (*counter)++;
    }
}

void g_preload_stage_or_results(void)
{
    int id = -1;
    int chara = active_monkey_id[modeCtrl.currPlayer];

    if (modeCtrl.courseFlags & 1)
    {
        if (modeCtrl.courseFlags & 0x10)
        {
            id = lbl_2_data_28C[chara];
        }
        else
        {
            switch (modeCtrl.difficulty)
            {
            case 0:
                id = lbl_2_data_280[chara];
                break;
            case 1:
                id = lbl_2_data_284[chara];
                break;
            case 2:
                id = lbl_2_data_288[chara];
                break;
            }
        }
    }
    if (id >= 0)
    {
        int group;

        fn_1_D391C();
        group = g_swapDiscQueueGroup(5);
        g_preload_next_stage_files(id, -1, 0);
        g_swapDiscQueueGroup(group);
    }
}
