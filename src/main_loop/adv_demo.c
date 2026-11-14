/* Attract mode intro cutscene ("I really love bananas!"): a scripted scene of the
 * monkeys, driven by the song's beat counter. */
#include "types.h"
#include "ape_internal.h"
#include "avdisp.h"
#include "background.h"
#include "ball.h"
#include "camera.h"
#include "dip.h"
#include "effect.h"
#include "event.h"
#include "frustum.h"
#include "input.h"
#include "libc.h"
#include "light.h"
#include "macros.h"
#include "mathutil.h"
#include "mode.h"
#include "pool.h"
#include "shadow.h"
#include "sprite.h"
#include "stage.h"
#include "stcoli.h"
#include "textdraw.h"
#include "world.h"

#ifndef __MWERKS__
int pc_text_is_lead(unsigned char c); /* PC: language-aware SJIS lead test (textdraw.c) */
#endif

struct Keyframe
{
    s32 frame;
    float value;
    float tangentIn;
    float tangentOut;
};

struct ScreenFadeInfo
{
    /*0x00*/ s32 type;
    /*0x04*/ u32 color;
    /*0x08*/ s32 timer;
    /*0x0C*/ s32 timerMax;
};

struct DemoApe
{
    /*0x00*/ struct Ape *ape;
    /*0x04*/ s8 unk4;
    /*0x05*/ s8 unk5;
    /*0x06*/ s16 rotY;
    /*0x08*/ float animSpeed;
    /*0x0C*/ u8 flags;
}; /* size = 0x10 */

struct DemoCam
{
    /*0x00*/ Vec unk0;
    /*0x0C*/ Vec unkC;
    /*0x18*/ float unk18;
    /*0x1C*/ float unk1C;
    /*0x20*/ float unk20;
    /*0x24*/ s16 unk24;
    /*0x26*/ s16 unk26;
}; /* size = 0x28 */

struct DemoWork
{
    /*0x00*/ s32 active;
    /*0x04*/ s32 stageQueued;
    /*0x08*/ s32 finished;
    /*0x0C*/ u8 beat;
    /*0x0D*/ u8 prevBeat;
    /*0x0E*/ u8 subBeat;
    /*0x10*/ s16 stageId;
    /*0x14*/ s32 discDistance;
    /*0x18*/ u8 unk18;
    /*0x19*/ u8 unk19;
    /*0x1A*/ u8 unk1A;
    /*0x1C*/ struct DemoApe apes[5];
    /*0x6C*/ s8 bananaState;
    /*0x70*/ Vec bananaPos;
    /*0x7C*/ s16 bananaRotX;
    /*0x7E*/ s16 bananaRotY;
    /*0x80*/ struct DemoCam cam;
    /*0xA8*/ u8 camFlags;
    /*0xAC*/ float eyeSpeed;
    /*0xB0*/ float lookAtSpeed;
    /*0xB4*/ s16 eyeRotX;
    /*0xB6*/ s16 eyeRotY;
    /*0xB8*/ s16 lookAtRotX;
    /*0xBA*/ s16 lookAtRotY;
    /*0xBC*/ u8 filler_BC[4];
}; /* size = 0xC0 */

struct DemoTimedCmd
{
    /*0x00*/ u32 time;
    /*0x04*/ s8 func;
    /*0x05*/ u8 arg0;
    /*0x08*/ int arg1;
}; /* size = 0xC */

struct DemoCmd
{
    /*0x00*/ u8 beat;
    /*0x01*/ u8 subBeat;
    /*0x02*/ s8 func;
    /*0x03*/ u8 arg0;
    /*0x04*/ int arg1;
}; /* size = 0x8 */

struct DemoApeCmd
{
    /*0x00*/ u8 beat;
    /*0x01*/ u8 subBeat;
    /*0x02*/ s8 cmd;
    /*0x04*/ float value;
}; /* size = 0x8 */

struct DemoShadowBone
{
    /*0x00*/ s32 bone;  /* negative ends the list */
    /*0x04*/ float scale;
    /*0x08*/ u8 color[4];
}; /* size = 0xC */

struct DemoShadowList
{
    /*0x00*/ s32 charaId;
    /*0x04*/ struct DemoShadowBone *bones;
}; /* size = 0x8 */

/* disc_load */
void disc_queue_status(int group);
int g_swapDiscQueueGroup(int group);
int disc_queue_distance(int group);
void fn_1_D391C(void);
void g_something_with_iteratively_freeing_memory(void);
void g_something_with_sound7_and_game_heaps(int group);
void g_load_minigame_gma_tpl(int idx);
void free_minigame_gma_tpl(void);

/* stage */
void queue_stage_load(int stageId);
void load_stage(int stage);
void fn_1_73900(u32 a);

/* hud */
void g_set_up_debugtext_buffer(void);
void create_titlescreen_press_start_sprites(int arg0);
void create_copyright_text_sprites(int arg0);
void fade_screen_to_color(s32 type, u32 color, int duration);
struct Sprite *get_sprite_with_unique_id(int tag);
extern struct ScreenFadeInfo g_screenfade_flags;

/* sound */
u32 fn_1_363C8(int track);
s16 g_maybe_related_to_music_crossfading(int track);
void g_change_music_volume(int track, u32 time, s8 volume);
void g_SoftStreamStart_with_some_defaults_2(int id);

/* draw */
void fn_1_D4870(void);
void g_maybe_some_bg_effect_handler(void);
void ord_tbl_draw_nodes(void);

/* ape, ball */
void g_smth_with_ape_rotation(struct Ape *ape, Vec *arg1);
void draw_ape(struct Ape *ape);
void fn_1_11355C(struct Ape *ape, float speed);
void fn_1_1132D0(struct Ape *ape, int b, int c, int d);
void preload_ape(int chara, int lod, int mesh, int scene);
struct Ape *load_ape(int chara, int lod, int scene, int mesh);
void set_ape_anim(struct Ape *ape, int anim);
void g_reset_ball(struct Ball *ball);
void g_create_stage_player_wrapper(u8 playerIdx, int mode, u8 apeChara, u8 colorId, u8 lod,
                                   void (*drawFunc)(struct Ape *));
void fn_1_47444(u8 playerIdx);
void mtxa_to_quat(Quat *q);

/* text */
int g_smth_with_fonts_chara_load_wrapper_discard_result(char *str);
void textdraw_reset(void);
void textdraw_set_font(int fontId);
void textdraw_set_alignment(s8 alignment);
void textdraw_set_depth(float depth);
void textdraw_set_scale(float x, float y);
void textdraw_set_flags(u32 flags);
void textdraw_set_mul_color(u32 color);
void textdraw_set_border(void);
void textdraw_set_font_style(int style);
void textdraw_set_rot_z(int rot);
void textdraw_set_spacing(float x, float y);
void textdraw_set_pos(float x, float y);
void textdraw_set_alpha(float alpha);
void textdraw_print(char *text);
float textdraw_get_pixel_width_of_string_as_f32(char *str);
void g_textdraw_set_unk6(float val);

/* misc */
void set_sub_mode_destination(void (*func)(void));
float fn_1_D2550(float frame, struct Keyframe *curve);
extern s32 active_monkey_id[];
extern u32 lbl_1_bss_6FB9C;
extern s32 lbl_1_bss_6FB6C;
extern float lbl_1_bss_6FB84;
extern float lbl_1_bss_6FB80;
extern float lbl_1_bss_6FB7C;
extern u16 lbl_1_bss_6FB8A;
extern s32 locale;

#ifndef __MWERKS__
#define __fmadds(a, b, c) ((a) * (b) + (c))
#endif

/* Unlike mathutil_sum_of_sq_2, leaves b in its own register */
static inline float demo_sum_of_sq_2(float a, float b)
{
    a = a * a;
    a = __fmadds(b, b, a);
    return a;
}

static inline void mathutil_mtxA_tf_point_z(register float z, register Vec *v)
{
#ifdef __MWERKS__
    register float *mtxA;
    register float x0, x1, y0, y1, z0, z1;

    asm
    {
        lis mtxA, LC_CACHE_BASE@ha
        lfs x0, 0x08(mtxA)
        lfs x1, 0x0C(mtxA)
        lfs y0, 0x18(mtxA)
        lfs y1, 0x1C(mtxA)
        lfs z0, 0x28(mtxA)
        lfs z1, 0x2C(mtxA)
        fmadds x0, x0, z, x1
        stfs x0, v->x
        fmadds y0, y0, z, y1
        stfs y0, v->y
        fmadds z0, z0, z, z1
        stfs z0, v->z
    }
#else
    v->x = mtxa->mtxA[0][2] * z + mtxa->mtxA[0][3];
    v->y = mtxa->mtxA[1][2] * z + mtxa->mtxA[1][3];
    v->z = mtxa->mtxA[2][2] * z + mtxa->mtxA[2][3];
#endif
}

void fn_1_F77C(s8 arg0, int arg1);
void fn_1_F7FC(s8 arg0, int arg1);
void fn_1_F820(s8 arg0, int arg1);
void fn_1_F978(void);
void fn_1_FD90(s8 arg0, int arg1);
void fn_1_FFC8(s8 arg0, int arg1);
void fn_1_1008C(s8 arg0, int arg1);
void fn_1_1120C(void);
void create_demo_mes_sprite(void);
void sprite_demo_mes_tick(s8 *status, struct Sprite *sprite);
void sprite_demo_mes_disp(struct Sprite *sprite);
void create_demo_mask_sprites(void);
void sprite_demo_mask_banana_tick(s8 *status, struct Sprite *sprite);
void fn_1_11D60(void);

u8 lbl_1_data_49B8[] =
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


struct DemoWork lbl_1_bss_1580;
struct DemoWork *lbl_1_data_4B28 = &lbl_1_bss_1580;

typedef void (*DemoFunc)(u8, int);

DemoFunc lbl_1_data_4B2C[] =
{
    (DemoFunc)fn_1_F77C,
    (DemoFunc)fn_1_F7FC,
    (DemoFunc)fn_1_F820,
    (DemoFunc)fn_1_FD90,
    (DemoFunc)fn_1_FFC8,
    (DemoFunc)fn_1_1008C,
};

struct DemoTimedCmd lbl_1_data_4B44[] =
{
    { 10, 2, 0, 397 },
    { 60, 4, 3, 0 },
    { 270, 1, 0, 95 },
    { 280, 4, 4, 0 },
};

struct DemoCmd lbl_1_data_4B74[] =
{
    { 0x00, 1, 4, 0, 0 },
    { 0x00, 1, 3, 0, 0 },
    { 0x02, 0, 5, 0, 0 },
    { 0x03, 18, 0, 16, 6 },
    { 0x04, 0, 5, 1, 0 },
    { 0x04, 0, 4, 2, 0 },
    { 0x04, 0, 2, 1, 397 },
    { 0x04, 0, 0, 1, 12 },
    { 0x04, 1, 2, 0, 204 },
    { 0x0C, 0, 3, 1, 0 },
    { 0x0C, 0, 3, 1, 1 },
    { 0x0C, 0, 3, 1, 2 },
    { 0x14, 0, 3, 1, 3 },
    { 0x14, 0, 4, 1, 0 },
    { 0x14, 1, 3, 0, 1 },
    { 0x20, 0, 4, 0, 0 },
    { 0x23, 0, 0, 16, 24 },
    { 0x24, 0, 3, 1, 4 },
    { 0x24, 0, 5, 2, 32 },
    { 0x24, 0, 4, 2, 0 },
    { 0x24, 0, 2, 1, 204 },
    { 0x24, 0, 0, 1, 24 },
    { 0x24, 1, 2, 0, 206 },
    { 0x34, 0, 5, 3, 32 },
    { 0x34, 0, 2, 1, 206 },
    { 0x34, 1, 2, 0, 13 },
    { 0x38, 0, 5, 2, 32 },
    { 0x3C, 0, 5, 3, 32 },
    { 0x40, 0, 5, 2, 32 },
    { 0x44, 0, 5, 3, 32 },
    { 0x44, 0, 2, 1, 13 },
    { 0x44, 1, 2, 0, 402 },
    { 0x4C, 0, 5, 2, 32 },
    { 0x54, 0, 5, 3, 32 },
    { 0x54, 0, 2, 1, 402 },
    { 0x54, 1, 2, 0, 397 },
    { 0x5C, 0, 2, 1, 397 },
    { 0x63, 0, 4, 5, 0 },
};


void g_something_with_loading_stages_on_gamestart(void)
{
    int group;

    lbl_1_data_4B28->unk18 = 0;
    lbl_1_data_4B28->stageId = 398;
    disc_queue_status(7);
    group = g_swapDiscQueueGroup(7);
    queue_stage_load(lbl_1_data_4B28->stageId);
    g_swapDiscQueueGroup(group);
    lbl_1_data_4B28->discDistance = disc_queue_distance(7);
    lbl_1_data_4B28->stageQueued = 1;
}

int fn_1_EE6C(void)
{
    int ready = 0;

    if (current_stage_id == lbl_1_data_4B28->stageId || disc_queue_distance(7) <= 0)
        ready = 1;
    return ready;
}

void g_something_w_adv_demo_init(void)
{
    struct World *world;
    s8 *status;
    int i;

    g_set_up_debugtext_buffer();
    dest_all_events();
    g_something_with_iteratively_freeing_memory();
    g_something_with_sound7_and_game_heaps(0x12);
    load_stage(lbl_1_data_4B28->stageId);
    fn_1_D391C();
    g_load_minigame_gma_tpl(0);
    modeCtrl.courseFlags &= ~4;
    modeCtrl.submodeTimer = 0;
    {
        int j;

        for (j = 0; j < 4; j++)
            g_poolInfo.playerPool.statusList[j] = 2;
    }
    active_monkey_id[0] = 0;
    active_monkey_id[1] = 1;
    active_monkey_id[2] = 2;
    active_monkey_id[3] = 3;
    event_init(0);
    event_init(1);
    event_init(2);
    event_init(3);
    event_init(4);
    event_init(7);
    event_init(0x10);
    event_init(0x11);
    event_init(0xE);
    event_init(0x14);
    world = world_infos;
    status = g_poolInfo.playerPool.statusList;
    for (i = 0; i < g_poolInfo.playerPool.count; i++, world++, status++)
    {
        if (*status == 2)
            world->state = 1;
    }
    set_camera_view_settings(0, 0.0f, 0.0f, 1.0f, 1.0f);
    enable_camera(0);
    camera_set_state_for_mask(1, 0x1D);
    create_titlescreen_press_start_sprites(0);
    create_copyright_text_sprites(0);
    lbl_1_data_4B28->finished = 0;
    lbl_1_data_4B28->beat = 0;
    lbl_1_data_4B28->prevBeat = 0;
    lbl_1_data_4B28->subBeat = 0;
    memset(&lbl_1_data_4B28->bananaState, 0, 0x14);
    memset(&lbl_1_data_4B28->cam, 0, 0x28);
    memset(&lbl_1_data_4B28->camFlags, 0, 0x14);
    fade_screen_to_color(0x100, 0, 60);
    lbl_1_data_4B28->active = 1;
}

int adv_demo_tick_function(void)
{
    struct DemoCmd *cmd;
    struct DemoTimedCmd *timed;
    DemoFunc func;
    u32 i;

    fn_1_363C8(95);
    if (!(g_some_other_flags & 0xA))
    {
        if (lbl_1_data_4B28->subBeat == 22)
        {
            lbl_1_data_4B28->prevBeat = lbl_1_data_4B28->beat;
            lbl_1_data_4B28->beat++;
            lbl_1_data_4B28->subBeat = 0;
        }
        else if ((s16)g_maybe_related_to_music_crossfading(95) != -1 || lbl_1_data_4B28->beat != 0)
        {
            lbl_1_data_4B28->subBeat++;
        }
    }
    if (!(g_some_other_flags & 0xA))
    {
        timed = lbl_1_data_4B44;
        for (i = 0; i < 4; i++, timed++)
        {
            if (timed->time == modeCtrl.submodeTimer)
            {
                func = lbl_1_data_4B2C[timed->func];
                func(timed->arg0, timed->arg1);
            }
        }
        cmd = lbl_1_data_4B74;
        for (i = 0; i < 38; i++, cmd++)
        {
            if (cmd->beat == lbl_1_data_4B28->beat && cmd->subBeat == lbl_1_data_4B28->subBeat)
            {
                func = lbl_1_data_4B2C[cmd->func];
                func(cmd->arg0, cmd->arg1);
            }
        }
    }
    fn_1_F978();
    if (lbl_1_data_4B28->bananaState >= 1)
        fn_1_1120C();
    if (g_some_other_flags & 0xA)
        return 0;
    if (lbl_1_data_4B28->finished == 0)
    {
        if (!(modeCtrl.courseFlags & 0x2000)
         && lbl_1_data_4B28->beat < 96
         && !(analog_inputs[0][0] & 0x100)
         && !(analog_inputs[0][0] & 0x200)
         && lbl_1_bss_6FB9C == 0
         && (pad_status_groups[0].unk0[2].button & 0x1000
          || pad_status_groups[1].unk0[2].button & 0x1000
          || pad_status_groups[2].unk0[2].button & 0x1000
          || pad_status_groups[3].unk0[2].button & 0x1000))
            lbl_1_data_4B28->finished = 1;
        else if (lbl_1_data_4B28->beat == 102)
            lbl_1_data_4B28->finished = 1;
        if (lbl_1_data_4B28->finished != 0)
        {
            fade_screen_to_color(0x101, 0, 60);
            g_change_music_volume(-1, 60, 0);
            return 1;
        }
    }
    modeCtrl.submodeTimer++;
    return 0;
}

void fn_1_F4C8(void)
{
    int i;

    camera_apply_viewport_if_changed(0);
    fn_1_D4870();
    g_maybe_some_bg_effect_handler();
    u_reset_light_group_stack(0);
    if (lbl_1_data_4B28->bananaState == 2)
    {
        mtxa_from_mtxb();
        mtxa_translate(&lbl_1_data_4B28->bananaPos);
        mtxa_rotate_y(lbl_1_data_4B28->bananaRotY);
        mtxa_rotate_x(lbl_1_data_4B28->bananaRotX);
        mtxa_scale_s(10.0f);
        avdisp_set_scale_factor(10.0f);
        load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
        avdisp_draw_model_culled_sort_auto(init_common_gma->modelEntries[68].model);
    }
    fn_1_11D60();
    for (i = 0; i < 5; i++)
    {
        struct DemoApe *demoApe = &lbl_1_data_4B28->apes[i];
        struct Ape *ape = demoApe->ape;
        Vec dir = { 0.0f, 0.0f, -1.0f };

        if (ape != NULL && !(demoApe->flags & 4) && !(demoApe->flags & 1))
        {
            mtxa_from_rotate_y(demoApe->rotY);
            mtxa_tf_vec(&dir, &dir);
            g_smth_with_ape_rotation(ape, &dir);
            mtxa_from_mtxb();
            draw_ape(ape);
        }
    }
    ord_tbl_draw_nodes();
}

void fn_1_F660(void)
{
    int i;

    if (lbl_1_data_4B28->stageQueued == 0)
        return;
    switch (sub_mode)
    {
    case 7:
    case 8:
        set_sub_mode_destination(fn_1_F660);
        return;
    }
    if (lbl_1_data_4B28->active != 0)
    {
        dest_all_events();
        g_something_with_iteratively_freeing_memory();
    }
    for (i = 0; i < 5; i++)
    {
        g_something_with_freeing_chara_heap_ape(lbl_1_data_4B28->apes[i].ape);
        lbl_1_data_4B28->apes[i].ape = NULL;
    }
    free_minigame_gma_tpl();
    disc_queue_status(1);
    disc_queue_status(7);
    g_something_with_sound7_and_game_heaps(0);
    lbl_1_data_4B28->active = 0;
    lbl_1_data_4B28->stageQueued = 0;
}

void fn_1_F77C(s8 arg0, int arg1)
{
    s8 white = arg0 & 0x10;

    if (lbl_1_data_4B28->finished == 0)
    {
        s8 mode = arg0 & 3;

        fade_screen_to_color(mode == 0 ? 0x101 : 0x100, white != 0 ? 0xFFFFFF : 0, arg1);
    }
}

void fn_1_F7FC(s8 arg0, int arg1)
{
    g_SoftStreamStart_with_some_defaults_2(arg1);
}

void fn_1_F820(s8 arg0, int arg1)
{
    if (arg0 == 0)
    {
        int group;

        lbl_1_data_4B28->stageId = arg1;
        disc_queue_status(7);
        group = g_swapDiscQueueGroup(7);
        queue_stage_load(lbl_1_data_4B28->stageId);
        g_swapDiscQueueGroup(group);
    }
    else
    {
        if (current_stage_id == lbl_1_data_4B28->stageId)
            OSReport("DEMO DT_STAGE_PRELOAD same stage %d\n", arg1);
        else if (disc_queue_distance(7) > 0)
            OSReport("DEMO DT_STAGE_PRELOAD load wait\n");
        if (arg1 != lbl_1_data_4B28->stageId)
        {
            OSReport("### DEMO WARNING ### DT_STAGE %d %d\n", lbl_1_data_4B28->stageId, arg1);
            lbl_1_data_4B28->stageId = arg1;
        }
        event_dest(4);
        event_dest(5);
        event_dest(0xE);
        event_dest(0x14);
        fn_1_73900(lbl_1_data_4B28->stageId);
        load_stage(lbl_1_data_4B28->stageId);
        event_init(4);
        event_init(5);
        event_init(0xE);
        event_init(0x14);
    }
}

struct Keyframe lbl_1_data_4D18[] =
{
    { 0, 0.0f, 0.0f, 0.0f },
    { 92, 11.7f, 0.0f, 0.0f },
    { 643, 9.6f, 0.0f, 0.0f },
    { 827, 9.6f, 0.0f, 0.0f },
    { 2116, -1.6f, 0.0f, 0.0f },
    { 2392, -1.6f, 0.0f, 0.0f },
};

struct Keyframe lbl_1_data_4D78[] =
{
    { 0, 0.0f, 0.0f, 0.0f },
    { 92, 1.25f, 0.0f, 0.0f },
    { 643, 1.45f, 0.0f, 0.0f },
    { 827, 1.45f, 0.0f, 0.0f },
    { 2116, 1.1f, 0.0f, 0.0f },
    { 2392, 1.1f, 0.0f, 0.0f },
};

struct Keyframe lbl_1_data_4DD8[] =
{
    { 0, 0.0f, 0.0f, 0.0f },
    { 92, 6.82f, 0.0f, 0.0f },
    { 643, 4.22f, 0.0f, 0.0f },
    { 827, 4.22f, 0.0f, 0.0f },
    { 2116, 0.0f, 0.0f, 0.0f },
    { 2392, 0.0f, 0.0f, 0.0f },
};

struct Keyframe lbl_1_data_4E38[] =
{
    { 0, 0.0f, 0.0f, 0.0f },
    { 92, 6144.0f, 0.0f, 0.0f },
    { 437, 6144.0f, 0.0f, 0.0f },
    { 643, 6144.0f, 0.0f, 0.0f },
    { 827, 6144.0f, 0.0f, 0.0f },
    { 2116, 33792.0f, 0.0f, 0.0f },
    { 2392, 33792.0f, 0.0f, 0.0f },
};

struct DemoApeCmd lbl_1_data_4EA8[] =
{
    { 0x0C, 0, 2, 2.0f },
    { 0x14, 0, -2, 2.0f },
    { 0x1A, 0, 2, 1.0f },
    { 0x1B, 0, 2, 0.0f },
    { 0x1C, 0, 1, 0.0f },
    { 0x24, 0, -3, 2.0f },
    { 0x24, 0, -2, 4.0f },
    { 0x24, 0, -4, 0.0f },
    { 0x34, 0, -5, 0.0f },
    { 0x54, 0, -2, 4.0f },
    { 0x5B, 0, -5, 0.0f },
    { 0x5C, 0, -3, 4.0f },
    { 0x5C, 0, 7, 0.0f },
    { 0x60, 0, 7, 8.0f },
    { 0x63, 0, -1, 0.0f },
    { 0xFF, 0, 0, 0.0f },
};

struct Keyframe lbl_1_data_4F28[] =
{
    { 0, 0.0f, 0.0f, 0.0f },
    { 92, 7.6f, 0.0f, 0.0f },
    { 437, 7.6f, 0.0f, 0.0f },
    { 644, 7.6f, 0.0f, 0.0f },
    { 828, 7.6f, 0.0f, 0.0f },
    { 2116, -2.2f, 0.0f, 0.0f },
    { 2392, -2.2f, 0.0f, 0.0f },
};

struct Keyframe lbl_1_data_4F98[] =
{
    { 0, 0.0f, 0.0f, 0.0f },
    { 92, 1.72f, 0.0f, 0.0f },
    { 437, 1.72f, 0.0f, 0.0f },
    { 644, 1.72f, 0.0f, 0.0f },
    { 828, 1.72f, 0.0f, 0.0f },
    { 2116, 1.1f, 0.0f, 0.0f },
    { 2392, 1.1f, 0.0f, 0.0f },
};

struct Keyframe lbl_1_data_5008[] =
{
    { 0, 0.0f, 0.0f, 0.0f },
    { 92, 1.52f, 0.0f, 0.0f },
    { 437, 1.52f, 0.0f, 0.0f },
    { 644, 1.52f, 0.0f, 0.0f },
    { 828, 1.52f, 0.0f, 0.0f },
    { 2116, 0.0f, 0.0f, 0.0f },
    { 2392, 0.0f, 0.0f, 0.0f },
};

struct Keyframe lbl_1_data_5078[] =
{
    { 0, 0.0f, 0.0f, 0.0f },
    { 92, 40960.0f, 0.0f, 0.0f },
    { 437, 40960.0f, 0.0f, 0.0f },
    { 644, 40960.0f, 0.0f, 0.0f },
    { 828, 40960.0f, 0.0f, 0.0f },
    { 2116, 35840.0f, 0.0f, 0.0f },
    { 2392, 35840.0f, 0.0f, 0.0f },
};

struct DemoApeCmd lbl_1_data_50E8[] =
{
    { 0x0F, 0, 7, 0.0f },
    { 0x0F, 0, -1, 0.8f },
    { 0x24, 0, -2, 4.0f },
    { 0x34, 0, -4, 0.0f },
    { 0x3C, 0, -5, 0.0f },
    { 0x54, 0, -2, 4.0f },
    { 0x5C, 0, -3, 4.0f },
    { 0x5C, 0, -1, 0.9f },
    { 0x5C, 0, 7, 4.0f },
    { 0x60, 0, -1, 1.0f },
    { 0x60, 0, 7, 6.0f },
    { 0x63, 0, -1, 0.0f },
    { 0xFF, 0, 0, 0.0f },
};

struct Keyframe lbl_1_data_5150[] =
{
    { 0, 0.0f, 0.0f, 0.0f },
    { 92, 8.7f, 0.0f, 0.0f },
    { 437, 8.7f, 0.0f, 0.0f },
    { 644, 8.7f, 0.0f, 0.0f },
    { 828, 8.7f, 0.0f, 0.0f },
    { 2116, -1.1f, 0.0f, 0.0f },
    { 2392, -1.1f, 0.0f, 0.0f },
};

struct Keyframe lbl_1_data_51C0[] =
{
    { 0, 0.0f, 0.0f, 0.0f },
    { 92, 1.82f, 0.0f, 0.0f },
    { 437, 1.82f, 0.0f, 0.0f },
    { 644, 1.82f, 0.0f, 0.0f },
    { 828, 1.82f, 0.0f, 0.0f },
    { 2116, 1.1f, 0.0f, 0.0f },
    { 2392, 1.1f, 0.0f, 0.0f },
};

struct Keyframe lbl_1_data_5230[] =
{
    { 0, 0.0f, 0.0f, 0.0f },
    { 92, 1.32f, 0.0f, 0.0f },
    { 437, 1.32f, 0.0f, 0.0f },
    { 644, 1.32f, 0.0f, 0.0f },
    { 828, 1.32f, 0.0f, 0.0f },
    { 2116, 0.1f, 0.0f, 0.0f },
    { 2392, 0.1f, 0.0f, 0.0f },
};

struct Keyframe lbl_1_data_52A0[] =
{
    { 0, 0.0f, 0.0f, 0.0f },
    { 92, 17152.0f, 0.0f, 0.0f },
    { 437, 17152.0f, 0.0f, 0.0f },
    { 644, 17152.0f, 0.0f, 0.0f },
    { 828, 17152.0f, 0.0f, 0.0f },
    { 2116, 30720.0f, 0.0f, 0.0f },
    { 2392, 30720.0f, 0.0f, 0.0f },
};

struct DemoApeCmd lbl_1_data_5310[] =
{
    { 0x10, 0, 7, 2.0f },
    { 0x10, 0, -1, 0.8f },
    { 0x24, 0, -2, 4.0f },
    { 0x3C, 0, -4, 0.0f },
    { 0x44, 0, -5, 0.0f },
    { 0x54, 0, -2, 4.0f },
    { 0x5C, 0, -3, 4.0f },
    { 0x5C, 0, -1, 1.0f },
    { 0x5C, 0, 7, 0.0f },
    { 0x60, 0, 7, 8.0f },
    { 0x63, 0, -1, 0.0f },
    { 0xFF, 0, 0, 0.0f },
};

struct Keyframe lbl_1_data_5370[] =
{
    { 0, 0.0f, 0.0f, 0.0f },
    { 92, 10.6f, 0.0f, 0.0f },
    { 828, 10.6f, 0.0f, 0.0f },
    { 2116, -1.3f, 0.0f, 0.0f },
    { 2392, -1.3f, 0.0f, 0.0f },
};

struct Keyframe lbl_1_data_53C0[] =
{
    { 0, 0.0f, 0.0f, 0.0f },
    { 92, 1.82f, 0.0f, 0.0f },
    { 828, 1.82f, 0.0f, 0.0f },
    { 2116, 1.15f, 0.0f, 0.0f },
    { 2392, 1.15f, 0.0f, 0.0f },
};

struct Keyframe lbl_1_data_5410[] =
{
    { 0, 0.0f, 0.0f, 0.0f },
    { 92, 1.12f, 0.0f, 0.0f },
    { 828, 1.12f, 0.0f, 0.0f },
    { 2116, -0.4f, 0.0f, 0.0f },
    { 2392, -0.4f, 0.0f, 0.0f },
};

struct Keyframe lbl_1_data_5460[] =
{
    { 0, 0.0f, 0.0f, 0.0f },
    { 92, 28160.0f, 0.0f, 0.0f },
    { 437, 28160.0f, 0.0f, 0.0f },
    { 644, 28160.0f, 0.0f, 0.0f },
    { 828, 28160.0f, 0.0f, 0.0f },
    { 2116, 31744.0f, 0.0f, 0.0f },
    { 2392, 31744.0f, 0.0f, 0.0f },
};

struct DemoApeCmd lbl_1_data_54D0[] =
{
    { 0x14, 0, 7, 6.0f },
    { 0x14, 0, -1, 0.5f },
    { 0x24, 0, -2, 4.0f },
    { 0x44, 0, -4, 0.0f },
    { 0x54, 0, -2, 4.0f },
    { 0x54, 0, -5, 0.0f },
    { 0x5C, 0, -3, 4.0f },
    { 0x5C, 0, -1, 0.9f },
    { 0x5C, 0, 7, 0.0f },
    { 0x61, 0, -1, 1.0f },
    { 0x61, 0, 7, 8.0f },
    { 0x63, 0, -1, 0.0f },
    { 0xFF, 0, 0, 0.0f },
};

struct Keyframe lbl_1_data_5538[] =
{
    { 0, 0.0f, 0.0f, 0.0f },
    { 2392, 0.0f, 0.0f, 0.0f },
};

struct Keyframe lbl_1_data_5558[] =
{
    { 0, -10.0f, 0.0f, 0.0f },
    { 1931, -10.0f, 0.0f, 0.0f },
    { 1932, 11.4f, 0.0f, 0.0f },
    { 2116, 11.4f, 0.0f, 0.0f },
    { 2392, 0.0f, 0.0f, 0.0f },
};

struct Keyframe lbl_1_data_55A8[] =
{
    { 0, 0.0f, 0.0f, 0.0f },
    { 1931, 0.0f, 0.0f, 0.0f },
    { 1932, 19.0f, 0.0f, 0.0f },
    { 2116, 19.0f, 0.0f, 0.0f },
    { 2392, 0.0f, 0.0f, 0.0f },
};

struct Keyframe lbl_1_data_55F8[] =
{
    { 0, 0.0f, 0.0f, 0.0f },
    { 1932, 32768.0f, 0.0f, 0.0f },
    { 2116, 32768.0f, 0.0f, 0.0f },
    { 2392, 0.0f, 0.0f, 0.0f },
};

struct DemoApeCmd lbl_1_data_5638[] =
{
    { 0x0C, 1, -2, 4.0f },
    { 0x54, 0, -6, 10.0f },
    { 0x54, 0, -3, 4.0f },
    { 0x5C, 0, -2, 4.0f },
    { 0xFF, 0, 0, 0.0f },
};

struct Keyframe *lbl_1_data_5660[] =
{
    lbl_1_data_4D18,
    lbl_1_data_4F28,
    lbl_1_data_5150,
    lbl_1_data_5370,
    lbl_1_data_5538,
};

struct Keyframe *lbl_1_data_5674[] =
{
    lbl_1_data_4D78,
    lbl_1_data_4F98,
    lbl_1_data_51C0,
    lbl_1_data_53C0,
    lbl_1_data_5558,
};

struct Keyframe *lbl_1_data_5688[] =
{
    lbl_1_data_4DD8,
    lbl_1_data_5008,
    lbl_1_data_5230,
    lbl_1_data_5410,
    lbl_1_data_55A8,
};

struct Keyframe *lbl_1_data_569C[] =
{
    lbl_1_data_4E38,
    lbl_1_data_5078,
    lbl_1_data_52A0,
    lbl_1_data_5460,
    lbl_1_data_55F8,
};

struct DemoApeCmd *lbl_1_data_56B0[] =
{
    lbl_1_data_4EA8,
    lbl_1_data_50E8,
    lbl_1_data_5310,
    lbl_1_data_54D0,
    lbl_1_data_5638,
};

struct DemoCam lbl_1_data_56C4[] =
{
    {{ 53.1f, -125.3f, -402.34f }, { 65.6f, -138.2f, -428.4f }, 0.0f, -1.0f, 1.08f, 32767, 32767 },
    {{ 10.276f, 1.66f, 7.616f }, { 10.572f, 1.722f, 2.585f }, -30.0f, 1.5f, 1.0f, 32767, 32767 },
};

struct DemoCam lbl_1_data_5714[] =
{
    {{ 28.6f, 299.1f, -111.3f }, { 23.8f, 5.1f, -67.7f }, 0.025f, 0.025f, 0.0f, 0, 0 },
    {{ 10.276f, 1.66f, 7.616f }, { 10.572f, 1.722f, 2.585f }, 0.4f, 0.4f, 0.0f, 0, 0 },
};

struct Keyframe lbl_1_data_5764[] =
{
    { 0, -0.011f, 0.0f, 0.0f },
    { 23, -0.011f, 0.0f, 0.0f },
    { 69, -45.2f, 0.0f, 0.0f },
    { 91, -45.2f, 0.0f, 0.0f },
    { 92, -1.937f, 0.0f, 0.0f },
    { 275, -2.261f, 0.0f, 0.0f },
    { 276, 11.58f, 0.0f, 0.0f },
    { 459, 11.85f, 0.0f, 0.0f },
    { 460, 8.086f, 0.0f, 0.0f },
    { 643, 11.153f, 0.0f, 0.0f },
    { 644, 10.222f, 0.0f, 0.0f },
    { 827, 9.813f, 0.0f, 0.0f },
    { 828, 2.62f, 0.0f, 0.0f },
    { 1011, 1.73f, 0.0f, 0.0f },
    { 1012, -1.28f, 0.0f, 0.0f },
    { 1195, -0.63f, 0.0f, 0.0f },
    { 1196, 23.948f, 0.0f, 0.0f },
    { 1287, 24.528f, 0.0f, 0.0f },
    { 1288, 1.396f, 0.0f, 0.0f },
    { 1379, 0.729f, 0.0f, 0.0f },
    { 1380, -5.866f, 0.0f, 0.0f },
    { 1471, -5.332f, 0.0f, 0.0f },
    { 1472, 0.87f, 0.0f, 0.0f },
    { 1563, 0.7f, 0.0f, 0.0f },
    { 1564, 714.73f, 0.0f, 0.0f },
    { 1747, 715.0f, 0.0f, 0.0f },
    { 1748, -1.845f, 0.0f, 0.0f },
    { 1839, 1.4f, 0.0f, 0.0f },
    { 1840, 0.091f, 0.0f, 0.0f },
    { 1931, -0.26f, 0.0f, 0.0f },
    { 1932, -6.045f, 0.0f, 0.0f },
    { 2115, -3.728f, 0.0f, 0.0f },
    { 2116, -2.811f, 0.0f, 0.0f },
    { 2161, -2.811f, 0.0f, 0.0f },
    { 2162, -0.477f, 0.0f, 0.0f },
    { 2207, -0.477f, 0.0f, 0.0f },
    { 2208, -1.419f, 0.0f, 0.0f },
    { 2276, -1.413f, 0.0f, 0.0f },
    { 2392, -1.413f, 0.0f, 0.0f },
};

struct Keyframe lbl_1_data_59D4[] =
{
    { 0, 0.412f, 0.0f, 0.0f },
    { 23, 0.412f, 0.0f, 0.0f },
    { 69, 0.38f, 0.0f, 0.0f },
    { 91, 0.38f, 0.0f, 0.0f },
    { 92, 2.246f, 0.0f, 0.0f },
    { 275, 1.655f, 0.0f, 0.0f },
    { 276, 1.477f, 0.0f, 0.0f },
    { 459, 1.565f, 0.0f, 0.0f },
    { 460, 2.319f, 0.0f, 0.0f },
    { 643, 1.695f, 0.0f, 0.0f },
    { 644, 1.844f, 0.0f, 0.0f },
    { 827, 1.564f, 0.0f, 0.0f },
    { 828, 1.92f, 0.0f, -0.01f },
    { 1011, 0.14f, 0.0f, 0.0f },
    { 1012, 0.21f, 0.0f, 0.0f },
    { 1195, 0.0f, 0.0f, 0.0f },
    { 1196, 1.826f, 0.0f, 0.0f },
    { 1287, 2.1f, 0.0f, 0.0f },
    { 1288, 0.424f, 0.0f, 0.0f },
    { 1379, 0.157f, 0.0f, 0.0f },
    { 1380, 0.32f, 0.0f, 0.0f },
    { 1471, 0.845f, 0.0f, 0.0f },
    { 1472, 2.1f, 0.0f, -0.02f },
    { 1563, -0.234f, 0.0f, 0.0f },
    { 1564, 408.12f, 0.0f, 0.0f },
    { 1747, 406.21f, 0.0f, 0.0f },
    { 1748, -14.84f, 0.0f, 0.0f },
    { 1839, -11.24f, 0.0f, 0.0f },
    { 1840, 0.647f, 0.0f, 0.0f },
    { 1931, -1.245f, 0.0f, 0.0f },
    { 1932, 17.693f, 0.0f, 0.0f },
    { 2115, 17.393f, 0.0f, 0.0f },
    { 2116, 1.075f, 0.0f, 0.0f },
    { 2161, 1.075f, 0.0f, 0.0f },
    { 2162, 1.077f, 0.0f, 0.0f },
    { 2207, 1.077f, 0.0f, 0.0f },
    { 2208, 1.5f, 0.0f, 0.0f },
    { 2276, 1.249f, 0.0f, 0.0f },
    { 2392, 1.249f, 0.0f, 0.0f },
};

struct Keyframe lbl_1_data_5C44[] =
{
    { 0, -0.516f, 0.0f, 0.0f },
    { 23, -0.516f, 0.0f, 0.0f },
    { 69, -85.0f, 0.0f, 0.0f },
    { 91, -85.0f, 0.0f, 0.0f },
    { 92, 12.46f, 0.0f, 0.0f },
    { 275, 12.705f, 0.0f, 0.0f },
    { 276, 4.648f, 0.0f, 0.0f },
    { 459, 5.106f, 0.0f, 0.0f },
    { 460, 2.997f, 0.0f, 0.0f },
    { 643, 2.083f, 0.0f, 0.0f },
    { 644, -0.11f, 0.0f, 0.0f },
    { 827, 2.842f, 0.0f, 0.0f },
    { 828, -0.57f, 0.0f, 0.01f },
    { 1011, -0.09f, 0.0f, 0.0f },
    { 1012, -1.6f, 0.0f, 0.0f },
    { 1195, -0.84f, 0.0f, 0.0f },
    { 1196, -93.15f, 0.0f, 0.0f },
    { 1287, -89.67f, 0.0f, 0.0f },
    { 1288, -0.236f, 0.0f, 0.0f },
    { 1379, -0.79f, 0.0f, 0.0f },
    { 1380, -75.8f, 0.0f, 0.0f },
    { 1471, -75.94f, 0.0f, 0.0f },
    { 1472, -1.556f, 0.0f, 0.0f },
    { 1563, -0.692f, 0.0f, 0.0f },
    { 1564, 182.29f, 0.0f, 0.0f },
    { 1747, 182.28f, 0.0f, 0.0f },
    { 1748, -14.87f, 0.0f, 0.0f },
    { 1839, -11.18f, 0.0f, 0.0f },
    { 1840, -1.725f, 0.0f, 0.0f },
    { 1931, -1.022f, 0.0f, 0.0f },
    { 1932, 15.853f, 0.0f, 0.0f },
    { 2115, 6.474f, 0.0f, 0.0f },
    { 2116, 1.124f, 0.0f, 0.0f },
    { 2161, 1.124f, 0.0f, 0.0f },
    { 2162, 0.841f, 0.0f, 0.0f },
    { 2207, 0.841f, 0.0f, 0.0f },
    { 2208, 1.337f, 0.0f, 0.0f },
    { 2276, 1.309f, 0.0f, 0.0f },
    { 2392, 1.309f, 0.0f, 0.0f },
};

struct Keyframe lbl_1_data_5EB4[] =
{
    { 0, -0.722f, 0.0f, 0.0f },
    { 23, -0.722f, 0.0f, 0.0f },
    { 69, -42.9f, 0.0f, 0.0f },
    { 91, -42.9f, 0.0f, 0.0f },
    { 92, -3.82f, 0.0f, 0.0f },
    { 275, -3.998f, 0.0f, 0.0f },
    { 276, 6.544f, 0.0f, 0.0f },
    { 459, 3.961f, 0.0f, 0.0f },
    { 460, 8.086f, 0.0f, 0.0f },
    { 643, 10.895f, 0.0f, 0.0f },
    { 644, 8.952f, 0.0f, 0.0f },
    { 827, 5.88f, 0.0f, 0.0f },
    { 828, 0.67f, 0.0f, 0.0f },
    { 1011, -0.3f, 0.0f, 0.0f },
    { 1012, 0.32f, 0.0f, 0.0f },
    { 1195, 0.18f, 0.0f, 0.0f },
    { 1196, 28.66f, 0.0f, 0.0f },
    { 1287, 29.14f, 0.0f, 0.0f },
    { 1288, -0.511f, 0.0f, 0.0f },
    { 1379, -0.774f, 0.0f, 0.0f },
    { 1380, -1.369f, 0.0f, 0.0f },
    { 1471, -4.018f, 0.0f, 0.0f },
    { 1472, -4.056f, 0.0f, 0.0f },
    { 1563, -3.312f, 0.0f, 0.0f },
    { 1564, 715.55f, 0.0f, 0.0f },
    { 1747, 707.36f, 0.0f, 0.0f },
    { 1748, 1.72f, 0.0f, 0.0f },
    { 1839, -1.44f, 0.0f, 0.0f },
    { 1840, 0.0f, 0.0f, 0.0f },
    { 1931, 0.0f, 0.0f, 0.0f },
    { 1932, -7.89f, 0.0f, 0.0f },
    { 2115, -4.621f, 0.0f, 0.0f },
    { 2116, 2.748f, 0.0f, 0.0f },
    { 2161, 2.748f, 0.0f, 0.0f },
    { 2162, -2.96f, 0.0f, 0.0f },
    { 2207, -2.96f, 0.0f, 0.0f },
    { 2208, -1.959f, 0.0f, 0.0f },
    { 2276, -2.128f, 0.0f, 0.0f },
    { 2392, -2.128f, 0.0f, 0.0f },
};

struct Keyframe lbl_1_data_6124[] =
{
    { 0, 0.412f, 0.0f, 0.0f },
    { 23, 0.412f, 0.0f, 0.0f },
    { 69, -1.8f, 0.0f, 0.0f },
    { 91, -1.8f, 0.0f, 0.0f },
    { 92, 12.282f, 0.0f, 0.0f },
    { 275, 3.725f, 0.0f, 0.0f },
    { 276, 2.198f, 0.0f, 0.0f },
    { 459, 2.753f, 0.0f, 0.0f },
    { 460, 0.455f, 0.0f, 0.0f },
    { 643, 1.748f, 0.0f, 0.0f },
    { 644, 1.542f, 0.0f, 0.0f },
    { 827, 3.115f, 0.0f, 0.0f },
    { 828, 2.15f, 0.0f, -0.01f },
    { 1011, 0.2f, 0.0f, 0.0f },
    { 1012, 0.45f, 0.0f, 0.0f },
    { 1195, 0.01f, 0.0f, 0.0f },
    { 1196, 3.0f, 0.0f, 0.0f },
    { 1287, 2.72f, 0.0f, 0.0f },
    { 1288, -0.172f, 0.0f, 0.0f },
    { 1379, -0.145f, 0.0f, 0.0f },
    { 1380, 2.342f, 0.0f, 0.0f },
    { 1471, 1.396f, 0.0f, 0.0f },
    { 1472, 2.93f, 0.0f, -0.02f },
    { 1563, 0.159f, 0.0f, 0.0f },
    { 1564, 408.7f, 0.0f, 0.0f },
    { 1747, 406.64f, 0.0f, 0.0f },
    { 1748, 14.2f, 0.0f, 0.0f },
    { 1839, 1.04f, 0.0f, 0.0f },
    { 1840, 0.0f, 0.0f, 0.0f },
    { 1931, 0.0f, 0.0f, 0.0f },
    { 1932, 17.378f, 0.0f, 0.0f },
    { 2115, 17.884f, 0.0f, 0.0f },
    { 2116, 1.198f, 0.0f, 0.0f },
    { 2161, 1.198f, 0.0f, 0.0f },
    { 2162, 1.164f, 0.0f, 0.0f },
    { 2207, 1.164f, 0.0f, 0.0f },
    { 2208, 0.33f, 0.0f, 0.0f },
    { 2276, 1.425f, 0.0f, 0.0f },
    { 2392, 1.425f, 0.0f, 0.0f },
};

struct Keyframe lbl_1_data_6394[] =
{
    { 0, -2.066f, 0.0f, 0.0f },
    { 23, -2.066f, 0.0f, 0.0f },
    { 69, -92.6f, 0.0f, 0.0f },
    { 91, -92.6f, 0.0f, 0.0f },
    { 92, 1.545f, 0.0f, 0.0f },
    { 275, -2.099f, 0.0f, 0.0f },
    { 276, 7.802f, 0.0f, 0.0f },
    { 459, 1.908f, 0.0f, 0.0f },
    { 460, -1.246f, 0.0f, 0.0f },
    { 643, 1.569f, 0.0f, 0.0f },
    { 644, 3.051f, 0.0f, 0.0f },
    { 827, 10.224f, 0.0f, 0.0f },
    { 828, -2.53f, 0.0f, 0.01f },
    { 1011, -0.17f, 0.0f, 0.0f },
    { 1012, -0.18f, 0.0f, 0.0f },
    { 1195, 0.09f, 0.0f, 0.0f },
    { 1196, -98.99f, 0.0f, 0.0f },
    { 1287, -101.6f, 0.0f, 0.0f },
    { 1288, 0.17f, 0.0f, 0.0f },
    { 1379, 0.77f, 0.0f, 0.0f },
    { 1380, -65.91f, 0.0f, 0.0f },
    { 1471, -65.09f, 0.0f, 0.0f },
    { 1472, -1.18f, 0.0f, 0.0f },
    { 1563, 2.414f, 0.0f, 0.0f },
    { 1564, 174.65f, 0.0f, 0.0f },
    { 1747, 184.16f, 0.0f, 0.0f },
    { 1748, 15.26f, 0.0f, 0.0f },
    { 1839, 1.78f, 0.0f, 0.0f },
    { 1840, 0.0f, 0.0f, 0.0f },
    { 1931, 0.0f, 0.0f, 0.0f },
    { 1932, 23.313f, 0.0f, 0.0f },
    { 2115, 23.83f, 0.0f, 0.0f },
    { 2116, -5.245f, 0.0f, 0.0f },
    { 2161, -5.245f, 0.0f, 0.0f },
    { 2162, -2.272f, 0.0f, 0.0f },
    { 2207, -2.272f, 0.0f, 0.0f },
    { 2208, -2.272f, 0.0f, 0.0f },
    { 2276, -2.816f, 0.0f, 0.0f },
    { 2392, -2.816f, 0.0f, 0.0f },
};

struct Keyframe lbl_1_data_6604[] =
{
    { 0, 0.0f, 0.0f, 0.0f },
    { 828, -3072.0f, 0.0f, 0.0f },
    { 874, -2048.0f, 0.0f, 0.0f },
    { 1011, 0.0f, 0.0f, 0.0f },
    { 1012, 0.0f, 0.0f, 0.0f },
    { 1196, -3072.0f, 0.0f, 0.0f },
    { 1379, -1024.0f, 0.0f, 0.0f },
    { 1380, -3072.0f, 0.0f, 0.0f },
    { 1402, -3072.0f, 0.0f, 0.0f },
    { 1403, 0.0f, 0.0f, 0.0f },
    { 1563, 0.0f, 0.0f, 0.0f },
    { 1564, -3072.0f, 0.0f, 0.0f },
    { 1931, -3072.0f, 0.0f, 0.0f },
    { 2392, 0.0f, 0.0f, 0.0f },
};

struct Keyframe lbl_1_data_66E4[] =
{
    { 0, 0.0f, 0.0f, 0.0f },
    { 828, 0.0f, 0.0f, 0.0f },
    { 1011, 0.0f, 0.0f, 0.0f },
    { 1012, -1024.0f, 0.0f, 0.0f },
    { 1149, -1024.0f, 0.0f, 0.0f },
    { 1150, 0.0f, 0.0f, 0.0f },
    { 1196, 512.0f, 0.0f, 0.0f },
    { 1286, 256.0f, 0.0f, 0.0f },
    { 1290, -256.0f, 0.0f, 0.0f },
    { 1379, -512.0f, 0.0f, 0.0f },
    { 1380, 0.0f, 0.0f, 0.0f },
    { 2392, 0.0f, 0.0f, 0.0f },
};

struct Keyframe lbl_1_data_67A4[] =
{
    { 0, 0.0f, 0.0f, 0.0f },
    { 2392, 0.0f, 0.0f, 0.0f },
};

struct Keyframe lbl_1_data_67C4[] =
{
    { 0, 0.0f, 0.0f, 0.0f },
    { 2392, 0.0f, 0.0f, 0.0f },
};

struct Keyframe lbl_1_data_67E4[] =
{
    { 0, 0.0f, 0.0f, 0.0f },
    { 2392, 0.0f, 0.0f, 0.0f },
};

struct Keyframe lbl_1_data_6804[] =
{
    { 0, 0.0f, 0.0f, 0.0f },
    { 2392, 0.0f, 0.0f, 0.0f },
};

struct Keyframe lbl_1_data_6824[] =
{
    { 0, 0.0f, 0.0f, 0.0f },
    { 2392, 0.0f, 0.0f, 0.0f },
};

struct Keyframe lbl_1_data_6844[] =
{
    { 0, 0.0f, 0.0f, 0.0f },
    { 2392, 0.0f, 0.0f, 0.0f },
};

struct Keyframe *lbl_1_data_6864[] =
{
    lbl_1_data_6604,
    lbl_1_data_67A4,
    lbl_1_data_67E4,
    lbl_1_data_6824,
};

struct Keyframe *lbl_1_data_6874[] =
{
    lbl_1_data_66E4,
    lbl_1_data_67C4,
    lbl_1_data_6804,
    lbl_1_data_6844,
};

struct Keyframe lbl_1_data_6884[] =
{
    { 0, -140.0f, 0.0f, 0.0f },
    { 92, 167.0f, 0.0f, 0.0f },
    { 460, -21.7f, 0.0f, 0.0f },
    { 644, -21.7f, 0.0f, 0.0f },
    { 736, -1170.0f, 0.0f, 0.0f },
    { 828, -346.0f, 0.0f, 0.0f },
    { 2392, 0.0f, 0.0f, 0.0f },
};

struct Keyframe lbl_1_data_68F4[] =
{
    { 0, 71.0f, 0.0f, 0.0f },
    { 92, -214.0f, 0.0f, 0.0f },
    { 460, 3.4f, 0.0f, 0.0f },
    { 644, 3.4f, 0.0f, 0.0f },
    { 736, 986.6f, 0.0f, 0.0f },
    { 828, 639.6f, 0.0f, 0.0f },
    { 2392, 0.0f, 0.0f, 0.0f },
};

struct Keyframe lbl_1_data_6964[] =
{
    { 0, -71.0f, 0.0f, 0.0f },
    { 92, -819.0f, 0.0f, 0.0f },
    { 460, -14.6f, 0.0f, 0.0f },
    { 644, -14.6f, 0.0f, 0.0f },
    { 736, 295.0f, 0.0f, 0.0f },
    { 828, 1491.0f, 0.0f, 0.0f },
    { 2392, 0.0f, 0.0f, 0.0f },
};


void fn_1_F978(void)
{
    float t;
    int frame;
    int i;
    struct DemoApe *demoApe;
    struct Ape *ape;
    struct DemoApeCmd *cmd;

    t = lbl_1_data_4B28->beat;
    frame = 23.0f * t + lbl_1_data_4B28->subBeat;
    for (i = 0; i < 5; i++)
    {
        demoApe = &lbl_1_data_4B28->apes[i];
        ape = demoApe->ape;
        cmd = lbl_1_data_56B0[i];
        if (ape == NULL)
            continue;
        for (; cmd->beat != 0xFF; cmd++)
        {
            if (cmd->beat != lbl_1_data_4B28->beat || cmd->subBeat != lbl_1_data_4B28->subBeat)
                continue;
            if (cmd->cmd == -1)
                demoApe->animSpeed = cmd->value;
            else if (cmd->cmd == -2)
                demoApe->flags |= (u32)cmd->value;
            else if (cmd->cmd == -3)
                demoApe->flags &= ~(u32)cmd->value;
            else if (cmd->cmd == -4)
            {
                g_create_stage_player_wrapper(0, 21, i, 0xFF, 0, 0);
                balls[0].unk179 = 3;
                demoApe->flags |= 1;
            }
            else if (cmd->cmd == -5)
            {
                fn_1_47444(0);
                demoApe->flags &= ~1;
            }
            else if (cmd->cmd == -6)
                ape->scale = cmd->value;
            else
            {
                demoApe->unk4 = cmd->cmd;
                demoApe->unk5 = cmd->value;
            }
        }
        fn_1_11355C(ape, demoApe->animSpeed);
        fn_1_1132D0(ape, 0, demoApe->unk4, demoApe->unk5);
        ape->pos.x = fn_1_D2550(frame, lbl_1_data_5660[i]);
        ape->pos.y = fn_1_D2550(frame, lbl_1_data_5674[i]);
        ape->pos.z = fn_1_D2550(frame, lbl_1_data_5688[i]);
        demoApe->rotY = fn_1_D2550(frame, lbl_1_data_569C[i]);
        if (i == lbl_1_bss_6FB6C)
        {
            ape->pos.x += lbl_1_bss_6FB84;
            ape->pos.y += lbl_1_bss_6FB80;
            ape->pos.z += lbl_1_bss_6FB7C;
            demoApe->rotY += lbl_1_bss_6FB8A;
        }
        if (!(g_some_other_flags & 0xA))
        {
            g_ape_anim_body(ape);
            if (demoApe->flags & 2)
                g_ape_anim_head(ape, &lbl_1_data_4B28->bananaPos);
        }
    }
}

void fn_1_FD90(s8 arg0, int arg1)
{
    int group;
    int i;

    switch (arg0)
    {
    case 0:
        if (arg1 == 0)
        {
            for (i = 0; i < 5; i++)
            {
                lbl_1_data_4B28->apes[i].ape = NULL;
                if (i == 4)
                {
                    lbl_1_data_4B28->apes[i].unk4 = 0;
                    lbl_1_data_4B28->apes[i].unk5 = 0;
                }
                else
                {
                    lbl_1_data_4B28->apes[i].unk4 = 1;
                    lbl_1_data_4B28->apes[i].unk5 = 0;
                }
                lbl_1_data_4B28->apes[i].animSpeed = 1.0f;
                lbl_1_data_4B28->apes[i].flags = 0;
            }
        }
        disc_queue_status(1);
        group = g_swapDiscQueueGroup(1);
        if (arg1 == 0)
        {
            preload_ape(0, 0, 0, 0x18);
            preload_ape(1, 0, 0, 0x18);
            preload_ape(2, 0, 0, 0x18);
            preload_ape(3, 0, 0, 0x18);
        }
        else
            preload_ape(4, 0, 0, 0x18);
        g_swapDiscQueueGroup(group);
        lbl_1_data_4B28->unk18 = 1;
        lbl_1_data_4B28->unk19 = 0;
        lbl_1_data_4B28->unk1A = 0;
        break;
    case 1:
        lbl_1_data_4B28->apes[arg1].ape = load_ape(arg1, 0, 0x18, 0);
        break;
    }
}

void fn_1_FFC8(s8 arg0, int arg1)
{
    struct Sprite *sprite;

    switch (arg0)
    {
    case 0:
        lbl_1_data_4B28->bananaState = 2;
        break;
    case 1:
        lbl_1_data_4B28->bananaState = 1;
        break;
    case 2:
        lbl_1_data_4B28->bananaState = 0;
        break;
    case 3:
        create_demo_mes_sprite();
        break;
    case 4:
        sprite = get_sprite_with_unique_id(0x3A);
        if (sprite != NULL)
            sprite->userVar = 1;
        break;
    case 5:
        create_demo_mask_sprites();
        break;
    }
}

void fn_1_1008C(s8 arg0, int arg1)
{
    switch (arg0)
    {
    case 0:
        lbl_1_data_4B28->camFlags |= 1;
        memcpy(&lbl_1_data_4B28->cam, &lbl_1_data_56C4[arg1], sizeof(struct DemoCam));
        break;
    case 1:
        lbl_1_data_4B28->camFlags &= ~1;
        break;
    case 2:
        lbl_1_data_4B28->camFlags |= arg1;
        break;
    case 3:
        lbl_1_data_4B28->camFlags &= ~arg1;
        break;
    case 4:
        lbl_1_data_4B28->camFlags |= 2;
        memcpy(&lbl_1_data_4B28->cam, &lbl_1_data_5714[arg1], sizeof(struct DemoCam));
        break;
    case 5:
        lbl_1_data_4B28->camFlags &= ~6;
        break;
    }
}

void fn_1_101AC(struct Camera *camera, struct Ball *ball)
{
    camera->flags |= 8;
}

void g_camera_func30_child(struct Camera *camera, struct Ball *ball)
{
    float y, x, z;
    Vec prevEye;
    Vec prevLookAt;
    Vec delta;
    float t;
    int frame;
    s16 rotX;
    s16 rotY;

    t = lbl_1_data_4B28->beat;
    prevEye = camera->eye;
    prevLookAt = camera->lookAt;
    frame = 23.0f * t + lbl_1_data_4B28->subBeat;
    if (!(lbl_1_data_4B28->camFlags & 4))
    {
        camera->eye.x = fn_1_D2550(frame * 60 / lbl_80198B38, lbl_1_data_5764);
        camera->eye.y = fn_1_D2550(frame * 60 / lbl_80198B38, lbl_1_data_59D4);
        camera->eye.z = fn_1_D2550(frame * 60 / lbl_80198B38, lbl_1_data_5C44);
        camera->lookAt.x = fn_1_D2550(frame * 60 / lbl_80198B38, lbl_1_data_5EB4);
        camera->lookAt.y = fn_1_D2550(frame * 60 / lbl_80198B38, lbl_1_data_6124);
        camera->lookAt.z = fn_1_D2550(frame * 60 / lbl_80198B38, lbl_1_data_6394);
        if ((lbl_1_data_4B28->camFlags & 0x20) && balls[0].ape != NULL)
        {
            camera->eye.x += balls[0].ape->pos.x;
            camera->eye.y += balls[0].ape->pos.y;
            camera->eye.z += balls[0].ape->pos.z;
            camera->lookAt.x += balls[0].ape->pos.x;
            camera->lookAt.y += balls[0].ape->pos.y;
            camera->lookAt.z += balls[0].ape->pos.z;
        }
    }
    if (lbl_1_data_4B28->camFlags & 1)
    {
        x = lbl_1_data_4B28->cam.unk0.x - camera->eye.x;
        y = lbl_1_data_4B28->cam.unk0.y - camera->eye.y;
        z = lbl_1_data_4B28->cam.unk0.z - camera->eye.z;
        rotY = math_atan2(x, z) - 0x8000;
        rotX = math_atan2(y, math_sqrt(demo_sum_of_sq_2(x, z)));
        if (lbl_1_data_4B28->camFlags & 8)
        {
            if (rotY - lbl_1_data_4B28->eyeRotY > lbl_1_data_4B28->cam.unk26)
                rotY = lbl_1_data_4B28->eyeRotY + lbl_1_data_4B28->cam.unk26;
            else if (rotY - lbl_1_data_4B28->eyeRotY < -lbl_1_data_4B28->cam.unk26)
                rotY = lbl_1_data_4B28->eyeRotY - lbl_1_data_4B28->cam.unk26;
            if (rotX - lbl_1_data_4B28->eyeRotX > lbl_1_data_4B28->cam.unk24)
                rotX = lbl_1_data_4B28->eyeRotX + lbl_1_data_4B28->cam.unk24;
            else if (rotX - lbl_1_data_4B28->eyeRotX < -lbl_1_data_4B28->cam.unk24)
                rotX = lbl_1_data_4B28->eyeRotX - lbl_1_data_4B28->cam.unk24;
        }
        mtxa_from_identity();
        mtxa_rotate_y(rotY);
        mtxa_rotate_x(rotX);
        mathutil_mtxA_tf_point_z(lbl_1_data_4B28->cam.unk18, &delta);
        if (__fabs(delta.x) > __fabs(x))
            delta.x *= __fabs(x) / __fabs(delta.x);
        if (__fabs(delta.y) > __fabs(y))
            delta.y *= __fabs(y) / __fabs(delta.y);
        if (__fabs(delta.z) > __fabs(z))
            delta.z *= __fabs(z) / __fabs(delta.z);
        if (!(g_some_other_flags & 0xA))
        {
            camera->eye.x += delta.x;
            camera->eye.y += delta.y;
            camera->eye.z += delta.z;
        }
        x = lbl_1_data_4B28->cam.unkC.x - camera->lookAt.x;
        y = lbl_1_data_4B28->cam.unkC.y - camera->lookAt.y;
        z = lbl_1_data_4B28->cam.unkC.z - camera->lookAt.z;
        rotY = math_atan2(x, z) - 0x8000;
        rotX = math_atan2(y, math_sqrt(demo_sum_of_sq_2(x, z)));
        mtxa_from_identity();
        mtxa_rotate_y(rotY);
        mtxa_rotate_x(rotX);
        mathutil_mtxA_tf_point_z(lbl_1_data_4B28->cam.unk18, &delta);
        if (__fabs(delta.x) > __fabs(x))
            delta.x *= __fabs(x) / __fabs(delta.x);
        if (__fabs(delta.y) > __fabs(y))
            delta.y *= __fabs(y) / __fabs(delta.y);
        if (__fabs(delta.z) > __fabs(z))
            delta.z *= __fabs(z) / __fabs(delta.z);
        if (!(g_some_other_flags & 0xA))
        {
            camera->lookAt.x += delta.x;
            camera->lookAt.y += delta.y;
            camera->lookAt.z += delta.z;
        }
        if (!(g_some_other_flags & 0xA))
        {
            lbl_1_data_4B28->cam.unk18 += lbl_1_data_4B28->cam.unk1C;
            if (lbl_1_data_4B28->camFlags & 0x10)
            {
                if (__fabs(lbl_1_data_4B28->cam.unk18) > __fabs(lbl_1_data_4B28->cam.unk20))
                    lbl_1_data_4B28->cam.unk18 = lbl_1_data_4B28->cam.unk20;
            }
            else
                lbl_1_data_4B28->cam.unk1C *= lbl_1_data_4B28->cam.unk20;
        }
    }
    else if (lbl_1_data_4B28->camFlags & 2)
    {
        if (!(g_some_other_flags & 0xA))
        {
            camera->eye.x += lbl_1_data_4B28->cam.unk18 * (lbl_1_data_4B28->cam.unk0.x - camera->eye.x);
            camera->eye.y += lbl_1_data_4B28->cam.unk18 * (lbl_1_data_4B28->cam.unk0.y - camera->eye.y);
            camera->eye.z += lbl_1_data_4B28->cam.unk18 * (lbl_1_data_4B28->cam.unk0.z - camera->eye.z);
            camera->lookAt.x += lbl_1_data_4B28->cam.unk1C * (lbl_1_data_4B28->cam.unkC.x - camera->lookAt.x);
            camera->lookAt.y += lbl_1_data_4B28->cam.unk1C * (lbl_1_data_4B28->cam.unkC.y - camera->lookAt.y);
            camera->lookAt.z += lbl_1_data_4B28->cam.unk1C * (lbl_1_data_4B28->cam.unkC.z - camera->lookAt.z);
        }
        lbl_1_data_4B28->camFlags |= 4;
    }
    x = camera->lookAt.x - camera->eye.x;
    y = camera->lookAt.y - camera->eye.y;
    z = camera->lookAt.z - camera->eye.z;
    camera->rotY = math_atan2(x, z) - 0x8000;
    x = x * x;
    x = __fmadds(z, z, x);
    camera->rotX = math_atan2(y, math_sqrt(x));
    camera->rotZ = 0;
    if (!(g_some_other_flags & 0xA))
    {
        x = camera->eye.x - prevEye.x;
        y = camera->eye.y - prevEye.y;
        z = camera->eye.z - prevEye.z;
        lbl_1_data_4B28->eyeRotY = math_atan2(x, z) - 0x8000;
        x = x * x;
        x = __fmadds(z, z, x);
        lbl_1_data_4B28->eyeRotX = math_atan2(y, math_sqrt(x));
        x = camera->lookAt.x - prevLookAt.x;
        y = camera->lookAt.y - prevLookAt.y;
        z = camera->lookAt.z - prevLookAt.z;
        lbl_1_data_4B28->lookAtRotY = math_atan2(x, z) - 0x8000;
        x = x * x;
        x = __fmadds(z, z, x);
        lbl_1_data_4B28->lookAtRotX = math_atan2(y, math_sqrt(x));
        lbl_1_data_4B28->eyeSpeed = mathutil_vec_distance(&camera->eye, &prevEye);
        lbl_1_data_4B28->lookAtSpeed = mathutil_vec_distance(&camera->lookAt, &prevLookAt);
    }
}

void fn_1_10D34(struct Ball *ball)
{
    Vec startPos[4] =
    {
        {  0.5f, 0.5f, -88.0f },
        { 28.0f, 0.5f, -93.0f },
        { -3.8f, 0.5f, -70.5f },
        { 698.0f, 447.0f, 243.0f },
    };
    s16 startRotY[4] = { 0x1000, 0, 0, 0 };

    g_reset_ball(ball);
    ball->pos.x = startPos[ball->ape->charaId].x;
    ball->pos.y = startPos[ball->ape->charaId].y;
    ball->pos.z = startPos[ball->ape->charaId].z;
    ball->prevPos.x = ball->pos.x;
    ball->prevPos.y = ball->pos.y;
    ball->prevPos.z = ball->pos.z;
    mtxa_from_translate(&ball->pos);
    mtxa_to_mtx(ball->unk30);
    ball->flags &= ~0x10;
    ball->flags |= 0x4000;
    if (ball->ape != NULL)
        ball->ape->flags &= ~0x20;
    {
        Vec dir = { 0.0f, 0.0f, -1.0f };

        mtxa_from_rotate_y(startRotY[ball->ape->charaId]);
        mtxa_tf_vec(&dir, &dir);
        g_smth_with_ape_rotation(ball->ape, &dir);
    }
    ball->state = 4;
    ball->speed = ball->unkCC = 0.0f;
    {
        Vec vel = { 0.0f, 0.0f, 0.0f };
        Vec *src = &vel;

        ball->unkC0 = *src;
    }
    ball->ape->flags &= ~0x4000;
    {
        Quat rot = { 0.0f, 0.0f, 0.0f, 1.0f };
        Quat *src = &rot;

        ball->unkA0 = *src;
    }
    mtxa_to_quat(&ball->unkB0);
    set_ape_anim(ball->ape, 5);
}

void fn_1_10FA8(struct World *world)
{
    float t;
    int frame;
    int rotX;
    Vec up;
    Quat q;

    t = lbl_1_data_4B28->beat;
    frame = 23.0f * t + lbl_1_data_4B28->subBeat;
    rotX = fn_1_D2550(frame * 60 / lbl_80198B38, lbl_1_data_6864[0]);
    frame = fn_1_D2550(frame * 60 / lbl_80198B38, lbl_1_data_6874[0]);
    mtxa_from_identity();
    mtxa_rotate_x(rotX);
    mtxa_rotate_z(frame);
    up.x = 0.0f;
    up.y = 1.0f;
    up.z = 0.0f;
    mtxa_tf_vec(&up, &up);
    up.x = world->unk20.x + 0.2 * (up.x - world->unk20.x);
    up.y = world->unk20.y + 0.2 * (up.y - world->unk20.y);
    up.z = world->unk20.z + 0.2 * (up.z - world->unk20.z);
    world->xrot = math_atan2(up.z, up.y);
    world->zrot = -math_atan2(up.x, math_sqrt(mathutil_sum_of_sq_2(up.z, up.y)));
    vec_normalize_len(&up);
    world->unk20 = up;
    {
        Vec dflt = { 0.0f, 1.0f, 0.0f };

        quat_from_dirs(&q, &dflt, &up);
    }
    mtxa_from_quat(&q);
    mtxa_rigid_inv_tf_point_xyz(0.0f, -1.0f, 0.0f, &world->unk10);
}

void fn_1_1120C(void)
{
    struct Effect effect;
    float t;
    int frame;
    int i;

    t = lbl_1_data_4B28->beat;
    frame = 23.0f * t + lbl_1_data_4B28->subBeat;
    lbl_1_data_4B28->bananaPos.x = fn_1_D2550(frame, lbl_1_data_6884);
    lbl_1_data_4B28->bananaPos.y = fn_1_D2550(frame, lbl_1_data_68F4);
    lbl_1_data_4B28->bananaPos.z = fn_1_D2550(frame, lbl_1_data_6964);
    lbl_1_data_4B28->bananaRotY -= 0x200;
    if (!(g_some_other_flags & 0xA))
    {
        for (i = 0; i < 3; i++)
        {
            memset(&effect, 0, sizeof(effect));
            effect.type = 4;
            effect.playerId = 0;
            effect.pos = lbl_1_data_4B28->bananaPos;
            spawn_effect(&effect);
        }
    }
}

char *lbl_1_data_6A64[] =
{
    "I really love bananas!",
    "Ich liebe Bananen!",
    "J'adore les bananes !",
    "\241Adoro las bananas!",
    "Adoro le banane!",
    "\203{\203N\202\315\203o\203i\203i\202\252\203_\201`\203C\203X\203L\202\310\202\361\202\276\201I",
};

char *lbl_1_data_6A90[] =
{
    "AIAI",
    "AIAI",
    "AIAI",
    "AIAI",
    "AIAI",
    "\203A\203C\203A\203C",
};


void create_demo_mes_sprite(void)
{
    struct Sprite *sprite = create_sprite();

    if (sprite != NULL)
    {
        sprite->tag = 0x3A;
        sprite->x = 80.0f;
        sprite->y = 80.0f;
        sprite->fontId = 3;
        sprite->mulR = 0xFF;
        sprite->mulG = 0xFF;
        sprite->mulB = 0x80;
        sprite->userVar = 0;
        sprite->counter = 0;
        sprite->mainFunc = sprite_demo_mes_tick;
        sprite->drawFunc = sprite_demo_mes_disp;
        sprite->flags |= 0x1000000;
        sprite->widescreenArg = 0x60;
        strcpy(sprite->text, "demo_mes");
    }
}

void sprite_demo_mes_tick(s8 *status, struct Sprite *sprite)
{
    sprite->counter++;
    if (sprite->userVar == 1)
        sprite->opacity = sprite->opacity + 0.1 * -sprite->opacity;
}

#define CLAMP01(x) ((x) < 0.0 ? 0.0 : ((x) > 1.0 ? 1.0 : (x)))

void sprite_demo_mes_disp(struct Sprite *sprite)
{
    u8 ch[3] = { 0, 0, 0 };
    u8 text[0x104];
    float width;
    float alpha;
    int i;
    int pos;
    s8 len;

    strcpy((char *)text, lbl_1_data_6A64[locale]);
    len = g_smth_with_fonts_chara_load_wrapper_discard_result((char *)text);
    textdraw_reset();
    textdraw_set_font(sprite->fontId);
    textdraw_set_alignment(sprite->textAlign);
    textdraw_set_depth(sprite->depth);
    textdraw_set_scale(sprite->scaleX, sprite->scaleY);
    textdraw_set_flags(sprite->flags);
    textdraw_set_mul_color((sprite->mulR << 16) | (sprite->mulG << 8) | sprite->mulB);
    textdraw_set_border();
    textdraw_set_font_style(5);
    textdraw_set_flags(0x80000000);
    textdraw_set_rot_z(0xF700);
    width = textdraw_get_pixel_width_of_string_as_f32((char *)text);
    textdraw_set_spacing(1.0f, 0.0f);
    textdraw_set_pos(sprite->x, sprite->y);
    for (i = 0, pos = 0; i < len; i++, pos++)
    {
        alpha = sprite->opacity * CLAMP01((float)(sprite->counter - (i << 2)) / 16.0);
        ch[0] = text[pos];
#ifdef __MWERKS__
        if (ch[0] >= 0x80)
#else
        if (pc_text_is_lead(ch[0]))
#endif
        {
            pos++;
            ch[1] = text[pos];
        }
        else
            ch[1] = 0;
        textdraw_set_alpha(alpha);
        g_textdraw_set_unk6(alpha);
        textdraw_print((char *)ch);
    }
    alpha = sprite->opacity * CLAMP01((float)(sprite->counter - (len << 2) - 45) / 32.0);
    textdraw_set_spacing(1.0f, 0.0f);
    textdraw_set_scale(sprite->scaleX, 0.8f * sprite->scaleY);
    textdraw_set_alpha(alpha);
    g_textdraw_set_unk6(alpha);
    textdraw_set_alignment(6);
    textdraw_set_pos(len + (10.0f + (sprite->x + width)), 36.0f + sprite->y);
    textdraw_print(lbl_1_data_6A90[locale]);
}

void create_demo_mask_sprites(void)
{
    struct Sprite *sprite = create_sprite();

    if (sprite != NULL)
    {
        sprite->type = 1;
        sprite->x = 320.0f;
        sprite->y = 240.1f;
        sprite->textAlign = 4;
        sprite->bmpId = 0x4B;
        sprite->depth = 0.01f;
        sprite->opacity = 1.0f;
        sprite->scaleX = 128.0f;
        sprite->scaleY = 60.0f;
        sprite->mulR = 0;
        sprite->mulG = 0;
        sprite->mulB = 0;
        sprintf(sprite->text, "demo_mask");
    }
    sprite = create_sprite();
    if (sprite != NULL)
    {
        sprite->type = 1;
        sprite->x = 320.0f;
        sprite->y = 240.1f;
        sprite->textAlign = 4;
        sprite->bmpId = 0x59;
        sprite->depth = 0.1f;
        sprite->opacity = 0.02f;
        sprite->scaleX = 10.0f;
        sprite->scaleY = 10.0f;
        sprite->mulR = 0;
        sprite->mulG = 0;
        sprite->mulB = 0;
        sprite->flags |= 0x40000;
        sprite->counter = 0;
        sprite->mainFunc = sprite_demo_mask_banana_tick;
        sprintf(sprite->text, "demo_mask_banana");
    }
}

void sprite_demo_mask_banana_tick(s8 *status, struct Sprite *sprite)
{
    if (sprite->scaleX < 1.51f)
    {
        if (++sprite->counter > 46)
        {
            sprite->scaleX += 0.2f * -sprite->scaleX;
            sprite->scaleY += 0.2f * -sprite->scaleY;
        }
    }
    else
    {
        sprite->scaleX += 0.2f * (1.5f - sprite->scaleX);
        sprite->scaleY += 0.2f * (1.5f - sprite->scaleY);
    }
    switch ((u8)g_screenfade_flags.type)
    {
    case 0:
        sprite->opacity = CLAMP01(0.02f + (float)g_screenfade_flags.timer / (float)g_screenfade_flags.timerMax);
        break;
    case 1:
        sprite->opacity = CLAMP01((1.0 + 0.02f) - (float)g_screenfade_flags.timer / (float)g_screenfade_flags.timerMax);
        break;
    }
}

struct DemoShadowBone lbl_1_data_6AD4[] =
{
    { 1, 0.2f, { 0xE8, 0xF8, 0xFF, 0xC0 } },
    { 8, 0.1f, { 0x74, 0x7C, 0x80, 0xFF } },
    { 12, 0.1f, { 0x74, 0x7C, 0x80, 0xFF } },
    { 18, 0.1f, { 0x74, 0x7C, 0x80, 0xFF } },
    { 23, 0.1f, { 0x74, 0x7C, 0x80, 0xFF } },
    { -1, 0.0f, { 0x00, 0x00, 0x00, 0x00 } },
};

struct DemoShadowBone lbl_1_data_6B1C[] =
{
    { 1, 0.25f, { 0xEC, 0xF8, 0xFF, 0xFF } },
    { 8, 0.2f, { 0x74, 0x7C, 0x80, 0xFF } },
    { 12, 0.2f, { 0x74, 0x7C, 0x80, 0xFF } },
    { 18, 0.2f, { 0x74, 0x7C, 0x80, 0xFF } },
    { 23, 0.2f, { 0x74, 0x7C, 0x80, 0xFF } },
    { -1, 0.0f, { 0x00, 0x00, 0x00, 0x00 } },
};


struct DemoShadowList lbl_1_data_6B64[] =
{
    { 3, lbl_1_data_6B1C },
};

void fn_1_11D60(void)
{
    float lightR, lightG, lightB;
    float finalR, finalG, finalB;
    float apeScale;
    float t;
    float invRadius;
    float r, g, b;
    int i;
    struct Ape *ape;
    struct DemoShadowBone *bones;
    struct DemoShadowBone *bone;
    struct GMAModel *model;
    struct DemoApe *demoApe;
    int n;
    struct DemoShadowList *entry;
    int hit;
    S16Vec rot;
    Vec rayPos;
    Vec pos;
    struct RaycastHit rayHit;
    Mtx mtx;

    model = init_common_gma->modelEntries[103].model;
    invRadius = 1.0f / model->boundSphereRadius;
    lightR = lbl_1_bss_2F48.ambient.r;
    lightG = lbl_1_bss_2F48.ambient.g;
    lightB = lbl_1_bss_2F48.ambient.b;
    avdisp_set_z_mode(1, 3, 0);
    for (i = 0; i < 5; i++)
    {
        demoApe = &lbl_1_data_4B28->apes[i];
        ape = demoApe->ape;
        if (ape == NULL || (demoApe->flags & 4) || (demoApe->flags & 1))
            continue;
        mtxa_from_quat(&ape->rot);
        mtxa_to_mtx(mtx);
        mtxa_from_translate(&ape->pos);
        mtxa_scale_s(ape->scale);
        mtxa_translate(&ape->unk264);
        mtxa_mult_right(mtx);
        entry = lbl_1_data_6B64;
        for (n = 1; n > 0; n--, entry++)
        {
            if (ape->charaId == entry->charaId)
            {
                bones = entry->bones;
                break;
            }
        }
        if (n == 0)
            bones = lbl_1_data_6AD4;
        apeScale = ape->scale;
        bone = bones;
        while (bone->bone >= 0)
        {
            mtxa_push();
            mtxa_mult_right(g_get_ape_anim_smth(ape, bone->bone));
            mathutil_mtxA_get_translate(&rayPos);
            rayPos.y += 0.2f;
            hit = raycast_stage_down(&rayPos, &rayHit, NULL);
            mtxa_pop();
            if (hit != 0)
            {
                t = 1.0 - (1.0 / (2.0f * apeScale)) * (rayPos.y - rayHit.pos.y);
                if (t < 0.0f)
                    continue;
                t *= 0.003921569f;
                r = bone->color[0] * (0.75f * (1.0f - lightR));
                g = bone->color[1] * (0.75f * (1.0f - lightG));
                b = bone->color[2] * (0.75f * (1.0f - lightB));
                finalR = r * t;
                finalG = g * t;
                finalB = b * t;
                t = (1.0f + 2.0f * (1.0f - t)) * (apeScale * bone->scale);
                vec_to_euler(&rayHit.normal, &rot);
                mtxa_push();
                mtxa_from_mtxb_translate(&rayHit.pos);
                mtxa_rotate_y(rot.y);
                mtxa_rotate_x(rot.x);
                mtxa_scale_s(t * invRadius);
                if (g_is_sphere_visible(&model->boundSphereCenter, t))
                {
                    mathutil_mtxA_get_translate(&pos);
                    t = mathutil_vec_len(&pos);
                    mtxa_to_mtx(mtx);
                    mtxa_from_identity();
                    mtxa_scale_s((t - 0.1f) / t);
                    mtxa_mult_right(mtx);
                    set_post_mult_color(finalR, finalG, finalB, 1.0f);
                    g_smth_with_gpu_and_locked_cache_mtx();
                    avdisp_draw_model_unculled_sort_never(model);
                }
                mtxa_pop();
            }
            bone++;
        }
    }
    fade_color_base_default();
    avdisp_set_z_mode(1, 3, 1);
}
