/* Story mode scenario state machine. */
#include "types.h"
#include "camera.h"
#include "dip.h"
#include "event.h"
#include "input.h"
#include "libc.h"
#include "mode.h"
#include "os.h"
#include "pool.h"
#include "scen.h"

#define PLAY_TIME_MAX 0x40000000

/* One save file slot, size 0x84. */
struct StorySave
{
    u8 filler0[4];
    /*0x04*/ u32 flags;
    /*0x08*/ char name[0xD];
    /*0x15*/ s8 world;
    /*0x16*/ u8 unk16;
    /*0x18*/ u32 playTime;
    /*0x1C*/ int score;
    /*0x20*/ int bananas;
    /*0x24*/ u8 stageCount;
    /*0x25*/ u8 stages[0x30 - 0x25];
    /*0x30*/ u32 unk30;
    u8 filler34[0x84 - 0x34];
};

/* What comes after a world. */
struct WorldEnd
{
    /*0x00*/ u16 id;
    /*0x02*/ s8 type;
    /*0x03*/ s8 unk3;
    /*0x04*/ int *world;  /* filled by scen_stage.c from its struct StoryUnlock */
};

struct SelectEntry
{
    /*0x00*/ s8 type;
    /*0x01*/ s8 x;
    /*0x02*/ s8 y;
    /*0x04*/ char *name;
    /*0x08*/ char *nameJp;
    /*0x0C*/ u16 next;
};

extern u32 g_font_char_var_3;
extern s16 g_another_stage_id;
extern u8 g_some_gift_menu_flags;
extern u32 g_related_to_pause_menu_input[];
extern struct StorySave storymode_save_files[];

void OSReport(const char *, ...);
void camera_set_state(int);
void create_story_file_nameentry_sprite(void);
void clear_sm_state(void);
void debug_textdraw_put_buf_1(char *);
void debug_textdraw_set_color(u8);
void debug_textdraw_set_pos(int, int);
void debug_textdraw_vsprintf_buf_1(char *, ...);
void dest_all_events(void);
void dest_all_sprites(void);
void disc_queue_status(int);
void dmd_scen_entry_main_child(void);
void dmd_scen_loadgame_main_child(void);
void dmd_scen_sel_floor_init_child(int);
void enable_camera(int);
void event_dest(int);
void event_init(int);
void fade_screen_to_color(int, int, int);
void fn_1_16A9D0(void);
void fn_1_16AA00(int);
void fn_1_16C720(void);
void fn_1_3E100(int, int);
int fn_1_A4CA8(int, int);
void fn_1_A4F58(int, int);
void fn_1_D37E8(int);
void fn_1_EFFD4(int);
void fn_2_10754(void);
void fn_2_10900(void);
void fn_2_12468(void);
int fn_2_1248C(void);
void fn_2_15CC4(void);
int fn_2_15CFC(void);
void fn_2_A9EC(void);
void fn_2_AD74(void);
void fn_2_EA58(int);
int fn_2_F1E0(void);
void fn_2_F398(void);
void fn_2_FC4C(void);
void fn_2_FE94(void);
int fn_2_FED8(void);
int fn_2_FEE8(int, int);
void g_assign_new_main_and_sub_mode_for_play_tick(int, int);
void g_check_if_game_over_or_after_credits_sequence(int, struct WorldEnd *);
void g_draw_now_loading_text(void);
void g_fade_track_volume(int, int);
void g_free_scen_stage_name_buffer(void);
int g_get_debug_level_select_loading_left_asterisks(void);
void g_get_story_file_name(char *);
int g_get_storymode_next_world(void);
void g_handle_goal(void);
void g_handle_storymode_stageselect_state(void);
void g_init_scen_stage_name_buffer(void);
int g_is_sjis_and_more_than_24_mib(void);
void g_preload_next_stage_files(int, int, int);
void g_related_to_loading_dataselect_menu(void);
void g_return_to_sel_ngc(int);
void g_set_unlockables_status(void);
void g_smth_with_storymode(void);
void g_some_scenario_init_func_2(void);
void g_some_scenario_init_func_3(void);
int g_swapDiscQueueGroup(int);
int get_story_mode_stage_id(int, int);
int get_storymode_menu_state(void);
int get_storymode_stage_time_limit(int, int);
int get_world_stage_count(int);
int get_world_unbeaten_stage_count(int);
int is_storymode_stage_beaten(int, int);
void load_bmp_by_id(int);
void load_bmp_by_id_child(int);
void load_stage(int);
void play_track_and_fade_out_other_tracks(int, int, int);
void preload_ape(int, int, int, int);
void queue_bmp_by_id_read(int);
void queue_stage_load(int);
void unlock_storymode_stage(int, int);

void dmd_scen_1st_init(void);
void dmd_scen_return_init(void);
void dmd_scen_newgame_init(void);
void dmd_scen_newgame_main(void);
void dmd_scen_loadgame_init(void);
void dmd_scen_loadgame_main(void);
void dmd_scen_sceneplay_init(void);
void dmd_scen_sceneplay_main(void);
void dmd_scen_sel_world_init(void);
void dmd_scen_sel_world_next(void);
void dmd_scen_sel_floor_init(void);
void dmd_scen_sel_floor_main(void);
void dmd_scen_game_init(void);
void dmd_scen_game_main(void);
void dmd_scen_game_clear_init(void);
void dmd_scen_game_clear_main(void);
void dmd_scen_game_over_init(void);
void dmd_scen_game_over_main(void);
void dmd_scen_select_init(void);
void dmd_scen_1st_init(void);
void dmd_scen_return_init(void);
void dmd_scen_newgame_init(void);
void dmd_scen_newgame_main(void);
void dmd_scen_loadgame_init(void);
void dmd_scen_loadgame_main(void);
void dmd_scen_sceneplay_init(void);
void dmd_scen_sceneplay_main(void);
void dmd_scen_sel_world_init(void);
void dmd_scen_sel_world_next(void);
void dmd_scen_sel_floor_init(void);
void dmd_scen_sel_floor_main(void);
void dmd_scen_game_init(void);
void dmd_scen_game_main(void);
void dmd_scen_game_clear_init(void);
void dmd_scen_game_clear_main(void);
void dmd_scen_game_over_init(void);
void dmd_scen_game_over_main(void);
void dmd_scen_select_init(void);
void dmd_scen_select_main(void);
void dmd_scen_entry_init(void);
void dmd_scen_entry_main(void);
void dmd_scen_scnplay_return(void);
void dmd_scen_scnplay_return_loading(void);
void dmd_scen_loadgame_restore(void);
void dmd_scen_nameentry_init(void);
void dmd_scen_nameentry_main(void);
void fn_2_8C40(struct StorySave *);
void fn_2_8EA4(void);
void fn_2_8F20(u32);
void fn_2_9BA0(void);
u32 g_get_storymode_playtime_frames(void);
void g_preload_ape_model_for_stageselect(void);
void g_save_storymode_progress(void *);
void g_some_scenario_init_func_1(void);
void g_some_storymode_mode_func(void);
void g_some_storymode_mode_handler(void);
struct StorySave *get_current_storymode_save_file(void);

u32 lbl_2_data_AF0[] = {
    0xFE000102, 0x03040506, 0x0708090A, 0x0B0CFE0D,
    0x0E0FFF00, 0x00010203, 0x04050607, 0x08090A0B,
    0x0C0D0E0F, 0xFF000000, 0x31000000, 0x00000000,
    0x3800B400, 0x00010000, 0x01000000, 0x00020000,
    0x10004C00, 0x00030000, 0x01000000, 0x00040000,
    0x18000000, 0x00090000, 0x40000000, 0x00000000,
    0x01000000, 0x000B0000, 0x18000000, 0x000C0000,
    0x18000000, 0x00050000, 0x40000000, 0x00000000,
    0x01000000, 0x00070000, 0x18000000, 0x00080000,
    0x3800B400, 0x000D0000, 0x40000000, 0x00000000,
    0x18000000, 0x00140000, 0x40000000, 0x00000000,
    0x01000000, 0x00160000, 0x18000000, 0x00170000,
    0x18000000, 0x00180000, 0x18000000, 0x000F0000,
    0x40000000, 0x00000000, 0x01000000, 0x00110000,
    0x18000000, 0x00120000, 0x18000000, 0x00130000,
    0x18000000, 0x00000000, 0x18000000, 0x00010000,
    0x18000000, 0x00020000, 0x18000000, 0x00030000,
    0x18000000, 0x00040000, 0x18000000, 0x00050000,
    0x18000000, 0x00060000, 0x18000000, 0x00070000,
    0x18000000, 0x00080000, 0x18000000, 0x00090000,
    0x18000000, 0x000A0000, 0x18000000, 0x000B0000,
    0x18000000, 0x000C0000, 0x18000000, 0x000D0000,
    0x18000000, 0x000E0000, 0x18000000, 0x000F0000,
};
void (*story_mode_funcs[])(void) = {
    dmd_scen_1st_init,
    dmd_scen_return_init,
    dmd_scen_newgame_init,
    dmd_scen_newgame_main,
    dmd_scen_loadgame_init,
    dmd_scen_loadgame_main,
    dmd_scen_sceneplay_init,
    dmd_scen_sceneplay_main,
    dmd_scen_sel_world_init,
    dmd_scen_sel_world_next,
    dmd_scen_sel_floor_init,
    dmd_scen_sel_floor_main,
    dmd_scen_game_init,
    dmd_scen_game_main,
    dmd_scen_game_clear_init,
    dmd_scen_game_clear_main,
    dmd_scen_game_over_init,
    dmd_scen_game_over_main,
    dmd_scen_select_init,
    dmd_scen_select_main,
    dmd_scen_entry_init,
    dmd_scen_entry_main,
    dmd_scen_scnplay_return,
    dmd_scen_scnplay_return_loading,
    dmd_scen_loadgame_restore,
    dmd_scen_nameentry_init,
    dmd_scen_nameentry_main,
    NULL,
};

char *story_mode_names[] = {
    "DMD_SCEN_1ST_INIT",
    "DMD_SCEN_RETURN_INIT",
    "DMD_SCEN_NEWGAME_INIT",
    "DMD_SCEN_NEWGAME_MAIN",
    "DMD_SCEN_LOADGAME_INIT",
    "DMD_SCEN_LOADGAME_MAIN",
    "DMD_SCEN_SCENEPLAY_INIT",
    "DMD_SCEN_SCENEPLAY_LOADING",
    "DMD_SCEN_SEL_WORLD_INIT",
    "DMD_SCEN_SEL_WORLD_NEXT",
    "DMD_SCEN_SEL_FLOOR_INIT",
    "DMD_SCEN_SEL_FLOOR_MAIN",
    "DMD_SCEN_GAME_INIT",
    "DMD_SCEN_GAME_MAIN",
    "DMD_SCEN_GAME_CLEAR_INIT",
    "DMD_SCEN_GAME_CLEAR_MAIN",
    "DMD_SCEN_GAME_OVER_INIT",
    "DMD_SCEN_GAME_OVER_MAIN",
    "DMD_SCEN_SELECT_INIT",
    "DMD_SCEN_SELECT_MAIN",
    "DMD_SCEN_ENTRY_INIT",
    "DMD_SCEN_ENTRY_MAIN",
    "DMD_SCEN_SCNPLAY_RETURN",
    "DMD_SCEN_SCNPLAY_RETURN_LOADING",
    "DMD_SCEN_LOADGAME_RESTORE",
    "DMD_SCEN_NAMEENTRY_INIT",
    "DMD_SCEN_NAMEENTRY_MAIN",
    NULL,
};

static void set_dest_func(void (*func)(void))
{
    if (scen_info.destFunc != NULL)
        OSPanic("scen_mode.c", 0x99, "multiple dest func");
    scen_info.destFunc = func;
}

void smd_game_scenario_init_child(void)
{
    g_some_scenario_init_func_1();
    g_some_scenario_init_func_2();
    g_some_scenario_init_func_3();
    g_init_scen_stage_name_buffer();
    scen_info.unk0 = 0;
}

void fn_2_881C(void)
{
    g_init_scen_stage_name_buffer();
    scen_info.unk0 = 1;
}

void fn_2_884C(void)
{
    g_some_scenario_init_func_2();
    g_some_scenario_init_func_3();
    g_init_scen_stage_name_buffer();
    scen_info.unk0 = 0x16;
}

void smd_game_scenario_tick_child(void)
{
    g_some_storymode_mode_handler();
    g_smth_with_storymode();
    g_handle_storymode_stageselect_state();
}

void g_some_storymode_dest_function(void)
{
    fn_2_8EA4();
    g_free_scen_stage_name_buffer();
    fn_2_10754();
    dest_all_sprites();
    event_dest(EVENT_CAMERA);
}

void game_scenario_draw_func(void)
{
    fn_2_F398();
    fn_2_10900();
    fn_2_FC4C();
}

int fn_2_8908(void)
{
    return fn_2_F1E0();
}

int get_storymode_score(void)
{
    return scen_info.score;
}

void set_storymode_score(int score)
{
    scen_info.score = score;
}

int get_storymode_banana_count(void)
{
    return scen_info.bananas;
}

void set_storymode_bananas(int bananas)
{
    scen_info.bananas = bananas;
}

#pragma auto_inline off
void g_preload_ape_model_for_stageselect(void)
{
    struct WorldEnd end;
    int world;
    int group;
    int next;

    fn_1_D37E8(4);
    group = g_swapDiscQueueGroup(4);
    next = fn_2_F1E0();
    if (next >= 0)
    {
        g_check_if_game_over_or_after_credits_sequence(next, &end);
        g_preload_next_stage_files(end.id, -1, 0);
    }
    else
    {
        world = scen_info.world;
        queue_stage_load(get_story_mode_stage_id(world, fn_2_FEE8(world, scen_info.unk6)));
        if (((u8 *)storymode_save_files)[0x31B] != 0)
            fn_1_EFFD4(0);
    }
    queue_bmp_by_id_read(0xF);
    preload_ape(0, 0, 0, 1);
    g_swapDiscQueueGroup(group);
}

void fn_2_8A40(struct StorySave *save)
{
    struct WorldEnd end;
    float stage;
    int group;
    int world;
    int next;

    fn_2_8C40(save);
    fn_1_D37E8(4);
    group = g_swapDiscQueueGroup(4);
    next = g_get_storymode_next_world();
    if (next >= 0)
    {
        g_check_if_game_over_or_after_credits_sequence(next, &end);
        g_preload_next_stage_files(end.id, -1, 0);
    }
    else
    {
        world = scen_info.world;
        stage = 0.0f;
        if (is_storymode_stage_beaten(world, stage) != 0)
            stage = fn_2_FEE8(world, scen_info.unk6);
        queue_stage_load(get_story_mode_stage_id(world, stage));
    }
    g_swapDiscQueueGroup(group);
}

void g_save_storymode_progress(void *dst)
{
    struct StorySave *save;
    int i;
    int world;
    int count;
    u8 *stage;

    save = get_current_storymode_save_file();
    if (dst != NULL)
        memcpy_handler(dst, save, sizeof(*save));
    world = scen_info.world;
    save->world = world;
    save->unk16 = 0;
    save->playTime = g_get_storymode_playtime_frames();
    save->score = scen_info.score;
    save->bananas = scen_info.bananas;
    save->unk30 = scen_info.unk34[0];
    count = get_world_unbeaten_stage_count(world);
    save->stageCount = count;
    stage = save->stages;
    for (i = 0; i < count; i++, stage++)
        *stage = fn_1_A4CA8(world, i);
}

void fn_2_8C40(struct StorySave *save)
{
    int i;
    int world;
    int count;
    u8 *stage;

    scen_info.world = save->world;
    fn_2_8F20(save->playTime);
    scen_info.score = save->score;
    scen_info.bananas = save->bananas;
    scen_info.unk34[0] = save->unk30;
    world = scen_info.world;
    count = save->stageCount;
    stage = save->stages;
    for (i = 0; i < count; i++, stage++)
        fn_1_A4F58(world, *stage);
    scen_info.flags &= ~0x40;
}

#pragma auto_inline reset

u32 fn_2_8D18(void)
{
    return scen_info.flags & 0x20;
}

u32 fn_2_8D2C(void)
{
    return scen_info.flags & 2;
}

int get_current_storymode_stage_time_limit(void)
{
    return get_storymode_stage_time_limit(scen_info.world, scen_info.unk6);
}

int get_storymode_stage_time_limit_wrapper(int world, int stage)
{
    return get_storymode_stage_time_limit(world, stage);
}

u32 fn_2_8D98(void)
{
    return g_get_storymode_playtime_frames();
}

void g_some_scenario_init_func_1(void)
{
    memset(&scen_info, 0, sizeof(scen_info));
    scen_info.unk2 = -1;
}

void g_some_storymode_mode_handler(void)
{
    void (*func)(void);

    if (scen_info.unk2 >= 0)
    {
        scen_info.unk0 = scen_info.unk2;
        scen_info.unk2 = -1;
        func = scen_info.destFunc;
        if (func != NULL)
        {
            scen_info.destFunc = NULL;
            func();
        }
    }
    story_mode_funcs[scen_info.unk0]();
}

void fn_2_8EA4(void)
{
    void (*func)(void);
    int mode;

    func = scen_info.destFunc;
    if (func != NULL)
    {
        mode = scen_info.unk0;
        scen_info.unk0 = -1;
        scen_info.destFunc = NULL;
        func();
        scen_info.unk0 = (s16)mode;
    }
}

void fn_2_8F20(u32 time)
{
    scen_info.playTime = time;
    scen_info.playTimeBase = frame_counter;
}

u32 g_get_storymode_playtime_frames(void)
{
    u32 total;
    u32 delta;

    if (scen_info.playTime >= PLAY_TIME_MAX)
    {
        total = PLAY_TIME_MAX;
    }
    else
    {
        delta = frame_counter;
        if (delta < scen_info.playTimeBase)
            delta = -1 - (scen_info.playTimeBase - delta);
        else
            delta -= scen_info.playTimeBase;
        if (delta >= PLAY_TIME_MAX)
        {
            total = PLAY_TIME_MAX;
        }
        else
        {
            total = scen_info.playTime + delta;
            if (total >= PLAY_TIME_MAX)
                total = PLAY_TIME_MAX;
        }
    }
    scen_info.playTime = total;
    scen_info.playTimeBase = frame_counter;
    return total;
}

struct SelectEntry story_select_menu[] = {
    { 1, 20, 4, "LOADGAME", "\x83\x8D\x81\x5B\x83\x68\x83\x51\x81\x5B\x83\x80", 4 },
    { 1, 20, -2, "ENTRY", "\x96\xBC\x91\x4F\x93\xFC\x97\xCD", 20 },
    { 1, 20, -2, "GAME START", "\x83\x51\x81\x5B\x83\x80\x8A\x4A\x8E\x6E", 2 },
};

static s16 story_select_sel;

void dmd_scen_select_init(void)
{
    int i;

    dest_all_events();
    fade_screen_to_color(0x100, 0, 1);
    modeCtrl.courseFlags |= 0x1000000;
    for (i = 0; story_select_menu[i].type == 0; i++)
        ;
    story_select_sel = (s16)i;
    scen_info.unk2 = 0x13;
}

#define DOWN_REPEAT() \
    ((pad_status_groups[0].unk0[4].button & PAD_BUTTON_DOWN) || (analog_inputs[0][4] & PAD_BUTTON_DOWN) \
     || (((pad_status_groups[0].unk0[0].button & PAD_BUTTON_DOWN) || (analog_inputs[0][0] & PAD_BUTTON_DOWN)) \
         && (analog_inputs[0][0] & PAD_BUTTON_B)))
#define UP_REPEAT() \
    ((pad_status_groups[0].unk0[4].button & PAD_BUTTON_UP) || (analog_inputs[0][4] & PAD_BUTTON_UP) \
     || (((pad_status_groups[0].unk0[0].button & PAD_BUTTON_UP) || (analog_inputs[0][0] & PAD_BUTTON_UP)) \
         && (analog_inputs[0][0] & PAD_BUTTON_B)))

void dmd_scen_select_main(void)
{
    struct SelectEntry *entry;
    int i;
    int prevX = 0;
    int prevY = 0;
    int x;
    int y;
    u32 phase;

    if ((g_some_other_flags & 0xA) == 0)
    {
        i = story_select_sel;
        if (DOWN_REPEAT())
        {
            do
            {
                i++;
                if (i >= 3U)
                    i = 0;
            } while (story_select_menu[i].type == 0);
        }
        if (UP_REPEAT())
        {
            do
            {
                i--;
                if (i < 0)
                    i = 2;
            } while (story_select_menu[i].type == 0);
        }
        story_select_sel = (s16)i;
        if (pad_status_groups[0].unk0[2].button & PAD_BUTTON_A)
        {
            scen_info.unk2 = (s16)story_select_menu[i].next;
            modeCtrl.courseFlags &= ~0x1000000;
        }
    }
    debug_textdraw_set_pos(1, 1);
    if (g_is_sjis_and_more_than_24_mib())
        debug_textdraw_put_buf_1("\x83\x58\x83\x67\x81\x5B\x83\x8A\x81\x5B\x83\x82\x81\x5B\x83\x68");
    else
        debug_textdraw_put_buf_1("STORY MODE");
    entry = story_select_menu;
    for (i = 0; i < 3U; i++, entry++)
    {
        x = entry->x;
        y = entry->y;
        if (x <= 0)
            x = prevX - x;
        if (y <= 0)
            y = prevY - y;
        prevX = x;
        prevY = y;
        debug_textdraw_set_pos(x, y);
        if (i == story_select_sel)
        {
            phase = (g_font_char_var_3 >> 3) % 6;
            if ((int)phase > 3)
                phase = 7 - phase;
            phase = ((phase & 3) << 6) | ((phase & 3) << 4) | ((phase & 3) << 2) | 3;
            debug_textdraw_set_color(phase);
            if (g_is_sjis_and_more_than_24_mib())
            {
                fn_1_3E100(-3, 0);
                debug_textdraw_vsprintf_buf_1("\x81\x9F ");
            }
            else
            {
                fn_1_3E100(-2, 0);
                debug_textdraw_put_buf_1("\x1C ");
            }
        }
        if (i == story_select_sel)
            debug_textdraw_set_color(0xFF);
        else
            debug_textdraw_set_color(0x57);
        switch (entry->type)
        {
        case 0:
            debug_textdraw_set_color(0x33);
            debug_textdraw_put_buf_1(g_is_sjis_and_more_than_24_mib() ? entry->nameJp : entry->name);
            debug_textdraw_set_color(0xFF);
            break;
        case 1:
            debug_textdraw_put_buf_1(g_is_sjis_and_more_than_24_mib() ? entry->nameJp : entry->name);
            break;
        }
    }
}

void dmd_scen_1st_init(void)
{
    int i;
    s8 *status;

    scen_info.unk2 = 4;
    modeCtrl.gameType = 0xE;
    modeCtrl.courseFlags &= 0x200;
    modeCtrl.courseFlags |= 0x400000;
    scen_info.score = 0;
    scen_info.bananas = 0;
    fn_2_8F20(0);
    modeCtrl.playerCount = 1;
    modeCtrl.unk30 = 1;
    modeCtrl.currPlayer = 0;
    status = g_poolInfo.playerPool.statusList;
    for (i = 0; i < modeCtrl.playerCount; i++, status++)
        *status = 2;
    for (; i < 4; i++, status++)
        *status = 0;
    dest_all_events();
    load_stage(0);
    event_init(EVENT_SOUND);
    event_init(EVENT_CAMERA);
    event_init(EVENT_SPRITE);
    event_init(EVENT_ITEM);
    event_init(EVENT_REND_EFC);
    event_init(EVENT_STOBJ);
    event_init(EVENT_EFFECT);
    event_init(EVENT_STAGE);
    event_init(EVENT_BACKGROUND);
    enable_camera(0);
    load_bmp_by_id_child(0xF);
    clear_sm_state();
    g_set_unlockables_status();
    g_fade_track_volume(30, 2);
    fade_screen_to_color(0x100, 0xFFFFFF, 30);
}

void dmd_scen_return_init(void)
{
    mode_info.flags &= ~0x100;
    fn_1_D37E8(4);
    g_get_storymode_playtime_frames();
    event_dest(EVENT_WORLD);
    event_dest(EVENT_BALL);
    event_dest(EVENT_APE);
    event_dest(EVENT_SOUND);
    event_dest(EVENT_MINIMAP);
    event_dest(EVENT_RECPLAY);
    event_init(EVENT_SOUND);
    event_init(EVENT_STAGE);
    dest_all_sprites();
    load_bmp_by_id_child(0xF);
    if (modeCtrl.unk43 != 0)
        fn_1_A4F58(scen_info.world, scen_info.unk6);
    fade_screen_to_color(0x100, 0xFFFFFF, 30);
    scen_info.nextWorld = (s16)g_get_storymode_next_world();
    if (scen_info.nextWorld < 0)
        scen_info.unk2 = 10;
    else
        scen_info.unk2 = 6;
}

void dmd_scen_newgame_init(void)
{
    scen_info.unk2 = 3;
}

void dmd_scen_newgame_main(void)
{
    scen_info.unk2 = 6;
    scen_info.nextWorld = 0;
}

void dmd_scen_loadgame_init(void)
{
    scen_info.unk2 = 5;
    set_dest_func(fn_2_9BA0);
    g_related_to_pause_menu_input[1] |= 2;
    dest_all_events();
    load_stage(0xC9);
    event_init(EVENT_SOUND);
    event_init(EVENT_CAMERA);
    event_init(EVENT_SPRITE);
    event_init(EVENT_ITEM);
    event_init(EVENT_REND_EFC);
    event_init(EVENT_STOBJ);
    event_init(EVENT_EFFECT);
    event_init(EVENT_STAGE);
    event_init(EVENT_BACKGROUND);
    enable_camera(0);
    camera_set_state(0x52);
    load_bmp_by_id(0x18);
    g_related_to_loading_dataselect_menu();
    fade_screen_to_color(0x100, 0xFFFFFF, 30);
}

void dmd_scen_loadgame_main(void)
{
    dmd_scen_loadgame_main_child();
    switch (get_storymode_menu_state())
    {
    case 1:
        fn_2_15CC4();
        scen_info.saveSlot = (s8)fn_2_15CFC();
        scen_info.unk2 = 0x14;
        break;
    case 2:
        fn_2_15CC4();
        scen_info.saveSlot = (s8)fn_2_15CFC();
        fn_2_8C40(get_current_storymode_save_file());
        scen_info.unk2 = 0x18;
        break;
    case 3:
        fn_2_15CC4();
        g_return_to_sel_ngc(-1);
        break;
    }
}

void dmd_scen_loadgame_restore(void)
{
    scen_info.nextWorld = (s16)g_get_storymode_next_world();
    fn_1_D37E8(4);
    if (scen_info.nextWorld < 0 && g_get_debug_level_select_loading_left_asterisks() != 0)
        return;
    fn_2_8C40(get_current_storymode_save_file());
    g_get_storymode_playtime_frames();
    event_dest(EVENT_WORLD);
    event_dest(EVENT_BALL);
    event_dest(EVENT_APE);
    event_dest(EVENT_SOUND);
    event_dest(EVENT_MINIMAP);
    event_init(EVENT_SOUND);
    dest_all_sprites();
    load_bmp_by_id_child(0xF);
    fade_screen_to_color(0x100, 0xFFFFFF, 30);
    scen_info.nextWorld = (s16)g_get_storymode_next_world();
    if (scen_info.nextWorld < 0)
        scen_info.unk2 = 10;
    else
        scen_info.unk2 = 6;
}

void fn_2_9BA0(void)
{
    switch (scen_info.unk0)
    {
    case 4:
    case 5:
        set_dest_func(fn_2_9BA0);
        break;
    default:
        g_related_to_pause_menu_input[1] &= ~2;
        break;
    }
}

void dmd_scen_sceneplay_init(void)
{
    scen_info.unk2 = 7;
    modeCtrl.submodeTimer = 1200;
    g_fade_track_volume(30, 2);
    event_init(EVENT_SPRITE);
    fade_screen_to_color(1, 0, 30);
    if (g_get_debug_level_select_loading_left_asterisks() > 4)
    {
        g_draw_now_loading_text();
        fade_screen_to_color(2, 0, 0);
    }
}

void dmd_scen_sceneplay_main(void)
{
    struct WorldEnd end;
    int left;
    int world;

    left = g_get_debug_level_select_loading_left_asterisks();
    if (modeCtrl.submodeTimer > 0)
    {
        modeCtrl.submodeTimer--;
    }
    else
    {
        left = 0;
        OSReport("[ WARNING ] Pre Load Sync Time Out!\n");
    }
    if (left == 0)
    {
        g_check_if_game_over_or_after_credits_sequence(scen_info.nextWorld, &end);
        switch (end.type)
        {
        case 0:
            scen_info.stageId = get_story_mode_stage_id(0, 0);
            break;
        case 1:
            world = scen_info.world;
            if (world < 9)
            {
                scen_info.stageId = get_story_mode_stage_id(world + 1, 0);
            }
            else
            {
                scen_info.stageId = -1;
                modeCtrl.courseFlags |= 0x100000;
            }
            break;
        default:
            scen_info.stageId = -1;
            break;
        }
        dest_all_events();
        modeCtrl.courseFlags &= ~4;
        fn_1_16A9D0();
        fn_1_16AA00(end.id);
        g_assign_new_main_and_sub_mode_for_play_tick(2, 0x5E);
        main_mode_request = 6;
        sub_mode_request = 0xF9;
    }
}

void dmd_scen_scnplay_return(void)
{
    int next;

    event_dest(EVENT_WORLD);
    event_dest(EVENT_BALL);
    event_dest(EVENT_APE);
    event_dest(EVENT_SOUND);
    dest_all_events();
    event_init(EVENT_SOUND);
    event_init(EVENT_SPRITE);
    dest_all_sprites();
    if ((scen_info.unk34[scen_info.nextWorld >> 5] & (1 << (scen_info.nextWorld & 0x1F))) == 0)
    {
        scen_info.unk34[scen_info.nextWorld >> 5] |= 1 << (scen_info.nextWorld & 0x1F);
        scen_info.flags |= 0x40;
    }
    next = g_get_storymode_next_world();
    if (next >= 0)
    {
        scen_info.nextWorld = (s16)next;
        scen_info.unk2 = 6;
    }
    else
    {
        scen_info.unk2 = 0x17;
        modeCtrl.submodeTimer = 1200;
        fn_1_D37E8(4);
        fade_screen_to_color(1, 0, 30);
        if (g_get_debug_level_select_loading_left_asterisks() > 2)
            g_draw_now_loading_text();
        scen_info.flags &= ~0x80;
    }
}

void dmd_scen_scnplay_return_loading(void)
{
    struct WorldEnd end;
    int left;

    left = g_get_debug_level_select_loading_left_asterisks();
    if (left < 6 && (scen_info.flags & 0x80) == 0)
    {
        g_check_if_game_over_or_after_credits_sequence(scen_info.nextWorld, &end);
        switch (end.unk3)
        {
        case 0:
        case 1:
            play_track_and_fade_out_other_tracks(0, 0x3C, 100);
            scen_info.flags |= 0x80;
            break;
        }
    }
    if (modeCtrl.submodeTimer > 0)
    {
        modeCtrl.submodeTimer--;
    }
    else
    {
        left = 0;
        OSReport("[ WARNING ] Pre Load Sync Time Out!\n");
    }
    if (left <= 0)
    {
        fn_2_EA58(60);
        event_init(EVENT_BALL);
        event_init(EVENT_APE);
        event_init(EVENT_CAMERA);
        enable_camera(0);
        load_bmp_by_id_child(0xF);
        fade_screen_to_color(0, 0xFFFFFF, 30);
        if ((scen_info.unk34[scen_info.nextWorld >> 5] & (1 << (scen_info.nextWorld & 0x1F))) == 0)
        {
            scen_info.unk34[scen_info.nextWorld >> 5] |= 1 << (scen_info.nextWorld & 0x1F);
            scen_info.flags |= 0x40;
        }
        g_check_if_game_over_or_after_credits_sequence(scen_info.nextWorld, &end);
        switch (end.unk3)
        {
        case 0:
            scen_info.unk2 = 8;
            break;
        case 1:
            scen_info.unk2 = 9;
            break;
        case 2:
            scen_info.unk2 = 0xE;
            break;
        case 3:
            scen_info.unk2 = 0x10;
            break;
        case 4:
            scen_info.unk2 = 0x19;
            break;
        }
    }
}

void dmd_scen_sel_world_init(void)
{
    scen_info.unk2 = 10;
    scen_info.world = 0;
}

void dmd_scen_sel_world_next(void)
{
    scen_info.unk2 = 10;
    scen_info.world++;
    scen_info.unk6 = 0;
}

void dmd_scen_sel_floor_init(void)
{
    scen_info.unk2 = 0xB;
    scen_info.flags |= 2;
    dmd_scen_sel_floor_init_child(0);
}

void dmd_scen_sel_floor_main(void)
{
    int i;
    int count;

    switch (fn_2_FED8())
    {
    case 2:
        scen_info.unk2 = 0xC;
        scen_info.flags &= ~2;
        break;
    case 3:
        scen_info.unk2 = 8;
        scen_info.flags &= ~2;
        break;
    }
    if ((dip_switches & 1) != 0)
    {
        if (((pad_status_groups[0].unk0[2].button & PAD_BUTTON_X) && (pad_status_groups[0].unk0[0].button & PAD_BUTTON_Y))
         || ((pad_status_groups[0].unk0[0].button & PAD_BUTTON_X) && (pad_status_groups[0].unk0[2].button & PAD_BUTTON_Y)))
        {
            count = get_world_stage_count(scen_info.world);
            for (i = 0; i < count; i++)
            {
                if (is_storymode_stage_beaten(scen_info.world, i) == 0)
                {
                    fn_1_A4F58(scen_info.world, i);
                    mode_info.storyStage = (s16)i;
                    g_handle_goal();
                }
            }
            fn_1_16C720();
            g_preload_ape_model_for_stageselect();
            scen_info.unk2 = 1;
            scen_info.flags &= ~2;
            fn_2_FE94();
            dest_all_sprites();
        }
    }
}

void dmd_scen_game_init(void)
{
    scen_info.unk2 = 0xD;
    modeCtrl.submodeTimer = 30;
    fade_screen_to_color(0x101, 0xFFFFFF, modeCtrl.submodeTimer);
    mode_info.storyWorld = scen_info.world;
    mode_info.storyStage = scen_info.unk6;
}

void dmd_scen_game_main(void)
{
    modeCtrl.submodeTimer--;
    if (modeCtrl.submodeTimer < 0)
    {
        main_mode_request = 2;
        sub_mode_request = 0x3C;
        g_another_stage_id = (s16)get_story_mode_stage_id(scen_info.world, scen_info.unk6);
        fn_2_FE94();
        fn_2_A9EC();
        fn_2_AD74();
        unlock_storymode_stage(scen_info.world, scen_info.unk6);
        g_save_storymode_progress(NULL);
    }
}

void dmd_scen_game_clear_init(void)
{
    scen_info.unk2 = 0xF;
}

void dmd_scen_game_clear_main(void)
{
    scen_info.flags |= 0x10;
    sub_mode_request = 0x53;
}

void dmd_scen_nameentry_init(void)
{
    g_some_gift_menu_flags |= 4;
    scen_info.unk2 = 0x1A;
}

void dmd_scen_nameentry_main(void)
{
    sub_mode_request = 0x3F;
}

void dmd_scen_game_over_init(void)
{
    scen_info.unk2 = 0x11;
}

void dmd_scen_game_over_main(void)
{
    main_mode_request = 2;
    sub_mode_request = 0x3D;
}

void dmd_scen_entry_init(void)
{
    int group;

    scen_info.unk2 = 0x15;
    scen_info.unk30 = 0;
    g_related_to_pause_menu_input[1] |= 2;
    set_dest_func(g_some_storymode_mode_func);
    dest_all_events();
    load_stage(0xC9);
    event_init(EVENT_SOUND);
    event_init(EVENT_CAMERA);
    event_init(EVENT_SPRITE);
    event_init(EVENT_ITEM);
    event_init(EVENT_REND_EFC);
    event_init(EVENT_STOBJ);
    event_init(EVENT_EFFECT);
    event_init(EVENT_STAGE);
    event_init(EVENT_BACKGROUND);
    enable_camera(0);
    camera_set_state(0x52);
    load_bmp_by_id(0x18);
    create_story_file_nameentry_sprite();
    group = g_swapDiscQueueGroup(6);
    g_preload_next_stage_files(0, -1, 0);
    g_swapDiscQueueGroup(group);
    fade_screen_to_color(0x100, 0xFFFFFF, 30);
}

void dmd_scen_entry_main(void)
{
    struct StorySave *save;

    dmd_scen_entry_main_child();
    switch (fn_2_1248C())
    {
    case 1:
        save = get_current_storymode_save_file();
        save->flags |= 1;
        g_get_story_file_name(save->name);
        fn_2_12468();
        scen_info.unk2 = 2;
        break;
    case 2:
        disc_queue_status(6);
        scen_info.unk2 = 4;
        break;
    }
}

void g_some_storymode_mode_func(void)
{
    switch (scen_info.unk0)
    {
    case 0x14:
    case 0x15:
        set_dest_func(g_some_storymode_mode_func);
        break;
    default:
        g_related_to_pause_menu_input[1] &= ~2;
        break;
    }
}

struct StorySave *get_current_storymode_save_file(void)
{
    return &storymode_save_files[scen_info.saveSlot];
}
