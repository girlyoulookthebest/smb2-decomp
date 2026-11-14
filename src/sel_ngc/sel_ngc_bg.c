/* sel_ngc_bg.c: Menu apes and menu background stage drawing. */
#include "sel_ngc.h"

/* sel_ngc.c's work area (bss 0x0-0x190, based at g_currently_visible_menu_screen).
 * TODO(core): replace with the work-area struct once sel_ngc.c defines one. */
#define MENU_WORK(type, offset) (*(type *)((u8 *)&g_currently_visible_menu_screen + (offset)))

/* ---- .bss ---- */
struct Ape *menu_apes[4];
s32 lbl_3_bss_1A8;  /* the menu apes have been loaded */
s32 lbl_3_bss_1AC;  /* disc queue distance right after the apes were queued */
s32 g_menu_color_overlay_timer;

void g_load_select_apes(void)
{
    int i;
    int group;

    for (i = 0; i < 4; i++)
        menu_apes[i] = NULL;
    lbl_3_bss_1A8 = 0;
    group = g_swapDiscQueueGroup(1);
    preload_ape(0, 0, 1, 0x19);
    preload_ape(1, 0, 1, 0x19);
    preload_ape(2, 0, 1, 0x19);
    preload_ape(3, 0, 1, 0x19);
    g_swapDiscQueueGroup(group);
    lbl_3_bss_1AC = disc_queue_distance(1);
    g_menu_color_overlay_timer = 0;
}

void fn_3_1CB58(void)
{
    if (lbl_3_bss_1A8 == 0 && disc_queue_distance(1) <= 0)
        g_load_some_apes();
}

void g_load_some_apes(void)
{
    if (lbl_3_bss_1A8 == 0 && disc_queue_distance(1) <= 0)
    {
        menu_apes[0] = load_ape(0, 0, 0x19, 1);
        menu_apes[1] = load_ape(1, 0, 0x19, 1);
        menu_apes[2] = load_ape(2, 0, 0x19, 1);
        menu_apes[3] = load_ape(3, 0, 0x19, 1);
        disc_queue_status(1);
        lbl_3_bss_1A8 = 1;
    }
}

/* Places the four menu apes in front of the menu camera and animates them.
 * Returns 0 when the ape sprite (unique id 0x3C) is gone or fully faded out. */
int fn_3_1CC74(void)
{
    float xOffsets[4] = { -1.62f, -0.55f, 0.55f, 1.62f };
    s16 yawOffsets[4] = { 0x1100, 0x0800, -0x0800, -0x1100 };
    float tilts[4] = { 0.1f, 0.03f, -0.03f, -0.1f };
    Vec pos;
    Vec dir;
    struct Sprite *sprite;
    float fade;
    int i;
    struct Ape *ape;
    struct Camera *camera;
    int anim;
    int j;
    u32 timer;
    int visible;

    sprite = get_sprite_with_unique_id(0x3C);
    if (sprite == NULL)
        return 0;
    fade = sprite->opacity * sprite->scaleY;
    for (i = 0; i < 4; i++)
    {
        ape = menu_apes[i];
        camera = &cameras[1];
        if (ape == NULL)
            continue;
        pos.x = xOffsets[i];
        pos.y = 0.25f;
        pos.z = -3.0f;
        mtxa_from_translate(&camera->eye);
        mtxa_rotate_y(camera->rotY);
        mtxa_rotate_x(camera->rotX);
        mtxa_tf_point(&pos, &ape->pos);
        pos.x = 0.0f;
        pos.y = 0.0f;
        pos.z = 1.0f;
        mtxa_from_rotate_y(camera->rotY + yawOffsets[i]);
        mtxa_tf_vec(&pos, &pos);
        dir = pos;
        if (0.0f == fade)
            continue;
        dir.y = 0.0f;
        vec_normalize_len(&dir);
        dir.x = -dir.x;
        dir.z = -dir.z;
        mtxa_from_identity();
        mtxa_rotate_y_sin_cos(dir.x, dir.z);
        mtxa_rotate_z_sin_cos(tilts[i], fade);
        ape->unk2A8.x = 1.0f;
        ape->unk2A8.y = 0.0f;
        ape->unk2A8.z = 0.0f;
        mtxa_to_quat(&ape->rot);
        anim = 0;
        timer = (&MENU_WORK(s32, 0x20))[i] % 60;
        for (j = 0; j < 4; j++)
        {
            if ((MENU_WORK(u8, 0x1C) & (1 << j)) && i == (s8)*fn_3_6A14(j))
                break;
        }
        if (timer != 0 && timer < 45)
            anim = 2;
        else if (j != 4)
            anim = 1;
        fn_1_1132D0(ape, 0x19, 0, anim);
        if (!(g_some_other_flags & 0xA))
            g_ape_anim_body(ape);
    }
    if (0.0f == fade)
        visible = FALSE;
    else
        visible = TRUE;
    return visible;
}

void menu_draw_background(void)
{
    struct Light savedLight;
    struct Color3f ambient;
    struct Color3f tmpAmbient;
    GXColor color;
    float scale;
    int i;
    struct Light *light;
    struct Ape *ape;
    s8 savedFov;

    camera_apply_viewport_if_changed(0);
    fn_1_D4870();
    g_maybe_some_bg_effect_handler();
    u_reset_light_group_stack(0);
    if (events[EVENT_REND_EFC].state == EV_STATE_RUNNING)
        g_something_with_rend_efc(4);
    if (current_stage_id != 3)
        g_draw_stage();
    switch (current_stage_id)
    {
    case 0x8A:
        g_draw_stage_0x8a_for_menu_bg();
        break;
    case 0x9F:
        g_draw_stage_0x9f_for_menu_bg();
        break;
    case 0xA0:
        g_draw_stage_for_menu_bg_2();
        break;
    }
    if (events[EVENT_BACKGROUND].state == EV_STATE_RUNNING)
    {
        g_smth_with_rendefc_reflective_height(400.0f);
        g_draw_bg();
        g_smth_with_rendefc_reflective_height(0.0f);
    }
    if (events[EVENT_REND_EFC].state == EV_STATE_RUNNING)
        g_something_with_rend_efc(0x10);
    if (events[EVENT_EFFECT].state == EV_STATE_RUNNING)
        g_draw_effects();
    if (world_theme.unk8 & 1)
        fn_1_D6F9C(0);
    ord_tbl_draw_nodes();
    if (world_theme.unk8 & 1)
        fn_1_D6D84(0);
    if (events[EVENT_REND_EFC].state == EV_STATE_RUNNING)
        g_something_with_rend_efc(8);

    if ((s8)g_currently_visible_menu_screen == 2 || g_is_screen_in_screen_stack(2))
    {
        if (g_menu_color_overlay_timer < 30)
            g_menu_color_overlay_timer++;
    }
    else if (g_menu_color_overlay_timer > 0)
        g_menu_color_overlay_timer--;
    if (g_menu_color_overlay_timer < 16)
        scale = 1.0f - 0.05f * g_menu_color_overlay_timer;
    else
        scale = 1.0f - 0.05f * (30.0f - g_menu_color_overlay_timer);
    color.r = 0x80;
    color.g = 0x80;
    color.b = 0x80;
    color.a = (u8)(int)(255.0f * ((60 - cameras[0].timerMax) / 60.0f) * scale);
    g_draw_menu_bg_overlay(&color, (g_menu_color_overlay_timer < 16) ? 4 : 1);
    fn_3_23080();

    if (fn_3_1CC74() != 0
     && ((u8)(g_currently_visible_menu_screen - 5) <= 1 || (s8)g_currently_visible_menu_screen == 0x43))
    {
        savedFov = g_related_to_fov2;
        if (modeCtrl.wideMode)
        {
            g_related_to_fov2 = 0;
            event_camera_tick();
        }
        camera_apply_viewport_if_changed(1);
        fn_1_D4870();
        g_maybe_some_bg_effect_handler();
        u_reset_light_group_stack(1);
        savedLight = s_lightPool[0];
        light = alloc_pool_light(8, 0);
        get_curr_light_group_ambient(&tmpAmbient);
        ambient = tmpAmbient;
        if (light != NULL)
            memcpy(&s_lightPool[0], light, sizeof(struct Light));
        g_set_light_color_maybe(2, 0.6f, 0.6f, 0.6f);
        mtxa_from_identity();
        g_set_active_light_group(2);
        s_lightPool[0] = savedLight;
        g_set_light_color_maybe(2, ambient.r, ambient.g, ambient.b);
        for (i = 0; i < 4; i++)
        {
            ape = menu_apes[i];
            if (ape != NULL)
            {
                mtxa_from_mtxb();
                draw_ape(ape);
            }
        }
        ord_tbl_draw_nodes();
        g_related_to_fov2 = savedFov;
    }
}

/* Copies the framebuffer into a half-size texture and draws it back over the whole
 * screen, multiplied by the given constant color (dims/tints the menu background). */
void g_draw_menu_bg_overlay(GXColor *color, int texFmt)
{
    GXTexObj texObj;
    float viewport[6];
    float proj[7];
    int left;
    int top;
    int width;
    int height;
    int halfWidth;
    int halfHeight;
    int tevStage;
    int texCoord;
    float invSx;
    float invSy;
    float x1;
    float x2;
    float y1;
    float y2;

    GXGetViewportv(viewport);
    left = viewport[0];
    top = viewport[1];
    width = viewport[2];
    height = viewport[3];
    halfWidth = width >> 1;
    halfHeight = height >> 1;
    GXSetTexCopySrc_cached(left, top, width, height);
    GXSetTexCopyDst_cached(halfWidth, halfHeight, texFmt, 1);
    GXCopyTex(lbl_801988E0, 0);
    GXInitTexObj(&texObj, lbl_801988E0, halfWidth, halfHeight, texFmt, 0, 0, 0);
    GXInvalidateTexAll();
    GXLoadTexObj_cached(&texObj, 0);
    g_something_with_GX_vtx_desc(0x2200);
    GXSetBlendMode_cached(1, 4, 5, 0);
    GXSetZMode_cached(1, 7, 1);
    g_something_with_raster_state(0);
    GXSetCullMode_cached(0);
    GXSetTevKColor_cached(0, *color);

    tevStage = 0;
    texCoord = 0;
    GXSetTexCoordGen2_cached(texCoord, 1, 4, 0x3C, 0, 0x7D);
    g_GXSetTevOrder_wrapper(tevStage, texCoord, 0, 0xFF);
    GXSetTevKColorSel_cached(tevStage, 0xC);
    GXSetTevKAlphaSel_cached(tevStage, 0x1C);
    GXSetTevColorIn_cached(tevStage, 0xF, 0xE, 8, 0xF);
    GXSetTevColorOp_cached(tevStage, 0, 0, 0, 1, 0);
    GXSetTevAlphaIn_cached(tevStage, 7, 7, 7, 6);
    GXSetTevAlphaOp_cached(tevStage, 0, 0, 0, 1, 0);
    g_GXSetTevIndirect_zero_if_different(tevStage);
    tevStage++;
    texCoord++;
    GXSetNumTevStages_cached(tevStage);
    GXSetNumTexGens_cached(texCoord);
    GXSetNumIndStages_cached(0);
    GXSetNumChans_cached(0);

    mtxa_push();
    mtxa_from_identity();
    GXLoadPosMtxImm(mtxa->mtxA, 0);
    mtxa_pop();

    GXGetProjectionv(proj);
    invSx = 1.0f / proj[1];
    invSy = 1.0f / proj[3];
    x1 = -10000.0f * (-1.0f - proj[2]) * invSx;
    x2 = -10000.0f * (1.0f - proj[2]) * invSx;
    y1 = -10000.0f * (-1.0f - proj[4]) * invSy;
    y2 = -10000.0f * (1.0f - proj[4]) * invSy;
    GXBegin(0x80, 0, 4);  /* GX_QUADS, GX_VTXFMT0 */
    GXPosition3f32(x1, y1, -10000.0f);
    GXTexCoord2f32(1.0f, 0.0f);
    GXPosition3f32(x2, y1, -10000.0f);
    GXTexCoord2f32(0.0f, 0.0f);
    GXPosition3f32(x2, y2, -10000.0f);
    GXTexCoord2f32(0.0f, 1.0f);
    GXPosition3f32(x1, y2, -10000.0f);
    GXTexCoord2f32(1.0f, 1.0f);
    GXEnd();
}

void free_some_apes(void)
{
    int i;

    for (i = 0; i < 4; i++)
    {
        g_something_with_freeing_chara_heap_ape(menu_apes[i]);
        menu_apes[i] = NULL;
    }
    disc_queue_status(1);
}

void g_draw_stage_0x9f_for_menu_bg(void)
{
    mtxa_from_mtxb();
    load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
    avdisp_draw_model_culled_sort_never(stage_gma->modelEntries[10].model);
    mtxa_translate_xyz(0.0f, 3.5f, -100.896f);
    load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
    avdisp_draw_model_culled_sort_never(stage_gma->modelEntries[9].model);
}

void g_draw_stage_0x8a_for_menu_bg(void)
{
    void *prevFunc;

    prevFunc = g_yet_another_unk_draw_func(fn_3_1DAA0);
    mtxa_from_mtxb();
    load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
    avdisp_draw_model_unculled_sort_never(stage_gma->modelEntries[34].model);
    avdisp_draw_model_unculled_sort_never(stage_gma->modelEntries[35].model);
    avdisp_draw_model_unculled_sort_never(stage_gma->modelEntries[36].model);
    avdisp_draw_model_unculled_sort_never(stage_gma->modelEntries[37].model);
    avdisp_draw_model_unculled_sort_never(stage_gma->modelEntries[38].model);
    g_yet_another_unk_draw_func(prevFunc);
}

void g_draw_stage_for_menu_bg_2(void)
{
    mtxa_from_mtxb();
    load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
    avdisp_draw_model_culled_sort_never(stage_gma->modelEntries[18].model);
}

/* Material callback for stage 0x8A's models: scrolls texture matrices 0x1E and 0x21. */
void fn_3_1DAA0(struct BallEnvArg *arg)
{
    struct TevStageInfo tev = arg->tev;

    mtxa_push();
    mtxa_from_translate_xyz(0.002f * g_font_char_var_3, 0.00375f * g_font_char_var_3, 0.0f);
    GXLoadTexMtxImm(mtxa->mtxA, 0x1E, 1);
    mtxa_from_translate_xyz(-0.002f * g_font_char_var_3, 0.00375f * g_font_char_var_3, 0.0f);
    GXLoadTexMtxImm(mtxa->mtxA, 0x21, 1);
    mtxa_pop();
    g_GXSetTevIndirect_zero_if_different(0);
    GXSetTexCoordGen2_cached(0, 1, 4, 0x1E, 0, 0x7D);
    GXSetTexCoordGen2_cached(1, 1, 4, 0x21, 0, 0x7D);
}
