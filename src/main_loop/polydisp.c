#include "types.h"
#include "avdisp.h"
#include "background.h"
#include "ball.h"
#include "bitmap.h"
#include "camera.h"
#include "dip.h"
#include "event.h"
#include "gx.h"
#include "input.h"
#include "light.h"
#include "mathutil.h"
#include "mode.h"
#include "nl2ngc.h"
#include "pool.h"
#include "world.h"
#ifndef __MWERKS__
#include "frustum.h" /* prototypes: float args must not be promoted to double on x86 */
#endif

u32 lbl_1_data_2CC8[] =
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

/* Defined at the end of the file, so accesses aren't pooled. */
struct SomeRenderFlag
{
    /*0x00*/ u32 flags;
    /*0x04*/ Vec unk4;
    /*0x10*/ Vec unk10;
    u8 filler1C[4];
    /*0x20*/ Vec unk20;
    /*0x2C*/ Vec unk2C;
    u8 filler38[4];
    /*0x3C*/ Vec unk3C;
    /*0x48*/ Vec unk48;
    u8 filler54[4];
    /*0x58*/ GXColor bgColor;
};

extern struct SomeRenderFlag g_some_render_flag;

struct PauseMenuScreenshot
{
    /*0x00*/ s32 requested;
    /*0x04*/ void *dest;
    /*0x08*/ u16 left;
    /*0x0A*/ u16 top;
    /*0x0C*/ u16 width;
    /*0x0E*/ u16 height;
    /*0x10*/ int format;
};

extern struct PauseMenuScreenshot g_pausemenu_screenshot_requested;

struct Unk1470
{
    float unk0;
    float r;
    float g;
    float b;
};

extern struct Unk1470 lbl_1_bss_1470;

struct Unk1480
{
    /*0x00*/ GXTexObj unk0;
    /*0x20*/ GXTexObj unk20;
    /*0x40*/ GXTexObj *unk40;
    u8 filler44[0x74 - 0x44];
    /*0x74*/ Mtx unk74;
};

extern struct Unk1480 lbl_1_bss_1480;

extern Vec lbl_1_bss_1524;

extern s32 g_sub_mode3;
extern s32 lbl_1_bss_1538;
extern u32 lbl_1_bss_153C;
extern struct
{
    /*0x00*/ Vec pos;
    /*0x0C*/ s16 xrot;
    /*0x0E*/ s16 zrot;
} lbl_1_bss_1540;
extern struct
{
    /*0x00*/ s32 stickXRot;
    /*0x04*/ s32 stickZRot;
    /*0x08*/ s32 state;
    /*0x0C*/ float t;
} lbl_1_bss_1568;
extern s16 lbl_1_bss_243E;
extern float lbl_1_bss_2440;
extern struct
{
    u8 filler0[0xC];
    /*0x0C*/ u8 r;
    /*0x0D*/ u8 g;
    /*0x0E*/ u8 b;
    /*0x0F*/ s8 enabled;
} lbl_1_bss_CF22C;
extern Vec lbl_1_bss_D4244;
extern void *lbl_801988E0;
extern s8 g_related_to_fov2;
extern u32 g_some_other_flags;
extern u32 g_font_char_var_3;
extern u32 frame_counter;
extern struct ViewMatrices *g_ord_tbl_stuff;
extern void (*test_draw_func_ptr)(void);
extern GXRenderModeObj *current_render_mode;

void debug_textdraw_set_pos(int, int);
void debug_textdraw_vsprintf_buf_1(char *fmt, ...);
void debug_textdraw_put_buf_1(char *);
void g_reset_draw_perf_counters(void);
int fn_1_D36C8(void);
int g_get_debug_level_select_loading_left_asterisks(void);
int g_check_flag_related_to_pausing(void);
void fog_main(void);
void g_something_with_rend_efc(int);
void g_something_with_view_stage2(void);
void debug_draw_shadow_textures(void);
void g_something_with_name_entry(void);
void g_draw_minimap(void);
void g_set_something3(int);
void g_something_with_raster_state(int);
void GXSetTexCopySrc_cached(u16 left, u16 top, u16 width, u16 height);
void GXSetTexCopyDst_cached(u16 width, u16 height, int format, u8 mipmap);
void GXCopyTex(void *dest, u8 clear);
void u_draw_naomi_ball(void);
void g_sel_draw_func_handler(void);
void game_roll_draw_func(void);
void game_continue_draw_func(void);
void game_nameentry_draw_func(void);
void game_scenario_draw_func(void);
void g_mini_commend_draw_func(void);
void mini_ending_draw_func(void);
void mini_nameentry_draw_func(void);
void g_mini_draw_func_handler(void);
void author_draw_func(void);
void fn_1_F4C8(void);
void ord_tbl_draw_nodes(void);
void g_maybe_draw_sega_splash(void);
void fn_1_D4870(void);
void g_maybe_some_bg_effect_handler(void);
void g_draw_stage(void);
void draw_shadow_reqs(void);
void g_smth_with_rendefc_reflective_height(float);
void g_draw_bg(void);
void draw_items(void);
void g_draw_stobjs(void);
void g_draw_effects(void);
void g_draw_ball_and_ape(void);
void fn_1_D6F9C(int);
void fn_1_D6D84(int);
void g_something_with_GX_vtx_desc(u32 attrs);
void GXSetCullMode_cached(int mode);
void g_GXSetTevIndirect_zero_if_different(int stage);
void g_GXSetTevOrder_wrapper(int stage, int coord, int map, int color);
void GXSetTevKAlphaSel_cached(int stage, int sel);
void GXSetTevKColorSel_cached(int stage, int sel);
void GXSetTevKColor_cached(int id, GXColor color);
void GXSetTevColorIn_cached(int stage, int a, int b, int c, int d);
void GXSetTevColorOp_cached(int stage, int op, int bias, int scale, u8 clamp, int outReg);
void GXSetTevAlphaIn_cached(int stage, int a, int b, int c, int d);
void GXSetTevAlphaOp_cached(int stage, int op, int bias, int scale, u8 clamp, int outReg);
void GXSetNumTevStages_cached(u8 n);
void GXSetNumChans_cached(u8 n);
void GXSetNumTexGens_cached(u8 n);
void GXSetNumIndStages_cached(u8 n);
void GXLoadPosMtxImm(Mtx m, u32 id);
void GXLoadNrmMtxImm(Mtx m, u32 id);
void GXSetChanMatColor(int chan, GXColor color);
void GXSetChanAmbColor(int chan, GXColor color);
void opti_GXSetChanCtrl(int chan, u8 enable, int ambSrc, int matSrc, u32 lightMask, int diffFn, int attnFn);
void g_GXSetTevColorIn_GXSetTevAlphaIn_wrapper(int stage, int mode);
void GXDrawSphere(u8 numMajor, u8 numMinor);
void GXGetProjectionv(float *proj);
void GXSetCopyClear_cached(GXColor color, u32 z);

void draw_func_handler(void);
void adv_draw_func_handler(void);
void fn_1_591C(void);
void fn_1_59C8(void);
void fn_1_61C8(void);
void game_main_draw_func(void);
void g_draw_world_child(void);
void fn_1_6BB8(void);
void game_extra_draw_func(void);
void game_result_draw_func(void);
void handle_test_camera(void);
void g_set_clear_color(void);
void g_smth_with_bg_color_drawing(GXColor *color);
void g_draw_func_init(void);
void draw_stage_shadows(void);
void g_reset_shadow_state(void);

#define CLAMP(x, lo, hi) ((x) < (lo) ? (lo) : ((x) > (hi) ? (hi) : (x)))

struct Throbber
{
    u8 chars[4];
};

const struct Throbber s_throbber = {{'-', '\\', '|', '/'}};

static inline void show_loading_msg(void)
{
    struct Throbber throbber = s_throbber;
    u32 idx;
    int n;

    debug_textdraw_set_pos(14, 15);
    idx = (g_font_char_var_3 / 2) % 4;
    debug_textdraw_vsprintf_buf_1("%c", throbber.chars[idx]);
    debug_textdraw_put_buf_1("NOW LOADING");
    debug_textdraw_vsprintf_buf_1("%c", throbber.chars[idx]);
    debug_textdraw_set_pos(15, 16);
    debug_textdraw_put_buf_1("LEFT: ");
    n = g_get_debug_level_select_loading_left_asterisks();
    while (n > 0)
    {
        debug_textdraw_put_buf_1("*");
        n--;
    }
}

void polydisp_main(void)
{
    g_reset_draw_perf_counters();
    if (main_mode == 1 && (sub_mode == 0x1D || sub_mode == 0x1E) && fn_1_D36C8())
        show_loading_msg();
    g_some_render_flag.flags &= ~0x11;
    if (g_check_flag_related_to_pausing())
        g_some_render_flag.flags |= 0x10;
    light_main();
    fog_main();
    if (events[EVENT_VIEW].state != EV_STATE_RUNNING)
        draw_stage_shadows();
    g_set_clear_color();
    if (events[EVENT_VIEW].state != EV_STATE_RUNNING)
    {
        if (events[EVENT_REND_EFC].state == EV_STATE_RUNNING)
            g_something_with_rend_efc(1);
        draw_func_handler();
        if (events[EVENT_REND_EFC].state == EV_STATE_RUNNING)
            g_something_with_rend_efc(2);
    }
    else
        g_something_with_view_stage2();
    if (dip_switches & 0x20000)
        debug_draw_shadow_textures();
    g_reset_shadow_state();
    if (events[EVENT_NAME_ENTRY].state == EV_STATE_RUNNING)
        g_something_with_name_entry();
    if (modeCtrl.unk30 <= 1 && events[EVENT_VIEW].state == EV_STATE_INACTIVE
     && events[EVENT_MINIMAP].state == EV_STATE_RUNNING)
        g_draw_minimap();
    g_set_something3(0);
    g_something_with_raster_state(0);
    if (g_pausemenu_screenshot_requested.requested != 0)
    {
        GXSetTexCopySrc_cached(g_pausemenu_screenshot_requested.left,
                               g_pausemenu_screenshot_requested.top,
                               g_pausemenu_screenshot_requested.width,
                               g_pausemenu_screenshot_requested.height);
        GXSetTexCopyDst_cached(g_pausemenu_screenshot_requested.width,
                               g_pausemenu_screenshot_requested.height,
                               g_pausemenu_screenshot_requested.format, 0);
        GXCopyTex(g_pausemenu_screenshot_requested.dest, 0);
        g_pausemenu_screenshot_requested.requested = 0;
    }
}

void draw_func_handler(void)
{
    u_draw_naomi_ball();
    switch (main_mode)
    {
    default:
        handle_test_camera();
        break;
    case 0:
        adv_draw_func_handler();
        break;
    case 1:
        g_draw_func_init();
        handle_test_camera();
        g_sel_draw_func_handler();
        break;
    case 2:
        switch (sub_mode)
        {
        default:
            game_main_draw_func();
            break;
        case 0x4C:
        case 0x4D:
        case 0x4E:
            game_result_draw_func();
            break;
        case 0x53:
        case 0x54:
            game_roll_draw_func();
            handle_test_camera();
            break;
        case 0x41:
        case 0x42:
            game_continue_draw_func();
            handle_test_camera();
            break;
        case 0x4A:
        case 0x4B:
            game_extra_draw_func();
            handle_test_camera();
            break;
        case 0x3F:
        case 0x40:
            game_nameentry_draw_func();
            handle_test_camera();
            break;
        case 0x3D:
        case 0x3E:
        case 0x44:
            switch (modeCtrl.gameType)
            {
            case 1:
                game_main_draw_func();
                break;
            default:
                if (modeCtrl.courseFlags & 0x20)
                    game_main_draw_func();
                break;
            }
            break;
        case 0x57:
        case 0x58:
            game_scenario_draw_func();
            break;
        case 0x4F:
        case 0x50:
        case 0x55:
        case 0x56:
            break;
        }
        break;
    case 3:
        g_draw_func_init();
        if (test_draw_func_ptr != NULL)
            test_draw_func_ptr();
        handle_test_camera();
        break;
    case 4:
        switch (sub_mode)
        {
        case 0xA9:
        case 0xAA:
            break;
        case 0xB3:
        case 0xB4:
            g_mini_commend_draw_func();
            break;
        case 0xB8:
            mini_ending_draw_func();
            break;
        case 0xC4:
            mini_nameentry_draw_func();
            break;
        default:
            g_mini_draw_func_handler();
            break;
        }
        handle_test_camera();
        break;
    case 5:
    case 7:
        if (test_draw_func_ptr != NULL)
            test_draw_func_ptr();
        handle_test_camera();
        break;
    case 6:
        author_draw_func();
        break;
    }
    camera_apply_main_projection();
}

void adv_draw_func_handler(void)
{
    switch (g_sub_mode3)
    {
    case 4:
        fn_1_591C();
        handle_test_camera();
        break;
    case 7:
    case 8:
        game_main_draw_func();
        fn_1_F4C8();
        break;
    case 14:
        game_main_draw_func();
        fn_1_59C8();
        break;
    case 11:
    case 12:
    case 15:
    case 16:
    case 17:
    case 18:
        game_main_draw_func();
        ord_tbl_draw_nodes();
        fn_1_61C8();
        break;
    case 24:
    case 25:
        g_maybe_draw_sega_splash();
        break;
    }
}

void fn_1_591C(void)
{
    mtxa_from_mtxb();
    mtxa_translate(&lbl_1_bss_1540.pos);
    mtxa_rotate_x(lbl_1_bss_1540.xrot);
    mtxa_rotate_z(lbl_1_bss_1540.zrot);
    if (lbl_1_bss_1538 != 0)
    {
        mtxa_scale_s(0.87f);
        avdisp_set_scale_factor(0.87f);
    }
    load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
    avdisp_draw_model_culled_sort_auto(init_common_gma->modelEntries[0x5B].model);
}

void fn_1_59C8(void)
{
    Vec sp48;
    int i;
    float baseScale;
    float stickX;
    float stickY;
    int stickXRot;
    int stickZRot;
    s16 fov = 0xB6;
    Mtx44 projMtx;

    if (modeCtrl.wideMode)
    {
        switch (g_related_to_fov2)
        {
        default:
            lbl_1_bss_243E = fov;
            lbl_1_bss_2440 = math_tan(fov / 2);
            break;
        case 1:
            lbl_1_bss_243E = (s16)(math_atan(0.003125f * (240.0f * math_tan(fov / 2))) * 2);
            lbl_1_bss_2440 = 0.003125f * (240.0f * math_tan(fov / 2));
            break;
        case 2:
            lbl_1_bss_243E = (s16)(math_atan(0.003125f * (277.12814f * math_tan(fov / 2))) * 2);
            lbl_1_bss_2440 = 0.003125f * (277.12814f * math_tan(fov / 2));
            break;
        }
    }
    else
    {
        lbl_1_bss_243E = fov;
        lbl_1_bss_2440 = math_tan(fov / 2);
    }

    C_MTXPerspective(projMtx, 0.005493164f * lbl_1_bss_243E, g_camera_aspect_ratio, 0.1f, 100000.0f);
    g_something_with_GXSetProjectionv(projMtx, 0);
    mtxa_from_identity();
    g_set_active_light_group(2);
    sp48.x = -0.0055f;
    if (modeCtrl.wideMode)
        sp48.x -= 0.0015f;
    sp48.y = -0.003f;
    sp48.z = -0.718f;

    if (!(g_some_other_flags & 0xA))
    {
        if (lbl_1_bss_1568.state > 0)
            lbl_1_bss_1568.t += 0.05 * -lbl_1_bss_1568.t;
        else
            lbl_1_bss_1568.t += 0.05 * (1.0 - lbl_1_bss_1568.t);
    }
    baseScale = 0.0005f;

    // Draw the analog stick base
    mtxa_from_identity();
    mtxa_translate(&sp48);
    mtxa_rotate_x(3328.0 + 32768.0 * (1.0 - lbl_1_bss_1568.t));
    mtxa_scale_xyz(baseScale, baseScale, baseScale);
    nl2ngc_set_scale(baseScale);
    GXLoadPosMtxImm(mtxa->mtxA, 0);
    GXLoadNrmMtxImm(mtxa->mtxA, 0);
    avdisp_draw_model_unculled_sort_auto(init_common_gma->modelEntries[0x63].model);

    // Draw the simulated analog stick
    mtxa_translate_xyz(0.0f, -2.7f, 0.0f);
    mtxa_push();
    mtxa_rotate_x(CLAMP(lbl_1_bss_1568.stickXRot * 12, -0x1000, 0x1000));
    mtxa_rotate_z(CLAMP(lbl_1_bss_1568.stickZRot * 8, -0x1000, 0x1000));
    GXLoadPosMtxImm(mtxa->mtxA, 0);
    GXLoadNrmMtxImm(mtxa->mtxA, 0);
    avdisp_set_alpha(lbl_1_bss_1568.t);
    avdisp_draw_model_unculled_sort_never(init_common_gma->modelEntries[0x62].model);
    mtxa_pop();

    // Draw the transparent stick based on the player's analog stick position
    stickX = 0.0f;
    stickY = 0.0f;
    for (i = 0; i < 4; i++)
    {
        if (pad_status_groups[i].unk0[0].err == 0)
        {
            stickX = (float)pad_status_groups[i].unk0[0].stickX / 60.0;
            stickY = -(float)pad_status_groups[i].unk0[0].stickY / 60.0;
            break;
        }
    }
    if (stickX < -1.0)
        stickX = -1.0f;
    else if (stickX > 1.0)
        stickX = 1.0f;
    if (stickY < -1.0)
        stickY = -1.0f;
    else if (stickY > 1.0)
        stickY = 1.0f;
    stickXRot = (4187.0f * stickY);
    stickZRot = (4187.0f * -stickX);
    mtxa_rotate_x(CLAMP(stickXRot * 2, -0x1000, 0x1000));
    mtxa_rotate_z(CLAMP(stickZRot * 2, -0x1000, 0x1000));
    mtxa_scale_s(0.99f);
    nl2ngc_set_scale(0.99f);
    GXLoadPosMtxImm(mtxa->mtxA, 0);
    GXLoadNrmMtxImm(mtxa->mtxA, 0);
    avdisp_set_alpha(lbl_1_bss_1568.t * 0.5);
    avdisp_draw_model_unculled_sort_auto(init_common_gma->modelEntries[0x62].model);

    // Draw the button base
    mtxa_from_identity();
    mtxa_translate(&sp48);
    mtxa_rotate_x(3328.0f + (-32768.0f * lbl_1_bss_1568.t));
    mtxa_translate_xyz(0.0f, 0.00058f, 0.0f);
    mtxa_scale_xyz(baseScale, baseScale, baseScale);
    nl2ngc_set_scale(baseScale);
    GXLoadPosMtxImm(mtxa->mtxA, 0);
    GXLoadNrmMtxImm(mtxa->mtxA, 0);
    avdisp_draw_model_unculled_sort_auto(init_common_gma->modelEntries[0x5D].model);

    // Draw the A button
    if (lbl_1_bss_1568.state == 2)
    {
        mtxa_scale_xyz(1.0f, 0.25f, 1.0f);
        nl2ngc_set_scale(1.0f);
    }
    GXLoadPosMtxImm(mtxa->mtxA, 0);
    GXLoadNrmMtxImm(mtxa->mtxA, 0);
    avdisp_set_alpha(1.0 - lbl_1_bss_1568.t);
    avdisp_draw_model_unculled_sort_auto(init_common_gma->modelEntries[0x5C].model);
    ord_tbl_draw_nodes();
}

struct ClearQuad
{
    float x1;
    float y1;
    float x2;
    float y2;
    float z;
};

static inline void clear_quad_depth(struct ClearQuad *a)
{
    float x1 = a->x1;
    float y1 = a->y1;
    float x2 = a->x2;
    float y2 = a->y2;
    float z = a->z;

    g_something_with_GX_vtx_desc(1 << 9);
    GXSetBlendMode_cached(1, 0, 1, 0);
    GXSetZMode_cached(1, 7, 1);
    g_something_with_raster_state(0);
    GXSetCullMode_cached(0);
    g_GXSetTevIndirect_zero_if_different(0);
    g_GXSetTevOrder_wrapper(0, 0xFF, 0xFF, 0xFF);
    GXSetTevKAlphaSel_cached(0, 0);
    GXSetTevColorIn_cached(0, 15, 15, 15, 15);
    GXSetTevColorOp_cached(0, 0, 0, 0, 1, 0);
    GXSetTevAlphaIn_cached(0, 7, 7, 7, 6);
    GXSetTevAlphaOp_cached(0, 0, 0, 3, 1, 0);
    GXSetNumTevStages_cached(1);
    mtxa_push();
    mtxa_from_identity();
    GXLoadPosMtxImm(mtxa->mtxA, 0);
    mtxa_pop();

    GXBegin(0x80, 0, 4);
    GXPosition3f32(x2, y1, z);
    GXPosition3f32(x1, y1, z);
    GXPosition3f32(x1, y2, z);
    GXPosition3f32(x2, y2, z);
    GXEnd();

    GXSetZMode_cached(1, 3, 1);
}

void fn_1_61C8(void)
{
    struct ClearQuad sp14;
    Vec sp8;
    float a;

    a = 10000.0f * g_current_camera->sub28.view.tanHalfFov;
    sp14.x1 = -a * g_current_camera->sub28.view.aspect;
    sp14.y1 = 0.0f;
    sp14.x2 = 0.0f;
    sp14.y2 = -a;
    sp14.z = -10000.0f;
    clear_quad_depth(&sp14);
    if (lbl_1_bss_1568.state == 0)
    {
        s16 r30;
        s16 v3;

        sp8.x = 0.0f;
        sp8.y = 1.0f;
        sp8.z = 0.0f;
        mtxa_from_identity();
        mtxa_rotate_y(-g_current_camera->rotY);
        mtxa_rotate_z(-world_infos[0].zrot);
        mtxa_rotate_x(-world_infos[0].xrot);
        mtxa_tf_vec(&sp8, &sp8);
        r30 = -math_atan2(sp8.z, sp8.y);
        v3 = math_atan2(sp8.x, math_sqrt(mathutil_sum_of_sq_2(sp8.z, sp8.y)));
        r30 *= 0.2;
        v3 *= 0.2;
        lbl_1_bss_1568.stickXRot = lbl_1_bss_1568.stickXRot + 0.2 * ((float)r30 - (float)lbl_1_bss_1568.stickXRot);
        lbl_1_bss_1568.stickZRot += 0.2 * ((float)v3 - (float)lbl_1_bss_1568.stickZRot);
        fn_1_59C8();
    }
}

void game_main_draw_func(void)
{
    int i;
    struct Ball *ballBackup = current_ball;

    for (i = 0; i < 5; i++)
    {
        if (cameras[i].sub28.viewportW > 0.0f && cameras[i].sub28.viewportH > 0.0f)
        {
            s8 status = g_poolInfo.cameraPool.statusList[i];

            if ((status != 0 && status != 4 && !(cameras[i].flags & 0x40)) || (cameras[i].flags & 0x80))
            {
                current_ball = &balls[i];
                camera_apply_viewport_if_changed(i);
                fn_1_D4870();
                g_maybe_some_bg_effect_handler();
                u_reset_light_group_stack(i);
                if (events[EVENT_REND_EFC].state == EV_STATE_RUNNING
                 || events[EVENT_REND_EFC].state == EV_STATE_SUSPENDED)
                    g_something_with_rend_efc(4);
                if (events[EVENT_STAGE].state == EV_STATE_RUNNING
                 || events[EVENT_STAGE].state == EV_STATE_SUSPENDED)
                    g_draw_stage();
                draw_shadow_reqs();
                if (events[EVENT_BACKGROUND].state == EV_STATE_RUNNING
                 || events[EVENT_BACKGROUND].state == EV_STATE_SUSPENDED)
                {
                    g_smth_with_rendefc_reflective_height(400.0f);
                    g_draw_bg();
                    g_smth_with_rendefc_reflective_height(0.0f);
                }
                if (events[EVENT_REND_EFC].state == EV_STATE_RUNNING
                 || events[EVENT_REND_EFC].state == EV_STATE_SUSPENDED)
                    g_something_with_rend_efc(0x10);
                if (events[EVENT_ITEM].state == EV_STATE_RUNNING
                 || events[EVENT_ITEM].state == EV_STATE_SUSPENDED)
                    draw_items();
                if (events[EVENT_STOBJ].state == EV_STATE_RUNNING
                 || events[EVENT_STOBJ].state == EV_STATE_SUSPENDED)
                    g_draw_stobjs();
                if (events[EVENT_EFFECT].state == EV_STATE_RUNNING
                 || events[EVENT_EFFECT].state == EV_STATE_SUSPENDED)
                    g_draw_effects();
                if (events[EVENT_BALL].state == EV_STATE_RUNNING
                 || events[EVENT_BALL].state == EV_STATE_SUSPENDED)
                    g_draw_ball_and_ape();
                if (world_theme.unk8 & 1)
                    fn_1_D6F9C(i);
                handle_test_camera();
                ord_tbl_draw_nodes();
                if (world_theme.unk8 & 1)
                    fn_1_D6D84(i);
                if (events[EVENT_REND_EFC].state == EV_STATE_RUNNING
                 || events[EVENT_REND_EFC].state == EV_STATE_SUSPENDED)
                    g_something_with_rend_efc(8);
            }
        }
    }
    g_draw_world_child();
    current_ball = ballBackup;
    camera_setup_default_projection();
}

void g_draw_world_child(void)
{
    int i;

    g_some_render_flag.flags |= 8;
    for (i = 0; i < 5; i++)
    {
        if (cameras[i].sub28.viewportW > 0.0f && cameras[i].sub28.viewportH > 0.0f
         && (cameras[i].flags & 0x40))
        {
            s8 stage;

            current_ball = &balls[i];
            camera_apply_viewport_if_changed(i);
            u_reset_light_group_stack(i);
            stage = events[EVENT_STAGE].state;
            if (stage == EV_STATE_RUNNING || stage == EV_STATE_SUSPENDED)
                g_draw_stage();
            if (events[EVENT_BACKGROUND].state == EV_STATE_RUNNING)
            {
                g_smth_with_rendefc_reflective_height(400.0f);
                g_draw_bg();
                g_smth_with_rendefc_reflective_height(0.0f);
            }
            if (events[EVENT_ITEM].state == EV_STATE_RUNNING)
                draw_items();
            if (events[EVENT_STOBJ].state == EV_STATE_RUNNING)
                g_draw_stobjs();
            if (events[EVENT_BALL].state == EV_STATE_RUNNING)
                g_draw_ball_and_ape();
            fn_1_6BB8();
            handle_test_camera();
            ord_tbl_draw_nodes();
        }
    }
    g_some_render_flag.flags &= ~8;
}

u16 lbl_1_data_2F3C[] = {1, 3, 5, 7};

void fn_1_6BB8(void)
{
    struct Ball *ball = balls;
    s8 *statusList = g_poolInfo.playerPool.statusList;
    int i;
    Vec sp8;
    float scale;

    for (i = 0; i < g_poolInfo.playerPool.count; i++, ball++, statusList++)
    {
        if (*statusList == 0 || *statusList == 4)
            continue;
        if (ball->flags & 0x10)
            continue;
        mtxa_from_identity();
        scale = 0.8 - 0.1 * (((g_font_char_var_3 / 16) + i) % 3);
        mtxa_scale_s(scale);
        mtxa_mult_right(mtxa->mtxB);
        mtxa_translate(&ball->pos);
        mtxa_rotate_y(cameras[i].rotY - 0x8000);
        mtxa_rotate_x(-0x4000);
        mtxa_translate_xyz(0.0f, ball->currRadius, 0.0f);
        mathutil_mtxA_get_translate(&sp8);
        if (sp8.z < -4.0 * scale)
            mtxa_scale_s(sp8.z / (-4.0 * scale));
        load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
        avdisp_draw_model_unculled_sort_auto(init_common_gma->modelEntries[lbl_1_data_2F3C[i]].model);
    }
}

void game_extra_draw_func(void)
{
    camera_apply_viewport_if_changed(modeCtrl.currPlayer);
    fn_1_D4870();
    g_maybe_some_bg_effect_handler();
    if (events[EVENT_REND_EFC].state == EV_STATE_RUNNING
     || events[EVENT_REND_EFC].state == EV_STATE_SUSPENDED)
        g_something_with_rend_efc(4);
    draw_shadow_reqs();
    if (events[EVENT_BACKGROUND].state == EV_STATE_RUNNING
     || events[EVENT_BACKGROUND].state == EV_STATE_SUSPENDED)
    {
        g_smth_with_rendefc_reflective_height(400.0f);
        g_draw_bg();
        g_smth_with_rendefc_reflective_height(0.0f);
    }
    if (events[EVENT_REND_EFC].state == EV_STATE_RUNNING
     || events[EVENT_REND_EFC].state == EV_STATE_SUSPENDED)
        g_something_with_rend_efc(0x10);
    if (events[EVENT_EFFECT].state == EV_STATE_RUNNING
     || events[EVENT_EFFECT].state == EV_STATE_SUSPENDED)
        g_draw_effects();
    if (events[EVENT_BALL].state == EV_STATE_RUNNING
     || events[EVENT_BALL].state == EV_STATE_SUSPENDED)
        g_draw_ball_and_ape();
    if (world_theme.unk8 & 1)
        fn_1_D6F9C(modeCtrl.currPlayer);
    handle_test_camera();
    ord_tbl_draw_nodes();
    if (world_theme.unk8 & 1)
        fn_1_D6D84(modeCtrl.currPlayer);
    if (events[EVENT_REND_EFC].state == EV_STATE_RUNNING
     || events[EVENT_REND_EFC].state == EV_STATE_SUSPENDED)
        g_something_with_rend_efc(8);
}

void game_result_draw_func(void)
{
    int i;
    struct Ball *ballBackup = current_ball;

    for (i = 0; i < 5; i++)
    {
        if (cameras[i].sub28.viewportW > 0.0f && cameras[i].sub28.viewportH > 0.0f)
        {
            s8 status = g_poolInfo.playerPool.statusList[i];

            if ((status != 0 && status != 4) || (cameras[i].flags & 0x40))
            {
                s8 stage;

                if (cameras[i].flags & 0x40)
                    g_some_render_flag.flags |= 8;
                current_ball = &balls[i];
                camera_apply_viewport_if_changed(i);
                fn_1_D4870();
                g_maybe_some_bg_effect_handler();
                u_reset_light_group_stack(i);
                if (events[EVENT_REND_EFC].state == EV_STATE_RUNNING)
                    g_something_with_rend_efc(4);
                stage = events[EVENT_STAGE].state;
                if (stage == EV_STATE_RUNNING || stage == EV_STATE_SUSPENDED)
                    g_draw_stage();
                draw_shadow_reqs();
                if (events[EVENT_BACKGROUND].state == EV_STATE_RUNNING)
                {
                    g_smth_with_rendefc_reflective_height(400.0f);
                    g_draw_bg();
                    g_smth_with_rendefc_reflective_height(0.0f);
                }
                if (events[EVENT_REND_EFC].state == EV_STATE_RUNNING)
                    g_something_with_rend_efc(0x10);
                if (events[EVENT_ITEM].state == EV_STATE_RUNNING)
                    draw_items();
                if (events[EVENT_STOBJ].state == EV_STATE_RUNNING)
                    g_draw_stobjs();
                if (events[EVENT_EFFECT].state == EV_STATE_RUNNING)
                    g_draw_effects();
                if (events[EVENT_BALL].state == EV_STATE_RUNNING)
                    g_draw_ball_and_ape();
                if (world_theme.unk8 & 1)
                    fn_1_D6F9C(i);
                handle_test_camera();
                ord_tbl_draw_nodes();
                if (world_theme.unk8 & 1)
                    fn_1_D6D84(i);
                if (events[EVENT_REND_EFC].state == EV_STATE_RUNNING)
                    g_something_with_rend_efc(8);
                if (cameras[i].flags & 0x40)
                    g_some_render_flag.flags &= ~8;
            }
        }
    }
    current_ball = ballBackup;
    camera_setup_default_projection();
}

/* Transforms the point (0, 0, z) by mtxA */
static inline void mathutil_mtxA_tf_point_z(register float scale, register Vec *v)
{
#ifdef __MWERKS__
    register float *mtxA;
    register float x, tx, y, ty, z, tz;

    asm
    {
        lis mtxA, LC_CACHE_BASE@ha
        lfs x, 0x08(mtxA)
        lfs tx, 0x0C(mtxA)
        lfs y, 0x18(mtxA)
        lfs ty, 0x1C(mtxA)
        lfs z, 0x28(mtxA)
        lfs tz, 0x2C(mtxA)
        fmadds x, x, scale, tx
        stfs x, v->x
        fmadds y, y, scale, ty
        stfs y, v->y
        fmadds z, z, scale, tz
        stfs z, v->z
    }
#else
    v->x = mtxa->mtxA[0][2] * scale + mtxa->mtxA[0][3];
    v->y = mtxa->mtxA[1][2] * scale + mtxa->mtxA[1][3];
    v->z = mtxa->mtxA[2][2] * scale + mtxa->mtxA[2][3];
#endif
}

void handle_test_camera(void)
{
    GXColor amb = {0xFF, 0xFF, 0xFF, 0xFF};
    GXColor mat = {0x00, 0x00, 0x00, 0xFF};
    GXColor red = {0xFF, 0x00, 0x00, 0xFF};
    GXColor green = {0x00, 0xFF, 0x00, 0xFF};
    GXColor blue = {0x00, 0x00, 0xFF, 0xFF};
    GXLightObj lightObj;
    Vec light2pos;
    Vec light3pos;
    Vec light4pos;
    Vec pos;

    if ((dip_switches & DIP_TEST_CAM) && !(dip_switches & (1 << 9)))
    {
        mtxa_from_translate(&g_current_camera->lookAt);
        mtxa_rotate_y((s16)(int)((frame_counter << 8) * 1.2f));
        mathutil_mtxA_tf_point_z(10.0f, &light2pos);

        mtxa_from_translate(&g_current_camera->lookAt);
        mtxa_rotate_x(frame_counter << 8);
        mathutil_mtxA_tf_point_z(-10.0f, &light3pos);

        mtxa_from_translate(&g_current_camera->lookAt);
        mtxa_rotate_z((s16)(int)((frame_counter << 8) * 0.8f));

        mtxa_tf_point_xyz(0.0f, 10.0f, 0.0f, &light4pos);
        mtxa_from_mtx(g_ord_tbl_stuff->matrices[0]);

        GXInitLightSpot(&lightObj, 0.0f, 0);
        GXInitLightDistAttn(&lightObj, 150.0f, 0.2f, 3);
        mtxa_tf_point(&light2pos, &pos);
        GXInitLightPos(&lightObj, pos.x, pos.y, pos.z);
        GXInitLightColor(&lightObj, green);
        GXLoadLightObjImm(&lightObj, 4);

        GXInitLightSpot(&lightObj, 0.0f, 0);
        GXInitLightDistAttn(&lightObj, 150.0f, 0.2f, 3);
        mtxa_tf_point(&light3pos, &pos);
        GXInitLightPos(&lightObj, pos.x, pos.y, pos.z);
        GXInitLightColor(&lightObj, blue);
        GXLoadLightObjImm(&lightObj, 8);

        GXInitLightSpot(&lightObj, 0.0f, 0);
        GXInitLightDistAttn(&lightObj, 150.0f, 0.2f, 3);
        mtxa_tf_point(&light4pos, &pos);
        GXInitLightPos(&lightObj, pos.x, pos.y, pos.z);
        GXInitLightColor(&lightObj, red);
        GXLoadLightObjImm(&lightObj, 0x10);

        GXSetChanMatColor(0, amb);
        GXSetChanAmbColor(0, mat);
        opti_GXSetChanCtrl(4, 1, 0, 0, 0x1C, 2, 2);
        GXSetNumChans_cached(1);
        g_GXSetTevOrder_wrapper(0, 0xFF, 0xFF, 4);
        g_GXSetTevColorIn_GXSetTevAlphaIn_wrapper(0, 4);
        GXSetNumTexGens_cached(0);
        GXSetNumTevStages_cached(1);

        mtxa_from_mtx(g_ord_tbl_stuff->matrices[0]);
        mtxa_translate(&g_current_camera->lookAt);
        mtxa_scale_xyz(0.05f, 0.05f, 0.05f);
        GXLoadNrmMtxImm(mtxa->mtxA, 0);
        GXLoadPosMtxImm(mtxa->mtxA, 0);
        GXDrawSphere(8, 8);
    }
}

void g_set_clear_color(void)
{
    BOOL flag = TRUE;
    GXColor color;

    switch (main_mode)
    {
    case 2:
        switch (sub_mode)
        {
        case 0x4F:
        case 0x50:
        case 0x55:
        case 0x56:
        case 0x61:
        case 0x62:
            if ((modeCtrl.courseFlags & 0x60) == 0)
            {
                color.r = 0;
                color.g = 0;
                color.b = 0;
            }
            else
            {
                color.r = 255;
                color.g = 255;
                color.b = 255;
            }
            flag = FALSE;
            break;
        default:
            color = world_theme.backdropColor;
            break;
        }
        break;
    case 1:
    case 4:
    case 6:
    case 7:
        color = world_theme.backdropColor;
        break;
    case 0:
        switch (sub_mode)
        {
        case 3:
        case 4:
        case 5:
        case 6:
        case 9:
        case 10:
        case 0x16:
        case 0x17:
        case 0x18:
        case 0x19:
            color.r = (u8)((lbl_1_bss_153C >> 16) & 0xFF);
            color.g = (u8)((lbl_1_bss_153C >> 8) & 0xFF);
            color.b = (u8)(lbl_1_bss_153C & 0xFF);
            break;
        default:
            color = world_theme.backdropColor;
            lbl_1_bss_153C = 0xFF000000 | (color.r << 16) | (color.g << 8) | color.b;
            break;
        }
        break;
    case 5:
        switch (sub_mode)
        {
        case 0xE5:
        case 0xE6:
            color = world_theme.backdropColor;
            break;
        default:
            color.r = color.g = color.b = 0;
            break;
        }
        break;
    case 3:
        switch (sub_mode)
        {
        case 0xA0:
        case 0xE2:
            color = world_theme.backdropColor;
            break;
        default:
            color.r = 64;
            color.g = 64;
            color.b = 64;
            break;
        }
        break;
    default:
        color.r = 64;
        color.g = 64;
        color.b = 64;
        break;
    }
    color.a = 255;
    if (flag && lbl_1_bss_CF22C.enabled != 0)
    {
        color.r = lbl_1_bss_CF22C.r;
        color.g = lbl_1_bss_CF22C.g;
        color.b = lbl_1_bss_CF22C.b;
    }
    GXSetCopyClear_cached(color, 0xFFFFFF);
    if (g_some_render_flag.flags & 0x40)
    {
        if (g_some_render_flag.bgColor.r != color.r || g_some_render_flag.bgColor.g != color.g
         || g_some_render_flag.bgColor.b != color.b || g_some_render_flag.bgColor.a != color.a)
            g_smth_with_bg_color_drawing(&color);
        g_some_render_flag.flags &= ~0x40;
    }
    g_some_render_flag.bgColor = color;
}

void g_smth_with_bg_color_drawing(GXColor *color)
{
    GXColor c = *color;
    float proj[7];
    float x1;
    float y1;
    float x2;
    float z;
    float y2;
    float near;
    float t;
    float far;

    g_something_with_GX_vtx_desc(1 << 9);
    GXSetCullMode_cached(0);
    g_something_with_GX_vtx_desc(1 << 9);
    g_GXSetTevIndirect_zero_if_different(0);
    GXSetTevKAlphaSel_cached(0, 0);
    GXSetTevKColorSel_cached(0, 12);
    GXSetTevKColor_cached(0, c);
    g_GXSetTevOrder_wrapper(0, 0xFF, 0xFF, 0xFF);
    GXSetTevColorIn_cached(0, 15, 15, 15, 14);
    GXSetTevColorOp_cached(0, 0, 0, 0, 1, 0);
    GXSetTevAlphaIn_cached(0, 7, 7, 7, 6);
    GXSetTevAlphaOp_cached(0, 0, 0, 0, 1, 0);
    GXSetNumTevStages_cached(1);
    GXSetNumChans_cached(0);
    GXSetNumTexGens_cached(1);
    GXSetNumIndStages_cached(0);
    GXSetBlendMode_cached(0, 1, 0, 0);
    GXSetZMode_cached(1, 7, 1);
    g_something_with_raster_state(0);
    GXGetProjectionv(proj);
    near = proj[6] / proj[5];
    switch ((u32)proj[0])
    {
    case 0:
        far = proj[6] / (proj[5] - 1.0f);
        z = -(near - (near - far) / 16777216.0f);
        t = z / proj[1];
        x1 = (1.0f - proj[2]) * t;
        x2 = (-1.0f - proj[2]) * t;
        t = z / proj[3];
        y1 = (1.0f - proj[4]) * t;
        y2 = (-1.0f - proj[4]) * t;
        break;
    default:
        far = -(1.0f + proj[6]) / proj[5];
        z = -(near - (near - far) / 16777216.0f);
        t = 1.0f / proj[1];
        x1 = (1.0f - proj[2]) * t;
        x2 = (-1.0f - proj[2]) * t;
        t = 1.0f / proj[3];
        y1 = (1.0f - proj[4]) * t;
        y2 = (-1.0f - proj[4]) * t;
        break;
    }
    mtxa_push();
    mtxa_from_identity();
    GXLoadPosMtxImm(mtxa->mtxA, 0);
    mtxa_pop();
    GXBegin(0x80, 0, 4);
    GXPosition3f32(x2, y1, z);
    GXPosition3f32(x1, y1, z);
    GXPosition3f32(x1, y2, z);
    GXPosition3f32(x2, y2, z);
    GXEnd();
}

struct MirrorRect
{
    float left;
    float top;
    float right;
    float bottom;
};

struct Unk81E4
{
    /*0x00*/ float z;
    /*0x04*/ float left;
    /*0x08*/ float top;
    /*0x0C*/ float right;
    /*0x10*/ float bottom;
    /*0x14*/ s32 mode;
    /*0x18*/ GXColor color;
    /*0x1C*/ s32 hasCorners;
    /*0x20*/ Vec corners[4];
};

struct Unk8A70
{
    /*0x000*/ u32 flags;
    /*0x004*/ s32 copyFmt;
    /*0x008*/ s32 texFmt;
    /*0x00C*/ GXColor color;
    u8 filler10[4];
    /*0x014*/ Vec scale;
    /*0x020*/ float unk20;
    /*0x024*/ float unk24;
    /*0x028*/ s32 unk28;
    /*0x02C*/ float unk2C;
    /*0x030*/ float unk30;
    /*0x034*/ s32 unk34;
    /*0x038*/ float unk38;
    /*0x03C*/ float unk3C;
    /*0x040*/ s32 unk40;
    /*0x044*/ s32 count;
    /*0x048*/ struct GMAModel *models[32];
    /*0x0C8*/ Mtx mtxs[32];
    /*0x6C8*/ Vec unk6C8;
};

struct Unk8A70Node
{
    struct OrdTblNode node;
    /*0x08*/ int lightGroup;
    /*0x0C*/ Mtx mtx;
    /*0x3C*/ struct Unk8A70 arg;
};

struct Unk993CLayer
{
    /*0x00*/ u32 flags;
    u8 filler4[4];
    /*0x08*/ GXTexObj * DPTR tex;  /* GMA model TEV layer (disc layout, 0x20 stride) */
    u8 fillerC[0x20 - 0xC];
};

struct Unk993CShape
{
    u8 filler0[0x12];
    /*0x12*/ u8 layerCount;
    u8 filler13[3];
    /*0x16*/ u16 layers[1];
};

struct Unk993C
{
    u8 filler0[4];
    /*0x04*/ struct Unk993CShape *shape;
    /*0x08*/ struct Unk993CLayer *layers;
};

void fn_1_CCBFC(float);
void fn_1_D27E4(u32);
void draw_ball_shadow(void);
void g_draw_goal_and_wormhole_shadows(void);
void g_draw_bg_model_shadows(void);
void fn_1_D462C(void);
void init_shadow_reqs(void);
void fade_color_base_default(void);
void fn_1_86B8(GXTexObj *tex, struct Unk81E4 *arg);
void fn_1_8914(struct Unk81E4 *arg);
void fn_1_8B04(struct Unk8A70Node *node);
void fn_1_8B44(struct Unk8A70 *arg);
int fn_1_8CCC(struct Unk8A70 *arg);
int fn_1_911C(struct Unk8A70 *arg, struct GMAModel *model, Mtx mtx);
int fn_1_993C(struct Unk993C *arg);
int fn_1_9A10(struct GMAModel **models, Mtx *mtxs, int count, float scale, struct MirrorRect *rect);
void g_smth_with_gpu_and_locked_cache_mtx(void);
void GXSetTexCoordGen2_cached(int dst, int func, int src, int mtx, int normalize, int postMtx);
void GXLoadTexObj_cached(GXTexObj *obj, int map);
void GXSetLineWidth_cached(u8 width, int texOffset);
void GXSetTevSwapMode_cached(int stage, int rasSel, int texSel);
void GXSetIndTexOrder_cached(int stage, int coord, int map);
void GXSetTevIndirect_cached(int stage, int indStage, int format, int biasSel, int mtxSel, int wrapS, int wrapT, int addPrev, int utcLod, int alphaSel);
void g_GXSetIndTexMtx_wrapper(int id, float mtx[2][3], int scale);
void GXLoadTexMtxImm(Mtx m, int id, int type);
void GXSetTevColor(int id, GXColor color);
void GXInitTexObj(GXTexObj *obj, void *image, u16 width, u16 height, int format, int wrapS, int wrapT, u8 mipmap);
int GXGetTexObjFmt(GXTexObj *obj);
u16 GXGetTexObjHeight(GXTexObj *obj);
u16 GXGetTexObjWidth(GXTexObj *obj);
u8 GXGetTexObjMipMap(GXTexObj *obj);
void *GXGetTexObjData(GXTexObj *obj);
void GXGetViewportv(float *vp);
void GXInvalidateTexAll(void);
void C_MTXOrtho(Mtx44 m, float t, float b, float l, float r, float n, float f);
void g_something_with_fog(int type, float start, float end);
void *fn_80069AC0(void *func);
void g_math_unk13(Vec *eye, Vec *up, Vec *target);
void fn_1_E2F1C(void);
void memcpy2(void *dest, void *src, u32 size);
struct OrdTblNode *ord_tbl_get_entry_for_pos(Vec *pos);
void *ord_tbl_alloc_node(u32 size);
void fn_1_CD418(struct OrdTblNode *list, struct OrdTblNode *node);
void draw_item_shadows(void);

void g_draw_func_init(void)
{
    fn_1_CCBFC(0.5f);
    fn_1_D27E4(0xFFFF7F);
    fn_1_CCBFC(-0.5f);
}

void draw_stage_shadows(void)
{
    if (events[EVENT_BALL].state == EV_STATE_RUNNING || events[EVENT_BALL].state == EV_STATE_SUSPENDED)
        draw_ball_shadow();
    if (events[EVENT_ITEM].state == EV_STATE_RUNNING || events[EVENT_ITEM].state == EV_STATE_SUSPENDED)
        draw_item_shadows();
    if (events[EVENT_STAGE].state == EV_STATE_RUNNING || events[EVENT_STAGE].state == EV_STATE_SUSPENDED)
        g_draw_goal_and_wormhole_shadows();
    if (events[EVENT_BACKGROUND].state == EV_STATE_RUNNING || events[EVENT_BACKGROUND].state == EV_STATE_SUSPENDED)
        g_draw_bg_model_shadows();
}

void g_reset_shadow_state(void)
{
    fn_1_D462C();
    init_shadow_reqs();
}

void fade_color_base_default(void)
{
    nl2ngc_set_material_color(lbl_1_bss_1470.r, lbl_1_bss_1470.g, lbl_1_bss_1470.b);
    set_post_mult_color(lbl_1_bss_1470.r, lbl_1_bss_1470.g, lbl_1_bss_1470.b, 1.0f);
}

void fn_1_7FF4(float r, float g, float b)
{
    lbl_1_bss_1470.unk0 = 0.0f;
    lbl_1_bss_1470.r = r;
    lbl_1_bss_1470.g = g;
    lbl_1_bss_1470.b = b;
}

float fn_1_8034(Vec *pos)
{
    Vec sp8;

    mtxa_tf_point(pos, &sp8);
    sp8.x -= g_some_render_flag.unk20.x;
    sp8.y -= g_some_render_flag.unk20.y;
    sp8.z -= g_some_render_flag.unk20.z;
    return mathutil_vec_dot_prod(&sp8, &g_some_render_flag.unk2C);
}

float fn_1_80D4(Vec *pos)
{
    Vec sp8;

    sp8.x = pos->x - g_some_render_flag.unk4.x;
    sp8.y = pos->y - g_some_render_flag.unk4.y;
    sp8.z = pos->z - g_some_render_flag.unk4.z;
    return mathutil_vec_dot_prod(&sp8, &g_some_render_flag.unk10);
}

float fn_1_815C(Vec *pos)
{
    Vec sp8;

    sp8.x = pos->x - g_some_render_flag.unk3C.x;
    sp8.y = pos->y - g_some_render_flag.unk3C.y;
    sp8.z = pos->z - g_some_render_flag.unk3C.z;
    return mathutil_vec_dot_prod(&sp8, &g_some_render_flag.unk48);
}

void fn_1_81E4(GXTexObj *tex, struct Unk81E4 *arg)
{
    float proj[7];
    float a, b;
    float y2;
    float cz;
    float bx;
    float by;
    float az;
    float ax;
    float cx;
    float x2;
    float ay;
    float y1;
    float x1;
    float dy;
    float dz;

    fn_1_86B8(tex, arg);
    mtxa_push();
    mtxa_from_identity();
    g_smth_with_gpu_and_locked_cache_mtx();
    mtxa_pop();
    GXGetProjectionv(proj);
    a = -1.0f / proj[1];
    b = 1.0f / proj[3];
    x1 = a * (arg->left * arg->z);
    x2 = a * (arg->right * arg->z);
    y1 = b * (arg->top * arg->z);
    y2 = b * (arg->bottom * arg->z);

    GXBegin(0x80, 0, 4);
    GXPosition3f32(x1, y1, arg->z);
    GXTexCoord2f32(0.0f, 0.0f);
    GXPosition3f32(x2, y1, arg->z);
    GXTexCoord2f32(1.0f, 0.0f);
    GXPosition3f32(x2, y2, arg->z);
    GXTexCoord2f32(1.0f, 1.0f);
    GXPosition3f32(x1, y2, arg->z);
    GXTexCoord2f32(0.0f, 1.0f);

    fn_1_8914(arg);

    ax = x1;
    ay = y1;
    az = arg->z;
    bx = x2;
    by = y1;
    cx = x2;
    cz = az;
    dy = y2;
    dz = az;

    GXBegin(0xB0, 0, 5);
    GXPosition3f32(ax, ay, az);
    GXPosition3f32(bx, by, az);
    GXPosition3f32(cx, y2, cz);
    GXPosition3f32(x1, dy, dz);
    GXPosition3f32(ax, ay, az);

    if (arg->hasCorners != 0)
    {
        GXBegin(0xB0, 0, 5);
        GXPosition3f32(arg->corners[0].x, arg->corners[0].y, arg->corners[0].z);
        GXPosition3f32(arg->corners[1].x, arg->corners[1].y, arg->corners[1].z);
        GXPosition3f32(arg->corners[2].x, arg->corners[2].y, arg->corners[2].z);
        GXPosition3f32(arg->corners[3].x, arg->corners[3].y, arg->corners[3].z);
        GXPosition3f32(arg->corners[0].x, arg->corners[0].y, arg->corners[0].z);

        GXBegin(0xA8, 0, 8);
        GXPosition3f32(arg->corners[0].x, arg->corners[0].y, arg->corners[0].z);
        GXPosition3f32(ax, ay, az);
        GXPosition3f32(arg->corners[1].x, arg->corners[1].y, arg->corners[1].z);
        GXPosition3f32(bx, by, az);
        GXPosition3f32(arg->corners[2].x, arg->corners[2].y, arg->corners[2].z);
        GXPosition3f32(cx, y2, cz);
        GXPosition3f32(arg->corners[3].x, arg->corners[3].y, arg->corners[3].z);
        GXPosition3f32(x1, dy, dz);
    }
}

void fn_1_86B8(GXTexObj *tex, struct Unk81E4 *arg)
{
    GXColor color;

    GXLoadTexObj_cached(tex, 0);
    GXSetBlendMode_cached(0, 1, 0, 0);
    GXSetZMode_cached(1, 3, 1);
    g_something_with_raster_state(0);
    GXSetCullMode_cached(0);
    switch (arg->mode)
    {
    case 0:
        g_GXSetTevIndirect_zero_if_different(0);
        GXSetTexCoordGen2_cached(0, 1, 4, 0x3C, 0, 0x7D);
        g_GXSetTevOrder_wrapper(0, 0, 0, 0xFF);
        GXSetTevKAlphaSel_cached(0, 0);
        GXSetTevColorIn_cached(0, 0xF, 8, 0xC, 0xF);
        GXSetTevColorOp_cached(0, 0, 0, 0, 1, 0);
        GXSetTevAlphaIn_cached(0, 7, 7, 7, 6);
        GXSetTevAlphaOp_cached(0, 0, 0, 0, 1, 0);
        GXSetNumTevStages_cached(1);
        GXSetNumTexGens_cached(1);
        GXSetNumIndStages_cached(0);
        GXSetNumChans_cached(0);
        break;
    case 1:
        g_GXSetTevIndirect_zero_if_different(0);
        GXSetTexCoordGen2_cached(0, 1, 4, 0x3C, 0, 0x7D);
        g_GXSetTevOrder_wrapper(0, 0, 0, 0xFF);
        color.r = 0xFF;
        color.g = 0;
        color.b = 0;
        GXSetTevKColor_cached(0, color);
        GXSetTevKColorSel_cached(0, 0xC);
        GXSetTevKAlphaSel_cached(0, 0);
        GXSetTevColorIn_cached(0, 0xE, 0xF, 9, 0xF);
        GXSetTevColorOp_cached(0, 0, 0, 0, 1, 0);
        GXSetTevAlphaIn_cached(0, 7, 7, 7, 6);
        GXSetTevAlphaOp_cached(0, 0, 0, 0, 1, 0);
        GXSetNumTevStages_cached(1);
        GXSetNumTexGens_cached(1);
        GXSetNumIndStages_cached(0);
        GXSetNumChans_cached(0);
        break;
    }
    g_something_with_GX_vtx_desc(0x2200);
}

void fn_1_8914(struct Unk81E4 *arg)
{
    GXSetBlendMode_cached(0, 1, 0, 0);
    GXSetZMode_cached(1, 3, 1);
    g_something_with_raster_state(0);
    GXSetLineWidth_cached(6, 0);
    GXSetCullMode_cached(0);
    g_GXSetTevIndirect_zero_if_different(0);
    GXSetTexCoordGen2_cached(0, 1, 4, 0x3C, 0, 0x7D);
    g_GXSetTevOrder_wrapper(0, 0, 0, 0xFF);
    GXSetTevKColor_cached(0, arg->color);
    GXSetTevKColorSel_cached(0, 0xC);
    GXSetTevKAlphaSel_cached(0, 0x1C);
    GXSetTevColorIn_cached(0, 0xF, 0xF, 0xF, 0xE);
    GXSetTevColorOp_cached(0, 0, 0, 0, 1, 0);
    GXSetTevAlphaIn_cached(0, 7, 7, 7, 6);
    GXSetTevAlphaOp_cached(0, 0, 0, 0, 1, 0);
    GXSetNumTevStages_cached(1);
    GXSetNumTexGens_cached(1);
    GXSetNumIndStages_cached(0);
    GXSetNumChans_cached(0);
    g_something_with_GX_vtx_desc(0x200);
}

void fn_1_8A70(struct Unk8A70 *arg)
{
    struct OrdTblNode *entry;
    struct Unk8A70Node *node;

    entry = ord_tbl_get_entry_for_pos(&arg->models[0]->boundSphereCenter);
    node = ord_tbl_alloc_node(sizeof(*node));
    if (node != NULL)
    {
        node->node.drawFunc = (void *)fn_1_8B04;
        node->lightGroup = get_active_light_group();
        mtxa_to_mtx(node->mtx);
        memcpy2(&node->arg, arg, sizeof(*arg));
        fn_1_CD418(entry, &node->node);
    }
}

void fn_1_8B04(struct Unk8A70Node *node)
{
    load_light_group_cached(node->lightGroup);
    mtxa_from_mtx(node->mtx);
    fn_1_8B44(&node->arg);
}

void fn_1_8B44(struct Unk8A70 *arg)
{
    GXTexObj *tex;
    void *old;
    int i;
    struct GMAModel **models;
    struct GMAModel *model;
    Mtx *mtxs;

    tex = init_common_gma->modelEntries[0x61].model->texObjs;
    GXInitTexObj(&lbl_1_bss_1480.unk20,
        GXGetTexObjData(tex),
        GXGetTexObjWidth(tex),
        GXGetTexObjHeight(tex),
        GXGetTexObjFmt(tex),
        2, 2,
        GXGetTexObjMipMap(tex));
    lbl_1_bss_1480.unk40 = init_common_gma->modelEntries[0x5F].model->texObjs;
    if (!(arg->flags & 2))
        arg->unk6C8 = lbl_1_bss_D4244;
    if (fn_1_8CCC(arg))
    {
        old = fn_80069AC0(fn_1_993C);
        load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
        models = arg->models;
        mtxs = arg->mtxs;
        for (i = arg->count; i > 0; i--, models++, mtxs++)
        {
            model = *models;
            load_gx_pos_nrm_mtx(*mtxs, 0);
            fn_1_911C(arg, model, *mtxs);
            avdisp_draw_model_unculled_sort_never(model);
        }
        fn_80069AC0(old);
    }
}

int fn_1_8CCC(struct Unk8A70 *arg)
{
    Mtx mtx;
    float proj[7];
    float vp[6];
    struct MirrorRect rect;
    struct Unk1480 *p;
    int dstW, dstH;
    int x, y, w, h;
    void *dest;
    int half;
    int t;
    float offX, offY;

    p = &lbl_1_bss_1480;
    if (fn_1_9A10(arg->models, arg->mtxs, arg->count, 1.0f, &rect) == 0)
        return 0;
    rect.left -= arg->scale.x;
    rect.top -= arg->scale.y;
    rect.right += arg->scale.x;
    rect.bottom += arg->scale.y;
    if (rect.left < -1.0f)
        rect.left = -1.0f;
    if (rect.top < -1.0f)
        rect.top = -1.0f;
    if (rect.right > 1.0f)
        rect.right = 1.0f;
    if (rect.bottom > 1.0f)
        rect.bottom = 1.0f;
    GXGetProjectionv(proj);
    half = arg->flags & 1;
    dest = lbl_801988E0;
    GXGetViewportv(vp);
    x = vp[0] + vp[2] * (0.5f * (1.0f + rect.left));
    y = vp[1] + vp[3] * (0.5f * (1.0f + rect.top));
    w = 0.5f * (vp[2] * (rect.right - rect.left));
    h = 0.5f * (vp[3] * (rect.bottom - rect.top));
    t = w + (x & 1);
    w = (t + 1) & ~1;
    t = h + (y & 1);
    h = (t + 1) & ~1;
    x &= ~1;
    y &= ~1;
    if (half == 0)
    {
        dstW = w;
        dstH = h;
    }
    else
    {
        dstW = w >> 1;
        dstH = h >> 1;
    }
    lbl_1_bss_1524.x = vp[2] / (float)w;
    lbl_1_bss_1524.y = vp[3] / (float)h;
    offX = ((float)(x + (w >> 1)) - vp[0]) / vp[2] - 0.5f;
    offY = ((float)(y + (h >> 1)) - vp[1]) / vp[3] - 0.5f;
    mtxa_push();
    mtxa_from_identity();
    mtxa->mtxA[0][2] = 0.5f;
    mtxa->mtxA[1][2] = 0.5f;
    mtxa_scale_xyz(lbl_1_bss_1524.x, lbl_1_bss_1524.y, 1.0f);
    mtxa_to_mtx(mtx);
    mtxa_from_identity();
    mtxa->mtxA[0][2] = -0.5f;
    mtxa->mtxA[1][2] = -0.5f;
    mtxa_mult_left(mtx);
    mtxa_to_mtx(mtx);
    mtxa_from_identity();
    mtxa->mtxA[0][2] = -offX;
    mtxa->mtxA[1][2] = -offY;
    mtxa_mult_left(mtx);
    mtxa_to_mtx(p->unk74);
    mtxa_pop();
    GXSetTexCopySrc_cached(x, y, w, h);
    GXSetTexCopyDst_cached(dstW, dstH, arg->copyFmt, half);
    GXCopyTex(dest, 0);
    GXInitTexObj(&p->unk0, dest, dstW, dstH, arg->texFmt, 0, 0, 0);
    GXInvalidateTexAll();
    return 1;
}

int fn_1_911C(struct Unk8A70 *arg, struct GMAModel *model, Mtx mtx)
{
    Mtx sp5C;
    float indMtx[2][3];
    float invTan;
    struct Unk1480 *p;
    int stage, coord, ind;
    GXColor color;

    p = &lbl_1_bss_1480;
    mtxa_from_mtx(mtx);
    GXSetNumChans_cached(0);
    GXLoadTexObj_cached(&p->unk0, 0);
    GXLoadTexObj_cached(&p->unk20, 1);

    mtxa_push();
    mathutil_mtxA_set_translate_xyz(0.0f, 0.0f, 0.0f);
    GXLoadTexMtxImm(mtxa->mtxA, 0x1E, 0);
    mtxa_from_translate_xyz(0.5f, 0.5f, 1.0f);
    mtxa_scale(&arg->scale);
    GXLoadTexMtxImm(mtxa->mtxA, 0x40, 0);
    mtxa_pop();

    mtxa_push();
    invTan = 1.0f / g_current_camera->sub28.view.tanHalfFov;
    mtxa_to_mtx(sp5C);
    mtxa_from_identity();
    mtxa->mtxA[0][0] = -0.5f * invTan * (1.0f / g_current_camera->sub28.view.aspect);
    mtxa->mtxA[0][2] = 0.5f - 0.5f * g_current_camera->sub28.view.unk28;
    mtxa->mtxA[1][1] = 0.5f * invTan;
    mtxa->mtxA[1][2] = 0.5f - 0.5f * g_current_camera->sub28.view.unk2C;
    mtxa_mult_right(sp5C);
    mtxa_mult_left(p->unk74);
    GXLoadTexMtxImm(mtxa->mtxA, 0x21, 0);
    mtxa_pop();

    mtxa_push();
    {
    Vec eye = {0.0f, 0.0f, 0.0f};
    Vec up = {0.0f, 1.0f, 0.0f};
    Vec target = arg->unk6C8;
    Vec dir;

    mtxa_tf_point(&model->boundSphereCenter, &dir);
    vec_normalize_len(&dir);
    dir.x *= -0.9f;
    dir.y *= -0.9f;
    dir.z *= -0.9f;
    target.x = 0.5f * (dir.x + target.x);
    target.y = 0.5f * (dir.y + target.y);
    target.z = 0.5f * (dir.z + target.z);
    g_math_unk13(&eye, &up, &target);
    }
    mtxa->mtxA[0][3] = 0.5f;
    mtxa->mtxA[1][0] *= -1.0f;
    mtxa->mtxA[1][1] *= -1.0f;
    mtxa->mtxA[1][2] *= -1.0f;
    mtxa->mtxA[1][3] = 0.5f;
    mtxa->mtxA[2][0] = 0.0f;
    mtxa->mtxA[2][1] = 0.0f;
    mtxa->mtxA[2][2] = 0.0f;
    mtxa->mtxA[2][3] = 1.0f;
    mtxa_scale_s(0.5f);
    GXLoadTexMtxImm(mtxa->mtxA, 0x43, 0);
    mtxa_pop();

    GXSetTevKColor_cached(0, arg->color);

    stage = 0;
    coord = 0;
    ind = 0;

    GXSetTexCoordGen2_cached(coord + 1, 0, 0, 0x21, 0, 0x7D);
    g_GXSetTevOrder_wrapper(stage, coord + 1, 0, 0xFF);
    GXSetTexCoordGen2_cached(coord, 1, 4, 0x3C, 0, 0x7D);
    GXSetIndTexOrder_cached(ind, coord, 3);
    GXSetTevIndirect_cached(stage, ind, 0, 7, 1, 6, 6, 0, 0, 0);
    indMtx[0][0] = 0.0f;
    indMtx[0][1] = 0.0f;
    indMtx[0][2] = arg->unk20 / lbl_1_bss_1524.x;
    indMtx[1][0] = 0.0f;
    indMtx[1][1] = arg->unk24 / lbl_1_bss_1524.y;
    indMtx[1][2] = 0.0f;
    g_GXSetIndTexMtx_wrapper(1, indMtx, (s8)arg->unk28);
    stage++;
    coord++;
    ind++;

    g_GXSetTevOrder_wrapper(stage, coord, 0, 0xFF);
    GXSetTevSwapMode_cached(stage, 0, 0);
    GXSetTevKColorSel_cached(stage, 0xC);
    GXSetTevKAlphaSel_cached(stage, 0x1C);
    if (arg->flags & 4)
        GXSetTevColorIn_cached(stage, 8, 0xF, 0xF, 0xE);
    else
        GXSetTevColorIn_cached(stage, 0xF, 8, 0xE, 0xF);
    GXSetTevColorOp_cached(stage, 0, 0, 0, 1, 0);
    GXSetTevAlphaIn_cached(stage, 7, 7, 7, 6);
    GXSetTevAlphaOp_cached(stage, 0, 0, 0, 1, 0);
    GXSetTexCoordGen2_cached(coord + 1, 0, 1, 0x1E, 1, 0x40);
    GXSetIndTexOrder_cached(ind, coord + 1, 1);
    GXSetTevIndirect_cached(stage, ind, 0, 7, 2, 0, 0, 1, 0, 0);
    indMtx[0][0] = 0.0f;
    indMtx[0][1] = 0.0f;
    indMtx[0][2] = arg->unk2C / lbl_1_bss_1524.x;
    indMtx[1][0] = 0.0f;
    indMtx[1][1] = arg->unk30 / lbl_1_bss_1524.y;
    indMtx[1][2] = 0.0f;
    g_GXSetIndTexMtx_wrapper(2, indMtx, (s8)arg->unk34);
    stage++;
    coord += 2;
    ind++;

    GXSetTexCoordGen2_cached(coord, 0, 1, 0x1E, 1, 0x43);
    g_GXSetTevOrder_wrapper(stage, coord, 2, 0xFF);
    GXSetTevSwapMode_cached(stage, 0, 1);
    GXSetTevColorIn_cached(stage, 0, 0xF, 0xF, 8);
    GXSetTevColorOp_cached(stage, 0, 0, 0, 1, 0);
    GXSetTevAlphaIn_cached(stage, 4, 7, 7, 0);
    GXSetTevAlphaOp_cached(stage, 0, 0, 0, 1, 0);
    GXSetTexCoordGen2_cached(coord + 1, 1, 4, 0x3C, 0, 0x7D);
    GXSetIndTexOrder_cached(ind, coord + 1, 3);
    GXSetTevIndirect_cached(stage, ind, 0, 7, 3, 0, 0, 0, 0, 0);
    indMtx[0][0] = 0.0f;
    indMtx[0][1] = 0.0f;
    indMtx[0][2] = arg->unk38;
    indMtx[1][0] = 0.0f;
    indMtx[1][1] = arg->unk3C;
    indMtx[1][2] = 0.0f;
    g_GXSetIndTexMtx_wrapper(3, indMtx, (s8)arg->unk40);
    stage++;
    coord += 2;
    ind++;

    GXSetNumTevStages_cached(stage);
    GXSetNumTexGens_cached(coord);
    GXSetNumIndStages_cached(ind);
    GXSetBlendMode_cached(1, 4, 5, 0);
    GXSetZMode_cached(1, 3, 1);
    fn_1_E2F1C();
    return 1;
}

int fn_1_993C(struct Unk993C *arg)
{
    struct Unk993CShape *shape;
    struct Unk993CLayer *layer;
    int i;
    u16 *ids;

    shape = arg->shape;
    if (shape->layerCount != 0)
    {
        layer = &arg->layers[shape->layers[0]];
        GXLoadTexObj_cached(layer->tex, 3);
    }
    ids = &shape->layers[1];
    for (i = shape->layerCount - 1; i > 0; i--, ids++)
    {
        layer = &arg->layers[*ids];
        if ((layer->flags & 0x8001) != 0)
            break;
    }
    if (i <= 0)
        GXLoadTexObj_cached(lbl_1_bss_1480.unk40, 2);
    else
        GXLoadTexObj_cached(layer->tex, 2);
    return 1;
}

int fn_1_9A10(struct GMAModel **models, Mtx *mtxs, int count, float scale, struct MirrorRect *rect)
{
    float proj[7];
    Vec pos;
    struct MirrorRect r;
    int i;
    struct GMAModel *model;
    float radius, extent, inv, center;

    GXGetProjectionv(proj);
    r.left = 1.0f;
    r.top = 1.0f;
    r.right = -1.0f;
    r.bottom = -1.0f;
    for (i = count; i > 0; i--, models++, mtxs++)
    {
        model = *models;
        mtxa_from_mtx(*mtxs);
        if (g_is_sphere_visible(&model->boundSphereCenter, model->boundSphereRadius * scale))
        {
            mtxa_tf_point(&model->boundSphereCenter, &pos);
            if (pos.z >= 0.0f)
            {
                r.left = -1.0f;
                r.top = -1.0f;
                r.right = 1.0f;
                r.bottom = 1.0f;
                break;
            }
            radius = model->boundSphereRadius * scale;
            inv = -1.0f / pos.z;
            extent = inv * (radius * proj[1]);
            center = proj[2] + inv * (pos.x * proj[1]);
            if (r.left > center - extent)
                r.left = center - extent;
            if (r.right < center + extent)
                r.right = center + extent;
            extent = inv * (radius * proj[3]);
            center = -(proj[4] + inv * (pos.y * proj[3]));
            if (r.top > center - extent)
                r.top = center - extent;
            if (r.bottom < center + extent)
                r.bottom = center + extent;
        }
    }
    if (r.left < -1.0f)
        r.left = -1.0f;
    if (r.right > 1.0f)
        r.right = 1.0f;
    if (r.top < -1.0f)
        r.top = -1.0f;
    if (r.bottom > 1.0f)
        r.bottom = 1.0f;
    if (r.left >= r.right || r.top >= r.bottom)
        return 0;
    *rect = r;
    return 1;
}

void take_pausemenu_screenshot(void *dest, int left, int top, int width, int height, int format)
{
    g_pausemenu_screenshot_requested.dest = dest;
    g_pausemenu_screenshot_requested.left = (u16)((u16)left & ~1);
    g_pausemenu_screenshot_requested.top = (u16)((u16)top & ~1);
    g_pausemenu_screenshot_requested.width = (u16)(((u16)width + 1) & ~1);
    g_pausemenu_screenshot_requested.height = (u16)(((u16)height + 1) & ~1);
    g_pausemenu_screenshot_requested.format = format;
    GXSetTexCopySrc_cached(g_pausemenu_screenshot_requested.left,
        g_pausemenu_screenshot_requested.top,
        g_pausemenu_screenshot_requested.width,
        g_pausemenu_screenshot_requested.height);
    GXSetTexCopyDst_cached(g_pausemenu_screenshot_requested.width,
        g_pausemenu_screenshot_requested.height,
        g_pausemenu_screenshot_requested.format, 0);
    GXCopyTex(g_pausemenu_screenshot_requested.dest, 0);
}

void init_pausemenu_screenshot_texobj(GXTexObj *tex)
{
    if (g_pausemenu_screenshot_requested.dest != NULL)
        GXInitTexObj(tex, g_pausemenu_screenshot_requested.dest,
            g_pausemenu_screenshot_requested.width,
            g_pausemenu_screenshot_requested.height,
            g_pausemenu_screenshot_requested.format, 0, 0, 0);
}

void g_draw_pausemenu_screenshot(GXTexObj *tex)
{
    Mtx44 mtx;
    GXColor color = {0x00, 0xFF, 0x00, 0xFF};
    float cx, cy;

    g_something_with_GX_vtx_desc(0x2200);
    GXSetNumChans_cached(0);
    GXSetNumTexGens_cached(1);
    GXSetNumTevStages_cached(1);
    g_GXSetTevIndirect_zero_if_different(0);
    g_GXSetTevColorIn_GXSetTevAlphaIn_wrapper(0, 0);
    GXSetTexCoordGen2_cached(0, 1, 4, 0x3C, 0, 0x7D);
    g_GXSetTevOrder_wrapper(0, 0, 0, 0xFF);
    GXSetTevSwapMode_cached(0, 0, 0);
    GXSetTevColorIn_cached(0, 0xF, 2, 8, 4);
    GXSetTevColorOp_cached(0, 0, 0, 0, 1, 0);
    GXSetTevAlphaIn_cached(0, 7, 1, 4, 2);
    GXSetTevAlphaOp_cached(0, 0, 0, 0, 1, 0);
    opti_GXSetChanCtrl(4, 0, 0, 0, 0, 2, 2);
    GXSetTevColor(1, color);
    GXSetBlendMode_cached(1, 4, 5, 0);
    g_something_with_fog(0, 0.0f, 100.0f);
    GXSetCullMode_cached(2);
    mtxa_from_identity();
    GXLoadPosMtxImm(mtxa->mtxA, 0);
    C_MTXOrtho(mtx, 0.0f, current_render_mode->efbHeight, 0.0f, current_render_mode->fbWidth, 0.0f, 20000.0f);
    g_something_with_GXSetProjectionv(mtx, 1);
    color.r = 0xFF;
    color.g = 0xFF;
    color.b = 0xFF;
    color.a = 0xFF;
    GXSetTevColor(1, color);
    color.r = 0;
    color.g = 0;
    color.b = 0;
    color.a = 0;
    GXSetTevColor(2, color);
    GXLoadTexObj_cached(tex, 0);

    GXBegin(0x80, 7, 4);
    cx = 0.5 * current_render_mode->fbWidth;
    cy = 0.5 * current_render_mode->efbHeight;
    GXPosition3f32(cx - 0.5 * current_render_mode->fbWidth, cy - 0.5 * current_render_mode->efbHeight, -1.0f);
    GXTexCoord2f32(0.0f, 0.0f);
    GXPosition3f32(cx + 0.5 * current_render_mode->fbWidth, cy - 0.5 * current_render_mode->efbHeight, -1.0f);
    GXTexCoord2f32(1.0f, 0.0f);
    GXPosition3f32(cx + 0.5 * current_render_mode->fbWidth, cy + 0.5 * current_render_mode->efbHeight, -1.0f);
    GXTexCoord2f32(1.0f, 1.0f);
    GXPosition3f32(cx - 0.5 * current_render_mode->fbWidth, cy + 0.5 * current_render_mode->efbHeight, -1.0f);
    GXTexCoord2f32(0.0f, 1.0f);
}

Vec lbl_1_bss_1524;
struct Unk1480 lbl_1_bss_1480;
struct Unk1470 lbl_1_bss_1470;
struct PauseMenuScreenshot g_pausemenu_screenshot_requested;
struct SomeRenderFlag g_some_render_flag;
