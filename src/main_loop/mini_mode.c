#pragma peephole off
#include "types.h"
#include "gx.h"
#include "background.h"
#include "camera.h"
#include "dip.h"
#include "input.h"
#include "mode.h"
#include "os.h"
#include "pool.h"

struct MiniRelEntry
{
    u16 subMode;
    u16 heapGroup;
    char *fileName;
};

struct MiniMenuEntry
{
    s8 kind;  /* 0 heading, 1 selectable */
    s8 x;  /* non-positive: relative to the previous entry */
    s8 y;
    char *name;
    char *nameJp;
    u16 subMode;
    s8 gameType;
};

extern void (*sub_mode_funcs[])(void);
extern s32 sub_mode_request;
extern s32 main_mode_request;
extern void *g_some_menu_func_1;
extern u32 g_font_char_var_3;
extern s32 active_monkey_id[4];

void debug_textdraw_set_pos(int x, int y);
void debug_textdraw_set_color(u8 color);
void debug_textdraw_put_buf_1(char *str);
void debug_textdraw_vsprintf_buf_1(char *fmt, ...);
void fn_1_3E100(int a, int b);
int g_is_sjis_and_more_than_24_mib(void);
void set_sub_mode_destination(void (*func)(void));
void dest_all_events(void);
void g_something_with_iteratively_freeing_memory(void);
void g_something_with_sound7_and_game_heaps(int group);
void load_additional_rel(char *name, void *rel);
void g_purge_rel(void *rel);
void fn_1_34828(void);
void fn_1_36140(void);
void g_alloc_memory_for_ending(void);
int fn_1_FD740(void);
void fn_1_FD97C(void);
void g_very_similar_to_puts(char *str);
void g_nameentry2_init(void);
void fn_1_11C02C(void);
void game_nameentry_draw_func(void);
void fn_1_D5FF4(void);
void g_smd_mini_generic_init(void);
void g_smd_mini_generic_tick(void);

/* Nothing here reads this; it is what the .data pool base lands on. */
static u8 lbl_1_data_52D58[0x170] =
{
    0xFE, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A,
    0x0B, 0x0C, 0xFE, 0x0D, 0x0E, 0x0F, 0xFF, 0x00, 0x00, 0x01, 0x02, 0x03,
    0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
    0xFF, 0x00, 0x00, 0x00, 0x31, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x38, 0x00, 0xB4, 0x00, 0x00, 0x01, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00,
    0x00, 0x02, 0x00, 0x00, 0x10, 0x00, 0x4C, 0x00, 0x00, 0x03, 0x00, 0x00,
    0x01, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00,
    0x00, 0x09, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x01, 0x00, 0x00, 0x00, 0x00, 0x0B, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00,
    0x00, 0x0C, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00,
    0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00,
    0x00, 0x07, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00,
    0x38, 0x00, 0xB4, 0x00, 0x00, 0x0D, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x14, 0x00, 0x00,
    0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00,
    0x00, 0x16, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x17, 0x00, 0x00,
    0x18, 0x00, 0x00, 0x00, 0x00, 0x18, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00,
    0x00, 0x0F, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x01, 0x00, 0x00, 0x00, 0x00, 0x11, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00,
    0x00, 0x12, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x13, 0x00, 0x00,
    0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00,
    0x00, 0x01, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00,
    0x18, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00,
    0x00, 0x04, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00,
    0x18, 0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00,
    0x00, 0x07, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00,
    0x18, 0x00, 0x00, 0x00, 0x00, 0x09, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00,
    0x00, 0x0A, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x0B, 0x00, 0x00,
    0x18, 0x00, 0x00, 0x00, 0x00, 0x0C, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00,
    0x00, 0x0D, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x0E, 0x00, 0x00,
    0x18, 0x00, 0x00, 0x00, 0x00, 0x0F, 0x00, 0x00,
};

static s32 lbl_1_bss_CE0B0;
static void (*g_minigame_tick_func)(void);
static void (*g_mini_draw_func_ptr)(void);
static void (*lbl_1_bss_CE0BC)(void);
static u8 lbl_1_bss_CE0C0[8];
static s16 lbl_1_bss_CE0C8;

static void *unused(void)
{
    lbl_1_bss_CE0B0 = 0;
    g_minigame_tick_func = NULL;
    g_mini_draw_func_ptr = NULL;
    lbl_1_bss_CE0BC = NULL;
    lbl_1_bss_CE0C0[0] = 0;
    lbl_1_bss_CE0C8 = 0;
    return lbl_1_data_52D58;
}

void md_mini_func(void)
{
    if (!(sub_mode > 0xA8 && sub_mode < 0xD1))
    {
        debug_textdraw_set_pos(10, 10);
        debug_textdraw_vsprintf_buf_1("sub_mode: error %d in Mini", sub_mode);
        return;
    }
    sub_mode_funcs[sub_mode]();
}

void g_set_minigame_specific_funcs(void (*tick)(void), void (*draw)(void), void *menu, void *camera)
{
    g_minigame_tick_func = tick;
    g_mini_draw_func_ptr = draw;
    lbl_1_bss_CE0BC = NULL;
    g_some_menu_func_1 = menu;
    g_minigame_camera_func = camera;
}

void fn_1_D5D7C(void (*tick)(void), void (*draw)(void), void (*extra)(void), void *menu, void *camera)
{
    g_minigame_tick_func = tick;
    g_mini_draw_func_ptr = draw;
    lbl_1_bss_CE0BC = extra;
    g_some_menu_func_1 = menu;
    g_minigame_camera_func = camera;
}

static const struct MiniRelEntry lbl_1_rodata_64518[] =
{
    { 0xAB, 0x0E, "mkb2.rel_mini_bowling.rel" },
    { 0xAD, 0x02, "mkb2.rel_mini_billiards.rel" },
    { 0xAF, 0x0F, "mkb2.shooting.rel" },
    { 0xB1, 0x01, "mkb2.rel_sample.rel" },
    { 0xB9, 0x03, "mkb2.race2.rel" },
    { 0xBB, 0x10, "mkb2.golf2.rel" },
    { 0xBD, 0x01, "mkb2.mini_fight2.rel" },
    { 0xBF, 0x04, "mkb2.pilot2.rel" },
    { 0xC1, 0x06, "mkb2.boat.rel" },
    { 0xC5, 0x05, "mkb2.rel_mini_futsal.rel" },
    { 0xC7, 0x0D, "mkb2.dogfight.rel" },
    { 0xC9, 0x09, "mkb2.baseball.rel" },
    { 0xCB, 0x0A, "mkb2.tennis.rel" },
    { 0, 0xFFFF, NULL },
};

void g_smd_mini_generic_init(void)
{
    const struct MiniRelEntry *entry = lbl_1_rodata_64518;

    if (lbl_1_bss_CE0B0 != 0)
        OSPanic("mini_mode.c", 0x80, "multiple relocation");
    dest_all_events();
    g_something_with_iteratively_freeing_memory();
    while (entry->fileName != NULL)
    {
        if (sub_mode == entry->subMode)
        {
            g_something_with_sound7_and_game_heaps(entry->heapGroup);
            load_additional_rel(entry->fileName, lbl_1_bss_CE0C0);
            break;
        }
        entry++;
    }
    lbl_1_bss_CE0B0 = 1;
    if (modeCtrl.gameType == 0xF || modeCtrl.gameType == 0x10)
    {
        g_minigame_tick_func = NULL;
        g_mini_draw_func_ptr = NULL;
        lbl_1_bss_CE0BC = NULL;
        g_some_menu_func_1 = NULL;
        g_minigame_camera_func = NULL;
        g_purge_rel(lbl_1_bss_CE0C0);
        g_something_with_sound7_and_game_heaps(0);
        lbl_1_bss_CE0B0 = 0;
        if (pad_status_groups[0].unk0[0].button & PAD_TRIGGER_Z)
        {
            main_mode_request = 0;
            sub_mode_request = 3;
        }
    }
    else
        set_sub_mode_destination(fn_1_D5FF4);
}

void g_smd_mini_generic_tick(void)
{
    if (g_minigame_tick_func != NULL)
        g_minigame_tick_func();
}

void g_mini_draw_func_handler(void)
{
    if (g_mini_draw_func_ptr != NULL)
        g_mini_draw_func_ptr();
}

void fn_1_D5FB0(void)
{
    if (lbl_1_bss_CE0BC != NULL)
        lbl_1_bss_CE0BC();
}

void fn_1_D5FF4(void)
{
    void (*func)(void) = sub_mode_funcs[sub_mode];

    if (func == g_smd_mini_generic_init || func == g_smd_mini_generic_tick)
    {
        set_sub_mode_destination(fn_1_D5FF4);
        return;
    }
    g_minigame_tick_func = NULL;
    g_mini_draw_func_ptr = NULL;
    lbl_1_bss_CE0BC = NULL;
    g_some_menu_func_1 = NULL;
    g_minigame_camera_func = NULL;
    g_purge_rel(lbl_1_bss_CE0C0);
    g_something_with_sound7_and_game_heaps(0);
    lbl_1_bss_CE0B0 = 0;
}

static struct MiniMenuEntry lbl_1_data_531B4[] =
{
    { 0, 4, 4, "         NEW", "        \220V\213K", 0x00, 0 },
    { 1, 20, 0, "BASEBALL", "\203x\201[\203X\203{\201[\203\213", 0xC9, 15 },
    { 1, 20, -2, "TENNIS", "\203e\203j\203X", 0xCB, 16 },
    { 1, 20, -2, "DOGFIGHT", "\203h\203b\203O\203t\203@\203C\203g", 0xC7, 13 },
    { 1, 20, -2, "SHOOTING", "\203V\203\205\201[\203e\203B\203\223\203O", 0xAF, 9 },
    { 1, 20, -2, "FUTSAL", "\203t\203b\203g\203T\203\213", 0xC5, 12 },
    { 0, 6, -3, "     RENEWAL", "\203\212\203j\203\205\201[\203A\203\213", 0x00, 0 },
    { 1, 22, 0, "BILLIARDS", "\203r\203\212\203\204\201[\203h", 0xAD, 6 },
    { 1, 22, -2, "GOLF2", "\203S\203\213\203t\202Q", 0xBB, 8 },
    { 1, 22, -2, "FIGHT2", "\203t\203@\203C\203g\202Q", 0xBD, 4 },
    { 1, 22, -2, "PILOT2", "\203p\203C\203\215\203b\203g\202Q", 0xBF, 5 },
    { 1, 22, -2, "BOWLING", "\203{\203E\203\212\203\223\203O\202Q", 0xAB, 7 },
    { 0, 8, -3, "         ETC", "      \202\273\202\314\221\274", 0x00, 0 },
    { 1, 24, 0, "NAMEENTRY TEST", "\203l\201[\203\200\203G\203\223\203g\203\212", 0xC3, 11 },
    { 1, 24, -2, "ENDING", "\203G\203\223\203f\203B\203\223\203O", 0xB7, -1 },
    { 1, 24, -2, "RANKING", "\203\211\203\223\203L\203\223\203O", 0xB5, -1 },
    { 1, 24, -2, "COMMEND", "\225\\\217\262", 0xB3, -1 },
};

void smd_mini_select_init(void)
{
    int i;

    dest_all_events();
    g_something_with_iteratively_freeing_memory();
    fn_1_34828();
    fn_1_36140();
    i = 0;
    while (lbl_1_data_531B4[i].kind == 0)
        i++;
    lbl_1_bss_CE0C8 = i;
    {
        GXColor color = { 0, 0, 0, 255 };
        GXColor *p = &color;

        world_theme.backdropColor = *p;
    }
    sub_mode_request = 0xAA;
}

void smd_mini_select_tick(void)
{
    struct MiniMenuEntry *entry;
    int i;
    int x;
    int y;

    x = 0;
    y = 0;
    if (!(g_some_other_flags & 0xA))
    {
        i = lbl_1_bss_CE0C8;
        if ((pad_status_groups[0].unk0[4].button & PAD_BUTTON_DOWN)
         || (analog_inputs[0][4] & PAD_BUTTON_DOWN)
         || (((pad_status_groups[0].unk0[0].button & PAD_BUTTON_DOWN) || (analog_inputs[0][0] & PAD_BUTTON_DOWN))
          && (analog_inputs[0][0] & 0x200)))
        {
            do
            {
                if (++i >= (u32)17)
                    i = 0;
            } while (lbl_1_data_531B4[i].kind == 0);
        }
        if ((pad_status_groups[0].unk0[4].button & PAD_BUTTON_UP)
         || (analog_inputs[0][4] & PAD_BUTTON_UP)
         || (((pad_status_groups[0].unk0[0].button & PAD_BUTTON_UP) || (analog_inputs[0][0] & PAD_BUTTON_UP))
          && (analog_inputs[0][0] & 0x200)))
        {
            do
            {
                if (--i <= 0)
                    i = 16;
            } while (lbl_1_data_531B4[i].kind == 0);
        }
        lbl_1_bss_CE0C8 = i;
        if (pad_status_groups[0].unk0[2].button & PAD_BUTTON_A)
        {
            sub_mode_request = lbl_1_data_531B4[i].subMode;
            modeCtrl.gameType = lbl_1_data_531B4[i].gameType;
        }
    }
    debug_textdraw_set_pos(1, 1);
    if (g_is_sjis_and_more_than_24_mib())
        debug_textdraw_put_buf_1("\203~\203j\203\202\201[\203h");
    else
        debug_textdraw_put_buf_1("MINI MODE");
    entry = lbl_1_data_531B4;
    for (i = 0; i < (u32)17; i++, entry++)
    {
        int ex = entry->x;
        int ey = entry->y;

        if (ex <= 0)
            ex = x - ex;
        if (ey <= 0)
            ey = y - ey;
        x = ex;
        y = ey;
        debug_textdraw_set_pos(ex, ey);
        if (i == lbl_1_bss_CE0C8)
        {
            u32 t = g_font_char_var_3 >> 3;
            int c = t % 6;

            if (c > 3)
                c = 7 - c;
            c = ((c & 3) << 2) | (((c & 3) << 6) | ((c & 3) << 4)) | 3;
            debug_textdraw_set_color(c);
            if (g_is_sjis_and_more_than_24_mib())
            {
                fn_1_3E100(-3, 0);
                debug_textdraw_vsprintf_buf_1("\201\237 ");
            }
            else
            {
                fn_1_3E100(-2, 0);
                debug_textdraw_put_buf_1("\034 ");
            }
        }
        if (i == lbl_1_bss_CE0C8)
            debug_textdraw_set_color(0xFF);
        else
            debug_textdraw_set_color(0x57);
        switch (entry->kind)
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

void smd_mini_ending_init(void)
{
    int i;

    sub_mode_request = 0xB8;
    camera_setup_singleplayer_viewport();
    for (i = 0; i < 4; i++)
    {
        g_poolInfo.playerPool.statusList[i] = 2;
        active_monkey_id[i] = i;
        set_camera_view_settings(i, 0.0f, 0.0f, 1.0f, 1.0f);
    }
    g_alloc_memory_for_ending();
}

void smd_mini_ending_tick(void)
{
    int done;

    done = 0;
    if ((pad_status_groups[0].unk0[2].button & PAD_BUTTON_LEFT)
     || (pad_status_groups[0].unk0[2].button & PAD_BUTTON_RIGHT)
     || (pad_status_groups[0].unk0[2].button & PAD_BUTTON_UP)
     || (pad_status_groups[0].unk0[2].button & PAD_BUTTON_DOWN))
        done = 1;
    if (pad_status_groups[0].unk0[2].button & PAD_BUTTON_LEFT)
    {
        modeCtrl.currPlayer--;
        if (modeCtrl.currPlayer < 0)
            modeCtrl.currPlayer += 4;
    }
    if (pad_status_groups[0].unk0[2].button & PAD_BUTTON_RIGHT)
    {
        modeCtrl.currPlayer++;
        if (modeCtrl.currPlayer >= 4)
            modeCtrl.currPlayer -= 4;
    }
    if (pad_status_groups[0].unk0[2].button & PAD_BUTTON_DOWN)
    {
        modeCtrl.difficulty--;
        if (modeCtrl.difficulty < 0)
            modeCtrl.difficulty += 3;
    }
    if (pad_status_groups[0].unk0[2].button & PAD_BUTTON_UP)
    {
        modeCtrl.difficulty++;
        if (modeCtrl.difficulty >= 3)
            modeCtrl.difficulty -= 3;
    }
    done |= !fn_1_FD740();
    if (done)
        sub_mode_request = 0xB7;
}

void mini_ending_draw_func(void)
{
    debug_textdraw_set_pos(4, 4);
    debug_textdraw_vsprintf_buf_1("ENDING VIEWER\n");
    debug_textdraw_set_pos(6, 6);
    debug_textdraw_vsprintf_buf_1("COURSE[%d]\n", modeCtrl.difficulty);
    debug_textdraw_vsprintf_buf_1("PLAYER[%d]\n", modeCtrl.currPlayer);
    fn_1_FD97C();
}

void smd_mini_s_roll_init(void)
{
    g_very_similar_to_puts("mini_roll_(nameentry)init\n");
    sub_mode_request = 0xC4;
    g_nameentry2_init();
}

void smd_mini_s_roll_tick(void)
{
    fn_1_11C02C();
}

void mini_nameentry_draw_func(void)
{
    game_nameentry_draw_func();
}
