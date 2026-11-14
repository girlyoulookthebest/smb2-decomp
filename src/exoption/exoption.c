/* Replay viewer: downloads a replay from the memory card (or a save) and plays it back. */
#include "types.h"
#include "ball.h"
#include "camera.h"
#include "event.h"
#include "input.h"
#include "libc.h"
#include "mathutil.h"
#include "mode.h"
#include "os.h"
#include "pool.h"
#include "recplay.h"
#include "sound.h"
#include "sprite.h"
#include "stage.h"
#include "world.h"

/* filled in by the save-file menu before this module loads */
struct ExOptionFile
{
    /*0x00*/ u8 name[0x20];
    /*0x20*/ u32 size;
    /*0x24*/ s32 unk24;
    /*0x28*/ s32 unk28;
};

struct ExOptionLoadRequest
{
    /*0x00*/ u8 unk0;
    /*0x04*/ u32 flags;
    u8 filler8[4];
    /*0x0C*/ void *data;
    /*0x10*/ u32 size;
    /*0x14*/ u8 *name;
}; /* size = 0x18 */

struct RendEfcRequest
{
    u8 filler0[6];
    /*0x06*/ u16 unk6;
    u8 filler8[0x10];
}; /* size = 0x18 */

/* decoded from the hex digits of the file name */
struct ExOptionFileId
{
    u8 unk0[3];
    /*0x03*/ u8 chara;
    /*0x04*/ u16 stageId;
    u8 unk6[6];
};

struct ExOptionWork
{
    /*0x00*/ struct ReplayHeader header;
    /*0x18*/ s16 view;
    /*0x1A*/ s16 nextView;
    /*0x1C*/ u32 flags;
    /*0x20*/ s32 voice;
    /*0x24*/ s32 state;
    /*0x28*/ u8 *fileBuf;
    /*0x2C*/ s8 savedStatus[4];
}; /* size = 0x30 */

extern struct ExOptionFile lbl_1_bss_12AC;
extern int main_heap;
extern u8 STAGE_WORLD_THEMES[];
extern s32 active_monkey_id[];
extern u32 g_some_other_flags;
extern s32 g_smth_with_sound;
extern u32 g_player_id_for_sound;
extern u8 ape_default_anim[];
extern void (*g_some_func_ptr_related_to_sub_mode2)(void);
extern void (*test_draw_func_ptr)(void);

void g_very_similar_to_puts(const char *s);
void OSReport(const char *fmt, ...);
u32 *OSGetStackPointer(void);
void dest_all_events(void);
void fade_screen_to_color(int flags, u32 color, int len);
void fn_1_AEBA0(int id);
void fn_1_1998(int mode);
void fn_1_56C18(void);
void fn_1_5FE3C(int a);
void fn_1_618F4(int replay);
void fn_1_D8130(int a, int b, struct RendEfcRequest *req);
void *alloc_from_heap_or_panic(int heap, u32 size, char *file, int line);
void OSFreeToHeap(int heap, void *ptr);
void g_some_replay_func4(int a, int b, struct ExOptionLoadRequest *req);
void queue_stage_load(int stageId);
void load_stage(int stage);
void preload_ape(int a, int b, int c, int d);
void g_handle_world_bgm(int a);
void g_fade_track_volume(int a, int b);
void SoundIcsReq(u32 a, u8 b, int c);
u8 g_check_some_memcard_field(void);
int g_get_some_memcard_var(void);
int g_get_result_code(void);
int g_smth_with_cmp_recplay(void *src);
void load_bmp_by_id(int id);
void call_free_bmp_by_id(int id);
int g_get_debug_level_select_loading_left_asterisks(void);
void g_return_to_sel_ngc(int a);
void g_set_up_debugtext_buffer(void);
void g_create_stage_player_wrapper(u8 a, int b, u8 c, u8 d, u8 e, u8 *f);
void g_init_rendefc_for_stage(void);
void g_advance_stage_animation(void);
void light_init(int stageId);
int call_SoundReqID_arg_1_copy(u32 soundId);
void game_main_draw_func(void);
void create_replay_hud_sprites(void);

void exoption_draw_func(void);
void fn_20_14C(void);
void fn_20_1DC(void);
void fn_20_290(void);
void fn_20_5F0(void);
void fn_20_9AC(void);
void fn_20_12C8(int view);

u32 lbl_20_data_0[] = {
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

void (*lbl_20_data_170[])(void) = {fn_20_1DC, fn_20_290, fn_20_5F0, fn_20_9AC, NULL};

static struct ExOptionWork lbl_20_bss_0;
static s32 lbl_20_bss_30;

/* decodes the hex digits of a file name into count bytes */
static inline void decode_hex(u8 *src, u8 *dst, int i)
{
    int v;
    u32 c;

    for (; i > 0; i--, dst++)
    {
        c = *src;
        if (c >= '0' && c <= '9')
            v = c - '0';
        else
            v = c - 0x37;
        src++;
        v <<= 4;
        c = *src;
        if (c >= '0' && c <= '9')
            v += c - '0';
        else
            v += c - 0x37;
        src++;
        *dst = (u8)v;
    }
}

void _prolog(void)
{
    dest_all_events();
    g_some_func_ptr_related_to_sub_mode2 = fn_20_14C;
    test_draw_func_ptr = exoption_draw_func;
    g_very_similar_to_puts("EXOPTION LOADED\n");
}

void _epilog(void)
{
    dest_all_events();
    g_some_func_ptr_related_to_sub_mode2 = NULL;
    test_draw_func_ptr = NULL;
    g_very_similar_to_puts("EXOPTION PURGED\n");
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
    OSPanic("exoption.c", 125, "\n");
}

void fn_20_14C(void)
{
    if (sub_mode <= 0x100 || sub_mode >= 0x108)
        printf("sub_mode: error %d in ExOption", sub_mode);
    else
    {
        int i = sub_mode - 0x101;
        lbl_20_data_170[i]();
    }
}

void fn_20_1DC(void)
{
    int i;
    s8 *src;
    s8 *dst;

    lbl_20_bss_0.state = 0;
    sub_mode_request = 0x102;
    fade_screen_to_color(1, 0, 1);
    src = g_poolInfo.playerPool.statusList;
    dst = lbl_20_bss_0.savedStatus;
    for (i = 4; i > 0; i--, src++, dst++)
    {
        *dst = *src;
        *src = 0;
    }
    *g_poolInfo.playerPool.statusList = 2;
    fn_1_AEBA0(5);
    event_init(0x11);
}

void fn_20_290(void)
{
    struct ExOptionLoadRequest req;
    struct ExOptionFileId id;

    switch (lbl_20_bss_0.state)
    {
    case 0:
        lbl_20_bss_0.state = 1;
    case 1:
    {
        u32 size;

        lbl_20_bss_0.state = 2;
        size = lbl_1_bss_12AC.size;
        lbl_20_bss_0.fileBuf = alloc_from_heap_or_panic(main_heap, size, "exoption.c", 0xFB);
        memset(&req, 0, sizeof(req));
        req.unk0 = 2;
        req.name = lbl_1_bss_12AC.name;
        req.data = lbl_20_bss_0.fileBuf;
        req.size = size - 0x2060;
        req.flags |= 0x20000000;
        g_some_replay_func4(1, 3, &req);
        decode_hex(lbl_1_bss_12AC.name, (u8 *)&id, 12);
        {
            int stage = id.stageId;
            s16 theme;

            if (current_stage_id != stage)
                queue_stage_load(stage);
            preload_ape(id.chara, 0, 0, 1);
            theme = world_theme;
            world_theme = STAGE_WORLD_THEMES[stage];
            g_handle_world_bgm(100);
            world_theme = (s16)theme;
        }
    }
    case 2:
        if (g_check_some_memcard_field() == 0)
        {
            lbl_1_bss_12AC.unk24 = g_get_some_memcard_var();
            lbl_1_bss_12AC.unk28 = g_get_result_code();
            if (lbl_1_bss_12AC.unk24 == 0)
            {
                OSReport("result_code: %d ", g_get_result_code());
                lbl_20_bss_0.state = 6;
            }
            else
            {
                lbl_20_bss_0.state = 3;
                g_smth_with_cmp_recplay(lbl_20_bss_0.fileBuf + 0x2074);
            }
            OSFreeToHeap(main_heap, lbl_20_bss_0.fileBuf);
        }
        break;
    case 3:
        lbl_20_bss_0.state = 4;
        lbl_1_bss_71EC8.unk14 = 0;
        lbl_1_bss_71EC8.unk0[lbl_1_bss_71EC8.unk14] = 0x11;
        fn_1_618F4(lbl_1_bss_71EC8.unk0[lbl_1_bss_71EC8.unk14]);
        load_bmp_by_id(5);
        create_replay_hud_sprites();
    case 4:
        if (g_get_debug_level_select_loading_left_asterisks() <= 0)
            lbl_20_bss_0.state = 5;
        break;
    case 5:
        sub_mode_request = 0x103;
        break;
    case 6:
        g_return_to_sel_ngc(-1);
        break;
    }
}

void fn_20_5F0(void)
{
    struct RendEfcRequest req;
    struct Ball *ball;

    lbl_1_bss_71EC8.unk14 = 0;
    lbl_1_bss_71EC8.unk0[lbl_1_bss_71EC8.unk14] = 0x11;
    fn_1_62AA0(lbl_1_bss_71EC8.unk0[lbl_1_bss_71EC8.unk14], &lbl_20_bss_0.header);
    if ((lbl_20_bss_0.header.flags & 0x200) || (lbl_20_bss_0.header.flags & 0x100))
    {
        g_return_to_sel_ngc(-1);
        return;
    }
    current_stage_id = (s16)lbl_20_bss_0.header.stageId;
    modeCtrl.gameType = 0;
    modeCtrl.currPlayer = 0;
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
    event_init(3);
    event_init(4);
    event_init(5);
    event_init(7);
    event_init(0xF);
    event_init(0x10);
    event_init(0x13);
    event_init(0xE);
    event_init(0x15);
    event_init(0x14);
    g_create_stage_player_wrapper(0, 2, active_monkey_id[0], 0xFF, 0, ape_default_anim);
    balls[0].unk179 = 3;
    balls[0].bananas = 0;
    enable_cameras_for_active_players();
    enable_camera(modeCtrl.currPlayer);
    mode_info.flags |= 8;
    g_init_rendefc_for_stage();
    memset(&req, 0, sizeof(req));
    req.unk6 = 0xFFFF;
    fn_1_D8130(2, 3, &req);
    g_handle_world_bgm(100);
    ball = &balls[lbl_1_bss_71EC8.unk14];
    ball->state = 9;
    fn_20_12C8(lbl_20_bss_0.view);
    mode_info.flags |= 0x10;
    lbl_1_bss_71EC8.unk10 = fn_1_61B18(lbl_1_bss_71EC8.unk0[lbl_1_bss_71EC8.unk14]);
    g_advance_stage_animation();
    if (current_stage_id != 3)
        queue_stage_load(3);
    fn_1_AEBA0(0x17);
    fn_1_1998(1);
    fade_screen_to_color(0, 0, 0x1E);
    sub_mode_request = 0x104;
    if (lbl_20_bss_0.header.flags & 2)
        modeCtrl.unk18 = 0x3C;
    else
        modeCtrl.unk18 = 0xB4;
    modeCtrl.submodeTimer = 0;
}

void fn_20_9AC(void)
{
    struct Ball *ball = &balls[lbl_1_bss_71EC8.unk14];
    struct Camera *camera = &cameras[lbl_1_bss_71EC8.unk14];
    struct World *world = &world_infos[lbl_1_bss_71EC8.unk14];
    struct ReplayHeader header;
    struct ReplayHeader header2;
    struct RendEfcRequest req;

    header = lbl_20_bss_0.header;
    if (lbl_20_bss_30)
    {
        if (merged_digital_inputs[2] & 0x400)
        {
            lbl_20_bss_30 = 0;
            g_some_other_flags &= ~8;
            g_fade_track_volume(100, 10);
        }
        else if (merged_digital_inputs[4] & 0x100)
        {
            g_some_other_flags &= ~8;
        }
        else
        {
            g_some_other_flags |= 8;
            g_fade_track_volume(50, 10);
            SoundIcsReq(ball->playerId, 0, 0);
        }
    }
    else if (merged_digital_inputs[2] & 0x400)
    {
        lbl_20_bss_30 = 1;
    }
    if (modeCtrl.submodeTimer > 0)
        modeCtrl.submodeTimer--;
    if (modeCtrl.submodeTimer == 1)
    {
        int i;
        s8 *dst;
        s8 *src;

        lbl_20_bss_30 = 0;
        g_some_other_flags &= ~8;
        dest_all_events();
        event_init(0x11);
        event_init(0x13);
        call_free_bmp_by_id(5);
        fn_1_5FE3C(0);
        fade_screen_to_color(0x100, 0, 0x1E);
        dst = g_poolInfo.playerPool.statusList;
        src = lbl_20_bss_0.savedStatus;
        for (i = 4; i > 0; i--, dst++, src++)
            *dst = *src;
        g_return_to_sel_ngc(-1);
        return;
    }
    if (modeCtrl.submodeTimer == 0 && (merged_digital_inputs[2] & 0x200) && !(g_some_other_flags & 2))
    {
        modeCtrl.submodeTimer = 0x1F;
        g_fade_track_volume(30, 2);
        fade_screen_to_color(0x101, 0, 0x1E);
    }
    if (ball->state == 10 || !(header.flags & 2))
    {
        if (merged_digital_inputs[2] & 1)
        {
            lbl_20_bss_0.nextView = (s16)(lbl_20_bss_0.view - 1);
            if (lbl_20_bss_0.nextView < 0)
                lbl_20_bss_0.nextView = 1;
        }
        if (merged_digital_inputs[2] & 2)
        {
            lbl_20_bss_0.nextView = (s16)(lbl_20_bss_0.view + 1);
            if (lbl_20_bss_0.nextView > 1)
                lbl_20_bss_0.nextView = 0;
        }
    }
    if (lbl_20_bss_0.nextView != -1)
    {
        fn_20_12C8(lbl_20_bss_0.nextView);
        lbl_20_bss_0.view = lbl_20_bss_0.nextView;
        lbl_20_bss_0.nextView = -1;
    }
    if (!(lbl_20_bss_0.flags & 1) && (header.flags & 2) && ball->state != 10)
    {
        lbl_20_bss_0.flags |= 1;
        g_smth_with_sound = ball->ape->charaId;
        g_player_id_for_sound = ball->playerId;
        lbl_20_bss_0.voice = call_SoundReqID_arg_1_copy(0x1D);
    }
    switch (lbl_20_bss_0.view)
    {
    case 0:
        if (ball->state != 10)
        {
            if (header.flags & 0x81)
            {
                if (camera->mode != 0xE && camera->mode != 0xF)
                {
                    camera->mode = 0xE;
                    world->state = 2;
                }
            }
            else if (header.flags & 2)
            {
                if (camera->mode != 4)
                {
                    camera->mode = 4;
                    world->state = 2;
                }
            }
            else if (header.flags & 4)
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
    if (ball->state == 4 && (header.flags & 0x80))
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
        event_dest(3);
        event_dest(0x13);
        fn_1_62AA0(lbl_1_bss_71EC8.unk0[lbl_1_bss_71EC8.unk14], &header2);
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
        event_init(3);
        event_init(0x13);
        g_create_stage_player_wrapper(0, 2, active_monkey_id[0], 0xFF, 0, ape_default_anim);
        balls[0].unk179 = 3;
        balls[0].bananas = 0;
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
        fn_20_12C8(lbl_20_bss_0.view);
        if (header.flags & 2)
            modeCtrl.unk18 = 0x3C;
        else
            modeCtrl.unk18 = 0xB4;
        lbl_20_bss_0.flags &= ~1;
        if (lbl_20_bss_0.voice != -1)
        {
            g_smth_with_sound = ball->ape->charaId;
            g_player_id_for_sound = ball->playerId;
            SoundOff(lbl_20_bss_0.voice & 0xFFFF);
            lbl_20_bss_0.voice = -1;
        }
    }
}

void fn_20_12C8(int view)
{
    struct World *world = &world_infos[lbl_1_bss_71EC8.unk14];

    enable_cameras_for_active_players();
    switch (view)
    {
    case 0:
        camera_set_state(0x4B);
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

void exoption_draw_func(void)
{
    game_main_draw_func();
}
