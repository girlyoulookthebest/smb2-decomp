#include "types.h"
#include "camera.h"
#include "futsal_debug.h"
#include "libc.h"
#include "rand.h"
#include "ball.h"
#include "dip.h"
#include "event.h"
#include "gx.h"
#include "input.h"
#include "mathutil.h"
#include "os.h"
#include "recplay.h"
#include "sel.h"
#include "stage.h"
#include "world.h"
#include "mode.h"
#include "pool.h"
#include "macros.h"
#include "frustum.h"
#include "pc_hfr.h"

/* .bss (0x1768 - 0x24F8). CodeWarrior addresses all of these from one base register,
 * so the set must stay complete and in order (order = first use; see FORCE_BSS_ORDER). */
s16 lbl_1_bss_1768;
s8 lbl_1_bss_176A;
s32 g_something_with_camera3;
s32 lbl_1_bss_1770;
struct Camera *g_current_camera;
struct Camera cameras[5];
float lbl_1_bss_2434;
float g_camera_aspect_ratio;
s8 lbl_1_bss_243C;
s8 g_related_to_fov2;
s16 lbl_1_bss_243E;
float lbl_1_bss_2440;
struct DemoCameraState demoCameraStates[5];
void *g_minigame_camera_func;
s16 g_camera_standstill_counters[8];

FORCE_BSS_ORDER(lbl_1_bss_1768)
FORCE_BSS_ORDER(lbl_1_bss_176A)

void init_cameras(void)
{
    int i;
    struct Camera *camera;

    g_something_with_camera3 = -1;
    lbl_1_bss_1770 = -1;
    g_current_camera = cameras;
    camera = cameras;
    lbl_1_bss_2434 = 640.0f;
    g_camera_aspect_ratio = 1.3333334f;
    for (i = 0; i < 5; i++, camera++)
    {
        camera->idx = i;
        camera->mode = 3;
        camera->unk210 = 3;
        camera->unk26 = 0;
        camera->sub28.view.unk28 = 0.0f;
        camera->sub28.view.unk2C = 0.0f;
        camera->sub28.view.fov = 0x2AAA;
        camera->sub28.view.nextFov = 0x2AAA;
        camera->sub28.view.aspect = g_camera_aspect_ratio;
        camera->sub28.viewportX = 0.0f;
        camera->sub28.viewportY = 0.0f;
        camera->sub28.viewportW = 1.0f;
        camera->sub28.viewportH = 1.0f;
    }
    lbl_1_bss_243C = -1;
    g_related_to_fov2 = 2;
    camera_apply_viewport(0);
}

/* Sets the virtual screen width and aspect ratio for the current view mode. */
static inline void camera_update_screen_size(void)
{
    if (modeCtrl.wideMode)
    {
        switch (g_related_to_fov2)
        {
        case 0:
            lbl_1_bss_2434 = 853.3333f;
            g_camera_aspect_ratio = 16.0f / 9.0f;
            break;
        case 1:
            lbl_1_bss_2434 = 640.0f;
            g_camera_aspect_ratio = 16.0f / 9.0f;
            break;
        case 2:
            lbl_1_bss_2434 = 739.00836f;
            g_camera_aspect_ratio = 16.0f / 9.0f;
            break;
        case 3:
            lbl_1_bss_2434 = 640.0f;
            g_camera_aspect_ratio = 4.0f / 3.0f;
            break;
        }
    }
    else
    {
        lbl_1_bss_2434 = 640.0f;
        g_camera_aspect_ratio = 4.0f / 3.0f;
    }
}

void event_camera_init(void)
{
    int i;
    struct Camera *camera;
    float viewportRatio;

    if (lbl_1_bss_243C != -1)
    {
        g_related_to_fov2 = lbl_1_bss_243C;
        lbl_1_bss_243C = -1;
    }
    else
        g_related_to_fov2 = 2;

    camera_update_screen_size();

    camera = cameras;
    for (i = 0; i < 5; i++, camera++)
    {
        camera->flags = 0;
        camera_clear(camera);
        camera->unk26 = 0;
        camera->sub28.view.unk28 = 0.0f;
        camera->sub28.view.unk2C = 0.0f;
        camera->sub28.view.fov = 0x2AAA;
        viewportRatio = camera->sub28.viewportW / camera->sub28.viewportH;
        if (modeCtrl.wideMode)
        {
            switch (g_related_to_fov2)
            {
            case 0:
            case 1:
            case 2:
                camera->sub28.view.aspect = 16.0f * viewportRatio / 9.0f;
                break;
            case 3:
                camera->sub28.view.aspect = 4.0f / 3.0f * viewportRatio;
                break;
            }
        }
        else
            camera->sub28.view.aspect = 4.0f / 3.0f * viewportRatio;
    }
    camera_apply_viewport(0);
}

extern s32 lbl_1_bss_D336C;
extern float lbl_1_bss_D3370;
extern float lbl_1_bss_D3374;
extern float lbl_1_data_65750;
extern s16 lbl_16_bss_34050;

/* .data: futsal_debug.h's tables come first, then these. */
s8 g_active_players[4] = { 0, 0, 0, 0 };  /* per-player flag read by the input code */
float lbl_1_data_6DC4 = 0.1f;  /* near plane for billiards' main sub-mode */

void g_camera_func0(struct Camera *, struct Ball *);
void g_camera_func1(struct Camera *, struct Ball *);
void g_camera_func2(struct Camera *, struct Ball *);
void g_camera_func3_setup(struct Camera *, struct Ball *);
void g_camera_func4_fallout(struct Camera *, struct Ball *);
void g_camera_func5(struct Camera *, struct Ball *);
void g_camera_func6(struct Camera *, struct Ball *);
void g_camera_func7(struct Camera *, struct Ball *);
void g_camera_func8(struct Camera *, struct Ball *);
void g_camera_func9(struct Camera *, struct Ball *);
void g_camera_func10(struct Camera *, struct Ball *);
void g_camera_func11_spinin(struct Camera *, struct Ball *);
void g_camera_func12(struct Camera *, struct Ball *);
void g_camera_func13(struct Camera *, struct Ball *);
void g_camera_func14(struct Camera *, struct Ball *);
void g_camera_func15_goal(struct Camera *, struct Ball *);
void g_camera_func16(struct Camera *, struct Ball *);
void g_camera_func17(struct Camera *, struct Ball *);
void g_camera_func18(struct Camera *, struct Ball *);
void g_camera_func19(struct Camera *, struct Ball *);
void g_camera_func20(struct Camera *, struct Ball *);
void g_camera_func21(struct Camera *, struct Ball *);
void g_camera_func22_23_24_timeover(struct Camera *, struct Ball *);
void g_camera_func25(struct Camera *, struct Ball *);
void g_camera_func26(struct Camera *, struct Ball *);
void g_camera_func27(struct Camera *, struct Ball *);
void g_camera_func28(struct Camera *, struct Ball *);
void g_camera_func29(struct Camera *, struct Ball *);
void g_camera_func30(struct Camera *, struct Ball *);
void g_camera_func31(struct Camera *, struct Ball *);
void g_camera_func32(struct Camera *, struct Ball *);
void g_camera_func33(struct Camera *, struct Ball *);
void g_camera_func34(struct Camera *, struct Ball *);
void g_camera_func35(struct Camera *, struct Ball *);
void g_camera_func36(struct Camera *, struct Ball *);
void g_camera_func37(struct Camera *, struct Ball *);
void g_camera_func38(struct Camera *, struct Ball *);
void g_camera_func39(struct Camera *, struct Ball *);
void g_camera_func40(struct Camera *, struct Ball *);
void g_camera_func41(struct Camera *, struct Ball *);
void g_camera_func42(struct Camera *, struct Ball *);
void g_camera_func43(struct Camera *, struct Ball *);
void g_camera_func44(struct Camera *, struct Ball *);
void g_camera_func45(struct Camera *, struct Ball *);
void g_camera_func46(struct Camera *, struct Ball *);
void g_camera_func47(struct Camera *, struct Ball *);
void g_camera_func48(struct Camera *, struct Ball *);
void g_camera_func49(struct Camera *, struct Ball *);
void g_camera_func50(struct Camera *, struct Ball *);
void g_camera_func51(struct Camera *, struct Ball *);
void g_camera_func52(struct Camera *, struct Ball *);
void g_camera_func53(struct Camera *, struct Ball *);
void g_camera_func54(struct Camera *, struct Ball *);
void g_camera_func55(struct Camera *, struct Ball *);
void g_camera_func56(struct Camera *, struct Ball *);
void g_camera_func57(struct Camera *, struct Ball *);
void g_camera_default_func(struct Camera *, struct Ball *);
void g_camera_func62(struct Camera *, struct Ball *);
void g_camera_func63(struct Camera *, struct Ball *);
void g_camera_func66(struct Camera *, struct Ball *);
void g_camera_func68(struct Camera *, struct Ball *);
void g_camera_func69(struct Camera *, struct Ball *);
void g_camera_func70(struct Camera *, struct Ball *);
void g_camera_func71(struct Camera *, struct Ball *);
void g_camera_func72(struct Camera *, struct Ball *);
void g_camera_func73(struct Camera *, struct Ball *);
void g_main_camera_function_func75(struct Camera *, struct Ball *);
void g_camera_func76_gameplay(struct Camera *, struct Ball *);
void g_camera_func80(struct Camera *, struct Ball *);
void g_camera_func81_mainmenu(struct Camera *, struct Ball *);
void g_camera_func82(struct Camera *, struct Ball *);
void g_camera_func83(struct Camera *, struct Ball *);
void g_camera_func84(struct Camera *, struct Ball *);
void g_camera_func85(struct Camera *, struct Ball *);
void g_camera_func87(struct Camera *, struct Ball *);
void g_camera_func88(struct Camera *, struct Ball *);
void g_camera_func89(struct Camera *, struct Ball *);
void g_camera_func90(struct Camera *, struct Ball *);
void g_camera_func91(struct Camera *, struct Ball *);
void g_camera_func92(struct Camera *, struct Ball *);

void (*camera_funcs[])(struct Camera *, struct Ball *) =
{
    g_camera_func0,
    g_camera_func1,
    g_camera_func2,
    g_camera_func3_setup,
    g_camera_func4_fallout,
    g_camera_func5,
    g_camera_func6,
    g_camera_func7,
    g_camera_func8,
    g_camera_func9,
    g_camera_func10,
    g_camera_func11_spinin,
    g_camera_func12,
    g_camera_func13,
    g_camera_func14,
    g_camera_func15_goal,
    g_camera_func16,
    g_camera_func17,
    g_camera_func18,
    g_camera_func19,
    g_camera_func20,
    g_camera_func21,
    g_camera_func22_23_24_timeover,
    g_camera_func22_23_24_timeover,
    g_camera_func22_23_24_timeover,
    g_camera_func25,
    g_camera_func26,
    g_camera_func27,
    g_camera_func28,
    g_camera_func29,
    g_camera_func30,
    g_camera_func31,
    g_camera_func32,
    g_camera_func33,
    g_camera_func34,
    g_camera_func35,
    g_camera_func36,
    g_camera_func37,
    g_camera_func38,
    g_camera_func39,
    g_camera_func40,
    g_camera_func41,
    g_camera_func42,
    g_camera_func43,
    g_camera_func44,
    g_camera_func45,
    g_camera_func46,
    g_camera_func47,
    g_camera_func48,
    g_camera_func49,
    g_camera_func50,
    g_camera_func51,
    g_camera_func52,
    g_camera_func53,
    g_camera_func54,
    g_camera_func55,
    g_camera_func56,
    g_camera_func57,
    g_camera_default_func,
    g_camera_default_func,
    g_camera_default_func,
    g_camera_default_func,
    g_camera_func62,
    g_camera_func63,
    g_camera_default_func,
    g_camera_default_func,
    g_camera_func66,
    g_camera_default_func,
    g_camera_func68,
    g_camera_func69,
    g_camera_func70,
    g_camera_func71,
    g_camera_func72,
    g_camera_func73,
    g_camera_default_func,
    g_main_camera_function_func75,
    g_camera_func76_gameplay,
    g_camera_default_func,
    g_camera_default_func,
    g_camera_default_func,
    g_camera_func80,
    g_camera_func81_mainmenu,
    g_camera_func82,
    g_camera_func83,
    g_camera_func84,
    g_camera_func85,
    g_camera_default_func,
    g_camera_func87,
    g_camera_func88,
    g_camera_func89,
    g_camera_func90,
    g_camera_func91,
    g_camera_func92,
};

void event_camera_tick(void)
{
    int i;
    struct Camera *camera;
    struct Ball *ball;
    s8 *status;
    Vec sp8;

    camera_update_screen_size();

    if (dip_switches & DIP_DEBUG)
    {
        u32 prevDipSwitches = dip_switches;
        s16 pad;

        for (pad = 0; pad < 4; pad++)
        {
            if (pad_status_groups[pad].unk0[0].err != 0)
                continue;
            if ((dip_switches & DIP_TEST_CAM) && (pad_status_groups[pad].unk0[2].button & PAD_BUTTON_Y))
                dip_switches ^= DIP_NO_INTR;
            if ((pad_status_groups[pad].unk0[0].button & PAD_BUTTON_B)
             && (pad_status_groups[pad].unk0[2].button & PAD_BUTTON_Y))
            {
                if (dip_switches & DIP_TEST_CAM)
                    dip_switches &= ~DIP_TEST_CAM;
                else if (g_some_other_flags & 1)
                {
                    dip_switches |= DIP_TEST_CAM;
                    dip_switches &= ~DIP_NO_INTR;
                }
            }
            if (dip_switches != prevDipSwitches)
                break;
        }
    }

    camera = cameras;
    ball = balls;
    status = g_poolInfo.cameraPool.statusList;
    for (i = 0; i < 5; i++, camera++, ball++, status++)
    {
        if ((*status != 0 && *status != 4)
         || (camera->flags & CAMERA_FLAG_UNK10)
         || (camera->flags & CAMERA_FLAG_UNK80))
        {
            camera->flags &= ~CAMERA_FLAG_UNK1;
            if (dip_switches & DIP_TEST_CAM)
                camera_funcs[CAMERA_MODE_TEST](camera, ball);
            else
                camera_funcs[camera->mode](camera, ball);
            mtxa_from_translate(&ball->pos);
            mtxa_rotate_y(camera->rotY);
            mtxa_rigid_inv_tf_point(&camera->eye, &camera->unkC4);
            mtxa_rigid_inv_tf_point(&camera->lookAt, &camera->unkD0);
            mtx_copy(camera->unk14C, camera->unk17C);
            mtx_copy(camera->unk1AC, camera->unk1DC);
            mtxa_from_identity();
            mtxa_rotate_z(-camera->rotZ);
            mtxa_rotate_x(-camera->rotX);
            mtxa_rotate_y(-camera->rotY);
            mtxa_translate_neg(&camera->eye);
            if (!(camera->flags & CAMERA_FLAG_UNK4))
            {
                mtxa_to_mtx(camera->unk1AC);
                /* stage tilt */
                if (events[EVENT_WORLD].state == EV_STATE_RUNNING
                 && !(camera->flags & CAMERA_FLAG_NO_STAGE_TILT)
                 && !(dip_switches & DIP_FIX_WORLD))
                {
                    mtxa_translate(&ball->pos);
                    mtxa_rotate_x(world_infos[i].xrot * 0.6);
                    mtxa_rotate_z(world_infos[i].zrot * 0.6);
                    mtxa_translate_neg(&ball->pos);
                }
                mtxa_to_mtx(camera->unk14C);
            }
            else
            {
                mtxa_push();
                mtxa_translate(&ball->pos);
                mtxa_rotate_z(-world_infos[i].zrot >> 1);
                mtxa_rotate_x(-world_infos[i].xrot >> 1);
                mtxa_translate_neg(&ball->pos);
                mtxa_to_mtx(camera->unk1AC);
                mtxa_pop();
                mtxa_translate(&ball->pos);
                mtxa_rotate_x(world_infos[i].xrot >> 1);
                mtxa_rotate_z(world_infos[i].zrot >> 1);
                mtxa_translate_neg(&ball->pos);
                mtxa_to_mtx(camera->unk14C);
            }

            switch (camera->unk26)
            {
            case 1:
                camera->sub28.view.unk28 = 0.0f;
                camera->sub28.view.unk2C = 0.0f;
                break;
            case 2:
            case 3:
                if (camera->sub28.view.unk28 < 0.0)
                {
                    camera->sub28.view.unk28 += 0.0041666666666666666;
                    if (camera->sub28.view.unk28 > 0.0)
                        camera->sub28.view.unk28 = 0.0f;
                }
                else if (camera->sub28.view.unk28 > 0.0)
                {
                    camera->sub28.view.unk28 -= 0.0041666666666666666;
                    if (camera->sub28.view.unk28 < 0.0)
                        camera->sub28.view.unk28 = 0.0f;
                }
                if (camera->sub28.view.unk2C < 0.0)
                {
                    camera->sub28.view.unk2C += 0.0041666666666666666;
                    if (camera->sub28.view.unk2C > 0.0)
                        camera->sub28.view.unk2C = 0.0f;
                }
                else if (camera->sub28.view.unk2C > 0.0)
                {
                    camera->sub28.view.unk2C -= 0.0041666666666666666;
                    if (camera->sub28.view.unk2C < 0.0)
                        camera->sub28.view.unk2C = 0.0f;
                }
                break;
            case 4:
                camera->sub28.view.unk28 = 0.0f;
                camera->sub28.view.unk2C = -0.375f;
                break;
            case 0:
                camera->sub28.view.unk28 = 0.0f;
                camera->sub28.view.unk2C = 0.0f;
                break;
            case 5:
                camera->sub28.view.unk28 = 0.0f;
                camera->sub28.view.unk2C = 0.0f;
                if (g_some_other_flags & 0xA)
                    break;
                mtxa_from_mtx(camera->unk14C);
                mtxa_tf_point(&ball->pos, &sp8);
                if (sp8.z > 0.0)
                    camera->sub28.view.fov = DEGREES_TO_S16(120);
                else
                {
                    float f1 = 2.0 * ball->currRadius + math_sqrt(mathutil_sum_of_sq_2(sp8.x, sp8.y));
                    s16 fov = math_atan2(f1, -sp8.z) * 2;

                    if (fov > DEGREES_TO_S16(120))
                        fov = DEGREES_TO_S16(120);
                    else if (fov < DEGREES_TO_S16(7))
                        fov = DEGREES_TO_S16(7);
                    camera->sub28.view.fov += (fov - camera->sub28.view.fov) >> 5;
                }
                break;
            case 6:
                switch (modeCtrl.difficulty)
                {
                case 0:
                    if (camera->timerCurr <= 1140)
                    {
                        camera->sub28.view.unk28 = 0.0f;
                        camera->sub28.view.unk2C = 0.0f;
                    }
                    else
                    {
                        camera->sub28.view.unk28 = 0.0f;
                        camera->sub28.view.unk2C = 0.0f;
                    }
                    break;
                default:
                    if (camera->timerCurr <= 1140)
                    {
                        camera->sub28.view.unk28 = -0.3f;
                        camera->sub28.view.unk2C = 0.0f;
                    }
                    else
                    {
                        camera->sub28.view.unk28 = 0.0f;
                        camera->sub28.view.unk2C = 0.0f;
                    }
                }
                camera->sub28.view.fov = DEGREES_TO_S16(30);
                break;
            case 7:
                camera->sub28.view.unk28 = 0.0f;
                camera->sub28.view.unk2C = -0.54f;
                break;
            case 8:
                camera->sub28.view.unk28 = 0.0f;
                camera->sub28.view.unk2C = 0.0f;
                camera->sub28.view.fov = DEGREES_TO_S16(30);
                break;
            case 10:
                if (i == 0 || i == 2)
                {
                    camera->sub28.view.fov = lbl_1_bss_1768;
                    if (camera->sub28.view.fov <= 0)
                        camera->sub28.view.fov = 1;
                }
                else
                    camera->sub28.view.fov = DEGREES_TO_S16(22.5);
                break;
            case 12:
                if (lbl_1_bss_D336C != 0)
                {
                    camera->sub28.view.unk28 = lbl_1_bss_D3370;
                    camera->sub28.view.unk2C = lbl_1_bss_D3374;
                    camera->sub28.view.fov = DEGREES_TO_S16(lbl_1_data_65750);
                }
                break;
            case 15:
                if (i == 0)
                    camera->sub28.view.fov = lbl_16_bss_34050;
                else
                    camera->sub28.view.fov = DEGREES_TO_S16(5);
                break;
            }
            camera->sub28.view.tanHalfFov = math_tan(camera->sub28.view.fov >> 1);
            camera->sub28.view.invTanHalfFov = 1.0 / camera->sub28.view.tanHalfFov;
            camera_update_shake(camera);
        }
    }
    camera_apply_viewport(0);
}

void event_camera_dest(void)
{
    disable_all_cameras();
}

void enable_camera(int cameraId)
{
    g_poolInfo.cameraPool.statusList[cameraId] = 2;
}

void disable_camera(int cameraId)
{
    g_poolInfo.cameraPool.statusList[cameraId] = 0;
}

void disable_all_cameras(void)
{
    int i;

    for (i = 0; i < 5; i++)
        g_poolInfo.cameraPool.statusList[i] = 0;
}

void enable_cameras_for_active_players(void)
{
    int i;

    for (i = 0; i < 5; i++)
    {
        if (g_poolInfo.playerPool.statusList[i] == 2 || g_poolInfo.playerPool.statusList[i] == 4)
            g_poolInfo.cameraPool.statusList[i] = 2;
    }
}

void camera_update_aspect_ratios(void)
{
    struct Camera *camera = cameras;
    int i;

    for (i = 0; i < 5; i++, camera++)
    {
        if (modeCtrl.wideMode)
        {
            switch (g_related_to_fov2)
            {
            case 0:
            case 1:
            case 2:
                camera->sub28.view.aspect = 16.0f / 9.0f;
                break;
            case 3:
                camera->sub28.view.aspect = 4.0f / 3.0f;
                break;
            }
        }
        else
            camera->sub28.view.aspect = 4.0f / 3.0f;
    }
}

float camera_get_aspect_ratio(void)
{
    return g_camera_aspect_ratio;
}

void camera_setup_default_projection(void)
{
    struct Camera *camera;
    Mtx44 mtx;

    g_something_with_camera3 = 5;
    lbl_1_bss_1770 = 5;
    g_current_camera = cameras;
    camera = cameras;
    HFR_VIEW(0, camera->unk14C);
    mtx_copy(camera->unk14C, mtxa->mtxB);
    mtx_copy(mtxa->mtxB, g_ord_tbl_stuff->matrices[2]);
    mtx_copy(camera->unk17C, g_ord_tbl_stuff->matrices[3]);
    mtx_copy(camera->unk1AC, g_ord_tbl_stuff->matrices[0]);
    mtx_copy(camera->unk1DC, g_ord_tbl_stuff->matrices[4]);
    camera_build_projection(mtx, DEGREES_TO_S16(60), g_camera_aspect_ratio, 0.1f, 20000.0f, 0.0f, 0.0f);
    g_something_with_GXSetProjectionv(mtx, 0);
    GXSetViewport(0.0f, 0.0f, current_render_mode->fbWidth, current_render_mode->efbHeight, 0.0f, 1.0f);
    GXSetScissor_cached(0, 0, current_render_mode->fbWidth, current_render_mode->xfbHeight);
    fn_1_22D64(&(Vec){ 0.0f, 0.0f, 0.0f }, &(S16Vec){ 0, 0, 0 }, 59.996338f, g_camera_aspect_ratio, 0.0f, 0.0f);
}

void set_camera_view_settings(int cameraId, float left, float top, float width, float height)
{
    struct Camera *camera = &cameras[cameraId];

    camera->sub28.viewportX = left;
    camera->sub28.viewportY = top;
    camera->sub28.viewportW = width;
    camera->sub28.viewportH = height;
    if (width > 0.0f && height > 0.0f)
    {
        camera->sub28.view.aspect = g_camera_aspect_ratio * (width / height);
        if (height == 1.0f)
            camera->sub28.view.nextFov = DEGREES_TO_S16(60);
        else
        {
            float f1 = (0.5f + 0.5f * height) * math_tan(DEGREES_TO_S16(30));
            s16 fov = math_atan2(f1, 1.0f) * 2;

            camera->sub28.view.nextFov = fov;
            camera->sub28.view.fov = camera->sub28.view.nextFov;
        }
    }
    else
        camera->sub28.view.aspect = 4.0f / 3.0f;
    if (g_something_with_camera3 == cameraId)
        camera_apply_viewport(cameraId);
}

struct PlayerViewport
{
    float left;
    float top;
    float width;
    float height;
};

struct PlayerViewport onePlayerViewports[] =
{
    { 0.0f, 0.0f, 1.0f, 1.0f },  /* whole screen */
};
struct PlayerViewport twoPlayerViewports[] =
{
    { 0.0f, 0.0f, 1.0f, 0.5f },  /* top half */
    { 0.0f, 0.5f, 1.0f, 0.5f },  /* bottom half */
};
struct PlayerViewport threePlayerViewports[] =
{
    { 0.0f, 0.0f, 1.0f, 0.5f },  /* top half */
    { 0.0f, 0.5f, 0.5f, 0.5f },  /* bottom left quadrant */
    { 0.5f, 0.5f, 0.5f, 0.5f },  /* bottom right quadrant */
};
struct PlayerViewport fourPlayerViewports[] =
{
    { 0.0f, 0.0f, 0.5f, 0.5f },  /* top left quadrant */
    { 0.5f, 0.0f, 0.5f, 0.5f },  /* top right quadrant */
    { 0.0f, 0.5f, 0.5f, 0.5f },  /* bottom left quadrant */
    { 0.5f, 0.5f, 0.5f, 0.5f },  /* bottom right quadrant */
};
struct PlayerViewport *splitscreenViewports[] =
{
    onePlayerViewports,
    twoPlayerViewports,
    threePlayerViewports,
    fourPlayerViewports,
};

void camera_setup_splitscreen_viewports(int playerCount)
{
    int i;
    struct PlayerViewport *vp;

    if (playerCount < 1 || playerCount > 4)
        OSPanic("camera.c", 790, "Ielligal player number");
    vp = splitscreenViewports[playerCount - 1];
    for (i = 0; i < 4; i++)
    {
        if (i >= playerCount)
            set_camera_view_settings(i, 0.0f, 0.0f, 0.0f, 0.0f);
        else
        {
            set_camera_view_settings(i, vp->left, vp->top, vp->width, vp->height);
            vp++;
        }
    }
}

void camera_setup_singleplayer_viewport(void)
{
    int i;

    for (i = 0; i < 5; i++)
        set_camera_view_settings(i, 0.0f, 0.0f, 0.0f, 0.0f);
    set_camera_view_settings(0, 0.0f, 0.0f, 1.0f, 1.0f);
}

void camera_apply_viewport(int cameraId)
{
    struct Camera *camera = &cameras[cameraId];
    Mtx44 projMtx;

    g_something_with_camera3 = cameraId;
    lbl_1_bss_1770 = cameraId;
    g_current_camera = camera;
    HFR_VIEW(cameraId, camera->unk14C);
    mtx_copy(camera->unk14C, mtxa->mtxB);
    mtx_copy(mtxa->mtxB, g_ord_tbl_stuff->matrices[2]);
    mtx_copy(camera->unk17C, g_ord_tbl_stuff->matrices[3]);
    mtx_copy(camera->unk1AC, g_ord_tbl_stuff->matrices[0]);
    mtx_copy(camera->unk1DC, g_ord_tbl_stuff->matrices[4]);
    mtx_copy(g_ord_tbl_stuff->matrices[0], g_ord_tbl_stuff->matrices[1]);
    g_ord_tbl_stuff->matrices[1][0][3] = 0.0f;
    g_ord_tbl_stuff->matrices[1][1][3] = -camera->eye.y;
    g_ord_tbl_stuff->matrices[1][2][3] = 0.0f;
    if (main_mode == MD_MINI
     && (sub_mode == SMD_MINI_BILLIARDS_MAIN || sub_mode == SMD_MINI_BILLIARDS_UNKC6)
     && cameraId != 1)
    {
        camera_build_projection_for(projMtx, camera,
            (sub_mode == SMD_MINI_BILLIARDS_MAIN) ? lbl_1_data_6DC4 : 0.17320509f / camera->sub28.view.tanHalfFov,
            20000.0f);
    }
    else
        camera_build_projection_for(projMtx, camera, camera->sub28.view.nearZ, camera->sub28.view.farZ);
    g_something_with_GXSetProjectionv(projMtx, 0);
    if (camera->sub28.viewportW > 0.0f && camera->sub28.viewportH > 0.0f)
    {
        float fbW = current_render_mode->fbWidth;
        float fbH = current_render_mode->efbHeight;
        float left = fbW * camera->sub28.viewportX;
        float top = fbH * camera->sub28.viewportY;
        float width = fbW * camera->sub28.viewportW;
        float height = fbH * camera->sub28.viewportH;

        GXSetViewport(left, top, width, height, 0.0f, 1.0f);
        GXSetScissor_cached(left, top, width, height);
    }
    fn_1_22D64(
        &camera->eye,
        &(S16Vec){ camera->rotX, camera->rotY, camera->rotZ },
        S16_TO_DEGREES(camera->sub28.view.fov),
        camera->sub28.view.aspect,
        camera->sub28.view.unk28,
        camera->sub28.view.unk2C);
}

void camera_apply_viewport_if_changed(int cameraId)
{
    if (g_something_with_camera3 != cameraId || lbl_1_bss_1770 != cameraId)
        camera_apply_viewport(cameraId);
}

/* camera_apply_viewport without touching the GX viewport/scissor. */
void camera_apply_viewport_2(int cameraId)
{
    struct Camera *camera = &cameras[cameraId];
    Mtx44 projMtx;

    g_something_with_camera3 = cameraId;
    g_current_camera = camera;
    HFR_VIEW(cameraId, camera->unk14C);
    mtx_copy(camera->unk14C, mtxa->mtxB);
    mtx_copy(mtxa->mtxB, g_ord_tbl_stuff->matrices[2]);
    mtx_copy(camera->unk17C, g_ord_tbl_stuff->matrices[3]);
    mtx_copy(camera->unk1AC, g_ord_tbl_stuff->matrices[0]);
    mtx_copy(camera->unk1DC, g_ord_tbl_stuff->matrices[4]);
    mtx_copy(g_ord_tbl_stuff->matrices[0], g_ord_tbl_stuff->matrices[1]);
    g_ord_tbl_stuff->matrices[1][0][3] = 0.0f;
    g_ord_tbl_stuff->matrices[1][1][3] = -camera->eye.y;
    g_ord_tbl_stuff->matrices[1][2][3] = 0.0f;
    if (main_mode == MD_MINI
     && (sub_mode == SMD_MINI_BILLIARDS_MAIN || sub_mode == SMD_MINI_BILLIARDS_UNKC6)
     && cameraId != 1)
    {
        camera_build_projection_for(projMtx, camera,
            (sub_mode == SMD_MINI_BILLIARDS_MAIN) ? lbl_1_data_6DC4 : 0.17320509f / camera->sub28.view.tanHalfFov,
            20000.0f);
    }
    else
        camera_build_projection_for(projMtx, camera, camera->sub28.view.nearZ, camera->sub28.view.farZ);
    g_something_with_GXSetProjectionv(projMtx, 0);
    fn_1_22D64(
        &camera->eye,
        &(S16Vec){ camera->rotX, camera->rotY, camera->rotZ },
        S16_TO_DEGREES(camera->sub28.view.fov),
        camera->sub28.view.aspect,
        camera->sub28.view.unk28,
        camera->sub28.view.unk2C);
}

void camera_apply_viewport_2_if_changed(int cameraId)
{
    if (g_something_with_camera3 != cameraId)
        camera_apply_viewport_2(cameraId);
}

void camera_apply_main_projection(void)
{
    struct Camera *camera = g_current_camera;
    Mtx44 projMtx;

    /* The fallback is overridden right away; kept because it changes codegen. */
    if (camera == NULL)
        camera = &cameras[0];
    camera = &cameras[0];
    camera_build_projection_for(projMtx, camera, camera->sub28.view.nearZ, camera->sub28.view.farZ);
    projMtx[0][2] -= projMtx[0][0] * camera->sub28.view.unk28 * camera->sub28.view.aspect * camera->sub28.view.tanHalfFov;
    projMtx[1][2] -= projMtx[1][1] * camera->sub28.view.unk2C * camera->sub28.view.tanHalfFov;
    g_something_with_GXSetProjectionv(projMtx, 0);
}

void camera_start_shake(int cameraId, int duration, Vec *impulse)
{
    struct Camera *camera = &cameras[cameraId];

    camera->shakeActive = 1;
    camera->shakeVel.x += impulse->x;
    camera->shakeVel.y += impulse->y;
    camera->shakeVel.z += impulse->z;
    if (duration < 0 || camera->shakeTimer < 0)
        camera->shakeTimer = -1;
    else if (camera->shakeTimer < duration)
        camera->shakeTimer = duration;
}

void camera_set_state(int state)
{
    int i;
    struct Camera *camera = &cameras[0];

    for (i = 5; i > 0; i--, camera++)
    {
        if (!(camera->flags & (1 << 5)))
        {
            camera->mode = state;
            camera->unk1F = 0;
        }
    }
}

void camera_set_state_for_mask(u32 mask, int state)
{
    struct Camera *camera = &cameras[0];

    for (; mask != 0; mask >>= 1, camera++)
    {
        if (mask & 1)
            camera->mode = state;
    }
}

void camera_set_or_clear_flags(int flags, int set)
{
    int i;
    struct Camera *camera = &cameras[0];

    if (set)
    {
        for (i = 5; i > 0; i--, camera++)
            camera->flags |= flags;
    }
    else
    {
        flags = ~flags;
        for (i = 5; i > 0; i--, camera++)
            camera->flags &= flags;
    }
}

static inline void camera_clear_shake(struct Camera *camera)
{
    camera->shakeActive = 0;
    camera->shakeTimer = 0;
    camera->shakeOffset.x = 0.0f;
    camera->shakeOffset.y = 0.0f;
    camera->shakeOffset.z = 0.0f;
    camera->shakeVel.x = 0.0f;
    camera->shakeVel.y = 0.0f;
    camera->shakeVel.z = 0.0f;
}

void camera_clear(struct Camera *camera)
{
    struct Camera backup;

    HFR_CUT(HFR_CUT_CAMERA_RESET);
    backup.idx = camera->idx;
    backup.mode = camera->mode;
    backup.flags = camera->flags & 0x50;
    backup.unk26 = camera->unk26;
    backup.sub28 = camera->sub28;
    backup.unk1F = camera->unk1F;
    memset(camera, 0, sizeof(*camera));
    camera->idx = backup.idx;
    camera->mode = backup.mode;
    camera->flags |= backup.flags | 1;
    camera->unk26 = backup.unk26;
    camera->sub28 = backup.sub28;
    camera->unk1F = backup.unk1F;

    camera->eye.z = 10.0f;
    camera->sub28.view.fov = camera->sub28.view.nextFov;
    camera->sub28.view.nearZ = 0.1f;
    camera->sub28.view.farZ = 20000.0f;
    camera->unk20 = 1.0f;

    camera_clear_shake(&cameras[camera->idx]);
}

/* Spring-damped camera shake, applied on top of the view matrices. */
void camera_update_shake(struct Camera *camera)
{
    if (camera->shakeActive != 0)
    {
        if (!(g_some_other_flags & 0xA))
        {
            camera->shakeVel.x -= 0.75 * camera->shakeOffset.x;
            camera->shakeVel.y -= 0.75 * camera->shakeOffset.y;
            camera->shakeVel.z -= 0.75 * camera->shakeOffset.z;

            camera->shakeVel.x *= 0.85;
            camera->shakeVel.y *= 0.85;
            camera->shakeVel.z *= 0.85;

            camera->shakeOffset.x += camera->shakeVel.x;
            camera->shakeOffset.y += camera->shakeVel.y;
            camera->shakeOffset.z += camera->shakeVel.z;
        }

        mtxa_from_mtx(camera->unk14C);
        mtxa_translate(&camera->shakeOffset);
        mtxa_to_mtx(camera->unk14C);

        mtxa_from_mtx(camera->unk1AC);
        mtxa_translate(&camera->shakeOffset);
        mtxa_to_mtx(camera->unk1AC);

        if (!(g_some_other_flags & 0xA) && camera->shakeTimer > 0)
        {
            camera->shakeTimer--;
            if (camera->shakeTimer == 0)
                camera_clear_shake(&cameras[camera->idx]);
        }
    }
}

/* Height above the ball to look at, and the default pitch behind it (stage 346 is special-cased). */
#define BALL_LOOK_AT_HEIGHT() (0.8 + (current_stage_id == 346 ? 0.0 : 0.18))
#define BALL_CAMERA_PITCH() (0x10000 - 0x900 - (current_stage_id == 346 ? 0 : 0x200))
#define BALL_PITCH_BIAS() (current_stage_id == 346 ? 0 : 0x200)

static inline void camera_face_direction(struct Camera *camera, Vec *lookDir)
{
    camera->rotY = math_atan2(lookDir->x, lookDir->z) - 32768;
    camera->rotX = math_atan2(lookDir->y, math_sqrt(mathutil_sum_of_sq_2(lookDir->x, lookDir->z)));
    camera->rotZ = 0;
}

void fn_1_101AC(struct Camera *camera, struct Ball *ball);
void g_camera_func13(struct Camera *camera, struct Ball *ball);
void g_camera_func30_child(struct Camera *camera, struct Ball *ball);

void g_camera_func27(struct Camera *camera, struct Ball *ball)
{
    camera_clear(camera);

    camera->eye.x = 0.0f;
    camera->eye.y = 0.0f;
    camera->eye.z = 280.0f;

    camera->lookAt.x = 0.0f;
    camera->lookAt.y = 0.0f;
    camera->lookAt.z = 0.0f;

    camera->sub28.view.fov = DEGREES_TO_S16(1);
    camera->rotX = 0;
    camera->rotY = 0;
    camera->rotZ = 0;
    camera->mode = 28;
}

void g_camera_func28(struct Camera *camera, struct Ball *ball) {}

void g_camera_func29(struct Camera *camera, struct Ball *ball)
{
    camera_clear(camera);
    fn_1_101AC(camera, ball);
    camera->mode = 30;
}

void g_camera_func30(struct Camera *camera, struct Ball *ball)
{
    g_camera_func30_child(camera, ball);
}

void g_camera_func54(struct Camera *camera, struct Ball *ball)
{
    camera_clear(camera);
    camera->flags |= CAMERA_FLAG_NO_STAGE_TILT;
    camera->mode = 55;
}

void g_camera_func55(struct Camera *camera, struct Ball *ball) {}

void g_camera_func43(struct Camera *camera, struct Ball *ball)
{
    Vec sp10;

    camera_clear(camera);

    camera->lookAt.x = ball->pos.x;
    camera->lookAt.y = ball->pos.y + BALL_LOOK_AT_HEIGHT();
    camera->lookAt.z = ball->pos.z;

    mtxa_from_translate(&camera->lookAt);
    mtxa_rotate_y(0);
    sp10.x = 0.0f;
    sp10.y = 1.0f;
    sp10.z = 3.0f;
    mtxa_tf_point(&sp10, &camera->eye);

    sp10.x = 0.0f;
    sp10.y = 0.0f;
    sp10.z = 3.0f;
    mtxa_tf_point(&sp10, &camera->unkB4);

    sp10.x = camera->lookAt.x - camera->eye.x;
    sp10.y = camera->lookAt.y - camera->eye.y;
    sp10.z = camera->lookAt.z - camera->eye.z;

    camera_face_direction(camera, &sp10);

    camera->mode = 1;
}

void get_curr_stage_fly_in_position(struct Sphere *sphere);

void g_camera_func12(struct Camera *camera, struct Ball *ball)
{
    camera_clear(camera);
    camera->unk94 = 0x80;
    camera->unk74 = 0;
    camera->unk88 = 1.0f;
    g_camera_func13(camera, ball);
    camera->mode = 13;
}

/* Orbit around the stage's fly-in sphere; players can zoom and spin it with the stick. */
void g_camera_func13(struct Camera *camera, struct Ball *ball)
{
    struct Sphere sp14;
    Vec sp8;
    int padId;
    int i;

    if (!(g_some_other_flags & 0xA) || camera->mode != 13)
    {
        get_curr_stage_fly_in_position(&sp14);
        camera->lookAt.x = sp14.pos.x;
        camera->lookAt.y = sp14.pos.y;
        camera->lookAt.z = sp14.pos.z;
        if (camera->timerCurr > 0)
        {
            for (i = 0; i < modeCtrl.playerCount; i++)
            {
                padId = player_pad_map[i];
                if (camera->unk88 > 0.75 && (analog_inputs[padId][0] & (1 << 7)))
                    camera->unk88 -= 0.01;
                if (camera->unk88 < 1.25 && (analog_inputs[padId][0] & (1 << 6)))
                    camera->unk88 += 0.01;
            }
        }

        sp8.x = 0.0f;
        if (sub_mode == SMD_GAME_RESULT_INIT
         || sub_mode == SMD_GAME_RESULT_MAIN
         || sub_mode == SMD_GAME_RESULT_MENU)
            sp8.y = sp14.radius * (0.35 * camera->unk88);
        else
            sp8.y = sp14.radius * (0.35 * camera->unk88);
        sp8.z = sp14.radius * (0.8 * camera->unk88);

        for (i = 0; i < modeCtrl.playerCount; i++)
        {
            padId = player_pad_map[i];
            if (camera->unk94 < 256 && (analog_inputs[padId][0] & (1 << 5)))
                camera->unk94 += 8;
            if (camera->unk94 > -256 && (analog_inputs[padId][0] & (1 << 4)))
                camera->unk94 -= 8;
        }
        if (camera->timerCurr > 0)
            camera->unk74 += camera->unk94;
        mtxa_from_translate(&camera->lookAt);
        mtxa_rotate_y(camera->unk74);
        mtxa_tf_point(&sp8, &camera->eye);

        sp8.x = camera->lookAt.x - camera->eye.x;
        sp8.y = camera->lookAt.y - camera->eye.y;
        sp8.z = camera->lookAt.z - camera->eye.z;

        camera_face_direction(camera, &sp8);

        if (camera->timerCurr < 60)
            camera->timerCurr++;
    }
}

void g_camera_func52(struct Camera *camera, struct Ball *ball)
{
    Vec v;

    camera_clear(camera);

    camera->eye.x = 0.0f;
    camera->eye.y = 0.0f;
    camera->eye.z = 42.0f;

    camera->lookAt.x = 0.0f;
    camera->lookAt.y = 0.0f;
    camera->lookAt.z = 0.0f;

    v.x = camera->lookAt.x - camera->eye.x;
    v.y = camera->lookAt.y - camera->eye.y;
    v.z = camera->lookAt.z - camera->eye.z;

    camera_face_direction(camera, &v);

    camera->mode = 53;
}

void g_camera_func53(struct Camera *camera, struct Ball *ball) {}

void g_camera_func81_mainmenu(struct Camera *camera, struct Ball *ball);

/* Main menu: start from a fixed shot, then hand off to mode 81. */
void g_camera_func80(struct Camera *camera, struct Ball *ball)
{
    Vec v;

    camera_clear(camera);

    camera->eye.x = -75.424f;
    camera->eye.y = 24.188f;
    camera->eye.z = 94.232f;

    camera->lookAt.x = -66.724f;
    camera->lookAt.y = 28.305f;
    camera->lookAt.z = 81.366f;

    v.x = camera->lookAt.x - camera->eye.x;
    v.y = camera->lookAt.y - camera->eye.y;
    v.z = camera->lookAt.z - camera->eye.z;

    camera_face_direction(camera, &v);

    if (sel_menu_info.menuStackPtr > 0)
    {
        camera->timerCurr = 1;
        camera->timerMax = 0;
    }
    else
    {
        camera->timerCurr = -1;
        camera->timerMax = 60;
    }
    g_camera_func81_mainmenu(camera, ball);
    camera->mode = 81;
}

/* Idle main-menu camera, per background stage. */
struct MenuCameraParams
{
    /*0x00*/ Vec eyeBase;
    /*0x0C*/ Vec lookAtBase;
    /*0x18*/ Vec eyeOffset;  /* applied with a random sign */
    /*0x24*/ Vec lookAtOffset;  /* applied with a random sign */
    /*0x30*/ float eyeJitter;
    /*0x34*/ float lookAtJitter;
    /*0x38*/ float wobble;
    /*0x3C*/ s16 maxRotX;
    /*0x3E*/ s16 minRotX;
}; /* size = 0x40 */

static const struct MenuCameraParams menuCameraParams[] =
{
    { { 0.0f, 0.0f, -100.0f }, { 0.0f, 0.0f, -100.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, 50.0f, 30.0f, 0.016f, 0x0C00, -0x3F00 },
    { { 0.0f, 50.0f, -50.0f }, { 0.0f, 20.0f, -50.0f }, { 50.0f, 0.0f, 50.0f }, { 0.0f, 0.0f, 0.0f }, 50.0f, 30.0f, 0.02f, 0x0C00, -0x3F00 },
    { { 0.0f, 100.0f, 0.0f }, { 0.0f, 25.0f, 0.0f }, { 50.0f, 0.0f, 50.0f }, { 0.0f, 0.0f, 0.0f }, 50.0f, 50.0f, 0.08f, 0x0C00, -0x3F00 },
    { { 0.0f, 15.0f, 0.0f }, { 0.0f, 2.5f, 0.0f }, { 14.0f, 0.0f, 14.0f }, { 0.0f, 0.0f, 0.0f }, 30.0f, 10.0f, 0.02f, 0x0C00, -0x3F00 },
    { { 0.0f, 100.0f, 350.0f }, { 0.0f, 100.0f, 350.0f }, { 100.0f, 0.0f, 100.0f }, { 25.0f, 0.0f, 25.0f }, 50.0f, 50.0f, 0.1f, 0x0C00, -0x1000 },
    { { 0.0f, 18.0f, 0.0f }, { 0.0f, 11.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, 22.0f, 20.0f, 0.016f, 0x0C00, -0x1800 },
    { { 0.0f, 10.0f, -50.0f }, { 0.0f, 0.0f, -150.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, 10.0f, 120.0f, 0.016f, 0x3FFF, -0x3F00 },
    { { 0.0f, 100.0f, -750.0f }, { 0.0f, 40.0f, -750.0f }, { 0.0f, 0.0f, 0.0f }, { 50.0f, 0.0f, 50.0f }, 50.0f, 50.0f, 0.04f, 0x0C00, -0x1800 },
    { { 30.0f, 15.0f, 30.0f }, { 30.0f, 12.5f, 30.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, 10.0f, 7.5f, 0.004f, 0x0C00, -0x1800 },
    { { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, 50.0f, 45.0f, 0.02f, 0x0C00, -0x3F00 },
    { { 0.0f, 120.0f, 0.0f }, { 0.0f, 75.0f, 0.0f }, { 200.0f, 0.0f, 200.0f }, { 75.0f, 0.0f, 75.0f }, 200.0f, 150.0f, 0.1f, 0x0C00, -0x1800 },
    { { 0.0f, 2.0f, 0.0f }, { 0.0f, -5.0f, 0.0f }, { 14.0f, 0.0f, 7.0f }, { 25.0f, 0.0f, 50.0f }, 1.0f, 50.0f, 0.0035f, 0x0C00, -0x1000 },
    { { 0.0f, 6.0f, -20.0f }, { 0.0f, 0.0f, -20.0f }, { 11.0f, 0.0f, 11.0f }, { 4.0f, 0.0f, 4.0f }, 7.5f, 8.0f, 0.0055f, 0x0C00, -0x3800 },
    { { 5.0f, 3.0f, 0.0f }, { 3.0f, 2.0f, 0.0f }, { 8.0f, 0.0f, 0.0f }, { 1.0f, 0.0f, 1.0f }, 3.5f, 7.0f, 0.003f, 0x0C00, -0x2800 },
};

extern const s16 lbl_1_rodata_5F8[12];  /* menu background stage IDs (owned by another file) */
extern struct { u8 unk0[8]; s8 unk8; } g_currently_visible_menu_screen;
extern s32 g_font_char_var_3;  /* global frame counter, despite the name */
s8 fn_1_B2FD4(void);

#define RAND_WOBBLE_FREQ() (((rand() & 0x7FFF) >> 10) + 16)


void g_camera_func81_mainmenu(struct Camera *camera, struct Ball *ball)
{
    s8 paramsIdx = 0;
    const struct MenuCameraParams *params;
    int i;
    float fade;
    Vec v;

    if (g_some_other_flags & 0xA)
        return;
    if (fn_1_B2FD4() == 2 && camera->timerCurr != 1 && camera->timerCurr != -1)
        return;

    if (current_stage_id == 3)
        paramsIdx = 0;
    else if (current_stage_id == 201)
        paramsIdx = 1;
    else
    {
        for (i = 0; i < 12; i++)
        {
            if (current_stage_id == lbl_1_rodata_5F8[i])
            {
                paramsIdx = i + 2;
                break;
            }
        }
    }
    params = &menuCameraParams[paramsIdx];

    /* pick a new shot */
    if (camera->timerCurr == 1 || camera->timerCurr == -1)
    {
        camera->unk70 = RAND_WOBBLE_FREQ();
        camera->unk74 = RAND_WOBBLE_FREQ();
        camera->unk78 = RAND_WOBBLE_FREQ();
        camera->unk5C.x = params->wobble * (RAND_FLOAT() - 0.5f);
        camera->unk5C.y = params->wobble * (RAND_FLOAT() - 0.5f);
        camera->unk5C.z = params->wobble * (RAND_FLOAT() - 0.5f);
        camera->unk90 = RAND_WOBBLE_FREQ();
        camera->unk94 = RAND_WOBBLE_FREQ();
        camera->unk98 = RAND_WOBBLE_FREQ();
        camera->unk7C.x = params->wobble * (RAND_FLOAT() - 0.5f);
        camera->unk7C.y = params->wobble * (RAND_FLOAT() - 0.5f);
        camera->unk7C.z = params->wobble * (RAND_FLOAT() - 0.5f);
        camera->unk88 = 0.0f;
        if (camera->timerCurr == -1)
        {
            /* Unused, but its constant has to enter the .rodata pool here to match. */
            float unused = -1.0f;

            camera->unk5C.y = 0.0f;
            camera->unk7C.x = 0.0f;
            camera->unk7C.y = RAND_FLOAT() * -0.005f - 0.01f;
            camera->unk7C.z = 0.0f;
        }
        else
        {
            s16 prevRotX = camera->rotX;
            s16 prevRotY = camera->rotY;

            /* reroll until the pitch is in range, the view moved enough, and it isn't too close */
            do
            {
                float r;
                float sign;

#define RAND_COORD(base, jitter, offset) \
    (r = RAND_FLOAT() - 0.5f, sign = (r > 0.0) ? 1.0f : -1.0f, (base) + r * (jitter) + sign * (offset))
                camera->eye.x = RAND_COORD(params->eyeBase.x, params->eyeJitter, params->eyeOffset.x);
                camera->eye.y = RAND_COORD(params->eyeBase.y, params->eyeJitter, params->eyeOffset.y);
                camera->eye.z = RAND_COORD(params->eyeBase.z, params->eyeJitter, params->eyeOffset.z);
                camera->lookAt.x = RAND_COORD(params->lookAtBase.x, params->lookAtJitter, params->lookAtOffset.x);
                camera->lookAt.y = RAND_COORD(params->lookAtBase.y, params->lookAtJitter, params->lookAtOffset.y);
                camera->lookAt.z = RAND_COORD(params->lookAtBase.z, params->lookAtJitter, params->lookAtOffset.z);
#undef RAND_COORD

                v.x = camera->lookAt.x - camera->eye.x;
                v.y = camera->lookAt.y - camera->eye.y;
                v.z = camera->lookAt.z - camera->eye.z;
                camera_face_direction(camera, &v);
            } while (camera->rotX >= params->maxRotX
                  || camera->rotX <= params->minRotX
                  || (abs(camera->rotX - prevRotX) < 0x2000 && abs(camera->rotY - prevRotY) < 0x2000)
                  || mathutil_vec_distance(&camera->eye, &camera->lookAt) < 500.0f * params->wobble);
        }
        camera->timerCurr = 600.0f + 60.0f * (10.0f * RAND_FLOAT());
    }

    if (g_currently_visible_menu_screen.unk8 == 0 && camera->timerCurr > 0)
        camera->timerCurr--;
    if (camera->timerMax > 0)
        camera->timerMax--;

    /* wobble, faded in over the first 60 frames */
    camera->unk88 += 1.0f;
    fade = (60 - camera->timerMax) / 60.0f;
    camera->eye.x += fade * (camera->unk5C.x * math_sin(g_font_char_var_3 * camera->unk70));
    camera->eye.y += fade * (camera->unk5C.y * math_sin(g_font_char_var_3 * camera->unk74));
    camera->eye.z += fade * (camera->unk5C.z * math_sin(g_font_char_var_3 * camera->unk78));
    camera->lookAt.x += fade * (camera->unk7C.x * math_sin(camera->unk88 * camera->unk90));
    camera->lookAt.y += fade * (camera->unk7C.y * math_sin(camera->unk88 * camera->unk94));
    camera->lookAt.z += fade * (camera->unk7C.z * math_sin(camera->unk88 * camera->unk98));
    v.x = camera->lookAt.x - camera->eye.x;
    v.y = camera->lookAt.y - camera->eye.y;
    v.z = camera->lookAt.z - camera->eye.z;
    camera_face_direction(camera, &v);
    if (camera->rotX >= params->maxRotX || camera->rotX <= params->minRotX)
        camera->timerCurr = 1;
}

void g_camera_func87(struct Camera *camera, struct Ball *ball)
{
    Vec v;

    camera_clear(camera);

    camera->eye.x = 0.0f;
    camera->eye.y = 3.0f;
    camera->eye.z = 10.0f;

    camera->lookAt.x = 0.0f;
    camera->lookAt.y = 0.0f;
    camera->lookAt.z = 0.0f;

    v.x = camera->lookAt.x - camera->eye.x;
    v.y = camera->lookAt.y - camera->eye.y;
    v.z = camera->lookAt.z - camera->eye.z;

    camera_face_direction(camera, &v);

    camera->mode = 88;
    camera->unk26 = 0;
    camera->sub28.view.fov = DEGREES_TO_S16(60);
}

void g_camera_func88(struct Camera *camera, struct Ball *ball) {}

#define SMOOTHSTEP(x) ((3.0 + -2.0 * (x)) * ((x) * (x)))

void g_camera_func11_spinin(struct Camera *camera, struct Ball *ball);

/* Fly-in variants: the intro starts from one of four yaw offsets. */
struct ReadyCameraVariant
{
    float distance;  /* times the stage's bounding radius */
    float height;  /* times the stage's bounding radius */
    s32 yawOffset;
};

struct ReadyCameraVariant readyCameraVariants[] =
{
    { 0.8f, 0.4f, -0xC000 },
    { 0.8f, 0.4f, 0xC000 },
    { 0.8f, 0.4f, -0x6000 },
    { 0.8f, 0.4f, 0x6000 },
};

void g_camera_func10(struct Camera *camera, struct Ball *ball)
{
    struct Sphere sp14;
    Vec sp8;
    int variantIdx;
    struct ReadyCameraVariant *variant;

    camera_clear(camera);
    variantIdx = frame_counter & 3;
    switch (current_stage_id)
    {
    case 286:
        if (variantIdx == 2)
            variantIdx = 0;
        break;
    }
    variant = &readyCameraVariants[variantIdx];

    get_curr_stage_fly_in_position(&sp14);
    camera->unk5C.x = sp14.pos.x;
    camera->unk5C.y = sp14.pos.y;
    camera->unk5C.z = sp14.pos.z;
    camera->unk68 = sp14.radius * variant->distance;
    camera->unk6C = sp14.radius * variant->height;
    mtxa_from_translate(&camera->unk5C);
    mtxa_rotate_y(stagedef->startPos->yrot);
    sp8.x = 0.0f;
    sp8.y = camera->unk6C;
    sp8.z = camera->unk68;
    mtxa_tf_point(&sp8, &sp8);
    sp8.x = camera->unk5C.x - sp8.x;
    sp8.y = camera->unk5C.y - sp8.y;
    sp8.z = camera->unk5C.z - sp8.z;
    camera->unk74 = variant->yawOffset + (s16)(math_atan2(sp8.x, sp8.z) - 32768);
    camera->unk70 = math_atan2(sp8.y, math_sqrt(mathutil_sum_of_sq_2(sp8.x, sp8.z)));
    camera->unk78 = 0;

    camera->unk7C.x = stagedef->startPos->pos.x;
    camera->unk7C.y = stagedef->startPos->pos.y + BALL_LOOK_AT_HEIGHT();
    camera->unk7C.z = stagedef->startPos->pos.z;
    camera->unk94 = stagedef->startPos->yrot;
    camera->unk90 = BALL_CAMERA_PITCH();
    camera->unk98 = 0;
    camera->flags |= 1;
    camera->timerMax = camera->timerCurr = 90;
    camera->unk26 = 1;
    g_camera_func11_spinin(camera, ball);
    camera->mode = 11;
}

/* Stage intro: ease from the overview down to behind the ball. */
void g_camera_func11_spinin(struct Camera *camera, struct Ball *ball)
{
    if (!(g_some_other_flags & 0xA) || camera->mode != 11)
    {
        float t;
        Vec sp8;

        if (camera->timerCurr > 0)
        {
            camera->timerCurr--;
            /* Speed up the fly-in if A is held. */
            if (mode_info.attempts == 1 && (merged_digital_inputs[0] & PAD_BUTTON_A) && modeCtrl.submodeTimer > 120)
                camera->timerCurr--;
        }

        t = (float)camera->timerCurr / (float)camera->timerMax;
        t = SMOOTHSTEP(t);

        camera->lookAt.x = camera->unk7C.x * (1.0 - t) + camera->unk5C.x * t;
        camera->lookAt.y = camera->unk7C.y * (1.0 - t) + camera->unk5C.y * t;
        camera->lookAt.z = camera->unk7C.z * (1.0 - t) + camera->unk5C.z * t;

        camera->rotX = (double)camera->unk70 + (s16)(camera->unk90 - camera->unk70) * (1.0 - t);
        camera->rotY = (double)camera->unk74 + (camera->unk94 - camera->unk74) * (1.0 - t);
        camera->rotZ = (double)camera->unk78 + (s16)(camera->unk98 - camera->unk78) * (1.0 - t);

        mtxa_from_translate(&camera->lookAt);
        mtxa_rotate_y(camera->rotY);

        sp8.x = 0.0f;
        sp8.y = (1.0 - t) + camera->unk6C * t;
        sp8.z = (1.0 - t) * 3.0 + camera->unk68 * t;
        mtxa_tf_point(&sp8, &camera->eye);
    }
}

void g_camera_func38(struct Camera *camera, struct Ball *ball)
{
    g_camera_func10(camera, ball);
    camera->timerMax = camera->timerCurr = 350;
}

struct StageFlyInPosition
{
    s32 stageId;
    struct Sphere boundSphere;
};

struct StageFlyInPosition stageFlyInPositions[] =
{
    { 52, { { 0.0f, 0.0f, -110.0f }, 150.0f } },
    { 279, { { 0.0f, 0.0f, -50.0f }, 100.0f } },
    { 17, { { 0.0f, 0.0f, 0.0f }, 200.0f } },
    { 0 },
};

extern struct Sphere lbl_1_bss_711B0;  /* the stage's bounding sphere */

void get_curr_stage_fly_in_position(struct Sphere *sphere)
{
    struct StageFlyInPosition *ptr = &stageFlyInPositions[0];

    while (ptr->stageId > 0)
    {
        if (ptr->stageId == current_stage_id)
        {
            *sphere = ptr->boundSphere;
            return;
        }
        ptr++;
    }
    *sphere = lbl_1_bss_711B0;
    if (sphere->radius < 31.25)
        sphere->radius = 31.25f;
}

/* Perspective projection with an off-center shift; widescreen view modes widen the FOV. */
void camera_build_projection(Mtx44 mtx, int fov, float aspect, float nearZ, float farZ, float offsetX, float offsetY)
{
    float tanHalf;

    if (modeCtrl.wideMode)
    {
        switch (g_related_to_fov2)
        {
        default:
            lbl_1_bss_243E = fov;
            lbl_1_bss_2440 = math_tan(fov / 2);
            break;
        case 1:
            lbl_1_bss_243E = math_atan((1.0f / 320.0f) * (240.0f * math_tan(fov / 2))) * 2;
            lbl_1_bss_2440 = (1.0f / 320.0f) * (240.0f * math_tan(fov / 2));
            break;
        case 2:
            lbl_1_bss_243E = math_atan((1.0f / 320.0f) * (277.12814f * math_tan(fov / 2))) * 2;
            lbl_1_bss_2440 = (1.0f / 320.0f) * (277.12814f * math_tan(fov / 2));
            break;
        }
    }
    else
    {
        lbl_1_bss_243E = fov;
        lbl_1_bss_2440 = math_tan(fov / 2);
    }
    C_MTXPerspective(mtx, S16_TO_DEGREES(lbl_1_bss_243E), aspect, nearZ, farZ);
    tanHalf = math_tan(fov >> 1);
    if (offsetX != 0.0f)
        mtx[0][2] -= mtx[0][0] * offsetX * aspect * tanHalf;
    if (offsetY != 0.0f)
        mtx[1][2] -= mtx[1][1] * offsetY * tanHalf;
}

/* Like camera_build_projection, but takes the FOV and offsets from a camera. */
void camera_build_projection_for(Mtx44 mtx, struct Camera *camera, float nearZ, float farZ)
{
    struct CameraView view = camera->sub28.view;

    if (modeCtrl.wideMode)
    {
        switch (g_related_to_fov2)
        {
        default:
            lbl_1_bss_243E = view.fov;
            lbl_1_bss_2440 = math_tan(view.fov / 2);
            break;
        case 1:
            lbl_1_bss_243E = math_atan((1.0f / 320.0f) * (240.0f * math_tan(view.fov / 2))) * 2;
            lbl_1_bss_2440 = (1.0f / 320.0f) * (240.0f * math_tan(view.fov / 2));
            break;
        case 2:
            lbl_1_bss_243E = math_atan((1.0f / 320.0f) * (277.12814f * math_tan(view.fov / 2))) * 2;
            lbl_1_bss_2440 = (1.0f / 320.0f) * (277.12814f * math_tan(view.fov / 2));
            break;
        }
    }
    else
    {
        lbl_1_bss_243E = view.fov;
        lbl_1_bss_2440 = math_tan(view.fov / 2);
    }
    C_MTXPerspective(mtx, S16_TO_DEGREES(lbl_1_bss_243E), view.aspect, nearZ, farZ);
    if (view.unk28 != 0.0f)
        mtx[0][2] -= mtx[0][0] * view.unk28 * view.aspect * view.tanHalfFov;
    if (view.unk2C != 0.0f)
        mtx[1][2] -= mtx[1][1] * view.unk2C * view.tanHalfFov;
}

void g_camera_func0(struct Camera *camera, struct Ball *ball)
{
    Vec sp8;

    camera_clear(camera);
    camera->unk26 = 0;
    camera->flags |= CAMERA_FLAG_NO_STAGE_TILT;
    camera->lookAt.x = ball->pos.x;
    camera->lookAt.y = ball->pos.y + BALL_LOOK_AT_HEIGHT();
    camera->lookAt.z = ball->pos.z;
    mtxa_from_translate(&camera->lookAt);
    mtxa_rotate_y(stagedef->startPos->yrot);
    sp8.x = 0.0f;
    sp8.y = 1.0f;
    sp8.z = 3.0f;
    mtxa_tf_point(&sp8, &camera->eye);
    sp8.x = 0.0f;
    sp8.y = 0.0f;
    sp8.z = 3.0f;
    mtxa_tf_point(&sp8, &camera->unkB4);

    sp8.x = camera->lookAt.x - camera->eye.x;
    sp8.y = camera->lookAt.y - camera->eye.y;
    sp8.z = camera->lookAt.z - camera->eye.z;

    camera->rotY = math_atan2(sp8.x, sp8.z) - 32768;
    camera->rotX = BALL_CAMERA_PITCH();
    camera->rotZ = 0;

    camera->mode = 1;
}

/* Follow camera: trails behind the ball, yaw slowly turning toward its heading. */
void g_camera_func1(struct Camera *camera, struct Ball *ball)
{
    s16 pitch;
    s16 yaw;
    Vec sp8;
    Vec prevEye;
    Vec prevLookAt;
    float f1;
    int r3;
    int r4;

    if (g_some_other_flags & 0xA)
        return;

    prevEye.x = camera->eye.x;
    prevEye.y = camera->eye.y;
    prevEye.z = camera->eye.z;

    prevLookAt.x = camera->lookAt.x;
    prevLookAt.y = camera->lookAt.y;
    prevLookAt.z = camera->lookAt.z;

    sp8.x = camera->unkB4.x - camera->lookAt.x;
    sp8.y = camera->unkB4.y - camera->lookAt.y;
    sp8.z = camera->unkB4.z - camera->lookAt.z;

    f1 = mathutil_sum_of_sq_3(sp8.x, sp8.y, sp8.z);
    if (f1 > FLT_EPSILON)
    {
        f1 = math_rsqrt(f1);
        sp8.x *= f1;
        sp8.y *= f1;
        sp8.z *= f1;
    }
    else
    {
        sp8.x = 1.0f;
        sp8.y = 0.0f;
        sp8.z = 0.0f;
    }

    sp8.x = sp8.x * 0.75 + camera->lookAt.x;
    sp8.y = sp8.y * 0.75 + camera->lookAt.y;
    sp8.z = sp8.z * 0.75 + camera->lookAt.z;

    camera->lookAt.x = ball->pos.x;
    camera->lookAt.y = ball->pos.y + BALL_LOOK_AT_HEIGHT();
    camera->lookAt.z = ball->pos.z;

    sp8.x = camera->lookAt.x - sp8.x;
    sp8.y = camera->lookAt.y - sp8.y;
    sp8.z = camera->lookAt.z - sp8.z;

    if (ball->unk80 < 60)
        pitch = 0;
    else
        pitch = math_atan2(sp8.y, math_sqrt(mathutil_sum_of_sq_2(sp8.x, sp8.z)));

    yaw = math_atan2(sp8.x, sp8.z) - 32768;
    r3 = (s16)(yaw - camera->rotY);
    r3 = CLAMP(r3, -512, 512);
    yaw = camera->rotY + r3;
    if (!(camera->flags & (1 << 1)) && !(ball->flags & BALL_FLAG_GOAL))
    {
        r3 = (s16)(ball->unk92 - yaw);
        if (r3 > 0x800)
            r3 -= 0x800;
        else if (r3 < -0x800)
            r3 += 0x800;
        else
            r3 = 0;
        r3 >>= 7;
        r4 = camera->unk114;
        if (r3 == 0)
            r4 = 0;
        else if ((r4 < 0 && r3 > 0) || (r4 > 0 && r3 < 0))
            r4 = 0;
        else if (r3 < 0)
        {
            if (r3 < r4 - 4)
                r4 -= 4;
            else
                r4 = r3;
        }
        else
        {
            if (r3 > r4 + 4)
                r4 += 4;
            else
                r4 = r3;
        }

        yaw += r4;
        r3 = (s16)(yaw - camera->rotY);
        r3 = CLAMP(r3, -768, 768);
        yaw = camera->rotY + r3;
    }

    if (pitch < -6144)
        pitch = -6144;
    else if (pitch > 6144)
        pitch = 6144;
    pitch = camera->unkC0 = camera->unkC0 + 0.2 * (pitch - camera->unkC0);

    mtxa_from_translate(&camera->lookAt);
    mtxa_rotate_y(yaw);
    mtxa_rotate_x(pitch);
    sp8.x = 0.0f;
    sp8.y = 0.0f;
    sp8.z = 3.0f;
    mtxa_tf_point(&sp8, &camera->unkB4);
    camera->unk114 = yaw - camera->rotY;
    camera->rotY = yaw;
    camera->rotX = pitch + BALL_CAMERA_PITCH();
    mtxa_from_translate(&camera->lookAt);
    mtxa_rotate_y(camera->rotY);
    mtxa_rotate_x(camera->rotX);
    sp8.x = 0.0f;
    sp8.y = 0.0f;
    sp8.z = math_sqrt(mathutil_sum_of_sq_2(3.0f, 1.0f));
    mtxa_tf_point(&sp8, &camera->eye);

    if (camera->rotZ != 0)
        camera->rotZ = 0.95f * camera->rotZ;

    camera->eyeVel.x = camera->eye.x - prevEye.x;
    camera->eyeVel.y = camera->eye.y - prevEye.y;
    camera->eyeVel.z = camera->eye.z - prevEye.z;

    camera->lookAtVel.x = camera->lookAt.x - prevLookAt.x;
    camera->lookAtVel.y = camera->lookAt.y - prevLookAt.y;
    camera->lookAtVel.z = camera->lookAt.z - prevLookAt.z;
}

/* Debug free-fly camera, driven by whichever controller is being touched. */
void g_camera_func2(struct Camera *camera, struct Ball *ball)
{
    Vec move;
    Vec lookAtMove;
    Vec sp8;
    float moveSpeed;
    float cappedSpeed;  /* speed capped for vertical/forward moves */
    s16 pad;

    if ((dip_switches & DIP_1P_TEST_CAM) && camera->idx != 0)
    {
        camera->eye = cameras[0].eye;
        camera->lookAt = cameras[0].lookAt;
        camera->rotX = cameras[0].rotX;
        camera->rotY = cameras[0].rotY;
        camera->rotZ = cameras[0].rotZ;
        return;
    }

    for (pad = 0; pad < 4; pad++)
    {
        if (pad_status_groups[pad].unk0[0].err != 0)
            continue;
        if (pad_status_groups[pad].unk0[0].stickX != 0
         || pad_status_groups[pad].unk0[0].stickY != 0
         || pad_status_groups[pad].unk0[0].substickX != 0
         || pad_status_groups[pad].unk0[0].substickY != 0
         || pad_status_groups[pad].unk0[0].triggerLeft != 0
         || (analog_inputs[pad][0] & (1 << 9)))
            break;
    }
    if (pad == 4)
        pad = 0;

    sp8.x = camera->lookAt.x - camera->eye.x;
    sp8.y = camera->lookAt.y - camera->eye.y;
    sp8.z = camera->lookAt.z - camera->eye.z;

    /* Must fold to 0.04000000000000001 like the original (0.2 / 5.0 folds to 0.04). */
    moveSpeed = 0.1 * 0.4 * math_sqrt(mathutil_sum_of_sq_3(sp8.x, sp8.y, sp8.z));
    moveSpeed *= 1.0 + 4.0 * pad_status_groups[pad].unk0[0].triggerLeft / 170.0;
    cappedSpeed = (moveSpeed > 50.0) ? 50.0 : moveSpeed;

    move.x = 0.0f;
    move.y = 0.0f;
    move.z = 0.0f;
    lookAtMove.x = 0.0f;
    lookAtMove.y = 0.0f;
    lookAtMove.z = 0.0f;

    move.x = moveSpeed * pad_status_groups[pad].unk0[0].stickX / 74.0;
    if (analog_inputs[pad][0] & (1 << 9))
        move.y = cappedSpeed * pad_status_groups[pad].unk0[0].stickY / 74.0;
    else
        move.z = -cappedSpeed * pad_status_groups[pad].unk0[0].stickY / 74.0;

    lookAtMove.x = moveSpeed * pad_status_groups[pad].unk0[0].substickX / 74.0;
    if (analog_inputs[pad][0] & (1 << 9))
        lookAtMove.y = cappedSpeed * pad_status_groups[pad].unk0[0].substickY / 74.0;
    if (!(analog_inputs[pad][0] & (1 << 9)))
        lookAtMove.z = -cappedSpeed * pad_status_groups[pad].unk0[0].substickY / 74.0;

    mtxa_from_translate(&camera->eye);
    mtxa_rotate_y(camera->rotY);
    mtxa_rotate_x(camera->rotX);
    mtxa_rotate_z(camera->rotZ);
    mtxa_tf_point(&move, &camera->eye);
    mtxa_rigid_inv_tf_point(&camera->lookAt, &sp8);

    sp8.x += lookAtMove.x;
    sp8.y += lookAtMove.y;
    sp8.z += lookAtMove.z;
    mtxa_tf_point(&sp8, &camera->lookAt);

    sp8.x = camera->lookAt.x - camera->eye.x;
    sp8.y = camera->lookAt.y - camera->eye.y;
    sp8.z = camera->lookAt.z - camera->eye.z;

    {
        register float x;
#ifdef __MWERKS__
        register float y;
        register float z;

        z = sp8.z;
        y = sp8.y;
        x = sp8.x;
        asm
        {
            fmuls x, x, x
            fmadds x, y, y, x
            fmadds x, z, z, x
        }
#else
        x = mathutil_sum_of_sq_3(sp8.x, sp8.y, sp8.z);
#endif
        if (x < 0.09)
        {
            float minDistanceScale = 0.3 / math_sqrt(x);

            camera->lookAt.x = camera->eye.x + sp8.x * minDistanceScale;
            camera->lookAt.y = camera->eye.y + sp8.y * minDistanceScale;
            camera->lookAt.z = camera->eye.z + sp8.z * minDistanceScale;
        }
    }

    sp8.x = camera->lookAt.x - camera->eye.x;
    sp8.y = camera->lookAt.y - camera->eye.y;
    sp8.z = camera->lookAt.z - camera->eye.z;

    camera_face_direction(camera, &sp8);
}

void g_camera_func3_setup(struct Camera *camera, struct Ball *ball)
{
    Vec v;

    v.x = camera->lookAt.x - camera->eye.x;
    v.y = camera->lookAt.y - camera->eye.y;
    v.z = camera->lookAt.z - camera->eye.z;

    camera_face_direction(camera, &v);
}

void g_camera_func62(struct Camera *camera, struct Ball *ball)
{
    Vec v;

    v.x = camera->lookAt.x - camera->eye.x;
    v.y = camera->lookAt.y - camera->eye.y;
    v.z = camera->lookAt.z - camera->eye.z;

    camera->rotY = math_atan2(v.x, v.z) - 32768;
    camera->rotX = math_atan2(v.y, math_sqrt(mathutil_sum_of_sq_2(v.x, v.z)));
    camera->rotZ = g_current_camera->rotZ;
}

/* Fall-out: drift on with damped velocity while watching the ball. */
void g_camera_func4_fallout(struct Camera *camera, struct Ball *ball)
{
    Vec sp10;

    camera->unk26 = 2;

    if (g_some_other_flags & 0xA)
        return;

    camera->eyeVel.x *= 0.97;
    camera->eyeVel.y *= 0.955;
    camera->eyeVel.z *= 0.97;

    camera->eye.x += camera->eyeVel.x;
    camera->eye.y += camera->eyeVel.y;
    camera->eye.z += camera->eyeVel.z;

    camera->lookAt.x = ball->pos.x;
    camera->lookAt.y = ball->pos.y;
    camera->lookAt.z = ball->pos.z;

    sp10.x = camera->lookAt.x - camera->eye.x;
    sp10.y = camera->lookAt.y - camera->eye.y;
    sp10.z = camera->lookAt.z - camera->eye.z;

    camera_face_direction(camera, &sp10);
}

void g_camera_func8(struct Camera *camera, struct Ball *ball);

/* Fall-out replay, variant A: start near the ball at a random time in the recording. */
void g_camera_func5(struct Camera *camera, struct Ball *ball)
{
    Vec sp30;
    struct ReplayBallFrame sp10;
    float f31;
    float f1;

    if (g_some_other_flags & 0xA)
        return;

    camera_clear(camera);

    camera->mode = 8;
    camera->unk26 = 2;
    camera->flags |= CAMERA_FLAG_UNK4;

    sp30.x = 3.0f;
    sp30.y = 3.0f;
    sp30.z = 3.0f;

    mtxa_from_identity();
    mtxa_rotate_y(rand() & 0x7FFF);
    mtxa_tf_vec(&sp30, &sp30);

    g_get_replay_frame_data(lbl_1_bss_71EC8.unk0[ball->playerId], (0.75 + 0.25 * RAND_FLOAT()) * lbl_1_bss_71EC8.unk10, &sp10);

    camera->eye.x = sp10.pos.x + sp30.x;
    camera->eye.y = sp10.pos.y + sp30.y;
    camera->eye.z = sp10.pos.z + sp30.z;

    g_get_replay_frame_data(lbl_1_bss_71EC8.unk0[ball->playerId], 0.0f, &sp10);

    sp30.x = camera->eye.x - sp10.pos.x;
    sp30.y = camera->eye.y - sp10.pos.y;
    sp30.z = camera->eye.z - sp10.pos.z;

    f1 = 1.0f / mathutil_vec_len(&sp30);
    if (f1 * sp30.y > 0.96592582630000001)
    {
        f31 = sp30.y;
        sp30.y = 0.0f;
        vec_set_len(0.25881904509999998 * f31, &sp30, &sp30);
        camera->eye.x = sp10.pos.x + sp30.x;
        camera->eye.z = sp10.pos.z + sp30.z;
    }

    camera->eyeVel.x *= 0.25;
    camera->eyeVel.y *= 0.25;
    camera->eyeVel.z *= 0.25;

    f31 = (lbl_1_bss_71EC8.unk10 < 60.0f) ? lbl_1_bss_71EC8.unk10 : 60.0f;

    g_get_replay_frame_data(lbl_1_bss_71EC8.unk0[ball->playerId], lbl_1_bss_71EC8.unk10 - f31, &sp10);

    f31 = 1.0 / f31;

    camera->eyeVel.x += f31 * (sp10.pos.x - ball->pos.x);
    camera->eyeVel.y += f31 * (sp10.pos.y - ball->pos.y);
    camera->eyeVel.z += f31 * (sp10.pos.z - ball->pos.z);

    camera->lookAt.x = ball->pos.x;
    camera->lookAt.y = ball->pos.y;
    camera->lookAt.z = ball->pos.z;

    camera->flags |= 1;

    g_camera_func8(camera, ball);
}

void g_camera_func6(struct Camera *camera, struct Ball *ball) {}

/* Fall-out replay, variant B: start early in the recording, moving with the ball. */
void g_camera_func7(struct Camera *camera, struct Ball *ball)
{
    Vec sp50;
    struct ReplayBallFrame sp30;
    struct ReplayBallFrame sp10;
    float f31;
    float f1;
    float f3_;

    if (g_some_other_flags & 0xA)
        return;

    camera_clear(camera);

    camera->mode = 8;
    camera->unk26 = 2;
    camera->flags |= CAMERA_FLAG_UNK4;

    sp50.x = 1.0f;
    sp50.y = 1.0f;
    sp50.z = 1.0f;

    mtxa_from_identity();
    mtxa_rotate_y(rand() & 0x7FFF);
    mtxa_tf_vec(&sp50, &sp50);

    f31 = (0.25 * RAND_FLOAT()) * lbl_1_bss_71EC8.unk10;

    g_get_replay_frame_data(lbl_1_bss_71EC8.unk0[ball->playerId], f31, &sp30);

    camera->eye.x = sp30.pos.x + sp50.x;
    camera->eye.y = sp30.pos.y + sp50.y;
    camera->eye.z = sp30.pos.z + sp50.z;

    g_get_replay_frame_data(lbl_1_bss_71EC8.unk0[ball->playerId], 0.0f, &sp30);

    sp50.x = camera->eye.x - sp30.pos.x;
    sp50.y = camera->eye.y - sp30.pos.y;
    sp50.z = camera->eye.z - sp30.pos.z;

    f1 = 1.0f / mathutil_vec_len(&sp50);
    if (f1 * sp50.y > 0.96592582630000001)
    {
        f3_ = sp50.y;
        sp50.y = 0.0f;
        vec_set_len(0.25881904509999998 * f3_, &sp50, &sp50);
        camera->eye.x = sp30.pos.x + sp50.x;
        camera->eye.z = sp30.pos.z + sp50.z;
    }

    camera->eyeVel.x *= 0.1;
    camera->eyeVel.y *= 0.1;
    camera->eyeVel.z *= 0.1;

    g_get_replay_frame_data(lbl_1_bss_71EC8.unk0[ball->playerId], f31, &sp30);
    g_get_replay_frame_data(lbl_1_bss_71EC8.unk0[ball->playerId], 1.0 + f31, &sp10);

    camera->eyeVel.x -= 0.6 * (sp30.pos.x - sp10.pos.x);
    camera->eyeVel.y -= 0.6 * (sp30.pos.y - sp10.pos.y);
    camera->eyeVel.z -= 0.6 * (sp30.pos.z - sp10.pos.z);

    camera->lookAt.x = ball->pos.x;
    camera->lookAt.y = ball->pos.y;
    camera->lookAt.z = ball->pos.z;

    camera->flags |= 1;

    g_camera_func8(camera, ball);
}

/* Fall-out replay: drift with damped velocity, look-at easing toward the ball. */
void g_camera_func8(struct Camera *camera, struct Ball *ball)
{
    Vec sp10;

    if (g_some_other_flags & 0xA)
        return;

    camera->eyeVel.x *= 0.97;
    camera->eyeVel.y *= 0.955;
    camera->eyeVel.z *= 0.97;

    camera->eye.x += camera->eyeVel.x;
    camera->eye.y += camera->eyeVel.y;
    camera->eye.z += camera->eyeVel.z;

    sp10.x = ball->pos.x - camera->lookAt.x;
    sp10.y = ball->pos.y - camera->lookAt.y;
    sp10.z = ball->pos.z - camera->lookAt.z;

    camera->lookAtVel.x = 0.15 * sp10.x;
    camera->lookAtVel.y = 0.15 * sp10.y;
    camera->lookAtVel.z = 0.15 * sp10.z;

    camera->lookAt.x += camera->lookAtVel.x;
    camera->lookAt.y += camera->lookAtVel.y;
    camera->lookAt.z += camera->lookAtVel.z;

    sp10.x = camera->lookAt.x - camera->eye.x;
    sp10.y = camera->lookAt.y - camera->eye.y;
    sp10.z = camera->lookAt.z - camera->eye.z;

    camera_face_direction(camera, &sp10);

    if (camera->flags & (1 << 8))
        camera->mode = 5;
}

void g_camera_func9(struct Camera *camera, struct Ball *ball) {}

void g_camera_func15_goal(struct Camera *camera, struct Ball *ball);

void g_camera_func14(struct Camera *camera, struct Ball *ball)
{
    camera->mode = 15;
    camera->unk26 = 3;
    g_camera_func15_goal(camera, ball);
}

/* Goal: coast past the goal, circling back toward the ball. */
void g_camera_func15_goal(struct Camera *camera, struct Ball *ball)
{
    Vec sp8;
    float f31;
    float f2;
    float f1;

    if (g_some_other_flags & 0xA)
        return;

    sp8.x = ball->pos.x - camera->eye.x;
    sp8.y = 0.0f;
    sp8.z = ball->pos.z - camera->eye.z;

    f31 = mathutil_vec_dot_prod(&sp8, &camera->eyeVel);
    vec_normalize_len(&sp8);

    camera->eyeVel.y *= 0.97;
    f2 = -0.01 * f31;
    camera->eyeVel.x += f2 * sp8.x;
    camera->eyeVel.z += f2 * sp8.z;
    if (f2 < 0.0)
    {
        f2 *= 0.5;
        camera->eyeVel.x += f2 * sp8.z;
        camera->eyeVel.z += f2 * sp8.x;
    }

    camera->eye.x += camera->eyeVel.x;
    camera->eye.y += camera->eyeVel.y;
    camera->eye.z += camera->eyeVel.z;

    camera->lookAtVel.x = 0.3 * (ball->pos.x - camera->lookAt.x);
    camera->lookAtVel.y = 0.3 * (ball->pos.y - camera->lookAt.y);
    camera->lookAtVel.z = 0.3 * (ball->pos.z - camera->lookAt.z);

    camera->lookAt.x += camera->lookAtVel.x;
    camera->lookAt.y += camera->lookAtVel.y;
    camera->lookAt.z += camera->lookAtVel.z;

    sp8.x = camera->eye.x - camera->lookAt.x;
    sp8.y = 0.0f;
    sp8.z = camera->eye.z - camera->lookAt.z;

    f1 = math_sqrt(mathutil_sum_of_sq_2(sp8.x, sp8.z));
    if (f1 > FLT_EPSILON)
    {
        float f3 = (f1 + 0.08 * (2.0 - f1)) / f1;

        camera->eye.x = sp8.x * f3 + camera->lookAt.x;
        camera->eye.z = sp8.z * f3 + camera->lookAt.z;
        if (!(ball->flags & BALL_FLAG_09))
        {
            sp8.y = camera->lookAt.y - camera->eye.y;
            camera->eye.y += 0.01 * sp8.y;
        }
    }

    sp8.x = camera->lookAt.x - camera->eye.x;
    sp8.y = camera->lookAt.y - camera->eye.y;
    sp8.z = camera->lookAt.z - camera->eye.z;

    camera_face_direction(camera, &sp8);
}

void g_camera_func17(struct Camera *camera, struct Ball *ball);

/* Pick a goal-replay shot. */
void g_camera_func16(struct Camera *camera, struct Ball *ball)
{
    struct StageGoal goal;
    struct ReplayBallFrame sp44;
    Vec sp38;
    Vec sp2C;
    Vec sp20;
    Vec sp14;
    Vec sp8;
    float f31;

    if (g_some_other_flags & 0xA)
        return;

    camera_clear(camera);

    camera->unk26 = 4;
    camera->flags |= CAMERA_FLAG_UNK4;

    if ((pad_status_groups[player_pad_map[ball->playerId]].unk0[0].button & PAD_BUTTON_A)
     && (merged_digital_inputs[0] & PAD_BUTTON_A))
    {
        camera->mode = 48;
        camera_funcs[camera->mode](camera, ball);
        return;
    }

    g_get_replay_frame_data(lbl_1_bss_71EC8.unk0[ball->playerId], 0.0f, &sp44);
    goal = stagedef->goals[mode_info.goalEntered];
    if (mode_info.goalItemGroup > 0)
    {
        mtxa_from_mtx(itemgroups[mode_info.goalItemGroup].transform);
        mtxa_tf_point(&goal.pos, &goal.pos);
    }
    if (mathutil_vec_distance(&goal.pos, &ball->pos) < 16.0)
    {
        camera->mode = 17;
        mtxa_from_translate(&goal.pos);
        mtxa_rotate_z(goal.rotZ);
        mtxa_rotate_y(goal.rotY);
        mtxa_rotate_x(goal.rotX);
        f31 = mathutil_vec_len(&mode_info.unk10);
        if (mathutil_vec_distance(&goal.pos, &sp44.pos) > 2.5 && (rand() & 3) && sp44.pos.y > goal.pos.y)
        {
            double zero;

            camera->eyeVel.x *= 0.02;
            camera->eyeVel.y *= 0.02;
            camera->eyeVel.z *= 0.02;

            sp20.x = 0.0f;
            sp20.y = 2.0f;
            sp20.z = 0.0f;
            mtxa_tf_point(&sp20, &sp20);

            sp38.x = sp44.pos.x - sp20.x;
            sp38.y = sp44.pos.y - sp20.y;
            sp38.z = sp44.pos.z - sp20.z;

            zero = 0.0;
            if (fabs(sp38.y) != zero)
            {
                float f1 = 0.35 / fabs(sp38.y);
                float f2;

                sp38.x *= f1;
                sp38.y *= f1;
                sp38.z *= f1;
                f2 = mathutil_vec_sq_len(&sp38);
                if (f2 < 1.0)
                    vec_set_len(1.0f, &sp38, &sp38);
                else if (f2 > 4.0)
                    vec_set_len(2.0f, &sp38, &sp38);
            }
            else
                vec_set_len(1.0 + RAND_FLOAT(), &sp38, &sp38);

            camera->eye.x = sp44.pos.x + sp38.x;
            camera->eye.y = sp44.pos.y + sp38.y;
            camera->eye.z = sp44.pos.z + sp38.z;
        }
        else
        {
            if (f31 > 0.25 && (rand() & 3))
            {
                struct StageGoal *goalp;

                camera->mode = 20;
                camera->unk5C.x = 2.0 + RAND_FLOAT();
                camera->unk5C.y = 1.0 + 5.0 * RAND_FLOAT();
                camera->unk5C.z = 4.0 * (RAND_FLOAT() - 0.5);
                if (rand() & 1)
                    camera->unk5C.x = -camera->unk5C.x;
                camera->unk68 = camera->unk5C.y + 0.5 * (0.5 + RAND_FLOAT());
                goalp = &stagedef->goals[mode_info.goalEntered];
                mtxa_from_mtx(itemgroups[mode_info.goalItemGroup].transform);
                mtxa_translate(&goalp->pos);
                mtxa_rotate_z(goalp->rotZ);
                mtxa_rotate_y(goalp->rotY);
                mtxa_rotate_x(goalp->rotX);
                mtxa_tf_point(&camera->unk5C, &sp8);
                sp14.x = sp8.x - ball->pos.x;
                sp14.y = sp8.y - ball->pos.y;
                sp14.z = sp8.z - ball->pos.z;
                vec_set_len(camera->unk68, &sp14, &sp14);
                camera->eye.x = sp8.x + sp14.x;
                camera->eye.y = sp8.y + sp14.y;
                camera->eye.z = sp8.z + sp14.z;
            }
            else
            {
                camera->eyeVel.x *= 0.02;
                camera->eyeVel.y = 0.0f;
                camera->eyeVel.z *= 0.02;
                sp38.x = 9.0 * (f31 * (RAND_FLOAT() - 0.5));
                sp38.y = 2.5f;
                sp38.z = 5.0f;
                mtxa_rigid_inv_tf_vec(&mode_info.unk10, &sp2C);
                if (sp2C.z > 0.0)
                    sp38.z = -sp38.z;
                mtxa_tf_point(&sp38, &camera->eye);
                if (mode_info.goalItemGroup > 0)
                    camera->mode = 19;
            }
        }
    }
    else
    {
        camera->mode = 18;
        sp38.x = ball->pos.x - goal.pos.x;
        sp38.y = ball->pos.y - goal.pos.y;
        sp38.z = ball->pos.z - goal.pos.z;
        if (sp38.x == 0.0 && sp38.y == 0.0 && sp38.z == 0.0)
            sp38.z = 1.0f;
        vec_set_len(1.0f, &sp38, &sp38);
        camera->eye.x = ball->pos.x + sp38.x;
        camera->eye.y = ball->pos.y + sp38.y;
        camera->eye.z = ball->pos.z + sp38.z;
    }
    camera->lookAt.x = ball->pos.x;
    camera->lookAt.y = ball->pos.y;
    camera->lookAt.z = ball->pos.z;
    camera->flags |= 1;
    g_camera_func17(camera, ball);
}

/* Goal replay: coast, look-at easing toward the ball. */
void g_camera_func17(struct Camera *camera, struct Ball *ball)
{
    Vec sp10;

    if (g_some_other_flags & 0xA)
        return;

    camera->eyeVel.x *= 0.98;
    camera->eyeVel.y *= 0.98;
    camera->eyeVel.z *= 0.98;

    camera->eye.x += camera->eyeVel.x;
    camera->eye.y += camera->eyeVel.y;
    camera->eye.z += camera->eyeVel.z;

    sp10.x = ball->pos.x - camera->lookAt.x;
    sp10.y = ball->pos.y - camera->lookAt.y;
    sp10.z = ball->pos.z - camera->lookAt.z;

    camera->lookAtVel.x = sp10.x * 0.15;
    camera->lookAtVel.y = sp10.y * 0.15;
    camera->lookAtVel.z = sp10.z * 0.15;

    camera->lookAt.x += camera->lookAtVel.x;
    camera->lookAt.y += camera->lookAtVel.y;
    camera->lookAt.z += camera->lookAtVel.z;

    sp10.x = camera->lookAt.x - camera->eye.x;
    sp10.y = camera->lookAt.y - camera->eye.y;
    sp10.z = camera->lookAt.z - camera->eye.z;

    camera_face_direction(camera, &sp10);
}

/* Goal replay: chase the ball until it's close to the goal, then pick a new shot. */
void g_camera_func18(struct Camera *camera, struct Ball *ball)
{
    Vec sp1C;
    Vec sp10;
    struct StageGoal *goal;

    if (g_some_other_flags & 0xA)
        return;

    goal = &stagedef->goals[mode_info.goalEntered];
    sp1C = goal->pos;

    if (mode_info.goalItemGroup > 0)
    {
        mtxa_from_mtx(itemgroups[mode_info.goalItemGroup].transform);
        mtxa_tf_point(&sp1C, &sp1C);
    }

    sp1C.x = ball->pos.x - sp1C.x;
    sp1C.y = ball->pos.y - sp1C.y;
    sp1C.z = ball->pos.z - sp1C.z;
    if (vec_normalize_len(&sp1C) < 12.0)
        camera->mode = 16;

    sp10.x = (ball->vel.x * 4.0) + (ball->pos.x + sp1C.x * 2.0);
    sp10.y = (ball->vel.y * 4.0) + (ball->pos.y + sp1C.y * 2.0) + 2.0;
    sp10.z = (ball->vel.z * 4.0) + (ball->pos.z + sp1C.z * 2.0);

    camera->eyeVel.x = 0.04 * (sp10.x - camera->eye.x);
    camera->eyeVel.y = 0.04 * (sp10.y - camera->eye.y);
    camera->eyeVel.z = 0.04 * (sp10.z - camera->eye.z);

    camera->eye.x += camera->eyeVel.x;
    camera->eye.y += camera->eyeVel.y;
    camera->eye.z += camera->eyeVel.z;

    sp1C.x = ball->pos.x - camera->lookAt.x;
    sp1C.y = ball->pos.y - camera->lookAt.y;
    sp1C.z = ball->pos.z - camera->lookAt.z;

    camera->lookAtVel.x = sp1C.x * 0.15;
    camera->lookAtVel.y = sp1C.y * 0.15;
    camera->lookAtVel.z = sp1C.z * 0.15;

    camera->lookAt.x += camera->lookAtVel.x;
    camera->lookAt.y += camera->lookAtVel.y;
    camera->lookAt.z += camera->lookAtVel.z;

    sp1C.x = camera->lookAt.x - camera->eye.x;
    sp1C.y = camera->lookAt.y - camera->eye.y;
    sp1C.z = camera->lookAt.z - camera->eye.z;

    camera_face_direction(camera, &sp1C);
}

/* Goal replay on a moving goal: hold a point behind the goal along the ball's path. */
void g_camera_func19(struct Camera *camera, struct Ball *ball)
{
    Vec sp28;
    Vec sp1C;
    Vec sp10;
    struct StageGoal *goal;

    if (g_some_other_flags & 0xA)
        return;

    goal = &stagedef->goals[mode_info.goalEntered];
    sp10 = goal->pos;

    if (mode_info.goalItemGroup > 0)
    {
        mtxa_from_mtx(itemgroups[mode_info.goalItemGroup].transform);
        mtxa_tf_point(&sp10, &sp10);
    }

    sp28.x = mode_info.unk10.x;
    sp28.y = mode_info.unk10.y * 0.05;
    sp28.z = mode_info.unk10.z;
    vec_set_len(-4.0f, &sp28, &sp28);

    sp28.y += 2.5;
    sp1C.x = sp10.x + sp28.x;
    sp1C.y = sp10.y + sp28.y;
    sp1C.z = sp10.z + sp28.z;

    camera->eyeVel.x = 0.04 * (sp1C.x - camera->eye.x);
    camera->eyeVel.y = 0.04 * (sp1C.y - camera->eye.y);
    camera->eyeVel.z = 0.04 * (sp1C.z - camera->eye.z);

    camera->eye.x += camera->eyeVel.x;
    camera->eye.y += camera->eyeVel.y;
    camera->eye.z += camera->eyeVel.z;

    sp28.x = ball->pos.x - camera->lookAt.x;
    sp28.y = ball->pos.y - camera->lookAt.y;
    sp28.z = ball->pos.z - camera->lookAt.z;

    camera->lookAtVel.x = sp28.x * 0.15;
    camera->lookAtVel.y = sp28.y * 0.15;
    camera->lookAtVel.z = sp28.z * 0.15;

    camera->lookAt.x += camera->lookAtVel.x;
    camera->lookAt.y += camera->lookAtVel.y;
    camera->lookAt.z += camera->lookAtVel.z;

    sp28.x = camera->lookAt.x - camera->eye.x;
    sp28.y = camera->lookAt.y - camera->eye.y;
    sp28.z = camera->lookAt.z - camera->eye.z;

    camera_face_direction(camera, &sp28);
}

/* Goal replay: orbit a random point near the goal (set up by mode 16). */
void g_camera_func20(struct Camera *camera, struct Ball *ball)
{
    Vec sp28;
    Vec sp1C;
    Vec sp10;
    struct StageGoal *goal;

    if (g_some_other_flags & 0xA)
        return;

    goal = &stagedef->goals[mode_info.goalEntered];
    mtxa_from_mtx(itemgroups[mode_info.goalItemGroup].transform);
    mtxa_translate(&goal->pos);
    mtxa_rotate_z(goal->rotZ);
    mtxa_rotate_y(goal->rotY);
    mtxa_rotate_x(goal->rotX);
    mtxa_tf_point(&camera->unk5C, &sp10);

    sp28.x = sp10.x - ball->pos.x;
    sp28.y = 0.0f;
    sp28.z = sp10.z - ball->pos.z;
    vec_set_len(camera->unk68, &sp28, &sp28);

    sp1C.x = sp10.x + sp28.x;
    sp1C.y = sp10.y + sp28.y;
    sp1C.z = sp10.z + sp28.z;

    camera->eyeVel.x = 0.02 * (sp1C.x - camera->eye.x);
    camera->eyeVel.y = 0.02 * (sp1C.y - camera->eye.y);
    camera->eyeVel.z = 0.02 * (sp1C.z - camera->eye.z);

    camera->eye.x += camera->eyeVel.x;
    camera->eye.y += camera->eyeVel.y;
    camera->eye.z += camera->eyeVel.z;

    sp28.x = ball->pos.x - camera->lookAt.x;
    sp28.y = ball->pos.y - camera->lookAt.y;
    sp28.z = ball->pos.z - camera->lookAt.z;

    camera->lookAtVel.x = sp28.x * 0.15;
    camera->lookAtVel.y = sp28.y * 0.15;
    camera->lookAtVel.z = sp28.z * 0.15;

    camera->lookAt.x += camera->lookAtVel.x;
    camera->lookAt.y += camera->lookAtVel.y;
    camera->lookAt.z += camera->lookAtVel.z;

    sp28.x = camera->lookAt.x - camera->eye.x;
    sp28.y = camera->lookAt.y - camera->eye.y;
    sp28.z = camera->lookAt.z - camera->eye.z;

    camera_face_direction(camera, &sp28);
}

void g_camera_func22_23_24_timeover(struct Camera *camera, struct Ball *ball);

void g_camera_func21(struct Camera *camera, struct Ball *ball)
{
    camera->mode = 22;
    g_camera_func22_23_24_timeover(camera, ball);
}

/* Time over: drift to a stop, look-at lagging behind the ball. */
void g_camera_func22_23_24_timeover(struct Camera *camera, struct Ball *ball)
{
    Vec sp10;

    if (g_some_other_flags & 0xA)
        return;

    camera->eyeVel.x *= 0.92;
    camera->eyeVel.y *= 0.92;
    camera->eyeVel.z *= 0.92;

    camera->eye.x += camera->eyeVel.x;
    camera->eye.y += camera->eyeVel.y;
    camera->eye.z += camera->eyeVel.z;

    sp10.x = ball->pos.x - camera->lookAt.x;
    sp10.y = ball->pos.y - camera->lookAt.y;
    sp10.z = ball->pos.z - camera->lookAt.z;

    camera->lookAtVel.x = (sp10.x - camera->lookAtVel.x) * 0.1;
    camera->lookAtVel.y = (sp10.y - camera->lookAtVel.y) * 0.1;
    camera->lookAtVel.z = (sp10.z - camera->lookAtVel.z) * 0.1;

    camera->lookAt.x += camera->lookAtVel.x;
    camera->lookAt.y += camera->lookAtVel.y;
    camera->lookAt.z += camera->lookAtVel.z;

    sp10.x = camera->lookAt.x - camera->eye.x;
    sp10.y = camera->lookAt.y - camera->eye.y;
    sp10.z = camera->lookAt.z - camera->eye.z;

    camera_face_direction(camera, &sp10);
}

/* Modes 25/26 moved to the main_game module in SMB2. */
void fn_2_7BD0(struct Camera *camera, struct Ball *ball);
void fn_2_7C30(struct Camera *camera, struct Ball *ball);

void g_camera_func25(struct Camera *camera, struct Ball *ball)
{
    fn_2_7BD0(camera, ball);
}

void g_camera_func26(struct Camera *camera, struct Ball *ball)
{
    fn_2_7C30(camera, ball);
}

void g_camera_func31(struct Camera *camera, struct Ball *ball)
{
    Vec sp10;

    camera_clear(camera);

    camera->lookAt.x = ball->pos.x;
    camera->lookAt.y = ball->pos.y;
    camera->lookAt.z = ball->pos.z;

    camera->eye.x = stagedef->startPos->pos.z;
    camera->eye.y = stagedef->startPos->pos.y;
    camera->eye.z = stagedef->startPos->pos.x;

    sp10.x = camera->lookAt.x - camera->eye.x;
    sp10.y = camera->lookAt.y - camera->eye.y;
    sp10.z = camera->lookAt.z - camera->eye.z;

    camera_face_direction(camera, &sp10);

    camera->mode = 32;
}

void g_camera_func32(struct Camera *camera, struct Ball *ball)
{
    Vec sp28;
    Vec sp1C;
    Vec sp10;

    if (g_some_other_flags & 0xA)
        return;

    sp1C = camera->eye;  /* unused */
    sp10 = camera->lookAt;

    camera->lookAt.x += (ball->pos.x - camera->lookAt.x) * 0.25;
    camera->lookAt.y += (ball->pos.y - camera->lookAt.y) * 0.25;
    camera->lookAt.z += (ball->pos.z - camera->lookAt.z) * 0.25;

    camera->eyeVel.x = 0.0f;
    camera->eyeVel.y = 0.0f;
    camera->eyeVel.z = 0.0f;

    camera->eye.x = stagedef->startPos->pos.z;
    camera->eye.y = stagedef->startPos->pos.y;
    camera->eye.z = stagedef->startPos->pos.x;

    sp28.x = camera->lookAt.x - camera->eye.x;
    sp28.y = camera->lookAt.y - camera->eye.y;
    sp28.z = camera->lookAt.z - camera->eye.z;

    camera_face_direction(camera, &sp28);

    camera->lookAtVel.x = camera->lookAt.x - sp10.x;
    camera->lookAtVel.y = camera->lookAt.y - sp10.y;
    camera->lookAtVel.z = camera->lookAt.z - sp10.z;
}

void g_camera_func33(struct Camera *camera, struct Ball *ball)
{
    Vec sp10;
    int r31;
    float f1;

    camera_clear(camera);

    camera->lookAt.x = ball->pos.x;
    camera->lookAt.y = ball->pos.y + BALL_LOOK_AT_HEIGHT();
    camera->lookAt.z = ball->pos.z;

    r31 = ((modeCtrl.submodeTimer - 30) << 9);
    if (r31 < 0)
        r31 = 0;
    if (r31 < 0x2000)
        f1 = 3.0f;
    else
        f1 = 3.0 + (float)(r31 - 8192) * -3.4877232142857141e-05;

    sp10.x = -ball->pos.x;
    sp10.y = 0.0f;
    sp10.z = -ball->pos.z;
    vec_set_len(f1, &sp10, &sp10);
    mtxa_from_identity();
    mtxa_rotate_y(r31);
    mtxa_tf_vec(&sp10, &sp10);

    camera->eye.x = sp10.x + ball->pos.x;
    camera->eye.y = ball->pos.y + 2.0 + (float)r31 * -3.0517578125e-05;
    camera->eye.z = sp10.z + ball->pos.z;

    sp10.x = camera->lookAt.x - camera->eye.x;
    sp10.y = camera->lookAt.y - camera->eye.y;
    sp10.z = camera->lookAt.z - camera->eye.z;

    camera_face_direction(camera, &sp10);

    camera->mode = 34;
}

void g_camera_func34(struct Camera *camera, struct Ball *ball)
{
    Vec sp34;
    Vec sp28;
    Vec sp1C;
    Vec sp10;
    int r28;
    float f30;
    float f31;

    if (g_some_other_flags & 0xA)
        return;

    sp1C = camera->eye;  /* unused */
    sp10 = camera->lookAt;

    camera->lookAt.x += (ball->pos.x - camera->lookAt.x) * 0.5;
    camera->lookAt.y += (ball->pos.y + BALL_LOOK_AT_HEIGHT() - camera->lookAt.y) * 0.5;
    camera->lookAt.z += (ball->pos.z - camera->lookAt.z) * 0.5;

    r28 = ((modeCtrl.submodeTimer - 30) << 9);
    if (r28 < 0)
        r28 = 0;
    if (r28 < 0x2000)
        f30 = 3.0f;
    else
        f30 = 3.0 + (float)(r28 - 8192) * -3.4877232142857141e-05;

    sp34.x = -ball->pos.x;
    sp34.y = 0.0f;
    sp34.z = -ball->pos.z;
    f31 = mathutil_vec_len(&sp34);
    vec_set_len(f30, &sp34, &sp34);

    mtxa_from_identity();
    mtxa_rotate_y(r28);
    mtxa_tf_vec(&sp34, &sp34);

    sp28.x = sp34.x + ball->pos.x;
    sp28.y = ball->pos.y + 2.0 * (f31 / 10.0) + (float)r28 * -3.0517578125e-05;
    sp28.z = sp34.z + ball->pos.z;

    if (modeCtrl.submodeTimer > 15)
    {
        camera->eyeVel.x += ((sp28.x - camera->eye.x) * 0.15 - camera->eyeVel.x) * 0.4;
        camera->eyeVel.y += ((sp28.y - camera->eye.y) * 0.15 - camera->eyeVel.y) * 0.4;
        camera->eyeVel.z += ((sp28.z - camera->eye.z) * 0.15 - camera->eyeVel.z) * 0.4;
    }
    else
    {
        camera->eyeVel.x += ((sp28.x - camera->eye.x) * 0.3 - camera->eyeVel.x) * 0.8;
        camera->eyeVel.y += ((sp28.y - camera->eye.y) * 0.3 - camera->eyeVel.y) * 0.8;
        camera->eyeVel.z += ((sp28.z - camera->eye.z) * 0.3 - camera->eyeVel.z) * 0.8;
    }

    camera->eye.x += camera->eyeVel.x;
    camera->eye.y += camera->eyeVel.y;
    camera->eye.z += camera->eyeVel.z;

    sp34.x = camera->lookAt.x - camera->eye.x;
    sp34.y = camera->lookAt.y - camera->eye.y;
    sp34.z = camera->lookAt.z - camera->eye.z;

    camera_face_direction(camera, &sp34);

    camera->lookAtVel.x = camera->lookAt.x - sp10.x;
    camera->lookAtVel.y = camera->lookAt.y - sp10.y;
    camera->lookAtVel.z = camera->lookAt.z - sp10.z;
}

void g_camera_func36(struct Camera *camera, struct Ball *ball);

void g_camera_func35(struct Camera *camera, struct Ball *ball)
{
    camera->mode = 36;
    g_camera_func36(camera, ball);
}

/* Dot product of (x1, y1, z1) and (x2, y2, z2). SMB1's version took its parameters
 * in a shuffled order; SMB2's argument evaluation matches this straightforward one. */
static inline float mathutil_dot_xyz(register float x1, register float y1, register float z1, register float x2, register float y2, register float z2)
{
#ifdef __MWERKS__
    asm
    {
        fmuls x1, x1, x2
        fmadds x1, y1, y2, x1
        fmadds x1, z1, z2, x1
    }
    return x1;
#else
    return x1 * x2 + y1 * y2 + z1 * z2;
#endif
}

void g_camera_func36(struct Camera *camera, struct Ball *ball)
{
    Vec sp28;
    Vec sp1C;
    Vec sp10;
    float len;
    float distSq;
    float dot;

    if (g_some_other_flags & 0xA)
        return;

    sp1C = camera->eye;
    sp10 = camera->lookAt;

    camera->lookAt.x = ball->pos.x;
    camera->lookAt.y = ball->pos.y + BALL_LOOK_AT_HEIGHT();
    camera->lookAt.z = ball->pos.z;

    sp28.x = -ball->pos.x;
    sp28.y = 0.0f;
    sp28.z = -ball->pos.z;

    len = mathutil_vec_len(&sp28);
    vec_set_len(3.0f, &sp28, &sp28);

    sp28.x += ball->pos.x;
    sp28.z += ball->pos.z;
    sp28.x += (camera->eye.x - sp28.x) * 0.95;
    sp28.z += (camera->eye.z - sp28.z) * 0.95;
    sp28.x -= ball->pos.x;
    sp28.z -= ball->pos.z;
    vec_set_len(3.0f, &sp28, &sp28);

    sp28.y = ball->pos.y + (len / 10.0) * 2.0;
    camera->eye.x = sp28.x + ball->pos.x;
    camera->eye.y += (sp28.y - camera->eye.y) * 0.1;
    camera->eye.z = sp28.z + ball->pos.z;

    distSq = camera->eye.x * camera->eye.x + camera->eye.z * camera->eye.z;
    dot = mathutil_dot_xyz(ball->pos.x, 0.0f, ball->pos.z, camera->eye.x, 0.0f, camera->eye.z);

    if (distSq < 4.0f || dot < 0.0f)
    {
        distSq = math_sqrt(distSq);
        sp28.x = ball->pos.x * 0.2f + sp1C.x;
        sp28.y = 0.0f;
        sp28.z = ball->pos.z * 0.2f + sp1C.z;
        if (distSq < 1.0f || dot < 0.0f)
        {
            vec_set_len(1.0f, &sp28, &sp28);
            camera->eye.x = sp28.x;
            camera->eye.z = sp28.z;
        }
        else
        {
            vec_set_len(1.0f, &sp28, &sp28);
            dot = 2.0f - distSq;
            camera->eye.x += (sp28.x - camera->eye.x) * dot;
            camera->eye.z += (sp28.z - camera->eye.z) * dot;
        }
    }

    sp28.x = camera->lookAt.x - camera->eye.x;
    sp28.y = camera->lookAt.y - camera->eye.y;
    sp28.z = camera->lookAt.z - camera->eye.z;

    camera_face_direction(camera, &sp28);

    camera->eyeVel.x = camera->eye.x - sp1C.x;
    camera->eyeVel.y = camera->eye.y - sp1C.y;
    camera->eyeVel.z = camera->eye.z - sp1C.z;

    camera->lookAtVel.x = camera->lookAt.x - sp10.x;
    camera->lookAtVel.y = camera->lookAt.y - sp10.y;
    camera->lookAtVel.z = camera->lookAt.z - sp10.z;
}

void g_camera_func37(struct Camera *camera, struct Ball *ball)
{
    Vec sp28;
    Vec sp1C;
    Vec sp10;

    if (g_some_other_flags & 0xA)
        return;

    sp1C = camera->eye;
    sp10 = camera->lookAt;

    camera->lookAt.x = ball->pos.x;
    camera->lookAt.y = ball->pos.y + BALL_LOOK_AT_HEIGHT();
    camera->lookAt.z = ball->pos.z;

    sp28.x = -stagedef->goals[0].pos.x;
    sp28.y = 0.0f;
    sp28.z = -stagedef->goals[0].pos.z;
    vec_set_len(3.0f, &sp28, &sp28);

    sp28.x += ball->pos.x;
    sp28.z += ball->pos.z;
    sp28.x += (camera->eye.x - sp28.x) * 0.95;
    sp28.z += (camera->eye.z - sp28.z) * 0.95;
    sp28.x -= ball->pos.x;
    sp28.z -= ball->pos.z;
    vec_set_len(3.0f, &sp28, &sp28);

    camera->eye.x = sp28.x + ball->pos.x;
    camera->eye.y = ball->pos.y + 2.0;
    camera->eye.z = sp28.z + ball->pos.z;

    sp28.x = camera->lookAt.x - camera->eye.x;
    sp28.y = camera->lookAt.y - camera->eye.y;
    sp28.z = camera->lookAt.z - camera->eye.z;

    camera_face_direction(camera, &sp28);

    camera->eyeVel.x = camera->eye.x - sp1C.x;
    camera->eyeVel.y = camera->eye.y - sp1C.y;
    camera->eyeVel.z = camera->eye.z - sp1C.z;

    camera->lookAtVel.x = camera->lookAt.x - sp10.x;
    camera->lookAtVel.y = camera->lookAt.y - sp10.y;
    camera->lookAtVel.z = camera->lookAt.z - sp10.z;
}

void g_camera_func39(struct Camera *camera, struct Ball *ball) {}

void g_camera_func40(struct Camera *camera, struct Ball *ball) {}

void g_camera_func42(struct Camera *camera, struct Ball *ball);

void g_camera_func41(struct Camera *camera, struct Ball *ball)
{
    camera_clear(camera);
    camera->timerCurr = 0;
    camera->unk26 = 6;
    g_camera_func42(camera, ball);
    camera->mode = 42;
}

/* Keyframes for the ending cutscene camera (mode 42). */
Vec endingCameraPositions[] =
{
    { 0.0f, 2.3f, -4.0f },
    { 0.0f, 0.4f, -4.0f },
    { 0.0f, 0.4f, -3.0f },
    { 0.0f, 0.4f, 0.0f },
    { 0.0f, 4.0f, -2.0f },
    { 0.0f, -150.0f, 580.0f },
};

/* SMB1's Struct802F1C28: the object the ending cutscene camera follows. */
struct EndingSubject
{
    u8 filler0[0x258];
    /*0x258*/ float unk258;
    /*0x25C*/ float unk25C;
    /*0x260*/ float unk260;
};

extern struct EndingSubject *lbl_2_bss_44;

void g_camera_func42(struct Camera *camera, struct Ball *ball)
{
    Vec sp1C;
    Vec sp10;
    struct EndingSubject *r5 = lbl_2_bss_44;

    if ((g_some_other_flags & 0xA) && camera->mode == 42)
        return;

    if (camera->timerCurr < 480)
    {
        float t;
        Vec *r3;
        Vec *r4;

        t = camera->timerCurr / 480.0f;
        t = SMOOTHSTEP(t);
        r3 = &endingCameraPositions[0];
        r4 = &endingCameraPositions[1];

        camera->eye.x = r3->x * (1.0 - t) + r4->x * t;
        camera->eye.y = r3->y * (1.0 - t) + r4->y * t;
        camera->eye.z = r3->z * (1.0 - t) + r4->z * t;

        camera->lookAt.x = r5->unk258;
        camera->lookAt.y = r5->unk25C + 0.4 + camera->eye.y - 0.4;
        camera->lookAt.z = r5->unk260;
    }
    else if (camera->timerCurr < 600)
    {
        camera->eye.x = endingCameraPositions[1].x;
        camera->eye.y = endingCameraPositions[1].y;
        camera->eye.z = endingCameraPositions[1].z;

        camera->lookAt.x = r5->unk258;
        camera->lookAt.y = r5->unk25C + 0.4;
        camera->lookAt.z = r5->unk260;
    }
    else if (camera->timerCurr < 600 + 540)
    {
        float t;
        Vec *r4;
        int r30;

        t = (float)(camera->timerCurr - 600) / 540.0f;
        t = SMOOTHSTEP(t);
        r4 = &endingCameraPositions[2];

        camera->eye.x = r4->x;
        camera->eye.y = r4->y;
        camera->eye.z = r4->z;

        camera->lookAt.x = r5->unk258;
        camera->lookAt.y = r5->unk25C + 0.4;
        camera->lookAt.z = r5->unk260;

        sp1C.x = camera->eye.x - camera->lookAt.x;
        sp1C.y = camera->eye.y - camera->lookAt.y;
        sp1C.z = camera->eye.z - camera->lookAt.z;

        r30 = -8192.0 * (1.0 - t) + 8192.0f * t;
        mtxa_from_translate(&camera->lookAt);
        mtxa_rotate_y(r30);
        mtxa_tf_point(&sp1C, &camera->eye);
    }
    else if (camera->timerCurr < 1200)
    {
        camera->eye.x = endingCameraPositions[4].x;
        camera->eye.y = endingCameraPositions[4].y;
        camera->eye.z = endingCameraPositions[4].z;

        camera->lookAt.x = r5->unk258;
        camera->lookAt.y = r5->unk25C + 0.4;
        camera->lookAt.z = r5->unk260;
    }
    else
    {
        float t = (float)(camera->timerCurr - 1200) / 165.0f;

        t = CLAMP(t, 0.0, 1.0);
        t = -2.0 * t * t * t + 3.0 * t * t;

        camera->eye.x = endingCameraPositions[4].x;
        camera->eye.y = endingCameraPositions[4].y;
        camera->eye.z = endingCameraPositions[4].z - t * 2.6;

        camera->lookAt.x = r5->unk258;
        camera->lookAt.y = 0.4 + t * 15.0;
        camera->lookAt.z = r5->unk260;
    }

    sp10.x = camera->lookAt.x - camera->eye.x;
    sp10.y = camera->lookAt.y - camera->eye.y;
    sp10.z = camera->lookAt.z - camera->eye.z;

    camera_face_direction(camera, &sp10);

    if (merged_digital_inputs[0] & PAD_BUTTON_A)
        camera->timerCurr += 2;
    else
        camera->timerCurr += 1;
}

void g_something_with_translating_items(int type, Vec *pos, float radius);

/* Attract-mode demo: pick a random replay camera, avoiding the previous one. */
void g_camera_func44(struct Camera *camera, struct Ball *ball)
{
    u32 modes[32];
    int modesLen;
    struct ReplayBallFrame sp28;
    struct ReplayBallFrame sp8;
    struct DemoCameraState *demo;
    float f31;
    int nextMode;
    int idx;

    if (g_some_other_flags & 0xA)
        return;

    camera_clear(camera);

    camera->timerCurr = 0;
    demo = &demoCameraStates[camera->idx];
    f31 = fn_1_61B18(lbl_1_bss_71EC8.unk0[ball->playerId]);
    fn_1_62AA0(lbl_1_bss_71EC8.unk0[ball->playerId], demo->replayHeader);

    modesLen = 0;
    modes[modesLen] = 5;
    modesLen++;

    if ((*(u16 *)demo->replayHeader & 1) && lbl_1_bss_71EC8.unk10 > f31 - 240.0f)
    {
        modes[modesLen] = 16;
        modesLen++;
    }

    g_get_replay_frame_data(lbl_1_bss_71EC8.unk0[ball->playerId], lbl_1_bss_71EC8.unk10, &sp8);
    g_get_replay_frame_data(lbl_1_bss_71EC8.unk0[ball->playerId], 0.0f, &sp28);
    if (mathutil_vec_distance(&sp8.pos, &sp28.pos) < 16.0)
    {
        modes[modesLen] = 7;
        modesLen++;
    }

    g_get_replay_frame_data(lbl_1_bss_71EC8.unk0[ball->playerId], lbl_1_bss_71EC8.unk10 - 60.0, &sp28);
    if (mathutil_vec_distance(&sp8.pos, &sp28.pos) > 1.0 && lbl_1_bss_71EC8.unk10 > 240.0)
    {
        modes[modesLen] = 46;
        modesLen++;
    }

    modes[modesLen] = 48;
    modesLen++;

    idx = (rand() & 0x7FFF) % modesLen;
    nextMode = modes[idx];
    if (nextMode == demo->lastSubMode)
    {
        idx++;
        idx = idx % modesLen;
        nextMode = modes[idx];
    }
    demo->lastSubMode = nextMode;
    camera->mode = nextMode;
    camera_funcs[nextMode](camera, ball);
    demo->subMode = camera->mode;
    camera->mode = 45;
}

/* Attract-mode demo: run the chosen camera and decide when to cut to another. */
void g_camera_func45(struct Camera *camera, struct Ball *ball)
{
    Vec sp8;
    struct ReplayBallFrame sp14;
    int cut;
    struct DemoCameraState *demo;

    if (g_some_other_flags & 0xA)
        return;

    camera->timerCurr++;
    demo = &demoCameraStates[camera->idx];
    camera_funcs[demo->subMode](camera, ball);
    if (camera->mode != 45)
    {
        demo->subMode = camera->mode;
        camera->mode = 45;
        camera->timerCurr = 0;
    }

    if (camera->idx == modeCtrl.currPlayer)
    {
        mtxa_from_mtx(camera->unk14C);
        mtxa_rigid_inv_tf_tl(&sp8);
        if (mathutil_vec_distance(&sp8, &ball->pos) < 5.0f)
            g_something_with_translating_items(5, &sp8, 0.5f);
    }

    cut = 0;
    switch (demo->subMode)
    {
    case 47:
        if (!(camera->timerCurr & 0x1F))
        {
            g_get_replay_frame_data(lbl_1_bss_71EC8.unk0[ball->playerId], lbl_1_bss_71EC8.unk10 - 60.0, &sp14);
            if (mathutil_vec_distance(&ball->pos, &sp14.pos) < 0.5)
                cut = 1;
        }
        /* fall through */
    case 8:
    case 17:
        if (camera->timerCurr > 120 && !(camera->timerCurr & 0x3F))
        {
            if (mathutil_vec_distance(&ball->pos, &camera->eye) > 16.0)
                cut = 1;
        }
        if (camera->timerCurr > 360)
            cut = 1;
        break;
    case 49:
        if (camera->timerCurr > 240 && !(camera->timerCurr & 0x1F) && (rand() & 1))
            cut = 1;
        break;
    case 9:
    case 18:
    case 20:
        if (camera->timerCurr > 120 && !(camera->timerCurr & 0x1F) && (rand() & 1))
            cut = 1;
        break;
    }
    if (camera->flags & (1 << 8))
        cut = 1;
    if (cut && ball->state == 10)
        camera->mode = 44;
}

void g_camera_func47(struct Camera *camera, struct Ball *ball);

/* Replay: follow the ball from a point further back along its recorded path. */
void g_camera_func46(struct Camera *camera, struct Ball *ball)
{
    struct ReplayBallFrame sp10;

    if (g_some_other_flags & 0xA)
        return;

    camera_clear(camera);
    camera->unk26 = 0;
    camera->flags |= CAMERA_FLAG_UNK4;
    if (lbl_1_bss_71EC8.unk10 > 180.0)
        camera->unk68 = lbl_1_bss_71EC8.unk10 - (RAND_FLOAT() * 1.5 + 0.5) * 60.0;
    else
        camera->unk68 = lbl_1_bss_71EC8.unk10 * (RAND_FLOAT() * 0.5 + 0.5);
    camera->unk6C = RAND_FLOAT() * 2.0 + 0.3;
    camera->unk88 = camera->unk68 * (RAND_FLOAT() * 0.5 + 0.2);
    camera->unk88 = (camera->unk88 - camera->unk68) / lbl_1_bss_71EC8.unk10;
    g_get_replay_frame_data(lbl_1_bss_71EC8.unk0[ball->playerId], camera->unk68, &sp10);
    camera->unk5C = sp10.pos;
    camera->eye = sp10.pos;
    camera->unk7C.x = 0.0f;
    camera->unk7C.y = 0.0f;
    camera->unk7C.z = 0.0f;
    camera->lookAt = ball->pos;
    camera->mode = 47;
    g_camera_func47(camera, ball);
}

void g_camera_func47(struct Camera *camera, struct Ball *ball)
{
    Vec sp3C;
    Vec sp30;
    struct ReplayBallFrame sp10;
    float f31;

    if (g_some_other_flags & 0xA)
        return;

    camera->unk68 += camera->unk88;
    g_get_replay_frame_data(lbl_1_bss_71EC8.unk0[ball->playerId], camera->unk68, &sp10);
    f31 = mathutil_vec_distance(&ball->pos, &camera->eye);
    f31 = 2.0 - f31 * 0.125;
    if (f31 < 0.0)
        f31 = 0.0f;
    sp30.x = ball->vel.z;
    sp30.y = 0.0f;
    sp30.z = ball->vel.x;

    sp3C.x = camera->eye.x - ball->pos.x;
    sp3C.y = 0.0f;
    sp3C.z = camera->eye.z - ball->pos.z;
    vec_normalize_len(&sp30);
    vec_normalize_len(&sp3C);

    if (sp30.x * sp3C.x + sp30.z * sp3C.z < -0.1)
    {
        sp30.x = -sp30.x;
        sp30.z = -sp30.z;
    }
    if (sp30.x == 0.0 && sp30.z == 0.0)
    {
        sp30.x = sp3C.x;
        sp30.z = sp3C.z;
    }

    vec_set_len(camera->unk6C * 2.0 * f31, &sp30, &sp30);
    camera->unk7C.x += (sp30.x - camera->unk7C.x) * 0.05;
    camera->unk7C.z += (sp30.z - camera->unk7C.z) * 0.05;
    camera->unk5C.x = sp10.pos.x;
    camera->unk5C.y = sp10.pos.y;
    camera->unk5C.z = sp10.pos.z;

    camera->eyeVel.x = (sp10.pos.x + camera->unk7C.x - camera->eye.x) * 0.1;
    camera->eyeVel.y = (sp10.pos.y + camera->unk6C - camera->eye.y) * 0.1;
    camera->eyeVel.z = (sp10.pos.z + camera->unk7C.z - camera->eye.z) * 0.1;

    camera->eye.x += camera->eyeVel.x;
    camera->eye.y += camera->eyeVel.y;
    camera->eye.z += camera->eyeVel.z;

    sp3C.x = (ball->pos.x + 6.0 * ball->vel.x) - camera->lookAt.x;
    sp3C.y = (ball->pos.y + 6.0 * ball->vel.y) - camera->lookAt.y;
    sp3C.z = (ball->pos.z + 6.0 * ball->vel.z) - camera->lookAt.z;

    camera->lookAtVel.x = sp3C.x * 0.1;
    camera->lookAtVel.y = sp3C.y * 0.1;
    camera->lookAtVel.z = sp3C.z * 0.1;

    camera->lookAt.x += camera->lookAtVel.x;
    camera->lookAt.y += camera->lookAtVel.y;
    camera->lookAt.z += camera->lookAtVel.z;

    sp3C.x = camera->lookAt.x - camera->eye.x;
    sp3C.y = camera->lookAt.y - camera->eye.y;
    sp3C.z = camera->lookAt.z - camera->eye.z;

    camera_face_direction(camera, &sp3C);

    if (lbl_1_bss_71EC8.unk10 < 60.0f)
        camera->mode = 14;
}

void g_camera_func49(struct Camera *camera, struct Ball *ball);

/* Replay: a fixed vantage point near the goal (or around the ball), riding its item group. */
void g_camera_func48(struct Camera *camera, struct Ball *ball)
{
    Vec sp58;
    Vec sp4C;
    u8 header[0x18];  /* SMB1's ReplayHeader */
    Vec sp28;
    Vec sp1C;
    Vec sp10;
    float f31;

    if (g_some_other_flags & 0xA)
        return;

    camera_clear(camera);
    camera->flags |= CAMERA_FLAG_UNK4;
    fn_1_62AA0(lbl_1_bss_71EC8.unk0[ball->playerId], header);

    if (!(*(u16 *)header & 1)
     || mode_info.goalEntered >= stagedef->goalCount
     || mode_info.goalItemGroup >= stagedef->itemGroupCount)
    {
        camera->unk116 = 0;
        camera->unk118 = -1;

        sp58.x = RAND_FLOAT() - 0.5;
        sp58.y = 0.25f;
        sp58.z = RAND_FLOAT() - 0.5;
        vec_set_len(RAND_FLOAT() * 20.0 + 20.0, &sp58, &sp58);
    }
    else
    {
        camera->unk116 = mode_info.goalItemGroup;
        camera->unk118 = mode_info.goalEntered;

        sp58 = stagedef->goals[camera->unk118].pos;
        mtxa_from_mtx(itemgroups[camera->unk116].transform);
        mtxa_rigid_inv_tf_point(&ball->pos, &sp28);

        f31 = mathutil_vec_distance(&sp58, &sp28);
        if (f31 > 60.0)
            f31 = 60.0f;
        else if (f31 < 20.0)
            f31 = 20.0f;

        sp4C.x = RAND_FLOAT() - 0.5;
        sp4C.y = 0.0001f;
        sp4C.z = RAND_FLOAT() - 0.5;
        vec_set_len(f31, &sp4C, &sp4C);

        sp4C.y = f31 * (RAND_FLOAT() * 0.5 + 0.5);
        if (camera->unk116 != 0)
        {
            sp58.x = sp4C.x;
            sp58.y = sp4C.y;
            sp58.z = sp4C.z;
        }
        else
        {
            sp58.x = sp4C.x + (sp58.x + sp28.x) * 0.5 - sp58.x;
            sp58.y = sp4C.y + (sp58.y + sp28.y) * 0.5 - sp58.y;
            sp58.z = sp4C.z + (sp58.z + sp28.z) * 0.5 - sp58.z;
        }
    }
    camera->unk11C = sp58;

    mtxa_from_mtx(itemgroups[camera->unk116].transform);
    if (camera->unk118 >= 0)
    {
        mtxa_translate(&stagedef->goals[camera->unk118].pos);
        mtxa_sq_from_identity();
    }
    f31 = (float)camera->sub28.view.nextFov * (RAND_FLOAT() + 0.1f);
    mtxa_rotate_y((s16)(f31 * (RAND_FLOAT() - 0.5f)));
    mtxa_rotate_x((s16)(f31 * (RAND_FLOAT() - 0.5f)));
    mtxa_rigid_inv_tf_point(&ball->pos, &camera->unk134);
    mtxa_rigid_inv_tf_vec(&ball->vel, &camera->unk140);

    mtxa_from_mtx(itemgroups[camera->unk116].transform);
    if (camera->unk118 >= 0)
    {
        mtxa_translate(&stagedef->goals[camera->unk118].pos);
        mtxa_sq_from_identity();
    }
    mtxa_tf_point(&camera->unk11C, &camera->eye);
    mtxa_tf_point(&camera->unk134, &camera->lookAt);

    camera->unk74 = (RAND_FLOAT() + 0.5f) * 15.0f;
    sp4C = camera->unk11C;

    mtxa_from_mtx(itemgroups[camera->unk116].transform);
    if (camera->unk118 >= 0)
    {
        mtxa_translate(&stagedef->goals[camera->unk118].pos);
        mtxa_sq_from_identity();
    }
    mtxa_tf_point(&sp4C, &sp1C);

    mtxa_from_mtx(itemgroups[camera->unk116].prevTransform);
    if (camera->unk118 >= 0)
    {
        mtxa_translate(&stagedef->goals[camera->unk118].pos);
        mtxa_sq_from_identity();
    }
    mtxa_tf_point(&sp4C, &sp10);

    sp4C.x = sp1C.x - sp10.x;
    sp4C.y = sp1C.y - sp10.y;
    sp4C.z = sp1C.z - sp10.z;

    camera->eyeVel = sp4C;
    camera->unk5C = sp4C;
    camera->unk70 = 1;
    camera->mode = 49;
    camera->unk26 = 5;
    camera->sub28.view.fov = f31;
    g_camera_func49(camera, ball);
}

void g_camera_func49(struct Camera *camera, struct Ball *ball)
{
    Vec sp34;
    Vec sp28;
    Vec sp1C;
    Vec sp10;

    if (g_some_other_flags & 0xA)
        return;

    if (camera->flags & (1 << 8))
        camera->mode = 46;

    mtxa_from_mtx(itemgroups[camera->unk116].transform);
    if (camera->unk118 >= 0)
    {
        mtxa_translate(&stagedef->goals[camera->unk118].pos);
        mtxa_sq_from_identity();
    }
    if (camera->unk70 != 0)
    {
        sp34 = camera->eye;
        mtxa_tf_point(&camera->unk11C, &camera->eye);

        camera->eyeVel.x = camera->eye.x - sp34.x;
        camera->eyeVel.y = camera->eye.y - sp34.y;
        camera->eyeVel.z = camera->eye.z - sp34.z;

        camera->unk140.x *= 0.92;
        camera->unk140.y *= 0.92;
        camera->unk140.z *= 0.92;

        sp34.x = ball->pos.x + ball->vel.x * 12.0;
        sp34.y = ball->pos.y + ball->vel.y * 12.0;
        sp34.z = ball->pos.z + ball->vel.z * 12.0;

        mtxa_rigid_inv_tf_point(&sp34, &sp28);

        /* possible oversight (also in SMB1): subtracts unk140.x on all three */
        camera->unk140.x = camera->unk140.x + (sp28.x - camera->unk134.x - camera->unk140.x) * 0.01;
        camera->unk140.y = camera->unk140.y + (sp28.y - camera->unk134.y - camera->unk140.x) * 0.01;
        camera->unk140.z = camera->unk140.z + (sp28.z - camera->unk134.z - camera->unk140.x) * 0.01;

        camera->unk134.x += camera->unk140.x;
        camera->unk134.y += camera->unk140.y;
        camera->unk134.z += camera->unk140.z;

        sp34 = camera->lookAt;
        mtxa_tf_point(&camera->unk134, &camera->lookAt);

        camera->lookAtVel.x = camera->lookAt.x - sp34.x;
        camera->lookAtVel.y = camera->lookAt.y - sp34.y;
        camera->lookAtVel.z = camera->lookAt.z - sp34.z;

        mtxa_push();
        mtxa_tf_point(&camera->unk11C, &sp1C);

        mtxa_from_mtx(itemgroups[camera->unk116].prevTransform);
        if (camera->unk118 >= 0)
        {
            mtxa_translate(&stagedef->goals[camera->unk118].pos);
            mtxa_sq_from_identity();
        }
        mtxa_tf_point(&camera->unk11C, &sp10);

        sp34.x = sp1C.x - sp10.x;
        sp34.y = sp1C.y - sp10.y;
        sp34.z = sp1C.z - sp10.z;

        if (camera->unk74 > 0)
        {
            camera->unk74--;
            camera->unk5C = sp34;
        }
        else
        {
            if (vec_dot_normalized_safe(&sp34, &camera->unk5C) < 0.4f)
                camera->unk70 = 0;
        }

        mtxa_pop();
    }
    else
    {
        camera->eyeVel.x *= 0.96f;
        camera->eyeVel.y *= 0.92f;
        camera->eyeVel.z *= 0.96f;

        camera->eye.x += camera->eyeVel.x;
        camera->eye.y += camera->eyeVel.y;
        camera->eye.z += camera->eyeVel.z;

        camera->lookAtVel.x *= 0.92;
        camera->lookAtVel.y *= 0.92;
        camera->lookAtVel.z *= 0.92;

        sp28.x = ball->pos.x + ball->vel.x * 12.0;
        sp28.y = ball->pos.y + ball->vel.y * 12.0;
        sp28.z = ball->pos.z + ball->vel.z * 12.0;

        /* possible oversight (also in SMB1): subtracts lookAtVel.x on all three */
        camera->lookAtVel.x += (sp28.x - camera->lookAt.x - camera->lookAtVel.x) * 0.01;
        camera->lookAtVel.y += (sp28.y - camera->lookAt.y - camera->lookAtVel.x) * 0.01;
        camera->lookAtVel.z += (sp28.z - camera->lookAt.z - camera->lookAtVel.x) * 0.01;

        camera->lookAt.x += camera->lookAtVel.x;
        camera->lookAt.y += camera->lookAtVel.y;
        camera->lookAt.z += camera->lookAtVel.z;
    }

    sp34.x = camera->lookAt.x - camera->eye.x;
    sp34.y = camera->lookAt.y - camera->eye.y;
    sp34.z = camera->lookAt.z - camera->eye.z;

    camera_face_direction(camera, &sp34);
}

void g_camera_func51(struct Camera *camera, struct Ball *ball);

void g_camera_func50(struct Camera *camera, struct Ball *ball)
{
    camera_clear(camera);
    camera->timerCurr = 0;
    camera->mode = 51;
}

/* Spin around the ball, bobbing up and down. */
void g_camera_func51(struct Camera *camera, struct Ball *ball)
{
    Vec sp10;

    if (g_some_other_flags & 0xA)
        return;

    camera->timerCurr++;
    camera->rotY = camera->timerCurr * 256;
    camera->rotX = mathutil_cos(camera->timerCurr * 128) * 4096.0f;
    camera->rotZ = 0;

    camera->lookAt.x = ball->pos.x;
    camera->lookAt.y = ball->pos.y + 0.4;
    camera->lookAt.z = ball->pos.z;

    sp10.x = 0.0f;
    sp10.y = 0.0f;
    sp10.z = mathutil_cos(camera->timerCurr * 128) * 1.5 + 3.5;

    mtxa_from_rotate_y(camera->rotY);
    mtxa_rotate_x(camera->rotX);
    mtxa_rotate_z(camera->rotZ);
    mtxa_tf_vec(&sp10, &sp10);

    camera->eye.x = camera->lookAt.x + sp10.x;
    camera->eye.y = camera->lookAt.y + sp10.y;
    camera->eye.z = camera->lookAt.z + sp10.z;
}

void g_camera_func57(struct Camera *camera, struct Ball *ball);

/* Random shot near the ball, then swing in behind it (mode 57). */
void g_camera_func56(struct Camera *camera, struct Ball *ball)
{
    Vec sp8 = { 0.0f, 0.0f, -1.0f };

    camera_clear(camera);

    camera->eye.x = ball->pos.x + (RAND_FLOAT() - 0.5f) * 15.0f;
    camera->eye.y = ball->pos.y + RAND_FLOAT() * 5.0f + 1.0f;
    camera->eye.z = ball->pos.z + (RAND_FLOAT() - 0.5f) * 15.0f;

    camera->unk74 = camera->rotY = (rand() & 0x7FFF) << 1;
    camera->unk70 = camera->rotX = (rand() & 0x7FFF) >> 2;
    camera->unk78 = camera->rotZ = 0;
    camera->unk94 = ball->rotY + 0x10000 - 0x8000;

    if (ball->ape->charaId == 2)
        camera->unk90 = -3265;
    else
        camera->unk90 = -1753;
    camera->unk98 = 0;

    mtxa_from_translate(&camera->eye);
    mtxa_rotate_y(camera->rotY);
    mtxa_rotate_x(camera->rotX);
    mtxa_tf_point(&sp8, &camera->lookAt);

    camera->timerMax = camera->timerCurr = 90;
    camera->mode = 57;
}

void g_camera_func57(struct Camera *camera, struct Ball *ball)
{
    Vec sp14 = { 0.0f, 0.57f, -1.6f };
    Vec sp8 = { 0.0f, 0.0f, -1.0f };
    float t;

    if (g_some_other_flags & 0xA)
        return;

    if (camera->timerCurr > 0)
        camera->timerCurr--;

    t = (float)camera->timerCurr / (float)camera->timerMax;
    t = SMOOTHSTEP(t);

    mtxa_from_translate(&ball->pos);
    mtxa_rotate_y(ball->rotY);
    mtxa_tf_point(&sp14, &sp14);

    camera->eye.x += (sp14.x - camera->eye.x) * 0.05;
    camera->eye.y += (sp14.y - camera->eye.y) * 0.05;
    camera->eye.z += (sp14.z - camera->eye.z) * 0.05;

    camera->rotY = camera->unk74 + (camera->unk94 - camera->unk74) * (1.0 - t);
    camera->rotX = camera->unk70 + (s16)(camera->unk90 - camera->unk70) * (1.0 - t);
    camera->rotZ = 0;

    mtxa_from_translate(&camera->eye);
    mtxa_rotate_y(camera->rotY);
    mtxa_rotate_x(camera->rotX);
    mtxa_tf_point(&sp8, &camera->lookAt);
}

void g_camera_func63(struct Camera *camera, struct Ball *ball)
{
    if (g_some_other_flags & 0xA)
        return;

    camera->eye.x = 0.0f;
    camera->eye.y = 0.0f;
    camera->eye.z = 10.0f;

    camera->lookAt.x = 0.0f;
    camera->lookAt.y = 0.0f;
    camera->lookAt.z = -5.0f;
}

void g_camera_func66(struct Camera *camera, struct Ball *ball)
{
    camera->eye.x = 0.0f;
    camera->eye.y = 1.0f;
    camera->eye.z = 0.0f;

    camera->lookAt.x = camera->lookAt.y = camera->lookAt.z = 0.0f;

    camera->rotY = 0;
    camera->rotX = -16384;
    camera->rotZ = 0;
}

void fn_1_1FFE4(struct Camera *camera, struct Ball *ball, Vec *eye, Vec *lookAt, s16 *xrot, s16 *yrot, s16 *zrot, int h);
void fn_1_203D0(struct Camera *camera, struct Ball *ball, Vec *eye, Vec *lookAt, s16 *xrot, s16 *yrot, s16 *zrot);

void g_camera_func68(struct Camera *camera, struct Ball *ball)
{
    Vec sp1C;
    Vec sp10;
    s16 spC;
    s16 spA;
    s16 sp8;

    fn_1_1FFE4(camera, ball, &sp1C, &sp10, &spC, &spA, &sp8, 0);

    camera->eye = sp1C;
    camera->lookAt = sp10;
    camera->rotX = spC;
    camera->rotY = spA;
    camera->rotZ = sp8;
    camera->mode = 69;
}

/* Multiplayer overview: frame all the balls from above. */
void g_camera_func69(struct Camera *camera, struct Ball *ball)
{
    Vec sp20;
    Vec sp14;
    s16 sp10;
    s16 spE;
    s16 spC;
    int intro = (cameras[0].mode == 10 || cameras[0].mode == 11);

    if (intro)
    {
        float t;

        fn_1_203D0(camera, ball, &sp20, &sp14, &sp10, &spE, &spC);
        fn_1_1FFE4(camera, ball, &camera->eye, &camera->lookAt, &camera->rotX, &camera->rotY, &camera->rotZ, 0);

        t = (float)cameras[0].timerCurr / (float)cameras[0].timerMax;
        t = SMOOTHSTEP(t);

        camera->eye.x = camera->eye.x * (1.0 - t) + sp20.x * t;
        camera->eye.y = camera->eye.y * (1.0 - t) + sp20.y * t;
        camera->eye.z = camera->eye.z * (1.0 - t) + sp20.z * t;

        camera->lookAt.x = camera->lookAt.x * (1.0 - t) + sp14.x * t;
        camera->lookAt.y = camera->lookAt.y * (1.0 - t) + sp14.y * t;
        camera->lookAt.z = camera->lookAt.z * (1.0 - t) + sp14.z * t;

        camera->rotX = sp10 + (s16)(camera->rotX - sp10) * (1.0 - t);
        camera->rotY = spE + (s16)(camera->rotY - spE) * (1.0 - t);
        camera->rotZ = spC + (s16)(camera->rotZ - spC) * (1.0 - t);
    }
    else
    {
        fn_1_1FFE4(camera, ball, &sp20, &sp14, &camera->rotX, &camera->rotY, &camera->rotZ, 0);

        sp20.x -= camera->eye.x;
        sp20.y -= camera->eye.y;
        sp20.z -= camera->eye.z;

        camera->eye.x += sp20.x * 0.2;
        camera->eye.y += sp20.y * 0.2;
        camera->eye.z += sp20.z * 0.2;

        sp14.y -= camera->lookAt.y;
        camera->lookAt.x = camera->eye.x;
        camera->lookAt.y += sp14.y * 0.2;
        camera->lookAt.z = camera->eye.z;
    }
}

/* Overview of all balls: look at the middle of their bounding box from above. */
void fn_1_1FFE4(struct Camera *camera, struct Ball *ball, Vec *eye, Vec *lookAt, s16 *xrot, s16 *yrot, s16 *zrot, int h)
{
    Vec sp14[3];
    Vec sp38;
    Vec sp44;
    Vec sp8;
    int i;
    struct Ball *b;
    float f31;

    for (b = &balls[0], i = 0; i < 3; i++, b++)
    {
        if (b->flags & (BALL_FLAG_11 | BALL_FLAG_GOAL))
            sp8 = b->unk15C;
        else
        {
            float f6 = b->unk15A / 30.0f;

            sp8.x = b->pos.x * (1.0 - f6) + b->unk15C.x * f6;
            sp8.y = b->pos.y * (1.0 - f6) + b->unk15C.y * f6;
            sp8.z = b->pos.z * (1.0 - f6) + b->unk15C.z * f6;
        }
        if (i == 0)
        {
            sp38.x = sp44.x = sp8.x;
            sp38.y = sp44.y = sp8.y;
            sp38.z = sp44.z = sp8.z;
        }
        else
        {
            if (sp8.x > sp38.x)
                sp38.x = sp8.x;
            if (sp8.y > sp38.y)
                sp38.y = sp8.y;
            if (sp8.z > sp38.z)
                sp38.z = sp8.z;

            if (sp8.x < sp44.x)
                sp44.x = sp8.x;
            if (sp8.y < sp44.y)
                sp44.y = sp8.y;
            if (sp8.z < sp44.z)
                sp44.z = sp8.z;
        }
        sp14[i] = sp8;
    }

    sp8.x = (sp38.x + sp44.x) * 0.5;
    sp8.y = sp38.y;
    sp8.z = (sp38.z + sp44.z) * 0.5;

    if (h == 1)
    {
        sp8.x -= camera->lookAt.x;
        sp8.y -= camera->lookAt.y;
        sp8.z -= camera->lookAt.z;

        lookAt->x = sp8.x * 0.2 + camera->lookAt.x;
        lookAt->y = sp8.y * 0.2 + camera->lookAt.y;
        lookAt->z = sp8.z * 0.2 + camera->lookAt.z;
    }
    else
    {
        lookAt->x = sp8.x;
        lookAt->y = sp8.y;
        lookAt->z = sp8.z;
    }

    f31 = 0.0f;
    for (b = &balls[0], i = 0; i < 3; i++, b++)  /* b unused, as in SMB1 */
    {
        float var1;

        sp8.x = lookAt->x - sp14[i].x;
        sp8.z = lookAt->z - sp14[i].z;
        var1 = mathutil_sum_of_sq_2(sp8.x, sp8.z);
        if (var1 > f31)
            f31 = var1;
    }
    f31 = math_sqrt(f31) * 2.3;
    if (f31 < 15.0)
        f31 = 15.0f;

    mtxa_from_translate(lookAt);
    mtxa_rotate_y(stagedef->startPos->yrot + 0x4000);
    mtxa_rotate_x(DEGREES_TO_S16(-90));
    mtxa_translate_xyz(0.0f, 0.0f, f31);

    mathutil_mtxA_get_translate(eye);

    mtxa_to_euler_yxz(yrot, xrot, zrot);
}

/* The overview the intro fly-in starts from: above the stage's fly-in sphere. */
void fn_1_203D0(struct Camera *camera, struct Ball *ball, Vec *eye, Vec *lookAt, s16 *xrot, s16 *yrot, s16 *zrot)
{
    struct Sphere sp8;

    get_curr_stage_fly_in_position(&sp8);

    lookAt->x = sp8.pos.x;
    lookAt->y = sp8.pos.y;
    lookAt->z = sp8.pos.z;

    mtxa_from_translate(lookAt);
    mtxa_rotate_y(stagedef->startPos->yrot + 0x4000);
    mtxa_rotate_x(DEGREES_TO_S16(-90));
    mtxa_translate_xyz(0.0f, 0.0f, sp8.radius * 1.2);

    mathutil_mtxA_get_translate(eye);

    mtxa_to_euler_yxz(yrot, xrot, zrot);
}

/* Award ceremony (SMB1's func_8009D7xx family, now in main_loop). */
int fn_1_E5E08(void);
int fn_1_E5E44(void);  /* number of podium monkeys */
void fn_1_E5E78(int idx, Vec *pos);
void fn_1_E6384(Vec *pos);
void fn_1_E678C(Mtx mtx);

void g_camera_func71(struct Camera *camera, struct Ball *ball);

void g_camera_func70(struct Camera *camera, struct Ball *ball)
{
    camera_clear(camera);
    camera->unk26 = 0;
    camera->sub28.view.unk28 = 0.0f;
    camera->sub28.view.unk2C = 0.0f;
    camera->sub28.view.fov = DEGREES_TO_S16(60);
    if (fn_1_E5E08() == 0)
        camera->timerCurr = fn_1_E5E44() * 272;
    g_camera_func71(camera, ball);
    camera->mode = 71;
}

void g_camera_func71(struct Camera *camera, struct Ball *ball)
{
    Vec sp10;
    int r31;
    int r29;
    s8 *r8;
    int r9;
    int r3;
    float t;

    if ((g_some_other_flags & 0xA) && camera->mode == 71)
        return;

    r31 = fn_1_E5E44();
    if (r31 <= 0)
        return;

    if (camera->timerCurr < r31 * 272)
    {
        r3 = camera->timerCurr / 272;
        r29 = camera->timerCurr - r3 * 272;

        if (r31 > 2)
        {
            switch (r3)
            {
            case 0:
                fn_1_E5E78(1, &sp10);
                break;
            case 1:
                fn_1_E5E78(0, &sp10);
                break;
            case 2:
                fn_1_E5E78(2, &sp10);
                break;
            case 3:
                fn_1_E5E78(3, &sp10);
                break;
            }
        }
        else
            fn_1_E5E78(r3, &sp10);

        if (r29 >= 204)
        {
            camera->eye.x = sp10.x + 0.0;
            camera->eye.y = sp10.y + 0.3;
            camera->eye.z = sp10.z - 1.2;

            camera->rotX = 0;
            camera->rotY = DEGREES_TO_S16(-180);
            camera->rotZ = 0;
        }
        else
        {
            t = r29 / 204.0f;
            t = SMOOTHSTEP(t);

            camera->eye.x = sp10.x + 0.0;
            camera->eye.y = t * 0.3 + (sp10.y - (0.75 * sp10.y) * (1.0 - t));
            camera->eye.z = sp10.z - 1.2 * (1.0 - t) - 1.2 * t;

            camera->rotX = 0;
            camera->rotY = DEGREES_TO_S16(-180);
            camera->rotZ = 0;
        }
    }
    else
    {
        fn_1_E6384(&sp10);

        camera->lookAt.x = sp10.x;
        camera->lookAt.y = sp10.y + 0.7;
        camera->lookAt.z = sp10.z;

        r29 = camera->timerCurr - r31 * 272;
        if (r29 < 8)
            t = (float)r29 / 8.0;
        else
            t = 1.0f;
        t = 1.0 - (1.0 - t) * (1.0 - t);

        camera->eye.x = camera->lookAt.x;
        camera->eye.y = camera->lookAt.y;
        camera->eye.z = camera->lookAt.z - (1.0 - t) - 3.0 * t;

        camera->rotX = 0;
        camera->rotY = -32768;
        camera->rotZ = 0;
    }

    fn_1_E678C(mtxa->mtxA);
    mtxa_translate(&camera->eye);
    mtxa_rotate_y(camera->rotY);
    mtxa_rotate_x(camera->rotX);
    mtxa_rotate_z(camera->rotZ);

    mathutil_mtxA_get_translate(&camera->eye);
    mtxa_to_euler_yxz(&camera->rotY, &camera->rotX, &camera->rotZ);
    mtxa_tf_point_xyz(0.0f, 0.0f, -1.0f, &camera->lookAt);
    camera->timerCurr++;

    r9 = 0;
    r8 = g_poolInfo.playerPool.statusList;
    for (r3 = 0; r3 < g_poolInfo.playerPool.count; r3++, r8++)
    {
        if (*r8 == 2 || *r8 == 4)
            r9 |= (pad_status_groups[player_pad_map[r3]].unk0[2].button & PAD_BUTTON_A) != 0;
    }

    if (r9 && camera->timerCurr > 8 && camera->timerCurr < r31 * 272)
        camera->timerCurr = r31 * 272;
    if (camera->timerCurr > 2176)
        camera->timerCurr = 2176;
}

void g_camera_func73(struct Camera *camera, struct Ball *ball);

void g_camera_func72(struct Camera *camera, struct Ball *ball)
{
    camera_clear(camera);
    camera->unk26 = 0;
    camera->sub28.view.unk28 = 0.0f;
    camera->sub28.view.unk2C = 0.0f;
    camera->sub28.view.fov = DEGREES_TO_S16(16);
    g_camera_func73(camera, ball);
    camera->mode = 73;
}

void g_camera_func73(struct Camera *camera, struct Ball *ball)
{
    struct Sphere sp10;

    get_curr_stage_fly_in_position(&sp10);

    camera->lookAt.x = sp10.pos.x;
    camera->lookAt.y = sp10.pos.y;
    camera->lookAt.z = sp10.pos.z;

    camera->eye.x = sp10.pos.x;
    camera->eye.y = sp10.pos.y - sp10.radius * 5.0;
    camera->eye.z = sp10.pos.z;

    mtxa_from_identity();
    mtxa_rotate_x(DEGREES_TO_S16(90));
    mtxa_to_euler_yxz(&camera->rotY, &camera->rotX, &camera->rotZ);
}

/* Minigame RELs install their own camera handler here. */
void g_camera_default_func(struct Camera *camera, struct Ball *ball)
{
    if (g_minigame_camera_func != NULL)
        ((void (*)(struct Camera *, struct Ball *))g_minigame_camera_func)(camera, ball);
}

void g_camera_func76_gameplay(struct Camera *camera, struct Ball *ball);

/* Gameplay camera setup: behind the ball, facing the stage start direction. */
void g_main_camera_function_func75(struct Camera *camera, struct Ball *ball)
{
    s16 *standstill = &g_camera_standstill_counters[camera->idx];
    Vec sp8;

    camera_clear(camera);
    camera->unk26 = 0;
    memset(standstill, 0, sizeof(*standstill));
    camera->lookAt.x = ball->pos.x;
    camera->lookAt.y = ball->pos.y + BALL_LOOK_AT_HEIGHT();
    camera->lookAt.z = ball->pos.z;
    mtxa_from_translate(&camera->lookAt);
    mtxa_rotate_y(stagedef->startPos->yrot);
    sp8.x = 0.0f;
    sp8.y = 1.0f;
    sp8.z = 3.0f;
    mtxa_tf_point(&sp8, &camera->eye);
    sp8.x = 0.0f;
    sp8.y = 0.0f;
    sp8.z = 3.0f;
    mtxa_tf_point(&sp8, &camera->unkB4);

    sp8.x = camera->lookAt.x - camera->eye.x;
    sp8.y = camera->lookAt.y - camera->eye.y;
    sp8.z = camera->lookAt.z - camera->eye.z;

    camera->rotY = math_atan2(sp8.x, sp8.z) - 32768;
    camera->rotX = BALL_CAMERA_PITCH();
    camera->rotZ = 0;

    camera->mode = 76;
}

/* Gameplay follow camera (SMB2's replacement for mode 1): turns toward the ball's
 * heading, more eagerly while the monkey stands still and the ball is slow. */
void g_camera_func76_gameplay(struct Camera *camera, struct Ball *ball)
{
    s16 *standstill;
    s16 pitch;
    s16 yaw;
    Vec sp8;
    /* Separate floats in this order reproduce the original register allocation. */
    float prevEyeZ, prevLookAtX, prevLookAtY, prevLookAtZ, prevEyeY, prevEyeX;
    float f1;
    int r3;
    int r25;
    int count;

    if (g_some_other_flags & 0xA)
        return;

    standstill = &g_camera_standstill_counters[camera->idx];

    prevEyeX = camera->eye.x;
    prevEyeY = camera->eye.y;
    prevEyeZ = camera->eye.z;

    prevLookAtX = camera->lookAt.x;
    prevLookAtY = camera->lookAt.y;
    prevLookAtZ = camera->lookAt.z;

    count = *standstill;
    if (ball->ape != NULL)
    {
        if (ball->ape->unk86 == 4)
        {
            count++;
            if (count > 60)
                count = 60;
        }
        else
        {
            count--;
            if (count < 0)
                count = 0;
        }
    }
    *standstill = count;

    sp8.x = camera->unkB4.x - camera->lookAt.x;
    sp8.y = camera->unkB4.y - camera->lookAt.y;
    sp8.z = camera->unkB4.z - camera->lookAt.z;

    f1 = mathutil_sum_of_sq_3(sp8.x, sp8.y, sp8.z);
    if (f1 > FLT_EPSILON)
    {
        f1 = math_rsqrt(f1);
        sp8.x *= f1;
        sp8.y *= f1;
        sp8.z *= f1;
    }
    else
    {
        sp8.x = 1.0f;
        sp8.y = 0.0f;
        sp8.z = 0.0f;
    }

    sp8.x = sp8.x * 0.75 + camera->lookAt.x;
    sp8.y = sp8.y * 0.75 + camera->lookAt.y;
    sp8.z = sp8.z * 0.75 + camera->lookAt.z;

    camera->lookAt.x = ball->pos.x;
    camera->lookAt.y = ball->pos.y + BALL_LOOK_AT_HEIGHT();
    camera->lookAt.z = ball->pos.z;

    sp8.x = camera->lookAt.x - sp8.x;
    sp8.y = camera->lookAt.y - sp8.y;
    sp8.z = camera->lookAt.z - sp8.z;

    if (ball->unk80 < 60)
        pitch = 0;
    else
        pitch = math_atan2(sp8.y, math_sqrt(mathutil_sum_of_sq_2(sp8.x, sp8.z)));

    yaw = math_atan2(sp8.x, sp8.z) - 32768;
    r25 = (s16)(yaw - camera->rotY);
    r25 *= 0.6;
    r25 = CLAMP(r25, -0x1A0, 0x1A0);
    yaw = camera->rotY + r25;
    if (ball->flags & 1)
    {
        float scale;

        r3 = 4096.0 * (1.0 - *standstill / 60.0f);
        r25 = (s16)(ball->unk92 - yaw);
        if (r25 > r3)
            r25 -= r3;
        else if (r25 < -r3)
            r25 += r3;
        else
            r25 = 0;

        f1 = math_sqrt(mathutil_sum_of_sq_3(ball->vel.x, ball->vel.y, ball->vel.z));
        if (f1 < 10.0 / 54.0)
            scale = 1.0f;
        else if (f1 < 15.0 / 54.0)
            scale = (f1 - 10.0 / 54.0) / ((15.0 - 10.0) / 54.0);
        else
            scale = 0.0f;
        r25 = r25 * scale;
        yaw += r25 >> 7;
    }

    r25 = (s16)(yaw - camera->rotY);
    r25 = CLAMP(r25, -0x300, 0x300);
    yaw = camera->rotY + r25;

    if (pitch < BALL_PITCH_BIAS() - 0x3000)
        pitch = BALL_PITCH_BIAS() - 0x3000;
    else if (pitch > BALL_PITCH_BIAS() + 0x3000)
        pitch = BALL_PITCH_BIAS() + 0x3000;

    if (camera->unkC0 < 0)
        camera->unkC0 *= 0.99;
    else
        camera->unkC0 *= 0.95;
    pitch = camera->unkC0 = camera->unkC0 + 0.2 * (pitch - camera->unkC0);

    mtxa_from_translate(&camera->lookAt);
    mtxa_rotate_y(yaw);
    mtxa_rotate_x(pitch);
    sp8.x = 0.0f;
    sp8.y = 0.0f;
    sp8.z = 3.0f;
    mtxa_tf_point(&sp8, &camera->unkB4);
    camera->unk114 = yaw - camera->rotY;
    camera->rotY = yaw;
    camera->rotX = pitch + BALL_CAMERA_PITCH();
    mtxa_from_translate(&camera->lookAt);
    mtxa_rotate_y(camera->rotY);
    mtxa_rotate_x(camera->rotX);
    sp8.x = 0.0f;
    sp8.y = 0.0f;
    sp8.z = math_sqrt(mathutil_sum_of_sq_2(3.0f, 1.0f));
    mtxa_tf_point(&sp8, &camera->eye);

    if (camera->rotZ != 0)
        camera->rotZ = 0.95f * camera->rotZ;

    camera->eyeVel.x = camera->eye.x - prevEyeX;
    camera->eyeVel.y = camera->eye.y - prevEyeY;
    camera->eyeVel.z = camera->eye.z - prevEyeZ;

    camera->lookAtVel.x = camera->lookAt.x - prevLookAtX;
    camera->lookAtVel.y = camera->lookAt.y - prevLookAtY;
    camera->lookAtVel.z = camera->lookAt.z - prevLookAtZ;
}

void g_camera_func83(struct Camera *camera, struct Ball *ball);

/* Random stage overview shot: eye and look-at at random directions and distances
 * around the stage, drifting for 5-10 seconds (mode 83).
 * Used as the Story Data Select / Name Entry backdrop (stage 0xC9). The eye may
 * land below the island and look up, so the screen shows only the deep-blue sky
 * for a whole shot: that is the original behaviour (seen in Dolphin too), not a
 * render bug. */
void g_camera_func82(struct Camera *camera, struct Ball *ball)
{
    struct Sphere sphere;
    s16 yaw;

    camera_clear(camera);
    camera->unk26 = 0;
    camera->timerCurr = (5.0f + 5.0f * RAND_FLOAT()) * 60.0f;
    camera->unkB4.x = 0.0f;
    camera->unkB4.y = 0.0f;
    camera->unkB4.z = 0.0f;
    get_curr_stage_fly_in_position(&sphere);

    mtxa_from_rotate_y(rand() & 0x7FFF);
    mtxa_rotate_x(rand() & 0x7FFF);
    mtxa_rotate_z(rand() & 0x7FFF);
    mtxa_tf_vec_xyz(0.0f, 0.0f, -1.0f, &camera->unk5C);

    camera->unk68 = sphere.radius * (0.1f + 0.4f * RAND_FLOAT());
    mtxa_tf_vec_xyz(camera->unk68, 0.0f, 0.0f, &camera->eye);
    camera->eye.x += sphere.pos.x;
    camera->eye.y += sphere.pos.y;
    camera->eye.z += sphere.pos.z;
    camera->unk6C = 0.5f * RAND_FLOAT();

    yaw = 65536.0f * (RAND_FLOAT() - 0.5f);
    mtxa_from_rotate_y(yaw);
    mtxa_rotate_x(24576.0f * (RAND_FLOAT() - 0.5f));
    mtxa_tf_vec_xyz(0.0f, 0.0f, -1.0f, &camera->unk7C);

    camera->unk88 = sphere.radius * (0.6f + 0.4f * RAND_FLOAT());
    yaw = yaw + 16384.0f * (1.0f + RAND_FLOAT());
    mtxa_from_rotate_y(yaw);
    mtxa_rotate_x(24576.0f * (RAND_FLOAT() - 0.5f));
    mtxa_tf_vec_xyz(0.0f, 0.0f, -1.0f, &camera->lookAt);
    camera->lookAt.x *= camera->unk88;
    camera->lookAt.y *= camera->unk88;
    camera->lookAt.z *= camera->unk88;
    camera->lookAt.x += sphere.pos.x;
    camera->lookAt.y += sphere.pos.y;
    camera->lookAt.z += sphere.pos.z;
    camera->unk8C = 0.5f * RAND_FLOAT();

    g_camera_func83(camera, ball);
    camera->mode = 83;
}

/* Stage overview: eye and look-at drift around the stage on their own axes; the
 * player can tilt the view with the C-stick while holding L or R. */
void g_camera_func83(struct Camera *camera, struct Ball *ball)
{
    Mtx sp6C;
    Vec sp60;
    Vec prevEye;
    Vec prevLookAt;
    Quat sp38;
    struct Sphere sphere;
    Vec sp1C;
    Vec sp10;
    S16Vec sp8;
    struct ControllerInfo *pad;
    int tilting;

    if ((g_some_other_flags & 0xA) && camera->mode == 83)
        return;

    get_curr_stage_fly_in_position(&sphere);
    prevEye = camera->eye;
    prevLookAt = camera->lookAt;

    camera->unk6C *= 0.998f;
    g_math_unk9_smth_w_quats(&sp38, &camera->unk5C, camera->unk6C / camera->unk68);
    mtxa_from_quat(&sp38);
    mtxa_to_mtx(sp6C);
    mtxa_from_translate(&sphere.pos);
    mtxa_mult_right(sp6C);
    mtxa_translate_neg(&sphere.pos);
    mtxa_tf_point(&camera->eye, &camera->eye);
    switch (world_theme)
    {
    case 25:
    case 36:
        if (camera->eye.y < *stagedef->falloutY)
            camera->eye.y = *stagedef->falloutY;
        break;
    }
    camera->eyeVel.x = camera->eye.x - prevEye.x;
    camera->eyeVel.y = camera->eye.y - prevEye.y;
    camera->eyeVel.z = camera->eye.z - prevEye.z;

    camera->unk8C *= 0.998f;
    g_math_unk9_smth_w_quats(&sp38, &camera->unk7C, camera->unk8C / camera->unk88);
    mtxa_from_quat(&sp38);
    mtxa_to_mtx(sp6C);
    mtxa_from_translate(&sphere.pos);
    mtxa_mult_right(sp6C);
    mtxa_translate_neg(&sphere.pos);
    mtxa_tf_point(&camera->lookAt, &camera->lookAt);
    camera->lookAtVel.x = camera->lookAt.x - prevLookAt.x;
    camera->lookAtVel.y = camera->lookAt.y - prevLookAt.y;
    camera->lookAtVel.z = camera->lookAt.z - prevLookAt.z;

    sp60.x = camera->lookAt.x - camera->eye.x;
    sp60.y = camera->lookAt.y - camera->eye.y;
    sp60.z = camera->lookAt.z - camera->eye.z;
    camera_face_direction(camera, &sp60);

    pad = &pad_status_groups[player_pad_map[ball->playerId]];
    tilting = 0;
    if (pad->unk0[0].button & (PAD_TRIGGER_L | PAD_TRIGGER_R))
        tilting = 1;
    if (tilting)
    {
        sp60.x = -(float)pad->unk0[0].substickX;
        sp60.y = -(float)pad->unk0[0].substickY;
    }
    else
    {
        sp60.x = 0.0f;
        sp60.y = 0.0f;
    }
    camera->unkB4.x += 0.05f * (sp60.x - camera->unkB4.x);
    camera->unkB4.y += 0.05f * (sp60.y - camera->unkB4.y);
    sp60.x = camera->unkB4.x;
    sp60.y = camera->unkB4.y;
    sp60.z = 0.0f;
    if (sp60.x != 0.0f || sp60.y != 0.0f)
    {
        float angle = 3.1415927f / 90.0 * mathutil_vec_len(&sp60);

        sp10.x = 0.0f;
        sp10.y = 0.0f;
        sp10.z = -1.0f;
        mathutil_vec_cross_prod(&sp10, &sp60, &sp1C);
        g_math_unk9_smth_w_quats(&sp38, &sp1C, angle);
        mtxa_from_quat(&sp38);
        mtxa_to_mtx(sp6C);
        mtxa_from_identity();
        mtxa_rotate_z(-camera->rotZ);
        mtxa_rotate_x(-camera->rotX);
        mtxa_rotate_y(-camera->rotY);
        mtxa_translate_neg(&camera->eye);
        mtxa_mult_left(sp6C);
        mtxa_rigid_invert();
        mtxa_to_euler(&sp8);
        camera->rotX = sp8.x;
        camera->rotY = sp8.y;
        camera->rotZ = sp8.z;
    }

    if (camera->timerCurr != 0)
        camera->timerCurr--;
    if (camera->timerCurr < 30.0f && tilting)
        camera->timerCurr = 30;
    if (camera->timerCurr <= 0)
        camera->mode = 82;
}

void g_camera_func84(struct Camera *camera, struct Ball *ball)
{
    g_camera_func85(camera, ball);
    camera->mode = 85;
}

/* Follow an externally supplied camera pose. */
void g_camera_func85(struct Camera *camera, struct Ball *ball)
{
    struct ExternalCameraPose *pose = &lbl_1_bss_DD4F4;
    Vec v;

    camera->lookAt = pose->lookAt;
    camera->eye = pose->eye;
    v.x = camera->lookAt.x - camera->eye.x;
    v.y = camera->lookAt.y - camera->eye.y;
    v.z = camera->lookAt.z - camera->eye.z;
    camera->rotY = math_atan2(v.x, v.z) - 32768;
    camera->rotX = math_atan2(v.y, math_sqrt(mathutil_sum_of_sq_2(v.x, v.z)));
    camera->rotZ = pose->rotZ;
    camera->sub28.view.fov = pose->fov;
}

/* Stage orbit whose starting yaw is seeded from the stage ID. */
void g_camera_func91(struct Camera *camera, struct Ball *ball)
{
    camera_clear(camera);
    camera->unk94 = 0x80;
    camera->unk74 = (current_stage_id << 12) + (s16)(9.752381f * current_stage_id);
    camera->unk88 = 1.0f;
    g_camera_func92(camera, ball);
    camera->mode = 92;
}

/* Like mode 13: orbit the stage's fly-in sphere, with stick zoom and spin, from a lower, closer angle. */
void g_camera_func92(struct Camera *camera, struct Ball *ball)
{
    struct Sphere sp14;
    Vec sp8;
    int padId;
    int i;

    if (!(g_some_other_flags & 0xA) || camera->mode != 92)
    {
        get_curr_stage_fly_in_position(&sp14);
        camera->lookAt.x = sp14.pos.x;
        camera->lookAt.y = sp14.pos.y;
        camera->lookAt.z = sp14.pos.z;
        if (camera->timerCurr > 0)
        {
            for (i = 0; i < modeCtrl.playerCount; i++)
            {
                padId = player_pad_map[i];
                if (camera->unk88 > 0.75 && (analog_inputs[padId][0] & (1 << 7)))
                    camera->unk88 -= 0.01;
                if (camera->unk88 < 1.25 && (analog_inputs[padId][0] & (1 << 6)))
                    camera->unk88 += 0.01;
            }
        }

        sp8.x = 0.0f;
        sp8.y = sp14.radius * (0.45 * camera->unk88);
        sp8.z = sp14.radius * (0.7 * camera->unk88);

        for (i = 0; i < modeCtrl.playerCount; i++)
        {
            padId = player_pad_map[i];
            if (camera->unk94 < 256 && (analog_inputs[padId][0] & (1 << 5)))
                camera->unk94 += 8;
            if (camera->unk94 > -256 && (analog_inputs[padId][0] & (1 << 4)))
                camera->unk94 -= 8;
        }
        if (camera->timerCurr > 0)
            camera->unk74 += camera->unk94;
        mtxa_from_translate(&camera->lookAt);
        mtxa_rotate_y(camera->unk74);
        mtxa_tf_point(&sp8, &camera->eye);

        sp8.x = camera->lookAt.x - camera->eye.x;
        sp8.y = camera->lookAt.y - camera->eye.y;
        sp8.z = camera->lookAt.z - camera->eye.z;

        camera_face_direction(camera, &sp8);

        if (camera->timerCurr < 60)
            camera->timerCurr++;
    }
}
