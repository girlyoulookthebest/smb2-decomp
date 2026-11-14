/* Continue screen draw. */
#include "types.h"
#include "avdisp.h"
#include "background.h"
#include "ball.h"
#include "camera.h"
#include "effect.h"
#include "event.h"
#include "gx.h"
#include "light.h"
#include "mathutil.h"
#include "mode.h"
#include "stage.h"

extern u32 g_some_render_flag;
extern struct GMAModel *continue_gma_model;

void g_something_with_raster_state(int);
void opti_GXSetChanCtrl(int chan, int enable, int ambSrc, int matSrc, int lightMask, int diffFn, int attnFn);
void GXSetNumChans_cached(u8 n);
void g_GXSetTevOrder_wrapper(int stage, int coord, int map, int color);
void GXSetTevColorIn_cached(int stage, int a, int b, int c, int d);
void GXSetTevColorOp_cached(int stage, int op, int bias, int scale, int clamp, int reg);
void GXSetTevAlphaIn_cached(int stage, int a, int b, int c, int d);
void GXSetTevAlphaOp_cached(int stage, int op, int bias, int scale, int clamp, int reg);
void g_GXSetTevIndirect_zero_if_different(int stage);
void GXSetNumTevStages_cached(u8 n);
void GXSetNumTexGens_cached(u8 n);
void GXSetNumIndStages_cached(u8 n);
void *fn_1_10A69C(void *func);
void *g_yet_another_unk_draw_func(void *func);
void fn_1_D4870(void);
void g_maybe_some_bg_effect_handler(void);
void ord_tbl_draw_nodes(void);
void g_draw_ball_and_ape(void);
void g_draw_bg(void);

#define CLAMP(x, lo, hi) ((x) < (lo) ? (lo) : ((x) > (hi) ? (hi) : (x)))

int fn_2_1E280(void)
{
    int stage;

    GXSetBlendMode_cached(1, 2, 1, 0);
    GXSetZMode_cached(1, 3, 0);
    g_something_with_raster_state(0);
    opti_GXSetChanCtrl(2, 0, 0, 1, 0, 0, 2);
    opti_GXSetChanCtrl(0, 1, 0, 1, 0, 0, 2);
    GXSetNumChans_cached(1);
    stage = 0;
    g_GXSetTevOrder_wrapper(stage, 0xFF, 0xFF, 4);
    GXSetTevColorIn_cached(stage, 0xF, 0xF, 0xF, 0xA);
    GXSetTevColorOp_cached(stage, 0, 0, 0, 1, 0);
    GXSetTevAlphaIn_cached(stage, 7, 7, 7, 5);
    GXSetTevAlphaOp_cached(stage, 0, 0, 0, 1, 0);
    g_GXSetTevIndirect_zero_if_different(stage);
    stage++;
    GXSetNumTevStages_cached(stage);
    GXSetNumTexGens_cached(0);
    GXSetNumIndStages_cached(0);
    return 1;
}

void fn_2_1E3C0(int pass)
{
    Mtx mtx;
    Vec scale;
    Vec pos;
    float blend;
    float dx;
    float dy;
    float dz;
    int i;
    void *prevFunc;
    int angle;
    int t;
    int flag;

    t = modeCtrl.submodeTimer - 30.0;
    t = CLAMP(t, 0, 60);
    flag = 0;
    if ((sub_mode == 0x41 || sub_mode == 0x42) && (modeCtrl.courseFlags & 4) && modeCtrl.unk10 == 1)
        flag = 1;
    if (!flag)
    {
        float fade;
        if (sub_mode != 0x41 && sub_mode != 0x42) {
            fade = 0.0f;
        } else if ((modeCtrl.courseFlags & 4) && modeCtrl.unk10 == 0) {
            fade = t / 60.0f;
            fade = 1.0f - fade;
        } else {
            fade = 0.0f;
        }
        dz = 0.55f;
        dy = 3.0f;
        switch (pass)
        {
        case 1:
            mtxa_from_mtxb();
            scale.x = 0.0f;
            scale.y = 0.01f;
            scale.z = 0.0f;
            mtxa_translate(&scale);
            mtxa_scale_xyz(dz, dy, dz);
            avdisp_set_scale_factor(dz > dy ? dz : dy);
            load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
            avdisp_set_z_mode(0, 3, 0);
            prevFunc = fn_1_10A69C(fn_2_1E280);
            avdisp_draw_model_unculled_sort_never(init_common_gma->modelEntries[0x258 / 8].model);
            fn_1_10A69C(prevFunc);
            avdisp_set_z_mode(1, 3, 1);
            break;
        case 0:
            mtxa_from_mtxb();
            mtxa_translate_xyz(0.0f, dy, 0.0f);
            mtxa_rotate_x(-0x4000);
            mtxa_scale_s(0.125f);
            avdisp_set_scale_factor(0.125f);
            load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
            avdisp_draw_model_culled_sort_never(init_common_gma->modelEntries[0x340 / 8].model);
            break;
        }
    }
    else
    {
        blend = t / 60.0f;
        blend = 2.0 * (blend - 0.5);
        blend = CLAMP(blend, 0.0, 1.0);
        blend = blend * blend;
        mtxa_from_identity();
        mtxa_to_mtx(mtx);
        mtx[0][0] = 0.9f;
        mtx[2][2] = 0.9f;
        mtx[1][1] = 3.0f;
        mtx[1][3] = 0.01f;
        for (i = 0; i < 4; i++)
        {
            float sn;
            float cs;

            angle = (i << 14) + 0x2000;
            sn = math_sin(angle);
            cs = math_sin(angle + 0x4000);
            pos.x = 1.75 * sn;
            pos.y = 3.0f;
            pos.z = 1.75 * cs;
            dx = 1.75 * sn * blend;
            dy = 0.0f;
            dz = 1.75 * cs * blend;
            mtx[0][1] = pos.x - dx;
            mtx[2][1] = pos.z - dz;
            mtx[0][3] = dx;
            mtx[2][3] = dz;
            switch (pass)
            {
            case 1:
                mtxa_from_mtx(mtx);
                mtxa_mult_left(mtxa->mtxB);
                avdisp_set_scale_factor(3.0f);
                load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
                avdisp_set_z_mode(0, 3, 0);
                prevFunc = fn_1_10A69C(fn_2_1E280);
                avdisp_draw_model_unculled_sort_never(init_common_gma->modelEntries[0x260 / 8].model);
                fn_1_10A69C(prevFunc);
                avdisp_set_z_mode(1, 3, 1);
                break;
            case 0:
                mtxa_from_mtxb_translate(&pos);
                dx -= pos.x;
                dy -= pos.y;
                dz -= pos.z;
                mtxa_rotate_y(math_atan2(dx, dz) - 0x8000);
                dx = dx * dx;
                dx = __fmadds(dz, dz, dx);
                mtxa_rotate_x(math_atan2(dy, math_sqrt(dx)));
                mtxa_scale_s(0.125f);
                avdisp_set_scale_factor(0.125f);
                load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
                avdisp_draw_model_culled_sort_never(init_common_gma->modelEntries[0x340 / 8].model);
                break;
            }
        }
    }
}

void game_continue_draw_func(void)
{
    Mtx mtxB;
    Mtx mtx;
    void *envMap;
    void *prevEnvMap;

    g_some_render_flag |= 1;
    camera_apply_viewport_if_changed(modeCtrl.currPlayer);
    fn_1_D4870();
    g_maybe_some_bg_effect_handler();
    u_reset_light_group_stack(modeCtrl.currPlayer);
    mtxa_from_mtxb();
    if (stagedef->unkD4 != NULL)
    {
        mtxa_translate(stagedef->unkD4);
        mtxa_rotate_z(((S16Vec *)((u8 *)stagedef->unkD4 + 0xC))->z);
        mtxa_rotate_y(((S16Vec *)((u8 *)stagedef->unkD4 + 0xC))->y);
        mtxa_rotate_x(((S16Vec *)((u8 *)stagedef->unkD4 + 0xC))->x);
    }
    mtxa_to_mtx(mtx);
    if (continue_gma_model != NULL)
    {
        mtx_copy(mtxa->mtxB, mtxB);
        mtx_copy(mtx, mtxa->mtxB);
        envMap = world_theme.unk8C;
        if (envMap != NULL)
            prevEnvMap = g_yet_another_unk_draw_func(envMap);
        mtxa_from_mtxb();
        load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
        avdisp_draw_model_unculled_sort_auto(continue_gma_model);
        if (envMap != NULL)
            g_yet_another_unk_draw_func(prevEnvMap);
        mtx_copy(mtxB, mtxa->mtxB);
    }
    if (events[EVENT_BALL].state == EV_STATE_RUNNING)
        g_draw_ball_and_ape();
    if (events[EVENT_BACKGROUND].state == EV_STATE_RUNNING)
        g_draw_bg();
    if (events[EVENT_EFFECT].state == EV_STATE_RUNNING)
        g_draw_effects();
    mtx_copy(mtxa->mtxB, mtxB);
    mtx_copy(mtx, mtxa->mtxB);
    fn_2_1E3C0(0);
    mtx_copy(mtxB, mtxa->mtxB);
    ord_tbl_draw_nodes();
    mtx_copy(mtxa->mtxB, mtxB);
    mtx_copy(mtx, mtxa->mtxB);
    fn_2_1E3C0(1);
    mtx_copy(mtxB, mtxa->mtxB);
}
