/* Scene player draw pass for every camera. A TU of its own: its two floats sit
 * before scnplay_fx.c's constant pool and are reached without a pool base. */
#include "camera.h"
#include "types.h"
#include "background.h"
#include "ball.h"
#include "effect.h"
#include "event.h"
#include "light.h"
#include "pool.h"
#include "shadow.h"

int fn_1_D73B4(void);
void fn_1_D4870(void);
void g_maybe_some_bg_effect_handler(void);
void camera_apply_viewport_if_changed(int i);
void g_something_with_rend_efc(int mask);
void g_draw_stage(void);
void g_smth_with_rendefc_reflective_height(float height);
void fn_1_141F7C(int a);
void g_draw_bg(void);
void draw_shadow_reqs(void);
void fn_1_D6F9C(int i);
void fn_1_D6D84(int i);
void handle_test_camera(void);
void ord_tbl_draw_nodes(void);
void fn_1_143F34(u32 mask);
void fn_1_143FB8(void);

#pragma pool_data off

void fn_1_1439F4(void)
{
    int i;
    struct Ball *ballBackup;
    int flag;

    fn_1_143FB8();
    flag = fn_1_D73B4() > 0;
    ballBackup = current_ball;
    for (i = 0; i < 5; i++)
    {
        s8 status;
        s8 stageStatus;

        if (!(cameras[i].sub28.viewportW > 0.0f) || !(cameras[i].sub28.viewportH > 0.0f))
            continue;
        status = g_poolInfo.cameraPool.statusList[i];
        if ((status == 0 || status == 4 || (cameras[i].flags & 0x40)) && !(cameras[i].flags & 0x80))
            continue;
        current_ball = &balls[i];
        camera_apply_viewport_if_changed(i);
        fn_1_D4870();
        g_maybe_some_bg_effect_handler();
        u_reset_light_group_stack(i);
        fn_1_143F34(4);
        if (events[21].state == 2)
            g_something_with_rend_efc(4);
        stageStatus = events[0].state;
        if (stageStatus == 2 || stageStatus == 4)
            g_draw_stage();
        if (flag)
            fn_1_141F7C(-3);
        else
            fn_1_141F7C(-1);
        if (events[20].state == 2)
        {
            g_smth_with_rendefc_reflective_height(400.0f);
            g_draw_bg();
            g_smth_with_rendefc_reflective_height(0.0f);
        }
        draw_shadow_reqs();
        if (flag)
            fn_1_141F7C(2);
        if (events[21].state == 2)
            g_something_with_rend_efc(0x10);
        if (events[14].state == 2)
            g_draw_effects();
        if (world_theme.unk8 & 1)
            fn_1_D6F9C(i);
        handle_test_camera();
        ord_tbl_draw_nodes();
        if (world_theme.unk8 & 1)
            fn_1_D6D84(i);
        if (events[21].state == 2)
            g_something_with_rend_efc(8);
        fn_1_143F34(8);
    }
    current_ball = ballBackup;
    camera_setup_default_projection();
}
