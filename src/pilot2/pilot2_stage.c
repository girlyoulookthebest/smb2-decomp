/* Monkey Target 2 stage: per-camera draw pass, players' wings, name tags and the target models. */
#include "types.h"
#include "avdisp.h"
#include "ball.h"
#include "background.h"
#include "camera.h"
#include "effect.h"
#include "event.h"
#include "frustum.h"
#include "item.h"
#include "light.h"
#include "mathutil.h"
#include "mode.h"
#include "pilot2.h"
#include "pool.h"
#include "stage.h"
#include "stcoli.h"
#include "shadow.h"

struct ShadowReceive
{
    /*0x00*/ void *obj;
    /*0x04*/ u16 layer;
    /*0x06*/ u16 modelType;
    /*0x08*/ void *model;
};

struct TargetLod
{
    /*0x00*/ char *name;
    /*0x04*/ float dist;
};

struct TargetModel
{
    /*0x00*/ struct TargetLod *lods;
    /*0x04*/ s16 lodCount;
    /*0x06*/ s16 animGroupId;
};

extern u32 lbl_1_data_65978;
extern u32 lbl_1_bss_D49BC[];
extern u32 lbl_1_bss_D4A10;
extern float lbl_80198B34;
extern struct GMA *minigameGma[];
extern s16 lbl_13_bss_12;
extern Vec lbl_13_bss_18;
extern int lbl_13_bss_220;
extern Vec lbl_13_bss_2AC;
extern s16 lbl_13_bss_2B8;
extern Vec lbl_13_bss_2BC;
extern Vec lbl_13_bss_2C8;
extern struct { u32 flags; u8 unk4[0x58 - 0x4]; GXColor bgColor; } g_some_render_flag;

u32 OSGetTick(void);
void fn_1_D4870(void);
void g_maybe_some_bg_effect_handler(void);
void g_smth_with_bg_color_drawing(GXColor *color);
void g_something_with_rend_efc(int mask);
void g_set_something3(int a);
void avdisp_set_fog_params(int a, float b, float c);
void avdisp_set_fog_color(int r, int g, int b);
void g_draw_stage(void);
void g_draw_bg(void);
void draw_shadow_reqs(void);
void g_smth_with_rendefc_reflective_height(float height);
void g_something_with_drawing_apes(void);
void g_draw_func_init(void);
void g_draw_ball_and_ape(void);
void g_draw_stobjs(void);
void fn_1_D6F9C(int i);
void fn_1_D6D84(int i);
void handle_test_camera(void);
void ord_tbl_draw_nodes(void);
void fn_1_E2D20(void);
void fn_1_4EAFC(Mtx mtx, int colorIdx, int mode);
void fn_1_4EBAC(Mtx mtx, int mode);
void fn_1_4EC60(Mtx mtx, int colorIdx, int mode);
int g_check_some_condition(int a);
void g_some_shadow_draw_func(struct ShadowReceive *s);
void fn_13_103E0(struct Ball *ball);
void fn_13_F7E8(struct Ball *ball);

void fn_13_12A6C(void);
void fn_13_12BEC(struct Ball *ball, int mode);
void fn_13_12C48(struct Ball *ball, int mode);
void fn_13_12E48(struct Ball *ball);
void fn_13_13114(void);
void fn_13_13330(void);
void fn_13_134C8(void);
struct GMAModel *fn_13_136E4(struct GMA *gma, char *name);
void fn_13_13770(float *x, float *y, float z, float screenX, float screenY, struct Camera *camera);

u32 lbl_13_data_1480[] =
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

u32 lbl_13_bss_2288;

#define TICKS_TO_USEC(t) (((u32)(t) * 8) / ((*(u32 *)0x800000F8 / 4) / 125000))
#define STOP_TIMER(n) lbl_1_bss_D49BC[n] += TICKS_TO_USEC(OSGetTick() - lbl_13_bss_2288)

void fn_13_11CD4(void)
{
    int i;

    for (i = 0; i < 0x15; i++)
        lbl_1_bss_D49BC[i] = 0;
    lbl_13_bss_2288 = OSGetTick();
    if (lbl_1_data_65978 & 1)
    {
        struct Ball *ballBackup;
        int tick;
        s8 state;

        tick = lbl_13_bss_2288;
        ballBackup = current_ball;
        for (i = 0; i < 5; i++)
        {
            if (cameras[i].sub28.viewportW > 0.0f && cameras[i].sub28.viewportH > 0.0f
             && g_poolInfo.cameraPool.statusList[i] == 2)
            {
                current_ball = &balls[i];
                camera_apply_viewport_if_changed(i);
                fn_1_D4870();
                g_maybe_some_bg_effect_handler();
                u_reset_light_group_stack(i);
                g_smth_with_bg_color_drawing(&g_some_render_flag.bgColor);
                lbl_13_bss_2288 = OSGetTick();
                if ((lbl_1_data_65978 & 2) && events[21].state == 2)
                    g_something_with_rend_efc(4);
                STOP_TIMER(1);
                if (g_current_camera->eye.y < 0.0f)
                {
                    avdisp_set_fog_params(2, 50.0f, 20000.0f);
                    avdisp_set_fog_color(0, 0x64, 0x96);
                    g_set_something3(1);
                }
                lbl_13_bss_2288 = OSGetTick();
                if (lbl_1_data_65978 & 0x80)
                {
                    state = events[0].state;
                    if (state == 2 || state == 4)
                        g_draw_stage();
                }
                STOP_TIMER(7);
                lbl_13_bss_2288 = OSGetTick();
                if (lbl_1_data_65978 & 0x100)
                    fn_13_134C8();
                STOP_TIMER(8);
                lbl_13_bss_2288 = OSGetTick();
                if (lbl_1_data_65978 & 0x200)
                    fn_13_13330();
                STOP_TIMER(9);
                lbl_13_bss_2288 = OSGetTick();
                if (lbl_1_data_65978 & 0x400)
                    draw_shadow_reqs();
                STOP_TIMER(10);
                lbl_13_bss_2288 = OSGetTick();
                if ((lbl_1_data_65978 & 0x800) && events[20].state == 2)
                {
                    g_smth_with_rendefc_reflective_height(400.0f);
                    g_draw_bg();
                    g_smth_with_rendefc_reflective_height(0.0f);
                }
                STOP_TIMER(11);
                switch (lbl_13_bss_12)
                {
                case 21:
                case 15:
                    break;
                case 18:
                    g_something_with_drawing_apes();
                    break;
                default:
                    lbl_13_bss_2288 = OSGetTick();
                    if (lbl_1_data_65978 & 8)
                        fn_13_13114();
                    STOP_TIMER(3);
                    lbl_13_bss_2288 = OSGetTick();
                    if (lbl_1_data_65978 & 0x10)
                        fn_13_12A6C();
                    STOP_TIMER(4);
                    lbl_13_bss_2288 = OSGetTick();
                    STOP_TIMER(5);
                    lbl_13_bss_2288 = OSGetTick();
                    if (lbl_1_data_65978 & 4)
                        g_draw_func_init();
                    STOP_TIMER(2);
                    lbl_13_bss_2288 = OSGetTick();
                    STOP_TIMER(6);
                    break;
                }
                lbl_13_bss_2288 = OSGetTick();
                if ((lbl_1_data_65978 & 0x1000) && events[21].state == 2)
                    g_something_with_rend_efc(0x10);
                STOP_TIMER(12);
                lbl_13_bss_2288 = OSGetTick();
                if ((lbl_1_data_65978 & 0x2000) && events[5].state == 2)
                    draw_items();
                STOP_TIMER(13);
                lbl_13_bss_2288 = OSGetTick();
                if ((lbl_1_data_65978 & 0x10000) && events[2].state == 2)
                {
                    g_draw_ball_and_ape();
                    fn_13_12E48(current_ball);
                    fn_13_F7E8(current_ball);
                }
                STOP_TIMER(16);
                lbl_13_bss_2288 = OSGetTick();
                if ((lbl_1_data_65978 & 0x4000) && events[4].state == 2)
                    g_draw_stobjs();
                STOP_TIMER(14);
                lbl_13_bss_2288 = OSGetTick();
                if ((lbl_1_data_65978 & 0x8000) && events[14].state == 2)
                    g_draw_effects();
                STOP_TIMER(15);
                lbl_13_bss_2288 = OSGetTick();
                if ((lbl_1_data_65978 & 0x20000) && (world_theme.unk8 & 1))
                    fn_1_D6F9C(i);
                STOP_TIMER(17);
                handle_test_camera();
                lbl_13_bss_2288 = OSGetTick();
                if (lbl_1_data_65978 & 0x40000)
                    ord_tbl_draw_nodes();
                STOP_TIMER(18);
                if (current_ball->status == 2 && current_ball->pos.y < 15.0f && current_ball->pos.y > 0.0f)
                {
                    struct RaycastHit hit;

                    if (math_sqrt(mathutil_sum_of_sq_2(current_ball->vel.x, current_ball->vel.z)) > 0.2777777910232544f
                     && (int)raycast_stage_down(&current_ball->pos, &hit, 0) == 0)
                        fn_13_103E0(current_ball);
                }
                fn_1_E2D20();
                lbl_13_bss_2288 = OSGetTick();
                if ((lbl_1_data_65978 & 0x80000) && (world_theme.unk8 & 1))
                    fn_1_D6D84(i);
                STOP_TIMER(19);
                lbl_13_bss_2288 = OSGetTick();
                if ((lbl_1_data_65978 & 0x100000) && events[21].state == 2)
                    g_something_with_rend_efc(8);
                STOP_TIMER(20);
            }
        }
        current_ball = ballBackup;
        lbl_13_bss_2288 = tick;
    }
    STOP_TIMER(0);
    camera_setup_default_projection();
}

void fn_13_12A6C(void)
{
    struct Ball *ball = balls;
    int i = 0;
    int mode;

    while (i < 5)
    {
        struct Pilot2Player *player = &lbl_13_bss_70[ball->playerId];

        if (ball->status == 2 && (ball->flags & 0x80))
        {
            switch (modeCtrl.unk30)
            {
            case 1:
                if (!(lbl_1_bss_D4A10 & 1))
                    mode = 0;
                else
                    mode = 1;
                break;
            case 2:
                mode = 1;
                break;
            default:
                if (ball == current_ball)
                    mode = 1;
                else
                    mode = 2;
                break;
            }
            mtxa_from_mtxb();
            if (player->unk10 > 0.0f)
            {
                if (g_is_sphere_visible(&ball->pos, 1.15f))
                    fn_13_12C48(ball, mode);
            }
            else if (g_is_sphere_visible(&ball->pos, 0.5f))
                fn_13_12BEC(ball, mode);
        }
        i++;
        ball++;
    }
}

void fn_13_12BEC(struct Ball *ball, int mode)
{
    mtxa_mult_right(ball->unk30);
    mtxa_rotate_y(0x4000);
    fn_1_4EAFC(mtxa->mtxA, ball->colorId, mode);
}

void fn_13_12C48(struct Ball *ball, int mode)
{
    struct Pilot2Player *player = &lbl_13_bss_70[ball->playerId];
    Vec wing = {0.5f, 0.0f, 0.0f};
    float extent = player->unk10;
    s16 angle = 90000.0f * player->unk4;
    int rotY;
    float offset;
    float height;

    if (angle > 0x600)
        angle = 0x600;
    else if (angle < -0x180)
        angle = -0x180;
    rotY = (player->unk10 * (double)(angle + 0x4A00)) / 20.0;
    if (player->flags & 0x20)
        extent = 20.0f;
    offset = 0.05f * (player->unk10 / 20.0f);
    height = 0.5f - 0.602f * (extent / 20.0f);
    mtxa_mult_right(ball->unk30);
    mtxa_translate_xyz(0.0f, height, 0.0f);
    mtxa_push();
    mtxa_translate_xyz(-offset, 0.0f, 0.0f);
    mtxa_rotate_x(0x4000);
    mtxa_rotate_y(0x4000 - rotY);
    mtxa_translate_neg(&wing);
    fn_1_4EBAC(mtxa->mtxA, mode);
    mtxa_pop();
    mtxa_translate_xyz(offset, 0.0f, 0.0f);
    mtxa_rotate_x(-0x4000);
    mtxa_rotate_y(0x4000 - rotY);
    mtxa_translate(&wing);
    fn_1_4EC60(mtxa->mtxA, ball->colorId, mode);
}

void fn_13_12E48(struct Ball *curBall)
{
    s16 modelIds[4] = {1, 3, 5, 7};
    float limit;
    float invScale;
    float scale;
    struct Ball *ball;
    s16 i;

    limit = 240.0f * g_current_camera->sub28.viewportH;
    invScale = 1.0f / limit;
    for (i = 0, ball = balls; i < 5; i++, ball++)
    {
        struct Pilot2Player *player = &lbl_13_bss_70[ball->playerId];

        if (ball->status == 2 && ball != curBall && player->unk42 <= 0)
        {
            Vec pos;
            Vec top = {0.0f, 0.0f, 0.0f};
            Vec *topPtr;
            float dist;
            float size;
            float t;

            top.x = ball->pos.x;
            top.y = ball->pos.y + ball->currRadius;
            top.z = ball->pos.z;
            topPtr = &top;
            pos = *topPtr;
            dist = g_current_camera->sub28.view.tanHalfFov * mathutil_vec_distance(&g_current_camera->eye, &pos);
            size = limit / dist;
            if (size < 22.0f)
                scale = 22.0f * dist * invScale;
            else if (size > 60.0f)
                scale = 60.0f * dist * invScale;
            else
                scale = 1.0f;
            if (lbl_13_bss_12 < 9)
                break;
            if (lbl_13_bss_12 == 9)
            {
                t = lbl_13_bss_220 / lbl_80198B34;
                if (t > 1.0f)
                    t = 1.0f;
                scale *= t;
            }
            mtxa_from_mtxb_translate(&pos);
            mtxa_sq_from_identity();
            mtxa_scale_s(scale);
            avdisp_set_scale_factor(scale);
            load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
            avdisp_draw_model_culled_sort_auto(init_common_gma->modelEntries[modelIds[i]].model);
        }
    }
}

void fn_13_13114(void)
{
    float dead = 0.4f;
    struct Pilot2Player *player;
    void *obj;
    float x;
    float y;
    float scale;
    float h;
    s16 angle;

    obj = current_ball;
    player = &lbl_13_bss_70[((struct Ball *)obj)->playerId];
    if (!(g_current_camera->flags & 0x40) && (lbl_13_bss_12 < 4 || lbl_13_bss_12 > 5) && lbl_13_bss_12 < 0xD
     && !(mathutil_vec_sq_len(&lbl_13_bss_18) < 1e-05f) && player->unk42 <= 0)
    {
        switch (lbl_13_bss_24)
        {
        case 0:
            scale = 60.0f;
            h = 192.0f;
            break;
        default:
            scale = 45.0f;
            h = 77.2f;
            break;
        }
        angle = math_atan2(lbl_13_bss_18.x, lbl_13_bss_18.z);
        fn_13_13770(&x, &y, -0.15f, (scale * lbl_1_bss_2434) / 640.0f, ((h * lbl_1_bss_2434) / g_camera_aspect_ratio) / 480.0f, obj = g_current_camera);
        scale = 0.008f;
        mtxa_from_mtxb();
        mathutil_mtxA_set_translate_xyz(x, y, -0.15f);
        mtxa_rotate_y(angle);
        mtxa_scale_s(scale);
        load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
        avdisp_set_scale_factor(scale);
        avdisp_draw_model_unculled_sort_auto(minigameGma[0]->modelEntries[0x9C].model);
    }
}

void fn_13_13330(void)
{
    struct GMAModel *left;
    struct GMAModel *right;
    struct GMAModel *ita;
    struct ShadowReceive s;

    switch (current_stage_id)
    {
    case 0x9B:
    case 0x9C:
    case 0x9D:
        left = fn_13_136E4(stage_gma, "TAR_DOOR_L");
        right = fn_13_136E4(stage_gma, "TAR_DOOR_R");
        ita = fn_13_136E4(stage_gma, "TAR_ITA");
        break;
    default:
        return;
    }
    mtxa_from_mtxb_translate(&lbl_13_bss_2AC);
    load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
    avdisp_draw_model_culled_sort_auto(ita);
    if (g_check_some_condition(4))
    {
        s.modelType = 6;
        s.obj = ita;
        s.layer = 4;
        s.model = ita;
        g_some_shadow_draw_func(&s);
    }
    if (lbl_13_bss_2B8 < 240.0f)
    {
        mtxa_from_mtxb_translate(&lbl_13_bss_2BC);
        load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
        avdisp_draw_model_culled_sort_auto(left);
        mtxa_from_mtxb_translate(&lbl_13_bss_2C8);
        load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
        avdisp_draw_model_culled_sort_auto(right);
    }
}

void fn_13_134C8(void)
{
    static struct TargetLod lod1[] = {{"NOCOLI_MOT_MATO_LV1Z", 100.0f}, {"NOCOLI_MOT_MATO_LV1Z_B", 0.0f}};
    static struct TargetLod lod2[] = {{"NOCOLI_MOT_MATO_LV2Z", 100.0f}, {"NOCOLI_MOT_MATO_LV2Z_B", 40.0f}, {"NOCOLI_MOT_MATO_LV2Z_C", 0.0f}};
    static struct TargetLod lod3[] = {{"NOCOLI_MOT_MATO_LV3Z", 100.0f}, {"NOCOLI_MOT_MATO_LV3Z_B", 40.0f}, {"NOCOLI_MOT_MATO_LV3Z_C", 0.0f}};
    static struct TargetModel models1[] = {{lod1, 2, 0}, {lod2, 3, 0}, {lod3, 3, 0}};
    static struct TargetLod lod4[] = {{"NOCOLI_MOT_MATO_LV4Z", 150.0f}, {"NOCOLI_MOT_MATO_LV4Z_B", 0.0f}};
    static struct TargetModel models2[] = {{lod4, 2, 0}};
    static struct TargetLod lod5[] = {{"NOCOLI_MOT_MATO_LV5Z", 300.0f}, {"NOCOLI_MOT_MATO_LV5Z_B", 0.0f}};
    static struct TargetModel models3[] = {{lod5, 2, 1}};
    struct TargetModel *models;
    int i;
    int j;
    int count;
    struct TargetModel *target;
    struct TargetLod *lods;
    struct GMAModel *model;
    struct ShadowReceive s;
    Vec pos;

    switch (current_stage_id)
    {
    case 0x9B:
        models = models1;
        count = 3;
        break;
    case 0x9C:
        models = models2;
        count = 1;
        break;
    case 0x9D:
        models = models3;
        count = 1;
        break;
    default:
        return;
    }
    for (i = 0; i < count; i++)
    {
        int group;

        target = &models[i];
        lods = target->lods;
        mtxa_from_mtxb();
        for (j = 0; j < target->lodCount; j++)
        {
            float size;

            model = fn_13_136E4(stage_gma, lods[j].name);
            mtxa_tf_point(&model->boundSphereCenter, &pos);
            size = (-480.0f * g_current_camera->sub28.viewportH * model->boundSphereRadius) / (pos.z * g_current_camera->sub28.view.tanHalfFov);
            if (size > lods[j].dist || size < 0.0f)
                break;
        }
        group = target->animGroupId;
        if (group > 0)
        {
            int k;

            for (k = 0; k < stagedef->itemGroupCount; k++)
            {
                if (group == stagedef->collHeaders[k].animGroupId)
                {
                    mtxa_mult_right(itemgroups[k].transform);
                    break;
                }
            }
        }
        load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
        avdisp_draw_model_culled_sort_auto(model);
        if (g_check_some_condition(4))
        {
            s.modelType = 6;
            s.obj = model;
            s.layer = 4;
            s.model = model;
            g_some_shadow_draw_func(&s);
        }
    }
}

struct GMAModel *fn_13_136E4(struct GMA *gma, char *name)
{
    struct GMAModel *found = NULL;
    int len = strlen(name);
    int i;

    for (i = 0; i < (int)gma->numModels; i++)
    {
        struct GMAModelEntry *entry = &gma->modelEntries[i];

        if (strncmp(name, entry->name, len) == 0)
        {
            found = entry->model;
            break;
        }
    }
    return found;
}

void fn_13_13770(float *x, float *y, float z, float screenX, float screenY, struct Camera *camera)
{
    float height = camera->sub28.viewportH * (0.5f * (lbl_1_bss_2434 / g_camera_aspect_ratio));
    float width = 0.5f * lbl_1_bss_2434 * camera->sub28.viewportW;
    float scale = (-z * camera->sub28.view.tanHalfFov) / height;

    *x = scale * (screenX - width);
    *y = scale * -(screenY - height);
    *x *= lbl_1_bss_2440 / camera->sub28.view.tanHalfFov;
    *y *= lbl_1_bss_2440 / camera->sub28.view.tanHalfFov;
}
