/* The circular stage minimap (SMB1 minimap.c). */
#include "types.h"
#include "avdisp.h"
#include "ball.h"
#include "bitmap.h"
#include "camera.h"
#include "dip.h"
#include "event.h"
#include "gx.h"
#include "light.h"
#include "mathutil.h"
#include "mode.h"
#include "nl2ngc.h"
#include "stobj.h"
#include "world.h"

extern struct NlModel * DPTR *g_init_common_gma_buf;  /* NlObj image: 4-byte slots */
extern struct GMA *minigameGma;
extern s32 active_monkey_id[];
extern u32 g_font_char_var_3;  /* global frame counter */
extern u32 monkey_flags;
extern struct
{
    u32 flags;
} g_some_render_flag;
extern struct
{
    u8 filler0[0x24];
    u32 unk24;
} perfInfo;

void fn_1_D2A88(struct NlModel *model, u32 mask, u32 value);
void fn_1_D2AC4(struct NlModel *model, u32 mask, u32 value);
void fn_1_D2B00(struct NlModel *model, u32 mask, u32 value);
void fn_1_57E14(Vec *center, float *radius);
void mtxa_from_translate_xyz(float x, float y, float z);
void g_set_something3(int);
void fn_1_E2D20(void);
void g_something_with_GX_vtx_desc(u32 attrs);
void g_something_with_raster_state(int a);
void g_GXSetTevIndirect_zero_if_different(int stage);
void g_GXSetTevOrder_wrapper(int stage, int texCoord, int texMap, int color);
void GXSetTevKAlphaSel_cached(int stage, int sel);
void GXSetTevColorIn_cached(int stage, int a, int b, int c, int d);
void GXSetTevColorOp_cached(int stage, int op, int bias, int scale, int clamp, int reg);
void GXSetTevAlphaIn_cached(int stage, int a, int b, int c, int d);
void GXSetTevAlphaOp_cached(int stage, int op, int bias, int scale, int clamp, int reg);
void GXSetNumTevStages_cached(u8 n);
void GXSetCullMode_cached(int mode);
void GXLoadPosMtxImm(Mtx mtx, int id);
void C_MTXFrustum(Mtx44 m, float t, float b, float l, float r, float n, float f);
void g_smth_with_gpu_and_locked_cache_mtx(void);
void g_math_unk16(Vec *src, Vec *dst, float tanHalfFov);
int g_smth_with_get_active_monkey_icon(int a, int b, int character);
void g_draw_stage(void);
void g_draw_stobjs(void);
void ord_tbl_draw_nodes(void);
int rand(void);

struct MinimapState
{
    /*0x00*/ u32 unk0;
    /*0x04*/ s32 mode;
    /*0x08*/ s16 tiltX;
    /*0x0A*/ u16 unkA;
    /*0x0C*/ float offsetX;
    /*0x10*/ float offsetY;
    /*0x14*/ float centerX;
    /*0x18*/ float centerY;
    /*0x1C*/ s32 zoom;
    /*0x20*/ float scale;
    /*0x24*/ Vec center;
    /*0x30*/ s16 screenX;
    /*0x32*/ s16 screenY;
    /*0x34*/ s16 size;
    /*0x36*/ s16 spin;
    /*0x38*/ s16 spinVel;
    /*0x3A*/ u16 unk3A;
    /*0x3C*/ float bubble;
    /*0x40*/ float bubbleTarget;
    /*0x44*/ float items;
    /*0x48*/ float itemsTarget;
};

struct MinimapState lbl_1_data_502E0 =
{
    0xFFFFFFFF,
    0,
    0, 0,
    1.0f,
    -0.8666666746139526f,
    -0.17499999701976776f,
    0.23333333432674408f,
    0,
    0.26499998569488525f,
    { 0.0f, 0.0f, 0.0f },
    0x0280, 0x01C0,
    0, 0,
    0, 0,
    0.0f,
    0.0f,
    0.0f,
    0.0f,
};

static struct
{
    Vec center;
    float radius;
} s_stageBounds;

void event_minimap_init(void)
{
    fn_1_D2AC4(g_init_common_gma_buf[1], 0x1BFFFFFF, 0x24000000);
    fn_1_D2AC4(g_init_common_gma_buf[2], 0x1FFFFFFF, 0xE0000000);
    fn_1_D2A88(g_init_common_gma_buf[3], 0xF8FFFFFF, 0x02000000);
    fn_1_D2AC4(g_init_common_gma_buf[3], 0x1FFFFFFF, 0xE0000000);
    fn_1_D2B00(g_init_common_gma_buf[3], 0x90EFFFFF, 0x08000000);
    if (sub_mode == 0xD)
    {
        lbl_1_data_502E0.screenY = 0x1A8;
        lbl_1_data_502E0.offsetY = -0.76666665077209473f;
    }
    else
    {
        lbl_1_data_502E0.screenY = 0x1C0;
        lbl_1_data_502E0.offsetY = -0.86666667461395264f;
    }
}

void event_minimap_tick(void)
{
    struct Ball *ball = current_ball;
    float radius;
    float scale;
    Vec center;

    if (g_some_other_flags & 0xA)
        return;
    if (lbl_1_data_502E0.mode == 0)
        return;

    switch (lbl_1_data_502E0.mode)
    {
    case 1:
        lbl_1_data_502E0.size -= 6;
        lbl_1_data_502E0.spinVel -= 71;
        if (lbl_1_data_502E0.size < 0)
        {
            lbl_1_data_502E0.mode = 0;
            lbl_1_data_502E0.size = 0;
        }
        break;
    case 2:
        lbl_1_data_502E0.spinVel -= lbl_1_data_502E0.spinVel >> 4;
        break;
    case 3:
        lbl_1_data_502E0.size += 8;
        lbl_1_data_502E0.spinVel += 33;
        if (lbl_1_data_502E0.size > 0x140)
        {
            lbl_1_data_502E0.mode = 2;
            lbl_1_data_502E0.size = 0x140;
        }
        break;
    }

    lbl_1_data_502E0.tiltX = lbl_1_data_502E0.size * 0x3000 / 0x140 + 0x1000;
    lbl_1_data_502E0.spin += lbl_1_data_502E0.spinVel;
    radius = s_stageBounds.radius;
    switch (lbl_1_data_502E0.zoom)
    {
    case 0:
        if (radius > 48.0f)
            scale = 0.05520833283662796f;
        else
            scale = 2.6500000000000004 / radius;
        center = ball->pos;
        lbl_1_data_502E0.bubbleTarget = 0.0f;
        lbl_1_data_502E0.itemsTarget = 0.4f;
        break;
    case 1:
        center = s_stageBounds.center;
        center.y = ball->pos.y;
        scale = 1.3250000000000002 / radius;
        lbl_1_data_502E0.bubbleTarget = 1.0f;
        lbl_1_data_502E0.itemsTarget = 0.0f;
        break;
    case 2:
    default:
        if (radius > 24.0f)
            scale = 0.22083333134651184f;
        else
            scale = 5.3000000000000007 / radius;
        center = ball->pos;
        lbl_1_data_502E0.bubbleTarget = 0.0f;
        lbl_1_data_502E0.itemsTarget = 0.6f;
        break;
    }

    lbl_1_data_502E0.scale += (scale - lbl_1_data_502E0.scale) * 0.1;
    lbl_1_data_502E0.center.x += (center.x - lbl_1_data_502E0.center.x) * 0.1;
    lbl_1_data_502E0.center.y += (center.y - lbl_1_data_502E0.center.y) * 0.1;
    lbl_1_data_502E0.center.z += (center.z - lbl_1_data_502E0.center.z) * 0.1;

    if (lbl_1_data_502E0.bubble < lbl_1_data_502E0.bubbleTarget)
    {
        lbl_1_data_502E0.bubble += 0.066666666666666666;
        if (lbl_1_data_502E0.bubble > lbl_1_data_502E0.bubbleTarget)
            lbl_1_data_502E0.bubble = lbl_1_data_502E0.bubbleTarget;
    }
    else if (lbl_1_data_502E0.bubble > lbl_1_data_502E0.bubbleTarget)
    {
        lbl_1_data_502E0.bubble -= 0.033333333333333333;
        if (lbl_1_data_502E0.bubble < lbl_1_data_502E0.bubbleTarget)
            lbl_1_data_502E0.bubble = lbl_1_data_502E0.bubbleTarget;
    }

    if (lbl_1_data_502E0.items < lbl_1_data_502E0.itemsTarget)
    {
        lbl_1_data_502E0.items += 0.066666666666666666;
        if (lbl_1_data_502E0.items > lbl_1_data_502E0.itemsTarget)
            lbl_1_data_502E0.items = lbl_1_data_502E0.itemsTarget;
    }
    else if (lbl_1_data_502E0.items > lbl_1_data_502E0.itemsTarget)
    {
        lbl_1_data_502E0.items -= 0.033333333333333333;
        if (lbl_1_data_502E0.items < lbl_1_data_502E0.itemsTarget)
            lbl_1_data_502E0.items = lbl_1_data_502E0.itemsTarget;
    }
}

void event_minimap_dest(void)
{
    lbl_1_data_502E0.mode = 0;
    lbl_1_data_502E0.size = 0;
    lbl_1_data_502E0.spinVel = 0;
    lbl_1_data_502E0.bubble = 0.0f;
}

void set_minimap_mode(int mode)
{
    switch (mode)
    {
    case 0:
        lbl_1_data_502E0.size = 0;
        lbl_1_data_502E0.bubble = 0.0f;
        break;
    case 2:
        lbl_1_data_502E0.size = 0x140;
        break;
    case 3:
        if (lbl_1_data_502E0.size == 0)
            lbl_1_data_502E0.spinVel = (rand() & 0x3FF) + 0x200;
        break;
    case 4:
        mode = lbl_1_data_502E0.mode;
        lbl_1_data_502E0.size = 0;
        lbl_1_data_502E0.bubble = 0.0f;
        lbl_1_data_502E0.zoom = 0;
        lbl_1_data_502E0.scale = 0.26499998569488525f;
        break;
    }

    if (lbl_1_data_502E0.mode == 0)
    {
        lbl_1_data_502E0.center.x = 0.0f;
        lbl_1_data_502E0.center.y = 0.0f;
        lbl_1_data_502E0.center.z = 0.0f;
    }
    lbl_1_data_502E0.mode = mode;
}

float g_lava_bumper_func1(void)
{
    return lbl_1_data_502E0.scale;
}

void fn_1_CB6CC(void)
{
    fn_1_57E14(&s_stageBounds.center, &s_stageBounds.radius);
}

void toggle_minimap_zoom(void)
{
    if (lbl_1_data_502E0.mode != 0)
    {
        lbl_1_data_502E0.zoom++;
        if (lbl_1_data_502E0.zoom > 2)
            lbl_1_data_502E0.zoom = 0;
    }
}

/* Clears the depth buffer behind the map; the coordinate variables are supplied by the caller. */
#define DRAW_DEPTH_QUAD(k, depth, x1, y1, x2, y2, z) \
{ \
    float scale_ = k; \
    x1 = aspectScale * ((left - 320) * scale_); \
    y1 = (240 - (top - 1)) * scale_; \
    x2 = aspectScale * ((right - 320) * scale_); \
    y2 = (240 - bottom) * scale_; \
    z = depth; \
\
    g_something_with_GX_vtx_desc(0x200); \
    GXSetBlendMode_cached(1, 0, 1, 0); \
    GXSetZMode_cached(1, 7, 1); \
    g_something_with_raster_state(0); \
    GXSetCullMode_cached(0); \
    g_GXSetTevIndirect_zero_if_different(0); \
    g_GXSetTevOrder_wrapper(0, 0xFF, 0xFF, 0xFF); \
    GXSetTevKAlphaSel_cached(0, 0); \
    GXSetTevColorIn_cached(0, 15, 15, 15, 15); \
    GXSetTevColorOp_cached(0, 0, 0, 0, 1, 0); \
    GXSetTevAlphaIn_cached(0, 7, 7, 7, 6); \
    GXSetTevAlphaOp_cached(0, 0, 0, 3, 1, 0); \
    GXSetNumTevStages_cached(1); \
    mtxa_push(); \
    mtxa_from_identity(); \
    GXLoadPosMtxImm(mtxa->mtxA, 0); \
    mtxa_pop(); \
    GXBegin(0x80, 0, 4); \
    GXPosition3f32(x2, y1, z); \
    GXPosition3f32(x1, y1, z); \
    GXPosition3f32(x1, y2, z); \
    GXPosition3f32(x2, y2, z); \
    GXSetZMode_cached(1, 3, 1); \
}

static u32 s_speechBubbleColors[] =
{
    0x00FFFFFF,
    0x00FFFFFF,
    0x00FFE080,
    0x00FFFFFF,
};

void g_draw_minimap(void)
{
    float aspectScale;
    float bubble;
    float opacity;
    float offsetY;
    float offsetX;
    float scale;
    float unused;
    struct Ball *ball;
    struct World *world;
    int left;
    int right;
    int top;
    int bottom;
    int scissorTop;
    int scissorBottom;
    s8 state;
    struct SpriteDrawRequest params;
    Mtx savedView;
    Mtx44 mapProj;
    Mtx44 screenProj;
    Vec vec;
    Vec ballPos;
    Vec dir;

    ball = current_ball;
    world = current_world_info;
    if ((monkey_flags & 0x200) && !(dip_switches & 0x40000000) && !(dip_switches & 0x80000000)
     && lbl_1_data_502E0.size != 0)
    {
        perf_init_timer(8);
        g_some_render_flag.flags |= 0x400;
        g_set_something3(0);
        switch (main_mode)
        {
        case 2:
            switch (modeCtrl.gameType)
            {
            case 0:
                camera_apply_viewport_2_if_changed(modeCtrl.currPlayer);
                break;
            default:
                camera_apply_viewport_2_if_changed(0);
                break;
            }
            break;
        default:
            camera_apply_viewport_2_if_changed(0);
            break;
        }

        unused = lbl_1_data_502E0.size * 0.0020833333333333333;
        left = lbl_1_data_502E0.screenX - (lbl_1_data_502E0.size >> 1);
        top = lbl_1_data_502E0.screenY - (lbl_1_data_502E0.size >> 1);
        right = left + lbl_1_data_502E0.size;
        bottom = top + lbl_1_data_502E0.size;
        if (right > 640)
            right = 640;
        if (bottom > 480)
            bottom = 480;
        scissorTop = top * current_render_mode->efbHeight / 480;
        scissorBottom = bottom * current_render_mode->efbHeight / 480;
        C_MTXPerspective(screenProj, 60.0f, camera_get_aspect_ratio(), 0.1f, 20000.0f);
        g_something_with_GXSetProjectionv(screenProj, 0);
        GXSetViewport(0.0f, 0.0f, current_render_mode->fbWidth, current_render_mode->efbHeight, 0.0f, 1.0f);
        GXSetScissor_cached(0, 0, current_render_mode->fbWidth, current_render_mode->efbHeight);

        aspectScale = modeCtrl.wideMode ? 1.3333333f : 1.0f;
        {
            float z, y1, x1, y2, x2;

            DRAW_DEPTH_QUAD(0.00024056263f, -0.1f, x1, y1, x2, y2, z);
        }
        mtx_copy(mtxa->mtxB, savedView);

        C_MTXPerspective(mapProj, 60.0f, camera_get_aspect_ratio(), 0.1f, 20000.0f);
        mapProj[0][2] += -0.57735026f * (mapProj[0][0] * lbl_1_data_502E0.offsetX * camera_get_aspect_ratio());
        mapProj[1][2] += -0.57735026f * (mapProj[1][1] * lbl_1_data_502E0.offsetY);
        g_something_with_GXSetProjectionv(mapProj, 0);
        push_light_group();
        mtxa_from_mtxb();
        g_set_active_light_group(1);
        nl2ngc_set_material_color(1.0f, 1.0f, 1.0f);

        if (minigameGma != NULL)
        {
            scale = (415.0 * lbl_1_data_502E0.size) / 320.0;
            mtxa_from_translate_xyz(0.0f, 0.0f, -1000.0f);
            mtxa_rotate_z(lbl_1_data_502E0.spin);
            mtxa_scale_xyz(scale, scale, scale);
            nl2ngc_set_scale(scale);
            nl2ngc_draw_model_sort_none(g_init_common_gma_buf[3]);

            scale = (0.46200000062584873 * lbl_1_data_502E0.size) / 320.0;
            mtxa_from_translate_xyz(0.0f, 0.0f, -1.1f);
            mtxa_rotate_z(lbl_1_data_502E0.spin);
            mtxa_scale_xyz(scale, scale, scale);
            g_smth_with_gpu_and_locked_cache_mtx();
            avdisp_set_scale_factor(scale);
            avdisp_set_z_mode(1, 7, 1);
            avdisp_draw_model_unculled_sort_never(minigameGma->modelEntries[0].model);
            avdisp_set_z_mode(1, 3, 1);
        }

        GXSetViewport(left, scissorTop, (float)right - (float)left, (float)scissorBottom - (float)scissorTop, 0.0f, 1.0f);
        GXSetScissor_cached(left, scissorTop, right - left, scissorBottom - scissorTop);

        offsetX = lbl_1_data_502E0.offsetX + lbl_1_data_502E0.centerX;
        offsetY = lbl_1_data_502E0.offsetY + lbl_1_data_502E0.centerY;
        scale = 0.057735026f;
        {
            float frustumTop = scale * (((240.0f - top) / 240.0f) - offsetY);
            float frustumBottom = scale * (((240.0f - bottom) / 240.0f) - offsetY);
            float frustumLeft = scale * (((left - 320.0f) / 320.0f - offsetX) * camera_get_aspect_ratio());

            C_MTXFrustum(
                mapProj,
                frustumTop,
                frustumBottom,
                frustumLeft,
                scale * (((right - 320.0f) / 320.0f - offsetX) * camera_get_aspect_ratio()),
                0.1f,
                20000.0f);
        }
        g_something_with_GXSetProjectionv(mapProj, 0);

        scale = lbl_1_data_502E0.scale;
        mtxa_from_identity();
        mtxa_rotate_x(lbl_1_data_502E0.tiltX);
        mtxa_rotate_y(-g_current_camera->rotY);
        if (!(g_current_camera->flags & CAMERA_FLAG_NO_STAGE_TILT) && events[EVENT_WORLD].state == 2)
        {
            mtxa_rotate_x(world->xrot);
            mtxa_rotate_z(world->zrot);
        }
        vec.x = 0.0f;
        vec.y = 0.0f;
        vec.z = -10.0f;
        mathutil_mtxA_set_translate(&vec);
        mtxa_scale_xyz(scale, scale, scale);
        vec = lbl_1_data_502E0.center;
        mtxa_translate_neg(&vec);
        mtxa_to_mtx(mtxa->mtxB);
        mtxa_scale_s(lbl_1_data_502E0.items / scale);
        mtxa_to_mtx(g_ord_tbl_stuff->matrices[2]);
        mtxa_from_mtxb();

        opacity = (lbl_1_data_502E0.size - 160) / 160.0f > 0.0f ? (lbl_1_data_502E0.size - 160) / 160.0f : 0.0f;

        params.unk30 = 2;
        params.addColor = 0;
        params.u1 = 0.0f;
        params.v1 = 0.0f;
        params.u2 = 1.0f;
        params.v2 = 1.0f;
        g_math_unk16(&ball->pos, &ballPos, 0.57735026f);

        if (lbl_1_data_502E0.bubble > 0.0)
        {
            bubble = lbl_1_data_502E0.bubble;
            ballPos.z = -2.0f;
            dir.x = -ballPos.y - (0.5 * ballPos.x);
            dir.y = ballPos.x - (0.5 * ballPos.y);
            dir.z = 0.0f;
            params.rotation = math_atan2(dir.y, (4.0 / 3.0) * dir.x) + 0x4000;
            if (dir.x == 0.0 && dir.y == 0.0)
                dir.y = 1.0f;
            vec_set_len(0.1125 * bubble, &dir, &dir);
            ballPos.x += dir.x;
            ballPos.y += dir.y;
            params.bmpId = 0x500;
            params.mulColor = s_speechBubbleColors[active_monkey_id[modeCtrl.currPlayer]];
            params.opacity = 0.75 * opacity;
            params.x = 320.0 + (320.0 * (ballPos.x + offsetX));
            params.y = 240.0 + (-240.0 * (ballPos.y + offsetY));
            params.z = -ballPos.z;
            params.scaleX = bubble;
            params.scaleY = bubble;
            params.flags = 0x2000A;
            draw_sprite_draw_request(&params);

            vec_set_len(0.025 * bubble, &dir, &dir);
            ballPos.x += dir.x;
            ballPos.y += dir.y;
            mtxa_translate(&ball->pos);
            mtxa_scale_xyz(1.25 * bubble, 1.25 * bubble, 1.25 * bubble);
            avdisp_set_scale_factor(1.25 * scale * bubble);
            set_post_mult_color(0.0f, 0.0f, 0.0f, 1.0f);
            load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
            avdisp_draw_model_culled_sort_auto(init_common_gma->modelEntries[30].model);
            set_post_mult_color(1.0f, 1.0f, 1.0f, 1.0f);
        }

        params.bmpId = g_smth_with_get_active_monkey_icon(0, 0, active_monkey_id[modeCtrl.currPlayer]);
        params.mulColor = 0xFFFFFFFF;
        params.opacity = (double)opacity;
        params.x = 320.0 + (320.0 * (ballPos.x + offsetX));
        params.y = 240.0 + (-240.0 * (ballPos.y + offsetY));
        params.z = -ballPos.z - 0.5;
        params.rotation = 0;
        params.scaleX = 0.28846154f;
        params.scaleY = 0.1875f;
        params.flags = 0x100100A;
        params.unk40 = params.x;
        draw_sprite_draw_request(&params);

        u_set_some_minimap_light_param(lbl_1_data_502E0.scale);
        mtxa_from_mtxb();
        g_set_active_light_group(3);
        state = events[EVENT_STAGE].state;
        if (state == 2 || state == 4)
            g_draw_stage();
        if (lbl_1_data_502E0.items > 0.0f && events[EVENT_ITEM].state == 2)
            draw_items();
        if (events[EVENT_STOBJ].state == 2)
            g_draw_stobjs();
        if (lbl_1_data_502E0.size == 320)
        {
            mtxa_from_mtxb();
            mtxa_translate(&ball->pos);
            mtxa_sq_from_identity();
            mtxa_rotate_z((g_font_char_var_3 << 8) + (g_font_char_var_3 << 7));
            mtxa_translate_xyz(0.0f, 0.0f, 0.5 * ball->modelScale * scale);
            mtxa_scale_xyz(
                2.0 * (ball->modelScale * scale),
                2.0 * (ball->modelScale * scale),
                2.0 * (ball->modelScale * scale));
            nl2ngc_set_scale(2.0 * (ball->modelScale * scale));
            nl2ngc_draw_model_sort_translucent_alt(g_init_common_gma_buf[1]);
        }
        ord_tbl_draw_nodes();
        g_something_with_GXSetProjectionv(screenProj, 0);
        GXSetViewport(0.0f, 0.0f, current_render_mode->fbWidth, current_render_mode->efbHeight, 0.0f, 1.0f);
        GXSetScissor_cached(0, 0, current_render_mode->fbWidth, current_render_mode->efbHeight);
        fade_color_base_default();
        DRAW_DEPTH_QUAD(48.11012f, -19999.0f, offsetX, offsetY, opacity, scale, bubble);
        camera_setup_default_projection();
        u_set_some_minimap_light_param(1.0f);
        pop_light_group();
        fn_1_E2D20();
        g_some_render_flag.flags &= ~0x400;
        perfInfo.unk24 += perf_stop_timer(8);
    }
}
