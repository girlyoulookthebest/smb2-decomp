/* Dogfight drawing: the per-camera render pass, cockpit wings and HUD markers. */
#include "types.h"
#include "ape_internal.h"
#include "avdisp.h"
#include "background.h"
#include "ball.h"
#include "camera.h"
#include "event.h"
#include "frustum.h"
#include "libc.h"
#include "light.h"
#include "macros.h"
#include "mathutil.h"
#include "mode.h"
#include "nl2ngc.h"
#include "pool.h"
#include "stage.h"
#include "stcoli.h"

struct DogfightPlayer
{
    u32 flags;
    float unk4;
    u8 filler8[8];
    float unk10;
    u8 filler14[0x22 - 0x14];
    s16 unk22[8];
    s16 unk32;
    u8 filler34[0x61 - 0x34];
    s8 unk61;
    u8 unk62;
    u8 unk63;
    u8 unk64;
    u8 filler65;
    u8 unk66;
    u8 filler67[0xB0 - 0x67];
};

struct DogfightShot
{
    s8 status;
    u8 filler1[2];
    s8 owner;
    u8 filler4[4];
    s32 unk8;
    s32 flags;
    u8 filler10[4];
    Vec pos;
    u8 filler20[0x44 - 0x20];
    Mtx mtx;
    u8 filler74[0x4EC - 0x74];
};

struct ModelLod
{
    char *name;
    float maxSize;
};

struct LodSet
{
    struct ModelLod *lods;
    s16 count;
    s16 group;
};

struct ArrowNode
{
    struct OrdTblNode node;
    int lightGroup;
    struct Ball *from;
    struct Ball *to;
};

extern struct GMA *minigameGma[];
extern u32 g_some_render_flag;
extern float lbl_80198B34;
extern u32 lbl_1_bss_D4CE4[];
extern u32 lbl_1_data_65984;
extern Vec lbl_1_data_2E418[];
extern s16 lbl_17_bss_14;
extern s32 lbl_17_bss_24;
extern s8 lbl_17_bss_3AC;
extern struct DogfightPlayer lbl_17_bss_48[];
extern struct DogfightShot lbl_17_bss_3DC[];
void PSVECAdd(Vec *a, Vec *b, Vec *dst);
void PSVECSubtract(Vec *a, Vec *b, Vec *dst);
void PSVECScale(Vec *src, Vec *dst, float scale);
u32 OSGetTick(void);
void fn_1_D4870(void);
void g_maybe_some_bg_effect_handler(void);
void g_something_with_rend_efc(int mask);
void avdisp_set_fog_params(int type, float start, float end);
void avdisp_set_fog_color(u8 r, u8 g, u8 b);
void g_set_something3(int arg0);
void g_draw_stage(void);
void draw_shadow_reqs(void);
void g_draw_bg(void);
void ord_tbl_draw_nodes(void);
void g_something_with_drawing_apes(void);
void g_draw_func_init(void);
void g_draw_ball_and_ape(void);
void g_draw_stobjs(void);
void fn_1_D6F9C(int i);
void handle_test_camera(void);
void fn_1_E2D20(void);
void fn_1_D6D84(int i);
void fn_1_4EBAC(Mtx mtx, int mode);
void fn_1_4EC60(Mtx mtx, int colorIdx, int mode);
void *fn_1_1135A4(struct Ape *ape);
struct OrdTblNode *fn_1_CCC18(float z);
void fn_17_118BC(void);
void fn_17_F964(struct Ball *ball);
void fn_17_1055C(struct Ball *ball);

void fn_17_155B4(void);
void fn_17_15678(struct Ball *ball);
void fn_17_1591C(struct Ball *ball);
void fn_17_159D0(struct Ball *ball);
void fn_17_15CB4(void);
void fn_17_15E18(void);
void fn_17_15F0C(struct Ball *ball);
void fn_17_16174(struct Ball *ball);
void fn_17_162CC(struct Ball *ball);
void fn_17_16744(struct Ball *ball);
void fn_17_16868(struct OrdTblNode *node);
void fn_17_16A28(struct Ball *ball);
void fn_17_16AC4(struct DogfightShot *shot);
void fn_17_16C60(void);
struct GMAModel *fn_17_16E4C(struct GMA *gma, char *name);
int fn_17_16ED8(Vec *pos, Vec *out, float tanHalfFov, float aspect);

u8 lbl_17_data_1410[0x170] =
{
    0xFE, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0xFE, 0x0D,
    0x0E, 0x0F, 0xFF, 0x00, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B,
    0x0C, 0x0D, 0x0E, 0x0F, 0xFF, 0x00, 0x00, 0x00, 0x31, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x38, 0x00, 0xB4, 0x00, 0x00, 0x01, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00,
    0x10, 0x00, 0x4C, 0x00, 0x00, 0x03, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00,
    0x18, 0x00, 0x00, 0x00, 0x00, 0x09, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x01, 0x00, 0x00, 0x00, 0x00, 0x0B, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x0C, 0x00, 0x00,
    0x18, 0x00, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x01, 0x00, 0x00, 0x00, 0x00, 0x07, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00,
    0x38, 0x00, 0xB4, 0x00, 0x00, 0x0D, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x18, 0x00, 0x00, 0x00, 0x00, 0x14, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x01, 0x00, 0x00, 0x00, 0x00, 0x16, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x17, 0x00, 0x00,
    0x18, 0x00, 0x00, 0x00, 0x00, 0x18, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x0F, 0x00, 0x00,
    0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x11, 0x00, 0x00,
    0x18, 0x00, 0x00, 0x00, 0x00, 0x12, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x13, 0x00, 0x00,
    0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00,
    0x18, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00,
    0x18, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00,
    0x18, 0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x07, 0x00, 0x00,
    0x18, 0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x09, 0x00, 0x00,
    0x18, 0x00, 0x00, 0x00, 0x00, 0x0A, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x0B, 0x00, 0x00,
    0x18, 0x00, 0x00, 0x00, 0x00, 0x0C, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x0D, 0x00, 0x00,
    0x18, 0x00, 0x00, 0x00, 0x00, 0x0E, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x0F, 0x00, 0x00,
};

static struct ModelLod lbl_17_data_15B0[] =
{
    {"NOCOLI_MOT_MATO_LV1Z", 300.0f},
    {"NOCOLI_MOT_MATO_LV1Z_B", 0.0f},
};

static struct ModelLod lbl_17_data_1608[] =
{
    {"NOCOLI_MOT_MATO_LV2Z", 150.0f},
    {"NOCOLI_MOT_MATO_LV2Z_B", 100.0f},
    {"NOCOLI_MOT_MATO_LV2Z_C", 0.0f},
};

static struct ModelLod lbl_17_data_1668[] =
{
    {"NOCOLI_MOT_MATO_LV3Z", 120.0f},
    {"NOCOLI_MOT_MATO_LV3Z_B", 50.0f},
    {"NOCOLI_MOT_MATO_LV3Z_C", 0.0f},
};

static struct LodSet lbl_17_data_1680[] =
{
    {lbl_17_data_15B0, 2, 0},
    {lbl_17_data_1608, 3, 0},
    {lbl_17_data_1668, 3, 0},
};

static struct ModelLod lbl_17_data_16C8[] =
{
    {"NOCOLI_MOT_MATO_LV4Z", 150.0f},
    {"NOCOLI_MOT_MATO_LV4Z_B", 0.0f},
};

static struct LodSet lbl_17_data_16D8[] =
{
    {lbl_17_data_16C8, 2, 0},
};

static struct ModelLod lbl_17_data_1710[] =
{
    {"NOCOLI_MOT_MATO_LV5Z", 300.0f},
    {"NOCOLI_MOT_MATO_LV5Z_B", 0.0f},
};

static struct LodSet lbl_17_data_1720[] =
{
    {lbl_17_data_1710, 2, 1},
};

static u32 lbl_17_bss_12B30;

static inline void timer_start(void)
{
    lbl_17_bss_12B30 = OSGetTick();
}

static inline u32 timer_stop(void)
{
    return ((OSGetTick() - lbl_17_bss_12B30) * 8) / ((*(u32 *)0x800000F8 / 4) / 125000);
}

void fn_17_14938(void)
{
    struct RaycastHit hit;
    int i;
    struct Ball *savedBall;
    u32 savedTimer;

    for (i = 0; i < 0x12; i++)
    {
        lbl_1_bss_D4CE4[i] = 0;
    }
    timer_start();
    if (lbl_1_data_65984 & 1)
    {
        savedTimer = lbl_17_bss_12B30;
        savedBall = current_ball;
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

                timer_start();
                if ((lbl_1_data_65984 & 2) && events[EVENT_REND_EFC].state == 2)
                {
                    g_something_with_rend_efc(4);
                }
                lbl_1_bss_D4CE4[1] += timer_stop();

                if (g_current_camera->eye.y < 0.0f)
                {
                    avdisp_set_fog_params(2, 50.0f, 20000.0f);
                    avdisp_set_fog_color(0, 0x64, 0x96);
                    g_set_something3(1);
                }

                timer_start();
                if (lbl_1_data_65984 & 0x20)
                {
                    u8 state = *(u8 *)&events[EVENT_STAGE].state;

                    if ((s8)state == 2 || (s8)state == 4)
                    {
                        g_draw_stage();
                    }
                }
                lbl_1_bss_D4CE4[5] += timer_stop();

                timer_start();
                if (lbl_1_data_65984 & 0x40)
                {
                    fn_17_16C60();
                }
                lbl_1_bss_D4CE4[6] += timer_stop();

                fn_17_15CB4();

                timer_start();
                if (lbl_1_data_65984 & 0x80)
                {
                    draw_shadow_reqs();
                }
                lbl_1_bss_D4CE4[7] += timer_stop();

                timer_start();
                if ((lbl_1_data_65984 & 0x100) && events[EVENT_BACKGROUND].state == 2)
                {
                    g_smth_with_rendefc_reflective_height(400.0f);
                    g_draw_bg();
                    g_smth_with_rendefc_reflective_height(0.0f);
                }
                lbl_1_bss_D4CE4[8] += timer_stop();

                switch (lbl_17_bss_14)
                {
                case 21:
                case 17:
                    break;
                case 19:
                    if (current_stage_id == 0x99)
                    {
                        ord_tbl_draw_nodes();
                    }
                    g_something_with_drawing_apes();
                    break;
                default:
                    timer_start();
                    if (lbl_1_data_65984 & 0x10)
                    {
                        fn_17_118BC();
                    }
                    lbl_1_bss_D4CE4[4] += timer_stop();
                    ord_tbl_draw_nodes();
                    timer_start();
                    if (lbl_1_data_65984 & 8)
                    {
                        fn_17_155B4();
                    }
                    lbl_1_bss_D4CE4[3] += timer_stop();
                    timer_start();
                    if (lbl_1_data_65984 & 4)
                    {
                        g_draw_func_init();
                    }
                    lbl_1_bss_D4CE4[2] += timer_stop();
                    break;
                }

                timer_start();
                if ((lbl_1_data_65984 & 0x200) && events[EVENT_REND_EFC].state == 2)
                {
                    g_something_with_rend_efc(0x10);
                }
                lbl_1_bss_D4CE4[9] += timer_stop();

                timer_start();
                if ((lbl_1_data_65984 & 0x400) && events[EVENT_ITEM].state == 2)
                {
                    draw_items();
                }
                lbl_1_bss_D4CE4[10] += timer_stop();

                timer_start();
                if ((lbl_1_data_65984 & 0x2000) && events[EVENT_BALL].state == 2)
                {
                    g_draw_ball_and_ape();
                    fn_17_159D0(current_ball);
                    fn_17_F964(current_ball);
                }
                lbl_1_bss_D4CE4[13] += timer_stop();

                timer_start();
                if ((lbl_1_data_65984 & 0x800) && events[EVENT_STOBJ].state == 2)
                {
                    g_draw_stobjs();
                }
                lbl_1_bss_D4CE4[11] += timer_stop();

                timer_start();
                if ((lbl_1_data_65984 & 0x1000) && events[EVENT_EFFECT].state == 2)
                {
                    g_draw_effects();
                }
                lbl_1_bss_D4CE4[12] += timer_stop();

                timer_start();
                if ((lbl_1_data_65984 & 0x4000) && (world_theme.unk8 & 1))
                {
                    fn_1_D6F9C(i);
                }
                lbl_1_bss_D4CE4[14] += timer_stop();

                handle_test_camera();

                timer_start();
                if (lbl_1_data_65984 & 0x8000)
                {
                    ord_tbl_draw_nodes();
                }
                lbl_1_bss_D4CE4[15] += timer_stop();

                fn_17_15E18();

                if (current_ball->status == 2 && current_ball->pos.y < 15.0f && current_ball->pos.y > 0.0f
                 && mathutil_vec_len(&current_ball->vel) > 0.023148147f
                 && (int)raycast_stage_down(&current_ball->pos, &hit, NULL) == 0)
                {
                    fn_17_1055C(current_ball);
                }

                fn_1_E2D20();

                timer_start();
                if ((lbl_1_data_65984 & 0x10000) && (world_theme.unk8 & 1))
                {
                    fn_1_D6D84(i);
                }
                lbl_1_bss_D4CE4[16] += timer_stop();

                timer_start();
                if ((lbl_1_data_65984 & 0x20000) && events[EVENT_REND_EFC].state == 2)
                {
                    g_something_with_rend_efc(8);
                }
                lbl_1_bss_D4CE4[17] += timer_stop();
            }
        }
        current_ball = savedBall;
        lbl_17_bss_12B30 = savedTimer;
    }
    lbl_1_bss_D4CE4[0] += timer_stop();
    camera_setup_default_projection();
}

void fn_17_155B4(void)
{
    struct Ball *ball;
    int i;

    ball = balls;
    for (i = 0; i < 4; i++, ball++)
    {
        if (ball->status == 2 && (ball->flags & 0x80))
        {
            mtxa_from_mtxb();
            if (g_is_sphere_visible(&ball->pos, 1.15f))
            {
                fn_17_15678(ball);
                fn_17_1591C(ball);
            }
        }
    }
}

void fn_17_15678(struct Ball *ball)
{
    struct DogfightPlayer *player = &lbl_17_bss_48[ball->playerId];
    Vec v = {0.5f, 0.0f, 0.0f};
    s16 angle;
    int rot;
    int mode;
    float offset;
    float height;
    float speed;

    switch (modeCtrl.unk30)
    {
    case 1:
        if (ball == current_ball)
        {
            mode = 0;
        }
        else
        {
            mode = 2;
        }
        break;
    case 2:
        if (ball == current_ball)
        {
            mode = 1;
        }
        else
        {
            mode = 2;
        }
        break;
    case 3:
        if (ball == current_ball)
        {
            mode = 1;
        }
        else
        {
            mode = 4;
        }
        break;
    default:
        if (ball == current_ball)
        {
            mode = 2;
        }
        else
        {
            mode = 4;
        }
        break;
    }
    speed = player->unk10;
    angle = 90000.0f * player->unk4;
    if (angle > 0x600)
    {
        angle = 0x600;
    }
    else if (angle < -0x180)
    {
        angle = -0x180;
    }
    rot = (double)player->unk10 * (double)(angle + 0x4A00) / 20.0;
    offset = 0.05f * (player->unk10 / 20.0f);
    height = 0.5f - 0.602f * (speed / 20.0f);
    mtxa_mult_right(ball->unk30);
    mtxa_translate_xyz(0.0f, height, 0.0f);
    mtxa_push();
    mtxa_translate_xyz(-offset, 0.0f, 0.0f);
    mtxa_rotate_x(0x4000);
    mtxa_rotate_y(0x4000 - rot);
    mtxa_translate_neg(&v);
    fn_1_4EBAC(mtxa->mtxA, mode);
    mtxa_pop();
    mtxa_translate_xyz(offset, 0.0f, 0.0f);
    mtxa_rotate_x(-0x4000);
    mtxa_rotate_y(0x4000 - rot);
    mtxa_translate(&v);
    fn_1_4EC60(mtxa->mtxA, ball->colorId, mode);
}

void fn_17_1591C(struct Ball *ball)
{
    struct Ape *ape = ball->ape;
    s16 models[4] = {0, 0x46, 0x53, 0x47};
    Mtx mtx;

    mtxa_from_quat(&ape->rot);
    mtxa_to_mtx(mtx);
    mtxa_from_mtxb_translate(&ape->pos);
    mtxa_translate(&ape->unk264);
    mtxa_mult_right(mtx);
    mtxa_mult_right(fn_1_1135A4(ape));
    load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
    avdisp_draw_model_culled_sort_auto(minigameGma[0]->modelEntries[models[ape->charaId]].model);
}

void fn_17_159D0(struct Ball *ball)
{
    s16 models[4] = {1, 3, 5, 7};
    float height;
    float inv;
    struct Ball *other;
    s16 i;

    if (lbl_17_bss_14 >= 9)
    {
        height = 240.0f * g_current_camera->sub28.viewportH;
        inv = 1.0f / height;
        for (i = 0, other = balls; i < 4; i++, other++)
        {
            struct DogfightPlayer *player = &lbl_17_bss_48[other->playerId];

            if (other->status == 2 && other != ball
             && (!(g_current_camera->flags & 0x40) || other->playerId != lbl_17_bss_3AC))
            {
                Vec pos;
                Vec v = {0.0f, 0.0f, 0.0f};
                Vec *p;
                float size;
                float ratio;
                float scale;

                v.x = other->pos.x;
                v.y = other->pos.y + other->currRadius;
                v.z = other->pos.z;
                p = &v;
                pos = *p;
                size = g_current_camera->sub28.view.tanHalfFov * mathutil_vec_distance(&g_current_camera->eye, &pos);
                ratio = height / size;
                if (ratio < 22.0f)
                {
                    scale = 22.0f * size * inv;
                }
                else if (ratio > 70.0f)
                {
                    scale = 70.0f * size * inv;
                }
                else
                {
                    scale = 1.0f;
                }
                mtxa_from_mtxb_translate(&pos);
                mtxa_sq_from_identity();
                mtxa_scale_s(scale);
                avdisp_set_scale_factor(scale);
                load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
                if (player->flags & 0x800)
                {
                    Vec *color = &lbl_1_data_2E418[other->playerId];

                    set_post_mult_color(color->x, color->y, color->z, 1.0f);
                    avdisp_draw_model_culled_sort_auto(init_common_gma->modelEntries[9].model);
                    fade_color_base_default();
                }
                else
                {
                    avdisp_draw_model_culled_sort_auto(init_common_gma->modelEntries[models[i]].model);
                }
            }
        }
    }
}

void fn_17_15CB4(void)
{
    int i;
    struct Ball *ball;
    struct DogfightPlayer *player;

    if (!(g_current_camera->flags & 0x40) && !(g_some_render_flag & 4) && lbl_17_bss_14 >= 0xB && lbl_17_bss_14 < 0xF
     && !(lbl_17_bss_48[current_ball->playerId].flags & 8))
    {
        for (i = 0; i < 4; i++)
        {
            ball = &balls[i];
            player = &lbl_17_bss_48[ball->playerId];
            if (ball->status == 2)
            {
                if (ball == current_ball)
                {
                    fn_17_16A28(ball);
                }
                else if (!(player->flags & 0x10) && !(lbl_17_bss_48[current_ball->playerId].flags & 0x10))
                {
                    fn_17_16744(ball);
                }
            }
        }
    }
}

void fn_17_15E18(void)
{
    int i;
    struct Ball *ball;

    if (!(g_current_camera->flags & 0x40) && lbl_17_bss_14 >= 0xB && lbl_17_bss_14 < 0xF)
    {
        GXSetZMode_cached(1, 7, 1);
        for (i = 0; i < 4; i++)
        {
            ball = &balls[i];
            if (ball->status == 2)
            {
                if (ball == current_ball)
                {
                    fn_17_15F0C(ball);
                    fn_17_16174(ball);
                }
                else
                {
                    fn_17_162CC(ball);
                }
            }
        }
        GXSetZMode_cached(1, 3, 1);
    }
}

void fn_17_15F0C(struct Ball *ball)
{
    Vec screen;
    Vec offset;
    Vec dir;
    struct GMAModel *model;
    float dist;
    struct Camera *camera;
    struct DogfightPlayer *player;
    float scale;

    player = &lbl_17_bss_48[ball->playerId];
    camera = g_current_camera;
    if (!(player->flags & 8))
    {
        if (player->flags & 0x2000)
        {
            dist = mathutil_vec_distance(&balls[player->unk61].pos, &ball->pos);
            if (dist < 2.0f)
            {
                dist = 2.0f;
            }
            model = minigameGma[0]->modelEntries[0x7F].model;
        }
        else
        {
            if (lbl_17_bss_24 == 0)
            {
                model = minigameGma[0]->modelEntries[0x7B].model;
            }
            else
            {
                model = minigameGma[0]->modelEntries[0x6C].model;
            }
            dist = 200.0f;
            set_post_mult_color(0.2f, 1.0f, 0.2f, 1.0f);
        }
        dir.x = -ball->unk30[0][2];
        dir.y = -ball->unk30[1][2];
        dir.z = -ball->unk30[2][2];
        PSVECScale(&dir, &dir, dist);
        mtxa_from_mtx(ball->unk30);
        mtxa_tf_point_xyz(0.0f, -0.5f, 0.0f, &offset);
        PSVECAdd(&offset, &dir, &dir);
        mtxa_from_mtxb();
        if (fn_17_16ED8(&dir, &screen, camera->sub28.view.tanHalfFov, camera->sub28.view.aspect))
        {
            float k = 0.2f * camera->sub28.view.tanHalfFov;

            screen.x *= k * camera->sub28.view.aspect;
            screen.y *= k;
            screen.z = -0.2f;
            mtxa_from_identity();
            mtxa_translate(&screen);
            scale = 0.025f;
            mtxa_scale_s(scale);
            avdisp_set_scale_factor(scale);
            load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
            avdisp_draw_model_unculled_sort_never(model);
            fade_color_base_default();
        }
    }
}

void fn_17_16174(struct Ball *ball)
{
    float scale;
    struct GMAModel *model;
    struct DogfightPlayer *player;

    player = &lbl_17_bss_48[ball->playerId];
    if (!(player->flags & 8))
    {
        if (lbl_17_bss_24 == 0)
        {
            model = minigameGma[0]->modelEntries[0x80].model;
        }
        else
        {
            model = minigameGma[0]->modelEntries[0x6D].model;
        }
        if (player->unk62 != 0)
        {
            if (player->unk63 != 0)
            {
                set_post_mult_color(1.0f, 0.2f, 0.2f, 1.0f);
            }
            else
            {
                set_post_mult_color(1.0f, 1.0f, 0.2f, 1.0f);
            }
        }
        else
        {
            set_post_mult_color(0.2f, 1.0f, 0.2f, 1.0f);
        }
        mtxa_from_identity();
        mtxa_translate_xyz(0.0f, 0.010000001f, -0.2f);
        scale = 0.1f;
        mtxa_scale_s(scale);
        load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
        avdisp_set_scale_factor(scale);
        avdisp_set_alpha(0.75f);
        avdisp_draw_model_unculled_sort_never(model);
        fade_color_base_default();
    }
}

void fn_17_162CC(struct Ball *ball)
{
    Vec screen;
    float scale;
    struct DogfightPlayer *player;
    struct DogfightPlayer *other;
    struct Camera *camera;
    int locked;
    int flash;

    player = &lbl_17_bss_48[current_ball->playerId];
    other = &lbl_17_bss_48[ball->playerId];
    camera = g_current_camera;
    locked = 0;
    flash = 0;
    if ((other->flags & 0x8010) || other->unk32 > lbl_80198B38 * 3)
    {
        return;
    }
    mtxa_from_mtxb();
    if (fn_17_16ED8(&ball->pos, &screen, camera->sub28.view.tanHalfFov, camera->sub28.view.aspect))
    {
        float k;

        if (other->unk66 & (1 << current_ball->playerId))
        {
            locked = 1;
        }
        k = 0.2f * camera->sub28.view.tanHalfFov;
        screen.x *= k * camera->sub28.view.aspect;
        screen.y *= k;
        screen.z = -0.2f;
        if (!(other->flags & 8) || !((g_font_char_var_3 >> 2) & 1))
        {
            scale = 1.0f;
            if (player->unk22[ball->playerId] != 0)
            {
                if (player->unk64 & (1 << ball->playerId))
                {
                    flash = 1;
                    scale = 1.0f + 2.0f * (0.2f * lbl_80198B34 - ((float)player->unk22[ball->playerId] - 2.0f * lbl_80198B34)) / (0.2f * lbl_80198B34);
                    if (scale < 1.0f)
                    {
                        scale = 1.0f;
                    }
                    if (g_font_char_var_3 & 1)
                    {
                        set_post_mult_color(1.0f, 1.0f, 0.0f, 1.0f);
                    }
                    else
                    {
                        set_post_mult_color(1.0f, 0.0f, 0.0f, 1.0f);
                    }
                }
                else if (player->unk63 & (1 << ball->playerId))
                {
                    flash = 1;
                    scale = 1.0f + 2.0f * (0.2f * lbl_80198B34 - ((float)player->unk22[ball->playerId] - 0.6f * lbl_80198B34)) / (0.2f * lbl_80198B34);
                    if (scale < 1.0f)
                    {
                        scale = 1.0f;
                    }
                    set_post_mult_color(1.0f, 0.0f, 0.0f, 1.0f);
                }
                else
                {
                    set_post_mult_color(1.0f, 1.0f, 0.0f, 1.0f);
                }
            }
            else
            {
                set_post_mult_color(0.0f, 1.0f, 0.0f, 1.0f);
            }
            mtxa_from_identity();
            mtxa_translate(&screen);
            mtxa_scale_s(0.02f);
            load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
            avdisp_draw_model_unculled_sort_never(minigameGma[0]->modelEntries[0x7E].model);
            if (flash != 0)
            {
                mtxa_scale_s(1.25f * scale);
                load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
                avdisp_draw_model_unculled_sort_never(minigameGma[0]->modelEntries[0x49].model);
            }
        }
        if (locked != 0)
        {
            set_post_mult_color(1.0f, 0.0f, 0.0f, 1.0f);
            mtxa_from_identity();
            mtxa_translate(&screen);
            mtxa_scale_s(0.02f);
            load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
            avdisp_draw_model_unculled_sort_never(minigameGma[0]->modelEntries[0x7D].model);
        }
        fade_color_base_default();
    }
}

void fn_17_16744(struct Ball *ball)
{
    Vec v;
    struct Camera *camera;
    struct OrdTblNode *entry;
    struct ArrowNode *node;

    camera = g_current_camera;
    mtxa_from_mtxb();
    if (fn_17_16ED8(&ball->pos, &v, camera->sub28.view.tanHalfFov, camera->sub28.view.aspect) == 0
     || !(v.x > -1.0f) || !(v.x < 1.0f) || !(v.y > -1.0f) || !(v.y < 1.0f))
    {
        mtxa_from_mtxb();
        mtxa_tf_point(&current_ball->pos, &v);
        entry = fn_1_CCC18(v.z - 10.0f);
        node = ord_tbl_alloc_node(sizeof(*node));
        if (node != NULL)
        {
            node->node.drawFunc = fn_17_16868;
            node->lightGroup = get_active_light_group();
            node->to = ball;
            node->from = current_ball;
            fn_1_CD418(entry, &node->node);
        }
    }
}

void fn_17_16868(struct OrdTblNode *arg)
{
    float colors[4][4] =
    {
        {1.0f, 0.2f, 0.2f, 1.0f},
        {0.2f, 0.2f, 1.0f, 1.0f},
        {1.0f, 1.0f, 0.0f, 1.0f},
        {0.0f, 1.0f, 0.0f, 1.0f},
    };
    Mtx mtx;
    Vec forward = {0.0f, 0.0f, -1.0f};
    Vec dir;
    Quat q;
    struct ArrowNode *node = (struct ArrowNode *)arg;
    struct Ball *to;
    struct Ball *from;
    float *color;
    float scale;

    to = node->to;
    from = node->from;
    color = colors[to->playerId];
    load_light_group_cached(node->lightGroup);
    PSVECSubtract(&to->pos, &from->pos, &dir);
    vec_normalize_len(&dir);
    quat_from_dirs(&q, &forward, &dir);
    mtxa_from_quat(&q);
    mtxa_to_mtx(mtx);
    mtxa_from_mtxb_translate(&from->pos);
    mtxa_mult_right(mtx);
    mtxa_translate_xyz(0.0f, 0.0f, -1.1f);
    scale = 0.18f;
    mtxa_scale_s(scale);
    load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
    avdisp_set_scale_factor(scale);
    set_post_mult_color(color[0], color[1], color[2], color[3]);
    avdisp_draw_model_unculled_sort_never(minigameGma[0]->modelEntries[0x78].model);
    fade_color_base_default();
}

void fn_17_16A28(struct Ball *ball)
{
    int i;
    struct DogfightShot *shot;

    for (i = 0; i < 0x3C; i++)
    {
        shot = &lbl_17_bss_3DC[i];
        if (shot->status == 2 && shot->unk8 == 0 && !(shot->flags & 3) && shot->owner == ball->playerId)
        {
            fn_17_16AC4(shot);
        }
    }
}

void fn_17_16AC4(struct DogfightShot *shot)
{
    Mtx mtx;
    float color[4] = {1.0f, 0.7f, 0.2f, 1.0f};
    float *c = color;
    Vec forward = {0.0f, 0.0f, -1.0f};
    Vec dir;
    Vec pos;
    Quat q;
    float scale;

    if (!((g_font_char_var_3 >> 2) & 1))
    {
        PSVECSubtract(&shot->pos, &current_ball->pos, &dir);
        vec_normalize_len(&dir);
        quat_from_dirs(&q, &forward, &dir);
        mtxa_from_quat(&q);
        mtxa_to_mtx(mtx);
        mtxa_from_mtxb_translate(&current_ball->pos);
        mtxa_mult_right(mtx);
        mtxa_tf_point_xyz(0.0f, 0.0f, -0.75f, &pos);
        mtxa_from_mtxb();
        mtxa_mult_right(shot->mtx);
        mathutil_mtxA_set_translate(&pos);
        scale = 0.18f;
        mtxa_scale_s(scale);
        load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
        avdisp_set_scale_factor(scale);
        set_post_mult_color(c[0], c[1], c[2], c[3]);
        avdisp_draw_model_unculled_sort_auto(minigameGma[0]->modelEntries[0x78].model);
        fade_color_base_default();
    }
}

void fn_17_16C60(void)
{
    Vec v;
    struct LodSet *sets;
    int i;
    int j;
    int count;
    struct LodSet *set;
    struct ModelLod *lods;
    struct GMAModel *model;
    float size;
    int group;
    int k;

    switch (current_stage_id)
    {
    case 0x9B:
        sets = lbl_17_data_1680;
        count = 3;
        break;
    case 0x9C:
        sets = lbl_17_data_16D8;
        count = 1;
        break;
    case 0x9D:
        sets = lbl_17_data_1720;
        count = 1;
        break;
    default:
        return;
    }
    for (i = 0; i < count; i++)
    {
        set = &sets[i];
        lods = set->lods;
        mtxa_from_mtxb();
        for (j = 0; j < set->count; j++)
        {
            model = fn_17_16E4C(stage_gma, lods[j].name);
            mtxa_tf_point(&model->boundSphereCenter, &v);
            size = (-480.0f * g_current_camera->sub28.viewportH * model->boundSphereRadius) / (v.z * g_current_camera->sub28.view.tanHalfFov);
            if (size > lods[j].maxSize || size < 0.0f)
            {
                break;
            }
        }
        group = set->group;
        if (group > 0)
        {
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
    }
}

struct GMAModel *fn_17_16E4C(struct GMA *gma, char *name)
{
    struct GMAModel *model;
    u32 len;
    int i;
    struct GMAModelEntry *entry;
    float dead;

    dead = 320.0f;
    model = NULL;
    len = strlen(name);
    for (i = 0; i < (int)gma->numModels; i++)
    {
        entry = &gma->modelEntries[i];
        if (strncmp(name, entry->name, len) == 0)
        {
            model = entry->model;
            break;
        }
    }
    return model;
}

int fn_17_16ED8(Vec *pos, Vec *out, float tanHalfFov, float aspect)
{
    float k;

    mtxa_tf_point(pos, out);
    if (out->z < -FLT_EPSILON)
    {
        k = -1.0f / (out->z * tanHalfFov);
        out->x = k * (1.0f / aspect * out->x);
        out->y *= k;
        return 1;
    }
    return 0;
}
