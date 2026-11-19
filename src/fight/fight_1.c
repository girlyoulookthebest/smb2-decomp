#include "fight.h"
#include "stobj.h"

/* Monkey Fight item box stobj: pushed around by the gloves, wobbles when punched, breaks into
   35 pieces after the second hit and drops a punch item. */

extern struct Stobj stobjs[];
extern u32 g_some_render_flag;
s8 fn_1_2F5B0(int cameraId, Vec *pos, s8 *pan, s8 *span);
void g_something_with_item_coli(Vec *normal, Vec *velA, float massA, Vec *velB, float massB);

/* .data */
/* box corner directions, used to find the lowest corner over the floor */
Vec lbl_12_data_328[4] = {
    {  1.0f, 1.0f,  1.0f },
    { -1.0f, 1.0f, -1.0f },
    { -1.0f, 1.0f,  1.0f },
    {  1.0f, 1.0f, -1.0f },
};

/* Spin a box stobj from a hit along normal (n) by something moving at otherVel. Uses the
   caller's locals local, rel, tmp, quat, rot and dot. */
#define FIGHT_BOX_SPIN(stobj, normal, otherVel, k)                                  \
    do                                                                              \
    {                                                                               \
        mtxa_from_rotate_y((stobj)->rotY);                                          \
        mtxa_rotate_x((stobj)->rotX);                                               \
        mtxa_rotate_z((stobj)->rotZ);                                               \
        mtxa_rigid_inv_tf_vec(normal, &local);                                          \
        rel.x = (otherVel)->x - (stobj)->unk64.x;                                   \
        rel.y = (otherVel)->y - (stobj)->unk64.y;                                   \
        rel.z = (otherVel)->z - (stobj)->unk64.z;                                   \
        mtxa_rigid_inv_tf_vec(&rel, &rel);                                          \
        dot = mathutil_vec_dot_prod(&local, &rel);                                        \
        rel.x -= dot * local.x;                                                           \
        rel.y -= dot * local.y;                                                           \
        rel.z -= dot * local.z;                                                           \
        mathutil_vec_cross_prod(&local, &rel, &tmp);                                   \
        rot.x = math_atan((k) / (stobj)->boundSphereRadius * mathutil_vec_len(&rel)) >> 1; \
        quat.w = mathutil_cos(rot.x);                                               \
        vec_set_len(math_sin(rot.x), &tmp, &tmp);                                 \
        quat.x = tmp.x;                                                            \
        quat.y = tmp.y;                                                            \
        quat.z = tmp.z;                                                            \
        mtxa_from_quat(&quat);                                                      \
        mtxa_to_euler(&rot);                                                        \
        (stobj)->unk76 += rot.x;                                                    \
        (stobj)->unk78 += rot.y;                                                    \
        (stobj)->unk7A += rot.z;                                                    \
    } while (0)

/* player stobj vs item boxes */
void fn_12_5AF8(struct Stobj *stobj)
{
    int i;
    struct Stobj *boxStobj;
    struct FightBox *box;
    int punching;
    int hit;
    struct Ball *ball;
    struct FightGlove *fighter;
    Vec posA;
    Vec posB;
    Vec d;
    Vec tmp;
    Vec local;
    Vec rel;
    Quat quat;
    S16Vec rot;
    float sumR;
    float dot;
    int sound;

    fighter = &lbl_12_bss_1ACA0.gloves[stobj->id];
    punching = fighter->flags & 8;
    ball = &balls[fighter->playerIdx];
    hit = 0;
    box = lbl_12_bss_118;
    for (i = g_poolInfo.stobjPool.count; i > 0; i--, box++)
    {
        if (box->stobj == NULL)
            continue;
        boxStobj = box->stobj;
        posA = stobj->position;
        posB = boxStobj->position;
        if (!fn_1_A74E4(&stobj->position_2, &posA, stobj->boundSphereRadius, &boxStobj->position_2, &posB, boxStobj->boundSphereRadius))
            continue;
        if (!punching)
        {
            d.x = stobj->position.x - boxStobj->position.x;
            d.y = stobj->position.y - boxStobj->position.y;
            d.z = stobj->position.z - boxStobj->position.z;
            sumR = stobj->boundSphereRadius + boxStobj->boundSphereRadius;
            if (mathutil_vec_len(&d) < sumR)
            {
                tmp = d;
                vec_set_len(sumR, &tmp, &tmp);
                tmp.x += boxStobj->position.x;
                tmp.y += boxStobj->position.y;
                tmp.z += boxStobj->position.z;
                stobj->u_some_pos.x += tmp.x - stobj->position.x;
                stobj->u_some_pos.y += tmp.y - stobj->position.y;
                stobj->u_some_pos.z += tmp.z - stobj->position.z;
                stobj->position.x = tmp.x;
                stobj->position.y = tmp.y;
                stobj->position.z = tmp.z;
                vec_normalize_len(&d);
                dot = mathutil_vec_dot_prod(&d, &stobj->unk64);
                if (dot < 0.0f)
                {
                    dot *= -1.2f;
                    stobj->unk64.x += d.x * dot;
                    stobj->unk64.y += d.y * dot;
                    stobj->unk64.z += d.z * dot;
                }
            }
        }
        else if (box->stobjCooldown[stobj->id] > 0)
        {
            d.x = stobj->position.x - boxStobj->position.x;
            d.y = stobj->position.y - boxStobj->position.y;
            d.z = stobj->position.z - boxStobj->position.z;
            sumR = stobj->boundSphereRadius + boxStobj->boundSphereRadius;
            if (mathutil_vec_len(&d) < sumR)
            {
                tmp = d;
                vec_set_len(sumR, &tmp, &tmp);
                tmp.x += boxStobj->position.x;
                tmp.y += boxStobj->position.y;
                tmp.z += boxStobj->position.z;
                stobj->u_some_pos.x += tmp.x - stobj->position.x;
                stobj->u_some_pos.y += tmp.y - stobj->position.y;
                stobj->u_some_pos.z += tmp.z - stobj->position.z;
                stobj->position.x = tmp.x;
                stobj->position.y = tmp.y;
                stobj->position.z = tmp.z;
            }
        }
        else
        {
            hit = 1;
            box->lastHitter = ((struct FightGlove *)stobj->extraData)->playerIdx;
            fn_12_6B50(boxStobj, &stobj->position, &stobj->unk64, stobj->boundSphereRadius);
            stobj->u_some_pos.x += posA.x - stobj->position.x;
            stobj->u_some_pos.y += posA.y - stobj->position.y;
            stobj->u_some_pos.z += posA.z - stobj->position.z;
            stobj->position = posA;
            boxStobj->u_some_pos.x += posB.x - boxStobj->position.x;
            boxStobj->u_some_pos.y += posB.y - boxStobj->position.y;
            boxStobj->u_some_pos.z += posB.z - boxStobj->position.z;
            boxStobj->position = posB;
            d.x = posA.x - posB.x;
            d.y = posA.y - posB.y;
            d.z = posA.z - posB.z;
            vec_normalize_len(&d);
            FIGHT_BOX_SPIN(boxStobj, &d, &stobj->unk64, 1.0f);
            if (!(ball->flags & (1ULL << 41)))
            {
                tmp.x = stobj->unk64.x - boxStobj->unk64.x;
                tmp.y = stobj->unk64.y - boxStobj->unk64.y;
                tmp.z = stobj->unk64.z - boxStobj->unk64.z;
                dot = fabs(mathutil_vec_dot_prod(&d, &tmp)) - 0.02f;
                if (dot > 0.0f)
                    g_rumble_controller(player_pad_map[ball->playerId], 1, 60.0f * (0.2f + 0.5f * dot));
            }
            g_something_with_item_coli(&d, &stobj->unk64, fn_12_474C(stobj), &boxStobj->unk64, 7.7999997f);
            box->stobjCooldown[stobj->id] = 30;
            if (fighter->sound3C >= 0)
                fighter->unk44 = 15;
        }
    }
    if (hit && punching && lbl_12_bss_1ACA0.gloves[stobj->id].unk2A <= 0)
    {
        fighter = &lbl_12_bss_1ACA0.gloves[stobj->id];
        if (fighter->powerLevel > 0)
            sound = 0x35C;
        else if (fighter->radLevel > 0)
            sound = 0xFF;
        else
            sound = lbl_12_data_230[active_monkey_id[fighter->playerIdx]];
        if (fighter->rangeLevel > 0)
            sound |= 0xD800;
        g_smth_with_sound = active_monkey_id[stobj->id];
        g_player_id_for_sound = stobj->id;
        call_SoundReqID_arg_0(sound);
        fighter->unk2A = 15;
    }
}

/* item boxes vs each other */
void fn_12_640C(void)
{
    int i;
    int j;
    struct Stobj *a;
    struct Stobj *b;
    struct FightBox *boxA;
    struct FightBox *boxB;
    Vec posA;
    Vec posB;
    Vec d;
    Vec tmp;
    Vec local;
    Vec rel;
    Quat quat;
    S16Vec rot;
    float sumR;
    float dot;

    boxA = lbl_12_bss_118;
    for (i = g_poolInfo.stobjPool.count; i > 0; i--, boxA++)
    {
        if (boxA->stobj == NULL)
            continue;
        a = boxA->stobj;
        for (boxB = boxA + 1, j = i - 1; j > 0; j--, boxB++)
        {
            if (boxB->stobj == NULL)
                continue;
            b = boxB->stobj;
            posA = a->position;
            posB = b->position;
            if (!fn_1_A74E4(&a->position_2, &posA, a->boundSphereRadius, &b->position_2, &posB, b->boundSphereRadius))
                continue;
            if (boxA->stobjCooldown[b->id] > 0)
            {
                d.x = a->position.x - b->position.x;
                d.y = a->position.y - b->position.y;
                d.z = a->position.z - b->position.z;
                sumR = a->boundSphereRadius + b->boundSphereRadius;
                if (mathutil_vec_len(&d) < sumR)
                {
                    tmp = d;
                    vec_set_len(sumR, &tmp, &tmp);
                    tmp.x += b->position.x;
                    tmp.y += b->position.y;
                    tmp.z += b->position.z;
                    a->u_some_pos.x += tmp.x - a->position.x;
                    a->u_some_pos.y += tmp.y - a->position.y;
                    a->u_some_pos.z += tmp.z - a->position.z;
                    a->position.x = tmp.x;
                    a->position.y = tmp.y;
                    a->position.z = tmp.z;
                }
            }
            else
            {
                a->u_some_pos.x += posA.x - a->position.x;
                a->u_some_pos.y += posA.y - a->position.y;
                a->u_some_pos.z += posA.z - a->position.z;
                a->position = posA;
                b->u_some_pos.x += posB.x - b->position.x;
                b->u_some_pos.y += posB.y - b->position.y;
                b->u_some_pos.z += posB.z - b->position.z;
                b->position = posB;
                d.x = posA.x - posB.x;
                d.y = posA.y - posB.y;
                d.z = posA.z - posB.z;
                vec_normalize_len(&d);
                FIGHT_BOX_SPIN(a, &d, &b->unk64, 0.2f);
                d.x = -d.x;
                d.y = -d.y;
                d.z = -d.z;
                FIGHT_BOX_SPIN(b, &d, &a->unk64, 0.2f);
                g_something_with_item_coli(&d, &a->unk64, 1.0f, &b->unk64, 1.0f);
                boxA->stobjCooldown[b->id] = 8;
            }
        }
    }
}

/* box hit by something at pos moving at vel: wobble, count the hit, shake loose pieces */
void fn_12_6B50(struct Stobj *stobj, Vec *pos, Vec *vel, float radius)
{
    int count;
    struct FightBox *box;
    struct FightBoxPiece *piece;
    struct GMAModel **models;
    int loose;
    Vec v;
    Vec local;
    Vec center;
    float speed;
    float r;
    s8 pan;
    s8 span;
    s8 pan2;
    s8 span2;

    box = stobj->extraData;
    v.x = stobj->unk64.x - vel->x;
    v.y = stobj->unk64.y - vel->y;
    v.z = stobj->unk64.z - vel->z;
    local.x = stobj->u_some_pos.x - pos->x;
    local.y = stobj->u_some_pos.y - pos->y;
    local.z = stobj->u_some_pos.z - pos->z;
    vec_normalize_len(&local);
    speed = fabs(mathutil_vec_dot_prod(&v, &local));
    v.x = 0.0f;
    v.y = 0.0f;
    v.z = 32768.0f * speed;
    mtxa_from_rotate_y(rand() & 0x7FFF);
    mtxa_rotate_x(rand() & 0x7FFF);
    mtxa_tf_vec_xyz(0.0f, 0.0f, 32768.0f * speed, &v);
    box->wobbleVel.x += v.x;
    box->wobbleVel.y += v.y;
    box->wobbleVel.z += v.z;
    box->hitCount++;
    box->flags |= 2;
    box->glow += 1.5f;
    if (box->hitCount > 1 && !(box->flags & 8))
    {
        stobj->state = 4;
        fn_1_2F5B0(0, &stobj->u_some_pos, &pan, &span);
        call_SoundReqID_arg_0(0xFE | ((pan & 0x7F) << 18) | ((span & 0x7F) << 25));
    }
    else
    {
        fn_1_2F5B0(0, &stobj->u_some_pos, &pan2, &span2);
        call_SoundReqID_arg_0(0xFD | ((pan2 & 0x7F) << 18) | ((span2 & 0x7F) << 25));
    }
    mtxa_from_translate(&stobj->u_some_pos);
    mtxa_rotate_y(stobj->rotY);
    mtxa_rotate_x(stobj->rotX);
    mtxa_rotate_z(stobj->rotZ);
    mtxa_rigid_inv_tf_point(pos, &local);
    models = lbl_12_bss_8C;
    count = box->pieceCount;
    piece = box->pieces;
    loose = (count >> 1 < (rand() & 7) + 2) ? count >> 1 : (rand() & 7) + 2;
    for (; count > 0; piece++, models++)
    {
        if (piece->timer < 0)
            continue;
        count--;
        r = radius + (*models)->boundSphereRadius;
        center = (*models)->boundSphereCenter;
        v.x = center.x + piece->pos.x - local.x;
        v.y = center.y + piece->pos.y - local.y;
        v.z = center.z + piece->pos.z - local.z;
        if (mathutil_vec_len(&v) < r)
        {
            vec_set_len(r, &v, &v);
            v.x += local.x - center.x;
            v.y += local.y - center.y;
            v.z += local.z - center.z;
            piece->pos.x = v.x;
            piece->pos.y = v.y;
            piece->pos.z = v.z;
        }
        if (loose != 0 && (rand() & 3) == 0)
        {
            loose--;
            if (piece->timer == 0)
                piece->timer = (int)(30.0f * (rand() / 32767.0f)) + 1;
        }
    }
}

/* make the attached pieces fall off from the top down within 60 frames */
static void fight_box_break_by_height(struct Stobj *stobj)
{
    int count;
    struct FightBoxPiece *piece;
    struct FightBox *box;
    int t;
    struct GMAModel **models;
    float scale;
    float r;
    Vec center;

    box = stobj->extraData;
    box->flags |= 2;
    mtxa_push();
    mtxa_from_rotate_y(stobj->rotY);
    mtxa_rotate_x(stobj->rotX);
    mtxa_rotate_z(stobj->rotZ);
    r = stobjs[0].boundSphereRadius;
    scale = 1.0f / (3.0f * r);
    models = lbl_12_bss_8C;
    count = box->pieceCount;
    piece = box->pieces;
    for (; count > 0; piece++, models++)
    {
        if (piece->timer < 0)
            continue;
        count--;
        mtxa_tf_vec(&(*models)->boundSphereCenter, &center);
        t = 60.0f * (1.0f - scale * (center.y + r));
        if (t < 1)
            t = 1;
        else if (t > 60)
            t = 60;
        if (piece->timer == 0 || piece->timer > t)
            piece->timer = (rand() & 0x7FFF) % t + 1;
    }
    mtxa_pop();
}

/* drop a punch item from the box, thrown towards the last hitter when off the floor */
static void fight_box_spawn_item(struct Stobj *stobj)
{
    struct FightBox *box;
    struct Item item;
    struct RaycastHit hit;
    Vec dir;
    Vec target;

    box = stobj->extraData;
    memset(&item, 0, sizeof(item));
    item.type = IT_FGT_PUNCH;
    item.subType = fn_12_1804C();
    item.animGroupId = 0;
    item.pos = stobj->u_some_pos;
    item.vel.y = 0.083333336f;
    if ((s32)raycast_stage_down(&item.pos, &hit, &dir) == 0)
    {
        if (box->lastHitter < 0)
        {
            target.x = 0.0f;
            target.y = 0.0f;
            target.z = 0.0f;
        }
        else
            target = balls[box->lastHitter].pos;
        dir.x = target.x - item.pos.x;
        dir.y = 0.0f;
        dir.z = target.z - item.pos.z;
        vec_set_len(0.083333336f, &dir, &dir);
        if (target.y > item.pos.y)
            dir.y = (target.y - item.pos.y) / 30.0f;
        item.vel.x += dir.x;
        item.vel.y += dir.y;
        item.vel.z += dir.z;
    }
    spawn_item(&item);
}

/* make the attached pieces fall off within maxTime frames */
static inline void fight_box_break(struct Stobj *stobj, int maxTime)
{
    struct FightBox *box;
    int count;
    struct FightBoxPiece *piece;

    if (maxTime < 1)
        maxTime = 1;
    box = stobj->extraData;
    box->flags |= 2;
    count = box->pieceCount;
    piece = box->pieces;
    for (; count > 0; piece++)
    {
        if (piece->timer < 0)
            continue;
        count--;
        if (piece->timer == 0 || piece->timer > maxTime)
            piece->timer = (rand() & 0x7FFF) % maxTime + 1;
    }
}

/* box init */
void fn_12_6FB0(struct Stobj *stobj)
{
    struct FightBox *box;

    stobj->state = 0;
    stobj->unk8 |= 0x1A;
    stobj->model = minigameGma->modelEntries[150].model;
    stobj->boundSphereRadius = 0.75f * stobj->model->boundSphereRadius;
    stobj->u_model_origin = stobj->model->boundSphereCenter;
    stobj->unk9C = 0.75f;
    stobj->unk90 = stobj->u_model_origin;
    box = &lbl_12_bss_118[stobj->id];
    memset(box, 0, sizeof(*box));
    box->stobj = stobj;
    stobj->extraData = box;
    box->lastHitter = -1;
    box->pieceCount = 35;
    box->alpha = 0.0f;
    box->coreScale = 1.0f;
}

/* box tick */
void fn_12_70B8(struct Stobj *stobj)
{
    struct FightBox *box;
    int count;
    struct FightBoxPiece *piece;
    struct GMAModel **models;
    int i;
    float r;
    float height;
    Vec *corner;
    S16Vec rot;
    Vec local;
    Vec floorVel;
    Vec up;
    Quat quat;
    struct RaycastHit hit;
    Mtx prevMtx;
    struct PhysicsBall physBall;
    struct Effect effect;
    s8 pan;
    s8 span;

    box = stobj->extraData;
    switch (stobj->state)
    {
    case 0:
    case 1:
        stobj->state = 2;
        break;
    case 2:
        stobj->state = 3;
        stobj->counter = 3600;
        /* fall through */
    case 3:
        stobj->counter--;
        if (stobj->counter < 0 && (box->flags & 1))
            stobj->state = 8;
        break;
    case 4:
        stobj->unk8 &= ~2;
        box->flags |= 0x10;
        fight_box_spawn_item(stobj);
        box->flags |= 8;
        fight_box_break(stobj, 15);
        /* fall through */
    case 5:
        stobj->state = 9;
        break;
    case 6:
        if (!(box->flags & 8))
        {
            fight_box_spawn_item(stobj);
            box->flags |= 8;
        }
        fight_box_break(stobj, 120);
        /* fall through */
    case 7:
        stobj->state = 9;
        break;
    case 8:
        stobj->state = 9;
        box->flags |= 0x10;
        if (!(box->flags & 1))
            fight_box_break(stobj, 60);
        else
            fight_box_break_by_height(stobj);
        /* fall through */
    case 9:
        if (!(box->flags & 8))
        {
            float target = (1.0f / 35.0f) * (box->pieceCount - 1);

            box->coreScale += 0.1f * (target - box->coreScale);
            if (box->coreScale <= 0.0f)
                box->flags |= 8;
        }
        if (box->pieceCount == 0)
            g_poolInfo.stobjPool.statusList[stobj->id] = 3;
        break;
    }

    stobj->unk76 = 0.97f * stobj->unk76;
    stobj->unk78 = 0.97f * stobj->unk78;
    stobj->unk7A = 0.97f * stobj->unk7A;
    stobj->rotX += stobj->unk76;
    stobj->rotY += stobj->unk78;
    stobj->rotZ += stobj->unk7A;
    stobj->unk64.y += -0.012f;
    stobj->unk64.x *= 0.97f;
    stobj->unk64.y *= 0.97f;
    stobj->unk64.z *= 0.97f;
    stobj->u_some_pos.x += stobj->unk64.x;
    stobj->u_some_pos.y += stobj->unk64.y;
    stobj->u_some_pos.z += stobj->unk64.z;
    box->flags &= ~1;

    if ((s32)raycast_stage_down(&stobj->u_some_pos, &hit, &floorVel) == 0)
    {
        hit.normal.x = 0.0f;
        hit.normal.y = 1.0f;
        hit.normal.z = 0.0f;
    }
    vec_to_euler(&hit.normal, &rot);
    mtxa_from_rotate_y(rot.y);
    mtxa_rotate_x(rot.x);
    mtxa_rotate_y(stobj->rotY);
    mtxa_rotate_x(stobj->rotX);
    mtxa_rotate_z(stobj->rotZ);
    height = 0.0f;
    for (corner = lbl_12_data_328, i = 4; i > 0; i--, corner++)
    {
        mtxa_tf_vec(corner, &local);
        r = fabs(local.z);
        if (height < r)
        {
            height = r;
            if (height > 1.4142f)
                break;
        }
    }
    height *= stobj->boundSphereRadius;

    physBall.flags = 0;
    physBall.pos = stobj->position;
    physBall.prevPos = stobj->position_2;
    physBall.vel = stobj->unk64;
    physBall.radius = height;
    physBall.gravityAccel = 0.0f;
    physBall.restitution = 0.5f;
    physBall.hardestColiSpeed = 0.0f;
    physBall.friction = 0.1f;
    physBall.itemGroupId = 0;
    physBall.hardestColiItemGroupId = 0;
    collide_ball_with_stage(&physBall, stagedef);
    if (physBall.flags & 1)
    {
        if (!(box->flags & 0x20))
        {
            box->flags |= 0x20;
            fn_1_2F5B0(0, &stobj->u_some_pos, &pan, &span);
            call_SoundReqID_arg_0(0xFA | ((pan & 0x7F) << 18) | ((span & 0x7F) << 25));
        }
        stobj->u_some_pos.x += physBall.pos.x - stobj->position.x;
        stobj->u_some_pos.y += physBall.pos.y - stobj->position.y;
        stobj->u_some_pos.z += physBall.pos.z - stobj->position.z;
        stobj->position = physBall.pos;
        stobj->unk64 = physBall.vel;
        box->flags |= 1;
        stobj->unk76 = 0.9f * stobj->unk76;
        stobj->unk78 = 0.9f * stobj->unk78;
        stobj->unk7A = 0.9f * stobj->unk7A;
        if (physBall.hardestColiItemGroupId > 0)
        {
            mtxa_from_mtx(itemgroups[physBall.hardestColiItemGroupId].transform);
            mtxa_tf_vec(&physBall.hardestColiNormal, &physBall.hardestColiNormal);
        }
        mtxa_from_rotate_y(stobj->rotY);
        mtxa_rotate_x(stobj->rotX);
        mtxa_rotate_z(stobj->rotZ);
        mtxa_rigid_inv_tf_vec(&physBall.hardestColiNormal, &local);
        up.x = 0.0f;
        if (local.y > 0.0f)
            up.y = 1.0f;
        else
            up.y = -1.0f;
        up.z = 0.0f;
        r = mathutil_vec_distance(&local, &up);
        mathutil_vec_cross_prod(&up, &local, &local);
        rot.x = math_atan(r * (0.025f / stobj->boundSphereRadius)) >> 1;
        quat.w = mathutil_cos(rot.x);
        vec_set_len(math_sin(rot.x), &local, &local);
        quat.x = local.x;
        quat.y = local.y;
        quat.z = local.z;
        mtxa_from_quat(&quat);
        mtxa_to_euler(&rot);
        if (rot.x < -0x180)
            rot.x = -0x180;
        else if (rot.x > 0x180)
            rot.x = 0x180;
        if (rot.y < -0x180)
            rot.y = -0x180;
        else if (rot.y > 0x180)
            rot.y = 0x180;
        if (rot.z < -0x180)
            rot.z = -0x180;
        else if (rot.z > 0x180)
            rot.z = 0x180;
        stobj->unk76 += rot.x;
        stobj->unk78 += rot.y;
        stobj->unk7A += rot.z;
        r = mathutil_vec_dot_prod(&stobj->unk64, &physBall.hardestColiNormal);
        if (r < 0.0f)
        {
            r *= -1.0f;
            stobj->unk64.x += physBall.hardestColiNormal.x * r;
            stobj->unk64.y += physBall.hardestColiNormal.y * r;
            stobj->unk64.z += physBall.hardestColiNormal.z * r;
            stobj->unk64.x += 0.1f * (floorVel.x - stobj->unk64.x);
            stobj->unk64.y += 0.1f * (floorVel.y - stobj->unk64.y);
            stobj->unk64.z += 0.1f * (floorVel.z - stobj->unk64.z);
            r *= 0.5f;
            stobj->unk64.x += physBall.hardestColiNormal.x * r;
            stobj->unk64.y += physBall.hardestColiNormal.y * r;
            stobj->unk64.z += physBall.hardestColiNormal.z * r;
        }
    }
    if (!(box->flags & 4) && stobj->u_some_pos.y < *stagedef->falloutY)
    {
        box->flags |= 4;
        stobj->state = 6;
    }

    box->glow += 0.1f * (-0.05f - box->glow);
    box->wobbleVel.x *= 0.95f;
    box->wobbleVel.y *= 0.95f;
    box->wobbleVel.z *= 0.95f;
    box->wobbleVel.x += -0.025f * box->wobble.x;
    box->wobbleVel.y += -128.0f * math_sin(box->wobble.y);
    box->wobbleVel.z += -0.025f * box->wobble.z;
    box->wobble.x += box->wobbleVel.x;
    box->wobble.y += box->wobbleVel.y;
    box->wobble.z += box->wobbleVel.z;
    {
        int n;
        s8 *cooldown;

        for (cooldown = box->playerCooldown, n = 4; n > 0; n--, cooldown++)
        {
            if (*cooldown > 0)
                (*cooldown)--;
        }
        for (cooldown = box->stobjCooldown, n = g_poolInfo.stobjPool.count; n > 0; n--, cooldown++)
        {
            if (*cooldown > 0)
                (*cooldown)--;
        }
    }
    if (box->alpha < 1.0f)
    {
        box->alpha += 0.15f * (1.1f - box->alpha);
        if (box->alpha > 1.0f)
            box->alpha = 1.0f;
    }

    /* launch the pieces whose timer runs out as effects */
    mtxa_from_translate(&stobj->unk7C);
    mtxa_rotate_y(stobj->unk8A);
    mtxa_rotate_x(stobj->unk88);
    mtxa_rotate_z(stobj->unk8C);
    mtxa_to_mtx(prevMtx);
    mtxa_from_translate(&stobj->u_some_pos);
    mtxa_rotate_y(stobj->rotY);
    mtxa_rotate_x(stobj->rotX);
    mtxa_rotate_z(stobj->rotZ);
    memset(&effect, 0, sizeof(effect));
    effect.type = 0x2A;
    i = 0;
    models = lbl_12_bss_8C;
    count = box->pieceCount;
    piece = box->pieces;
    for (; count > 0; piece++, models++, i++)
    {
        if (piece->timer < 0)
            continue;
        count--;
        piece->pos.x += -0.0125f * piece->pos.x;
        piece->pos.y += -0.0125f * piece->pos.y;
        piece->pos.z += -0.0125f * piece->pos.z;
        if (piece->timer > 0)
        {
            piece->timer--;
            if (piece->timer != 0)
                continue;
            piece->timer = -1;
            box->pieceCount--;
            effect.model = *models;
            effect.unkA8 = i;
            effect.rotX = stobj->rotX;
            effect.rotY = stobj->rotY;
            effect.rotZ = stobj->rotZ;
            effect.unk52 = stobj->unk76;
            effect.unk54 = stobj->unk78;
            effect.unk56 = stobj->unk7A;
            mtxa_tf_point(&effect.model->boundSphereCenter, &effect.pos);
            mtxa_push();
            mtxa_from_mtx(prevMtx);
            mtxa_tf_point(&effect.model->boundSphereCenter, &local);
            mtxa_pop();
            effect.vel.x = effect.pos.x - local.x;
            effect.vel.y = effect.pos.y - local.y;
            effect.vel.z = effect.pos.z - local.z;
            if (box->flags & 0x10)
            {
                local.x = effect.pos.x - stobj->u_some_pos.x;
                local.y = effect.pos.y - stobj->u_some_pos.y;
                local.z = effect.pos.z - stobj->u_some_pos.z;
                vec_set_len(0.2f * (rand() / 32767.0f), &local, &local);
                effect.vel.x += local.x;
                effect.vel.y += local.y;
                effect.vel.z += local.z;
            }
            mtxa_push();
            spawn_effect(&effect);
            mtxa_pop();
        }
    }
}

/* box disp: whole box, or its loose pieces once hit */
void fn_12_83F8(struct Stobj *stobj)
{
    struct FightBox *box;
    struct GMAModel **models;
    float glow;
    float alpha;
    float z;
    struct FightBoxPiece *piece;
    int count;
    s16 rotX;
    s16 rotY;
    s16 rotZ;
    int loaded;

    box = stobj->extraData;
    if (g_some_render_flag & 4)
    {
        if (stobj->u_some_pos.y < -stobj->boundSphereRadius)
            return;
        models = lbl_12_bss_0;
    }
    else
        models = lbl_12_bss_8C;
    mtxa_from_mtxb_translate_xyz(stobj->u_some_pos.x, stobj->u_some_pos.y, stobj->u_some_pos.z);
    mtxa_rotate_y(stobj->rotY);
    mtxa_rotate_x(stobj->rotX);
    mtxa_rotate_z(stobj->rotZ);
    if (!g_is_sphere_visible(&stobj->model->boundSphereCenter, stobj->model->boundSphereRadius))
        return;
    alpha = box->alpha;
    z = mtxa->mtxA[2][3] + stobj->model->boundSphereRadius;
    if (z > 0.0f)
        alpha = 0.0f;
    else if (z > -1.0f)
        alpha *= -z;
    if (alpha <= 0.0f)
        return;

    if ((box->flags & 2) && !(box->flags & 8))
    {
        mtxa_push();
        mtxa_sq_from_identity();
        mtxa_rotate_y(box->wobble.y);
        mtxa_rotate_x(box->wobble.x);
        mtxa_rotate_z(box->wobble.z + 65536.0f * box->coreScale);
        mtxa_scale_s(box->coreScale);
        if (alpha < 1.0f)
        {
            avdisp_set_alpha(alpha);
            avdisp_draw_model_culled_sort_always(minigameGma->modelEntries[151].model);
        }
        else
        {
            GXLoadPosMtxImm(mtxa->mtxA, 0);
            GXLoadNrmMtxImm(mtxa->mtxA, 0);
            avdisp_draw_model_culled_sort_auto(minigameGma->modelEntries[151].model);
        }
        mtxa_pop();
    }

    glow = box->glow;
    if (glow > 0.0f)
    {
        float r = glow;
        float g = 0.8f * glow;
        float b = 0.5f * glow;

        if (r > 1.0f)
            r = 1.0f;
        if (g > 1.0f)
            g = 1.0f;
        if (b > 1.0f)
            b = 1.0f;
        g_stores_doubles2(r, g, b, 0.0f);
    }

    if (!(box->flags & 2))
    {
        if (alpha < 1.0f)
        {
            avdisp_set_alpha(alpha);
            avdisp_draw_model_culled_sort_always(stobj->model);
        }
        else
        {
            GXLoadPosMtxImm(mtxa->mtxA, 0);
            GXLoadNrmMtxImm(mtxa->mtxA, 0);
            avdisp_draw_model_unculled_sort_auto(stobj->model);
        }
    }
    else
    {
        loaded = 0;
        count = box->pieceCount;
        piece = box->pieces;
        if (alpha < 1.0f)
        {
            for (; count > 0; piece++, models++)
            {
                if (piece->timer < 0)
                    continue;
                count--;
                if (piece->pos.x != 0.0f || piece->pos.y != 0.0f || piece->pos.z != 0.0f)
                {
                    rotX = 8192.0f * piece->pos.x;
                    rotY = 8192.0f * piece->pos.y;
                    rotZ = 8192.0f * piece->pos.z;
                    mtxa_push();
                    mtxa_translate(&piece->pos);
                    mtxa_translate(&(*models)->boundSphereCenter);
                    mtxa_rotate_y(rotY);
                    mtxa_rotate_x(rotX);
                    mtxa_rotate_z(rotZ);
                    mtxa_translate_neg(&(*models)->boundSphereCenter);
                    avdisp_set_alpha(alpha);
                    avdisp_draw_model_unculled_sort_always(*models);
                    mtxa_pop();
                }
                else
                {
                    avdisp_set_alpha(alpha);
                    avdisp_draw_model_unculled_sort_always(*models);
                }
            }
        }
        else
        {
            for (; count > 0; piece++, models++)
            {
                if (piece->timer < 0)
                    continue;
                count--;
                if (piece->pos.x != 0.0f || piece->pos.y != 0.0f || piece->pos.z != 0.0f)
                {
                    rotX = 8192.0f * piece->pos.x;
                    rotY = 8192.0f * piece->pos.y;
                    rotZ = 8192.0f * piece->pos.z;
                    mtxa_push();
                    mtxa_translate(&piece->pos);
                    mtxa_translate(&(*models)->boundSphereCenter);
                    mtxa_rotate_y(rotY);
                    mtxa_rotate_x(rotX);
                    mtxa_rotate_z(rotZ);
                    mtxa_translate_neg(&(*models)->boundSphereCenter);
                    loaded = 0;
                    GXLoadPosMtxImm(mtxa->mtxA, 0);
                    GXLoadNrmMtxImm(mtxa->mtxA, 0);
                    avdisp_draw_model_unculled_sort_auto(*models);
                    mtxa_pop();
                }
                else
                {
                    if (!loaded)
                    {
                        loaded = 1;
                        GXLoadPosMtxImm(mtxa->mtxA, 0);
                        GXLoadNrmMtxImm(mtxa->mtxA, 0);
                    }
                    avdisp_draw_model_unculled_sort_auto(*models);
                }
            }
        }
    }
    if (glow > 0.0f)
        g_stores_doubles2(0.0f, 0.0f, 0.0f, 0.0f);
}

/* dot product of (x1, y1, z1) and (x2, y2, z2) */
static inline float fight_dot_xyz(register float x1, register float y1, register float z1, register float x2, register float y2, register float z2)
{
#ifdef __MWERKS__
    asm
    {
        fmuls x1, x1, x2
        fmadds x1, y1, y2, x1
        fmadds x1, z1, z2, x1
    }
    return x1;
#else
    return x1 * x2 + y1 * y2 + z1 * z2;
#endif
}

/* .data */
s32 lbl_12_data_380[4] = { 0x17, 0x18, 0x1A, 0 };  /* box hit sounds by strength */

/* box coli (ball hits the box) */
void fn_12_8970(struct Stobj *stobj, struct PhysicsBall *physBall)
{
    struct FightBox *box;
    struct Ball *ball;
    Vec q;
    Vec d;
    Vec tmp;
    Vec local;
    Vec rel;
    Quat quat;
    S16Vec rot;
    float sumR;
    float dot;
    int sound;

    box = stobj->extraData;
    ball = current_ball;
    if (box->playerCooldown[ball->playerId] > 0)
    {
        d.x = stobj->position.x - physBall->pos.x;
        d.y = stobj->position.y - physBall->pos.y;
        d.z = stobj->position.z - physBall->pos.z;
        sumR = physBall->radius + stobj->boundSphereRadius;
        if (mathutil_vec_len(&d) < sumR)
        {
            tmp = d;
            vec_set_len(sumR, &tmp, &tmp);
            q.x = physBall->pos.x + tmp.x;
            q.y = physBall->pos.y + tmp.y;
            q.z = physBall->pos.z + tmp.z;
            stobj->u_some_pos.x += q.x - stobj->position.x;
            stobj->u_some_pos.y += q.y - stobj->position.y;
            stobj->u_some_pos.z += q.z - stobj->position.z;
            stobj->position.x = q.x;
            stobj->position.y = q.y;
            stobj->position.z = q.z;
        }
        return;
    }
    q = stobj->position;
    fn_1_A74E4(&physBall->prevPos, &physBall->pos, physBall->radius, &stobj->position_2, &q, stobj->boundSphereRadius);
    stobj->u_some_pos.x += q.x - stobj->position.x;
    stobj->u_some_pos.y += q.y - stobj->position.y;
    stobj->u_some_pos.z += q.z - stobj->position.z;
    stobj->position = q;
    d.x = physBall->pos.x - q.x;
    d.y = physBall->pos.y - q.y;
    d.z = physBall->pos.z - q.z;
    vec_normalize_len(&d);
    FIGHT_BOX_SPIN(stobj, &d, &physBall->vel, 1.0f);
    ball->flags |= 0x20;
    dot = fabs(fight_dot_xyz(d.x, d.y, d.z,
                               stobj->unk64.x - physBall->vel.x,
                               stobj->unk64.y - physBall->vel.y,
                               stobj->unk64.z - physBall->vel.z));
    if (dot > 0.05f)
    {
        sound = dot / 0.2f;
        if (sound > 2)
            sound = 2;
        sound = lbl_12_data_380[sound];
        g_smth_with_sound = ball->ape->charaId;
        g_player_id_for_sound = ball->playerId;
        call_SoundReqID_arg_0(sound);
    }
    g_something_with_item_coli(&d, &stobj->unk64, 1.0f, &physBall->vel, (2.0f * ball->currRadius) * (2.0f * ball->currRadius));
    box->playerCooldown[ball->playerId] = 4;
}

/* box dest */
void fn_12_8EC0(struct Stobj *stobj)
{
    struct FightBox *box = stobj->extraData;

    box->stobj = NULL;
}
