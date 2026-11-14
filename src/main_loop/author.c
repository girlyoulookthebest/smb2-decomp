/* Author mode: the cutscene viewer and the cutscene playback sub modes. */
#include "types.h"
#include "camera.h"
#include "dip.h"
#include "event.h"
#include "gx.h"
#include "input.h"
#include "libc.h"
#include "mode.h"
#include "scen.h"

#define PAD_BUTTON_START 0x1000

/* Things to load or set up at a given point of a cutscene. */
enum
{
    PRELOAD_FILE = 1,
    PRELOAD_APE,
    PRELOAD_MAL,
    PRELOAD_CHARA_LOD,
    PRELOAD_STAGE_QUEUE,
    PRELOAD_STAGE,
    PRELOAD_CLEAR,
    PRELOAD_BG_COLOR,
};

struct AuthorScene
{
    /*0x00*/ char *name;
    /*0x04*/ int sceneFiles;
    /*0x08*/ s8 *scenes;
    /*0x0C*/ s8 sceneCount;
}; /* size = 0x10 */

struct AuthorPreload
{
    /*0x00*/ int scene;  /* -1 = before playback, 0x7FFF = after */
    /*0x04*/ int frame;
    /*0x08*/ u32 type;
    /*0x0C*/ uptr arg1;  /* PRELOAD_FILE: char * (u32 on GC) */
    /*0x10*/ u32 arg2;
    /*0x14*/ u32 arg3;
}; /* size = 0x18 */

struct AuthorPreloadList
{
    /*0x00*/ s8 count;
    /*0x04*/ struct AuthorPreload *items;
}; /* size = 0x8 */

struct Author
{
    /*0x00*/ u32 flags;
    /*0x04*/ u32 id;
    /*0x08*/ int scene;
    /*0x0C*/ int mainMode;
    /*0x10*/ int subMode;
    /*0x14*/ void (*initFunc)(void);
    /*0x18*/ void (*tickFunc)(void);
    /*0x1C*/ void (*destFunc)(void);
    /*0x20*/ void (*drawFunc)(void);
    u8 unk24[4];
}; /* size = 0x28 */

/* fn_1_16A9D0 clears everything up to unk24. */
#ifdef __MWERKS__
#define AUTHOR_CLEAR_SIZE 0x24
#else
#include <stddef.h>
#define AUTHOR_CLEAR_SIZE offsetof(struct Author, unk24)
#endif

struct AuthorCutsceneTimer
{
    /*0x00*/ int timer;
    /*0x04*/ u32 flags;
}; /* size = 0x8 */

extern u8 storymode_save_files[];
extern s32 active_monkey_id[];
extern void *efcmdlGma;
extern void (*sub_mode_funcs[])(void);
extern s32 sub_mode_request;
extern s32 main_mode_request;

void dest_all_events(void);
void fn_1_36140(void);
void fn_1_34828(void);
void fn_1_D3930(void);
void debug_textdraw_set_pos(int x, int y);
void debug_textdraw_vsprintf_buf_1(char *fmt, ...);
void debug_textdraw_put_buf_1(char *str);
void debug_textdraw_set_color(u8 color);
void g_something_with_sound7_and_game_heaps(int group);
void fn_1_141288(void);
void load_scene_files(int category);
void fn_1_142ECC(void);
void g_load_efcmdl_gmatpl(void);
void load_stage(int stage);
void event_init(int id);
void get_load_group_status(void);
void fn_1_141370(int sceneId, int frame);
void set_sub_mode_destination(void (*func)(void));
void g_something_freeing_heap_4(void);
void fn_1_141E48(void);
void fade_screen_to_color(s32 type, u32 color, int duration);
void g_change_music_volume(int track, u32 time, s8 volume);
void g_return_to_sel_ngc(int a);
void fn_1_142F00(void);
void g_something_with_cutscenes_func2(void);
void g_something_freeing_heap_parent(void);
BOOL fn_1_D36C8(void);
void fn_1_3E100(int a, int b);
int g_get_debug_level_select_loading_left_asterisks(void);
void fn_1_1439F4(void);
int g_is_movie_unlocked(int id);
void g_set_movie_as_unlocked(int id);
int g_swapDiscQueueGroup(int group);
void queue_stage_load(int stageId);
void queue_bmp_by_id_read(int grpId);
void preload_ape(int chara, int lod, int mesh, int scene);
void fn_1_EFFD4(int file);
void queue_disc_read(char *file);
void g_load_mal_files_from_disc(int chara, int lod, int mesh, int scene);
void g_something_freeing_chara_heap_4(int chara, int lod, int mesh);
#ifdef __MWERKS__
void fn_1_4E6FC(GXColor color);
#else
/* pc_port: MWCC passes structs by invisible reference, so this by-value
 * declaration and ball_draw.c's GXColor* definition agree on GC; on x86
 * they do not, so route the call through the address. */
void fn_1_4E6FC(GXColor *color);
#define fn_1_4E6FC(c) fn_1_4E6FC(&(c))
#endif
void load_efcmdl_files_from_disc(void);

void g_print_author_debug_data(void);
void smd_author_play_tick(void);
void fn_1_16A208(void);
void fn_1_16A4C0(void);
void fn_1_16A680(void);
void fn_1_16A9D0(void);
void fn_1_16AA00(int id);
void g_assign_new_main_and_sub_mode_for_play_tick(int mainMode, int subMode);
void fn_1_16AA48(void);
void fn_1_16AA68(void);
void fn_1_16AA88(void (*func)(void));
void g_set_some_author_tick_func(void (*func)(void));
void fn_1_16AAA8(void (*func)(void));
void fn_1_16AAB8(void (*func)(void));
void fn_1_16AAC8(void);
void fn_1_16AAE8(void);
int g_get_some_author_related_data(void);
int g_get_author_scene(void);
int g_get_author_scene_max(void);
int g_get_author_frame(void);
int g_get_author_frame_max(void);
void fn_1_16AB78(void);
void g_some_author_cutscene_related_func(void);
void fn_1_16ADD4(void);
void fn_1_16ADD8(void);
void g_load_stageselect_after_cutscene(void);
void g_preload_next_stage_files(int id, int scene, int frame);

#include "author_scenes.inc"

#define AUTHOR_SCENE_COUNT (sizeof(authorScenes) / sizeof(authorScenes[0]))

struct Author lbl_1_bss_DD980;
struct AuthorCutsceneTimer g_some_author_cutscene_timer;

static inline struct ExternalCameraPose *get_camera_pose(void)
{
    return &lbl_1_bss_DD4F4;
}

void smd_author_select_init(void)
{
    dest_all_events();
    fn_1_36140();
    modeCtrl.unk10 = 0;
    sub_mode_request = 0xF6;
}

void smd_author_select_tick(void)
{
    int sel = modeCtrl.unk10;

    if ((pad_status_groups[0].unk0[4].button & PAD_BUTTON_UP) || (analog_inputs[0][4] & PAD_BUTTON_UP)
     || (((pad_status_groups[0].unk0[0].button & PAD_BUTTON_UP) || (analog_inputs[0][0] & PAD_BUTTON_UP))
      && (analog_inputs[0][0] & PAD_BUTTON_B)))
    {
        if (--sel < 0)
            sel = 30;
    }
    if ((pad_status_groups[0].unk0[4].button & PAD_BUTTON_DOWN) || (analog_inputs[0][4] & PAD_BUTTON_DOWN)
     || (((pad_status_groups[0].unk0[0].button & PAD_BUTTON_DOWN) || (analog_inputs[0][0] & PAD_BUTTON_DOWN))
      && (analog_inputs[0][0] & PAD_BUTTON_B)))
    {
        if (++sel >= AUTHOR_SCENE_COUNT)
            sel = 0;
    }
    if (sel != modeCtrl.unk10)
    {
        fn_1_D3930();
        g_preload_next_stage_files(sel, -1, 0);
        modeCtrl.unk10 = sel;
    }
    if (pad_status_groups[0].unk0[2].button & PAD_BUTTON_A)
    {
        fn_1_16A9D0();
        fn_1_16AA00(modeCtrl.unk10);
        g_assign_new_main_and_sub_mode_for_play_tick(6, 0xF5);
        g_set_some_author_tick_func(g_print_author_debug_data);
        sub_mode_request = 0xF7;
    }
    if (pad_status_groups[0].unk0[2].button & PAD_BUTTON_Y)
    {
        fn_1_16A9D0();
        fn_1_16AA00(modeCtrl.unk10);
        fn_1_16AA48();
        g_assign_new_main_and_sub_mode_for_play_tick(6, 0xF5);
        g_set_some_author_tick_func(g_print_author_debug_data);
        sub_mode_request = 0xF7;
    }
    if (pad_status_groups[0].unk0[2].button & PAD_BUTTON_X)
    {
        fn_1_16A9D0();
        fn_1_16AA00(modeCtrl.unk10);
        fn_1_16AA68();
        g_assign_new_main_and_sub_mode_for_play_tick(6, 0xF5);
        g_set_some_author_tick_func(g_print_author_debug_data);
        sub_mode_request = 0xF7;
    }
}

void author_select_draw(void)
{
    struct AuthorScene *entry = authorScenes;
    int i;

    for (i = 0; i < AUTHOR_SCENE_COUNT; i++, entry++)
    {
        debug_textdraw_set_pos(10, i + 4);
        if (i == modeCtrl.unk10)
        {
            if (fn_1_D36C8())
            {
                fn_1_3E100(-5, 0);
                debug_textdraw_vsprintf_buf_1("%2d   ", g_get_debug_level_select_loading_left_asterisks());
            }
            fn_1_3E100(-2, 0);
            debug_textdraw_set_color(0xC3);
            debug_textdraw_put_buf_1("\034 ");
            debug_textdraw_set_color(0xFF);
        }
        debug_textdraw_put_buf_1(entry->name);
    }
}

void g_print_author_debug_data(void)
{
    if (g_some_other_flags & 4)
    {
        debug_textdraw_set_pos(2, 2);
        debug_textdraw_vsprintf_buf_1("%s %d\n", authorScenes[g_get_some_author_related_data()].name, g_get_author_scene());
        debug_textdraw_vsprintf_buf_1("Scene: %d\n", g_get_author_scene());
        debug_textdraw_vsprintf_buf_1("  Max: %d\n", g_get_author_scene_max());
        debug_textdraw_vsprintf_buf_1("Frame: %d\n", g_get_author_frame());
        debug_textdraw_vsprintf_buf_1("  Max: %d\n", g_get_author_frame_max());
    }
    if (merged_digital_inputs[2] & PAD_BUTTON_A)
        fn_1_16AAE8();
    if ((merged_digital_inputs[2] & PAD_BUTTON_START) || (merged_digital_inputs[2] & PAD_BUTTON_B))
        fn_1_16AAC8();
}

void smd_author_play_init(void)
{
    struct AuthorScene *entry;

    if (g_some_other_flags & 0xA)
        return;
    modeCtrl.courseFlags |= 0x04000000;
    dest_all_events();
    g_something_with_sound7_and_game_heaps(8);
    entry = &authorScenes[lbl_1_bss_DD980.id];
    lbl_1_bss_DD980.scene = 0;
    fn_1_141288();
    load_scene_files(entry->sceneFiles);
    fn_1_142ECC();
    g_load_efcmdl_gmatpl();
    load_stage(0);
    event_init(EVENT_EFFECT);
    event_init(EVENT_REND_EFC);
    event_init(EVENT_STAGE);
    event_init(EVENT_BACKGROUND);
    event_init(EVENT_SPRITE);
    event_init(EVENT_CAMERA);
    event_init(EVENT_SOUND);
    enable_camera(0);
    camera_set_state(0x54);
    get_load_group_status();
    g_preload_next_stage_files(lbl_1_bss_DD980.id, lbl_1_bss_DD980.scene, -1);
    fn_1_141370(entry->scenes[lbl_1_bss_DD980.scene], 0);
    set_sub_mode_destination(fn_1_16A680);
    sub_mode_request = 0xF8;
    if (lbl_1_bss_DD980.initFunc != NULL)
        lbl_1_bss_DD980.initFunc();
    smd_author_play_tick();
}

void smd_author_play_tick(void)
{
    struct AuthorScene *entry;
    struct ExternalCameraPose *pose;

    if (g_some_other_flags & 0xA)
        return;
    pose = get_camera_pose();
    entry = &authorScenes[lbl_1_bss_DD980.id];
    if ((u16)pose->frame >= pose->frameMax || (lbl_1_bss_DD980.flags & 4))
    {
        lbl_1_bss_DD980.scene++;
        if (lbl_1_bss_DD980.scene < entry->sceneCount)
        {
            printf("================== author.step ( %d ) =====================\n", lbl_1_bss_DD980.scene);
            g_something_freeing_heap_4();
            g_preload_next_stage_files(lbl_1_bss_DD980.id, lbl_1_bss_DD980.scene, -1);
            fn_1_141370(entry->scenes[lbl_1_bss_DD980.scene], 0);
        }
        else if (lbl_1_bss_DD980.flags & 8)
            lbl_1_bss_DD980.flags |= 0x10;
        else if (lbl_1_bss_DD980.flags & 0x20)
            lbl_1_bss_DD980.flags |= 0x40;
        else
            lbl_1_bss_DD980.flags |= 2;
        lbl_1_bss_DD980.flags &= ~4;
    }
    fn_1_141E48();
    g_preload_next_stage_files(lbl_1_bss_DD980.id, lbl_1_bss_DD980.scene, pose->frame);
    if (lbl_1_bss_DD980.flags & 2)
    {
        if (lbl_1_bss_DD980.flags & 1)
        {
            main_mode_request = lbl_1_bss_DD980.mainMode;
            sub_mode_request = lbl_1_bss_DD980.subMode;
        }
        else
            sub_mode_request = 0xF5;
    }
    else if ((lbl_1_bss_DD980.flags & 0x10) || (lbl_1_bss_DD980.flags & 0x40))
        sub_mode_request = 0xF7;
    if (lbl_1_bss_DD980.tickFunc != NULL)
        lbl_1_bss_DD980.tickFunc();
}

void smd_author_play_story_init(void)
{
    fn_1_16AA88(fn_1_16AB78);
    g_set_some_author_tick_func(g_some_author_cutscene_related_func);
    fn_1_16AAA8(fn_1_16ADD4);
    fn_1_16AAB8(fn_1_16ADD8);
    sub_mode_request = 0xF7;
}

void fn_1_16A208(void)
{
    switch (modeCtrl.unk48)
    {
    case 0:
        if (g_get_author_scene() == g_get_author_scene_max() - 1
         && g_get_author_frame() >= g_get_author_frame_max() - 30)
        {
            fade_screen_to_color(0x101, 0xFFFFFF, 30);
            modeCtrl.unk48 = 2;
        }
        else if (merged_digital_inputs[2] & PAD_BUTTON_START)
        {
            modeCtrl.submodeTimer = 60;
            fade_screen_to_color(0x101, 0xFFFFFF, modeCtrl.submodeTimer);
            modeCtrl.unk48++;
        }
        break;
    case 1:
        if (--modeCtrl.submodeTimer <= 0)
        {
            fn_1_16AAC8();
            modeCtrl.unk48++;
        }
        break;
    case 2:
        break;
    }
}

void smd_author_play_ending_init(void)
{
    static s8 beginnerEndings[] = { 15, 16, 17, 18 };
    static s8 advancedEndings[] = { 19, 20, 21, 22 };
    static s8 expertEndings[] = { 23, 24, 25, 26 };
    static s8 masterEndings[] = { 27, 28, 29, 30 };
    int id;
    int chara;

    if (g_some_other_flags & 0xA)
        return;
    fade_screen_to_color(0x100, 0, 30);
    id = 15;
    chara = active_monkey_id[modeCtrl.currPlayer];
    if (modeCtrl.courseFlags & 1)
    {
        if (modeCtrl.courseFlags & 0x10)
            id = masterEndings[chara];
        else
        {
            switch (modeCtrl.difficulty)
            {
            case DIFFICULTY_BEGINNER:
                id = beginnerEndings[chara];
                break;
            case DIFFICULTY_ADVANCED:
                id = advancedEndings[chara];
                break;
            case DIFFICULTY_EXPERT:
                id = expertEndings[chara];
                break;
            }
        }
    }
    fn_1_16A9D0();
    fn_1_16AA00(id);
    g_assign_new_main_and_sub_mode_for_play_tick(2, 0x53);
    g_set_some_author_tick_func(fn_1_16A208);
    modeCtrl.submodeTimer = 0;
    modeCtrl.unk48 = 0;
    main_mode_request = 6;
    sub_mode_request = 0xF7;
}

void fn_1_16A4C0(void)
{
    int scenesLeft = g_get_author_scene_max() - g_get_author_scene();
    int framesLeft = g_get_author_frame_max() - g_get_author_frame();
    int isLast = (scenesLeft == 1);

    if (isLast && framesLeft == 30)
        fade_screen_to_color(0x101, 0, 30);
    if (modeCtrl.submodeTimer == 0)
    {
        if (merged_digital_inputs[2] & (PAD_BUTTON_START | PAD_BUTTON_B))
        {
            modeCtrl.submodeTimer = 120;
            fade_screen_to_color(0x101, 0, 60);
        }
    }
    else
    {
        if (modeCtrl.submodeTimer == 60)
        {
            g_change_music_volume(-1, 60, 0);
            fn_1_34828();
        }
        if (--modeCtrl.submodeTimer <= 0)
            fn_1_16AAC8();
    }
}

void smd_author_play_from_sel(void)
{
    dest_all_events();
    fn_1_36140();
    modeCtrl.submodeTimer = 0;
    fade_screen_to_color(0x100, 0, 60);
    fn_1_16A9D0();
    fn_1_16AA00(modeCtrl.unk10);
    g_assign_new_main_and_sub_mode_for_play_tick(6, 0xFB);
    g_set_some_author_tick_func(fn_1_16A4C0);
    sub_mode_request = 0xF7;
}

void smd_author_play_ret_sel(void)
{
    g_return_to_sel_ngc(-1);
}

void fn_1_16A680(void)
{
    switch (sub_mode)
    {
    case 0xF8:
        set_sub_mode_destination(fn_1_16A680);
        break;
    default:
        fn_1_36140();
        fn_1_34828();
        g_something_freeing_heap_4();
        fn_1_142F00();
        g_something_with_cutscenes_func2();
        g_preload_next_stage_files(lbl_1_bss_DD980.id, 0x7FFF, 0);
        if (lbl_1_bss_DD980.flags & 0x10)
        {
            lbl_1_bss_DD980.id++;
            if (lbl_1_bss_DD980.id >= AUTHOR_SCENE_COUNT)
                lbl_1_bss_DD980.id = 0;
            lbl_1_bss_DD980.flags &= ~0x10;
        }
        else if (lbl_1_bss_DD980.flags & 0x40)
            lbl_1_bss_DD980.flags &= ~0x40;
        if (lbl_1_bss_DD980.destFunc != NULL)
            lbl_1_bss_DD980.destFunc();
        dest_all_events();
        g_something_freeing_heap_parent();
        g_something_with_sound7_and_game_heaps(0);
        modeCtrl.courseFlags &= ~0x04000000;
        break;
    }
}

void md_author_func(void)
{
    if (sub_mode <= 0xF4 || sub_mode >= 0xFF)
    {
        debug_textdraw_set_pos(10, 10);
        debug_textdraw_vsprintf_buf_1("sub_mode: error %d in Author", sub_mode);
    }
    else
        sub_mode_funcs[sub_mode]();
}

void author_draw_func(void)
{
    if (g_some_other_flags & 4)
    {
        debug_textdraw_set_pos(1, 1);
        debug_textdraw_put_buf_1("AUTHOR MODE");
    }
    switch (sub_mode)
    {
    case 0xF5:
    case 0xF6:
        author_select_draw();
        break;
    case 0xF7:
    case 0xF8:
        fn_1_1439F4();
        if (lbl_1_bss_DD980.drawFunc != NULL)
            lbl_1_bss_DD980.drawFunc();
        break;
    }
}

void fn_1_16A9D0(void)
{
    memset(&lbl_1_bss_DD980, 0, AUTHOR_CLEAR_SIZE);
}

void fn_1_16AA00(int id)
{
    lbl_1_bss_DD980.id = id;
}

void g_assign_new_main_and_sub_mode_for_play_tick(int mainMode, int subMode)
{
    lbl_1_bss_DD980.flags |= 1;
    lbl_1_bss_DD980.mainMode = mainMode;
    lbl_1_bss_DD980.subMode = subMode;
}

void fn_1_16AA48(void)
{
    lbl_1_bss_DD980.flags |= 8;
}

void fn_1_16AA68(void)
{
    lbl_1_bss_DD980.flags |= 0x20;
}

void fn_1_16AA88(void (*func)(void))
{
    lbl_1_bss_DD980.initFunc = func;
}

void g_set_some_author_tick_func(void (*func)(void))
{
    lbl_1_bss_DD980.tickFunc = func;
}

void fn_1_16AAA8(void (*func)(void))
{
    lbl_1_bss_DD980.destFunc = func;
}

void fn_1_16AAB8(void (*func)(void))
{
    lbl_1_bss_DD980.drawFunc = func;
}

void fn_1_16AAC8(void)
{
    lbl_1_bss_DD980.flags |= 2;
}

void fn_1_16AAE8(void)
{
    lbl_1_bss_DD980.flags |= 4;
}

int g_get_some_author_related_data(void)
{
    return lbl_1_bss_DD980.id;
}

int g_get_author_scene(void)
{
    return lbl_1_bss_DD980.scene;
}

int g_get_author_scene_max(void)
{
    struct AuthorScene *entry = &authorScenes[lbl_1_bss_DD980.id];

    return entry->sceneCount;
}

int g_get_author_frame(void)
{
    struct ExternalCameraPose *pose = get_camera_pose();

    return pose->frame;
}

int g_get_author_frame_max(void)
{
    struct ExternalCameraPose *pose = get_camera_pose();

    return pose->frameMax;
}

#pragma dont_inline on

void fn_1_16AB78(void)
{
    int id;

    fade_screen_to_color(0x100, 0, 30);
    memset(&g_some_author_cutscene_timer, 0, sizeof(g_some_author_cutscene_timer));
    id = g_get_some_author_related_data();
    if (g_is_movie_unlocked(id))
        g_some_author_cutscene_timer.flags |= 2;
    g_set_movie_as_unlocked(id);
}

void g_some_author_cutscene_related_func(void)
{
    int done = 0;
    int scenesLeft = g_get_author_scene_max() - g_get_author_scene();
    int framesLeft = g_get_author_frame_max() - g_get_author_frame();
    int isLast = (scenesLeft == 1);
    int canSkip = 1;

    if (isLast && !(g_some_author_cutscene_timer.flags & 1))
    {
        g_some_author_cutscene_timer.flags |= 1;
        g_load_stageselect_after_cutscene();
    }
    if (g_some_author_cutscene_timer.timer == 0)
    {
        if (canSkip && (merged_digital_inputs[2] & PAD_BUTTON_START))
            g_some_author_cutscene_timer.timer = 30;
        if (isLast && (float)framesLeft <= 30.0f)
            g_some_author_cutscene_timer.timer = framesLeft;
        if (g_some_author_cutscene_timer.timer != 0)
        {
            fade_screen_to_color(0x101, 0, g_some_author_cutscene_timer.timer);
            if (!(g_some_author_cutscene_timer.flags & 1))
            {
                g_some_author_cutscene_timer.flags |= 1;
                g_load_stageselect_after_cutscene();
            }
        }
    }
    else
    {
        g_some_author_cutscene_timer.timer--;
        if (g_some_author_cutscene_timer.timer == 0)
            done = 1;
    }
    if (done)
        fn_1_16AAC8();
}

#pragma dont_inline off

void fn_1_16ADD4(void) {}

void fn_1_16ADD8(void) {}

void g_load_stageselect_after_cutscene(void)
{
    int group = g_swapDiscQueueGroup(4);

    if (scen_info.stageId >= 0)
        queue_stage_load(scen_info.stageId);
    queue_bmp_by_id_read(15);
    preload_ape(0, 0, 0, 1);
    if (storymode_save_files[0x31B] != 0)
        fn_1_EFFD4(0);
    g_swapDiscQueueGroup(group);
}

u32 lbl_1_data_79E84[] = { 0 };

#include "author_preload.inc"

static inline void set_bg_color(struct AuthorPreload *item)
{
    GXColor color;

    color.a = (u8)((item->arg1 >> 24) & 0xFF);
    color.r = (u8)((item->arg1 >> 16) & 0xFF);
    color.g = (u8)((item->arg1 >> 8) & 0xFF);
    color.b = (u8)(item->arg1 & 0xFF);
    fn_1_4E6FC(color);
}

void g_preload_next_stage_files(int id, int scene, int frame)
{
    struct AuthorPreload *item = preloadLists[id].items;
    int i;

    for (i = 0; i < preloadLists[id].count; i++, item++)
    {
        if (scene == item->scene && (scene == -1 || scene == 0x7FFF || frame == item->frame))
        {
            switch (item->type)
            {
            case PRELOAD_FILE:
                queue_disc_read((char *)item->arg1);
                break;
            case PRELOAD_APE:
                preload_ape(item->arg1, item->arg2, 0, item->arg3);
                break;
            case PRELOAD_MAL:
                g_load_mal_files_from_disc(item->arg1, item->arg2, 0, item->arg3);
                break;
            case PRELOAD_CHARA_LOD:
                g_something_freeing_chara_heap_4(item->arg1, item->arg2, 0);
                break;
            case PRELOAD_STAGE_QUEUE:
                queue_stage_load(item->arg1);
                break;
            case PRELOAD_STAGE:
                load_stage(item->arg1);
                break;
            case PRELOAD_CLEAR:
                fn_1_D3930();
                break;
            case PRELOAD_BG_COLOR:
                set_bg_color(item);
                break;
            }
        }
    }
    if (efcmdlGma == NULL)
        load_efcmdl_files_from_disc();
}
