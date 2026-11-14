/* Dogfight weapons: shots and rockets, targeting, and damage scoring. */
#include "types.h"
#include "ape_internal.h"
#include "avdisp.h"
#include "ball.h"
#include "camera.h"
#include "effect.h"
#include "gx.h"
#include "input.h"
#include "libc.h"
#include "light.h"
#include "mathutil.h"
#include "mode.h"
#include "nl2ngc.h"
#include "rand.h"
#include "stage.h"

struct DogfightPlayer
{
    u32 flags;
    u8 filler4[0x1C - 0x4];
    s16 unk1C;
    s8 unk1E[4];
    s16 unk22[4];
    u8 filler2A[2];
    s16 unk2C;
    s16 unk2E;
    s16 unk30;
    s16 unk32;
    s16 unk34;
    u8 filler36[2];
    s16 unk38;
    s8 unk3A;
    s8 unk3B;
    s16 unk3C;
    u8 filler3E[2];
    s32 unk40;
    s32 unk44;
    u8 filler48[4];
    float unk4C;
    u8 filler50[0x5C - 0x50];
    s16 unk5C;
    s8 unk5E;
    u8 filler5F;
    u8 unk60;
    s8 unk61;
    u8 unk62;
    u8 unk63;
    u8 unk64;
    u8 unk65;
    u8 unk66;
    s8 unk67;
    u8 filler68[4];
    s32 unk6C;
    u32 unk70;
    u8 filler74[0x90 - 0x74];
    s8 unk90;
    s8 unk91;
    u8 filler92[0xB0 - 0x92];
};

struct DogfightShot
{
    s8 status;
    s8 index;
    s8 owner;
    s8 target;
    u8 unk4;
    s8 unk5;
    u8 filler6[2];
    s32 type;
    s32 flags;
    s16 age;
    s16 unk12;
    Vec pos;
    Vec prevPos;
    Vec vel;
    Vec aim;
    Mtx mtx;
    s16 unk74;
    u8 filler76[2];
    float radius;
    float unk7C;
    float speed;
    s32 unk84;
    s16 trailCount;
    u8 filler8A[2];
    Vec trail[80];
    s16 trailTex[80];
};

extern struct GMA *minigameGma[];
extern float lbl_80198B34;
extern u32 lbl_1_bss_D4CDC;
extern s8 lbl_1_bss_D4D2D;
extern u32 lbl_1_bss_D4D30;
extern s16 lbl_17_bss_14;
extern s32 lbl_17_bss_24;
extern s32 lbl_17_bss_2C[];
extern struct DogfightPlayer lbl_17_bss_48[];
extern s32 lbl_17_bss_334[];
extern s32 lbl_17_bss_348;
extern s32 lbl_17_bss_34C;
extern s16 lbl_17_bss_374[];
extern u32 g_some_other_flags;
extern s32 g_player_id_for_sound;
extern s32 g_smth_with_sound;
void PSVECAdd(Vec *a, Vec *b, Vec *dst);
void PSVECSubtract(Vec *a, Vec *b, Vec *dst);
void PSVECScale(Vec *src, Vec *dst, float scale);
void call_SoundReqID_arg_0(int soundId);
void SoundOffID(int soundId);
void SoundVolID(int soundId, u8 vol);
void SoundPanID(int soundId, u8 a, u8 b);
s8 fn_1_2F8F0(int a, Vec *pos, s8 *pan, s8 *span);
struct OrdTblNode *fn_1_CCC18(float z);
void g_rumble_controller(int pad, int a, int b);
int g_some_item_collision_check(Vec *prevPos, Vec *pos, float radius, Vec *a, Vec *b, float c);
void g_something_with_GX_vtx_desc(u32 attrs);
void opti_GXSetChanCtrl(int chan, u8 enable, int ambSrc, int matSrc, u32 lightMask, int diffFn, int attnFn);
void g_GXSetTevIndirect_zero_if_different(int a);
void GXLoadTexObj_cached(void *obj, int map);
void GXSetTexCoordGen2_cached(int dst, int func, int src, int mtx, int normalize, int postMtx);
void g_GXSetTevOrder_wrapper(int stage, int coord, int map, int color);
void g_GXSetTevColorIn_GXSetTevAlphaIn_wrapper(int a, int b);
void GXSetNumTevStages_cached(u8 n);
void GXSetNumTexGens_cached(u8 n);
void GXSetNumIndStages_cached(u8 n);
void GXSetNumChans_cached(u8 n);
void GXSetCullMode_cached(int mode);
int fn_17_16ED8(Vec *pos, Vec *out, float tanHalfFov, float aspect);

void fn_17_119A8(struct DogfightShot *shot);
void fn_17_11A40(struct OrdTblNode *arg);
void fn_17_11AFC(int count, Vec *points, struct DogfightShot *shot);
void fn_17_11EE4(struct DogfightShot *shot);
void fn_17_122F4(struct DogfightShot *shot);
int fn_17_128AC(struct Ball *ball);
int fn_17_12C0C(struct Ball *ball);
void fn_17_12EC0(struct Ball *ball);
void fn_17_136C4(struct Ball *ball);
void fn_17_139A4(struct DogfightShot *shot);
void fn_17_14030(struct DogfightShot *shot);
void fn_17_146D8(struct DogfightShot *shot, struct PhysicsBall *physBall);
void fn_17_14784(int playerId, int score, int time);
void fn_17_14804(int playerId, int score, int time);
void fn_17_1487C(int playerId, int score, int time);
int fn_17_148F4(int score);

struct ShotNode
{
    struct OrdTblNode node;
    int lightGroup;
    struct DogfightShot *shot;
};

static const s16 lbl_17_rodata_980[4] = {0x2B0, 0x2B1, 0x2B2, 0x2B3};

s8 lbl_17_bss_3D8;
s8 lbl_17_bss_3D9;
struct DogfightShot lbl_17_bss_3DC[0x3C];
s8 lbl_17_bss_12B2C;
s8 lbl_17_bss_12B2D;
s8 lbl_17_bss_12B2E;
s8 lbl_17_bss_12B2F;

void (*lbl_17_data_13F8[])(struct DogfightShot *) = {fn_17_11EE4, fn_17_122F4};
Vec lbl_17_data_1400 = {0.0f, -0.5f, -0.9f};

void fn_17_116EC(void)
{
    int i;

    switch (lbl_17_bss_24)
    {
    case 0:
        lbl_17_bss_3D8 = 1;
        break;
    case 4:
    case 1:
        lbl_17_bss_3D8 = 2;
        break;
    case 2:
        lbl_17_bss_3D8 = 3;
        break;
    default:
        lbl_17_bss_3D8 = 5;
        break;
    }
    lbl_17_bss_3D9 = 0xA0 / (lbl_17_bss_3D8 + 1);
    for (i = 0; i < 0x3C; i++)
    {
        struct DogfightShot *shot = &lbl_17_bss_3DC[i];

        shot->status = 0;
    }
    lbl_17_bss_12B2C = 0;
    lbl_17_bss_12B2D = 0;
}

void fn_17_117E8(void)
{
    int i;
    struct DogfightShot *shot;

    if (!(g_some_other_flags & 0xA))
    {
        lbl_17_bss_12B2E = lbl_17_bss_12B2C;
        lbl_17_bss_12B2F = lbl_17_bss_12B2D;
        lbl_17_bss_12B2C = 0;
        lbl_17_bss_12B2D = 0;
        for (i = 0; i < 0x3C; i++)
        {
            shot = &lbl_17_bss_3DC[i];
            if (shot->status != 0)
            {
                lbl_17_data_13F8[shot->type](shot);
                shot->flags &= ~4;
            }
        }
    }
}

void fn_17_118BC(void)
{
    int i;
    struct DogfightShot *shot;
    float scale;

    for (i = 0; i < 0x3C; i++)
    {
        shot = &lbl_17_bss_3DC[i];
        if (shot->status != 0)
        {
            if (shot->type == 1)
            {
                if (!(shot->flags & 3))
                {
                    mtxa_from_mtxb_translate(&shot->pos);
                    mtxa_mult_right(shot->mtx);
                    scale = 2.0f;
                    mtxa_scale_s(scale);
                    g_smth_with_gpu_and_locked_cache_mtx();
                    avdisp_set_scale_factor(scale);
                    avdisp_draw_model_culled_sort_always(minigameGma[0]->modelEntries[0x84].model);
                }
            }
            else if (shot->type == 0)
            {
                fn_17_119A8(shot);
            }
        }
    }
}

void fn_17_119A8(struct DogfightShot *shot)
{
    Vec v;
    struct ShotNode *node;
    struct OrdTblNode *list;

    mtxa_from_mtxb();
    mtxa_tf_point(&current_ball->pos, &v);
    list = fn_1_CCC18(v.z);
    node = ord_tbl_alloc_node(sizeof(*node));
    if (node != NULL)
    {
        node->node.drawFunc = fn_17_11A40;
        node->lightGroup = get_active_light_group();
        node->shot = shot;
        fn_1_CD418(list, &node->node);
    }
}

void fn_17_11A40(struct OrdTblNode *arg)
{
    struct ShotNode *node = (struct ShotNode *)arg;
    struct DogfightShot *shot = node->shot;
    int count;

    load_light_group_cached(node->lightGroup);
    if (!(shot->flags & 3))
    {
        mtxa_from_mtxb_translate(&shot->pos);
        mtxa_mult_right(shot->mtx);
        g_smth_with_gpu_and_locked_cache_mtx();
        avdisp_draw_model_culled_sort_never(minigameGma[0]->modelEntries[0x79].model);
    }
    if (shot->trailCount > 0)
    {
        count = lbl_17_bss_3D9 - shot->unk84;
        if (count > shot->trailCount)
        {
            count = shot->trailCount;
        }
        mtxa_from_mtxb();
        fn_17_11AFC(count, shot->trail, shot);
    }
}

void fn_17_11AFC(int count, Vec *points, struct DogfightShot *shot)
{
    Vec side;
    Vec toCam;
    Vec dir;
    int i;
    float v;
    u8 r;
    u8 g;
    u8 b;
    s16 alpha;
    int next;

    if (count >= 2)
    {
        g_something_with_GX_vtx_desc(0x2A00);
        GXSetBlendMode_cached(1, 4, 1, 0);
        opti_GXSetChanCtrl(4, 0, 1, 1, 0, 2, 1);
        g_GXSetTevIndirect_zero_if_different(0);
        GXLoadTexObj_cached(minigameGma[0]->modelEntries[0x75].model->texObjs, 0);
        GXSetTexCoordGen2_cached(0, 1, 4, 0x3C, 0, 0x7D);
        g_GXSetTevOrder_wrapper(0, 0, 0, 4);
        g_GXSetTevColorIn_GXSetTevAlphaIn_wrapper(0, 0);
        GXSetNumTevStages_cached(1);
        GXSetNumTexGens_cached(1);
        GXSetNumIndStages_cached(0);
        GXSetNumChans_cached(1);
        GXSetCullMode_cached(0);
        GXSetZMode_cached(1, 3, 0);
        load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
        GXBegin(0x98, 0, count * 2);
        for (i = 0; i < count; i++)
        {
            r = 0xFF;
            g = 0xFF;
            b = 0xFF;
            v = shot->trailTex[i];
            alpha = ((lbl_17_bss_3D9 - shot->unk84 - i) * 0x90) / lbl_17_bss_3D9;
            next = i + 1;
            if (next == count)
            {
                next = i - 1;
            }
            PSVECSubtract(&g_current_camera->eye, &points[i], &toCam);
            PSVECSubtract(&points[next], &points[i], &dir);
            if (next > i)
            {
                PSVECSubtract(&points[next], &points[i], &dir);
            }
            else
            {
                PSVECSubtract(&points[i], &points[next], &dir);
            }
            mathutil_vec_cross_prod(&toCam, &dir, &side);
            vec_set_len(0.15f, &side, &side);
            GXPosition3f32(points[i].x - side.x, points[i].y - side.y, points[i].z - side.z);
            GXColor4u8(r, g, b, alpha);
            GXTexCoord2f32(v, 0.0f);
            GXPosition3f32(side.x + points[i].x, side.y + points[i].y, side.z + points[i].z);
            GXColor4u8(r, g, b, alpha);
            GXTexCoord2f32(v, 1.0f);
        }
        GXSetZMode_cached(1, 3, 1);
    }
}

void fn_17_11EE4(struct DogfightShot *shot)
{
    int life;

    lbl_17_bss_12B2C++;
    shot->age++;
    life = (float)(lbl_80198B38 * 7) - (float)lbl_17_bss_12B2E * ((3.0f * lbl_80198B34) / 20.0f);
    if (life < lbl_80198B38 * 2)
    {
        life = lbl_80198B38 * 2;
    }
    if ((shot->target < 0 && shot->age > life) || shot->age > lbl_80198B38 * 7)
    {
        shot->flags |= 2;
    }
    if (!(shot->flags & 4))
    {
        shot->prevPos = shot->pos;
    }
    if (!(shot->flags & 3))
    {
        shot->unk84 = 0;
        fn_17_139A4(shot);
        PSVECAdd(&shot->pos, &shot->vel, &shot->pos);
        shot->unk74 = math_atan2(-shot->vel.x, -shot->vel.z);
        if (shot->unk5 <= 0)
        {
            if (shot->trailCount < lbl_17_bss_3D9)
            {
                shot->trailCount++;
            }
            memcpy(&shot->trail[1], &shot->trail[0], (lbl_17_bss_3D9 - 1) * sizeof(Vec));
            memcpy(&shot->trailTex[1], &shot->trailTex[0], (lbl_17_bss_3D9 - 1) * sizeof(s16));
            shot->trail[0] = shot->pos;
            shot->trailTex[0] = shot->unk4;
            shot->unk4++;
            shot->unk5 = lbl_17_bss_3D8;
        }
        else
        {
            if (shot->trailCount == 1)
            {
                shot->trailCount++;
                memcpy(&shot->trail[1], &shot->trail[0], (lbl_17_bss_3D9 - 1) * sizeof(Vec));
                memcpy(&shot->trailTex[1], &shot->trailTex[0], (lbl_17_bss_3D9 - 1) * sizeof(s16));
            }
            shot->unk5--;
            shot->trail[0] = shot->pos;
            shot->trailTex[0] = shot->unk4;
        }
        if (shot->radius < 0.75f)
        {
            shot->radius += 0.75f / (0.5f * lbl_80198B34);
        }
        fn_17_14030(shot);
        return;
    }
    shot->unk84++;
    if ((shot->flags & 8) && shot->target >= 0 && shot->unk84 == (int)(0.5f * lbl_80198B34)
     && lbl_17_bss_2C[shot->owner] != 0)
    {
        g_player_id_for_sound = shot->owner;
        call_SoundReqID_arg_0(0x29E);
    }
    if (shot->unk84 > lbl_17_bss_3D9)
    {
        shot->status = 0;
    }
}

void fn_17_122F4(struct DogfightShot *shot)
{
    int life;

    lbl_17_bss_12B2D++;
    shot->age++;
    life = (1.3f * lbl_80198B34) - (float)lbl_17_bss_12B2F * ((0.5f * lbl_80198B34) / 20.0f);
    if ((float)life < 0.5f * lbl_80198B34)
    {
        life = 0.5f * lbl_80198B34;
    }
    if ((!(shot->flags & 0x20) && shot->age > life) || (float)shot->age > 1.3f * lbl_80198B34)
    {
        shot->status = 0;
        return;
    }
    if (!(shot->flags & 4))
    {
        shot->prevPos = shot->pos;
    }
    if (!(shot->flags & 1))
    {
        PSVECAdd(&shot->pos, &shot->vel, &shot->pos);
        if (shot->radius < 1.1f)
        {
            shot->radius += 1.1f / (0.5f * lbl_80198B34);
        }
        fn_17_14030(shot);
    }
    if (shot->flags & 1)
    {
        shot->status = 0;
    }
}

static void init_shot(struct DogfightShot *shot, int index)
{
    memset(shot, 0, sizeof(*shot));
    shot->flags |= 4;
    shot->index = index;
    shot->radius = 1.0f;
    shot->unk7C = 0.1f;
    shot->speed = 1.0f;
}

void fn_17_12520(struct Ball *ball)
{
    s16 sounds[4] = {0x295, 0x296, 0x297, 0x298};
    u8 vol;
    u8 pan;
    struct DogfightPlayer *player;
    u16 fire;
    u16 rocket;
    int firing;
    s8 volume;

    player = &lbl_17_bss_48[ball->playerId];
    firing = 0;
    if (!(player->flags & 0x400) || lbl_17_bss_14 != 0xD)
    {
        fire = 0;
        rocket = 0;
    }
    else if (player->flags & 0x800)
    {
        fire = player->unk90;
        rocket = player->unk91;
    }
    else if (!(lbl_1_bss_D4CDC & 0x10))
    {
        fire = pad_status_groups[player->unk5E].unk0[0].button & 0x100;
        rocket = pad_status_groups[player->unk5E].unk0[2].button & 0x200;
    }
    else
    {
        fire = pad_status_groups[player->unk5E].unk0[0].button & 0x400;
        rocket = pad_status_groups[player->unk5E].unk0[3].button & 0x800;
    }
    if (player->unk2E > 0)
    {
        player->unk2E--;
    }
    if (player->unk2C > 0)
    {
        player->unk2C--;
    }
    fn_17_12EC0(ball);
    fn_17_136C4(ball);
    if (!(player->flags & 8))
    {
        if (rocket != 0)
        {
            if ((lbl_1_bss_D4CDC & 1) || (player->flags & 0x800))
            {
                player->unk60 = 20;
            }
            if ((s8)player->unk60 == 0)
            {
                if (lbl_17_bss_2C[ball->playerId] != 0)
                {
                    call_SoundReqID_arg_0(0x2C5);
                }
            }
            else if (player->unk2E == 0)
            {
                fn_17_128AC(ball);
            }
        }
        if (fire != 0)
        {
            if (player->unk30 < lbl_80198B38 * 2)
            {
                player->unk30++;
                if (player->unk2C == 0)
                {
                    fn_17_12C0C(ball);
                }
                firing = 1;
                if (!(player->unk70 & 1))
                {
                    player->unk70 |= 1;
                    call_SoundReqID_arg_0(sounds[ball->playerId]);
                }
                volume = fn_1_2F8F0(modeCtrl.unk30, &ball->pos, (s8 *)&vol, (s8 *)&pan);
                SoundVolID(sounds[ball->playerId], volume);
                SoundPanID(sounds[ball->playerId], vol, pan);
            }
        }
        else if (player->unk30 > 0)
        {
            player->unk30 -= 10;
            if (player->unk30 < 0)
            {
                player->unk30 = 0;
            }
        }
    }
    if (firing == 0 && (player->unk70 & 1))
    {
        SoundOffID(sounds[ball->playerId]);
        player->unk70 &= ~1;
    }
}

int fn_17_128AC(struct Ball *ball)
{
    Vec dir;
    s8 vol;
    s8 pan;
    s8 span;
    struct DogfightPlayer *player;
    int i;
    struct DogfightShot *shot;

    player = &lbl_17_bss_48[ball->playerId];
    if (lbl_17_bss_12B2C >= 30)
    {
        return 0;
    }
    lbl_17_bss_12B2C++;
    for (i = 0; i < 0x3C; i++)
    {
        shot = &lbl_17_bss_3DC[i];
        if (shot->status == 0)
        {
            player->unk2E = lbl_80198B38 / 3;
            player->unk60--;
            init_shot(shot, i);
            shot->status = 2;
            shot->type = 0;
            shot->radius = 0.05f;
            shot->owner = ball->playerId;
            mtxa_from_mtx(ball->unk30);
            mtxa_tf_point(&lbl_17_data_1400, &shot->pos);
            shot->prevPos = ball->pos;
            shot->trail[0] = shot->pos;
            shot->vel = ball->vel;
            dir.x = -ball->unk30[0][2];
            dir.y = -ball->unk30[1][2];
            dir.z = -ball->unk30[2][2];
            mtxa_from_identity();
            mtxa_rotate_y(math_atan2(-dir.x, -dir.z));
            mtxa_rotate_x(math_atan2(dir.y, math_sqrt(mathutil_sum_of_sq_2(dir.x, dir.z))));
            mtxa_to_mtx(shot->mtx);
            PSVECScale(&dir, &dir, 0.046296295f);
            PSVECAdd(&shot->vel, &dir, &shot->vel);
            if (player->unk1E[0] != -1)
            {
                shot->target = player->unk1E[0];
                if (player->unk64 & (1 << shot->target))
                {
                    shot->flags |= 0x10;
                    shot->speed = 3.0f;
                }
                player->unk22[shot->target] = -1;
            }
            else
            {
                shot->target = -1;
            }
            if (lbl_17_bss_2C[ball->playerId] != 0)
            {
                SoundOffID(0x28F);
                SoundOffID(0x290);
                g_player_id_for_sound = ball->playerId;
                call_SoundReqID_arg_0(0x29A);
            }
            span = fn_1_2F8F0(modeCtrl.unk30, &ball->pos, &vol, &pan);
            call_SoundReqID_arg_0(((pan & 0x7F) << 25) | (((span & 0x7F) << 11) | 0x294 | ((vol & 0x7F) << 18)));
            return 1;
        }
    }
    return 0;
}

int fn_17_12C0C(struct Ball *ball)
{
    Vec dir;
    Vec spread;
    struct DogfightPlayer *player;
    int i;
    struct DogfightShot *shot;
    s16 angle;

    player = &lbl_17_bss_48[ball->playerId];
    if (lbl_17_bss_12B2D >= 30)
    {
        return 0;
    }
    lbl_17_bss_12B2D++;
    for (i = 0; i < 0x3C; i++)
    {
        shot = &lbl_17_bss_3DC[i];
        if (shot->status == 0)
        {
            player->unk2C = lbl_80198B38 / 8;
            init_shot(shot, i);
            shot->status = 2;
            shot->type = 1;
            shot->radius = 0.05f;
            shot->owner = ball->playerId;
            mtxa_from_mtx(ball->unk30);
            mtxa_tf_point(&lbl_17_data_1400, &shot->pos);
            mtxa_sq_to_mtx(shot->mtx);
            shot->prevPos = ball->pos;
            shot->trail[0] = shot->pos;
            shot->vel = ball->vel;
            dir.x = -ball->unk30[0][2];
            dir.y = -ball->unk30[1][2];
            dir.z = -ball->unk30[2][2];
            PSVECScale(&dir, &dir, 5.5555553f);
            PSVECAdd(&shot->vel, &dir, &shot->vel);
            angle = (u16)(rand() & 0x7FFF) * 2;
            {
                Vec v = {0.0f, 0.0f, 0.0f};
                Vec *p;

                v.x = 0.090000004f * ((float)rand() / 32767.0f);
                p = &v;
                dir = *p;
            }
            mtxa_rotate_z(angle);
            mtxa_tf_vec(&dir, &dir);
            PSVECAdd(&shot->vel, &dir, &shot->vel);
            shot->target = -1;
            if (player->flags & 0x2000)
            {
                shot->flags |= 0x20;
            }
            return 1;
        }
    }
    return 0;
}

void fn_17_12EC0(struct Ball *ball)
{
    s16 lockSoundA[4] = {0x28F, 0x3C7, 0x3C8, 0x3C9};
    s16 lockSoundB[4] = {0x290, 0x3CA, 0x3CB, 0x3CC};
    Vec forward;
    Vec best;
    Vec dir;
    Vec screen;
    struct DogfightPlayer *player;
    struct Camera *camera;
    int i;
    int bestId;
    u32 inSight;
    float bestDot;

    player = &lbl_17_bss_48[ball->playerId];
    camera = &cameras[ball->playerId];
    inSight = 0;
    player->flags &= ~0x2000;
    player->unk64 = 0;
    player->unk63 = 0;
    player->unk61 = -1;
    player->unk62 = 0;
    if (lbl_17_bss_14 != 0xD || (player->flags & 8))
    {
        for (i = 0; i < 4; i++)
        {
            player->unk22[i] = 0;
        }
        SoundOffID(lockSoundA[ball->playerId]);
        SoundOffID(lockSoundB[ball->playerId]);
        player->unk70 &= ~0xC;
        return;
    }
    forward.x = -ball->unk30[0][2];
    forward.y = -ball->unk30[1][2];
    forward.z = -ball->unk30[2][2];
    mtxa_from_mtx(camera->unk14C);
    bestId = -1;
    bestDot = 0.0f;
    for (i = 0; i < 4; i++)
    {
        struct Ball *other = &balls[i];
        struct DogfightPlayer *otherPlayer = &lbl_17_bss_48[i];

        if (other->status == 2 && other != ball && !(otherPlayer->flags & 0x4010)
         && otherPlayer->unk32 <= lbl_80198B38 * 3)
        {
            float dot;

            PSVECSubtract(&other->pos, &ball->pos, &dir);
            vec_normalize_len(&dir);
            dot = mathutil_vec_dot_prod(&forward, &dir);
            if (dot > 0.707f && fn_17_16ED8(&other->pos, &screen, camera->sub28.view.tanHalfFov, camera->sub28.view.aspect)
             && screen.x > -1.0f && screen.x < 1.0f && screen.y > -1.0f && screen.y < 1.0f)
            {
                if (dot > bestDot)
                {
                    bestId = i;
                    bestDot = dot;
                    best = screen;
                }
                screen.x *= camera->sub28.view.tanHalfFov;
                screen.y *= camera->sub28.view.tanHalfFov;
                screen.x *= camera->sub28.view.aspect;
                if (screen.x > -0.23f && screen.x < 0.23f && screen.y > -0.18f && screen.y < 0.28f)
                {
                    inSight |= 1 << i;
                }
            }
        }
    }
    player->unk61 = (s8)bestId;
    player->unk62 = inSight;
    if (player->unk61 >= 0)
    {
        if (mathutil_vec_distance(&balls[player->unk61].pos, &ball->pos) < 220.0f)
        {
            player->flags |= 0x2000;
        }
    }
    for (i = 0; i < 4; i++)
    {
        if (i != ball->playerId)
        {
            if (inSight & (1 << i))
            {
                if (player->unk22[i] < 600)
                {
                    player->unk22[i]++;
                }
            }
            else
            {
                player->unk22[i] = 0;
            }
            if (player->unk22[i] >= (int)(0.6f * lbl_80198B34))
            {
                player->unk63 |= 1 << i;
                if (player->unk22[i] >= (int)(2.0f * lbl_80198B34))
                {
                    player->unk64 |= 1 << i;
                }
            }
        }
    }
    for (i = 0; i < 4; i++)
    {
        if (i != ball->playerId && !(player->unk63 & (1 << i)))
        {
            int j;

            for (j = 0; j < 3; j++)
            {
                if (player->unk1E[j] == i)
                {
                    int k;

                    player->unk1E[j] = -1;
                    for (k = j + 1; k < 3; k++)
                    {
                        if (player->unk1E[k] != -1)
                        {
                            player->unk1E[k - 1] = player->unk1E[k];
                        }
                    }
                }
            }
        }
    }
    for (i = 0; i < 4; i++)
    {
        if (i != ball->playerId && (player->unk63 & (1 << i)))
        {
            int j;

            for (j = 0; j < 3; j++)
            {
                if (player->unk1E[j] == i)
                {
                    goto next;
                }
            }
            for (j = 0; j < 3; j++)
            {
                if (player->unk1E[j] == -1)
                {
                    player->unk1E[j] = (s8)i;
                    break;
                }
            }
        }
    next:;
    }
    if (lbl_17_bss_2C[ball->playerId] != 0)
    {
        for (i = 0; i < 4; i++)
        {
            if (player->unk22[i] == (int)(0.6f * lbl_80198B34))
            {
                g_player_id_for_sound = ball->playerId;
                call_SoundReqID_arg_0(0x299);
                break;
            }
        }
        if (player->unk63 != 0)
        {
            SoundOffID(lockSoundA[ball->playerId]);
            player->unk70 &= ~4;
            if (!(player->unk70 & 8))
            {
                player->unk70 |= 8;
                g_player_id_for_sound = ball->playerId;
                call_SoundReqID_arg_0(lockSoundB[ball->playerId]);
            }
        }
        else if (inSight != 0 && player->unk63 == 0)
        {
            SoundOffID(lockSoundB[ball->playerId]);
            player->unk70 &= ~8;
            if (!(player->unk70 & 4))
            {
                player->unk70 |= 4;
                g_player_id_for_sound = ball->playerId;
                call_SoundReqID_arg_0(lockSoundA[ball->playerId]);
            }
        }
        else
        {
            SoundOffID(lockSoundA[ball->playerId]);
            SoundOffID(lockSoundB[ball->playerId]);
            player->unk70 &= ~0xC;
        }
    }
}

void fn_17_136C4(struct Ball *ball)
{
    struct DogfightPlayer *player;
    s8 lastShot;
    u8 mask;
    int i;

    player = &lbl_17_bss_48[ball->playerId];
    lastShot = -1;
    mask = 0;
    for (i = 0; i < 4; i++)
    {
        struct Ball *other = &balls[i];
        struct DogfightPlayer *otherPlayer = &lbl_17_bss_48[i];

        if (other->status == 2 && other != ball && (otherPlayer->unk63 & (1 << ball->playerId)))
        {
            mask |= 1 << i;
        }
    }
    player->unk65 = mask;
    mask = 0;
    for (i = 0; i < 0x3C; i++)
    {
        struct DogfightShot *shot = &lbl_17_bss_3DC[i];

        if (shot->status != 0 && !(shot->flags & 3) && shot->target == ball->playerId)
        {
            mask |= 1 << shot->owner;
            lastShot = shot->index;
        }
    }
    player->unk66 = mask;
    if (player->unk66 != 0)
    {
        player->unk40++;
        if (player->unk40 == lbl_80198B38)
        {
            if (lbl_17_bss_2C[ball->playerId] != 0 && (player->flags & 0x400))
            {
                g_player_id_for_sound = ball->playerId;
                call_SoundReqID_arg_0(0x2C0);
            }
        }
        else if (player->unk40 > lbl_80198B38 && player->unk67 != lastShot)
        {
            player->unk40 = 0;
        }
        if (lbl_17_bss_2C[ball->playerId] != 0 && !(player->unk70 & 2) && (player->flags & 0x400))
        {
            player->unk70 |= 2;
            g_player_id_for_sound = ball->playerId;
            call_SoundReqID_arg_0(lbl_17_rodata_980[ball->playerId]);
        }
    }
    else
    {
        player->unk40 = 0;
        if (player->unk70 & 2)
        {
            SoundOffID(lbl_17_rodata_980[ball->playerId]);
            player->unk70 &= ~2;
        }
    }
    player->unk67 = lastShot;
}

void fn_17_139A4(struct DogfightShot *shot)
{
    Vec up = {0.0f, 1.0f, 0.0f};
    Vec axis;
    Vec localAxis;
    Vec forward;
    Vec dir;
    Vec v;
    Quat q;
    struct Ball *ball;
    struct DogfightPlayer *player;
    int lost;
    float speed;
    float maxSpeed;
    float turn;
    float dist;

    lost = 0;
    forward.x = -shot->mtx[0][2];
    forward.y = -shot->mtx[1][2];
    forward.z = -shot->mtx[2][2];
    if (shot->target >= 0)
    {
        ball = &balls[shot->target];
        player = &lbl_17_bss_48[shot->target];
        dist = mathutil_vec_distance(&ball->pos, &shot->pos);
        maxSpeed = 0.037037037f * dist;
        if (maxSpeed > 4.6296296f)
        {
            maxSpeed = 4.6296296f;
        }
        else if (maxSpeed < 1.6203704f)
        {
            maxSpeed = 1.6203704f;
        }
    }
    else
    {
        maxSpeed = 2.3148148f;
    }
    maxSpeed *= shot->speed;
    speed = mathutil_vec_len(&shot->vel);
    if (speed < maxSpeed)
    {
        PSVECScale(&forward, &v, (0.69444442f * shot->speed) / lbl_80198B34);
        PSVECAdd(&shot->vel, &v, &shot->vel);
    }
    else if (speed > maxSpeed)
    {
        vec_set_len(maxSpeed, &shot->vel, &shot->vel);
    }
    speed = mathutil_vec_len(&shot->vel);
    if (shot->target >= 0)
    {
        if (ball->status != 2 || (player->flags & 0x4000))
        {
            lost = 1;
        }
        if ((float)shot->age < 0.6f * lbl_80198B34)
        {
            turn = (float)shot->age * (1.0f / (0.6f * lbl_80198B34));
        }
        else
        {
            turn = 1.0f;
        }
        if (player->flags & 8)
        {
            turn *= 0.75f;
        }
        turn *= 1.0f - (0.5f * (float)player->unk3C) / (float)(lbl_80198B38 * 2);
        PSVECSubtract(&ball->pos, &shot->pos, &dir);
        vec_normalize_len(&dir);
        if (mathutil_vec_dot_prod(&forward, &dir) < 0.707f)
        {
            if (shot->unk12 == 0 && lbl_17_bss_2C[ball->playerId] != 0 && dist < 10.0f)
            {
                g_player_id_for_sound = ball->playerId;
                call_SoundReqID_arg_0(0x1E2AD);
            }
            if (shot->unk12 > (int)(0.3f * lbl_80198B34))
            {
                lost = 1;
            }
            shot->unk12++;
        }
        else
        {
            shot->unk12 = 0;
        }
        if (lost != 0)
        {
            shot->target = -1;
            if (lbl_17_bss_2C[shot->owner] != 0 && lbl_17_bss_14 == 0xD)
            {
                g_player_id_for_sound = shot->owner;
                call_SoundReqID_arg_0(0x29E);
            }
            return;
        }
        if (shot->unk12 <= 0)
        {
            float t;
            float limit;
            float angle;
            float d;

            PSVECAdd(&ball->pos, &shot->aim, &v);
            PSVECScale(&v, &v, 0.5f);
            PSVECSubtract(&v, &shot->pos, &v);
            d = mathutil_vec_len(&v);
            t = speed;
            if (t < 0.46296296f)
            {
                t = 0.46296296f;
            }
            d /= t;
            d *= 0.95f;
            if (lbl_1_bss_D4CDC & 0x20)
            {
                d *= 0.5f;
            }
            limit = 0.8f * lbl_80198B34;
            if (shot->flags & 0x10)
            {
                limit *= 0.5f;
                d *= 0.5f;
                turn *= 3.3f;
            }
            if (d > limit)
            {
                d = limit;
            }
            PSVECScale(&ball->vel, &v, d);
            PSVECAdd(&ball->pos, &v, &shot->aim);
            PSVECSubtract(&shot->aim, &shot->pos, &dir);
            vec_normalize_len(&dir);
            mathutil_vec_cross_prod(&forward, &dir, &axis);
            angle = vec_normalize_len(&axis);
            if (!(shot->flags & 0x10) && angle > 0.22f)
            {
                angle = 0.22f;
            }
            mtxa_from_mtx(shot->mtx);
            mtxa_rigid_inv_tf_vec(&axis, &localAxis);
            quat_from_axis_angle(&q, &localAxis, 992.0f * (shot->speed * (angle * turn)));
            mtxa_from_quat(&q);
            mtxa_mult_left(shot->mtx);
            mtxa_sq_to_mtx(shot->mtx);
            PSVECScale(&forward, &shot->vel, speed);
        }
    }
}

void fn_17_14030(struct DogfightShot *shot)
{
    struct Effect hit;
    struct Effect spark;
    struct PhysicsBall physBall;
    Vec shake;
    int i;
    struct Ball *ball;
    struct DogfightPlayer *player;
    int damage;
    float dist;

    fn_17_146D8(shot, &physBall);
    collide_ball_with_stage(&physBall, stagedef);
    if (physBall.flags & 1)
    {
        memset(&hit, 0, sizeof(hit));
        hit.type = 0x28;
        hit.pos = shot->prevPos;
        {
            Vec scale = {0.5f, 0.5f, 0.5f};
            Vec *p = &scale;

            hit.scale = *p;
        }
        spawn_effect(&hit);
        shot->flags |= 9;
        return;
    }
    for (i = 0; i < 4; i++)
    {
        ball = &balls[i];
        if (ball->status == 2 && ball->playerId != shot->owner)
        {
            player = &lbl_17_bss_48[i];
            if (!(player->flags & 0x4000))
            {
                dist = mathutil_vec_sq_distance(&ball->pos, &shot->pos);
                if (!(dist > 36.0f)
                 && g_some_item_collision_check(&ball->prevPos, &ball->pos, ball->currRadius, &shot->prevPos, &shot->pos, shot->radius))
                {
                    switch (shot->type)
                    {
                    case 1:
                        if (!(player->flags & 0x10))
                        {
                            damage = 5;
                            if (!(lbl_1_bss_D4CDC & 2))
                            {
                                player->unk5C = player->unk5C - damage;
                            }
                            fn_17_14804(shot->owner, damage * 10, lbl_80198B38 / 2);
                        }
                        {
                            s8 vol;
                            s8 pan;
                            s8 span;

                            span = fn_1_2F8F0(modeCtrl.unk30, &ball->pos, &vol, &pan);
                            call_SoundReqID_arg_0(((pan & 0x7F) << 25) | (((span & 0x7F) << 11) | 0x2AB | ((vol & 0x7F) << 18)));
                        }
                        break;
                    case 0:
                        if (!(player->flags & 0x10))
                        {
                            damage = 25;
                            if (!(lbl_1_bss_D4CDC & 2))
                            {
                                player->unk5C = player->unk5C - damage;
                            }
                            fn_17_14804(shot->owner, damage * 10, lbl_80198B38 / 2);
                            if (shot->flags & 0x10)
                            {
                                fn_17_14784(shot->owner, 250, lbl_80198B38);
                            }
                            lbl_17_bss_48[shot->owner].unk34 = 0;
                            lbl_17_bss_48[shot->owner].unk3A = ball->playerId;
                            player->unk3C = lbl_80198B38 * 2;
                        }
                        {
                            s8 vol;
                            s8 pan;
                            s8 span;

                            span = fn_1_2F8F0(modeCtrl.unk30, &ball->pos, &vol, &pan);
                            call_SoundReqID_arg_0(((pan & 0x7F) << 25) | (((span & 0x7F) << 11) | 0x291 | ((vol & 0x7F) << 18)));
                        }
                        if (lbl_17_bss_2C[i] != 0)
                        {
                            mtxa_from_mtxb();
                            mtxa_tf_vec(&shot->vel, &shake);
                            PSVECScale(&shake, &shake, 0.75f);
                            camera_start_shake(i, lbl_80198B34, &shake);
                        }
                        break;
                    }
                    if (player->unk5C <= 0)
                    {
                        if (!(player->flags & 8))
                        {
                            fn_17_14784(shot->owner, 1000, lbl_80198B38);
                            player->flags |= 8;
                            lbl_17_bss_48[shot->owner].unk38 = 0;
                            lbl_17_bss_48[ball->playerId].unk3B = shot->owner;
                            if (lbl_1_bss_D4D2D == 1)
                            {
                                player->flags |= 0x40000;
                                player->unk6C = lbl_17_bss_34C;
                            }
                        }
                        player->unk4C = 0.15f;
                    }
                    g_player_id_for_sound = ball->playerId;
                    g_smth_with_sound = ball->ape->charaId;
                    switch (shot->type)
                    {
                    case 1:
                        call_SoundReqID_arg_0(0x17);
                        break;
                    case 0:
                        call_SoundReqID_arg_0(0x1A);
                        break;
                    }
                    memset(&spark, 0, sizeof(spark));
                    spark.type = 0x28;
                    spark.playerId = ball->playerId;
                    spark.pos.x = ball->pos.x;
                    spark.pos.y = ball->pos.y;
                    spark.pos.z = ball->pos.z;
                    if (shot->type == 0)
                    {
                        spark.pos.y -= 1.0f;
                        {
                            Vec scale = {0.5f, 0.5f, 0.5f};
                            Vec *p = &scale;

                            spark.scale = *p;
                        }
                        PSVECScale(&ball->vel, &spark.vel, 0.8f);
                    }
                    else
                    {
                        {
                            Vec scale = {0.15f, 0.15f, 0.15f};
                            Vec *p = &scale;

                            spark.scale = *p;
                        }
                        PSVECScale(&ball->vel, &spark.vel, 0.9f);
                    }
                    spawn_effect(&spark);
                    if (shot->type == 0)
                    {
                        g_rumble_controller(player_pad_map[ball->playerId], 1, 30);
                    }
                    else
                    {
                        g_rumble_controller(player_pad_map[ball->playerId], 1, 10);
                    }
                    shot->flags |= 1;
                }
            }
        }
    }
}

void fn_17_146D8(struct DogfightShot *shot, struct PhysicsBall *physBall)
{
    physBall->flags = 0;
    physBall->pos = shot->pos;
    physBall->prevPos = shot->prevPos;
    physBall->vel = shot->vel;
    physBall->radius = shot->radius;
    physBall->gravityAccel = 0.0f;
    physBall->restitution = 0.0f;
    physBall->hardestColiSpeed = 0.0f;
    physBall->itemGroupId = 0;
    physBall->hardestColiItemGroupId = 0;
    physBall->friction = 0.01f;
}

void fn_17_14784(int playerId, int score, int time)
{
    struct DogfightPlayer *player = &lbl_17_bss_48[playerId];

    score = fn_17_148F4(score);
    fn_17_1487C(playerId, score, time);
    player->unk44 += score;
}

void fn_17_14804(int playerId, int score, int time)
{
    struct DogfightPlayer *player = &lbl_17_bss_48[playerId];

    score = fn_17_148F4(score);
    fn_17_1487C(playerId, score, time);
    player->unk44 = score;
}

void fn_17_1487C(int playerId, int score, int time)
{
    struct DogfightPlayer *player = &lbl_17_bss_48[playerId];

    lbl_17_bss_334[playerId] += score;
    player->unk1C = lbl_80198B38;
    if (lbl_17_bss_374[playerId] < time)
    {
        lbl_17_bss_374[playerId] = time;
    }
}

int fn_17_148F4(int score)
{
    if ((lbl_1_bss_D4D30 & 0x20) && lbl_17_bss_348 < lbl_80198B38 * 10)
    {
        score *= 10;
    }
    return score;
}
