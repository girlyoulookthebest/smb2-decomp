/* The main_loop REL's entry points: _prolog holds the game loop. */
#include "types.h"
#include "ball.h"
#include "dip.h"
#include "file.h"
#include "gx.h"
#include "light.h"
#include "mode.h"
#include "os.h"
#include "world.h"

struct PauseInputState
{
    u8 filler0[0x18];
    /*0x18*/ s8 screenshotState;
};

extern struct PauseInputState g_related_to_pause_menu_input;
extern uptr g_ord_tbl_stuff[2];
extern int is_more_than_24_mib;
extern uptr lbl_1_bss_131C;
extern uptr lbl_1_bss_1320;
extern uptr g_some_buffer_ptr;
extern int main_heap;
extern u32 main_heap_size;
extern u32 new_arena_hi;
extern u32 old_arena_hi;
extern u32 lbl_80198B48;
extern u32 lbl_80198B4C;
extern u8 lbl_1_bss_A87E0[];
extern u32 g_enable_perf;
extern u32 g_some_perf_timer1[];
extern void *lbl_801988E0;
extern s16 g_some_stage_id;
extern s16 lbl_1_bss_711EC;

void g_very_similar_to_puts(const char *);
int printf(const char *, ...);
u32 *OSGetStackPointer(void);
void g_init_some_stuff(void);
void g_set_some_initial_state(void);
void load_bmp_com(void);
void window_init(void);
void init_events(void);
void g_init_perf_profiling(void);
void init_sound(void);
void g_init_lights(void);
void g_set_some_func_ptr1(void (*)(void));
void g_set_some_card_func_ptr(void (*)(void));
void g_set_some_func_ptr3(void (*)(void));
void set_console_reset_callback(int (*)(void));
void g_something_with_sound6_and_stop_pad_motors_callback(void);
void g_something_with_card_callback(void);
void g_something_with_sound11_callback(void);
int console_reset_callback_impl(void);
void pool_init(void);
void pool_tick(void);
void fn_1_D26BC(void *, int);
void fn_1_D27E4(u32);
void g_something_with_replays5(void);
void init_cameras(void);
void fn_1_5368(void);
void fn_1_CD3CC(u32, int);
#ifndef __MWERKS__
/* PC port: must match ord_tbl.c's definition order. On PPC the mixed
 * GPR/FPR convention makes both orders identical; on x86 they are not. */
void fn_1_CCAA0(float depthOffset, float minDepth, float maxDepth, void *buffer, int maxEntries);
#else
void fn_1_CCAA0(void *, int, float, float, float);
#endif
void fn_1_CCB24(void);
void fn_1_D3A40(void);
void fn_1_D7194(void);
void fn_1_117DB0(void);
void fn_1_117E90(void);
void fn_1_C00(void);
void fn_1_D462C(void);
void g_load_init_common(void);
void event_ape_init(void);
void g_something_with_smb1_ranking_main_game_defaults_wrapper(void);
void g_init_rankings_to_defaults_wrapper(void);
void g_stgname_init(void);
void clear_unlocked_cm_stages(void);
void clear_unlocked_storymode_stages(void);
void clear_unlock_info(void);
void g_set_lots_of_initial_state(void);
long long OSGetTime(void);
void srand(u32);
void g_something_loading_fonts(void);
void g_remake_initial_main_heap_somehow(void);
void SoundGroupLoad(int);
void g_setup_and_create_game_heaps(void);
void g_something_with_fonts(void);
void PERFEventStart(int);
void PERFEventEnd(int);
void g_zero_some_sprite_related_state(void);
void gx_finish_frame_and_reset_z_and_alpha_compare_modes(void);
void g_idle_function(int);
void g_something_with_music_fading(void);
void g_read_inputs_and_maybe_reset_console(void);
void process_inputs(void);
void g_check_debug_mode_input(void);
void load_disc_queue(void);
void mode_tick(void);
void g_something_with_card(void);
void tick_events(void);
void polydisp_main(void);
void init_pausemenu_screenshot_texobj(void *);
void g_draw_pausemenu_screenshot(void *);
void take_pausemenu_screenshot(void *, int, int, int, int, int);
void GXSetViewport(float, float, float, float, float, float);
void g_maybe_save_previews(void);
void g_something_with_replays3(void);
void draw_ui(void);
void g_something_with_graphics_processor2(void);
void g_something_with_GXPeekZ(void);
void g_something_with_video_this_also_finishes_a_frame(void);
void hwExit_wrapper(void);
void g_purge_rel2(void);
void g_something_with_sound6(void);
void event_vibration_dest(void);
int fn_1_F004C(void);
void g_something_with_sound11(void);
void cd_to_dvd_root(void);
void cd_to_dvd_root2(void);
void g_create_initial_main_heap(void);
void g_something_with_card13(void);
void fn_1_10B28C(void);
void unset_all_dip_switches(void);
void fn_1_E7674(void);
void g_something_with_gx_modes_and_compare(void);
void g_init_something_with_heaps(void);
void g_free_some_memory2(void);
void fn_1_DB8(void);
int DVDChangeDir(const char *);
u32 OSGetArenaLo(void);
u32 OSGetArenaHi(void);
void OSSetArenaLo(u32);
u32 OSInitAlloc(u32, u32, int);
int OSCreateHeap(u32, u32);
int OSSetCurrentHeap(int);
u32 OSCheckHeap(int);
u32 g_something_with_locked_cache(int);
void fn_8006ABF0(void);
void fn_80064FE0(u32);
void fn_8006AF40(void);
u32 OSGetConsoleSimulatedMemSize(void);

u32 frame_counter;
u8 lbl_1_bss_34[0x1240 * PC_PTR_SCALE]; /* 0x80 struct Thread */
u32 g_font_char_var_3[4];

u32 lbl_1_data_50[] =
{
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

#define PERF_TIMED(idx, call) \
    do { \
        perf_init_timer(4); \
        call; \
        g_some_perf_timer1[idx] = perf_stop_timer(4); \
    } while (0)

void draw_pause_screenshot(void)
{
    u8 texObj[0x20];

    fn_1_CCB24();
    init_pausemenu_screenshot_texobj(texObj);
    GXSetViewport(0.0f, 0.0f, current_render_mode->fbWidth, current_render_mode->efbHeight, 0.0f, 1.0f);
    GXSetScissor_cached(0, 0, current_render_mode->fbWidth, current_render_mode->efbHeight);
    GXSetZMode_cached(1, 7, 0);
    g_draw_pausemenu_screenshot(texObj);
    GXSetZMode_cached(1, 3, 1);
}

void _prolog(void)
{
    g_very_similar_to_puts("RELOCATION MAIN_LOOP LOADED\n");
    frame_counter = 0;
    g_init_some_stuff();
    g_set_some_initial_state();
    load_bmp_com();
    window_init();
    init_events();
    g_init_perf_profiling();
    init_sound();
    g_init_lights();
    g_set_some_func_ptr1(g_something_with_sound6_and_stop_pad_motors_callback);
    g_set_some_card_func_ptr(g_something_with_card_callback);
    g_set_some_func_ptr3(g_something_with_sound11_callback);
    set_console_reset_callback(console_reset_callback_impl);
    current_ball = balls;
    current_world_info = world_infos;
    pool_init();
    fn_1_D26BC(lbl_1_bss_34, 0x80);
    g_something_with_replays5();
    init_cameras();
    fn_1_5368();
    fn_1_CD3CC(lbl_1_bss_131C, 0x10000);
#ifndef __MWERKS__
    fn_1_CCAA0(0.0f, 0.0f, 400.0f, lbl_1_bss_A87E0, 0x4000);
#else
    fn_1_CCAA0(lbl_1_bss_A87E0, 0x4000, 0.0f, 0.0f, 400.0f);
#endif
    fn_1_D3A40();
    fn_1_D7194();
    fn_1_117DB0();
#ifndef __MWERKS__
    /* PC port: the DOL init chain (g_some_heap_init) that normally runs
     * before this REL is skipped, so load init/common.gma+tpl here. */
    {
        void load_common_gma_tpl(void);
        load_common_gma_tpl();
    }
#endif
    g_load_init_common();
    event_ape_init();
    g_something_with_smb1_ranking_main_game_defaults_wrapper();
    g_init_rankings_to_defaults_wrapper();
    g_stgname_init();
    g_some_stage_id = 1;
    lbl_1_bss_711EC = 0;
    clear_unlocked_cm_stages();
    clear_unlocked_storymode_stages();
    clear_unlock_info();
    g_set_lots_of_initial_state();
    frame_counter++;
    srand(OSGetTime());
    g_something_loading_fonts();
    g_remake_initial_main_heap_somehow();
    SoundGroupLoad(1);
    SoundGroupLoad(15);
    SoundGroupLoad(21);
    g_setup_and_create_game_heaps();
    if (is_more_than_24_mib)
        g_something_with_fonts();

    do
    {
        if (g_enable_perf != 0)
            PERFEventStart(0);
        PERF_TIMED(0, g_zero_some_sprite_related_state(); gx_finish_frame_and_reset_z_and_alpha_compare_modes());
        g_idle_function(0);
        PERF_TIMED(1, g_something_with_music_fading());
        PERF_TIMED(2, g_read_inputs_and_maybe_reset_console(); process_inputs());
        PERF_TIMED(3, g_check_debug_mode_input());
        PERF_TIMED(4, load_disc_queue());
        PERF_TIMED(5, mode_tick());
        if (g_enable_perf != 0)
            PERFEventEnd(0);
        if (g_enable_perf != 0)
            PERFEventStart(1);
        PERF_TIMED(6, g_something_with_card());
        PERF_TIMED(7, pool_tick());
        PERF_TIMED(8, fn_1_D27E4(0xFFFFEF));
        PERF_TIMED(9, tick_events());
        PERF_TIMED(10, fn_1_D27E4(0xFFFFDF));
        if (g_enable_perf != 0)
            PERFEventEnd(1);
        if (g_enable_perf != 0)
            PERFEventStart(2);
        perf_init_timer(4);
        switch (g_related_to_pause_menu_input.screenshotState)
        {
        case 0:
        case 1:
            polydisp_main();
            break;
        case 2:
            draw_pause_screenshot();
            break;
        }
        g_some_perf_timer1[11] = perf_stop_timer(4);
        g_maybe_save_previews();
        PERF_TIMED(14, g_something_with_replays3());
        PERF_TIMED(13, draw_ui());
        switch (g_related_to_pause_menu_input.screenshotState)
        {
        case 1:
            take_pausemenu_screenshot(lbl_801988E0, 0, 0, current_render_mode->fbWidth, current_render_mode->efbHeight, 5);
            break;
        }
        if (g_enable_perf != 0)
            PERFEventEnd(2);
        PERF_TIMED(15, g_something_with_graphics_processor2());
        PERF_TIMED(18, g_something_with_GXPeekZ());
        PERF_TIMED(16, g_something_with_video_this_also_finishes_a_frame());
        PERF_TIMED(17, fn_1_D462C());
        frame_counter++;
        if ((g_some_other_flags & 0xA) == 0)
            g_font_char_var_3[0]++;
    } while (!(modeCtrl.courseFlags & (1 << 21)));
    fn_1_117E90();
    hwExit_wrapper();
    fn_1_C00();
}

void _epilog(void)
{
    g_purge_rel2();
    g_set_some_func_ptr1(NULL);
    g_set_some_func_ptr3(NULL);
    set_console_reset_callback(NULL);
    g_very_similar_to_puts("RELOCATION MAIN_LOOP PURGED\n");
}

void _unresolved(void)
{
    u32 i;
    u32 *sp;

    g_very_similar_to_puts("\nError: A called an unlinked function.\n");
    g_very_similar_to_puts("Address:      Back Chain    LR Save\n");
    i = 0;
    for (sp = OSGetStackPointer(); sp != NULL && (u32)sp != 0xFFFFFFFF && i++ < 16; sp = (u32 *)sp[0])
        printf("0x%08x:   0x%08x    0x%08x\n", sp, sp[0], sp[1]);
    OSPanic("main_loop.c", 431, "\n");
}

u8 lbl_1_data_27C[] = { 0, 0, 1, 1, 2, 3, 2, 3, 5, 4, 6, 0 };

struct ArenaBuffer
{
    u32 *ptr;
    u32 size;
};

struct ArenaBuffer lbl_1_data_288[] =
{
    { &g_some_buffer_ptr, 0x10000 },
    { &lbl_1_bss_1320, 0x1E000 },
    { &lbl_1_bss_131C, 0x10000 },
};

void g_something_with_sound6_and_stop_pad_motors_callback(void)
{
    g_something_with_sound6();
    event_vibration_dest();
}

void g_something_with_card_callback(void)
{
    if (fn_1_F004C() != 0)
        g_something_with_card();
}

void g_something_with_sound11_callback(void)
{
    g_something_with_sound11();
}

void revert_console_reset_callback(void)
{
    set_console_reset_callback(console_reset_callback_impl);
}

int console_reset_callback_impl(void)
{
    event_vibration_dest();
    return fn_1_F004C() == 0;
}

void g_init_some_stuff(void)
{
    cd_to_dvd_root2();
    g_create_initial_main_heap();
    g_something_with_card13();
    fn_1_10B28C();
    unset_all_dip_switches();
    fn_1_E7674();
    g_something_with_gx_modes_and_compare();
    g_init_something_with_heaps();
    lbl_80198B4C = new_arena_hi;
    lbl_80198B48 = old_arena_hi;
}

void fn_1_C00(void)
{
    g_free_some_memory2();
    cd_to_dvd_root();
    fn_1_DB8();
}

void cd_to_dvd_root2(void)
{
    DVDChangeDir("/");
}

void cd_to_dvd_root(void)
{
    DVDChangeDir("/");
}

void g_create_initial_main_heap(void)
{
    u32 lo;
    u32 hi;
    u32 i;

    lo = OSGetArenaLo();
    OSGetArenaHi();
    for (i = 0; i < 3; i++)
    {
        *lbl_1_data_288[i].ptr = lo;
        lo = OSRoundUp32B(lo + lbl_1_data_288[i].size);
    }
    OSSetArenaLo(lo);
    lo = OSGetArenaLo();
    hi = OSGetArenaHi();
    lo = OSInitAlloc(lo, hi, 5);
    lo = OSRoundUp32B(lo);
    main_heap = OSCreateHeap(lo, hi);
    OSSetCurrentHeap(main_heap);
    lo = hi;
    OSSetArenaLo(lo);
    main_heap_size = OSCheckHeap(main_heap);
    g_ord_tbl_stuff[0] = g_something_with_locked_cache(1);
    fn_8006ABF0();
    if (OSGetConsoleSimulatedMemSize() > 0x1800000)
        is_more_than_24_mib = 1;
}

void fn_1_DB8(void)
{
    if (g_ord_tbl_stuff[0] != 0)
    {
        fn_80064FE0(g_ord_tbl_stuff[0]);
        g_ord_tbl_stuff[0] = 0;
    }
    fn_8006AF40();
}

/* mwcc lays these out in reverse, so this keeps the target .bss order */
uptr g_some_buffer_ptr;
uptr lbl_1_bss_1320;
uptr lbl_1_bss_131C;
int is_more_than_24_mib;
uptr g_ord_tbl_stuff[2];
u8 scen_info[0x38 * PC_PTR_SCALE]; /* struct ScenInfo (0x40 on x64) */
u8 lbl_1_bss_12AC[0x2C];
uptr g_init_common_gma_buf;
uptr g_stage_nl_buf[2];
uptr g_some_nl_model_buffer_2;
u32 g_init_common_tpl_buf[3];
uptr lbl_1_bss_1288[2];
uptr g_some_tpl_buffer;
