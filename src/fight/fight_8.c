#include "fight.h"

/* Monkey Fight item IT_FGT_PUNCH (the glove power-up lying on the ring). */

void fn_12_23070(struct Item *item);
void fn_12_23130(struct Item *item);
void fn_12_2341C(struct Item *item);
void fn_12_235E8(struct Item *item, struct PhysicsBall *ball);
void fn_12_23854(struct Item *item);

struct ItemFuncs lbl_12_data_9E20 = {
    fn_12_23070,
    fn_12_23130,
    fn_12_2341C,
    fn_12_235E8,
    fn_12_23854,
    NULL,
    NULL,
};

/* minigameGma model per item subType */
s16 lbl_12_data_9E3C[] = { 0x95, 0x98, 0x99, 0xAE, 0, 0 };

/* install the punch item funcs */
void fn_12_2301C(void)
{
    item_replace_type_funcs(IT_FGT_PUNCH, &lbl_12_data_9E20);
}

/* restore the default punch item funcs */
void fn_12_23048(void)
{
    item_replace_type_funcs(IT_FGT_PUNCH, NULL);
}

/* init */
void fn_12_23070(struct Item *item)
{
    item->state = 0;
    if (item->unk12 == 0)
        item->unk12 = 480;
    item->modelLODs = minigameGma->modelEntries[lbl_12_data_9E3C[item->subType]].model;
    item->flags = 0x20;
    item->unk14 = ((struct GMAModel *)item->modelLODs)->boundSphereRadius;
    item->unk18 = 0.25f;
    item->shadowModel = init_common_gma->modelEntries[103].model;
    item->shadowColor.r = 0x80;
    item->shadowColor.g = 0x80;
    item->shadowColor.b = 0x80;
    item->unk7C.x = item->unk14;
    item->unk7C.y = item->unk14;
    item->unk7C.z = item->unk14;
}

/* tick: grow in, then drift on the ring and fade out at the end of its lifetime */
void fn_12_23130(struct Item *item)
{
    Vec normal;
    struct RaycastHit hit;
    float height;
    float t;

    item->prevPos = item->pos;
    switch (item->state)
    {
    case 0:
        item->state = 1;
        if (item->unk10 == 0)
            item->unk10 = 39;
        /* fall through */
    case 1:
        item->unk14 += (1.0f - item->unk14) / item->unk10;
        item->unk10--;
        if (item->unk10 <= 0 || item->unk14 >= 1.0f)
        {
            item->unk14 = 1.0f;
            item->state = 2;
        }
        break;
    case 2:
        item->state = 3;
        item->flags |= 2;
        /* fall through */
    case 3:
        item->vel.x *= 0.92f;
        item->vel.y *= 0.98f;
        item->vel.z *= 0.92f;
        item->vel.y += -0.008f;
        if ((s32)raycast_stage_down(&item->pos, &hit, &normal) != 0)
        {
            height = 0.25f + (1.0f + hit.pos.y) - item->pos.y;
            if (item->vel.y < height)
                item->vel.y += 0.4f * (height - item->vel.y);
        }
        break;
    case 4:
    case 5:
        break;
    }
    item->pos.x += item->vel.x;
    item->pos.y += item->vel.y;
    item->pos.z += item->vel.z;
    item->rotY += 0x200;
    if (item->unk12 < 30)
    {
        t = (float)item->unk12 / (float)(item->unk12 + 1);
        item->shadowColor.r = item->shadowColor.r * t;
        item->shadowColor.g = item->shadowColor.g * t;
        item->shadowColor.b = item->shadowColor.b * t;
    }
}

/* disp: bobbing, spinning glove; blinks in its last second and glows while growing in */
void fn_12_2341C(struct Item *item)
{
    float scale;
    struct GMAModel *model;
    int timer;
    float r, g, b;
    float unusedTimer;  /* dead int->float conversion kept by the original */

    timer = item->unk12;
    if (timer < 60 && (timer & 4))
        return;
    scale = item->unk14;
    model = item->modelLODs;
    unusedTimer = timer;
    mtxa_from_mtxb_translate_xyz(item->pos.x, item->pos.y + 0.2f * (1.0f + math_sin(timer << 9)), item->pos.z);
    mtxa_sq_from_identity();
    mtxa_rotate_z(196608.0f * scale + 2048.0f * math_sin((item->id << 9) - (timer << 10)));
    mtxa_scale_s(item->unk14 / model->boundSphereRadius);
    GXLoadPosMtxImm(mtxa->mtxA, 0);
    GXLoadNrmMtxImm(mtxa->mtxA, 0);
    if (scale < 1.0f)
    {
        r = 3.0f * (1.0f - scale);
        g = 4.0f * (1.0f - scale);
        b = 5.0f * (1.0f - scale);
        if (r > 1.0f)
            r = 1.0f;
        if (g > 1.0f)
            g = 1.0f;
        if (b > 1.0f)
            b = 1.0f;
        g_stores_doubles2(r, g, b, 0.0f);
    }
    avdisp_draw_model_culled_sort_auto(model);
    if (scale < 1.0f)
        g_stores_doubles2(0.0f, 0.0f, 0.0f, 0.0f);
}

/* coli: picked up by current_ball */
void fn_12_235E8(struct Item *item, struct PhysicsBall *ball)
{
    struct Effect effect;
    GXColor color;
    float scale;
    int sound;
    int voice;

    g_poolInfo.itemPool.statusList[item->id] = 3;
    item->flags |= 1;
    item->flags &= ~2;
    fn_12_16880(current_ball, item->subType);
    memset(&effect, 0, sizeof(effect));
    scale = item->unk14;
    effect.type = 0x2B;
    effect.playerId = current_ball->playerId;
    effect.model = item->modelLODs;
    effect.pos = item->pos;
    effect.rotZ = 196608.0f * scale + 2048.0f * math_sin((item->id << 9) - (item->unk12 << 10));
    effect.scale.x = item->unk14 / effect.model->boundSphereRadius;
    color = lbl_12_data_A218[current_ball->colorId];
    effect.unk18 = 0.00390625f * (128.0f + color.r);
    effect.unk1C = 0.00390625f * (128.0f + color.g);
    effect.unk20 = 0.00390625f * (128.0f + color.b);
    spawn_effect(&effect);
    switch (item->subType)
    {
    case 2:
        voice = 0xD839;
        sound = 0x20;
        break;
    default:
        voice = 0xD803;
        sound = 0x1F;
        break;
    }
    g_smth_with_sound = current_ball->ape->charaId;
    g_player_id_for_sound = current_ball->playerId;
    call_SoundReqID_arg_0(voice);
    call_SoundReqID_arg_0(sound);
}

/* dest */
void fn_12_23854(struct Item *item) {}
