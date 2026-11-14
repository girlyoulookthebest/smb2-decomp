/* Attract mode: logos, title screen, how-to-play demo, replays and ranking. */
#include "types.h"
#include "ball.h"
#include "camera.h"
#include "dip.h"
#include "event.h"
#include "input.h"
#include "libc.h"
#include "light.h"
#include "macros.h"
#include "mathutil.h"
#include "mode.h"
#include "pool.h"
#include "recplay.h"
#include "sprite.h"
#include "stage.h"
#include "world.h"

#define ANY_CONTROLLER_PRESSED(btn) \
( \
    (pad_status_groups[0].unk0[2].button & (btn)) \
 || (pad_status_groups[1].unk0[2].button & (btn)) \
 || (pad_status_groups[2].unk0[2].button & (btn)) \
 || (pad_status_groups[3].unk0[2].button & (btn)) \
)

#define DEBUG_SKIP_PRESSED() \
( \
    (pad_status_groups[0].unk0[0].button & 0x400) \
 && (pad_status_groups[0].unk0[2].button & 0x800) \
)

#define CAN_SKIP(minTimer) \
( \
    !(modeCtrl.courseFlags & 0x2000) \
 && modeCtrl.submodeTimer > (minTimer) \
 && lbl_1_bss_1534 == 0 \
 && ANY_CONTROLLER_PRESSED(0x1000) \
)

struct AdvLogoInfo
{
    /*0x00*/ Vec pos;
    /*0x0C*/ s16 xrot;
    /*0x0E*/ s16 zrot;
    /*0x10*/ s32 rollTimer;
    /*0x14*/ s32 timer;
}; /* size = 0x18 */

struct Unk1568
{
    /*0x00*/ s32 unk0;
    /*0x04*/ s32 unk4;
    /*0x08*/ s32 unk8;
    /*0x0C*/ float unkC;
}; /* size = 0x10 */

struct ScreenFadeInfo
{
    /*0x00*/ s32 type;
    /*0x04*/ u32 color;
    /*0x08*/ s32 timer;
    /*0x0C*/ s32 timerMax;
};

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

/* One scripted step of the how-to-play demo. */
struct AdvInfoScript
{
    /*0x00*/ s16 time;  /* submode timer value this step fires on; 0 ends the list */
    /*0x02*/ s8 id;  /* message index when >= 0, otherwise a command */
    /*0x04*/ char *msg;  /* Japanese message; unused by the code */
    /*0x08*/ u32 param;
}; /* size = 0xC */

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
#ifdef __MWERKS__
    u8 filler8[0x10];
#else
    /* PC: same layout as rend_efc.c's struct RenderEffect (fn_1_D8130 copies
     * sizeof(struct RenderEffect), 0x20 on x64) */
    u8 filler8[8];
    void *work;
    void *funcs;
#endif
}; /* size = 0x18 */

extern void (*sub_mode_funcs[])(void);
extern s32 active_monkey_id[];
extern s32 locale;
extern struct ScreenFadeInfo g_screenfade_flags;
extern u32 lbl_1_bss_13C4;
extern struct TextBox lbl_1_bss_A5C40[];
extern s32 lbl_1_bss_CF3D4;
extern u8 storymode_save_files[];
extern s32 main_mode_request;
extern u32 g_player_id_for_sound;

void ape_default_anim(struct Ape *ape);
void OSReport(char *fmt, ...);
void SoundIcsReq(u32 a, u8 b, int c);
int adv_demo_tick_function(void);
void call_SoundReqID_arg_0(int id);
void call_SoundReqID_arg_1(int id);
void call_free_bmp_by_id(int id);
void camera_setup_singleplayer_viewport(void);
void camera_setup_splitscreen_viewports(int count);
void create_copyright_text_sprites(int arg);
void create_go_sprite(int time);
void create_player_num_and_ready_sprites(int time);
void create_title_sprite(int arg);
void create_titlescreen_press_start_sprites(int arg);
void debug_textdraw_set_pos(int x, int y);
void debug_textdraw_vsprintf_buf_1(char *fmt, ...);
void dest_all_events(void);
void destroy_sprite_with_unique_id(int id);
void draw_sega_splash_wrapper(void);
void enable_cameras_for_active_players(void);
void fade_screen_to_color(int a, int b, int c);
void fn_1_16A9D0(void);
void fn_1_16AA00(int stage);
void fn_1_16AA88(void (*func)(void));
void fn_1_16AAA8(void (*func)(void));
void fn_1_16AAB8(void (*func)(void));
void fn_1_16AAC8(void);
void fn_1_3061C(int a, int b);
void fn_1_34828(void);
void fn_1_36140(void);
void fn_1_56C18(void);
int fn_1_5FC54(void);
void fn_1_618F4(int replay);
void fn_1_63F54(int replay);
void fn_1_D0EC4(void);
void fn_1_D0FA4(int arg);
void fn_1_D1220(int arg);
void fn_1_D12CC(void);
void fn_1_D13D0(void);
void fn_1_D1828(void);
void fn_1_D1900(void);
void fn_1_D19FC(int arg);
void fn_1_D391C(int arg);
void fn_1_D8130(int a, int b, struct RendEfcRequest *req);
int fn_1_EE6C(void);
void fn_1_EFF6C(int slot, int a, int b, int c);
void fn_1_EFFB0(void);
int fn_1_F00BC(int slot);
void fn_1_F660(void);
void fn_8000A71C(void);
void free_minigame_gma_tpl(void);
void g_SoftStreamStart_with_some_defaults_2(int id);
void g_advance_stage_animation(void);
void g_assign_new_main_and_sub_mode_for_play_tick(int a, int b);
u8 g_check_some_memcard_field(void);
void g_create_smd_adv_logo_sprites(void);
void g_create_some_controls_description_sprite(void);
void g_create_stage_player_wrapper(int a, int b, u8 c, int d, int e, void *f);
void g_display_playpoint_or_gift_message_child(int id, int state, struct TextBox *tb);
void g_fade_track_volume(int a, int b);
int g_get_author_frame(void);
int g_get_author_frame_max(void);
int g_get_author_scene(void);
int g_get_author_scene_max(void);
int g_get_last_used_memcard_slot(void);
int g_get_result_code(void);
int g_get_some_memcard_var(void);
void g_handle_world_bgm(int a);
void g_init_rendefc_for_stage(void);
void g_load_minigame_gma_tpl(int a);
void g_preload_next_stage_files(int stage, int a, int b);
void g_save_game_data(void);
void g_set_smth_with_sound(int a);
void g_set_some_author_tick_func(void (*func)(void));
void g_set_up_debugtext_buffer(void);
void g_smth_with_ape_rotation(struct Ape *ape, Vec *v);
void g_smth_with_playpoint_or_gift_msg(int id, char *msg);
void g_some_loading_function(void);
void g_some_replay_func4(int a, int b, struct MemcardRequest *req);
void g_something_w_adv_demo_init(void);
void g_something_with_init_or_maybe_leftover_from_smb1(int a);
void g_something_with_iteratively_freeing_memory(void);
void g_something_with_loading_stages_on_gamestart(void);
void g_something_with_translating_items(int a, Vec *v, float f);
void g_sprintf_memcard_error(int a, char *buf);
int get_load_group_status(void);
struct Sprite *get_sprite_with_unique_id(int id);
void load_bmp_by_id(int id);
void load_bmp_by_id_child(int id);
void load_stage(int stageId);
void preload_ape(int a, int b, int c, int d);
void queue_disc_read(char *file);
void queue_stage_load(int stageId);
void set_minimap_mode(int mode);
void set_sub_mode_destination(void (*func)(void));
void smd_game_over_save_child(void);
void toggle_minimap_zoom(void);
void unload_stage(void);

void fn_1_C394(void);
void fn_1_CC94(void);
int fn_1_D77C(void);
int fn_1_D8A0(void);
int fn_1_DA34(void);
void fn_1_DA88(void);
int return_0x2c(void);
void fn_1_E75C(void);
void g_init_some_globals_for_adv(void);
void g_smth_with_adv_sprites_and_stage_loading(int arg);

s32 g_sub_mode3;
s32 lbl_1_bss_1534;
s32 lbl_1_bss_1538;
u32 lbl_1_bss_153C;
struct AdvLogoInfo lbl_1_bss_1540;
Vec lbl_1_bss_1558;
s32 adv_demo_frame_counter;
struct Unk1568 lbl_1_bss_1568;
s32 lbl_1_bss_1578;
s32 lbl_1_bss_157C;

/* part 1 */
u8 lbl_1_data_2F68[] =
{
    0xFE, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0xFE, 0x0D,
    0x0E, 0x0F, 0xFF, 0x00, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B,
    0x0C, 0x0D, 0x0E, 0x0F, 0xFF, 0x00, 0x00, 0x00, 0x31, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x38, 0x00, 0xB4, 0x00, 0x00, 0x01, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00,
    0x10, 0x00, 0x4C, 0x00, 0x00, 0x03, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00,
    0x18, 0x00, 0x00, 0x00, 0x00, 0x09, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x01, 0x00, 0x00, 0x00, 0x00, 0x0B, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x0C, 0x00, 0x00,
    0x18, 0x00, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x01, 0x00, 0x00, 0x00, 0x00, 0x07, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00,
    0x38, 0x00, 0xB4, 0x00, 0x00, 0x0D, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x18, 0x00, 0x00, 0x00, 0x00, 0x14, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x01, 0x00, 0x00, 0x00, 0x00, 0x16, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x17, 0x00, 0x00,
    0x18, 0x00, 0x00, 0x00, 0x00, 0x18, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x0F, 0x00, 0x00,
    0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x11, 0x00, 0x00,
    0x18, 0x00, 0x00, 0x00, 0x00, 0x12, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x13, 0x00, 0x00,
    0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00,
    0x18, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00,
    0x18, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00,
    0x18, 0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x07, 0x00, 0x00,
    0x18, 0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x09, 0x00, 0x00,
    0x18, 0x00, 0x00, 0x00, 0x00, 0x0A, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x0B, 0x00, 0x00,
    0x18, 0x00, 0x00, 0x00, 0x00, 0x0C, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x0D, 0x00, 0x00,
    0x18, 0x00, 0x00, 0x00, 0x00, 0x0E, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x0F, 0x00, 0x00,
};

s8 lbl_1_data_30D8[3] = { 0xC, 0xD, 0xE };
s8 lbl_1_data_30DB = 3;

void md_adv_func(void)
{
    if (sub_mode <= 0 || sub_mode >= 0x1B)
    {
        debug_textdraw_set_pos(10, 10);
        debug_textdraw_vsprintf_buf_1("sub_mode: error %d in Adv", sub_mode);
        return;
    }
    if (!(sub_mode == 9 || sub_mode == 10))
        g_sub_mode3 = sub_mode;
    if (!(g_some_other_flags & 0xA) && lbl_1_bss_1534 > 0 && --lbl_1_bss_1534 == 0)
        sub_mode_request = 0x13;
    sub_mode_funcs[sub_mode]();
}

void fn_1_A4CC(void) {}

void fn_1_A4D0(void) {}

void smd_adv_logo_init(void)
{
    if (g_some_other_flags & 0xA)
        return;
    lbl_1_bss_1538 = modeCtrl.wideMode != 0;
    fn_1_34828();
    fn_1_36140();
    g_init_some_globals_for_adv();
    lbl_1_bss_153C = 0xFFFFFF;
    modeCtrl.submodeTimer = 840;
    modeCtrl.courseFlags = 0;
    lbl_1_bss_1540.timer = modeCtrl.submodeTimer;
    lbl_1_bss_1540.rollTimer = 0;
    lbl_1_bss_1558.x = 0.0f;
    lbl_1_bss_1558.y = 0.0f;
    lbl_1_bss_1558.z = 0.0f;
    g_set_up_debugtext_buffer();
    light_init(0);
    active_monkey_id[0] = 0;
    dest_all_events();
    g_something_with_iteratively_freeing_memory();
    event_init(0x10);
    event_init(0x11);
    event_init(0x13);
    enable_cameras_for_active_players();
    camera_set_state(0x1B);
    unload_stage();
    load_bmp_by_id(1);
    g_create_smd_adv_logo_sprites();
    call_SoundReqID_arg_0(0x21);
    if (lbl_1_data_30DB == sizeof(lbl_1_data_30D8))
        g_something_with_loading_stages_on_gamestart();
    else
    {
        queue_stage_load(1);
        g_preload_next_stage_files(lbl_1_data_30D8[lbl_1_data_30DB], -1, 0);
    }
    light_init(0);
    fade_screen_to_color(0x100, 0, 30);
    sub_mode_request = 4;
}

void fn_1_A6A8(void)
{
    if (lbl_1_bss_1540.rollTimer <= 120)
        lbl_1_bss_1540.pos.x = 0.0f;
    else if (lbl_1_bss_1540.rollTimer <= 240)
        lbl_1_bss_1540.pos.x = 4.5 * (1.0 - fabs(math_sin((lbl_1_bss_1540.rollTimer - 120) * 0x111 + 0x4000)));

    if (lbl_1_bss_1540.rollTimer <= 180)
        lbl_1_bss_1540.pos.y = -0.004 * (180 - lbl_1_bss_1540.rollTimer);

    if (lbl_1_bss_1540.rollTimer <= 180)
        lbl_1_bss_1540.pos.z = 280.0 * (1.0 - math_sin(lbl_1_bss_1540.rollTimer * 0x5B));
    else if (lbl_1_bss_1540.rollTimer > 300 && lbl_1_bss_1540.rollTimer <= 420)
        lbl_1_bss_1540.pos.z = 1.35 * (lbl_1_bss_1540.rollTimer - 300);

    if (lbl_1_bss_1540.rollTimer <= 120)
        lbl_1_bss_1540.zrot = 0;
    else if (lbl_1_bss_1540.rollTimer <= 180)
        lbl_1_bss_1540.zrot = (lbl_1_bss_1540.rollTimer - 120) * -0x222;
    else if (lbl_1_bss_1540.rollTimer <= 240)
        lbl_1_bss_1540.zrot = (240 - lbl_1_bss_1540.rollTimer) * -0x222;

    if (lbl_1_bss_1540.rollTimer <= 180)
        lbl_1_bss_1540.xrot = lbl_1_bss_1540.rollTimer * -0x222;
    else if (lbl_1_bss_1540.rollTimer <= 300)
        lbl_1_bss_1540.xrot = 0;
    else if (lbl_1_bss_1540.rollTimer <= 420)
        lbl_1_bss_1540.xrot = (lbl_1_bss_1540.rollTimer - 300) * 0x222;

    if (lbl_1_bss_1540.rollTimer > 0)
    {
        s8 r4;
        int r5;
        s8 r29, r28;
        float speed = math_sqrt(
            (lbl_1_bss_1540.pos.x - lbl_1_bss_1558.x) * (lbl_1_bss_1540.pos.x - lbl_1_bss_1558.x)
          + (lbl_1_bss_1540.pos.z - lbl_1_bss_1558.z) * (lbl_1_bss_1540.pos.z - lbl_1_bss_1558.z));

        speed *= 10.0;
        r4 = MIN(1.5 * speed, 127.0);
        r5 = 0.85000002384185791 * MIN(15.0 * speed, 80.0);
        r29 = CLAMP(16.0 * lbl_1_bss_1540.pos.x, -64.0, 63.0);
        r28 = CLAMP(0.1 * (280.0 - lbl_1_bss_1540.pos.z), -64.0, 63.0);
        g_player_id_for_sound = 0;
        SoundIcsReq(((r29 & 0x7F) << 18) | ((r28 & 0x7F) << 25), r4, r5);
    }
    lbl_1_bss_1558 = lbl_1_bss_1540.pos;
}

static inline void update_av_logo(void)
{
    if (lbl_1_bss_1540.timer != 0)
    {
        if (lbl_1_bss_1540.timer <= 660)
            lbl_1_bss_1540.rollTimer++;
        fn_1_A6A8();
        if (lbl_1_bss_1540.timer == 460)
            call_SoundReqID_arg_0(0x3B1F4);
        if (lbl_1_bss_1540.timer == 240)
            fn_1_3061C(0x51F2, 0x51F3);
        if (lbl_1_bss_1540.timer < 240 && lbl_1_bss_1540.timer >= 210)
        {
            int t = 240 - lbl_1_bss_1540.timer;

            lbl_1_bss_153C = t >= 30
              ? 0xFFC000
              : 0xFF0000 | ((int)(255.0 - 2.1 * t) << 8) | (int)(255.0 - 8.5 * t);
        }
        if (lbl_1_bss_1540.timer > 0)
            lbl_1_bss_1540.timer--;
    }
}

void smd_adv_logo_tick(void)
{
    if (g_some_other_flags & 0xA)
        return;
    if ((dip_switches & 1) && modeCtrl.submodeTimer > 30 && DEBUG_SKIP_PRESSED())
        modeCtrl.submodeTimer = 30;
    update_av_logo();
    if (CAN_SKIP(60) && modeCtrl.submodeTimer < 0x2B2)
    {
        g_smth_with_adv_sprites_and_stage_loading(30);
        g_fade_track_volume(0x36, 0);
    }
    if (!(modeCtrl.courseFlags & 0x2000))
    {
        if (modeCtrl.submodeTimer == 30)
        {
            fade_screen_to_color(0x101, 0, 30);
            g_display_playpoint_or_gift_message_child(0, 20, NULL);
        }
        if (--modeCtrl.submodeTimer <= 0)
        {
            destroy_sprite_with_unique_id(3);
            sub_mode_request = lbl_1_data_30DB == sizeof(lbl_1_data_30D8) ? 7 : 1;
        }
    }
}

void smd_adv_demo_init(void)
{
    if (g_some_other_flags & 0xA)
        return;
    if (fn_1_EE6C())
    {
        g_init_some_globals_for_adv();
        adv_demo_frame_counter = 0;
        g_something_w_adv_demo_init();
        lbl_1_data_30DB = 0;
        set_sub_mode_destination(fn_1_F660);
        sub_mode_request = 8;
    }
}

void smd_adv_demo_tick(void)
{
    int r = adv_demo_tick_function();

    if (g_some_other_flags & 0xA)
        return;
    if (adv_demo_frame_counter > 0)
    {
        if (--adv_demo_frame_counter == 0)
            sub_mode_request = 2;
    }
    else if (r != 0)
        adv_demo_frame_counter = 60;
}

void fn_1_B08C(void)
{
    fade_screen_to_color(0x100, 0, 30);
    modeCtrl.submodeTimer = 60;
    modeCtrl.unk48 = 0;
    event_init(0x11);
    create_titlescreen_press_start_sprites(3);
    create_copyright_text_sprites(0);
}

void fn_1_B0F0(void)
{
    switch (modeCtrl.unk48)
    {
    case 0:
        if (g_get_author_scene() == g_get_author_scene_max() - 1
         && g_get_author_frame() >= g_get_author_frame_max() - 60)
        {
            fade_screen_to_color(0x101, 0, 60);
            modeCtrl.unk48 = 2;
        }
        else if (!(modeCtrl.courseFlags & 0x2000)
              && modeCtrl.submodeTimer > 0
              && lbl_1_bss_1534 == 0
              && ANY_CONTROLLER_PRESSED(0x1000))
        {
            modeCtrl.submodeTimer = 30;
            fade_screen_to_color(0x101, 0, modeCtrl.submodeTimer);
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

void fn_1_B2A8(void)
{
    lbl_1_data_30DB++;
}

void fn_1_B2C8(void) {}

void test_mode_sub_mode_handler(void)
{
    dest_all_events();
    modeCtrl.courseFlags &= ~4;
    fn_1_16A9D0();
    fn_1_16AA00(lbl_1_data_30D8[lbl_1_data_30DB]);
    g_assign_new_main_and_sub_mode_for_play_tick(0, 2);
    fn_1_16AA88(fn_1_B08C);
    g_set_some_author_tick_func(fn_1_B0F0);
    fn_1_16AAA8(fn_1_B2A8);
    fn_1_16AAB8(fn_1_B2C8);
    main_mode_request = 6;
    sub_mode_request = 0xF7;
}

void smd_adv_movie_return(void)
{
    if (!(modeCtrl.courseFlags & 4))
    {
        dest_all_events();
        event_init(0x11);
        load_bmp_by_id(1);
        fade_screen_to_color(0x100, 0, 30);
        g_smth_with_adv_sprites_and_stage_loading(1);
        modeCtrl.courseFlags |= 4;
    }
}

void g_smth_with_adv_sprites_and_stage_loading(int arg)
{
    struct Sprite *sprite;

    modeCtrl.courseFlags &= ~1;
    modeCtrl.courseFlags &= ~2;
    destroy_sprite_with_unique_id(0x71);
    sprite = get_sprite_with_unique_id(0x25);
    if (sprite != NULL)
        sprite->userVar = -1;
    sprite = get_sprite_with_unique_id(0x26);
    if (sprite != NULL)
        sprite->userVar = -1;
    sprite = get_sprite_with_unique_id(0x27);
    if (sprite != NULL)
        sprite->userVar = -1;
    g_display_playpoint_or_gift_message_child(1, 20, NULL);
    g_display_playpoint_or_gift_message_child(2, 20, NULL);
    g_display_playpoint_or_gift_message_child(3, 20, NULL);
    sprite = get_sprite_with_unique_id(0x1E);
    if (sprite != NULL)
        sprite->counter = -1;
    sprite = get_sprite_with_unique_id(0x1F);
    if (sprite != NULL)
        sprite->counter = -1;
    sprite = get_sprite_with_unique_id(0x20);
    if (sprite != NULL)
        sprite->counter = -1;
    g_display_playpoint_or_gift_message_child(0, 20, NULL);
    sprite = get_sprite_with_unique_id(0x11);
    if (sprite != NULL && sprite->userVar != 2)
        sprite->userVar = 1;
    create_title_sprite(arg);
    call_SoundReqID_arg_0(0xA022);
    g_fade_track_volume(0x36, 0);
    fn_1_D391C(get_load_group_status());
    queue_disc_read("mkb2.sel_ngc.rel");
    queue_stage_load(3);
    queue_disc_read("bmp\bmp_sel2.tpl");
    queue_disc_read("bmp\bmp_how.tpl");
    queue_disc_read("bmp\bmp_nml.tpl");
    modeCtrl.courseFlags |= 0x2000;
    lbl_1_bss_1534 = arg;
}

void smd_adv_title_init(void)
{
    if (g_some_other_flags & 0xA)
        return;
    fn_1_F660();
    lbl_1_bss_1538 = modeCtrl.wideMode != 0;
    modeCtrl.submodeTimer = 0x4B0;
    modeCtrl.unk10 = 0;
    modeCtrl.courseFlags |= 4;
    lbl_1_bss_1534 = 0;
    event_dest(2);
    event_dest(0);
    event_dest(4);
    event_dest(5);
    event_dest(7);
    event_dest(0x10);
    event_dest(0xE);
    event_dest(0x13);
    event_dest(0x15);
    event_dest(0x14);
    destroy_sprite_with_unique_id(0x25);
    destroy_sprite_with_unique_id(0x26);
    enable_cameras_for_active_players();
    create_titlescreen_press_start_sprites(2);
    if (g_screenfade_flags.timer != 0)
        fade_screen_to_color(0x100, 0, 30);
    queue_disc_read("bmp/bmp_sel.tpl");
    sub_mode_request = 0x14;
}

void smd_adv_title_reinit(void)
{
    if (g_some_other_flags & 0xA)
        return;
    g_init_some_globals_for_adv();
    lbl_1_bss_1538 = modeCtrl.wideMode != 0;
    lbl_1_bss_153C = 0xFFFFFF;
    modeCtrl.submodeTimer = 0x4B0;
    modeCtrl.unk10 = 0;
    modeCtrl.courseFlags = 0x2000;
    modeCtrl.courseFlags |= 4;
    lbl_1_bss_1534 = 0;
    g_set_up_debugtext_buffer();
    light_init(0);
    active_monkey_id[0] = 0;
    dest_all_events();
    g_something_with_iteratively_freeing_memory();
    event_init(0x11);
    event_init(0x13);
    camera_set_state(0x1B);
    unload_stage();
    destroy_sprite_with_unique_id(0x25);
    destroy_sprite_with_unique_id(0x26);
    load_bmp_by_id(1);
    create_title_sprite(1);
    create_titlescreen_press_start_sprites(2);
    load_stage(0x96);
    light_init(0);
    call_SoundReqID_arg_0(0xA022);
    g_fade_track_volume(0x36, 0);
    fade_screen_to_color(0x100, 0, 30);
    sub_mode_request = 0x14;
}

void smd_adv_title_tick(void)
{
    if ((dip_switches & 1) && DEBUG_SKIP_PRESSED() && modeCtrl.submodeTimer > 30)
        modeCtrl.submodeTimer = 30;
    if (g_some_other_flags & 0xA)
        return;
    if (lbl_1_bss_A5C40[0].state == 10
     && lbl_1_bss_A5C40[0].timer >= 0xF
     && !(dip_switches & 1)
     && !(modeCtrl.courseFlags & 4))
        modeCtrl.courseFlags |= 4;
#ifndef __MWERKS__
    /* PC port: B (or Esc) on the title screen opens the "Quit to Desktop?"
     * yes/no box (pc_quit.c); hint "B:Quit" next to PRESS START (hud.c).
     * While it is open the title is frozen: the timer to the attract demo
     * stops and START (mode.c) only closes the box. */
    {
        int pc_menu_quit_tick(int where, int can_open, unsigned int open_buttons);

        switch (pc_menu_quit_tick(1 /* PCMQ_TITLE */,
                                  (modeCtrl.courseFlags & 4) && !(modeCtrl.courseFlags & 2)
                                   && lbl_1_bss_1534 == 0 && modeCtrl.submodeTimer > 30
                                   && sub_mode_request == -1 && main_mode_request == -1,
                                  PAD_BUTTON_B))
        {
        case -1:
            break;
        case 1: /* PCQ_MOVED */
            call_SoundReqID_arg_1(0x6F);
            return;
        case 2: /* PCQ_NO */
            call_SoundReqID_arg_1(0x70);
            return;
        case 3: /* PCQ_YES */
        case 4: /* PCQ_OPENED */
            call_SoundReqID_arg_1(0x6E);
            return;
        default:
            return;
        }
    }
#endif
    if (modeCtrl.submodeTimer == 30)
    {
        fade_screen_to_color(0x101, 0, 30);
        g_display_playpoint_or_gift_message_child(0, 20, NULL);
        g_fade_track_volume(modeCtrl.submodeTimer, 2);
    }
    if (--modeCtrl.submodeTimer <= 0)
    {
        lbl_1_bss_13C4 = 0;
        sub_mode_request = 0xD;
    }
}

char lbl_1_data_314C[] = "/bcff8000/\203Q\201[\203\200\203X\203^\201[\203g";

char *lbl_1_data_3204[] =
{
    "   Controls description!   ",
    "   Steuerungsfunktionen!   ",
    "   Les commandes   ",
    "   Descripci\363n controles   ",
    "   Descrizione comandi!   ",
    "   \202\273\202\244\202\263\202\271\202\302\202\337\202\242   ",
};

void smd_adv_info_init(void)
{
    Vec v;
    struct TextBox tb;
    char buf[0x40];

    if (g_some_other_flags & 0xA)
        return;
    g_init_some_globals_for_adv();
    modeCtrl.submodeTimer = 0x1428;
    modeCtrl.courseFlags &= ~0x2004;
    modeCtrl.playerCount = 1;
    active_monkey_id[0] = 0;
    g_set_up_debugtext_buffer();
    dest_all_events();
    load_stage(0x46);
    g_load_minigame_gma_tpl(0);
    event_init(0);
    event_init(1);
    event_init(2);
    event_init(3);
    event_init(4);
    event_init(9);
    event_init(5);
    event_init(7);
    event_init(0xF);
    event_init(0x10);
    event_init(0x11);
    event_init(0x13);
    event_init(0xE);
    event_init(0x14);
    light_init(current_stage_id);
    g_create_stage_player_wrapper(0, 0x10, 0, 0xFF, 0, NULL);
    balls[0].unk179 = 3;
    balls[0].bananas = 0;
    enable_cameras_for_active_players();
    camera_set_state(0x2B);
    v.x = 0.0f;
    v.y = 0.0f;
    v.z = 1.0f;
    g_smth_with_ape_rotation(balls[0].ape, &v);
    load_bmp_by_id(5);
    create_titlescreen_press_start_sprites(0);
    create_copyright_text_sprites(0);
    if (!(modeCtrl.courseFlags & 0x2000))
    {
        memset(&tb, 0, sizeof(tb));
        tb.x = 320;
        tb.y = 0xD2;
        tb.rows = 1;
        tb.columns = 1;
        tb.style = 0xB;
        tb.callback = NULL;
        g_display_playpoint_or_gift_message_child(1, 2, &tb);
        tb.flags = 4;
        tb.x = 320;
        tb.y = 60;
        tb.rows = 1;
        tb.columns = 0;
        tb.style = 0xD;
        tb.callback = NULL;
        tb.textColor = 0xFF5000;
        g_display_playpoint_or_gift_message_child(2, 1, &tb);
        sprintf(buf, "   %s   ", lbl_1_data_3204[locale]);
        g_smth_with_playpoint_or_gift_msg(2, buf);
    }
    set_minimap_mode(4);
    set_minimap_mode(3);
    fade_screen_to_color(0x100, 0, 30);
    lbl_1_bss_1568.unk0 = 0;
    lbl_1_bss_1568.unk4 = 0;
    lbl_1_bss_1568.unkC = 0.0f;
    g_fade_track_volume(0xFFF60007, 0);
    set_sub_mode_destination(fn_1_C394);
    sub_mode_request = 0xE;
}

static const struct AdvInfoScript lbl_1_rodata_20C[] =
{
    { 5160, -12, NULL, 0x0 },
    { 5160, -1, NULL, 0x2 },
    { 5100, 0, "\203X\201[\203p\201[\203\202\203\223\203L\201[\203{\201[\203\213\202\314\202\240\202\273\202\321\202\251\202\275", 0x0 },
    { 4920, -4, NULL, 0x4 },
    { 4920, -5, NULL, 0x1 },
    { 4920, -1, NULL, 0x2 },
    { 4920, 1, "\203R\203\223\203g\203\215\201[\203\213\203X\203e\203B\203b\203N\202\360\n\223\374\202\352\202\275\202\331\202\244\202\311 \203R\203\215\202\252\202\351\202\346", 0x0 },
    { 4620, 2, "\203{\201[\203\213\202\252\223\256\202\255\202\306 \203J\203\201\203\211\202\340\223\256\202\255\202\251\202\347", 0x0 },
    { 4380, 3, "\202\273\202\352\202\311\215\207\202\355\202\271\202\304\n\203R\203\223\203g\203\215\201[\203\213\203X\203e\203B\203b\203N\202\360\223\374\202\352\202\346\202\244", 0x0 },
    { 4200, -1, NULL, 0x2 },
    { 4080, -12, NULL, 0x1 },
    { 4020, 4, "\211E\211\272\202\311\202\240\202\351\202\314\202\315 \203}\203b\203v\202\276\202\346", 0x0 },
    { 3780, 5, "/bcffffff/\203\237/bc000000/\203{\203^\203\223\202\360\211\237\202\267\202\306", 0x0 },
    { 3750, -3, NULL, 0x0 },
    { 3750, -12, NULL, 0x2 },
    { 3735, -12, NULL, 0x1 },
    { 3540, 6, "\203}\203b\203v\202\314\221\345\202\253\202\263\202\252 \225\317\202\355\202\351\202\361\202\276", 0x0 },
    { 3510, -3, NULL, 0x0 },
    { 3510, -12, NULL, 0x2 },
    { 3495, -12, NULL, 0x1 },
    { 3300, 7, "\203S\201[\203\213\222n\223_\202\342 \222n\214`\202\252\202\346\202\255\202\355\202\251\202\351\202\346", 0x0 },
    { 3270, -3, NULL, 0x0 },
    { 3270, -12, NULL, 0x2 },
    { 3255, -12, NULL, 0x1 },
    { 3030, -12, NULL, 0x0 },
    { 3000, 8, "\203{\203N\202\315\203o\203i\203i\202\252\203_\201`\203C\203X\203L\201I", 0x0 },
    { 2760, 9, "\203o\203i\203i\202\360\202P\202O\202O\226{\220H\202\327\202\351\202\306", 0x0 },
    { 2520, 10, "1UP\202\267\202\351\202\361\202\276\202\346", 0x0 },
    { 2330, 11, "\222n\214`\202\311\215\207\202\355\202\271\202\304\n\203R\203\223\203g\203\215\201[\203\213\203X\203e\203B\203b\203N\202\360\223\374\202\352\202\346\202\244", 0x0 },
    { 2160, 12, "\215\342\223\271\202\360\202\314\202\332\202\351\216\236\202\315", 0x0 },
    { 2020, 13, "\203R\203\223\203g\203\215\201[\203\213\203X\203e\203B\203b\203N\202\360\n\221\345\202\253\202\255\202\251\202\275\202\336\202\257\202\304\202\313", 0x0 },
    { 1880, 14, "\203t\203\215\203A\202\251\202\347\227\216\202\277\202\351\202\306\202P\203~\203X\202\276\202\346", 0x0 },
    { 1740, 15, "\227\216\202\277\202\310\202\242\202\346\202\244\202\311 \213C\202\360\202\302\202\257\202\304\202\313\201`\201I", 0x1 },
    { 1380, -7, NULL, 0x10 },
    { 1365, -6, NULL, 0xf },
    { 1365, -4, NULL, 0x10 },
    { 1365, -8, NULL, 0x2b },
    { 1364, -4, NULL, 0x4 },
    { 1364, -1, NULL, 0x2 },
    { 1380, 16, "\203X\203C\203b\203`\202\360\202\335\202\302\202\257\202\275\202\347 \202\323\202\361\202\305\202\335\202\346\202\244", 0x0 },
    { 1170, 17, "\202\265\202\251\202\257\202\252\202\244\202\262\202\242\202\275\202\350 \202\306\202\334\202\301\202\275\202\350\202\267\202\351\202\346", 0x0 },
    { 870, 18, "\203V\201[\203\\\201[\202\311\202\310\202\301\202\304\202\242\202\351\202\344\202\251\202\360\202\355\202\275\202\351\202\306\202\253\202\315", 0x0 },
    { 690, 19, "\203o\203\211\203\223\203X\202\360\202\306\202\350\202\310\202\252\202\347\202\355\202\275\202\353\202\244\202\313", 0x0 },
    { 510, 20, "\203^\203C\203\200\202\252\202O\202\311\202\310\202\351\202\334\202\246\202\311", 0x0 },
    { 330, 21, "\203S\201[\203\213\202\305\202\253\202\352\202\316 \203X\203e\201[\203W\203N\203\212\203A\202\276\202\346", 0x0 },
    { 90, -10, NULL, 0x100 },
    { 90, -10, NULL, 0x400 },
    { 90, -9, NULL, 0x200 },
    { 90, -11, NULL, 0x0 },
    { 90, -13, NULL, 0x126 },
    { 0, 0, NULL, 0x0 },
};

char *lbl_1_data_36EC[] =
{
    "How to play MONKEY BALL",
    "Move towards direction\njoystick is pressed",
    "As ball moves, camera moves",
    "Control joystick according to camera",
    "Map on right below",
    "Press A button",
    "Size of map changes",
    "Check out goal and landscape",
    "I love bananas!",
    "As I get 100 bananas",
    "I get 1UP",
    "Control joystick\naccording to landscape",
    "When you go up slope",
    "Press joystick deeper",
    "1 miss for fall out",
    "Be careful not to fall out!",
    "Before time runs out",
    "Reach goal and go to next floor!",
};

char *lbl_1_data_46BC[][6] =
{
    {
        "How to play Super Monkey Ball 2!",
        "Spielregeln Super Monkey Ball 2!",
        "R\350gles de Super Monkey Ball 2 !",
        "\241C\363mo jugar Super Monkey Ball 2!",
        "Per giocare Super Monkey Ball 2!",
        "\203X\201[\203p\201[\203\202\203\223\203L\201[\203{\201[\203\213\202Q\202\314\202\240\202\273\202\321\202\251\202\275",
    },
    {
        "The further you push the Control Stick,\nthe faster you'll roll.",
        "Dr\374ckst du den Control Stick \nst\344rker, rollst du schneller.",
        "Plus tu pousses sur le Stick\ndirectionnel, plus \347a roule vite.",
        "Cuanto m\341s empujes el Stick\nde Control, m\341s r\341pido rodar\341s.",
        "Pi\371 terrai premuto il Control Stick,\npi\371 velocemente rotolerai.",
        "\203R\203\223\203g\203\215\201[\203\213\203X\203e\203B\203b\203N\202\360\n\202\242\202\352\202\275\202\331\202\244\202\311 \203R\203\215\202\252\202\351\202\346",
    },
    {
        "And if the ball starts moving,\nthe camera will follow.",
        "Und sobald die Kugel rollt,\nfolgt die Kamera.",
        "Et quand la boule commence\n\340 rouler, la cam\351ra la suit.",
        "Y si la bola empieza a moverse,\nla c\341mara la seguir\341.",
        "E se la palla si muover\340,\nla telecamera la seguir\340.",
        "\203{\201[\203\213\202\252\202\244\202\262\202\255\202\306 \203J\203\201\203\211\202\340\202\244\202\262\202\255\202\251\202\347",
    },
    {
        "So, you'll want to make sure\nto remember that when you move.",
        "Daran solltest du denken,\nwenn du dich bewegst.",
        "Alors n'oublie surtout pas\n\347a quand tu te d\351places.",
        "As\355 que, recuerda esto\ncuando te muevas.",
        "Quindi sar\340 meglio che te\nlo ricordi mentre ti sposti.",
        "\202\273\202\352\202\311\202\240\202\355\202\271\202\304\n\203R\203\223\203g\203\215\201[\203\213\203X\203e\203B\203b\203N\202\360\202\242\202\352\202\304\202\313",
    },
    {
        "There's a map of the stage\nin the bottom right corner.",
        "Es gibt eine Levelkarte\nunten in der Ecke.",
        "Il y a une carte du niveau\nen bas de l'\351cran.",
        "Hay un mapa de la fase en\nla esquina inferior derecha.",
        "C'\350 una mappa del livello\nin basso a destra.",
        "\202\335\202\254\202\265\202\275\202\311\202\240\202\351\202\314\202\315 \203}\203b\203v\202\276\202\346",
    },
    {
        "If you press /bcffffff/\203\237/bc000000/,",
        "Mit dem /bcffffff/\203\237/bc000000/-Knopf kannst du",
        "Appuie sur le Bouton /bcffffff/\203\237/bc000000/",
        "\241Si pulsas el Bot\363n /bcffffff/\203\237/bc000000/,",
        "Se premi il Pulsante /bcffffff/\203\237/bc000000/,",
        "/bcffffff/\203\237/bc000000/\203{\203^\203\223\202\360 \202\250\202\267\202\306",
    },
    {
        "you can change the size of the map!",
        "die Kartengr\366\337e ver\344ndern!",
        "pour changer la taille de la carte.",
        "puedes cambiar el tama\361o del mapa!",
        "puoi cambiarne le dimensioni!",
        "\203}\203b\203v\202\314\202\250\202\250\202\253\202\263\202\252 \202\251\202\355\202\351\202\361\202\276",
    },
    {
        "The goal and the level boundaries\nshould be easy to see.",
        "Ziel und Levelgrenzen sollten\nso leicht zu finden sein.",
        "L'arriv\351e et les limites\ndu niveau sont faciles \340 voir.",
        "La meta y los l\355mites del\nnivel son f\341ciles de ver.",
        "L'arrivo e i confini del\nlivello saranno ben visibili.",
        "\203S\201[\203\213\202\277\202\304\202\361\202\342 \202\277\202\257\202\242\202\252\202\346\202\255\202\355\202\251\202\351\202\346",
    },
    {
        "I really love bananas!",
        "Ich liebe Bananen!",
        "J'adore les bananes !",
        "\241Adoro las bananas!",
        "Adoro le banane!",
        "\203{\203N\202\315\203o\203i\203i\202\252\203_\201`\203C\203X\203L\202\310\202\361\202\276\201I",
    },
    {
        "Get a bunch of bananas",
        "****",
        "****",
        "****",
        "****",
        "\203o\203i\203i\202\360\202\275\202\255\202\263\202\361\202\306\202\301\202\304",
    },
    {
        "and make it to the goal!!",
        "****",
        "****",
        "****",
        "****",
        "\203S\201[\203\213\202\305\202\253\202\275\202\347 \202\244\202\352\202\265\202\242\202\310",
    },
    {
        "Make sure to use the Control Stick\nand stay on the ground!",
        "Benutze den Control Stick\nund bleib auf dem Boden!",
        "Utilise le stick directionnel,\net reste sur le sol !",
        "\241Utiliza el Stick de\nControl y rueda por el suelo!",
        "Ricorda di usare il Control Stick\ne resta sulla terra!",
        "\202\277\202\257\202\242\202\311\202\240\202\355\202\271\202\304\n\203R\203\223\203g\203\215\201[\203\213\203X\203e\203B\203b\203N\202\360\202\242\202\352\202\346\202\244",
    },
    {
        "When you want to climb up a hill,",
        "Wenn du einen H\374gel hochklettern",
        "Pour monter une pente,",
        "\241Cuando quieras subir una colina,",
        "Quando vuoi scalare una collina,",
        "\202\263\202\251\202\335\202\277\202\360\202\314\202\332\202\351\202\306\202\253\202\315",
    },
    {
        "make sure to press hard\non the Control Stick!",
        "willst, dr\374cke den\nControl Stick st\344rker!",
        "pousse le Stick directionnel, \340 fond !",
        "empuja fuerte el Stick de Control!",
        "premi a fondo il Control Stick!",
        "\203R\203\223\203g\203\215\201[\203\213\203X\203e\203B\203b\203N\202\360\n\202\250\202\250\202\253\202\255\202\251\202\275\202\336\202\257\202\304\202\313",
    },
    {
        "Be careful not to drop off the stage!",
        "****",
        "****",
        "****",
        "****",
        "\203X\203e\201[\203W\202\251\202\347\202\250\202\277\202\275\202\347 \203_\203\201\202\276\202\346\201I",
    },
    {
        "Make sure not to fall off!",
        "Du solltest nicht fallen!",
        "Ne tombe surtout pas !",
        "\241Intenta no caerte!",
        "Attento a non cadere!",
        "\202\250\202\277\202\310\202\242\202\346\202\244\202\311 \202\253\202\360\202\302\202\257\202\304\202\313\201`\201I",
    },
    {
        "Push if you find a switch.",
        "****",
        "****",
        "****",
        "****",
        "\203X\203C\203b\203`\202\360\202\335\202\302\202\257\202\275\202\347 \202\323\202\361\202\305\202\335\202\346\202\244",
    },
    {
        "You can start and stop the moving parts.",
        "****",
        "****",
        "****",
        "****",
        "\202\265\202\251\202\257\202\252\202\244\202\262\202\242\202\275\202\350 \202\306\202\334\202\301\202\275\202\350\202\267\202\351\202\346",
    },
    {
        "Try to keep your balance when",
        "****",
        "****",
        "****",
        "****",
        "\203V\201[\203\\\201[\202\311\202\310\202\301\202\304\202\242\202\351\202\344\202\251\202\360\202\355\202\275\202\351\202\306\202\253\202\315",
    },
    {
        "you're on a seesaw floor.",
        "****",
        "****",
        "****",
        "****",
        "\203o\203\211\203\223\203X\202\360\202\306\202\350\202\310\202\252\202\347 \202\355\202\275\202\353\202\244\202\313",
    },
    {
        "If you make it to the goal\nbefore time runs out,",
        "Wenn du im Ziel bist,\nbevor die Zeit abl\344uft,",
        "Si tu atteins l'arriv\351e\navant la fin du temps,",
        "\241Llega a la meta antes\nde que termine el tiempo",
        "Se arrivi al traguardo\nentro il tempo limite,",
        "\203^\203C\203\200\202\252\202O\202\311\202\310\202\351\202\334\202\246\202\311",
    },
    {
        "You will complete the stage\nwhen you touch the goal!",
        "****",
        "****",
        "****",
        "****",
        "\203S\201[\203\213\202\305\202\253\202\352\202\316 \203X\203e\201[\203W\203N\203\212\203A\202\276\202\346",
    },
};

void smd_adv_info_tick(void)
{
    struct TextBox tb;
    const struct AdvInfoScript *p;

    if (g_some_other_flags & 0xA)
        return;
    if ((dip_switches & 1) && DEBUG_SKIP_PRESSED() && modeCtrl.submodeTimer > 30)
        modeCtrl.submodeTimer = 30;
    if (modeCtrl.submodeTimer == 0x640)
        modeCtrl.submodeTimer = 0x564;
    if (mode_info.flags & 4)
    {
        mode_info.flags &= ~4;
        camera_set_state(4);
        call_SoundReqID_arg_0(0x1D);
    }
    if (modeCtrl.submodeTimer == 0x553)
    {
        balls[0].flags &= ~0x800;
        mode_info.flags &= ~8;
    }
    if (mode_info.flags & 1)
    {
        mode_info.flags &= ~1;
        balls[0].state = 5;
        balls[0].flags |= 0x500;
        camera_set_state(0xE);
    }
    if (modeCtrl.submodeTimer < 0x102C && modeCtrl.submodeTimer > 0xDD4)
    {
        Vec v;

        v.x = 1.15f;
        v.y = 0.5f;
        v.z = 3.8f;
        g_something_with_translating_items(5, &v, 1.0f);
    }
    if (!(modeCtrl.courseFlags & 0x2000) && modeCtrl.submodeTimer == 0x13EC)
        g_create_some_controls_description_sprite();
    for (p = lbl_1_rodata_20C; p->time != 0; p++)
    {
        if (modeCtrl.submodeTimer <= p->time && modeCtrl.submodeTimer >= p->time)
        {
            if (p->id >= 0 && !(modeCtrl.courseFlags & 0x2000))
            {
                memset(&tb, 0, sizeof(tb));
                if (p->id == 0x10)
                    tb.y = 200;
                if (p->id == 0x11)
                    tb.y = 0xB4;
                tb.rows = 1;
                tb.style = 0xB;
                tb.flags = (p->param ? 8 : 0) | 4;
                g_display_playpoint_or_gift_message_child(1, 0x15, &tb);
                g_smth_with_playpoint_or_gift_msg(1, lbl_1_data_46BC[p->id][locale]);
            }
            switch (p->id)
            {
            case -1:
                camera_set_or_clear_flags(p->param, 1);
                break;
            case -2:
                camera_set_or_clear_flags(p->param, 0);
                break;
            case -3:
                toggle_minimap_zoom();
                break;
            case -4:
                balls[0].state = p->param;
                break;
            case -5:
                world_infos[0].state = p->param;
                break;
            case -6:
                fade_screen_to_color(0x100, 0, (s8)p->param);
                break;
            case -7:
                fade_screen_to_color(0x101, 0, (s8)p->param);
                break;
            case -8:
                camera_set_state((s8)p->param);
                break;
            case -9:
                balls[0].flags |= p->param;
                break;
            case -10:
                balls[0].flags &= ~p->param;
                break;
            case -11:
                g_display_playpoint_or_gift_message_child(1, 20, NULL);
                break;
            case -12:
                lbl_1_bss_1568.unk8 = p->param;
                break;
            case -13:
                call_SoundReqID_arg_0(p->param);
                break;
            }
        }
    }
    if (CAN_SKIP(60))
        g_smth_with_adv_sprites_and_stage_loading(30);
    if (modeCtrl.submodeTimer == 30)
    {
        struct Sprite *sprite;

        fade_screen_to_color(0x101, 0, 31);
        g_display_playpoint_or_gift_message_child(0, 20, NULL);
        g_display_playpoint_or_gift_message_child(1, 20, NULL);
        g_display_playpoint_or_gift_message_child(2, 20, NULL);
        g_fade_track_volume(modeCtrl.submodeTimer, 2);
        sprite = get_sprite_with_unique_id(0x11);
        if (sprite != NULL)
            sprite->userVar = 1;
    }
    if (--modeCtrl.submodeTimer <= 0)
        sub_mode_request = 0xF;
}

void fn_1_C394(void)
{
    switch (sub_mode)
    {
    case 13:
    case 14:
        set_sub_mode_destination(fn_1_C394);
        break;
    default:
        free_minigame_gma_tpl();
        break;
    }
}

void smd_adv_game_ready_init(void)
{
    struct ReplayHeader hdr;

    if (g_some_other_flags & 0xA)
        return;
    if (fn_1_5FC54() == 0)
    {
        sub_mode_request = 3;
        return;
    }
    fn_1_DA88();
    lbl_1_bss_71EC8.unk14 = 0;
    lbl_1_bss_71EC8.unk0[lbl_1_bss_71EC8.unk14] = fn_1_D77C();
    fn_1_63F54(lbl_1_bss_71EC8.unk0[lbl_1_bss_71EC8.unk14]);
    if (lbl_1_bss_71EC8.unk0[lbl_1_bss_71EC8.unk14] < 0)
    {
        lbl_1_bss_71EC8.unk0[lbl_1_bss_71EC8.unk14] = 0;
        sub_mode_request = 3;
        fade_screen_to_color(2, 0, 1);
        return;
    }
    lbl_1_bss_71EC8.unk10 = fn_1_61B18(lbl_1_bss_71EC8.unk0[lbl_1_bss_71EC8.unk14]);
    fn_1_62AA0(lbl_1_bss_71EC8.unk0[lbl_1_bss_71EC8.unk14], &hdr);
    current_stage_id = (s16)hdr.stageId;
    dest_all_events();
    load_bmp_by_id(3);
    modeCtrl.gameType = 0;
    modeCtrl.playerCount = 1;
    modeCtrl.unk30 = 1;
    camera_setup_splitscreen_viewports(modeCtrl.playerCount);
    fn_1_56C18();
    g_set_up_debugtext_buffer();
    event_init(9);
    fn_1_618F4(lbl_1_bss_71EC8.unk0[lbl_1_bss_71EC8.unk14]);
    load_stage(current_stage_id);
    g_load_minigame_gma_tpl(0);
    event_init(0);
    event_init(1);
    event_init(2);
    event_init(3);
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
    g_create_stage_player_wrapper(0, 2, active_monkey_id[0], 0xFF, 0, ape_default_anim);
    balls[0].unk179 = 3;
    balls[0].bananas = 0;
    g_init_rendefc_for_stage();
    event_freeze(1);
    light_init(current_stage_id);
    set_minimap_mode(4);
    load_bmp_by_id_child(5);
    mode_info.flags |= 0x108;
    modeCtrl.submodeTimer = 120;
    enable_cameras_for_active_players();
    camera_set_state(10);
    fn_1_D0EC4();
    fn_1_D1828();
    create_titlescreen_press_start_sprites(0);
    create_copyright_text_sprites(0);
    fn_1_D13D0();
    lbl_1_bss_1568.unk8 = 0;
    lbl_1_bss_1578 = 0;
    g_handle_world_bgm(0x5A);
    fade_screen_to_color(0x100, 0, 30);
    set_sub_mode_destination(fn_1_CC94);
    sub_mode_request = 0x10;
    modeCtrl.unk3C = modeCtrl.submodeTimer;
}

void smd_adv_game_ready_tick(void)
{
    struct ReplayHeader hdr;

    if (g_some_other_flags & 0xA)
        return;
    if (modeCtrl.submodeTimer == 120)
    {
        struct Sprite *sprite = get_sprite_with_unique_id(0xF);

        if (sprite != NULL)
            sprite->userVar = 0xF;
        create_player_num_and_ready_sprites(120);
        set_minimap_mode(3);
    }
    if (24.0 == modeCtrl.submodeTimer)
        balls[0].state = 3;
    if (CAN_SKIP(30))
        g_smth_with_adv_sprites_and_stage_loading(30);
    if (--modeCtrl.submodeTimer <= 0)
    {
        fn_1_62AA0(fn_1_D8A0(), &hdr);
        if (g_some_other_flags & 4)
            printf("/*-- pre_load_stage(%d), APE(%d) --*/\n", hdr.stageId, hdr.monkey);
        queue_stage_load(hdr.stageId);
        preload_ape(hdr.monkey, 0, 0, 1);
        sub_mode_request = 0x11;
    }
}

void smd_adv_game_play_init(void)
{
    if (g_some_other_flags & 0xA)
        return;
    modeCtrl.submodeTimer = 30.0 + fn_1_61B18(lbl_1_bss_71EC8.unk0[lbl_1_bss_71EC8.unk14]);
    event_restart(1);
    create_go_sprite(60);
    mode_info.flags |= 0x810;
    mode_info.flags &= ~0x108;
    balls[0].state = 9;
    world_infos[0].state = 9;
    camera_set_state(0x4B);
    mode_info.flags |= 0x810;
    lbl_1_bss_71EC8.unk10 = fn_1_61B18(lbl_1_bss_71EC8.unk0[lbl_1_bss_71EC8.unk14]);
    g_advance_stage_animation();
    sub_mode_request = 0x12;
}

void smd_adv_game_play_tick(void)
{
    if (g_some_other_flags & 0xA)
        return;
    if (mode_info.flags & 1)
    {
        mode_info.flags &= ~1;
        modeCtrl.unk18 = 0xB4;
        sub_mode_request = 0xB;
        world_infos[0].state = 6;
        camera_set_state(0xE);
        set_minimap_mode(1);
    }
    if (CAN_SKIP(30))
        g_smth_with_adv_sprites_and_stage_loading(30);
    modeCtrl.submodeTimer--;
    if (modeCtrl.submodeTimer < 0)
    {
        sub_mode_request = 0xB;
        set_minimap_mode(1);
    }
}

void fn_1_CC94(void)
{
    switch (sub_mode)
    {
    case 15:
    case 16:
    case 17:
    case 18:
        set_sub_mode_destination(fn_1_CC94);
        break;
    default:
        free_minigame_gma_tpl();
        break;
    }
}

void smd_adv_ranking_init(void)
{
    modeCtrl.submodeTimer = 0x9D8;
    destroy_sprite_with_unique_id(2);
    destroy_sprite_with_unique_id(0x12);
    sub_mode_request = 0xC;
}

void smd_adv_ranking_tick(void)
{
    struct ReplayHeader hdr50;
    struct ReplayHeader hdr38;
    struct RendEfcRequest req;
    struct ReplayHeader hdr8;
    struct Ball *ballp;

    if (g_some_other_flags & 0xA)
        return;
    ballp = balls;
    switch (modeCtrl.submodeTimer)
    {
    case 0x9D8:
        destroy_sprite_with_unique_id(0);
        destroy_sprite_with_unique_id(0x25);
        destroy_sprite_with_unique_id(0x27);
        create_copyright_text_sprites(0);
        fn_1_D0FA4(0);
        if (get_sprite_with_unique_id(0x11) != NULL)
            get_sprite_with_unique_id(0x11)->userVar = 1;
        lbl_1_bss_1568.unk8 = 1;
        break;
    case 0x99C:
        break;
    case 0x870:
        fn_1_D12CC();
        if (lbl_1_bss_1534 == 0)
            fn_1_D1900();
        break;
    case 0x834:
        fn_1_D1220(0);
        break;
    case 0x654:
        if (lbl_1_bss_1534 == 0)
        {
            fn_1_D0FA4(1);
            modeCtrl.unk18 = 0xB4;
        }
        break;
    case 0x4EC:
        fn_1_D12CC();
        break;
    case 0x4B0:
        fn_1_D1220(0);
        break;
    case 0x2D0:
        if (lbl_1_bss_1534 == 0)
        {
            fn_1_D0FA4(2);
            modeCtrl.unk18 = 0xB4;
        }
        break;
    case 0x168:
        fn_1_D12CC();
        break;
    case 0x12C:
        fn_1_D1220(0);
        break;
    case 0x5A:
        fn_1_D19FC(0);
        break;
    }
    if (ballp->state == 4)
    {
        fn_1_62AA0(lbl_1_bss_71EC8.unk0[lbl_1_bss_71EC8.unk14], &hdr50);
        if (hdr50.flags & 0x80)
        {
            ballp->state = 5;
            ballp->flags |= 0x3500;
        }
    }
    if (ballp->state == 4 || ballp->state == 6)
        mode_info.flags &= ~0x810;
    if (ballp->state != 10)
        modeCtrl.unk18--;
    if (modeCtrl.unk18 < 0
     && (ballp->state == 6 || ballp->state == 4)
     && ((modeCtrl.submodeTimer & 0x1F) == 0 || !(mode_info.flags & 0x20))
     && modeCtrl.submodeTimer > 180.0)
    {
        float t;

        modeCtrl.unk18 = 0x96;
        event_dest(0);
        event_dest(4);
        event_dest(5);
        event_dest(0xE);
        event_dest(0x15);
        event_dest(0x14);
        event_dest(2);
        event_dest(3);
        event_dest(0x13);
        lbl_1_bss_71EC8.unk0[lbl_1_bss_71EC8.unk14] = fn_1_DA34();
        fn_1_62AA0(lbl_1_bss_71EC8.unk0[lbl_1_bss_71EC8.unk14], &hdr38);
        current_stage_id = (s16)hdr38.stageId;
        fn_1_63F54(lbl_1_bss_71EC8.unk0[lbl_1_bss_71EC8.unk14]);
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
        g_init_rendefc_for_stage();
        memset(&req, 0, sizeof(req));
        req.unk6 = 0xFFFF;
        fn_1_D8130(2, 3, &req);
        g_set_smth_with_sound(1);
        light_init(current_stage_id);
        balls[0].state = 9;
        balls[0].bananas = 0;
        if (modeCtrl.submodeTimer > 0x9D8)
        {
            world_infos[0].state = 9;
            camera_set_state(0x4B);
        }
        else
        {
            world_infos[0].state = 6;
            camera_set_state(return_0x2c());
            mode_info.flags &= ~0x800;
        }
        t = fn_1_61B18(lbl_1_bss_71EC8.unk0[lbl_1_bss_71EC8.unk14]);
        if (hdr38.flags & 0x10)
            t -= 30.0f;
        if (t > (float)modeCtrl.submodeTimer - 120.0)
            t = (int)((float)modeCtrl.submodeTimer - 120.0);
        else if (t > 0.5 * (float)modeCtrl.submodeTimer)
            t = (int)(0.5 * (float)modeCtrl.submodeTimer);
        lbl_1_bss_71EC8.unk10 = t;
        g_advance_stage_animation();
        fn_1_62AA0(fn_1_D8A0(), &hdr8);
        if (g_some_other_flags & 4)
            printf("/*-- pre_load_stage(%d), APE(%d) --*/\n", hdr8.stageId, hdr8.monkey);
        queue_stage_load(hdr8.stageId);
        preload_ape(hdr8.monkey, 0, 0, 1);
    }
    if (CAN_SKIP(60))
    {
        g_smth_with_adv_sprites_and_stage_loading(30);
        fn_1_D1220(0);
        fn_1_D19FC(0);
    }
    if (modeCtrl.submodeTimer == 180)
    {
        g_something_with_init_or_maybe_leftover_from_smb1(2);
        queue_stage_load(1);
    }
    if (modeCtrl.submodeTimer == 30)
    {
        fade_screen_to_color(0x101, 0xFFFFFF, 30);
        g_display_playpoint_or_gift_message_child(0, 20, NULL);
        g_fade_track_volume(modeCtrl.submodeTimer, 2);
    }
    BALL_FOREACH(
        if (!(ball->flags & 0x200) && (ball->ape->flags & 0x4000))
        {
            ball->flags &= ~0x500;
            ball->flags |= 0x200;
            call_SoundReqID_arg_0(0x126);
        }
    )
    if (--modeCtrl.submodeTimer <= 0)
    {
        mode_info.flags &= ~0x800;
        fn_1_D1220(1);
        fn_1_D19FC(1);
        destroy_sprite_with_unique_id(3);
        call_free_bmp_by_id(3);
        camera_setup_singleplayer_viewport();
        sub_mode_request = 3;
    }
}

s32 lbl_1_data_4928 = 12;
s32 lbl_1_data_492C = 12;

int fn_1_D77C(void)
{
    struct ReplayHeader hdr;
    int i;

    for (i = lbl_1_data_4928 + 1; i < 13; i++)
    {
        fn_1_62AA0(i, &hdr);
        if (!(hdr.flags & 0x200) && (hdr.flags & 0x10) && (hdr.flags & 1))
            break;
    }
    if (i >= 13)
    {
        for (i = 0; i < lbl_1_data_4928; i++)
        {
            fn_1_62AA0(i, &hdr);
            if (!(hdr.flags & 0x200) && (hdr.flags & 0x10) && (hdr.flags & 1))
                break;
        }
    }
    lbl_1_data_4928 = i;
    fn_1_62AA0(lbl_1_data_4928, &hdr);
    if (hdr.stageId == 0)
        hdr.stageId = 1;
    return lbl_1_data_4928;
}

int fn_1_D8A0(void)
{
    struct ReplayHeader hdr;
    int i;

    for (i = lbl_1_data_492C + 1; i < 13; i++)
    {
        if (i == lbl_1_data_4928)
            continue;
        fn_1_62AA0(i, &hdr);
        if (hdr.flags & 0x200)
            continue;
        if (fn_1_61B18(i) < 300.0)
            continue;
        if (hdr.flags & 0x83)
            break;
    }
    if (i >= 13)
    {
        for (i = 0; i <= lbl_1_data_492C; i++)
        {
            if (i == lbl_1_data_4928)
                continue;
            fn_1_62AA0(i, &hdr);
            if (hdr.flags & 0x200)
                continue;
            if (fn_1_61B18(i) < 300.0)
                continue;
            if (hdr.flags & 0x83)
                break;
        }
        if (i > lbl_1_data_492C)
            i = lbl_1_data_4928;
    }
    lbl_1_data_492C = i;
    fn_1_62AA0(lbl_1_data_492C, &hdr);
    if (hdr.stageId == 0)
        hdr.stageId = 1;
    lbl_1_bss_157C = 1;
    return lbl_1_data_492C;
}

int fn_1_DA34(void)
{
    int r = lbl_1_bss_157C ? lbl_1_data_492C : fn_1_D8A0();

    lbl_1_bss_157C = 0;
    return r;
}

void fn_1_DA88(void)
{
    lbl_1_bss_157C = 0;
}

int return_0x2c(void)
{
    return 0x2C;
}

void smd_adv_start_init(void)
{
    if (g_some_other_flags & 0xA)
        return;
    lbl_1_bss_1534 = 0;
    call_SoundReqID_arg_1(2);
    g_display_playpoint_or_gift_message_child(0, 20, NULL);
    g_display_playpoint_or_gift_message_child(1, 20, NULL);
    g_display_playpoint_or_gift_message_child(2, 20, NULL);
    g_display_playpoint_or_gift_message_child(3, 20, NULL);
    if (get_sprite_with_unique_id(0x11) != NULL && get_sprite_with_unique_id(0x11)->userVar == 0)
        get_sprite_with_unique_id(0x11)->userVar = 1;
    destroy_sprite_with_unique_id(1);
    if (dip_switches & 1)
        update_av_logo();
    if (lbl_1_bss_CF3D4 == 0)
        modeCtrl.unk48 = 0;
    else
        modeCtrl.unk48 = 2;
    set_sub_mode_destination(fn_1_E75C);
    sub_mode_request = 0xA;
}

void smd_adv_start_tick(void)
{
    struct MemcardRequest req40;
    struct MemcardRequest req28;
    char buf[0x20];

    if (g_some_other_flags & 0xA)
        return;
    if (dip_switches & 1)
        update_av_logo();
    switch (modeCtrl.unk48)
    {
    case 0:
        if (g_check_some_memcard_field() == 0)
        {
            if (fn_1_F00BC(0) == 0 && fn_1_F00BC(1) == 0)
                modeCtrl.unk48 = 0xB;
            else
            {
                memset(&req40, 0, sizeof(req40));
                req40.unk0 = 0;
                req40.flags |= 0x20800000;
                req40.files = storymode_save_files;
                g_some_replay_func4(1, 4, &req40);
                modeCtrl.unk48 = 1;
            }
        }
        break;
    case 1:
        if (g_check_some_memcard_field() == 0)
        {
            if (g_get_some_memcard_var() != 0)
            {
                g_some_loading_function();
                smd_game_over_save_child();
                modeCtrl.unk48 = 2;
            }
            else
            {
                switch (g_get_result_code())
                {
                case -0x79:
                case -4:
                    modeCtrl.unk48 = 4;
                    break;
                case -9:
                case -8:
                    modeCtrl.unk48 = 9;
                    break;
                case -3:
                    modeCtrl.unk48 = 0xB;
                    break;
                case -2:
                    modeCtrl.unk48 = 0xD;
                    break;
                case -0x78:
                    g_smth_with_adv_sprites_and_stage_loading(1);
                    break;
                case -0x77:
                    modeCtrl.unk48 = 2;
                    break;
                default:
                    modeCtrl.unk48 = 8;
                    break;
                }
            }
        }
        break;
    case 4:
#ifndef __MWERKS__
        /* pc_port: the card has no game file yet (first start).  Instead of
         * asking "Do you want to save Game Data in Slot A?", take the YES
         * branch: the game's own save code creates the file (case 6). */
        modeCtrl.unk48 = 6;
        break;
#endif
        fn_1_EFF6C(g_get_last_used_memcard_slot(), 0x3E, 0x19, 0x43);
        modeCtrl.unk10 = 0;
        modeCtrl.courseFlags &= ~4;
        modeCtrl.submodeTimer = 12;
        modeCtrl.unk48 = 5;
        break;
    case 5:
        if (!(modeCtrl.courseFlags & 4))
        {
            if ((merged_digital_inputs[2] & 1) || (merged_analog_inputs[2] & 1))
                modeCtrl.unk10 = 0;
            else if ((merged_digital_inputs[2] & 2) || (merged_analog_inputs[2] & 2))
                modeCtrl.unk10 = 1;
            if (merged_digital_inputs[2] & 0x100)
                modeCtrl.courseFlags |= 4;
            else if (merged_digital_inputs[2] & 0x200)
                g_smth_with_adv_sprites_and_stage_loading(1);
        }
        else if (--modeCtrl.submodeTimer <= 0)
        {
            fn_1_EFFB0();
            if (modeCtrl.unk10 == 0)
                modeCtrl.unk48 = 6;
            else
                modeCtrl.unk48 = 2;
        }
        break;
    case 6:
        g_save_game_data();
        memset(&req28, 0, sizeof(req28));
        req28.unk0 = 0;
        g_sprintf_memcard_error(0, buf);
        req28.fileName = buf;
        req28.files = storymode_save_files;
#ifndef __MWERKS__
        /* pc_port: no "Finished saving ... press A" stop; same flag the game's
         * auto-saves use (data written is unchanged). */
        req28.flags |= 0x20000000;
#endif
        g_some_replay_func4(0, 3, &req28);
        modeCtrl.unk48 = 7;
        break;
    case 7:
        if (g_check_some_memcard_field() == 0)
        {
            if (g_get_some_memcard_var() != 0)
            {
                smd_game_over_save_child();
                modeCtrl.unk48 = 2;
            }
            else
                modeCtrl.unk48 = 8;
        }
        break;
    case 8:
        fn_1_EFF6C(g_get_last_used_memcard_slot(), 0x3F, 0x16, 0x43);
        modeCtrl.unk48 = 0xA;
        break;
    case 9:
        switch (g_get_result_code())
        {
        case -8:
            fn_1_EFF6C(g_get_last_used_memcard_slot(), 0x25, 0x16, 0x43);
            break;
        case -9:
            fn_1_EFF6C(g_get_last_used_memcard_slot(), 0x27, 0x16, 0x43);
            break;
        }
        modeCtrl.unk48 = 0xA;
        break;
    case 0xA:
        if (merged_digital_inputs[2] & 0x100)
        {
            fn_1_EFFB0();
            modeCtrl.unk48 = 2;
        }
        else if (merged_digital_inputs[2] & 0x200)
            g_smth_with_adv_sprites_and_stage_loading(1);
        else if (merged_digital_inputs[2] & 0x800)
        {
            fn_1_EFFB0();
            modeCtrl.unk48 = 0;
        }
        else if (merged_digital_inputs[2] & 0x400)
        {
            fn_8000A71C();
            modeCtrl.unk48 = 0xF;
        }
        break;
    case 0xB:
        fn_1_EFF6C(g_get_last_used_memcard_slot(), 0x40, 0x17, 0x43);
        modeCtrl.unk48 = 0xC;
        break;
    case 0xC:
        if (merged_digital_inputs[2] & 0x100)
        {
            fn_1_EFFB0();
            modeCtrl.unk48 = 2;
        }
        else if (merged_digital_inputs[2] & 0x200)
            g_smth_with_adv_sprites_and_stage_loading(1);
        else if (merged_digital_inputs[2] & 0x800)
        {
            fn_1_EFFB0();
            modeCtrl.unk48 = 0;
        }
        break;
    case 0xD:
        fn_1_EFF6C(g_get_last_used_memcard_slot(), 0x41, 0x17, 0x43);
        modeCtrl.unk48 = 0xE;
        break;
    case 0xE:
        if (merged_digital_inputs[2] & 0x100)
        {
            fn_1_EFFB0();
            modeCtrl.unk48 = 2;
        }
        else if (merged_digital_inputs[2] & 0x200)
            g_smth_with_adv_sprites_and_stage_loading(1);
        else if (merged_digital_inputs[2] & 0x800)
        {
            fn_1_EFFB0();
            modeCtrl.unk48 = 0;
        }
        break;
    case 2:
        modeCtrl.submodeTimer = 30;
        fade_screen_to_color(0x101, 0, 32);
        g_fade_track_volume(modeCtrl.submodeTimer, 2);
        modeCtrl.unk48 = 3;
        break;
    case 3:
        if (--modeCtrl.submodeTimer <= 0)
        {
            g_init_some_globals_for_adv();
            lbl_1_bss_1540.timer = 0;
            camera_setup_singleplayer_viewport();
            if (!(dip_switches & 1))
            {
                main_mode_request = 1;
                sub_mode_request = 0x1F;
                if (modeCtrl.courseFlags & 0x40000)
                    OSReport("### ERROR ### mode.flag & M_MODE_OPTION_MODE\n");
            }
            else if (modeCtrl.courseFlags & 1)
            {
                main_mode_request = 1;
                sub_mode_request = 0x1F;
            }
            else if (modeCtrl.courseFlags & 0x40000)
            {
                main_mode_request = 5;
                sub_mode_request = 0xD3;
            }
            else
            {
                main_mode_request = 1;
                sub_mode_request = 0x1D;
            }
        }
        break;
    case 0xF:
        break;
    }
}

void fn_1_E75C(void)
{
    switch (sub_mode)
    {
    case 9:
    case 10:
        fn_1_F660();
        set_sub_mode_destination(fn_1_E75C);
        break;
    default:
        fn_1_EFFB0();
        break;
    }
}

void g_init_some_globals_for_adv(void)
{
    g_poolInfo.playerPool.statusList[0] = 2;
    g_poolInfo.playerPool.statusList[1] = 0;
    g_poolInfo.playerPool.statusList[2] = 0;
    g_poolInfo.playerPool.statusList[3] = 0;
    modeCtrl.playerCount = 1;
    modeCtrl.unk30 = 1;
    modeCtrl.gameType = 0;
    modeCtrl.unk40 = 0;
    modeCtrl.currPlayer = 0;
    current_ball = &balls[modeCtrl.currPlayer];
    camera_setup_singleplayer_viewport();
}

void smd_adv_prog_init(void) {}

void smd_adv_prog_tick(void) {}

void smd_adv_first_logo_init(void)
{
    if (g_some_other_flags & 0xA)
        return;
    lbl_1_bss_1538 = modeCtrl.wideMode != 0;
    fn_1_34828();
    fn_1_36140();
    g_init_some_globals_for_adv();
    lbl_1_bss_153C = 0xFFC000;
    modeCtrl.submodeTimer = 0x348;
    modeCtrl.courseFlags = 0;
    g_set_up_debugtext_buffer();
    light_init(0);
    dest_all_events();
    g_something_with_iteratively_freeing_memory();
    event_init(0x11);
    event_init(0x13);
    load_bmp_by_id(1);
    g_something_with_loading_stages_on_gamestart();
    call_SoundReqID_arg_0(0x21);
    fade_screen_to_color(0x101, 0, 60);
    modeCtrl.courseFlags &= ~4;
    modeCtrl.submodeTimer = 0x12C;
    sub_mode_request = 0x19;
}

void smd_adv_first_logo_tick(void)
{
    struct Sprite *sprite;

    if (g_some_other_flags & 0xA)
        return;
    if ((dip_switches & 1) && modeCtrl.submodeTimer > 30 && DEBUG_SKIP_PRESSED())
        modeCtrl.submodeTimer = 30;
    if (modeCtrl.submodeTimer == 0xF0)
    {
        sprite = create_sprite();
        if (sprite != NULL)
        {
            sprite->tag = 3;
            sprite->type = SPRITE_TYPE_BITMAP;
            sprite->x = 320.0f;
            sprite->y = 240.0f;
            sprite->textAlign = 4;
            sprite->bmpId = 0x100;
            strcpy(sprite->text, "AV LOGO");
            if (lbl_1_bss_1538 != 0)
            {
                sprite->flags |= 0x1000000;
                sprite->widescreenArg = 0x140;
            }
        }
        sprite = create_sprite();
        if (sprite != NULL)
        {
            sprite->tag = 3;
            sprite->type = SPRITE_TYPE_BITMAP;
            sprite->bmpId = 0x107;
            sprite->x = 320.0f;
            sprite->y = 74.0f;
            sprite->scaleY = 0.15625f;
            sprite->textAlign = 4;
            sprite->unk90 = 0.0f;
            sprite->unk94 = 0.78125f;
            sprite->unk98 = 1.0f;
            sprite->unk9C = 0.9375f;
            strcpy(sprite->text, "Created by");
        }
        fade_screen_to_color(0x100, 0, 60);
        g_SoftStreamStart_with_some_defaults_2(0x28);
    }
    if (CAN_SKIP(60))
    {
        g_smth_with_adv_sprites_and_stage_loading(30);
        g_fade_track_volume(0x36, 0);
    }
    if (!(modeCtrl.courseFlags & 0x2000))
    {
        if (modeCtrl.submodeTimer == 30)
        {
            fade_screen_to_color(0x101, 0, 30);
            g_display_playpoint_or_gift_message_child(0, 20, NULL);
        }
        if (--modeCtrl.submodeTimer <= 0)
        {
            destroy_sprite_with_unique_id(3);
            sub_mode_request = 7;
        }
    }
}

void g_maybe_draw_sega_splash(void)
{
    if (modeCtrl.submodeTimer > 0xF0)
        draw_sega_splash_wrapper();
}
