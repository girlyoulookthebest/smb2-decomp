/* Main menu mode: persistent menu settings and the sel_stage/sel_ngc loaders. */
#include "types.h"
#include "camera.h"
#include "input.h"
#include "libc.h"
#include "mode.h"
#include "sel.h"

struct SelMenuState
{
    /*0x00*/ u8 unk0[0x58];
    /*0x58*/ u8 unk58;
    /*0x59*/ u8 unk59;
    /*0x5A*/ u8 unk5A;
    /*0x5B*/ u8 unk5B;
    /*0x5C*/ u32 unk5C;
    u8 filler60[4];
}; /* size = 0x64 */

struct SelStorySettings
{
    /*0x00*/ u8 unk0;
    /*0x01*/ u8 unk1[2];
    /*0x03*/ u8 unk3[2][10];
    /*0x17*/ u8 unk17;
}; /* size = 0x18 */

struct SelSettings8
{
    /*0x00*/ u8 unk0;
    /*0x01*/ u8 unk1[6];
    /*0x08*/ u32 unk8;
}; /* size = 0xC */

struct SelSettings9
{
    /*0x00*/ u32 unk0;
    /*0x04*/ u8 unk4;
    /*0x05*/ u8 unk5[3];
    /*0x08*/ u8 unk8[4];
}; /* size = 0xC */

struct SelSettingsA
{
    /*0x00*/ u8 unk0;
    /*0x01*/ u8 unk1;
    /*0x04*/ u32 unk4;
}; /* size = 0x8 */

struct SelSettingsB
{
    /*0x00*/ u8 unk0;
    /*0x01*/ u8 unk1[5];
    /*0x06*/ u8 unk6;
    /*0x07*/ u8 unk7;
    /*0x08*/ u8 unk8;
    /*0x09*/ u8 unk9[4];
}; /* size = 0x10 */

struct SelSettingsC
{
    /*0x00*/ u8 unk0;
    /*0x01*/ s8 unk1;
}; /* size = 0x4 */

struct SelSettingsD
{
    /*0x00*/ u8 unk0;
    /*0x01*/ u8 unk1;
}; /* size = 0x4 */

struct SelSettingsE
{
    /*0x00*/ u8 unk0;
    /*0x01*/ u8 unk1[3];
    /*0x04*/ s16 unk4;
}; /* size = 0x8 */

struct SelSettingsF
{
    /*0x00*/ u32 unk0;
    u8 filler4[4];
}; /* size = 0x8 */

struct SelSettingsG
{
    /*0x00*/ u8 unk0;
    /*0x01*/ u8 unk1;
    /*0x02*/ u8 unk2;
    /*0x04*/ u32 unk4;
}; /* size = 0x8 */

struct SelSettingsH
{
    /*0x00*/ u8 unk0;
    /*0x01*/ u8 unk1;
    /*0x02*/ u8 unk2;
    /*0x03*/ u8 unk3;
    /*0x04*/ u8 unk4;
    /*0x05*/ u8 unk5;
    /*0x06*/ u8 unk6;
    /*0x08*/ u32 unk8;
}; /* size = 0xC */

struct SelSettingsI
{
    /*0x00*/ u8 unk0;
    /*0x01*/ u8 unk1;
    /*0x02*/ u8 unk2;
    /*0x03*/ u8 unk3;
    /*0x04*/ u8 unk4;
    /*0x05*/ u8 unk5[5];
    /*0x0A*/ u8 unkA;
}; /* size = 0xC */

struct SelStageChoice
{
    /*0x00*/ u32 unk0;
    /*0x04*/ u32 unk4;
}; /* size = 0x8 */

extern u8 storymode_save_files[];
extern u8 lbl_1_data_65970[];
extern s32 main_mode_request;
extern s32 sub_mode_request;
extern void (*sub_mode_funcs[])(void);
extern void (*g_some_menu_func_1)(void);

s8 get_num_unlocked_monkeys(void);
void g_something_calls_sndPopGroup(void);
void debug_textdraw_set_pos(int x, int y);
void debug_textdraw_vsprintf_buf_1(char *fmt, ...);
void disc_queue_status(int group);
int g_swapDiscQueueGroup(int group);
void queue_stage_load(int stageId);
void fn_1_73900(u32 a);
void load_stage(int stage);
void load_additional_rel(char *name, void *rel);
void g_purge_rel(void *rel);
void set_sub_mode_destination(void (*func)(void));
void dest_all_events(void);
void g_something_with_iteratively_freeing_memory(void);
void g_something_with_sound7_and_game_heaps(int group);
void g_sel_ngc_sub_mode_destination(void);

const s16 lbl_1_rodata_5F8[] =
{
    0x83, 0x8F, 0x9B, 0xA0, 0x9F, 0xA1, 0x8A, 0x79, 0x98, 0x66, 0x15F, 0x161,
};

u8 lbl_1_data_6B70[6] = { 5, 5, 4, 3, 2, 2 };

struct SelMenuState g_focused_root_menu;
struct SelMenuInfo sel_menu_info;
s8 number_of_starting_monkeys;
struct SelStorySettings stageselect_is_storymode;
struct SelSettings8 lbl_1_bss_16D0;
struct SelSettings9 lbl_1_bss_16DC;
struct SelSettingsA lbl_1_bss_16E8;
struct SelSettingsB lbl_1_bss_16F0;
struct SelSettingsC lbl_1_bss_1700;
struct SelSettingsD lbl_1_bss_1704;
struct SelSettingsE lbl_1_bss_1708;
struct SelSettingsF g_auto_reload_setting;
struct SelSettingsG lbl_1_bss_1718;
struct SelSettingsH lbl_1_bss_1720;
struct SelSettingsH lbl_1_bss_172C;
struct SelSettingsI lbl_1_bss_1738;
struct SelStageChoice lbl_1_bss_1744;
void (*menu_tick_func)(void);
void (*menu_draw_func)(void);
#ifdef __MWERKS__
u8 g_some_sel_ngc_rel_buffer[8];
#else
/* PC: holds a struct RelInfo (relocation.c), two native pointers: 16 bytes
 * on x64, where [8] let load_additional_rel write past it */
u8 g_some_sel_ngc_rel_buffer[2 * sizeof(void *)] __attribute__((aligned(8)));
#endif
u8 lbl_1_bss_175C[0xC];

void g_set_lots_of_initial_state(void)
{
    int i;
    int j;

    for (i = 0; i < 0x58; i++)
        g_focused_root_menu.unk0[i] = 0;
    g_focused_root_menu.unk58 = 0;
    g_focused_root_menu.unk59 = 1;
    g_focused_root_menu.unk5A = 2;
    g_focused_root_menu.unk5B = 3;
    g_focused_root_menu.unk5C = 0;

    sel_menu_info.menuStackPtr = 0;
    sel_menu_info.unkB = 1;
    sel_menu_info.unkC = 1;
    sel_menu_info.unkE = -1;

    number_of_starting_monkeys = (s8)get_num_unlocked_monkeys();

    stageselect_is_storymode.unk0 = 1;
    for (i = 0; i < 2; i++)
    {
        stageselect_is_storymode.unk1[i] = 0;
        for (j = 0; j < 10; j++)
            stageselect_is_storymode.unk3[i][j] = 0;
    }
    stageselect_is_storymode.unk17 = 0;

    lbl_1_bss_16D0.unk0 = 0;
    for (i = 0; i < 6; i++)
        lbl_1_bss_16D0.unk1[i] = lbl_1_data_6B70[i];
    lbl_1_bss_16D0.unk8 = 7;

    lbl_1_bss_16DC.unk0 = 0;
    lbl_1_bss_16DC.unk4 = 3;
    for (i = 0; i < 3; i++)
        lbl_1_bss_16DC.unk5[i] = 0;
    for (i = 0; i < 4; i++)
        lbl_1_bss_16DC.unk8[i] = 2;

    lbl_1_bss_16E8.unk0 = 6;
    lbl_1_bss_16E8.unk1 = 1;
    lbl_1_bss_16E8.unk4 = 6;

    lbl_1_bss_16F0.unk0 = 3;
    lbl_1_bss_16F0.unk6 = 0;
    lbl_1_bss_16F0.unk7 = 0;
    lbl_1_bss_16F0.unk8 = 1;
    for (i = 0; i < 5; i++)
        lbl_1_bss_16F0.unk1[i] = 2;
    for (i = 0; i < 4; i++)
        lbl_1_bss_16F0.unk9[i] = 0;

    lbl_1_bss_1700.unk0 = 0;
    lbl_1_bss_1700.unk1 = -1;

    lbl_1_bss_1704.unk0 = 0;
    lbl_1_bss_1704.unk1 = 0;

    lbl_1_bss_1708.unk0 = 0;
    for (i = 0; i < 3; i++)
        lbl_1_bss_1708.unk1[i] = lbl_1_data_65970[i];
    lbl_1_bss_1708.unk4 = 14;

    g_auto_reload_setting.unk0 = 0;

    lbl_1_bss_1718.unk0 = 0;
    lbl_1_bss_1718.unk1 = 0;
    lbl_1_bss_1718.unk2 = 1;
    lbl_1_bss_1718.unk4 = 1;

    lbl_1_bss_1720.unk0 = 0;
    lbl_1_bss_1720.unk1 = 0;
    lbl_1_bss_1720.unk2 = 0;
    lbl_1_bss_1720.unk3 = 2;
    lbl_1_bss_1720.unk5 = 2;
    lbl_1_bss_1720.unk4 = 1;
    lbl_1_bss_1720.unk8 = 0;

    lbl_1_bss_172C.unk0 = 0;
    lbl_1_bss_172C.unk1 = 0;
    lbl_1_bss_172C.unk2 = 3;
    lbl_1_bss_172C.unk3 = 0;
    lbl_1_bss_172C.unk5 = 9;
    lbl_1_bss_172C.unk6 = 2;
    lbl_1_bss_172C.unk4 = 0;
    lbl_1_bss_172C.unk8 = 1;

    lbl_1_bss_1738.unk0 = 0;
    lbl_1_bss_1738.unk1 = 0;
    lbl_1_bss_1738.unk2 = 0;
    lbl_1_bss_1738.unk3 = 4;
    lbl_1_bss_1738.unk4 = 3;
    for (i = 0; i < 5; i++)
        lbl_1_bss_1738.unk5[i] = 2;
    lbl_1_bss_1738.unkA = 0;
}

void fn_1_125C0(void)
{
    memcpy_handler(storymode_save_files + 0x18C, &g_focused_root_menu, 0x64);
    memcpy_handler(storymode_save_files + 0x1F0, &stageselect_is_storymode, 0x18);
    memcpy_handler(storymode_save_files + 0x208, &lbl_1_bss_16D0, 0xC);
    memcpy_handler(storymode_save_files + 0x214, &lbl_1_bss_16DC, 0xC);
    memcpy_handler(storymode_save_files + 0x220, &lbl_1_bss_16E8, 8);
    memcpy_handler(storymode_save_files + 0x228, &lbl_1_bss_16F0, 0xD);
    memcpy_handler(storymode_save_files + 0x235, &lbl_1_bss_1700, 2);
    memcpy_handler(storymode_save_files + 0x237, &lbl_1_bss_1704, 2);
    memcpy_handler(storymode_save_files + 0x23A, &lbl_1_bss_1708, 6);
    memcpy_handler(storymode_save_files + 0x240, &g_auto_reload_setting, 8);
    memcpy_handler(storymode_save_files + 0x248, &lbl_1_bss_1718, 8);
    memcpy_handler(storymode_save_files + 0x250, &lbl_1_bss_1720, 0xC);
    memcpy_handler(storymode_save_files + 0x25C, &lbl_1_bss_172C, 0xC);
    memcpy_handler(storymode_save_files + 0x268, &lbl_1_bss_1738, 0xB);
}

void fn_1_1273C(void)
{
    memcpy_handler(&g_focused_root_menu, storymode_save_files + 0x18C, 0x64);
    memcpy_handler(&stageselect_is_storymode, storymode_save_files + 0x1F0, 0x18);
    memcpy_handler(&lbl_1_bss_16D0, storymode_save_files + 0x208, 0xC);
    memcpy_handler(&lbl_1_bss_16DC, storymode_save_files + 0x214, 0xC);
    memcpy_handler(&lbl_1_bss_16E8, storymode_save_files + 0x220, 8);
    memcpy_handler(&lbl_1_bss_16F0, storymode_save_files + 0x228, 0xD);
    memcpy_handler(&lbl_1_bss_1700, storymode_save_files + 0x235, 2);
    memcpy_handler(&lbl_1_bss_1704, storymode_save_files + 0x237, 2);
    memcpy_handler(&lbl_1_bss_1708, storymode_save_files + 0x23A, 6);
    memcpy_handler(&g_auto_reload_setting, storymode_save_files + 0x240, 8);
    memcpy_handler(&lbl_1_bss_1718, storymode_save_files + 0x248, 8);
    memcpy_handler(&lbl_1_bss_1720, storymode_save_files + 0x250, 0xC);
    memcpy_handler(&lbl_1_bss_172C, storymode_save_files + 0x25C, 0xC);
    memcpy_handler(&lbl_1_bss_1738, storymode_save_files + 0x268, 0xB);
}

void md_sel_func(void)
{
    if (sub_mode <= 0x1C || sub_mode >= 0x2E)
    {
        debug_textdraw_set_pos(10, 10);
        debug_textdraw_vsprintf_buf_1("sub_mode: error %d in Sel", sub_mode);
        return;
    }
    sub_mode_funcs[sub_mode]();
}

void fn_1_12950(void)
{
    lbl_1_bss_1744.unk0 = 0;
    lbl_1_bss_1744.unk4 = 1;
}

void smd_sel_ngc_dest(void)
{
    main_mode_request = 3;
    sub_mode_request = 0x72;
}

void g_return_to_sel_ngc(int a)
{
    switch (a)
    {
    case 1:
    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
    case 8:
    case 9:
    case 10:
    case 12:
    case 13:
    case 15:
    case 16:
        modeCtrl.courseFlags |= 0x10000;
        break;
    case -1:
    case 0:
    case 2:
    case 11:
    case 14:
        break;
    }

    if (modeCtrl.gameType == 1)
        g_something_calls_sndPopGroup();

    main_mode_request = 1;
    sub_mode_request = 0x21;
}

void g_load_stage_for_menu_bg(s8 a, int b)
{
    s16 stageId = 3;
    int group;

    if (a == -2)
        stageId = 0xC9;
    else if (a != -1)
        stageId = lbl_1_rodata_5F8[a];

    if (b != 0)
    {
        disc_queue_status(3);
        group = g_swapDiscQueueGroup(3);
        queue_stage_load(stageId);
        g_swapDiscQueueGroup(group);
    }
    else
    {
        fn_1_73900(stageId);
        load_stage(stageId);
        cameras[0].timerCurr = 1;
    }
    sel_menu_info.unkE = stageId;
}

void g_set_some_sel_ngc_global_func_ptrs(void (*a)(void), void (*b)(void), void (*c)(void), void (*d)(void))
{
    menu_tick_func = a;
    menu_draw_func = b;
    g_some_menu_func_1 = c;
    g_minigame_camera_func = d;
}

char *lbl_1_data_6C08[] =
{
    "",
    "mkb2.sel_stage.rel",
    "",
    "mkb2.sel_ngc.rel",
    "",
    "mkb2.sel_ngc.rel",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
};

void smd_sel_ngc_init(void)
{
    dest_all_events();
    g_something_with_iteratively_freeing_memory();
    if (sub_mode == 0x1D || sub_mode == 0x1E)
        g_something_with_sound7_and_game_heaps(0x11);
    else
        g_something_with_sound7_and_game_heaps(0xB);
    load_additional_rel(lbl_1_data_6C08[sub_mode - 0x1C], &g_some_sel_ngc_rel_buffer);
    set_sub_mode_destination(g_sel_ngc_sub_mode_destination);
}

void smd_sel_ngc_tick(void)
{
    if (menu_tick_func != NULL)
        menu_tick_func();
}

void g_sel_draw_func_handler(void)
{
    if (menu_draw_func != NULL)
        menu_draw_func();
}

void g_sel_ngc_sub_mode_destination(void)
{
    int i;
    int pad;

    switch (main_mode)
    {
    case 1:
        set_sub_mode_destination(g_sel_ngc_sub_mode_destination);
        break;
    default:
        menu_tick_func = NULL;
        menu_draw_func = NULL;
        g_some_menu_func_1 = NULL;
        g_minigame_camera_func = NULL;
        g_purge_rel(&g_some_sel_ngc_rel_buffer);
        for (i = 0; i < 4; i++)
        {
            if (player_pad_map[i] != -1)
                break;
        }
        if (i == 4)
        {
            i = 0;
            for (pad = 0; pad < 4; pad++)
            {
                if (pad_status_groups[pad].unk0[0].err == 0)
                {
                    player_pad_map[i] = pad;
                    i++;
                }
            }
        }
        dest_all_events();
        g_something_with_iteratively_freeing_memory();
        g_something_with_sound7_and_game_heaps(0);
        break;
    }
}
