#include "background.h"
#include "boat.h"
#include "macros.h"
#include "avdisp.h"
#include "effect.h"
#include "item.h"
#include "stage.h"

extern float lbl_1_data_6580C;
extern float lbl_1_data_65810;

s16 g_math_unk3(float x);
void ray_scale(Vec *a, Vec *b, Vec *dst, float t);

float lbl_14_bss_2B48;
struct BoatItemInfo lbl_14_bss_2B4C[0x100];
s32 lbl_14_bss_434C;

struct ItemFuncs lbl_14_data_19D0[5] = {
    {fn_14_16ABC, fn_14_16B90, fn_14_15FEC, fn_14_171E0, fn_14_16090, (void *) fn_14_1561C, (void *) fn_14_15620},
    {fn_14_172D0, fn_14_173B4, fn_14_15FEC, fn_14_17A10, fn_14_16090, (void *) fn_14_1561C, (void *) fn_14_15620},
    {fn_14_17A90, fn_14_17B64, fn_14_15FEC, fn_14_18160, fn_14_16090, (void *) fn_14_1561C, (void *) fn_14_15620},
    {(void *) fn_14_15610, (void *) fn_14_15614, fn_14_15FEC, (void *) fn_14_15618, fn_14_16090, (void *) fn_14_1561C, (void *) fn_14_15620},
    {fn_14_160B4, fn_14_16168, fn_14_15FEC, fn_14_16810, fn_14_16090, (void *) fn_14_1561C, (void *) fn_14_15620},
};
u32 lbl_14_data_1A5C[3] = {1, 2, 4};
u32 lbl_14_data_1A68[6] = {1, 8, 2, 8, 4, 8};
u8 lbl_14_data_1A80[0x10] = {3, 2, 1, 0, 3, 2, 1, 3, 2, 3, 3};


void fn_14_15624(void)
{
    s16 i;

    for (i = 0; i < 5; i++) {
        item_replace_type_funcs(i + 7, &lbl_14_data_19D0[i]);
    }
    lbl_14_bss_2B48 = 9.8 / (lbl_80198B34 * lbl_80198B34);
    fn_14_156C0();
}

void fn_14_156C0(void)
{
    memset(lbl_14_bss_2B4C, 0, sizeof(lbl_14_bss_2B4C));
    g_item_tick_callback = fn_14_15720;
    lbl_14_bss_434C = lbl_80198B38 * 0xF;
}

void fn_14_15720(void)
{
    s16 i;

    fn_14_15CA0();
    for (i = 0; i < (s32) g_poolInfo.itemPool.count; i++) {
        if (g_poolInfo.itemPool.statusList[i] != 0) {
            struct Item *item = &items[i];
            struct BoatItemInfo *info;

            item->prevPos = item->pos;
            item->prevRotX = item->rotX;
            item->prevRotY = item->rotY;
            item->prevRotZ = item->rotZ;
            info = &lbl_14_bss_2B4C[i];
            if ((info->flags & 1) && info->unk2 > 0) {
                info->unk2--;
            }
        }
    }
    if (lbl_14_bss_434C > 0) {
        lbl_14_bss_434C--;
    }
}

void fn_14_15834(void)
{
    s16 i;

    for (i = 0; i < 5; i++) {
        item_replace_type_funcs(i + 7, NULL);
    }
}

void fn_14_15880(struct Item *item, struct BoatItemInfo *info)
{
    struct Item spawn;
    s16 i;

    for (i = 0; i < 0x100; i++) {
        if ((lbl_14_bss_2B4C[i].flags & 1) == 0) {
            spawn = *item;
            lbl_14_bss_2B4C[i] = *info;
            lbl_14_bss_2B4C[i].flags |= 1;
            spawn.type += 7;
            spawn.stageBanana = (struct StageBanana *) &lbl_14_bss_2B4C[i];
            spawn.prevPos = spawn.pos;
            spawn.prevRotX = spawn.rotX;
            spawn.prevRotY = spawn.rotY;
            spawn.prevRotZ = spawn.rotZ;
            spawn_item(&spawn);
            break;
        }
    }
}

void fn_14_159CC(void)
{
    struct Item item;
    struct BoatItemInfo info;
    u8 id;
    struct BoatPlayer *pl;

    if (lbl_14_bss_434C <= 0) {
        id = lbl_14_data_1A80[fn_14_12904() % 11u];
        if (id < lbl_14_bss_24) {
            pl = lbl_14_bss_2988[id];
            if ((pl->unk8 & 8) == 0) {
                memset(&item, 0, sizeof(item));
                memset(&info, 0, sizeof(info));
                item.type = 4;
                info.unk10 = pl->unk2DC + (13.0 * lbl_14_bss_2AE8->unkC);
                fn_14_11524(lbl_14_bss_70, info.unk10, &item.pos);
                item.pos.x += (2.0 * fn_14_12944()) - 1.0;
                item.pos.z += (2.0 * fn_14_12944()) - 1.0;
                item.pos.y -= 0.5;
                info.unk14 = pl->unk0;
                fn_14_15880(&item, &info);
                lbl_14_bss_434C = lbl_80198B38 * 3;
            }
        }
    }
}

void fn_14_15CA0(void)
{
    Vec mid;
    Vec na;
    Vec nb;
    Vec va;
    Vec vb;
    Vec tmp;
    struct Item *a;
    struct Item *b;
    s16 i;
    s16 j;
    float d;

    for (i = 0; i < g_poolInfo.itemPool.count - 1; i++) {
        if (g_poolInfo.itemPool.statusList[i] != 0) {
            a = &items[i];
            if (a->type == 0xB) {
                for (j = i + 1; j < g_poolInfo.itemPool.count; j++) {
                    if (g_poolInfo.itemPool.statusList[j] != 0) {
                        b = &items[j];
                        if (b->type == 0xB) {
                            if (mathutil_vec_sq_distance(&a->pos, &b->pos) > (a->unk14 + b->unk14) * (a->unk14 + b->unk14)) {
                                continue;
                            }
                            ray_scale(&a->pos, &b->pos, &mid, 0.5f);
                            PSVECSubtract(&a->pos, &mid, &na);
                            vec_normalize_len(&na);
                            PSVECScale(&na, &tmp, a->unk14);
                            PSVECAdd(&mid, &tmp, &a->pos);
                            d = mathutil_vec_dot_prod(&na, &a->vel);
                            if (d < 0.0f) {
                                PSVECScale(&na, &tmp, -d);
                                PSVECAdd(&a->vel, &tmp, &a->vel);
                                PSVECScale(&tmp, &va, -0.3f);
                            } else {
                                va = (Vec){0.0f, 0.0f, 0.0f};
                            }
                            PSVECSubtract(&b->pos, &mid, &nb);
                            vec_normalize_len(&nb);
                            PSVECScale(&nb, &tmp, b->unk14);
                            PSVECAdd(&mid, &tmp, &b->pos);
                            d = mathutil_vec_dot_prod(&nb, &b->vel);
                            if (d < 0.0f) {
                                PSVECScale(&nb, &tmp, -d);
                                PSVECAdd(&b->vel, &tmp, &b->vel);
                                PSVECScale(&tmp, &vb, -0.3f);
                            } else {
                                vb = (Vec){0.0f, 0.0f, 0.0f};
                            }
                            PSVECAdd(&a->vel, &vb, &a->vel);
                            PSVECAdd(&b->vel, &va, &b->vel);
                        }
                    }
                }
            }
        }
    }
}

void fn_14_15FEC(struct Item *item)
{
    struct BoatItemInfo *info = (struct BoatItemInfo *) item->stageBanana;
    float scale;

    if ((info->flags & 2) == 0) {
        scale = item->unk14 / info->unkC;
        mtxa_from_mtxb_translate(&item->pos);
        mtxa_rotate_y(item->rotY);
        mtxa_rotate_x(item->rotX);
        mtxa_rotate_z(item->rotZ);
        mtxa_scale_s(scale);
        avdisp_set_scale_factor(scale);
        load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
        avdisp_draw_model_unculled_sort_auto(item->modelLODs);
    }
}

void fn_14_16090(struct Item *item)
{
    if (item->stageBanana != NULL) {
        struct BoatItemInfo *info = (struct BoatItemInfo *) item->stageBanana;

        info->flags = 0;
        item->stageBanana = NULL;
    }
}

void boat_item_to_phys(struct Item *item, struct PhysicsBall *phys) {
    phys->flags = 0;
    phys->pos = item->pos;
    phys->prevPos = item->prevPos;
    phys->vel = item->vel;
    phys->radius = 0.5f;
    phys->gravityAccel = 3.6 * lbl_14_bss_2B48;
    phys->restitution = 0.01f;
    phys->hardestColiSpeed = 0.0f;
    phys->itemGroupId = 0;
    phys->hardestColiItemGroupId = 0;
    phys->friction = 0.01f;
}

void boat_phys_to_item(struct PhysicsBall *phys, struct Item *item) {
    item->pos = phys->pos;
    item->vel = phys->vel;
}

void fn_14_160B4(struct Item *item)
{
    struct BoatItemInfo *info = (struct BoatItemInfo *) item->stageBanana;

    item->flags |= 2;
    item->modelLODs = lbl_14_bss_B4[1];
    item->unk14 = 0.0f;
    item->unk12 = lbl_80198B38 * 0xA;
    item->rotVelY = 0x300;
    info->unk4 = 0.6f;
    info->unk8 = info->unk4 / (0.5 * lbl_80198B34);
    info->unkC = ((struct GMAModel *) item->modelLODs)->boundSphereRadius;
    fn_14_12B24(0x261, &item->pos);
}

void fn_14_16168(struct Item *item)
{
    struct PhysicsBall phys;
    Vec pos;
    Vec dir;
    struct BoatItemInfo *info = (struct BoatItemInfo *) item->stageBanana;
    struct Ball *ball;
    float t;

    switch (item->state) {
    case 0:
        item->unk14 += info->unk8;
        if (item->unk14 >= info->unk4) {
            item->unk14 = info->unk4;
            item->state = 1;
        }
        /* fallthrough */
    case 1:
        if (item->unk12 >= lbl_80198B38) {
            s16 i;
            u8 nearest = 0;
            float best = 10000.0f;
            float scale;

            i = 0;
            ball = balls;
            for (; i < lbl_14_bss_24; i++) {
                float d = mathutil_vec_sq_distance(&ball->pos, &item->pos);

                if (d < best) {
                    nearest = i;
                    best = d;
                }
            }
            if (best < 9.0) {
                ball = &balls[nearest];
                scale = 1.0 + (60.0 / lbl_80198B34);
            } else {
                ball = &balls[info->unk14];
                scale = 3.0 / lbl_80198B34;
            }
            PSVECSubtract(&ball->pos, &item->pos, &pos);
            pos.y = 0.0f;
            vec_normalize_len(&pos);
            t = mathutil_vec_dot_prod(&pos, &item->vel);
            if (t < 0.0) {
                PSVECScale(&pos, &dir, -t * scale);
                PSVECAdd(&item->vel, &dir, &item->vel);
            }
            info->unk10 += (6.0 / lbl_80198B34) * lbl_14_bss_2AE8->unkC;
            if (info->unk10 >= 100.0) {
                info->unk10 -= 100.0;
            }
            fn_14_11524(lbl_14_bss_70, info->unk10, &pos);
            PSVECSubtract(&pos, &item->pos, &pos);
            vec_set_len(0.2 * (3.6 * lbl_14_bss_2B48), &pos, &pos);
            item->vel.x += pos.x;
            item->vel.y -= 3.6 * lbl_14_bss_2B48;
            item->vel.z += pos.z;
        } else {
            item->vel.y -= 1.4 * (3.6 * lbl_14_bss_2B48);
            if (item->unk12 & 8) {
                info->flags |= 2;
            } else {
                info->flags &= ~2;
            }
        }
        break;
    case 2:
        ball = &balls[info->unk15];
        item->vel.x = item->vel.z = 0.0f;
        item->pos.y = 1.0 + ball->pos.y;
        item->rotVelY *= 2;
        item->unk12 = lbl_80198B38;
        item->state = 3;
        /* fallthrough */
    case 3:
        ball = &balls[info->unk15];
        item->pos.x = ball->pos.x;
        item->pos.z = ball->pos.z;
        item->vel.y += 0.5 * (3.6 * lbl_14_bss_2B48);
        item->unk14 -= info->unk8;
        if (item->unk14 < 0.0) {
            item->unk14 = 0.0f;
        }
        if (item->unk12 <= 0) {
            item->state = 6;
        }
        break;
    case 6:
        g_poolInfo.itemPool.statusList[item->id] = 3;
        break;
    }
    pos = item->pos;
    pos.y -= item->unk14;
    t = fn_14_13C6C(&pos);
    if (t >= 0.0) {
        if (t < item->unk14) {
            float frac = ((item->unk14 * item->unk14) - ((item->unk14 - t) * (item->unk14 - t))) / (item->unk14 * item->unk14);
            item->vel.y += 0.011000000000000001 * frac;
        } else {
            item->vel.y += 0.011000000000000001;
        }
        PSVECScale(&item->vel, &item->vel, lbl_1_data_65810);
    } else {
        PSVECScale(&item->vel, &item->vel, lbl_1_data_6580C);
    }
    item->rotY += item->rotVelY;
    PSVECAdd(&item->pos, &item->vel, &item->pos);
    boat_item_to_phys(item, &phys);
    phys.radius = item->unk14;
    fn_14_22254(&phys, stagedef);
    boat_phys_to_item(&phys, item);
}

void fn_14_16810(struct Item *item, struct PhysicsBall *unused)
{
    struct BoatItemInfo *info = (struct BoatItemInfo *) item->stageBanana;
    struct BoatPlayer *pl = &lbl_14_bss_208[current_ball->playerId];
    s32 big = 0;
    s16 slot;
    s16 j;

    if (item->state == 0) {
        item->unkC = 0;
        return;
    }
    item->flags &= ~2;
    if (pl->unk8 & 0x28) {
        item->unk12 = lbl_80198B38;
        return;
    }
    info->unk15 = pl->unk0;
    if (pl->unk498[2] != 0) {
        slot = 2;
        for (j = 1; j < 3; j++) {
            pl->unk498[j - 1] = pl->unk498[j];
        }
    } else {
        for (slot = 0; slot < 3; slot++) {
            if (pl->unk498[slot] == 0) {
                break;
            }
        }
    }
    if (pl->unk2 >= 2) {
        float diff = lbl_14_bss_2988[0]->unk304 - pl->unk304;
        float chance = 0.02 * (diff - 10.0);

        if (fn_14_12944() <= chance) {
            big = 1;
        }
    }
    if (big != 0) {
        pl->unk498[slot] = lbl_14_data_1A68[fn_14_12904() % 6u];
    } else {
        pl->unk498[slot] = lbl_14_data_1A5C[fn_14_12904() % 3u];
    }
    if ((current_ball->flags & 0x80000000000LL) == 0) {
        pl->unk8 |= 0x1000;
        fn_14_12B24(0x39, &item->pos);
        fn_14_12B24(0x20, &item->pos);
    }
    item->state = 2;
}

void fn_14_16ABC(struct Item *item)
{
    struct BoatItemInfo *info = (struct BoatItemInfo *) item->stageBanana;

    item->flags |= 2;
    item->modelLODs = lbl_14_bss_B4[2];
    item->unk14 = 0.0f;
    item->unk12 = lbl_80198B38;
    item->vel.y = 0.0f;
    info->unk4 = 1.0f;
    info->unk8 = info->unk4 / (0.1 * lbl_80198B34);
    info->unkC = ((struct GMAModel *) item->modelLODs)->boundSphereRadius;
    info->unk2 = 0.2 * lbl_80198B34;
    fn_14_12B24(0x264, &item->pos);
}

void fn_14_16B90(struct Item *item)
{
    struct Effect effect;
    struct PhysicsBall phys;
    Vec pos;
    Vec toBall;
    Vec vel;
    Vec axis;
    Vec prev;
    struct BoatItemInfo *info = (struct BoatItemInfo *) item->stageBanana;
    struct Ball *ball;
    float height;
    s16 angle;

    pos = item->pos;
    pos.y -= item->unk14;
    height = fn_14_13C6C(&pos);
    switch (item->state) {
    case 0:
        item->unk14 += info->unk8;
        if (item->unk14 >= info->unk4) {
            item->unk14 = info->unk4;
            item->state = 1;
        }
        /* fallthrough */
    case 1:
        if (item->unk12 == 2) {
            item->state = 4;
            return;
        }
        break;
    case 4:
        if ((item->flags & 2) == 0 && height > -0.5f) {
            fn_14_1853C(item, height);
        } else {
            memset(&effect, 0, sizeof(effect));
            effect.type = 0;
            effect.pos = item->pos;
            fn_14_186F4(&effect, NULL);
        }
        item->unk12 = 0.1 * lbl_80198B34;
        PSVECScale(&item->vel, &item->vel, 0.01f);
        item->state = 5;
        /* fallthrough */
    case 5:
        if ((item->flags & 2) == 0 && height > -0.5f) {
            fn_14_183EC(item, height);
        }
        item->unk14 -= info->unk4 / (0.1 * lbl_80198B34);
        if (item->unk14 < 0.0f || item->unk12 == 1) {
            item->unk14 = 0.0f;
            item->state = 6;
        }
        break;
    case 6:
        g_poolInfo.itemPool.statusList[item->id] = 3;
        return;
    }
    if (item->unk12 < lbl_80198B38 - 7) {
        fn_14_18250(item, height);
    }
    if (height >= 0.0) {
        PSVECScale(&item->vel, &item->vel, lbl_1_data_65810);
        item->vel.y += 0.05 * (3.6 * lbl_14_bss_2B48);
    } else {
        PSVECScale(&item->vel, &item->vel, lbl_1_data_6580C);
        item->vel.y -= 3.6 * lbl_14_bss_2B48;
    }
    if (info->unk15 >= 0 && info->unk2 == 0 && (lbl_14_bss_208[info->unk15].unk8 & 8) == 0) {
        ball = &balls[info->unk15];
        PSVECSubtract(&ball->pos, &item->pos, &toBall);
        vec_normalize_len(&toBall);
        vel = item->vel;
        vec_normalize_len(&vel);
        mathutil_vec_cross_prod(&vel, &toBall, &axis);
        angle = g_math_unk3(vec_normalize_len(&axis));
        angle = 0.03 * angle;
        if (angle > 0x400) {
            angle = 0x400;
        }
        if (angle != 0) {
            mtxa_from_identity();
            fn_14_129C0(&axis, angle);
            mtxa_tf_vec(&item->vel, &item->vel);
        }
        item->vel.y += 0.02 * (ball->pos.y - item->pos.y);
    }
    item->rotX = math_atan2(item->vel.y, math_sqrt(mathutil_sum_of_sq_3(item->vel.x, 0.0f, item->vel.z)));
    item->rotY = math_atan2(item->vel.x, item->vel.z) + 0x8000;
    item->rotZ = 0;
    PSVECAdd(&item->pos, &item->vel, &item->pos);
    if ((item->flags & 2) && stagedef != NULL) {
        prev = item->pos;
        boat_item_to_phys(item, &phys);
        fn_14_22254(&phys, stagedef);
        boat_phys_to_item(&phys, item);
        if (prev.x != item->pos.x || prev.y != item->pos.y || prev.z != item->pos.z) {
            item->state = 4;
            item->flags &= ~2;
        }
    }
}

void fn_14_171E0(struct Item *item, struct PhysicsBall *phys)
{
    struct BoatPlayer *pl = &lbl_14_bss_208[current_ball->playerId];
    struct BoatItemInfo *info = (struct BoatItemInfo *) item->stageBanana;

    if (current_ball->playerId == info->unk14) {
        item->unkC = 0;
        return;
    }
    if ((pl->unk8 & 8) == 0) {
        item->flags &= ~2;
        g_rumble_controller(pl->unk1, 1, 0.3 * lbl_80198B34);
        pl->unk4B8 |= 1;
        item->state = 4;
    }
}

void fn_14_172D0(struct Item *item)
{
    struct BoatItemInfo *info = (struct BoatItemInfo *) item->stageBanana;

    item->flags |= 2;
    item->modelLODs = lbl_14_bss_B4[11];
    item->unk14 = 0.0f;
    item->unk12 = lbl_80198B38;
    item->vel.y = 0.0f;
    item->rotX = 0x400;
    item->rotVelY = 0x1800;
    info->unk4 = 1.0f;
    info->unk8 = info->unk4 / (0.1 * lbl_80198B34);
    info->unkC = ((struct GMAModel *) item->modelLODs)->boundSphereRadius;
    info->unk2 = 0.2 * lbl_80198B34;
    fn_14_12B24(0x264, &item->pos);
}

static inline Vec boat_vec_xz_to(Vec *a, Vec *b)
{
    Vec v = {0.0f, 0.0f, 0.0f};

    v.x = a->x - b->x;
    v.z = a->z - b->z;
    {
        Vec *p = &v;
        return *p;
    }
}

void fn_14_173B4(struct Item *item)
{
    struct Effect effect;
    struct PhysicsBall phys;
    struct BoatObst obst;
    Vec pos;
    Vec toBall;
    Vec vel;
    Vec axis;
    Vec prev;
    struct BoatItemInfo *info = (struct BoatItemInfo *) item->stageBanana;
    struct Ball *ball;
    float height;
    s16 angle;

    pos = item->pos;
    pos.y -= item->unk14;
    height = fn_14_13C6C(&pos);
    switch (item->state) {
    case 0:
        item->unk14 += info->unk8;
        if (item->unk14 >= info->unk4) {
            item->unk14 = info->unk4;
            item->state = 1;
        }
        /* fallthrough */
    case 1:
        if (item->unk12 == 2) {
            item->state = 4;
            return;
        }
        break;
    case 4:
        memset(&effect, 0, sizeof(effect));
        effect.type = 0;
        effect.pos = item->pos;
        fn_14_186F4(&effect, NULL);
        if (height > -0.5f) {
            memset(&obst, 0, sizeof(obst));
            obst.type = 0;
            obst.owner = info->unk14;
            obst.pos = item->pos;
            fn_14_23554(&obst);
        }
        item->unk12 = 0.1 * lbl_80198B34;
        PSVECScale(&item->vel, &item->vel, 0.01f);
        item->state = 5;
        /* fallthrough */
    case 5:
        item->unk14 -= info->unk4 / (0.1 * lbl_80198B34);
        if (item->unk14 < 0.0f || item->unk12 == 1) {
            item->unk14 = 0.0f;
            item->state = 6;
        }
        break;
    case 6:
        g_poolInfo.itemPool.statusList[item->id] = 3;
        return;
    }
    if (item->unk12 < lbl_80198B38 - 7) {
        fn_14_18250(item, height);
    }
    if (height >= 0.0) {
        PSVECScale(&item->vel, &item->vel, lbl_1_data_65810);
        item->vel.y += 0.05 * (3.6 * lbl_14_bss_2B48);
    } else {
        PSVECScale(&item->vel, &item->vel, lbl_1_data_6580C);
        item->vel.y -= 3.6 * lbl_14_bss_2B48;
    }
    item->rotY += item->rotVelY;
    if (info->unk15 >= 0 && info->unk2 == 0 && (lbl_14_bss_208[info->unk15].unk8 & 8) == 0) {
        ball = &balls[info->unk15];
        toBall = boat_vec_xz_to(&ball->pos, &item->pos);
        PSVECSubtract(&ball->pos, &item->pos, &toBall);
        vec_normalize_len(&toBall);
        vel = item->vel;
        vec_normalize_len(&vel);
        mathutil_vec_cross_prod(&vel, &toBall, &axis);
        angle = g_math_unk3(vec_normalize_len(&axis));
        angle = 0.03 * angle;
        if (angle > 0x400) {
            angle = 0x400;
        }
        if (angle != 0) {
            mtxa_from_identity();
            fn_14_129C0(&axis, angle);
            mtxa_tf_vec(&item->vel, &item->vel);
        }
        item->vel.y += 0.02 * (ball->pos.y - item->pos.y);
    }
    PSVECAdd(&item->pos, &item->vel, &item->pos);
    if ((item->flags & 2) && stagedef != NULL) {
        prev = item->pos;
        boat_item_to_phys(item, &phys);
        fn_14_22254(&phys, stagedef);
        boat_phys_to_item(&phys, item);
        if (prev.x != item->pos.x || prev.y != item->pos.y || prev.z != item->pos.z) {
            item->state = 4;
            item->flags &= ~2;
        }
    }
}

void fn_14_17A10(struct Item *item, struct PhysicsBall *phys)
{
    struct BoatPlayer *pl = &lbl_14_bss_208[current_ball->playerId];
    struct BoatItemInfo *info = (struct BoatItemInfo *) item->stageBanana;

    if (current_ball->playerId == info->unk14) {
        item->unkC = 0;
        return;
    }
    if ((pl->unk8 & 8) == 0) {
        item->flags &= ~2;
        item->state = 4;
    }
}

void fn_14_17A90(struct Item *item)
{
    struct BoatItemInfo *info = (struct BoatItemInfo *) item->stageBanana;

    item->flags |= 2;
    item->modelLODs = lbl_14_bss_B4[13];
    item->unk14 = 0.0f;
    item->unk12 = lbl_80198B38;
    item->vel.y = 0.0f;
    info->unk4 = 2.2f;
    info->unk8 = info->unk4 / (0.1 * lbl_80198B34);
    info->unkC = ((struct GMAModel *) item->modelLODs)->boundSphereRadius;
    info->unk2 = 0.2 * lbl_80198B34;
    fn_14_12B24(0x264, &item->pos);
}

void fn_14_17B64(struct Item *item)
{
    struct Effect effect;
    struct PhysicsBall phys;
    Vec pos;
    Vec toBall;
    Vec vel;
    Vec axis;
    Vec prev;
    struct BoatItemInfo *info = (struct BoatItemInfo *) item->stageBanana;
    struct Ball *ball;
    float height;
    s16 angle;

    pos = item->pos;
    pos.y -= item->unk14;
    height = fn_14_13C6C(&pos);
    switch (item->state) {
    case 0:
        item->unk14 += info->unk8;
        if (item->unk14 >= info->unk4) {
            item->unk14 = info->unk4;
            item->state = 1;
        }
        /* fallthrough */
    case 1:
        if (item->unk12 == 2) {
            item->state = 4;
            return;
        }
        break;
    case 4:
        memset(&effect, 0, sizeof(effect));
        effect.type = 0;
        effect.pos = item->pos;
        fn_14_186F4(&effect, NULL);
        item->unk12 = 0.1 * lbl_80198B34;
        PSVECScale(&item->vel, &item->vel, 0.01f);
        item->state = 5;
        /* fallthrough */
    case 5:
        item->unk14 -= info->unk4 / (0.1 * lbl_80198B34);
        if (item->unk14 < 0.0f || item->unk12 == 1) {
            item->unk14 = 0.0f;
            item->state = 6;
        }
        break;
    case 6:
        g_poolInfo.itemPool.statusList[item->id] = 3;
        return;
    }
    if (item->unk12 < lbl_80198B38 - 7) {
        fn_14_18250(item, height);
    }
    if (height >= 0.0) {
        PSVECScale(&item->vel, &item->vel, lbl_1_data_65810);
        item->vel.y += 0.05 * (3.6 * lbl_14_bss_2B48);
    } else {
        PSVECScale(&item->vel, &item->vel, lbl_1_data_6580C);
        item->vel.y -= 3.6 * lbl_14_bss_2B48;
    }
    if (info->unk15 >= 0 && info->unk2 == 0 && (lbl_14_bss_208[info->unk15].unk8 & 8) == 0) {
        ball = &balls[info->unk15];
        PSVECSubtract(&ball->pos, &item->pos, &toBall);
        vec_normalize_len(&toBall);
        vel = item->vel;
        vec_normalize_len(&vel);
        mathutil_vec_cross_prod(&vel, &toBall, &axis);
        angle = g_math_unk3(vec_normalize_len(&axis));
        angle = 0.03 * angle;
        if (angle > 0x400) {
            angle = 0x400;
        }
        if (angle != 0) {
            mtxa_from_identity();
            fn_14_129C0(&axis, angle);
            mtxa_tf_vec(&item->vel, &item->vel);
        }
        item->vel.y += 0.02 * (ball->pos.y - item->pos.y);
    }
    item->rotX = math_atan2(item->vel.y, math_sqrt(mathutil_sum_of_sq_3(item->vel.x, 0.0f, item->vel.z)));
    item->rotY = math_atan2(item->vel.x, item->vel.z) + 0x8000;
    item->rotZ = 0;
    PSVECAdd(&item->pos, &item->vel, &item->pos);
    if ((item->flags & 2) && stagedef != NULL) {
        prev = item->pos;
        boat_item_to_phys(item, &phys);
        fn_14_22254(&phys, stagedef);
        boat_phys_to_item(&phys, item);
        if (prev.x != item->pos.x || prev.y != item->pos.y || prev.z != item->pos.z) {
            item->state = 4;
            item->flags &= ~2;
        }
    }
}

void fn_14_18160(struct Item *item, struct PhysicsBall *phys)
{
    struct BoatPlayer *pl = &lbl_14_bss_208[current_ball->playerId];
    struct BoatItemInfo *info = (struct BoatItemInfo *) item->stageBanana;

    if (current_ball->playerId == info->unk14) {
        item->unkC = 0;
        return;
    }
    if ((pl->unk8 & 8) == 0) {
        item->flags &= ~2;
        g_rumble_controller(pl->unk1, 1, 0.3 * lbl_80198B34);
        pl->unk4B8 |= 4;
        item->state = 4;
    }
}

void fn_14_18250(struct Item *item, float height)
{
    if (!(height > 1.0f) && !(height < -0.5f)) {
        struct BoatParticleParam param;
        Vec pos;
        Vec vel;
        s8 models[1] = {0x12};
        float f;

        pos.x = item->pos.x - (5.0f * item->vel.x);
        pos.y = item->pos.y - (5.0f * item->vel.y);
        pos.z = item->pos.z - (5.0f * item->vel.z);
        f = mathutil_vec_len(&item->vel);
        f = (f - 0.01f) / 0.44f;
        f = (f < 0.0f) ? 0.0f : ((f > 1.0f) ? 1.0f : f);
        vel.x = 0.0f;
        vel.y = 0.3f * f;
        vel.z = 0.0f;
        fn_14_2190C(&param);
        param.pos = &pos;
        param.vel = &vel;
        param.accel.y = -0.012495f;
        param.velScale = 0.2f;
        param.drag = 0.95f;
        param.scale = 1.5f;
        param.posScale = 0.1f;
        param.life = 10.0f + (10.0f * f);
        param.count = 1;
        param.models = models;
        param.numModels = 1;
        fn_14_215F8(&param);
    }
}

void fn_14_183EC(struct Item *item, float height)
{
    struct Effect effect;

    memset(&effect, 0, sizeof(effect));
    effect.type = 2;
    effect.pos.x = (item->pos.x - 0.28f) + 0.56f * (rand() / 32767.0f);
    effect.pos.y = item->pos.y + height;
    effect.pos.z = (item->pos.z - 0.28f) + 0.56f * (rand() / 32767.0f);
    effect.colorFactor = 1.3f + 0.5f * (rand() / 32767.0f);
    effect.timer = (rand() & 7) + 0xA;
    fn_14_186F4(&effect, NULL);
}

void fn_14_1853C(struct Item *item, float height)
{
    struct BoatParticleParam param;
    Vec pos;
    Vec vel;
    s8 models[1] = {0x12};

    pos.x = item->pos.x;
    pos.y = item->pos.y + height;
    pos.z = item->pos.z;
    vel.x = 0.0f;
    vel.y = 0.42f;
    vel.z = 0.0f;
    fn_14_2190C(&param);
    param.pos = &pos;
    param.vel = &vel;
    param.accel.y = -0.012495f;
    param.velScale = 0.15f;
    param.drag = 0.95f;
    param.scale = 1.5f;
    param.posScale = 0.4f;
    param.life = 0x28;
    param.count = 5;
    param.models = models;
    param.numModels = 1;
    fn_14_215F8(&param);
    param.fadeTime = 1;
    fn_14_215F8(&param);
    param.fadeTime = 2;
    fn_14_215F8(&param);
    param.fadeTime = 3;
    fn_14_215F8(&param);
    param.fadeTime = 4;
    fn_14_215F8(&param);
}

