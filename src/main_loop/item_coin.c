/* Banana items (SMB1 item_coin.c): init/tick/draw/collect for the single banana and the
 * banana bunch, plus the LOD picker shared with the effect that flies to the HUD. */
#include "types.h"
#include "avdisp.h"
#include "ball.h"
#include "camera.h"
#include "effect.h"
#include "frustum.h"
#include "item.h"
#include "libc.h"
#include "mathutil.h"
#include "mode.h"
#include "stage.h"

void g_something_with_translating_items(int type, Vec *pos, float weight);
void call_SoundReqID_arg_0(int soundId);
void add_bananas(int count);
void increment_score(int type, int score);
void call_item_coin_coli_func_for_cur_world_theme(int a);
void g_smth_with_gpu_and_locked_cache_mtx(void);
void mtxa_sq_from_mtx(Mtx mtx);
int debug_window_printf(int buf, char *fmt, ...);
extern struct
{
    /*0x00*/ u32 flags;
} g_some_render_flag;

u32 lbl_1_data_37C98[0x5C] __attribute__((aligned(8))) =
{
    0xFE000102, 0x03040506, 0x0708090A, 0x0B0CFE0D, 0x0E0FFF00, 0x00010203,
    0x04050607, 0x08090A0B, 0x0C0D0E0F, 0xFF000000, 0x31000000, 0x00000000,
    0x3800B400, 0x00010000, 0x01000000, 0x00020000, 0x10004C00, 0x00030000,
    0x01000000, 0x00040000, 0x18000000, 0x00090000, 0x40000000, 0x00000000,
    0x01000000, 0x000B0000, 0x18000000, 0x000C0000, 0x18000000, 0x00050000,
    0x40000000, 0x00000000, 0x01000000, 0x00070000, 0x18000000, 0x00080000,
    0x3800B400, 0x000D0000, 0x40000000, 0x00000000, 0x18000000, 0x00140000,
    0x40000000, 0x00000000, 0x01000000, 0x00160000, 0x18000000, 0x00170000,
    0x18000000, 0x00180000, 0x18000000, 0x000F0000, 0x40000000, 0x00000000,
    0x01000000, 0x00110000, 0x18000000, 0x00120000, 0x18000000, 0x00130000,
    0x18000000, 0x00000000, 0x18000000, 0x00010000, 0x18000000, 0x00020000,
    0x18000000, 0x00030000, 0x18000000, 0x00040000, 0x18000000, 0x00050000,
    0x18000000, 0x00060000, 0x18000000, 0x00070000, 0x18000000, 0x00080000,
    0x18000000, 0x00090000, 0x18000000, 0x000A0000, 0x18000000, 0x000B0000,
    0x18000000, 0x000C0000, 0x18000000, 0x000D0000, 0x18000000, 0x000E0000,
    0x18000000, 0x000F0000,
};

struct ModelLOD s_singleBananaLODs[] =
{
    { 0x41, 150 },
    { 0x40, 100 },
    { 0x42,  50 },
    { 0x3F,   0 },
    { -1, 0 },
};


/* Per banana type immutable info */
struct BananaInfo
{
    struct ModelLOD **modelLODs;
    float unk4;
    s16 bananaValue;  /* number of bananas given when collected */
    s16 unkA;
    s16 pointValue;  /* number of points given when collected */
    s16 rotVelX;
    s16 rotVelY;
    s16 rotVelZ;
};

struct ModelLOD *s_singleBananaLODsPtr = s_singleBananaLODs;

struct ModelLOD s_bananaBunchLODs[] =
{
    { 0x44, 100 },
    { 0x43,   0 },
    { -1, 0 },
};

struct ModelLOD *s_bananaBunchLODsPtr = s_bananaBunchLODs;

struct BananaInfo s_bananaInfos[] =
{
    { &s_singleBananaLODsPtr,  0.5,  1, 0,  100, 0, 1024,    0 },
    { &s_bananaBunchLODsPtr,  0.75, 10, 1, 1000, 0,  768,    0 },
    { &s_singleBananaLODsPtr,  0.5,  1, 0,  100, 0, 1024, -128 },
    { &s_bananaBunchLODsPtr,  0.75, 10, 1, 1000, 0, 1024, -128 },
};

void item_coin_init(struct Item *item)
{
    item->unk12 = -1;
    item->state = 1;
    item->modelLODs = s_bananaInfos[item->subType].modelLODs;
    item->flags = 0x22;
    item->unk14 = s_bananaInfos[item->subType].unk4;
    item->unk18 = 0.25f;
    item->rotVelX = s_bananaInfos[item->subType].rotVelX;
    item->rotVelY = s_bananaInfos[item->subType].rotVelY;
    item->rotVelZ = s_bananaInfos[item->subType].rotVelZ;
    item->shadowModel = init_common_gma->modelEntries[103].model;
    item->shadowColor.r = 0x46;
    item->shadowColor.g = 0x47;
    item->shadowColor.b = 0x5F;
    item->unk7C.x = item->unk14;
    item->unk7C.y = item->unk14 * 0.8f;
    item->unk7C.z = item->unk14;
    if (current_stage_id == 300)
    {
        item->flags |= ITEM_FLAG_ON_STAGE | ITEM_FLAG_NO_RAYCAST;
        item->hit.flags = 1;
        item->hit.pos.x = 0.0f;
        item->hit.pos.y = 0.0f;
        item->hit.pos.z = 0.0f;
        item->hit.normal.x = 0.0f;
        item->hit.normal.y = 1.0f;
        item->hit.normal.z = 0.0f;
    }
}

void item_coin_tick(struct Item *item)
{
    if (item->state == 0)
        return;
    switch (item->state)
    {
    case 1:
        item->state = 2;
        break;
    case 2:
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

    item->pos.x += item->vel.x;
    item->pos.y += item->vel.y;
    item->pos.z += item->vel.z;

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
    item->unk6C.z = -item->rotY;
    item->unk7C.x = item->unk14;
    item->unk7C.y = item->unk14 * 0.7f;
}

void item_coin_disp(struct Item *item)
{
    float scale;
    float f30 = item->unk14;
    struct GMAModel *model;
    Vec spC;

    mtxa_from_mtxb();
    mtxa_translate(&item->pos);
    mtxa_sq_from_mtx(g_ord_tbl_stuff->matrices[2]);
    mtxa_rotate_y(item->rotY);
    mtxa_rotate_x(item->rotX);
    mtxa_rotate_z(item->rotZ);
    model = g_something_with_coins(item->modelLODs);
    scale = (f30 / model->boundSphereRadius) * 1.5;
    if (g_is_sphere_visible_scaled(&model->boundSphereCenter, model->boundSphereRadius, scale) == 0)
        return;
    if (scale != 1.0)
        mtxa_scale_xyz(scale, scale, scale);
    mathutil_mtxA_get_translate(&spC);
    f30 = -(((spC.z + f30) + 0.1f) / f30);
    if (modeCtrl.gameType == GAMETYPE_MAIN_COMPETITION && (current_ball->flags & BALL_FLAG_GOAL))
        f30 = 0.25f;
    if (f30 > 0.0f)
    {
        if (f30 < 1.0f)
            avdisp_set_alpha(f30);
        if (f30 < 1.0f || (g_some_render_flag.flags & (1 << 8)))
            avdisp_draw_model_unculled_sort_always(model);
        else
        {
            g_smth_with_gpu_and_locked_cache_mtx();
            avdisp_draw_model_unculled_sort_never(model);
        }
    }
}

void item_coin_coli(struct Item *item, struct PhysicsBall *ball)
{
    if (modeCtrl.gameType == GAMETYPE_MAIN_COMPETITION && (current_ball->flags & BALL_FLAG_GOAL))
        return;
    item->flags &= ~(1 << 1);
    item->state = 3;
    item->vel.y += item->unk14 * 0.1875;
    item->rotVelY <<= 2;
    item->vel.x += ball->vel.x * 0.25;
    item->vel.y += ball->vel.y * 0.25;
    item->vel.z += ball->vel.z * 0.25;
    if (item->unk5E < 0 && !(current_ball->flags & BALL_FLAG_REPLAY) && sub_mode != 8)
    {
        struct Effect effect;

        item->unk5E = mode_info.timerCurr;
        add_bananas(s_bananaInfos[item->subType].bananaValue);
        increment_score(s_bananaInfos[item->subType].unkA, s_bananaInfos[item->subType].pointValue);
        item->state = 0;
        item->flags |= ITEM_FLAG_INVISIBLE;
        item->flags &= ~(1 << 1);

        /* spawn banana effect that travels towards counter in HUD */
        memset(&effect, 0, sizeof(effect));
        effect.type = 3;
        effect.playerId = current_ball->playerId;
        mtxa_from_mtx(itemgroups[ball->itemGroupId].transform);
        mtxa_tf_point(&item->pos, &effect.pos);
        mtxa_tf_vec(&item->vel, &effect.vel);
        effect.rotX = item->rotX;
        effect.rotY = item->rotY;
        effect.rotZ = item->rotZ;
        effect.model = g_something_with_coins(item->modelLODs);
        effect.scale.x = (item->unk14 / effect.model->boundSphereRadius) * 1.5;
        effect.scale.y = effect.scale.x;
        effect.scale.z = effect.scale.y;
        spawn_effect(&effect);
    }
    if (item->subType == 1)
    {
        call_SoundReqID_arg_0(0x39);
        if ((mode_info.flags & INFO_FLAG_11) || !(mode_info.flags & INFO_FLAG_REPLAY))
            call_SoundReqID_arg_0(0x2820);
        call_item_coin_coli_func_for_cur_world_theme(1);
    }
    else
    {
        call_SoundReqID_arg_0(3);
        if ((mode_info.flags & INFO_FLAG_11) || !(mode_info.flags & INFO_FLAG_REPLAY))
            call_SoundReqID_arg_0(0x281F);
        call_item_coin_coli_func_for_cur_world_theme(0);
    }
}

void item_coin_dest(struct Item *item) {}

void item_coin_replay_init(struct Item *item)
{
    if (item->state != 2)
    {
        item->pos = item->stageBanana->pos;
        item->vel.x = 0.0f;
        item->vel.y = 0.0f;
        item->vel.z = 0.0f;
        item_coin_init(item);
    }
}

char wtfisthis[] =
{
    0x16, 0x16, 0x16, 0x16,
    0x16, 0x16, 0x16, 0x16,
    0x16, 0x16, 0x16, 0x16,
    0x16, 0x16, 0x16, 0x16,
    0x16, 0x16, 0x16, 0x16,
    0x16, 0x16, 0x16, 0x0A,
    0x00
};

void item_debug_coin(struct Item *item)
{
    debug_window_printf(2, wtfisthis);
    debug_window_printf(2, "Coin Value: %d\n", s_bananaInfos[item->subType].bananaValue);
}

/* needed to force float constant ordering */
float item_coin_dummy(void) { return -480.0f; }

struct GMAModel *g_something_with_coins(struct ModelLOD **a)
{
    struct ModelLOD *r31 = *a;
    struct GMAModel *model;
    int modelId;
    float f31;
    Vec spC;
    float f1;

    modelId = r31->modelId;
    model = init_common_gma->modelEntries[modelId].model;
    f31 = model->boundSphereRadius;
    mtxa_tf_point(&model->boundSphereCenter, &spC);
    if (spC.z > f31)
    {
        while (r31->modelId > 0)
        {
            modelId = r31->modelId;
            r31++;
        }
        return init_common_gma->modelEntries[modelId].model;
    }
    if (spC.z > -0.1f)
        return model;
    f1 = (g_current_camera->sub28.viewportH * -480.0f) * f31 / (spC.z * g_current_camera->sub28.view.tanHalfFov);
    while (r31->modelId > 0)
    {
        modelId = r31->modelId;
        if (r31->distance < f1)
            break;
        r31++;
    }
    model = init_common_gma->modelEntries[modelId].model;
    return model;
}
