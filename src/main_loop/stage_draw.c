/* Stage drawing (SMB1 stage_draw): stage models, goals, wormholes and stage-specific effects. */
#include "types.h"
#include "avdisp.h"
#include "ball.h"
#include "camera.h"
#include "dip.h"
#include "frustum.h"
#include "light.h"
#include "mathutil.h"
#include "mode.h"
#include "nl2ngc.h"
#include "stage.h"
#include "pc_hfr.h"

/* Request handed to the shadow renderer (SMB1 Struct80092F90). */
struct ShadowReceive
{
    /*0x00*/ void *obj;
    /*0x04*/ u16 unk4;
    /*0x06*/ u16 unk6;
    /*0x08*/ void *model;
};

/* Translucent wormhole queued in the ordering table. */
struct WormholeDrawNode
{
    /*0x00*/ struct OrdTblNode node;
    /*0x08*/ float alpha;
    /*0x0C*/ Mtx mtx;
    /*0x3C*/ struct GMAModel *model;
};

extern struct GMAModel *wormhole_gma_model;
extern struct GMAModel *goal_gma_models[3];
extern u32 g_some_render_flag;
extern u32 monkey_flags;
extern s32 g_font_char_var_3;  /* global frame counter */
extern u32 perfInfo[];

int g_check_some_condition(int a);
void g_some_shadow_draw_func(struct ShadowReceive *s);
void g_something_with_texture_scroll(void *scroll);
void g_something_with_texture_scroll_2(void *scroll);
void g_smth_with_rendefc_reflective_height(float height);
u32 g_smth_with_challenge_mode_var_and_3p(void);
void *g_yet_another_unk_draw_func(void *func);
void avdisp_set_post_mult_color(float r, float g, float b, float a);
struct OrdTblNode *fn_1_CCDC8(Vec *pos);
struct OrdTblNode *fn_1_CCC18(float depth);
void *ord_tbl_alloc_node(u32 size);
void fn_1_CD418(struct OrdTblNode *list, struct OrdTblNode *node);  /* ord_tbl_insert_node */
void g_draw_stage_collision(void);
void g_smth_with_gpu_and_locked_cache_mtx(void);

void g_handle_hardcoded_special_case_stages(void);
void g_draw_collision_triangles(void);
void fn_1_59F0C(struct GMAModel *model, float alpha);
void fn_1_59FB4(struct WormholeDrawNode *node);
void fn_1_5A650(struct OrdTblNode *node);
BOOL is_stage_id_348_revolution(void);
void g_special_handler_for_st348_revolution(void);
void g_special_load_for_st348(void);
void g_smth_hardcoded_for_meltingpot(void);
void g_smth_hardcoded_for_totalitarianism_and_meltingpot(void);
void g_smth_hardcoded_for_st340(void);
void g_smth_hardcoded_for_trainworm_and_nintendo(void);

/* Translucent panels drawn on st340; flags bit 0 = always draw. */
struct St340Panel
{
    /*0x00*/ s8 modelIdx;
    /*0x04*/ Vec pos;
    /*0x10*/ Vec normal;
    /*0x1C*/ u8 flags;
};

extern struct BgLightInfo lbl_1_bss_2F48;  /* light.c's s_bgLightInfo */

/* Positions of the shadows drawn on totalitarianism, relative to item group 1. */
Vec lbl_1_data_2F060[16] =
{
    { 6.0f, 14.5f, -157.0f },
    { -8.0f, 14.5f, -154.0f },
    { -1.0f, 14.5f, -146.0f },
    { 5.0f, 14.5f, -136.0f },
    { -8.0f, 14.5f, -131.0f },
    { 6.0f, 14.5f, -120.0f },
    { -6.0f, 14.5f, -119.0f },
    { 10.0f, 14.5f, -108.0f },
    { -1.0f, 14.5f, -108.0f },
    { -10.0f, 14.5f, -102.0f },
    { -1.0f, 14.5f, -94.0f },
    { -10.0f, 14.5f, -87.0f },
    { 6.0f, 14.5f, -84.0f },
    { -4.0f, 14.5f, -78.0f },
    { 7.0f, 14.5f, -72.0f },
    { -6.0f, 14.5f, -66.0f },
};

struct St340Panel lbl_1_data_2F120[7] =
{
    { 3, { 0.0f, 0.0f, 0.0f }, { 1.0f, 0.0f, 0.0f }, 0 },
    { 7, { 0.0f, 3.75f, 0.0f }, { 0.0f, 1.0f, 0.0f }, 0 },
    { 8, { 0.0f, 0.0f, 0.0f }, { -1.0f, 0.0f, 0.0f }, 0 },
    { 9, { 0.0f, -3.75f, 0.0f }, { 0.0f, -1.0f, 0.0f }, 0 },
    { 10, { 0.0f, 0.0f, 5.0f }, { 0.0f, 0.0f, 1.0f }, 0 },
    { 11, { 0.0f, 0.0f, -5.0f }, { 0.0f, 0.0f, -1.0f }, 0 },
    { 12, { 0.0f, -3.75f, 0.0f }, { 0.0f, -1.0f, 0.0f }, 0 },
};

void fn_1_5A8E0(Vec *cam, Vec *ball);
void fn_1_5AB64(Vec *cam, Vec *ball);
void fn_1_5ADB8(struct OrdTblNode *node);
void fn_1_5AFB0(struct OrdTblNode *node);

#define MAX(a, b) ((a) > (b) ? (a) : (b))

void g_init_smth_with_stage_models(void)
{
    struct StageModelRef * DPTR *refs;
    int i;

    refs = stagedef->modelRefs;
    for (i = 0; i < stagedef->modelRefCount; i++, refs++)
    {
        if ((*refs)->flags & 8)
        {
            g_some_frame_counter.flags |= 8;
            break;
        }
    }
}

void g_draw_stage_models(void)
{
    struct ItemGroup *ig;
    struct StageCollHeader *coll;
    struct StageModelRef * DPTR *refs;
    int j;
    int i;
    BOOL mtxLoaded;
    int shadow;
    struct GMAModel *model;
    struct ShadowReceive s;

    s.unk6 = 6;
    shadow = g_check_some_condition(6);
    ig = itemgroups;
    coll = stagedef->collHeaders;
    for (i = 0; i < stagedef->itemGroupCount; i++, ig++, coll++)
    {
        struct StageModelInstance *inst;
        u32 mask;

        if ((g_some_frame_counter.flags & 0x10)
         && (coll->unkA7 & (1 << (g_some_frame_counter.unkD - 1))))
            continue;
        HFR_TAG(HFR_K_STAGE_IG, i);
        mtxa_from_mtxb();
        if (i > 0)
            mtxa_mult_right(ig->transform);
        mtxLoaded = FALSE;
        refs = coll->modelRefs;
        for (j = 0; j < coll->modelRefCount; j++, refs++)
        {
            if ((*refs)->flags & 8)
                continue;
            model = g_some_draw_func2(*refs);
            if (model == NULL)
                continue;
            if (!g_is_sphere_visible(&model->boundSphereCenter, model->boundSphereRadius))
                continue;
            if (coll->textureScroll != NULL)
                g_something_with_texture_scroll(coll->textureScroll);
            if (!mtxLoaded)
            {
                load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
                mtxLoaded = TRUE;
            }
            if ((*refs)->flags & 0x10)
            {
                g_smth_with_rendefc_reflective_height(400.0f);
                avdisp_draw_model_unculled_sort_always(model);
                g_smth_with_rendefc_reflective_height(0.0f);
            }
            else
                avdisp_draw_model_unculled_sort_never(model);
            if (coll->textureScroll != NULL)
                g_something_with_texture_scroll_2(NULL);
            if (shadow)
            {
                s.obj = refs;
                if ((*refs)->flags & 2)
                    s.unk4 = 4;
                else
                    s.unk4 = 2;
                s.model = model;
                g_some_shadow_draw_func(&s);
            }
        }

        mask = g_smth_with_challenge_mode_var_and_3p();
        inst = stagedef->collHeaders[i].instances;
        mtxa_push();
        for (j = 0; j < stagedef->collHeaders[i].instanceCount; j++, inst++)
        {
            float scale;

            if (inst->flags & mask)
                continue;
            mtxa_peek();
            mtxa_translate(&inst->pos);
            mtxa_rotate_z(inst->rotZ);
            mtxa_rotate_y(inst->rotY);
            mtxa_rotate_x(inst->rotX);
            mtxa_scale(&inst->scale);
            scale = MAX(inst->scale.x, inst->scale.y);
            scale = MAX(inst->scale.z, scale);
            avdisp_set_scale_factor(scale);
            model = g_some_draw_func4(inst->modelRef, scale);
            if (model == NULL)
                continue;
            if (!g_is_sphere_visible(&model->boundSphereCenter, model->boundSphereRadius * scale))
                continue;
            load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
            avdisp_draw_model_unculled_sort_never(model);
            if (shadow)
            {
                s.obj = inst;
                if (inst->modelRef->flags & 2)
                    s.unk4 = 4;
                else
                    s.unk4 = 2;
                s.model = model;
                g_some_shadow_draw_func(&s);
            }
        }
        mtxa_pop();
        HFR_UNTAG();
    }
    g_handle_hardcoded_special_case_stages();
    if (g_some_draw_var != NULL)
    {
        struct DynamicStagePart *dyn;

        mtxa_from_mtxb();
        for (dyn = g_some_draw_var; dyn->modelName != NULL; dyn++)
        {
            if (dyn->tempModel != NULL)
            {
                nl2ngc_draw_model_sort_none(dyn->tempModel);
                if (shadow)
                {
                    s.unk6 = 0;
                    s.model = dyn->tempModel;
                    g_some_shadow_draw_func(&s);
                }
            }
        }
    }
    g_draw_collision_triangles();
}

void g_draw_goalpost_models(void)
{
    struct ItemGroup *ig;
    struct StageCollHeader *coll;
    int i;
    struct GMAModel *model;
    struct StageGoal *goal;
    int j;
    int shadow;
    struct ShadowReceive s;
    Mtx mtx;

    s.unk4 = 0x40;
    s.unk6 = 4;
    shadow = g_check_some_condition(0x40);
    ig = itemgroups;
    coll = stagedef->collHeaders;
    for (i = 0; i < stagedef->itemGroupCount; i++, ig++, coll++)
    {
        if (coll->goalCount == 0)
            continue;
        mtxa_from_mtxb();
        if (i > 0)
            mtxa_mult_right(ig->transform);
        mtxa_to_mtx(mtx);
        goal = coll->goals;
        for (j = 0; j < coll->goalCount; j++, goal++)
        {
            mtxa_from_mtx(mtx);
            mtxa_translate(&goal->pos);
            mtxa_rotate_z(goal->rotZ);
            mtxa_rotate_y(goal->rotY);
            mtxa_rotate_x(goal->rotX);
            model = goal_gma_models[goal->type];
            if (model == NULL)
                continue;
            if (!g_is_sphere_visible(&model->boundSphereCenter, model->boundSphereRadius))
                continue;
            load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
            avdisp_draw_model_unculled_sort_auto(model);
            s.model = model;
            if (shadow)
            {
                s.obj = goal;
                s.model = model;
                g_some_shadow_draw_func(&s);
            }
        }
    }
}

/* Fades a wormhole out as the camera gets close to it. */
float fn_1_59704(struct GMAModel *model)
{
    float radius;
    Vec pos;
    Vec dir;
    float dist;
    float alpha;

    radius = model->boundSphereRadius;
    mtxa_tf_point(&model->boundSphereCenter, &pos);
    mtxa_tf_vec_xyz(0.0f, 0.0f, -1.0f, &dir);
    dist = mathutil_vec_len(&pos);
    radius = radius * (1.0 / (1.0 + 3.0 * fabs(vec_dot_normalized_safe(&dir, &pos))));
    alpha = 1.0f;
    if (dist < 0.1f + radius)
        alpha *= 0.2f;
    else if (dist < 0.1f + (2.5f + radius))
    {
        float t = 0.4f * (dist - (0.1f + radius));
        alpha *= 0.2f + 0.8f * t;
    }
    return alpha;
}

void g_draw_wormhole_models(void)
{
    struct GMAModel *model;
    struct ItemGroup *ig;
    struct StageCollHeader *coll;
    int i;
    struct StageWormhole *wormhole;
    int j;
    int shadow;
    struct ShadowReceive s;
    Mtx mtx;

    model = wormhole_gma_model;
    if (model != NULL && stagedef->wormholeCount != 0 && !(g_some_render_flag & 0x80))
    {
        s.unk4 = 0x40;
        s.unk6 = 4;
        shadow = g_check_some_condition(0x40);
        ig = itemgroups;
        coll = stagedef->collHeaders;
        for (i = 0; i < stagedef->itemGroupCount; i++, ig++, coll++)
        {
            if (coll->wormholeCount == 0)
                continue;
            mtxa_from_mtxb();
            if (i > 0)
                mtxa_mult_right(ig->transform);
            mtxa_to_mtx(mtx);
            wormhole = coll->wormholes;
            for (j = 0; j < coll->wormholeCount; j++, wormhole++)
            {
                float alpha;

                mtxa_from_mtx(mtx);
                mtxa_translate(&wormhole->pos);
                mtxa_rotate_z(wormhole->rotZ);
                mtxa_rotate_y(wormhole->rotY);
                mtxa_rotate_x(wormhole->rotX);
                if (model == NULL)
                    continue;
                if (!g_is_sphere_visible(&model->boundSphereCenter, model->boundSphereRadius))
                    continue;
                alpha = fn_1_59704(model);
                if (alpha > 0.0f)
                {
                    if (alpha < 1.0f)
                        fn_1_59F0C(model, alpha);
                    else
                    {
                        load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
                        avdisp_draw_model_unculled_sort_never(model);
                        if (alpha == 1.0f)
                        {
                            s.obj = wormhole;
                            s.model = model;
                            if (shadow)
                                g_some_shadow_draw_func(&s);
                        }
                    }
                }
            }
        }
    }
}

void g_draw_generator_models(void)
{
    struct GMAModel *model;
    struct ItemGroup *ig;
    struct StageCollHeader *coll;
    int i;
    struct StageGenerator *gen;
    int j;
    Mtx mtx;

    model = init_common_gma->modelEntries[35].model;
    if (stagedef->generatorCount == 0)
        return;
    ig = itemgroups;
    coll = stagedef->collHeaders;
    for (i = 0; i < stagedef->itemGroupCount; i++, ig++, coll++)
    {
        if (coll->generatorCount == 0)
            continue;
        mtxa_from_mtxb();
        if (i > 0)
            mtxa_mult_right(ig->transform);
        mtxa_to_mtx(mtx);
        gen = coll->generators;
        for (j = 0; j < coll->generatorCount; j++, gen++)
        {
            float sx = 1.0f;
            float sy = 1.0f;
            float sz = 1.0f;
            float r = 1.0f;
            float g = 1.0f;
            float b = 1.0f;
            float a = 0.0f;
            float t;
            float len;

            t = (float)g_some_frame_counter.frameCounter / 60.0;
            len = gen->loopEnd - gen->loopStart;
            t -= len * mathutil_floor(t / len);
            t += gen->loopStart;
            if (gen->scaleX.frames)
                sx = evaluate_stagedef_keyframe(t, gen->scaleX.count, gen->scaleX.frames);
            if (gen->scaleY.frames)
                sy = evaluate_stagedef_keyframe(t, gen->scaleY.count, gen->scaleY.frames);
            if (gen->scaleZ.frames)
                sz = evaluate_stagedef_keyframe(t, gen->scaleZ.count, gen->scaleZ.frames);
            if (gen->red.frames)
            {
                r = evaluate_stagedef_keyframe(t, gen->red.count, gen->red.frames);
                r = (r < 0.0) ? 0.0 : ((r > 1.0) ? 1.0 : r);
            }
            if (gen->green.frames)
            {
                g = evaluate_stagedef_keyframe(t, gen->green.count, gen->green.frames);
                g = (g < 0.0) ? 0.0 : ((g > 1.0) ? 1.0 : g);
            }
            if (gen->blue.frames)
            {
                b = evaluate_stagedef_keyframe(t, gen->blue.count, gen->blue.frames);
                b = (b < 0.0) ? 0.0 : ((b > 1.0) ? 1.0 : b);
            }
            if (gen->alpha.frames)
                a = evaluate_stagedef_keyframe(t, gen->alpha.count, gen->alpha.frames);
            mtxa_from_mtx(mtx);
            mtxa_translate(&gen->pos);
            mtxa_rotate_z(gen->rotZ);
            mtxa_rotate_y(gen->rotY);
            mtxa_rotate_x(gen->rotX);
            mtxa_scale_xyz(sx, sy, sz);
            avdisp_set_post_mult_color(r, g, b, 1.0 - a);
            if (model && g_is_sphere_visible(&model->boundSphereCenter, model->boundSphereRadius))
            {
                load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
                avdisp_draw_model_unculled_sort_auto(model);
            }
        }
    }
    fade_color_base_default();
}

void fn_1_59F0C(struct GMAModel *model, float alpha)
{
    Vec pos;
    struct WormholeDrawNode *node;
    struct OrdTblNode *list;

    mtxa_tf_point(&model->boundSphereCenter, &pos);
    list = fn_1_CCC18(pos.z + model->boundSphereRadius);
    node = ord_tbl_alloc_node(sizeof(*node));
    if (node != NULL)
    {
        node->node.drawFunc = (void *)fn_1_59FB4;
        node->alpha = alpha;
        node->model = model;
        mtxa_to_mtx(node->mtx);
        fn_1_CD418(list, &node->node);
    }
}

void fn_1_59FB4(struct WormholeDrawNode *node)
{
    mtxa_from_mtx(node->mtx);
    load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
    avdisp_set_alpha(node->alpha);
    avdisp_draw_model_unculled_sort_never(node->model);
}

void g_draw_start_position_marker(void)
{
    if (sub_mode == 0x31 && !(g_some_render_flag & 2))
    {
        nl2ngc_set_material_color(1.0f, 1.0f, 1.0f);
        if (g_some_render_flag & 8)
        {
            mtxa_from_identity();
            mtxa_scale_s(0.8f);
            mtxa_mult_right(mtxa->mtxB);
            mtxa_translate(&stagedef->startPos->pos);
            mtxa_rotate_y(stagedef->startPos->yrot);
            mtxa_rotate_x(0x4000);
            mtxa_rotate_y(g_font_char_var_3 << 9);
            mtxa_scale_s(2.0f);
        }
        else
        {
            mtxa_from_mtxb();
            mtxa_translate(&stagedef->startPos->pos);
            mtxa_rotate_y(-g_font_char_var_3 << 9);
        }
        load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
        if (mode_info.attempts == 1)
        {
            if (modeCtrl.submodeTimer > 120)
                avdisp_draw_model_culled_sort_auto(init_common_gma->modelEntries[77].model);
            else if (modeCtrl.submodeTimer > 60)
            {
                avdisp_set_alpha((modeCtrl.submodeTimer - 60) / 60.0f);
                avdisp_draw_model_culled_sort_always(init_common_gma->modelEntries[77].model);
            }
        }
        else
        {
            if (modeCtrl.submodeTimer > 75)
                avdisp_draw_model_culled_sort_auto(init_common_gma->modelEntries[77].model);
            else if (modeCtrl.submodeTimer > 45)
            {
                avdisp_set_alpha((modeCtrl.submodeTimer - 45) / 30.0f);
                avdisp_draw_model_culled_sort_always(init_common_gma->modelEntries[77].model);
            }
        }
    }
}

/* SMB1's backgroundInfo; only the field used here is named. */
struct WorldTheme
{
    u8 unk0[0x8C];
    /*0x8C*/ void *drawFunc;
};

void g_draw_stage(void)
{
    void *drawFunc;
    Mtx mtx;

    if (!(g_some_render_flag & 4) && !(dip_switches & DIP_NO_STAGE) && (monkey_flags & 1))
    {
        perf_init_timer(8);
        drawFunc = ((struct WorldTheme *)&world_theme)->drawFunc;
        if (drawFunc != NULL)
        {
            if (!(g_some_render_flag & 0x80))
                g_yet_another_unk_draw_func(drawFunc);
            else
                drawFunc = NULL;
        }
        g_draw_goalpost_models();
        g_draw_wormhole_models();
        g_draw_generator_models();
        if (dip_switches & DIP_TRIANGLE)
        {
            mtxa_from_mtxb();
            mtxa_rotate_x(-0x4000);
            mtxa_scale_xyz(10.0f, 10.0f, 10.0f);
            avdisp_set_scale_factor(10.0f);
            load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
            avdisp_draw_model_culled_sort_auto(init_common_gma->modelEntries[89].model);
        }
        else if (is_stage_id_348_revolution())
            g_special_handler_for_st348_revolution();
        else
        {
            g_draw_stage_models();
            g_draw_start_position_marker();
        }
        if (drawFunc != NULL)
            g_yet_another_unk_draw_func(NULL);
        if (dip_switches & DIP_STCOLI)
        {
            struct ItemGroup *ig;
            struct StageCollHeader *coll;
            int i;
            struct FalloutVolume *vol;
            int j;

            mtx_copy(mtxa->mtxB, mtx);
            ig = itemgroups;
            coll = stagedef->collHeaders;
            for (i = 0; i < stagedef->itemGroupCount; i++, ig++, coll++)
            {
                mtxa_from_mtx(mtx);
                mtxa_mult_right(ig->transform);
                mtxa_to_mtx(mtxa->mtxB);
                vol = coll->falloutVolumes;
                for (j = 0; j < coll->falloutVolumeCount; j++, vol++)
                {
                    float scale;

                    mtxa_from_mtxb();
                    mtxa_translate(&vol->pos);
                    mtxa_rotate_z(vol->rotZ);
                    mtxa_rotate_y(vol->rotY);
                    mtxa_rotate_x(vol->rotX);
                    mtxa_scale(&vol->scale);
                    scale = MAX(vol->scale.x, vol->scale.y);
                    scale = MAX(scale, vol->scale.z);
                    avdisp_set_scale_factor(scale);
                    avdisp_set_alpha(0.5f);
                    load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
                    avdisp_draw_model_culled_sort_always(init_common_gma->modelEntries[39].model);
                }
            }
            mtx_copy(mtx, mtxa->mtxB);
        }
        perfInfo[0] += perf_stop_timer(8);
    }
}

void g_draw_collision_triangles(void)
{
    Vec pos;
    struct OrdTblNode *list;
    struct OrdTblNode *node;

    if ((g_some_frame_counter.flags & 8) || (dip_switches & DIP_STCOLI))
    {
        pos.x = 0.0f;
        pos.y = 0.0f;
        pos.z = 0.0f;
        list = fn_1_CCDC8(&pos);
        node = ord_tbl_alloc_node(sizeof(*node));
        if (node != NULL)
        {
            node->drawFunc = fn_1_5A650;
            fn_1_CD418(list, node);
        }
    }
}

void fn_1_5A650(struct OrdTblNode *node)
{
    struct ItemGroup *ig;
    int i;
    struct StageCollHeader *coll;
    struct StageModelRef * DPTR *refs;
    int j;
    struct GMAModel *model;
    Mtx savedB;
    Mtx scaleMtx;

    avdisp_set_z_mode(1, 3, 0);
    ig = itemgroups;
    coll = stagedef->collHeaders;
    for (i = 0; i < stagedef->itemGroupCount; i++, ig++, coll++)
    {
        mtxa_from_mtxb();
        if (i > 0)
            mtxa_mult_right(ig->transform);
        load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
        refs = coll->modelRefs;
        for (j = 0; j < coll->modelRefCount; j++, refs++)
        {
            if (!((*refs)->flags & 8))
                continue;
            model = g_some_draw_func2(*refs);
            if (model == NULL)
                continue;
            if (coll->textureScroll != NULL)
                g_something_with_texture_scroll(coll->textureScroll);
            avdisp_draw_model_culled_sort_never(model);
            if (coll->textureScroll != NULL)
                g_something_with_texture_scroll_2(NULL);
        }
    }
    avdisp_set_z_mode(1, 3, 1);
    if (dip_switches & DIP_STCOLI)
    {
        mtx_copy(mtxa->mtxB, savedB);
        mtxa_from_identity();
        mtxa_scale_s(0.99f);
        mtxa_to_mtx(scaleMtx);
        mtxa_from_mtxb();
        mtxa_mult_left(scaleMtx);
        mtxa_to_mtx(mtxa->mtxB);
        g_draw_stage_collision();
        mtx_copy(savedB, mtxa->mtxB);
    }
}

BOOL is_stage_id_348_revolution(void)
{
    switch (current_stage_id)
    {
    case 348:
        return TRUE;
    default:
        return FALSE;
    }
}

void g_special_handler_for_st348_revolution(void)
{
    g_special_load_for_st348();
}

void g_handle_hardcoded_special_case_stages(void)
{
    switch (current_stage_id)
    {
    case 0x1E:
    case 0x107:
        g_smth_hardcoded_for_meltingpot();
        break;
    case 0xF:
    case 0xE5:
        g_smth_hardcoded_for_totalitarianism_and_meltingpot();
        break;
    case 0x154:
        g_smth_hardcoded_for_st340();
        break;
    case 0x140:
        g_smth_hardcoded_for_trainworm_and_nintendo();
        break;
    }
}

#define DRAW_STAGE_MODEL(i) avdisp_draw_model_unculled_sort_never(stage_gma->modelEntries[i].model)

void fn_1_5A8E0(Vec *cam, Vec *ball)
{
    if (cam->y > 8.0)
    {
        if (ball->y >= -8.0)
            DRAW_STAGE_MODEL(3);
        if (ball->y >= 0.0)
            DRAW_STAGE_MODEL(4);
        if (ball->y >= 8.0)
            DRAW_STAGE_MODEL(5);
    }
    else if (cam->y > 0.0)
    {
        if (ball->y >= -8.0)
            DRAW_STAGE_MODEL(3);
        if (ball->y >= 0.0)
            DRAW_STAGE_MODEL(4);
        if (ball->y <= 8.0)
            DRAW_STAGE_MODEL(5);
    }
    else if (cam->y > -8.0)
    {
        if (ball->y <= 8.0)
            DRAW_STAGE_MODEL(5);
        if (ball->y <= 0.0)
            DRAW_STAGE_MODEL(4);
        if (ball->y >= -8.0)
            DRAW_STAGE_MODEL(3);
    }
    else
    {
        if (ball->y <= 8.0)
            DRAW_STAGE_MODEL(5);
        if (ball->y <= 0.0)
            DRAW_STAGE_MODEL(4);
        if (ball->y <= -8.0)
            DRAW_STAGE_MODEL(3);
    }
}

void fn_1_5AB64(Vec *cam, Vec *ball)
{
    if (cam->y > 8.0)
    {
        if (ball->y < -8.0)
            DRAW_STAGE_MODEL(3);
        if (ball->y < 0.0)
            DRAW_STAGE_MODEL(4);
        if (ball->y < 8.0)
            DRAW_STAGE_MODEL(5);
    }
    else if (cam->y > 0.0)
    {
        if (ball->y < -8.0)
            DRAW_STAGE_MODEL(3);
        if (ball->y < 0.0)
            DRAW_STAGE_MODEL(4);
        if (ball->y > 8.0)
            DRAW_STAGE_MODEL(5);
    }
    else if (cam->y > -8.0)
    {
        if (ball->y > 8.0)
            DRAW_STAGE_MODEL(5);
        if (ball->y > 0.0)
            DRAW_STAGE_MODEL(4);
        if (ball->y < -8.0)
            DRAW_STAGE_MODEL(3);
    }
    else
    {
        if (ball->y > 8.0)
            DRAW_STAGE_MODEL(5);
        if (ball->y > 0.0)
            DRAW_STAGE_MODEL(4);
        if (ball->y > -8.0)
            DRAW_STAGE_MODEL(3);
    }
}

void fn_1_5ADB8(struct OrdTblNode *node)
{
    struct ItemGroup *ig;
    Vec camPos;
    Vec ballPos;

    ig = &itemgroups[1];
    mtxa_from_translate(&ig->pos);
    mtxa_rotate_z(ig->rot.z);
    mtxa_rotate_y(ig->rot.y);
    mtxa_rotate_x(ig->rot.x);
    mtxa_rigid_inv_tf_point(&g_current_camera->eye, &camPos);
    mtxa_rigid_inv_tf_point(&current_ball->pos, &ballPos);
    mtxa_from_mtxb();
    mtxa_mult_right(itemgroups[1].transform);
    g_smth_with_gpu_and_locked_cache_mtx();
    if (camPos.x > 40.0)
    {
        if (ballPos.x >= -40.0)
            DRAW_STAGE_MODEL(6);
        fn_1_5A8E0(&camPos, &ballPos);
        if (ballPos.x >= 40.0)
            DRAW_STAGE_MODEL(7);
    }
    else if (camPos.x > -40.0)
    {
        DRAW_STAGE_MODEL(6);
        DRAW_STAGE_MODEL(7);
        fn_1_5A8E0(&camPos, &ballPos);
    }
    else
    {
        if (ballPos.x <= 40.0)
            DRAW_STAGE_MODEL(7);
        fn_1_5A8E0(&camPos, &ballPos);
        if (ballPos.x <= -40.0)
            DRAW_STAGE_MODEL(6);
    }
}

void fn_1_5AFB0(struct OrdTblNode *node)
{
    struct ItemGroup *ig;
    Vec camPos;
    Vec ballPos;

    ig = &itemgroups[1];
    mtxa_from_translate(&ig->pos);
    mtxa_rotate_z(ig->rot.z);
    mtxa_rotate_y(ig->rot.y);
    mtxa_rotate_x(ig->rot.x);
    mtxa_rigid_inv_tf_point(&g_current_camera->eye, &camPos);
    mtxa_rigid_inv_tf_point(&current_ball->pos, &ballPos);
    mtxa_from_mtxb();
    mtxa_mult_right(itemgroups[1].transform);
    g_smth_with_gpu_and_locked_cache_mtx();
    if (camPos.x > 40.0)
    {
        if (ballPos.x < -40.0)
            DRAW_STAGE_MODEL(6);
        fn_1_5AB64(&camPos, &ballPos);
        if (ballPos.x < 40.0)
            DRAW_STAGE_MODEL(7);
    }
    else if (camPos.x > -40.0)
    {
        DRAW_STAGE_MODEL(6);
        DRAW_STAGE_MODEL(7);
        fn_1_5AB64(&camPos, &ballPos);
    }
    else
    {
        if (ballPos.x > 40.0)
            DRAW_STAGE_MODEL(7);
        fn_1_5AB64(&camPos, &ballPos);
        if (ballPos.x > -40.0)
            DRAW_STAGE_MODEL(6);
    }
}

void g_special_load_for_st348(void)
{
    Vec pos;
    struct OrdTblNode *list;
    struct OrdTblNode *node;

    g_draw_stage_models();
    g_smth_with_rendefc_reflective_height(-1.0f);
    mtxa_from_mtxb();
    mtxa_tf_point(&current_ball->pos, &pos);
    pos.x *= 1.1;
    pos.y *= 1.1;
    pos.z *= 1.1;
    list = fn_1_CCDC8(&pos);
    node = ord_tbl_alloc_node(sizeof(*node));
    if (node != NULL)
    {
        node->drawFunc = fn_1_5ADB8;
        fn_1_CD418(list, node);
    }
    pos.x *= 0.8181818181818181;
    pos.y *= 0.8181818181818181;
    pos.z *= 0.8181818181818181;
    list = fn_1_CCDC8(&pos);
    node = ord_tbl_alloc_node(sizeof(*node));
    if (node != NULL)
    {
        node->drawFunc = fn_1_5AFB0;
        fn_1_CD418(list, node);
    }
    g_smth_with_rendefc_reflective_height(0.0f);
    g_smth_with_rendefc_reflective_height(-100.0f);
    g_draw_start_position_marker();
    g_smth_with_rendefc_reflective_height(0.0f);
}

void g_smth_hardcoded_for_meltingpot(void)
{
    struct ItemGroup *ig;
    struct StageCollHeader *coll;
    int i;
    Mtx mtx;
    float t;
    float s;
    float scale;

    set_post_mult_color(0.4f, 0.4f, 0.4f, 0.4f);
    avdisp_set_z_mode(1, 3, 0);
    ig = itemgroups;
    coll = stagedef->collHeaders;
    for (i = 0; i < stagedef->itemGroupCount; i++, ig++, coll++)
    {
        if (coll->coliSphereCount > 0)
        {
            mtxa_from_identity();
            mtxa_scale_s(0.95f);
            mtxa_to_mtx(mtx);
            mtxa_from_mtxb();
            mtxa_mult_left(mtx);
            mtxa_translate(&ig->pos);
            mtxa_rotate_y(lbl_1_bss_2F48.infLightRotY - 0x8000);
            t = math_tan(0x4000 - lbl_1_bss_2F48.infLightRotX);
            s = math_sin(0x8000 - lbl_1_bss_2F48.infLightRotX);
            mtxa_translate_xyz(0.0f, -2.0f, -2.0 * t);
            if (fabs(s) > 1.1920928955078125e-07)
                scale = 1.0 / s;
            else
                scale = 100.0f;
            mtxa_scale_xyz(2.5f, 2.5f, 2.5 * scale);
            mtxa_rotate_x(0x4000);
            load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
            avdisp_draw_model_culled_sort_never(init_common_gma->modelEntries[103].model);
        }
    }
    set_post_mult_color(1.0f, 1.0f, 1.0f, 1.0f);
    avdisp_set_z_mode(1, 3, 1);
}

void g_smth_hardcoded_for_totalitarianism_and_meltingpot(void)
{
    float dx;
    float dy;
    float dz;
    struct GMAModel *model;
    u32 i;
    Vec pos;
    Mtx mtx;
    float t;

    if (!(g_some_render_flag & 0x400))
    {
        dx = itemgroups[1].pos.x - stagedef->collHeaders[1].initPos.x;
        dy = itemgroups[1].pos.y - stagedef->collHeaders[1].initPos.y;
        dz = itemgroups[1].pos.z - stagedef->collHeaders[1].initPos.z;
        mtxa_from_identity();
        mtxa_scale_s(0.99f);
        mtxa_to_mtx(mtx);
        switch (current_stage_id)
        {
        case 0xD:
        default:
            model = stage_gma->modelEntries[6].model;
            break;
        case 0xE5:
            model = stage_gma->modelEntries[52].model;
            break;
        }
        set_post_mult_color(0.0f, 0.0f, 0.0f, 0.5f);
        avdisp_set_z_mode(1, 3, 0);
        for (i = 0; i < 16; i++)
        {
            pos = lbl_1_data_2F060[i];
            pos.x += dx;
            pos.y += dy;
            pos.z += dz;
            mtxa_from_mtxb();
            mtxa_mult_left(mtx);
            mtxa_translate(&pos);
            mtxa_rotate_y(lbl_1_bss_2F48.infLightRotY - 0x8000);
            t = math_tan(0x4000 - lbl_1_bss_2F48.infLightRotX);
            mtxa_translate_xyz(0.0f, -pos.y, -pos.y * t);
            g_smth_with_gpu_and_locked_cache_mtx();
            avdisp_draw_model_culled_sort_never(model);
        }
        set_post_mult_color(1.0f, 1.0f, 1.0f, 1.0f);
        avdisp_set_z_mode(1, 3, 1);
    }
}

void g_smth_hardcoded_for_st340(void)
{
    struct ItemGroup *ig;
    int shadow;
    struct St340Panel *panel;
    u32 i;
    struct GMAModel *model;
    struct ShadowReceive receive;
    Vec camPos;
    float dx;
    float dy;
    float dz;

    ig = &itemgroups[2];
    receive.unk6 = 6;
    shadow = g_check_some_condition(6);
    mtxa_from_mtxb();
    mtxa_invert();
    mtxa_tf_point_xyz(0.0f, 0.0f, 0.0f, &camPos);
    mtxa_from_mtx(ig->transform);
    mtxa_rigid_inv_tf_point(&camPos, &camPos);
    mtxa_from_mtxb();
    mtxa_mult_right(ig->transform);
    g_smth_with_gpu_and_locked_cache_mtx();
    panel = lbl_1_data_2F120;
    for (i = 0; i < 7; i++, panel++)
    {
        if (!(panel->flags & 1))
        {
            dx = camPos.x - panel->pos.x;
            dy = camPos.y - panel->pos.y;
            dz = camPos.z - panel->pos.z;
            if (dz * panel->normal.z + (dx * panel->normal.x + dy * panel->normal.y) < 0.0)
                continue;  /* camera is behind the panel */
        }
        model = stage_gma->modelEntries[panel->modelIdx].model;
        avdisp_draw_model_culled_sort_never(model);
        if (shadow)
        {
            receive.obj = model;
            receive.unk4 = 4;
            receive.model = model;
            g_some_shadow_draw_func(&receive);
        }
    }
}

void g_smth_hardcoded_for_trainworm_and_nintendo(void)
{
    struct ItemGroup *ig;
    struct StageCollHeader *coll;
    int i;
    Mtx mtx;
    float t;

    mtxa_from_identity();
    mtxa_scale_s(0.99f);
    mtxa_to_mtx(mtx);
    set_post_mult_color(0.0f, 0.0f, 0.0f, 0.45f);
    avdisp_set_z_mode(1, 3, 0);
    ig = itemgroups;
    coll = stagedef->collHeaders;
    for (i = 0; i < stagedef->itemGroupCount; i++, ig++, coll++)
    {
        if (i != 0)
        {
            mtxa_from_mtxb();
            mtxa_mult_left(mtx);
            mtxa_translate(&ig->pos);
            mtxa_rotate_y(lbl_1_bss_2F48.infLightRotY - 0x8000);
            t = math_tan(0x4000 - lbl_1_bss_2F48.infLightRotX);
            mtxa_translate_xyz(0.0f, -ig->pos.y, -ig->pos.y * t);
            mtxa_rotate_y(-(lbl_1_bss_2F48.infLightRotY - 0x8000));
            mtxa_rotate_z(ig->rot.z);
            mtxa_rotate_y(ig->rot.y);
            mtxa_rotate_x(ig->rot.x);
            mtxa_rotate_y(0x4000);
            switch (coll->animGroupId)
            {
            default:
                mtxa_scale_s(1.2f);
                break;
            case 1:
                mtxa_translate_xyz(0.0f, 0.0f, 1.4f);
                mtxa_scale_xyz(1.2f, 1.2f, 1.34f);
                break;
            case 2:
                mtxa_translate_xyz(0.0f, 0.0f, 1.4f);
                mtxa_scale_xyz(1.2f, 1.2f, 0.5f);
                break;
            }
            g_smth_with_gpu_and_locked_cache_mtx();
            DRAW_STAGE_MODEL(79);
        }
    }
    set_post_mult_color(1.0f, 1.0f, 1.0f, 1.0f);
    avdisp_set_z_mode(1, 3, 1);
}
