/* Monkey Target 2 items: fruit and the like floating around the course. */
#include "types.h"
#include "avdisp.h"
#include "ball.h"
#include "camera.h"
#include "effect.h"
#include "frustum.h"
#include "input.h"
#include "item.h"
#include "libc.h"
#include "mathutil.h"
#include "mode.h"
#include "pilot2.h"
#include "rand.h"
#include "stage.h"

struct PilotLOD
{
    s32 modelId;
    float distance;
    s32 flag;  /* 1 = always face the camera, 2 = draw nothing */
};

struct PilotModel
{
    struct PilotLOD *lods;
    s8 count;
    float scale;
    struct GMA **gma;
};

struct PilotItemInfo
{
    struct PilotModel *model;
    float radius;
    s16 points;
    s16 rotVelX;
    s16 rotVelY;
    s16 rotVelZ;
};

extern struct GMA *minigameGma[];
extern u32 g_some_render_flag;
extern u32 g_font_char_var_3;
extern u32 lbl_1_bss_D4A10;
void PSVECScale(Vec *src, Vec *dst, float scale);
void g_rumble_controller(int pad, int a, int b);
void g_something_with_translating_items(int a, Vec *b, float c);

void fn_13_11450(struct Item *item, struct PhysicsBall *physBall);
struct GMAModel *fn_13_115D4(struct PilotModel *pilotModel);

static struct PilotLOD lbl_13_data_1270[] =
{
    {0x41, 150.0f, 0},
    {0x40, 100.0f, 0},
    {0x42, 50.0f, 0},
    {0x3F, 3.0f, 0},
    {-1, 0.0f, 2},
};
static struct PilotModel lbl_13_data_12AC = {lbl_13_data_1270, 5, 1.5f, &init_common_gma};
static struct PilotLOD lbl_13_data_12BC[] =
{
    {0x80, 100.0f, 0},
    {0x81, 50.0f, 0},
    {0x82, 3.0f, 0},
    {-1, 0.0f, 2},
};
static struct PilotModel lbl_13_data_12EC = {lbl_13_data_12BC, 4, 1.5f, minigameGma};
static struct PilotLOD lbl_13_data_12FC[] =
{
    {0x4B, 100.0f, 0},
    {0x4C, 50.0f, 1},
    {-1, 0.0f, 2},
};
static struct PilotModel lbl_13_data_1320 = {lbl_13_data_12FC, 3, 1.0f, minigameGma};
static struct PilotLOD lbl_13_data_1330[] =
{
    {0x4D, 100.0f, 0},
    {0x4E, 50.0f, 0},
    {0x4F, 0.0f, 1},
};
static struct PilotModel lbl_13_data_1354 = {lbl_13_data_1330, 3, 1.0f, minigameGma};
static struct PilotLOD lbl_13_data_1364[] =
{
    {0x25, 3.0f, 0},
    {-1, 0.0f, 2},
};
static struct PilotModel lbl_13_data_137C = {lbl_13_data_1364, 2, 0.15f, &init_common_gma};
static struct PilotLOD lbl_13_data_138C[] = {{0xA5, 0.0f, 0}};
static struct PilotModel lbl_13_data_1398 = {lbl_13_data_138C, 1, 2.0f, minigameGma};
static struct PilotLOD lbl_13_data_13A8[] = {{0xA6, 0.0f, 0}};
static struct PilotModel lbl_13_data_13B4 = {lbl_13_data_13A8, 1, 2.0f, minigameGma};

static struct PilotItemInfo lbl_13_data_13C4[] =
{
    {&lbl_13_data_12AC, 1.0f, 1, 0, 0x300, 0},
    {&lbl_13_data_12AC, 1.0f, 1, 0, 0x300, 0},
    {&lbl_13_data_12EC, 3.0f, 10, 0, 0x300, 0},
    {&lbl_13_data_1320, 1.0f, 0, 0, 0x80, 0},
    {&lbl_13_data_1354, 3.0f, 0, 0, 0x80, 0},
    {&lbl_13_data_1354, 3.0f, 10, 0, 0x80, 0},
    {&lbl_13_data_137C, 3.0f, 10, 0, 0x80, 0},
    {&lbl_13_data_1398, 3.0f, 10, 0, 0x80, 0},
    {&lbl_13_data_13B4, 3.0f, 10, 0, 0x80, 0},
};

s32 lbl_13_bss_348;
u8 lbl_13_bss_378[0x10];
u8 lbl_13_bss_36C[0xC];
u8 lbl_13_bss_34C[0x20];

void item_pilot_init(struct Item *item)
{
    item->unk12 = -1;
    item->state = 1;
    item->modelLODs = lbl_13_data_13C4[item->subType].model;
    item->flags = 2;
    item->unk14 = lbl_13_data_13C4[item->subType].radius;
    item->unk18 = 0.25f;
    item->rotVelX = lbl_13_data_13C4[item->subType].rotVelX;
    item->rotVelY = lbl_13_data_13C4[item->subType].rotVelY;
    item->rotVelZ = lbl_13_data_13C4[item->subType].rotVelZ;
    item->shadowModel = init_common_gma->modelEntries[0x67].model;
    item->shadowColor.r = 0x46;
    item->shadowColor.g = 0x47;
    item->shadowColor.b = 0x5F;
    item->unk7C.x = 1.0f;
    item->unk7C.y = 0.7f * item->unk7C.x;
    item->unk7C.z = item->unk7C.x;
}

void item_pilot_tick(struct Item *item)
{
    float groundY;

    if (item->unk64 != 0)
        groundY = item->pos.y + item->unk74;
    else
        groundY = -100.0f;
    if (item->state == 0)
        return;
    switch (item->state)
    {
    case 1:
        item->state = 2;
        /* fall through */
    case 2:
        if (item->subType == 3 || item->subType == 1)
        {
            item->vel.y -= 0.008f;
            if (item->pos.y < -1.0f)
                item->state = 6;
        }
        else if (item->subType == 4)
        {
            {
                struct Ball *ball = current_ball;
                Vec delta;
                Vec tmp;

                PSVECSubtract(&ball->pos, &item->pos, &delta);
                if (mathutil_vec_len(&delta) < 60.0f && mathutil_vec_dot_prod(&ball->vel, &delta) < 0.0f)
                {
                    PSVECScale(&ball->vel, &tmp, 60.0f);
                    PSVECAdd(&delta, &tmp, &delta);
                    vec_normalize_len(&delta);
                    PSVECScale(&delta, &delta, 0.005f);
                    if (item->pos.y < 30.0f && item->vel.y < 0.0f)
                    {
                        delta.y = 0.0f;
                        item->vel.y *= 0.9f;
                    }
                    PSVECAdd(&item->vel, &delta, &item->vel);
                    if (mathutil_vec_len(&item->vel) > 0.06481481343507767f)
                    {
                        vec_normalize_len(&item->vel);
                        PSVECScale(&item->vel, &item->vel, 0.06481481343507767f);
                    }
                }
                else
                    PSVECScale(&item->vel, &item->vel, 0.9f);
            }
        }
        else if (item->subType == 2)
        {
            {
                int i;
                int found = 0;

                for (i = 0; i < 4; i++)
                {
                    struct Ball *ball = &balls[i];

                    if (ball->status == 2)
                    {
                        Vec delta;
                        Vec tmp;

                        PSVECSubtract(&ball->pos, &item->pos, &delta);
                        if (mathutil_vec_len(&delta) < 30.0f && mathutil_vec_dot_prod(&ball->vel, &delta) < 0.0f)
                        {
                            found += 1;
                            PSVECScale(&ball->vel, &tmp, 60.0f);
                            PSVECAdd(&delta, &tmp, &delta);
                            vec_normalize_len(&delta);
                            PSVECScale(&delta, &delta, 0.02f);
                            if (item->pos.y < 30.0f && item->vel.y < 0.0f)
                            {
                                delta.y = 0.0f;
                                item->vel.y *= 0.9f;
                            }
                            PSVECAdd(&item->vel, &delta, &item->vel);
                        }
                    }
                }
                if (found == 0)
                    PSVECScale(&item->vel, &item->vel, 0.9f);
                if (mathutil_vec_len(&item->vel) > 0.046296294778585434f)
                {
                    vec_normalize_len(&item->vel);
                    PSVECScale(&item->vel, &item->vel, 0.046296294778585434f);
                }
            }
        }
        break;
    case 3:
        item->state = 4;
        /* fall through */
    case 4:
        item->state = 5;
        item->unk10 = 15;
        /* fall through */
    case 5:
        item->unk10--;
        if (item->unk10 < 0)
            item->state = 6;
        break;
    case 6:
        item->unk14 -= 0.033333333333333333;
        if (item->unk14 < 1.1920928955078125e-07f)
        {
            item->state = 0;
            item->flags |= ITEM_FLAG_INVISIBLE;
            item->unk14 = 1.1920928955078125e-07f;
        }
        break;
    }

    item->prevPos = item->pos;

    item->prevRotX = item->rotX;
    item->prevRotY = item->rotY;
    item->prevRotZ = item->rotZ;

    PSVECScale(&item->vel, &item->vel, 1.0f);
    PSVECAdd(&item->pos, &item->vel, &item->pos);

    item->rotX += item->rotVelX;
    item->rotY += item->rotVelY;
    item->rotZ += item->rotVelZ;

    if (item->animGroupId == 0)
        g_something_with_translating_items(2, &item->pos, 1.0f);
    else
    {
        Vec spC;

        mtxa_from_mtx(itemgroups[item->animGroupId].transform);
        mtxa_tf_point(&item->pos, &spC);
        g_something_with_translating_items(2, &spC, 1.0f);
    }
    if (item->pos.y - groundY < item->unk14)
    {
        item->pos.y = groundY + item->unk14;
        if (item->vel.y < 0.0f)
            item->vel.y *= -0.4f;
    }
    item->unk6C.z = -item->rotY;
}

void item_pilot_disp(struct Item *item)
{
    struct GMAModel *model;
    float scale;
    float size;
    float alpha;
    Vec spC;

    if (g_some_render_flag & 4)
        return;
    mtxa_from_mtxb_translate(&item->pos);
    mtxa_sq_from_mtx(g_ord_tbl_stuff->matrices[2]);
    mtxa_rotate_y(item->rotY);
    mtxa_rotate_x(item->rotX);
    mtxa_rotate_z(item->rotZ);
    model = fn_13_115D4(item->modelLODs);
    if (model == NULL)
        return;
    scale = ((struct PilotModel *)item->modelLODs)->scale;
    size = model->boundSphereRadius * scale;
    if (g_is_sphere_visible_scaled(&model->boundSphereCenter, model->boundSphereRadius, scale) == 0)
        return;
    if (lbl_13_bss_348 == 1)
        mtxa_sq_from_identity();
    if (scale != 1.0f)
        mtxa_scale_s(scale);
    mathutil_mtxA_get_translate(&spC);
    alpha = -(((spC.z + size) + 0.1f) / size);
    if (alpha <= 0.0f)
        return;
    avdisp_set_scale_factor(scale);
    load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
    if (alpha < 1.0f)
    {
        if (alpha < 0.5f)
            alpha = 0.5f;
        avdisp_set_alpha(alpha);
        avdisp_draw_model_unculled_sort_always(model);
    }
    else
        avdisp_draw_model_unculled_sort_never(model);
    if (item->subType == 2)
    {
        float r, g, b;

        switch ((g_font_char_var_3 / 10) & 3)
        {
        case 0:
            r = 1.0f;
            g = 1.0f;
            b = 1.0f;
            break;
        case 1:
            r = 0.0f;
            g = 0.0f;
            b = 1.0f;
            break;
        case 2:
            r = 1.0f;
            g = 1.0f;
            b = 0.0f;
            break;
        default:
            r = 1.0f;
            g = 0.0f;
            b = 0.0f;
            break;
        }
        set_post_mult_color(r, g, b, 1.0f);
        mtxa_sq_from_identity();
        mtxa_scale_s(2.5f);
        load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
        avdisp_draw_model_unculled_sort_auto(minigameGma[0]->modelEntries[0x4A].model);
        set_post_mult_color(1.0f, 1.0f, 1.0f, 1.0f);
    }
}

void item_pilot_coli(struct Item *item, struct PhysicsBall *physBall)
{
    struct Ball *ball = current_ball;
    struct Pilot2Player *player = &lbl_13_bss_70[ball->playerId];
    int idx;

    item->flags &= ~(1 << 1);
    item->state = 3;
    switch (item->subType)
    {
    case 0:
    case 1:
    case 2:
    case 6:
    case 7:
    case 8:
        item->unk5E = mode_info.timerCurr;
        fn_13_11450(item, physBall);
        break;
    }
    switch (item->subType)
    {
    case 0:
    case 1:
    case 2:
        player->unk4E++;
        if (item->subType == 2)
        {
            call_SoundReqID_arg_0(0x39);
            call_SoundReqID_arg_0(0x2820);
        }
        else
        {
            call_SoundReqID_arg_0(3);
            call_SoundReqID_arg_0(0x281F);
        }
        break;
    case 6:
        player->unk48++;
        if (player->unk48 > 4)
            player->unk48 = 4;
        call_SoundReqID_arg_0(0x39);
        break;
    case 7:
        player->unk4A++;
        if (ball->restitution > 0.07f)
            ball->restitution = 0.07f;
        call_SoundReqID_arg_0(0x39);
        break;
    case 8:
        player->unk4C++;
        if (ball->restitution > 0.02f)
            ball->restitution = 0.02f;
        call_SoundReqID_arg_0(0x39);
        break;
    case 5:
        {
            struct Effect effect;

            player->unk20 += 60;
            memset(&effect, 0, sizeof(effect));
            effect.type = 0x27;
            effect.playerId = ball->playerId;
            effect.pos.x = ball->pos.x;
            effect.pos.y = ball->pos.y - 1.0f;
            effect.pos.z = ball->pos.z;
            effect.scale = (Vec){3.5f, 4.5f, 3.5f};
            spawn_effect(&effect);
        }
        break;
    case 3:
        {
            struct Effect effect;

            player->flags |= 0x42;
            call_SoundReqID_arg_0(0x10B);
            call_SoundReqID_arg_0(0x1C);
            g_rumble_controller(player_pad_map[ball->playerId], 1, 0x1C);
            physBall->vel.y += 0.9259259104728699f;
            player->unk38 = 0.6f;
            memset(&effect, 0, sizeof(effect));
            effect.type = 0x27;
            effect.playerId = ball->playerId;
            effect.pos.x = ball->pos.x;
            effect.pos.y = ball->pos.y - 1.0f;
            effect.pos.z = ball->pos.z;
            effect.scale = (Vec){3.5f, 4.5f, 3.5f};
            spawn_effect(&effect);
        }
        break;
    case 4:
        {
            struct Effect effect;
            int i;

            player->flags |= 0x82;
            call_SoundReqID_arg_0(0x2F);
            call_SoundReqID_arg_0(0x1C);
            if (ball->unk154 == 4)
                call_SoundReqID_arg_0(0x16C);
            g_rumble_controller(player_pad_map[ball->playerId], 1, 0x1C);
            physBall->vel.y += 0.1388888955116272f;
            physBall->vel.x += 0.6481481194496155f * ((float)rand() / 32767.0f);
            physBall->vel.z += 0.6481481194496155f * ((float)rand() / 32767.0f);
            player->unk38 = 0.6f;
            player->unk10 = 1.0f;
            memset(&effect, 0, sizeof(effect));
            effect.type = 5;
            effect.playerId = ball->playerId;
            effect.pos = ball->pos;
            for (i = 0; i < 30; i++)
            {
                effect.vel.x = physBall->vel.x + (0.2 * ((float)rand() / 32767.0f) - 0.1);
                effect.vel.y = 0.1 + physBall->vel.y + (0.2 * ((float)rand() / 32767.0f) - 0.1);
                effect.vel.z = physBall->vel.z + (0.2 * ((float)rand() / 32767.0f) - 0.1);
                spawn_effect(&effect);
            }
        }
        break;
    }
    if (lbl_1_bss_D4A10 & 1)
        idx = player->unk41;
    else
        idx = ball->playerId;
    if (lbl_13_data_13C4[item->subType].points > 0)
    {
        struct Pilot2Player *p = &lbl_13_bss_70[idx];
        int points = lbl_13_data_13C4[item->subType].points;

        lbl_13_bss_50[idx] += points;
        p->unk30 = points;
        p->unk26 = 0x28;
        if (lbl_13_bss_218[idx] < 0x1E)
            lbl_13_bss_218[idx] = 0x1E;
    }
}

void item_pilot_dest(struct Item *item) {}

void item_pilot_replay_init(struct Item *item) {}

void fn_13_11450(struct Item *item, struct PhysicsBall *physBall)
{
    struct Effect effect;
    struct Ball *ball = current_ball;

    item->vel.y += 0.1875 * item->unk14;
    item->rotVelY <<= 2;
    item->state = 0;
    item->flags |= ITEM_FLAG_INVISIBLE;
    item->flags &= ~(1 << 1);
    memset(&effect, 0, sizeof(effect));
    mtxa_from_mtx(itemgroups[physBall->itemGroupId].transform);
    mtxa_tf_point(&item->pos, &effect.pos);
    mtxa_tf_vec(&item->vel, &effect.vel);
    effect.model = fn_13_115D4(item->modelLODs);
    if (effect.model != NULL)
    {
        switch (item->subType)
        {
        case 0:
        case 1:
        case 2:
            effect.flags |= 0x10000000;
            break;
        }
        effect.type = 0x28;
        effect.playerId = ball->playerId;
        effect.rotX = item->rotX;
        effect.rotY = item->rotY;
        effect.rotZ = item->rotZ;
        effect.scale.x = 0.7f * ((struct PilotModel *)item->modelLODs)->scale;
        effect.scale.y = effect.scale.x;
        effect.scale.z = effect.scale.x;
        spawn_effect(&effect);
    }
}

void item_debug_pilot(struct Item *item) {}

struct GMAModel *fn_13_115D4(struct PilotModel *pilotModel)
{
    struct PilotLOD *lods = pilotModel->lods;
    struct GMAModelEntry *entries = (*pilotModel->gma)->modelEntries;
    struct GMAModel *model = entries[lods[0].modelId].model;
    int lod;
    float radius;
    Vec sp8;

    mtxa_tf_point(&model->boundSphereCenter, &sp8);
    radius = model->boundSphereRadius;
    radius *= pilotModel->scale;
    if (sp8.z > radius)
        lod = pilotModel->count - 1;
    else if (sp8.z > -0.1f)
        lod = 0;
    else
    {
        float size = (g_current_camera->sub28.viewportH * -480.0f) * radius / (sp8.z * g_current_camera->sub28.view.tanHalfFov);
        int i;

        for (i = 0; i < pilotModel->count; i++)
        {
            lod = i;
            if (lods[i].distance < size)
                break;
        }
    }
    lbl_13_bss_348 = lods[lod].flag;
    if (lbl_13_bss_348 == 2)
        model = NULL;
    else
        model = entries[lods[lod].modelId].model;
    return model;
}
