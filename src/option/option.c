/* Options screen: the mode handlers behind the menu panels, and the module entry points. */
#include "types.h"
#include "ball.h"
#include "camera.h"
#include "event.h"
#include "gx.h"
#include "input.h"
#include "libc.h"
#include "mathutil.h"
#include "mode.h"
#include "option.h"
#include "os.h"
#include "recplay.h"
#include "sound.h"
#include "sprite.h"
#include "stage.h"
#include "world.h"

struct StickPoint
{
    s8 x;
    s8 y;
};

struct MemcardRequest
{
    /*0x00*/ u8 unk0;
    u8 filler1[3];
    /*0x04*/ u32 flags;
    /*0x08*/ char *fileName;
    /*0x0C*/ void *files;
    u8 filler10[8];
}; /* size = 0x18 */

struct RendEfcRequest
{
    u8 filler0[6];
    /*0x06*/ u16 unk6;
    u8 filler8[0x10];
}; /* size = 0x18 */

struct ModeEntry
{
    /*0x00*/ s32 unk0;
    /*0x04*/ char *name;
    /*0x08*/ s32 subMode;
};

struct RankingList
{
    /*0x00*/ s8 *modes;
    /*0x04*/ s8 count;
};

struct MemcardWork
{
    /*0x00*/ s8 memcardState;
    /*0x01*/ s8 memcardStep;
};

struct ReplayView
{
    /*0x00*/ s16 view;
    /*0x02*/ s16 nextView;
    /*0x04*/ u32 flags;
    /*0x08*/ s32 voice;
};

extern s32 lbl_1_bss_C8B48;
extern u8 lbl_1_bss_D1320[];
extern struct ControllerInfo lbl_80145210[];
extern struct StickPoint lbl_80081AC0[4][8];
extern u32 g_font_char_var_3;
extern u32 g_some_other_flags;
extern s32 g_smth_with_sound;
extern u32 g_player_id_for_sound;
extern int stage_heap;
extern void (*g_some_func_ptr_related_to_sub_mode2)(void);
extern void (*test_draw_func_ptr)(void);

u32 *OSGetStackPointer(void);
void g_very_similar_to_puts(const char *s);
void OSReport(const char *fmt, ...);
int OSSetCurrentHeap(int heap);
u32 OSGetSoundMode(void);
void g_set_up_debugtext_buffer(void);
void g_something_with_iteratively_freeing_memory(void);
void unload_stage(void);
void light_init(int stageId);
void dest_all_events(void);
void load_bmp_by_id(int id);
void call_free_bmp_by_id(int id);
void fade_screen_to_color(int flags, u32 color, int len);
void g_fade_track_volume(int a, int b);
void set_sub_mode_destination(void (*func)(void));
void destroy_sprite_with_unique_id(int id);
void debug_textdraw_set_pos(int x, int y);
void debug_textdraw_put_buf_1(char *str);
void debug_textdraw_vsprintf_buf_1(char *fmt, ...);
u8 g_check_some_memcard_field(void);
int g_get_some_memcard_var(void);
int g_get_result_code(void);
void g_sprintf_memcard_error(int a, char *buf);
void g_some_replay_func4(int a, int b, struct MemcardRequest *req);
void fn_1_F0314(void);
void fn_1_F0330(void);
void fn_1_367E4(int mode);
void fn_8000A380(struct StickPoint *point, struct StickPoint *cal);
void fn_1_E2FD0(int width);
void fn_1_E3164(u16 count, Vec *points, GXColor *c);
void fn_1_F8230(void);
void fn_1_D0EC4(void);
void fn_1_D0FA4(int difficulty);
void fn_1_D1220(int a);
int fn_1_D1344(void);
void fn_1_F8060(int mode, int flag);
void fn_1_F812C(int mode, int flag);
void fn_1_F828C(void);
void fn_1_F83A4(void);
void fn_1_F844C(void);
void fn_1_F8508(void);
u8 fn_1_FD63C(void);
void fn_1_FD62C(u8 a);
void fn_1_56C18(void);
void fn_1_618F4(int replay);
void fn_1_D8130(int a, int b, struct RendEfcRequest *req);
void load_stage(int stage);
void g_handle_world_bgm(int a);
void g_init_rendefc_for_stage(void);
void g_advance_stage_animation(void);
int call_SoundReqID_arg_1_copy(u32 soundId);
int g_get_replay_time(void);
void game_main_draw_func(void);

void textdraw_reset(void);
void textdraw_set_font(int font);
void textdraw_set_scale(float x, float y);
void textdraw_set_depth(float depth);
void textdraw_set_mul_color(u32 color);
void textdraw_set_add_color(u32 color);
void textdraw_set_pos(float x, float y);
float textdraw_print(char *text);
void textdraw_printf(char *fmt, ...);

void fn_9_80C4(void);
void fn_9_8190(void);
void fn_9_81F0(void);
void fn_9_845C(void);
void fn_9_84D4(void);
void fn_9_8640(void);
void fn_9_8698(void);
void fn_9_86CC(void);
void fn_9_8A24(void);
void fn_9_8C14(void);
void fn_9_99BC(void);
void fn_9_A0CC(void);
void fn_9_A0FC(void);
void fn_9_A150(void);
void fn_9_A1D4(void);
void fn_9_A788(void);
void fn_9_A7F0(void);
void fn_9_A804(void);
void fn_9_A838(void);
void fn_9_A83C(void);
void fn_9_A840(void);
void fn_9_A870(void);
void fn_9_AA14(void);
void fn_9_AD88(void);
void fn_9_B51C(void);
void fn_9_B790(int view);
void fn_9_B854(void);
void fn_9_B8AC(void);
void fn_9_BA14(void);
void fn_9_BA64(void);
void fn_9_BC24(void);

u32 lbl_9_data_1968[] = {
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

void (*lbl_9_data_1AD8[])(void) = {
    fn_9_8190, fn_9_81F0, fn_9_845C, fn_9_84D4, fn_9_8A24, fn_9_8C14,
    fn_9_8698, fn_9_86CC, fn_9_A0CC, fn_9_A0FC, fn_9_A150, fn_9_A1D4,
    fn_9_A838, fn_9_A83C, fn_9_A7F0, fn_9_A804, fn_9_A840, fn_9_A870,
    fn_9_AA14, fn_9_AD88, fn_9_B854, fn_9_B8AC, fn_9_BA14, fn_9_BA64,
    NULL,
};

u8 lbl_9_data_1B3C = 3;

static u8 lbl_9_bss_80;
struct OptionWork lbl_9_bss_84;
static s16 lbl_9_bss_238;
static struct MemcardWork lbl_9_bss_23C;
static struct StickPoint s_rawMax[4][8];
static struct StickPoint s_seenMax[4][8];
static struct StickPoint s_clamped[4][8];
static Vec s_rawHistory[4][0x100];
static u16 s_historyLen[4];
static s32 s_historyFull[4];
static s8 s_lastX[4];
static s8 s_lastY[4];
static Vec s_clampedHistory[4][0x100];
static float s_centerY;
static struct ReplayView s_replayView;
static struct ReplayHeader lbl_9_bss_6330;
static s32 s_paused;
struct ReplayEntry lbl_9_bss_634C[0x80];

/* never called; the .data pool must exist before the .bss pool */
static void *force_data_pool(int i)
{
    if (i == 0)
        return lbl_9_data_1968;
    if (i == 1)
        return lbl_9_data_1AD8;
    return &lbl_9_data_1B3C;
}

/* never called; keeps the .bss objects in definition order */
static void *force_bss_order(int i)
{
    if (i == 0)
        return &lbl_9_bss_80;
    if (i == 1)
        return &lbl_9_bss_84;
    if (i == 2)
        return &lbl_9_bss_238;
    if (i == 3)
        return &lbl_9_bss_23C;
    if (i == 4)
        return s_rawMax;
    if (i == 5)
        return s_seenMax;
    if (i == 6)
        return s_clamped;
    if (i == 7)
        return s_rawHistory;
    if (i == 8)
        return s_historyLen;
    if (i == 9)
        return s_historyFull;
    if (i == 10)
        return s_lastX;
    if (i == 11)
        return s_lastY;
    if (i == 12)
        return s_clampedHistory;
    if (i == 13)
        return &s_centerY;
    if (i == 14)
        return &s_replayView;
    if (i == 15)
        return &lbl_9_bss_6330;
    if (i == 16)
        return &s_paused;
    return lbl_9_bss_634C;
}

/* a pad's stick readings start from the shape of the octagonal gate */
static inline void reset_stick_gate(int i)
{
    s_rawMax[i][0] = (struct StickPoint){0x54, 0};
    s_rawMax[i][1] = (struct StickPoint){0x3B, 0x3B};
    s_rawMax[i][2] = (struct StickPoint){0, 0x54};
    s_rawMax[i][3] = (struct StickPoint){-0x3B, 0x3B};
    s_rawMax[i][4] = (struct StickPoint){-0x54, 0};
    s_rawMax[i][5] = (struct StickPoint){-0x3B, -0x3B};
    s_rawMax[i][6] = (struct StickPoint){0, -0x54};
    s_rawMax[i][7] = (struct StickPoint){0x3B, -0x3B};
}

void _prolog(void)
{
    lbl_1_bss_C8B48 = 1;
    g_set_up_debugtext_buffer();
    g_something_with_iteratively_freeing_memory();
    unload_stage();
    light_init(0);
    dest_all_events();
    event_init(0x11);
    event_init(0x13);
    load_bmp_by_id(4);
    fn_9_7E4();
    fade_screen_to_color(0x100, 0, 0x1E);
    g_some_func_ptr_related_to_sub_mode2 = fn_9_80C4;
    test_draw_func_ptr = fn_9_BC24;
    g_very_similar_to_puts("OPTION LOADED\n");
}

void _epilog(void)
{
    dest_all_events();
    g_some_func_ptr_related_to_sub_mode2 = NULL;
    test_draw_func_ptr = NULL;
    g_very_similar_to_puts("OPTION PURGED\n");
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
    OSPanic("option.c", 150, "\n");
}

void fn_9_80C4(void)
{
    if (g_font_char_var_3 & 4)
        lbl_9_bss_84.unk38 = 0xBF;
    else
        lbl_9_bss_84.unk38 = 0;
    if (sub_mode <= 0xD2 || sub_mode >= 0xF3)
        printf("sub_mode: error %d in Option", sub_mode);
    else
    {
        int i = sub_mode - 0xD3;
        lbl_9_data_1AD8[i]();
    }
}

struct ModeEntry lbl_9_data_1C3C[7] = {
    {0, "REPLAY", 0xE1},
    {0, "POINT", 0xDB},
    {0, "RANKING", 0xDD},
    {0, "SOUND", 0xD5},
    {0, "SEPARATE MODE", 0xE3},
    {0, "PAD", 0xE7},
    {0, "MEMORY CARD", 0xD9},
};

void fn_9_8190(void)
{
    lbl_9_bss_84.unk39 = 0;
    fn_9_10E8();
    g_fade_track_volume(0x49, 0);
    modeCtrl.courseFlags &= ~4;
    sub_mode_request = 0xD4;
}

void fn_9_81F0(void)
{
    if (!(modeCtrl.courseFlags & 4))
    {
        struct Panel *panel = &lbl_9_bss_84.panels[0];
        s8 sel = panel->sel;
        int disabled;

        if ((merged_digital_inputs[4] & 4) || (merged_analog_inputs[4] & 4))
        {
            sel++;
            if (sel >= 7U)
                sel = 0;
        }
        if ((merged_digital_inputs[4] & 8) || (merged_analog_inputs[4] & 8))
        {
            sel--;
            if (sel < 0)
                sel = 6;
        }
        if (sel != panel->sel)
        {
            call_SoundReqID_arg_0(0x6C);
            panel->sel = sel;
        }
        if (lbl_9_data_1C3C[panel->sel].unk0 != 0)
            disabled = 1;
        else if (merged_digital_inputs[2] & 0x100)
        {
            call_SoundReqID_arg_0(0x6A);
            fn_9_0(0x58);
            modeCtrl.submodeTimer = 0x1E;
            sub_mode_request = lbl_9_data_1C3C[panel->sel].subMode;
            return;
        }
        if (merged_digital_inputs[2] & 0x200)
        {
            call_SoundReqID_arg_0(0x6B);
            modeCtrl.submodeTimer = 0x1E;
            fade_screen_to_color(0x101, 0, modeCtrl.submodeTimer);
            g_fade_track_volume(modeCtrl.submodeTimer, 2);
            modeCtrl.courseFlags |= 4;
        }
    }
    else
    {
        if (--modeCtrl.submodeTimer <= 0)
        {
            modeCtrl.submodeTimer = 0;
            modeCtrl.menuSel = 3;
            modeCtrl.unk10 = 1;
            main_mode_request = 0;
            sub_mode_request = 0x15;
        }
    }
}

void fn_9_845C(void)
{
    struct Panel *panel = &lbl_9_bss_84.panels[4];

    g_fade_track_volume(lbl_9_bss_80, 0);
    fn_9_4510();
    panel->unk1C = 0x1E;
    lbl_9_bss_238 = 0;
    set_sub_mode_destination(fn_9_8640);
    sub_mode_request = 0xD6;
}

void fn_9_84D4(void)
{
    struct Panel *panel = &lbl_9_bss_84.panels[4];

    if (--panel->unk1C == 0)
    {
        call_SoundReqID_arg_0(lbl_9_data_1B3C);
        panel->unk1C = 0x3C;
    }
    if (lbl_9_bss_238 > 0)
    {
        if (--lbl_9_bss_238 > 0)
            return;
        fn_1_367E4(OSGetSoundMode() == 1 ? 0 : 1);
        g_fade_track_volume(lbl_9_bss_80, 0);
    }
    if ((merged_digital_inputs[4] & 1) || (merged_analog_inputs[4] & 1)
     || (merged_digital_inputs[4] & 2) || (merged_analog_inputs[4] & 2))
    {
        call_SoundReqID_arg_0(0x6C);
        g_fade_track_volume(-1, 1);
        lbl_9_bss_238 = 5;
    }
    else if (merged_digital_inputs[2] & 0x200)
    {
        call_SoundReqID_arg_0(0x6B);
        fn_9_68(0x5C);
        sub_mode_request = 0xD3;
    }
}

void fn_9_8640(void)
{
    switch (sub_mode)
    {
    case 0xD5:
    case 0xD6:
        set_sub_mode_destination(fn_9_8640);
        return;
    }
    g_fade_track_volume(0x1E, 2);
}

void fn_9_8698(void)
{
    lbl_9_bss_23C.memcardState = 0;
    lbl_9_bss_23C.memcardStep = 0;
    sub_mode_request = 0xDA;
}

void fn_9_86CC(void)
{
    struct MemcardRequest req;
    char fileName[0x20];
    struct MemcardRequest req2;

    debug_textdraw_set_pos(1, 1);
    debug_textdraw_put_buf_1("MEMCARD");
    switch (lbl_9_bss_23C.memcardState)
    {
    case 0:
        debug_textdraw_set_pos(3, 3);
        debug_textdraw_vsprintf_buf_1("A: LOAD\n");
        debug_textdraw_vsprintf_buf_1("X: SAVE\n");
        debug_textdraw_vsprintf_buf_1("B: EXIT\n");
        if (pad_status_groups[0].unk0[2].button & 0x100)
        {
            lbl_9_bss_23C.memcardState = 2;
            lbl_9_bss_23C.memcardStep = 0;
        }
        if (pad_status_groups[0].unk0[2].button & 0x200)
            sub_mode_request = 0xD3;
        else if (pad_status_groups[0].unk0[2].button & 0x400)
        {
            lbl_9_bss_23C.memcardState = 1;
            lbl_9_bss_23C.memcardStep = 0;
        }
        break;
    case 1:
        debug_textdraw_set_pos(3, 3);
        debug_textdraw_vsprintf_buf_1("SAVE");
        switch (lbl_9_bss_23C.memcardStep)
        {
        case 0:
            if (g_check_some_memcard_field() == 0)
            {
                fn_1_F0314();
                memset(&req, 0, sizeof(req));
                req.unk0 = 1;
                g_sprintf_memcard_error(1, fileName);
                req.fileName = fileName;
                req.files = lbl_1_bss_D1320;
                g_some_replay_func4(0, 4, &req);
                lbl_9_bss_23C.memcardStep++;
            }
            break;
        case 1:
            if (g_check_some_memcard_field() == 0)
            {
                if (g_get_some_memcard_var() == 0)
                    printf("result_code: %d ", g_get_result_code());
                lbl_9_bss_23C.memcardState = 0;
                lbl_9_bss_23C.memcardStep = 0;
            }
            break;
        }
        break;
    case 2:
        debug_textdraw_set_pos(3, 3);
        debug_textdraw_vsprintf_buf_1("LOAD");
        switch (lbl_9_bss_23C.memcardStep)
        {
        case 0:
            if (g_check_some_memcard_field() == 0)
            {
                memset(&req2, 0, sizeof(req2));
                req2.unk0 = 1;
                req2.files = lbl_1_bss_D1320;
                g_some_replay_func4(1, 4, &req2);
                lbl_9_bss_23C.memcardStep++;
            }
            break;
        case 1:
            if (g_check_some_memcard_field() == 0)
            {
                if (g_get_some_memcard_var() != 0)
                    fn_1_F0330();
                else
                    printf("result_code: %d ", g_get_result_code());
                lbl_9_bss_23C.memcardState = 0;
                lbl_9_bss_23C.memcardStep = 0;
                printf("dip_switch: %08X\n", *(u32 *)lbl_1_bss_D1320);
            }
            break;
        }
        break;
    }
}

void fn_9_8A24(void)
{
    struct Panel *panel = &lbl_9_bss_84.panels[9];
    int i;

    lbl_9_bss_84.unk39 = 0;
    memset(s_rawHistory, 0, sizeof(s_rawHistory));
    memset(s_historyLen, 0, sizeof(s_historyLen));
    for (i = 0; i < 4; i++)
    {
        s_historyFull[i] = 0;
        s_lastX[i] = s_lastY[i] = -0x80;
        reset_stick_gate(i);
    }
    memset(s_seenMax, 0, sizeof(s_seenMax));
    memset(s_clamped, 0, sizeof(s_clamped));
    fn_9_7E5C();
    sub_mode_request = 0xD8;
    panel->unk1C = 0;
    panel->sel2 = 0;
}

void fn_9_8C14(void)
{
    struct Panel *panel;
    int i;
    int x;
    int y;

    if (g_some_other_flags & 0xA)
        return;
    panel = &lbl_9_bss_84.panels[9];
    switch (panel->unk1C)
    {
    case 0:
    default:
        for (i = 0; i < 4; i++)
        {
            if (pad_status_groups[i].unk0[0].err == -1 || pad_status_groups[i].unk0[0].err == -3)
            {
                memset(s_rawHistory[i], 0, sizeof(s_rawHistory[i]));
                s_historyLen[i] = 0;
                s_historyFull[i] = 0;
                s_lastX[i] = s_lastY[i] = -0x80;
                reset_stick_gate(i);
                memset(s_seenMax[i], 0, sizeof(s_seenMax[i]));
                memset(s_clamped[i], 0, sizeof(s_clamped[i]));
            }
            else
            {
                struct StickPoint point;
                float fx;
                float fy;

                x = lbl_80145210[i].unk0[0].stickX;
                y = lbl_80145210[i].unk0[0].stickY;
                if (s_rawMax[i][0].x < x)
                {
                    s_rawMax[i][0].x = (s8)x;
                    s_rawMax[i][0].y = (s8)y;
                }
                if (s_rawMax[i][4].x > x)
                {
                    s_rawMax[i][4].x = (s8)x;
                    s_rawMax[i][4].y = (s8)y;
                }
                if (s_rawMax[i][2].y < y)
                {
                    s_rawMax[i][2].x = (s8)x;
                    s_rawMax[i][2].y = (s8)y;
                }
                if (s_rawMax[i][6].y > y)
                {
                    s_rawMax[i][6].x = (s8)x;
                    s_rawMax[i][6].y = (s8)y;
                }
                if (s_seenMax[i][0].x < x)
                {
                    s_seenMax[i][0].x = s_clamped[i][0].x = (s8)x;
                    s_seenMax[i][0].y = s_clamped[i][0].y = (s8)y;
                    fn_8000A380(&s_clamped[i][0], s_rawMax[i]);
                }
                if (s_seenMax[i][4].x > x)
                {
                    s_seenMax[i][4].x = s_clamped[i][4].x = (s8)x;
                    s_seenMax[i][4].y = s_clamped[i][4].y = (s8)y;
                    fn_8000A380(&s_clamped[i][4], s_rawMax[i]);
                }
                if (s_seenMax[i][2].y < y)
                {
                    s_seenMax[i][2].x = s_clamped[i][2].x = (s8)x;
                    s_seenMax[i][2].y = s_clamped[i][2].y = (s8)y;
                    fn_8000A380(&s_clamped[i][2], s_rawMax[i]);
                }
                if (s_seenMax[i][6].y > y)
                {
                    s_seenMax[i][6].x = s_clamped[i][6].x = (s8)x;
                    s_seenMax[i][6].y = s_clamped[i][6].y = (s8)y;
                    fn_8000A380(&s_clamped[i][6], s_rawMax[i]);
                }
                if (__abs(__abs(x) - __abs(y)) < 0x10)
                {
                    int k;
                    float rawDist;
                    float seenDist;
                    float dist;

                    if (y > 0)
                        k = (x > 0) ? 1 : 3;
                    else if (x < 0)
                        k = 5;
                    else
                        k = 7;
                    rawDist = mathutil_sum_of_sq_2(s_rawMax[i][k].x, s_rawMax[i][k].y);
                    seenDist = mathutil_sum_of_sq_2(s_seenMax[i][k].x, s_seenMax[i][k].y);
                    dist = mathutil_sum_of_sq_2(x, y);
                    if (rawDist < dist)
                    {
                        s_rawMax[i][k].x = (s8)x;
                        s_rawMax[i][k].y = (s8)y;
                    }
                    if (seenDist < dist)
                    {
                        s_seenMax[i][k].x = s_clamped[i][k].x = (s8)x;
                        s_seenMax[i][k].y = s_clamped[i][k].y = (s8)y;
                        fn_8000A380(&s_clamped[i][k], s_rawMax[i]);
                    }
                }
                point.x = (s8)x;
                point.y = (s8)y;
                fn_8000A380(&point, s_rawMax[i]);
                fx = 1.7f * point.x;
                fy = 1.7f * point.y;
                if (__abs(s_lastX[i] - x) > 1 || __abs(s_lastY[i] - y) > 1)
                {
                    s_lastX[i] = (s8)x;
                    s_lastY[i] = (s8)y;
                    if (s_historyLen[i] == 0x100)
                    {
                        s_historyFull[i] = 1;
                        s_historyLen[i] = 0;
                    }
                    s_clampedHistory[i][s_historyLen[i]] = (Vec){0.01f * fx, 0.01f * fy, -2.0f};
                    s_rawHistory[i][s_historyLen[i]++] = (Vec){0.01f * x, 0.01f * y, -2.0f};
                }
            }
        }
        if (merged_digital_inputs[2] & 0x100)
        {
            call_SoundReqID_arg_0(0x6A);
            panel->sel2 = 0;
            panel->unk1C = 1;
            return;
        }
        if (merged_digital_inputs[2] & 0x200)
        {
            fn_9_68(0x61);
            call_SoundReqID_arg_0(0x6B);
            sub_mode_request = 0xE7;
        }
        break;
    case 1:
        y = panel->sel2;
        if ((merged_digital_inputs[2] & 2) || (merged_analog_inputs[2] & 2))
            y = 0;
        if ((merged_digital_inputs[2] & 1) || (merged_analog_inputs[2] & 1))
            y = 1;
        if (y != panel->sel2)
        {
            call_SoundReqID_arg_0(0x6C);
            panel->sel2 = y;
        }
        if (merged_digital_inputs[2] & 0x100)
        {
            if (panel->sel2 == 1)
            {
                call_SoundReqID_arg_0(0x6A);
                for (i = 0; i < 4; i++)
                {
                    if (pad_status_groups[i].unk0[0].err != -1
                     && pad_status_groups[i].unk0[0].err != -3
                     && (s_clamped[i][0].x != 0x3C
                      || s_clamped[i][1].x != 0x3C
                      || s_clamped[i][1].y != 0x3C
                      || s_clamped[i][2].y != 0x3C
                      || s_clamped[i][3].x != -0x3C
                      || s_clamped[i][3].y != 0x3C
                      || s_clamped[i][4].x != -0x3C
                      || s_clamped[i][5].x != -0x3C
                      || s_clamped[i][5].y != -0x3C
                      || s_clamped[i][6].y != -0x3C
                      || s_clamped[i][7].x != 0x3C
                      || s_clamped[i][7].y != -0x3C))
                    {
                        panel->unk1C = 4;
                        break;
                    }
                }
                if (panel->unk1C != 4)
                {
                    memcpy_handler(lbl_80081AC0, s_rawMax, sizeof(s_rawMax));
                    panel->unk1C = 2;
                }
            }
            else
            {
                call_SoundReqID_arg_0(0x6B);
                panel->unk1C = 3;
            }
        }
        else if (merged_digital_inputs[2] & 0x200)
        {
            call_SoundReqID_arg_0(0x6B);
            panel->unk1C = 3;
        }
        break;
    case 2:
    case 3:
        if (merged_digital_inputs[2])
        {
            fn_9_68(0x61);
            call_SoundReqID_arg_0(0x6B);
            sub_mode_request = 0xE7;
        }
        break;
    case 4:
        if (merged_digital_inputs[2])
        {
            call_SoundReqID_arg_0(0x6B);
            panel->unk1C = 0;
        }
        break;
    }
}

float lbl_9_data_1CF4 = -400.0f;
float lbl_9_data_1CF8 = 70.0f;
float lbl_9_data_1CFC = 0.6135f;
s16 lbl_9_data_1D00 = 0x49;
s16 lbl_9_data_1D02 = -7;

static void set_centered_text_pos(float x, float y, float chars, float size)
{
    x -= 0.5 * (chars * size);
    y -= 0.5 * size;
    textdraw_set_pos(x, y);
}

void fn_9_99BC(void)
{
    GXColor colors[4] = {{0xFF, 0, 0, 0xFF}, {0, 0, 0xFF, 0xFF}, {0xFF, 0xFF, 0, 0xFF}, {0, 0xFF, 0, 0xFF}};
    GXColor black = {0, 0, 0, 0xFF};
    Vec gate[9] = {
        {60.0f, 0.0f, 0.0f}, {60.0f, 60.0f, 0.0f}, {0.0f, 60.0f, 0.0f},
        {-60.0f, 60.0f, 0.0f}, {-60.0f, 0.0f, 0.0f}, {-60.0f, -60.0f, 0.0f},
        {0.0f, -60.0f, 0.0f}, {60.0f, -60.0f, 0.0f}, {60.0f, 0.0f, 0.0f},
    };
    Vec verts[9];
    struct Panel *panel = &lbl_9_bss_84.panels[9];
    int i;
    int j;

    textdraw_reset();
    textdraw_set_font(0x8F);
    textdraw_set_scale(lbl_9_data_1CFC, lbl_9_data_1CFC);
    GXSetZMode_cached(1, 3, 1);
    mtxa_from_identity();
    fn_1_E2FD0(0x18);
    for (j = 0; j < 9; j++)
        verts[j].z = lbl_9_data_1CF4;
    for (i = 0; i < 4; i++)
    {
        set_centered_text_pos(panel->x + (320.0f + lbl_9_data_1D00 * (float)(i * 2 - 3)), 150.0f, 7.0f,
                              24.0f * lbl_9_data_1CFC);
        textdraw_set_mul_color(0);
        textdraw_printf("k/KONTORO-RA%d", i + 1);
        if (lbl_80145210[i].unk0[0].err == -1)
        {
            set_centered_text_pos(2.0 + (panel->x + (320.0f + lbl_9_data_1D00 * (float)(i * 2 - 3))),
                                  2.0 + (240.0f + lbl_9_data_1D02), 8.0f, 24.0f * lbl_9_data_1CFC);
            textdraw_set_depth(1.0f);
            textdraw_set_mul_color(i == 0 ? 0xFF7F7F : i == 1 ? 0x7F7FFF : i == 2 ? 0xFFFF7F : 0x7FFF7F);
            textdraw_print("k/KONTORO-RAh/GA\nh/SASATTEIMASEN");
            set_centered_text_pos(panel->x + (320.0f + lbl_9_data_1D00 * (float)(i * 2 - 3)),
                                  240.0f + lbl_9_data_1D02, 8.0f, 24.0f * lbl_9_data_1CFC);
            textdraw_set_depth(0.1f);
            textdraw_set_mul_color(0);
            textdraw_print("k/KONTORO-RAh/GA\nh/SASATTEIMASEN");
        }
        else
        {
            if (panel->unk1C <= 0)
            {
                textdraw_set_pos(panel->x + (320.0 + 320.0 * ((pad_status_groups[i].unk0[0].stickX + lbl_9_data_1CF8 * (i * 2 - 3)) / 307.92) - 0.5 * (24.0 * lbl_9_data_1CFC)),
                                 240.0 + 320.0 * ((-pad_status_groups[i].unk0[0].stickY + s_centerY * (i * 2 - 3)) / 307.92) - 0.5 * (24.0 * lbl_9_data_1CFC));
                textdraw_set_mul_color(0xFFFFFF);
                textdraw_print("p/LEVER/");
            }
            for (j = 0; j < 8; j++)
            {
                verts[j].x = 307.92 * (panel->x / 320.0) + (gate[j].x + lbl_9_data_1CF8 * (i * 2 - 3));
                verts[j].y = s_centerY + gate[j].y;
            }
            verts[8] = verts[0];
            fn_1_E3164(9, verts, &black);
            for (j = 0; j < 8; j++)
            {
                verts[j].x = 307.92 * (panel->x / 320.0) + (lbl_9_data_1CF8 * (i * 2 - 3) + s_clamped[i][j].x);
                verts[j].y = s_centerY + s_clamped[i][j].y;
            }
            verts[8] = verts[0];
            fn_1_E3164(9, verts, &colors[i]);
        }
    }
}

void fn_9_A0CC(void)
{
    fn_9_3B78();
    sub_mode_request = 0xDC;
}

void fn_9_A0FC(void)
{
    if (merged_digital_inputs[2] & 0x200)
    {
        call_SoundReqID_arg_0(0x6B);
        fn_9_68(0x5A);
        sub_mode_request = 0xD3;
    }
}

void fn_9_A150(void)
{
    struct Panel *panel = &lbl_9_bss_84.panels[3];

    fn_1_F8230();
    g_fade_track_volume(0x42, 0);
    fn_9_5AD0();
    panel->sel = 0;
    panel->sel2 = 0;
    modeCtrl.courseFlags &= ~4;
    set_sub_mode_destination(fn_9_A788);
    sub_mode_request = 0xDE;
}

s8 lbl_9_data_1D44[] = {0, 1, 2, 3, 4, 5, 6, 7, 8};
s8 lbl_9_data_1D50[] = {9, 10, 11};
struct RankingList lbl_9_data_1D54[3] = {
    {NULL, 0},
    {lbl_9_data_1D44, 9},
    {lbl_9_data_1D50, 3},
};

void fn_9_A1D4(void)
{
    struct RankingList *list;
    struct Panel *panel;
    int sel;

    panel = &lbl_9_bss_84.panels[3];
    if (!(modeCtrl.courseFlags & 4))
    {
        sel = panel->sel;

        if ((merged_digital_inputs[4] & 1) || (merged_analog_inputs[4] & 1))
        {
            sel--;
            if (sel < 0)
                sel = 2;
        }
        if ((merged_digital_inputs[4] & 2) || (merged_analog_inputs[4] & 2))
        {
            sel++;
            if (sel >= 3U)
                sel = 0;
        }
        if (sel != panel->sel)
        {
            call_SoundReqID_arg_0(0x6C);
            panel->sel = sel;
        }
        if (merged_digital_inputs[2] & 0x100)
        {
            call_SoundReqID_arg_0(0x6A);
            modeCtrl.submodeTimer = (panel->sel == 0) ? 0x1E : 0x1E;
            fn_9_0(0x5B);
            modeCtrl.courseFlags |= 4;
        }
        else if (merged_digital_inputs[2] & 0x200)
        {
            call_SoundReqID_arg_0(0x6B);
            fn_9_68(0x5B);
            sub_mode_request = 0xD3;
        }
    }
    else
    {
        if (panel->sel == 0)
        {
            if (modeCtrl.submodeTimer > 0)
            {
                modeCtrl.submodeTimer--;
                if (modeCtrl.submodeTimer <= 0)
                {
                    panel->unk1C = 0x1E;
                    panel->sel2 = 0;
                    sel = OSSetCurrentHeap(stage_heap);
                    load_bmp_by_id(3);
                    OSSetCurrentHeap(sel);
                    fn_1_D0EC4();
                    fn_1_D0FA4(0);
                    fn_1_F8508();
                    fn_1_F844C();
                }
            }
            else
            {
                if (fn_1_D1344() != 0 && panel->unk1C <= 0)
                {
                    sel = panel->sel2;
                    if ((merged_digital_inputs[4] & 1) || (merged_analog_inputs[4] & 1) || (merged_digital_inputs[4] & 0x40))
                    {
                        sel--;
                        if (sel < 0)
                            sel = 0;
                    }
                    if ((merged_digital_inputs[4] & 2) || (merged_analog_inputs[4] & 2) || (merged_digital_inputs[4] & 0x20))
                    {
                        sel++;
                        if (sel >= 3)
                            sel = 2;
                    }
                    if (sel != panel->sel2)
                    {
                        destroy_sprite_with_unique_id(0x65);
                        fn_1_D1220(0);
                        fn_1_D0FA4(sel);
                        switch (sel)
                        {
                        case 0:
                        default:
                            fn_1_F844C();
                            break;
                        case 1:
                            fn_1_F844C();
                            fn_1_F83A4();
                            break;
                        case 2:
                            fn_1_F83A4();
                            break;
                        }
                        panel->sel2 = sel;
                    }
                }
                if (panel->unk1C > 0)
                    panel->unk1C--;
            }
        }
        else if (modeCtrl.submodeTimer > 0)
        {
            modeCtrl.submodeTimer--;
            if (modeCtrl.submodeTimer <= 0)
            {
                list = &lbl_9_data_1D54[panel->sel];
                panel->sel2 = 0;
                sel = OSSetCurrentHeap(stage_heap);
                fn_1_F8060(list->modes[panel->sel2], 0);
                OSSetCurrentHeap(sel);
                fn_1_F844C();
            }
        }
        else
        {
            list = &lbl_9_data_1D54[panel->sel];
            sel = panel->sel2;
            if ((merged_digital_inputs[4] & 1) || (merged_analog_inputs[4] & 1) || (merged_digital_inputs[4] & 0x40))
            {
                sel--;
                if (sel < 0)
                    sel = 0;
            }
            if ((merged_digital_inputs[4] & 2) || (merged_analog_inputs[4] & 2) || (merged_digital_inputs[4] & 0x20))
            {
                sel++;
                if (sel >= list->count)
                    sel = list->count - 1;
            }
            if (sel != panel->sel2)
            {
                fn_1_F812C(list->modes[sel], 0);
                if (sel != 0)
                    fn_1_F83A4();
                if (sel < list->count - 1)
                    fn_1_F844C();
                call_SoundReqID_arg_0(0x65);
                panel->sel2 = sel;
            }
        }
        if (merged_digital_inputs[2] & 0x200)
        {
            if (panel->sel == 0)
            {
                fn_1_D1220(1);
                destroy_sprite_with_unique_id(0x65);
                call_free_bmp_by_id(3);
            }
            else if (modeCtrl.submodeTimer <= 0)
                fn_1_F828C();
            fn_9_5AD0();
            call_SoundReqID_arg_0(0x6B);
            modeCtrl.courseFlags &= ~4;
        }
    }
}

void fn_9_A788(void)
{
    switch (sub_mode)
    {
    case 0xDD:
    case 0xDE:
        set_sub_mode_destination(fn_9_A788);
        return;
    }
    if (modeCtrl.courseFlags & 4)
        fn_1_F828C();
}

void fn_9_A7F0(void)
{
    sub_mode_request = 0xE2;
}

void fn_9_A804(void)
{
    call_SoundReqID_arg_0(0x6B);
    sub_mode_request = 0xD3;
}

void fn_9_A838(void)
{
}

void fn_9_A83C(void)
{
}

void fn_9_A840(void)
{
    fn_9_5194();
    sub_mode_request = 0xE4;
}

void fn_9_A870(void)
{
    u8 var = modeCtrl.splitscreenMode;

    if (var % 2)
    {
        if ((merged_digital_inputs[2] & 1) || (merged_analog_inputs[2] & 1))
            var--;
    }
    else if ((merged_digital_inputs[2] & 2) || (merged_analog_inputs[2] & 2))
        var++;
    if (var < 2)
    {
        if ((merged_digital_inputs[2] & 4) || (merged_analog_inputs[2] & 4))
            var += 2;
    }
    else if ((merged_digital_inputs[2] & 8) || (merged_analog_inputs[2] & 8))
        var -= 2;
    if (var > 3)
        var = 0;
    if (var != modeCtrl.splitscreenMode)
    {
        call_SoundReqID_arg_0(0x6C);
        modeCtrl.splitscreenMode = var;
    }
    if (merged_digital_inputs[2] & 0x200)
    {
        call_SoundReqID_arg_0(0x6B);
        fn_9_68(0x5D);
        sub_mode_request = 0xD3;
    }
}

void fn_9_AA14(void)
{
    struct Panel *panel = &lbl_9_bss_84.panels[1];
    struct RendEfcRequest req;
    struct Ball *ball;

    s_replayView.view = 0;
    s_replayView.nextView = -1;
    s_replayView.flags = 0;
    s_replayView.voice = -1;
    lbl_1_bss_71EC8.unk14 = 0;
    lbl_1_bss_71EC8.unk0[lbl_1_bss_71EC8.unk14] = 0x11;
    if ((lbl_1_bss_71EC8.unk10 = fn_1_61B18(lbl_1_bss_71EC8.unk0[lbl_1_bss_71EC8.unk14])) == 0.0f)
    {
        sub_mode_request = 0xDD;
        return;
    }
    fn_1_62AA0(lbl_1_bss_71EC8.unk0[lbl_1_bss_71EC8.unk14], &lbl_9_bss_6330);
    current_stage_id = (s16)lbl_9_bss_6330.stageId;
    dest_all_events();
    modeCtrl.gameType = 0;
    modeCtrl.playerCount = 1;
    modeCtrl.unk30 = 1;
    camera_setup_splitscreen_viewports(modeCtrl.playerCount);
    fn_1_56C18();
    g_set_up_debugtext_buffer();
    event_init(9);
    fn_1_618F4(lbl_1_bss_71EC8.unk0[lbl_1_bss_71EC8.unk14]);
    mode_info.flags |= 0x810;
    load_stage(current_stage_id);
    event_init(0);
    event_init(1);
    event_init(2);
    event_init(4);
    event_init(5);
    event_init(7);
    event_init(0xF);
    event_init(0x10);
    event_init(0x11);
    event_init(0x13);
    event_init(0xE);
    event_init(0x15);
    event_init(0x14);
    mode_info.flags |= 8;
    g_init_rendefc_for_stage();
    memset(&req, 0, sizeof(req));
    req.unk6 = 0xFFFF;
    fn_1_D8130(2, 3, &req);
    g_handle_world_bgm(100);
    ball = &balls[lbl_1_bss_71EC8.unk14];
    ball->state = 9;
    fn_9_B790(s_replayView.view);
    mode_info.flags |= 0x10;
    lbl_1_bss_71EC8.unk10 = fn_1_61B18(lbl_1_bss_71EC8.unk0[lbl_1_bss_71EC8.unk14]);
    g_advance_stage_animation();
    load_bmp_by_id(5);
    fade_screen_to_color(0x100, 0, 0x1E);
    panel->unk1C = 0;
    sub_mode_request = 0xE6;
    if (lbl_9_bss_6330.flags & 2)
        modeCtrl.unk18 = 0x3C;
    else
        modeCtrl.unk18 = 0xB4;
}

void fn_9_AD88(void)
{
    struct Ball *ball;
    struct Camera *camera;
    struct Panel *panel;
    struct World *world;
    struct ReplayHeader header;
    struct RendEfcRequest req;

    panel = &lbl_9_bss_84.panels[1];
    ball = &balls[lbl_1_bss_71EC8.unk14];
    camera = &cameras[lbl_1_bss_71EC8.unk14];
    world = &world_infos[lbl_1_bss_71EC8.unk14];
    if (s_paused)
    {
        if (merged_digital_inputs[2] & 0x400)
        {
            s_paused = 0;
            g_some_other_flags &= ~8;
        }
        else if (merged_digital_inputs[4] & 0x100)
        {
            g_some_other_flags &= ~8;
        }
        else
        {
            g_some_other_flags |= 8;
        }
    }
    else if (merged_digital_inputs[2] & 0x400)
    {
        s_paused = 1;
    }
    if (panel->unk1C > 0)
        panel->unk1C--;
    if (panel->unk1C == 1)
    {
        s_paused = 0;
        g_some_other_flags &= ~8;
        light_init(0);
        dest_all_events();
        event_init(0x11);
        event_init(0x13);
        call_free_bmp_by_id(5);
        load_bmp_by_id(4);
        g_fade_track_volume(0x49, 0);
        fade_screen_to_color(0x100, 0, 0x1E);
        fn_9_2DAC();
        sub_mode_request = 0xE2;
        panel->mode = 1;
        return;
    }
    if (panel->unk1C == 0 && (merged_digital_inputs[2] & 0x200) && !(g_some_other_flags & 2))
    {
        panel->unk1C = 0x1F;
        g_fade_track_volume(30, 2);
        fade_screen_to_color(0x101, 0, 0x1E);
    }
    if (ball->state == 10 || !(lbl_9_bss_6330.flags & 2))
    {
        if (merged_digital_inputs[2] & 1)
        {
            s_replayView.nextView = (s16)(s_replayView.view - 1);
            if (s_replayView.nextView < 0)
                s_replayView.nextView = 1;
        }
        if (merged_digital_inputs[2] & 2)
        {
            s_replayView.nextView = (s16)(s_replayView.view + 1);
            if (s_replayView.nextView > 1)
                s_replayView.nextView = 0;
        }
    }
    if (s_replayView.nextView != -1)
    {
        fn_9_B790(s_replayView.nextView);
        s_replayView.view = s_replayView.nextView;
        s_replayView.nextView = -1;
    }
    if (!(s_replayView.flags & 1) && (lbl_9_bss_6330.flags & 2) && ball->state != 10)
    {
        s_replayView.flags |= 1;
        g_smth_with_sound = ball->ape->charaId;
        g_player_id_for_sound = ball->playerId;
        s_replayView.voice = call_SoundReqID_arg_1_copy(0x1D);
    }
    switch (s_replayView.view)
    {
    case 0:
        if (ball->state != 10)
        {
            if (lbl_9_bss_6330.flags & 0x81)
            {
                if (camera->mode != 0xE && camera->mode != 0xF)
                {
                    camera->mode = 0xE;
                    world->state = 2;
                }
            }
            else if (lbl_9_bss_6330.flags & 2)
            {
                if (camera->mode != 4)
                {
                    camera->mode = 4;
                    world->state = 2;
                }
            }
            else if (lbl_9_bss_6330.flags & 4)
            {
                if (ball->state != 0)
                {
                    ball->state = 0;
                    camera->mode = 0x15;
                    world->state = 4;
                }
            }
        }
        break;
    }
    if (ball->state == 4 && (lbl_9_bss_6330.flags & 0x80))
    {
        ball->state = 5;
        ball->flags |= 0x3500;
    }
    if (ball->state != 10)
        mode_info.flags &= ~0x810;
    if (!(g_some_other_flags & 0xA) && ball->state != 10)
        modeCtrl.unk18--;
    if (modeCtrl.unk18 < 0)
    {
        event_dest(0);
        event_dest(4);
        event_dest(5);
        event_dest(0xE);
        event_dest(0x15);
        event_dest(0x14);
        event_dest(2);
        event_dest(0x13);
        fn_1_62AA0(lbl_1_bss_71EC8.unk0[lbl_1_bss_71EC8.unk14], &header);
        fn_1_618F4(lbl_1_bss_71EC8.unk0[lbl_1_bss_71EC8.unk14]);
        mode_info.flags |= 0x10;
        load_stage(current_stage_id);
        event_init(0);
        event_init(4);
        event_init(5);
        event_init(0xE);
        event_init(0x15);
        event_init(0x14);
        event_init(2);
        event_init(0x13);
        mode_info.flags |= 8;
        g_init_rendefc_for_stage();
        memset(&req, 0, sizeof(req));
        req.unk6 = 0xFFFF;
        fn_1_D8130(2, 3, &req);
        light_init(current_stage_id);
        ball->state = 9;
        ball->bananas = 0;
        lbl_1_bss_71EC8.unk10 = fn_1_61B18(lbl_1_bss_71EC8.unk0[lbl_1_bss_71EC8.unk14]);
        g_advance_stage_animation();
        fn_9_B790(s_replayView.view);
        if (lbl_9_bss_6330.flags & 2)
            modeCtrl.unk18 = 0x3C;
        else
            modeCtrl.unk18 = 0xB4;
        s_replayView.flags &= ~1;
        if (s_replayView.voice != -1)
        {
            g_smth_with_sound = ball->ape->charaId;
            g_player_id_for_sound = ball->playerId;
            SoundOff(s_replayView.voice & 0xFFFF);
            s_replayView.voice = -1;
        }
    }
}

#pragma auto_inline off
void fn_9_B51C(void)
{
    struct Panel *panel = &lbl_9_bss_84.panels[1];
    char buf[0x20];
    u32 x;
    u32 y;
    int time;
    int isExtra;

    if (panel->unk1C <= 0 || panel->unk1C >= 3)
    {
        if (lbl_9_bss_6330.flags & 0x20)
            isExtra = 1;
        if (lbl_9_bss_6330.flags & 0x40)
            sprintf(buf, "MASTER %d", lbl_9_bss_6330.floor);
        else if (lbl_9_bss_6330.flags & 0x20)
            sprintf(buf, "EXTRA %d", lbl_9_bss_6330.floor);
        else
            sprintf(buf, "STAGE %d", lbl_9_bss_6330.floor);
        y = 0x1E;
        sprintf(buf, "SCORE:%5d", lbl_9_bss_6330.score);
        x = 0x14;
        y += 0x22;
        textdraw_set_scale(0.8f, 0.8f);
        textdraw_set_mul_color(0xFFFFFF);
        textdraw_set_add_color(0);
        textdraw_set_pos((f32)x, (f32)y);
        textdraw_print(buf);
        time = g_get_replay_time();
        sprintf(buf, " TIME:%5.2f", (f32)(s16)time / 60.0f);
        y += 0x12;
        textdraw_set_pos((f32)x, (f32)y);
        textdraw_print(buf);
        if (frame_counter & 0x30)
        {
            sprintf(buf, "REPLAY");
            textdraw_set_scale(1.0f, 1.0f);
            textdraw_set_mul_color(0xFFFFFF);
            textdraw_set_add_color(0);
            textdraw_set_pos(510.0f, 440.0f);
            textdraw_print(buf);
        }
    }
}

#pragma auto_inline reset

void fn_9_B790(int view)
{
    struct World *world = &world_infos[lbl_1_bss_71EC8.unk14];

    enable_cameras_for_active_players();
    switch (view)
    {
    case 0:
        camera_set_state(0);
        world->state = 8;
        mode_info.flags |= 0x800;
        break;
    case 1:
        camera_set_state(0x2C);
        world->state = 6;
        mode_info.flags &= ~0x800;
        break;
    }
}

void fn_9_B854(void)
{
    struct Panel *panel = &lbl_9_bss_84.panels[6];

    fn_9_64A8();
    panel->sel2 = lbl_9_bss_84.unk39;
    sub_mode_request = 0xE8;
}

void fn_9_B8AC(void)
{
    struct Panel *panel = &lbl_9_bss_84.panels[6];
    int sel = panel->sel2;

    if ((merged_digital_inputs[2] & 8) || (merged_analog_inputs[2] & 8)
     || (merged_digital_inputs[2] & 4) || (merged_analog_inputs[2] & 4))
        sel = !sel;
    if (sel != panel->sel2)
    {
        call_SoundReqID_arg_0(0x6C);
        panel->sel2 = sel;
    }
    if (merged_digital_inputs[2] & 0x100)
    {
        call_SoundReqID_arg_0(0x6A);
        if (panel->sel2 == 0)
        {
            fn_9_0(0x5E);
            sub_mode_request = 0xD7;
        }
        else if (panel->sel2 == 1)
        {
            fn_9_0(0x5E);
            sub_mode_request = 0xE9;
        }
    }
    else if (merged_digital_inputs[2] & 0x200)
    {
        call_SoundReqID_arg_0(0x6B);
        fn_9_68(0x5E);
        sub_mode_request = 0xD3;
    }
}

void fn_9_BA14(void)
{
    lbl_9_bss_84.unk39 = 1;
    fn_9_712C();
    lbl_9_bss_84.panels[8].sel2 = 0;
    sub_mode_request = 0xEA;
}

void fn_9_BA64(void)
{
    int sel;
    u8 flags;
    u8 orig;

    orig = fn_1_FD63C();
    flags = orig;
    sel = lbl_9_bss_84.panels[8].sel2;

    if ((merged_digital_inputs[2] & 8) || (merged_analog_inputs[2] & 8))
    {
        sel--;
        if (sel < 0)
            sel = 3;
    }
    else if ((merged_digital_inputs[2] & 4) || (merged_analog_inputs[2] & 4))
    {
        sel++;
        if (sel >= 4)
            sel = 0;
    }
    if (sel != lbl_9_bss_84.panels[8].sel2)
    {
        call_SoundReqID_arg_0(0x6C);
        lbl_9_bss_84.panels[8].sel2 = sel;
    }
    if ((merged_digital_inputs[2] & 2) || (merged_analog_inputs[2] & 2)
     || (merged_digital_inputs[2] & 1) || (merged_analog_inputs[2] & 1))
    {
        call_SoundReqID_arg_0(0x65);
        flags ^= 1 << sel;
    }
    if (flags != orig)
        fn_1_FD62C(flags);
    if (merged_digital_inputs[2] & 0x200)
    {
        call_SoundReqID_arg_0(0x6B);
        fn_9_68(0x60);
        sub_mode_request = 0xE7;
    }
}

void fn_9_BC24(void)
{
    switch (sub_mode)
    {
    case 0xD7:
    case 0xD8:
        fn_9_99BC();
        break;
    case 0xE5:
    case 0xE6:
        game_main_draw_func();
        fn_9_B51C();
        break;
    }
}

struct Panel *fn_9_BC84(int id)
{
    switch (id)
    {
    case 0x58:
        return &lbl_9_bss_84.panels[0];
    case 0x59:
        return &lbl_9_bss_84.panels[1];
    case 0x5A:
        return &lbl_9_bss_84.panels[2];
    case 0x5B:
        return &lbl_9_bss_84.panels[3];
    case 0x5C:
        return &lbl_9_bss_84.panels[4];
    case 0x5D:
        return &lbl_9_bss_84.panels[5];
    case 0x5E:
        return &lbl_9_bss_84.panels[6];
    case 0x5F:
        return &lbl_9_bss_84.panels[7];
    case 0x60:
        return &lbl_9_bss_84.panels[8];
    case 0x61:
        return &lbl_9_bss_84.panels[9];
    default:
        return NULL;
    }
}
