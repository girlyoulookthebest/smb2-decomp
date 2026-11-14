#include "boat.h"
#include "macros.h"
#include "stage.h"
#include "mathutil.h"
#include "camera.h"

void avdisp_draw_model_unculled_sort_never(struct GMAModel *model);
void fn_1_4E12C(Mtx m, u8 a);

struct BoatEffectExtra lbl_14_bss_4350[0x200];

struct EffectFuncs lbl_14_data_1A90[6] = {
    {fn_14_1895C, fn_14_18990, fn_14_189B4, fn_14_18A20},
    {fn_14_18A24, fn_14_18A94, fn_14_18B50, fn_14_18BFC},
    {fn_14_18C00, fn_14_18C90, fn_14_18D60, fn_14_18E28},
    {fn_14_18E2C, fn_14_18E48, fn_14_18FB0, fn_14_19050},
    {fn_14_19054, fn_14_190C8, fn_14_191A0, fn_14_1928C},
    {fn_14_19290, fn_14_1930C, fn_14_19520, fn_14_19614},
};

void fn_14_1864C(void)
{
    s16 i;

    for (i = 0; i < 6; i++) {
        overwrite_effect_funcs(i + 0x27, &lbl_14_data_1A90[i]);
    }
}

void fn_14_186A8(void)
{
    s16 i;

    for (i = 0; i < 6; i++) {
        overwrite_effect_funcs(i + 0x27, NULL);
    }
}

void fn_14_186F4(struct Effect *effect, struct BoatEffectExtra *extra)
{
    struct Effect *spawned;
    s16 type;

    type = effect->type;
    effect->type += 0x27;
    spawned = fn_1_6876C(spawn_effect(effect));
    effect->type = type;
    if (spawned->poolIndex >= 0 && spawned->poolIndex < 0x200 && extra != NULL) {
        lbl_14_bss_4350[spawned->poolIndex] = *extra;
    }
}

void fn_14_187A4(void)
{
    s16 i;
    struct Effect *e;
    s8 *status;

    e = effects;
    status = g_poolInfo.effectPool.statusList;
    i = g_poolInfo.effectPool.count;
    while (i > 0) {
        if (*status != 0) {
            if ((u32)(e->flags & 1) != 0) {
                e->flags |= 0x20000000;
            } else {
                e->flags &= 0xDFFFFFFF;
                if ((u32)(e->flags & 0x10000000) != 0) {
                    e->flags |= 1;
                }
            }
        }
        i--;
        status++;
        e++;
    }
}

void fn_14_18844(void)
{
    s16 i;
    struct Effect *e;
    s8 *status;

    e = effects;
    status = g_poolInfo.effectPool.statusList;
    i = g_poolInfo.effectPool.count;
    while (i > 0) {
        if (*status != 0 && (u32)(e->flags & 0x20000000) == 0) {
            if ((u32)(e->flags & 0x10000000) != 0) {
                e->flags &= ~1;
            } else {
                e->flags |= 1;
            }
        }
        i--;
        status++;
        e++;
    }
}

void fn_14_188D8(void)
{
    s16 i;
    struct Effect *e;
    s8 *status;

    e = effects;
    status = g_poolInfo.effectPool.statusList;
    i = g_poolInfo.effectPool.count;
    while (i > 0) {
        if (*status != 0) {
            if ((u32)(e->flags & 0x20000000) != 0) {
                e->flags |= 1;
            } else {
                e->flags &= ~1;
            }
        }
        i--;
        status++;
        e++;
    }
}

void fn_14_1895C(struct Effect *effect)
{
    effect->timer = 0x27;
    fn_14_12B24(0x26D, &effect->pos);
}

void fn_14_18990(struct Effect *effect)
{
    effect->model = lbl_14_bss_B4[0x4C - effect->timer];
}

void fn_14_189B4(struct Effect *effect)
{
    mtxa_from_mtxb_translate(&effect->pos);
    mtxa_sq_from_identity();
    mtxa_translate_xyz(0.0f, -0.5f, 1.0f);
    mtxa_scale_xyz(1.0f, 5.5f, 1.0f);
    avdisp_draw_model_unculled_sort_always(effect->model);
}

void fn_14_18A20(struct Effect *effect)
{
}

void fn_14_18A24(struct Effect *effect)
{
    struct BoatPlayer *player;

    player = &lbl_14_bss_208[effect->playerId];
    effect->flags |= 0x10000000;
    effect->timer = player->unk4BC[3];
    effect->unk52 = 32768.0 / (0.5 * lbl_80198B34);
}

void fn_14_18A94(struct Effect *effect)
{
    s32 rot;
    struct BoatPlayer *player;

    player = &lbl_14_bss_208[effect->playerId];
    rot = effect->rotX;
    effect->timer = player->unk4BC[3];
    if (effect->timer <= 0.5 * lbl_80198B34) {
        rot -= effect->unk52;
        if (rot < 0) {
            effect->rotX = 0;
        }
    } else {
        rot += effect->unk52;
        if (rot >= 0x8000) {
            rot = 0x7FFF;
        }
    }
    effect->rotX = (s16)rot;
}

void fn_14_18B50(struct Effect *effect)
{
    Mtx m;
    struct Ball *ball;
    struct BoatPlayer *player;

    ball = &balls[effect->playerId];
    player = &lbl_14_bss_208[effect->playerId];
    mtxa_from_mtxb_translate(&ball->pos);
    mtxa_rotate_y(player->unk3E2);
    mtxa_rotate_x(player->unk3E0);
    mtxa_rotate_z(player->unk3E4);
    mtxa_rotate_x(effect->rotX - 0x4000);
    mtxa_scale_s(player->unk3B0);
    mtxa_to_mtx(m);
    fn_1_4E12C(m, ball->unk179);
}

void fn_14_18BFC(struct Effect *effect)
{
}

void fn_14_18C00(struct Effect *effect)
{
    if (effect->timer == 0) {
        effect->timer = 0.3f * lbl_80198B34;
    }
    effect->model = lbl_14_bss_B4[0x17];
    effect->scale = (Vec){1.0f, 1.0f, 1.0f};
}

void fn_14_18C90(struct Effect *effect)
{
    float t;

    t = effect->timer / (0.3f * lbl_80198B34);
    effect->unk80.x = math_sin(16384.0f + 16384.0f * t);
    effect->unk80.y = math_sin(32768.0f * t);
    effect->scale.y = effect->colorFactor * effect->unk80.x;
}

void fn_14_18D60(struct Effect *effect)
{
    mtxa_from_mtxb_translate(&effect->pos);
    mtxa_rotate_y(g_current_camera->rotY);
    mtxa_scale(&effect->scale);
    load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
    set_post_mult_color(1.0f, 1.0f, 1.0f, effect->unk80.y);
    avdisp_draw_model_unculled_sort_always(effect->model);
    set_post_mult_color(1.0f, 1.0f, 1.0f, 1.0f);
}

void fn_14_18E28(struct Effect *effect)
{
}

void fn_14_18E2C(struct Effect *effect)
{
    effect->model = lbl_14_bss_B4[0x1C];
    effect->timer = 2;
}

void fn_14_18E48(struct Effect *effect)
{
    struct Ball *ball;
    struct BoatPlayer *player;
    u32 item;
    s16 i;

    ball = &balls[effect->playerId];
    player = &lbl_14_bss_208[effect->playerId];
    if ((u32)(player->unk8 & 0x1000) != 0 && player->unk498[0] != 0) {
        for (i = 2; i >= 0; i--) {
            if (player->unk498[i] != 0) {
                item = player->unk498[i];
                break;
            }
        }
        for (i = 0; i < 5; i++) {
            if (item == (u32)(1 << i)) {
                effect->model = lbl_14_bss_B4[i + 0x1C];
                break;
            }
        }
        effect->timer = lbl_80198B38;
        player->unk8 &= ~0x1000;
    }
    if (effect->timer <= 2) {
        effect->timer = 2;
    }
    if (effect->timer != 2) {
        effect->pos = ball->pos;
        effect->pos.y += 0.25 + ball->currRadius;
        if (effect->pos.y < player->unk478) {
            effect->pos.y = player->unk478;
        }
    }
}

void fn_14_18FB0(struct Effect *effect)
{
    if ((g_current_camera->flags & 0x40) == 0 && effect->playerId == g_current_camera->idx
        && effect->timer > 2 && (effect->timer & 8) == 0) {
        mtxa_from_mtxb_translate(&effect->pos);
        mtxa_sq_from_identity();
        mtxa_scale_s(0.7f);
        avdisp_draw_model_unculled_sort_always(effect->model);
    }
}

void fn_14_19050(struct Effect *effect)
{
}

void fn_14_19054(struct Effect *effect)
{
    struct BoatPlayer *player;
    struct BoatEffectExtra *extra;

    player = &lbl_14_bss_208[effect->playerId];
    extra = &lbl_14_bss_4350[effect->poolIndex];
    effect->flags |= 0x10000000;
    effect->timer = player->unk4BC[3];
    effect->colorFactor = 0.0f;
    extra->unk8 = 1.0 / (0.5 * lbl_80198B34);
}

void fn_14_190C8(struct Effect *effect)
{
    struct BoatPlayer *player;
    struct BoatEffectExtra *extra;

    player = &lbl_14_bss_208[effect->playerId];
    extra = &lbl_14_bss_4350[effect->poolIndex];
    effect->timer = player->unk4BC[3];
    if (effect->timer <= 0.5 * lbl_80198B34) {
        effect->colorFactor -= extra->unk8;
        if (effect->colorFactor < 0.0) {
            effect->colorFactor = 0.0f;
        }
    } else {
        effect->colorFactor += extra->unk8;
        if (effect->colorFactor > 1.0) {
            effect->colorFactor = 1.0f;
        }
    }
}

void fn_14_191A0(struct Effect *effect)
{
    Vec v;
    struct Ball *ball;
    struct BoatPlayer *player;

    ball = &balls[effect->playerId];
    player = &lbl_14_bss_208[effect->playerId];
    PSVECScale(&effect->unk8C, &v, 1.0 - effect->colorFactor);
    mtxa_from_rotate_y(player->unk3E2);
    mtxa_tf_vec(&v, &v);
    mtxa_from_mtxb_translate(&ball->pos);
    mtxa_translate(&v);
    mtxa_rotate_y(player->unk3E2);
    mtxa_rotate_x(player->unk3E0);
    mtxa_rotate_z(player->unk3E4);
    mtxa_translate(&effect->pos);
    mtxa_scale_s(effect->colorFactor);
    load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
    avdisp_draw_model_unculled_sort_never(effect->model);
}

void fn_14_1928C(struct Effect *effect)
{
}

void fn_14_19290(struct Effect *effect)
{
    struct BoatPlayer *player;
    struct BoatEffectExtra *extra;

    player = &lbl_14_bss_208[effect->playerId];
    extra = &lbl_14_bss_4350[effect->poolIndex];
    effect->flags |= 0x10000000;
    effect->timer = player->unk4BC[3];
    effect->colorFactor = 0.0f;
    effect->unk56 = 0;
    extra->unk8 = 1.0 / (0.5 * lbl_80198B34);
}

void fn_14_1930C(struct Effect *effect)
{
    double dir;
    struct Ball *ball;
    struct BoatPlayer *player;
    struct BoatEffectExtra *extra;

    ball = &balls[effect->playerId];
    player = &lbl_14_bss_208[effect->playerId];
    extra = &lbl_14_bss_4350[effect->poolIndex];
    effect->timer = player->unk4BC[3];
    if (effect->timer <= 0.5 * lbl_80198B34) {
        effect->colorFactor -= extra->unk8;
        if (effect->colorFactor < 0.0) {
            effect->colorFactor = 0.0f;
        }
    } else {
        effect->colorFactor += extra->unk8;
        if (effect->colorFactor > 1.0) {
            effect->colorFactor = 1.0f;
        }
    }
    if ((ball->flags & 0x80000000000LL) != 0 || (effect->pos.x < 0.0f && player->unk3C == 1)
        || (effect->pos.x > 0.0f && player->unk3C == 2)
        || ((player->unk3C == 1 || player->unk3C == 2) && player->unk4C > 40.0)) {
        if (effect->unk8C.x < 0.0f) {
            dir = 1.0;
        } else {
            dir = -1.0;
        }
        effect->unk56 = 4096.0 * (player->unk44 * dir);
    } else if (effect->unk56 != 0) {
        effect->unk56 = 0.97 * effect->unk56;
    }
    effect->rotZ += effect->unk56;
}

void fn_14_19520(struct Effect *effect)
{
    Vec v;
    struct Ball *ball;
    struct BoatPlayer *player;

    ball = &balls[effect->playerId];
    player = &lbl_14_bss_208[effect->playerId];
    PSVECScale(&effect->unk8C, &v, 1.0 - effect->colorFactor);
    mtxa_from_rotate_y(player->unk3E2);
    mtxa_tf_vec(&v, &v);
    mtxa_from_mtxb_translate(&ball->pos);
    mtxa_translate(&v);
    mtxa_rotate_y(player->unk3E2);
    mtxa_rotate_x(player->unk3E0);
    mtxa_rotate_z(player->unk3E4);
    mtxa_translate(&effect->pos);
    mtxa_rotate_z(effect->rotZ);
    mtxa_scale_s(effect->colorFactor);
    load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
    avdisp_draw_model_unculled_sort_never(effect->model);
}

void fn_14_19614(struct Effect *effect)
{
}

