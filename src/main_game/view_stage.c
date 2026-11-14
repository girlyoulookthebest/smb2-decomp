/* "View Stage" pause menu option (SMB1's view.c). */
#include "types.h"
#include "avdisp.h"
#include "background.h"
#include "ball.h"
#include "camera.h"
#include "course.h"
#include "event.h"
#include "frustum.h"
#include "gx.h"
#include "input.h"
#include "libc.h"
#include "light.h"
#include "mathutil.h"
#include "mode.h"
#include "nl2ngc.h"
#include "sprite.h"
#include "stage.h"
#include "stcoli.h"
#include "ape_internal.h"

struct StageBanana
{
    /*0x00*/ Vec pos;
    /*0x0C*/ s32 type;
}; /* size = 0x10 */

struct StageBumper
{
    /*0x00*/ Vec pos;
    /*0x0C*/ s16 rotX;
    /*0x0E*/ s16 rotY;
    /*0x10*/ s16 rotZ;
    u8 unk12[0x20 - 0x12];
}; /* size = 0x20 */

struct StageJamabar
{
    /*0x00*/ Vec pos;
    /*0x0C*/ s16 rotX;
    /*0x0E*/ s16 rotY;
    /*0x10*/ s16 rotZ;
    u8 unk12[0x20 - 0x12];
}; /* size = 0x20 */

struct StageButton
{
    /*0x00*/ Vec pos;
    /*0x0C*/ s16 rotX;
    /*0x0E*/ s16 rotY;
    /*0x10*/ s16 rotZ;
    /*0x12*/ s16 type;
    u8 unk14[0x18 - 0x14];
}; /* size = 0x18 */

struct PauseInputState
{
    u8 filler0[0x14];
    /*0x14*/ s8 padId;
};

struct StageViewInfo
{
    /*0x00*/ Vec eye;
    /*0x0C*/ Vec target;
    /*0x18*/ s16 rotX;
    /*0x1A*/ s16 rotY;
    /*0x1C*/ s16 rotZ;
    /*0x1E*/ s16 frameCounter;
    /*0x20*/ float unk20;
    /*0x24*/ float unk24;
    /*0x28*/ struct Sphere stageBoundSphere;
    /*0x38*/ s16 unk38;
    /*0x3A*/ s16 unk3A;
    /*0x3C*/ s16 unk3C;
    /*0x3E*/ s16 unk3E;
    /*0x40*/ float unk40;
    /*0x44*/ float unk44;
}; /* size = 0x48 */

extern struct ItemGroup *itemgroups_copy;
extern struct PauseInputState g_related_to_pause_menu_input;
extern u32 g_some_render_flag;
extern int locale;
extern void * DPTR *g_init_common_gma_buf;  /* NlObj image: 4-byte slots */
extern struct GMAModel *goal_gma_models[];
extern struct GMAModel *wormhole_gma_model;
extern struct GMAModel *worm_surface_gma_model;
extern struct GMAModel *lbl_1_bss_9AB5C;
extern struct GMAModel *lbl_1_bss_9AB60[];
extern struct GMAModel *lbl_1_bss_9AB38[];

void g_something_with_rend_efc(int);
void fn_1_D4870(void);
void g_maybe_some_bg_effect_handler(void);
void g_draw_bg(void);
void ord_tbl_draw_nodes(void);
void textdraw_reset(void);
void textdraw_set_font(int);
void textdraw_set_flags(u32);
void set_ui_widescreen_scale_mtx(int);
void reset_ui_widescreen_scale_mtx(void);
void textdraw_set_alignment(int);
void textdraw_set_pos(float, float);
void textdraw_print(char *);
void destroy_sprite_with_unique_id(int);
void init_itemgroups(void);
void g_advance_stage_animation(void);
int is_stage_id_348_revolution(void);
void g_special_handler_for_st348_revolution(void);
void g_handle_hardcoded_special_case_stages(void);
void g_draw_collision_triangles(void);
void get_curr_stage_fly_in_position(struct Sphere *);

void fn_2_1F730(void);
void fn_2_1F76C(void);
void fn_2_1F790(void);
void g_call_advance_stage_animation(void);
void fn_2_1F7D0(void);
void fn_2_1F7EC(void);
void g_increment_bg_related_timer(void);
void fn_2_1F830(void);
void view_stage_draw_bananas(void);
void fn_2_1FA14(void);
void g_smth_with_rendering_models_for_reflective_surfaces(void);
void g_draw_stobjs_in_viewstage(void);
void fn_2_207F4(struct Sphere *);
void g_something_with_view_stage_camera(struct Camera *camera);

char *lbl_2_data_563C0[] = { "Stage Overview", "", "", "", "", "\x83\x58\x83\x65\x81\x5B\x83\x57\x82\xBA\x82\xF1\x82\xAF\x82\xA2" };
char *lbl_2_data_56404[] = {
    "\x83\xAD" "Rotate  \x83\xA6" "Pan camera", "", "", "", "",
    "\x83\xAD\x82\xA2\x82\xC7\x82\xA4 \x83\xA6\x82\xAD\x82\xD1\x82\xD3\x82\xE8",
};

struct StageViewInfo view_stage_camera_x;

void event_view_init(void)
{
    memset(&view_stage_camera_x, 0, sizeof(view_stage_camera_x));
    fn_2_207F4(&view_stage_camera_x.stageBoundSphere);
    view_stage_camera_x.unk20 = 1.1313 * view_stage_camera_x.stageBoundSphere.radius;
    view_stage_camera_x.unk24 = 0.75f;
    view_stage_camera_x.unk38 = -0x1600;
    view_stage_camera_x.unk3A = 0;
    memcpy(itemgroups_copy, itemgroups, stagedef->itemGroupCount * sizeof(struct ItemGroup));
    if (modeCtrl.gameType == 1)
        camera_setup_singleplayer_viewport();
    fn_2_1F790();
    fn_2_1F7EC();
    fn_2_1F730();
}

void event_view_tick(void)
{
    float dx;
    float dy;
    float dz;
    int cstickY;
    int cstickX;

    g_call_advance_stage_animation();
    g_increment_bg_related_timer();
    view_stage_camera_x.unk3A += pad_status_groups[g_related_to_pause_menu_input.padId].unk0[0].stickX * 5;
    view_stage_camera_x.unk24 -= pad_status_groups[g_related_to_pause_menu_input.padId].unk0[0].stickY * 0.0003;
    if (view_stage_camera_x.unk24 > 1.0)
        view_stage_camera_x.unk24 = 1.0f;
    else if (view_stage_camera_x.unk24 < 0.5)
        view_stage_camera_x.unk24 = 0.5f;
    cstickY = pad_status_groups[g_related_to_pause_menu_input.padId].unk0[0].substickY * 64;
    cstickX = -pad_status_groups[g_related_to_pause_menu_input.padId].unk0[0].substickX * 0xC0;
    view_stage_camera_x.unk3C += (cstickY - view_stage_camera_x.unk3C) >> 5;
    view_stage_camera_x.unk3E += (cstickX - view_stage_camera_x.unk3E) >> 5;
    mtxa_from_translate(&view_stage_camera_x.stageBoundSphere.pos);
    mtxa_rotate_y(view_stage_camera_x.unk3A);
    mtxa_rotate_x(view_stage_camera_x.unk38);
    mtxa_translate_xyz(0.0f, 0.0f, view_stage_camera_x.unk20 * view_stage_camera_x.unk24);
    mtxa_rotate_x(-view_stage_camera_x.unk38);
    mtxa_rotate_y(view_stage_camera_x.unk3E);
    mtxa_rotate_x(view_stage_camera_x.unk38);
    mtxa_rotate_x(view_stage_camera_x.unk3C);
    mtxa_tf_point_xyz(0.0f, 0.0f, 0.0f, &view_stage_camera_x.eye);
    mtxa_tf_point_xyz(0.0f, 0.0f, -10.0f, &view_stage_camera_x.target);
    dx = view_stage_camera_x.target.x - view_stage_camera_x.eye.x;
    dy = view_stage_camera_x.target.y - view_stage_camera_x.eye.y;
    dz = view_stage_camera_x.target.z - view_stage_camera_x.eye.z;
    view_stage_camera_x.rotY = math_atan2(dx, dz) - 0x8000;
    dx *= dx;
    dx = __fmadds(dz, dz, dx);
    view_stage_camera_x.rotX = math_atan2(dy, math_sqrt(dx));
    view_stage_camera_x.rotZ = 0;
    view_stage_camera_x.frameCounter++;
    g_font_char_var_3++;
}

void event_view_dest(void)
{
    fn_2_1F7D0();
    fn_2_1F830();
    fn_2_1F76C();
    memcpy(itemgroups, itemgroups_copy, stagedef->itemGroupCount * sizeof(struct ItemGroup));
    if (modeCtrl.gameType == 1)
    {
        if (modeCtrl.gameType == 1 && modeCtrl.playerCount == 3)
        {
            switch (modeCtrl.splitscreenMode)
            {
            default:
            case 0:
                camera_setup_splitscreen_viewports(3);
                break;
            case 1:
                set_camera_view_settings(0, 0.0f, 0.5f, 0.5f, 0.5f);
                set_camera_view_settings(1, 0.0f, 0.0f, 1.0f, 0.5f);
                set_camera_view_settings(2, 0.5f, 0.5f, 0.5f, 0.5f);
                set_camera_view_settings(3, 0.0f, 0.0f, 0.0f, 0.0f);
                break;
            case 2:
                set_camera_view_settings(0, 0.0f, 0.0f, 0.5f, 0.5f);
                set_camera_view_settings(1, 0.5f, 0.0f, 0.5f, 0.5f);
                set_camera_view_settings(2, 0.0f, 0.5f, 1.0f, 0.5f);
                set_camera_view_settings(3, 0.0f, 0.0f, 0.0f, 0.0f);
                break;
            case 3:
                camera_setup_splitscreen_viewports(4);
                break;
            }
        }
        else
            camera_setup_splitscreen_viewports(modeCtrl.playerCount);
    }
}

void g_something_with_view_stage2(void)
{
    struct Camera *camera;
    struct Camera *savedCamera;
    s32 savedCamera3;
    struct Ball *savedBall;
    struct Camera cameraBackup = cameras[modeCtrl.currPlayer];
    Mtx44 projMtx;
    s8 state;

    savedCamera = g_current_camera;
    savedCamera3 = g_something_with_camera3;
    camera = &cameras[modeCtrl.currPlayer];
    g_current_camera = camera;
    g_something_with_camera3 = camera->idx;
    g_some_render_flag |= 2;
    g_something_with_view_stage_camera(camera);
    C_MTXPerspective(projMtx, 59.99633789f, 1.33333333f, 0.1f, 20000.0f);
    g_something_with_GXSetProjectionv(projMtx, 0);
    {
        S16Vec rotation = {0};
        S16Vec *rot;

        rotation.x = view_stage_camera_x.rotX;
        rotation.y = view_stage_camera_x.rotY;
        rotation.z = view_stage_camera_x.rotZ;
        rot = &rotation;
        fn_1_22D64(&view_stage_camera_x.eye, rot, 59.99633789f, 1.33333333f, 0.0f, 0.0f);
    }
    light_main();
    savedBall = current_ball;
    current_ball = balls;
    if (events[EVENT_REND_EFC].state == 2)
        g_something_with_rend_efc(1);
    g_something_with_view_stage_camera(camera);
    fn_1_D4870();
    g_maybe_some_bg_effect_handler();
    u_reset_light_group_stack(0);
    if (events[EVENT_REND_EFC].state == 2)
        g_something_with_rend_efc(4);
    g_something_with_view_stage_camera(camera);
    state = events[EVENT_STAGE].state;
    if (state == 2 || state == 4)
        g_smth_with_rendering_models_for_reflective_surfaces();
    if (events[EVENT_ITEM].state == 2)
        fn_2_1FA14();
    if (events[EVENT_BACKGROUND].state == 2)
    {
        g_smth_with_rendefc_reflective_height(400.0f);
        g_draw_bg();
        g_smth_with_rendefc_reflective_height(0.0f);
    }
    if (events[EVENT_REND_EFC].state == 2)
        g_something_with_rend_efc(0x10);
    g_something_with_view_stage_camera(camera);
    if (events[EVENT_ITEM].state == 2)
        view_stage_draw_bananas();
    if (events[EVENT_STOBJ].state == 2)
        g_draw_stobjs_in_viewstage();
    ord_tbl_draw_nodes();
    if (events[EVENT_REND_EFC].state == 2)
        g_something_with_rend_efc(8);
    g_something_with_view_stage_camera(camera);
    current_ball = savedBall;
    camera_setup_default_projection();
    g_some_render_flag &= ~2;
    g_current_camera = savedCamera;
    g_something_with_camera3 = savedCamera3;
    cameras[modeCtrl.currPlayer] = cameraBackup;
}

void draw_reflective_objects_in_viewstage(void)
{
    s8 state;

    state = events[EVENT_STAGE].state;
    if (state == 2 || state == 4)
        g_smth_with_rendering_models_for_reflective_surfaces();
    if (events[EVENT_BACKGROUND].state == 2)
    {
        g_smth_with_rendefc_reflective_height(400.0f);
        g_draw_bg();
        g_smth_with_rendefc_reflective_height(0.0f);
    }
    if (events[EVENT_ITEM].state == 2)
        view_stage_draw_bananas();
    if (events[EVENT_STOBJ].state == 2)
        g_draw_stobjs_in_viewstage();
    ord_tbl_draw_nodes();
}

void fn_2_1F66C(struct Sprite *sprite)
{
    textdraw_reset();
    textdraw_set_font(3);
    textdraw_set_flags(0xA0000000);
    set_ui_widescreen_scale_mtx(0x60);
    textdraw_set_alignment(0);
    textdraw_set_pos(24.0f, 24.0f);
    textdraw_print(lbl_2_data_563C0[locale]);
    set_ui_widescreen_scale_mtx(0x220);
    textdraw_set_alignment(8);
    textdraw_set_pos(616.0f, 456.0f);
    textdraw_print(lbl_2_data_56404[locale]);
    reset_ui_widescreen_scale_mtx();
}

void fn_2_1F730(void)
{
    struct Sprite *sprite = create_sprite();

    if (sprite != NULL)
    {
        sprite->tag = 100;
        sprite->drawFunc = fn_2_1F66C;
    }
}

void fn_2_1F76C(void)
{
    destroy_sprite_with_unique_id(100);
}

void fn_2_1F790(void)
{
    init_itemgroups();
}

void g_call_advance_stage_animation(void)
{
    g_advance_stage_animation();
}

void fn_2_1F7D0(void)
{
    g_some_frame_counter.timer = view_stage_camera_x.unk40;
}

void fn_2_1F7EC(void)
{
    view_stage_camera_x.unk44 = world_theme.animTimer;
}

void g_increment_bg_related_timer(void)
{
    world_theme.animTimer += 1.0;
}

void fn_2_1F830(void)
{
    world_theme.animTimer = view_stage_camera_x.unk44;
}

void view_stage_draw_bananas(void)
{
    struct StageBanana *banana;
    int count;
    int i;
    int j;
    s32 spinSpeeds[2] = { 0x400, 0x300 };

    if (modeCtrl.gameType != 1 || is_bonus_stage(current_stage_id) || (modeCtrl.courseFlags & 0x1000))
    {
        struct GMAModel *models[2];

        models[0] = init_common_gma->modelEntries[0x41].model;
        models[1] = init_common_gma->modelEntries[0x44].model;
        for (i = 0; i < stagedef->itemGroupCount; i++)
        {
            banana = stagedef->collHeaders[i].bananas;
            count = stagedef->collHeaders[i].bananaCount;
            for (j = 0; j < count; j++, banana++)
            {
                mtxa_from_mtx(itemgroups[i].transform);
                mtxa_translate(&banana->pos);
                mtxa_sq_from_identity();
                mtxa_rotate_y(view_stage_camera_x.frameCounter * spinSpeeds[banana->type]);
                mtxa_mult_left(mtxa->mtxB);
                load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
                avdisp_draw_model_culled_sort_auto(models[banana->type]);
            }
        }
    }
}

void fn_2_1FA14(void)
{
    struct StageBanana *banana;
    int count;
    int i;
    int j;
    Mtx mtx;
    struct RaycastHit hit;
    Quat quat;
    Vec pos;
    Vec up = { 0.0f, 0.0f, -1.0f };

    if ((modeCtrl.gameType != 1 || is_bonus_stage(current_stage_id) || (modeCtrl.courseFlags & 0x1000))
     && stagedef->bananaCount > 0)
    {
        avdisp_set_z_mode(1, 3, 0);
        for (i = 0; i < stagedef->itemGroupCount; i++)
        {
            banana = stagedef->collHeaders[i].bananas;
            count = stagedef->collHeaders[i].bananaCount;
            for (j = 0; j < count; j++, banana++)
            {
                mtxa_from_mtx(itemgroups[i].transform);
                mtxa_tf_point(&banana->pos, &pos);
                if ((int)raycast_stage_down(&pos, &hit, NULL) != 0)
                {
                    float height;
                    float alpha;

                    mtxa_from_mtx(mtxa->mtxB);
                    pos.x = hit.pos.x + 0.02 * hit.normal.x;
                    pos.y = hit.pos.y + 0.02 * hit.normal.y;
                    pos.z = hit.pos.z + 0.02 * hit.normal.z;
                    mtxa_translate(&pos);
                    mtxa_to_mtx(mtx);
                    quat_from_dirs(&quat, &up, &hit.normal);
                    mtxa_from_quat(&quat);
                    mtxa_mult_left(mtx);
                    if (banana->type != 0)
                        mtxa_scale_s(0.8f);
                    else
                        mtxa_scale_s(0.45f);
                    height = banana->pos.y - hit.pos.y;
                    if (height > 5.0)
                        continue;
                    if (height > 1.0)
                        alpha = (5.0 - height) / 4.0;
                    else
                        alpha = 1.0f;
                    set_post_mult_color(0.3 * alpha, 0.3 * alpha, 0.3 * alpha, 0.3 * alpha);
                    load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
                    avdisp_draw_model_culled_sort_auto(init_common_gma->modelEntries[0x67].model);
                }
            }
        }
        set_post_mult_color(1.0f, 1.0f, 1.0f, 1.0f);
        avdisp_set_z_mode(1, 3, 1);
    }
}

void g_smth_with_rendering_models_for_reflective_surfaces(void)
{
    struct ItemGroup *itemgroup;
    struct StageCollHeader *coll;
    int i;
    struct StageModelInstance *inst;
    int j;
    struct StageModelRef * DPTR *refs;
    struct GMAModel *model;
    float scale;

    if (is_stage_id_348_revolution())
    {
        g_special_handler_for_st348_revolution();
        mtxa_from_mtxb();
        mtxa_translate(&stagedef->startPos->pos);
        mtxa_rotate_y(view_stage_camera_x.frameCounter << 9);
        load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
        avdisp_draw_model_culled_sort_auto(init_common_gma->modelEntries[0x4D].model);
        return;
    }
    nl2ngc_set_material_color(1.0f, 1.0f, 1.0f);
    mtxa_from_mtxb();
    mtxa_translate(&stagedef->startPos->pos);
    mtxa_rotate_y(view_stage_camera_x.frameCounter << 9);
    load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
    avdisp_draw_model_culled_sort_auto(init_common_gma->modelEntries[0x4D].model);
    fade_color_base_default();
    if (stage_gma != NULL)
    {
        itemgroup = itemgroups;
        coll = stagedef->collHeaders;
        for (i = 0; i < stagedef->itemGroupCount; i++, itemgroup++, coll++)
        {
            u32 mask;

            mtxa_from_mtxb();
            if (i > 0)
                mtxa_mult_right(itemgroup->transform);
            load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
            refs = coll->modelRefs;
            for (j = 0; j < coll->modelRefCount; j++, refs++)
            {
                if (!((*refs)->flags & 8))
                {
                    struct GMAModel *lod = g_some_draw_func2(*refs);

                    if (lod != NULL)
                        avdisp_draw_model_culled_sort_never(lod);
                }
            }
            mask = g_smth_with_challenge_mode_var_and_3p();
            inst = stagedef->collHeaders[i].instances;
            mtxa_push();
            for (j = 0; j < stagedef->collHeaders[i].instanceCount; j++, inst++)
            {
                if (!(inst->flags & mask))
                {
                    mtxa_peek();
                    mtxa_translate(&inst->pos);
                    mtxa_rotate_z(inst->rotZ);
                    mtxa_rotate_y(inst->rotY);
                    mtxa_rotate_x(inst->rotX);
                    mtxa_scale(&inst->scale);
                    scale = (inst->scale.x > inst->scale.y) ? inst->scale.x : inst->scale.y;
                    scale = (inst->scale.z > scale) ? inst->scale.z : scale;
                    avdisp_set_scale_factor(scale);
                    model = g_some_draw_func4(inst->modelRef, scale);
                    if (model != NULL)
                    {
                        if (g_is_sphere_visible(&model->boundSphereCenter, model->boundSphereRadius * scale))
                        {
                            load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
                            avdisp_draw_model_unculled_sort_never(model);
                        }
                    }
                }
            }
            mtxa_pop();
        }
    }
    if (g_some_draw_var != NULL)
    {
        struct DynamicStagePart *dyn;

        mtxa_from_mtxb();
        dyn = g_some_draw_var;
        while (dyn->modelName != NULL)
        {
            nl2ngc_draw_model_sort_none(dyn->tempModel);
            dyn++;
        }
    }
    g_handle_hardcoded_special_case_stages();
    g_draw_collision_triangles();
}

void g_draw_stobjs_in_viewstage(void)
{
    Mtx mtx;
    int count;
    struct StageGoal *goal;
    int i;
    int j;
    struct GMAModel *goalModel;
    struct StageWormhole *worm;
    struct GMAModel *wormModel;
    struct GMAModel *surfaceModel;
    struct StageBumper *bumper;
    int total;
    struct StageJamabar *jamabar;
    struct StageButton *button;

    for (i = 0; i < stagedef->itemGroupCount; i++)
    {
        goal = stagedef->collHeaders[i].goals;
        count = stagedef->collHeaders[i].goalCount;
        mtxa_from_mtxb();
        if (i > 0)
            mtxa_mult_right(itemgroups[i].transform);
        mtxa_to_mtx(mtx);
        for (j = 0; j < count; j++, goal++)
        {
            mtxa_from_mtx(mtx);
            mtxa_translate(&goal->pos);
            mtxa_rotate_z(goal->rotZ);
            mtxa_rotate_y(goal->rotY);
            mtxa_rotate_x(goal->rotX);
            goalModel = goal_gma_models[goal->type];
            if (goalModel != NULL)
            {
                load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
                avdisp_draw_model_culled_sort_auto(goalModel);
            }
            nl2ngc_draw_model_sort_translucent(g_init_common_gma_buf[4]);
            mtxa_push();
            mtxa_translate_xyz(0.0f, 2.8f, 0.0f);
            load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
            avdisp_draw_model_culled_sort_auto(init_common_gma->modelEntries[0x3E].model);
            mtxa_pop();
            mtxa_push();
            load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
            avdisp_draw_model_culled_sort_auto(init_common_gma->modelEntries[0x4E].model);
            mtxa_translate_xyz(-0.45f, 0.0f, 0.0f);
            load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
            avdisp_draw_model_culled_sort_auto(init_common_gma->modelEntries[0x4E].model);
            mtxa_pop();
            load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
            avdisp_draw_model_culled_sort_auto(init_common_gma->modelEntries[0x32].model);
            mtxa_translate_xyz(-0.6666f, 0.0f, 0.0f);
            load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
            avdisp_draw_model_culled_sort_auto(init_common_gma->modelEntries[0x32].model);
            mtxa_translate_xyz(-0.6666f, 0.0f, 0.0f);
            load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
            avdisp_draw_model_culled_sort_auto(init_common_gma->modelEntries[0x32].model);
        }
    }

    for (i = 0; i < stagedef->itemGroupCount; i++)
    {
        if (g_some_render_flag & 0x80)
            break;
        worm = stagedef->collHeaders[i].wormholes;
        count = stagedef->collHeaders[i].wormholeCount;
        wormModel = wormhole_gma_model;
        surfaceModel = worm_surface_gma_model;
        mtxa_from_mtxb();
        if (i > 0)
            mtxa_mult_right(itemgroups[i].transform);
        mtxa_to_mtx(mtx);
        for (j = 0; j < count; j++, worm++)
        {
            mtxa_from_mtx(mtx);
            mtxa_translate(&worm->pos);
            mtxa_rotate_z(worm->rotZ);
            mtxa_rotate_y(worm->rotY);
            mtxa_rotate_x(worm->rotX);
            if (wormModel != NULL)
            {
                load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
                avdisp_draw_model_culled_sort_auto(wormModel);
                if (surfaceModel != NULL)
                    avdisp_draw_model_culled_sort_auto(surfaceModel);
            }
        }
    }

    for (i = 0; i < stagedef->itemGroupCount; i++)
    {
        bumper = stagedef->collHeaders[i].bumpers;
        count = stagedef->collHeaders[i].bumperCount;
        mtxa_from_mtxb();
        if (i > 0)
            mtxa_mult_right(itemgroups[i].transform);
        mtxa_to_mtx(mtx);
        for (j = 0; j < count; j++, bumper++)
        {
            mtxa_from_mtx(mtx);
            mtxa_translate(&bumper->pos);
            mtxa_rotate_z(bumper->rotZ);
            mtxa_rotate_y(bumper->rotY);
            mtxa_rotate_x(bumper->rotX);
            mtxa_rotate_y(view_stage_camera_x.frameCounter << 8);
            load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
            avdisp_draw_model_culled_sort_auto(lbl_1_bss_9AB38[5]);
        }
    }

    total = 0;
    for (i = 0; i < stagedef->itemGroupCount; i++)
    {
        jamabar = stagedef->collHeaders[i].jamabars;
        count = stagedef->collHeaders[i].jamabarCount;
        mtxa_from_mtxb();
        if (i > 0)
            mtxa_mult_right(itemgroups[i].transform);
        mtxa_to_mtx(mtx);
        for (j = 0; j < count; j++, jamabar++)
        {
            float t;

            mtxa_from_mtx(mtx);
            mtxa_translate(&jamabar->pos);
            mtxa_rotate_z(jamabar->rotZ);
            mtxa_rotate_y(jamabar->rotY);
            mtxa_rotate_x(jamabar->rotX);
            t = (float)((view_stage_camera_x.frameCounter - total * (60 / stagedef->jamabarCount)) % 60) / 60.0f;
            t = 2.0 * t;
            if (t >= 1.0)
                t = 2.0 - t;
            mtxa_translate_xyz(0.0f, 0.0f, 2.5 * -t);
            load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
            avdisp_draw_model_culled_sort_auto(lbl_1_bss_9AB5C);
            total++;
        }
    }

    for (i = 0; i < stagedef->itemGroupCount; i++)
    {
        button = stagedef->collHeaders[i].buttons;
        count = stagedef->collHeaders[i].buttonCount;
        mtxa_from_mtxb();
        if (i > 0)
            mtxa_mult_right(itemgroups[i].transform);
        mtxa_to_mtx(mtx);
        for (j = 0; j < count; j++, button++)
        {
            mtxa_from_mtx(mtx);
            mtxa_translate(&button->pos);
            mtxa_rotate_z(button->rotZ);
            mtxa_rotate_y(button->rotY);
            mtxa_rotate_x(button->rotX);
            load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
            avdisp_draw_model_culled_sort_auto(lbl_1_bss_9AB60[button->type]);
        }
    }
}

void fn_2_207F4(struct Sphere *sphere)
{
    get_curr_stage_fly_in_position(sphere);
}

void g_something_with_view_stage_camera(struct Camera *camera)
{
    camera->eye = view_stage_camera_x.eye;
    camera->rotX = view_stage_camera_x.rotX;
    camera->rotY = view_stage_camera_x.rotY;
    camera->rotZ = view_stage_camera_x.rotZ;
    mtxa_from_identity();
    mtxa_rotate_z(-camera->rotZ);
    mtxa_rotate_x(-camera->rotX);
    mtxa_rotate_y(-camera->rotY);
    mtxa_translate_neg(&camera->eye);
    mtx_copy(mtxa->mtxA, camera->unk14C);
    mtx_copy(mtxa->mtxA, mtxa->mtxB);
    mtx_copy(mtxa->mtxA, camera->unk17C);
    mtx_copy(mtxa->mtxA, camera->unk1AC);
    mtx_copy(mtxa->mtxA, camera->unk1DC);
    mtx_copy(mtxa->mtxA, mtxa->mtxB);
    mtx_copy(mtxa->mtxA, g_ord_tbl_stuff->matrices[2]);
    mtx_copy(mtxa->mtxA, g_ord_tbl_stuff->matrices[3]);
    mtx_copy(mtxa->mtxA, g_ord_tbl_stuff->matrices[0]);
    mtx_copy(mtxa->mtxA, g_ord_tbl_stuff->matrices[4]);
    mtx_copy(mtxa->mtxA, g_ord_tbl_stuff->matrices[1]);
}
