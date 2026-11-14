#include "baseball.h"
#include "bitmap.h"
#include "event.h"
#include "input.h"

void fn_80061AF0(void);
void fn_1_7FF4(float r, float g, float b);
void gx_finish_frame_and_reset_z_and_alpha_compare_modes(void);

void fn_18_3C0(void)
{
    u32 size;

    printf("[CHECK]*************************\n");
    size = OSCheckHeap(main_heap);
    printf("MainHeap %x %x\n", main_heap, size);
    size = OSCheckHeap(stage_heap);
    printf("StageHeap %x %x\n", stage_heap, size);
    size = OSCheckHeap(bg_heap);
    printf("BackgroundHeap %x %x\n", bg_heap, size);
    size = OSCheckHeap(chara_heap);
    printf("CharaHeap %x %x\n", chara_heap, size);
}

void fn_18_4A8(void)
{
    fn_18_354();
    fn_18_25C();
    fn_18_5E0();
}

void fn_18_4D0(void)
{
    fn_18_640();
    fn_18_270();
    fn_18_390();
}

s32 g_baseball_game_loop(void)
{
    draw_ui();
    while (fn_18_28C() == 0) {
        g_zero_some_sprite_related_state();
        gx_finish_frame_and_reset_z_and_alpha_compare_modes();
        g_idle_function(0);
        g_something_with_music_fading();
        g_read_inputs_and_maybe_reset_console();
        process_inputs();
        camera_apply_viewport_2(0);
        fn_18_748();
        fn_18_680();
        tick_events();
        fn_18_6BC();
        g_something_with_replays3();
        draw_ui();
        g_something_with_graphics_processor2();
        g_something_with_GXPeekZ();
        gp_wait();
        video_finish_frame();
        if (dip_switches & 1) {
            if ((pad_status_groups[0].unk0[0].button & 0x10) != 0 && (pad_status_groups[0].unk0[2].button & 0x1000) != 0) {
                gx_start_new_frame();
                gp_wait();
                video_finish_frame();
                break;
            }
        }
    }
    fn_18_2B4();
    gx_start_new_frame();
    gp_wait();
    video_finish_frame();
    return 1;
}

void fn_18_5E0(void)
{
    fn_18_E9C();
    fn_18_18DA4();
    fn_18_1FB0();
    fn_18_192B0();
    fn_18_24B4(1);
    modeCtrl.gameType = 0xF;
    g_set_minigame_specific_funcs(NULL, NULL, fn_18_8374, fn_18_EA54);
}

void fn_18_640(void)
{
    fn_18_11F48();
    fn_18_2239C();
    fn_18_15A38();
    fn_18_134EC();
    fn_18_1214C();
    fn_18_744();
    fn_1_474D0();
    dest_all_events();
    fn_18_18DF0();
}

void fn_18_680(void)
{
    fn_18_2504();
    fn_18_12540();
    if (fn_18_1798() == 0) {
        fn_18_22458();
    }
    fn_18_18E10();
}

void fn_18_6BC(void)
{
    fn_18_11584();
}

void fn_18_6DC(void)
{
    mtxa_from_mtxb();
    fn_1_7FF4(1.0f, 1.0f, 1.0f);
    fn_18_7AC();
    fn_18_878();
    fn_18_944();
    fn_18_988();
    fn_80066BE0(3);
    nl2ngc_set_light_mask(3);
}

void fn_18_744(void)
{

}

void fn_18_748(void)
{
    fn_1_7FF4(1.0f, 1.0f, 1.0f);
    fn_18_7AC();
    fn_18_878();
    fn_18_944();
    fn_18_988();
    fn_80066BE0(3);
    nl2ngc_set_light_mask(3);
}

void fn_18_7AC(void)
{
    GXLightObj light;
    Vec pos;
    GXColor color;

    color.r = 0xFF;
    color.g = 0xFF;
    color.b = 0xFF;
    color.a = 0xFF;
    pos.x = -65.0f;
    pos.y = 92.0f;
    pos.z = 105.0f;
    mtxa_from_mtxb();
    mtxa_tf_point(&pos, &pos);
    GXInitLightPos(&light, pos.x, pos.y, pos.z);
    GXInitLightSpot(&light, 0.0f, 0);
    GXInitLightColor(&light, color);
    GXInitLightDistAttn(&light, 0.0f, 0.0f, 0);
    GXLoadLightObjImm(&light, 1);
}

void fn_18_878(void)
{
    GXLightObj light;
    Vec pos;
    GXColor color;

    color.r = 0xBE;
    color.g = 0xBE;
    color.b = 0xBE;
    color.a = 0xFF;
    pos.x = -65.0f;
    pos.y = 92.0f;
    pos.z = -105.0f;
    mtxa_from_mtxb();
    mtxa_tf_point(&pos, &pos);
    GXInitLightPos(&light, pos.x, pos.y, pos.z);
    GXInitLightSpot(&light, 0.0f, 0);
    GXInitLightColor(&light, color);
    GXInitLightDistAttn(&light, 0.0f, 0.0f, 0);
    GXLoadLightObjImm(&light, 2);
}

void fn_18_944(void)
{
    g_avdisp_set_ambient(0.35f, 0.35f, 0.35f);
}

void fn_18_988(void)
{
    Vec sp8;

    sp8.x = -3.0f;
    sp8.y = -2.0f;
    sp8.z = 3.0f;
    mtxa_from_mtxb();
    mtxa_tf_vec((Vec *) &sp8, (Vec *) &sp8);
    fn_1_10A3A0((Vec *) &sp8);
    fn_1_10A410(0.5f, 0.5f, 0.5f);
}

